// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for LLVM.

#include <cstdlib>
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
	int outerState = SCE_LLVM_DEFAULT;
	int digitsLeft = 0;

	// highlight any character as escape sequence.
	bool resetEscapeState(int state, int chNext) noexcept {
		if (IsEOLChar(chNext)) {
			return false;
		}
		outerState = state;
		digitsLeft = IsHexDigit(chNext) ? 2 : 1;
		return true;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

constexpr bool IsLLVMIdentifierChar(int ch) {
	return IsIdentifierChar(ch) || ch == '-' || ch == '.';
}

enum {
	LLVMLineStateMaskLineComment = 1, // line comment
};

bool IsLLVMTypeVar(LexAccessor &styler, Sci_Position pos) noexcept {
	while (styler[pos] != '=') {
		++pos;
	}
	++pos;
	while (isspacechar(styler.SafeGetCharAt(pos))) {
		++pos;
	}
	return styler.Match(pos, "type") && isspacechar(styler.SafeGetCharAt(pos + 4));
}

int CheckLLVMVarType(StyleContext &sc, int kwType) noexcept {
	int state = sc.state;
	if (kwType == SCE_LLVM_LABEL || sc.ch == ':') {
		state = SCE_LLVM_LABEL;
	} else {
		const int chNext = sc.GetNextNSChar();
		if (chNext == '(') {
			char s[8];
			sc.GetCurrent(s, sizeof(s));
			const bool quoted = state == SCE_LLVM_QUOTED_VARIABLE || state == SCE_LLVM_QUOTED_GLOBAL_VARIABLE;
			if (StrStartsWith(s + (quoted? 2 : 1), "llvm.")) {
				state = SCE_LLVM_INTRINSIC;
			} else {
				state = SCE_LLVM_FUNCTION;
			}
		} else if (chNext == '=' && IsLLVMTypeVar(sc.styler, sc.currentPos)) {
			// var = type {}
			state = SCE_LLVM_TYPE;
		}
	}
	return state;
}

void ColouriseLLVMDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineComment = 0;
	int kwType = SCE_LLVM_DEFAULT;
	int visibleChars = 0;
	bool noCharacterBefore = false;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);

	while (sc.More()) {
		switch (sc.state) {
		case SCE_LLVM_OPERATOR:
			sc.SetState(SCE_LLVM_DEFAULT);
			break;

		case SCE_LLVM_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_LLVM_DEFAULT);
			}
			break;

		case SCE_LLVM_IDENTIFIER:
		case SCE_LLVM_VARIABLE:
		case SCE_LLVM_GLOBAL_VARIABLE:
		case SCE_LLVM_COMDAT:
		case SCE_LLVM_METADATA:
		case SCE_LLVM_ATTRIBUTE_GROUP:
			if (sc.ch == '\\' && IsHexDigit(sc.chNext)) {
				sc.Forward();
			} else if (!IsLLVMIdentifierChar(sc.ch)) {
				int state = sc.state;
				if (sc.state == SCE_LLVM_VARIABLE || sc.state == SCE_LLVM_GLOBAL_VARIABLE) {
					state = CheckLLVMVarType(sc, kwType);
					kwType = SCE_LLVM_DEFAULT;
				} else if (sc.state == SCE_LLVM_IDENTIFIER) {
					char s[128];
					sc.GetCurrent(s, sizeof(s));
					kwType = SCE_LLVM_DEFAULT;
					if (keywordLists[0]->InList(s)) {
						state = SCE_LLVM_WORD;
					} else if (keywordLists[1]->InList(s)) {
						state = SCE_LLVM_WORD2;
						if (strcmp(s, "label") == 0) {
							kwType = SCE_LLVM_LABEL;
						}
					} else if (keywordLists[2]->InListPrefixed(s, '(')) {
						state = SCE_LLVM_ATTRIBUTE;
					} else if (keywordLists[3]->InList(s)) {
						state = SCE_LLVM_INSTRUCTION;
					} else if (sc.ch == ':' && noCharacterBefore) {
						state = SCE_LLVM_LABEL;
					}
				}
				sc.ChangeState(state);
				sc.SetState(SCE_LLVM_DEFAULT);
			}
			break;

		case SCE_LLVM_QUOTED_VARIABLE:
		case SCE_LLVM_QUOTED_GLOBAL_VARIABLE:
			if (sc.ch == '\\' && !IsEOLChar(sc.chNext)) {
				sc.Forward();
			} else if (sc.ch == '\"' || sc.atLineStart) {
				if (sc.ch == '\"') {
					sc.Forward();
				}

				const int state = CheckLLVMVarType(sc, kwType);
				sc.ChangeState(state);
				sc.SetState(SCE_LLVM_DEFAULT);
				kwType = SCE_LLVM_DEFAULT;
			}
			break;

		case SCE_LLVM_WORD2:
			if (!IsADigit(sc.ch)) {
				if (IsIdentifierChar(sc.ch)) {
					sc.ChangeState(SCE_LLVM_IDENTIFIER);
				} else {
					sc.SetState(SCE_LLVM_DEFAULT);
				}
			}
			break;

		case SCE_LLVM_STRING:
		case SCE_LLVM_META_STRING:
			if (sc.atLineStart) {
				sc.SetState(SCE_LLVM_DEFAULT);
			} else if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_LLVM_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_LLVM_DEFAULT);
			}
			break;

		case SCE_LLVM_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				const int outerState = escSeq.outerState;
				if (sc.ch == '\\' && escSeq.resetEscapeState(outerState, sc.chNext)) {
					sc.Forward();
				} else {
					sc.SetState(outerState);
					if (sc.ch == '\"') {
						sc.ForwardSetState(SCE_LLVM_DEFAULT);
					}
				}
			}
			break;

		case SCE_LLVM_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_LLVM_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_LLVM_DEFAULT) {
			if (sc.ch == ';') {
				sc.SetState(SCE_LLVM_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineComment = LLVMLineStateMaskLineComment;
				}
			} else if (sc.ch == '\"' || sc.Match('c', '\"')) {
				sc.SetState(SCE_LLVM_STRING);
				if (sc.ch == 'c') {
					sc.Forward();
				}
			} else if (sc.Match('!', '\"')) {
				sc.SetState(SCE_LLVM_META_STRING);
				sc.Forward();
			} else if (sc.ch == '@' || sc.ch == '%') {
				if (IsLLVMIdentifierChar(sc.chNext)) {
					sc.SetState((sc.ch == '@')? SCE_LLVM_GLOBAL_VARIABLE : SCE_LLVM_VARIABLE);
					sc.Forward();
				} else if (sc.chNext == '\"') {
					sc.SetState((sc.ch == '@')? SCE_LLVM_QUOTED_GLOBAL_VARIABLE : SCE_LLVM_QUOTED_VARIABLE);
					sc.Forward();
				} else {
					sc.SetState(SCE_LLVM_OPERATOR);
				}
			} else if (sc.ch == '!' && IsLLVMIdentifierChar(sc.chNext)) {
				sc.SetState(SCE_LLVM_METADATA);
			} else if (sc.ch == '$' && IsLLVMIdentifierChar(sc.chNext)) {
				sc.SetState(SCE_LLVM_COMDAT);
			} else if (sc.ch == '#' && IsLLVMIdentifierChar(sc.chNext)) {
				sc.SetState(SCE_LLVM_ATTRIBUTE_GROUP);
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_LLVM_NUMBER);
			} else if (sc.ch == 'i' && IsADigit(sc.chNext)) {
				// iN
				sc.SetState(SCE_LLVM_WORD2);
			} else if (sc.ch == 'x' && isspacechar(sc.chNext)) {
				sc.SetState(SCE_LLVM_OPERATOR);
			} else if (IsIdentifierStart(sc.ch)) {
				noCharacterBefore = visibleChars == 0;
				sc.SetState(SCE_LLVM_IDENTIFIER);
			} else if (isoperator(sc.ch)) {
				sc.SetState(SCE_LLVM_OPERATOR);
			}
		}

		if (visibleChars == 0 && !isspacechar(sc.ch)) {
			visibleChars = 1;
		}
		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, lineStateLineComment);
			lineStateLineComment = 0;
			visibleChars = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr int GetLineCommentState(int lineState) noexcept {
	return lineState & LLVMLineStateMaskLineComment;
}

void FoldLLVMDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList, Accessor &styler) {
	const int foldComment = styler.GetPropertyInt("fold.comment", 1);

	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = GetLineCommentState(styler.GetLineState(lineCurrent - 1));
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = GetLineCommentState(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int style = styler.StyleAt(i);

		if (style == SCE_LLVM_OPERATOR) {
			const char ch = styler[i];
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		}

		if (i == lineEndPos) {
			const int lineCommentNext = GetLineCommentState(styler.GetLineState(lineCurrent + 1));
			if (foldComment & lineCommentCurrent) {
				levelNext += lineCommentNext - lineCommentPrev;
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

LexerModule lmLLVM(SCLEX_LLVM, ColouriseLLVMDoc, "llvm", FoldLLVMDoc);
