// Scintilla source code edit control
/** @file CharacterSet.cxx
 ** Simple case functions for ASCII.
 ** Lexer infrastructure.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>

#include <algorithm>

#include "CharacterSet.h"

using namespace Scintilla;

namespace Scintilla {

CharacterSet::CharacterSet(setBase base, const char *initialSet, bool valueAfter_) noexcept:
	valueAfter(valueAfter_) {
	AddString(initialSet);
	if (base & setLower) {
		//AddString("abcdefghijklmnopqrstuvwxyz");
		std::fill_n(bset + 'a', 26, true);
	}
	if (base & setUpper) {
		//AddString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		std::fill_n(bset + 'A', 26, true);
	}
	if (base & setDigits) {
		//AddString("0123456789");
		std::fill_n(bset + '0', 10, true);
	}
}

void CharacterSet::AddString(const char *setToAdd) noexcept {
	for (const char *cp = setToAdd; *cp; cp++) {
		const unsigned char uch = *cp;
		assert(uch < 128);
		bset[uch] = true;
	}
}

#if 0
int CompareCaseInsensitive(const char *a, const char *b) noexcept {
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
}

int CompareNCaseInsensitive(const char *a, const char *b, size_t len) noexcept {
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
}
#endif
}
