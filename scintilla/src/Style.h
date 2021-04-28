// Scintilla source code edit control
/** @file Style.h
 ** Defines the font and colour style for a class of text.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla {

struct FontSpecification {
	const char *fontName = nullptr;
	int weight = SC_WEIGHT_NORMAL;
	bool italic = false;
	int size = 10 * SC_FONT_SIZE_MULTIPLIER;
	int characterSet = SC_CHARSET_DEFAULT;
	int extraFontFlag = 0;
	constexpr FontSpecification() noexcept = default;
	bool operator==(const FontSpecification &other) const noexcept;
	bool operator<(const FontSpecification &other) const noexcept;
};

struct FontMeasurements {
	unsigned int ascent;
	unsigned int descent;
	XYPOSITION capitalHeight;	// Top of capital letter to baseline: ascent - internal leading
	XYPOSITION aveCharWidth;
	XYPOSITION spaceWidth;
	int sizeZoomed;
	FontMeasurements() noexcept;
	void ClearMeasurements() noexcept;
};

// used to optimize style copy.
struct StylePod {
	ColourAlpha fore;
	ColourAlpha back;
	bool eolFilled;
	bool underline;
	bool strike;
	enum class CaseForce : uint8_t {
		mixed, upper, lower, camel
	};
	CaseForce caseForce;
	bool visible;
	bool changeable;
	bool hotspot;
};

/**
 */
class Style : public FontSpecification, public FontMeasurements, public StylePod {
public:
	std::shared_ptr<Font> font;

	Style() noexcept;
	Style(const Style &source) noexcept;
	Style(Style &&) noexcept = default;
	~Style() = default;
	Style &operator=(const Style &source) noexcept;
	Style &operator=(Style &&) = delete;
	void ResetDefault(const char *fontName_) noexcept;
	void ClearTo(const Style &source) noexcept;
	void Copy(std::shared_ptr<Font> font_, const FontMeasurements &fm_) noexcept;
	bool IsProtected() const noexcept {
		return !(changeable && visible);
	}
};

}
