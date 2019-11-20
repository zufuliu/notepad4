// Scintilla source code edit control
/** @file DefaultLexer.h
 ** A lexer base class with default empty implementations of methods.
 ** For lexers that do not support all features so do not need real implementations.
 ** Does have real implementation for style metadata.
 **/
// Copyright 2017 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla {

// A simple lexer with no state
class DefaultLexer : public ILexer4 {
	const LexicalClass *lexClasses;
	size_t nClasses;
public:
	DefaultLexer(const LexicalClass *lexClasses_ = nullptr, size_t nClasses_ = 0);
	virtual ~DefaultLexer();
	void SCI_METHOD Release() noexcept override;
	int SCI_METHOD Version() const noexcept override;
	const char * SCI_METHOD PropertyNames() const noexcept override;
	int SCI_METHOD PropertyType(const char *name) const override;
	const char * SCI_METHOD DescribeProperty(const char *name) const override;
	Sci_Position SCI_METHOD PropertySet(const char *key, const char *val) override;
	const char * SCI_METHOD DescribeWordListSets() const noexcept override;
	Sci_Position SCI_METHOD WordListSet(int n, const char *wl) override;
	void SCI_METHOD Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) override = 0;
	void SCI_METHOD Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) override;
	void *SCI_METHOD PrivateCall(int operation, void *pointer) override;
	int SCI_METHOD LineEndTypesSupported() const noexcept override;
	int SCI_METHOD AllocateSubStyles(int styleBase, int numberStyles) override;
	int SCI_METHOD SubStylesStart(int styleBase) const noexcept override;
	int SCI_METHOD SubStylesLength(int styleBase) const noexcept override;
	int SCI_METHOD StyleFromSubStyle(int subStyle) const noexcept override;
	int SCI_METHOD PrimaryStyleFromStyle(int style) const noexcept override;
	void SCI_METHOD FreeSubStyles() noexcept override;
	void SCI_METHOD SetIdentifiers(int style, const char *identifiers) override;
	int SCI_METHOD DistanceToSecondaryStyles() const noexcept override;
	const char * SCI_METHOD GetSubStyleBases() const noexcept override;
	int SCI_METHOD NamedStyles() const noexcept override;
	const char * SCI_METHOD NameOfStyle(int style) const noexcept override;
	const char * SCI_METHOD TagsOfStyle(int style) const noexcept override;
	const char * SCI_METHOD DescriptionOfStyle(int style) const noexcept override;
};

}
