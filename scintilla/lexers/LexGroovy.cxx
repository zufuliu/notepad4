// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Groovy, Gradle.

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
	int outerState = SCE_GROOVY_DEFAULT;
	int digitsLeft = 0;

	// highlight any character as escape sequence.
	bool resetEscapeState(int state, int chNext) noexcept {
		if (IsEOLChar(chNext)) {
			return false;
		}
		outerState = state;
		digitsLeft = 1;
		if (chNext == 'u') {
			digitsLeft = 5;
		}
		return true;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

enum {
	GroovyLineStateMaskLineComment = 1, // line comment
	GroovyLineStateMaskImport = 1 << 1, // import
};

enum class DocTagState {
	None,
	At,				// @param x
	InlineAt,		// {@link package.class#member label}
	TagOpen,		// <tag>
	TagClose,		// </tag>
};

static_assert(DefaultNestedStateBaseStyle + 1 == SCE_GROOVY_STRING_DQ);
static_assert(DefaultNestedStateBaseStyle + 2 == SCE_GROOVY_TRIPLE_STRING_DQ);
static_assert(DefaultNestedStateBaseStyle + 3 == SCE_GROOVY_SLASHY_STRING);
static_assert(DefaultNestedStateBaseStyle + 4 == SCE_GROOVY_DOLLAR_SLASHY);

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_GROOVY_TASKMARKER;
}

constexpr bool FollowExpression(int chPrevNonWhite, int stylePrevNonWhite) noexcept {
	return chPrevNonWhite == ')' || chPrevNonWhite == ']'
		|| stylePrevNonWhite == SCE_GROOVY_OPERATOR_PF
		|| IsIdentifierCharEx(chPrevNonWhite);
}

constexpr bool IsSlashyStringStart(int chPrevNonWhite, int stylePrevNonWhite) noexcept {
	// TODO: avoid colouring remaining document on typing `/` for line/block comment.
	return stylePrevNonWhite == SCE_GROOVY_WORD || !FollowExpression(chPrevNonWhite, stylePrevNonWhite);
}

void ColouriseGroovyDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineType = 0;

	int kwType = SCE_GROOVY_DEFAULT;
	int chBeforeIdentifier = 0;

	std::vector<int> nestedState; // string interpolation "${}"
	int bracketCount = 0; // for () and []

	int visibleChars = 0;
	int chBefore = 0;
	int visibleCharsBefore = 0;
	int chPrevNonWhite = 0;
	int stylePrevNonWhite = SCE_JS_DEFAULT;
	DocTagState docTagState = DocTagState::None;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		2: lineStateLineType
		3: nestedState count
		3*4: nestedState
		*/
		bracketCount = (lineState >> 2) & 0x3f;
		lineState >>= 8;
		if (lineState) {
			UnpackLineState(lineState, nestedState);
		}
	}
	if (startPos == 0) {
		if (sc.Match('#', '!')) {
			// Shell Shebang at beginning of file
			sc.SetState(SCE_GROOVY_COMMENTLINE);
			sc.Forward();
			lineStateLineType = GroovyLineStateMaskLineComment;
		}
	} else if (IsSpaceEquiv(initStyle)) {
		// look back for better slashy string colouring
		LookbackNonWhite(styler, startPos, SCE_GROOVY_TASKMARKER, chPrevNonWhite, stylePrevNonWhite);
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_GROOVY_OPERATOR:
		case SCE_GROOVY_OPERATOR2:
		case SCE_GROOVY_OPERATOR_PF:
			sc.SetState(SCE_GROOVY_DEFAULT);
			break;

		case SCE_GROOVY_NUMBER:
			if (!IsDecimalNumberEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_GROOVY_DEFAULT);
			}
			break;

		case SCE_GROOVY_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				if (s[0] == '@') {
					if (StrEqual(s, "@interface")) {
						sc.ChangeState(SCE_GROOVY_WORD);
						kwType = SCE_GROOVY_ANNOTATION;
					} else {
						sc.ChangeState(SCE_GROOVY_ANNOTATION);
						continue;
					}
				} else if (keywordLists[0]->InList(s)) {
					const bool demoted = chBefore == '.' || chBefore == '&';
					sc.ChangeState(demoted ? SCE_GROOVY_WORD_DEMOTED : SCE_GROOVY_WORD);
					if (!demoted) {
						if (StrEqual(s, "import")) {
							if (visibleChars == sc.LengthCurrent()) {
								lineStateLineType = GroovyLineStateMaskImport;
							}
						} else if (StrEqualsAny(s, "class", "new", "extends", "instanceof", "throws", "as")) {
							kwType = SCE_GROOVY_CLASS;
						} else if (StrEqualsAny(s, "interface", "implements")) {
							kwType = SCE_GROOVY_INTERFACE;
						} else if (StrEqual(s, "trait")) {
							kwType = SCE_GROOVY_TRAIT;
						} else if (StrEqual(s, "enum")) {
							kwType = SCE_GROOVY_ENUM;
						} else if (StrEqualsAny(s, "if", "while")) {
							// to avoid treating following code as type cast:
							// if (identifier) expression, while (identifier) expression
							kwType = SCE_GROOVY_WORD;
						}
						if (kwType != SCE_GROOVY_DEFAULT && kwType != SCE_GROOVY_WORD) {
							const int chNext = sc.GetDocNextChar();
							if (!IsIdentifierStartEx(chNext)) {
								kwType = SCE_GROOVY_DEFAULT;
							}
						}
					}
				} else if (keywordLists[1]->InList(s)) {
					sc.ChangeState(SCE_GROOVY_WORD2);
				} else if (keywordLists[3]->InList(s)) {
					sc.ChangeState(SCE_GROOVY_CLASS);
				} else if (keywordLists[4]->InList(s)) {
					sc.ChangeState(SCE_GROOVY_INTERFACE);
				} else if (keywordLists[5]->InList(s)) {
					sc.ChangeState(SCE_GROOVY_ENUM);
				} else if (keywordLists[6]->InList(s)) {
					sc.ChangeState(SCE_GROOVY_CONSTANT);
				} else if (sc.ch == ':') {
					if (sc.chNext == ':') {
						// type::method
						sc.ChangeState(SCE_GROOVY_CLASS);
					} else if (bracketCount == 0 && nestedState.empty() && visibleChars == sc.LengthCurrent()) {
						const int chNext = sc.GetDocNextChar(true);
						if (IsADigit(chNext) || chNext == '\'' || chNext == '\"') {
							sc.ChangeState(SCE_GROOVY_PROPERTY);
						} else {
							sc.ChangeState(SCE_GROOVY_LABEL);
						}
					} else {
						sc.ChangeState(SCE_GROOVY_PROPERTY);
					}
				} else if (sc.ch != '.') {
					if (kwType != SCE_GROOVY_DEFAULT && kwType != SCE_GROOVY_WORD) {
						sc.ChangeState(kwType);
					} else {
						const int chNext = sc.GetDocNextChar(sc.ch == ')');
						if (sc.ch == ')') {
							if (chBeforeIdentifier == '(' && (chNext == '(' || (kwType != SCE_GROOVY_WORD && IsIdentifierCharEx(chNext)))) {
								// (type)(expression)
								// (type)expression, (type)++identifier, (type)--identifier
								sc.ChangeState(SCE_GROOVY_CLASS);
							}
						} else if (chNext == '(' || IsADigit(chNext) || chNext == '\'' || chNext == '"') {
							// property value
							// method parameter
							sc.ChangeState(SCE_GROOVY_FUNCTION);
						} else if (sc.Match('[', ']') || sc.Match('.', '&')
							|| (sc.ch == '<' && (sc.chNext == '>' || sc.chNext == '?'))
							|| (chBeforeIdentifier == '<' && (chNext == '>' || chNext == '<'))) {
							// type[] identifier
							// type.&method
							// type<>, type<?>, type<? super T>
							// type<type>
							// type<type<type>>
							sc.ChangeState(SCE_GROOVY_CLASS);
						} else if (IsIdentifierStartEx(chNext)) {
							// type identifier
							// method parameter
							sc.ChangeState(IsLowerCase(s[0]) ? SCE_GROOVY_FUNCTION : SCE_GROOVY_CLASS);
						} else if (chNext == '{' && IsLowerCase(s[0])) {
							// block {}
							sc.ChangeState(SCE_GROOVY_ACTION);
						}
					}
				}
				stylePrevNonWhite = sc.state;
				if (sc.state != SCE_GROOVY_WORD && sc.ch != '.') {
					kwType = SCE_GROOVY_DEFAULT;
				}
				sc.SetState(SCE_GROOVY_DEFAULT);
			}
			break;

		case SCE_GROOVY_ANNOTATION:
		case SCE_GROOVY_ATTRIBUTE_AT:
			if (sc.ch == '.' && sc.state == SCE_GROOVY_ANNOTATION) {
				sc.SetState(SCE_GROOVY_OPERATOR);
				sc.ForwardSetState(SCE_GROOVY_ANNOTATION);
				continue;
			}
			if (!IsIdentifierCharEx(sc.ch)) {
				sc.SetState(SCE_GROOVY_DEFAULT);
			}
			break;

		case SCE_GROOVY_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_GROOVY_DEFAULT);
			} else {
				HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_GROOVY_TASKMARKER);
			}
			break;

		case SCE_GROOVY_COMMENTBLOCK:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_GROOVY_DEFAULT);
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_GROOVY_TASKMARKER)) {
				continue;
			}
			break;

		case SCE_GROOVY_COMMENTBLOCKDOC:
			switch (docTagState) {
			case DocTagState::At:
				docTagState = DocTagState::None;
				break;
			case DocTagState::InlineAt:
				if (sc.ch == '}') {
					docTagState = DocTagState::None;
					sc.SetState(SCE_GROOVY_COMMENTTAGAT);
					sc.ForwardSetState(SCE_GROOVY_COMMENTBLOCKDOC);
				}
				break;
			case DocTagState::TagOpen:
			case DocTagState::TagClose:
				if (sc.Match('/', '>') || sc.ch == '>') {
					docTagState = DocTagState::None;
					sc.SetState(SCE_GROOVY_COMMENTTAGHTML);
					sc.Forward((sc.ch == '/') ? 2 : 1);
					sc.SetState(SCE_GROOVY_COMMENTBLOCKDOC);
				}
				break;
			default:
				break;
			}
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_GROOVY_DEFAULT);
			} else if (sc.ch == '@' && IsAlpha(sc.chNext) && IsCommentTagPrev(sc.chPrev)) {
				docTagState = DocTagState::At;
				sc.SetState(SCE_GROOVY_COMMENTTAGAT);
			} else if (sc.Match('{', '@') && IsAlpha(sc.GetRelative(2))) {
				docTagState = DocTagState::InlineAt;
				sc.SetState(SCE_GROOVY_COMMENTTAGAT);
				sc.Forward();
			} else if (sc.ch == '<') {
				if (IsAlpha(sc.chNext)) {
					docTagState = DocTagState::TagOpen;
					sc.SetState(SCE_GROOVY_COMMENTTAGHTML);
				} else if (sc.chNext == '/' && IsAlpha(sc.GetRelative(2))) {
					docTagState = DocTagState::TagClose;
					sc.SetState(SCE_GROOVY_COMMENTTAGHTML);
					sc.Forward();
				}
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_GROOVY_TASKMARKER)) {
				continue;
			}
			break;

		case SCE_GROOVY_COMMENTTAGAT:
		case SCE_GROOVY_COMMENTTAGHTML:
			if (!(IsIdentifierChar(sc.ch) || sc.ch == '-' || sc.ch == ':')) {
				sc.SetState(SCE_GROOVY_COMMENTBLOCKDOC);
				continue;
			}
			break;

		case SCE_GROOVY_STRING_SQ:
		case SCE_GROOVY_TRIPLE_STRING_SQ:
			if (sc.state == SCE_GROOVY_STRING_SQ && sc.atLineStart) {
				sc.SetState(SCE_GROOVY_DEFAULT);
			} else if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_GROOVY_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '\'' && (sc.state == SCE_GROOVY_STRING_SQ || sc.MatchNext('\'', '\''))) {
				if (sc.state == SCE_GROOVY_TRIPLE_STRING_SQ) {
					sc.Forward(2);
				}
				sc.ForwardSetState(SCE_GROOVY_DEFAULT);
			}
			break;

		case SCE_GROOVY_STRING_DQ:
		case SCE_GROOVY_TRIPLE_STRING_DQ:
			if (sc.state == SCE_GROOVY_STRING_DQ && sc.atLineStart) {
				sc.SetState(SCE_GROOVY_DEFAULT);
			} else if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_GROOVY_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '$') {
				if (sc.chNext == '{') {
					nestedState.push_back(sc.state);
					sc.SetState(SCE_GROOVY_OPERATOR2);
					sc.Forward();
				} else if (IsIdentifierStartEx(sc.chNext)) {
					escSeq.outerState = sc.state;
					sc.SetState(SCE_GROOVY_VARIABLE);
				}
			} else if (sc.ch == '"' && (sc.state == SCE_GROOVY_STRING_DQ || sc.MatchNext('"', '"'))) {
				if (sc.state == SCE_GROOVY_TRIPLE_STRING_DQ) {
					sc.SetState(SCE_GROOVY_TRIPLE_STRING_DQEND);
					sc.Forward(2);
				}
				sc.ForwardSetState(SCE_GROOVY_DEFAULT);
			}
			break;

		case SCE_GROOVY_SLASHY_STRING:
			if (sc.Match('\\', '/')) {
				sc.SetState(SCE_GROOVY_ESCAPECHAR);
				sc.Forward();
				sc.ForwardSetState(SCE_GROOVY_SLASHY_STRING);
			}
			if (sc.ch == '$') {
				if (sc.chNext == '{') {
					nestedState.push_back(sc.state);
					sc.SetState(SCE_GROOVY_OPERATOR2);
					sc.Forward();
				} else if (IsIdentifierStartEx(sc.chNext)) {
					escSeq.outerState = sc.state;
					sc.SetState(SCE_GROOVY_VARIABLE);
				}
			} else if (sc.ch == '/') {
				sc.SetState(SCE_GROOVY_SLASHY_STRINGEND);
				sc.ForwardSetState(SCE_GROOVY_DEFAULT);
			}
			break;

		case SCE_GROOVY_DOLLAR_SLASHY:
			if (sc.ch == '$') {
				if (sc.chNext == '{') {
					nestedState.push_back(sc.state);
					sc.SetState(SCE_GROOVY_OPERATOR2);
					sc.Forward();
				} else if (IsIdentifierStartEx(sc.chNext)) {
					escSeq.outerState = sc.state;
					sc.SetState(SCE_GROOVY_VARIABLE);
				} else if (sc.chNext == '$' || sc.chNext == '/') {
					sc.SetState(SCE_GROOVY_ESCAPECHAR);
					sc.Forward();
					sc.ForwardSetState(SCE_GROOVY_DOLLAR_SLASHY);
					continue;
				}
			} else if (sc.Match('/', '$')) {
				sc.SetState(SCE_GROOVY_SLASHY_STRINGEND);
				sc.Forward();
				sc.ForwardSetState(SCE_GROOVY_DEFAULT);
			}
			break;

		case SCE_GROOVY_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_GROOVY_VARIABLE:
			if (!IsIdentifierCharEx(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;
		}

		if (sc.state == SCE_GROOVY_DEFAULT) {
			if (sc.ch == '/') {
				if (sc.chNext == '/') {
					visibleCharsBefore = visibleChars;
					sc.SetState(SCE_GROOVY_COMMENTLINE);
					if (visibleChars == 0) {
						lineStateLineType = GroovyLineStateMaskLineComment;
					}
				} else if (sc.chNext == '*') {
					visibleCharsBefore = visibleChars;
					docTagState = DocTagState::None;
					sc.SetState(SCE_GROOVY_COMMENTBLOCK);
					sc.Forward(2);
					if (sc.ch == '*' && sc.chNext != '*') {
						sc.ChangeState(SCE_GROOVY_COMMENTBLOCKDOC);
					}
					continue;
				} else {
					if (IsSlashyStringStart(chPrevNonWhite, stylePrevNonWhite)) {
						sc.SetState(SCE_GROOVY_SLASHY_STRINGSTART);
						sc.ForwardSetState(SCE_GROOVY_SLASHY_STRING);
						continue;
					}
					sc.SetState(SCE_GROOVY_OPERATOR);
				}
			} else if (sc.ch == '\"') {
				if (sc.MatchNext('"', '"')) {
					sc.SetState(SCE_GROOVY_TRIPLE_STRING_DQSTART);
					sc.Forward(2);
					sc.ForwardSetState(SCE_GROOVY_TRIPLE_STRING_DQ);
					continue;
				}
				sc.SetState(SCE_GROOVY_STRING_DQ);
			} else if (sc.ch == '\'') {
				if (sc.MatchNext('\'', '\'')) {
					sc.SetState(SCE_GROOVY_TRIPLE_STRING_SQ);
					sc.Forward(2);
				} else {
					sc.SetState(SCE_GROOVY_STRING_SQ);
				}
			} else if (sc.Match('$', '/')) {
				sc.SetState(SCE_GROOVY_SLASHY_STRINGSTART);
				sc.Forward();
				sc.ForwardSetState(SCE_GROOVY_DOLLAR_SLASHY);
			} else if (sc.ch == '+' || sc.ch == '-') {
				if (sc.ch == sc.chNext) {
					sc.SetState(SCE_GROOVY_OPERATOR_PF);
					sc.Forward();
				} else {
					sc.SetState(SCE_GROOVY_OPERATOR);
				}
			} else if (IsNumberStartEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_GROOVY_NUMBER);
			} else if (IsIdentifierStartEx(sc.ch)) {
				chBefore = sc.chPrev;
				if (chBefore != '.') {
					chBeforeIdentifier = chBefore;
				}
				sc.SetState(SCE_GROOVY_IDENTIFIER);
			} else if ((sc.ch == '@' || sc.ch == '$') && IsIdentifierStartEx(sc.chNext)) {
				escSeq.outerState = SCE_GROOVY_DEFAULT;
				const int state = (sc.ch == '$') ? SCE_GROOVY_VARIABLE
					: ((sc.chPrev == '.') ? SCE_GROOVY_ATTRIBUTE_AT
						: ((sc.chNext == 'i') ? SCE_GROOVY_IDENTIFIER : SCE_GROOVY_ANNOTATION));
				sc.SetState(state);
			} else if (isoperator(sc.ch)) {
				const bool interpolating = !nestedState.empty();
				sc.SetState(interpolating ? SCE_GROOVY_OPERATOR2 : SCE_GROOVY_OPERATOR);
				if (interpolating) {
					if (sc.ch == '{') {
						nestedState.push_back(SCE_GROOVY_DEFAULT);
					} else if (sc.ch == '}') {
						const int outerState = TakeAndPop(nestedState);
						sc.ForwardSetState(outerState);
						continue;
					}
				} else {
					if (sc.ch == '[' || sc.ch == '(') {
						++bracketCount;
					} else if (sc.ch == ']' || sc.ch == ')') {
						--bracketCount;
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
			int lineState = lineStateLineType | (bracketCount << 2);
			if (!nestedState.empty()) {
				lineState |= PackLineState(nestedState) << 8;
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineType = 0;
			visibleChars = 0;
			visibleCharsBefore = 0;
			docTagState = DocTagState::None;
			kwType = SCE_GROOVY_DEFAULT;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int packageImport;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & GroovyLineStateMaskLineComment),
		packageImport((lineState >> 1) & 1) {
	}
};

constexpr bool IsInnerStyle(int style) noexcept {
	return style == SCE_GROOVY_ESCAPECHAR
		|| style == SCE_GROOVY_COMMENTTAGAT || style == SCE_GROOVY_TASKMARKER;
}

void FoldGroovyDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_GROOVY_OPERATOR, SCE_GROOVY_TASKMARKER);
		if (bracePos) {
			startPos = bracePos + 1; // skip the brace
		}
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
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
		case SCE_GROOVY_COMMENTBLOCK:
		case SCE_GROOVY_COMMENTBLOCKDOC:
		case SCE_GROOVY_TRIPLE_STRING_SQ:
			if (style != stylePrev && !IsInnerStyle(stylePrev)) {
				levelNext++;
			} else if (style != styleNext && !IsInnerStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_GROOVY_TRIPLE_STRING_DQSTART:
		case SCE_GROOVY_SLASHY_STRINGSTART:
			if (style != stylePrev) {
				levelNext++;
			}
			break;

		case SCE_GROOVY_TRIPLE_STRING_DQEND:
		case SCE_GROOVY_SLASHY_STRINGEND:
			if (style != styleNext) {
				levelNext--;
			}
			break;

		case SCE_GROOVY_OPERATOR: {
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
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.packageImport) {
				levelNext += foldNext.packageImport - foldPrev.packageImport;
			} else if (visibleChars) {
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_GROOVY_OPERATOR, SCE_GROOVY_TASKMARKER);
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

LexerModule lmGroovy(SCLEX_GROOVY, ColouriseGroovyDoc, "groovy", FoldGroovyDoc);
