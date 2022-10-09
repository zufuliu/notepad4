// Scintilla source code edit control
/** @file Accessor.cxx
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2002 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cassert>

#include <string>
#include <string_view>
#include <map>

#include "ILexer.h"
#include "Scintilla.h"

#include "PropSetSimple.h"
#include "LexAccessor.h"
#include "Accessor.h"

using namespace Lexilla;

Accessor::Accessor(Scintilla::IDocument *pAccess_, const PropSetSimple &props_) noexcept : LexAccessor(pAccess_), props(props_) {
}

int Accessor::GetPropertyInt(const char *key, size_t keyLen, int defaultValue) const {
	return props.GetInt(key, keyLen, defaultValue);
}

int Accessor::IndentAmount(Sci_Line line) noexcept {
	const Sci_Position end = Length();
	Sci_Position pos = LineStart(line);

	char ch = '\0';
	int indent = 0;

	// TODO: avoid expanding tab, mixed indentation with space and tab is syntax error in languages like Python.
	while (pos < end) {
		ch = (*this)[pos++];
		if (ch == ' ') {
			indent++;
		} else if (ch == '\t') {
			indent = (indent + 4) & ~3;
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
