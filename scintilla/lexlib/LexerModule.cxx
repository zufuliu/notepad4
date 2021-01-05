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

void LexerModule::Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle,
	LexerWordList keywordLists, Accessor &styler) const {
	if (fnLexer) {
		fnLexer(startPos, lengthDoc, initStyle, keywordLists, styler);
	}
}

void LexerModule::Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle,
	LexerWordList keywordLists, Accessor &styler) const {
	if (fnFolder) {
		Sci_Line lineCurrent = styler.GetLine(startPos);
		// Move back one line in case deletion wrecked current line fold state
		if (lineCurrent > 0) {
			lineCurrent--;
			const Sci_Position newStartPos = styler.LineStart(lineCurrent);
			lengthDoc += startPos - newStartPos;
			startPos = newStartPos;
			initStyle = 0;
			if (startPos > 0) {
				initStyle = styler.StyleAt(startPos - 1);
			}
		}
		fnFolder(startPos, lengthDoc, initStyle, keywordLists, styler);
	}
}
