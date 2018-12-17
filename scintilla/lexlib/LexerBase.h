// Scintilla source code edit control
/** @file LexerBase.h
 ** A simple lexer with no state.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef LEXERBASE_H
#define LEXERBASE_H

namespace Scintilla {

// A simple lexer with no state
class LexerBase : public ILexer4 {
protected:
	PropSetSimple props;
	enum {
		numWordLists = KEYWORDSET_MAX + 1
	};
	WordList *keyWordLists[numWordLists + 1];
public:
	LexerBase();
	virtual ~LexerBase();
	void SCI_METHOD Release() noexcept override;
	int SCI_METHOD Version() const noexcept override;
	Sci_Position SCI_METHOD PropertySet(const char *key, const char *val) override;
	Sci_Position SCI_METHOD WordListSet(int n, const char *wl) override;
	void SCI_METHOD Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) override = 0;
	void SCI_METHOD Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) override = 0;
	int SCI_METHOD LineEndTypesSupported() const noexcept override;
};

}

#endif
