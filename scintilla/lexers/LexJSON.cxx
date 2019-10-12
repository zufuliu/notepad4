// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for JSON, JSON5.
// https://www.json.org
// https://json5.org/
// https://spec.json5.org/

#include <cstring>
#include <cassert>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Scintilla;

namespace {

constexpr bool IsJsonOp(int ch) noexcept {
	return ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ':' || ch == ',' || ch == '+' || ch == '-';
}

#define MAX_WORD_LENGTH	7
void ColouriseJSONDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const bool fold = styler.GetPropertyInt("fold", 1) != 0;

	int state = initStyle;
	char chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	const Sci_PositionU endPos = startPos + length;

	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	}
	int levelNext = levelCurrent;
	char buf[MAX_WORD_LENGTH + 1] = "";
	int wordLen = 0;
	// JSON5 line continue
	bool lineContinue = false;
	Sci_PositionU lineStartCurrent = styler.LineStart(lineCurrent);
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		if (styler.IsLeadByte(ch)) {
			// ignore DBCS character
			i++;
			chNext = styler.SafeGetCharAt(i + 1);
			continue;
		}

		const bool atEOL = i == lineStartNext - 1;
		const bool atLineStart = i == lineStartCurrent;
		if (atEOL || i == endPos - 1) {
			if (fold) {
				const int levelUse = levelCurrent;
				int lev = levelUse | levelNext << 16;
				if (levelUse < levelNext) {
					lev |= SC_FOLDLEVELHEADERFLAG;
				}
				if (lev != styler.LevelAt(lineCurrent)) {
					styler.SetLevel(lineCurrent, lev);
				}
				levelCurrent = levelNext;
			}
			lineCurrent++;
			lineStartCurrent = lineStartNext;
			lineStartNext = styler.LineStart(lineCurrent + 1);
		}

		switch (state) {
		case SCE_C_OPERATOR:
			styler.ColourTo(i - 1, state);
			state = SCE_C_DEFAULT;
			break;
		case SCE_C_NUMBER:
			if (!(iswordchar(ch) || ((ch == '+' || ch == '-') && IsADigit(chNext)))) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
			}
			break;
		case SCE_C_IDENTIFIER:
			if (!IsIdentifierChar(ch)) {
				buf[wordLen] = 0;
				if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0 || strcmp(buf, "null") == 0) {
					styler.ColourTo(i - 1, SCE_C_WORD);
				} else if (ch == ':' || chNext == ':' || LexGetNextChar(i + 1, styler) == ':') {
					styler.ColourTo(i - 1, SCE_C_LABEL);
				}
				state = SCE_C_DEFAULT;
				wordLen = 0;
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = static_cast<char>(ch);
			}
			break;
		case SCE_C_STRING:
		case SCE_C_CHARACTER:
			if (atEOL) {
				if (lineContinue) {
					lineContinue = false;
				} else {
					styler.ColourTo(i - 1, state);
					state = SCE_C_DEFAULT;
				}
			} else if (ch == '\\') {
				styler.ColourTo(i - 1, state);
				if (chNext == '\r' || chNext == '\n') {
					styler.ColourTo(i, SCE_C_DEFAULT);
					lineContinue = true;
				} else {
					++i;
					if (chNext == 'u' || chNext == 'x') {
						int count = (chNext == 'u') ? 4 : 2;
						do {
							chNext = styler.SafeGetCharAt(i + 1);
							if (!IsHexDigit(chNext)) {
								break;
							}
							--count;
							++i;
						} while (count);
					}

					chNext = styler.SafeGetCharAt(i + 1);
					styler.ColourTo(i, SCE_C_ESCAPECHAR);
				}
			} else if ((state == SCE_C_STRING && ch == '\"') || (state == SCE_C_CHARACTER && ch == '\'')) {
				if (chNext == ':' || LexGetNextChar(i + 1, styler) == ':') {
					styler.ColourTo(i, SCE_C_LABEL);
				} else {
					styler.ColourTo(i, SCE_C_STRING);
				}
				state = SCE_C_DEFAULT;
				continue;
			}
			break;
		case SCE_C_COMMENTLINE:
			if (atLineStart) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
			}
			break;
		case SCE_C_COMMENT:
			if (ch == '*' && chNext == '/') {
				i++;
				chNext = styler.SafeGetCharAt(i + 1);
				styler.ColourTo(i, state);
				state = SCE_C_DEFAULT;
				levelNext--;
				continue;
			}
			break;
		}

		if (state == SCE_C_DEFAULT) {
			if (ch == '/' && chNext == '/') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENTLINE;
			} else if (ch == '/' && chNext == '*') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENT;
				levelNext++;
				i++;
				chNext = styler.SafeGetCharAt(i + 1);
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_STRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_CHARACTER;
			} else if (IsADigit(ch) || (ch == '.' && IsADigit(chNext))) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_NUMBER;
			} else if (IsIdentifierStart(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_IDENTIFIER;
				buf[wordLen++] = static_cast<char>(ch);
			} else if (IsJsonOp(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_OPERATOR;
				if (ch == '{' || ch == '[') {
					levelNext++;
				} else if (ch == '}' || ch == ']') {
					levelNext--;
				}
			}
		}
	}

	// Colourise remaining document
	styler.ColourTo(endPos - 1, state);
}

}

LexerModule lmJSON(SCLEX_JSON, ColouriseJSONDoc, "json");
