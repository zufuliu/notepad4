// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Haxe.

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
#include "StringUtils.h"
#include "LexerModule.h"
#include "LexerUtils.h"

using namespace Scintilla;

namespace {

struct EscapeSequence {
	int outerState = SCE_HAXE_DEFAULT;
	int digitsLeft = 0;
	int numBase = 0;

	// highlight any character as escape sequence, no highlight for hex in '\u{hex}'.
	bool resetEscapeState(int state, int chNext) noexcept {
		outerState = state;
		digitsLeft = 1;
		numBase = 16;
		if (chNext == 'u') {
			digitsLeft = 5;
		} else if (chNext == 'x') {
			digitsLeft = 3;
		} else if (IsOctalDigit(chNext)) {
			digitsLeft = 3;
			numBase = 8;
		}
		return true;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsADigit(ch, numBase);
	}
};

enum {
	HaxeLineStateMaskLineComment = 1, // line comment
	HaxeLineStateMaskImport = 1 << 1, // import
};

static_assert(DefaultNestedStateBaseStyle + 1 == SCE_HAXE_STRINGSQ);

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_HAXE_TASKMARKER;
}

void ColouriseHaxeDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineType = 0;
	bool insideRegexRange = false; // inside regex character range []

	int kwType = SCE_HAXE_DEFAULT;
	int chBeforeIdentifier = 0;
	std::vector<int> nestedState; // string interpolation "${}"

	int visibleChars = 0;
	int visibleCharsBefore = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		2: lineStateLineType
		3: nestedState count
		3*4: nestedState
		*/
		lineState >>= 8;
		if (lineState) {
			UnpackLineState(lineState, nestedState);
		}
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_HAXE_OPERATOR:
		case SCE_HAXE_OPERATOR2:
			sc.SetState(SCE_HAXE_DEFAULT);
			break;

		case SCE_HAXE_NUMBER:
			if (!IsDecimalNumberEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_HAXE_DEFAULT);
			}
			break;

		case SCE_HAXE_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (s[0] == '#') {
					if (keywordLists[1]->InList(s + 1)) {
						sc.ChangeState(SCE_HAXE_PREPROCESSOR);
					}
				} else if (keywordLists[0]->InList(s)) {
					sc.ChangeState(SCE_HAXE_WORD);
					if (StrEqual(s, "import")) {
						if (visibleChars == sc.LengthCurrent()) {
							lineStateLineType = HaxeLineStateMaskImport;
						}
					} else if (StrEqualsAny(s, "class", "new", "extends", "abstract", "typedef")) {
						if (kwType != SCE_HAXE_ENUM) {
							kwType = SCE_HAXE_CLASS;
						}
					} else if (StrEqualsAny(s, "interface", "implements")) {
						kwType = SCE_HAXE_INTERFACE;
					} else if (StrEqual(s, "enum")) {
						kwType = SCE_HAXE_ENUM;
					} else if (StrEqual(s, "function")) {
						kwType = SCE_HAXE_FUNCTION_DEFINE;
					}
					if (kwType != SCE_HAXE_DEFAULT) {
						const int chNext = sc.GetDocNextChar();
						if (!IsIdentifierStartEx(chNext)) {
							kwType = SCE_HAXE_DEFAULT;
						}
					}
				} else if (keywordLists[2]->InList(s)) {
					sc.ChangeState(SCE_HAXE_CLASS);
				} else if (keywordLists[3]->InList(s)) {
					sc.ChangeState(SCE_HAXE_INTERFACE);
				} else if (keywordLists[4]->InList(s)) {
					sc.ChangeState(SCE_HAXE_ENUM);
				} else if (keywordLists[5]->InList(s)) {
					sc.ChangeState(SCE_HAXE_CONSTANT);
				} else if (sc.ch != '.') {
					if (kwType != SCE_HAXE_DEFAULT) {
						sc.ChangeState(kwType);
					} else {
						const int chNext = sc.GetDocNextChar();
						if (chNext == '(') {
							sc.ChangeState(SCE_HAXE_FUNCTION);
						} else if (sc.Match('[', ']')
							|| (chBeforeIdentifier == '<' && (chNext == '>' || chNext == '<'))) {
							// type[]
							// type<type>
							// type<type<type>>
							sc.ChangeState(SCE_HAXE_CLASS);
						}
					}
				}
				if (sc.state != SCE_HAXE_WORD && sc.ch != '.') {
					kwType = SCE_HAXE_DEFAULT;
				}
				sc.SetState(SCE_HAXE_DEFAULT);
			}
			break;

		case SCE_HAXE_MATADATA:
			if (sc.ch == '.') {
				sc.SetState(SCE_HAXE_OPERATOR);
				sc.ForwardSetState(SCE_HAXE_MATADATA);
				continue;
			}
			if (!IsIdentifierCharEx(sc.ch)) {
				sc.SetState(SCE_HAXE_DEFAULT);
			}
			break;

		case SCE_HAXE_VARIABLE:
			if (!IsIdentifierCharEx(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_HAXE_STRINGDQ:
		case SCE_HAXE_STRINGSQ:
			if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_HAXE_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.state == SCE_HAXE_STRINGSQ && sc.ch == '$') {
				if (sc.chNext == '{') {
					nestedState.push_back(sc.state);
					sc.SetState(SCE_HAXE_OPERATOR2);
					sc.Forward();
				} else if (IsIdentifierStartEx(sc.chNext)) {
					escSeq.outerState = sc.state;
					sc.SetState(SCE_HAXE_VARIABLE);
				}
			} else if ((sc.state == SCE_HAXE_STRINGDQ && sc.ch == '"')
				|| (sc.state == SCE_HAXE_STRINGSQ && sc.ch == '\'')) {
				sc.SetState(SCE_HAXE_STRING_END);
				sc.ForwardSetState(SCE_HAXE_DEFAULT);
			}
			break;

		case SCE_HAXE_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_HAXE_REGEX:
			if (sc.ch == '\\') {
				sc.Forward();
			} else if (sc.ch == '[' || sc.ch == ']') {
				insideRegexRange = sc.ch == '[';
			} else if (sc.ch == '/' && !insideRegexRange) {
				sc.Forward();
				// regex flags
				while (IsLowerCase(sc.ch)) {
					sc.Forward();
				}
				sc.SetState(SCE_HAXE_DEFAULT);
			}
			break;

		case SCE_HAXE_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_HAXE_DEFAULT);
			} else {
				HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_HAXE_TASKMARKER);
			}
			break;

		case SCE_HAXE_COMMENTBLOCK:
		case SCE_HAXE_COMMENTBLOCKDOC:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_HAXE_DEFAULT);
			} else if (sc.state == SCE_HAXE_COMMENTBLOCKDOC && sc.ch == '@' && IsAlpha(sc.chNext) && IsCommentTagPrev(sc.chPrev)) {
				sc.SetState(SCE_HAXE_COMMENTTAGAT);
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_HAXE_TASKMARKER)) {
				continue;
			}
			break;

		case SCE_HAXE_COMMENTTAGAT:
			if (!IsAlpha(sc.ch)) {
				sc.SetState(SCE_HAXE_COMMENTBLOCKDOC);
				continue;
			}
			break;
		}

		if (sc.state == SCE_HAXE_DEFAULT) {
			if (sc.Match('/', '/')) {
				visibleCharsBefore = visibleChars;
				sc.SetState(SCE_HAXE_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineType = HaxeLineStateMaskLineComment;
				}
			} else if (sc.Match('/', '*')) {
				visibleCharsBefore = visibleChars;
				sc.SetState(SCE_HAXE_COMMENTBLOCK);
				sc.Forward(2);
				if (sc.ch == '*' && sc.chNext != '*') {
					sc.ChangeState(SCE_HAXE_COMMENTBLOCKDOC);
				}
				continue;
			} else if (sc.Match('~', '/')) {
				insideRegexRange = false;
				sc.SetState(SCE_HAXE_REGEX);
				sc.Forward();
			} else if (sc.ch == '\"' || sc.ch == '\'') {
				const int state = (sc.ch == '\"') ? SCE_HAXE_STRINGDQ : SCE_HAXE_STRINGSQ;
				sc.SetState(SCE_HAXE_STRING_BEGIN);
				sc.ForwardSetState(state);
				continue;
			} else if (IsNumberStartEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_HAXE_NUMBER);
			} else if (IsIdentifierStartEx(sc.ch) || (sc.ch == '#' && (sc.chNext == 'e' || sc.chNext == 'i'))) {
				if (sc.chPrev != '.') {
					chBeforeIdentifier = sc.chPrev;
				}
				sc.SetState(SCE_HAXE_IDENTIFIER);
			} else if (sc.ch == '@' && (sc.chNext == ':' || IsIdentifierStartEx(sc.chNext))) {
				sc.SetState(SCE_HAXE_MATADATA);
				if (sc.chNext == ':') {
					sc.Forward();
				}
			} else if (sc.ch == '$' && IsIdentifierStartEx(sc.chNext)) {
				escSeq.outerState = sc.state;
				sc.SetState(SCE_HAXE_VARIABLE);
			} else if (isoperator(sc.ch)) {
				const bool interpolating = !nestedState.empty();
				sc.SetState(interpolating ? SCE_HAXE_OPERATOR2 : SCE_HAXE_OPERATOR);
				if (interpolating) {
					if (sc.ch == '{') {
						nestedState.push_back(SCE_HAXE_DEFAULT);
					} else if (sc.ch == '}') {
						const int outerState = TakeAndPop(nestedState);
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
			int lineState = lineStateLineType;
			if (!nestedState.empty()) {
				lineState |= PackLineState(nestedState) << 8;
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineType = 0;
			visibleChars = 0;
			visibleCharsBefore = 0;
			kwType = SCE_HAXE_DEFAULT;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int packageImport;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & HaxeLineStateMaskLineComment),
		packageImport((lineState >> 1) & 1) {
	}
};

constexpr bool IsInnerStyle(int style) noexcept {
	return style == SCE_HAXE_COMMENTTAGAT || style == SCE_HAXE_TASKMARKER;
}

void FoldHaxeDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_HAXE_OPERATOR, SCE_HAXE_TASKMARKER, SCE_HAXE_PREPROCESSOR);
		if (bracePos) {
			startPos = bracePos + 1; // skip the brace
		}
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;
	int visibleChars = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		switch (style) {
		case SCE_HAXE_COMMENTBLOCK:
		case SCE_HAXE_COMMENTBLOCKDOC:
		case SCE_HAXE_REGEX:
			if (style != stylePrev && !IsInnerStyle(stylePrev)) {
				levelNext++;
			} else if (style != styleNext && !IsInnerStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_HAXE_STRING_BEGIN:
			levelNext++;
			break;

		case SCE_HAXE_STRING_END:
			levelNext--;
			break;

		case SCE_HAXE_OPERATOR:
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
			break;

		case SCE_HAXE_PREPROCESSOR:
			if (ch == '#') {
				if (chNext == 'i' && styler.SafeGetCharAt(i + 2) == 'f') {
					levelNext++;
				} else if (chNext == 'e' && styler.Match(i + 1, "end")) {
					levelNext--;
				}
			}
			break;
		}

		if (visibleChars == 0 && !IsSpaceEquiv(style)) {
			++visibleChars;
		}
		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.packageImport) {
				levelNext += foldNext.packageImport - foldPrev.packageImport;
			} else if (visibleChars) {
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_HAXE_OPERATOR, SCE_HAXE_TASKMARKER, SCE_HAXE_PREPROCESSOR);
				if (bracePos) {
					levelNext++;
					i = bracePos; // skip the brace
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
			lineEndPos = sci::min(lineStartNext, endPos) - 1;
			levelCurrent = levelNext;
			foldPrev = foldCurrent;
			foldCurrent = foldNext;
			visibleChars = 0;
		}
	}
}

}

LexerModule lmHaxe(SCLEX_HAXE, ColouriseHaxeDoc, "haxe", FoldHaxeDoc);
