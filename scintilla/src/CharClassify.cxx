// Scintilla source code edit control
/** @file CharClassify.cxx
 ** Character classifications used by Document and RESearch.
 **/
// Copyright 2006 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>
#include <cstring>

#include "CharacterSet.h"
#include "CharClassify.h"

using namespace Scintilla;

CharClassify::CharClassify() noexcept : charClass{} {
	SetDefaultCharClasses(true);
}

void CharClassify::SetDefaultCharClasses(bool includeWordClass) noexcept {
	// Initialize all char classes to default values
	for (int ch = 0; ch < 128; ch++) {
		if (ch == '\r' || ch == '\n')
			charClass[ch] = ccNewLine;
		else if (ch < 0x20 || ch == ' ')
			charClass[ch] = ccSpace;
		else if (includeWordClass && (IsAlphaNumeric(ch) || ch == '_'))
			charClass[ch] = ccWord;
		else
			charClass[ch] = ccPunctuation;
	}

	const int w = includeWordClass ? ccWord : ccPunctuation;
	memset(charClass + 128, w, 128);
}

void CharClassify::SetCharClasses(const unsigned char *chars, cc newCharClass) noexcept {
	// Apply the newCharClass to the specifed chars
	if (chars) {
		const unsigned char w = static_cast<unsigned char>(newCharClass);
		while (*chars) {
			charClass[*chars] = w;
			chars++;
		}
	}
}

void CharClassify::SetCharClassesEx(const unsigned char *chars, int length) noexcept {
	if (chars == nullptr || length <= 0) {
		memset(charClass + 128, ccWord, 128);
	} else if (length == 16) {
		for (int i = 0; i < 128; i++) {
			const unsigned char w = 1 + ((chars[i >> 3] >> (i & 7)) & 1);
			charClass[i + 128] = w;
		}
	} else if (length == 32) {
		for (int i = 0; i < 128; i++) {
			const unsigned char w = (chars[i >> 2] >> (2 * (i & 3))) & 3;
			charClass[i + 128] = w;
		}
	}
}

int CharClassify::GetCharsOfClass(cc characterClass, unsigned char *buffer) const noexcept {
	// Get characters belonging to the given char class; return the number
	// of characters (if the buffer is NULL, don't write to it).
	int count = 0;
	for (int ch = maxChar - 1; ch >= 0; --ch) {
		if (charClass[ch] == characterClass) {
			++count;
			if (buffer) {
				*buffer = static_cast<unsigned char>(ch);
				buffer++;
			}
		}
	}
	return count;
}
