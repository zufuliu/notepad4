// Scintilla source code edit control
/** @file CharacterSet.cxx
 ** Simple case functions for ASCII.
 ** Lexer infrastructure.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>

#include "CharacterSet.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

CharacterSet::CharacterSet(setBase base, const char *initialSet, int size_, bool valueAfter_) {
	size = size_;
	valueAfter = valueAfter_;
	bset = new bool[size];
	for (int i=0; i < size; i++) {
		bset[i] = false;
	}
	AddString(initialSet);
	if (base & setLower)
		AddString("abcdefghijklmnopqrstuvwxyz");
	if (base & setUpper)
		AddString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	if (base & setDigits)
		AddString("0123456789");
}
CharacterSet::CharacterSet(const CharacterSet &other) {
	size = other.size;
	valueAfter = other.valueAfter;
	bset = new bool[size];
	for (int i=0; i < size; i++) {
		bset[i] = other.bset[i];
	}
}
CharacterSet& CharacterSet::operator=(CharacterSet &&other) {
	if (this != &other) {
		delete []bset;
		size = other.size;
		valueAfter = other.valueAfter;
		bset = other.bset;
		other.size = 0;
		other.bset = nullptr;
	}
	return *this;
}
void CharacterSet::AddString(const char *setToAdd) {
	for (const char *cp=setToAdd; *cp; cp++) {
		int val = static_cast<unsigned char>(*cp);
		assert(val >= 0);
		assert(val < size);
		bset[val] = true;
	}
}




int CompareCaseInsensitive(const char *a, const char *b) {
	while (*a && *b) {
		if (*a != *b) {
			const int upperA = MakeUpperCase(*a);
			const int upperB = MakeUpperCase(*b);
			if (upperA != upperB)
				return upperA - upperB;
		}
		a++;
		b++;
	}
	// Either *a or *b is nul
	return *a - *b;
}

int CompareNCaseInsensitive(const char *a, const char *b, size_t len) {
	while (*a && *b && len) {
		if (*a != *b) {
			const int upperA = MakeUpperCase(*a);
			const int upperB = MakeUpperCase(*b);
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

#ifdef SCI_NAMESPACE
}
#endif
