// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Vim.

#include <cassert>
#include <cstring>

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

namespace {

struct EscapeSequence {
	int digitsLeft = 0;

	bool resetEscapeState(int chNext) noexcept {
		// https://vimhelp.org/eval.txt.html#string
		if (chNext > 0x7F || strchr("xXuUbefnrt\\\"", chNext) == nullptr) {
			return false;
		}
		digitsLeft = (chNext == 'u') ? 5 : ((chNext == 'U') ? 9 : ((chNext == 'x' || chNext == 'X')? 3 : 1));
		return true;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

constexpr bool IsVimOp(int ch) noexcept {
	return isoperator(ch) || ch == '#';
}

enum {
	VimLineStateMaskAutoCommand = 1, // autocmd
	VimLineStateMaskLineComment = 2, // line comment
	VimLineStateMaskLineContinue = 4, // line continue
};

void ColouriseVimDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineAutoCommand = 0;
	int lineStateLineComment = 0;
	int lineStateLineContinue = 0;
	int actuallyVisibleChars = 0;
	int visibleChars = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		lineStateLineAutoCommand = lineState & VimLineStateMaskAutoCommand;
	}
	if (startPos == 0 && sc.Match('#', '!')) {
		// Shell Shebang at beginning of file
		sc.SetState(SCE_VIM_COMMENTLINE);
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_VIM_OPERATOR:
			sc.SetState(SCE_VIM_DEFAULT);
			break;
		case SCE_VIM_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_VIM_DEFAULT);
			}
			break;
		case SCE_VIM_IDENTIFIER:
			if (!IsIdentifierChar(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (keywordLists[0]->InList(s)) {
					if (!lineStateLineAutoCommand && visibleChars == sc.LengthCurrent()) {
						sc.ChangeState(SCE_VIM_WORD);
						lineStateLineAutoCommand = strcmp(s, "au") == 0 || strcmp(s, "autocmd") == 0;
					} else {
						sc.ChangeState(SCE_VIM_WORD_DEMOTED);
					}
				} else if (keywordLists[1]->InList(s)) {
					sc.ChangeState(SCE_VIM_COMMANDS);
				} else if (sc.GetNextNSChar() == '(') {
					sc.ChangeState(SCE_VIM_FUNCTION);
				}
				sc.SetState(SCE_VIM_DEFAULT);
			}
			break;
		case SCE_VIM_STRING:
			if (sc.atLineStart) {
				sc.SetState(SCE_VIM_DEFAULT);
			} else if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.chNext)) {
					sc.SetState(SCE_VIM_ESCAPECHAR);
				}
				sc.Forward();
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_VIM_DEFAULT);
			}
			break;
		case SCE_VIM_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				if (sc.ch == '\\') {
					if (!escSeq.resetEscapeState(sc.chNext)) {
						sc.SetState(SCE_VIM_STRING);
					}
					sc.Forward();
				} else {
					sc.SetState(SCE_VIM_STRING);
					if (sc.ch == '\"') {
						sc.ForwardSetState(SCE_VIM_DEFAULT);
					}
				}
			}
			break;
		case SCE_VIM_CHARACTER:
			if (sc.atLineStart) {
				sc.SetState(SCE_VIM_DEFAULT);
			} else if (sc.ch == '\'') {
				if (sc.chNext == '\'') {
					sc.SetState(SCE_VIM_ESCAPECHAR);
					sc.Forward();
					sc.ForwardSetState(SCE_VIM_CHARACTER);
					continue;
				}
				sc.ForwardSetState(SCE_VIM_DEFAULT);
			}
			break;
		case SCE_VIM_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_VIM_DEFAULT);
			}
			break;
		case SCE_VIM_BLOB_HEX:
			if (!iswordchar(sc.ch)) {
				sc.SetState(SCE_VIM_DEFAULT);
			}
			break;
		case SCE_VIM_ENV_VARIABLE:
		case SCE_VIM_OPTION:
		case SCE_VIM_REGISTER:
			if (!IsIdentifierChar(sc.ch)) {
				sc.SetState(SCE_VIM_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_VIM_DEFAULT) {
			if (sc.ch == '\"') {
				sc.SetState((visibleChars == 0) ? SCE_VIM_COMMENTLINE : SCE_VIM_STRING);
				if (actuallyVisibleChars == 0) {
					lineStateLineComment = VimLineStateMaskLineComment;
				}
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_VIM_CHARACTER);
			} else if (sc.ch == '0' && (sc.chNext == 'z' || sc.chNext == 'Z')) {
				sc.SetState(SCE_VIM_BLOB_HEX);
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_VIM_NUMBER);
			} else if ((sc.ch == '$' || sc.ch == '&') && IsIdentifierChar(sc.chNext)) {
				sc.SetState((sc.ch == '$') ? SCE_VIM_ENV_VARIABLE :  SCE_VIM_OPTION);
				sc.Forward();
			} else if (sc.ch == '@') {
				sc.SetState(SCE_VIM_REGISTER);
				sc.Forward();
			} else if (sc.ch == '\\' && visibleChars != 0) {
				sc.Forward();
			} else if (IsIdentifierStart(sc.ch)) {
				if (sc.chNext == ':' && IsLowerCase(sc.ch)) {
					sc.SetState(SCE_VIM_ENV_VARIABLE); // internal variable namespace
					sc.ForwardSetState(SCE_VIM_OPERATOR);
				} else {
					sc.SetState(SCE_VIM_IDENTIFIER);
				}
			} else if (IsVimOp(sc.ch)) {
				sc.SetState(SCE_VIM_OPERATOR);
				if (sc.ch == '|' && sc.chNext != '|' && !lineStateLineAutoCommand) {
					// pipe, change visibleChars to 0 in next block
					visibleChars = -1;
				}
			}
		}

		if (!isspacechar(sc.ch) && !(actuallyVisibleChars == 0 && sc.ch == ':')) {
			if (actuallyVisibleChars == 0) {
				if (sc.ch == '\\') {
					lineStateLineContinue = VimLineStateMaskLineContinue;
				} else {
					lineStateLineAutoCommand = 0;
				}
			}
			actuallyVisibleChars++;
			visibleChars++;
		}

		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, lineStateLineAutoCommand | lineStateLineComment | lineStateLineContinue);
			lineStateLineComment = 0;
			lineStateLineContinue = 0;
			actuallyVisibleChars = 0;
			visibleChars = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int lineContinue;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & VimLineStateMaskLineComment),
		lineContinue(lineState & VimLineStateMaskLineContinue) {
	}
};

void FoldVimDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;

	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;

	int styleNext = styler.StyleAt(startPos);

	constexpr int MaxFoldWordLength = 5 + 1; // while
	char buf[MaxFoldWordLength + 1] = "";
	int wordLen = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		if (style == SCE_VIM_WORD) {
			if (wordLen < MaxFoldWordLength) {
				buf[wordLen++] = styler[i];
			}
			if (styleNext != SCE_VIM_WORD) {
				buf[wordLen] = '\0';
				wordLen = 0;
				if (strcmp(buf, "if") == 0 || strcmp(buf, "while") == 0 || strncmp(buf, "fun", 3) == 0
					|| strcmp(buf, "for") == 0 || strcmp(buf, "try") == 0) {
					levelNext++;
				} else if (strncmp(buf, "end", 3) == 0) {
					levelNext--;
				}
			}
		}

		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldComment && foldCurrent.lineComment) {
				if (!foldPrev.lineComment && foldNext.lineComment) {
					levelNext++;
				} else if (foldPrev.lineComment && !foldNext.lineComment) {
					levelNext--;
				}
			} else {
				if (!foldCurrent.lineContinue && foldNext.lineContinue) {
					levelNext++;
				} else if (foldCurrent.lineContinue && !foldNext.lineContinue) {
					levelNext--;
				}
			}

			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}

			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
			lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;
			levelCurrent = levelNext;
			foldPrev = foldCurrent;
			foldCurrent = foldNext;
		}
	}
}

}

LexerModule lmVim(SCLEX_VIM, ColouriseVimDoc, "vim", FoldVimDoc);
