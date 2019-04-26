// Scintilla source code edit control
/** @file CharacterSet.h
 ** Encapsulates a set of characters. Used to test if a character is within a set.
 **/
// Copyright 2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef CHARACTERSET_H
#define CHARACTERSET_H

namespace Scintilla {

class CharacterSet {
	int size;
	bool valueAfter;
	bool *bset;
public:
	enum setBase {
		setNone = 0,
		setLower = 1,
		setUpper = 2,
		setDigits = 4,
		setAlpha = setLower | setUpper,
		setAlphaNum = setAlpha | setDigits
	};
	CharacterSet(setBase base = setNone, const char *initialSet = "", int size_ = 0x80, bool valueAfter_ = false);
	CharacterSet(const CharacterSet &other) = delete;
	CharacterSet(CharacterSet &&other) = delete;
	CharacterSet &operator=(const CharacterSet &other) = delete;
	CharacterSet &operator=(CharacterSet &&other) = delete;
	~CharacterSet() {
		delete[]bset;
		bset = nullptr;
		size = 0;
	}
	void Add(int val) noexcept {
		assert(val >= 0);
		assert(val < size);
		bset[val] = true;
	}
	void AddString(const char *setToAdd) noexcept;
	bool Contains(int val) const noexcept {
		assert(val >= 0);
		if (val < 0) return false;
		return (val < size) ? bset[val] : valueAfter;
	}
	bool Contains(char ch) const noexcept {
		// Overload char as char may be signed
		const unsigned char uch = ch;
		return Contains(uch);
	}
};

// Functions for classifying characters

constexpr bool IsASpace(int ch) noexcept {
    return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

constexpr bool IsASpaceOrTab(int ch) noexcept {
	return (ch == ' ') || (ch == '\t');
}

constexpr bool IsADigit(int ch) noexcept  {
	return (ch >= '0') && (ch <= '9');
}

constexpr bool IsHexDigit(int ch) noexcept {
	return (ch >= '0' && ch <= '9')
		|| (ch >= 'A' && ch <= 'F')
		|| (ch >= 'a' && ch <= 'f');
}

#if 0
constexpr bool IsADigit(int ch, int base) noexcept {
	if (base <= 10) {
		return (ch >= '0') && (ch < '0' + base);
	} else {
		return ((ch >= '0') && (ch <= '9')) ||
		       ((ch >= 'A') && (ch < 'A' + base - 10)) ||
		       ((ch >= 'a') && (ch < 'a' + base - 10));
	}
}
#endif

constexpr bool IsASCII(int ch) noexcept {
	return (ch >= 0) && (ch < 0x80);
}

constexpr bool IsLowerCase(int ch) noexcept {
	return (ch >= 'a') && (ch <= 'z');
}

constexpr bool IsUpperCase(int ch) noexcept {
	return (ch >= 'A') && (ch <= 'Z');
}

constexpr bool IsUpperOrLowerCase(int ch) noexcept {
	return IsUpperCase(ch) || IsLowerCase(ch);
}

constexpr bool IsAlpha(int ch) noexcept {
	return 	((ch >= 'a') && (ch <= 'z')) ||
			((ch >= 'A') && (ch <= 'Z'));
}

constexpr bool IsAlphaNumeric(int ch) noexcept {
	return
		((ch >= '0') && (ch <= '9')) ||
		((ch >= 'a') && (ch <= 'z')) ||
		((ch >= 'A') && (ch <= 'Z'));
}

/**
 * Check if a character is a space.
 * This is ASCII specific but is safe with chars >= 0x80.
 */
constexpr bool isspacechar(int ch) noexcept {
    return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

constexpr bool iswordchar(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '.' || ch == '_';
}

constexpr bool iswordstart(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '_';
}

constexpr bool isoperator(int ch) noexcept {
	if (IsAlphaNumeric(ch))
		return false;
	if (ch == '%' || ch == '^' || ch == '&' || ch == '*' ||
		ch == '(' || ch == ')' || ch == '-' || ch == '+' ||
		ch == '=' || ch == '|' || ch == '{' || ch == '}' ||
		ch == '[' || ch == ']' || ch == ':' || ch == ';' ||
		ch == '<' || ch == '>' || ch == ',' || ch == '/' ||
		ch == '?' || ch == '!' || ch == '.' || ch == '~')
		return true;
	return false;
}

// Simple case functions for ASCII.
template <typename T>
constexpr T MakeUpperCase(T ch) noexcept {
	if (ch < 'a' || ch > 'z')
		return ch;
	else
		return ch - 'a' + 'A';
}

template <typename T>
constexpr T MakeLowerCase(T ch) noexcept {
	if (ch < 'A' || ch > 'Z')
		return ch;
	else
		return ch - 'A' + 'a';
}

#if 0
int CompareCaseInsensitive(const char *a, const char *b) noexcept;
int CompareNCaseInsensitive(const char *a, const char *b, size_t len) noexcept;
#else
#define CompareCaseInsensitive		_stricmp
#define CompareNCaseInsensitive		_strnicmp
#endif

}

#endif
