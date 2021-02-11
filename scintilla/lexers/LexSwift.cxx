// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Swift.

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

enum {
	SwiftLineStateMaskLineComment = 1,		// line comment
	SwiftLineStateMaskImport = (1 << 1),	// import
};

static_assert(DefaultNestedStateBaseStyle + 1 == SCE_SWIFT_STRING);
static_assert(DefaultNestedStateBaseStyle + 2 == SCE_SWIFT_TRIPLE_STRING);
static_assert(DefaultNestedStateBaseStyle + 3 == SCE_SWIFT_STRING_ED);
static_assert(DefaultNestedStateBaseStyle + 4 == SCE_SWIFT_TRIPLE_STRING_ED);

constexpr bool IsEscapeSequence(int ch) noexcept {
	return AnyOf(ch, '0', '\\', 't', 'n', 'r', '"', '\'', 'u');
}

enum class DelimiterCheck {
	Start,
	End,
	Escape,
};

bool CheckSwiftStringDelimiter(LexAccessor &styler, Sci_PositionU pos, DelimiterCheck what, int &delimiterCount) noexcept {
	++pos; // first '#'
	int count = 1;
	char ch;
	while ((ch = styler.SafeGetCharAt(pos)) == '#') {
		++count;
		++pos;
	}
	if (what == DelimiterCheck::End) {
		return count == delimiterCount;
	}

	if (what == DelimiterCheck::Start) {
		if (ch == '\"') {
			delimiterCount = count;
			return true;
		}
	} else {
		if (count == delimiterCount && (ch == '(' || IsEscapeSequence(ch))) {
			return true;
		}
	}
	return false;
}

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_SWIFT_TASKMARKER;
}

void ColouriseSwiftDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineType = 0;
	int commentLevel = 0;	// nested block comment level

	int kwType = SCE_SWIFT_DEFAULT;
	int chBeforeIdentifier = 0;

	int delimiterCount = 0;	// count of '#'
	std::vector<int> delimiters;	// nested extended string delimiters in string interpolation
	std::vector<int> nestedState;	// string interpolation "\()"

	int visibleChars = 0;
	int visibleCharsBefore = 0;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		2: lineStateLineType
		6: commentLevel
		8: delimiterCount
		3: nestedState count
		3*4: nestedState
		*/
		commentLevel = (lineState >> 2) & 0x3f;
		delimiterCount = (lineState >> 8) & 0xff;
		lineState >>= 16;
		if (lineState) {
			UnpackLineState(lineState, nestedState);
		}
		if (delimiterCount) {
			// TODO: backtrack to the line without string interpolation.
			delimiters.push_back(delimiterCount);
		}
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_SWIFT_OPERATOR:
		case SCE_SWIFT_OPERATOR2:
			sc.SetState(SCE_SWIFT_DEFAULT);
			break;

		case SCE_SWIFT_NUMBER:
			if (!IsDecimalNumberEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_SWIFT_DEFAULT);
			}
			break;

		case SCE_SWIFT_IDENTIFIER:
		case SCE_SWIFT_IDENTIFIER_BT:
		case SCE_SWIFT_ATTRIBUTE:
		case SCE_SWIFT_DIRECTIVE:
		case SCE_SWIFT_VARIABLE:
			if (!IsIdentifierCharEx(sc.ch)) {
				if (sc.state == SCE_SWIFT_IDENTIFIER_BT) {
					if (sc.ch == '`') {
						sc.Forward();
					}
				} else if (sc.state == SCE_SWIFT_IDENTIFIER || sc.state == SCE_SWIFT_DIRECTIVE) {
					char s[128];
					sc.GetCurrent(s, sizeof(s));
					if (sc.state == SCE_SWIFT_DIRECTIVE) {
						if (!keywordLists[1]->InListPrefixed(s + 1, '(')) {
							// required for code folding
							sc.ChangeState(SCE_SWIFT_DEFAULT);
						}
					} else if (keywordLists[0]->InList(s)) {
						sc.ChangeState(SCE_SWIFT_WORD);
						if (StrEqual(s, "import")) {
							if (visibleChars == sc.LengthCurrent()) {
								lineStateLineType = SwiftLineStateMaskImport;
							}
						} else if (StrEqualsAny(s, "class", "extension", "typealias", "as", "is")) {
							kwType = SCE_SWIFT_CLASS;
						} else if (StrEqual(s, "struct")) {
							kwType = SCE_SWIFT_STRUCT;
						} else if (StrEqual(s, "protocol")) {
							kwType = SCE_SWIFT_PROTOCOL;
						} else if (StrEqual(s, "enum")) {
							kwType = SCE_SWIFT_ENUM;
						} else if (StrEqual(s, "func")) {
							kwType = SCE_SWIFT_FUNCTION_DEFINE;
						} else if (StrEqualsAny(s, "break", "continue")) {
							kwType = SCE_SWIFT_LABEL;
						}
						if (kwType != SCE_SWIFT_DEFAULT) {
							const int chNext = sc.GetLineNextChar();
							if (!IsIdentifierStartEx(chNext)) {
								kwType = SCE_SWIFT_DEFAULT;
							}
						}
					} else if (keywordLists[3]->InList(s)) {
						sc.ChangeState(SCE_SWIFT_CLASS);
					} else if (keywordLists[4]->InList(s)) {
						sc.ChangeState(SCE_SWIFT_STRUCT);
					} else if (keywordLists[5]->InList(s)) {
						sc.ChangeState(SCE_SWIFT_PROTOCOL);
					} else if (keywordLists[6]->InList(s)) {
						sc.ChangeState(SCE_SWIFT_ENUM);
					}
				}
				if (sc.state == SCE_SWIFT_IDENTIFIER || sc.state == SCE_SWIFT_IDENTIFIER_BT) {
					if (sc.ch == ':') {
						if (visibleChars == sc.LengthCurrent()) {
							const int chNext = sc.GetLineNextChar(true);
							if (IsJumpLabelNextChar(chNext)) {
								sc.ChangeState(SCE_SWIFT_LABEL);
							}
						}
					} else if (sc.ch != '.') {
						if (kwType != SCE_SWIFT_DEFAULT) {
							sc.ChangeState(kwType);
						} else {
							const int chNext = sc.GetDocNextChar(sc.ch == '?');
							if (chNext == '(') {
								sc.ChangeState(SCE_SWIFT_FUNCTION);
							} else if ((chBeforeIdentifier == '<' && (chNext == '>' || chNext == '<'))
								|| (chBeforeIdentifier == '[' && (sc.ch == ']' && AnyOf(sc.chNext, '(', ']')))) {
								// type<type>
								// type<type?>
								// type<type<type>>
								// [type]()
								// [[type]]()
								sc.ChangeState(SCE_SWIFT_CLASS);
							}
						}
					}
				}
				if (sc.state != SCE_SWIFT_WORD && sc.ch != '.') {
					kwType = SCE_SWIFT_DEFAULT;
				}
				sc.SetState(SCE_SWIFT_DEFAULT);
			}
			break;

		case SCE_SWIFT_COMMENTLINE:
		case SCE_SWIFT_COMMENTLINEDOC:
			if (sc.atLineStart) {
				sc.SetState(SCE_SWIFT_DEFAULT);
			} else {
				HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_SWIFT_TASKMARKER);
			}
			break;

		case SCE_SWIFT_COMMENTBLOCK:
		case SCE_SWIFT_COMMENTBLOCKDOC:
			if (sc.Match('*', '/')) {
				sc.Forward();
				--commentLevel;
				if (commentLevel == 0) {
					sc.ForwardSetState(SCE_SWIFT_DEFAULT);
				}
			} else if (sc.Match('/', '*')) {
				sc.Forward();
				++commentLevel;
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_SWIFT_TASKMARKER)) {
				continue;
			}
			break;

		case SCE_SWIFT_STRING:
		case SCE_SWIFT_TRIPLE_STRING:
			if (sc.state == SCE_SWIFT_STRING && sc.atLineStart) {
				sc.SetState(SCE_SWIFT_DEFAULT);
			} else if (sc.ch == '\\') {
				if (sc.chNext == '(') {
					nestedState.push_back(sc.state);
					sc.SetState(SCE_SWIFT_OPERATOR2);
					sc.Forward();
				} else if (IsEscapeSequence(sc.chNext)) {
					const int state = sc.state;
					sc.SetState(SCE_SWIFT_ESCAPECHAR);
					sc.Forward();
					sc.ForwardSetState(state);
					continue;
				}
			} else if (sc.ch == '"' && (sc.state == SCE_SWIFT_STRING
				|| (sc.state == SCE_SWIFT_TRIPLE_STRING && sc.MatchNext('"', '"')))) {
				if (sc.state == SCE_SWIFT_TRIPLE_STRING) {
					sc.SetState(SCE_SWIFT_TRIPLE_STRINGEND);
					sc.Forward(2);
				}
				sc.ForwardSetState(SCE_SWIFT_DEFAULT);
			}
			break;

		case SCE_SWIFT_STRING_ED:
		case SCE_SWIFT_TRIPLE_STRING_ED:
			if (sc.state == SCE_SWIFT_STRING_ED && sc.atLineStart) {
				sc.SetState(SCE_SWIFT_DEFAULT);
			} else if (sc.Match('\\', '#')) {
				const int state = sc.state;
				sc.SetState(SCE_SWIFT_ESCAPECHAR);
				sc.Forward();
				if (CheckSwiftStringDelimiter(styler, sc.currentPos, DelimiterCheck::Escape, delimiterCount)) {
					sc.Forward(delimiterCount);
					if (sc.ch == '(') {
						nestedState.push_back(state);
						sc.SetState(SCE_SWIFT_OPERATOR2);
					}
				}
				if (sc.state == SCE_SWIFT_ESCAPECHAR){
					sc.ForwardSetState(state);
					continue;
				}
			} else if (sc.ch == '"' && ((sc.state == SCE_SWIFT_STRING_ED && sc.chNext == '#')
				|| (sc.state == SCE_SWIFT_TRIPLE_STRING_ED && sc.MatchNext('"', '"', '#')))) {
				const int offset = (sc.state == SCE_SWIFT_STRING_ED) ? 1 : 3;
				if (CheckSwiftStringDelimiter(styler, sc.currentPos + offset, DelimiterCheck::End, delimiterCount)) {
					if (sc.state == SCE_SWIFT_TRIPLE_STRING_ED) {
						sc.SetState(SCE_SWIFT_TRIPLE_STRING_EDEND);
					}
					sc.Forward(delimiterCount + offset);
					sc.SetState(SCE_SWIFT_DEFAULT);
					delimiterCount = TryPopAndPeek(delimiters);
				} else {
					sc.Forward(offset);
				}
			}
			break;
		}

		if (sc.state == SCE_SWIFT_DEFAULT) {
			if (sc.ch == '/' && (sc.chNext == '/' || sc.chNext == '*')) {
				visibleCharsBefore = visibleChars;
				const int chNext = sc.chNext;
				sc.SetState((chNext == '/') ? SCE_SWIFT_COMMENTLINE : SCE_SWIFT_COMMENTBLOCK);
				sc.Forward(2);
				if (sc.ch == ':' || sc.ch == '!' || (sc.ch == chNext && sc.chNext != chNext)) {
					sc.ChangeState((chNext == '/') ? SCE_SWIFT_COMMENTLINEDOC : SCE_SWIFT_COMMENTBLOCKDOC);
				}
				if (chNext == '/') {
					if (visibleChars == 0) {
						lineStateLineType = SwiftLineStateMaskLineComment;
					}
				} else {
					commentLevel = 1;
				}
				continue;
			}
			if (sc.ch == '"') {
				if (sc.MatchNext('"', '"')) {
					sc.SetState(SCE_SWIFT_TRIPLE_STRINGSTART);
					sc.Forward(2);
					sc.ForwardSetState(SCE_SWIFT_TRIPLE_STRING);
					continue;
				}
				sc.SetState(SCE_SWIFT_STRING);
			} else if (IsNumberStartEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_SWIFT_NUMBER);
			} else if ((sc.ch == '@' || sc.ch == '`') && IsIdentifierStartEx(sc.chNext)) {
				if (sc.chPrev != '.') {
					chBeforeIdentifier = sc.chPrev;
				}
				sc.SetState((sc.ch == '@') ? SCE_SWIFT_ATTRIBUTE : SCE_SWIFT_IDENTIFIER_BT);
			} else if (sc.ch == '$' && IsIdentifierCharEx(sc.chNext)) {
				sc.SetState(SCE_SWIFT_VARIABLE);
			} else if (sc.ch == '#') {
				if (IsIdentifierStartEx(sc.chNext)) {
					sc.SetState(SCE_SWIFT_DIRECTIVE);
				} else {
					int delimiter = 0;
					if (CheckSwiftStringDelimiter(styler, sc.currentPos, DelimiterCheck::Start, delimiter)) {
						delimiterCount = delimiter;
						delimiters.push_back(delimiter);
						sc.SetState(SCE_SWIFT_STRING_ED);
						sc.Forward(delimiter);
						if (sc.Match('"', '"', '"')) {
							sc.ChangeState(SCE_SWIFT_TRIPLE_STRING_EDSTART);
							sc.Forward(2);
							sc.ForwardSetState(SCE_SWIFT_TRIPLE_STRING_ED);
							continue;
						}
					}
				}
			} else if (IsIdentifierStartEx(sc.ch)) {
				if (sc.chPrev != '.') {
					chBeforeIdentifier = sc.chPrev;
				}
				sc.SetState(SCE_SWIFT_IDENTIFIER);
			} else if (isoperator(sc.ch) || sc.ch == '\\') {
				const bool interpolating = !nestedState.empty();
				sc.SetState(interpolating ? SCE_SWIFT_OPERATOR2 : SCE_SWIFT_OPERATOR);
				if (interpolating) {
					if (sc.ch == '(') {
						nestedState.push_back(SCE_SWIFT_DEFAULT);
					} else if (sc.ch == ')') {
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
			int lineState = (commentLevel << 2) | (delimiterCount << 8) | lineStateLineType;
			if (!nestedState.empty()) {
				lineState |= PackLineState(nestedState) << 16;
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineType = 0;
			visibleChars = 0;
			visibleCharsBefore = 0;
			kwType = SCE_SWIFT_DEFAULT;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int packageImport;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & SwiftLineStateMaskLineComment),
		packageImport((lineState >> 1) & 1) {
	}
};

void FoldSwiftDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_SWIFT_OPERATOR, SCE_SWIFT_TASKMARKER, SCE_SWIFT_DIRECTIVE);
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
		case SCE_SWIFT_COMMENTBLOCK:
		case SCE_SWIFT_COMMENTBLOCKDOC: {
			const int level = (ch == '/' && chNext == '*') ? 1 : ((ch == '*' && chNext == '/') ? -1 : 0);
			if (level != 0) {
				levelNext += level;
				i++;
				chNext = styler.SafeGetCharAt(i + 1);
				styleNext = styler.StyleAt(i + 1);
			}
		} break;

		case SCE_SWIFT_TRIPLE_STRINGSTART:
		case SCE_SWIFT_TRIPLE_STRING_EDSTART:
			if (style != stylePrev) {
				levelNext++;
			}
			break;

		case SCE_SWIFT_TRIPLE_STRINGEND:
		case SCE_SWIFT_TRIPLE_STRING_EDEND:
			if (style != styleNext) {
				levelNext--;
			}
			break;

		case SCE_SWIFT_OPERATOR:
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
			break;

		case SCE_SWIFT_DIRECTIVE:
			if (ch == '#') {
				if (chNext == 'i' && styler.SafeGetCharAt(i + 2) == 'f') {
					levelNext++;
				} else if (chNext == 'e' && styler.Match(i + 1, "endif")) {
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
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_SWIFT_OPERATOR, SCE_SWIFT_TASKMARKER, SCE_SWIFT_DIRECTIVE);
				if (bracePos) {
					levelNext++;
					i = bracePos; // skip the brace
					chNext = '\0';
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

LexerModule lmSwift(SCLEX_SWIFT, ColouriseSwiftDoc, "swift", FoldSwiftDoc);
