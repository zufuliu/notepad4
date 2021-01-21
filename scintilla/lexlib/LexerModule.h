// Scintilla source code edit control
/** @file LexerModule.h
 ** Colourise for particular languages.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla {

class Accessor;
class WordList;
struct LexicalClass;

typedef const WordList * const LexerWordList[];

typedef void (*LexerFunction)(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle,
	LexerWordList keywordLists, Accessor &styler);
typedef ILexer5 *(*LexerFactoryFunction)();

/**
 * A LexerModule is responsible for lexing and folding a particular language.
 * The Catalogue class maintains a list of LexerModules which can be searched to find a
 * module appropriate to a particular language.
 * The ExternalLexerModule subclass holds lexers loaded from DLLs or shared libraries.
 */
class LexerModule final {
	const int language;
	LexerFunction const fnLexer;
	LexerFunction const fnFolder;
	LexerFactoryFunction const fnFactory;

public:
	const char *const languageName;
	constexpr LexerModule(
		int language_,
		LexerFunction fnLexer_,
		const char *languageName_ = nullptr,
		LexerFunction fnFolder_ = nullptr) noexcept:
		language(language_),
		fnLexer(fnLexer_),
		fnFolder(fnFolder_),
		fnFactory(nullptr),
		languageName(languageName_) {
	}

	constexpr LexerModule(
		int language_,
		LexerFactoryFunction fnFactory_,
		const char *languageName_ = nullptr) noexcept:
		language(language_),
		fnLexer(nullptr),
		fnFolder(nullptr),
		fnFactory(fnFactory_),
		languageName(languageName_) {
	}

	int GetLanguage() const noexcept;

	// -1 is returned if no WordList information is available
	int GetNumWordLists() const noexcept;
	const char *GetWordListDescription(int index) const noexcept;
	const LexicalClass *LexClasses() const noexcept;
	size_t NamedStyles() const noexcept;

	ILexer5 *Create() const;

	friend class LexerSimple;
	friend class CatalogueModules;
};

constexpr int SCE_SIMPLE_OPERATOR = 5;
constexpr int SimpleLineStateMaskLineComment = 1;

// code folding for braces, brackets, parentheses and consecutive line comments.
void FoldSimpleDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList /*keywordLists*/, Accessor &styler);

}
