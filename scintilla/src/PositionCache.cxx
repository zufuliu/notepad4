// Scintilla source code edit control
/** @file PositionCache.cxx
 ** Classes for caching layout information.
 **/
// Copyright 1998-2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <climits>

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <algorithm>
#include <iterator>
#include <memory>

#include "ScintillaTypes.h"
#include "ScintillaMessages.h"
#include "ILoader.h"
#include "ILexer.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"
#include "VectorISA.h"

//#include "CharacterCategory.h"
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

using namespace Scintilla;
using namespace Scintilla::Internal;

void BidiData::Resize(size_t maxLineLength_) {
	stylesFonts.resize(maxLineLength_ + 1);
	widthReprs.resize(maxLineLength_ + 1);
}

LineLayout::LineLayout(Sci::Line lineNumber_, int maxLineLength_) :
	lenLineStarts(0),
	lineNumber(lineNumber_),
	maxLineLength(-1),
	numCharsInLine(0),
	numCharsBeforeEOL(0),
	validity(ValidLevel::invalid),
	xHighlightGuide(0),
	highlightColumn(false),
	containsCaret(false),
	edgeColumn(0),
	bracePreviousStyles{},
	hotspot(0, 0),
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
		chars = std::make_unique<char[]>(maxLineLength_ + 1);
		styles = std::make_unique<unsigned char[]>(maxLineLength_ + 1);
		// Extra position allocated as sometimes the Windows
		// GetTextExtentExPoint API writes an extra element.
		positions = std::make_unique<XYPOSITION[]>(maxLineLength_ + 1 + 1);
		if (bidiData) {
			bidiData->Resize(maxLineLength_);
		}

		maxLineLength = maxLineLength_;
	}
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
	bidiData.reset();
}

void LineLayout::Invalidate(ValidLevel validity_) noexcept {
	if (validity > validity_)
		validity = validity_;
}

Sci::Line LineLayout::LineNumber() const noexcept {
	return lineNumber;
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

void LineLayout::SetLineStart(int line, int start) {
	if ((line >= lenLineStarts) && (line != 0)) {
		const int newMaxLines = line + 20;
		std::unique_ptr<int[]> newLineStarts = std::make_unique<int[]>(newMaxLines);
		if (lenLineStarts) {
			std::copy(lineStarts.get(), lineStarts.get() + lenLineStarts, newLineStarts.get());
		}
		lineStarts = std::move(newLineStarts);
		lenLineStarts = newMaxLines;
	}
	lineStarts[line] = start;
}

void LineLayout::SetBracesHighlight(Range rangeLine, const Sci::Position braces[],
	char bracesMatchStyle, int xHighlight, bool ignoreStyle) noexcept {
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

int LineLayout::EndLineStyle() const noexcept {
	return styles[numCharsBeforeEOL > 0 ? numCharsBeforeEOL - 1 : 0];
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
	ctrlCharPadding(vs.ctrlCharPadding),
	tabWidth(vs.tabWidth),
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

LineLayoutCache::LineLayoutCache() :
	lastCaretSlot(SIZE_MAX),
	level(LineCache::None),
	allInvalidated(false), styleClock(-1) {
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
	return UINT64_C(1) << (64 - np2::clz(x - 1));
#else
	return 1U << (32 - np2::clz(x - 1));
#endif
}
#endif

constexpr size_t AlignUp(size_t value, size_t alignment) noexcept {
	return (value + alignment - 1) & (~(alignment - 1));
}

}

void LineLayoutCache::AllocateForLevel(Sci::Line linesOnScreen, Sci::Line linesInDoc) {
	// round up cache size to avoid rapidly resizing when linesOnScreen or linesInDoc changed.
	size_t lengthForLevel = 0;
	if (level == LineCache::Page) {
		// see comment in Retrieve() method.
		lengthForLevel = 1 + AlignUp(4*linesOnScreen, 64);
	} else if (level == LineCache::Caret) {
		lengthForLevel = 2;
	} else if (level == LineCache::Document) {
		lengthForLevel = AlignUp(linesInDoc, 64);
	}
	if (lengthForLevel != cache.size()) {
		allInvalidated = false;
		cache.resize(lengthForLevel);
		//printf("%s level=%d, size=%zu/%zu, LineLayout=%zu/%zu, BidiData=%zu, XYPOSITION=%zu\n",
		//	__func__, level, cache.size(), cache.capacity(), sizeof(LineLayout),
		//	sizeof(std::unique_ptr<LineLayout>), sizeof(BidiData), sizeof(XYPOSITION));
	}
	PLATFORM_ASSERT(cache.size() >= lengthForLevel);
}

void LineLayoutCache::Deallocate() noexcept {
	cache.clear();
	lastCaretSlot = SIZE_MAX;
}

void LineLayoutCache::Invalidate(LineLayout::ValidLevel validity_) noexcept {
	if (!cache.empty() && !allInvalidated) {
		for (const auto &ll : cache) {
			if (ll) {
				ll->Invalidate(validity_);
			}
		}
		if (validity_ == LineLayout::ValidLevel::invalid) {
			allInvalidated = true;
		}
	}
}

void LineLayoutCache::SetLevel(LineCache level_) noexcept {
	if (level != level_) {
		level = level_;
		allInvalidated = false;
		cache.clear();
		lastCaretSlot = SIZE_MAX;
	}
}

LineLayout *LineLayoutCache::Retrieve(Sci::Line lineNumber, Sci::Line lineCaret, int maxChars, int styleClock_,
	Sci::Line linesOnScreen, Sci::Line linesInDoc, Sci::Line topLine) {
	AllocateForLevel(linesOnScreen, linesInDoc);
	if (styleClock != styleClock_) {
		Invalidate(LineLayout::ValidLevel::checkTextAndStyle);
		styleClock = styleClock_;
	}
	allInvalidated = false;

	size_t pos = 0;
	if (level == LineCache::Page) {
		// two arenas, each with two pages to ensure cache efficiency on scrolling.
		// first arena for lines near top visible line.
		// second arena for other lines, e.g. folded lines near top visible line.
		// TODO: use/cleanup second arena after some periods, e.g. after Editor::WrapLines() finished.
		const size_t diff = std::abs(lineNumber - topLine);
		const size_t gap = cache.size() / 2;
		pos = 1 + (lineNumber % gap) + ((diff < gap) ? 0 : gap);
		// first slot reserved for caret line, which is rapidly retrieved when caret blinking.
		if (lineNumber == lineCaret) {
			if (lastCaretSlot == 0 && cache[0]->lineNumber == lineCaret) {
				pos = 0;
			} else {
				lastCaretSlot = pos;
			}
		} else if (pos == lastCaretSlot) {
			// save cache for caret line.
			lastCaretSlot = 0;
			std::swap(cache[0], cache[pos]);
		}
	} else if (level == LineCache::Caret) {
		pos = lineNumber != lineCaret;
	} else if (level == LineCache::Document) {
		pos = lineNumber;
	}

	LineLayout *ret = cache[pos].get();
	if (ret) {
		if (!ret->CanHold(lineNumber, maxChars)) {
			//printf("USE line=%zd/%zd, caret=%zd/%zd top=%zd, pos=%zu, clock=%d\n",
			//	lineNumber, ret->lineNumber, lineCaret, lastCaretSlot, topLine, pos, styleClock_);
			ret->~LineLayout();
			ret = new (ret) LineLayout(lineNumber, maxChars);
		} else {
			//printf("HIT line=%zd, caret=%zd/%zd top=%zd, pos=%zu, clock=%d, validity=%d\n",
			//	lineNumber, lineCaret, lastCaretSlot, topLine, pos, styleClock_, ret->validity);
		}
	} else {
		//printf("NEW line=%zd, caret=%zd/%zd top=%zd, pos=%zu, clock=%d\n",
		//	lineNumber, lineCaret, lastCaretSlot, topLine, pos, styleClock_);
		cache[pos] = std::make_unique<LineLayout>(lineNumber, maxChars);
		ret = cache[pos].get();
	}

	// LineLineCache::None is not supported, we only use LineCache::Page.
	return ret;
}

// Simply pack the (maximum 4) character bytes into an int
static constexpr unsigned int KeyFromString(std::string_view charBytes) noexcept {
	PLATFORM_ASSERT(charBytes.length() <= 4);
	unsigned int k = 0;
	for (const unsigned char uc : charBytes) {
		k = (k << 8) | uc;
	}
	return k;
}

void SpecialRepresentations::SetRepresentation(std::string_view charBytes, std::string_view value) {
	if ((charBytes.length() <= 4) && (value.length() <= Representation::maxLength)) {
		const unsigned int key = KeyFromString(charBytes);
		const auto [it, inserted] = mapReprs.try_emplace(key, value);
		if (inserted) {
			// New entry so increment for first byte
			const unsigned char ucStart = charBytes.empty() ? 0 : charBytes[0];
			startByteHasReprs[ucStart]++;
		} else {
			it->second = Representation(value);
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
		it->second.appearance = mapReprs[key].appearance | RepresentationAppearance::Colour;
		it->second.colour = colour;
	}
}

void SpecialRepresentations::ClearRepresentation(std::string_view charBytes) {
	if (charBytes.length() <= 4) {
		const auto it = mapReprs.find(KeyFromString(charBytes));
		if (it != mapReprs.end()) {
			mapReprs.erase(it);
			const unsigned char ucStart = charBytes.empty() ? 0 : charBytes[0];
			startByteHasReprs[ucStart]--;
		}
	}
}

const Representation *SpecialRepresentations::RepresentationFromCharacter(std::string_view charBytes) const {
	if (charBytes.length() <= 4) {
		const unsigned char ucStart = charBytes.empty() ? 0 : charBytes[0];
		if (!startByteHasReprs[ucStart])
			return nullptr;
		const auto it = mapReprs.find(KeyFromString(charBytes));
		if (it != mapReprs.end()) {
			return &(it->second);
		}
	}
	return nullptr;
}

bool SpecialRepresentations::Contains(std::string_view charBytes) const {
	PLATFORM_ASSERT(charBytes.length() <= 4);
	const unsigned char ucStart = charBytes.empty() ? 0 : charBytes[0];
	if (!startByteHasReprs[ucStart])
		return false;
	const auto it = mapReprs.find(KeyFromString(charBytes));
	return it != mapReprs.end();
}

void SpecialRepresentations::Clear() noexcept {
	mapReprs.clear();
	constexpr unsigned char none = 0;
	std::fill(startByteHasReprs, std::end(startByteHasReprs), none);
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

BreakFinder::BreakFinder(const LineLayout *ll_, const Selection *psel, Range lineRange_, Sci::Position posLineStart_,
	XYPOSITION xStart, bool breakForSelection, const Document *pdoc_, const SpecialRepresentations *preprs_, const ViewStyle *pvsDraw) :
	ll(ll_),
	lineRange(lineRange_),
	posLineStart(posLineStart_),
	nextBreak(static_cast<int>(lineRange_.start)),
	saeCurrentPos(0),
	saeNext(0),
	subBreak(-1),
	pdoc(pdoc_),
	encodingFamily(pdoc_->CodePageFamily()),
	preprs(preprs_) {

	// Search for first visible break
	// First find the first visible character
	if (xStart > 0.0f)
		nextBreak = ll->FindBefore(xStart, lineRange);
	// Now back to a style break
	while ((nextBreak > lineRange.start) && (ll->styles[nextBreak] == ll->styles[nextBreak - 1])) {
		nextBreak--;
	}

	if (breakForSelection) {
		const SelectionPosition posStart(posLineStart);
		const SelectionPosition posEnd(posLineStart + lineRange.end);
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
	}
	if (pvsDraw && pvsDraw->indicatorsSetFore) {
		for (const auto *const deco : pdoc->decorations->View()) {
			if (pvsDraw->indicators[deco->Indicator()].OverridesTextFore()) {
				Sci::Position startPos = deco->EndRun(posLineStart);
				while (startPos < (posLineStart + lineRange.end)) {
					Insert(startPos - posLineStart);
					startPos = deco->EndRun(startPos);
				}
			}
		}
	}
	Insert(ll->edgeColumn);
	Insert(lineRange.end);
	saeNext = (!selAndEdge.empty()) ? selAndEdge[0] : -1;
}

BreakFinder::~BreakFinder() = default;

TextSegment BreakFinder::Next() {
	if (subBreak == -1) {
		const int prev = nextBreak;
		while (nextBreak < lineRange.end) {
			int charWidth = 1;
			if (encodingFamily == EncodingFamily::unicode)
				charWidth = UTF8DrawBytes(reinterpret_cast<unsigned char *>(&ll->chars[nextBreak]),
					static_cast<int>(lineRange.end - nextBreak));
			else if (encodingFamily == EncodingFamily::dbcs)
				charWidth = pdoc->DBCSDrawBytes(
					std::string_view(&ll->chars[nextBreak], lineRange.end - nextBreak));
			// Special case \r\n line ends if there is a representation
			if (preprs->Contains("\r\n") && ll->chars[nextBreak] == '\r' && ll->chars[nextBreak + 1] == '\n')
				charWidth = 2;
			const Representation *repr = preprs->RepresentationFromCharacter(std::string_view(&ll->chars[nextBreak], charWidth));
			if (((nextBreak > 0) && (ll->styles[nextBreak] != ll->styles[nextBreak - 1])) ||
				repr ||
				(nextBreak == saeNext)) {
				while ((nextBreak >= saeNext) && (saeNext < lineRange.end)) {
					saeCurrentPos++;
					saeNext = static_cast<int>((saeCurrentPos < selAndEdge.size()) ? selAndEdge[saeCurrentPos] : lineRange.end);
				}
				if ((nextBreak > prev) || repr) {
					// Have a segment to report
					if (nextBreak == prev) {
						nextBreak += charWidth;
					} else {
						repr = nullptr;	// Optimize -> should remember repr
					}
					if ((nextBreak - prev) < lengthStartSubdivision) {
						return TextSegment(prev, nextBreak - prev, repr);
					} else {
						break;
					}
				}
			}
			nextBreak += charWidth;
		}
		if ((nextBreak - prev) < lengthStartSubdivision) {
			return TextSegment(prev, nextBreak - prev);
		}
		subBreak = prev;
	}
	// Splitting up a long run from prev to nextBreak in lots of approximately lengthEachSubdivision.
	// For very long runs add extra breaks after spaces or if no spaces before low punctuation.
	const int startSegment = subBreak;
	if ((nextBreak - subBreak) <= lengthEachSubdivision) {
		subBreak = -1;
		return TextSegment(startSegment, nextBreak - startSegment);
	} else {
		subBreak += pdoc->SafeSegment(&ll->chars[subBreak], nextBreak - subBreak, lengthEachSubdivision);
		if (subBreak >= nextBreak) {
			subBreak = -1;
			return TextSegment(startSegment, nextBreak - startSegment);
		} else {
			return TextSegment(startSegment, subBreak - startSegment);
		}
	}
}

bool BreakFinder::More() const noexcept {
	return (subBreak >= 0) || (nextBreak < lineRange.end);
}

PositionCacheEntry::PositionCacheEntry() noexcept :
	styleNumber(0), len(0), clock(0) {
}

// Copy constructor not currently used, but needed for being element in std::vector.
PositionCacheEntry::PositionCacheEntry(const PositionCacheEntry &other) :
	styleNumber(other.styleNumber), len(other.styleNumber), clock(other.styleNumber) {
	if (other.positions) {
		const size_t lenData = len + (len / sizeof(XYPOSITION)) + 1;
		positions = std::make_unique<XYPOSITION[]>(lenData);
		memcpy(positions.get(), other.positions.get(), lenData * sizeof(XYPOSITION));
	}
}

void PositionCacheEntry::Set(unsigned int styleNumber_, std::string_view sv,
	const XYPOSITION *positions_, unsigned int clock_) {
	styleNumber = styleNumber_;
	len = static_cast<unsigned int>(sv.length());
	clock = clock_;
	if (sv.data() && positions_) {
		positions = std::make_unique<XYPOSITION[]>(len + (len / sizeof(XYPOSITION)) + 1);
		for (unsigned int i = 0; i < len; i++) {
			positions[i] = positions_[i];
		}
		memcpy(&positions[len], sv.data(), sv.length());
	} else {
		positions.reset();
	}
}

PositionCacheEntry::~PositionCacheEntry() {
	Clear();
}

void PositionCacheEntry::Clear() noexcept {
	positions.reset();
	styleNumber = 0;
	len = 0;
	clock = 0;
}

bool PositionCacheEntry::Retrieve(unsigned int styleNumber_, std::string_view sv, XYPOSITION *positions_) const noexcept {
	if ((styleNumber == styleNumber_) && (len == sv.length()) &&
		(memcmp(&positions[len], sv.data(), sv.length()) == 0)) {
		for (unsigned int i = 0; i < len; i++) {
			positions_[i] = positions[i];
		}
		return true;
	} else {
		return false;
	}
}

size_t PositionCacheEntry::Hash(unsigned int styleNumber_, std::string_view sv) noexcept {
	const size_t h1 = std::hash<std::string_view>{}(sv);
	const size_t h2 = std::hash<unsigned int>{}(styleNumber_);
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

#define PositionCacheHashSizeUsePowerOfTwo	1
PositionCache::PositionCache() {
	clock = 1;
#if PositionCacheHashSizeUsePowerOfTwo
	pces.resize(2048);
#else
	pces.resize(2039);
#endif
	allClear = true;
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
	if (size_ != pces.size()) {
#if PositionCacheHashSizeUsePowerOfTwo
		if (size_ & (size_ - 1)) {
			size_ = NextPowerOfTwo(size_);
		}
#endif
		pces.resize(size_);
	}
}

size_t PositionCache::GetSize() const noexcept {
	return pces.size();
}

void PositionCache::MeasureWidths(Surface *surface, const ViewStyle &vstyle, unsigned int styleNumber,
	std::string_view sv, XYPOSITION *positions) {

	allClear = false;
	size_t probe = pces.size();	// Out of bounds
	if ((sv.length() < 64)) {
		// Only store short strings in the cache so it doesn't churn with
		// long comments with only a single comment.
#if PositionCacheHashSizeUsePowerOfTwo
		const size_t mask = probe - 1;
#else
		const size_t modulo = probe;
#endif
		// Two way associative: try two probe positions.
		const size_t hashValue = PositionCacheEntry::Hash(styleNumber, sv);
#if PositionCacheHashSizeUsePowerOfTwo
		probe = hashValue & mask;
#else
		probe = hashValue % modulo;
#endif
		if (pces[probe].Retrieve(styleNumber, sv, positions)) {
			return;
		}
#if PositionCacheHashSizeUsePowerOfTwo
		const size_t probe2 = (hashValue * 37) & mask;
#else
		const size_t probe2 = (hashValue * 37) % modulo;
#endif
		if (pces[probe2].Retrieve(styleNumber, sv, positions)) {
			return;
		}
		// Not found. Choose the oldest of the two slots to replace
		if (pces[probe].NewerThan(pces[probe2])) {
			probe = probe2;
		}
	}
	const Font *fontStyle = vstyle.styles[styleNumber].font.get();
	surface->MeasureWidths(fontStyle, sv, positions);
	if (probe < pces.size()) {
		// Store into cache
		clock++;
		if (clock > 60000) {
			// Since there are only 16 bits for the clock, wrap it round and
			// reset all cache entries so none get stuck with a high clock.
			for (PositionCacheEntry &pce : pces) {
				pce.ResetClock();
			}
			clock = 2;
		}
		pces[probe].Set(styleNumber, sv, positions, clock);
	}
}
