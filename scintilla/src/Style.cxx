// Scintilla source code edit control
/** @file Style.cxx
 ** Defines the font and colour style for a class of text.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdexcept>
#include <string_view>
#include <vector>
#include <memory>

#include "Platform.h"

#include "Scintilla.h"
#include "Style.h"

using namespace Scintilla;

FontAlias::FontAlias() noexcept = default;

FontAlias::FontAlias(const FontAlias &other) noexcept {
	SetID(other.fid);
}

FontAlias::FontAlias(FontAlias &&other) noexcept {
	SetID(other.fid);
	other.ClearFont();
}

FontAlias::~FontAlias() {
	SetID(FontID{});
	// ~Font will not release the actual font resource since it is now 0
}

void FontAlias::MakeAlias(const Font &fontOrigin) noexcept {
	SetID(fontOrigin.GetID());
}

void FontAlias::ClearFont() noexcept {
	SetID(FontID{});
}

bool FontSpecification::operator==(const FontSpecification &other) const noexcept {
	return fontName == other.fontName &&
		weight == other.weight &&
		italic == other.italic &&
		size == other.size &&
		characterSet == other.characterSet &&
		extraFontFlag == other.extraFontFlag;
}

bool FontSpecification::operator<(const FontSpecification &other) const noexcept {
	if (fontName != other.fontName)
		return fontName < other.fontName;
	if (weight != other.weight)
		return weight < other.weight;
	if (italic != other.italic)
		return !italic;
	if (size != other.size)
		return size < other.size;
	if (characterSet != other.characterSet)
		return characterSet < other.characterSet;
	if (extraFontFlag != other.extraFontFlag)
		return extraFontFlag < other.extraFontFlag;
	return false;
}

FontMeasurements::FontMeasurements() noexcept {
	ClearMeasurements();
}

void FontMeasurements::ClearMeasurements() noexcept {
	ascent = 1;
	descent = 1;
	capitalHeight = 1;
	aveCharWidth = 1;
	spaceWidth = 1;
	sizeZoomed = 2;
}

Style::Style() noexcept {
	fore = ColourDesired(0, 0, 0);
	back = ColourDesired(0xff, 0xff, 0xff);
	eolFilled = false;
	underline = false;
	strike = false;
	caseForce = caseMixed;
	visible = true;
	changeable = true;
	hotspot = false;
}

Style::Style(const Style &source) noexcept : FontSpecification(source), FontMeasurements(source) {
	fore = source.fore;
	back = source.back;
	eolFilled = source.eolFilled;
	underline = source.underline;
	strike = source.strike;
	caseForce = source.caseForce;
	visible = source.visible;
	changeable = source.changeable;
	hotspot = source.hotspot;
}

Style::~Style() = default;

Style &Style::operator=(const Style &source) noexcept {
	if (this == &source) {
		return *this;
	}

	fontName = source.fontName;
	weight = source.weight;
	italic = source.italic;
	size = source.size;
	characterSet = source.characterSet;
	fore = source.fore;
	back = source.back;
	eolFilled = source.eolFilled;
	underline = source.underline;
	strike = source.strike;
	caseForce = source.caseForce;
	visible = source.visible;
	changeable = source.changeable;
	hotspot = source.hotspot;
	font.ClearFont();
	FontMeasurements::ClearMeasurements();
	return *this;
}

void Style::ResetDefault(const char *fontName_) noexcept {
	fontName = fontName_;
	weight = SC_WEIGHT_NORMAL;
	italic = false;
	size = Platform::DefaultFontSize() * SC_FONT_SIZE_MULTIPLIER;
	characterSet = SC_CHARSET_DEFAULT;
	fore = ColourDesired(0, 0, 0);
	back = ColourDesired(0xff, 0xff, 0xff);
	eolFilled = false;
	underline = false;
	strike = false;
	caseForce = caseMixed;
	visible = true;
	changeable = true;
	hotspot = false;
	font.ClearFont();
	FontMeasurements::ClearMeasurements();
}

void Style::ClearTo(const Style &source) noexcept {
	*this = source;
}

void Style::Copy(const Font &font_, const FontMeasurements &fm_) noexcept {
	font.MakeAlias(font_);
	(FontMeasurements &)(*this) = fm_;
}
