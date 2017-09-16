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
	CharacterSet(setBase base=setNone, const char *initialSet="", int size_=0x80, bool valueAfter_=false);
	CharacterSet(const CharacterSet &other);
	CharacterSet &operator=(CharacterSet &&other);
	~CharacterSet() {
		delete []bset;
		bset = 0;
		size = 0;
	}
	CharacterSet& operator=(const CharacterSet &other);
	void Add(int val) {
		assert(val >= 0);
		assert(val < size);
		bset[val] = true;
	}
	void AddString(const char *setToAdd);
	bool Contains(int val) const {
		assert(val >= 0);
		if (val < 0) return false;
		return (val < size) ? bset[val] : valueAfter;
	}
};

// Functions for classifying characters

static inline bool IsASpace(int ch) {
    return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

static inline bool IsASpaceOrTab(int ch) {
	return (ch == ' ') || (ch == '\t');
}

static inline bool IsADigit(int ch) {
	return (ch >= '0') && (ch <= '9');
}

static inline bool IsHexDigit(int ch) {
	return (ch >= '0' && ch <= '9')
		|| (ch >= 'A' && ch <= 'F')
		|| (ch >= 'a' && ch <= 'f');
}

static inline bool IsADigit(int ch, int base) {
	if (base <= 10) {
		return (ch >= '0') && (ch < '0' + base);
	} else {
		return ((ch >= '0') && (ch <= '9')) ||
		       ((ch >= 'A') && (ch < 'A' + base - 10)) ||
		       ((ch >= 'a') && (ch < 'a' + base - 10));
	}
}

static inline bool IsASCII(int ch) {
	return (ch >= 0) && (ch < 0x80);
}

static inline bool IsLowerCase(int ch) {
	return (ch >= 'a') && (ch <= 'z');
}

static inline bool IsUpperCase(int ch) {
	return (ch >= 'A') && (ch <= 'Z');
}

static inline bool IsAlpha(int ch) {
	return 	((ch >= 'a') && (ch <= 'z')) ||
			((ch >= 'A') && (ch <= 'Z'));
}

static inline bool IsAlphaNumeric(int ch) {
	return
		((ch >= '0') && (ch <= '9')) ||
		((ch >= 'a') && (ch <= 'z')) ||
		((ch >= 'A') && (ch <= 'Z'));
}

/**
 * Check if a character is a space.
 * This is ASCII specific but is safe with chars >= 0x80.
 */
static inline bool isspacechar(int ch) {
    return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

static inline bool iswordchar(int ch) {
	return IsAlphaNumeric(ch) || ch == '.' || ch == '_';
}

static inline bool iswordstart(int ch) {
	return IsAlphaNumeric(ch) || ch == '_';
}

static inline bool isoperator(int ch) {
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

static inline int MakeUpperCase(int ch) {
	if (ch < 'a' || ch > 'z')
		return ch;
	else
		return (ch - 'a' + 'A');
}

static inline int MakeLowerCase(int ch) {
	if (ch < 'A' || ch > 'Z')
		return ch;
	else
		return ch - 'A' + 'a';
}

int CompareCaseInsensitive(const char *a, const char *b);
int CompareNCaseInsensitive(const char *a, const char *b, size_t len);

}

#endif
