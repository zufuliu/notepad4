// Scintilla source code edit control
/** @file LexerBase.cxx
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

using namespace Lexilla;

LexerBase::LexerBase() {
	auto *iter = keywordLists;
	for (int wl = KEYWORDSET_MAX; wl; wl--) {
		*iter++ = new WordList;
	}
}

LexerBase::~LexerBase() {
	auto *iter = keywordLists;
	for (int wl = KEYWORDSET_MAX; wl; wl--) {
		delete *iter++;
	}
}

void SCI_METHOD LexerBase::Release() noexcept {
	delete this;
}

int SCI_METHOD LexerBase::Version() const noexcept {
	return Scintilla::lvRelease5;
}

const char * SCI_METHOD LexerBase::PropertyNames() const noexcept {
	return "";
}

int SCI_METHOD LexerBase::PropertyType(const char *) const noexcept {
	return SC_TYPE_BOOLEAN;
}

const char * SCI_METHOD LexerBase::DescribeProperty(const char *) const noexcept {
	return "";
}

Sci_Position SCI_METHOD LexerBase::PropertySet(const char *key, const char *val) {
	if (props.Set(key, val)) {
		return 0;
	}
	return -1;
}

const char *SCI_METHOD LexerBase::PropertyGet(const char *key) const {
	return props.Get(key);
}

const char * SCI_METHOD LexerBase::DescribeWordListSets() const noexcept {
	return "";
}

Sci_Position SCI_METHOD LexerBase::WordListSet(int n, bool toLower, const char *wl) {
	if (n < KEYWORDSET_MAX) {
		if (keywordLists[n]->Set(wl, toLower)) {
			return 0;
		}
	}
	return -1;
}

void * SCI_METHOD LexerBase::PrivateCall(int, void *) noexcept {
	return nullptr;
}

int SCI_METHOD LexerBase::LineEndTypesSupported() const noexcept {
	return SC_LINE_END_TYPE_DEFAULT;
}

int SCI_METHOD LexerBase::AllocateSubStyles(int, int) noexcept {
	return -1;
}

int SCI_METHOD LexerBase::SubStylesStart(int) const noexcept {
	return -1;
}

int SCI_METHOD LexerBase::SubStylesLength(int) const noexcept {
	return 0;
}

int SCI_METHOD LexerBase::StyleFromSubStyle(int subStyle) const noexcept {
	return subStyle;
}

int SCI_METHOD LexerBase::PrimaryStyleFromStyle(int style) const noexcept {
	return style;
}

void SCI_METHOD LexerBase::FreeSubStyles() noexcept {
}

void SCI_METHOD LexerBase::SetIdentifiers(int, const char *) noexcept {
}

int SCI_METHOD LexerBase::DistanceToSecondaryStyles() const noexcept {
	return 0;
}

const char * SCI_METHOD LexerBase::GetSubStyleBases() const noexcept {
	return "";
}

int SCI_METHOD LexerBase::NamedStyles() const noexcept {
	return 0;
}

const char * SCI_METHOD LexerBase::NameOfStyle([[maybe_unused]] int style) const noexcept {
	return "";
}

const char * SCI_METHOD LexerBase::TagsOfStyle([[maybe_unused]] int style) const noexcept {
	return "";
}

const char * SCI_METHOD LexerBase::DescriptionOfStyle([[maybe_unused]] int style) const noexcept {
	return "";
}

// ILexer5 methods

const char *SCI_METHOD LexerBase::GetName() const noexcept {
	return "";
}

int SCI_METHOD LexerBase::GetIdentifier() const noexcept {
	return SCLEX_AUTOMATIC;
}
