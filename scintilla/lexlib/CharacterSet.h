// Scintilla source code edit control
/** @file CharacterSet.h
 ** Encapsulates a set of characters. Used to test if a character is within a set.
 **/
// Copyright 2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Lexilla {

//[[deprecated]]
class CharacterSet final {
	// ASCII character only, not useful for UTF-8 or DBCS multi byte character
	bool bset[128]{};
	const bool valueAfter;
public:
	enum setBase {
		setNone = 0,
		setLower = 1,
		setUpper = 2,
		setDigits = 4,
		setAlpha = setLower | setUpper,
		setAlphaNum = setAlpha | setDigits
	};

	//[[deprecated]]
	CharacterSet(setBase base = setNone, bool valueAfter_ = false) noexcept : valueAfter(valueAfter_) {
		if (base & setLower) {
			for (int ch = 'a'; ch <= 'z'; ch++) {
				bset[ch] = true;
			}
		}
		if (base & setUpper) {
			for (int ch = 'A'; ch <= 'Z'; ch++) {
				bset[ch] = true;
			}
		}
		if (base & setDigits) {
			for (int ch = '0'; ch <= '9'; ch++) {
				bset[ch] = true;
			}
		}
	}

	//[[deprecated]]
	template <size_t N>
	CharacterSet(setBase base, const char (&initialSet)[N], bool valueAfter_ = false) noexcept : valueAfter(valueAfter_) {
		AddString(initialSet);
		if (base & setLower) {
			for (int ch = 'a'; ch <= 'z'; ch++) {
				bset[ch] = true;
			}
		}
		if (base & setUpper) {
			for (int ch = 'A'; ch <= 'Z'; ch++) {
				bset[ch] = true;
			}
		}
		if (base & setDigits) {
			for (int ch = '0'; ch <= '9'; ch++) {
				bset[ch] = true;
			}
		}
	}

	CharacterSet(const CharacterSet &other) = delete;
	CharacterSet(CharacterSet &&other) = delete;
#if 1
	CharacterSet &operator=(const CharacterSet &other) = delete;
#else
	CharacterSet &operator=(const CharacterSet &other) noexcept {
		for (size_t i = 0; i < sizeof(bset); i++) {
			bset[i] = other.bset[i];
		}
		valueAfter = other.valueAfter;
	}
#endif
	CharacterSet &operator=(CharacterSet &&other) = delete;

	void Add(unsigned char ch) noexcept {
		bset[ch] = true;
	}

	template <size_t N>
	void AddString(const char (&setToAdd)[N]) noexcept {
		static_assert(N != 0);
		for (size_t i = 0; i < N - 1; i++) {
			const unsigned char ch = setToAdd[i];
			bset[ch] = true;
		}
	}

	bool Contains(int ch) const noexcept {
		const unsigned int uch = ch;
		return (uch < sizeof(bset)) ? bset[uch] : valueAfter;
	}

	bool Contains(char ch) const noexcept {
		const unsigned char uch = ch;
		return (uch < sizeof(bset)) ? bset[uch] : valueAfter;
	}
};

template <typename T, typename... Args>
constexpr bool AnyOf(T t, Args... args) noexcept {
#if defined(__clang__)
	static_assert(__is_integral(T));
#endif
	return ((t == args) || ...);
}

// prevent pointer without <type_traits>
template <typename T, typename... Args>
constexpr void AnyOf([[maybe_unused]] T *t, [[maybe_unused]] Args... args) noexcept {}
template <typename T, typename... Args>
constexpr void AnyOf([[maybe_unused]] const T *t, [[maybe_unused]] Args... args) noexcept {}

constexpr bool Between(int value, int lower, int high) noexcept {
	return value >= lower && value <= high;
}

// Functions for classifying characters

constexpr bool IsEOLChar(int ch) noexcept {
	return ch == '\r' || ch == '\n';
}

constexpr bool IsASpace(int ch) noexcept {
	return ch == ' ' || (ch >= 0x09 && ch <= 0x0d);
}

constexpr bool IsASpaceOrTab(int ch) noexcept {
	return ch == ' ' || ch == '\t';
}

constexpr bool IsADigit(int ch) noexcept {
	return ch >= '0' && ch <= '9';
}

constexpr bool IsHexDigit(int ch) noexcept {
	return (ch >= '0' && ch <= '9')
		|| (ch >= 'A' && ch <= 'F')
		|| (ch >= 'a' && ch <= 'f');
}

constexpr bool IsOctalDigit(int ch) noexcept {
	return ch >= '0' && ch <= '7';
}

constexpr bool IsADigit(int ch, int base) noexcept {
	if (base <= 10) {
		return (ch >= '0' && ch < '0' + base);
	}
	return (ch >= '0' && ch <= '9')
		|| Between(ch | 0x20, 'a', 'a' + base - 10);
}

constexpr bool IsNumberStart(int ch, int chNext) noexcept {
	return IsADigit(ch) || (ch == '.' && IsADigit(chNext));
}

constexpr bool IsNumberStartEx(int chPrev, int ch, int chNext) noexcept {
	return IsADigit(ch) || (chPrev != '.' && ch == '.' && IsADigit(chNext));
}

constexpr bool IsNumberContinue(int chPrev, int ch, int chNext) noexcept {
	return ((ch == '+' || ch == '-') && (chPrev == 'e' || chPrev == 'E'))
		|| (ch == '.' && chNext != '.');
}

constexpr bool IsNumberContinueEx(int chPrev, int ch, int chNext) noexcept {
	return ((ch == '+' || ch == '-') && (chPrev == 'e' || chPrev == 'E' || chPrev == 'p' || chPrev == 'P'))
		|| (ch == '.' && chNext != '.');
}

constexpr bool IsFloatExponent(int ch, int chNext) noexcept {
	return (ch == 'e' || ch == 'E')
		&& (chNext == '+' || chNext == '-' || IsADigit(chNext));
}

constexpr bool IsFloatExponent(int base, int ch, int chNext) noexcept {
	return ((base == 10 && (ch == 'e' || ch == 'E'))
		|| (base == 16 && (ch == 'p' || ch == 'P')))
		&& (chNext == '+' || chNext == '-' || IsADigit(chNext));
}

//[[deprecated]]
//constexpr bool IsASCII(int ch) noexcept {
//	return ch >= 0 && ch < 0x80;
//}

constexpr bool IsLowerCase(int ch) noexcept {
	return ch >= 'a' && ch <= 'z';
}

constexpr bool IsUpperCase(int ch) noexcept {
	return ch >= 'A' && ch <= 'Z';
}

constexpr bool IsUpperOrLowerCase(int ch) noexcept {
	return (ch >= 'A' && ch <= 'Z')
		|| (ch >= 'a' && ch <= 'z');
}

constexpr bool IsAlpha(int ch) noexcept {
	return (ch >= 'a' && ch <= 'z')
		|| (ch >= 'A' && ch <= 'Z');
}

constexpr bool IsAlphaNumeric(int ch) noexcept {
	return (ch >= '0' && ch <= '9')
		|| (ch >= 'a' && ch <= 'z')
		|| (ch >= 'A' && ch <= 'Z');
}

/**
 * Check if a character is a space.
 * This is ASCII specific but is safe with chars >= 0x80.
 */
constexpr bool isspacechar(int ch) noexcept {
	return ch == ' ' || (ch >= 0x09 && ch <= 0x0d);
}

//[[deprecated]]
constexpr bool iswordchar(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '.' || ch == '_';
}

//[[deprecated]]
constexpr bool iswordstart(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '_';
}

constexpr bool IsIdentifierChar(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '_';
}

constexpr bool IsIdentifierStart(int ch) noexcept {
	return IsAlpha(ch) || ch == '_';
}

constexpr bool IsDecimalNumber(int chPrev, int ch, int chNext) noexcept {
	return IsIdentifierChar(ch) || IsNumberContinue(chPrev, ch, chNext);
}

constexpr bool IsDecimalNumberEx(int chPrev, int ch, int chNext) noexcept {
	return IsIdentifierChar(ch) || IsNumberContinueEx(chPrev, ch, chNext);
}

// simply treat every character/byte larger than 127 as Unicode identifier,
// i.e. we don't support Unicode operators. proper implementation will need
// enable multiByteAccess in StyleContext and use functions from CharacterCategory.

constexpr bool IsIdentifierCharEx(int ch) noexcept {
	return IsIdentifierChar(ch) || ch >= 0x80;
}

constexpr bool IsIdentifierStartEx(int ch) noexcept {
	return IsIdentifierStart(ch) || ch >= 0x80;
}

constexpr bool isoperator(int ch) noexcept {
	return ch == '%' || ch == '^' || ch == '&' || ch == '*'
		|| ch == '(' || ch == ')' || ch == '-' || ch == '+'
		|| ch == '=' || ch == '|' || ch == '{' || ch == '}'
		|| ch == '[' || ch == ']' || ch == ':' || ch == ';'
		|| ch == '<' || ch == '>' || ch == ',' || ch == '/'
		|| ch == '?' || ch == '!' || ch == '.' || ch == '~';
}

// isoperator() excludes following eight punctuation: '"', '#', '$', '\'', '@', '\\', '_', '`'
// in most lexers, isoperator(ch) is equivalent to following code:
// IsAGraphic(ch) && !AnyOf(ch, '#', '$', '@', '\\', '`');

constexpr bool IsAGraphic(int ch) noexcept {
	// excludes C0 control characters and whitespace
	return ch > 32 && ch < 127;
}

constexpr bool IsGraphic(int ch) noexcept {
	// excludes C0 control characters and whitespace
	return ch > 32 && ch != 127;
}

constexpr bool IsPunctuation(int ch) noexcept {
	//return IsAGraphic(ch) && !IsAlphaNumeric(ch);
	return (ch > 32 && ch < '0')
		|| (ch > '9' && ch < 'A')
		|| (ch > 'Z' && ch < 'a')
		|| (ch > 'z' && ch < 127);
}

constexpr bool IsCommentTagPrev(int chPrev) noexcept {
	return chPrev <= 32 || AnyOf(chPrev, '/', '*', '!');
}

constexpr bool IsInvalidUrlChar(int ch) noexcept {
	return ch <= 32 || AnyOf(ch, '"', '<', '>', '\\', '^', '`', '{', '|', '}', 127);
}

// characters can follow jump `label:`, based on Swift's document Labeled Statement at
// https://docs.swift.org/swift-book/ReferenceManual/Statements.html#grammar_labeled-statement
// good coding style should place left aligned label on it's own line.
constexpr bool IsJumpLabelNextChar(int chNext) noexcept {
	// own line, comment, for, foreach, while, do, if, switch, repeat
	// TODO: match each word exactly like HighlightTaskMarker().
	return AnyOf(chNext, '\0', '/', 'f', 'w', 'd', 'i', 's', 'r');
}

// Simple case functions for ASCII supersets.

template <typename T>
constexpr T MakeUpperCase(T ch) noexcept {
	return (ch >= 'a' && ch <= 'z') ? (ch - 'a' + 'A') : ch;
}

template <typename T>
constexpr T MakeLowerCase(T ch) noexcept {
	return (ch >= 'A' && ch <= 'Z') ? (ch - 'A' + 'a') : ch;
}

#ifndef _WIN32
#define CompareCaseInsensitive		strcasecmp
#define CompareNCaseInsensitive		strncasecmp
#else
#define CompareCaseInsensitive		_stricmp
#define CompareNCaseInsensitive		_strnicmp
#endif

}
