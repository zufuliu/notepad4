// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for LLVM.

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

#define MAX_WORD_LENGTH	31

static void CheckLLVMVarType(Sci_PositionU pos, Sci_PositionU endPos, Accessor &styler, bool &is_func, bool &is_type) noexcept {
	char c = 0;
	while (pos < endPos) {
		c = styler.SafeGetCharAt(pos);
		if (!(IsASpaceOrTab(c) || c == '*')) break;
		pos++;
	}

	if (c == '(') {
		is_func = true;
		return;
	}
	if (c == '%') {
		is_type = true;
		return;
	}
	if (c == '=') {
		pos++;
		while (pos < endPos) {
			c = styler.SafeGetCharAt(pos);
			if (!IsASpaceOrTab(c)) break;
			pos++;
		}
		if (c == 't' && styler.SafeGetCharAt(pos + 1) == 'y'
			&& styler.SafeGetCharAt(pos + 2) == 'p' && styler.SafeGetCharAt(pos + 3) == 'e'
			&& isspacechar(styler.SafeGetCharAt(pos + 4))) {
			is_type = true;
		}
	}
}

static void ColouriseLLVMDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	const WordList &keywords2 = *keywordLists[1];
	const WordList &kwAttr = *keywordLists[4];
	const WordList &kwInstruction = *keywordLists[10];

	int state = initStyle;
	int chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	const Sci_PositionU endPos = startPos + length;

	Sci_Position lineCurrent = styler.GetLine(startPos);
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
			if (!iswordchar(ch)) {
				buf[wordLen] = '\0';
				if (buf[0] == '@' || buf[0] == '%') {
					if (strncmp(buf + 1, "llvm.", 5) == 0) {
						styler.ColourTo(i - 1, SCE_C_DIRECTIVE);
					} else {
						bool is_func = false;
						bool is_type = false;
						CheckLLVMVarType(i, endPos, styler, is_func, is_type);
						if (is_func) {
							state = SCE_C_PREPROCESSOR;
						} else if (is_type) {
							state = SCE_C_CLASS;
						} else if (buf[0] == '@') {
							state = SCE_C_STRUCT;
						}
						styler.ColourTo(i - 1, state);
					}
				} else if (keywords.InList(buf)) {
					styler.ColourTo(i - 1, SCE_C_WORD);
				} else if (keywords2.InList(buf)) {
					styler.ColourTo(i - 1, SCE_C_WORD2);
				} else if (kwInstruction.InList(buf)) {
					styler.ColourTo(i - 1, SCE_C_ASM_INSTRUCTION);
				} else if (kwAttr.InList(buf)) {
					styler.ColourTo(i - 1, SCE_C_ATTRIBUTE);
				} else if (ch == ':') {
					styler.ColourTo(i - 1, SCE_C_LABEL);
				}
				state = SCE_C_DEFAULT;
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = static_cast<char>(ch);
			}
			break;
		case SCE_C_WORD2:
			if (!IsADigit(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_DEFAULT;
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
			if (ch == ';') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_COMMENTLINE;
			} else if (ch == '\"' || (ch == 'c' && chNext == '\"')) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_STRING;
				if (ch == 'c') {
					++i;
					chNext = styler.SafeGetCharAt(i + 1);
				}
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_CHARACTER;
			} else if (IsADigit(ch) || (ch == '.' && IsADigit(chNext))) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_NUMBER;
			} else if (ch == 'i' && IsADigit(chNext)) { // iN
				styler.ColourTo(i - 1, state);
				state = SCE_C_WORD2;
			} else if (ch == 'x' && isspacechar(chNext)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_OPERATOR;
			} else if (iswordstart(ch) || ch == '@' || ch == '%') {
				styler.ColourTo(i - 1, state);
				state = SCE_C_IDENTIFIER;
				buf[0] = static_cast<char>(ch);
				wordLen = 1;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_C_OPERATOR;
			}
		}

		if (atEOL || i == endPos - 1) {
			lineCurrent++;
		}
	}

	// Colourise remaining document
	styler.ColourTo(endPos - 1, state);
}

#define IsCommentLine(line)			IsLexCommentLine(line, styler, SCE_C_COMMENTLINE)
static void FoldLLVMDoc(Sci_PositionU startPos, Sci_Position length, int /*initStyle*/, LexerWordList, Accessor &styler) {
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

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		if (style == SCE_C_OPERATOR) {
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
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

LexerModule lmLLVM(SCLEX_LLVM, ColouriseLLVMDoc, "llvm", FoldLLVMDoc);
