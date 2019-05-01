// Scintilla source code edit control
/** @file DefaultLexer.cxx
 ** A lexer base class that provides reasonable default behaviour.
 **/
// Copyright 2017 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>
#include <cstring>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "PropSetSimple.h"
#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "LexerModule.h"
#include "DefaultLexer.h"

using namespace Scintilla;

static const char styleSubable[] = { 0 };

DefaultLexer::DefaultLexer(const LexicalClass *lexClasses_, size_t nClasses_) :
	lexClasses(lexClasses_), nClasses(nClasses_) {
}

DefaultLexer::~DefaultLexer() = default;

void SCI_METHOD DefaultLexer::Release() noexcept {
	delete this;
}

int SCI_METHOD DefaultLexer::Version() const noexcept {
	return lvRelease4;
}

const char * SCI_METHOD DefaultLexer::PropertyNames() const noexcept {
	return "";
}

int SCI_METHOD DefaultLexer::PropertyType(const char *) const {
	return SC_TYPE_BOOLEAN;
}

const char * SCI_METHOD DefaultLexer::DescribeProperty(const char *) const {
	return "";
}

Sci_Position SCI_METHOD DefaultLexer::PropertySet(const char *, const char *) {
	return -1;
}

const char * SCI_METHOD DefaultLexer::DescribeWordListSets() const noexcept {
	return "";
}

Sci_Position SCI_METHOD DefaultLexer::WordListSet(int, const char *) {
	return -1;
}

void SCI_METHOD DefaultLexer::Fold(Sci_PositionU, Sci_Position, int, IDocument *) {
}

void * SCI_METHOD DefaultLexer::PrivateCall(int, void *) {
	return nullptr;
}

int SCI_METHOD DefaultLexer::LineEndTypesSupported() const noexcept {
	return SC_LINE_END_TYPE_DEFAULT;
}

int SCI_METHOD DefaultLexer::AllocateSubStyles(int, int) {
	return -1;
}

int SCI_METHOD DefaultLexer::SubStylesStart(int) const noexcept {
	return -1;
}

int SCI_METHOD DefaultLexer::SubStylesLength(int) const noexcept {
	return 0;
}

int SCI_METHOD DefaultLexer::StyleFromSubStyle(int subStyle) const noexcept {
	return subStyle;
}

int SCI_METHOD DefaultLexer::PrimaryStyleFromStyle(int style) const noexcept {
	return style;
}

void SCI_METHOD DefaultLexer::FreeSubStyles() noexcept {
}

void SCI_METHOD DefaultLexer::SetIdentifiers(int, const char *) {
}

int SCI_METHOD DefaultLexer::DistanceToSecondaryStyles() const noexcept {
	return 0;
}

const char * SCI_METHOD DefaultLexer::GetSubStyleBases() const noexcept {
	return styleSubable;
}

int SCI_METHOD DefaultLexer::NamedStyles() const noexcept {
	return static_cast<int>(nClasses);
}

const char * SCI_METHOD DefaultLexer::NameOfStyle(int style) const noexcept {
	return (style < NamedStyles()) ? lexClasses[style].name : "";
}

const char * SCI_METHOD DefaultLexer::TagsOfStyle(int style) const noexcept {
	return (style < NamedStyles()) ? lexClasses[style].tags : "";
}

const char * SCI_METHOD DefaultLexer::DescriptionOfStyle(int style) const noexcept {
	return (style < NamedStyles()) ? lexClasses[style].description : "";
}
