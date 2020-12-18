// Scintilla source code edit control
/** @file Accessor.cxx
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2002 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "PropSetSimple.h"
#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"

using namespace Scintilla;

Accessor::Accessor(IDocument *pAccess_, const PropSetSimple *pprops_) noexcept : LexAccessor(pAccess_), pprops(pprops_) {
}

int Accessor::GetPropertyInt(const char *key, int defaultValue) const {
	return pprops->GetInt(key, defaultValue);
}

int Accessor::IndentAmount(Sci_Position line) noexcept {
	const Sci_Position end = Length();
	Sci_Position pos = LineStart(line);

	char ch = '\0';
	unsigned int indent = 0;

	// TODO: avoid expanding tab, mixed indentation with space and tab is syntax error in languages like Python.
	while (pos < end) {
		ch = (*this)[pos++];
		if (ch == ' ') {
			indent++;
		} else if (ch == '\t') {
			indent = (indent / 4 + 1) * 4;
		} else {
			break;
		}
	}

	indent += SC_FOLDLEVELBASE;
	if ((pos == end) || (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) {
		indent |= SC_FOLDLEVELWHITEFLAG;
	}
	return indent;
}
