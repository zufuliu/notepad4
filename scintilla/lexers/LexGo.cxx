// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Go.

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
	int outerState = SCE_GO_DEFAULT;
	int digitsLeft = 0;
	int numBase = 16;

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

	GoLineStateMaskLineComment = (1 << 0), // line comment
};

constexpr bool IsFormatSpecifier(uint8_t ch) noexcept {
	return ch == 'v'
		|| ch == 'b'
		|| ch == 'c'
		|| ch == 'd'
		|| ch == 'e' || ch == 'E'
		|| ch == 'f' || ch == 'F'
		|| ch == 'g' || ch == 'G'
		|| ch == 'o' || ch == 'O'
		|| ch == 'p'
		|| ch == 'q'
		|| ch == 's'
		|| ch == 't' || ch == 'T'
		|| ch == 'x' || ch == 'X'
		|| ch == 'U';
}

Sci_Position CheckFormatSpecifier(const StyleContext &sc) noexcept {
	if (sc.chNext == '%') {
		return 2;
	}
	if (IsASpaceOrTab(sc.chNext) && IsADigit(sc.chPrev)) {
		// ignore word after percent: "5% x"
		return 0;
	}

	Sci_Position pos = sc.currentPos + 1;
	if (sc.chNext == '+' || sc.chNext == '-' || sc.chNext == '#' || sc.chNext == ' ') {
		++pos;
	}
	while (pos < sc.lineStartNext) {
		const uint8_t ch = sc.styler[pos];
		if (IsFormatSpecifier(ch)) {
			// TODO: fix percent encoded URL string
			return pos - sc.currentPos + 1;
		}
		if (!(IsADigit(ch) || ch == '*' || ch == '.' || ch == '[' || ch == ']')) {
			break;
		}
		++pos;
	}
	return 0;
}

int DetectIdentifierType(LexAccessor &styler, int funcState, int chNext, Sci_Position startPos, Sci_Position lineStartCurrent) noexcept {
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
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);

	Sci_Position identifierStart = 0;
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
					if (strcmp(s, "func") == 0) {
						funcState = (visibleChars == 4)? GoFunction_Define : GoFunction_Param;
					} else if (strcmp(s, "type") == 0) {
						kwType = SCE_GO_TYPE;
					} else if (strcmp(s, "const") == 0) {
						kwType = SCE_GO_CONSTANT;
					} else if (strcmp(s, "map") == 0 || strcmp(s, "chan") == 0) {
						kwType = SCE_GO_IDENTIFIER;
					}
				} else if (keywordLists[1]->InList(s)) {
					sc.ChangeState(SCE_GO_WORD2);
				} else if (keywordLists[2]->InListPrefixed(s, '(')) {
					sc.ChangeState(SCE_GO_BUILTIN_FUNC);
					if (sc.ch == '(' && strcmp(s, "new") == 0) {
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
					const int chNext = sc.GetLineNextChar();
					if (chNext == '(') {
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
						const int state = DetectIdentifierType(styler, funcState, chNext, identifierStart, lineStartCurrent);
						if (state != SCE_GO_DEFAULT) {
							sc.ChangeState(state);
						}
					}
				}

				if (sc.state == SCE_GO_WORD || sc.state == SCE_GO_WORD2) {
					identifierStart = lineStartCurrent = sc.currentPos;
				}
				if (kwType != SCE_GO_DEFAULT && kwPrev == kwType && sc.ch != '.') {
					kwType = SCE_GO_DEFAULT;
				}
				if (sc.ch == '.' && IsIdentifierStartEx(sc.chNext)) {
					sc.SetState(SCE_GO_OPERATOR);
					sc.ForwardSetState(SCE_GO_IDENTIFIER);
				} else {
					sc.SetState(SCE_GO_DEFAULT);
				}
			}
			break;

		case SCE_GO_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_GO_DEFAULT);
			} else if (visibleChars - visibleCharsBefore == 2) {
				if (IsUpperCase(sc.ch)) {
					if (IsUpperCase(sc.chNext)) {
						escSeq.outerState = sc.state;
						sc.SetState(SCE_GO_TASK_MARKER);
						sc.Forward();
					}
				} else if ((sc.ch == '+' && sc.Match("+build")) || sc.Match('g', 'o', ':')) {
					sc.SetState(SCE_GO_TASK_MARKER_LINE);
				}
			}
			break;

		case SCE_GO_TASK_MARKER_LINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_GO_DEFAULT);
			}
			break;

		case SCE_GO_TASK_MARKER:
			if (sc.ch == ':' || sc.ch == '(') {
				sc.SetState(escSeq.outerState);
			} else if (!IsUpperCase(sc.ch)) {
				sc.ChangeState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_GO_COMMENTBLOCK:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_GO_DEFAULT);
			} else if (visibleChars == 0 && IsUpperCase(sc.ch) && IsUpperCase(sc.chNext)) {
				escSeq.outerState = sc.state;
				sc.SetState(SCE_GO_TASK_MARKER);
				sc.Forward();
			}
			break;

		case SCE_GO_STRING:
		case SCE_GO_RAW_STRING:
			if (sc.state == SCE_GO_STRING && sc.ch == '\\') {
				escSeq.resetEscapeState(sc.state, sc.chNext);
				sc.SetState(SCE_GO_ESCAPECHAR);
				sc.Forward();
			} else if (sc.ch == '%') {
				const Sci_Position length = CheckFormatSpecifier(sc);
				if (length != 0) {
					const int state = sc.state;
					sc.SetState(SCE_GO_FORMAT_SPECIFIER);
					sc.Forward(length);
					sc.SetState(state);
					continue;
				}
			} else if ((sc.state == SCE_GO_STRING && sc.ch == '\"') || (sc.state == SCE_GO_RAW_STRING && sc.ch == '`')) {
				sc.ForwardSetState(SCE_GO_DEFAULT);
			} else if (sc.state == SCE_GO_STRING && sc.atLineStart) {
				sc.SetState(SCE_GO_DEFAULT);
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
				sc.SetState(SCE_GO_COMMENTLINE);
				if (visibleChars == 0) {
					lineStateLineComment = GoLineStateMaskLineComment;
				}
				visibleCharsBefore = visibleChars;
			} else if (sc.Match('/', '*')) {
				sc.SetState(SCE_GO_COMMENTBLOCK);
				sc.Forward();
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_GO_STRING);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_GO_CHARACTER);
			} else if (sc.ch == '`') {
				sc.SetState(SCE_GO_RAW_STRING);
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_GO_NUMBER);
			} else if (IsIdentifierStartEx(sc.ch)) {
				sc.SetState(SCE_GO_IDENTIFIER);
				identifierStart = sc.currentPos;
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
				} else if (sc.ch == ')' && IsASpaceOrTab(sc.chNext) && sc.GetLineNextChar(1) == '(') {
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
			funcState = GoFunction_None;
			lineStartCurrent = sc.lineStartNext;
			identifierStart = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr bool IsInnerStyle(int style) noexcept {
	return style == SCE_GO_TASK_MARKER || style == SCE_GO_FORMAT_SPECIFIER;
}

void FoldGoDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;

	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = styler.GetLineState(lineCurrent - 1) & GoLineStateMaskLineComment;
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = styler.GetLineState(lineCurrent) & GoLineStateMaskLineComment;
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = ((lineStartNext < endPos) ? lineStartNext : endPos) - 1;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		if (style == SCE_GO_RAW_STRING || (foldComment && style == SCE_GO_COMMENTBLOCK)) {
			if (style != stylePrev && !IsInnerStyle(stylePrev)) {
				levelNext++;
			} else if (style != styleNext && !IsInnerStyle(styleNext)) {
				levelNext--;
			}
		} else if (style == SCE_GO_OPERATOR) {
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		}

		if (i == lineEndPos) {
			const int lineCommentNext = styler.GetLineState(lineCurrent + 1) & GoLineStateMaskLineComment;
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

LexerModule lmGo(SCLEX_GO, ColouriseGoDoc, "go", FoldGoDoc);
