// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Go.

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
#include "StyleContext.h"
#include "CharacterSet.h"
#include "StringUtils.h"
#include "LexerModule.h"

using namespace Lexilla;

namespace {

struct EscapeSequence {
	int outerState = SCE_GO_DEFAULT;
	int digitsLeft = 0;
	int numBase = 0;

	// highlight any character as escape sequence.
	void resetEscapeState(int state, int chNext) noexcept {
		outerState = state;
		digitsLeft = 1;
		numBase = 16;
		if (chNext == 'x') {
			digitsLeft = 3;
		} else if (chNext == 'u') {
			digitsLeft = 5;
		} else if (chNext == 'U') {
			digitsLeft = 9;
		} else if (IsOctalDigit(chNext)) {
			digitsLeft = 3;
			numBase = 8;
		}
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsADigit(ch, numBase);
	}
};

enum {
	GoFunction_None = 0,
	GoFunction_Define,
	GoFunction_Caller,
	GoFunction_Name,
	GoFunction_Param,
	GoFunction_Return,
};

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_GO_TASKMARKER;
}

// https://pkg.go.dev/fmt

constexpr bool IsFormatSpecifier(char ch) noexcept {
	return AnyOf(ch, 'v',
					'b',
					'c',
					'd',
					'e', 'E',
					'f', 'F',
					'g', 'G',
					'o', 'O',
					'p',
					'q',
					's',
					't', 'T',
					'U',
					'x', 'X');
}

inline Sci_Position CheckFormatSpecifier(const StyleContext &sc, LexAccessor &styler, bool insideUrl) noexcept {
	if (sc.chNext == '%') {
		return 2;
	}
	if (insideUrl && IsHexDigit(sc.chNext)) {
		// percent encoded URL string
		return 0;
	}
	if (IsASpaceOrTab(sc.chNext) && IsADigit(sc.chPrev)) {
		// ignore word after percent: "5% x"
		return 0;
	}

	Sci_PositionU pos = sc.currentPos + 1;
	// flags
	char ch = styler.SafeGetCharAt(pos);
	while (AnyOf(ch, ' ', '+', '-', '#', '0')) {
		ch = styler.SafeGetCharAt(++pos);
	}
	// argument index
	if (ch == '[') {
		ch = styler.SafeGetCharAt(++pos);
		while (IsADigit(ch))  {
			ch = styler.SafeGetCharAt(++pos);
		}
		if (ch == ']') {
			ch = styler.SafeGetCharAt(++pos);
		} else {
			return 0;
		}
	}
	// width
	if (ch == '*') {
		ch = styler.SafeGetCharAt(++pos);
	} else if (ch == '[') {
		ch = styler.SafeGetCharAt(++pos);
		while (IsADigit(ch))  {
			ch = styler.SafeGetCharAt(++pos);
		}
		if (ch == ']') {
			ch = styler.SafeGetCharAt(++pos);
		} else {
			return 0;
		}
	} else {
		while (IsADigit(ch))  {
			ch = styler.SafeGetCharAt(++pos);
		}
	}
	// precision
	if (ch == '.') {
		ch = styler.SafeGetCharAt(++pos);
		if (ch == '*') {
			ch = styler.SafeGetCharAt(++pos);
		} else if (ch == '[') {
			ch = styler.SafeGetCharAt(++pos);
			while (IsADigit(ch))  {
				ch = styler.SafeGetCharAt(++pos);
			}
			if (ch == ']') {
				ch = styler.SafeGetCharAt(++pos);
			} else {
				return 0;
			}
		} else {
			while (IsADigit(ch))  {
				ch = styler.SafeGetCharAt(++pos);
			}
		}
	}
	// verb
	if (IsFormatSpecifier(ch)) {
		return pos - sc.currentPos + 1;
	}
	return 0;
}

inline int DetectIdentifierType(LexAccessor &styler, int funcState, int chNext, Sci_Position startPos, Sci_Position lineStartCurrent) noexcept {
	if (((funcState == GoFunction_Caller || funcState == GoFunction_Return) && (chNext == ')' || chNext == ','))
		|| (funcState > GoFunction_Name && chNext == '{')) {
		// func (identifier *Type) (Type, error)
		// func (identifier Type) Type
		return SCE_GO_TYPE;
	}

	Sci_Position pos = --startPos;
	uint8_t ch = 0;
	while (pos > lineStartCurrent) {
		ch = styler[pos];
		if (!IsASpaceOrTab(ch)) {
			break;
		}
		--pos;
	}

	const bool star = (ch == '*' && pos == startPos);
	uint8_t chPrev = styler.SafeGetCharAt(pos - 1);
	const bool space = IsASpaceOrTab(chPrev);

	if (star) {
		if (chNext == ':' && space) {
			// case *Type:
			return SCE_GO_TYPE;
		}

		--pos;
		while (pos > lineStartCurrent) {
			ch = styler[pos];
			if (!IsASpaceOrTab(ch)) {
				break;
			}
			--pos;
		}

		chPrev = styler.SafeGetCharAt(pos - 1);
		if (ch == '-' && chPrev == '<') {
			// chan<- *Type
			return SCE_GO_TYPE;
		}
	} else if (ch == '&') {
		if (chNext == '{' && chPrev != '&') {
			// &Type{}
			return SCE_GO_TYPE;
		}
		return SCE_GO_DEFAULT;
	}

	if ((ch == '(' && chPrev == '.')
		|| ch == ']'
		|| (chNext == '{' && (ch == ':' || (ch == '=' && (chPrev == ':' || !isoperator(chPrev)))))
	) {
		// .(*Type), .(Type)
		// []*Type, []Type, [...]Type, [ArrayLength]Type, map[KeyType]ElementType
		// identifier = Type{}, identifier: Type{}, identifier := Type{}
		return SCE_GO_TYPE;
	}
	if ((!star || space) && IsIdentifierCharEx(ch)) {
		// identifier *Type, identifier Type
		return SCE_GO_TYPE;
	}
	return SCE_GO_DEFAULT;
}

void ColouriseGoDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineComment = 0;
	int funcState = GoFunction_None;
	int kwType = SCE_GO_DEFAULT;

	int visibleChars = 0;
	int visibleCharsBefore = 0;
	bool insideUrl = false;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);

	Sci_Position identifierStartPos = 0;
	Sci_Position lineStartCurrent = styler.LineStart(sc.currentLine);

	while (sc.More()) {
		switch (sc.state) {
		case SCE_GO_OPERATOR:
			sc.SetState(SCE_GO_DEFAULT);
			break;

		case SCE_GO_NUMBER:
			if (!IsDecimalNumberEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_GO_DEFAULT);
			}
			break;

		case SCE_GO_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				const int kwPrev = kwType;
				if (keywordLists[0]->InList(s)) {
					sc.ChangeState(SCE_GO_WORD);
					if (StrEqual(s, "func")) {
						funcState = (visibleChars == 4)? GoFunction_Define : GoFunction_Param;
					} else if (StrEqual(s, "type")) {
						kwType = SCE_GO_TYPE;
					} else if (StrEqual(s, "const")) {
						kwType = SCE_GO_CONSTANT;
					} else if (StrEqualsAny(s, "map", "chan")) {
						kwType = SCE_GO_IDENTIFIER;
					} else if (StrEqualsAny(s, "goto", "break", "continue")) {
						kwType = SCE_GO_LABEL;
					}
					if (kwType == SCE_GO_TYPE || kwType == SCE_GO_LABEL) {
						const int chNext = sc.GetLineNextChar();
						if (!IsIdentifierStartEx(chNext)) {
							kwType = SCE_GO_DEFAULT;
						}
					}
				} else if (keywordLists[1]->InList(s)) {
					sc.ChangeState(SCE_GO_WORD2);
				} else if (keywordLists[2]->InListPrefixed(s, '(')) {
					sc.ChangeState(SCE_GO_BUILTIN_FUNC);
					if (sc.ch == '(' && StrEqual(s, "new")) {
						kwType = SCE_GO_IDENTIFIER;
					}
				} else if (keywordLists[3]->InList(s)) {
					sc.ChangeState(SCE_GO_TYPE);
				} else if (keywordLists[4]->InList(s)) {
					sc.ChangeState(SCE_GO_STRUCT);
				} else if (keywordLists[5]->InList(s)) {
					sc.ChangeState(SCE_GO_INTERFACE);
				} else if (keywordLists[6]->InList(s)) {
					sc.ChangeState(SCE_GO_CONSTANT);
				} else {
					const bool ignoreCurrent = sc.ch == ':' && visibleChars == sc.LengthCurrent();
					const int chNext = sc.GetLineNextChar(ignoreCurrent);
					if (ignoreCurrent) {
						if (IsJumpLabelNextChar(chNext)) {
							sc.ChangeState(SCE_GO_LABEL);
						}
					} else if (chNext == '(') {
						if (funcState != GoFunction_None) {
							funcState = GoFunction_Name;
							sc.ChangeState(SCE_GO_FUNCTION_DEFINE);
						} else {
							sc.ChangeState(SCE_GO_FUNCTION);
						}
					} else if (sc.Match('{', '}')) {
						// Type{}
						sc.ChangeState(SCE_GO_TYPE);
					} else if (kwType != SCE_GO_DEFAULT) {
						if (kwType == SCE_GO_TYPE) {
							const Sci_Position pos = LexSkipWhiteSpace(sc.currentPos + 1, sc.lineStartNext, styler);
							if (chNext == 'i' && styler.Match(pos, "interface")) {
								kwType = SCE_GO_INTERFACE;
							} else if (chNext == 's' && styler.Match(pos, "struct")) {
								kwType = SCE_GO_STRUCT;
							}
						} else if (kwType == SCE_GO_IDENTIFIER && chNext != '.') {
							// map[KeyType]ElementType
							// chan ElementType
							// new(Type)
							kwType = SCE_GO_TYPE;
						}
						if (kwType != SCE_GO_IDENTIFIER) {
							sc.ChangeState(kwType);
							kwType = SCE_GO_DEFAULT;
						}
					} else if (!(chNext == '.' || chNext == '*')) {
						const int state = DetectIdentifierType(styler, funcState, chNext, identifierStartPos, lineStartCurrent);
						if (state != SCE_GO_DEFAULT) {
							sc.ChangeState(state);
						}
					}
				}

				if (sc.state == SCE_GO_WORD || sc.state == SCE_GO_WORD2) {
					identifierStartPos = lineStartCurrent = sc.currentPos;
				}
				if (kwType != SCE_GO_DEFAULT && kwPrev == kwType && sc.ch != '.') {
					kwType = SCE_GO_DEFAULT;
				}
				sc.SetState(SCE_GO_DEFAULT);
			}
			break;

		case SCE_GO_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_GO_DEFAULT);
			} else if (visibleChars - visibleCharsBefore == 2
				&& ((sc.ch == '+' && sc.Match("+build")) || sc.Match('g', 'o', ':'))) {
				sc.SetState(SCE_GO_TASKMARKERLINE);
			} else {
				HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_GO_TASKMARKER);
			}
			break;

		case SCE_GO_TASKMARKERLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_GO_DEFAULT);
			}
			break;

		case SCE_GO_COMMENTBLOCK:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_GO_DEFAULT);
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_GO_TASKMARKER)) {
				continue;
			}
			break;

		case SCE_GO_STRING:
		case SCE_GO_RAW_STRING:
			if (sc.state == SCE_GO_STRING && sc.ch == '\\') {
				escSeq.resetEscapeState(sc.state, sc.chNext);
				sc.SetState(SCE_GO_ESCAPECHAR);
				sc.Forward();
			} else if (sc.ch == '%') {
				const Sci_Position length = CheckFormatSpecifier(sc, styler, insideUrl);
				if (length != 0) {
					const int state = sc.state;
					sc.SetState(SCE_GO_FORMAT_SPECIFIER);
					sc.Advance(length);
					sc.SetState(state);
					continue;
				}
			} else if ((sc.state == SCE_GO_STRING && sc.ch == '\"') || (sc.state == SCE_GO_RAW_STRING && sc.ch == '`')) {
				sc.ForwardSetState(SCE_GO_DEFAULT);
			} else if (sc.state == SCE_GO_STRING && sc.atLineStart) {
				sc.SetState(SCE_GO_DEFAULT);
			} else if (sc.Match(':', '/', '/') && IsLowerCase(sc.chPrev)) {
				insideUrl = true;
			} else if (insideUrl && IsInvalidUrlChar(sc.ch)) {
				insideUrl = false;
			}
			break;

		case SCE_GO_CHARACTER:
			if (sc.ch == '\\') {
				escSeq.resetEscapeState(sc.state, sc.chNext);
				sc.SetState(SCE_GO_ESCAPECHAR);
				sc.Forward();
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_GO_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_GO_DEFAULT);
			}
			break;

		case SCE_GO_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;
		}

		if (sc.state == SCE_GO_DEFAULT) {
			if (sc.Match('/', '/')) {
				visibleCharsBefore = visibleChars;
				sc.SetState(SCE_GO_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineComment = SimpleLineStateMaskLineComment;
				}
			} else if (sc.Match('/', '*')) {
				visibleCharsBefore = visibleChars;
				sc.SetState(SCE_GO_COMMENTBLOCK);
				sc.Forward();
			} else if (sc.ch == '\"') {
				insideUrl = false;
				sc.SetState(SCE_GO_STRING);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_GO_CHARACTER);
			} else if (sc.ch == '`') {
				insideUrl = false;
				sc.SetState(SCE_GO_RAW_STRING);
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_GO_NUMBER);
			} else if (IsIdentifierStartEx(sc.ch)) {
				if (sc.chPrev != '.') {
					identifierStartPos = sc.currentPos;
				}
				sc.SetState(SCE_GO_IDENTIFIER);
			} else if (isoperator(sc.ch)) {
				sc.SetState(SCE_GO_OPERATOR);
				if (funcState != GoFunction_None) {
					switch (sc.ch) {
					case '(':
						switch (funcState) {
						case GoFunction_Define:
							funcState = GoFunction_Caller;
							break;
						case GoFunction_Caller:
						case GoFunction_Name:
							funcState = GoFunction_Param;
							break;
						case GoFunction_Param:
							funcState = GoFunction_Return;
							break;
						default:
							break;
						}
						break;
					case ')':
						if (funcState == GoFunction_Param) {
							funcState = GoFunction_Return;
						}
						break;
					case '{':
						if (!(sc.chPrev == 'e' && sc.chNext == '}')) {
							// interface{}
							funcState = GoFunction_None;
						}
						break;
					}
				} else if (sc.ch == ')' && IsASpaceOrTab(sc.chNext) && sc.GetLineNextChar(true) == '(') {
					funcState = GoFunction_Return;
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, lineStateLineComment);
			lineStateLineComment = 0;
			visibleChars = 0;
			visibleCharsBefore = 0;
			funcState = GoFunction_None;
			lineStartCurrent = sc.lineStartNext;
			identifierStartPos = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr bool IsInnerStyle(int style) noexcept {
	return style == SCE_GO_ESCAPECHAR || style == SCE_GO_FORMAT_SPECIFIER
		|| style == SCE_GO_TASKMARKER;
}

constexpr int GetLineCommentState(int lineState) noexcept {
	return lineState & SimpleLineStateMaskLineComment;
}

void FoldGoDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = GetLineCommentState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_GO_OPERATOR, SCE_GO_TASKMARKER);
		if (bracePos) {
			startPos = bracePos + 1; // skip the brace
		}
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = GetLineCommentState(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;
	int visibleChars = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		switch (style) {
		case SCE_GO_RAW_STRING:
		case SCE_GO_COMMENTBLOCK:
			if (style != stylePrev && !IsInnerStyle(stylePrev)) {
				levelNext++;
			} else if (style != styleNext && !IsInnerStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_GO_OPERATOR: {
			const char ch = styler[i];
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		} break;
		}

		if (visibleChars == 0 && !IsSpaceEquiv(style)) {
			++visibleChars;
		}
		if (i == lineEndPos) {
			const int lineCommentNext = GetLineCommentState(styler.GetLineState(lineCurrent + 1));
			if (lineCommentCurrent) {
				levelNext += lineCommentNext - lineCommentPrev;
			} else if (visibleChars) {
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_GO_OPERATOR, SCE_GO_TASKMARKER);
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
			lineCommentPrev = lineCommentCurrent;
			lineCommentCurrent = lineCommentNext;
			visibleChars = 0;
		}
	}
}

}

LexerModule lmGo(SCLEX_GO, ColouriseGoDoc, "go", FoldGoDoc);
