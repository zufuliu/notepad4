// Scintilla source code edit control
/** @file EditView.h
 ** Defines the appearance of the main text area of the editor window.
 **/
// Copyright 1998-2014 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

struct PrintParameters {
	/// 2018-09-04 Changed to a percent value
	int magnification;
	Scintilla::PrintOption colourMode;
	Scintilla::Wrap wrapState;
	PrintParameters() noexcept;
};

/**
* The view may be drawn in separate phases.
*/
enum class DrawPhase {
	none = 0x0,
	back = 0x1,
	indicatorsBack = 0x2,
	text = 0x4,
	indentationGuides = 0x8,
	indicatorsFore = 0x10,
	selectionTranslucent = 0x20,
	lineTranslucent = 0x40,
	foldLines = 0x80,
	carets = 0x100,
	all = 0x1FF
};

enum class LayoutLineOption {
	AutoUpdate,
	ManualUpdate,
	KeepPosition,
	Printing,
};

inline std::string_view FormatNumber(char (&number)[32], size_t value) noexcept {
	//const auto [ptr, error] = std::to_chars(number, std::end(number), value);
	//return std::string_view(number, ptr - number);
	char * const end = number + (10*sizeof(size_t) + 2)/4;
	char *ptr = end;
	do {
		*--ptr = static_cast<char>((value % 10) + '0');
		value /= 10;
	} while (value != 0);
	return std::string_view(ptr, end - ptr);
}

bool ValidStyledText(const ViewStyle &vs, size_t styleOffset, const StyledText &st) noexcept;
int WidestLineWidth(Surface *surface, const ViewStyle &vs, int styleOffset, const StyledText &st);
void DrawTextNoClipPhase(Surface *surface, PRectangle rc, const Style &style, XYPOSITION ybase,
	std::string_view text, DrawPhase phase);
void DrawStyledText(Surface *surface, const ViewStyle &vs, int styleOffset, PRectangle rcText,
	const StyledText &st, size_t start, size_t length, DrawPhase phase);

typedef void (*DrawTabArrowFn)(Surface *surface, PRectangle rcTab, int ymid,
	const ViewStyle &vsDraw, Stroke stroke);

class LineTabstops;

/**
* EditView draws the main text area.
*/
class EditView final {
public:
	PrintParameters printParameters;
	std::unique_ptr<LineTabstops> ldTabstops;
	int tabWidthMinimumPixels;

	bool drawOverstrikeCaret; // used by the curses platform

	/** In bufferedDraw mode, graphics operations are drawn to a pixmap and then copied to
	* the screen. This avoids flashing but is about 30% slower. */
	bool bufferedDraw;
	/** In phasesTwo mode, drawing is performed in two phases, first the background
	* and then the foreground. This avoids chopping off characters that overlap the next run.
	* In multiPhaseDraw mode, drawing is performed in multiple phases with each phase drawing
	* one feature over the whole drawing area, instead of within one line. This allows text to
	* overlap from one line to the next. */
	Scintilla::PhasesDraw phasesDraw;

	int lineWidthMaxSeen;

	bool additionalCaretsBlink;
	bool additionalCaretsVisible;

	bool imeCaretBlockOverride;

	std::unique_ptr<Surface> pixmapLine;
	std::unique_ptr<Surface> pixmapIndentGuide;
	std::unique_ptr<Surface> pixmapIndentGuideHighlight;

	LineLayoutCache llc;
	PositionCache posCache;

	int tabArrowHeight; // draw arrow heads this many pixels above/below line midpoint
	/** Some platforms, notably PLAT_CURSES, do not support Scintilla's native
	 * DrawTabArrow function for drawing tab characters. Allow those platforms to
	 * override it instead of creating a new method in the Surface class that
	 * existing platforms must implement as empty. */
	DrawTabArrowFn customDrawTabArrow;
	DrawWrapMarkerFn customDrawWrapMarker;

	EditView();
	// Deleted so EditView objects can not be copied.
	EditView(const EditView &) = delete;
	EditView(EditView &&) = delete;
	void operator=(const EditView &) = delete;
	void operator=(EditView &&) = delete;
	~EditView();

	bool SetTwoPhaseDraw(bool twoPhaseDraw) noexcept;
	bool SetPhasesDraw(int phases) noexcept;
	bool LinesOverlap() const noexcept;

	void ClearAllTabstops() noexcept;
	XYPOSITION SCICALL NextTabstopPos(Sci::Line line, XYPOSITION x, XYPOSITION tabWidth) const noexcept;
	bool ClearTabstops(Sci::Line line) const noexcept;
	bool AddTabstop(Sci::Line line, int x);
	int GetNextTabstop(Sci::Line line, int x) const noexcept;
	void LinesAddedOrRemoved(Sci::Line lineOfPos, Sci::Line linesAdded) const;

	void DropGraphics() noexcept;
	void RefreshPixMaps(Surface *surfaceWindow, const ViewStyle &vsDraw);

	LineLayout *RetrieveLineLayout(Sci::Line lineNumber, const EditModel &model);
	uint64_t LayoutLine(const EditModel &model, Surface *surface, const ViewStyle &vstyle,
		LineLayout *ll, int width, LayoutLineOption option, int posInLine = 0);

	static void UpdateBidiData(const EditModel &model, const ViewStyle &vstyle, LineLayout *ll);

	Point SCICALL LocationFromPosition(Surface *surface, const EditModel &model, SelectionPosition pos, Sci::Line topLine,
		const ViewStyle &vs, PointEnd pe, PRectangle rcClient);
	Range RangeDisplayLine(Surface *surface, const EditModel &model, Sci::Line lineVisible, const ViewStyle &vs);
	SelectionPosition SCICALL SPositionFromLocation(Surface *surface, const EditModel &model, PointDocument pt, bool canReturnInvalid,
		bool charPosition, bool virtualSpace, const ViewStyle &vs, PRectangle rcClient);
	SelectionPosition SPositionFromLineX(Surface *surface, const EditModel &model, Sci::Line lineDoc, int x, const ViewStyle &vs);
	Sci::Line DisplayFromPosition(Surface *surface, const EditModel &model, Sci::Position pos, const ViewStyle &vs);
	Sci::Position StartEndDisplayLine(Surface *surface, const EditModel &model, Sci::Position pos, bool start, const ViewStyle &vs);

private:
	void SCICALL DrawEOL(Surface *surface, const EditModel &model, const ViewStyle &vsDraw, const LineLayout *ll,
		Sci::Line line, int xStart, PRectangle rcLine, int subLine, Sci::Position lineEnd, XYPOSITION subLineStart, ColourOptional background) const;
	void SCICALL DrawFoldDisplayText(Surface *surface, const EditModel &model, const ViewStyle &vsDraw, const LineLayout *ll,
		Sci::Line line, int xStart, PRectangle rcLine, int subLine, XYPOSITION subLineStart, DrawPhase phase);
	void SCICALL DrawEOLAnnotationText(Surface *surface, const EditModel &model, const ViewStyle &vsDraw, const LineLayout *ll,
		Sci::Line line, int xStart, PRectangle rcLine, int subLine, XYPOSITION subLineStart, DrawPhase phase);
	void SCICALL DrawAnnotation(Surface *surface, const EditModel &model, const ViewStyle &vsDraw, const LineLayout *ll,
		Sci::Line line, int xStart, PRectangle rcLine, int subLine, DrawPhase phase);
	void SCICALL DrawCarets(Surface *surface, const EditModel &model, const ViewStyle &vsDraw, const LineLayout *ll,
		Sci::Line lineDoc, int xStart, PRectangle rcLine, int subLine) const;
	void SCICALL DrawIndentGuide(Surface *surface, XYPOSITION start, PRectangle rcSegment, bool highlight, bool offset) const;
	void SCICALL DrawForeground(Surface *surface, const EditModel &model, const ViewStyle &vsDraw, const LineLayout *ll,
		int xStart, PRectangle rcLine, int subLine, Sci::Line lineVisible, Range lineRange, Sci::Position posLineStart,
		ColourOptional background) const;
	void SCICALL DrawIndentGuidesOverEmpty(Surface *surface, const EditModel &model, const ViewStyle &vsDraw, const LineLayout *ll,
		Sci::Line line, int xStart, PRectangle rcLine, int subLine, Sci::Line lineVisible) const;
	void SCICALL DrawLine(Surface *surface, const EditModel &model, const ViewStyle &vsDraw, const LineLayout *ll,
		Sci::Line line, Sci::Line lineVisible, int xStart, PRectangle rcLine, int subLine, DrawPhase phase);

public:
	void SCICALL PaintText(Surface *surfaceWindow, const EditModel &model, const ViewStyle &vsDraw,
		PRectangle rcArea, PRectangle rcClient);
	Sci::Position SCICALL FormatRange(bool draw, CharacterRangeFull chrg, Scintilla::Rectangle rc, Surface *surface, Surface *surfaceMeasure,
		const EditModel &model, const ViewStyle &vs);
};

}
