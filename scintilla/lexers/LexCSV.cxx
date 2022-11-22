// Scintilla source code edit control
/** @file LexCSV.cxx
** Lexer for CSV.
**/
// Rainbow clouring for CSV files
// Written by RaiKoHoff
// Readapted by Matteo-Nigro
// The License.txt file describes the conditions under which this software may be distributed.

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
#include "StyleContext.h"
#include "CharacterSet.h"
#include "StringUtils.h"
#include "LexerModule.h"

using namespace Lexilla;
using namespace Scintilla;

namespace {

enum delim : unsigned int { eComma = 0, eSemic, eTab, ePipe, eMax };
static int const DelimList[eMax] = { ',',	 ';',	 '\t',	'|' };

constexpr int GetStateByColumn(const int col) noexcept{
	switch (col % 10){
		case 0: return SCE_CSV_COL_0;
		case 1: return SCE_CSV_COL_1;
		case 2: return SCE_CSV_COL_2;
		case 3: return SCE_CSV_COL_3;
		case 4: return SCE_CSV_COL_4;
		case 5: return SCE_CSV_COL_5;
		case 6: return SCE_CSV_COL_6;
		case 7: return SCE_CSV_COL_7;
		case 8: return SCE_CSV_COL_8;
		case 9: return SCE_CSV_COL_9;
		default: break;
	}
	return SCE_CSV_COL_0;
}

constexpr bool IsNewline(const int ch) {
	return (ch == '\n' || ch == '\r' || ch == '\0');
}

constexpr bool IsSingleQuoteChar(const int ch) noexcept{
	return (ch == '\'');
}

constexpr bool IsDoubleQuoteChar(const int ch) noexcept{
	return (ch == '"');
}

constexpr unsigned int IsDelimiter(const int ch) noexcept{
	for (unsigned int i = 0; i < eMax; ++i){
		if (DelimList[i] == ch){
			return i;
		}
	}
	return eMax;
}

constexpr Sci_PositionU CountCharOccTillLineEnd(StyleContext& sc, const Sci_PositionU endPos){
	Sci_Position i = 0;
	Sci_PositionU count = 0;
	while (((sc.currentPos + i) < endPos) && !IsNewline(sc.GetRelative(i))){
		if (sc.GetRelative(++i) == sc.ch) { ++count; };
	}
	return count;
}

static inline bool HandleQuoteContext(StyleContext& sc, bool& isInSQString, bool& isInDQString, const Sci_PositionU endPos){
	if (IsSingleQuoteChar(sc.ch)){
		Sci_PositionU const focc = isInSQString ? 1 : CountCharOccTillLineEnd(sc, endPos);
		if (!isInDQString && (focc % 2 == 1)){
			isInSQString = !isInSQString;
		}
		return true;
	}
	if (IsDoubleQuoteChar(sc.ch)){
		Sci_PositionU const focc = isInDQString ? 1 : CountCharOccTillLineEnd(sc, endPos);
		if (!isInSQString && (focc % 2 == 1)){
			isInDQString = !isInDQString;
		}
		return true;
	}
	return false;
}

void ColouriseCSVDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler){
	Sci_PositionU endPos = startPos + length;
	Sci_PositionU delimCount[eMax] = { 0 };
	Sci_PositionU countPerPrevLine[eMax] = { 0 };
	Sci_PositionU smartDelimVote[eMax] = { 0 };
	Sci_PositionU columnAvg = 0;
	bool isInSQString = false;
	bool isInDQString = false;
	StyleContext sc(startPos, length, initStyle, styler);
	for (; sc.More(); sc.Forward()){
		if (sc.atLineStart){
			isInSQString = false;
			isInDQString = false;
			for (unsigned int i = 0; i < eMax; ++i){
				Sci_PositionU const dlm = delimCount[i];
				if (dlm > 0){
					smartDelimVote[i] += 1;
					if ((dlm == countPerPrevLine[i])){
						smartDelimVote[i] += dlm;
					}
					if (dlm == columnAvg){
						smartDelimVote[i] += dlm;
					}
					columnAvg = (columnAvg == 0) ? dlm : (columnAvg + dlm - 1) >> 1;
				}
				countPerPrevLine[i] = dlm;
				delimCount[i] = 0;
			}
		}
		if (!HandleQuoteContext(sc, isInSQString, isInDQString, endPos) && (!isInSQString && !isInDQString)){
			unsigned int i = IsDelimiter(sc.ch);
			if (i < eMax){
				++delimCount[i];
			}
		}
	}
	sc.Complete();
	int delim = DelimList[0];
	Sci_PositionU maxVote = smartDelimVote[0];
	for (unsigned int i = 1; i < eMax; ++i){
		if (maxVote < smartDelimVote[i]){
			delim = DelimList[i];
			maxVote = smartDelimVote[i];
		}
	}
	int const delimiter = delim;
	int csvColumn = 0;
	isInSQString = false;
	isInDQString = false;
	StyleContext sc2(startPos, length, initStyle, styler);

	for (; sc2.More(); sc2.Forward()){
		if (sc2.atLineStart){
			csvColumn = 0;
			isInSQString = false;
			isInDQString = false;
			sc2.SetState(GetStateByColumn(csvColumn));
		}
		if (!HandleQuoteContext(sc2, isInSQString, isInDQString, endPos) && (delimiter == sc2.ch)){
			if (!isInSQString && !isInDQString){
				sc2.SetState(GetStateByColumn(++csvColumn));
			}
		}
	}
	sc2.Complete();
}
}
LexerModule lmCSV(SCLEX_CSV, ColouriseCSVDoc, "csv");