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
	static_assert(__is_integral(T) || __is_enum(T));
#endif
	return ((t == args) || ...);
}

// prevent pointer without <type_traits>
template <typename T, typename... Args>
constexpr void AnyOf([[maybe_unused]] T *t, [[maybe_unused]] Args... args) noexcept {}
template <typename T, typename... Args>
constexpr void AnyOf([[maybe_unused]] const T *t, [[maybe_unused]] Args... args) noexcept {}

template <typename T>
constexpr bool IsPowerOfTwo(T value) noexcept {
	return (value & (value - 1)) == 0;
}

template <char ch0, char ch1, typename T>
constexpr bool AnyOf(T ch) noexcept {
	// [chr(ch) for ch in range(256) if ((ch - ord('E')) & ~0x20) == 0] ['E', 'e']
	static_assert(IsPowerOfTwo(ch1 - ch0));
	if constexpr ((ch1 - ch0) <= 1) {
		return ch >= ch0 && ch <= ch1;
	} else {
		return ((ch - ch0) & ~(ch1 - ch0)) == 0;
	}
}

template <char ch0, char ch1, char ch2, char ch3, typename T>
constexpr bool AnyOf(T ch) noexcept {
	// [chr(ch) for ch in range(256) if ((ch - ord('E')) & ~0x21) == 0] ['E', 'F', 'e', 'f']
	static_assert(IsPowerOfTwo(ch1 - ch0) && IsPowerOfTwo(ch2 - ch0) && (ch1 - ch0) == (ch3 - ch2));
	return ((ch - ch0) & ~((ch1 - ch0) | (ch2 - ch0))) == 0;
}

template <typename T>
constexpr T UnsafeLower(T ch) noexcept {
	// [(ch, chr(ch | 0x20)) for ch in range(0, 32) if chr(ch | 0x20) != chr(ch).lower()]
	// [(chr(ch), chr(ch | 0x20)) for ch in range(32, 128) if chr(ch | 0x20) != chr(ch).lower()]
	// [('@', '`'), ('[', '{'), ('\\', '|'), (']', '}'), ('^', '~'), ('_', '\x7f')]
	return ch | 0x20;
}

template <typename T>
constexpr T UnsafeUpper(T ch) noexcept {
	// [(chr(ch), ch & ~0x20) for ch in range(0, 64) if chr(ch & ~0x20) != chr(ch).upper()]
	// [(chr(ch), chr(ch & ~0x20)) for ch in range(64, 128) if chr(ch & ~0x20) != chr(ch).upper()]
	// [('`', '@'), ('{', '['), ('|', '\\'), ('}', ']'), ('~', '^'), ('\x7f', '_')]
	return ch & ~0x20;
}

constexpr bool Between(int value, int lower, int high) noexcept {
	return value >= lower && value <= high;
}

// Functions for classifying characters

constexpr bool IsEOLChar(int ch) noexcept {
	return ch == '\r' || ch == '\n';
}

/**
 * Check if a character is a space.
 * This is ASCII specific but is safe with chars >= 0x80.
 */
constexpr bool IsASpace(int ch) noexcept {
	return ch == ' ' || (ch >= 0x09 && ch <= 0x0d);
}

constexpr bool IsASpaceOrTab(int ch) noexcept {
	return ch == ' ' || ch == '\t';
}

constexpr bool IsSpaceOrTab(int ch) noexcept {
	return ch == ' ' || ch == '\t';
}

constexpr int GetTabIndentCount(int indentCount) noexcept {
	return (indentCount + 4) & ~3;
}

constexpr bool IsControlCharacter(unsigned char ch) noexcept {
	// iscntrl() returns true for lots of characters > 127 which are displayable,
	// currently only check C0 control characters.
	return ch < 32 || ch == 127;
}

constexpr bool IsControl(int ch) noexcept {
	return ((ch >= 0) && (ch <= 0x1F)) || (ch == 0x7F);
}

constexpr bool IsEOLCharacter(int ch) noexcept {
	return ch == '\r' || ch == '\n';
}

constexpr bool IsBreakSpace(unsigned char ch) noexcept {
	// used for text breaking, treat C0 control character as space.
	// by default C0 control character is handled as special representation,
	// so not appears in normal text. 0x7F DEL is omitted to simplify the code.
	return ch <= ' ';
}

constexpr bool IsADigit(int ch) noexcept {
	return ch >= '0' && ch <= '9';
}

constexpr bool IsXDigit(int ch) noexcept {
	return (ch >= '0' && ch <= '9') || Between(UnsafeLower(ch), 'a', 'f');
}

constexpr bool IsHexDigit(int ch) noexcept {
	return (ch >= '0' && ch <= '9')
		|| (ch >= 'A' && ch <= 'F')
		|| (ch >= 'a' && ch <= 'f');
}

constexpr bool IsLowerHex(int ch) noexcept {
	return (ch >= '0' && ch <= '9')
		|| (ch >= 'a' && ch <= 'f');
}

constexpr bool IsUpperHex(int ch) noexcept {
	return (ch >= '0' && ch <= '9')
		|| (ch >= 'A' && ch <= 'F');
}

constexpr bool IsOctalDigit(int ch) noexcept {
	return ch >= '0' && ch <= '7';
}

constexpr bool IsOctalOrHex(int ch, bool hex) noexcept {
	const unsigned diff = ch - '0';
	return diff < 8 || (hex && (diff < 10 || Between(UnsafeLower(ch), 'a', 'f')));
}

constexpr bool IsDecimalOrHex(int ch, bool hex) noexcept {
	return IsADigit(ch) || (hex && Between(UnsafeLower(ch), 'a', 'f'));
}

constexpr bool IsADigitEx(int ch, int base) noexcept {
	if (base <= 10) {
		return (ch >= '0' && ch < '0' + base);
	}
	return (ch >= '0' && ch <= '9')
		|| Between(UnsafeLower(ch), 'a', 'a' + base - 10);
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

constexpr bool IsFloatExponentEx(int base, int ch, int chNext) noexcept {
	return ((base == 10 && (ch == 'e' || ch == 'E'))
		|| (base == 16 && (ch == 'p' || ch == 'P')))
		&& (chNext == '+' || chNext == '-' || IsADigit(chNext));
}

constexpr bool IsFloatExponent(int chPrev, int ch, int chNext) noexcept {
	return (chPrev == 'e' || chPrev == 'E')
		&& (ch == '+' || ch == '-') && IsADigit(chNext);
}

constexpr bool IsISODateTime(int ch, int chNext) noexcept {
	return ((ch == '+' || ch == '-' || ch == ':' || ch == '.') && IsADigit(chNext))
		|| (ch == ' ' && (chNext == '+' || chNext == '-' || IsADigit(chNext)));
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
	return IsLowerCase(UnsafeLower(ch));
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

constexpr bool IsSchemeNameChar(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '+' || ch == '-' || ch == '.';
}

constexpr bool IsDomainNameChar(int ch) noexcept {
	return IsIdentifierChar(ch) || ch == '-';
}

constexpr bool IsInvalidUrlChar(int ch) noexcept {
	// TODO: https://url.spec.whatwg.org/ and https://www.rfc-editor.org/rfc/rfc3986
	return ch <= 32 || AnyOf(ch, '"', '<', '>', '\\', '^', '`', '{', '|', '}', 127);
}

constexpr bool IsJumpLabelPrevChar(int chPrev) noexcept {
	return chPrev == ';' || chPrev == '{' || chPrev == '}';
}
constexpr bool IsJumpLabelPrevASI(int chPrev) noexcept {
	// TODO: automatic semicolon insertion
	return chPrev == ';' || chPrev == '{' || chPrev == '}' || chPrev == ')' || chPrev == ']';
}

constexpr bool IsInterfaceName(char ch, char chNext) noexcept {
	return ch == 'I' && IsUpperCase(chNext);
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

inline void ToLowerCase(char *s) noexcept {
	while (*s) {
		if (*s >= 'A' && *s <= 'Z') {
			*s |= 'a' - 'A';
		}
		++s;
	}
}

}
