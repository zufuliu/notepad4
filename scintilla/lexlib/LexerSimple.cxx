// Scintilla source code edit control
/** @file LexerSimple.cxx
 ** A simple lexer with no state.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>

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

LexerSimple::LexerSimple(const LexerModule *module_) : module(module_) {
}

void SCI_METHOD LexerSimple::Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) {
	Accessor astyler(pAccess, &props);
	module->fnLexer(startPos, lengthDoc, initStyle, keywordLists, astyler);
	astyler.Flush();
}

void SCI_METHOD LexerSimple::Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) {
	if (module->fnFolder && props.GetInt("fold")) {
		Accessor astyler(pAccess, &props);
		Sci_Line lineCurrent = astyler.GetLine(startPos);
		// Move back one line in case deletion wrecked current line fold state
		if (lineCurrent > 0) {
			lineCurrent--;
			const Sci_Position newStartPos = astyler.LineStart(lineCurrent);
			lengthDoc += startPos - newStartPos;
			startPos = newStartPos;
			initStyle = 0;
			if (startPos > 0) {
				initStyle = astyler.StyleAt(startPos - 1);
			}
		}
		module->fnFolder(startPos, lengthDoc, initStyle, keywordLists, astyler);
		astyler.Flush();
	}
}

const char * SCI_METHOD LexerSimple::GetName() const noexcept {
	return module->languageName;
}

int SCI_METHOD LexerSimple::GetIdentifier() const noexcept {
	return module->GetLanguage();
}
