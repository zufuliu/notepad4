// Scintilla source code edit control
/** @file LexerModule.h
 ** Colourise for particular languages.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef LEXERMODULE_H
#define LEXERMODULE_H

namespace Scintilla {

class Accessor;
class WordList;
struct LexicalClass;

typedef const WordList * const LexerWordList[];

typedef void (*LexerFunction)(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle,
	LexerWordList keywordLists, Accessor &styler);
typedef ILexer4 *(*LexerFactoryFunction)();

/**
 * A LexerModule is responsible for lexing and folding a particular language.
 * The Catalogue class maintains a list of LexerModules which can be searched to find a
 * module appropriate to a particular language.
 * The ExternalLexerModule subclass holds lexers loaded from DLLs or shared libraries.
 */
class LexerModule {
protected:
	int language;
	LexerFunction fnLexer;
	LexerFunction fnFolder;
	LexerFactoryFunction fnFactory;
	const char *const *wordListDescriptions;
	const LexicalClass *lexClasses;
	size_t nClasses;

public:
	const char *const languageName;
	LexerModule(
		int language_,
		LexerFunction fnLexer_,
		const char *languageName_ = nullptr,
		LexerFunction fnFolder_ = nullptr,
		const char *const wordListDescriptions_[] = nullptr,
		const LexicalClass *lexClasses_ = nullptr,
		size_t nClasses_ = 0) noexcept;
	LexerModule(
		int language_,
		LexerFactoryFunction fnFactory_,
		const char *languageName_ = nullptr,
		const char *const wordListDescriptions_[] = nullptr) noexcept;
	virtual ~LexerModule();
	int GetLanguage() const noexcept;

	// -1 is returned if no WordList information is available
	int GetNumWordLists() const noexcept;
	const char *GetWordListDescription(int index) const noexcept;
	const LexicalClass *LexClasses() const noexcept;
	size_t NamedStyles() const noexcept;

	ILexer4 *Create() const;

	virtual void Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle,
		LexerWordList keywordLists, Accessor &styler) const;
	virtual void Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle,
		LexerWordList keywordLists, Accessor &styler) const;

	friend class Catalogue;
};

}

#endif
