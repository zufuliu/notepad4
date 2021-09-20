// Scintilla source code edit control
/** @file LexerSimple.h
 ** A simple lexer with no state.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Lexilla {

// A simple lexer with no state
class LexerSimple final : public LexerBase {
	const LexerModule * const lexer;
public:
	explicit LexerSimple(const LexerModule *module_);
	void SCI_METHOD Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) override;
	void SCI_METHOD Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) override;
	// ILexer5 methods
	const char * SCI_METHOD GetName() const noexcept override;
	int SCI_METHOD GetIdentifier() const noexcept override;
};

}
