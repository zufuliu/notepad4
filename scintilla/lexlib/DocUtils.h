// This file is part of Notepad2.
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
	return ch <= 32 || AnyOf(ch, '"', '\'', '\\', '`', '=', '<', '>', 127);
}

#define SCE_H_VOID_TAG		SCE_H_TAGEND
#define js_style(style)		((style) + SCE_PHP_LABEL + 1)
#define css_style(style)	((style) + SCE_PHP_LABEL + SCE_JS_LABEL + 2)

constexpr bool IsJsIdentifierStart(int ch) noexcept {
	return IsIdentifierStartEx(ch) || ch == '$'
		|| ch == '#'; // ECMAScript 2021 private field and method
}

constexpr bool IsJsIdentifierChar(int ch) noexcept {
	return IsIdentifierCharEx(ch) || ch == '$';
}

constexpr bool IsCssOperator(int ch) noexcept {
	return ch == '{' || ch == '}' || ch == ':' || ch == ',' || ch == ';' ||
		ch == '.' || ch == '#' || ch == '!' || ch == '@' ||
		ch == '*' || ch == '>' || ch == '+' || ch == '=' || ch == '~' || ch == '|' ||
		ch == '[' || ch == ']' || ch == '(' || ch == ')';
}

}
