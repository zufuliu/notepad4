// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for JavaScript, JScript, TypeScript, ActionScript.

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
	int outerState = SCE_JS_DEFAULT;
	int digitsLeft = 0;

	// highlight any character as escape sequence, no highlight for hex in '\u{hex}'.
	bool resetEscapeState(int state, int chNext) noexcept {
		outerState = state;
		digitsLeft = (chNext == 'x')? 3 : ((chNext == 'u') ? 5 : 1);
		return true;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

enum {
	JsLineStateMaskLineComment = 1,		// line comment
	JsLineStateMaskImport = (1 << 1),	// import

	JsLineStateInsideJsxExpression = 1 << 3,
	JsLineStateLineContinuation = 1 << 4,
};

enum class DocTagState {
	None,
	At,				/// @param x
	InlineAt,		/// {@link https://tsdoc.org/}
	XmlOpen,		/// <reference path="" />
	XmlClose,		/// </param>, No this (C# like) style
};

static_assert(DefaultNestedStateBaseStyle + 1 == SCE_JS_STRING_BT);
static_assert(DefaultNestedStateBaseStyle + 2 == SCE_JSX_TEXT);

constexpr bool IsJsIdentifierStart(int ch) noexcept {
	return IsIdentifierStartEx(ch) || ch == '$';
}

constexpr bool IsJsIdentifierChar(int ch) noexcept {
	return IsIdentifierCharEx(ch) || ch == '$';
}

inline bool IsJsIdentifierStartNext(const StyleContext &sc) noexcept {
	return IsJsIdentifierStart(sc.chNext) || (sc.chNext == '\\' && sc.GetRelative(2) == 'u');
}

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_JS_TASKMARKER;
}

constexpr bool FollowExpression(int chPrevNonWhite, int stylePrevNonWhite) noexcept {
	return chPrevNonWhite == ')' || chPrevNonWhite == ']'
		|| stylePrevNonWhite == SCE_JS_OPERATOR_PF
		|| IsJsIdentifierChar(chPrevNonWhite);
}

constexpr bool IsRegexStart(int chPrevNonWhite, int stylePrevNonWhite) noexcept {
	return stylePrevNonWhite == SCE_JS_WORD || !FollowExpression(chPrevNonWhite, stylePrevNonWhite);
}

inline bool IsJsxTagStart(const StyleContext &sc, int chPrevNonWhite, int stylePrevNonWhite) noexcept {
	// https://facebook.github.io/jsx/
	// https://reactjs.org/docs/jsx-in-depth.html
	return IsRegexStart(chPrevNonWhite, stylePrevNonWhite)
		&& (IsJsIdentifierStartNext(sc) || sc.chNext == '>' || sc.chNext == '{');
}

void ColouriseJsDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineType = 0;
	int lineContinuation = 0;
	bool insideReRange = false; // inside regex character range []

	int kwType = SCE_JS_DEFAULT;
	int chBeforeIdentifier = 0;

	std::vector<int> nestedState; // string interpolation "${}"
	bool insideJsxTag = false;
	int jsxTagLevel = 0;
	std::vector<int> jsxTagLevels;// nested JSX tag in expression

	// JSX syntax conflicts with TypeScript type assert.
	// https://www.typescriptlang.org/docs/handbook/jsx.html
	const bool enableJsx = styler.GetPropertyInt("lexer.jsx", 1) & true;

	int visibleChars = 0;
	int visibleCharsBefore = 0;
	int chPrevNonWhite = 0;
	int stylePrevNonWhite = SCE_JS_DEFAULT;
	DocTagState docTagState = DocTagState::None;
	EscapeSequence escSeq;

	if (enableJsx && startPos != 0) {
		// backtrack to the line starts JSX for better coloring on typing.
		BacktrackToStart(styler, JsLineStateInsideJsxExpression, startPos, lengthDoc, initStyle);
	}

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		2: lineStateLineType
		1: JsLineStateInsideJsxExpression
		1: lineContinuation
		3: nestedState count
		3*4: nestedState
		*/
		lineContinuation = lineState & JsLineStateLineContinuation;
		lineState >>= 8;
		if (lineState) {
			UnpackLineState(lineState, nestedState);
		}
	}
	if (startPos == 0) {
		if (sc.Match('#', '!')) {
			// Shell Shebang at beginning of file
			sc.SetState(SCE_JS_COMMENTLINE);
			sc.Forward();
			lineStateLineType = JsLineStateMaskLineComment;
		}
	} else if (IsSpaceEquiv(initStyle)) {
		// look back to set chPrevNonWhite properly for better regex colouring
		Sci_PositionU back = startPos - 1;
		while (back) {
			const int style = styler.StyleAt(back);
			if (!IsSpaceEquiv(style)) {
				chPrevNonWhite = static_cast<unsigned char>(styler.SafeGetCharAt(back));
				stylePrevNonWhite = style;
				break;
			}
			--back;
		}
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_JS_OPERATOR:
		case SCE_JS_OPERATOR2:
		case SCE_JS_OPERATOR_PF:
			sc.SetState(SCE_JS_DEFAULT);
			break;

		case SCE_JS_NUMBER:
			if (!IsDecimalNumberEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_JS_DEFAULT);
			}
			break;

		case SCE_JS_IDENTIFIER:
		case SCE_JSX_TAG:
		case SCE_JSX_ATTRIBUTE:
		case SCE_JSX_ATTRIBUTE_AT:
		case SCE_JS_DECORATOR:
			if ((sc.state != SCE_JS_IDENTIFIER && sc.ch == '.') || (sc.ch == ':' && (sc.state == SCE_JSX_TAG || sc.state == SCE_JSX_ATTRIBUTE))) {
				const int state = sc.state;
				sc.SetState(SCE_JS_OPERATOR2);
				sc.ForwardSetState(state);
				continue;
			}
			if (sc.Match('\\', 'u') || (sc.ch == '-' && (sc.state == SCE_JSX_TAG || sc.state == SCE_JSX_ATTRIBUTE))) {
				sc.Forward();
			} else if (!IsJsIdentifierChar(sc.ch)) {
				if (sc.state == SCE_JS_IDENTIFIER) {
					char s[128];
					sc.GetCurrent(s, sizeof(s));
					if (keywordLists[0]->InList(s)) {
						sc.ChangeState(SCE_JS_WORD);
						if (EqualsAny(s, "as", "class", "extends", "is", "new", "type")) {
							kwType = SCE_JS_CLASS;
						} else if (strcmp(s, "function") == 0) {
							kwType = SCE_JS_FUNCTION_DEFINE;
						} else if (EqualsAny(s, "interface", "implements")) {
							kwType = SCE_JS_INTERFACE;
						} else if (strcmp(s, "enum") == 0) {
							kwType = SCE_JS_ENUM;
						} else if (EqualsAny(s, "break", "continue")) {
							kwType = SCE_JS_LABEL;
						}
						if (kwType != SCE_JS_DEFAULT) {
							const int chNext = sc.GetLineNextChar();
							if (!(IsJsIdentifierStart(chNext) || chNext == '\\')) {
								kwType = SCE_JS_DEFAULT;
							}
						}
					} else if (keywordLists[1]->InList(s)) {
						sc.ChangeState(SCE_JS_WORD2);
					} else if (keywordLists[2]->InList(s)) {
						sc.ChangeState(SCE_JS_DIRECTIVE);
						if (EqualsAny(s, "import", "require")) {
							lineStateLineType = JsLineStateMaskImport;
						}
					} else if (keywordLists[3]->InList(s)) {
						sc.ChangeState(SCE_JS_CLASS);
					} else if (keywordLists[4]->InList(s)) {
						sc.ChangeState(SCE_JS_INTERFACE);
					} else if (keywordLists[5]->InList(s)) {
						sc.ChangeState(SCE_JS_ENUM);
					} else if (keywordLists[6]->InList(s)) {
						sc.ChangeState(SCE_JS_CONSTANT);
					} else if (sc.ch != '.') {
						if (kwType != SCE_JS_DEFAULT) {
							sc.ChangeState(kwType);
						} else {
							const int chNext = sc.GetDocNextChar(sc.ch == '?');
							if (chNext == '(') {
								sc.ChangeState(SCE_JS_FUNCTION);
							} else if ((chBeforeIdentifier == '<' && (chNext == '>' || chNext == '<')) || sc.Match('[', ']')) {
								// type<type>
								// type<type?>
								// type<type<type>>
								// type[]
								sc.ChangeState(SCE_JS_CLASS);
							}
						}
					}
					stylePrevNonWhite = sc.state;
					if (sc.state != SCE_JS_WORD && sc.ch != '.') {
						kwType = SCE_JS_DEFAULT;
					}
				} else if (sc.state == SCE_JSX_TAG || sc.state == SCE_JSX_ATTRIBUTE) {
					sc.SetState(SCE_JSX_TEXT);
					continue;
				}
				sc.SetState(SCE_JS_DEFAULT);
			}
			break;

		case SCE_JS_STRING_SQ:
		case SCE_JS_STRING_DQ:
		case SCE_JSX_STRING_SQ:
		case SCE_JSX_STRING_DQ:
			if (sc.ch == '\\') {
				if (IsEOLChar(sc.chNext)) {
					lineContinuation = JsLineStateLineContinuation;
				} else if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_JS_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.atLineStart) {
				if (lineContinuation) {
					lineContinuation = 0;
				} else {
					if (sc.state == SCE_JSX_STRING_SQ || sc.state == SCE_JSX_STRING_DQ) {
						sc.SetState(SCE_JSX_TEXT);
						continue;
					}
					sc.SetState(SCE_JS_DEFAULT);
				}
			} else if ((sc.ch == '\'' && (sc.state == SCE_JS_STRING_SQ || sc.state == SCE_JSX_STRING_SQ))
				|| (sc.ch == '"' && (sc.state == SCE_JS_STRING_DQ || sc.state == SCE_JSX_STRING_DQ))) {
				sc.Forward();
				if (sc.state == SCE_JSX_STRING_SQ || sc.state == SCE_JSX_STRING_DQ) {
					sc.SetState(SCE_JSX_TEXT);
					continue;
				}
				sc.SetState(SCE_JS_DEFAULT);
				continue;
			}
			break;

		case SCE_JS_STRING_BT:
			if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_JS_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.Match('$', '{')) {
				nestedState.push_back(sc.state);
				sc.SetState(SCE_JS_OPERATOR2);
				sc.Forward();
			} else if (sc.ch == '`') {
				sc.SetState(SCE_JS_STRING_BTEND);
				sc.ForwardSetState(SCE_JS_DEFAULT);
			}
			break;

		case SCE_JS_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_JS_REGEX:
			if (sc.ch == '\\') {
				sc.Forward();
			} else if (sc.ch == '[' || sc.ch == ']') {
				insideReRange = sc.ch == '[';
			} else if (sc.ch == '/' && !insideReRange) {
				sc.Forward();
				// regex flags
				while (IsLowerCase(sc.ch)) {
					sc.Forward();
				}
				sc.SetState(SCE_JS_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_JS_DEFAULT);
			}
			break;

		case SCE_JS_COMMENTLINE:
		case SCE_JS_COMMENTLINEDOC:
		case SCE_JS_COMMENTBLOCK:
		case SCE_JS_COMMENTBLOCKDOC:
			switch (docTagState) {
			case DocTagState::At:
				docTagState = DocTagState::None;
				break;
			case DocTagState::InlineAt:
				if (sc.ch == '}') {
					docTagState = DocTagState::None;
					sc.SetState(SCE_JS_COMMENTTAGAT);
					sc.ForwardSetState(SCE_JS_COMMENTBLOCKDOC);
					break;
				}
				break;
			case DocTagState::XmlOpen:
			case DocTagState::XmlClose:
				if (sc.Match('/', '>') || sc.ch == '>') {
					docTagState = DocTagState::None;
					sc.SetState(SCE_JS_COMMENTTAGXML);
					sc.Forward((sc.ch == '/') ? 2 : 1);
					sc.SetState(SCE_JS_COMMENTLINEDOC);
					break;
				}
				break;
			default:
				break;
			}
			if (sc.state == SCE_JS_COMMENTLINE || sc.state == SCE_JS_COMMENTLINEDOC) {
				if (sc.atLineStart) {
					sc.SetState(SCE_JS_DEFAULT);
					break;
				}
			} else if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_JS_DEFAULT);
				break;
			}
			if (docTagState == DocTagState::None) {
				if (sc.ch == '@' && IsLowerCase(sc.chNext) && IsCommentTagPrev(sc.chPrev)) {
					docTagState = DocTagState::At;
					escSeq.outerState = sc.state;
					sc.SetState(SCE_JS_COMMENTTAGAT);
				} else if (sc.state == SCE_JS_COMMENTBLOCKDOC && sc.Match('{', '@') && IsLowerCase(sc.GetRelative(2))) {
					docTagState = DocTagState::InlineAt;
					escSeq.outerState = sc.state;
					sc.SetState(SCE_JS_COMMENTTAGAT);
					sc.Forward();
				} else if (sc.state == SCE_JS_COMMENTLINEDOC && sc.ch == '<') {
					if (IsLowerCase(sc.chNext)) {
						docTagState = DocTagState::XmlOpen;
						escSeq.outerState = sc.state;
						sc.SetState(SCE_JS_COMMENTTAGXML);
					} else if (sc.chNext == '/' && IsLowerCase(sc.GetRelative(2))) {
						docTagState = DocTagState::XmlClose;
						escSeq.outerState = sc.state;
						sc.SetState(SCE_JS_COMMENTTAGXML);
						sc.Forward();
					}
				} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_JS_TASKMARKER)) {
					continue;
				}
			}
			break;

		case SCE_JS_COMMENTTAGAT:
		case SCE_JS_COMMENTTAGXML:
			if (!(IsIdentifierChar(sc.ch) || sc.ch == '-')) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_JSX_TEXT:
			if (sc.ch == '>' || sc.Match('/', '>')) {
				insideJsxTag = false;
				sc.SetState(SCE_JSX_TAG);
				if (sc.ch == '/') {
					// self closing <tag />
					--jsxTagLevel;
					sc.Forward();
				}
				sc.Forward();
				if (jsxTagLevel == 0) {
					sc.SetState(SCE_JS_DEFAULT);
				} else {
					sc.SetState(SCE_JSX_TEXT);
					continue;
				}
			} else if (sc.ch == '=' && insideJsxTag) {
				sc.SetState(SCE_JS_OPERATOR2);
				sc.ForwardSetState(SCE_JSX_TEXT);
				continue;
			} else if ((sc.ch == '\'' || sc.ch == '"') && insideJsxTag) {
				sc.SetState((sc.ch == '\'') ? SCE_JSX_STRING_SQ : SCE_JSX_STRING_DQ);
			} else if (insideJsxTag && (IsJsIdentifierStart(sc.ch) || sc.Match('\\', 'u'))) {
				sc.SetState(SCE_JSX_ATTRIBUTE);
			} else if (sc.ch == '{') {
				jsxTagLevels.push_back(jsxTagLevel);
				nestedState.push_back(sc.state);
				sc.SetState(SCE_JS_OPERATOR2);
				jsxTagLevel = 0;
			} else if (sc.Match('<', '/')) {
				--jsxTagLevel;
				insideJsxTag = false;
				sc.SetState(SCE_JSX_TAG);
				sc.Forward();
			} else if (sc.ch == '<') {
				++jsxTagLevel;
				insideJsxTag = true;
				sc.SetState(SCE_JSX_TAG);
			}
			break;
		}

		if (sc.state == SCE_JS_DEFAULT) {
			if (sc.ch == '/') {
				if (sc.chNext == '/') {
					docTagState = DocTagState::None;
					visibleCharsBefore = visibleChars;
					const int chNext = sc.GetRelative(2);
					sc.SetState((chNext == '/' || chNext == '!') ? SCE_JS_COMMENTLINEDOC : SCE_JS_COMMENTLINE);
					if (visibleChars == 0) {
						lineStateLineType = JsLineStateMaskLineComment;
					}
				} else if (sc.chNext == '*') {
					docTagState = DocTagState::None;
					visibleCharsBefore = visibleChars;
					const int chNext = sc.GetRelative(2);
					sc.SetState((chNext == '*' || chNext == '!') ? SCE_JS_COMMENTBLOCKDOC : SCE_JS_COMMENTBLOCK);
					sc.Forward();
				} else if (!IsEOLChar(sc.chNext) && IsRegexStart(chPrevNonWhite, stylePrevNonWhite)) {
					insideReRange = false;
					sc.SetState(SCE_JS_REGEX);
				} else {
					sc.SetState(SCE_JS_OPERATOR);
				}
			}
			else if (sc.ch == '\'') {
				sc.SetState(SCE_JS_STRING_SQ);
			} else if (sc.ch == '"') {
				sc.SetState(SCE_JS_STRING_DQ);
			} else if (sc.ch == '`') {
				sc.SetState(SCE_JS_STRING_BTSTART);
				sc.ForwardSetState(SCE_JS_STRING_BT);
				continue;
			} else if (IsNumberStartEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_JS_NUMBER);
			} else if (sc.ch == '@' && IsJsIdentifierStartNext(sc)) {
				sc.SetState((sc.chPrev == '.') ? SCE_JSX_ATTRIBUTE_AT : SCE_JS_DECORATOR);
			} else if (IsJsIdentifierStart(sc.ch) || sc.Match('\\', 'u')) {
				if (sc.chPrev != '.') {
					chBeforeIdentifier = sc.chPrev;
				}
				sc.SetState(SCE_JS_IDENTIFIER);
			}
			else if (sc.ch == '+' || sc.ch == '-') {
				if (sc.ch == sc.chNext) {
					// highlight ++ and -- as different style to simplify regex detection.
					sc.SetState(SCE_JS_OPERATOR_PF);
					sc.Forward();
				} else {
					sc.SetState(SCE_JS_OPERATOR);
				}
			} else if (sc.ch == '<' && enableJsx) {
				// <tag></tag>
				if (sc.chNext == '/') {
					insideJsxTag = false;
					--jsxTagLevel;
					sc.SetState(SCE_JSX_TAG);
					sc.Forward();
				} else if (IsJsxTagStart(sc, chPrevNonWhite, stylePrevNonWhite)) {
					insideJsxTag = true;
					++jsxTagLevel;
					sc.SetState(SCE_JSX_TAG);
				} else {
					sc.SetState(SCE_JS_OPERATOR);
				}
			} else if (isoperator(sc.ch)) {
				const bool interpolating = !nestedState.empty();
				sc.SetState(interpolating ? SCE_JS_OPERATOR2 : SCE_JS_OPERATOR);
				if (interpolating) {
					if (sc.ch == '{') {
						nestedState.push_back(SCE_JS_DEFAULT);
						if (enableJsx) {
							jsxTagLevels.push_back(jsxTagLevel);
							jsxTagLevel = 0;
						}
					} else if (sc.ch == '}') {
						if (enableJsx) {
							jsxTagLevel = TryTakeAndPop(jsxTagLevels);
						}
						const int outerState = TakeAndPop(nestedState);
						sc.ForwardSetState(outerState);
						continue;
					}
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
			if (!IsSpaceEquiv(sc.state)) {
				chPrevNonWhite = sc.ch;
				stylePrevNonWhite = sc.state;
			}
		}
		if (sc.atLineEnd) {
			int lineState = lineContinuation | lineStateLineType;
			if (enableJsx && !(jsxTagLevel == 0 && jsxTagLevels.empty())) {
				lineState |= JsLineStateInsideJsxExpression;
			}
			if (!nestedState.empty()) {
				lineState |= PackLineState(nestedState) << 8;
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineType = 0;
			visibleChars = 0;
			visibleCharsBefore = 0;
			kwType = SCE_JS_DEFAULT;
			docTagState = DocTagState::None;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int packageImport;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & JsLineStateMaskLineComment),
		packageImport((lineState >> 1) & 1) {
	}
};

constexpr bool IsInnerCommentStyle(int style) noexcept {
	return style == SCE_JS_COMMENTTAGAT || style == SCE_JS_COMMENTTAGXML || style == SCE_JS_TASKMARKER;
}

void FoldJsDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		switch (style) {
		case SCE_JS_COMMENTBLOCK:
		case SCE_JS_COMMENTBLOCKDOC:
			if (style != stylePrev && !IsInnerCommentStyle(stylePrev)) {
				levelNext++;
			} else if (style != styleNext && !IsInnerCommentStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_JS_STRING_BTSTART:
			if (style != stylePrev) {
				levelNext++;
			}
			break;

		case SCE_JS_STRING_BTEND:
			if (style != styleNext) {
				levelNext--;
			}
			break;

		case SCE_JS_OPERATOR:
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
			break;

		case SCE_JSX_TAG:
			if (ch == '<') {
				if (chNext == '/') {
					levelNext--;
					i++;
					chNext = styler.SafeGetCharAt(i + 1);
					styleNext = styler.StyleAt(i + 1);
				} else {
					levelNext++;
				}
			} else if (ch == '/' && chNext == '>') {
				levelNext--;
			}
			break;
		}

		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.packageImport) {
				levelNext += foldNext.packageImport - foldPrev.packageImport;
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
		}
	}
}

}

LexerModule lmJavaScript(SCLEX_JAVASCRIPT, ColouriseJsDoc, "js", FoldJsDoc);
