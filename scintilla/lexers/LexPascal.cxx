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
#include <cctype>

#include <string>
#include <vector>
#include <map>

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

enum {
	stateInAsm = 0x1000,
	stateInProperty = 0x2000,
	stateInExport = 0x4000,
	stateFoldInPreprocessor = 0x0100,
	stateFoldInRecord = 0x0200,
	stateFoldInPreprocessorLevelMask = 0x00FF,
	stateFoldMaskAll = 0x0FFF
};

/*static const char * const pascalWordListDesc[] = {
	"Keywords",
	"Type",
	"Function",
	"Procedure",
	0
};*/

static void ClassifyPascalWord(LexerWordList keywordLists, StyleContext &sc, int &curLineState, bool bSmartHighlighting) {
	const WordList &keywords = *keywordLists[0];
	const WordList &typewords = *keywordLists[1];
	const WordList &funwords = *keywordLists[2];
	const WordList &prcwords = *keywordLists[3];
	char s[128];
	sc.GetCurrentLowered(s, sizeof(s));
	if (typewords.InList(s)) {
		sc.ChangeState(SCE_PAS_TYPE);
	}
	if (keywords.InList(s)) {
		if (curLineState & stateInAsm) {
			if (strcmp(s, "end") == 0 && sc.GetRelative(-4) != '@') {
				curLineState &= ~stateInAsm;
				sc.ChangeState(SCE_PAS_WORD);
			} else {
				sc.ChangeState(SCE_PAS_ASM);
			}
		} else {
			bool ignoreKeyword = false;
			if (strcmp(s, "asm") == 0) {
				curLineState |= stateInAsm;
			} else if (bSmartHighlighting) {
				if (strcmp(s, "property") == 0) {
					curLineState |= stateInProperty;
				} else if (strcmp(s, "exports") == 0) {
					curLineState |= stateInExport;
				} else if (!(curLineState & (stateInProperty | stateInExport)) && strcmp(s, "index") == 0) {
					ignoreKeyword = true;
				} else if (!(curLineState & stateInExport) && strcmp(s, "name") == 0) {
					ignoreKeyword = true;
				} else if (!(curLineState & stateInProperty) &&
					(strcmp(s, "read") == 0 || strcmp(s, "write") == 0 ||
						//					 strcmp(s, "default") == 0 || strcmp(s, "nodefault") == 0 ||
						strcmp(s, "stored") == 0 || strcmp(s, "implements") == 0 ||
						strcmp(s, "readonly") == 0 || strcmp(s, "writeonly") == 0 ||
						strcmp(s, "add") == 0 || strcmp(s, "remove") == 0)) {
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

static void ColourisePascalDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const bool bSmartHighlighting = styler.GetPropertyInt("lexer.pascal.smart.highlighting", 1) != 0;

	const CharacterSet setWordStart(CharacterSet::setAlpha, "_", 0x80, true);
	const CharacterSet setWord(CharacterSet::setAlphaNum, "_", 0x80, true);
	const CharacterSet setNumber(CharacterSet::setDigits, ".-+eE");
	const CharacterSet setHexNumber(CharacterSet::setDigits, "abcdefABCDEF");
	const CharacterSet setOperator(CharacterSet::setNone, "#$&'()*+,-./:;<=>@[]^{}");

	Sci_Position curLine = styler.GetLine(startPos);
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
			if (!setNumber.Contains(sc.ch) || (sc.ch == '.' && sc.chNext == '.')) {
				sc.SetState(SCE_PAS_DEFAULT);
			} else if (sc.ch == '-' || sc.ch == '+') {
				if (sc.chPrev != 'E' && sc.chPrev != 'e') {
					sc.SetState(SCE_PAS_DEFAULT);
				}
			}
			break;
		case SCE_PAS_IDENTIFIER:
			if (!setWord.Contains(sc.ch)) {
				ClassifyPascalWord(keywordLists, sc, curLineState, bSmartHighlighting);
			}
			break;
		case SCE_PAS_HEXNUMBER:
			if (!setHexNumber.Contains(sc.ch)) {
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
		case SCE_PAS_STRING:
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
			if (!setHexNumber.Contains(sc.ch) && sc.ch != '$') {
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
				if (sc.MatchIgnoreCase("0x")) {
					sc.SetState(SCE_PAS_HEXNUMBER);
					sc.Forward(2);
					while (IsHexDigit(sc.ch))
						sc.Forward();
				}
			} else if (setWordStart.Contains(sc.ch)) {
				sc.SetState(SCE_PAS_IDENTIFIER);
				//			} else if (sc.ch == '$' && !(curLineState & stateInAsm)) {
			} else if (sc.ch == '$') {
				sc.SetState(SCE_PAS_HEXNUMBER);
				if (curLineState & stateInAsm) {
					if (sc.MatchIgnoreCase("$0x"))
						sc.Forward(2);
				}
			} else if (sc.Match('{', '$')) {
				sc.SetState(SCE_PAS_PREPROCESSOR);
			} else if (sc.ch == '{') {
				sc.SetState(SCE_PAS_COMMENT);
			} else if (sc.Match("(*$")) {
				sc.SetState(SCE_PAS_PREPROCESSOR2);
			} else if (sc.Match('(', '*')) {
				sc.SetState(SCE_PAS_COMMENT2);
				sc.Forward();	// Eat the * so it isn't used for the end of the comment
			} else if (sc.Match('/', '/')) {
				sc.SetState(SCE_PAS_COMMENTLINE);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_PAS_STRING);
			} else if (sc.ch == '#') {
				if (curLineState & stateInAsm) {
					sc.SetState(SCE_PAS_HEXNUMBER);
					if (sc.MatchIgnoreCase("#0x"))
						sc.Forward(2);
				} else
					sc.SetState(SCE_PAS_CHARACTER);
			} else if (setOperator.Contains(sc.ch) && !(curLineState & stateInAsm)) {
				sc.SetState(SCE_PAS_OPERATOR);
			} else if (curLineState & stateInAsm) {
				sc.SetState(SCE_PAS_ASM);
			}
		}
	}

	if (sc.state == SCE_PAS_IDENTIFIER && setWord.Contains(sc.chPrev)) {
		ClassifyPascalWord(keywordLists, sc, curLineState, bSmartHighlighting);
	}

	sc.Complete();
}

static constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_PAS_COMMENT || style == SCE_PAS_COMMENT2;
}

static constexpr unsigned int GetFoldInPreprocessorLevelFlag(int lineFoldStateCurrent) noexcept {
	return lineFoldStateCurrent & stateFoldInPreprocessorLevelMask;
}

static void SetFoldInPreprocessorLevelFlag(int &lineFoldStateCurrent, unsigned int nestLevel) noexcept {
	lineFoldStateCurrent &= ~stateFoldInPreprocessorLevelMask;
	lineFoldStateCurrent |= nestLevel & stateFoldInPreprocessorLevelMask;
}

static void ClassifyPascalPreprocessorFoldPoint(int &levelCurrent, int &lineFoldStateCurrent,
	Sci_PositionU startPos, Accessor &styler) {
	const CharacterSet setWord(CharacterSet::setAlpha);

	char s[16];	// Size of the longest possible keyword + one additional character + null
	LexGetRangeLowered(startPos, styler, setWord, s, sizeof(s));

	unsigned int nestLevel = GetFoldInPreprocessorLevelFlag(lineFoldStateCurrent);

	if (strcmp(s, "if") == 0 ||
		strcmp(s, "ifdef") == 0 ||
		strcmp(s, "ifndef") == 0 ||
		strcmp(s, "ifopt") == 0 ||
		strcmp(s, "region") == 0) {
		nestLevel++;
		SetFoldInPreprocessorLevelFlag(lineFoldStateCurrent, nestLevel);
		lineFoldStateCurrent |= stateFoldInPreprocessor;
		levelCurrent++;
	} else if (strcmp(s, "endif") == 0 ||
		strcmp(s, "ifend") == 0 ||
		strcmp(s, "endregion") == 0) {
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

static void ClassifyPascalWordFoldPoint(int &levelCurrent, int &lineFoldStateCurrent,
	Sci_Position startPos, Sci_PositionU endPos,
	Sci_PositionU lastStart, Sci_PositionU currentPos, Accessor &styler) {
	char s[128];
	const CharacterSet setWordStart(CharacterSet::setAlpha, "_");
	const CharacterSet setWord(CharacterSet::setAlphaNum, "_");

	styler.GetRangeLowered(lastStart, currentPos + 1, s, sizeof(s));

	if (strcmp(s, "record") == 0) {
		lineFoldStateCurrent |= stateFoldInRecord;
		levelCurrent++;
	} else if (strcmp(s, "begin") == 0 ||
		strcmp(s, "asm") == 0 ||
		strcmp(s, "try") == 0 ||
		(strcmp(s, "case") == 0 && !(lineFoldStateCurrent & stateFoldInRecord))) {
		levelCurrent++;
	} else if (strcmp(s, "class") == 0 || strcmp(s, "object") == 0) {
		// "class" & "object" keywords require special handling...
		bool ignoreKeyword = false;
		Sci_PositionU j = LexSkipWhiteSpace(currentPos, endPos, styler, IsStreamCommentStyle);
		if (j < endPos) {

			if (styler.SafeGetCharAt(j) == ';') {
				// Handle forward class declarations ("type TMyClass = class;")
				// and object method declarations ("TNotifyEvent = procedure(Sender: TObject) of object;")
				ignoreKeyword = true;
			} else if (strcmp(s, "class") == 0) {
				// "class" keyword has a few more special cases...
				if (styler.SafeGetCharAt(j) == '(') {
					// Handle simplified complete class declarations ("type TMyClass = class(TObject);")
					j = LexSkipWhiteSpace(j, endPos, styler, IsStreamCommentStyle, setWord);
					if (j < endPos && styler.SafeGetCharAt(j) == ')') {
						j = LexSkipWhiteSpace(j, endPos, styler, IsStreamCommentStyle);
						if (j < endPos && styler.SafeGetCharAt(j) == ';') {
							ignoreKeyword = true;
						}
					}
				} else if (setWordStart.Contains(styler.SafeGetCharAt(j))) {
					char s2[16];	// Size of the longest possible keyword + one additional character + null
					LexGetRangeLowered(j, styler, setWord, s2, sizeof(s2));

					if (strcmp(s2, "procedure") == 0 ||
						strcmp(s2, "function") == 0 ||
						strcmp(s2, "of") == 0 ||
						strcmp(s2, "var") == 0 ||
						strcmp(s2, "property") == 0 ||
						strcmp(s2, "operator") == 0) {
						ignoreKeyword = true;
					}
				}
			}
		}
		if (!ignoreKeyword) {
			levelCurrent++;
		}
	} else if (strcmp(s, "interface") == 0) {
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
			const Sci_PositionU k = LexSkipWhiteSpace(currentPos, endPos, styler);
			if (k < endPos && styler.SafeGetCharAt(k) == ';') {
				// Handle forward interface declarations ("type IMyInterface = interface;")
				ignoreKeyword = true;
			}
		}
		if (!ignoreKeyword) {
			levelCurrent++;
		}
	} else if (strcmp(s, "dispinterface") == 0) {
		// "dispinterface" keyword requires special handling...
		bool ignoreKeyword = false;
		const Sci_PositionU j = LexSkipWhiteSpace(currentPos, endPos, styler);
		if (j < endPos && styler.SafeGetCharAt(j) == ';') {
			// Handle forward dispinterface declarations ("type IMyInterface = dispinterface;")
			ignoreKeyword = true;
		}
		if (!ignoreKeyword) {
			levelCurrent++;
		}
	} else if (strcmp(s, "end") == 0) {
		lineFoldStateCurrent &= ~stateFoldInRecord;
		levelCurrent--;
		if (levelCurrent < SC_FOLDLEVELBASE) {
			levelCurrent = SC_FOLDLEVELBASE;
		}
	}
}

#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_PAS_COMMENTLINE)

static void FoldPascalDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	const bool foldPreprocessor = styler.GetPropertyInt("fold.preprocessor") != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;
	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	int lineFoldStateCurrent = lineCurrent > 0 ? styler.GetLineState(lineCurrent - 1) & stateFoldMaskAll : 0;
	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	Sci_PositionU lastStart = 0;
	const CharacterSet setWord(CharacterSet::setAlphaNum, "_", 0x80, true);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (foldComment && IsStreamCommentStyle(style)) {
			if (!IsStreamCommentStyle(stylePrev)) {
				levelCurrent++;
			} else if (!IsStreamCommentStyle(styleNext) && !atEOL) {
				// Comments don't end at end of line and the next character may be unstyled.
				levelCurrent--;
			}
		}
		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelCurrent++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelCurrent--;
		}
		if (foldPreprocessor) {
			if (style == SCE_PAS_PREPROCESSOR && ch == '{' && chNext == '$') {
				ClassifyPascalPreprocessorFoldPoint(levelCurrent, lineFoldStateCurrent, i + 2, styler);
			} else if (style == SCE_PAS_PREPROCESSOR2 && ch == '(' && chNext == '*'
				&& styler.SafeGetCharAt(i + 2) == '$') {
				ClassifyPascalPreprocessorFoldPoint(levelCurrent, lineFoldStateCurrent, i + 3, styler);
			}
		}

		if (stylePrev != SCE_PAS_WORD && style == SCE_PAS_WORD) {
			// Store last word start point.
			lastStart = i;
		}
		if (stylePrev == SCE_PAS_WORD && !(lineFoldStateCurrent & stateFoldInPreprocessor)) {
			if (setWord.Contains(ch) && !setWord.Contains(chNext)) {
				ClassifyPascalWordFoldPoint(levelCurrent, lineFoldStateCurrent, startPos, endPos, lastStart, i, styler);
			}
		}

		if (!isspacechar(ch))
			visibleChars++;

		if (atEOL) {
			int lev = levelPrev;
			if (visibleChars == 0 && foldCompact)
				lev |= SC_FOLDLEVELWHITEFLAG;
			if ((levelCurrent > levelPrev) && (visibleChars > 0))
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			const int newLineState = (styler.GetLineState(lineCurrent) & ~stateFoldMaskAll) | lineFoldStateCurrent;
			styler.SetLineState(lineCurrent, newLineState);
			lineCurrent++;
			levelPrev = levelCurrent;
			visibleChars = 0;
		}
	}

	// If we didn't reach the EOL in previous loop, store line level and whitespace information.
	// The rest will be filled in later...
	int lev = levelPrev;
	if (visibleChars == 0 && foldCompact)
		lev |= SC_FOLDLEVELWHITEFLAG;
	styler.SetLevel(lineCurrent, lev);
}

LexerModule lmPascal(SCLEX_PASCAL, ColourisePascalDoc, "pascal", FoldPascalDoc);
