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
#include "StringUtils.h"
#include "LexerModule.h"

using namespace Lexilla;

namespace {

constexpr bool IsVimEscapeChar(int ch) noexcept {
	return AnyOf(ch, '\\', '\"', 'b', 'e', 'f', 'n', 'r', 't');
}

struct EscapeSequence {
	int digitsLeft = 0;
	int numBase = 0;

	bool resetEscapeState(int chNext) noexcept {
		// https://vimhelp.org/eval.txt.html#string
		digitsLeft = 0;
		numBase = 16;
		if (chNext == 'x' || chNext == 'X') {
			digitsLeft = 3;
		} else if (chNext == 'u') {
			digitsLeft = 5;
		} else if (chNext == 'U') {
			digitsLeft = 9;
		} else if (IsOctalDigit(chNext)) {
			digitsLeft = 3;
			numBase = 8;
		} else if (IsVimEscapeChar(chNext)) {
			digitsLeft = 1;
		}
		return digitsLeft != 0;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsADigit(ch, numBase);
	}
};

constexpr bool IsVimOp(int ch) noexcept {
	return isoperator(ch) || ch == '#';
}

enum {
	VimLineStateMaskLineComment = 1 << 0,		// line comment
	VimLineStateMaskLineContinuation = 1 << 1,	// line continuation
	VimLineStateMaskAutoCommand = 1 << 2,		// autocmd
};

void ColouriseVimDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineAutoCommand = 0;
	int lineStateLineComment = 0;
	int lineStateLineContinuation = 0;
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
		sc.Forward();
		lineStateLineComment = VimLineStateMaskLineComment;
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
						if (StrEqualsAny(s, "au", "autocmd")) {
							lineStateLineAutoCommand = VimLineStateMaskAutoCommand;
						}
					} else {
						sc.ChangeState(SCE_VIM_WORD_DEMOTED);
					}
				} else if (keywordLists[1]->InList(s)) {
					sc.ChangeState(SCE_VIM_COMMANDS);
				} else if (sc.GetDocNextChar() == '(') {
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
				sc.SetState(SCE_VIM_STRING);
				continue;
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
			if (!(IsIdentifierChar(sc.ch) || sc.ch == '.')) {
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
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_VIM_NUMBER);
			} else if ((sc.ch == '$' || sc.ch == '&') && IsIdentifierChar(sc.chNext)) {
				sc.SetState((sc.ch == '$') ? SCE_VIM_ENV_VARIABLE : SCE_VIM_OPTION);
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
					lineStateLineContinuation = VimLineStateMaskLineContinuation;
				} else {
					lineStateLineAutoCommand = 0;
				}
			}
			actuallyVisibleChars++;
			visibleChars++;
		}

		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, lineStateLineAutoCommand | lineStateLineComment | lineStateLineContinuation);
			lineStateLineComment = 0;
			lineStateLineContinuation = 0;
			actuallyVisibleChars = 0;
			visibleChars = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int lineContinuation;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & VimLineStateMaskLineComment),
		lineContinuation((lineState >> 1) & 1) {
	}
};

void FoldVimDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	int styleNext = styler.StyleAt(startPos);

	char buf[8]; // while
	constexpr int MaxFoldWordLength = sizeof(buf) - 1;
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
				if (StrEqualsAny(buf, "if", "while", "for", "try") || StrStartsWith(buf, "fun")) {
					levelNext++;
				} else if (StrStartsWith(buf, "end")) {
					levelNext--;
				}
			}
		}

		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else {
				levelNext += foldNext.lineContinuation - foldCurrent.lineContinuation;
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
			lineEndPos = sci::min(lineStartNext, endPos) - 1;
			levelCurrent = levelNext;
			foldPrev = foldCurrent;
			foldCurrent = foldNext;
		}
	}
}

}

LexerModule lmVim(SCLEX_VIM, ColouriseVimDoc, "vim", FoldVimDoc);
