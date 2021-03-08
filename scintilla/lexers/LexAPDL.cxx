// Scintilla source code edit control
// code based on original lexers for APDL (by Hadar Raz) and ABAQUS (by Sergio Lucato and Gertjan Kloosterman).
// The License.txt file describes the conditions under which this software may be distributed.
//! Lexer for APDL, ABAQUS

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

using namespace Scintilla;

namespace {

#define FOLD_APDL_BY_INDENTATION	0

enum {
	APDLLineStateLineNone = 0,
	APDLLineStateLineComment = 1,
#if FOLD_APDL_BY_INDENTATION
	APDLLineStateEmptyLine = 2,
#else
	APDLLineStateDataLine = 2,
#endif
};

void ColouriseAPDLDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const bool apdl = styler.GetPropertyInt("lexer.apdl") & true;

	int visibleChars = 0;
#if FOLD_APDL_BY_INDENTATION
	int indentCount = 0;
#endif
	int lineStateLineType = APDLLineStateLineNone;
	int stringStartChar = 0;
	bool commandLine = false;
	int commandValue = 0;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);

	while (sc.More()) {
		switch (sc.state) {
		case SCE_APDL_OPERATOR:
			sc.SetState(SCE_APDL_DEFAULT);
			break;

		case SCE_APDL_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_APDL_DEFAULT);
			}
			break;

		case SCE_APDL_IDENTIFIER:
		case SCE_APDL_COMMAND:
		case SCE_APDL_SLASHCOMMAND:
		case SCE_APDL_STARCOMMAND:
			if (!(IsIdentifierChar(sc.ch)
				|| (sc.state == SCE_APDL_STARCOMMAND && sc.ch == '*')
				|| (commandValue && (sc.ch == '-' || sc.ch == '.')))) {
				const int chNext = sc.GetLineNextChar();
				if (sc.state == SCE_APDL_COMMAND) {
					if (!AnyOf(chNext, '\0', ',', '!', '*')) {
						sc.ChangeState(SCE_APDL_IDENTIFIER);
					}
				}
				if (sc.state == SCE_APDL_IDENTIFIER || sc.state == SCE_APDL_STARCOMMAND) {
					char s[128];
					sc.GetCurrentLowered(s, sizeof(s));
					if (sc.state == SCE_APDL_STARCOMMAND) {
						if (keywordLists[0]->InList(s + 1)) {
							// for code folding
							sc.ChangeState(SCE_APDL_WORD);
							if (StrEqual(s, "*end")) {
								// highlight next word as keyword
								commandValue = 2;
							}
						} else if (StrEqual(s, "*com")) {
							sc.ChangeState(SCE_APDL_COMMENT);
							lineStateLineType = APDLLineStateLineComment;
						}
					} else if (commandValue == 2) {
						sc.ChangeState(SCE_APDL_WORD);
						commandValue = 0;
					} else if (keywordLists[4]->InList(s)) {
						sc.ChangeState(SCE_APDL_ARGUMENT);
					} else if (chNext == '(' && keywordLists[5]->InListPrefixed(s, '(')) {
						sc.ChangeState(SCE_APDL_FUNCTION);
					}
				}
				if (commandLine && commandValue != 2) {
					if (sc.state == SCE_APDL_IDENTIFIER) {
						if (!commandValue && AnyOf(chNext, '\0', ',', '=', '*')) {
							sc.ChangeState(SCE_APDL_ARGUMENT);
						}
					}
					commandValue = chNext == '=';
				}
				if (sc.state != SCE_APDL_COMMENT) {
					sc.SetState(SCE_APDL_DEFAULT);
				}
			}
			break;

		case SCE_APDL_STRING:
			if (sc.atLineStart) {
				sc.SetState(SCE_APDL_DEFAULT);
			} else if (sc.ch == '\\') {
				sc.Forward();
			} else if (sc.ch == stringStartChar) {
				sc.ForwardSetState(SCE_APDL_DEFAULT);
			}
			break;

		case SCE_APDL_COMMENT:
			if (sc.atLineStart) {
				sc.SetState(SCE_APDL_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_APDL_DEFAULT) {
			if ((apdl && sc.ch == '!') || (!apdl && sc.Match('*', '*'))) {
				sc.SetState(SCE_APDL_COMMENT);
				if (visibleChars == 0) {
					lineStateLineType = APDLLineStateLineComment;
				}
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_APDL_NUMBER);
#if !FOLD_APDL_BY_INDENTATION
				if (visibleChars == 0) {
					lineStateLineType = APDLLineStateDataLine;
				}
#endif
				commandValue = 0;
			} else if (sc.ch == '"' || sc.ch == '\'') {
				sc.SetState(SCE_APDL_STRING);
				stringStartChar = sc.ch;
				commandValue = 0;
			} else if (visibleChars == 0 && (sc.ch == '/' || sc.ch == '*') && IsIdentifierStart(sc.chNext)) {
				sc.SetState((sc.ch == '/') ? SCE_APDL_SLASHCOMMAND : SCE_APDL_STARCOMMAND);
				commandLine = sc.ch == '*' && !apdl;
				commandValue = 0;
			} else if (IsIdentifierStart(sc.ch)) {
				sc.SetState((visibleChars == 0) ? SCE_APDL_COMMAND : SCE_APDL_IDENTIFIER);
			} else if (isoperator(sc.ch)) {
				sc.SetState(SCE_APDL_OPERATOR);
#if !FOLD_APDL_BY_INDENTATION
				if (sc.ch == '-' && visibleChars == 0 && (IsADigit(sc.chNext) || sc.chNext == '.')) {
					lineStateLineType = APDLLineStateDataLine;
				}
#endif
			}
		}

#if FOLD_APDL_BY_INDENTATION
		if (visibleChars == 0) {
			if (sc.ch == ' ' || sc.ch == '\t') {
				++indentCount;
			} else if (!isspacechar(sc.ch)) {
				++visibleChars;
			}
		}
#else
		if (visibleChars == 0 && !isspacechar(sc.ch)) {
			visibleChars++;
		}
#endif
		if (sc.atLineEnd) {
#if FOLD_APDL_BY_INDENTATION
			if (visibleChars == 0 && lineStateLineType == APDLLineStateLineNone) {
				lineStateLineType = APDLLineStateEmptyLine;
			}
			lineStateLineType |= indentCount << 8;
			indentCount = 0;
#endif
			styler.SetLineState(sc.currentLine, lineStateLineType);
			lineStateLineType = APDLLineStateLineNone;
			visibleChars = 0;
			commandLine = false;
			commandValue = 0;
		}
		sc.Forward();
	}

	sc.Complete();
}

#if FOLD_APDL_BY_INDENTATION
constexpr int GetLineType(int lineState) noexcept {
	return lineState & 3;
}

struct FoldLineState {
	int lineType;
	int indentCount;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineType(GetLineType(lineState)),
		indentCount(lineState >> 8) {
	}
	constexpr bool Empty() const noexcept {
		return lineType != APDLLineStateLineNone;
	}
};

// code folding based on LexYAML
void FoldAPDLDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList, Accessor &styler) {
	const Sci_Position maxPos = startPos + lengthDoc;
	const Sci_Line docLines = styler.GetLine(styler.Length());
	const Sci_Line maxLines = (maxPos == styler.Length()) ? docLines : styler.GetLine(maxPos - 1);

	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState stateCurrent(styler.GetLineState(lineCurrent));
	while (lineCurrent > 0) {
		lineCurrent--;
		stateCurrent = FoldLineState(styler.GetLineState(lineCurrent));
		if (!stateCurrent.Empty()) {
			break;
		}
	}

	while (lineCurrent <= maxLines) {
		Sci_Position lineNext = lineCurrent + 1;
		FoldLineState stateNext = stateCurrent;
		if (lineNext <= docLines) {
			stateNext = FoldLineState(styler.GetLineState(lineNext));
		}
		if (stateNext.Empty()) {
			stateNext.indentCount = stateCurrent.indentCount;
		}
		while ((lineNext < docLines) && stateNext.Empty()) {
			lineNext++;
			stateNext = FoldLineState(styler.GetLineState(lineNext));
		}

		int lev = stateCurrent.indentCount + SC_FOLDLEVELBASE;
		if (!stateCurrent.Empty()) {
			if (stateCurrent.indentCount < stateNext.indentCount) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
		}

		styler.SetLevel(lineCurrent, lev);
		lineCurrent++;

		const int levelAfterBlank = stateNext.indentCount;
		const int skipLevel = levelAfterBlank + SC_FOLDLEVELBASE;

		if (lineCurrent < lineNext) {
			int prevLineType = stateCurrent.lineType;
			int nextLineType = GetLineType(styler.GetLineState(lineCurrent));
			int prevLevel = skipLevel;
			// comment on first line
			if (prevLineType == APDLLineStateLineComment) {
				nextLineType = prevLineType;
				prevLineType = APDLLineStateLineNone;
				--lineCurrent;
			}
			for (; lineCurrent < lineNext; lineCurrent++) {
				int level = skipLevel;
				const int currentLineType = nextLineType;
				nextLineType = GetLineType(styler.GetLineState(lineCurrent + 1));
				if (currentLineType == APDLLineStateLineComment) {
					if (nextLineType == APDLLineStateLineComment && prevLineType != APDLLineStateLineComment) {
						level |= SC_FOLDLEVELHEADERFLAG;
					} else if (prevLevel & SC_FOLDLEVELHEADERFLAG) {
						level++;
					} else {
						level = prevLevel;
					}
				}

				styler.SetLevel(lineCurrent, level);
				prevLineType = currentLineType;
				prevLevel = level;
			}
		}

		stateCurrent = stateNext;
	}
}

#else
struct FoldLineState {
	int lineComment;
	int dataLine;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & APDLLineStateLineComment),
		dataLine((lineState >> 1) & 1) {
	}
};

void FoldAPDLDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList keywordLists, Accessor &styler) {
	const bool apdl = styler.GetPropertyInt("lexer.apdl") & true;

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

	int styleNext = styler.StyleAt(startPos);

	char buf[16];
	constexpr int MaxFoldWordLength = sizeof(buf) - 1;
	int wordLen = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		if (style == SCE_APDL_WORD) {
			if (wordLen < MaxFoldWordLength) {
				buf[wordLen++] = MakeLowerCase(styler[i]);
			}
			if (styleNext != SCE_APDL_WORD) {
				buf[wordLen] = '\0';
				wordLen = 0;
				if (StrStartsWith(buf, "*end")) {
					levelNext--;
				} else if (buf[0] == '*') {
					if ((apdl && (StrEqual(buf, "*if") || StrStartsWith(buf, "*do")))
						|| (!apdl && keywordLists[0]->InList(buf + 1))) {
						levelNext++;
					}
				}
			}
		}

		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.dataLine) {
				levelNext += foldNext.dataLine - foldPrev.dataLine;
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
#endif // FOLD_APDL_BY_INDENTATION

}

LexerModule lmAPDL(SCLEX_APDL, ColouriseAPDLDoc, "apdl", FoldAPDLDoc);
