// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Lisp.

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
#include "LexerModule.h"

using namespace Lexilla;

namespace {
// TODO: https://www.gnu.org/software/emacs/manual/html_node/elisp/Character-Type.html

constexpr bool IsLispOp(int ch) noexcept {
	return ch == '(' || ch == ')' || ch == '[' || ch == ']'
		|| ch == '\'' || ch == '*' || ch == '?' || ch == '.'
		|| ch == '+' || ch == '-' || ch == '<' || ch == '>'
		|| ch == ':' || ch == '=' || ch == ','
		|| ch == '`' || ch == '#' || ch == '@' || ch == '&';
}

#define MAX_WORD_LENGTH	15
void ColouriseLispDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = keywordLists[0];

	int state = initStyle;
	int ch = 0;
	int chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	const Sci_PositionU endPos = startPos + length;

	Sci_Line lineCurrent = styler.GetLine(startPos);
	char buf[MAX_WORD_LENGTH + 1] = "";
	int wordLen = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int chPrev = ch;
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineStart = i == static_cast<Sci_PositionU>(styler.LineStart(lineCurrent));
		if (atEOL || i == endPos - 1) {
			lineCurrent++;
		}

		switch (state) {
		case SCE_LISP_OPERATOR:
			styler.ColorTo(i, state);
			state = SCE_LISP_DEFAULT;
			break;
		case SCE_LISP_NUMBER:
			if (!IsDecimalNumber(chPrev, ch, chNext)) {
				styler.ColorTo(i, state);
				state = SCE_LISP_DEFAULT;
			}
			break;
		case SCE_LISP_IDENTIFIER:
			if (!(iswordchar(ch) || ch == '-')) {
				buf[wordLen] = '\0';
				if (keywords.InList(buf)) {
					styler.ColorTo(i, SCE_LISP_KEYWORD);
				}
				state = SCE_LISP_DEFAULT;
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = static_cast<char>(ch);
			}
			break;
		case SCE_LISP_CHARACTER:
			styler.ColorTo(i + 1, state);
			state = SCE_LISP_DEFAULT;
			continue;
		case SCE_LISP_STRING:
			if (ch == '\\' && (chNext == '\\' || chNext == '\"')) {
				i++;
				ch = chNext;
				chNext = styler.SafeGetCharAt(i + 1);
			} else if (ch == '\"') {
				styler.ColorTo(i + 1, state);
				state = SCE_LISP_DEFAULT;
				continue;
			}
			break;
		case SCE_LISP_COMMENTLINE:
			if (atLineStart) {
				styler.ColorTo(i, state);
				state = SCE_LISP_DEFAULT;
			}
			break;
		case SCE_LISP_COMMENTBLOCK:
			if (ch == '|' && chNext == '#') {
				i++;
				chNext = styler.SafeGetCharAt(i + 1);
				styler.ColorTo(i + 1, state);
				state = SCE_LISP_DEFAULT;
				continue;
			}
			break;
		}

		if (state == SCE_LISP_DEFAULT) {
			if (((ch == '?' || ch == '#') && chNext == '\\') || (ch == '?' && IsGraphic(chNext))) {
				styler.ColorTo(i, state);
				state = SCE_LISP_CHARACTER;
				if (chNext == '\\') {
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (ch == ';') {
				styler.ColorTo(i, state);
				state = SCE_LISP_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColorTo(i, state);
				state = SCE_LISP_STRING;
			} else if (ch == '#' && chNext == '|') {
				styler.ColorTo(i, state);
				state = SCE_LISP_COMMENTBLOCK;
				i++;
				chNext = styler.SafeGetCharAt(i + 1);
			} else if (IsNumberStart(ch, chNext)) {
				styler.ColorTo(i, state);
				state = SCE_LISP_NUMBER;
			} else if (iswordstart(ch)) {
				styler.ColorTo(i, state);
				state = SCE_LISP_IDENTIFIER;
				buf[0] = static_cast<char>(ch);
				wordLen = 1;
			} else if (IsLispOp(ch)) {
				styler.ColorTo(i, state);
				state = SCE_LISP_OPERATOR;
			}
		}
	}

	// Colourise remaining document
	styler.ColorTo(endPos, state);
}

#define IsCommentLine(line)			IsLexCommentLine(styler, line, SCE_LISP_COMMENTLINE)
#define IsStreamStyle(style)		((style) == SCE_LISP_STRING)
#define IsStreamCommantStyle(style)	((style) == SCE_LISP_COMMENTBLOCK)
void FoldListDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList /*keywordLists*/, Accessor &styler) {
	const Sci_PositionU endPos = startPos + length;
	Sci_Line lineCurrent = styler.GetLine(startPos);
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

		if (atEOL && IsCommentLine(lineCurrent)) {
			levelNext += IsCommentLine(lineCurrent + 1) - IsCommentLine(lineCurrent - 1);
		}
		if (IsStreamCommantStyle(style)) {
			if (!IsStreamCommantStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamCommantStyle(styleNext) && !atEOL) {
				levelNext--;
			}
		} else if (IsStreamStyle(style)) {
			if (!IsStreamStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamStyle(styleNext) && !atEOL) {
				levelNext--;
			}
		}

		if (style == SCE_LISP_OPERATOR) {
			if (ch == '(')
				levelNext++;
			else if (ch == ')')
				levelNext--;
		}

		if (atEOL || (i == endPos - 1)) {
			levelNext = sci::max(levelNext, SC_FOLDLEVELBASE);
			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			styler.SetLevel(lineCurrent, lev);
			lineCurrent++;
			levelCurrent = levelNext;
		}
	}
}

}

LexerModule lmLisp(SCLEX_LISP, ColouriseLispDoc, "lisp", FoldListDoc);
