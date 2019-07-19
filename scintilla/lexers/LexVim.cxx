// Lexer for Vim.

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

static constexpr bool IsVimOp(int ch) noexcept {
	//return ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ':' || ch == ',' || ch == '+' || ch == '-';
	return isoperator(ch);
}

#define MAX_WORD_LENGTH	15
static void ColouriseVimDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];

	int state = initStyle;
	int chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	const Sci_PositionU endPos = startPos + length;

	Sci_Position lineCurrent = styler.GetLine(startPos);
	int visibleChars = 0;
	char buf[MAX_WORD_LENGTH + 1] = "";
	int wordLen = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		int ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineStart = i == static_cast<Sci_PositionU>(styler.LineStart(lineCurrent));

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
				if (keywords.InList(buf)) {
					styler.ColourTo(i - 1, SCE_C_WORD);
				}
				state = SCE_C_DEFAULT;
				wordLen = 0;
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = static_cast<char>(ch);
			}
			break;
		case SCE_C_STRING:
		case SCE_C_CHARACTER:
			if (atLineStart) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
			} else if (ch == '\\' && (chNext == '\\' || chNext == '\"')) {
				i++;
				ch = chNext;
				chNext = styler.SafeGetCharAt(i + 1);
			} else if ((state == SCE_C_STRING && ch == '\"') || (state == SCE_C_CHARACTER && ch == '\'')) {
				styler.ColourTo(i, state);
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
		}

		if (state == SCE_C_DEFAULT) {
			if (visibleChars == 0 && ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENTLINE;
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
			} else if (IsVimOp(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_OPERATOR;
			}
		}

		if (atEOL || i == endPos - 1) {
			lineCurrent++;
			visibleChars = 0;
		}
		if (!isspacechar(ch) && !(visibleChars == 0 && ch == ':')) {
			visibleChars++;
		}
	}

	// Colourise remaining document
	styler.ColourTo(endPos - 1, state);
}

#define IsCommentLine(line)			IsLexCommentLine(line, styler, SCE_C_COMMENTLINE)
static void FoldVimDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	if (styler.GetPropertyInt("fold") == 0)
		return;
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;

	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		if (visibleChars == 0 && iswordchar(ch) && style == SCE_C_WORD && stylePrev != SCE_C_WORD) {
			char buf[MAX_WORD_LENGTH + 1];
			LexGetRange(i, styler, iswordstart, buf, sizeof(buf));
			if (strcmp(buf, "if") == 0 || strcmp(buf, "while") == 0 || strncmp(buf, "fun", 3) == 0 || strcmp(buf, "for") == 0 || strcmp(buf, "try") == 0) {
				levelNext++;
			} else if (buf[0] == 'e' && buf[1] == 'n' && buf[2] == 'd') {
				levelNext--;
			}
		}

		if (!isspacechar(ch))
			visibleChars++;

		if (atEOL || (i == endPos - 1)) {
			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (visibleChars == 0 && foldCompact)
				lev |= SC_FOLDLEVELWHITEFLAG;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelCurrent = levelNext;
			visibleChars = 0;
		}
	}
}

LexerModule lmVim(SCLEX_VIM, ColouriseVimDoc, "vim", FoldVimDoc);
