// Scintilla source code edit control
/** @file LexerModule.cxx
 ** Colourise for particular languages.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>

#include <string>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "PropSetSimple.h"
#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "LexerModule.h"
#include "LexerBase.h"
#include "LexerSimple.h"

using namespace Scintilla;

int LexerModule::GetLanguage() const noexcept {
	return language;
}

int LexerModule::GetNumWordLists() const noexcept {
	return -1;
}

const char *LexerModule::GetWordListDescription([[maybe_unused]] int index) const noexcept {
	return "";
}

const LexicalClass *LexerModule::LexClasses() const noexcept {
	return nullptr;
}

size_t LexerModule::NamedStyles() const noexcept {
	return 0;
}

ILexer5 *LexerModule::Create() const {
	if (fnFactory) {
		return fnFactory();
	}
	return new LexerSimple(this);
}
