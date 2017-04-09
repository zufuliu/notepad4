// Lexer for Inno Setup.

#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

/*static const char * const innoWordListDesc[] = {
	"Sections",
	"Keywords",
	"Parameters",
	"Preprocessor directives",
	"Pascal keywords",
	"User defined keywords",
	0
};*/

static void ColouriseInnoDoc(Sci_PositionU startPos, Sci_Position length, int, WordList *keywordLists[], Accessor &styler) {
	const WordList &sectionKeywords = *keywordLists[0];
	const WordList &standardKeywords = *keywordLists[1];
	const WordList &parameterKeywords = *keywordLists[2];
	const WordList &preprocessorKeywords = *keywordLists[3];
	const WordList &pascalKeywords = *keywordLists[4];
	const WordList &typeKeywords = *keywordLists[5];

	int state = SCE_INNO_DEFAULT;
	static bool sectionFound = false;
	char ch = 0;
	char chNext = styler[startPos];
	int lengthDoc = startPos + length;
	char *buffer = new char[length + 1];
	Sci_Position bufferCount = 0;
	bool isBOL, isEOL, isWS, isBOLWS = 0;
	bool isCStyleComment = false;

	Sci_Position curLine = styler.GetLine(startPos);
	Sci_Position curLineState = curLine > 0 ? styler.GetLineState(curLine - 1) : 0;
	bool isCode = (curLineState == 1);

	// Go through all provided text segment
	// using the hand-written state machine shown below
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	for (Sci_Position i = startPos; i < lengthDoc; i++) {
		const char chPrev = ch;
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		if (styler.IsLeadByte(ch)) {
			chNext = styler.SafeGetCharAt(i + 2);
			i++;
			continue;
		}

		isBOL = (chPrev == 0) || (chPrev == '\n') || (chPrev == '\r' && ch != '\n');
		isBOLWS = (isBOL) ? 1 : (isBOLWS && (chPrev == ' ' || chPrev == '\t'));
		isEOL = (ch == '\n' || ch == '\r');
		isWS = (ch == ' ' || ch == '\t');

		if ((ch == '\r' && chNext != '\n') || (ch == '\n')) {
			// Remember the line state for future incremental lexing
			curLine = styler.GetLine(i);
			styler.SetLineState(curLine, (isCode ? 1 : 0));
		}

		switch(state) {
			case SCE_INNO_DEFAULT:
				if (!isCode && ch == ';' && isBOLWS) {
					// Start of a comment
					state = SCE_INNO_COMMENT;
				} else if (ch == '[' && isBOLWS) {
					// Start of a section name
					bufferCount = 0;
					state = SCE_INNO_SECTION;
					sectionFound = true;
				} else if (ch == '#' && isBOLWS) {
					// Start of a preprocessor directive
					state = SCE_INNO_PREPROC;
				} else if (!isCode && ch == '{' && chNext != '{' && chPrev != '{') {
					// Start of an inline expansion
					state = SCE_INNO_INLINE_EXPANSION;
				} else if (isCode && (ch == '{' || (ch == '(' && chNext == '*'))) {
					// Start of a Pascal comment
					state = SCE_INNO_COMMENT_PASCAL;
					isCStyleComment = ch == '(';
				} else if ((isCode || !sectionFound) && ch == '/' && chNext == '/') {
					// Apparently, C-style comments are legal, too
					state = SCE_INNO_COMMENT;
					//isCStyleComment = true;
				} else if (ch == '"') {
					// Start of a double-quote string
					state = SCE_INNO_STRING_DOUBLE;
				} else if (ch == '\'') {
					// Start of a single-quote string
					state = SCE_INNO_STRING_SINGLE;
				} else if (IsASCII(ch) && (isalpha(ch) || (ch == '_'))) {
					// Start of an identifier
					bufferCount = 0;
					buffer[bufferCount++] = static_cast<char>(tolower(ch));
					state = SCE_INNO_IDENTIFIER;
				} else {
					// Style it the default style
					styler.ColourTo(i, SCE_INNO_DEFAULT);
				}
				break;

			case SCE_INNO_COMMENT:
				if (isEOL) {
					state = SCE_INNO_DEFAULT;
					styler.ColourTo(i, SCE_INNO_COMMENT);
				}
				break;

			case SCE_INNO_IDENTIFIER:
				if (IsASCII(ch) && (isalnum(ch) || (ch == '_'))) {
					buffer[bufferCount++] = static_cast<char>(tolower(ch));
				} else {
					state = SCE_INNO_DEFAULT;
					buffer[bufferCount] = '\0';

					// Check if the buffer contains a keyword
					if (!isCode && standardKeywords.InList(buffer)) {
						styler.ColourTo(i-1, SCE_INNO_KEYWORD);
					} else if (!isCode && parameterKeywords.InList(buffer)) {
						styler.ColourTo(i-1, SCE_INNO_PARAMETER);
					} else if (isCode && pascalKeywords.InList(buffer)) {
						styler.ColourTo(i-1, SCE_INNO_KEYWORD_PASCAL);
					} else if (isCode && typeKeywords.InList(buffer)) {
						styler.ColourTo(i-1, SCE_INNO_PAS_TYPE);
					} else {
						styler.ColourTo(i-1, SCE_INNO_DEFAULT);
					}

					// Push back the faulty character
					chNext = styler[i--];
					ch = chPrev;
				}
				break;

			case SCE_INNO_SECTION:
				if (ch == ']') {
					state = SCE_INNO_DEFAULT;
					buffer[bufferCount] = '\0';

					 //Check if the buffer contains a section name
					if (sectionKeywords.InList(buffer)) {
						styler.ColourTo(styler.LineStart(styler.GetLine(i)+1)-1, SCE_INNO_SECTION);
						isCode = !CompareCaseInsensitive(buffer, "code");
					} else {
						styler.ColourTo(i, SCE_INNO_DEFAULT);
					}
				} else if (IsASCII(ch) && (isalnum(ch) || (ch == '_'))) {
					buffer[bufferCount++] = static_cast<char>(tolower(ch));
				} else {
					state = SCE_INNO_DEFAULT;
					styler.ColourTo(i, SCE_INNO_DEFAULT);
				}
				break;

			case SCE_INNO_PREPROC:
				if (isWS || isEOL) {
					if (IsASCII(chPrev) && isalpha(chPrev)) {
						state = SCE_INNO_DEFAULT;
						buffer[bufferCount] = '\0';

						// Check if the buffer contains a preprocessor directive
						if (preprocessorKeywords.InList(buffer)) {
							styler.ColourTo(i-1, SCE_INNO_PREPROC);
						} else {
							styler.ColourTo(i-1, SCE_INNO_DEFAULT);
						}

						// Push back the faulty character
						chNext = styler[i--];
						ch = chPrev;
					}
				} else if (IsASCII(ch) && isalpha(ch)) {
					if (chPrev == '#' || chPrev == ' ' || chPrev == '\t')
						bufferCount = 0;
					buffer[bufferCount++] = static_cast<char>(tolower(ch));
				}
				break;

			case SCE_INNO_STRING_DOUBLE:
				if (ch == '"' || isEOL) {
					state = SCE_INNO_DEFAULT;
					styler.ColourTo(i, SCE_INNO_STRING_DOUBLE);
				}
				break;

			case SCE_INNO_STRING_SINGLE:
				if (ch == '\'' || isEOL) {
					state = SCE_INNO_DEFAULT;
					styler.ColourTo(i, SCE_INNO_STRING_SINGLE);
				}
				break;

			case SCE_INNO_INLINE_EXPANSION:
				if (ch == '}') {
					state = SCE_INNO_DEFAULT;
					styler.ColourTo(i, SCE_INNO_INLINE_EXPANSION);
				} else if (isEOL) {
					state = SCE_INNO_DEFAULT;
					styler.ColourTo(i, SCE_INNO_DEFAULT);
				}
				break;

			case SCE_INNO_COMMENT_PASCAL:
				if (isCStyleComment) {
					if (ch == ')' && chPrev == '*') {
						state = SCE_INNO_DEFAULT;
						styler.ColourTo(i, SCE_INNO_COMMENT_PASCAL);
					}
				} else {
					if (ch == '}') {
						state = SCE_INNO_DEFAULT;
						styler.ColourTo(i, SCE_INNO_COMMENT_PASCAL);
					}
				}
				break;

		}
	}
	delete []buffer;
}

#define LexMatchIC(pos, str)	LexMatchIgnoreCase(pos, styler, str)
#define IsCommentLine(line)		IsLexCommentLine(line, styler, SCE_INNO_COMMENT)
static inline bool IsStreamCommentStyle(int style) {
	return style == SCE_INNO_COMMENT_PASCAL;
}
static bool IsSectionEnd(Sci_Position curPos, Accessor &styler) {
	Sci_Position curLine = styler.GetLine(curPos);
	Sci_Position pos = LexSkipSpaceTab(styler.LineStart(curLine+1), styler.LineStart(curLine+2)-1, styler);
	char ch = styler.SafeGetCharAt(pos);
	if (ch == '[' && styler.StyleAt(pos) == SCE_INNO_SECTION)
		return true;
	return false;
}

static void FoldInnoDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, WordList *[], Accessor &styler) {
	if (styler.GetPropertyInt("fold") == 0)
		return;
	const bool foldComment = styler.GetPropertyInt("fold.comment", 1) != 0;
	const bool foldPreprocessor = styler.GetPropertyInt("fold.preprocessor", 1) != 0;
	//const bool foldCompact = styler.GetPropertyInt("fold.compact", 0) != 0;

	Sci_PositionU endPos = startPos + length;
	static int sectionFound = -1;
	//int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent-1) >> 16;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		if (foldComment && IsStreamCommentStyle(style)) {
			if (!IsStreamCommentStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamCommentStyle(styleNext) && !atEOL) {
				levelNext--;
			}
		}
		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if(!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if(IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}

		if (ch == '[' && style == SCE_INNO_SECTION) {
			Sci_Position curLine = styler.GetLine(i);
			if (sectionFound == -1 || sectionFound > curLine)
				sectionFound = curLine;
			levelNext++;
		}
		if ((sectionFound != -1) && lineCurrent > sectionFound && atEOL && IsSectionEnd(i, styler)) {
			levelNext--;
		}

		if (foldPreprocessor && ch == '#' && style == SCE_INNO_PREPROC) {
			Sci_Position pos = LexSkipSpaceTab(i+1, endPos, styler);
			if (LexMatchIC(pos, "if")) {
				levelNext++;
			} else if (LexMatchIC(pos, "end")) {
				levelNext--;
			}
		}

		if (style == SCE_INNO_KEYWORD_PASCAL) {
			if (LexMatchIC(i, "begin") || LexMatchIC(i, "case") || LexMatchIC(i, "try") || LexMatchIC(i, "record")
				|| LexMatchIC(i, "interface")) {
				levelNext++;
			} else if (LexMatchIC(i, "end")) {
				levelNext--;
			}
		}

		//if (!isspacechar(ch))
		//	visibleChars++;
		if (atEOL || (i == endPos-1)) {
			int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			//if (visibleChars == 0 && foldCompact)
			//	lev |= SC_FOLDLEVELWHITEFLAG;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelCurrent = levelNext;
			//visibleChars = 0;
		}
	}
}

LexerModule lmInno(SCLEX_INNOSETUP, ColouriseInnoDoc, "inno", FoldInnoDoc);
