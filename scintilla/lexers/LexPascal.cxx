// Scintilla source code edit control
/** @file LexPascal.cxx
 ** Lexer for Pascal.
 ** Written by Laurent le Tynevez
 ** Updated by Simon Steele <s.steele@pnotepad.org> September 2002
 ** Updated by Mathias Rauen <scite@madshi.net> May 2003 (Delphi adjustments)
 ** Completely rewritten by Marko Njezic <sf@maxempire.com> October 2008
 **/

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

enum {
	stateInAsm = 0x1000,
	stateInProperty = 0x2000,
	stateInExport = 0x4000,
	stateFoldInPreprocessor = 0x0100,
	stateFoldInRecord = 0x0200,
	stateFoldInPreprocessorLevelMask = 0x00FF,
	stateFoldMaskAll = 0x0FFF
};

/*const char * const pascalWordListDesc[] = {
	"Keywords",
	"Type",
	"Function",
	"Procedure",
	0
};*/

void ClassifyPascalWord(LexerWordList keywordLists, StyleContext &sc, int &curLineState, bool bSmartHighlighting) {
	const WordList &keywords = keywordLists[0];
	const WordList &typewords = keywordLists[1];
	const WordList &funwords = keywordLists[2];
	const WordList &prcwords = keywordLists[3];
	char s[128];
	sc.GetCurrentLowered(s, sizeof(s));
	if (typewords.InList(s)) {
		sc.ChangeState(SCE_PAS_TYPE);
	}
	if (keywords.InList(s)) {
		if (curLineState & stateInAsm) {
			if (StrEqual(s, "end") && sc.GetRelative(-4) != '@') {
				curLineState &= ~stateInAsm;
				sc.ChangeState(SCE_PAS_WORD);
			} else {
				sc.ChangeState(SCE_PAS_ASM);
			}
		} else {
			bool ignoreKeyword = false;
			if (StrEqual(s, "asm")) {
				curLineState |= stateInAsm;
			} else if (bSmartHighlighting) {
				if (StrEqual(s, "property")) {
					curLineState |= stateInProperty;
				} else if (StrEqual(s, "exports")) {
					curLineState |= stateInExport;
				} else if (!(curLineState & (stateInProperty | stateInExport)) && StrEqual(s, "index")) {
					ignoreKeyword = true;
				} else if (!(curLineState & stateInExport) && StrEqual(s, "name")) {
					ignoreKeyword = true;
				} else if (!(curLineState & stateInProperty) &&
					(StrEqualsAny(s, "read", "write", /*"default", "nodefault",*/ "stored", "implements", "readonly", "writeonly", "add", "remove"))) {
					ignoreKeyword = true;
				}
			}
			if (!ignoreKeyword) {
				sc.ChangeState(SCE_PAS_WORD);
			}
		}
	} else if (curLineState & stateInAsm) {
		sc.ChangeState(SCE_PAS_ASM);
	}
	while (!sc.atLineEnd && sc.ch == ' ')
		sc.Forward();
	if (sc.ch == '(') {
		if (funwords.InList(s) || prcwords.InList(s)) {
			sc.ChangeState(SCE_PAS_FUNCTION1);
		} else {
			sc.ChangeState(SCE_PAS_FUNCTION);
		}
	}

	sc.SetState(SCE_PAS_DEFAULT);
}

constexpr bool IsPascalOperator(int ch) noexcept {
	return AnyOf(ch, '#', '$', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', '<', '=', '>', '@', '[', ']', '^', '{', '}');
}

void ColourisePascalDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	constexpr bool bSmartHighlighting = true; //styler.GetPropertyBool("lexer.pascal.smart.highlighting", true);

	Sci_Line curLine = styler.GetLine(startPos);
	int curLineState = curLine > 0 ? styler.GetLineState(curLine - 1) : 0;

	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {
		if (sc.atLineEnd) {
			// Update the line state, so it can be seen by next line
			curLine = styler.GetLine(sc.currentPos);
			styler.SetLineState(curLine, curLineState);
		}

		// Determine if the current state should terminate.
		switch (sc.state) {
		case SCE_PAS_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_PAS_DEFAULT);
			}
			break;
		case SCE_PAS_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				ClassifyPascalWord(keywordLists, sc, curLineState, bSmartHighlighting);
			}
			break;
		case SCE_PAS_HEXNUMBER:
			if (!IsHexDigit(sc.ch)) {
				sc.SetState(SCE_PAS_DEFAULT);
			}
			break;
		case SCE_PAS_COMMENT:
		case SCE_PAS_PREPROCESSOR:
			if (sc.ch == '}') {
				sc.ForwardSetState(SCE_PAS_DEFAULT);
			}
			break;
		case SCE_PAS_COMMENT2:
		case SCE_PAS_PREPROCESSOR2:
			if (sc.Match('*', ')')) {
				sc.Forward();
				sc.ForwardSetState(SCE_PAS_DEFAULT);
			}
			break;
		case SCE_PAS_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_PAS_DEFAULT);
			}
			break;
		case SCE_PAS_STRING_DQ:
			if (sc.atLineEnd) {
				sc.ChangeState(SCE_PAS_STRINGEOL);
			} else if (sc.ch == '\'' && sc.chNext == '\'') {
				sc.Forward();
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_PAS_DEFAULT);
			}
			break;
		case SCE_PAS_STRINGEOL:
			if (sc.atLineStart) {
				sc.SetState(SCE_PAS_DEFAULT);
			}
			break;
		case SCE_PAS_CHARACTER:
			if (!IsHexDigit(sc.ch) && sc.ch != '$') {
				sc.SetState(SCE_PAS_DEFAULT);
			}
			break;
		case SCE_PAS_OPERATOR:
			if (bSmartHighlighting && sc.chPrev == ';') {
				curLineState &= ~(stateInProperty | stateInExport);
			}
			sc.SetState(SCE_PAS_DEFAULT);
			break;
		case SCE_PAS_ASM:
			sc.SetState(SCE_PAS_DEFAULT);
			break;
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_PAS_DEFAULT) {
			//			if (IsADigit(sc.ch) && !(curLineState & stateInAsm)) {
			if (IsADigit(sc.ch)) {
				sc.SetState(SCE_PAS_NUMBER);
				if (sc.ch == '0' && (sc.chNext =='x' || sc.chNext == 'X')) {
					sc.SetState(SCE_PAS_HEXNUMBER);
					sc.Advance(2);
					while (IsHexDigit(sc.ch))
						sc.Forward();
				}
			} else if (IsIdentifierStartEx(sc.ch)) {
				sc.SetState(SCE_PAS_IDENTIFIER);
				//			} else if (sc.ch == '$' && !(curLineState & stateInAsm)) {
			} else if (sc.ch == '$') {
				sc.SetState(SCE_PAS_HEXNUMBER);
				if (curLineState & stateInAsm) {
					if (sc.chNext == '0' && styler.MatchLower(sc.currentPos + 2, 'x'))
						sc.Advance(2);
				}
			} else if (sc.Match('{', '$')) {
				sc.SetState(SCE_PAS_PREPROCESSOR);
			} else if (sc.ch == '{') {
				sc.SetState(SCE_PAS_COMMENT);
			} else if (sc.Match('(', '*', '$')) {
				sc.SetState(SCE_PAS_PREPROCESSOR2);
			} else if (sc.Match('(', '*')) {
				sc.SetState(SCE_PAS_COMMENT2);
				sc.Forward();	// Eat the * so it isn't used for the end of the comment
			} else if (sc.Match('/', '/')) {
				sc.SetState(SCE_PAS_COMMENTLINE);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_PAS_STRING_DQ);
			} else if (sc.ch == '#') {
				if (curLineState & stateInAsm) {
					sc.SetState(SCE_PAS_HEXNUMBER);
					if (sc.chNext == '0' && styler.MatchLower(sc.currentPos + 2, 'x'))
						sc.Advance(2);
				} else
					sc.SetState(SCE_PAS_CHARACTER);
			} else if (IsPascalOperator(sc.ch) && !(curLineState & stateInAsm)) {
				sc.SetState(SCE_PAS_OPERATOR);
			} else if (curLineState & stateInAsm) {
				sc.SetState(SCE_PAS_ASM);
			}
		}
	}

	if (sc.state == SCE_PAS_IDENTIFIER && IsIdentifierCharEx(sc.chPrev)) {
		ClassifyPascalWord(keywordLists, sc, curLineState, bSmartHighlighting);
	}

	sc.Complete();
}

constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_PAS_COMMENT || style == SCE_PAS_COMMENT2;
}

constexpr unsigned int GetFoldInPreprocessorLevelFlag(int lineFoldStateCurrent) noexcept {
	return lineFoldStateCurrent & stateFoldInPreprocessorLevelMask;
}

void SetFoldInPreprocessorLevelFlag(int &lineFoldStateCurrent, unsigned int nestLevel) noexcept {
	lineFoldStateCurrent &= ~stateFoldInPreprocessorLevelMask;
	lineFoldStateCurrent |= nestLevel & stateFoldInPreprocessorLevelMask;
}

void ClassifyPascalPreprocessorFoldPoint(int &levelCurrent, int &lineFoldStateCurrent,
	Sci_PositionU startPos, Accessor &styler) noexcept {
	const CharacterSet setWord(CharacterSet::setAlpha);

	char s[16];	// Size of the longest possible keyword + one additional character + null
	LexGetRangeLowered(styler, startPos, setWord, s, sizeof(s));

	unsigned int nestLevel = GetFoldInPreprocessorLevelFlag(lineFoldStateCurrent);

	if (StrEqualsAny(s, "if", "ifdef", "ifndef", "ifopt", "region")) {
		nestLevel++;
		SetFoldInPreprocessorLevelFlag(lineFoldStateCurrent, nestLevel);
		lineFoldStateCurrent |= stateFoldInPreprocessor;
		levelCurrent++;
	} else if (StrEqualsAny(s, "endif", "ifend", "endregion")) {
		nestLevel--;
		SetFoldInPreprocessorLevelFlag(lineFoldStateCurrent, nestLevel);
		if (nestLevel == 0) {
			lineFoldStateCurrent &= ~stateFoldInPreprocessor;
		}
		levelCurrent--;
		if (levelCurrent < SC_FOLDLEVELBASE) {
			levelCurrent = SC_FOLDLEVELBASE;
		}
	}
}

void ClassifyPascalWordFoldPoint(const CharacterSet &setWord, int &levelCurrent, int &lineFoldStateCurrent,
	Sci_Position startPos, Sci_PositionU endPos,
	Sci_PositionU lastStart, Sci_PositionU currentPos, Accessor &styler) noexcept {
	char s[128];

	styler.GetRangeLowered(lastStart, currentPos + 1, s, sizeof(s));

	if (StrEqual(s, "record")) {
		lineFoldStateCurrent |= stateFoldInRecord;
		levelCurrent++;
	} else if (StrEqualsAny(s, "begin", "asm", "try") ||
		(StrEqual(s, "case") && !(lineFoldStateCurrent & stateFoldInRecord))) {
		levelCurrent++;
	} else if (StrEqualsAny(s, "class", "object")) {
		// "class" & "object" keywords require special handling...
		bool ignoreKeyword = false;
		Sci_PositionU j = LexSkipWhiteSpace(styler, currentPos, endPos, IsStreamCommentStyle);
		if (j < endPos) {

			if (styler.SafeGetCharAt(j) == ';') {
				// Handle forward class declarations ("type TMyClass = class;")
				// and object method declarations ("TNotifyEvent = procedure(Sender: TObject) of object;")
				ignoreKeyword = true;
			} else if (StrEqual(s, "class")) {
				// "class" keyword has a few more special cases...
				if (styler.SafeGetCharAt(j) == '(') {
					// Handle simplified complete class declarations ("type TMyClass = class(TObject);")
					j = LexSkipWhiteSpace(styler, j, endPos, IsStreamCommentStyle, setWord);
					if (j < endPos && styler.SafeGetCharAt(j) == ')') {
						j = LexSkipWhiteSpace(styler, j, endPos, IsStreamCommentStyle);
						if (j < endPos && styler.SafeGetCharAt(j) == ';') {
							ignoreKeyword = true;
						}
					}
				} else if (IsAlpha(styler.SafeGetCharAt(j))) {
					char s2[16];	// Size of the longest possible keyword + one additional character + null
					LexGetRangeLowered(styler, j, setWord, s2, sizeof(s2));

					if (StrEqualsAny(s2, "procedure", "function", "of", "var", "property", "operator")) {
						ignoreKeyword = true;
					}
				}
			}
		}
		if (!ignoreKeyword) {
			levelCurrent++;
		}
	} else if (StrEqual(s, "interface")) {
		// "interface" keyword requires special handling...
		bool ignoreKeyword = true;
		Sci_Position j = lastStart - 1;
		char ch = styler.SafeGetCharAt(j);
		while ((j >= startPos) && (IsASpaceOrTab(ch) || ch == '\r' || ch == '\n' ||
			IsStreamCommentStyle(styler.StyleAt(j)))) {
			j--;
			ch = styler.SafeGetCharAt(j);
		}
		if (j >= startPos && styler.SafeGetCharAt(j) == '=') {
			ignoreKeyword = false;
		}
		if (!ignoreKeyword) {
			const Sci_PositionU k = LexSkipWhiteSpace(styler, currentPos, endPos);
			if (k < endPos && styler.SafeGetCharAt(k) == ';') {
				// Handle forward interface declarations ("type IMyInterface = interface;")
				ignoreKeyword = true;
			}
		}
		if (!ignoreKeyword) {
			levelCurrent++;
		}
	} else if (StrEqual(s, "dispinterface")) {
		// "dispinterface" keyword requires special handling...
		bool ignoreKeyword = false;
		const Sci_PositionU j = LexSkipWhiteSpace(styler, currentPos, endPos);
		if (j < endPos && styler.SafeGetCharAt(j) == ';') {
			// Handle forward dispinterface declarations ("type IMyInterface = dispinterface;")
			ignoreKeyword = true;
		}
		if (!ignoreKeyword) {
			levelCurrent++;
		}
	} else if (StrEqual(s, "end")) {
		lineFoldStateCurrent &= ~stateFoldInRecord;
		levelCurrent--;
		if (levelCurrent < SC_FOLDLEVELBASE) {
			levelCurrent = SC_FOLDLEVELBASE;
		}
	}
}

#define IsCommentLine(line)		IsLexCommentLine(styler, line, SCE_PAS_COMMENTLINE)

void FoldPascalDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + length;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	int lineFoldStateCurrent = lineCurrent > 0 ? styler.GetLineState(lineCurrent - 1) & stateFoldMaskAll : 0;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	Sci_PositionU lastStart = 0;
	const CharacterSet setWord(CharacterSet::setAlphaNum, "_", true);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (IsStreamCommentStyle(style)) {
			if (!IsStreamCommentStyle(stylePrev)) {
				levelCurrent++;
			} else if (!IsStreamCommentStyle(styleNext) && !atEOL) {
				// Comments don't end at end of line and the next character may be unstyled.
				levelCurrent--;
			}
		}
		if (atEOL && IsCommentLine(lineCurrent)) {
			levelCurrent += IsCommentLine(lineCurrent + 1) - IsCommentLine(lineCurrent - 1);
		}
		if (style == SCE_PAS_PREPROCESSOR) {
			if (ch == '{' && chNext == '$') {
				ClassifyPascalPreprocessorFoldPoint(levelCurrent, lineFoldStateCurrent, i + 2, styler);
			} else if (ch == '(' && chNext == '*' && styler.SafeGetCharAt(i + 2) == '$') {
				ClassifyPascalPreprocessorFoldPoint(levelCurrent, lineFoldStateCurrent, i + 3, styler);
			}
		}

		if (stylePrev != SCE_PAS_WORD && style == SCE_PAS_WORD) {
			// Store last word start point.
			lastStart = i;
		}
		if (stylePrev == SCE_PAS_WORD && !(lineFoldStateCurrent & stateFoldInPreprocessor)) {
			if (setWord.Contains(ch) && !setWord.Contains(chNext)) {
				ClassifyPascalWordFoldPoint(setWord, levelCurrent, lineFoldStateCurrent, startPos, endPos, lastStart, i, styler);
			}
		}

		if (atEOL) {
			int lev = levelPrev;
			if ((levelCurrent > levelPrev))
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			const int newLineState = (styler.GetLineState(lineCurrent) & ~stateFoldMaskAll) | lineFoldStateCurrent;
			styler.SetLineState(lineCurrent, newLineState);
			lineCurrent++;
			levelPrev = levelCurrent;
		}
	}

	// If we didn't reach the EOL in previous loop, store line level and whitespace information.
	// The rest will be filled in later...
	styler.SetLevel(lineCurrent, levelPrev);
}

}

LexerModule lmPascal(SCLEX_PASCAL, ColourisePascalDoc, "pascal", FoldPascalDoc);
