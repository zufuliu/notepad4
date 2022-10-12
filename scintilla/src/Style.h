// Scintilla source code edit control
/** @file Style.h
 ** Defines the font and colour style for a class of text.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

struct FontSpecification {
	// fontName is allocated by a ViewStyle container object and may be null
	const char *fontName;
	int size;
	Scintilla::FontWeight weight = Scintilla::FontWeight::Normal;
	bool italic = false;
	bool checkMonospaced = false;
	Scintilla::CharacterSet characterSet = Scintilla::CharacterSet::Default;
	Scintilla::FontQuality extraFontFlag = Scintilla::FontQuality::QualityDefault;
	constexpr FontSpecification(const char *fontName_ = nullptr, int size_ = 10*Scintilla::FontSizeMultiplier) noexcept :
		fontName(fontName_), size(size_) { }
	bool operator==(const FontSpecification &other) const noexcept;
	bool operator<(const FontSpecification &other) const noexcept;
};

struct FontMeasurements {
	XYPOSITION ascent = 1;
	XYPOSITION descent = 1;
	XYPOSITION capitalHeight = 1;	// Top of capital letter to baseline: ascent - internal leading
	XYPOSITION aveCharWidth = 1;
	//XYPOSITION monospaceCharacterWidth = 1; // for macOS
	XYPOSITION spaceWidth = 1;
	bool monospaceASCII = false;
	int sizeZoomed = 2;
};

constexpr size_t maxInvisibleStyleRepresentationLength = 6;

// used to optimize style copy.
struct StylePod {
	ColourRGBA fore = ColourRGBA(0, 0, 0);
	ColourRGBA back = ColourRGBA(0xff, 0xff, 0xff);
	bool eolFilled = false;
	bool underline = false;
	bool strike = false;
	bool overline = false;
	enum class CaseForce : uint8_t {
		mixed, upper, lower, camel
	};
	CaseForce caseForce = CaseForce::mixed;
	bool visible = true;
	bool changeable = true;
	bool hotspot = false;

	char invisibleRepresentation[maxInvisibleStyleRepresentationLength + 1]{};
	uint8_t invisibleRepresentationLength = 0;
};

/**
 */
class Style final : public FontSpecification, public FontMeasurements, public StylePod {
public:
	std::shared_ptr<Font> font;

	Style(const char *fontName_ = nullptr) noexcept;
	void ResetDefault(const char *fontName_ = nullptr) noexcept;
	void Copy(std::shared_ptr<Font> font_, const FontMeasurements &fm_) noexcept;
	bool IsProtected() const noexcept {
		return !(changeable && visible);
	}
	std::string_view GetInvisibleRepresentation() const noexcept {
		return {invisibleRepresentation, invisibleRepresentationLength};
	}
};

}
