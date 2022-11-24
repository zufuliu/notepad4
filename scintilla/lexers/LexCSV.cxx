// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for CSV.

#include <cassert>
#include <cstring>

#include <string>
#include <string_view>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Lexilla;

namespace {

void ColouriseCSVDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList /*keywordLists*/, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	constexpr char delimiter = ',';
	bool quoted = false;
	initStyle = SCE_CSV_COLUMN_0; 
	Sci_Line lineCurrent = styler.GetLine(startPos);
	if (lineCurrent > 0) {
		const int lineState = styler.GetLineState(lineCurrent - 1);
		if (lineState & 1) {
			quoted = true;
			initStyle = lineState >> 1;
		}
	}

	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = styler[i];
		if (quoted) {
			if (ch == '\"') {
				quoted = false;
			}
		} else {
			if (ch == '\"') {
				quoted = true;
			} else if (ch == delimiter) {
				styler.ColorTo(i, initStyle);
				styler.ColorTo(i + 1, SCE_CSV_DELIMITER);
				++initStyle;
				if (initStyle == SCE_CSV_DELIMITER) {
					initStyle = SCE_CSV_COLUMN_0;
				}
			}
		}

		if (styler.IsLeadByte(ch)) {
			// ignore trail byte in DBCS character
			i++;
		}

		if (i == lineEndPos) {
			styler.ColorTo(i + 1, initStyle);
			int lineState = 0;
			if (quoted) {
				lineState = 1 | (initStyle << 1);
			} else {
				initStyle = SCE_CSV_COLUMN_0;
			}
			styler.SetLineState(lineCurrent, lineState);
			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
			lineEndPos = sci::min(lineStartNext, endPos) - 1;
		}
	}

	// Colourise remaining document
	styler.ColorTo(endPos, initStyle);
}

}

LexerModule lmCSV(SCLEX_CSV, ColouriseCSVDoc, "csv");
