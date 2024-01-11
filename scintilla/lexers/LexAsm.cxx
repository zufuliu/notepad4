// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Assembler, include MASM NASM GAS.

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

constexpr bool IsAsmWordChar(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '.' || ch == '_' || ch == '?' || ch == '@' || ch == '$';
}
constexpr bool IsAsmWordStart(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '_' || ch == '.' ||
		ch == '%' || ch == '@' || ch == '$' || ch == '?' || ch == '#';
}
constexpr bool IsAsmOperator(int ch) noexcept {
	return isoperator(ch) && !(ch == '%' || ch == '$' || ch == '#');
}
constexpr bool IsAsmNumber(int ch, int chPrev) noexcept {
	return IsHexDigit(ch)
		|| ((ch == 'x' || ch == 'X') && chPrev == '0')
		|| ((ch == 'H' || ch == 'h') && IsHexDigit(chPrev))
		|| ((ch == '+' || ch == '-') && (chPrev == 'E' || chPrev == 'e'))
		|| ((ch == 'Q' || ch == 'q') && IsOctalDigit(chPrev));
}

/*const char * const asmWordListDesc[] = {
	"CPU instructions",
	"FPU instructions",
	"Registers",
	"Directives",
	"Directive operands",
	"Extended instructions",
	"Directives for fold start",
	"Directives for fold end",
	"GNU Assembler directives ",
	0
};*/


void ColouriseAsmDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &cpuInstruction = keywordLists[0];
	const WordList &mathInstruction = keywordLists[1];
	const WordList &registers = keywordLists[2];
	const WordList &directive = keywordLists[3];
	const WordList &directiveOperand = keywordLists[4];
	const WordList &extInstruction = keywordLists[5];
	//const WordList &directives4foldstart= keywordLists[6];
	//const WordList &directives4foldend= keywordLists[7];
	const WordList &GNUdirective = keywordLists[8];
	const WordList &kwProprocessor = keywordLists[9];
	constexpr std::string_view delimiters;
	// Do not leak onto next line
	if (initStyle == SCE_ASM_STRINGEOL)
		initStyle = SCE_ASM_DEFAULT;
	const Sci_PositionU endPos = startPos + length;
	StyleContext sc(startPos, length, initStyle, styler);
	const Sci_Line lineCurrent = styler.GetLine(startPos);
	bool isIncludePreprocessor = false;
	static bool isGnuAsmSource = false;
	Sci_Line lastGnuAsmDefLine = -1;

	for (; sc.More(); sc.Forward()) {
		// Prevent SCE_ASM_STRINGEOL from leaking back to previous line
		if (sc.atLineStart) {
			if (sc.state == SCE_ASM_STRING || sc.state == SCE_ASM_CHARACTER)
				sc.SetState(sc.state);
			isIncludePreprocessor = false;
		}

		// Handle line continuation generically.
		if (sc.ch == '\\') {
			if (sc.chNext == '\n' || sc.chNext == '\r') {
				sc.Forward();
				if (sc.ch == '\r' && sc.chNext == '\n') {
					sc.Forward();
				}
				continue;
			}
		}

		// Determine if the current state should terminate.
		if (sc.state == SCE_ASM_OPERATOR) {
			sc.SetState(SCE_ASM_DEFAULT);
		} else if (sc.state == SCE_ASM_NUMBER) {
			if (!(IsAsmWordChar(sc.ch) && IsAsmNumber(sc.ch, sc.chPrev))) {
				sc.SetState(SCE_ASM_DEFAULT);
			}
		} else if (sc.state == SCE_ASM_IDENTIFIER) {
			if (!IsAsmWordChar(sc.ch)) {
				char s[128];
				sc.GetCurrentLowered(s, sizeof(s));
				bool IsDirective = false;

				if (cpuInstruction.InList(s)) {
					sc.ChangeState(SCE_ASM_CPUINSTRUCTION);
				} else if (mathInstruction.InList(s)) {
					sc.ChangeState(SCE_ASM_MATHINSTRUCTION);
				} else if (registers.InList(s)) {
					sc.ChangeState(SCE_ASM_REGISTER);
				} else if (s[0] == '%' && registers.InList(s + 1)) {
					isGnuAsmSource = true;
					sc.ChangeState(SCE_ASM_REGISTER);
				} else if (directive.InList(s)) {
					sc.ChangeState(SCE_ASM_DIRECTIVE);
					IsDirective = true;
				} else if (s[0] == '.' && GNUdirective.InList(s + 1)) {
					isGnuAsmSource = true;
					sc.ChangeState(SCE_ASM_DIRECTIVE);
					if (StrEqual(s, ".def"))
						lastGnuAsmDefLine = lineCurrent;
				} else if (directiveOperand.InList(s)) {
					sc.ChangeState(SCE_ASM_DIRECTIVEOPERAND);
				} else if (extInstruction.InList(s)) {
					sc.ChangeState(SCE_ASM_EXTINSTRUCTION);
				} else if ((s[0] == '#' || s[0] == '%') && kwProprocessor.InList(s + 1)) {
					sc.ChangeState(SCE_ASM_PREPROCESSOR);
					IsDirective = true;
				} else if (s[0] == '.' && IsADigit(s[1])) {
					sc.ChangeState(SCE_ASM_NUMBER);
				} else if (sc.ch == ':') {
					Sci_Position pos = sc.currentPos + 1;
					while (IsASpaceOrTab(styler.SafeGetCharAt(pos)))
						pos++;
					if (isspacechar(styler.SafeGetCharAt(pos))) {
						sc.Forward();
						sc.ChangeState(SCE_ASM_LABEL);
					}
				}
				sc.SetState(SCE_ASM_DEFAULT);
				if (IsDirective && (s[0] == '#' || s[0] == '%') && StrEqualsAny(s, "#error", "%error", "#warning", "#message")) {
					while (!IsASpaceOrTab(sc.ch) && !IsEOLChar(sc.chNext))
						sc.Forward();
					if (!sc.atLineEnd)
						sc.ForwardSetState(SCE_ASM_STRING);
				}
				if (IsDirective && StrEqualsAny(s, "#include", "include", "includelib", "%include")) {
					isIncludePreprocessor = true;
					sc.ForwardSetState(SCE_ASM_STRING);
				}
				if (IsDirective && StrEqual(s, "comment")) { // MASM32 block comment
					while (sc.ch == ' ' || sc.ch == '\t')
						sc.Forward();
					if (sc.ch == '*')
						sc.SetState(SCE_ASM_COMMENT2);
				}
				if (sc.atLineEnd) {
					sc.SetState(SCE_ASM_DEFAULT);
				}
				if (IsDirective && StrEqual(s, "comment")) {
					const char delimiter = delimiters.empty() ? '~' : delimiters[0];
					while (IsASpaceOrTab(sc.ch) && !sc.atLineEnd) {
						sc.ForwardSetState(SCE_ASM_DEFAULT);
					}
					if (sc.ch == delimiter) {
						sc.SetState(SCE_ASM_COMMENTDIRECTIVE);
					}
				}
			}
		} else if (sc.state == SCE_ASM_COMMENTDIRECTIVE) {
			const char delimiter = delimiters.empty() ? '~' : delimiters[0];
			if (sc.ch == delimiter) {
				while (!sc.atLineEnd) {
					sc.Forward();
				}
				sc.SetState(SCE_ASM_DEFAULT);
			}
		} else if (sc.state == SCE_ASM_COMMENTLINE) {
			if (sc.atLineStart) {
				sc.SetState(SCE_ASM_DEFAULT);
			}
		} else if (sc.state == SCE_ASM_COMMENT) {
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			}
		} else if (sc.state == SCE_ASM_COMMENT2) {
			if (sc.ch == '*') {
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			}
		} else if (sc.state == SCE_ASM_STRING) {
			if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			} else if (sc.atLineEnd) {
				sc.ChangeState(SCE_ASM_STRINGEOL);
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			} else if (isIncludePreprocessor) {
				if (sc.ch == '>' || sc.ch == '\'') {
					sc.ForwardSetState(SCE_ASM_DEFAULT);
					isIncludePreprocessor = false;
				}
			}
		} else if (sc.state == SCE_ASM_CHARACTER) {
			if (sc.ch == '\\') {
				if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
					sc.Forward();
				}
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			} else if (sc.atLineEnd) {
				sc.ChangeState(SCE_ASM_STRINGEOL);
				sc.ForwardSetState(SCE_ASM_DEFAULT);
			}
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_ASM_DEFAULT) {
			if (sc.ch == ';') {
				// .def xx; .scl xx; .type xx; .endef
				if (isGnuAsmSource && lastGnuAsmDefLine >= 0 && lastGnuAsmDefLine == lineCurrent) {
					Sci_Position i = sc.currentPos + 1;
					while (styler[i] == ' ' || styler[i] == '\t')
						i++;
					if (styler[i] == '.')
						sc.SetState(SCE_ASM_DEFAULT);
					else
						sc.SetState(SCE_ASM_COMMENTLINE);
				} else if (IsCppInDefine(styler, sc.currentPos))
					sc.SetState(SCE_ASM_DEFAULT);
				else
					sc.SetState(SCE_ASM_COMMENTLINE);
			} else if (sc.Match('/', '/')) {
				sc.SetState(SCE_ASM_COMMENTLINE);
			} else if (sc.Match('/', '*')) {
				sc.SetState(SCE_ASM_COMMENT);
				sc.Forward();
			} else if (sc.ch == '@') {
				if (IsOctalDigit(sc.chNext)
					&& (IsAsmOperator(sc.chPrev) || isspacechar(sc.chPrev))) {	// Freescale Octal
					sc.SetState(SCE_ASM_NUMBER);
				} else if (isspacechar(sc.chNext)) {
					sc.SetState(SCE_ASM_COMMENTLINE);
				} else {
					sc.SetState(SCE_ASM_IDENTIFIER);
				}
			} else if (sc.ch == '#') {
				if (IsCppInDefine(styler, sc.currentPos)) {
					sc.SetState(SCE_ASM_OPERATOR);
					if (sc.chNext == '#')
						sc.Forward();
				} else if (IsADigit(sc.chNext) || (sc.chNext == '$' && IsHexDigit(sc.GetRelative(2)))) {
					sc.SetState(SCE_ASM_NUMBER);
				} else if (IsAsmWordStart(sc.chNext)) {
					sc.SetState(SCE_ASM_IDENTIFIER);
				} else {
					char pp[128];
					const Sci_Position pos = LexSkipSpaceTab(styler, sc.currentPos + 1, endPos);
					const Sci_PositionU len = LexGetRange(styler, pos, iswordstart, pp, sizeof(pp));
					if (kwProprocessor.InList(pp)) {
						sc.SetState(SCE_ASM_PREPROCESSOR);
						sc.Advance(pos - sc.currentPos + len);
						if (StrEqual(pp, "include")) {
							isIncludePreprocessor = true;
							if (!sc.atLineEnd)
								sc.SetState(SCE_ASM_STRING);
						} else if (StrEqualsAny(pp, "error", "warning", "message")) {
							if (!sc.atLineEnd)
								sc.SetState(SCE_ASM_STRING);
						} else {
							sc.ForwardSetState(SCE_ASM_DEFAULT);
						}
					} else
						sc.SetState(SCE_ASM_COMMENTLINE);
				}
			} else if (IsADigit(sc.ch)
				|| (sc.ch == '%' && (sc.chNext == '0' || sc.chNext == '1')) // Freescale Binary
				|| (sc.ch == '$' && IsHexDigit(sc.chNext)) // Freescale Hexadecimal
				) {
				sc.SetState(SCE_ASM_NUMBER);
			} else if (sc.chNext == '\'' && IsHexDigit(sc.GetRelative(2)) && (sc.ch == 'B' || sc.ch == 'b'
				|| sc.ch == 'Q' || sc.ch == 'q' || sc.ch == 'D' || sc.ch == 'd'
				|| sc.ch == 'H' || sc.ch == 'h')) { // ARM
				sc.SetState(SCE_ASM_NUMBER);
				sc.Forward();	// eat next '
			} else if (IsAsmWordStart(sc.ch)) {
				sc.SetState(SCE_ASM_IDENTIFIER);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_ASM_STRING);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_ASM_CHARACTER);
			} else if (sc.ch == '\\') {
				sc.Forward();
			} else if (IsAsmOperator(sc.ch)) {
				sc.SetState(SCE_ASM_OPERATOR);
			}
		}

	}

	sc.Complete();
}

constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_ASM_COMMENT || style == SCE_ASM_COMMENT2 || style == SCE_ASM_COMMENTDIRECTIVE;
}
#define IsCommentLine(line)		IsLexCommentLine(styler, line, SCE_ASM_COMMENTLINE)

constexpr bool IsAsmDefaultStyle(int style) noexcept {
	return style == SCE_ASM_DEFAULT || style == SCE_ASM_IDENTIFIER;
}
bool IsEquLine(LexAccessor &styler, Sci_Line line) noexcept {
	if (line < 0) {
		return false;
	}
	const Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	Sci_Position pos = LexSkipWhiteSpace(styler, startPos, endPos, IsAsmDefaultStyle);
	if (styler.StyleAt(pos) == SCE_ASM_DIRECTIVE) {
		if (styler[pos] == '.')
			pos++;
		return styler.MatchLowerCase(pos, "equ");
	}
	return false;
}
bool IsAsmDefineLine(LexAccessor &styler, Sci_Line line) noexcept {
	if (line < 0) {
		return false;
	}
	const Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	Sci_Position pos = LexSkipSpaceTab(styler, startPos, endPos);
	const char ch = styler[pos];
	const int stl = styler.StyleAt(pos);
	if (stl == SCE_ASM_DIRECTIVE) {
		if (ch == '.')
			pos++;
		return styler.MatchLowerCase(pos, "include");
	}
	if ((ch == '#' || ch == '%') && stl == SCE_ASM_PREPROCESSOR) {
		pos++;
		pos = LexSkipSpaceTab(styler, pos, endPos);
		return (styler.Match(pos, "inclide") || (styler.Match(pos, "define") && !IsBackslashLine(styler, line)));
	}
	return false;
}

#define IsEndLine(line)			IsLexLineStartsWith(styler, line, "end", false, SCE_ASM_DIRECTIVE)

#define MAX_ASM_WORD_LEN	15
void FoldAsmDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &directives4foldstart = keywordLists[6];
	const WordList &directives4foldend = keywordLists[7];

	const Sci_PositionU endPos = startPos + length;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	int style = initStyle;
	int styleEOL = initStyle;
	int styleNext = styler.StyleAt(startPos);
	char word[MAX_ASM_WORD_LEN + 1] = "";
	int wordlen = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (IsStreamCommentStyle(style)) {
			if (!IsStreamCommentStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamCommentStyle(styleNext) && !atEOL) {
				levelNext--;
			}
		}
		if (atEOL && IsCommentLine(lineCurrent)) {
			levelNext += IsCommentLine(lineCurrent + 1) - IsCommentLine(lineCurrent - 1);
		}
		if (atEOL && IsAsmDefineLine(styler, lineCurrent)) {
			levelNext += IsAsmDefineLine(styler, lineCurrent + 1) - IsAsmDefineLine(styler, lineCurrent - 1);
		}
		if (atEOL && !IsStreamCommentStyle(style) && !IsStreamCommentStyle(styleEOL)) {
			levelNext += IsBackslashLine(styler, lineCurrent) - IsBackslashLine(styler, lineCurrent - 1);
		}
		if (atEOL && IsEquLine(styler, lineCurrent)) {
			levelNext += IsEquLine(styler, lineCurrent + 1) - IsEquLine(styler, lineCurrent - 1);
		}
		if (style == SCE_ASM_OPERATOR) {
			if (ch == '{') {
				levelNext++;
			} else if (ch == '}') {
				levelNext--;
			}
		}
		if (style == SCE_ASM_PREPROCESSOR) {
			if (styler.Match(i, "#if") || styler.Match(i, "%if"))
				levelNext++;
			else if (styler.Match(i, "#end") || styler.Match(i, "%end"))
				levelNext--;
		}
		if (style == SCE_ASM_DIRECTIVE) {
			if (wordlen < MAX_ASM_WORD_LEN) {
				word[wordlen++] = MakeLowerCase(ch);
			}
			if (styleNext != SCE_ASM_DIRECTIVE) {   // reading directive ready
				word[wordlen] = '\0';
				wordlen = 0;
				if (directives4foldstart.InList(word)) {
					levelNext++;
					if (StrEqual(word, "dialog")) {
						const Sci_Position pos = LexSkipSpaceTab(styler, i + 1, endPos);
						if (styler[pos] == '\"')
							levelNext--;
					}
					if (StrEqual(word, "if") && IsEndLine(lineCurrent)) { // FASM: end if
						levelNext -= 2;
					}
				} else if (directives4foldend.InList(word)) {
					levelNext--;
					if (StrEqual(word, "enddialog")) {
						const Sci_Position pos = LexSkipSpaceTab(styler, i + 1, endPos);
						if (styler[pos] == ',')
							levelNext++;
					}
					if (StrEqual(word, "endproc")) {
						const Sci_Position pos = LexSkipSpaceTab(styler, i + 1, endPos);
						if (styler[pos] == '(')
							levelNext++;
					}
				}
			}
		}
		if (atEOL || (i == endPos - 1)) {
			levelNext = sci::max(levelNext, SC_FOLDLEVELBASE);
			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			styler.SetLevel(lineCurrent, lev);
			lineCurrent++;
			levelCurrent = levelNext;
			styleEOL = style;
		}
	}
}

}

LexerModule lmAsm(SCLEX_ASM, ColouriseAsmDoc, "asm", FoldAsmDoc);
