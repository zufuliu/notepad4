// Scintilla source code edit control
/** @file ViewStyle.cxx
 ** Store information on how the document is to be viewed.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cassert>
#include <cstring>
#include <cmath>

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <map>
#include <optional>
#include <algorithm>
#include <memory>
#include <numeric>

#include "ScintillaTypes.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"
#include "VectorISA.h"

#include "Position.h"
#include "UniqueString.h"
#include "Indicator.h"
#include "XPM.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"

using namespace Scintilla;
using namespace Scintilla::Internal;
static_assert(StyleDefault == 0);

namespace {

// Colour component proportions of maximum 0xffU
constexpr unsigned int light = 0xc0U;
// The middle point of 0..0xff is between 0x7fU and 0x80U and both are used
constexpr unsigned int mid = 0x80U;
constexpr unsigned int half = 0x7fU;
constexpr unsigned int quarter = 0x3fU;

}

void FontRealised::Realise(Surface &surface, int zoomLevel, Technology technology, const FontSpecification &fs, const char *localeName) {
	PLATFORM_ASSERT(fs.fontName);
	measurements.sizeZoomed = GetFontSizeZoomed(fs.size, zoomLevel);
	const XYPOSITION deviceHeight = static_cast<XYPOSITION>(surface.DeviceHeightFont(measurements.sizeZoomed));
	const FontParameters fp(fs.fontName, deviceHeight / FontSizeMultiplier, fs.weight,
		fs.italic, fs.extraFontFlag, technology, fs.characterSet, localeName);
	font = Font::Allocate(fp);

	// floor here is historical as platform layers have tweaked their values to match.
	// ceil would likely be better to ensure (nearly) all of the ink of a character is seen
	// but that would require platform layer changes.
	measurements.ascent = surface.Ascent(font.get());
	measurements.descent = surface.Descent(font.get());
	measurements.capitalHeight = surface.Ascent(font.get()) - surface.InternalLeading(font.get());
	measurements.aveCharWidth = surface.AverageCharWidth(font.get());
	//measurements.monospaceCharacterWidth = measurements.aveCharWidth;
	measurements.spaceWidth = surface.WidthText(font.get(), " ");

	if (fs.checkMonospaced) {
		// "Ay" is normally strongly kerned and "fi" may be a ligature
		constexpr std::string_view allASCIIGraphic("Ayfi"
		// python: ''.join(chr(ch) for ch in range(32, 127))
		" !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
		std::array<XYPOSITION, allASCIIGraphic.length()> positions {};
		surface.MeasureWidthsUTF8(font.get(), allASCIIGraphic, positions.data());
		std::adjacent_difference(positions.begin(), positions.end(), positions.begin());
		const XYPOSITION maxWidth = *std::max_element(positions.begin(), positions.end());
		const XYPOSITION minWidth = *std::min_element(positions.begin(), positions.end());
		const XYPOSITION variance = maxWidth - minWidth;
		const XYPOSITION scaledVariance = variance / measurements.aveCharWidth;
		constexpr XYPOSITION monospaceWidthEpsilon = 0.000001;	// May need tweaking if monospace fonts vary more
		measurements.monospaceASCII = scaledVariance < monospaceWidthEpsilon;
		//measurements.monospaceCharacterWidth = minWidth;
	} else {
		measurements.monospaceASCII = false;
	}
}

ViewStyle::ViewStyle(size_t stylesSize_):
	styles(stylesSize_),
	markers(MarkerMax + 1),
	indicators(static_cast<size_t>(IndicatorNumbers::Max) + 1),
	ms(MaxMargin + 1),
	elementColours(static_cast<size_t>(Element::Max)),
	elementBaseColours(static_cast<size_t>(Element::Max)) {

	ResetDefaultStyle();

	indicators[0] = Indicator(IndicatorStyle::Squiggle, ColourRGBA(0, half, 0));	// Green
	indicators[1] = Indicator(IndicatorStyle::TT, ColourRGBA(0, 0, maximumByte));	// Blue
	indicators[2] = Indicator(IndicatorStyle::Plain, ColourRGBA(maximumByte, 0, 0));	// Red

	// Reverted to origin
	constexpr ColourRGBA revertedToOrigin(0x40, 0xA0, 0xBF);
	// Saved
	constexpr ColourRGBA saved(0x0, 0xA0, 0x0);
	// Modified
	constexpr ColourRGBA modified(0xFF, 0x80, 0x0);
	// Reverted to change
	constexpr ColourRGBA revertedToChange(0xA0, 0xC0, 0x0);

	// Edition indicators
	constexpr size_t indexHistory = static_cast<size_t>(IndicatorNumbers::HistoryRevertedToOriginInsertion);

	// Default indicators are moderately intense so they don't overwhelm text
	constexpr int alphaFill = 30;
	constexpr int alphaOutline = 50;
	indicators[indexHistory + 0] = Indicator(IndicatorStyle::CompositionThick, revertedToOrigin, false, alphaFill, alphaOutline);
	indicators[indexHistory + 1] = Indicator(IndicatorStyle::Point, revertedToOrigin);
	indicators[indexHistory + 2] = Indicator(IndicatorStyle::CompositionThick, saved, false, alphaFill, alphaOutline);
	indicators[indexHistory + 3] = Indicator(IndicatorStyle::Point, saved);
	indicators[indexHistory + 4] = Indicator(IndicatorStyle::CompositionThick, modified, false, alphaFill, alphaOutline);
	indicators[indexHistory + 5] = Indicator(IndicatorStyle::PointTop, modified);
	indicators[indexHistory + 6] = Indicator(IndicatorStyle::CompositionThick, revertedToChange, false, alphaFill, alphaOutline);
	indicators[indexHistory + 7] = Indicator(IndicatorStyle::Point, revertedToChange);

	// Edition markers
	// Reverted to origin
	constexpr size_t indexHistoryRevertedToOrigin = static_cast<size_t>(MarkerOutline::HistoryRevertedToOrigin);
	markers[indexHistoryRevertedToOrigin].back = revertedToOrigin;
	markers[indexHistoryRevertedToOrigin].fore = revertedToOrigin;
	markers[indexHistoryRevertedToOrigin].markType = MarkerSymbol::Bar;
	// Saved
	constexpr size_t indexHistorySaved = static_cast<size_t>(MarkerOutline::HistorySaved);
	markers[indexHistorySaved].back = saved;
	markers[indexHistorySaved].fore = saved;
	markers[indexHistorySaved].markType = MarkerSymbol::Bar;
	// Modified
	constexpr size_t indexHistoryModified = static_cast<size_t>(MarkerOutline::HistoryModified);
	markers[indexHistoryModified].back = Platform::Chrome();
	markers[indexHistoryModified].fore = modified;
	markers[indexHistoryModified].markType = MarkerSymbol::Bar;
	// Reverted to change
	constexpr size_t indexHistoryRevertedToModified = static_cast<size_t>(MarkerOutline::HistoryRevertedToModified);
	markers[indexHistoryRevertedToModified].back = revertedToChange;
	markers[indexHistoryRevertedToModified].fore = revertedToChange;
	markers[indexHistoryRevertedToModified].markType = MarkerSymbol::Bar;

	nextExtendedStyle = 256;
	// There are no image markers by default, so no need for calling CalcLargestMarkerHeight()
	largestMarkerHeight = 0;

	indicatorsDynamic = false;
	indicatorsSetFore = false;
	technology = Technology::Default;
	lineHeight = 1;
	lineOverlap = 0;
	maxAscent = 1;
	maxDescent = 1;
	aveCharWidth = 8;
	spaceWidth = 8;
	tabWidth = spaceWidth * 8;

	// Default is for no selection foregrounds
	elementColoursMask = 0;
	elementBaseColoursMask = (1 << static_cast<int>(Element::SelectionBack))
		| (1 << static_cast<int>(Element::SelectionAdditionalBack))
		| (1 << static_cast<int>(Element::SelectionSecondaryBack))
		| (1 << static_cast<int>(Element::SelectionInactiveBack))
		| (1 << static_cast<int>(Element::Caret))
		| (1 << static_cast<int>(Element::CaretAdditional));
	// Shades of grey for selection backgrounds
	elementBaseColours[static_cast<unsigned>(Element::SelectionBack)] = ColourRGBA::Grey(light);
	constexpr unsigned int veryLight = 0xd7U;
	elementBaseColours[static_cast<unsigned>(Element::SelectionAdditionalBack)] = ColourRGBA::Grey(veryLight);
	constexpr unsigned int halfLight = 0xb0;
	elementBaseColours[static_cast<unsigned>(Element::SelectionSecondaryBack)] = ColourRGBA::Grey(halfLight);
	elementBaseColours[static_cast<unsigned>(Element::SelectionInactiveBack)] = ColourRGBA::Grey(mid, quarter);
	elementBaseColours[static_cast<unsigned>(Element::Caret)] = black;
	elementBaseColours[static_cast<unsigned>(Element::CaretAdditional)] = ColourRGBA::Grey(half);

	controlCharSymbol = 0;	/* Draw the control characters */
	controlCharWidth = 0;
	selbar = Platform::Chrome();
	selbarlight = Platform::ChromeHighlight();
	styles[StyleLineNumber].fore = black;
	styles[StyleLineNumber].back = Platform::Chrome();

	hotspotUnderline = true;
	marginInside = true;
	leftMarginWidth = 1;
	rightMarginWidth = 1;
	ms[0] = MarginStyle(MarginType::Number);
	ms[1] = MarginStyle(MarginType::Symbol, 16, ~MaskFolders);
	ms[2] = MarginStyle(MarginType::Symbol);
	CalculateMarginWidthAndMask();
	textStart = marginInside ? fixedColumnWidth : leftMarginWidth;
	zoomLevel = 100;
	viewWhitespace = WhiteSpace::Invisible;
	tabDrawMode = TabDrawMode::LongArrow;
	whitespaceSize = 1;
	viewIndentationGuides = IndentView::None;
	viewEOL = false;

	someStylesProtected = false;
	someStylesForceCase = false;
	extraFontFlag = FontQuality::QualityDefault;
	extraAscent = 0;
	extraDescent = 0;

	annotationVisible = AnnotationVisible::Hidden;
	annotationStyleOffset = 0;
	eolAnnotationVisible = EOLAnnotationVisible::Hidden;
	eolAnnotationStyleOffset = 0;
	braceHighlightIndicatorSet = false;
	braceBadLightIndicatorSet = false;
	braceHighlightIndicator = 0;
	braceBadLightIndicator = 0;

	edgeState = EdgeVisualStyle::None;
	theEdge = EdgeProperties(0, ColourRGBA::Grey(light));

	marginStyleOffset = 0;
	marginNumberPadding = 3;
	ctrlCharPadding = 3; // +3 For a blank on front and rounded edge each side
	lastSegItalicsOffset = 2;

	localeName = localeNameDefault;
	maxFontAscent = 1;
	maxFontDescent = 1;
}

// Copy constructor only called when printing copies the screen ViewStyle so it can be
// modified for printing styles.
ViewStyle::ViewStyle(const ViewStyle &source) : ViewStyle(source.styles.size()) {
	styles = source.styles;
	for (auto &style : styles) {
		// Can't just copy fontName as its lifetime is relative to its owning ViewStyle
		style.fontName = fontNames.Save(style.fontName);
	}
	markers = source.markers;
	nextExtendedStyle = source.nextExtendedStyle;
	CalcLargestMarkerHeight();

	indicators = source.indicators;

	indicatorsDynamic = source.indicatorsDynamic;
	indicatorsSetFore = source.indicatorsSetFore;

	selection = source.selection;

	controlCharSymbol = source.controlCharSymbol;
	controlCharWidth = source.controlCharWidth;
	selbar = source.selbar;
	selbarlight = source.selbarlight;
	foldmarginColour = source.foldmarginColour;
	foldmarginHighlightColour = source.foldmarginHighlightColour;
	hotspotUnderline = source.hotspotUnderline;
	marginInside = source.marginInside;
	leftMarginWidth = source.leftMarginWidth;
	rightMarginWidth = source.rightMarginWidth;
	maskInLine = source.maskInLine;
	maskDrawInText = source.maskDrawInText;
	maskDrawWrapped = source.maskDrawWrapped;
	ms = source.ms;
	fixedColumnWidth = source.fixedColumnWidth;
	textStart = source.textStart;
	zoomLevel = source.zoomLevel;
	viewWhitespace = source.viewWhitespace;
	tabDrawMode = source.tabDrawMode;
	whitespaceSize = source.whitespaceSize;
	viewIndentationGuides = source.viewIndentationGuides;
	viewEOL = source.viewEOL;

	caret = source.caret;
	caretLine = source.caretLine;

	someStylesProtected = false;
	someStylesForceCase = false;
	extraFontFlag = source.extraFontFlag;
	extraAscent = source.extraAscent;
	extraDescent = source.extraDescent;

	annotationVisible = source.annotationVisible;
	annotationStyleOffset = source.annotationStyleOffset;
	eolAnnotationVisible = source.eolAnnotationVisible;
	eolAnnotationStyleOffset = source.eolAnnotationStyleOffset;
	braceHighlightIndicatorSet = source.braceHighlightIndicatorSet;
	braceBadLightIndicatorSet = source.braceBadLightIndicatorSet;
	braceHighlightIndicator = source.braceHighlightIndicator;
	braceBadLightIndicator = source.braceBadLightIndicator;

	edgeState = source.edgeState;
	theEdge = source.theEdge;
	theMultiEdge = source.theMultiEdge;

	marginStyleOffset = source.marginStyleOffset;
	marginNumberPadding = source.marginNumberPadding;
	ctrlCharPadding = source.ctrlCharPadding;
	lastSegItalicsOffset = source.lastSegItalicsOffset;

	wrap = source.wrap;

	localeName = source.localeName;
}

ViewStyle::~ViewStyle() = default;

void ViewStyle::CalculateMarginWidthAndMask() noexcept {
	fixedColumnWidth = marginInside ? leftMarginWidth : 0;
	maskInLine = 0xffffffffU;
	MarkerMask maskDefinedMarkers = 0;
	for (const MarginStyle &m : ms) {
		fixedColumnWidth += m.width;
		if (m.width > 0)
			maskInLine &= ~m.mask;
		maskDefinedMarkers |= m.mask;
	}
	maskDrawInText = 0;
	maskDrawWrapped = 0;
	for (int markBit = 0; markBit <= MarkerMax; markBit++) {
		const MarkerMask maskBit = 1U << markBit;
		switch (markers[markBit].markType) {
		case MarkerSymbol::Empty:
			maskInLine &= ~maskBit;
			break;
		case MarkerSymbol::Background:
		case MarkerSymbol::Underline:
			maskInLine &= ~maskBit;
			maskDrawInText |= maskDefinedMarkers & maskBit;
			break;
		case MarkerSymbol::Bar:
			maskDrawWrapped |= maskBit;
			break;
		default:	// Other marker types do not affect the masks
			break;
		}
	}
}

void ViewStyle::Refresh(Surface &surface, int tabInChars) {
	if (!fontsValid) {
		fontsValid = true;
		fonts.clear();

		// Apply the extra font flag which controls text drawing quality to each style.
		const FontQuality quality = extraFontFlag;
		// Create a FontRealised object for each unique font in the styles.
		for (auto &style : styles) {
			style.extraFontFlag = quality;
			CreateAndAddFont(style);
		}

		// Ask platform to allocate each unique font.
		for (const auto &font : fonts) {
			font.second->Realise(surface, zoomLevel, technology, font.first, localeName.c_str());
		}

		// Set the platform font handle and measurements for each style.
		for (auto &style : styles) {
			const FontRealised *fr = Find(style);
			style.Copy(fr->font, fr->measurements);
		}

		FindMaxAscentDescent();
		aveCharWidth = styles[StyleDefault].aveCharWidth;
		spaceWidth = styles[StyleDefault].spaceWidth;
	}

	selbar = Platform::Chrome();
	selbarlight = Platform::ChromeHighlight();

	bool flagDynamic = false;
	bool flagSetFore = false;
	for (const auto &indicator : indicators) {
		if (indicator.IsDynamic()) {
			flagDynamic = true;
		}
		if (indicator.OverridesTextFore()) {
			flagSetFore = true;
		}
	}
	indicatorsDynamic = flagDynamic;
	indicatorsSetFore = flagSetFore;

	maxAscent = maxFontAscent;
	maxDescent = maxFontDescent;
	// Ensure reasonable values: lines less than 1 pixel high will not work
	maxAscent = std::max(1.0, maxAscent + extraAscent);
	maxDescent = std::max(0.0, maxDescent + extraDescent);
	lineHeight = static_cast<int>(std::lround(maxAscent + maxDescent));
	lineOverlap = std::clamp(lineHeight / 10, 2, lineHeight);

	bool flagProtected = false;
	constexpr bool flagForceCase = false;
	for (const auto &style : styles) {
		if (style.IsProtected()) {
			flagProtected = true;
			break;
		}
		//if (style.caseForce != Style::CaseForce::mixed) {
		//	flagForceCase = true;
		//}
	}
	someStylesProtected = flagProtected;
	someStylesForceCase = flagForceCase;

	tabWidth = aveCharWidth * tabInChars;

	controlCharWidth = 0.0;
	if (controlCharSymbol >= 32) {
		const char cc[2] = { static_cast<char>(controlCharSymbol), '\0' };
		controlCharWidth = surface.WidthText(styles[StyleControlChar].font.get(), cc);
	}

	CalculateMarginWidthAndMask();
	textStart = marginInside ? fixedColumnWidth : leftMarginWidth;
}

void ViewStyle::ReleaseAllExtendedStyles() noexcept {
	nextExtendedStyle = 256;
}

int ViewStyle::AllocateExtendedStyles(int numberStyles) {
	const int startRange = nextExtendedStyle;
	nextExtendedStyle += numberStyles;
	EnsureStyle(nextExtendedStyle);
	return startRange;
}

void ViewStyle::EnsureStyle([[maybe_unused]] size_t index) {
#if 0 // we don't use extended style
	const size_t i = styles.size();
	if (index >= i) {
		fontsValid = false;
		index += 1;
		styles.resize(index);
		const Style &source = styles[StyleDefault];
		for (auto it = styles.begin() + i; it != styles.end(); ++it) {
			*it = source;
		}
	}
#endif
}

void ViewStyle::ResetDefaultStyle() {
	fontsValid = false;
	styles[StyleDefault].ResetDefault(fontNames.Save(Platform::DefaultFont()));
}

void ViewStyle::ClearStyles() noexcept {
	fontsValid = false;
	// Reset all styles to be like the default style
	const Style &source = styles[StyleDefault];
	for (auto it = styles.begin() + 1; it != styles.end(); ++it) {
		*it = source;
	}

	styles[StyleLineNumber].back = Platform::Chrome();
	// Set call tip fore/back to match the values previously set for call tips
	styles[StyleCallTip].back = white;
	styles[StyleCallTip].fore = ColourRGBA::Grey(mid);
}

void ViewStyle::CopyStyles(size_t sourceIndex, size_t destStyles) {
	const size_t offset = sourceIndex >> 8;
	sourceIndex &= 0xff;
	const Style &source = styles[sourceIndex];
	do {
		const size_t index = (destStyles & 0xff) + offset;
		assert(sourceIndex != index);
		if (index != sourceIndex) {
			styles[index] = source;
		}
		destStyles >>= 8;
	} while (destStyles);
}

void ViewStyle::SetStyleFontName(int styleIndex, const char *name) {
	fontsValid = false;
	styles[styleIndex].fontName = fontNames.Save(name);
}

void ViewStyle::SetFontLocaleName(const char *name) {
	fontsValid = false;
	localeName = name;
}

bool ViewStyle::ProtectionActive() const noexcept {
	return someStylesProtected;
}

int ViewStyle::ExternalMarginWidth() const noexcept {
	return marginInside ? 0 : fixedColumnWidth;
}

int ViewStyle::MarginFromLocation(Point pt) const noexcept {
	XYPOSITION x = marginInside ? 0 : -fixedColumnWidth;
	int margin = 0;
	for (const auto &marginStyle : ms) {
		const XYPOSITION width = marginStyle.width;
		if ((pt.x >= x) && (pt.x < x + width)) {
			return margin;
		}
		++margin;
		x += width;
	}
	return -1;
}

bool ViewStyle::ValidStyle(size_t styleIndex) const noexcept {
	return styleIndex < styles.size();
}

void ViewStyle::CalcLargestMarkerHeight() noexcept {
	largestMarkerHeight = 0;
	for (const auto &marker : markers) {
		switch (marker.markType) {
		case MarkerSymbol::Pixmap:
			if (marker.pxpm && marker.pxpm->GetHeight() > largestMarkerHeight)
				largestMarkerHeight = marker.pxpm->GetHeight();
			break;
		case MarkerSymbol::RgbaImage:
			if (marker.image && marker.image->GetHeight() > largestMarkerHeight)
				largestMarkerHeight = marker.image->GetHeight();
			break;
		case MarkerSymbol::Bar:
			largestMarkerHeight = lineHeight + 2;
			break;
		default:	// Only images have their own natural heights
			break;
		}
	}
}

int ViewStyle::GetFrameWidth() const noexcept {
	return std::clamp(caretLine.frame, 1, lineHeight / 3);
}

bool ViewStyle::IsLineFrameOpaque(bool caretActive, bool lineContainsCaret) const noexcept {
	return lineContainsCaret && (caretActive || caretLine.alwaysShow)
		&& caretLine.frame != 0 && caretLine.layer == Layer::Base
		&& ElementIsSet(Element::CaretLineBack);
}

// See if something overrides the line background colour:  Either if caret is on the line
// and background colour is set for that, or if a marker is defined that forces its background
// colour onto the line, or if a marker is defined but has no selection margin in which to
// display itself (as long as it's not an MarkerSymbol::Empty marker).  These are checked in order
// with the earlier taking precedence.  When multiple markers cause background override,
// the colour for the highest numbered one is used.
ColourOptional ViewStyle::Background(MarkerMask marksOfLine, bool caretActive, bool lineContainsCaret) const noexcept {
	ColourOptional background;
	if (lineContainsCaret && (caretActive || caretLine.alwaysShow)
		&& caretLine.frame == 0 && caretLine.layer == Layer::Base) {
		background = ElementColour(Element::CaretLineBack);
	}
	if (!background && marksOfLine) {
		MarkerMask marks = marksOfLine;
		for (int markBit = 0; (markBit <= MarkerMax) && marks; markBit++) {
			if ((marks & 1) && (markers[markBit].markType == MarkerSymbol::Background) &&
				(markers[markBit].layer == Layer::Base)) {
				background = markers[markBit].back;
			}
			marks >>= 1;
		}
	}
	if (!background && maskInLine) {
		MarkerMask marksMasked = marksOfLine & maskInLine;
		if (marksMasked) {
			for (int markBit = 0; (markBit <= MarkerMax) && marksMasked; markBit++) {
				if ((marksMasked & 1) &&
					(markers[markBit].layer == Layer::Base)) {
					background = markers[markBit].back;
				}
				marksMasked >>= 1;
			}
		}
	}
	if (background) {
		return background->Opaque();
	} else {
		return {};
	}
}

bool ViewStyle::SelectionBackgroundDrawn() const noexcept {
	return selection.layer == Layer::Base;
}

bool ViewStyle::SelectionTextDrawn() const noexcept {
	constexpr unsigned mask = (1 << static_cast<int>(Element::SelectionText))
		| (1 << static_cast<int>(Element::SelectionAdditionalText))
		| (1 << static_cast<int>(Element::SelectionSecondaryText))
		| (1 << static_cast<int>(Element::SelectionInactiveText))
		| (1 << static_cast<int>(Element::SelectionInactiveAdditionalText));
	return (elementColoursMask & mask) != 0;
}

bool ViewStyle::WhitespaceBackgroundDrawn() const noexcept {
	return (viewWhitespace != WhiteSpace::Invisible) && (ElementIsSet(Element::WhiteSpaceBack));
}

bool ViewStyle::WhiteSpaceVisible(bool inIndent) const noexcept {
	return (!inIndent && viewWhitespace == WhiteSpace::VisibleAfterIndent) ||
		(inIndent && viewWhitespace == WhiteSpace::VisibleOnlyInIndent) ||
		viewWhitespace == WhiteSpace::VisibleAlways;
}

ColourRGBA ViewStyle::WrapColour() const noexcept {
	return ElementColour(Element::WhiteSpace).value_or(styles[StyleDefault].fore).Opaque();
}

// Insert new edge in sorted order.
void ViewStyle::AddMultiEdge(int column, ColourRGBA colour) {
	theMultiEdge.insert(
		std::upper_bound(theMultiEdge.begin(), theMultiEdge.end(), column,
		[](const EdgeProperties &a, const EdgeProperties &b) noexcept {
			return a.column < b.column;
		}),
		EdgeProperties(column, colour));
}

ColourOptional ViewStyle::ElementColour(Element element) const noexcept {
	const auto index = static_cast<unsigned>(element);
	if (bittest(&elementColoursMask, index)) {
		return elementColours[index];
	}
	if (bittest(&elementBaseColoursMask, index)) {
		return elementBaseColours[index];
	}
	return {};
}

ColourRGBA ViewStyle::ElementColourForced(Element element) const noexcept {
	// Like ElementColour but never returns empty - when not found return opaque black.
	// This method avoids warnings for unwrapping potentially empty optionals from
	// Visual C++ Code Analysis
	const ColourOptional colour = ElementColour(element);
	return colour.value_or(black);
}

bool ViewStyle::ResetElement(Element element) noexcept {
	const auto index = static_cast<unsigned>(element);
	if (bittestandreset(&elementColoursMask, index)) {
		elementColours[index] = ColourRGBA();
		return true;
	}
	return false;
}

bool ViewStyle::SetElementColour(Element element, ColourRGBA colour) noexcept {
	const auto index = static_cast<unsigned>(element);
	if (!bittestandset(&elementColoursMask, index) || elementColours[index] != colour) {
		elementColours[index] = colour;
		return true;
	}
	return false;
}

bool ViewStyle::ElementIsSet(Element element) const noexcept {
	return bittest(&elementColoursMask, static_cast<unsigned>(element));
}

bool ViewStyle::SetElementBase(Element element, ColourRGBA colour) noexcept {
	const auto index = static_cast<unsigned>(element);
	if (!bittestandset(&elementBaseColoursMask, index) || elementBaseColours[index] != colour) {
		elementBaseColours[index] = colour;
		return true;
	}
	return false;
}

bool ViewStyle::SetWrapState(Wrap wrapState_) noexcept {
	const bool changed = wrap.state != wrapState_;
	wrap.state = wrapState_;
	return changed;
}

bool ViewStyle::SetWrapVisualFlags(WrapVisualFlag wrapVisualFlags_) noexcept {
	const bool changed = wrap.visualFlags != wrapVisualFlags_;
	wrap.visualFlags = wrapVisualFlags_;
	return changed;
}

bool ViewStyle::SetWrapVisualFlagsLocation(WrapVisualLocation wrapVisualFlagsLocation_) noexcept {
	const bool changed = wrap.visualFlagsLocation != wrapVisualFlagsLocation_;
	wrap.visualFlagsLocation = wrapVisualFlagsLocation_;
	return changed;
}

bool ViewStyle::SetWrapVisualStartIndent(int wrapVisualStartIndent_) noexcept {
	const bool changed = wrap.visualStartIndent != wrapVisualStartIndent_;
	wrap.visualStartIndent = wrapVisualStartIndent_;
	return changed;
}

bool ViewStyle::SetWrapIndentMode(WrapIndentMode wrapIndentMode_) noexcept {
	const bool changed = wrap.indentMode != wrapIndentMode_;
	wrap.indentMode = wrapIndentMode_;
	return changed;
}

bool ViewStyle::IsBlockCaretStyle() const noexcept {
	return ((caret.style & CaretStyle::InsMask) == CaretStyle::Block) ||
		FlagSet(caret.style, (CaretStyle::OverstrikeBlock | CaretStyle::Curses));
}

bool ViewStyle::IsCaretVisible(bool isMainSelection) const noexcept {
	return caret.width > 0 &&
		((caret.style & CaretStyle::InsMask) != CaretStyle::Invisible ||
		(FlagSet(caret.style, CaretStyle::Curses) && !isMainSelection)); // only draw additional selections in curses mode
}

bool ViewStyle::DrawCaretInsideSelection(bool inOverstrike, bool imeCaretBlockOverride) const noexcept {
	if (FlagSet(caret.style, CaretStyle::BlockAfter)) {
		return false;
	}
	return ((caret.style & CaretStyle::InsMask) == CaretStyle::Block) ||
		(inOverstrike && FlagSet(caret.style, CaretStyle::OverstrikeBlock)) ||
		imeCaretBlockOverride ||
		FlagSet(caret.style, CaretStyle::Curses);
}

ViewStyle::CaretShape ViewStyle::CaretShapeForMode(bool inOverstrike, bool isMainSelection, bool drawDrag, bool drawOverstrikeCaret, bool imeCaretBlockOverride) const noexcept {
	if (drawDrag) {
		// Dragging text, use a line caret
		return CaretShape::line;
	}
	if (inOverstrike) {
		if (FlagSet(caret.style, CaretStyle::OverstrikeBlock)) {
			return CaretShape::block;
		}
		if (drawOverstrikeCaret) {
			return CaretShape::bar;
		}
	}
	if (imeCaretBlockOverride || (FlagSet(caret.style, CaretStyle::Curses) && !isMainSelection)) {
		return CaretShape::block;
	}
	const CaretStyle style = caret.style & CaretStyle::InsMask;
	return (style <= CaretStyle::Block) ? static_cast<CaretShape>(style) : CaretShape::line;
}

bool ViewStyle::ZoomIn() noexcept {
	if (zoomLevel < MaxZoomLevel) {
		int level = zoomLevel;
		if (level < 200) {
			level += 10;
		} else {
			level += 25;
		}

		level = std::min(level, MaxZoomLevel);
		if (level != zoomLevel) {
			zoomLevel = level;
			fontsValid = false;
			return true;
		}
	}
	return false;
}

bool ViewStyle::ZoomOut() noexcept {
	if (zoomLevel > MinZoomLevel) {
		int level = zoomLevel;
		if (level <= 200) {
			level -= 10;
		} else {
			level -= 25;
		}

		level = std::max(level, MinZoomLevel);
		if (level != zoomLevel) {
			zoomLevel = level;
			fontsValid = false;
			return true;
		}
	}
	return false;
}

void ViewStyle::CreateAndAddFont(const FontSpecification &fs) {
	if (fs.fontName) {
		const auto it = fonts.find(fs);
		if (it == fonts.end()) {
			fonts.emplace(fs, std::make_unique<FontRealised>());
		}
	}
}

FontRealised *ViewStyle::Find(const FontSpecification &fs) const {
	if (!fs.fontName)	// Invalid specification so return arbitrary object
		return fonts.begin()->second.get();
	const auto it = fonts.find(fs);
	if (it != fonts.end()) {
		// Should always reach here since map was just set for all styles
		return it->second.get();
	}
	return nullptr;
}

void ViewStyle::FindMaxAscentDescent() noexcept {
	XYPOSITION ascent = 1;
	XYPOSITION descent = 1;
	int index = 0;
	for (const Style &style : styles) {
		if (index != StyleCallTip) {
			if (ascent < style.ascent) {
				ascent = style.ascent;
			}
			if (descent < style.descent) {
				descent = style.descent;
			}
		}
		index++;
	}
	maxFontAscent = ascent;
	maxFontDescent = descent;
}
