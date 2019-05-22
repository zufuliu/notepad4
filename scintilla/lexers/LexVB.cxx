// Scintilla source code edit control
/** @file LexVB.cxx
 ** Lexer for Visual Basic (include VB6) and VBScript.

 ** Changes by Zufu Liu <zufuliu@gmail.com> 2011/08
 **    - Updated ColouriseVBDoc, updated number format checking, added preprocessor handling
 **    - rewrited FoldVBDoc fucntion, fixed folding bugs, added some properties.
 **/
// Copyright 1998-2005 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstring>
#include <cassert>
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

// Internal state, highlighted as number
#define SCE_B_FILENUMBER	(SCE_B_DEFAULT + 100)

#define LexCharAt(pos)	styler.SafeGetCharAt(pos)

static constexpr bool IsTypeCharacter(int ch) noexcept {
	return ch == '%' || ch == '&' || ch == '@' || ch == '!' || ch == '#' || ch == '$';
}

static constexpr bool IsVBNumberPrefix(int ch) noexcept {
	return ch == 'h' || ch == 'H'	// Hexadecimal
		|| ch == 'o' || ch == 'O'	// Octal
		|| ch == 'b' || ch == 'B';	// Binary
}

static constexpr bool IsVBNumber(int ch, int chPrev) noexcept {
	return IsHexDigit(ch)|| ch == '_'
		|| (ch == '.' && chPrev != '.')
		|| ((ch == '+' || ch == '-') && (chPrev == 'E' || chPrev == 'e'))
		|| ((ch == 'S' || ch == 'I' || ch == 'L' || ch == 's' || ch == 'i' || ch == 'l')
			&& (IsADigit(chPrev) || chPrev == 'U' || chPrev == 'u'))
		|| ((ch == 'R' || ch == 'r' || ch == '%' || ch == '@' || ch == '!' || ch == '#')
			&& IsADigit(chPrev))
		|| (ch == '&' && IsHexDigit(chPrev));
}

/*static const char * const vbWordListDesc[] = {
	"Keyword",
	"Type keyword"
	"Unused keyword",
	"Preprocessor",
	"Attribute",
	"Constant"
	0
};*/

static constexpr bool IsSpaceEquiv(int state) noexcept {
	// including SCE_B_DEFAULT, SCE_B_COMMENT
	return (state <= SCE_B_COMMENT);
}

static void ColouriseVBDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler, bool vbScriptSyntax) {
	const WordList &keywords = *keywordLists[0];
	const WordList &keywords2 = *keywordLists[1];
	const WordList &keywords3 = *keywordLists[2];
	const WordList &keywords4 = *keywordLists[3];
	const WordList &keywords5 = *keywordLists[4];
	const WordList &keywords6 = *keywordLists[5];

	int fileNbDigits = 0;
	int visibleChars = 0;
	bool isIfThenPreprocessor = false;
	bool isEndPreprocessor = false;

	StyleContext sc(startPos, length, initStyle, styler);

	for (; sc.More(); sc.Forward()) {

		if (sc.atLineStart) {
			isIfThenPreprocessor = false;
			isEndPreprocessor = false;
			visibleChars = 0;
		}

		if (sc.state == SCE_B_OPERATOR) {
			sc.SetState(SCE_B_DEFAULT);
		} else if (sc.state == SCE_B_IDENTIFIER) {
			if (!(iswordstart(sc.ch))) {
				// In Basic (except VBScript), a variable name or a function name
				// can end with a special character indicating the type of the value
				// held or returned.
				bool skipType = false;
				if (!vbScriptSyntax && IsTypeCharacter(sc.ch)) {
					sc.Forward();	// Skip it
					skipType = true;
				}
				if (sc.ch == ']') { // bracketed [keyword] identifier
					sc.Forward();
				}
				char s[128];
				Sci_Position len = sc.GetCurrentLowered(s, sizeof(s));
				if (skipType) {
					s[len - 1] = '\0';
				}
				if (visibleChars == len && sc.GetNextNSChar() == ':') {
					sc.ChangeState(SCE_B_LABEL);
					sc.SetState(SCE_B_DEFAULT);
				} else
					if (strcmp(s, "rem") == 0) {
						sc.ChangeState(SCE_B_COMMENT);
					} else {
						if ((isIfThenPreprocessor && strcmp(s, "then") == 0) || (isEndPreprocessor
							&& (strcmp(s, "if") == 0 || strcmp(s, "region") == 0 || strcmp(s, "externalsource") == 0))) {
							sc.ChangeState(SCE_B_PREPROCESSOR);
						} else if (keywords.InList(s)) {
							sc.ChangeState(SCE_B_KEYWORD);
						} else if (keywords2.InList(s)) {
							sc.ChangeState(SCE_B_KEYWORD2);
						} else if (keywords3.InList(s)) {
							sc.ChangeState(SCE_B_KEYWORD3);
						} else if (!vbScriptSyntax && s[0] == '#' && keywords4.InList(s + 1)) {
							sc.ChangeState(SCE_B_PREPROCESSOR);
							isIfThenPreprocessor = strcmp(s, "#if") == 0 || strcmp(s, "#elseif") == 0;
							isEndPreprocessor = strcmp(s, "#end") == 0;
						} else if (keywords5.InList(s)) {
							sc.ChangeState(SCE_B_KEYWORD4);
						} else if (keywords6.InList(s)) {
							sc.ChangeState(SCE_B_CONSTANT);
						}

						sc.SetState(SCE_B_DEFAULT);
					}
			}
		} else if (sc.state == SCE_B_NUMBER) {
			if (!IsVBNumber(sc.ch, sc.chPrev)) {
				sc.SetState(SCE_B_DEFAULT);
			}
		} else if (sc.state == SCE_B_STRING) {
			// VB doubles quotes to preserve them, so just end this string
			// state now as a following quote will start again
			if (sc.ch == '\"') {
				if (sc.chNext == '\"') {
					sc.Forward();
				} else {
					if (sc.chNext == 'c' || sc.chNext == 'C' || sc.chNext == '$') {
						sc.Forward();
					}
					sc.ForwardSetState(SCE_B_DEFAULT);
				}
			} else if (sc.atLineEnd) {
				sc.ChangeState(SCE_B_STRINGEOL);
				sc.ForwardSetState(SCE_B_DEFAULT);
			}
		} else if (sc.state == SCE_B_COMMENT) {
			if (sc.atLineStart) {
				sc.SetState(SCE_B_DEFAULT);
			}
		} else if (sc.state == SCE_B_FILENUMBER) {
			if (IsADigit(sc.ch)) {
				fileNbDigits++;
				if (fileNbDigits > 3) {
					sc.ChangeState(SCE_B_DATE);
				}
			} else if (sc.ch == '\r' || sc.ch == '\n' || sc.ch == ',') {
				// Regular uses: Close #1; Put #1, ...; Get #1, ... etc.
				// Too bad if date is format #27, Oct, 2003# or something like that...
				// Use regular number state
				sc.ChangeState(SCE_B_NUMBER);
				sc.SetState(SCE_B_DEFAULT);
			} else if (sc.ch == '#') {
				sc.ChangeState(SCE_B_DATE);
				sc.ForwardSetState(SCE_B_DEFAULT);
			} else {
				sc.ChangeState(SCE_B_DATE);
			}
			if (sc.state != SCE_B_FILENUMBER) {
				fileNbDigits = 0;
			}
		} else if (sc.state == SCE_B_DATE) {
			if (sc.atLineEnd) {
				sc.ChangeState(SCE_B_STRINGEOL);
				sc.ForwardSetState(SCE_B_DEFAULT);
			} else if (sc.ch == '#') {
				sc.ForwardSetState(SCE_B_DEFAULT);
			}
		}

		if (sc.state == SCE_B_DEFAULT) {
			if (sc.ch == '\'') {
				sc.SetState(SCE_B_COMMENT);
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_B_STRING);
			} else if (sc.ch == '#') {
				const int chNUp = MakeUpperCase(sc.chNext);
				if (chNUp == 'E' || chNUp == 'I' || chNUp == 'R' || chNUp == 'C')
					sc.SetState(SCE_B_IDENTIFIER);
				else
					sc.SetState(SCE_B_FILENUMBER);
			} else if (sc.ch == '&' && IsVBNumberPrefix(sc.chNext)) {
				sc.SetState(SCE_B_NUMBER);
				sc.Forward();
			} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
				sc.SetState(SCE_B_NUMBER);
			} else if (sc.ch == '_' && isspacechar(sc.chNext)) {
				sc.SetState(SCE_B_OPERATOR);
			} else if (iswordstart(sc.ch) || sc.ch == '[') { // bracketed [keyword] identifier
				sc.SetState(SCE_B_IDENTIFIER);
			} else if (isoperator(sc.ch) || (sc.ch == '\\')) { // Integer division
				sc.SetState(SCE_B_OPERATOR);
			}
		}

		if (!(isspacechar(sc.ch) || IsSpaceEquiv(sc.state))) {
			visibleChars++;
		}
	}

	sc.Complete();
}

static bool VBLineStartsWith(Sci_Position line, Accessor &styler, const char* word) noexcept {
	const Sci_Position pos = LexLineSkipSpaceTab(line, styler);
	return (styler.StyleAt(pos) == SCE_B_KEYWORD) && (styler.MatchIgnoreCase(pos, word));
}
static bool VBMatchNextWord(Sci_Position startPos, Sci_Position endPos, Accessor &styler, const char *word) noexcept {
	const Sci_Position pos = LexSkipSpaceTab(startPos, endPos, styler);
	return isspacechar(LexCharAt(pos + static_cast<int>(strlen(word))))
		&& styler.MatchIgnoreCase(pos, word);
}
static bool IsVBProperty(Sci_Position line, Sci_Position startPos, Accessor &styler) noexcept {
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	for (Sci_Position i = startPos; i < endPos; i++) {
		if (styler.StyleAt(i) == SCE_B_OPERATOR && LexCharAt(i) == '(')
			return true;
	}
	return false;
}
static bool IsVBSome(Sci_Position line, int kind, Accessor &styler) noexcept {
	const Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	Sci_Position pos = LexSkipSpaceTab(startPos, endPos, styler);
	int stl = styler.StyleAt(pos);
	if (stl == SCE_B_KEYWORD) {
		if (styler.MatchIgnoreCase(pos, "public")) {
			pos += 6;
		} else if (styler.MatchIgnoreCase(pos, "private")) {
			pos += 7;
		} else if (styler.MatchIgnoreCase(pos, "protected")) {
			pos += 9;
			pos = LexSkipSpaceTab(endPos, pos, styler);
		}
		if (styler.MatchIgnoreCase(pos, "friend"))
			pos += 6;
		pos = LexSkipSpaceTab(pos, endPos, styler);
		stl = styler.StyleAt(pos);
		if (stl == SCE_B_KEYWORD) {
			return (kind == 1 && isspacechar(LexCharAt(pos + 4)) && styler.MatchIgnoreCase(pos, "type"))
				|| (kind == 2 && isspacechar(LexCharAt(pos + 5)) && styler.MatchIgnoreCase(pos, "const"));
		}
	}
	return false;
}
#define VBMatch(word)			styler.MatchIgnoreCase(i, word)
#define VBMatchNext(pos, word)	VBMatchNextWord(pos, endPos, styler, word)
#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_B_COMMENT)
#define IsDimLine(line)			VBLineStartsWith(line, styler, "dim")
#define IsConstLine(line)		IsVBSome(line, 2, styler)
#define IsVB6Type(line)			IsVBSome(line, 1, styler)

static void FoldVBDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	if (styler.GetPropertyInt("fold") == 0)
		return;
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	const bool foldPreprocessor = styler.GetPropertyInt("fold.preprocessor") != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;

	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;

	char ch = '\0';
	char chNext = styler[startPos];
	int style = initStyle;
	int styleNext = styler.StyleAt(startPos);

	int numBegin = 0;		// nested Begin ... End, found in VB6 Form
	bool isEnd = false;		// End {Function Sub}{If}{Class Module Structure Interface Operator Enum}{Property Event}{Type}
	bool isInterface = false;// {Property Function Sub Event Interface Class Structure }
	bool isProperty = false;// Property: Get Set
	bool isCustom = false;	// Custom Event
	bool isExit = false;	// Exit {Function Sub Property}
	bool isDeclare = false;	// Declare, Delegate {Function Sub}
	bool isIf = false;		// If ... Then \r\n ... \r\n End If
	static Sci_Position lineIf = 0;
	static Sci_Position lineThen = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char chPrev = ch;
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineBegin = (visibleChars == 0) && !atEOL;

		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		if (atEOL && IsDimLine(lineCurrent)) {
			if (!IsDimLine(lineCurrent - 1) && IsDimLine(lineCurrent + 1))
				levelNext++;
			else if (IsDimLine(lineCurrent - 1) && !IsDimLine(lineCurrent + 1))
				levelNext--;
		}
		if (atEOL && IsConstLine(lineCurrent)) {
			if (!IsConstLine(lineCurrent - 1) && IsConstLine(lineCurrent + 1))
				levelNext++;
			else if (IsConstLine(lineCurrent - 1) && !IsConstLine(lineCurrent + 1))
				levelNext--;
		}

		if (style == SCE_B_KEYWORD && stylePrev != SCE_B_KEYWORD
			&& !(chPrev == '.' || chPrev == '[')) { // not a member, not bracketed [keyword] identifier
			if (atLineBegin && (VBMatch("for") || (VBMatch("do") && isspacechar(LexCharAt(i + 2))) // not Double
				|| VBMatch("while") || (VBMatch("try") && isspacechar(LexCharAt(i + 3))) // not TryCast
				|| (VBMatch("select") && VBMatchNext(i + 6, "case")) // Select Case
				|| (VBMatch("with") && isspacechar(LexCharAt(i + 4))) // not WithEvents, not With {...}
				|| VBMatch("namespace") || VBMatch("synclock") || VBMatch("using")
				|| (isProperty && (VBMatch("set") || (VBMatch("get") && isspacechar(LexCharAt(i + 3))))) // not GetType
				|| (isCustom && (VBMatch("raiseevent") || VBMatch("addhandler") || VBMatch("removehandler")))
				)) {
				levelNext++;
			} else if (atLineBegin && (VBMatch("next") || VBMatch("loop") || VBMatch("wend"))) {
				levelNext--;
			} else if (VBMatch("exit") && (VBMatchNext(i + 4, "function") || VBMatchNext(i + 4, "sub")
				|| VBMatchNext(i + 4, "property")
				)) {
				isExit = true;
			} else if (VBMatch("begin")) {
				levelNext++;
				if (isspacechar(LexCharAt(i + 5)))
					numBegin++;
			} else if (VBMatch("end")) {
				levelNext--;
				int chEnd = static_cast<unsigned char>(LexCharAt(i + 3));
				if (chEnd == ' ' || chEnd == '\t') {
					const Sci_Position pos = LexSkipSpaceTab(i + 3, endPos, styler);
					chEnd = static_cast<unsigned char>(LexCharAt(pos));
					// check if End is used to terminate statement
					if (isalpha(chEnd) && (VBMatchNext(pos, "function") || VBMatchNext(pos, "sub")
						|| VBMatchNext(pos, "if") || VBMatchNext(pos, "class") || VBMatchNext(pos, "structure")
						|| VBMatchNext(pos, "module") || VBMatchNext(pos, "enum") || VBMatchNext(pos, "interface")
						|| VBMatchNext(pos, "operator") || VBMatchNext(pos, "property") || VBMatchNext(pos, "event")
						|| VBMatchNext(pos, "type") // VB6
						)) {
						isEnd = true;
					}
				}
				if (chEnd == '\r' || chEnd == '\n' || chEnd == '\'') {
					isEnd = false;
					if (numBegin == 0)	levelNext++;// End can be placed anywhere, but not used to terminate statement
					if (numBegin > 0)	numBegin--;
				}
				if (VBMatch("endif")) // same as End If
					isIf = false;
				// one line: If ... Then ... End If
				if (lineCurrent == lineIf && lineCurrent == lineThen)
					levelNext++;
			} else if (VBMatch("if")) {
				isIf = true;
				lineIf = lineCurrent;
				if (isEnd) {
					isEnd = false; isIf = false;
				} else				levelNext++;
			} else if (VBMatch("then")) {
				if (isIf) {
					isIf = false;
					const Sci_Position pos = LexSkipSpaceTab(i + 4, endPos, styler);
					const char chEnd = LexCharAt(pos);
					if (!(chEnd == '\r' || chEnd == '\n' || chEnd == '\''))
						levelNext--;
				}
				lineThen = lineCurrent;
			} else if ((!isInterface && (VBMatch("class") || VBMatch("structure")))
				|| VBMatch("module") || VBMatch("enum") || VBMatch("operator")
				) {
				if (isEnd)			isEnd = false;
				else				levelNext++;
			} else if (VBMatch("interface")) {
				if (!(isEnd || isInterface))
					levelNext++;
				isInterface = true;
				if (isEnd) {
					isEnd = false; isInterface = false;
				}
			} else if (VBMatch("declare") || VBMatch("delegate")) {
				isDeclare = true;
			} else if (!isInterface && (VBMatch("sub") || VBMatch("function"))) {
				if (!(isEnd || isExit || isDeclare))
					levelNext++;
				if (isEnd)			isEnd = false;
				if (isExit)			isExit = false;
				if (isDeclare)		isDeclare = false;
			} else if (!isInterface && VBMatch("property")) {
				isProperty = true;
				if (!(isEnd || isExit)) {
					levelNext++;
					if (!IsVBProperty(lineCurrent, i + 8, styler)) {
						isProperty = false;
						levelNext--;
					}
				}
				if (isEnd) {
					isEnd = false; isProperty = false;
				}
				if (isExit)			isExit = false;
			} else if (VBMatch("custom")) {
				isCustom = true;
			} else if (!isInterface && isCustom && VBMatch("event")) {
				if (isEnd) {
					isEnd = false; isCustom = false;
				} else 				levelNext++;
			} else if (VBMatch("type") && isspacechar(LexCharAt(i + 4))) { // not TypeOf, VB6: [...] Type ... End Type
				if (!isEnd && IsVB6Type(lineCurrent))
					levelNext++;
				if (isEnd)	isEnd = false;
			}
		}

		if (foldPreprocessor && style == SCE_B_PREPROCESSOR) {
			if (VBMatch("#if") || VBMatch("#region") || VBMatch("#externalsource"))
				levelNext++;
			else if (VBMatch("#end"))
				levelNext--;
		}

		if (style == SCE_B_OPERATOR) {
			// Anonymous With { ... }
			if (ch == '{')
				levelNext++;
			else if (ch == '}')
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

static void ColouriseVBNetDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	ColouriseVBDoc(startPos, length, initStyle, keywordLists, styler, false);
}
static void ColouriseVBScriptDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	ColouriseVBDoc(startPos, length, initStyle, keywordLists, styler, true);
}

LexerModule lmVB(SCLEX_VB, ColouriseVBNetDoc, "vb", FoldVBDoc);
LexerModule lmVBScript(SCLEX_VBSCRIPT, ColouriseVBScriptDoc, "vbscript", FoldVBDoc);
