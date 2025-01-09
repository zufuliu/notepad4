// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#pragma once

namespace Lexilla {

// based on CommonMark Spec 6.6 Raw HTML
constexpr bool IsHtmlTagStart(int ch) noexcept {
	return IsAlpha(ch);
}

constexpr bool IsHtmlTagChar(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '-' || ch == ':';
}

constexpr bool IsHtmlAttrStart(int ch) noexcept {
	return IsIdentifierStart(ch) || ch == ':';
}

constexpr bool IsHtmlAttrChar(int ch) noexcept {
	return IsIdentifierChar(ch) || ch == ':' || ch == '.' || ch == '-';
}

constexpr bool IsHtmlInvalidAttrChar(int ch) noexcept {
	// characters not allowed in unquoted attribute value
	return ch <= 32 || AnyOf(ch, '"', '\'', '\\', '`', '=', '<', '>');
}

inline bool IsHtmlCommentDelimiter(const StyleContext &sc) noexcept {
	if (sc.Match('<', '!') || sc.Match('-', '-')) {
		const int chNext = sc.GetRelative(2);
		if (sc.ch != '<') {
			return chNext == '>';
		}
		if (chNext == '-' ){
			return sc.GetRelative(3) == '-';
		}
	}
	return false;
}

#define SCE_H_VOID_TAG		SCE_H_TAGEND
#define js_style(style)		((style) + SCE_PHP_LABEL + 1)
#define css_style(style)	((style) + SCE_PHP_LABEL + SCE_JS_LABEL + 2)

constexpr bool IsJsSpaceEquiv(int state) noexcept {
	return (state >= js_style(SCE_JS_DEFAULT) && state <= js_style(SCE_JS_TASKMARKER));
}

constexpr bool IsJsIdentifierStart(int ch) noexcept {
	return IsIdentifierStartEx(ch) || ch == '$'
		|| ch == '#'; // ECMAScript 2021 private field and method
}

constexpr bool IsJsIdentifierChar(int ch) noexcept {
	return IsIdentifierCharEx(ch) || ch == '$';
}


constexpr bool IsCssSpaceEquiv(int style) noexcept {
	return style >= css_style(SCE_CSS_DEFAULT) && style <= css_style(SCE_CSS_CDO_CDC);
}

constexpr bool IsCssProperty(int style) noexcept {
	return style == css_style(SCE_CSS_PROPERTY) || style == css_style(SCE_CSS_UNKNOWN_PROPERTY);
}

constexpr bool IsCssUnicodeRangeChar(int ch) noexcept {
	return IsHexDigit(ch) || ch == '?';
}

constexpr bool IsCssIdentifierNext(int chNext) noexcept {
	return chNext == '-' || IsIdentifierStartEx(chNext);
}

constexpr bool IsCssIdentifierChar(int ch) noexcept {
	return IsIdentifierCharEx(ch) || ch == '-';
}

constexpr bool IsCssIdentifierStart(int ch, int chNext) noexcept {
	return IsIdentifierStartEx(ch)
		|| ((ch == '-' || ch == '@') && IsCssIdentifierNext(chNext));
}

}
