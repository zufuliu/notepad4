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
	const char * const option = styler.GetProperty("lexer.lang");
	const uint8_t delimiter = option[0];
	const uint8_t quoteChar = option[1];
	const bool mergeDelimiter = option[2] & true;

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
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos);

	uint8_t chPrev = 0;
	uint8_t chPrevNonWhite = delimiter;
	while (startPos < endPos) {
		const uint8_t ch = styler[startPos++];
		if (quoted) {
			if (ch == quoteChar) {
				if (static_cast<uint8_t>(styler[startPos]) == quoteChar) {
					startPos++;
				} else {
					quoted = false;
				}
			}
		} else {
			if (ch == delimiter) {
				chPrevNonWhite = delimiter;
				styler.ColorTo(startPos - 1, initStyle);
				styler.ColorTo(startPos, SCE_CSV_DELIMITER);
				if (!mergeDelimiter || chPrev != delimiter) {
					++initStyle;
					if (initStyle == SCE_CSV_DELIMITER) {
						initStyle = SCE_CSV_COLUMN_0;
					}
				}
			} else if (chPrevNonWhite == delimiter) {
				if (ch == quoteChar) {
					quoted = true;
				} else if (ch == '=' && static_cast<uint8_t>(styler[startPos]) == quoteChar) {
					quoted = true;
					startPos++;
				}
			}
		}

		chPrev = ch;
		if (ch > ' ') {
			chPrevNonWhite = ch;
			if (styler.IsLeadByte(ch)) {
				// ignore trail byte in DBCS character
				startPos++;
			}
		}

		if (startPos == lineEndPos) {
			chPrev = 0;
			chPrevNonWhite = delimiter;
			styler.ColorTo(startPos, initStyle);
			int lineState = 0;
			if (quoted) {
				lineState = 1 | (initStyle << 1);
			} else {
				initStyle = SCE_CSV_COLUMN_0;
			}
			styler.SetLineState(lineCurrent, lineState);
			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
			lineEndPos = sci::min(lineStartNext, endPos);
		}
	}

	// Colourise remaining document
	styler.ColorTo(endPos, initStyle);
}

}

LexerModule lmCSV(SCLEX_CSV, ColouriseCSVDoc, "csv");
