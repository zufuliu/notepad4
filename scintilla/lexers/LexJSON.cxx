// Lexer for JSON.

#include <cstring>
#include <cassert>
#include <cctype>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Scintilla;

static constexpr bool IsJsonOp(int ch) noexcept {
	return ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ':' || ch == ',' || ch == '+' || ch == '-';
}

#define MAX_WORD_LENGTH	7
static void ColouriseJSONDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const bool fold = styler.GetPropertyInt("fold", 1) != 0;

	int state = initStyle;
	int chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	Sci_PositionU endPos = startPos + length;
	if (endPos == static_cast<Sci_PositionU>(styler.Length()))
		++endPos;

	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;
	char buf[MAX_WORD_LENGTH + 1] = "";
	int wordLen = 0;
	// last non-escaped character before new line
	int chBefore = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		int ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineStart = i == static_cast<Sci_PositionU>(styler.LineStart(lineCurrent));
		if (atEOL || i == endPos - 1) {
			if (fold) {
				const int levelUse = levelCurrent;
				int lev = levelUse | levelNext << 16;
				if (levelUse < levelNext)
					lev |= SC_FOLDLEVELHEADERFLAG;
				if (lev != styler.LevelAt(lineCurrent)) {
					styler.SetLevel(lineCurrent, lev);
				}
				levelCurrent = levelNext;
			}
			lineCurrent++;
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
			if (!iswordstart(ch)) {
				buf[wordLen] = 0;
				if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0 || strcmp(buf, "null") == 0) {
					styler.ColourTo(i - 1, SCE_C_WORD);
				} else if (ch == ':' || IsASpace(ch)) {
					bool isProp = ch == ':' || chNext == ':';
					if (!isProp) {
						Sci_PositionU pos = i + 1;
						while (IsASpace(styler.SafeGetCharAt(pos++)));
						isProp = styler[pos - 1] == ':';
					}
					if (isProp) {
						styler.ColourTo(i - 1, SCE_C_LABEL);
					}
				}
				state = SCE_C_DEFAULT;
				wordLen = 0;
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = static_cast<char>(ch);
			}
			break;
		case SCE_C_STRINGEOL:
			if (atLineStart) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
			}
			break;
		case SCE_C_STRING:
		case SCE_C_CHARACTER:
			if (atEOL) {
				if (chBefore != '\\') {
					state = SCE_C_STRINGEOL;
				}
			} else if (ch == '\\' && (chNext == '\\' || chNext == '\"' || chNext == '\'')) {
				i++;
				ch = chNext;
				chNext = styler.SafeGetCharAt(i + 1);
				chBefore = 0;
			} else if ((state == SCE_C_STRING && ch == '\"') || (state == SCE_C_CHARACTER && ch == '\'')) {
				Sci_PositionU pos = i + 1;
				while (IsASpace(styler.SafeGetCharAt(pos++)));
				if (styler[pos - 1] == ':') {
					styler.ColourTo(i, SCE_C_LABEL);
				} else {
					styler.ColourTo(i, SCE_C_STRING);
				}
				state = SCE_C_DEFAULT;
				continue;
			} else if (ch != '\r') {
				chBefore = ch;
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
			} else if (iswordstart(ch)) {
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

LexerModule lmJSON(SCLEX_JSON, ColouriseJSONDoc, "json");
