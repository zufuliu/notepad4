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
#include "LexerModule.h"

using namespace Scintilla;

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
	while (styler.SafeGetCharAt(pos) == '#') {
		++count;
		++pos;
	}

	if (start) {
		const char ch = styler.SafeGetCharAt(pos);
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
	RustLineStateMaskAttribute = (1 << 24), // attribute block
	RustLineStateMaskLineComment = (1 << 25), // line comment
	RustLineStateMaskPubUse = (1 << 26), // [pub] use
	MaxRustCharLiteralLength = 2 + 2 + 2 + 6, // '\u{10FFFF}'
};

void ColouriseRustDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateAttribute = 0;
	int lineStateLineComment = 0;
	int lineStatePubUse = 0;

	int squareBracket = 0;	// count of '[' and ']' for attribute
	int commentLevel = 0;	// nested block comment level
	int hashCount = 0;		// count of '#' for raw (byte) string
	int kwType = SCE_RUST_DEFAULT;

	int visibleChars = 0;
	Sci_PositionU charStartPos = 0;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		squareBracket = lineState & 0xff;
		commentLevel = (lineState >> 8) & 0xff;
		hashCount = (lineState >> 16) & 0xff;
		lineStateAttribute = lineState & RustLineStateMaskAttribute;
	}
	if (startPos == 0 && sc.Match('#', '!')) {
		// Shell Shebang at beginning of file
		sc.SetState(SCE_RUST_COMMENTLINE);
		lineStateLineComment = RustLineStateMaskLineComment;
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
			if (!IsIdentifierChar(sc.ch)) {
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
						if (strcmp(s, "struct") == 0) {
							kwType = SCE_RUST_STRUCT;
						} else if (strcmp(s, "fn") == 0) {
							kwType = SCE_RUST_FUNCTION_DEFINE;
						} else if (strcmp(s, "trait") == 0) {
							kwType = SCE_RUST_TRAIT;
						} else if (strcmp(s, "enum") == 0) {
							kwType = SCE_RUST_ENUMERATION;
						} else if (strcmp(s, "type") == 0) {
							kwType = SCE_RUST_TYPE; // type alias
						} else if (strcmp(s, "const") == 0) {
							kwType = SCE_RUST_CONSTANT;
						} else if (strcmp(s, "union") == 0) {
							kwType = SCE_RUST_UNION;
						}
						if (kwType != SCE_RUST_DEFAULT) {
							const int chNext = sc.GetNextNSChar();
							if (!IsIdentifierStart(chNext)) {
								kwType = SCE_RUST_DEFAULT;
							}
						}
						if ((visibleChars == 3 || visibleChars == 6) && strcmp(s, "use") == 0) {
							lineStatePubUse = RustLineStateMaskPubUse;
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
					} else {
						const int chNext = sc.GetNextNSChar();
						if (chNext == '(') {
							sc.ChangeState((kwType == SCE_RUST_FUNCTION_DEFINE)? kwType : SCE_RUST_FUNCTION);
						} else if (chNext == '!') {
							sc.ChangeState(SCE_RUST_MACRO);
						} else if (kwType != SCE_RUST_DEFAULT) {
							if (kwType != SCE_RUST_CONSTANT || chNext == ':') {
								sc.ChangeState(kwType);
							}
						}
					}
				}
				if (sc.state != SCE_RUST_WORD) {
					kwType = SCE_RUST_DEFAULT;
				}
				sc.SetState(SCE_RUST_DEFAULT);
			}
			break;

		case SCE_RUST_VARIABLE:
		case SCE_RUST_LIFETIME:
			if (!IsIdentifierChar(sc.ch)) {
				sc.SetState(SCE_RUST_DEFAULT);
			}
			break;

		case SCE_RUST_COMMENTLINE:
		case SCE_RUST_COMMENTLINEDOC:
			if (sc.atLineStart) {
				sc.SetState(SCE_RUST_DEFAULT);
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
				sc.Forward(2);
				++commentLevel;
			}
			break;

		case SCE_RUST_STRING:
		case SCE_RUST_BYTESTRING:
			if (sc.ch == '\\') {
				const int state = sc.state;
				if (IsEOLChar(sc.chNext)) {
					sc.SetState(SCE_RUST_LINE_CONTINUE);
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
				if (IsEOLChar(sc.chNext)) {
					sc.ForwardSetState(SCE_RUST_DEFAULT);
				} else {
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
				const int outerState = escSeq.outerState;
				if (outerState == SCE_RUST_STRING || outerState == SCE_RUST_BYTESTRING) {
					if (sc.ch == '\\') {
						if (IsEOLChar(sc.chNext)) {
							sc.SetState(SCE_RUST_LINE_CONTINUE);
							sc.ForwardSetState(outerState);
						} else {
							escSeq.resetEscapeState(outerState, sc.chNext);
							sc.Forward();
						}
					} else {
						sc.SetState(outerState);
						if (sc.ch == '\"') {
							sc.ForwardSetState(SCE_RUST_DEFAULT);
						}
					}
				} else {
					sc.SetState(outerState);
					if (sc.ch == '\'') {
						sc.ForwardSetState(SCE_RUST_DEFAULT);
					}
					continue;
				}
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
			if (sc.ch == '#') {
				if (sc.chNext == '[' || ((sc.chNext == '!' || isspacechar(sc.chNext)) && LexGetNextChar(sc.currentPos + 2, styler) == '[')) {
					// only support `#...[attr]` or `#!...[attr]`, not `#...!...[attr]`
					sc.SetState(SCE_RUST_ATTRIBUTE);
					if (sc.chNext == '!') {
						sc.Forward();
					}
					lineStateAttribute = RustLineStateMaskAttribute;
				}
			} else if (sc.Match('/', '/')) {
				const int chNext = sc.GetRelative(2);
				if (chNext == '!' || (chNext == '/' && sc.GetRelative(3) != '/')) {
					sc.SetState(SCE_RUST_COMMENTLINEDOC);
				} else {
					sc.SetState(SCE_RUST_COMMENTLINE);
				}
				if (visibleChars == 0) {
					lineStateLineComment = RustLineStateMaskLineComment;
				}
			} else if (sc.Match('/', '*')) {
				const int chNext = sc.GetRelative(2);
				if (chNext == '!' || (chNext == '*' && sc.GetRelative(3) != '*')) {
					sc.SetState(SCE_RUST_COMMENTBLOCKDOC);
				} else {
					sc.SetState(SCE_RUST_COMMENTBLOCK);
				}
				sc.Forward();
				commentLevel = 1;
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_RUST_STRING);
			} else if (sc.ch == '\'') {
				if (IsIdentifierStart(sc.chNext) && sc.GetRelative(2) != '\'') {
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
					sc.SetState(SCE_RUST_IDENTIFIER);
				}
			} else if (sc.ch == '$' && IsIdentifierStart(sc.chNext)) {
				sc.SetState(SCE_RUST_VARIABLE);
			} else if (IsADigit(sc.ch)) {
				sc.SetState(SCE_RUST_NUMBER);
			} else if (IsIdentifierStart(sc.ch)) {
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
			const int lineState = squareBracket | (commentLevel << 8) | (hashCount << 16)
				| lineStateAttribute | lineStateLineComment | lineStatePubUse;
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineComment = 0;
			lineStatePubUse = 0;
			visibleChars = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_RUST_COMMENTBLOCK || style == SCE_RUST_COMMENTBLOCKDOC;
}

constexpr bool IsMultilineStringStyle(int style) noexcept {
	return style == SCE_RUST_STRING || style == SCE_RUST_BYTESTRING
		|| style == SCE_RUST_RAW_STRING || style == SCE_RUST_RAW_BYTESTRING;
}

constexpr bool IsStringInnerStyle(int style) noexcept {
	return style == SCE_RUST_ESCAPECHAR || style == SCE_RUST_LINE_CONTINUE;
}

struct FoldLineState {
	int lineComment;
	int pubUse;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment((lineState >> 25) & 1),
		pubUse((lineState >> 26) & 1) {
	}
};

void FoldRustDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const int foldComment = styler.GetPropertyInt("fold.comment", 1);

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
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		if (IsStreamCommentStyle(style)) {
			if (foldComment) {
				const int level = (ch == '/' && chNext == '*') ? 1 : ((ch == '*' && chNext == '/') ? -1 : 0);
				if (level != 0) {
					levelNext += level;
					i++;
					style = styleNext;
					chNext = styler.SafeGetCharAt(i + 1);
					styleNext = styler.StyleAt(i + 1);
				}
			}
		} else if (IsMultilineStringStyle(style)) {
			if (style != stylePrev && !IsStringInnerStyle(stylePrev)) {
				levelNext++;
			} else if (style != styleNext && !IsStringInnerStyle(styleNext)) {
				levelNext--;
			}
		} else if (style == SCE_RUST_OPERATOR) {
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		}

		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldComment & foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.pubUse) {
				levelNext += foldNext.pubUse - foldPrev.pubUse;
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

LexerModule lmRust(SCLEX_RUST, ColouriseRustDoc, "rust", FoldRustDoc);
