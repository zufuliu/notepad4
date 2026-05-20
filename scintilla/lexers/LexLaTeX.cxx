// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
//! Lexer for LaTeX.

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

enum class Command {
	None,
	CatCode,	// catcode, sfcode, uccode, char, newlinechar
	Begin,
	End,
	TitleToc,
	TitleText,
};

constexpr bool IsLSpecial(int ch) noexcept {
	return AnyOf(ch, '#', '\\', '$', '%', '&', '^', '_', '{', '}', '~');
}

constexpr bool IsLigature(int ch, int chNext) noexcept {
	return (chNext == '`' && AnyOf(ch, '!', '?'))
		|| (ch == chNext && AnyOf(ch, '`', '\'', '-'));
}

int CheckBlockType(LexAccessor &styler, Sci_PositionU startPos, Sci_PositionU endPos, int state) noexcept {
	char s[16]{};
	++startPos;
	styler.GetRange(startPos, endPos, s, sizeof(s));
	char *p = s;
	if (state != SCE_L_TAG) {
		if (!StrStartsWith(p, "nd{")) {
			return SCE_L_DEFAULT;
		}
		p += 3;
		char *t = p;
		while (*t > ' ') {
			if (*t == '}') {
				*t = '\0';
			}
			++t;
		}
	}
	if (StrEqual(p, "comment")) {
		return SCE_L_COMMENT2;
	}
	if (StrStartsWith(p, "verbatim") && (p[8] == '\0' || p[8] == '*')) {
		return SCE_L_VERBATIM2;
	}
	if (StrEqual(p, "lstlisting")) {
		return SCE_L_LISTCODE;
	}
	return SCE_L_DEFAULT;
}

void ColouriseLaTeXDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList /*keywordLists*/, Accessor &styler) {
	int visibleChars = 0;
	int chPrevNonWhite = 0;
	int stylePrevNonWhite = SCE_L_DEFAULT;
	int verbDelimiter = 0;
	uint32_t lineState = 0;
	int blockDelta = 0;
	int headerLevel = 0;
	int outerState = SCE_L_DEFAULT;
	int headerBrace = 0;
	Command command = Command::None;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		lineState = styler.GetLineState(sc.currentLine - 1);
		headerBrace = lineState >> 16;
		lineState = 0;
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_L_COMMENT:
			if (sc.atLineStart) {
				sc.SetState(SCE_L_DEFAULT);
			}
			break;

		case SCE_L_COMMAND:
		case SCE_L_MARCO:
			if (!IsAlphaNumeric(sc.ch)) {
				if (sc.state == SCE_L_COMMAND) {
					char s[16];
					sc.GetCurrent(s, sizeof(s));
					command = Command::None;
					Sci_PositionU startSeg = styler.GetStartSegment();
					const size_t len = sci::min(sizeof(s) - 1, sc.currentPos - startSeg);
					// \catcode123
					constexpr size_t catLen = CStrLen("\\catcode") + 4;
					if (len < catLen && IsADigit(sc.chPrev) && StrStartsWith(s, "\\catcode")) {
						sc.ChangeState(SCE_L_CATCODE);
						startSeg += catLen - 4;
						styler.ColorTo(startSeg, SCE_L_COMMAND);
					} else {
						if (sc.ch == '*') {
							sc.Forward();
						}
						if (StrEndsWith(s, len, "code") || StrEndsWith(s, len, "char")) {
							command = Command::CatCode;
						} else {
							const char * const p = s + 1;
							if (StrEqual(p, "begin")) {
								blockDelta += 1;
								command = Command::Begin;
							} else if (StrEqual(p, "end")) {
								blockDelta -= 1;
								command = Command::End;
							} else if (StrEqual(p, "chapter")) {
								headerLevel = 2;
								command = Command::TitleToc;
							} else if (StrEqual(p, "section")) {
								headerLevel = 3;
								command = Command::TitleToc;
							} else if (StrEqual(p, "subsection")) {
								headerLevel = 4;
								command = Command::TitleToc;
							} else if (StrEqual(p, "subsubsection")) {
								headerLevel = 5;
								command = Command::TitleToc;
							} else if (StrEqualsAny(p, "title", "caption", "part")) {
								headerLevel = 1;
								command = Command::TitleToc;
							} else if (StrEqual(p, "verb")) {
								verbDelimiter = sc.ch;
								sc.SetState(SCE_L_VERBATIM);
								break;
							}
						}
					}
				}
				sc.SetState(outerState);
				continue;
			}
			break;

		case SCE_L_MARCO_PARAM:
			if (!IsADigit(sc.ch)) {
				sc.SetState(outerState);
				continue;
			}
			break;

		case SCE_L_OPT_PARAM:
		case SCE_L_TITLE_TOC:
		case SCE_L_TITLE:
		case SCE_L_CHAPTER:
		case SCE_L_SECTION:
		case SCE_L_SECTION1:
		case SCE_L_SECTION2:
			if (sc.ch == ']' && AnyOf(sc.state, SCE_L_OPT_PARAM, SCE_L_TITLE_TOC)) {
				outerState = SCE_L_DEFAULT;
				if (sc.state == SCE_L_TITLE_TOC && headerLevel != 0) {
					command = Command::TitleText;
				}
				sc.SetState(SCE_L_DEFAULT);
			} else if (IsLSpecial(sc.ch) || IsLigature(sc.ch, sc.chNext)) {
				if (sc.state == SCE_L_OPT_PARAM) {
					// Prefer inner highlighting inside optional command argument
					sc.ChangeState(SCE_L_DEFAULT);
				}
				outerState = sc.state;
			}
			break;

		case SCE_L_VERBATIM: // \verb
			if (sc.atLineEnd || sc.ch == verbDelimiter) {
				if (!sc.atLineEnd) {
					sc.Forward();
				}
				sc.SetState(outerState);
				continue;
			}
			break;

		case SCE_L_TAG: // \begin{}
		case SCE_L_TAG2:// \end{}
			if (sc.ch == '}') {
				int state = SCE_L_DEFAULT;
				if (sc.state == SCE_L_TAG) {
					state = CheckBlockType(styler, styler.GetStartSegment(), sc.currentPos, sc.state);
				}
				sc.ForwardSetState(state);
			}
			break;

		case SCE_L_COMMENT2:
		case SCE_L_VERBATIM2:
		case SCE_L_LISTCODE:
			if (sc.Match('\\', 'e')) {
				const int state = CheckBlockType(styler, sc.currentPos + 1, sc.lineStartNext, sc.state);
				if (state == sc.state) {
					sc.SetState(SCE_L_COMMAND);
				}
			}
			break;

		case SCE_L_OPERATOR:
		case SCE_L_SPECIAL:
		case SCE_L_CATCODE:
		case SCE_L_MATH:
		case SCE_L_MATH2:
			sc.SetState(outerState);
			continue;
		}

		if (sc.state == SCE_L_DEFAULT || sc.state >= SCE_L_TITLE_TOC) {
			if (sc.ch == '\\') {
				if (IsAlpha(sc.chNext)) {
					sc.SetState(SCE_L_COMMAND);
				} else {
					int state = SCE_L_SPECIAL;
					if (sc.chNext == '\\') {
						state = SCE_L_OPERATOR;
					} else if (AnyOf<'(', ')'>(sc.chNext)) {
						state = SCE_L_MATH;
					} else if (AnyOf<'[', ']'>(sc.chNext)) {
						state = SCE_L_MATH2;
						blockDelta += ('[' + ']')/2 - sc.chNext;
					}
					sc.SetState(state);
					if (!IsEOLChar(sc.ch)) {
						sc.Forward();
						if (sc.ch == '@' && IsAlpha(sc.chNext)) {
							sc.ChangeState(SCE_L_MARCO);
						} else if (sc.Match('\\', '*')) {
							sc.Forward();
						}
					}
				}
			} else if (sc.ch == '$') {
				sc.SetState(SCE_L_MATH);
				if (sc.chNext == '$') { // not recommended: $$ $$
					sc.ChangeState(SCE_L_MATH2);
					sc.Forward();
				}
			} else if (sc.ch == '%') {
				sc.SetState(SCE_L_COMMENT);
				if (visibleChars == 0) {
					lineState = SimpleLineStateMaskLineComment;
				}
			} else if (sc.ch == '#') {
				sc.SetState(SCE_L_MARCO_PARAM);
			} else if (IsLigature(sc.ch, sc.chNext)) {
				sc.SetState(SCE_L_SPECIAL);
				sc.Forward();
				if (sc.Match('-', '-')) { // em-dash
					sc.Forward();
				}
			} else if (AnyOf(sc.ch, '&', '^', '_', '{', '}', '~') || sc.Match('@', '{')
				|| (sc.state == SCE_L_DEFAULT && AnyOf<'[', ']'>(sc.ch)) // optional parameter
				|| (sc.ch == '=' && AnyOf(stylePrevNonWhite, SCE_L_COMMAND, SCE_L_CATCODE))) {
				int state = SCE_L_OPERATOR;
				if (sc.ch == '{' && sc.state == SCE_L_DEFAULT) {
					if (stylePrevNonWhite == SCE_L_COMMAND && AnyOf(command, Command::Begin, Command::End)) {
						state = (command == Command::Begin) ? SCE_L_TAG : SCE_L_TAG2;
						command = Command::None;
					} else if ((stylePrevNonWhite == SCE_L_COMMAND && command == Command::TitleToc)
						|| (chPrevNonWhite == ']' && command == Command::TitleText)) {
						command = Command::None;
						headerBrace = 1;
						outerState = headerLevel + SCE_L_TITLE - 1;
					}
				} else if (sc.ch == '[' && stylePrevNonWhite == SCE_L_COMMAND) {
					outerState = (command == Command::TitleToc) ? SCE_L_TITLE_TOC : SCE_L_OPT_PARAM;
					command = Command::None;
				}
				if (state == SCE_L_OPERATOR && AnyOf<'{', '}'>(sc.ch)) {
					blockDelta += ('{' + '}')/2 - sc.ch;
					if (sc.state >= SCE_L_TITLE) {
						headerBrace += ('{' + '}')/2 - sc.ch;
						if (headerBrace <= 0) {
							headerBrace = 0;
							outerState = SCE_L_DEFAULT;
						}
					}
				}
				sc.SetState(state);
			} else if (sc.ch == '`' && command == Command::CatCode && stylePrevNonWhite == SCE_L_COMMAND) {
				if (!IsEOLChar(sc.chNext)) {
					sc.SetState(SCE_L_CATCODE);
					sc.Forward();
					// \catcode`^^M =, \catcode`\^^M =
					if (sc.Match('^', '^') || (sc.ch == '\\' && !IsEOLChar(sc.chNext))) {
						sc.Forward();
						if (sc.chPrev == '\\' && sc.Match('^', '^')) {
							sc.Forward();
						}
						if (sc.chPrev == '^' && sc.ch == '^' && !IsEOLChar(sc.chNext)) {
							sc.Forward();
						}
					}
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
			chPrevNonWhite = sc.ch;
			stylePrevNonWhite = sc.state;
		}
		if (sc.atLineEnd) {
			if (sc.state == SCE_L_OPT_PARAM) {
				sc.ChangeState(SCE_L_DEFAULT);
			}
			lineState |= (static_cast<uint32_t>(headerBrace) << 16);
			if (headerLevel > 1) {
				lineState = (headerLevel - 1) << 4;
			}
			if (blockDelta != 0) {
				blockDelta = sci::clamp(blockDelta + 128, 1, 255);
				lineState |= static_cast<uint32_t>(blockDelta) << 8;
				blockDelta = 0;
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineState = 0;
			headerLevel = 0;
			outerState = SCE_L_DEFAULT;
			visibleChars = 0;
			command = Command::None;
		}
		sc.Forward();
	}

	sc.Complete();
}

void FoldLaTeXDoc(Sci_PositionU startPos, Sci_Position length, int /*initStyle*/, LexerWordList /*keywordLists*/, Accessor &styler) {
	const Sci_Position endPos = startPos + length;
	const Sci_Line maxLines = styler.GetLine((endPos == styler.Length()) ? endPos : endPos - 1);
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	uint32_t lineStatePrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineStatePrev = styler.GetLineState(lineCurrent - 1);
	}

	int levelNext = levelCurrent;
	uint32_t lineStateNext = styler.GetLineState(lineCurrent);
	while (lineCurrent <= maxLines) {
		const uint32_t lineState = lineStateNext;
		lineStateNext = styler.GetLineState(lineCurrent + 1);
		const int headerLevel = (lineState >> 4) & 7;
		if (headerLevel) {
			levelNext = headerLevel + SC_FOLDLEVELBASE;
			levelCurrent = levelNext - 1;
		} else if (lineState & SimpleLineStateMaskLineComment) {
			levelNext += (lineStateNext & SimpleLineStateMaskLineComment) - (lineStatePrev & SimpleLineStateMaskLineComment);
		} else {
			const int blockDelta = (lineState >> 8) & 0xff;
			if (blockDelta != 0) {
				levelNext += blockDelta - 128;
			}
		}

		levelNext = sci::max(levelNext, SC_FOLDLEVELBASE);
		const int levelUse = levelCurrent;
		int lev = levelUse | (levelNext << 16);
		if (levelUse < levelNext) {
			lev |= SC_FOLDLEVELHEADERFLAG;
		}
		styler.SetLevel(lineCurrent, lev);
		levelCurrent = levelNext;
		lineStatePrev = lineState;
		lineCurrent++;
	}
}

}

extern const LexerModule lmLatex(SCLEX_LATEX, ColouriseLaTeXDoc, "latex", FoldLaTeXDoc);
