// Scintilla source code edit control
/** @file ViewStyle.h
 ** Store information on how the document is to be viewed.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

/**
 */
class MarginStyle final {
public:
	Scintilla::MarginType style;
	ColourRGBA back;
	int width;
	MarkerMask mask;
	bool sensitive = false;
	Scintilla::CursorShape cursor = Scintilla::CursorShape::ReverseArrow;
	constexpr MarginStyle(Scintilla::MarginType style_ = Scintilla::MarginType::Symbol, int width_ = 0, MarkerMask mask_ = 0) noexcept:
		style(style_), width(width_), mask(mask_) {}
	constexpr bool ShowsFolding() const noexcept {
		return (mask & Scintilla::MaskFolders) != 0;
	}
};

/**
 */
class FontRealised final {
public:
	FontMeasurements measurements;
	std::shared_ptr<Font> font;
	void Realise(Surface &surface, int zoomLevel, Scintilla::Technology technology, const FontSpecification &fs, const char *localeName);
};

using FontMap = std::map<FontSpecification, std::unique_ptr<FontRealised>>;
using ColourOptional = std::optional<ColourRGBA>;

constexpr int GetFontSizeZoomed(int size, int zoomLevel) noexcept {
	size = (size * zoomLevel + 50) / 100;
	// May fail if sizeZoomed (in point) < 1
	return std::max(size, Scintilla::FontSizeMultiplier);
}

constexpr ColourOptional OptionalColour(uptr_t wParam, sptr_t lParam) {
	if (wParam) {
		return ColourRGBA::FromIpRGB(lParam);
	}
	return std::nullopt;
}

struct SelectionAppearance {
	// Whether to draw on base layer or over text
	Scintilla::Layer layer = Layer::Base;
	// Is the selection visible?
	bool visible = true;
	// Draw selection past line end characters up to right border
	bool eolFilled = false;
	int eolSelectedWidth = 100;
};

struct CaretLineAppearance {
	// Whether to draw on base layer or over text
	Scintilla::Layer layer = Layer::Base;
	// Also show when non-focused
	bool alwaysShow = false;
	// highlight sub line instead of whole line
	bool subLine = false;
	// Non-0: draw a rectangle around line instead of filling line. Value is pixel width of frame
	int frame = 0;
};

struct CaretAppearance {
	// Line, block, over-strike bar ...
	Scintilla::CaretStyle style = CaretStyle::Line;
	// Width in pixels
	int width = 1;
};

struct WrapAppearance {
	// No wrapping, word, character, whitespace appearance
	Scintilla::Wrap state = Wrap::None;
	// Show indication of wrap at line end, line start, or in margin
	Scintilla::WrapVisualFlag visualFlags = WrapVisualFlag::None;
	// Show indication near margin or near text
	Scintilla::WrapVisualLocation visualFlagsLocation = WrapVisualLocation::Default;
	// How much indentation to show wrapping
	int visualStartIndent = 0;
	// WrapIndentMode::Fixed, Same, Indent, DeepIndent
	Scintilla::WrapIndentMode indentMode = WrapIndentMode::Fixed;
};

struct EdgeProperties {
	int column;
	ColourRGBA colour;
	constexpr EdgeProperties(int column_ = 0, ColourRGBA colour_ = ColourRGBA(0)) noexcept :
		column(column_), colour(colour_) {}
};

// This is an old style enum so that its members can be used directly as indices without casting
enum StyleIndices {
	StyleDefault = static_cast<int>(Scintilla::StylesCommon::Default),
	StyleLineNumber = static_cast<int>(Scintilla::StylesCommon::LineNumber),
	StyleBraceLight = static_cast<int>(Scintilla::StylesCommon::BraceLight),
	StyleBraceBad = static_cast<int>(Scintilla::StylesCommon::BraceBad),
	StyleControlChar = static_cast<int>(Scintilla::StylesCommon::ControlChar),
	StyleIndentGuide = static_cast<int>(Scintilla::StylesCommon::IndentGuide),
	StyleCallTip = static_cast<int>(Scintilla::StylesCommon::CallTip),
	StyleFoldDisplayText = static_cast<int>(Scintilla::StylesCommon::FoldDisplayText),
};

/**
 */
class ViewStyle final {
	UniqueStringSet fontNames;
	FontMap fonts;
public:
	std::vector<Style> styles;
	std::vector<LineMarker> markers;
	std::vector<Indicator> indicators;
	int nextExtendedStyle;
	int largestMarkerHeight;
	bool indicatorsDynamic;
	bool indicatorsSetFore;
	bool fontsValid;
	Scintilla::Technology technology;
	int lineHeight;
	int lineOverlap;
	XYPOSITION maxAscent;
	XYPOSITION maxDescent;
	XYPOSITION aveCharWidth;
	XYPOSITION spaceWidth;
	XYPOSITION tabWidth;

	SelectionAppearance selection;

	int controlCharSymbol;
	XYPOSITION controlCharWidth;
	ColourRGBA selbar;
	ColourRGBA selbarlight;
	ColourOptional foldmarginColour;
	ColourOptional foldmarginHighlightColour;
	bool hotspotUnderline;
	bool marginInside;	///< true: margin included in text view, false: separate views
	/// Margins are ordered: Line Numbers, Selection Margin, Spacing Margin
	int leftMarginWidth;	///< Spacing margin on left of text
	int rightMarginWidth;	///< Spacing margin on right of text
	MarkerMask maskInLine = 0;	///< Mask for markers to be put into text because there is nowhere for them to go in margin
	MarkerMask maskDrawInText = 0;///< Mask for markers that always draw in text
	MarkerMask maskDrawWrapped = 0;	///< Mask for markers that draw on wrapped lines
	std::vector<MarginStyle> ms;
	int fixedColumnWidth = 0;	///< Total width of margins
	int textStart;	///< Starting x position of text within the view
	/// 2018-09-04 Changed to a percent value
	int zoomLevel;
	Scintilla::WhiteSpace viewWhitespace;
	Scintilla::TabDrawMode tabDrawMode;
	int whitespaceSize;
	Scintilla::IndentView viewIndentationGuides;
	bool viewEOL;

	CaretAppearance caret;
	CaretLineAppearance caretLine;

	bool someStylesProtected;
	bool someStylesForceCase;
	Scintilla::FontQuality extraFontFlag;
	int extraAscent;
	int extraDescent;

	Scintilla::AnnotationVisible annotationVisible;
	int annotationStyleOffset;
	Scintilla::EOLAnnotationVisible eolAnnotationVisible;
	int eolAnnotationStyleOffset;
	bool braceHighlightIndicatorSet;
	bool braceBadLightIndicatorSet;
	int braceHighlightIndicator;
	int braceBadLightIndicator;

	Scintilla::EdgeVisualStyle edgeState;
	EdgeProperties theEdge;
	std::vector<EdgeProperties> theMultiEdge;

	int marginStyleOffset;
	int marginNumberPadding; // the right-side padding of the number margin
	int ctrlCharPadding; // the padding around control character text blobs
	int lastSegItalicsOffset; // the offset so as not to clip italic characters at EOLs

	uint32_t elementColoursMask;
	uint32_t elementBaseColoursMask;
	std::vector<ColourRGBA> elementColours;
	std::vector<ColourRGBA> elementBaseColours;

	WrapAppearance wrap;

	std::string localeName;

	ViewStyle(size_t stylesSize_ = 256);
	ViewStyle(const ViewStyle &source);
	ViewStyle(ViewStyle &&) = delete;
	// Can only be copied through copy constructor which ensures font names initialised correctly
	ViewStyle &operator=(const ViewStyle &) = delete;
	ViewStyle &operator=(ViewStyle &&) = delete;
	~ViewStyle();
	void CalculateMarginWidthAndMask() noexcept;
	void Refresh(Surface &surface, int tabInChars);
	void ReleaseAllExtendedStyles() noexcept;
	int AllocateExtendedStyles(int numberStyles);
	void CopyStyles(size_t sourceIndex, size_t destStyles);
	void EnsureStyle(size_t index);
	void ResetDefaultStyle();
	void ClearStyles() noexcept;
	void SetStyleFontName(int styleIndex, const char *name);
	void SetFontLocaleName(const char *name);
	bool ProtectionActive() const noexcept;
	int ExternalMarginWidth() const noexcept;
	int SCICALL MarginFromLocation(Point pt) const noexcept;
	bool ValidStyle(size_t styleIndex) const noexcept;
	void CalcLargestMarkerHeight() noexcept;
	int GetFrameWidth() const noexcept;
	bool IsLineFrameOpaque(bool caretActive, bool lineContainsCaret) const noexcept;
	ColourOptional Background(MarkerMask marksOfLine, bool caretActive, bool lineContainsCaret) const noexcept;
	bool SelectionTextDrawn() const noexcept;
	bool SelectionBackgroundDrawn() const noexcept;
	bool WhitespaceBackgroundDrawn() const noexcept;
	ColourRGBA WrapColour() const noexcept;

	void AddMultiEdge(int column, ColourRGBA colour);

	ColourOptional ElementColour(Scintilla::Element element) const noexcept;
	ColourRGBA ElementColourForced(Scintilla::Element element) const noexcept;
	static constexpr bool ElementAllowsTranslucent(Scintilla::Element element) noexcept {
		return (element >= Scintilla::Element::SelectionText && element <= Scintilla::Element::WhiteSpace)
			|| element == Scintilla::Element::HotSpotActive;
	}
	bool ResetElement(Scintilla::Element element) noexcept;
	bool SetElementColour(Scintilla::Element element, ColourRGBA colour) noexcept;
	bool ElementIsSet(Scintilla::Element element) const noexcept;
	bool SetElementBase(Scintilla::Element element, ColourRGBA colour) noexcept;

	bool SetWrapState(Scintilla::Wrap wrapState_) noexcept;
	bool SetWrapVisualFlags(Scintilla::WrapVisualFlag wrapVisualFlags_) noexcept;
	bool SetWrapVisualFlagsLocation(Scintilla::WrapVisualLocation wrapVisualFlagsLocation_) noexcept;
	bool SetWrapVisualStartIndent(int wrapVisualStartIndent_) noexcept;
	bool SetWrapIndentMode(Scintilla::WrapIndentMode wrapIndentMode_) noexcept;

	bool WhiteSpaceVisible(bool inIndent) const noexcept;

	enum class CaretShape { invisible, line, block, bar };
	bool IsBlockCaretStyle() const noexcept;
	bool IsCaretVisible(bool isMainSelection) const noexcept;
	bool DrawCaretInsideSelection(bool inOverstrike, bool imeCaretBlockOverride) const noexcept;
	CaretShape CaretShapeForMode(bool inOverstrike, bool isMainSelection, bool drawDrag, bool drawOverstrikeCaret, bool imeCaretBlockOverride) const noexcept;

	bool ZoomIn() noexcept;
	bool ZoomOut() noexcept;

private:
	XYPOSITION maxFontAscent;
	XYPOSITION maxFontDescent;
	void CreateAndAddFont(const FontSpecification &fs);
	FontRealised *Find(const FontSpecification &fs) const;
	void FindMaxAscentDescent() noexcept;
};

}
