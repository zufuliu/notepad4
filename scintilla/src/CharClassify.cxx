// Scintilla source code edit control
/** @file CharClassify.cxx
 ** Character classifications used by Document and RESearch.
 **/
// Copyright 2006 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cctype>

#include <stdexcept>
#include <algorithm>

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
		else if (includeWordClass && (isalnum(ch) || ch == '_'))
			charClass[ch] = ccWord;
		else
			charClass[ch] = ccPunctuation;
	}

	const unsigned char w = static_cast<unsigned char>(includeWordClass ? ccWord : ccPunctuation);
	std::fill_n(charClass + 128, 128, w);
}

void CharClassify::SetCharClasses(const unsigned char *chars, cc newCharClass) noexcept {
	// Apply the newCharClass to the specifed chars
	if (chars) {
		while (*chars) {
			charClass[*chars] = static_cast<unsigned char>(newCharClass);
			chars++;
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
