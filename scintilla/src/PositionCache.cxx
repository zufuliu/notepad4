// Scintilla source code edit control
/** @file PositionCache.cxx
 ** Classes for caching layout information.
 **/
// Copyright 1998-2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cmath>
#include <climits>

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <optional>
#include <algorithm>
#include <iterator>
#include <memory>

#include "ParallelSupport.h"
#include "ScintillaTypes.h"
#include "ScintillaMessages.h"
#include "ILoader.h"
#include "ILexer.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"
#include "VectorISA.h"

#include "CharacterSet.h"
//#include "CharacterCategory.h"
//#include "EastAsianWidth.h"
#include "Position.h"
#include "UniqueString.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "KeyMap.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "CaseFolder.h"
#include "Document.h"
#include "UniConversion.h"
#include "Selection.h"
#include "PositionCache.h"
#include "EditModel.h"

using namespace Scintilla;
using namespace Scintilla::Internal;
using namespace Lexilla;

void BidiData::Resize(size_t maxLineLength_) {
	stylesFonts.resize(maxLineLength_ + 1);
	widthReprs.resize(maxLineLength_ + 1);
}

LineLayout::LineLayout(Sci::Line lineNumber_, int maxLineLength_) :
	lineNumber(lineNumber_),
	lenLineStarts(0),
	maxLineLength(-1),
	lastSegmentEnd(0),
	numCharsInLine(0),
	numCharsBeforeEOL(0),
	validity(ValidLevel::invalid),
	xHighlightGuide(0),
	highlightColumn(false),
	containsCaret(false),
	bracePreviousStyles{},
	edgeColumn(0),
	caretPosition(0),
	widthLine(wrapWidthInfinite),
	lines(1),
	wrapIndent(0) {
	Resize(maxLineLength_);
}

LineLayout::~LineLayout() {
	Free();
}

void LineLayout::Resize(int maxLineLength_) {
	if (maxLineLength_ > maxLineLength) {
		Free();
		const size_t lineAllocation = maxLineLength_ + 1;
		chars = std::make_unique<char[]>(lineAllocation);
		styles = std::make_unique<unsigned char[]>(lineAllocation);
		// Extra position allocated as sometimes the Windows
		// GetTextExtentExPoint API writes an extra element.
		positions = std::make_unique<XYPOSITION[]>(lineAllocation + 1);
		if (bidiData) {
			bidiData->Resize(maxLineLength_);
		}

		maxLineLength = maxLineLength_;
	}
}

void LineLayout::Reset(Sci::Line lineNumber_, Sci::Position maxLineLength_) {
	lineNumber = lineNumber_;
	Resize(static_cast<int>(maxLineLength_));
	lines = 0;
	Invalidate(ValidLevel::invalid);
}

void LineLayout::EnsureBidiData() {
	if (!bidiData) {
		bidiData = std::make_unique<BidiData>();
		bidiData->Resize(maxLineLength);
	}
}

void LineLayout::Free() noexcept {
	chars.reset();
	styles.reset();
	positions.reset();
	lineStarts.reset();
	lenLineStarts = 0;
	bidiData.reset();
}

void LineLayout::ClearPositions() const noexcept {
	//std::fill_n(positions.get(), maxLineLength + 2, 0.0f);
	memset(positions.get(), 0, (maxLineLength + 2) * sizeof(XYPOSITION));
}

void LineLayout::Invalidate(ValidLevel validity_) noexcept {
	if (validity > validity_)
		validity = validity_;
}

bool LineLayout::CanHold(Sci::Line lineDoc, int lineLength_) const noexcept {
	return (lineNumber == lineDoc) && (lineLength_ <= maxLineLength);
}

int LineLayout::LineStart(int line) const noexcept {
	if (line <= 0) {
		return 0;
	} else if ((line >= lines) || !lineStarts) {
		return numCharsInLine;
	} else {
		return lineStarts[line];
	}
}

int LineLayout::LineLength(int line) const noexcept {
	if (!lineStarts) {
		return numCharsInLine;
	} if (line >= lines - 1) {
		return numCharsInLine - lineStarts[line];
	} else {
		return lineStarts[line + 1] - lineStarts[line];
	}
}

int LineLayout::LineLastVisible(int line, Scope scope) const noexcept {
	if (line < 0) {
		return 0;
	} else if ((line >= lines - 1) || !lineStarts) {
		if (PartialPosition()) {
			return lastSegmentEnd;
		}
		return scope == Scope::visibleOnly ? numCharsBeforeEOL : numCharsInLine;
	} else {
		return lineStarts[line + 1];
	}
}

Range LineLayout::SubLineRange(int subLine, Scope scope) const noexcept {
	return Range(LineStart(subLine), LineLastVisible(subLine, scope));
}

bool LineLayout::InLine(int offset, int line) const noexcept {
	return ((offset >= LineStart(line)) && (offset < LineStart(line + 1))) ||
		((offset == numCharsInLine) && (line == (lines - 1)));
}

int LineLayout::SubLineFromPosition(int posInLine, PointEnd pe) const noexcept {
	if (!lineStarts || (posInLine > maxLineLength)) {
		return lines - 1;
	}

	for (int line = 0; line < lines; line++) {
		if (FlagSet(pe, PointEnd::subLineEnd)) {
			// Return subline not start of next
			if (lineStarts[line + 1] <= posInLine + 1)
				return line;
		} else {
			if (lineStarts[line + 1] <= posInLine)
				return line;
		}
	}

	return lines - 1;
}

void LineLayout::AddLineStart(Sci::Position start) {
	lines++;
	if (lines >= lenLineStarts) {
		const int newMaxLines = lines + 20;
		std::unique_ptr<int[]> newLineStarts = std::make_unique<int[]>(newMaxLines);
		if (lenLineStarts) {
			std::copy(lineStarts.get(), lineStarts.get() + lenLineStarts, newLineStarts.get());
		}
		lineStarts = std::move(newLineStarts);
		lenLineStarts = newMaxLines;
	}
	lineStarts[lines] = static_cast<int>(start);
}

void LineLayout::SetBracesHighlight(Range rangeLine, const Sci::Position braces[],
	unsigned char bracesMatchStyle, int xHighlight, bool ignoreStyle) noexcept {
	if (!ignoreStyle && rangeLine.ContainsCharacter(braces[0])) {
		const Sci::Position braceOffset = braces[0] - rangeLine.start;
		if (braceOffset < numCharsInLine) {
			bracePreviousStyles[0] = styles[braceOffset];
			styles[braceOffset] = bracesMatchStyle;
		}
	}
	if (!ignoreStyle && rangeLine.ContainsCharacter(braces[1])) {
		const Sci::Position braceOffset = braces[1] - rangeLine.start;
		if (braceOffset < numCharsInLine) {
			bracePreviousStyles[1] = styles[braceOffset];
			styles[braceOffset] = bracesMatchStyle;
		}
	}
	if ((braces[0] >= rangeLine.start && braces[1] <= rangeLine.end) ||
		(braces[1] >= rangeLine.start && braces[0] <= rangeLine.end)) {
		xHighlightGuide = xHighlight;
	}
}

void LineLayout::RestoreBracesHighlight(Range rangeLine, const Sci::Position braces[], bool ignoreStyle) noexcept {
	if (!ignoreStyle && rangeLine.ContainsCharacter(braces[0])) {
		const Sci::Position braceOffset = braces[0] - rangeLine.start;
		if (braceOffset < numCharsInLine) {
			styles[braceOffset] = bracePreviousStyles[0];
		}
	}
	if (!ignoreStyle && rangeLine.ContainsCharacter(braces[1])) {
		const Sci::Position braceOffset = braces[1] - rangeLine.start;
		if (braceOffset < numCharsInLine) {
			styles[braceOffset] = bracePreviousStyles[1];
		}
	}
	xHighlightGuide = 0;
}

int LineLayout::FindBefore(XYPOSITION x, Range range) const noexcept {
	Sci::Position lower = range.start;
	Sci::Position upper = range.end;
	do {
		const Sci::Position middle = (upper + lower + 1) / 2; 	// Round high
		const XYPOSITION posMiddle = positions[middle];
		if (x < posMiddle) {
			upper = middle - 1;
		} else {
			lower = middle;
		}
	} while (lower < upper);
	return static_cast<int>(lower);
}

int LineLayout::FindPositionFromX(XYPOSITION x, Range range, bool charPosition) const noexcept {
	int pos = FindBefore(x, range);
	while (pos < range.end) {
		if (charPosition) {
			if (x < (positions[pos + 1])) {
				return pos;
			}
		} else {
			if (x < ((positions[pos] + positions[pos + 1]) / 2)) {
				return pos;
			}
		}
		pos++;
	}
	return static_cast<int>(range.end);
}

Point LineLayout::PointFromPosition(int posInLine, int lineHeight, PointEnd pe) const noexcept {
	Point pt;
	// In case of very long line put x at arbitrary large position
	if (posInLine > maxLineLength) {
		pt.x = positions[maxLineLength] - positions[LineStart(lines)];
	}

	for (int subLine = 0; subLine < lines; subLine++) {
		const Range rangeSubLine = SubLineRange(subLine, Scope::visibleOnly);
		if (posInLine >= rangeSubLine.start) {
			pt.y = static_cast<XYPOSITION>(subLine*lineHeight);
			if (posInLine <= rangeSubLine.end) {
				pt.x = positions[posInLine] - positions[rangeSubLine.start];
				if (rangeSubLine.start != 0)	// Wrapped lines may be indented
					pt.x += wrapIndent;
				if (FlagSet(pe, PointEnd::subLineEnd))	// Return end of first subline not start of next
					break;
			} else if (FlagSet(pe, PointEnd::lineEnd) && (subLine == (lines - 1))) {
				pt.x = positions[numCharsInLine] - positions[rangeSubLine.start];
				if (rangeSubLine.start != 0)	// Wrapped lines may be indented
					pt.x += wrapIndent;
			}
		} else {
			break;
		}
	}
	return pt;
}

XYPOSITION LineLayout::XInLine(Sci::Position index) const noexcept {
	// For positions inside line return value from positions
	// For positions after line return last position + 1.0
	if (index <= numCharsInLine) {
		return positions[index];
	}
	return positions[numCharsInLine] + 1.0;
}

Interval LineLayout::Span(int start, int end) const noexcept {
	return { positions[start], positions[end] };
}

Interval LineLayout::SpanByte(int index) const noexcept {
	return Span(index, index+1);
}

int LineLayout::EndLineStyle() const noexcept {
	return styles[numCharsBeforeEOL > 0 ? numCharsBeforeEOL - 1 : 0];
}

namespace {

enum class WrapBreak {
	None = 0,
	Before = 1,
	After = 2,
	Both = 3,
	Undefined = 4,
};

constexpr uint8_t ASCIIWrapBreakTable[128] = {
//++Autogenerated -- start of section automatically generated
// Created with Python 3.13.0a1, Unicode 15.1.0
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 0, 1, 1, 2, 2, 0, 1, 2, 2, 1, 2, 2, 2, 2,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 2, 2, 2,
1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 1, 2,
//--Autogenerated -- end of section automatically generated
};

constexpr WrapBreak GetWrapBreak(unsigned char ch) noexcept {
	return (ch & 0x80)? WrapBreak::None : static_cast<WrapBreak>(ASCIIWrapBreakTable[ch]);
}

constexpr WrapBreak GetWrapBreakEx(unsigned int ch, bool isUtf8) noexcept {
	if (ch < 0x80) {
		return static_cast<WrapBreak>(ASCIIWrapBreakTable[ch]);
	}
	if (isUtf8) {
		// fullwidth forms
		if (ch > 0xFF00 && ch < 0xFF5F) {
			return static_cast<WrapBreak>(ASCIIWrapBreakTable[ch - 0xFEE0]);
		}
	}

	return WrapBreak::Undefined;
}

}

void LineLayout::WrapLine(const Document *pdoc, Sci::Position posLineStart, Wrap wrapState, XYPOSITION wrapWidth, XYPOSITION wrapIndent_, bool partialLine) {
	Sci::Position lastLineStart = 0;
	XYPOSITION startOffset = wrapWidth;
	Sci::Position p = 0;
	if (partialLine && lines > 2 && wrapIndent == wrapIndent_) {
		lastLineStart = LineStart(lines - 2);
		lines -= 2;
		p = lastLineStart + 1;
		startOffset += positions[lastLineStart] - wrapIndent_;
	} else {
		lines = 0;
		wrapIndent = wrapIndent_;
	}

	const bool isUtf8 = CpUtf8 == pdoc->dbcsCodePage;
	while (p < lastSegmentEnd) {
		while (p < lastSegmentEnd && positions[p + 1] < startOffset) {
			p++;
		}
		if (p < lastSegmentEnd) {
			// backtrack to find lastGoodBreak
			Sci::Position lastGoodBreak = p;
			if (p > 0) {
				lastGoodBreak = pdoc->MovePositionOutsideChar(p + posLineStart, -1) - posLineStart;
			}
			if (wrapState != Wrap::Char) {
				Sci::Position pos = lastGoodBreak;
				CharacterClass ccPrev = CharacterClass::space;
				WrapBreak wbPrev = WrapBreak::None;
				if (wrapState == Wrap::Auto) {
					const int character = pdoc->CharacterAfter(pos + posLineStart).character;
					ccPrev = pdoc->WordCharacterClass(character);
					wbPrev = GetWrapBreakEx(character, isUtf8);
				} else if (wrapState == Wrap::Word) {
					wbPrev = GetWrapBreak(chars[pos]);
				}
				while (pos > lastLineStart) {
					// style boundary and space
					if (wrapState != Wrap::WhiteSpace && (styles[pos - 1] != styles[pos])) {
						break;
					}
					if (IsBreakSpace(chars[pos - 1]) && !IsBreakSpace(chars[pos])) {
						break;
					}

					const Sci::Position posBefore = pdoc->MovePositionOutsideChar(pos + posLineStart - 1, -1) - posLineStart;
					if (wrapState == Wrap::Auto) {
						// word boundary
						// TODO: Unicode Line Breaking Algorithm https://www.unicode.org/reports/tr14/
						const WrapBreak wbPos = wbPrev;
						const CharacterClass ccPos = ccPrev;
						const int chPrevious = pdoc->CharacterAfter(posBefore + posLineStart).character;
						ccPrev = pdoc->WordCharacterClass(chPrevious);
						wbPrev = GetWrapBreakEx(chPrevious, isUtf8);
						if (wbPrev != WrapBreak::Before && wbPos != WrapBreak::After) {
							if ((ccPrev == CharacterClass::cjkWord || ccPos == CharacterClass::cjkWord) ||
								//(wbPrev == WrapBreak::Both || wbPos == WrapBreak::Both) ||
								(wbPrev != wbPos && (wbPrev == WrapBreak::After || wbPos == WrapBreak::Before)) ||
								(ccPrev != ccPos && (wbPrev == WrapBreak::Undefined || wbPos == WrapBreak::Undefined))
							) {
								break;
							}
						}
					} else if (wrapState == Wrap::Word) {
						const WrapBreak wbPos = wbPrev;
						wbPrev = GetWrapBreak(chars[posBefore]);
						if (wbPrev != WrapBreak::Before && wbPos != WrapBreak::After) {
							if (//(wbPrev == WrapBreak::Both || wbPos == WrapBreak::Both) ||
								(wbPrev != wbPos && (wbPrev == WrapBreak::After || wbPos == WrapBreak::Before))
							) {
								break;
							}
						}
					}
					pos = posBefore;
				}
				if (pos > lastLineStart) {
					lastGoodBreak = pos;
				}
			}
			if (lastGoodBreak == lastLineStart) {
				// Try moving to start of last character
				if (p > 0) {
					lastGoodBreak = pdoc->MovePositionOutsideChar(p + posLineStart, -1) - posLineStart;
				}
				if (lastGoodBreak == lastLineStart) {
					// Ensure at least one character on line.
					lastGoodBreak = pdoc->MovePositionOutsideChar(lastGoodBreak + posLineStart + 1, 1) - posLineStart;
				}
			}
			lastLineStart = lastGoodBreak;
			AddLineStart(lastLineStart);
			startOffset = positions[lastLineStart];
			// take into account the space for start wrap mark and indent
			startOffset += wrapWidth - wrapIndent;
			p = lastLineStart + 1;
		}
	}
	lines++;
}

ScreenLine::ScreenLine(
	const LineLayout *ll_,
	int subLine,
	const ViewStyle &vs,
	XYPOSITION width_,
	int tabWidthMinimumPixels_) noexcept:
	ll(ll_),
	start(ll->LineStart(subLine)),
	len(ll->LineLength(subLine)),
	width(width_),
	height(static_cast<float>(vs.lineHeight)),
	tabWidth(vs.tabWidth),
	ctrlCharPadding(vs.ctrlCharPadding),
	tabWidthMinimumPixels(tabWidthMinimumPixels_) {}

ScreenLine::~ScreenLine() noexcept = default;

std::string_view ScreenLine::Text() const noexcept {
	return std::string_view(&ll->chars[start], len);
}

size_t ScreenLine::Length() const noexcept {
	return len;
}

size_t ScreenLine::RepresentationCount() const {
	return std::count_if(&ll->bidiData->widthReprs[start],
		&ll->bidiData->widthReprs[start + len],
		[](XYPOSITION w) noexcept { return w > 0.0f; });
}

XYPOSITION ScreenLine::Width() const noexcept {
	return width;
}

XYPOSITION ScreenLine::Height() const noexcept {
	return height;
}

XYPOSITION ScreenLine::TabWidth() const noexcept {
	return tabWidth;
}

XYPOSITION ScreenLine::TabWidthMinimumPixels() const noexcept {
	return static_cast<XYPOSITION>(tabWidthMinimumPixels);
}

const Font *ScreenLine::FontOfPosition(size_t position) const noexcept {
	return ll->bidiData->stylesFonts[start + position].get();
}

XYPOSITION ScreenLine::RepresentationWidth(size_t position) const noexcept {
	return ll->bidiData->widthReprs[start + position];
}

XYPOSITION ScreenLine::TabPositionAfter(XYPOSITION xPosition) const noexcept {
	return (std::floor((xPosition + TabWidthMinimumPixels()) / TabWidth()) + 1) * TabWidth();
}

bool SignificantLines::LineMayCache(Sci::Line line) const noexcept {
	switch (level) {
	case LineCache::None:
		return false;
	case LineCache::Caret:
		return line == lineCaret;
	case LineCache::Page:
		return (std::abs(line - lineCaret) < linesOnScreen)
			|| (std::abs(line - lineTop) < linesOnScreen);
	case LineCache::Document:
	default:
		return true;
	}
}

LineLayoutCache::LineLayoutCache() noexcept:
	lastCaretSlot(SIZE_MAX),
	level(LineCache::None),
	maxValidity(LineLayout::ValidLevel::invalid), styleClock(-1) {
}

LineLayoutCache::~LineLayoutCache() = default;

namespace {

// https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
// Bit Twiddling Hacks Copyright 1997-2005 Sean Eron Anderson
#if 0
constexpr size_t NextPowerOfTwo(size_t x) noexcept {
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
#if SIZE_MAX > UINT_MAX
	x |= x >> 32;
#endif
	x++;
	return x;
}
#else
inline size_t NextPowerOfTwo(size_t x) noexcept {
#if SIZE_MAX > UINT_MAX
	return UINT64_C(1) << (1 + np2::bsr(x - 1));
#else
	return 1U << (1 + np2::bsr(x - 1));
#endif
}
#endif

#if 1
// test for ASCII only since all C0 control character has special representation.
#if NP2_USE_SSE2
inline bool AllGraphicASCII(std::string_view text) noexcept {
	const char *ptr = text.data();
	const size_t length = text.length();
	const char * const end = ptr + length;
	if (length >= sizeof(__m128i)) {
		const char * const xend = end - sizeof(__m128i);
		do {
			const __m128i chunk = _mm_loadu_si128((const __m128i *)ptr);
			if (_mm_movemask_epi8(chunk)) {
				return false;
			}
			ptr += sizeof(__m128i);
		} while (ptr <= xend);
	}
#if 0//NP2_USE_AVX2
	if (const uint32_t remain = length & (sizeof(__m128i) - 1)) {
		const __m128i chunk = _mm_loadu_si128((const __m128i *)ptr);
		if (bit_zero_high_u32(_mm_movemask_epi8(chunk), remain)) {
			return false;
		}
	}
#else
	for (; ptr < end; ptr++) {
		if (*ptr & 0x80) {
			return false;
		}
	}
#endif
	return true;
}

#else
constexpr bool AllGraphicASCII(std::string_view text) noexcept {
	for (const unsigned char ch : text) {
		if (ch & 0x80) {
			return false;
		}
	}
	return true;
}
#endif

#else
#if NP2_USE_SSE2
inline bool AllGraphicASCII(std::string_view text) noexcept {
	const char *ptr = text.data();
	const char * const end = ptr + text.length();
	if (text.length() >= sizeof(__m128i)) {
		const char * const xend = end - sizeof(__m128i);
#if NP2_USE_AVX2
		const __m128i range = _mm_setr_epi8(' ', '~', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		do {
			if (_mm_cmpistrc(range, *(const __m128i *)ptr, _SIDD_SBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_MASKED_NEGATIVE_POLARITY)) {
				return false;
			}
			ptr += sizeof(__m128i);
		} while (ptr <= xend);
#else
		const __m128i space = _mm_set1_epi8(' ');
		const __m128i del = _mm_set1_epi8('\x7f');
		do {
			__m128i chunk = _mm_loadu_si128((const __m128i *)ptr);
			chunk = _mm_or_si128(_mm_cmplt_epi8(chunk, space), _mm_cmpeq_epi8(chunk, del));
			if (_mm_movemask_epi8(chunk)) {
				return false;
			}
			ptr += sizeof(__m128i);
		} while (ptr <= xend);
#endif
	}
	for (; ptr < end; ptr++) {
		const unsigned char ch = *ptr;
		if (ch < ' ' || ch > '~') {
			return false;
		}
	}
	return true;
}

#else
constexpr bool AllGraphicASCII(std::string_view text) noexcept {
	for (const unsigned char ch : text) {
		if (ch < ' ' || ch > '~') {
			return false;
		}
	}
	return true;
}
#endif
#endif

}

void LineLayoutCache::AllocateForLevel(Sci::Line linesOnScreen, Sci::Line linesInDoc) {
	// round up cache size to avoid rapidly resizing when linesOnScreen or linesInDoc changed.
	size_t lengthForLevel = 0;
	if (level == LineCache::Page) {
		// see comment in Retrieve() method.
		lengthForLevel = 1 + NP2_align_up(4*linesOnScreen, 64);
	} else if (level == LineCache::Caret) {
		lengthForLevel = 2;
	} else if (level == LineCache::Document) {
		lengthForLevel = NP2_align_up(linesInDoc, 64);
	}
	if (lengthForLevel != shortCache.size()) {
		maxValidity = LineLayout::ValidLevel::lines;
		shortCache.resize(lengthForLevel);
		//printf("%s level=%d, size=%zu/%zu, LineLayout=%zu/%zu, BidiData=%zu, XYPOSITION=%zu\n",
		//	__func__, level, shortCache.size(), shortCache.capacity(), sizeof(LineLayout),
		//	sizeof(std::unique_ptr<LineLayout>), sizeof(BidiData), sizeof(XYPOSITION));
	}
	PLATFORM_ASSERT(shortCache.size() >= lengthForLevel);
}

void LineLayoutCache::Deallocate() noexcept {
	maxValidity = LineLayout::ValidLevel::invalid;
	lastCaretSlot = SIZE_MAX;
	shortCache.clear();
	longCache.clear();
}

void LineLayoutCache::Invalidate(LineLayout::ValidLevel validity_) noexcept {
	if (maxValidity > validity_) {
		maxValidity = validity_;
		for (const auto &ll : shortCache) {
			if (ll) {
				ll->Invalidate(validity_);
			}
		}
		for (const auto &ll : longCache) {
			if (ll) {
				ll->Invalidate(validity_);
			}
		}
	}
}

void LineLayoutCache::SetLevel(LineCache level_) noexcept {
	if (level != level_) {
		level = level_;
		maxValidity = LineLayout::ValidLevel::invalid;
		lastCaretSlot = SIZE_MAX;
		shortCache.clear();
		longCache.clear();
	}
}

LineLayout *LineLayoutCache::Retrieve(Sci::Line lineNumber, Sci::Line lineCaret, int maxChars, int styleClock_,
	Sci::Line linesOnScreen, Sci::Line linesInDoc, Sci::Line topLine) {
	AllocateForLevel(linesOnScreen, linesInDoc);
	if (styleClock != styleClock_) {
		Invalidate(LineLayout::ValidLevel::checkTextAndStyle);
		styleClock = styleClock_;
	}
	maxValidity = LineLayout::ValidLevel::lines;

	size_t pos = 0;
	LineLayout *ret = nullptr;
	const int useLongCache = UseLongCache(maxChars);
	if (useLongCache) {
		for (const auto &ll : longCache) {
			if (ll->LineNumber() == lineNumber) {
				ret = ll.get();
				break;
			}
		}
	} else if (level == LineCache::Page) {
		// two arenas, each with two pages to ensure cache efficiency on scrolling.
		// first arena for lines near top visible line.
		// second arena for other lines, e.g. folded lines near top visible line.
		// TODO: use/cleanup second arena after some periods, e.g. after Editor::WrapLines() finished.
		const size_t diff = std::abs(lineNumber - topLine);
		const size_t gap = shortCache.size() / 2;
		pos = 1 + (lineNumber % gap) + ((diff < gap) ? 0 : gap);
		// first slot reserved for caret line, which is rapidly retrieved when caret blinking.
		if (lineNumber == lineCaret) {
			if (lastCaretSlot == 0 && shortCache[0]->LineNumber() == lineCaret) {
				pos = 0;
			} else {
				lastCaretSlot = pos;
			}
		} else if (pos == lastCaretSlot) {
			// save cache for caret line.
			lastCaretSlot = 0;
			std::swap(shortCache[0], shortCache[pos]);
		}
	} else if (level == LineCache::Caret) {
		pos = lineNumber != lineCaret;
	} else if (level == LineCache::Document) {
		pos = lineNumber;
	}

	if (!useLongCache) {
		ret = shortCache[pos].get();
	}
	if (ret) {
		if (!ret->CanHold(lineNumber, maxChars)) {
			//printf("USE line=%zd/%zd, caret=%zd/%zd top=%zd, pos=%zu, clock=%d\n",
			//	lineNumber, ret->lineNumber, lineCaret, lastCaretSlot, topLine, pos, styleClock_);
			ret->Free();
			new (ret) LineLayout(lineNumber, maxChars);
		} else {
			//printf("HIT line=%zd, caret=%zd/%zd top=%zd, pos=%zu, clock=%d, validity=%d\n",
			//	lineNumber, lineCaret, lastCaretSlot, topLine, pos, styleClock_, ret->validity);
		}
	} else {
		//printf("NEW line=%zd, caret=%zd/%zd top=%zd, pos=%zu, clock=%d\n",
		//	lineNumber, lineCaret, lastCaretSlot, topLine, pos, styleClock_);
		auto ll = std::make_unique<LineLayout>(lineNumber, maxChars);
		ret = ll.get();
		if (useLongCache) {
			longCache.push_back(std::move(ll));
		} else {
			shortCache[pos].swap(ll);
		}
	}

	// LineLineCache::None is not supported, we only use LineCache::Page.
	return ret;
}

namespace {

// Simply pack the (maximum 4) character bytes into an int
#if 0
constexpr unsigned int KeyFromString(std::string_view charBytes) noexcept {
	PLATFORM_ASSERT(charBytes.length() <= 4);
	unsigned int k = 0;
	for (const unsigned char uc : charBytes) {
		k = (k << 8) | uc;
	}
	return k;
}

#else
inline unsigned int KeyFromString(std::string_view charBytes) noexcept {
	unsigned int k = 0;
	if (!charBytes.empty()) {
		k = loadbe_u32(charBytes.data());
		if (const size_t diff = 4 - charBytes.length()) {
			k >>= diff*8;
		}
	}
	return k;
}
#endif

constexpr unsigned int representationKeyCrLf = ('\r' << 8) | '\n';

}

void SpecialRepresentations::SetRepresentation(std::string_view charBytes, std::string_view value) {
	if ((charBytes.length() <= 4) && (value.length() <= Representation::maxLength)) {
		const unsigned int key = KeyFromString(charBytes);
		const bool inserted = mapReprs.insert_or_assign(key, Representation(value)).second;
		if (inserted) {
			// New entry so increment for first byte
			const unsigned char ucStart = charBytes.empty() ? 0 : charBytes[0];
			startByteHasReprs[ucStart]++;
			if (key > maxKey) {
				maxKey = key;
			}
			if (key == representationKeyCrLf) {
				crlf = true;
			}
		}
	}
}

void SpecialRepresentations::SetRepresentationAppearance(std::string_view charBytes, RepresentationAppearance appearance) {
	if (charBytes.length() <= 4) {
		const unsigned int key = KeyFromString(charBytes);
		const auto it = mapReprs.find(key);
		if (it == mapReprs.end()) {
			// Not present so fail
			return;
		}
		it->second.appearance = appearance;
	}
}

void SpecialRepresentations::SetRepresentationColour(std::string_view charBytes, ColourRGBA colour) {
	if (charBytes.length() <= 4) {
		const unsigned int key = KeyFromString(charBytes);
		const auto it = mapReprs.find(key);
		if (it == mapReprs.end()) {
			// Not present so fail
			return;
		}
		it->second.appearance = it->second.appearance | RepresentationAppearance::Colour;
		it->second.colour = colour;
	}
}

void SpecialRepresentations::ClearRepresentation(std::string_view charBytes) {
	if (charBytes.length() <= 4) {
		const unsigned int key = KeyFromString(charBytes);
		const auto it = mapReprs.find(key);
		if (it != mapReprs.end()) {
			mapReprs.erase(it);
			const unsigned char ucStart = charBytes.empty() ? 0 : charBytes[0];
			startByteHasReprs[ucStart]--;
			if (key == maxKey && startByteHasReprs[ucStart] == 0) {
				maxKey = mapReprs.empty() ? 0 : mapReprs.crbegin()->first;
			}
			if (key == representationKeyCrLf) {
				crlf = false;
			}
		}
	}
}

const Representation *SpecialRepresentations::GetRepresentation(std::string_view charBytes) const {
	const unsigned int key = KeyFromString(charBytes);
	if (key > maxKey) {
		return nullptr;
	}
	const auto it = mapReprs.find(key);
	if (it != mapReprs.end()) {
		return &(it->second);
	}
	return nullptr;
}

const Representation *SpecialRepresentations::RepresentationFromCharacter(std::string_view charBytes) const {
	if (charBytes.length() <= 4) {
		const unsigned char ucStart = charBytes.empty() ? 0 : charBytes[0];
		if (!startByteHasReprs[ucStart]) {
			return nullptr;
		}
		return GetRepresentation(charBytes);
	}
	return nullptr;
}

void SpecialRepresentations::Clear() noexcept {
	mapReprs.clear();
	constexpr unsigned char none = 0;
	std::fill(startByteHasReprs, std::end(startByteHasReprs), none);
	maxKey = 0;
	crlf = false;
}

void SpecialRepresentations::SetDefaultRepresentations(int dbcsCodePage) {
	Clear();

	// C0 control set
	for (size_t j = 0; j < std::size(repsC0) - 1; j++) {
		const char c[2] = { static_cast<char>(j), '\0' };
		const char *rep = repsC0[j];
		SetRepresentation(std::string_view(c, 1), std::string_view(rep, (rep[2] == '\0') ? 2 : 3));
	}

	struct CharacterRepresentation {
		char code[4];
		char rep[4];
	};
	static constexpr CharacterRepresentation repsMisc[] = {
		{ "\x7f", "DEL" },
		{ "\xe2\x80\xa8", "LS" },
		{ "\xe2\x80\xa9", "PS" },
	};
	SetRepresentation(std::string_view(repsMisc[0].code, 1), std::string_view(repsMisc[0].rep, 3));

	// C1 control set
	// As well as Unicode mode, ISO-8859-1 should use these
	if (CpUtf8 == dbcsCodePage) {
		static constexpr char repsC1[][5] = {
			"PAD", "HOP", "BPH", "NBH", "IND", "NEL", "SSA", "ESA",
			"HTS", "HTJ", "VTS", "PLD", "PLU", "RI", "SS2", "SS3",
			"DCS", "PU1", "PU2", "STS", "CCH", "MW", "SPA", "EPA",
			"SOS", "SGCI", "SCI", "CSI", "ST", "OSC", "PM", "APC"
		};
		for (size_t j = 0; j < std::size(repsC1); j++) {
			const char c1[3] = { '\xc2', static_cast<char>(0x80 + j), '\0' };
			const char *rep = repsC1[j];
			const size_t len = (rep[2] == '\0') ? 2 : ((rep[3] == '\0') ? 3 : 4);
			SetRepresentation(std::string_view(c1, 2), std::string_view(rep, len));
		}
		SetRepresentation(std::string_view(repsMisc[1].code, 3), std::string_view(repsMisc[1].rep, 2));
		SetRepresentation(std::string_view(repsMisc[2].code, 3), std::string_view(repsMisc[2].rep, 2));
	}
	if (dbcsCodePage) {
		// UTF-8 invalid bytes or DBCS invalid single bytes.
		for (int k = 0x80; k < 0x100; k++) {
			if (!IsDBCSValidSingleByte(dbcsCodePage, k)) {
				const char hiByte[2] = { static_cast<char>(k), '\0' };
				const char hexits[4] = { 'x', "0123456789ABCDEF"[k >> 4], "0123456789ABCDEF"[k & 15], '\0' };
				SetRepresentation(std::string_view(hiByte, 1), std::string_view(hexits, 3));
			}
		}
	}
}

void BreakFinder::Insert(Sci::Position val) {
	const int posInLine = static_cast<int>(val);
	if (posInLine > nextBreak) {
		const auto it = std::lower_bound(selAndEdge.begin(), selAndEdge.end(), posInLine);
		if (it == selAndEdge.end()) {
			selAndEdge.push_back(posInLine);
		} else if (*it != posInLine) {
			selAndEdge.insert(it, 1, posInLine);
		}
	}
}

BreakFinder::BreakFinder(const LineLayout *ll_, const Selection *psel, Range lineRange, Sci::Position posLineStart,
	XYPOSITION xStart, BreakFor breakFor, const EditModel &model, const ViewStyle *pvsDraw, uint32_t posInLine) :
	ll(ll_),
	nextBreak(static_cast<int>(lineRange.start)),
	subBreak(-1),
	endPos(static_cast<int>(lineRange.end)),
	stopPos(endPos),
	saeCurrentPos(0),
	saeNext(0),
	pdoc(model.pdoc),
	encodingFamily(pdoc->CodePageFamily()),
	reprs(model.reprs) {

	// Search for first visible break
	// First find the first visible character
	if (xStart > 0.0f) {
		const int startPos = nextBreak;
		nextBreak = ll->FindBefore(xStart, lineRange);
		// Now back to a style break
		while ((nextBreak > startPos) && (ll->styles[nextBreak] == ll->styles[nextBreak - 1])) {
			nextBreak--;
		}
	}

	currentPos = nextBreak;
	if (breakFor == BreakFor::Layout && posInLine < static_cast<uint32_t>(stopPos)) {
		posInLine = std::max(posInLine, currentPos + model.maxParallelLayoutLength);
		if (posInLine < static_cast<uint32_t>(stopPos)) {
			stopPos = static_cast<int>(posInLine);
		}
	}

	if (FlagSet(breakFor, BreakFor::Selection)) {
		const SelectionPosition posStart(posLineStart);
		const SelectionPosition posEnd(posLineStart + endPos);
		const SelectionSegment segmentLine(posStart, posEnd);
		for (size_t r = 0; r < psel->Count(); r++) {
			const SelectionSegment portion = psel->Range(r).Intersect(segmentLine);
			if (!(portion.start == portion.end)) {
				if (portion.start.IsValid())
					Insert(portion.start.Position() - posLineStart);
				if (portion.end.IsValid())
					Insert(portion.end.Position() - posLineStart);
			}
		}
		// On the curses platform, the terminal is drawing its own caret, so add breaks around the
		// caret in the main selection in order to help prevent the selection from being drawn in
		// the caret's cell.
		if (FlagSet(pvsDraw->caret.style, CaretStyle::Curses) && !psel->RangeMain().Empty()) {
			const Sci::Position caretPos = psel->RangeMain().caret.Position();
			const Sci::Position anchorPos = psel->RangeMain().anchor.Position();
			if (caretPos < anchorPos) {
				const Sci::Position nextPos = pdoc->MovePositionOutsideChar(caretPos + 1, 1);
				Insert(nextPos - posLineStart);
			} else if (caretPos > anchorPos && pvsDraw->DrawCaretInsideSelection(false, false)) {
				const Sci::Position prevPos = pdoc->MovePositionOutsideChar(caretPos - 1, -1);
				if (prevPos > anchorPos)
					Insert(prevPos - posLineStart);
			}
		}
	}
	if (FlagSet(breakFor, BreakFor::Foreground) && pvsDraw->indicatorsSetFore) {
		for (const auto *const deco : pdoc->decorations->View()) {
			if (pvsDraw->indicators[deco->Indicator()].OverridesTextFore()) {
				Sci::Position startPos = deco->EndRun(posLineStart);
				while (startPos < (posLineStart + endPos)) {
					Insert(startPos - posLineStart);
					startPos = deco->EndRun(startPos);
				}
			}
		}
	}
	Insert(ll->edgeColumn);
	Insert(endPos);
	saeNext = (!selAndEdge.empty()) ? selAndEdge[0] : -1;
}

BreakFinder::~BreakFinder() = default;

TextSegment BreakFinder::Next() {
	if (subBreak < 0) {
		const int prev = nextBreak;
		const Representation *repr = nullptr;
		while (nextBreak < endPos) {
			int charWidth = 1;
			const char * const chars = &ll->chars[nextBreak];
			const unsigned char ch = chars[0];
			//bool characterStyleConsistent = true;	// All bytes of character in same style?
			if (!UTF8IsAscii(ch) && encodingFamily != EncodingFamily::eightBit) {
				if (encodingFamily == EncodingFamily::unicode) {
					charWidth = UTF8DrawBytes(chars, endPos - nextBreak);
				} else {
					charWidth = pdoc->DBCSDrawBytes(chars, endPos - nextBreak);
				}
				//for (int trail = 1; trail < charWidth; trail++) {
				//	if (ll->styles[nextBreak] != ll->styles[nextBreak + trail]) {
				//		characterStyleConsistent = false;
				//	}
				//}
			}
			//if (!characterStyleConsistent) {
			//	if (nextBreak == prev) {
			//		// Show first character representation bytes since it has inconsistent styles.
			//		charWidth = 1;
			//	} else {
			//		// Return segment before nextBreak but allow to be split up if too long
			//		// If not split up, next call will hit the above 'charWidth = 1;' and display bytes.
			//		break;
			//	}
 			//}
			repr = nullptr;
			if (reprs.MayContains(ch)) {
				// Special case \r\n line ends if there is a representation
				if (ch == '\r' && reprs.ContainsCrLf() && chars[1] == '\n') {
					charWidth = 2;
				}
				repr = reprs.GetRepresentation(std::string_view(chars, charWidth));
			}
			if (((nextBreak > 0) && (ll->styles[nextBreak] != ll->styles[nextBreak - 1])) ||
				repr ||
				(nextBreak == saeNext)) {
				while ((nextBreak >= saeNext) && (saeNext < endPos)) {
					saeCurrentPos++;
					saeNext = static_cast<int>((saeCurrentPos < selAndEdge.size()) ? selAndEdge[saeCurrentPos] : endPos);
				}
				if ((nextBreak > prev) || repr) {
					// Have a segment to report
					if (nextBreak == prev) {
						nextBreak += charWidth;
					} else {
						repr = nullptr;	// Optimize -> should remember repr
					}
					break;
				}
			}
			nextBreak += charWidth;
		}

		const int lengthSegment = nextBreak - prev;
		if (lengthSegment < lengthStartSubdivision) {
			currentPos = nextBreak;
			return {prev, lengthSegment, repr};
		}
		subBreak = prev;
	}

	// Splitting up a long run from prev to nextBreak in lots of approximately lengthEachSubdivision.
	const int startSegment = subBreak;
	const int remaining = nextBreak - startSegment;
	int lengthSegment = remaining;
	if (lengthSegment > lengthEachSubdivision) {
		lengthSegment = static_cast<int>(pdoc->SafeSegment(&ll->chars[startSegment], lengthEachSubdivision, encodingFamily));
	}
	if (lengthSegment < remaining) {
		subBreak += lengthSegment;
	} else {
		subBreak = -1;
	}
	currentPos += lengthSegment;
	return {startSegment, lengthSegment, nullptr};
}

void PositionCacheEntry::Set(uint16_t styleNumber_, size_t length, std::unique_ptr<char[]> &positions_, uint32_t clock_) noexcept {
	styleNumber = styleNumber_;
	clock = static_cast<uint16_t>(clock_);
	len = static_cast<uint32_t>(length);
	positions.swap(positions_);
}

void PositionCacheEntry::Clear() noexcept {
	styleNumber = 0;
	clock = 0;
	len = 0;
	positions.reset();
}

bool PositionCacheEntry::Retrieve(uint16_t styleNumber_, std::string_view sv, XYPOSITION *positions_) const noexcept {
	if (styleNumber == styleNumber_ && len == sv.length()) {
		const size_t offset = sv.length()*sizeof(XYPOSITION);
		if (memcmp(&positions[offset], sv.data(), sv.length()) == 0) {
			memcpy(positions_, &positions[0], offset);
			return true;
		}
	}
	return false;
}

size_t PositionCacheEntry::Hash(uint16_t styleNumber_, std::string_view sv) noexcept {
	const size_t h1 = std::hash<std::string_view>{}(sv);
	const size_t h2 = std::hash<uint8_t>{}(styleNumber_ & 0xff);
	return h1 ^ (h2 << 1);
}

bool PositionCacheEntry::NewerThan(const PositionCacheEntry &other) const noexcept {
	return clock > other.clock;
}

void PositionCacheEntry::ResetClock() noexcept {
	if (clock > 0) {
		clock = 1;
	}
}

PositionCache::PositionCache() {
	clock = 1;
	allClear = true;
	pces.resize(1024);
}

void PositionCache::Clear() noexcept {
	if (!allClear) {
		for (auto &pce : pces) {
			pce.Clear();
		}
	}
	clock = 1;
	allClear = true;
}

void PositionCache::SetSize(size_t size_) {
	Clear();
	if (size_ & (size_ - 1)) {
		size_ = NextPowerOfTwo(size_);
	}
	pces.resize(size_);
}

size_t PositionCache::GetSize() const noexcept {
	return pces.size();
}

void PositionCache::MeasureWidths(Surface *surface, const Style &style, uint16_t styleNumber, std::string_view sv, XYPOSITION *positions) {
	if (style.monospaceASCII && AllGraphicASCII(sv)) {
		const XYPOSITION characterWidth = style.aveCharWidth;
		const size_t length = sv.length();
#if NP2_USE_SSE2
		if (length >= 2) {
			XYPOSITION *ptr = positions;
			const XYPOSITION * const end = ptr + length - 1;
			const __m128d one = _mm_set1_pd(characterWidth);
			const __m128d two = _mm_set1_pd(2);
			__m128d inc = _mm_setr_pd(1, 2);
			do {
				_mm_storeu_pd(ptr, _mm_mul_pd(one, inc));
				inc = _mm_add_pd(inc, two);
				ptr += 2;
			} while (ptr < end);
			if (ptr == end) {
				_mm_store_sd(ptr, _mm_mul_sd(one, inc));
			}
		} else {
			positions[0] = characterWidth;
		}
#else
		for (size_t i = 0; i < length; i++) {
			positions[i] = characterWidth * (i + 1);
		}
#endif
		return;
	}

#ifdef MeasureWidthsUseEastAsianWidth
	if (style.monospaceASCII) {
		const XYPOSITION characterWidth = style.aveCharWidth;
		XYPOSITION *ptr = positions;
		XYPOSITION lastPos = 0;
		for (auto it = sv.begin(); it != sv.end();) {
			const uint8_t ch = *it;
			lastPos += characterWidth;
			if (UTF8IsAscii(ch)) {
				*ptr++ = lastPos;
				++it;
			} else {
				int byteCount = UTF8BytesOfLead(ch);
				const uint32_t character = UnicodeFromUTF8(reinterpret_cast<const uint8_t *>(&*it));
				if (GetEastAsianWidth(character)) {
					lastPos += characterWidth;
				}
				it += byteCount;
				while (byteCount--) {
					*ptr++ = lastPos;
				}
			}
		}
		return;
	}
#endif // MeasureWidthsUseEastAsianWidth

	PositionCacheEntry *entry = nullptr;
	PositionCacheEntry *entry2 = nullptr;
	constexpr size_t maxLength = (512 - 16)/(sizeof(XYPOSITION) + 1);
	if (sv.length() <= maxLength) {
		// Only store short strings in the cache so it doesn't churn with
		// long comments with only a single comment.

		// Two way associative: try two probe positions.
		const size_t hashValue = PositionCacheEntry::Hash(styleNumber, sv);
		const size_t mask = pces.size() - 1;
		const size_t probe = hashValue & mask;
		entry = &pces[probe];

		const LockGuard<NativeMutex> readLock(cacheLock);
		if (entry->Retrieve(styleNumber, sv, positions)) {
			return;
		}

		const size_t probe2 = (hashValue * 37) & mask;
		entry2 = &pces[probe2];
		if (entry2->Retrieve(styleNumber, sv, positions)) {
			return;
		}
	}

	surface->MeasureWidths(style.font.get(), sv, positions);
	if (entry) {
		// constructed here to reduce lock time
		const size_t length = sv.length();
		const size_t offset = length*sizeof(XYPOSITION);
		std::unique_ptr<char[]> positions_ = make_unique_for_overwrite<char[]>(offset + length);
		memcpy(&positions_[0], positions, offset);
		memcpy(&positions_[offset], sv.data(), length);

		// Store into cache
		const LockGuard<NativeMutex> writeLock(cacheLock);
		// Choose the oldest of the two slots to replace
		if (entry->NewerThan(*entry2)) {
			entry = entry2;
		}

		clock++;
		if (clock > UINT16_MAX) {
			// Since there are only 16 bits for the clock, wrap it round and
			// reset all cache entries so none get stuck with a high clock.
			for (PositionCacheEntry &pce : pces) {
				pce.ResetClock();
			}
			clock = 2;
		}
		allClear = false;
		entry->Set(styleNumber, length, positions_, clock);
	}
}
