// Scintilla source code edit control
/** @file CharacterSet.h
 ** Encapsulates a set of characters. Used to test if a character is within a set.
 **/
// Copyright 2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla {

//[[deprecated]]
class CharacterSet final {
	bool valueAfter;
	// ASCII character only, not useful for UTF-8 or DBCS multi byte character
	bool bset[128]{};
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
	CharacterSet(setBase base = setNone, const char *initialSet = "", bool valueAfter_ = false) noexcept;
	CharacterSet(const CharacterSet &other) = delete;
	CharacterSet(CharacterSet &&other) = delete;
	CharacterSet &operator=(const CharacterSet &other) = delete;
	CharacterSet &operator=(CharacterSet &&other) = delete;
	void Add(int val) noexcept {
		assert(val >= 0);
		assert(val < 128);
		bset[val] = true;
	}
	void AddString(const char *setToAdd) noexcept;
	bool Contains(int val) const noexcept {
		assert(val >= 0);
		if (val < 0) {
			return false;
		}
		return (val < 128) ? bset[val] : valueAfter;
	}
	bool Contains(char ch) const noexcept {
		// Overload char as char may be signed
		const unsigned char uch = ch;
		return Contains(uch);
	}
};

template <typename T, typename... Args>
constexpr bool AnyOf(T t, Args... args) noexcept {
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

constexpr bool IsASCII(int ch) noexcept {
	return ch >= 0 && ch < 0x80;
}

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
	return ch <= 32 || ch >= 127
		|| AnyOf(ch, '"', '<', '>', '\\', '^', '`', '{', '|', '}');
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

#if 0
int CompareCaseInsensitive(const char *a, const char *b) noexcept;
int CompareNCaseInsensitive(const char *a, const char *b, size_t len) noexcept;
#else
#define CompareCaseInsensitive		_stricmp
#define CompareNCaseInsensitive		_strnicmp
#endif

template <int N>
constexpr int CStrLen([[maybe_unused]] const char (&s)[N]) {
	return N - 1;
}

#if defined(_INC_STRING)
template <typename... Args>
inline bool EqualsAny(const char *s, Args... args) noexcept {
	return ((::strcmp(s, args) == 0) || ...);
}

inline bool StrEqual(const char *s, const char *q) noexcept {
	return ::strcmp(s, q) == 0;
}

template <size_t N>
inline bool StrStartsWith(const char *s, const char (&prefix)[N]) noexcept {
	return ::strncmp(s, prefix, N - 1) == 0;
}

template <size_t N>
inline bool StrEndsWith(const char *s, size_t len, const char (&suffix)[N]) noexcept {
	return ::strcmp(s + (len + 1 - N), suffix) == 0;
}

#if defined(__clang__) || defined(__GNUC__) || !defined(_MSC_BUILD) || (_MSC_VER >= 1920)
template <size_t N>
inline bool CStrEqual(const char *s, const char (&q)[N]) noexcept {
	return ::memcmp(s, q, N) == 0;
}

template <size_t N>
inline bool CStrStartsWith(const char *s, const char (&prefix)[N]) noexcept {
	return ::memcmp(s, prefix, N - 1) == 0;
}
#else
// Visual C++ 2017 failed to optimize out string literal in memcmp().
namespace Private {

constexpr unsigned int as_i4(const char *s) noexcept {
	return *(const unsigned int *)s;
}

constexpr unsigned short as_i2(const char *s) noexcept {
	return *(const unsigned short *)s;
}

template <size_t N>
constexpr bool ConstStringCmp(const char *s, const char *q) noexcept {
	if constexpr (N == 2) {
		return as_i2(s) == as_i2(q);
	}
	if constexpr (N == 3) {
		return as_i2(s) == as_i2(q) && s[2] == q[2];
	}
	if constexpr (N == 4) {
		return as_i4(s) == as_i4(q);
	}
	if constexpr (N == 5) {
		return as_i4(s) == as_i4(q) && s[4] == q[4];
	}
	if constexpr (N == 6) {
		return as_i4(s) == as_i4(q) && as_i2(s + 4) == as_i2(q + 4);
	}
	if constexpr (N == 7) {
		return as_i4(s) == as_i4(q) && as_i2(s + 4) == as_i2(q + 4) && s[6] == q[6];
	}
	if constexpr (N == 8) {
		return as_i4(s) == as_i4(q) && as_i4(s + 4) == as_i4(q + 4);
	}
	if constexpr (N == 9) {
		return as_i4(s) == as_i4(q) && as_i4(s + 4) == as_i4(q + 4) && s[8] == q[8];
	}
	if constexpr (N == 10) {
		return as_i4(s) == as_i4(q) && as_i4(s + 4) == as_i4(q + 4) && as_i2(s + 8) == as_i2(q + 8);
	}
	if constexpr (N == 11) {
		return as_i4(s) == as_i4(q) && as_i4(s + 4) == as_i4(q + 4) && as_i2(s + 8) == as_i2(q + 8) && s[10] == q[10];
	}
	if constexpr (N == 12) {
		return as_i4(s) == as_i4(q) && as_i4(s + 4) == as_i4(q + 4) && as_i4(s + 8) == as_i4(q + 8);
	}
	if constexpr (N == 13) {
		return as_i4(s) == as_i4(q) && as_i4(s + 4) == as_i4(q + 4) && as_i4(s + 8) == as_i4(q + 8) && s[12] == q[12];
	}
	if constexpr (N == 14) {
		return as_i4(s) == as_i4(q) && as_i4(s + 4) == as_i4(q + 4) && as_i4(s + 8) == as_i4(q + 8) && as_i2(s + 12) == as_i2(q + 12);
	}
	if constexpr (N == 15) {
		return as_i4(s) == as_i4(q) && as_i4(s + 4) == as_i4(q + 4) && as_i4(s + 8) == as_i4(q + 8) && as_i2(s + 12) == as_i2(q + 12) && s[14] == q[14];
	}
	if constexpr (N == 16) {
		return as_i4(s) == as_i4(q) && as_i4(s + 4) == as_i4(q + 4) && as_i4(s + 8) == as_i4(q + 8) && as_i4(s + 12) == as_i4(q + 12);
	}
}

}
template <size_t N>
constexpr bool CStrEqual(const char *s, const char (&q)[N]) noexcept {
	return Private::ConstStringCmp<N>(s, q);
}

template <size_t N>
constexpr bool CStrStartsWith(const char *s, const char (&prefix)[N]) noexcept {
	return Private::ConstStringCmp<N - 1>(s, prefix);
}
#endif

template <size_t N>
inline bool CStrEndsWith(const char *s, size_t len, const char (&suffix)[N]) noexcept {
	return CStrEqual(s + (len + 1 - N), suffix);
}

template <size_t N>
inline bool CStrEqualEx(const char *s, const char (&q)[N]) noexcept {
	return ::memcmp(s, q, N) == 0;
}

template <size_t N>
inline bool CStrStartsWithEx(const char *s, const char (&prefix)[N]) noexcept {
	return ::memcmp(s, prefix, N - 1) == 0;
}

template <size_t N>
inline bool CStrEndsWithEx(const char *s, size_t len, const char (&suffix)[N]) noexcept {
	return ::memcmp(s + (len + 1 - N), suffix, N) == 0;
}

template <size_t N1>
inline bool CStrEqualsAny(const char *s, const char (&s1)[N1]) noexcept {
	return CStrEqual(s, s1);
}

template <size_t N1, size_t N2>
inline bool CStrEqualsAny(const char *s, const char (&s1)[N1], const char (&s2)[N2]) noexcept {
	return CStrEqual(s, s1) || CStrEqual(s, s2);
}

template <size_t N1, size_t N2, size_t N3>
inline bool CStrEqualsAny(const char *s, const char (&s1)[N1], const char (&s2)[N2], const char (&s3)[N3]) noexcept {
	return CStrEqual(s, s1) || CStrEqual(s, s2) || CStrEqual(s, s3);
}

template <size_t N1, size_t N2, size_t N3, size_t N4>
inline bool CStrEqualsAny(const char *s, const char (&s1)[N1], const char (&s2)[N2], const char (&s3)[N3],
	const char (&s4)[N4]) noexcept {
	return CStrEqual(s, s1) || CStrEqual(s, s2) || CStrEqual(s, s3) || CStrEqual(s, s4);
}

template <size_t N1, size_t N2, size_t N3, size_t N4, size_t N5>
inline bool CStrEqualsAny(const char *s, const char (&s1)[N1], const char (&s2)[N2], const char (&s3)[N3],
	const char (&s4)[N4], const char (&s5)[N5]) noexcept {
	return CStrEqual(s, s1) || CStrEqual(s, s2) || CStrEqual(s, s3) || CStrEqual(s, s4) || CStrEqual(s, s5);
}

template <size_t N1, size_t N2, size_t N3, size_t N4, size_t N5, size_t N6>
inline bool CStrEqualsAny(const char *s, const char (&s1)[N1], const char (&s2)[N2], const char (&s3)[N3],
	const char (&s4)[N4], const char (&s5)[N5], const char (&s6)[N6]) noexcept {
	return CStrEqual(s, s1) || CStrEqual(s, s2) || CStrEqual(s, s3) || CStrEqual(s, s4)
		|| CStrEqual(s, s5) || CStrEqual(s, s6);
}

template <size_t N1, size_t N2, size_t N3, size_t N4, size_t N5, size_t N6, size_t N7>
inline bool CStrEqualsAny(const char *s, const char (&s1)[N1], const char (&s2)[N2], const char (&s3)[N3],
	const char (&s4)[N4], const char (&s5)[N5], const char (&s6)[N6], const char (&s7)[N7]) noexcept {
	return CStrEqual(s, s1) || CStrEqual(s, s2) || CStrEqual(s, s3) || CStrEqual(s, s4)
		|| CStrEqual(s, s5) || CStrEqual(s, s6) || CStrEqual(s, s7);
}

template <size_t N1, size_t N2, size_t N3, size_t N4, size_t N5, size_t N6, size_t N7, size_t N8>
inline bool CStrEqualsAny(const char *s, const char (&s1)[N1], const char (&s2)[N2], const char (&s3)[N3],
	const char (&s4)[N4], const char (&s5)[N5], const char (&s6)[N6], const char (&s7)[N7], const char (&s8)[N8]) noexcept {
	return CStrEqual(s, s1) || CStrEqual(s, s2) || CStrEqual(s, s3) || CStrEqual(s, s4)
		|| CStrEqual(s, s5) || CStrEqual(s, s6) || CStrEqual(s, s7) || CStrEqual(s, s8);
}

template <size_t N1, size_t N2, size_t N3, size_t N4, size_t N5, size_t N6, size_t N7, size_t N8, size_t N9>
inline bool CStrEqualsAny(const char *s, const char (&s1)[N1], const char (&s2)[N2], const char (&s3)[N3],
	const char (&s4)[N4], const char (&s5)[N5], const char (&s6)[N6], const char (&s7)[N7], const char (&s8)[N8],
	const char (&s9)[N9]) noexcept {
	return CStrEqual(s, s1) || CStrEqual(s, s2) || CStrEqual(s, s3) || CStrEqual(s, s4)
		|| CStrEqual(s, s5) || CStrEqual(s, s6) || CStrEqual(s, s7) || CStrEqual(s, s8)
		|| CStrEqual(s, s9);
}

#endif // _INC_STRING

}
