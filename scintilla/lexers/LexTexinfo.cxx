// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for GNU Texinfo.

#include <cassert>
#include <cstring>
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

static constexpr bool IsTexiSpec(int ch) noexcept {
	return ch == '@' || ch == '{' || ch == '}' ||
		ch == '*' || ch == '/' || ch == '-' ||
		ch == ':' || ch == '.' || ch == '?' || ch == '?' ||
		ch == '\"' || ch == '\'' || ch == ',' || ch == '=' || ch == '~';
}

#define MAX_WORD_LENGTH	31
static void ColouriseTexiDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const bool fold = styler.GetPropertyInt("fold", 1) != 0;
	//const WordList &keywords = *keywordLists[0]; // command
	const WordList &keywords2 = *keywordLists[1];// fold
	//const WordList &keywords3 = *keywordLists[2];// condition
	//const WordList &keywords4 = *keywordLists[3];// command with arg

	int state = initStyle;
	int ch = 0;
	int chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	const Sci_PositionU endPos = startPos + length;

	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;
	char buf[MAX_WORD_LENGTH + 1] = "";
	int wordLen = 0;
	bool isCommand = false;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int chPrev = ch;
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineStart = i == static_cast<Sci_PositionU>(styler.LineStart(lineCurrent));

		switch (state) {
		case SCE_L_OPERATOR:
			styler.ColourTo(i - 1, state);
			state = SCE_L_DEFAULT;
			break;
		case SCE_L_SPECIAL:
			styler.ColourTo(i, state);
			state = SCE_L_DEFAULT;
			continue;
		case SCE_L_COMMAND:
			if (!IsAlpha(ch)) {
				buf[wordLen] = '\0';
				if (strcmp(buf, "@c") == 0 || strcmp(buf, "@comment") == 0) {
					state = SCE_L_COMMENT;
				} else if (strcmp(buf, "@end") == 0) {
					levelNext--;
					isCommand = true;
				} else {
					if (buf[0] == '@' && keywords2.InList(buf + 1)) {
						levelNext++;
					}
					if (strcmp(buf, "@settitle") == 0) {
						state = SCE_L_TITLE;
					} else if (strcmp(buf, "@chapter") == 0) {
						state = SCE_L_CHAPTER;
					} else if (strcmp(buf, "@section") == 0) {
						state = SCE_L_SECTION;
					} else if (strcmp(buf, "@subsection") == 0) {
						state = SCE_L_SECTION1;
					} else if (strcmp(buf, "@subsubsection") == 0) {
						state = SCE_L_SECTION2;
					}
				}
				if (state == SCE_L_COMMAND) {
					styler.ColourTo(i - 1, state);
					state = SCE_L_DEFAULT;
				}
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = static_cast<char>(ch);
			}
			break;
		case SCE_L_TAG:
			if (!IsLowerCase(ch)) {
				if (isCommand) {
					styler.ColourTo(i - 1, SCE_L_COMMAND);
				}
				state = SCE_L_DEFAULT;
			}
			break;
		case SCE_L_COMMENT:
		case SCE_L_TITLE:
		case SCE_L_CHAPTER:
		case SCE_L_SECTION:
		case SCE_L_SECTION1:
		case SCE_L_SECTION2:
			if (atLineStart) {
				styler.ColourTo(i - 1, state);
				state = SCE_L_DEFAULT;
			}
			break;
		}

		if (state == SCE_L_DEFAULT) {
			if (lineCurrent == 0 && i == 0 && ch == '\\' && chNext == 'i') { // \input texinfo.tex
				state = SCE_L_COMMAND;
				buf[0] = static_cast<char>(ch);
				wordLen = 1;
			} else if (ch == '@') {
				if (IsTexiSpec(chNext)) {
					styler.ColourTo(i - 1, state);
					state = SCE_L_SPECIAL;
				} else if (IsAlpha(chNext)) {
					styler.ColourTo(i - 1, state);
					state = SCE_L_COMMAND;
					buf[0] = static_cast<char>(ch);
					wordLen = 1;
				}
			} else if (ch == '@' || ch == '{' || ch == '}' ||
				(ch == '-' && !IsAlpha(chPrev) && !IsAlpha(chNext))) {
				styler.ColourTo(i - 1, state);
				state = SCE_L_OPERATOR;
			} else if (IsLowerCase(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_L_TAG;
			}
		}

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
			isCommand = false;
		}

	}

	// Colourise remaining document
	styler.ColourTo(endPos - 1, state);
}

LexerModule lmTexinfo(SCLEX_TEXINFO, ColouriseTexiDoc, "texi");
