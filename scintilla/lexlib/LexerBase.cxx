// Scintilla source code edit control
/** @file LexerBase.cxx
 ** A simple lexer with no state.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>
#include <cstring>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "PropSetSimple.h"
#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "LexerModule.h"
#include "LexerBase.h"

using namespace Scintilla;

LexerBase::LexerBase() {
	for (int wl = 0; wl < numWordLists; wl++) {
		keyWordLists[wl] = new WordList;
	}
	keyWordLists[numWordLists] = nullptr;
}

LexerBase::~LexerBase() {
	for (int wl = 0; wl < numWordLists; wl++) {
		delete keyWordLists[wl];
		keyWordLists[wl] = nullptr;
	}
	keyWordLists[numWordLists] = nullptr;
}

void SCI_METHOD LexerBase::Release() noexcept {
	delete this;
}

int SCI_METHOD LexerBase::Version() const noexcept {
	return lvRelease4;
}

Sci_Position SCI_METHOD LexerBase::PropertySet(const char *key, const char *val) {
	const char *valOld = props.Get(key);
	if (strcmp(val, valOld) != 0) {
		props.Set(key, val, strlen(key), strlen(val));
		return 0;
	} else {
		return -1;
	}
}

Sci_Position SCI_METHOD LexerBase::WordListSet(int n, const char *wl) {
	if (n < numWordLists) {
		WordList wlNew;
		wlNew.Set(wl);
		if (*keyWordLists[n] != wlNew) {
			keyWordLists[n]->Set(wl);
			return 0;
		}
	}
	return -1;
}

int SCI_METHOD LexerBase::LineEndTypesSupported() const noexcept {
	return SC_LINE_END_TYPE_DEFAULT;
}
