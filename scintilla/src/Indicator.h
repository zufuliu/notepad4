// Scintilla source code edit control
/** @file Indicator.h
 ** Defines the style of indicators which are text decorations such as underlining.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

struct StyleAndColour {
	Scintilla::IndicatorStyle style = Scintilla::IndicatorStyle::Plain;
	ColourRGBA fore = black;
	constexpr StyleAndColour() noexcept = default;
	constexpr StyleAndColour(Scintilla::IndicatorStyle style_, ColourRGBA fore_ = black) noexcept : style(style_), fore(fore_) {}
	bool operator==(const StyleAndColour &other) const noexcept {
		return __builtin_memcmp(this, &other, sizeof(other)) == 0;
	}
};

/**
 */
class Indicator {
public:
	enum class State {
		normal, hover
	};
	StyleAndColour sacNormal;
	StyleAndColour sacHover;
	bool dynamic = false;
	bool overrideTextFore = false;
	bool under = false;
	bool hoverUnderline = false;
	int fillAlpha = 30;
	int outlineAlpha = 50;
	Scintilla::IndicFlag attributes = Scintilla::IndicFlag::None;
	XYPOSITION strokeWidth = 1.0f;
	constexpr Indicator() noexcept = default;
	constexpr Indicator(Scintilla::IndicatorStyle style_, ColourRGBA fore_ = black, bool under_ = false, int fillAlpha_ = 30, int outlineAlpha_ = 50) noexcept :
		sacNormal(style_, fore_), sacHover(style_, fore_), under(under_), fillAlpha(fillAlpha_), outlineAlpha(outlineAlpha_) {}
	void SCICALL Draw(Surface *surface, PRectangle rc, PRectangle rcLine, PRectangle rcCharacter, State state, int value) const;

	void Refresh() noexcept {
		dynamic = hoverUnderline || !(sacNormal == sacHover);
		overrideTextFore = sacNormal.style == Scintilla::IndicatorStyle::TextFore || sacHover.style == Scintilla::IndicatorStyle::TextFore;
	}
	bool IsDynamic() const noexcept {
		return dynamic;
	}
	bool OverridesTextFore() const noexcept {
		return overrideTextFore;
	}
	Scintilla::IndicFlag Flags() const noexcept {
		return attributes;
	}
	void SetFlags(Scintilla::IndicFlag attributes_) noexcept;
};

}
