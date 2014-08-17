// Lexer for JSVM Jasmin, Android Dalvik Smali.

#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "CharacterSet.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "LexerModule.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

static inline bool IsSmaliOp(int ch) {
	return ch == ';' || ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == '='
		|| ch == ',' || ch == '<' || ch == '>' || ch == '+' || ch == '-' || ch == ':' || ch == '.'
		|| ch == '/';
}
static inline bool IsDelimiter(int ch) {
	return ch != '/' && ch != '$' && (IsASpace(ch) || IsSmaliOp(ch));
}
static inline bool IsSmaliWordChar(int ch) {
	return iswordchar(ch) || ch == '-';
}

static inline bool IsJavaTypeChar(int ch) {
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
static bool IsJavaType(int ch, int chPrev, int chNext) {
	if (chPrev == 'L' || chPrev == '>' || chPrev == '/')
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
static void ColouriseSmaliDoc(unsigned int startPos, int length, int initStyle, WordList *keywordLists[], Accessor &styler) {
	WordList &keywords = *keywordLists[0];
	//WordList &kwInstruction = *keywordLists[10];

	int state = initStyle;
	int chPrev, ch = 0, chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int endPos = startPos + length;
	if (endPos == (unsigned)styler.Length())
		++endPos;

	int lineCurrent = styler.GetLine(startPos);
	int curLineState =	(lineCurrent > 0) ? styler.GetLineState(lineCurrent-1) : 0;
	char buf[MAX_WORD_LENGTH + 1] = {0};
	int wordLen = 0;
	int visibleChars = 0;
	int nextWordType = 0;

	for (unsigned int i = startPos; i < endPos; i++) {
		chPrev = ch;
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineStart = i == (unsigned)styler.LineStart(lineCurrent);

		switch (state) {
		case SCE_SMALI_OPERATOR:
			styler.ColourTo(i - 1, state);
			state = SCE_SMALI_DEFAULT;
			break;
		case SCE_SMALI_NUMBER:
			if (!(iswordchar(ch) || ((ch == '+' || ch == '-') && IsADigit(chNext)))) {
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
				buf[wordLen] = 0;
				if (buf[0] == '.') {
					if (!(strcmp(&buf[1], "end") && strcmp(&buf[1], "restart") && strcmp(&buf[1], "limit"))) {
						nextWordType = kWordType_Directive;
					} else if (!strcmp(&buf[1], "field")) {
						nextWordType = kWrodType_Field;
					} else if (!strcmp(&buf[1], "method")) {
						nextWordType = kWordType_Method;
					}
					if (!(strcmp(&buf[1], "annotation") && strcmp(&buf[1], "subannotation"))) {
						curLineState = 1;
					} else if (!(strcmp(&buf[1], "packed-switch") && strcmp(&buf[1], "sparse-switch"))) {
						curLineState = 2;
					} else if (!strcmp(&buf[1], "end")) {
						curLineState = 0;
					}
				}
				styler.ColourTo(i - 1, state);
				state = SCE_L_DEFAULT;
				wordLen = 0;
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = (char)ch;
			}
			break;
		case SCE_SMALI_REGISTER:
			if (!IsADigit(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DEFAULT;
			}
			break;
		case SCE_SMALI_INSTRUCTION:
			if (!(iswordchar(ch) || ch == '-' || ch == '/')) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DEFAULT;
			}
			break;
		case SCE_SMALI_LABEL:
			if (!iswordchar(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DEFAULT;
			}
			break;
		case SCE_SMALI_TYPE:
			styler.ColourTo(i - 1, state);
			state = SCE_SMALI_DEFAULT;
			break;
		case SCE_SMALI_IDENTIFIER:
			if (!IsSmaliWordChar(ch)) {
				buf[wordLen] = 0;
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
					state = (ch == ':')? SCE_SMALI_DEFAULT : SCE_SMALI_FIELD;
				} else if (nextWordType == kWordType_Method) {
					nextWordType = 0;
					styler.ColourTo(i - 1, SCE_SMALI_METHOD);
					if (ch == '$')
						styler.ColourTo(i - 1, SCE_SMALI_OPERATOR);
					state = (ch == '(' || ch == '>')? SCE_SMALI_DEFAULT : SCE_SMALI_METHOD;
				} else if (/*kwInstruction.InList(buf)*/ wordLen == visibleChars && !curLineState) {
					if (IsASpace(ch)) {
						styler.ColourTo(i - 1, SCE_SMALI_INSTRUCTION);
						state = SCE_SMALI_DEFAULT;
					} else {
						state = SCE_SMALI_INSTRUCTION;
					}
				} else {
					state = SCE_SMALI_DEFAULT;
				}
				wordLen = 0;
			} else if (wordLen < MAX_WORD_LENGTH) {
				buf[wordLen++] = (char)ch;
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
			} else if (ch == ':' || ch == '(') {
				styler.ColourTo(i - 1, (ch == ':' ? SCE_SMALI_FIELD : SCE_SMALI_METHOD));
				state = SCE_SMALI_DEFAULT;
			} break;
		}

		if (state == SCE_SMALI_DEFAULT) {
			if (ch == '#' || (ch == ';' && !iswordchar(chPrev))) { // jasmin
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
			} else if (ch == '.' && IsAlpha(chNext)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_DIRECTIVE;
				buf[wordLen++] = (char)ch;
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
				buf[wordLen++] = (char)ch;
			} else if (IsSmaliOp(ch)) {
				styler.ColourTo(i - 1, state);
				state = SCE_SMALI_OPERATOR;
			}
		}

		if (atEOL || i == endPos-1) {
			styler.SetLineState(lineCurrent, curLineState);
			lineCurrent++;
			visibleChars = 0;
			nextWordType = 0;
		}
		if (!isspacechar(ch)) {
			visibleChars++;
		}

	}

	// Colourise remaining document
	styler.ColourTo(endPos - 1, state);
}

#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_SMALI_COMMENTLINE)
static inline bool IsFoldWord(const char *word) {
	return strcmp(word, "method") == 0
		|| strcmp(word, "annotation") == 0
		|| strcmp(word, "subannotation") == 0
		|| strcmp(word, "packed-switch") == 0
		|| strcmp(word, "sparse-switch") == 0
		|| strcmp(word, "array-data") == 0
		|| strcmp(word, "constant-pool") == 0
		|| strcmp(word, "attribute") == 0;
}
// field/parameter with annotation?
static bool IsAnnotationLine(int line, Accessor &styler) {
	int scan_line = 10;
	while (scan_line-- > 0) {
		int startPos = styler.LineStart(line);
		int endPos = styler.LineStart(line + 1) - 1;
		int pos = LexSkipSpaceTab(startPos, endPos, styler);
		if (styler[pos] == '.') {
			return styler.StyleAt(pos) == SCE_SMALI_DIRECTIVE && styler[pos + 1] == 'a';
		}
		++line;
	}
	return false;
}

static void FoldSmaliDoc(unsigned int startPos, int length, int initStyle, WordList *[], Accessor &styler) {
	if (styler.GetPropertyInt("fold") == 0)
		return;
	const bool foldComment = styler.GetPropertyInt("fold.comment") != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;

	unsigned int endPos = startPos + length;
	int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent-1) >> 16;
	int levelNext = levelCurrent;

	int chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (unsigned int i = startPos; i < endPos; i++) {
		int ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if (!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if (IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		if (iswordchar(ch) && style == SCE_SMALI_DIRECTIVE && stylePrev != SCE_SMALI_DIRECTIVE) {
			char buf[MAX_WORD_LENGTH + 1];
			LexGetRange(i, styler, IsSmaliWordChar, buf, sizeof(buf));
			if (buf[0] == '.' && (IsFoldWord(&buf[1]) || (strcmp(&buf[1], "field") == 0 && IsAnnotationLine(lineCurrent + 1, styler)))) {
				levelNext++;
			} else if (buf[0] != '.' && (IsFoldWord(buf) || strcmp(buf, "field") == 0)) {
				levelNext--;
			}
		}

		if (!isspacechar(ch))
			visibleChars++;

		if (atEOL || (i == endPos-1)) {
			int levelUse = levelCurrent;
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

