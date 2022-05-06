// Scintilla source code edit control
/** @file LexerBase.h
 ** A simple lexer with no state.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Lexilla {

// A simple lexer with no state
class LexerBase final : public Scintilla::ILexer5 {
	const LexerModule lexer;
	PropSetSimple props;
	WordList *keywordLists[KEYWORDSET_MAX];
public:
	explicit LexerBase(const LexerModule *module_);
	virtual ~LexerBase();
	void SCI_METHOD Release() noexcept override;
	int SCI_METHOD Version() const noexcept override;
	const char * SCI_METHOD PropertyNames() const noexcept override;
	int SCI_METHOD PropertyType(const char *name) const noexcept override;
	const char * SCI_METHOD DescribeProperty(const char *name) const noexcept override;
	Sci_Position SCI_METHOD PropertySet(const char *key, const char *val) override;
	const char * SCI_METHOD DescribeWordListSets() const noexcept override;
	Sci_Position SCI_METHOD WordListSet(int n, int attribute, const char *wl) override;
	void SCI_METHOD Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) override;
	void SCI_METHOD Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, Scintilla::IDocument *pAccess) override;
	void *SCI_METHOD PrivateCall(int operation, void *pointer) noexcept override;
	int SCI_METHOD LineEndTypesSupported() const noexcept override;
	int SCI_METHOD AllocateSubStyles(int styleBase, int numberStyles) noexcept override;
	int SCI_METHOD SubStylesStart(int styleBase) const noexcept override;
	int SCI_METHOD SubStylesLength(int styleBase) const noexcept override;
	int SCI_METHOD StyleFromSubStyle(int subStyle) const noexcept override;
	int SCI_METHOD PrimaryStyleFromStyle(int style) const noexcept override;
	void SCI_METHOD FreeSubStyles() noexcept override;
	void SCI_METHOD SetIdentifiers(int style, const char *identifiers) noexcept override;
	int SCI_METHOD DistanceToSecondaryStyles() const noexcept override;
	const char * SCI_METHOD GetSubStyleBases() const noexcept override;
	int SCI_METHOD NamedStyles() const noexcept override;
	const char * SCI_METHOD NameOfStyle(int style) const noexcept override;
	const char * SCI_METHOD TagsOfStyle(int style) const noexcept override;
	const char * SCI_METHOD DescriptionOfStyle(int style) const noexcept override;
	// ILexer5 methods
	const char * SCI_METHOD GetName() const noexcept override;
	int SCI_METHOD GetIdentifier() const noexcept override;
	const char *SCI_METHOD PropertyGet(const char *key) const override;
};

}
