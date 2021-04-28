// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Rust.

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

struct EscapeSequence {
	int outerState = SCE_RUST_DEFAULT;
	int digitsLeft = 0;

	// highlight any character as escape sequence, no highlight for hex in '\u{hex}'.
	void resetEscapeState(int state, int chNext) noexcept {
		outerState = state;
		digitsLeft = (chNext == 'x') ? 3 : 1;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

bool IsRustRawString(LexAccessor &styler, Sci_PositionU pos, bool start, int &hashCount) noexcept {
	int count = 0;
	char ch;
	while ((ch = styler.SafeGetCharAt(pos)) == '#') {
		++count;
		++pos;
	}

	if (start) {
		if (ch == '\"') {
			hashCount = count;
			return true;
		}
	} else {
		return count == hashCount;
	}
	return false;
}

enum {
	RustLineStateMaskLineComment = (1 << 0),	// line comment
	RustLineStateMaskPubUse = (1 << 1),			// [pub] use
	RustLineStateMaskAttribute = (1 << 2),		// attribute block
	MaxRustCharLiteralLength = 2 + 2 + 2 + 6,	// '\u{10FFFF}'
};

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_RUST_TASKMARKER;
}

void ColouriseRustDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateAttribute = 0;
	int lineStateLineType = 0;

	int squareBracket = 0;	// count of '[' and ']' for attribute
	int commentLevel = 0;	// nested block comment level
	int hashCount = 0;		// count of '#' for raw (byte) string
	int kwType = SCE_RUST_DEFAULT;

	int chBeforeIdentifier = 0;
	int visibleChars = 0;
	int visibleCharsBefore = 0;
	Sci_PositionU charStartPos = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		2: lineStateLineType
		1: lineStateAttribute
		8: squareBracket
		8: commentLevel
		8: hashCount
		*/
		squareBracket = (lineState >> 3) & 0xff;
		commentLevel = (lineState >> 11) & 0xff;
		hashCount = (lineState >> 19) & 0xff;
		lineStateAttribute = lineState & RustLineStateMaskAttribute;
	}
	if (startPos == 0 && sc.Match('#', '!')) {
		// Shell Shebang at beginning of file
		sc.SetState(SCE_RUST_COMMENTLINE);
		sc.Forward();
		lineStateLineType = RustLineStateMaskLineComment;
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_RUST_OPERATOR:
		case SCE_RUST_ATTRIBUTE:
			sc.SetState(SCE_RUST_DEFAULT);
			break;

		case SCE_RUST_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_RUST_DEFAULT);
			}
			break;

		case SCE_RUST_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				if (lineStateAttribute) {
					sc.ChangeState(SCE_RUST_ATTRIBUTE);
				} else if (sc.ch == '!') {
					sc.ChangeState(SCE_RUST_MACRO);
					sc.Forward();
				} else {
					char s[128];
					sc.GetCurrent(s, sizeof(s));
					if (keywordLists[0]->InList(s)) {
						sc.ChangeState(SCE_RUST_WORD);
						if (StrEqual(s, "struct")) {
							kwType = SCE_RUST_STRUCT;
						} else if (StrEqual(s, "fn")) {
							kwType = SCE_RUST_FUNCTION_DEFINE;
						} else if (StrEqual(s, "trait")) {
							kwType = SCE_RUST_TRAIT;
						} else if (StrEqual(s, "enum")) {
							kwType = SCE_RUST_ENUMERATION;
						} else if (StrEqual(s, "type")) {
							kwType = SCE_RUST_TYPE; // type alias
						} else if (StrEqual(s, "const")) {
							kwType = SCE_RUST_CONSTANT;
						} else if (StrEqual(s, "union")) {
							kwType = SCE_RUST_UNION;
						}
						if (kwType != SCE_RUST_DEFAULT) {
							const int chNext = sc.GetDocNextChar();
							if (!IsIdentifierStartEx(chNext)) {
								kwType = SCE_RUST_DEFAULT;
							}
						}
						if ((visibleChars == 3 || visibleChars == 6) && StrEqual(s, "use")) {
							lineStateLineType = RustLineStateMaskPubUse;
						}
					} else if (keywordLists[1]->InList(s)) {
						sc.ChangeState(SCE_RUST_WORD2);
					} else if (keywordLists[2]->InList(s)) {
						sc.ChangeState(SCE_RUST_TYPE);
					} else if (keywordLists[3]->InList(s)) {
						sc.ChangeState(SCE_RUST_STRUCT);
					} else if (keywordLists[4]->InList(s)) {
						sc.ChangeState(SCE_RUST_TRAIT);
					} else if (keywordLists[5]->InList(s)) {
						sc.ChangeState(SCE_RUST_ENUMERATION);
					} else if (keywordLists[6]->InList(s)) {
						sc.ChangeState(SCE_RUST_UNION);
					} else if (keywordLists[7]->InList(s)) {
						sc.ChangeState(SCE_RUST_CONSTANT);
					} else if (sc.ch != '.') {
						const int chNext = sc.GetDocNextChar();
						if (chNext == '(') {
							sc.ChangeState((kwType == SCE_RUST_FUNCTION_DEFINE)? kwType : SCE_RUST_FUNCTION);
						} else if (chNext == '!') {
							sc.ChangeState(SCE_RUST_MACRO);
						} else if (kwType != SCE_RUST_DEFAULT) {
							if (kwType != SCE_RUST_CONSTANT || chNext == ':') {
								sc.ChangeState(kwType);
							} else if (chBeforeIdentifier == '[' && sc.ch == ';') {
								// array: [T; N]
								sc.ChangeState(SCE_RUST_TYPE);
							}
						}
					}
				}
				if (sc.state != SCE_RUST_WORD && sc.ch != '.') {
					kwType = SCE_RUST_DEFAULT;
				}
				sc.SetState(SCE_RUST_DEFAULT);
			}
			break;

		case SCE_RUST_VARIABLE:
		case SCE_RUST_LIFETIME:
			if (!IsIdentifierCharEx(sc.ch)) {
				sc.SetState(SCE_RUST_DEFAULT);
			}
			break;

		case SCE_RUST_COMMENTLINE:
		case SCE_RUST_COMMENTLINEDOC:
			if (sc.atLineStart) {
				sc.SetState(SCE_RUST_DEFAULT);
			} else {
				HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_RUST_TASKMARKER);
			}
			break;

		case SCE_RUST_COMMENTBLOCK:
		case SCE_RUST_COMMENTBLOCKDOC:
			if (sc.Match('*', '/')) {
				sc.Forward();
				--commentLevel;
				if (commentLevel == 0) {
					sc.ForwardSetState(SCE_RUST_DEFAULT);
				}
			} else if (sc.Match('/', '*')) {
				// TODO: nested block comment
				//const int chNext = sc.GetRelative(2);
				//if (chNext == '!' || (chNext == '*' && sc.GetRelative(3) != '*')) {
				//	sc.SetState(SCE_RUST_COMMENTBLOCKDOC);
				//} else {
				//	sc.SetState(SCE_RUST_COMMENTBLOCK);
				//}
				sc.Forward();
				++commentLevel;
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_RUST_TASKMARKER)) {
				continue;
			}
			break;

		case SCE_RUST_STRING:
		case SCE_RUST_BYTESTRING:
			if (sc.ch == '\\') {
				const int state = sc.state;
				if (IsEOLChar(sc.chNext)) {
					sc.SetState(SCE_RUST_LINE_CONTINUATION);
					sc.ForwardSetState(state);
				} else {
					escSeq.resetEscapeState(state, sc.chNext);
					sc.SetState(SCE_RUST_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_RUST_DEFAULT);
			}
			break;

		case SCE_RUST_CHARACTER:
		case SCE_RUST_BYTE_CHARACTER:
			if (sc.ch == '\\') {
				if (!IsEOLChar(sc.chNext)) {
					escSeq.resetEscapeState(sc.state, sc.chNext);
					sc.SetState(SCE_RUST_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_RUST_DEFAULT);
			} else if (sc.atLineEnd || sc.currentPos - charStartPos >= MaxRustCharLiteralLength - 1) {
				// prevent changing styles for remain document on typing.
				sc.SetState(SCE_RUST_DEFAULT);
			}
			break;

		case SCE_RUST_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_RUST_RAW_STRING:
		case SCE_RUST_RAW_BYTESTRING:
			if (sc.ch == '\"') {
				if (hashCount == 0 || (sc.chNext == '#' && IsRustRawString(styler, sc.currentPos + 1, false, hashCount))) {
					sc.Forward(hashCount);
					hashCount = 0;
					sc.ForwardSetState(SCE_RUST_DEFAULT);
				}
			}
			break;
		}

		if (sc.state == SCE_RUST_DEFAULT) {
			if (sc.ch == '/' && (sc.chNext == '/' || sc.chNext == '*')) {
				visibleCharsBefore = visibleChars;
				const int chNext = sc.chNext;
				sc.SetState((chNext == '/') ? SCE_RUST_COMMENTLINE : SCE_RUST_COMMENTBLOCK);
				sc.Forward(2);
				if (sc.ch == '!' || (sc.ch == chNext && sc.chNext != chNext)) {
					sc.ChangeState((chNext == '/') ? SCE_RUST_COMMENTLINEDOC : SCE_RUST_COMMENTBLOCKDOC);
				}
				if (chNext == '/') {
					if (visibleChars == 0) {
						lineStateLineType = RustLineStateMaskLineComment;
					}
				} else {
					commentLevel = 1;
				}
				continue;
			}
			if (sc.ch == '#') {
				if (sc.chNext == '[' || ((sc.chNext == '!' || isspacechar(sc.chNext)) && LexGetNextChar(sc.currentPos + 2, styler) == '[')) {
					// only support `#...[attr]` or `#!...[attr]`, not `#...!...[attr]`
					sc.SetState(SCE_RUST_ATTRIBUTE);
					if (sc.chNext == '!') {
						sc.Forward();
					}
					lineStateAttribute = RustLineStateMaskAttribute;
				}
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_RUST_STRING);
			} else if (sc.ch == '\'') {
				if (IsIdentifierStartEx(sc.chNext) && sc.GetRelative(2) != '\'') {
					sc.SetState(SCE_RUST_LIFETIME);
				} else {
					charStartPos = sc.currentPos;
					sc.SetState(SCE_RUST_CHARACTER);
				}
			} else if (sc.Match('r', '#')) {
				if (IsRustRawString(styler, sc.currentPos + 2, true, hashCount)) {
					hashCount += 1;
					sc.SetState(SCE_RUST_RAW_STRING);
					sc.Forward(hashCount + 1);
				} else {
					if (sc.chPrev != '.') {
						chBeforeIdentifier = sc.chPrev;
					}
					sc.SetState(SCE_RUST_IDENTIFIER);
					const int chNext = sc.GetRelative(2);
					if (IsIdentifierStart(chNext)) {
						// raw identifier: r# + keyword
						sc.Forward();
					}
				}
			} else if (sc.Match('r', '\"')) {
				hashCount = 0;
				sc.SetState(SCE_RUST_RAW_STRING);
				sc.Forward();
			} else if (sc.Match('b', '\"')) {
				sc.SetState(SCE_RUST_BYTESTRING);
				sc.Forward();
			} else if (sc.Match('b', '\'')) {
				charStartPos = sc.currentPos;
				sc.SetState(SCE_RUST_BYTE_CHARACTER);
				sc.Forward();
			} else if (sc.Match('b', 'r')) {
				if (IsRustRawString(styler, sc.currentPos + 2, true, hashCount)) {
					sc.SetState(SCE_RUST_RAW_BYTESTRING);
					sc.Forward(hashCount + 2);
				} else {
					if (sc.chPrev != '.') {
						chBeforeIdentifier = sc.chPrev;
					}
					sc.SetState(SCE_RUST_IDENTIFIER);
				}
			} else if (sc.ch == '$' && IsIdentifierStartEx(sc.chNext)) {
				sc.SetState(SCE_RUST_VARIABLE);
			} else if (IsADigit(sc.ch)) {
				sc.SetState(SCE_RUST_NUMBER);
			} else if (IsIdentifierStartEx(sc.ch)) {
				if (sc.chPrev != '.') {
					chBeforeIdentifier = sc.chPrev;
				}
				sc.SetState(SCE_RUST_IDENTIFIER);
			} else if (isoperator(sc.ch) || sc.ch == '$' || sc.ch == '@') {
				sc.SetState(SCE_RUST_OPERATOR);
				if (lineStateAttribute) {
					if (sc.ch == '[') {
						++squareBracket;
					} else if (sc.ch == ']') {
						--squareBracket;
						if (squareBracket == 0) {
							lineStateAttribute = 0;
						}
					}
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
		}
		if (sc.atLineEnd) {
			const int lineState = (squareBracket << 3) | (commentLevel << 11) | (hashCount << 19)
				| lineStateAttribute | lineStateLineType;
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineType = 0;
			visibleChars = 0;
			visibleCharsBefore = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr bool IsStringInnerStyle(int style) noexcept {
	return style == SCE_RUST_ESCAPECHAR || style == SCE_RUST_LINE_CONTINUATION;
}

struct FoldLineState {
	int lineComment;
	int pubUse;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & RustLineStateMaskLineComment),
		pubUse((lineState >> 1) & 1) {
	}
};

void FoldRustDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_RUST_OPERATOR, SCE_RUST_TASKMARKER);
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
		case SCE_RUST_COMMENTBLOCK:
		case SCE_RUST_COMMENTBLOCKDOC:  {
			const int level = (ch == '/' && chNext == '*') ? 1 : ((ch == '*' && chNext == '/') ? -1 : 0);
			if (level != 0) {
				levelNext += level;
				i++;
				style = styleNext;
				chNext = styler.SafeGetCharAt(i + 1);
				styleNext = styler.StyleAt(i + 1);
			}
		} break;

		case SCE_RUST_STRING:
		case SCE_RUST_BYTESTRING:
		case SCE_RUST_RAW_STRING:
		case SCE_RUST_RAW_BYTESTRING:
			if (style != stylePrev && !IsStringInnerStyle(stylePrev)) {
				levelNext++;
			} else if (style != styleNext && !IsStringInnerStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_RUST_OPERATOR:
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
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
			} else if (foldCurrent.pubUse) {
				levelNext += foldNext.pubUse - foldPrev.pubUse;
			} else if (visibleChars) {
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_RUST_OPERATOR, SCE_RUST_TASKMARKER);
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

LexerModule lmRust(SCLEX_RUST, ColouriseRustDoc, "rust", FoldRustDoc);
