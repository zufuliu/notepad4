// Scintilla source code edit control
/** @file ILexer.h
 ** Interface between Scintilla and lexers.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

#include "Sci_Position.h"

namespace Scintilla {

enum {
	dvRelease4 = 2
};

class IDocument {
public:
	virtual int SCI_METHOD Version() const noexcept = 0;
	virtual void SCI_METHOD SetErrorStatus(int status) noexcept = 0;
	virtual Sci_Position SCI_METHOD Length() const noexcept = 0;
	virtual void SCI_METHOD GetCharRange(char *buffer, Sci_Position position, Sci_Position lengthRetrieve) const noexcept = 0;
	virtual unsigned char SCI_METHOD StyleAt(Sci_Position position) const noexcept = 0;
	virtual Sci_Position SCI_METHOD LineFromPosition(Sci_Position position) const noexcept = 0;
	virtual Sci_Position SCI_METHOD LineStart(Sci_Position line) const noexcept = 0;
	virtual int SCI_METHOD GetLevel(Sci_Position line) const noexcept = 0;
	virtual int SCI_METHOD SetLevel(Sci_Position line, int level) = 0;
	virtual int SCI_METHOD GetLineState(Sci_Position line) const noexcept = 0;
	virtual int SCI_METHOD SetLineState(Sci_Position line, int state) = 0;
	virtual void SCI_METHOD StartStyling(Sci_Position position) noexcept = 0;
	virtual bool SCI_METHOD SetStyleFor(Sci_Position length, unsigned char style) = 0;
	virtual bool SCI_METHOD SetStyles(Sci_Position length, const unsigned char *styles) = 0;
	virtual void SCI_METHOD DecorationSetCurrentIndicator(int indicator) noexcept = 0;
	virtual void SCI_METHOD DecorationFillRange(Sci_Position position, int value, Sci_Position fillLength) = 0;
	virtual void SCI_METHOD ChangeLexerState(Sci_Position start, Sci_Position end) = 0;
	virtual int SCI_METHOD CodePage() const noexcept = 0;
	virtual bool SCI_METHOD IsDBCSLeadByte(unsigned char ch) const noexcept = 0;
	virtual const char * SCI_METHOD BufferPointer() = 0;
	virtual int SCI_METHOD GetLineIndentation(Sci_Position line) const noexcept = 0;
	virtual Sci_Position SCI_METHOD LineEnd(Sci_Position line) const noexcept = 0;
	virtual Sci_Position SCI_METHOD GetRelativePosition(Sci_Position positionStart, Sci_Position characterOffset) const noexcept = 0;
	virtual int SCI_METHOD GetCharacterAndWidth(Sci_Position position, Sci_Position *pWidth) const noexcept = 0;
};

enum {
	lvRelease4 = 2
};

class ILexer4 {
public:
	virtual int SCI_METHOD Version() const noexcept = 0;
	virtual void SCI_METHOD Release() noexcept = 0;
	virtual const char * SCI_METHOD PropertyNames() const noexcept = 0;
	virtual int SCI_METHOD PropertyType(const char *name) const = 0;
	virtual const char * SCI_METHOD DescribeProperty(const char *name) const = 0;
	virtual Sci_Position SCI_METHOD PropertySet(const char *key, const char *val) = 0;
	virtual const char * SCI_METHOD DescribeWordListSets() const noexcept = 0;
	virtual Sci_Position SCI_METHOD WordListSet(int n, const char *wl) = 0;
	virtual void SCI_METHOD Lex(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) = 0;
	virtual void SCI_METHOD Fold(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, IDocument *pAccess) = 0;
	virtual void * SCI_METHOD PrivateCall(int operation, void *pointer) = 0;
	virtual int SCI_METHOD LineEndTypesSupported() const noexcept = 0;
	virtual int SCI_METHOD AllocateSubStyles(int styleBase, int numberStyles) = 0;
	virtual int SCI_METHOD SubStylesStart(int styleBase) const noexcept = 0;
	virtual int SCI_METHOD SubStylesLength(int styleBase) const noexcept = 0;
	virtual int SCI_METHOD StyleFromSubStyle(int subStyle) const noexcept = 0;
	virtual int SCI_METHOD PrimaryStyleFromStyle(int style) const noexcept = 0;
	virtual void SCI_METHOD FreeSubStyles() noexcept = 0;
	virtual void SCI_METHOD SetIdentifiers(int style, const char *identifiers) = 0;
	virtual int SCI_METHOD DistanceToSecondaryStyles() const noexcept = 0;
	virtual const char * SCI_METHOD GetSubStyleBases() const noexcept = 0;
	virtual int SCI_METHOD NamedStyles() const noexcept = 0;
	virtual const char * SCI_METHOD NameOfStyle(int style) const noexcept = 0;
	virtual const char * SCI_METHOD TagsOfStyle(int style) const noexcept = 0;
	virtual const char * SCI_METHOD DescriptionOfStyle(int style) const noexcept = 0;
};

}
