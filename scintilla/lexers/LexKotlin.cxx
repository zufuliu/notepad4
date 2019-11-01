// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Kotlin.

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

constexpr bool IsKotlinNumber(int chPrev, int ch, int chNext) noexcept {
	return IsIdentifierChar(ch)
		|| ((ch == '+' || ch == '-') && (chPrev == 'E' || chPrev == 'e'))
		|| (ch == '.' && chNext != '.');
}

enum {
	KotlinLineStateMaskLineComment = (1 << 24), // line comment
	KotlinLineStateMaskImport = (1 << 25), // import
};

void ColouriseKotlinDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineComment = 0;
	int lineStateImport = 0;

	int kwType = SCE_KOTLIN_DEFAULT;
	int chBeforeIdentifier = 0;

	int commentLevel = 0;	// nested block comment level
	int stringTemplate = SCE_KOTLIN_DEFAULT;
	int curlyBrace = 0; // "${}"

	int visibleChars = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		curlyBrace = lineState & 0xff;
		commentLevel = (lineState >> 8) & 0xff;
		if (curlyBrace) {
			stringTemplate = SCE_KOTLIN_RAWSTRING;
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
			if (!IsKotlinNumber(sc.chPrev, sc.ch, sc.chNext)) {
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
			if (sc.atLineStart) {
				sc.SetState(SCE_KOTLIN_DEFAULT);
			} else if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_KOTLIN_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.Match('$', '{')) {
				stringTemplate = sc.state;
				curlyBrace = 1;
				sc.SetState(SCE_KOTLIN_OPERATOR2);
				sc.Forward();
			} else if (sc.ch == '$' && IsIdentifierStartEx(sc.chNext)) {
				stringTemplate = sc.state;
				sc.SetState(SCE_KOTLIN_VARIABLE);
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_KOTLIN_DEFAULT);
			}
			break;
		case SCE_KOTLIN_RAWSTRING:
			if (sc.Match('$', '{')) {
				stringTemplate = sc.state;
				curlyBrace = 1;
				sc.SetState(SCE_KOTLIN_OPERATOR2);
				sc.Forward();
			} else if (sc.ch == '$' && IsIdentifierStartEx(sc.chNext)) {
				stringTemplate = sc.state;
				sc.SetState(SCE_KOTLIN_VARIABLE);
			} else if (sc.Match(R"(""")")) {
				sc.Forward(2);
				sc.SetState(SCE_KOTLIN_RAWSTRINGEND);
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
				sc.SetState(stringTemplate);
				stringTemplate = SCE_KOTLIN_DEFAULT;
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
				if (stringTemplate == SCE_KOTLIN_DEFAULT) {
					sc.SetState(SCE_KOTLIN_OPERATOR);
				} else {
					sc.SetState(SCE_KOTLIN_OPERATOR2);
					if (sc.ch == '{') {
						++curlyBrace;
					} else if (sc.ch == '}') {
						--curlyBrace;
						if (curlyBrace == 0) {
							sc.ForwardSetState(stringTemplate);
							stringTemplate = SCE_KOTLIN_DEFAULT;
							continue;
						}
					}
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			if (curlyBrace != 0 && stringTemplate == SCE_KOTLIN_STRING) {
				curlyBrace = 0;
				stringTemplate = SCE_KOTLIN_DEFAULT;
				sc.SetState(SCE_KOTLIN_STRING);
			}

			const int lineState = curlyBrace | (commentLevel << 8) | lineStateLineComment | lineStateImport;
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
	Sci_PositionU lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) -1;

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
			lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) -1;
			levelCurrent = levelNext;
			foldPrev = foldCurrent;
			foldCurrent = foldNext;
		}
	}
}

}

LexerModule lmKotlin(SCLEX_KOTLIN, ColouriseKotlinDoc, "kotlin", FoldKotlinDoc);
