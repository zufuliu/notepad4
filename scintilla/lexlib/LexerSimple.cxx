// Scintilla source code edit control
/** @file LexerSimple.cxx
 ** A simple lexer with no state.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>

#include <string_view>

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

using namespace Lexilla;

LexerSimple::LexerSimple(const LexerModule *module_) : lexer(module_) {
}

void SCI_METHOD LexerSimple::Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) {
	Accessor styler(pAccess, &props);
	lexer->fnLexer(startPos, lengthDoc, initStyle, keywordLists, styler);
	styler.Flush();
}

void SCI_METHOD LexerSimple::Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) {
	if (lexer->fnFolder && props.GetInt("fold")) {
		Accessor styler(pAccess, &props);
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
		lexer->fnFolder(startPos, lengthDoc, initStyle, keywordLists, styler);
		styler.Flush();
	}
}

const char * SCI_METHOD LexerSimple::GetName() const noexcept {
	return lexer->languageName;
}

int SCI_METHOD LexerSimple::GetIdentifier() const noexcept {
	return lexer->GetLanguage();
}
