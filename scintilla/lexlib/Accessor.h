// Scintilla source code edit control
/** @file Accessor.h
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla {

enum {
	wsSpace = 1, wsTab = 2, wsSpaceTab = 4, wsInconsistent = 8
};

class Accessor;
class WordList;
class PropSetSimple;

typedef bool (*PFNIsCommentLeader)(Accessor &styler, Sci_Position pos, Sci_Position len);

class Accessor : public LexAccessor {
	const PropSetSimple * const pprops;
public:
	Accessor(IDocument *pAccess_, const PropSetSimple *pprops_) noexcept;
	int GetPropertyInt(const char *key, int defaultValue = 0) const;
	int IndentAmount(Sci_Position line) noexcept;

	[[deprecated]]
	int IndentAmount(Sci_Position line, [[maybe_unused]] int *flags, [[maybe_unused]] PFNIsCommentLeader pfnIsCommentLeader = nullptr) noexcept {
		return IndentAmount(line);
	}
};

}
