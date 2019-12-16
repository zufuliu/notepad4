// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for javap, Jasmin, Android Dalvik Smali.

#include <cassert>
#include <cstring>
#include <cctype>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Scintilla;

static constexpr bool IsSmaliOp(int ch) noexcept {
	return ch == ';' || ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == '='
		|| ch == ',' || ch == '<' || ch == '>' || ch == '+' || ch == '-' || ch == ':' || ch == '.'
		|| ch == '/' || ch == '&' || ch == '|' || ch == '^' || ch == '!' || ch == '~' || ch == '*' || ch == '%';
}
static constexpr bool IsDelimiter(int ch) noexcept {
	return ch != '/' && ch != '$' && (IsASpace(ch) || IsSmaliOp(ch));
}
static constexpr bool IsSmaliWordChar(int ch) noexcept {
	return iswordstart(ch) || ch == '-';
}
static constexpr bool IsSmaliWordCharX(int ch) noexcept {
	return iswordchar(ch) || ch == '-';
}

static constexpr bool IsOpcodeChar(int ch) noexcept {
	return ch == '_' || ch == '-' || IsAlpha(ch);
}

static constexpr bool IsJavaTypeChar(int ch) noexcept {
	return ch == 'V'	// void
		|| ch == 'Z'	// boolean
		|| ch == 'B'	// byte
		|| ch == 'S'	// short
		|| ch == 'C'	// char
		|| ch == 'I'	// int
		|| ch == 'J'	// long
		|| ch == 'F'	// float
		|| ch == 'D'	// double
		|| ch == 'L'	// object
		|| ch == '[';	// array
}
static bool IsJavaType(int ch, int chPrev, int chNext) noexcept {
	if (chPrev == 'L' || chPrev == '>' || chPrev == '/' || chPrev == '.')
		return false;
	if (ch == '[')
		return IsJavaTypeChar(chNext);
	if (!IsJavaTypeChar(ch))
		return false;
	if (ch == 'L') {
		if (!(iswordstart(chNext) || chNext == '$'))
			return false;
		return IsDelimiter(chPrev) || IsJavaTypeChar(chPrev);
	}
	return IsSmaliOp(chPrev) || IsJavaTypeChar(chPrev) || (IsDelimiter(chPrev) && IsDelimiter(chNext));
}

#define kWordType_Directive 1
#define kWrodType_Field		2
#define kWordType_Method	3

#define MAX_WORD_LENGTH	31
static void ColouriseSmaliDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	//const WordList &kwInstruction = *keywordLists[10];

	int state = initStyle;
	int ch = 0;
	int chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	const Sci_PositionU endPos = startPos + length;

	Sci_Position lineCurrent = styler.GetLine(startPos);
	int curLineState = (lineCurrent > 0) ? styler.GetLineState(lineCurrent - 1) : 0;
	char buf[MAX_WORD_LENGTH + 1] = "";
	int wordLen = 0;
	int visibleChars = 0;
	int nextWordType = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int chPrev = ch;
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineStart = i == static_cast<Sci_PositionU>(styler.LineStart(lineCurrent));

		switch (state) {
		case SCE_SMALI_OPERATOR:
			styler.ColourTo(i - 1, state);
			state = SCE_SMALI_DEFAULT;
			break;
		case SCE_SMALI_NUMBER:
			if (!IsDecimalNumber(chPrev, ch, chNext)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DEFAULT;
			}
			break;
		case SCE_SMALI_STRING:
		case SCE_SMALI_CHARACTER:
			if (atLineStart) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DEFAULT;
			} else if (ch == '\\' && (chNext == '\\' || chNext == '\"')) {
				i++;
				ch = chNext;
				chNext = styler.SafeGetCharAt(i + 1);
			} else if ((state == SCE_SMALI_STRING && ch == '\"') || (state == SCE_SMALI_CHARACTER && ch == '\'')) {
				styler.ColourTo(i, state);
				state = SCE_SMALI_DEFAULT;
				continue;
			}
			break;
		case SCE_SMALI_DIRECTIVE:
			if (!IsSmaliWordChar(ch)) {
				buf[wordLen] = '\0';
				if (buf[0] == '.') {
					if (strcmp(buf + 1,"end") == 0 || strcmp(buf + 1,"restart") == 0 || strcmp(buf + 1,"limit") == 0) {
						nextWordType = kWordType_Directive;
					} else if (strcmp(buf + 1,"field") == 0) {
						nextWordType = kWrodType_Field;
					} else if (strcmp(buf + 1,"method") == 0) {
						nextWordType = kWordType_Method;
					}
					if (strcmp(buf + 1,"annotation") == 0 || strcmp(buf + 1,"subannotation") == 0) {
						curLineState = 1;
					} else if (strcmp(buf + 1,"packed-switch") == 0 || strcmp(buf + 1,"sparse-switch") == 0) {
						curLineState = 2;
					} else if (strcmp(buf + 1,"end") == 0) {
						curLineState = 0;
					}
				}
				styler.ColourTo(i - 1, state);
				state = SCE_L_DEFAULT;
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = static_cast<char>(ch);
			}
			break;
		case SCE_SMALI_REGISTER:
			if (!IsADigit(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DEFAULT;
			}
			break;
		case SCE_SMALI_INSTRUCTION:
			if (!(IsSmaliWordChar(ch) || ch == '/')) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DEFAULT;
			}
			break;
		case SCE_SMALI_LABEL:
			if (!iswordchar(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DEFAULT;
				visibleChars = 0;
			}
			break;
		case SCE_SMALI_TYPE:
			styler.ColourTo(i - 1, state);
			state = SCE_SMALI_DEFAULT;
			break;
		case SCE_SMALI_IDENTIFIER:
			if (!IsSmaliWordChar(ch)) {
				buf[wordLen] = '\0';
				if (nextWordType == kWordType_Directive) {
					nextWordType = 0;
					styler.ColourTo(i - 1, SCE_SMALI_DIRECTIVE);
					state = SCE_SMALI_DEFAULT;
				} else if (ch == ':' && IsDelimiter(chNext)) {
					state = SCE_SMALI_LABEL;
				} else if (keywords.InList(buf)) {
					styler.ColourTo(i - 1, SCE_SMALI_WORD);
					state = SCE_SMALI_DEFAULT;
				} else if (nextWordType == kWrodType_Field) {
					nextWordType = 0;
					styler.ColourTo(i - 1, SCE_SMALI_FIELD);
					if (ch == '$')
						styler.ColourTo(i - 1, SCE_SMALI_OPERATOR);
					state = (ch == ':') ? SCE_SMALI_DEFAULT : SCE_SMALI_FIELD;
				} else if (nextWordType == kWordType_Method) {
					nextWordType = 0;
					styler.ColourTo(i - 1, SCE_SMALI_METHOD);
					if (ch == '$')
						styler.ColourTo(i - 1, SCE_SMALI_OPERATOR);
					state = (ch == '(' || ch == '>') ? SCE_SMALI_DEFAULT : SCE_SMALI_METHOD;
				} else if (/*kwInstruction.InList(buf)*/ wordLen == visibleChars && !curLineState && (IsASpace(ch) || ch == '/')) {
					if (!(IsSmaliWordChar(ch) || ch == '/')) {
						styler.ColourTo(i - 1, SCE_SMALI_INSTRUCTION);
						state = SCE_SMALI_DEFAULT;
					} else {
						state = SCE_SMALI_INSTRUCTION;
					}
				} else {
					Sci_PositionU pos = i;
					while (pos < endPos && IsASpace(styler.SafeGetCharAt(pos))) ++pos;
					if (styler.SafeGetCharAt(pos) == '(') {
						styler.ColourTo(i - 1, SCE_SMALI_METHOD);
					}
					state = SCE_SMALI_DEFAULT;
				}
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = static_cast<char>(ch);
			}
			break;
		case SCE_SMALI_LABEL_EOL:	// packed-switch sparse-switch
		case SCE_SMALI_COMMENTLINE:
			if (atLineStart) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DEFAULT;
			}
			break;
		case SCE_SMALI_FIELD:
		case SCE_SMALI_METHOD:
			if (ch == '$') {
				styler.ColourTo(i - 1, state);
				styler.ColourTo(i, SCE_SMALI_OPERATOR);
			} else if (ch == ':' || ch == '(' || IsASpace(ch)) {
				styler.ColourTo(i - 1, (ch == ':' ? SCE_SMALI_FIELD : SCE_SMALI_METHOD));
				state = SCE_SMALI_DEFAULT;
			} break;
		}

		if (state == SCE_SMALI_DEFAULT) {
			if (ch == '#') {
				styler.ColourTo(i - 1, state);
				if (IsADigit(chNext)) { // javap
					state = SCE_SMALI_NUMBER;
				} else {
					state = SCE_SMALI_COMMENTLINE;
				}
			} else if (ch == '/' && chNext == '/') { // javap
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_COMMENTLINE;
			} else if (ch == ';' && !(chPrev == '>' || iswordchar(chPrev)) && !IsEOLChar(chNext)) { // jasmin
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_STRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_CHARACTER;
			} else if (IsADigit(ch) || (ch == '.' && IsADigit(chNext))) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_NUMBER;
			} else if (ch == '.' && visibleChars == 0 && IsAlpha(chNext)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DIRECTIVE;
				buf[0] = static_cast<char>(ch);
				wordLen = 1;
			} else if ((ch == 'v' || ch == 'p') && IsADigit(chNext)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_REGISTER;
			} else if (!curLineState && visibleChars == 0 && ch == ':' && (chNext == 's' || chNext == 'p')) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_LABEL_EOL; // packed-switch sparse-switch
			} else if (ch == ':' && IsDelimiter(chPrev) && iswordstart(chNext)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_LABEL;
			} else if ((visibleChars > 0 || (curLineState && visibleChars == 0 && ch == 'L')) && IsJavaType(ch, chPrev, chNext)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_TYPE;
			} else if (chPrev == '-' && ch == '>') { // field/mathod
				styler.ColourTo(i, SCE_SMALI_OPERATOR);
				state = SCE_SMALI_FIELD;
			} else if ((iswordstart(ch) || ch == '$' || ch == '/')) {
				styler.ColourTo(i - 1, state);
				if (ch == '/' || ch == '$') {
					styler.ColourTo(i, SCE_SMALI_OPERATOR);
				}
				state = SCE_SMALI_IDENTIFIER;
				buf[0] = static_cast<char>(ch);
				wordLen = 1;
			} else if (IsSmaliOp(ch) || (ch == '[' || ch == ']')) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_OPERATOR;
			}
		}

		if (atEOL || i == endPos - 1) {
			styler.SetLineState(lineCurrent, curLineState);
			lineCurrent++;
			visibleChars = 0;
			nextWordType = 0;
		}
		if (ch == '.' || (ch == ':' && !IsASpace(chNext)) || IsOpcodeChar(ch) || (IsADigit(ch) && IsOpcodeChar(chPrev))) {
			visibleChars++;
		}
	}

	// Colourise remaining document
	styler.ColourTo(endPos - 1, state);
}

#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_SMALI_COMMENTLINE)
static inline bool IsFoldWord(const char *word) noexcept {
	return strcmp(word, "method") == 0
		|| strcmp(word, "annotation") == 0
		|| strcmp(word, "subannotation") == 0
		|| strcmp(word, "packed-switch") == 0
		|| strcmp(word, "sparse-switch") == 0
		|| strcmp(word, "array-data") == 0
		// not used in Smali and Jasmin or javap
		|| strcmp(word, "tableswitch") == 0
		|| strcmp(word, "lookupswitch") == 0
		|| strcmp(word, "constant-pool") == 0
		|| strcmp(word, "attribute") == 0;
}
// field/parameter with annotation?
static bool IsAnnotationLine(Sci_Position line, Accessor &styler) noexcept {
	Sci_Position scan_line = 10;
	while (scan_line-- > 0) {
		const Sci_Position startPos = styler.LineStart(line);
		const Sci_Position endPos = styler.LineStart(line + 1) - 1;
		const Sci_Position pos = LexSkipSpaceTab(startPos, endPos, styler);
		if (styler[pos] == '.') {
			return styler.StyleAt(pos) == SCE_SMALI_DIRECTIVE && styler[pos + 1] == 'a';
		}
		++line;
	}
	return false;
}

static void FoldSmaliDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;

	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;

	int chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		if (iswordchar(ch) && style == SCE_SMALI_DIRECTIVE && stylePrev != SCE_SMALI_DIRECTIVE) {
			char buf[MAX_WORD_LENGTH + 1];
			LexGetRange(i, styler, IsSmaliWordCharX, buf, sizeof(buf));
			if (buf[0] == '.' && (IsFoldWord(buf + 1) || (strcmp(buf + 1,"field") == 0 && IsAnnotationLine(lineCurrent + 1, styler)))) {
				levelNext++;
			} else if (buf[0] != '.' && (IsFoldWord(buf) || strcmp(buf, "field") == 0)) {
				levelNext--;
			}
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

LexerModule lmSmali(SCLEX_SMALI, ColouriseSmaliDoc, "smali", FoldSmaliDoc);

