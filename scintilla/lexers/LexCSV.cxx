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

// https://www.rfc-editor.org/rfc/rfc4180
// https://commons.apache.org/proper/commons-csv/apidocs/org/apache/commons/csv/CSVFormat.html

enum {
	CsvOption_BackslashEscape = 1 << 15,
	CsvOption_MergeDelimiter = 1 << 16,
	CsvRowGroup = 100,
};

constexpr uint32_t asU4(const char *s) noexcept {
	return *(const uint32_t *)s;
}

void ColouriseCSVDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList /*keywordLists*/, Accessor &styler) {
	const bool fold = styler.GetPropertyBool("fold");
	const char * const option = styler.GetProperty("lexer.lang");
	const uint32_t csvOption = asU4(option);
	const uint8_t delimiter = csvOption & 0xff;
	const uint8_t quoteChar = (csvOption >> 8) & 0x7f;

	bool quoted = false;
	int rows = 0;
	initStyle = SCE_CSV_COLUMN_0;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	if (lineCurrent > 0) {
		rows = static_cast<int>(lineCurrent % CsvRowGroup);
		const int lineState = styler.GetLineState(lineCurrent - 1);
		if (lineState) {
			quoted = true;
			initStyle = lineState;
		}
	}

	assert(startPos == static_cast<Sci_PositionU>(styler.LineStart(lineCurrent)));
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	const Sci_PositionU endPos = startPos + lengthDoc;
	lineStartNext = sci::min(lineStartNext, endPos);

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
				chPrevNonWhite = ch;
				styler.ColorTo(startPos - 1, initStyle);
				styler.ColorTo(startPos, SCE_CSV_DELIMITER);
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
					startPos++;
				}
			}
		}

		chPrev = ch;
		if (ch > ' ') {
			chPrevNonWhite = ch;
			if (ch == '\\' && (csvOption & CsvOption_BackslashEscape) != 0) {
				startPos++;
			}
			if (styler.IsLeadByte(ch)) {
				// ignore trail byte in DBCS character
				startPos++;
			}
		}

		if (startPos == lineStartNext) {
			if (fold) {
				++rows;
				int lev = SC_FOLDLEVELBASE + 1;
				if (rows == CsvRowGroup || lineCurrent == 0) {
					rows = 0;
					lev = SC_FOLDLEVELBASE | SC_FOLDLEVELHEADERFLAG;
				}
				styler.SetLevel(lineCurrent, lev);
			}

			chPrev = 0;
			chPrevNonWhite = delimiter;
			styler.ColorTo(startPos, initStyle);
			const int lineState = quoted ? initStyle : 0;
			initStyle = quoted ? initStyle : SCE_CSV_COLUMN_0;
			styler.SetLineState(lineCurrent, lineState);
			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
			lineStartNext = sci::min(lineStartNext, endPos);
		}
	}

	// Colourise remaining document
	styler.ColorTo(endPos, initStyle);
}

}

LexerModule lmCSV(SCLEX_CSV, ColouriseCSVDoc, "csv");
