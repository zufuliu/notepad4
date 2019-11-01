// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for LaTeX.

#include <cassert>
#include <cstring>
#include <cctype>

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

static constexpr bool IsLSpecial(int ch) noexcept {
	return ch == '#' || ch == '$' || ch == '%' || ch == '&'
		|| ch == '^' || ch == '_' || ch == '{' || ch == '}' || ch == '~';
}
static inline bool IsLWordChar(int ch) noexcept {
	return (ch < 0x80) && isalnum(ch);
}
static inline bool IsLWordStart(int ch) noexcept {
	return (ch < 0x80) && isalpha(ch);
}

#define IsCmdEnd(pos)	(!IsLWordChar(sc.GetRelative(pos)))

static void ColouriseLatexDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	if (initStyle == SCE_L_COMMENT)
		initStyle = SCE_L_DEFAULT;

	StyleContext sc(startPos, length, initStyle, styler);

	char chVerbatimDelim = '\0';
	int bracesInTitle = 0;
	bool isTitleTag = false;
	bool isStringTag = false;
	bool isSquareBrace = false;
	bool isCatcode = false;

	for (; sc.More(); sc.Forward()) {

		if (sc.state == SCE_L_COMMENT) {
			if (sc.atLineStart)
				sc.SetState(SCE_L_DEFAULT);
		} else if (sc.state == SCE_L_COMMAND) {
			if (sc.GetRelative(4) == '`' && (sc.Match("code") || sc.Match("char")))
				isCatcode = true;
			if (!IsLWordChar(sc.ch)) {
				if (sc.ch == '*' && sc.chNext == '{')
					sc.Forward();
				while (IsASpaceOrTab(sc.ch))
					sc.Forward();
				sc.SetState(SCE_L_DEFAULT);
				if (sc.ch == '[') {
					if (isSquareBrace)
						isSquareBrace = false;
					else
						sc.SetState(SCE_L_CMDOPT);
				} else if (sc.ch == '{') {
					if (isTitleTag) {
						isTitleTag = false;
						sc.SetState(SCE_L_TITLE);
						bracesInTitle = 1;
					} else if (isStringTag) {
						isStringTag = false;
						sc.SetState(SCE_L_STRING);
					}
				} else if (sc.ch == '`') {
					if (isCatcode) {
						isCatcode = false;
						sc.SetState(SCE_L_SPECIAL);
						sc.Forward();
						if (IsLSpecial(sc.chNext) || (sc.chNext == '\\' && !IsLWordStart(sc.GetRelative(2))))
							sc.Forward();
					}
				}
			} else if (sc.chPrev == '\\') {
				if (IsCmdEnd(5) && sc.Match("begin{")) {
					sc.Forward(5);
					sc.SetState(SCE_L_TAG);
				} else if (IsCmdEnd(3) && sc.Match("end{")) {
					sc.Forward(3);
					sc.SetState(SCE_L_TAG2);
				} else if ((IsCmdEnd(7) && (sc.Match("chapter") || sc.Match("section") || sc.Match("caption")))
					|| (IsCmdEnd(5) && sc.Match("title"))) {
					isTitleTag = true;
				} else if ((IsCmdEnd(13) && sc.Match("documentclass"))
					|| (IsCmdEnd(10) && sc.Match("usepackage")) || (IsCmdEnd(5) && sc.Match("input"))
					|| sc.Match("include")
					) {
					isStringTag = true;
				} else if (sc.Match("left[") || sc.Match("Big[")) {
					isSquareBrace = true;
				} else if (sc.Match("verb")) {
					sc.Forward(4);
					if (!IsLWordStart(sc.ch)) {
						if (sc.ch == '*')
							sc.Forward();
						sc.SetState(SCE_L_DEFAULT);
						if (IsASpaceOrTab(sc.ch)) {
							chVerbatimDelim = ' ';
							while (IsASpaceOrTab(sc.ch))
								sc.Forward();
						} else {
							chVerbatimDelim = static_cast<char>(sc.ch);
						}
						sc.SetState(SCE_L_VERBATIM);
					}
				} else if (IsCmdEnd(7) && (sc.Match("chapter") || sc.Match("section"))) {
					isTitleTag = true;
				} else if ((IsCmdEnd(7) && sc.Match("Package")) || (IsCmdEnd(5) && sc.Match("style"))) {
					isStringTag = true;
				}
			}
		} else if (sc.state == SCE_L_CMDOPT) {
			if (sc.ch == ']') {
				if (sc.chNext == '[') {
					sc.Forward();
				} else if (sc.chNext == '{') {
					if (isStringTag) {
						isStringTag = false;
						sc.ForwardSetState(SCE_L_STRING);
					} else if (isTitleTag) {
						isTitleTag = false;
						sc.ForwardSetState(SCE_L_TITLE);
						bracesInTitle = 1;
					} else {
						sc.ForwardSetState(SCE_L_DEFAULT);
					}
				} else {
					sc.ForwardSetState(SCE_L_DEFAULT);
				}
			}
		} else if (sc.state == SCE_L_TAG) { // \begin{}
			if (sc.ch == '}') {
				sc.ForwardSetState(SCE_L_DEFAULT);
			} else if (sc.chPrev == '{') {
				if (sc.Match("verbatim}")) {
					sc.Forward(8);
					sc.ForwardSetState(SCE_L_VERBATIM2);
				} else if (sc.Match("lstlisting}")) {
					sc.Forward(10);
					sc.ForwardSetState(SCE_L_LISTCODE);
				} else if (sc.Match("comment}")) {
					sc.Forward(7);
					sc.ForwardSetState(SCE_L_COMMENT2);
				}
			}
		} else if (sc.state == SCE_L_TAG2) { // \end{}
			if (sc.ch == '}')
				sc.ForwardSetState(SCE_L_DEFAULT);
		} else if (sc.state == SCE_L_TITLE) {
			if (sc.ch == '{')
				bracesInTitle++;
			else if (sc.ch == '}')
				bracesInTitle--;
			if (bracesInTitle == 0)
				sc.ForwardSetState(SCE_L_DEFAULT);
		} else if (sc.state == SCE_L_STRING) {
			if (sc.ch == '}') {
				sc.ForwardSetState(SCE_L_DEFAULT);
			}
		} else if (sc.state == SCE_L_VERBATIM) { // \verb
			if (sc.ch == chVerbatimDelim || (IsASpaceOrTab(sc.ch) && chVerbatimDelim == ' ')) {
				sc.ForwardSetState(SCE_L_DEFAULT);
				chVerbatimDelim = '\0';
			}
		} else if (sc.state == SCE_L_VERBATIM2) {
			if (sc.Match("\\end{verbatim}"))
				sc.SetState(SCE_L_COMMAND);
		} else if (sc.state == SCE_L_LISTCODE) {
			if (sc.Match("\\end{lstlisting}"))
				sc.SetState(SCE_L_COMMAND);
		} else if (sc.state == SCE_L_COMMENT2) {
			if (sc.Match("\\end{comment}"))
				sc.SetState(SCE_L_COMMAND);
		} else if (sc.state == SCE_L_QUOTE1) {
			if (sc.ch == '\'')
				sc.ForwardSetState(SCE_L_DEFAULT);
		} else if (sc.state == SCE_L_QUOTE2) {
			if (sc.chPrev == '\'' && sc.ch == '\'')
				sc.ForwardSetState(SCE_L_DEFAULT);
		} else if (sc.state == SCE_L_SPECIAL || sc.state == SCE_L_OPERATOR
			|| sc.state == SCE_L_MATH || sc.state == SCE_L_MATH2) {
			sc.SetState(SCE_L_DEFAULT);
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_L_DEFAULT) {
			if (sc.ch == '\\') {
				if (IsLSpecial(sc.chNext) || sc.chNext == '`' || sc.chNext == '\'' || sc.chNext == '\"') {
					sc.SetState(SCE_L_SPECIAL);
					sc.Forward();
				} else if (IsLWordStart(sc.chNext)) {
					sc.SetState(SCE_L_COMMAND);
				} else if (sc.chNext == '\\') {
					sc.SetState(SCE_L_OPERATOR);
					sc.Forward();
				} else if (sc.chNext == '(' || sc.chNext == ')') {
					sc.SetState(SCE_L_MATH);
					sc.Forward();
				} else if (sc.chNext == '[' || sc.chNext == ']') {
					sc.SetState(SCE_L_MATH2);
					sc.Forward();
				}
			} else if (sc.ch == '$') {
				if (sc.chNext == '$') { // not recommended: $$ $$
					sc.SetState(SCE_L_MATH2);
					sc.Forward();
				} else {
					sc.SetState(SCE_L_MATH);
				}
			} else if (sc.ch == '%') {
				sc.SetState(SCE_L_COMMENT);
			} else if (sc.ch == '`') {
				if (sc.chNext == '`') {
					sc.SetState(SCE_L_QUOTE2);
					sc.Forward();
				} else {
					sc.SetState(SCE_L_QUOTE1);
				}
			} else if ((sc.ch == '!' || sc.ch == '?') && sc.chNext == '`') {
				sc.SetState(SCE_L_SPECIAL);
				sc.Forward();
			} else if (IsLSpecial(sc.ch)) {
				sc.SetState(SCE_L_OPERATOR);
			}
		}
	}

	sc.Complete();
}

static bool IsLBegin(Sci_Position line, Accessor &styler, const char* word, int wlen) noexcept {
	const Sci_Position pos = LexLineSkipSpaceTab(line, styler);
	const Sci_Position chp = pos + 1 + wlen;
	if (styler[pos] == '\\' && styler.StyleAt(pos) == SCE_L_COMMAND
		&& (styler[chp] == '{' || (styler[chp] == '*' && styler[chp + 1] == '{'))
		&& styler.Match(pos + 1, word))
		return true;
	return false;
}
static bool IsLEnd(Sci_Position line, Accessor &styler) noexcept {
	const Sci_Position pos = LexLineSkipSpaceTab(line, styler);
	if (styler[pos] == '\\' && styler.StyleAt(pos) == SCE_L_COMMAND && styler.Match(pos + 1, "end")) {
		if (styler.Match(pos + 4, "{document}") || styler.Match(pos + 4, "input"))
			return true;
	}
	return false;
}
#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_L_COMMENT)
#define IsChapter(line)			IsLBegin(line, styler, "chapter", 7)
#define IsSection(line)			IsLBegin(line, styler, "section", 7)
#define IsSubsection(line)		IsLBegin(line, styler, "subsection", 10)
#define IsSubsubsection(line)	IsLBegin(line, styler, "subsubsection", 13)
#define IsEndDoc(line)			IsLEnd(line, styler)

static void FoldLatexDoc(Sci_PositionU startPos, Sci_Position length, int /*initStyle*/, LexerWordList, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;

	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);

	bool isChapter = false;
	bool isSection = false;
	bool isSubsection = false;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		if (atEOL && IsChapter(lineCurrent)) {
			isChapter = true;
			levelNext++;
		}
		if (atEOL && IsSection(lineCurrent)) {
			isSection = true;
			levelNext++;
		}
		if (atEOL && IsSubsection(lineCurrent)) {
			isSubsection = true;
			levelNext++;
		}

		if (atEOL && IsSubsection(lineCurrent + 1)) {
			if (isSubsection) {
				isSubsection = false;
				levelNext--;
			}
		}
		if (atEOL && IsSection(lineCurrent + 1)) {
			if (isSubsection) {
				isSubsection = false;
				levelNext--;
			}
			if (isSection) {
				isSection = false;
				levelNext--;
			}
		}
		if (atEOL && (IsChapter(lineCurrent + 1) || IsEndDoc(lineCurrent + 1))) {
			if (isSubsection) {
				isSubsection = false;
				levelNext--;
			}
			if (isSection) {
				isSection = false;
				levelNext--;
			}
			if (isChapter) {
				isChapter = false;
				levelNext--;
			}
		}

		if (ch == '\\' && style == SCE_L_COMMAND) {
			if (styler.Match(i, "\\begin{") || styler.Match(i, "\\begingroup"))
				levelNext++;
			else if (styler.Match(i, "\\end{") || styler.Match(i, "\\endgroup"))
				levelNext--;
		}

		if (style == SCE_L_OPERATOR) {
			if (ch == '{')
				levelNext++;
			else if (ch == '}')
				levelNext--;
		}
		if (style == SCE_L_MATH2) {
			if (ch == '[')
				levelNext++;
			else if (ch == ']')
				levelNext--;
		}

		if (!isspacechar(ch))
			visibleChars++;

		if (atEOL || (i == endPos - 1)) {
			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (visibleChars == 0 && foldCompact)
				lev |= SC_FOLDLEVELWHITEFLAG;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelCurrent = levelNext;
			visibleChars = 0;
		}
	}
}

LexerModule lmLatex(SCLEX_LATEX, ColouriseLatexDoc, "latex", FoldLatexDoc);
