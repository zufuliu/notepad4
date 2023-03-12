// Scintilla source code edit control
/** @file PositionCache.h
 ** Classes for caching layout information.
 **/
// Copyright 1998-2009 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

/**
* A point in document space.
* Uses double for sufficient resolution in large (>20,000,000 line) documents.
*/
class PointDocument final {
public:
	double x;
	double y;

	explicit PointDocument(double x_ = 0, double y_ = 0) noexcept : x(x_), y(y_) {}

	// Conversion from Point.
	explicit PointDocument(Point pt) noexcept : x(pt.x), y(pt.y) {}
};

// There are two points for some positions and this enumeration
// can choose between the end of the first line or subline
// and the start of the next line or subline.
enum class PointEnd {
	start = 0x0,
	lineEnd = 0x1,
	subLineEnd = 0x2,
	endEither = lineEnd | subLineEnd,
};

class BidiData final {
public:
	std::vector<std::shared_ptr<Font>> stylesFonts;
	std::vector<XYPOSITION> widthReprs;
	void Resize(size_t maxLineLength_);
};

/**
 */
class LineLayout final {
private:
	std::unique_ptr<int[]> lineStarts;
	/// Drawing is only performed for @a maxLineLength characters on each line.
	Sci::Line lineNumber;
	int lenLineStarts;
public:
	enum {
		wrapWidthInfinite = 0x7ffffff
	};

	int maxLineLength;
	int lastSegmentEnd;
	int numCharsInLine;
	int numCharsBeforeEOL;
	enum class ValidLevel {
		invalid, checkTextAndStyle, positions, lines
	} validity;
	int xHighlightGuide;
	bool highlightColumn;
	bool containsCaret;
	unsigned char bracePreviousStyles[2];
	int edgeColumn;
	int caretPosition;
	std::unique_ptr<char[]> chars;
	std::unique_ptr<unsigned char[]> styles;
	std::unique_ptr<XYPOSITION[]> positions;

	std::unique_ptr<BidiData> bidiData;

	// Wrapped line support
	int widthLine;
	int lines;
	XYPOSITION wrapIndent; // In pixels

	LineLayout(Sci::Line lineNumber_, int maxLineLength_);
	// Deleted so LineLayout objects can not be copied.
	LineLayout(const LineLayout &) = delete;
	LineLayout(LineLayout &&) = delete;
	void operator=(const LineLayout &) = delete;
	void operator=(LineLayout &&) = delete;
	~LineLayout();
	void Resize(int maxLineLength_);
	void Reset(Sci::Line lineNumber_, Sci::Position maxLineLength_);
	void EnsureBidiData();
	void Free() noexcept;
	void ClearPositions() const noexcept;
	void Invalidate(ValidLevel validity_) noexcept;
	Sci::Line LineNumber() const noexcept {
		return lineNumber;
	}
	bool PartialPosition() const noexcept {
		return lastSegmentEnd < numCharsInLine;
	}
	bool CanHold(Sci::Line lineDoc, int lineLength_) const noexcept;
	int LineStart(int line) const noexcept;
	int LineLength(int line) const noexcept;
	enum class Scope {
		visibleOnly, includeEnd
	};
	int LineLastVisible(int line, Scope scope) const noexcept;
	Range SubLineRange(int subLine, Scope scope) const noexcept;
	bool InLine(int offset, int line) const noexcept;
	int SubLineFromPosition(int posInLine, PointEnd pe) const noexcept;
	void AddLineStart(Sci::Position start);
	void SetBracesHighlight(Range rangeLine, const Sci::Position braces[],
		unsigned char bracesMatchStyle, int xHighlight, bool ignoreStyle) noexcept;
	void RestoreBracesHighlight(Range rangeLine, const Sci::Position braces[], bool ignoreStyle) noexcept;
	int SCICALL FindBefore(XYPOSITION x, Range range) const noexcept;
	int SCICALL FindPositionFromX(XYPOSITION x, Range range, bool charPosition) const noexcept;
	Point PointFromPosition(int posInLine, int lineHeight, PointEnd pe) const noexcept;
	XYPOSITION XInLine(Sci::Position index) const noexcept;
	Interval Span(int start, int end) const noexcept;
	Interval SpanByte(int index) const noexcept;
	int EndLineStyle() const noexcept;
	void SCICALL WrapLine(const Document *pdoc, Sci::Position posLineStart, Wrap wrapState, XYPOSITION wrapWidth, XYPOSITION wrapIndent_, bool partialLine);
};

struct ScreenLine : public IScreenLine {
	const LineLayout *ll;
	size_t start;
	size_t len;
	XYPOSITION width;
	XYPOSITION height;
	XYPOSITION tabWidth;
	int ctrlCharPadding;
	int tabWidthMinimumPixels;

	ScreenLine(const LineLayout *ll_, int subLine, const ViewStyle &vs, XYPOSITION width_, int tabWidthMinimumPixels_) noexcept;
	// Deleted so ScreenLine objects can not be copied.
	ScreenLine(const ScreenLine &) = delete;
	ScreenLine(ScreenLine &&) = delete;
	void operator=(const ScreenLine &) = delete;
	void operator=(ScreenLine &&) = delete;
	~ScreenLine() noexcept override;

	std::string_view Text() const noexcept override;
	size_t Length() const noexcept override;
	size_t RepresentationCount() const override;
	XYPOSITION Width() const noexcept override;
	XYPOSITION Height() const noexcept override;
	XYPOSITION TabWidth() const noexcept override;
	XYPOSITION TabWidthMinimumPixels() const noexcept override;
	const Font *FontOfPosition(size_t position) const noexcept override;
	XYPOSITION RepresentationWidth(size_t position) const noexcept override;
	XYPOSITION TabPositionAfter(XYPOSITION xPosition) const noexcept override;
};

struct SignificantLines {
	Sci::Line lineCaret;
	Sci::Line lineTop;
	Sci::Line linesOnScreen;
	Sci::Line linesTotal;
	int styleClock;
	Scintilla::LineCache level;
	bool LineMayCache(Sci::Line line) const noexcept;
};

/**
 */
class LineLayoutCache final {
private:
	std::vector<std::unique_ptr<LineLayout>> shortCache;
	std::vector<std::unique_ptr<LineLayout>> longCache;
	size_t lastCaretSlot;
	Scintilla::LineCache level;
	bool allInvalidated;
	int styleClock;
	void AllocateForLevel(Sci::Line linesOnScreen, Sci::Line linesInDoc);
public:
	LineLayoutCache() noexcept;
	// Deleted so LineLayoutCache objects can not be copied.
	LineLayoutCache(const LineLayoutCache &) = delete;
	LineLayoutCache(LineLayoutCache &&) = delete;
	void operator=(const LineLayoutCache &) = delete;
	void operator=(LineLayoutCache &&) = delete;
	~LineLayoutCache();
	void Deallocate() noexcept;
	void Invalidate(LineLayout::ValidLevel validity_) noexcept;
	void SetLevel(Scintilla::LineCache level_) noexcept;
	Scintilla::LineCache GetLevel() const noexcept {
		return level;
	}
	LineLayout* SCICALL Retrieve(Sci::Line lineNumber, Sci::Line lineCaret, int maxChars, int styleClock_,
		Sci::Line linesOnScreen, Sci::Line linesInDoc, Sci::Line topLine);
	LineLayout* Retrieve(Sci::Line lineNumber, const SignificantLines &significantLines, int maxChars) {
		return Retrieve(lineNumber, significantLines.lineCaret,
			maxChars, significantLines.styleClock,
			significantLines.linesOnScreen, significantLines.linesTotal, significantLines.lineTop);
	}

	static constexpr int UseLongCache(unsigned maxChars) noexcept {
		return maxChars >> (20 + 1); // 2MiB
	}
};

class PositionCacheEntry {
	uint16_t styleNumber = 0;
	uint16_t clock = 0;
	uint32_t len = 0;
	std::unique_ptr<char[]> positions;
public:
	void Set(uint16_t styleNumber_, size_t length, std::unique_ptr<char[]> &positions_, uint32_t clock_) noexcept;
	void Clear() noexcept;
	bool Retrieve(uint16_t styleNumber_, std::string_view sv, XYPOSITION *positions_) const noexcept;
	static size_t Hash(uint16_t styleNumber_, std::string_view sv) noexcept;
	bool NewerThan(const PositionCacheEntry &other) const noexcept;
	void ResetClock() noexcept;
};

class Representation {
public:
	// for Unicode control or format characters in hex code form
	static constexpr size_t maxLength = 7;
	char stringRep[maxLength + 1]{};
	size_t length;
	RepresentationAppearance appearance = RepresentationAppearance::Blob;
	ColourRGBA colour;
	explicit Representation(std::string_view value) noexcept {
		memcpy(stringRep, value.data(), value.length());
		length = value.length();
	}
	std::string_view GetStringRep() const noexcept {
		return {stringRep, length};
	}
};

constexpr char repsC0[][4] = {
	"NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
	"BS", "HT", "LF", "VT", "FF", "CR", "SO", "SI",
	"DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
	"CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US", "BAD"
};

class SpecialRepresentations {
	std::map<unsigned int, Representation> mapReprs;
	unsigned char startByteHasReprs[0x100] {};
	unsigned int maxKey = 0;
	bool crlf = false;
public:
	void SetRepresentation(std::string_view charBytes, std::string_view value);
	void SetRepresentationAppearance(std::string_view charBytes, RepresentationAppearance appearance);
	void SetRepresentationColour(std::string_view charBytes, ColourRGBA colour);
	void ClearRepresentation(std::string_view charBytes);
	const Representation *GetRepresentation(std::string_view charBytes) const;
	const Representation *RepresentationFromCharacter(std::string_view charBytes) const;
	bool ContainsCrLf() const noexcept {
		return crlf;
	}
	bool MayContains(unsigned char ch) const noexcept {
		return startByteHasReprs[ch] != 0;
	}
	void Clear() noexcept;
	void SetDefaultRepresentations(int dbcsCodePage);
};

struct TextSegment {
	const int start;
	const int length;
	const Representation * const representation;
	int end() const noexcept {
		return start + length;
	}
};

class EditModel;

// Class to break a line of text into shorter runs at sensible places.
class BreakFinder {
	const LineLayout *ll;
	int nextBreak;
	int subBreak;
	const int endPos;
	int stopPos;
	int currentPos;
	std::vector<int> selAndEdge;
	unsigned int saeCurrentPos;
	int saeNext;
	const Document *pdoc;
	const EncodingFamily encodingFamily;
	const SpecialRepresentations &reprs;
	void Insert(Sci::Position val);
public:
	// If a whole run is longer than lengthStartSubdivision then subdivide
	// into smaller runs at spaces or punctuation.
	enum {
		lengthStartSubdivision = 4096
	};
	// Try to make each subdivided run lengthEachSubdivision or shorter.
	enum {
		lengthEachSubdivision = 1024
	};
	enum class BreakFor {
		Text = 0,
		Selection = 1,
		Foreground = 2,
		ForegroundAndSelection = 3,
		Layout = 4,
	};
	BreakFinder(const LineLayout *ll_, const Selection *psel, Range lineRange, Sci::Position posLineStart,
		XYPOSITION xStart, BreakFor breakFor, const EditModel &model, const ViewStyle *pvsDraw, uint32_t posInLine);
	// Deleted so BreakFinder objects can not be copied.
	BreakFinder(const BreakFinder &) = delete;
	BreakFinder(BreakFinder &&) = delete;
	void operator=(const BreakFinder &) = delete;
	void operator=(BreakFinder &&) = delete;
	~BreakFinder();
	TextSegment Next();
	bool More() const noexcept {
		return currentPos < stopPos;
	}
	int CurrentPos() const noexcept {
		return currentPos;
	}
};

class PositionCache {
	std::vector<PositionCacheEntry> pces;
	NativeMutex cacheLock;
	uint32_t clock;
	bool allClear;
public:
	PositionCache();
	void Clear() noexcept;
	void SetSize(size_t size_);
	size_t GetSize() const noexcept;
	void MeasureWidths(Surface *surface, const Style &style, uint16_t styleNumber, std::string_view sv, XYPOSITION *positions);
};

}
