// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for WebAssembly.

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

	// highlight any character as escape sequence, no highlight for hex in '\u{hex}'.
	void resetEscapeState(int chNext) noexcept {
		digitsLeft = 1;
		if (IsHexDigit(chNext)) {
			digitsLeft = 2;
		}
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

enum {
	WASMLineStateMaskLineComment = (1 << 8), // line comment
};

void ColouriseWASMDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const CharacterSet setIdChar(CharacterSet::setAlphaNum, "!#$%&'*+-./:<=>?@\\^_`|~");

	int lineStateLineComment = 0;
	int commentLevel = 0;	// nested block comment level

	int visibleChars = 0;
	Sci_PositionU prefixLen = 0;

	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		8: commentLevel
		1: lineStateLineComment
		*/
		commentLevel = lineState & 0xff;
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_WASM_OPERATOR:
			sc.SetState(SCE_WASM_DEFAULT);
			break;

		case SCE_WASM_NUMBER:
			if (!IsDecimalNumberEx(sc.chPrev, sc.ch, sc.chNext)) {
				if (setIdChar.Contains(sc.ch)) {
					sc.ChangeState(SCE_WASM_IDENTIFIER);
				} else {
					sc.SetState(SCE_WASM_DEFAULT);
				}
			}
			break;

		case SCE_WASM_WORD:
			if (prefixLen == 0 && sc.ch == '.') {
				prefixLen = sc.LengthCurrent();
			} else if (!setIdChar.Contains(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (keywordLists[0]->InList(s)) {
					sc.ChangeState(SCE_WASM_KEYWORD);
				} else if (keywordLists[1]->InList(s)) {
					sc.ChangeState(SCE_WASM_TYPE);
				} else if (keywordLists[2]->InList(s)) {
					sc.ChangeState(SCE_WASM_INSTRUCTION);
				} else if (prefixLen != 0 && prefixLen < sizeof(s)) {
					s[prefixLen] = '\0';
					if (keywordLists[1]->InList(s)) {
						// instructions with type prefix
						sc.ChangeState(SCE_WASM_INSTRUCTION);
					}
				}
				sc.SetState(SCE_WASM_DEFAULT);
			}
			break;

		case SCE_WASM_IDENTIFIER:
			if (!setIdChar.Contains(sc.ch)) {
				sc.SetState(SCE_WASM_DEFAULT);
			}
			break;

		case SCE_WASM_STRING:
			if (sc.ch == '\\') {
				escSeq.resetEscapeState(sc.chNext);
				sc.SetState(SCE_WASM_ESCAPECHAR);
				sc.Forward();
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_WASM_DEFAULT);
			}
			break;

		case SCE_WASM_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				if (sc.ch == '\\') {
					escSeq.resetEscapeState(sc.chNext);
					sc.Forward();
				} else {
					sc.SetState(SCE_WASM_STRING);
					continue;
				}
			}
			break;

		case SCE_WASM_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_WASM_DEFAULT);
			}
			break;
		case SCE_WASM_COMMENTBLOCK:
			if (sc.Match(';', ')')) {
				sc.Forward();
				--commentLevel;
				if (commentLevel == 0) {
					sc.ForwardSetState(SCE_KOTLIN_DEFAULT);
				}
			} else if (sc.Match('(', ';')) {
				sc.Forward(2);
				++commentLevel;
			}
			break;
		}

		if (sc.state == SCE_WASM_DEFAULT) {
			if (sc.Match(';', ';')) {
				sc.SetState(SCE_WASM_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineComment = WASMLineStateMaskLineComment;
				}
			} else if (sc.Match('(', ';')) {
				sc.SetState(SCE_WASM_COMMENTBLOCK);
				sc.Forward();
				commentLevel = 1;
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_WASM_STRING);
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_WASM_NUMBER);
			} else if (IsLowerCase(sc.ch)) {
				sc.SetState(SCE_WASM_WORD);
				prefixLen = 0;
			} else if (isoperator(sc.ch)) {
				sc.SetState(SCE_WASM_OPERATOR);
			} else if (!isspacechar(sc.ch)) {
				sc.SetState(SCE_WASM_IDENTIFIER);
			}
		}

		if (visibleChars == 0 && !isspacechar(sc.ch)) {
			++visibleChars;
		}
		if (sc.atLineEnd) {
			const int lineState = commentLevel | lineStateLineComment;
			styler.SetLineState(sc.currentLine, lineState);
			visibleChars = 0;
			lineStateLineComment = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

void FoldWASMDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;

	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = styler.GetLineState(lineCurrent - 1) & WASMLineStateMaskLineComment;
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = styler.GetLineState(lineCurrent) & WASMLineStateMaskLineComment;
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		if (style == SCE_WASM_COMMENTBLOCK) {
			if (foldComment) {
				const int level = (ch == '(' && chNext == ';') ? 1 : ((ch == ';' && chNext == ')') ? -1 : 0);
				if (level != 0) {
					levelNext += level;
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
					styleNext = styler.StyleAt(i + 1);
				}
			}
		} else if (style == SCE_WASM_OPERATOR) {
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		}

		if (i == lineEndPos) {
			const int lineCommentNext = styler.GetLineState(lineCurrent + 1) & WASMLineStateMaskLineComment;
			if (foldComment && lineCommentCurrent) {
				if (!lineCommentPrev && lineCommentNext) {
					levelNext++;
				} else if (lineCommentPrev && !lineCommentNext) {
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
			lineCommentPrev = lineCommentCurrent;
			lineCommentCurrent = lineCommentNext;
		}
	}
}

}

LexerModule lmWASM(SCLEX_WASM, ColouriseWASMDoc, "wasm", FoldWASMDoc);
