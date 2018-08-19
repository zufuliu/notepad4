// Scintilla source code edit control
/** @file CharacterSet.cxx
 ** Simple case functions for ASCII.
 ** Lexer infrastructure.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>
#include <cstring>

#include <windows.h>
#include <shlwapi.h>

#include "CharacterSet.h"

using namespace Scintilla;

namespace Scintilla {

CharacterSet::CharacterSet(setBase base, const char *initialSet, int size_, bool valueAfter_) {
	size = size_;
	valueAfter = valueAfter_;
	bset = new bool[size]();
	AddString(initialSet);
	if (base & setLower)
		AddString("abcdefghijklmnopqrstuvwxyz");
	if (base & setUpper)
		AddString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	if (base & setDigits)
		AddString("0123456789");
}

void CharacterSet::AddString(const char *setToAdd) {
	for (const char *cp = setToAdd; *cp; cp++) {
		int val = static_cast<unsigned char>(*cp);
		assert(val >= 0);
		assert(val < size);
		bset[val] = true;
	}
}

int CompareCaseInsensitive(const char *a, const char *b) noexcept {
#if 1
	return lstrcmpiA(a, b);
#else
	while (*a && *b) {
		if (*a != *b) {
			const char upperA = MakeUpperCase(*a);
			const char upperB = MakeUpperCase(*b);
			if (upperA != upperB)
				return upperA - upperB;
		}
		a++;
		b++;
	}
	// Either *a or *b is nul
	return *a - *b;
#endif
}

int CompareNCaseInsensitive(const char *a, const char *b, size_t len) noexcept {
#if 1
	return StrCmpNIA(a, b, static_cast<int>(len));
#else
	while (*a && *b && len) {
		if (*a != *b) {
			const char upperA = MakeUpperCase(*a);
			const char upperB = MakeUpperCase(*b);
			if (upperA != upperB)
				return upperA - upperB;
		}
		a++;
		b++;
		len--;
	}
	if (len == 0)
		return 0;
	else
		// Either *a or *b is nul
		return *a - *b;
#endif
}
}
