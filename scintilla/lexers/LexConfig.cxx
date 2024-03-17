// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Configuration Files.

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

constexpr bool IsConfOp(int ch) noexcept {
	return ch == '=' || ch == ':' || ch == ';' || ch == '{' || ch == '}' || ch == '(' || ch == ')' ||
		ch == '!' || ch == ',' || ch == '|' || ch == '*' || ch == '$' || ch == '.';
}
constexpr bool IsUnit(int ch) noexcept {
	return ch == 'K' || ch == 'M' || ch == 'G' || ch == 'k' || ch == 'm' || ch == 'g';
}
constexpr bool IsDelimiter(int ch) noexcept {
	return IsASpace(ch) || IsConfOp(ch);
}

void ColouriseConfDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList /*keywordLists*/, Accessor &styler) {
	int state = initStyle;
	int chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	const Sci_PositionU endPos = startPos + length;

	int visibleChars = 0;
	bool insideTag = false;
	Sci_Line lineCurrent = styler.GetLine(startPos);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		int ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineStart = i == static_cast<Sci_PositionU>(styler.LineStart(lineCurrent));

		switch (state) {
		case SCE_CONF_OPERATOR:
			styler.ColorTo(i, state);
			state = SCE_CONF_DEFAULT;
			break;
		case SCE_CONF_NUMBER:
			if (!(IsADigit(ch) || ch == '.')) {
				if (IsUnit(ch) && IsDelimiter(chNext)) {
					styler.ColorTo(i + 1, state);
					state = SCE_CONF_DEFAULT;
					continue;
				}
				if (iswordchar(ch)) {
					state = SCE_CONF_IDENTIFIER;
				} else {
					styler.ColorTo(i, state);
					state = SCE_CONF_DEFAULT;
				}
			}
			break;
		case SCE_CONF_HEXNUM:
			if (!IsHexDigit(ch)) {
				styler.ColorTo(i, state);
				state = SCE_CONF_DEFAULT;
			}
			break;
		case SCE_CONF_STRING:
			if (atLineStart) {
				styler.ColorTo(i, state);
				state = SCE_CONF_DEFAULT;
			} else if (ch == '\\' && (chNext == '\\' || chNext == '\"')) {
				i++;
				ch = chNext;
				chNext = styler.SafeGetCharAt(i + 1);
			} else if (ch == '\"') {
				styler.ColorTo(i + 1, state);
				state = SCE_CONF_DEFAULT;
				continue;
			}
			break;
		case SCE_CONF_DIRECTIVE:
			if (insideTag && ch == ':') {
				styler.ColorTo(i, state);
				if (chNext == ':') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
				styler.ColorTo(i + 1, SCE_CONF_OPERATOR);
				state = SCE_CONF_DIRECTIVE;
			} else if (IsDelimiter(ch) || (insideTag && ch == '>')) {
				styler.ColorTo(i, state);
				if (ch == '.') {
					styler.ColorTo(i + 1, SCE_CONF_OPERATOR);
				} else {
					state = SCE_CONF_DEFAULT;
				}
			}
			break;
		case SCE_CONF_SECTION:
		case SCE_CONF_COMMENT:
			if (atLineStart) {
				styler.ColorTo(i, state);
				state = SCE_CONF_DEFAULT;
			}
			break;
		case SCE_CONF_IDENTIFIER:
			if (IsDelimiter(ch) || (insideTag && ch == '>') || (ch == '<' && chNext == '/')) {
				styler.ColorTo(i, state);
				if (ch == '.') {
					styler.ColorTo(i + 1, SCE_CONF_OPERATOR);
				} else {
					state = SCE_CONF_DEFAULT;
				}
			}
			break;
		}

		if (state != SCE_CONF_COMMENT && ch == '\\' && IsEOLChar(chNext)) {
			i++;
			lineCurrent++;
			ch = chNext;
			chNext = styler.SafeGetCharAt(i + 1);
			if (ch == '\r' && chNext == '\n') {
				i++;
				chNext = styler.SafeGetCharAt(i + 1);
			}
			continue;
		}

		if (state == SCE_CONF_DEFAULT) {
			if (ch == '#') {
				styler.ColorTo(i, state);
				state = SCE_CONF_COMMENT;
			} else if (ch == '\"') {
				styler.ColorTo(i, state);
				state = SCE_CONF_STRING;
			} else if (IsConfOp(ch)) {
				styler.ColorTo(i, state);
				state = SCE_CONF_OPERATOR;
			} else if ((visibleChars == 0 && !IsASpace(ch)) || (ch == '<' && chNext == '/')) {
				styler.ColorTo(i, state);
				if (ch == '[') {
					state = SCE_CONF_SECTION;
				} else {
					state = SCE_CONF_DIRECTIVE;
					insideTag = ch == '<';
					if (chNext == '/') {
						i++;
						ch = chNext;
						chNext = styler.SafeGetCharAt(i + 1);
					}
				}
			} else if (insideTag && (ch == '>' || ((ch == '/' || ch == '?') && chNext == '>'))) {
				styler.ColorTo(i, state);
				if (ch == '/' || ch == '?') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				}
				styler.ColorTo(i + 1, SCE_CONF_DIRECTIVE);
				state = SCE_CONF_DEFAULT;
				insideTag = false;
			} else if ((ch == '+' || ch == '-') && IsAlphaNumeric(chNext)) {
				styler.ColorTo(i, state);
				state = SCE_CONF_OPERATOR;
			} else if (IsADigit(ch)) {
				styler.ColorTo(i, state);
				if (ch == '0' && (chNext == 'x' || chNext == 'X')) {
					state = SCE_CONF_HEXNUM;
					i++;
					ch = chNext;
					chNext = styler.SafeGetCharAt(i + 1);
				} else {
					state = SCE_CONF_NUMBER;
				}
			} else if (!IsASpace(ch)) {
				styler.ColorTo(i, state);
				state = SCE_CONF_IDENTIFIER;
			}
		}

		if (atEOL || i == endPos - 1) {
			lineCurrent++;
			visibleChars = 0;
		}
		if (visibleChars == 0 && !isspacechar(ch)) {
			visibleChars++;
		}
	}

	// Colourise remaining document
	styler.ColorTo(endPos, state);
}

#define IsCommentLine(line)		IsLexCommentLine(styler, line, SCE_CONF_COMMENT)

void FoldConfDoc(Sci_PositionU startPos, Sci_Position length, int /*initStyle*/, LexerWordList /*keywordLists*/, Accessor &styler) {
	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (atEOL && IsCommentLine(lineCurrent)) {
			levelNext += IsCommentLine(lineCurrent + 1) - IsCommentLine(lineCurrent - 1);
		}

		if (style == SCE_CONF_DIRECTIVE) {
			if (visibleChars == 0 && ch == '<' && !(chNext == '/' || chNext == '?')) {
				levelNext++;
			} else if (ch == '<' && chNext == '/') {
				levelNext--;
			} else if (ch == '/' && chNext == '>') {
				Sci_PositionU pos = i;
				while (pos > startPos) {
					pos--;
					const char c = styler.SafeGetCharAt(pos);
					if (!(c == ' ' || c == '\t')) {
						break;
					}
				}
				if (!(IsAlphaNumeric(styler.SafeGetCharAt(pos)) && styler.StyleAt(pos) == SCE_CONF_DIRECTIVE)) {
					levelNext--;
				}
			}
		}

		if (style == SCE_CONF_OPERATOR) {
			if (ch == '{') {
				levelNext++;
			} else if (ch == '}') {
				levelNext--;
			}
		}

		if (visibleChars == 0 && !isspacechar(ch)) {
			visibleChars++;
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
			visibleChars = 0;
		}
	}
}

}

LexerModule lmConfig(SCLEX_CONFIG, ColouriseConfDoc, "conf", FoldConfDoc);
