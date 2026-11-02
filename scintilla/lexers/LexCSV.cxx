// This file is part of Notepad4.
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

// https://www.rfc-editor.org/rfc/rfc4180
// https://commons.apache.org/proper/commons-csv/apidocs/org/apache/commons/csv/CSVFormat.html

enum {
	// see Styles.cpp
	CsvOption_BackslashEscape = 1 << 15,
	CsvOption_MergeDelimiter = 1 << 16,
	CsvRowGroup = 100,
};

void ColouriseCSVDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList /*keywordLists*/, Accessor &styler) {
	const bool fold = styler.GetPropertyBool("fold");
	const uint32_t csvOption = styler.GetPropertyAsU4("lexer.lang");
	const int delimiter = csvOption & 0xff;
	const int quoteChar = (csvOption >> 8) & 0x7f;

	bool quoted = false;
	int rows = CsvRowGroup - 1;
	initStyle = SCE_CSV_COLUMN_0;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	if (lineCurrent > 0) {
		rows = static_cast<uint32_t>(lineCurrent - 1) % CsvRowGroup;// max 4GB
		const int lineState = styler.GetLineState(lineCurrent - 1);
		if (lineState) {
			quoted = true;
			initStyle = lineState;
		}
	}

	assert(startPos == static_cast<Sci_PositionU>(styler.LineStart(lineCurrent)));
	styler.StartAt(startPos);
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_PositionU validLen = 0;

	int chPrev = 0;
	int chPrevNonWhite = delimiter;
	uint8_t styleBuf[4096];

	while (startPos < endPos) {
		const int ch = static_cast<uint8_t>(styler[startPos]);
		styleBuf[validLen++] = static_cast<uint8_t>(initStyle);
		startPos++;
		if (quoted) {
			if (ch == quoteChar) {
				if (static_cast<uint8_t>(styler[startPos]) == quoteChar) {
					styleBuf[validLen++] = static_cast<uint8_t>(initStyle);
					startPos++;
				} else {
					quoted = false;
				}
			}
		} else {
			if (ch == delimiter) {
				chPrevNonWhite = ch;
				styleBuf[validLen - 1] = SCE_CSV_DELIMITER;
				if (ch != chPrev || (csvOption & CsvOption_MergeDelimiter) == 0) {
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
					styleBuf[validLen++] = static_cast<uint8_t>(initStyle);
					startPos++;
				}
			}
		}

		chPrev = ch;
		if (ch > ' ') {
			chPrevNonWhite = ch;
			if (ch == '\\' && (csvOption & CsvOption_BackslashEscape) != 0) {
				styleBuf[validLen++] = static_cast<uint8_t>(initStyle);
				startPos++;
			}
			if (styler.IsLeadByte(static_cast<uint8_t>(ch))) {
				// ignore trail byte in DBCS character
				styleBuf[validLen++] = static_cast<uint8_t>(initStyle);
				startPos++;
			}
		}

		if (startPos == lineStartNext) {
			if (fold) {
				++rows;
				int lev = SC_FOLDLEVELBASE + 1;
				if (rows == CsvRowGroup) {
					rows = 0;
					lev = SC_FOLDLEVELBASE | SC_FOLDLEVELHEADERFLAG;
				}
				styler.SetLevel(lineCurrent, lev);
			}

			chPrev = 0;
			chPrevNonWhite = delimiter;
			const int lineState = quoted ? initStyle : 0;
			initStyle = quoted ? initStyle : SCE_CSV_COLUMN_0;
			styler.SetLineState(lineCurrent, lineState);
			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
		}
		if (validLen >= sizeof(styleBuf) - 4) {
			styler.MultiByteAccess()->SetStyles(validLen, styleBuf);
			validLen = 0;
		}
	}

	// Colourise remaining document
	if (validLen != 0) {
		styler.MultiByteAccess()->SetStyles(validLen, styleBuf);
	}
}

}

extern const LexerModule lmCSV(SCLEX_CSV, ColouriseCSVDoc, "csv");
