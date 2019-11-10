// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Kotlin.

#include <cassert>
#include <cstring>

#include <vector>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"
#include "LexerUtils.h"

using namespace Scintilla;

namespace {

struct EscapeSequence {
	int outerState = SCE_KOTLIN_DEFAULT;
	int digitsLeft = 0;

	// highlight any character as escape sequence.
	bool resetEscapeState(int state, int chNext) noexcept {
		if (chNext == '\r' || chNext == '\n') {
			return false;
		}
		outerState = state;
		digitsLeft = (chNext == 'u') ? 5 : 1;
		return true;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

enum {
	MaxKotlinNestedStateCount = 4,
	KotlinLineStateMaskLineComment = (1 << 14), // line comment
	KotlinLineStateMaskImport = (1 << 15), // import
};

constexpr int PackState(int state) noexcept {
	switch (state) {
	case SCE_KOTLIN_STRING:
		return 1;
	case SCE_KOTLIN_RAWSTRING:
		return 2;
	default:
		return 0;
	}
}

constexpr int UnpackState(int state) noexcept  {
	switch (state) {
	case 1:
		return SCE_KOTLIN_STRING;
	case 2:
		return SCE_KOTLIN_RAWSTRING;
	default:
		return SCE_KOTLIN_DEFAULT;
	}
}

int PackNestedState(const std::vector<int>& nestedState) {
	return PackLineState<2, MaxKotlinNestedStateCount, PackState>(nestedState) << 16;
}

void UnpackNestedState(int lineState, int count, std::vector<int>& nestedState) {
	UnpackLineState<2, MaxKotlinNestedStateCount, UnpackState>(lineState, count, nestedState);
}

void ColouriseKotlinDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineComment = 0;
	int lineStateImport = 0;
	int commentLevel = 0;	// nested block comment level

	int kwType = SCE_KOTLIN_DEFAULT;
	int chBeforeIdentifier = 0;

	int curlyBrace = 0; // "${}"
	int variableOuter = SCE_KOTLIN_DEFAULT;	// variable inside string
	std::vector<int> nestedState;

	int visibleChars = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		8: curlyBrace
		6: commentLevel
		1: lineStateLineComment
		1: lineStateImport
		8: nestedState
		*/
		curlyBrace = lineState & 0xff;
		commentLevel = (lineState >> 8) & 0x3f;
		if (curlyBrace) {
			UnpackNestedState((lineState >> 16) & 0xff, curlyBrace, nestedState);
		}
	}
	if (startPos == 0 && sc.Match('#', '!')) {
		// Shell Shebang at beginning of file
		sc.SetState(SCE_KOTLIN_COMMENTLINE);
		lineStateLineComment = KotlinLineStateMaskLineComment;
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_KOTLIN_OPERATOR:
		case SCE_KOTLIN_OPERATOR2:
			sc.SetState(SCE_KOTLIN_DEFAULT);
			break;

		case SCE_KOTLIN_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_KOTLIN_DEFAULT);
			}
			break;

		case SCE_KOTLIN_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (keywordLists[0]->InList(s)) {
					sc.ChangeState(SCE_KOTLIN_WORD);
					if (strcmp(s, "import") == 0) {
						if (visibleChars == sc.LengthCurrent()) {
							lineStateImport = KotlinLineStateMaskImport;
						}
					} else if (strcmp(s, "break") == 0 || strcmp(s, "continue") == 0 || strcmp(s, "return") == 0
						|| strcmp(s, "this") == 0 || strcmp(s, "super") == 0) {
						kwType = SCE_KOTLIN_LABEL;
					} else if (((kwType != SCE_KOTLIN_ANNOTATION && kwType != SCE_KOTLIN_ENUM)
							&& (chBeforeIdentifier != ':' && strcmp(s, "class") == 0))
						|| strcmp(s, "typealias") == 0) {
						kwType = SCE_KOTLIN_CLASS;
					} else if (strcmp(s, "enum") == 0) {
						kwType = SCE_KOTLIN_ENUM;
					} else if (strcmp(s, "annotation") == 0) {
						kwType = SCE_KOTLIN_ANNOTATION;
					} else if (strcmp(s, "interface") == 0) {
						kwType = SCE_KOTLIN_INTERFACE;
					}
					if (kwType != SCE_KOTLIN_DEFAULT) {
						const int chNext = sc.GetNextNSChar();
						if (!((kwType == SCE_KOTLIN_LABEL && chNext == '@') || (kwType != SCE_KOTLIN_LABEL && IsIdentifierStart(chNext)))) {
							kwType = SCE_KOTLIN_DEFAULT;
						}
					}
				} else if (sc.ch == '@') {
					sc.ChangeState(SCE_KOTLIN_LABEL);
					sc.Forward();
				} else if (keywordLists[1]->InList(s)) {
					sc.ChangeState(SCE_KOTLIN_CLASS);
				} else if (keywordLists[2]->InList(s)) {
					sc.ChangeState(SCE_KOTLIN_INTERFACE);
				} else if (keywordLists[3]->InList(s)) {
					sc.ChangeState(SCE_KOTLIN_ENUM);
				} else {
					if (kwType != SCE_KOTLIN_DEFAULT && kwType != SCE_KOTLIN_LABEL) {
						sc.ChangeState(kwType);
					} else if (sc.GetNextNSChar() == '(') {
						sc.ChangeState(SCE_KOTLIN_FUNCTION);
					}
				}
				if (sc.state != SCE_KOTLIN_WORD) {
					kwType = SCE_KOTLIN_DEFAULT;
				}
				sc.SetState(SCE_KOTLIN_DEFAULT);
			}
			break;

		case SCE_KOTLIN_LABEL:
			if (!IsIdentifierCharEx(sc.ch)) {
				sc.SetState(SCE_KOTLIN_DEFAULT);
			}
			break;
		case SCE_KOTLIN_ANNOTATION:
			if (!IsWordCharEx(sc.ch)) {
				// highlight whole package.Name as annotation
				sc.SetState(SCE_KOTLIN_DEFAULT);
			}
			break;

		case SCE_KOTLIN_COMMENTLINE:
		case SCE_KOTLIN_COMMENTLINEDOC:
			if (sc.atLineStart) {
				sc.SetState(SCE_KOTLIN_DEFAULT);
			}
			break;
		case SCE_KOTLIN_COMMENTBLOCK:
			if (sc.Match('*', '/')) {
				sc.Forward();
				--commentLevel;
				if (commentLevel == 0) {
					sc.ForwardSetState(SCE_KOTLIN_DEFAULT);
				}
			} else if (sc.Match('/', '*')) {
				sc.Forward(2);
				++commentLevel;
			}
			break;
		case SCE_KOTLIN_COMMENTBLOCKDOC:
			if (sc.ch == '@' && IsLowerCase(sc.chNext) && isspacechar(sc.chPrev)) {
				sc.SetState(SCE_KOTLIN_COMMENTDOCWORD);
			} else if (sc.Match('*', '/')) {
				sc.Forward();
				--commentLevel;
				if (commentLevel == 0) {
					sc.ForwardSetState(SCE_KOTLIN_DEFAULT);
				}
			} else if (sc.Match('/', '*')) {
				sc.Forward(2);
				++commentLevel;
			}
			break;
		case SCE_KOTLIN_COMMENTDOCWORD:
			if (!IsLowerCase(sc.ch)) {
				sc.SetState(SCE_KOTLIN_COMMENTBLOCKDOC);
				continue;
			}
			break;

		case SCE_KOTLIN_STRING:
		case SCE_KOTLIN_RAWSTRING:
			if (sc.state == SCE_KOTLIN_STRING && sc.atLineStart) {
				sc.SetState(SCE_KOTLIN_DEFAULT);
			} else if (sc.state == SCE_KOTLIN_STRING && sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_KOTLIN_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '$' && IsIdentifierStartEx(sc.chNext)) {
				variableOuter = sc.state;
				sc.SetState(SCE_KOTLIN_VARIABLE);
			} else if (sc.Match('$', '{')) {
				++curlyBrace;
				nestedState.push_back(sc.state);
				sc.SetState(SCE_KOTLIN_OPERATOR2);
				sc.Forward();
			} else if (curlyBrace && sc.ch == '}') {
				const int outerState = nestedState.empty()? SCE_KOTLIN_DEFAULT : nestedState.back();
				if (!nestedState.empty()) {
					nestedState.pop_back();
				}
				--curlyBrace;
				sc.SetState(SCE_KOTLIN_OPERATOR2);
				sc.ForwardSetState(outerState);
				continue;
			} else if (sc.ch == '\"' && (sc.state == SCE_KOTLIN_STRING || sc.Match(R"(""")"))) {
				if (sc.state == SCE_KOTLIN_RAWSTRING) {
					sc.Forward(2);
					sc.SetState(SCE_KOTLIN_RAWSTRINGEND);
				}
				sc.ForwardSetState(SCE_KOTLIN_DEFAULT);
			}
			break;

		case SCE_KOTLIN_CHARACTER:
			if (sc.atLineStart) {
				sc.SetState(SCE_KOTLIN_DEFAULT);
			} else if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_KOTLIN_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_KOTLIN_DEFAULT);
			}
			break;
		case SCE_KOTLIN_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				const int outerState = escSeq.outerState;
				if (outerState == SCE_KOTLIN_STRING) {
					if (sc.ch == '\\' && escSeq.resetEscapeState(outerState, sc.chNext)) {
						sc.Forward();
					} else {
						sc.SetState(outerState);
						if (sc.ch == '\"') {
							sc.ForwardSetState(SCE_KOTLIN_DEFAULT);
						}
					}
				} else {
					sc.SetState(outerState);
					if (sc.ch == '\'') {
						sc.ForwardSetState(SCE_KOTLIN_DEFAULT);
					}
				}
			}
			break;
		case SCE_KOTLIN_VARIABLE:
			if (!IsIdentifierCharEx(sc.ch)) {
				sc.SetState(variableOuter);
				continue;
			}
			break;

		case SCE_KOTLIN_BACKTICKS:
			if (sc.atLineStart) {
				sc.SetState(SCE_KOTLIN_DEFAULT);
			} else if (sc.ch == '`') {
				sc.ForwardSetState(SCE_KOTLIN_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_KOTLIN_DEFAULT) {
			if (sc.Match('/', '/')) {
				const int chNext = sc.GetRelative(2);
				sc.SetState((chNext == '!' || chNext == '/') ? SCE_KOTLIN_COMMENTLINEDOC : SCE_KOTLIN_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineComment = KotlinLineStateMaskLineComment;
				}
			} else if (sc.Match('/', '*')) {
				const int chNext = sc.GetRelative(2);
				sc.SetState((chNext == '*' || chNext == '!') ? SCE_KOTLIN_COMMENTBLOCKDOC : SCE_KOTLIN_COMMENTBLOCK);
				sc.Forward();
				commentLevel = 1;
			} else if (sc.Match(R"(""")")) {
				sc.SetState(SCE_KOTLIN_RAWSTRINGSTART);
				sc.ForwardSetState(SCE_KOTLIN_RAWSTRING);
				sc.Forward();
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_KOTLIN_STRING);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_KOTLIN_CHARACTER);
			} else if (IsADigit(sc.ch)) {
				sc.SetState(SCE_KOTLIN_NUMBER);
			} else if (sc.ch == '@' && IsIdentifierStartEx(sc.chNext)) {
				sc.SetState((kwType == SCE_KOTLIN_LABEL) ? SCE_KOTLIN_LABEL : SCE_KOTLIN_ANNOTATION);
				kwType = SCE_KOTLIN_DEFAULT;
			} else if (sc.ch == '`') {
				sc.SetState(SCE_KOTLIN_BACKTICKS);
			} else if (IsIdentifierStartEx(sc.ch)) {
				chBeforeIdentifier = sc.chPrev;
				sc.SetState(SCE_KOTLIN_IDENTIFIER);
			} else if (isoperator(sc.ch)) {
				sc.SetState(curlyBrace ? SCE_KOTLIN_OPERATOR2 : SCE_KOTLIN_OPERATOR);
				if (curlyBrace) {
					if (sc.ch == '{') {
						++curlyBrace;
						nestedState.push_back(SCE_KOTLIN_DEFAULT);
					} else if (sc.ch == '}') {
						--curlyBrace;
						const int outerState = nestedState.empty() ? SCE_KOTLIN_DEFAULT: nestedState.back();
						if (!nestedState.empty()) {
							nestedState.pop_back();
						}
						sc.ForwardSetState(outerState);
						continue;
					}
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			int lineState = curlyBrace | (commentLevel << 8) | lineStateLineComment | lineStateImport;
			if (curlyBrace) {
				lineState |= PackNestedState(nestedState);
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineComment = 0;
			lineStateImport = 0;
			visibleChars = 0;
			kwType = SCE_KOTLIN_DEFAULT;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int packageImport;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & KotlinLineStateMaskLineComment),
		packageImport(lineState & KotlinLineStateMaskImport) {
	}
};

constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_KOTLIN_COMMENTBLOCK || style == SCE_KOTLIN_COMMENTBLOCKDOC;
}

void FoldKotlinDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList, Accessor &styler) {
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

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		if (IsStreamCommentStyle(style)) {
			if (foldComment) {
				const int level = (ch == '/' && chNext == '*') ? 1 : ((ch == '*' && chNext == '/') ? -1 : 0);
				if (level != 0) {
					levelNext += level;
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
					styleNext = styler.StyleAt(i + 1);
				}
			}
		} else if (style == SCE_KOTLIN_RAWSTRINGSTART) {
			levelNext++;
		} else if (style == SCE_KOTLIN_RAWSTRINGEND) {
			levelNext--;
		} else if (style == SCE_KOTLIN_OPERATOR) {
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
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
			} else if (foldCurrent.packageImport) {
				if (!foldPrev.packageImport && foldNext.packageImport) {
					levelNext++;
				} else if (foldPrev.packageImport && !foldNext.packageImport) {
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

LexerModule lmKotlin(SCLEX_KOTLIN, ColouriseKotlinDoc, "kotlin", FoldKotlinDoc);
