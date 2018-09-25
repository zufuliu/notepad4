// Edit AutoComplete

#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <limits.h>
#include "Edit.h"
#include "Styles.h"
#include "Helpers.h"
#include "SciCall.h"
#include "resource.h"

static inline BOOL IsASpace(int ch) {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

static inline BOOL IsAAlpha(int ch) {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

static inline BOOL IsWordStart(int ch) {
	return ch != '.' && IsDocWordChar(ch);
}

static inline BOOL IsEscapeChar(int ch) {
	return ch == 't' || ch == 'n' || ch == 'r' || ch == 'a' || ch == 'b' || ch == 'v' || ch == 'f'
		   || ch == '$'; // PHP
	// x u U
}

static inline BOOL IsCppCommentStyle(int style) {
	return style == SCE_C_COMMENT || style == SCE_C_COMMENTLINE
		   || style == SCE_C_COMMENTDOC || style == SCE_C_COMMENTLINEDOC
		   || style == SCE_C_COMMENTDOC_TAG || style == SCE_C_COMMENTDOC_TAG_XML;
}

static inline BOOL IsCppStringStyle(int style) {
	return style == SCE_C_STRING || style == SCE_C_CHARACTER || style == SCE_C_STRINGEOL || style == SCE_C_STRINGRAW
		   || style == SCE_C_VERBATIM || style == SCE_C_DSTRINGX || style == SCE_C_DSTRINGQ || style == SCE_C_DSTRINGT;
}

static inline BOOL IsSpecialStartChar(int ch, int chPrev) {
	return (ch == '.')	// member
		   || (ch == '#')	// preprocessor
		   || (ch == '@') // Java/PHP/Doxygen Doc Tag
		   // ObjC Keyword, Java Annotation, Python Decorator, Cobra Directive
		   || (ch == '<') // HTML/XML Tag, C# Doc Tag
		   || (ch == '\\')// Doxygen Doc Tag, LaTeX Command
		   || (chPrev == '<' && ch == '/')	// HTML/XML Close Tag
		   || (chPrev == '-' && ch == '>')	// member(C/C++/PHP)
		   || (chPrev == ':' && ch == ':');// namespace(C++), static member(C++/Java8/PHP)
}


#include "EditAutoC_Data0.c"
#include "EditAutoC_WordList.c"

//=============================================================================
//
// EditCompleteWord()
// Auto-complete words
//

// item count in AutoComplete List
extern int	iAutoCItemCount;
extern BOOL	bAutoCompleteWords;
extern int	iAutoCDefaultShowItemCount;
extern int	iAutoCMinWordLength;
extern int	iAutoCMinNumberLength;
extern BOOL bAutoCIncludeDocWord;

BOOL IsDocWordChar(int ch) {
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '_' || ch == '.') {
		return TRUE;
	}

	switch (pLexCurrent->rid) {
	case NP2LEX_CPP:
		return (ch == '#' || ch == '@' || ch == ':');
	case NP2LEX_CSHARP:
		return (ch == '#' || ch == '@');
	case NP2LEX_JAVA:
		return (ch == '$' || ch == '@' || ch == ':');
	case NP2LEX_GROOVY:
	case NP2LEX_SCALA:
	case NP2LEX_PYTHON:
	case NP2LEX_PERL:
	case NP2LEX_RUBY:
	case NP2LEX_SQL:
		return (ch == '$' || ch == '@');
	case NP2LEX_RC:
	case NP2LEX_D:
	case NP2LEX_HAXE:
	case NP2LEX_VB:
		return (ch == '#');
	case NP2LEX_PHP:
	case NP2LEX_HTML:
	case NP2LEX_PS1:
	case NP2LEX_BATCH:
	case NP2LEX_BASH:
		return (ch == '-' || ch == '$');
	case NP2LEX_MAKE:
		return (ch == '-' || ch == '$' || ch == '!');
	case NP2LEX_XML:
		return (ch == '-' || ch == ':');
	case NP2LEX_LLVM:
		return (ch == '@' || ch == '%');
	case NP2LEX_CSS:
	case NP2LEX_SMALI:
	case NP2LEX_LISP:
	case NP2LEX_DOT:
	case NP2LEX_ANSI:
		return (ch == '-');
	}
	return FALSE;
}

static inline BOOL IsOperatorStyle(int style) {
	switch (pLexCurrent->iLexer) {
	case SCLEX_CPP:
	case SCLEX_JSON:
	case SCLEX_NSIS:
	case SCLEX_GRAPHVIZ:
	case SCLEX_LISP:
		return style == SCE_C_OPERATOR;
	case SCLEX_SMALI:
		return style == SCE_SMALI_OPERATOR;
	case SCLEX_PYTHON:
		return style == SCE_PY_OPERATOR;
	case SCLEX_RUBY:
		return style == SCE_RB_OPERATOR;
	case SCLEX_SQL:
		return style == SCE_SQL_OPERATOR;
	case SCLEX_TCL:
		return style == SCE_TCL_OPERATOR;
	case SCLEX_VB:
	case SCLEX_VBSCRIPT:
		return style == SCE_B_OPERATOR;
	case SCLEX_VERILOG:
		return style == SCE_V_OPERATOR;
	case SCLEX_VHDL:
		return style == SCE_VHDL_OPERATOR;
	case SCLEX_PERL:
		return style == SCE_PL_OPERATOR;
	case SCLEX_MATLAB:
		return style == SCE_MAT_OPERATOR;
	case SCLEX_LUA:
		return style == SCE_LUA_OPERATOR;
	case SCLEX_ASM:
		return style == SCE_ASM_OPERATOR;
	case SCLEX_AU3:
		return style == SCE_AU3_OPERATOR;
	case SCLEX_BASH:
		return style == SCE_SH_OPERATOR;
	case SCLEX_BATCH:
		return style == SCE_BAT_OPERATOR;
	case SCLEX_CMAKE:
		return style == SCE_CMAKE_OPERATOR;
	case SCLEX_CONF:
		return style == SCE_CONF_OPERATOR;
	case SCLEX_CSS:
		return style == SCE_CSS_OPERATOR;
	case SCLEX_FORTRAN:
		return style == SCE_F_OPERATOR;
	case SCLEX_FSHARP:
		return style == SCE_FSHARP_OPERATOR;
	case SCLEX_HTML:
		return style == SCE_HPHP_OPERATOR || style == SCE_HJ_SYMBOLS || style == SCE_HJA_SYMBOLS;
	case SCLEX_LATEX:
	case SCLEX_TEXINFO:
		return style == SCE_L_OPERATOR;
	case SCLEX_MAKEFILE:
		return style == SCE_MAKE_OPERATOR;
	case SCLEX_PASCAL:
		return style == SCE_PAS_OPERATOR;
	case SCLEX_POWERSHELL:
		return style == SCE_POWERSHELL_OPERATOR;
	}
	return FALSE;
}

static inline BOOL IsWordStyleToIgnore(int style) {
	switch (pLexCurrent->iLexer) {
	case SCLEX_CPP:
		return style == SCE_C_WORD || style == SCE_C_WORD2 || style == SCE_C_PREPROCESSOR;
	case SCLEX_PYTHON:
		return style == SCE_PY_WORD || style == SCE_PY_WORD2 || style == SCE_PY_BUILDIN_CONST || style == SCE_PY_BUILDIN_FUNC || style == SCE_PY_ATTR || style == SCE_PY_OBJ_FUNC;
	case SCLEX_JSON:
		return style == SCE_C_WORD;
	case SCLEX_SQL:
		return style == SCE_SQL_WORD || style == SCE_SQL_WORD2 || style == SCE_SQL_USER1 || style == SCE_SQL_HEX || style == SCE_SQL_HEX2;
	case SCLEX_SMALI:
		return style == SCE_SMALI_WORD || style == SCE_SMALI_DIRECTIVE || style == SCE_SMALI_INSTRUCTION;
	}
	return FALSE;
}

// https://en.wikipedia.org/wiki/Printf_format_string
static inline BOOL IsStringFormatChar(int ch, int style) {
	if (!IsAAlpha(ch)) {
		return FALSE;
	}
	switch (pLexCurrent->iLexer) {
	case SCLEX_CPP:
		return style != SCE_C_OPERATOR;
	case SCLEX_FSHARP:
		return style != SCE_FSHARP_OPERATOR;
	case SCLEX_LUA:
		return style != SCE_LUA_OPERATOR;
	case SCLEX_MATLAB:
		return style != SCE_MAT_OPERATOR;
	case SCLEX_PERL:
		return style != SCE_PL_OPERATOR;
	case SCLEX_PYTHON:
		return style != SCE_PY_OPERATOR;
	case SCLEX_RUBY:
		return style != SCE_RB_OPERATOR;
	case SCLEX_TCL:
		return style != SCE_TCL_OPERATOR;
	}
	return FALSE;
}

static inline BOOL NeedSpaceAfterKeyword(const char *word, int length) {
	const char *p = StrStrA(
		" if for try using while elseif switch foreach synchronized "
		, word);
	return p != NULL && p[-1] == ' ' && p[length] == ' ';
}

void AutoC_AddDocWord(HWND hwnd, struct WordList *pWList, BOOL bIgnore) {
	UNREFERENCED_PARAMETER(hwnd);

	LPCSTR pRoot = pWList->pWordStart;
	int iRootLen = pWList->iStartLen;
	struct Sci_TextToFind ft = {{0, 0}, 0, {0, 0}};
	struct Sci_TextRange tr = { { 0, -1 }, NULL };
	Sci_Position iCurrentPos = SciCall_GetCurrentPos() - iRootLen;
	int iDocLen = SciCall_GetLength();
	int findFlag = bIgnore ? SCFIND_WORDSTART : (SCFIND_WORDSTART | SCFIND_MATCHCASE);
	int iPosFind;

	ft.lpstrText = pRoot;
	ft.chrg.cpMax = iDocLen;
	iPosFind = SciCall_FindText(findFlag, &ft);

	while (iPosFind >= 0 && iPosFind < iDocLen) {
		int wordEnd = iPosFind + iRootLen;
		int style = SciCall_GetStyleAt(wordEnd - 1);
		if (iPosFind != iCurrentPos && !IsWordStyleToIgnore(style)) {
			int chPrev, ch = *pRoot, chNext = SciCall_GetCharAt(wordEnd);
			int wordLength = -iPosFind;
			BOOL bSubWord = FALSE;
			while (wordEnd < iDocLen) {
				chPrev = ch;
				ch = chNext;
				chNext = SciCall_GetCharAt(wordEnd + 1);
				if (!IsDocWordChar(ch) || (ch == ':' && pLexCurrent->iLexer == SCLEX_CPP)) {
					if ((ch == ':' && chNext == ':' && IsDocWordChar(chPrev))
							|| (chPrev == ':' && ch == ':' && IsDocWordChar(chNext))
							|| (ch == '-' && chNext == '>' && IsDocWordChar(chPrev))
							|| (chPrev == '-' && ch == '>' && IsDocWordChar(chNext))
							|| (ch == '-' && !IsOperatorStyle(SciCall_GetStyleAt(wordEnd)))
					   ) {
						bSubWord = TRUE;
					} else {
						break;
					}
				}
				if (ch == '.' || ch == ':') {
					bSubWord = TRUE;
				}
				wordEnd++;
			}
			wordLength += wordEnd;

			if (wordLength >= iRootLen) {
				char *pWord = pWList->wordBuf + DefaultAlignment;
				bIgnore = TRUE;
				tr.lpstrText = pWord;
				tr.chrg.cpMin = iPosFind;
				tr.chrg.cpMax = wordEnd;
				if (wordLength > NP2_AUTOC_MAX_WORD_LENGTH) {
					tr.chrg.cpMax = iPosFind + NP2_AUTOC_MAX_WORD_LENGTH;
				}

				SciCall_GetTextRange(&tr);
				ch = SciCall_GetCharAt(iPosFind - 1);
				// word after escape char or format char
				chPrev = SciCall_GetCharAt(iPosFind - 2);
				if (chPrev != '\\' && ch == '\\' && IsEscapeChar(*pWord)) {
					pWord++;
					--wordLength;
					bIgnore = FALSE;
				} else if (ch == '%' && IsStringFormatChar(*pWord, SciCall_GetStyleAt(iPosFind - 1))) {
					pWord++;
					--wordLength;
					bIgnore = FALSE;
				}
				//if (pLexCurrent->rid == NP2LEX_PHP && wordLength >= 2 && *pWord == '$' && *(pWord+1) == '$') {
				//	pWord++;
				//	--wordLength;
				//	bIgnore = FALSE;
				//}
				while (wordLength > 0 && (pWord[wordLength - 1] == '-' || pWord[wordLength - 1] == ':' || pWord[wordLength - 1] == '.')) {
					--wordLength;
					pWord[wordLength] = '\0';
				}
				if (!bIgnore) {
					CopyMemory(pWList->wordBuf, pWord, wordLength + 1);
					pWord = pWList->wordBuf;
					bIgnore = wordLength >= iRootLen && WordList_StartsWith(pWList, pWord);
				}
				ch = *pWord;
				if (bIgnore && !(ch == ':' && *(pWord + 1) != ':')) {
					int count = 0;
					if (!(pLexCurrent->iLexer == SCLEX_CPP && style == SCE_C_MACRO)) {
						while (IsASpace(SciCall_GetCharAt(wordEnd))) {
							++count;
							wordEnd++;
						}
					}
					if (SciCall_GetCharAt(wordEnd) == '(') {
						if (count && NeedSpaceAfterKeyword(pWord, wordLength)) {
							pWord[wordLength++] = ' ';
						}

						pWord[wordLength++] = '(';
						pWord[wordLength++] = ')';
						//} else if (SciCall_GetCharAt(wordEnd) == '[') {
						//	pWord[wordLength++] = '[';
						//	pWord[wordLength++] = ']';
					}
					if (wordLength >= iRootLen) {
						if (bSubWord && !(ch >= '0' && ch <= '9')) {
							int i;
							ch = 0; chNext = *pWord;
							for (i = 0; i < wordLength - 1; i++) {
								chPrev = ch;
								ch = chNext;
								chNext = pWord[i + 1];
								if (i >= iRootLen && (ch == '.' || ch == ':' || ch == '-')
										&& !(chPrev == '.' || chPrev == ':' || chPrev == '-')
										&& !(ch == '-' && (chNext >= '0' && chNext <= '9'))) {
									pWord[i] = '\0';
									WordList_AddWord(pWList, pWord, i);
									pWord[i] = (char)ch;
								}
							}
						}
						pWord[wordLength] = '\0';
						WordList_AddWord(pWList, pWord, wordLength);
					}
				}
			}
		}

		ft.chrg.cpMin = wordEnd;
		iPosFind = SciCall_FindText(findFlag, &ft);
	}
}

void AutoC_AddKeyword(struct WordList *pWList, int iCurrentStyle) {
	for (int i = 0; i < NUMKEYWORD; i++) {
		const char *pKeywords = pLexCurrent->pKeyWords->pszKeyWords[i];
		if (pKeywords && *pKeywords && !(currentLexKeywordAttr[i] & KeywordAttr_NoAutoComp)) {
			WordList_AddList(pWList, pKeywords);
		}
	}

	// additional keywords
	if (np2_LexKeyword && !(pLexCurrent->iLexer == SCLEX_CPP && !IsCppCommentStyle(iCurrentStyle))) {
		WordList_AddList(pWList, (*np2_LexKeyword)[0]);
		WordList_AddList(pWList, (*np2_LexKeyword)[1]);
		WordList_AddList(pWList, (*np2_LexKeyword)[2]);
		WordList_AddList(pWList, (*np2_LexKeyword)[3]);
	}
}

INT AutoC_AddSpecWord(struct WordList *pWList, int iCurrentStyle, int ch, int chPrev) {
	if (pLexCurrent->iLexer == SCLEX_CPP && IsCppCommentStyle(iCurrentStyle) && np2_LexKeyword) {
		if ((ch == '@' && (*np2_LexKeyword == kwJavaDoc || *np2_LexKeyword == kwPHPDoc || *np2_LexKeyword == kwDoxyDoc))
				|| (ch == '\\' && *np2_LexKeyword == kwDoxyDoc)
				|| ((ch == '<' || chPrev == '<') && *np2_LexKeyword == kwNETDoc)) {
			WordList_AddList(pWList, (*np2_LexKeyword)[0]);
			WordList_AddList(pWList, (*np2_LexKeyword)[1]);
			WordList_AddList(pWList, (*np2_LexKeyword)[2]);
			WordList_AddList(pWList, (*np2_LexKeyword)[3]);
			return 0;
		}
		if (ch == '#' && (*np2_LexKeyword == kwJavaDoc) && IsWordStart(*(pWList->pWordStart))) { // package.Class#member
			return 1;
		}
	} else if ((pLexCurrent->rid == NP2LEX_HTML || pLexCurrent->rid == NP2LEX_XML || pLexCurrent->rid == NP2LEX_CONF)
			   && ((ch == '<') || (chPrev == '<' && ch == '/'))) {
		if (pLexCurrent->rid == NP2LEX_HTML || pLexCurrent->rid == NP2LEX_CONF) { // HTML Tag
			WordList_AddList(pWList, pLexCurrent->pKeyWords->pszKeyWords[0]);// HTML Tag
		} else {
			WordList_AddList(pWList, pLexCurrent->pKeyWords->pszKeyWords[3]);// XML Tag
			if (np2_LexKeyword) { // XML Tag
				WordList_AddList(pWList, (*np2_LexKeyword)[0]);
			}
		}
		return 0;
	} else if ((pLexCurrent->iLexer == SCLEX_CPP && iCurrentStyle == SCE_C_DEFAULT)
			   || (pLexCurrent->iLexer == SCLEX_PYTHON && iCurrentStyle == SCE_PY_DEFAULT)
			   || (pLexCurrent->rid == NP2LEX_VB && iCurrentStyle == SCE_B_DEFAULT)
			   || (pLexCurrent->iLexer == SCLEX_SMALI && iCurrentStyle == SCE_C_DEFAULT)) {
		if (ch == '#' && pLexCurrent->iLexer == SCLEX_CPP) { // #preprocessor
			const char *pKeywords = pLexCurrent->pKeyWords->pszKeyWords[2];
			if (pKeywords && *pKeywords) {
				WordList_AddList(pWList, pKeywords);
				return 0;
			}
		} else if (ch == '#' && pLexCurrent->rid == NP2LEX_VB) { // #preprocessor
			const char *pKeywords = pLexCurrent->pKeyWords->pszKeyWords[3];
			if (pKeywords && *pKeywords) {
				WordList_AddList(pWList, pKeywords);
				return 0;
			}
		} else if (ch == '@') { // @directive, @annotation, @decorator
			if (pLexCurrent->rid == NP2LEX_CSHARP) { // verbatim identifier
				//WordList_AddList(pWList, pLexCurrent->pKeyWords->pszKeyWords[0]);
				//WordList_AddList(pWList, pLexCurrent->pKeyWords->pszKeyWords[1]);
				//return 0;
			} else {
				const char *pKeywords = pLexCurrent->pKeyWords->pszKeyWords[3];
				if (pKeywords && *pKeywords) {
					WordList_AddList(pWList, pKeywords);
					return 0; // user defined annotation
				}
			}
		} else if (ch == '.' && pLexCurrent->iLexer == SCLEX_SMALI) {
			WordList_AddList(pWList, pLexCurrent->pKeyWords->pszKeyWords[9]);
			return 0;
		}
		//else if (chPrev == ':' && ch == ':') {
		//	WordList_AddList(pWList, "C++/namespace C++/Java8/PHP/static SendMessage()");
		//	return 0;
		//}
		//else if (chPrev == '-' && ch == '>') {
		//	WordList_AddList(pWList, "C/C++pointer PHP-variable");
		//	return 0;
		//}
	}
	return 1;
}

void EditCompleteWord(HWND hwnd, BOOL autoInsert) {
	const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
	const int iCurrentStyle = SciCall_GetStyleAt(iCurrentPos);
	int iLine = SciCall_LineFromPosition(iCurrentPos);
	const int iLineStartPos = SciCall_PositionFromLine(iLine);
	Sci_Position iCurrentLinePos = iCurrentPos - iLineStartPos;
	Sci_Position iStartWordPos = iCurrentLinePos;

	char *pLine;
	char *pRoot = NULL;
	char *pSubRoot = NULL;
	int iRootLen;
	int iDocLen;
#ifdef NDEBUG
	int ch = 0, chPrev = 0;
#else
	char ch = 0, chPrev = 0;
#endif

	BOOL bIgnore = FALSE; // ignore number
	struct WordList *pWList = NULL;

	iAutoCItemCount = 0; // recreate list

	iDocLen = SciCall_GetLineLength(iLine);
	pLine = NP2HeapAlloc(iDocLen + 1);
	SciCall_GetLine(iLine, pLine);
	iRootLen = iAutoCMinWordLength;

	while (iStartWordPos > 0 && IsDocWordChar(pLine[iStartWordPos - 1])) {
		iStartWordPos--;
		if (iStartWordPos >= 0 && IsSpecialStartChar(pLine[iStartWordPos], '\0')) {
			iStartWordPos++;
			break;
		}
	}
	if (iStartWordPos >= 0 && pLine[iStartWordPos] == ':') {
		if (iStartWordPos < iDocLen && pLine[iStartWordPos + 1] != ':') {
			iStartWordPos++;
		}
	}
	if (iStartWordPos >= 0 && pLine[iStartWordPos] >= '0' && pLine[iStartWordPos] <= '9') {
		if (iAutoCMinNumberLength <= 0) {
			bIgnore = TRUE;
		} else {
			iRootLen = iAutoCMinNumberLength;
			if (iStartWordPos < iDocLen && !(pLine[iStartWordPos + 1] >= '0' && pLine[iStartWordPos + 1] <= '9')) {
				iRootLen += 2;
			}
		}
	}
	// word after escape char
	if (iStartWordPos > 1) {
		if (pLine[iStartWordPos - 1] == '\\' && IsEscapeChar(pLine[iStartWordPos])) {
			if (!(iStartWordPos > 2 && pLine[iStartWordPos - 2] == '\\')) {
				++iStartWordPos;
			}
		} else if (pLine[iStartWordPos - 1] == '%' && IsStringFormatChar(pLine[iStartWordPos], SciCall_GetStyleAt(iLineStartPos + iStartWordPos - 1))) {
			++iStartWordPos;
		}
	}

	if (iStartWordPos > 0) {
		iLine = iStartWordPos - 1;
		ch = pLine[iLine];
		if (iLine > 0) {
			chPrev = pLine[iLine - 1];
		}
		if (pLexCurrent->rid == NP2LEX_CPP && (ch == '#' || IsASpace(ch))) {
			while (iLine >= 0 && IsASpace(ch)) {
				ch = pLine[iLine--];
			}
			while (iLine >= 0 && IsASpace(pLine[iLine--])){}
			if (iLine > 0) {
				ch = '\0';
				chPrev = '\0';
			}
		}
	}

	if (iStartWordPos == iCurrentLinePos || bIgnore || (iCurrentLinePos - iStartWordPos < iRootLen)) {
		if (!IsSpecialStartChar(ch, chPrev)) {
			NP2HeapFree(pLine);
			return;
		}
	}

	if (iRootLen) {
		pRoot = NP2HeapAlloc(iCurrentLinePos - iStartWordPos + iRootLen);
		lstrcpynA(pRoot, pLine + iStartWordPos, iCurrentLinePos - iStartWordPos + 1);
		iRootLen = lstrlenA(pRoot);
		if (!iRootLen) {
			NP2HeapFree(pRoot);
			NP2HeapFree(pLine);
			return;
		}
		// word started with number
		if (!autoInsert && !IsWordStart(*pRoot)) {
			if (!IsSpecialStartChar(ch, chPrev)) {
				NP2HeapFree(pRoot);
				NP2HeapFree(pLine);
				return;
			}
		}
		pSubRoot = pRoot;
	}

	bIgnore = (iStartWordPos >= 0 && pLine[iStartWordPos] >= '0' && pLine[iStartWordPos] <= '9');
	pWList = WordList_Alloc(pRoot, iRootLen, bIgnore);
	if (bIgnore) {
		goto label_add_doc_word;
	}
//#ifndef NDEBUG
//goto label_add_doc_word;
//#endif

	if (IsSpecialStartChar(ch, chPrev)) {
		if (!AutoC_AddSpecWord(pWList, iCurrentStyle, ch, chPrev) && pWList->nWordCount > 0) {
			goto label_show_word_list;
		}
		pSubRoot = StrPBrkA(pRoot, ":.#@<\\/->");
		if (pSubRoot) {
			pSubRoot = pRoot;
			goto label_show_word_list;
		}
	}

label_retry:
	// keywords
	AutoC_AddKeyword(pWList, iCurrentStyle);
label_add_doc_word:
	if (bAutoCIncludeDocWord) {
		AutoC_AddDocWord(hwnd, pWList, bIgnore);
	}
label_show_word_list:
	iAutoCItemCount = pWList->nWordCount;
	if (iAutoCItemCount == 0 && pSubRoot && *pSubRoot) {
		pSubRoot = StrPBrkA(pSubRoot, ":.#@<\\/->");
		if (pSubRoot) {
			while (*pSubRoot && StrChrA(":.#@<\\/->", *pSubRoot)) {
				pSubRoot++;
			}
		}
		if (pSubRoot && *pSubRoot) {
			pWList->pWordStart = pSubRoot;
			pWList->iStartLen = lstrlenA(pSubRoot);
			pWList->iMaxLength = pWList->iStartLen;
			goto label_retry;
		}
	}
	if (iAutoCItemCount > 0) {
		char *pList = NULL;
		int maxWordLength = pWList->iMaxLength;
		if (iAutoCItemCount == 1 && maxWordLength == iRootLen) {
			WordList_Free(pWList);
			goto end;
		}
		WordList_GetList(pWList, &pList);
		//DLog(pList);
		SendMessage(hwnd, SCI_AUTOCSETORDER, SC_ORDER_PRESORTED, 0); // pre-sorted
		SendMessage(hwnd, SCI_AUTOCSETIGNORECASE, 1, 0); // case insensitive
		SendMessage(hwnd, SCI_AUTOCSETSEPARATOR, '\n', 0);
		SendMessage(hwnd, SCI_AUTOCSETFILLUPS, 0, (LPARAM)" \t\n\r;,([]){}\\/");
		SendMessage(hwnd, SCI_AUTOCSETCHOOSESINGLE, 0, 0);
		//SendMessage(hwnd, SCI_AUTOCSETDROPRESTOFWORD, 1, 0); // delete orginal text: pRoot
		maxWordLength <<= 1;
		SendMessage(hwnd, SCI_AUTOCSETMAXWIDTH, maxWordLength, 0); // width columns, default auto
		maxWordLength = iAutoCItemCount;
		if (maxWordLength > iAutoCDefaultShowItemCount) {
			maxWordLength = iAutoCDefaultShowItemCount;
		}
		SendMessage(hwnd, SCI_AUTOCSETMAXHEIGHT, maxWordLength, 0); // height rows, default 5
		SendMessage(hwnd, SCI_AUTOCSHOW, pWList->iStartLen, (LPARAM)(pList));
		NP2HeapFree(pList);
	}
end:
	NP2HeapFree(pLine);
	NP2HeapFree(pRoot);
	NP2HeapFree(pWList);
}


void EditAutoCloseBraceQuote(HWND hwnd, int ch) {
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	int chPrev = SciCall_GetCharAt(iCurPos - 2);
	int chNext = SciCall_GetCharAt(iCurPos);
	int iCurrentStyle = SciCall_GetStyleAt(iCurPos - 2);
	int iNextStyle = SciCall_GetStyleAt(iCurPos);
	char tchIns[2] = "";
	if (pLexCurrent->iLexer == SCLEX_CPP) {
		// within char
		if (iCurrentStyle == SCE_C_CHARACTER && iNextStyle == SCE_C_CHARACTER && pLexCurrent->rid != NP2LEX_PHP) {
			return;
		}
		if (ch == '`' && !IsCppStringStyle(iCurrentStyle)) {
			return;
		}
	}
	switch (ch) {
	case '(':
		tchIns[0] = ')';
		break;
	case '[':
		if (!(pLexCurrent->rid == NP2LEX_SMALI)) { // Smali array type
			tchIns[0] = ']';
		}
		break;
	case '{':
		tchIns[0] = '}';
		break;
	case '<':
		if (pLexCurrent->rid == NP2LEX_CPP || pLexCurrent->rid == NP2LEX_CSHARP || pLexCurrent->rid == NP2LEX_JAVA) {
			// geriatric type, template
			if (iCurrentStyle == SCE_C_CLASS || iCurrentStyle == SCE_C_INTERFACE || iCurrentStyle ==  SCE_C_STRUCT) {
				tchIns[0] = '>';
			}
		}
		break;
	case '\"':
		if (chPrev != '\\') {
			tchIns[0] = '\"';
		}
		break;
	case '\'':
		if (chPrev != '\\' && !(pLexCurrent->iLexer == SCLEX_NULL	// someone's
								|| pLexCurrent->iLexer == SCLEX_HTML || pLexCurrent->iLexer == SCLEX_XML
								|| pLexCurrent->iLexer == SCLEX_VB			// line comment
								|| pLexCurrent->iLexer == SCLEX_VBSCRIPT	// line comment
								|| pLexCurrent->iLexer == SCLEX_VERILOG		// inside number
								|| pLexCurrent->iLexer == SCLEX_LISP 		// operator
								|| (pLexCurrent->iLexer == SCLEX_CPP && iCurrentStyle == SCE_C_NUMBER)
							   )) {
			tchIns[0] = '\'';
		}
		break;
	case '`':
		//if (pLexCurrent->iLexer == SCLEX_BASH
		//|| pLexCurrent->rid == NP2LEX_JULIA
		//|| pLexCurrent->iLexer == SCLEX_MAKEFILE
		//|| pLexCurrent->iLexer == SCLEX_SQL
		//) {
		//	tchIns[0] = '`';
		//} else if (0) {
		//	tchIns[0] = '\'';
		//}
		tchIns[0] = '`';
		break;
	case ',':
		if (!(chNext == ' ' || chNext == '\t' || (chPrev == '\'' && chNext == '\'') || (chPrev == '\"' && chNext == '\"'))) {
			tchIns[0] = ' ';
		}
		break;
	default:
		break;
	}
	if (tchIns[0]) {
		SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
		SendMessage(hwnd, SCI_REPLACESEL, 0, (LPARAM)tchIns);
		if (ch == ',') {
			iCurPos++;
		}
		SendMessage(hwnd, SCI_SETSEL, iCurPos, iCurPos);
		SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
	}
}

static inline BOOL IsHtmlVoidTag(const char *word, int length) {
	// see classifyTagHTML() in LexHTML.cxx
	const char *p = StrStrIA(
		// void elements
		" area base basefont br col command embed frame hr img input isindex keygen link meta param source track wbr "
		, word);
	return p != NULL && p[-1] == ' ' && p[length] == ' ';
}

void EditAutoCloseXMLTag(HWND hwnd) {
	char tchBuf[512];
	Sci_Position iCurPos = SciCall_GetCurrentPos();
	int	 iHelper = iCurPos - (COUNTOF(tchBuf) - 1);
	int	 iStartPos = max_i(0, iHelper);
	int	 iSize = iCurPos - iStartPos;
	BOOL autoClosed = FALSE;

	if (pLexCurrent->iLexer == SCLEX_CPP) {
		int iCurrentStyle = SciCall_GetStyleAt(iCurPos);
		if (iCurrentStyle == SCE_C_OPERATOR || iCurrentStyle == SCE_C_DEFAULT) {
			iHelper = FALSE;
		} else {
			int iLine = SciCall_LineFromPosition(iCurPos);
			Sci_Position iCurrentLinePos = SciCall_PositionFromLine(iLine);
			while (iCurrentLinePos < iCurPos && IsASpace(SciCall_GetCharAt(iCurrentLinePos))) {
				iCurrentLinePos++;
			}
			iCurrentStyle = SciCall_GetStyleAt(iCurrentLinePos);
			if (SciCall_GetCharAt(iCurrentLinePos) == '#' && iCurrentStyle == SCE_C_PREPROCESSOR) {
				iHelper = FALSE;
			}
		}
	}

	if (iSize >= 3 && iHelper) {
		struct Sci_TextRange tr;
		tr.chrg.cpMin = iStartPos;
		tr.chrg.cpMax = iCurPos;
		tr.lpstrText = tchBuf;
		SendMessage(hwnd, SCI_GETTEXTRANGE, 0, (LPARAM)&tr);

		if (tchBuf[iSize - 2] != '/') {
			char tchIns[516] = "</";
			int	 cchIns = 2;
			const char *pBegin = tchBuf;
			const char *pCur = tchBuf + iSize - 2;

			while (pCur > pBegin && *pCur != '<' && *pCur != '>') {
				--pCur;
			}

			if (*pCur == '<') {
				pCur++;
				while (StrChrA(":_-.", *pCur) || IsCharAlphaNumericA(*pCur)) {
					tchIns[cchIns++] = *pCur;
					pCur++;
				}
			}

			tchIns[cchIns++] = '>';
			tchIns[cchIns] = '\0';

			iHelper = cchIns > 3;
			if (iHelper && pLexCurrent->iLexer == SCLEX_HTML) {
				tchIns[cchIns - 1] = '\0';
				iHelper = !IsHtmlVoidTag(tchIns + 2, cchIns - 3);
			}
			if (iHelper) {
				tchIns[cchIns - 1] = '>';
				autoClosed = TRUE;
				SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
				SendMessage(hwnd, SCI_REPLACESEL, 0, (LPARAM)tchIns);
				SendMessage(hwnd, SCI_SETSEL, iCurPos, iCurPos);
				SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
			}
		}
	}
	if (!autoClosed && bAutoCompleteWords) {
		iCurPos = SciCall_GetCurrentPos();
		if (SciCall_GetCharAt(iCurPos - 2) == '-') {
			EditCompleteWord(hwnd, FALSE); // obj->field, obj->method
		}
	}
}


BOOL IsIndentKeywordStyle(int style) {
	switch (pLexCurrent->iLexer) {
	//case SCLEX_CPP:
	//	return style == SCE_C_PREPROCESSOR;
	//case SCLEX_VB:
	//case SCLEX_VBSCRIPT:
	//	return style == SCE_B_KEYWORD;
	case SCLEX_RUBY:
		return style == SCE_RB_WORD;
	case SCLEX_MATLAB:
		return style == SCE_MAT_KEYWORD;
	case SCLEX_LUA:
		return style == SCE_LUA_WORD;
	case SCLEX_MAKEFILE:
		return style == SCE_MAKE_PREPROCESSOR;
	case SCLEX_BASH:
		return style == SCE_BAT_WORD;
	case SCLEX_CMAKE:
		return style == SCE_CMAKE_WORD;
	//case SCLEX_VHDL:
	//	return style == SCE_VHDL_KEYWORD;
	//case SCLEX_VERILOG:
	//	return style == SCE_V_WORD;
	//case SCLEX_PASCAL:
	//case SCLEX_INNOSETUP:
	//	return style == SCE_INNO_KEYWORD_PASCAL;
	//case SCLEX_NSIS:
	//	return style == SCE_C_WORD || style == SCE_C_PREPROCESSOR;
	//case SCLEX_AU3:
	//	return style == SCE_AU3_KEYWORD;
	case SCLEX_SQL:
		return style == SCE_SQL_WORD;
	}
	return FALSE;
}

const char *EditKeywordIndent(const char *head, int *indent) {
	char word[64] = "";
	char word_low[64] = "";
	int length = 0;
	const char *endPart = NULL;
	*indent = 0;
	while (*head && length < 63 && IsAAlpha(*head)) {
		word[length] = *head;
		word_low[length] = (*head) | 0x20;
		++length;
		++head;
	}
	switch (pLexCurrent->iLexer) {
	//case SCLEX_CPP:
	//case SCLEX_VB:
	//case SCLEX_VBSCRIPT:
	case SCLEX_RUBY:
		if (!strcmp(word, "if") || !strcmp(word, "do") || !strcmp(word, "while") || !strcmp(word, "for")) {
			*indent = 2;
			endPart = "end";
		}
		break;
	case SCLEX_MATLAB:
		if (!strcmp(word, "function")) {
			*indent = 1;
		} else if (!strcmp(word, "if") || !strcmp(word, "for") || !strcmp(word, "while") || !strcmp(word, "switch") || !strcmp(word, "try")) {
			*indent = 2;
			if (pLexCurrent->rid == NP2LEX_OCTAVE || np2LexLangIndex == IDM_LANG_OCTAVE) {
				if (strcmp(word, "if") == 0) {
					endPart = "endif";
				} else if (strcmp(word, "for") == 0) {
					endPart = "endfor";
				} else if (strcmp(word, "while") == 0) {
					endPart = "endwhile";
				} else if (strcmp(word, "switch") == 0) {
					endPart = "endswitch";
				} else if (strcmp(word, "try") == 0) {
					endPart = "end_try_catch";
				}
			}
			if (endPart == NULL) {
				endPart = "end";
			}
		}
		break;
	case SCLEX_LUA:
		if (!strcmp(word, "function") || !strcmp(word, "if") || !strcmp(word, "do")) {
			*indent = 2;
			endPart = "end";
		}
		break;
	case SCLEX_BASH:
		if (np2LexLangIndex == IDM_LANG_CSHELL) {
			if (!strcmp(word, "if")) {
				*indent = 2;
				endPart = "endif";
			} else if (!strcmp(word, "switch")) {
				*indent = 2;
				endPart = "endif";
			} else if (!strcmp(word, "foreach") || !strcmp(word, "while")) {
				*indent = 2;
				endPart = "end";
			}
		} else {
			if (!strcmp(word, "if")) {
				*indent = 2;
				endPart = "fi";
			} else if (!strcmp(word, "case")) {
				*indent = 2;
				endPart = "esac";
			} else if (!strcmp(word, "do")) {
				*indent = 2;
				endPart = "done";
			}
		}
		break;
	case SCLEX_MAKEFILE:
		if (!strcmp(word, "if")) {
			*indent = 2;
			endPart = "endif";
		} else if (!strcmp(word, "define")) {
			*indent = 2;
			endPart = "endef";
		} else if (!strcmp(word, "for")) {
			*indent = 2;
			endPart = "endfor";
		}
		break;
	case SCLEX_CMAKE:
		if (!strcmp(word, "function")) {
			*indent = 2;
			endPart = "endfunction()";
		} else if (!strcmp(word, "macro")) {
			*indent = 2;
			endPart = "endmacro()";
		} else if (!strcmp(word, "if")) {
			*indent = 2;
			endPart = "endif()";
		} else if (!strcmp(word, "foreach")) {
			*indent = 2;
			endPart = "endforeach()";
		} else if (!strcmp(word, "while")) {
			*indent = 2;
			endPart = "endwhile()";
		}
		break;
	//case SCLEX_VHDL:
	//case SCLEX_VERILOG:
	//case SCLEX_PASCAL:
	//case SCLEX_INNOSETUP:
	//case SCLEX_NSIS:
	//case SCLEX_AU3:
	case SCLEX_SQL:
		if (!strcmp(word_low, "if")) {
			*indent = 2;
			endPart = "END IF;";
		} else if (!strcmp(word_low, "while")) {
			*indent = 2;
			endPart = "END WHILE;";
		} else if (!strcmp(word_low, "repeat")) {
			*indent = 2;
			endPart = "END REPEAT;";
		} else if (!strcmp(word_low, "loop") || !strcmp(word_low, "for")) {
			*indent = 2;
			endPart = "END LOOP;";
		} else if (!strcmp(word_low, "case")) {
			*indent = 2;
			endPart = "END CASE;";
		} else if (!strcmp(word_low, "begin")) {
			*indent = 2;
			if (StrStrIA(head, "transaction") != NULL) {
				endPart = "COMMIT;";
			} else {
				endPart = "END";
			}
		} else if (!strcmp(word_low, "start")) {
			if (StrStrIA(head, "transaction") != NULL) {
				*indent = 2;
				endPart = "COMMIT;";
			}
		}
		break;
	}
	return endPart;
}

extern int	iEOLMode;
extern BOOL	bTabsAsSpaces;
extern BOOL	bTabIndents;
extern int	iTabWidth;
extern int	iIndentWidth;

void EditAutoIndent(HWND hwnd) {
	char *pLineBuf;

	int iCurPos = (int)SendMessage(hwnd, SCI_GETCURRENTPOS, 0, 0);
	//int iAnchorPos = (int)SendMessage(hwnd, SCI_GETANCHOR, 0, 0);
	int iCurLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)iCurPos, 0);
	//int iLineLength = (int)SendMessage(hwnd, SCI_LINELENGTH, iCurLine, 0);
	//int iIndentBefore = (int)SendMessage(hwnd, SCI_GETLINEINDENTATION, (WPARAM)iCurLine-1, 0);

#ifdef BOOKMARK_EDITION
	// Move bookmark along with line if inserting lines (pressing return at beginning of line) because Scintilla does not do this for us
	if (iCurLine > 0) {
		int iPrevLineLength = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, iCurLine - 1, 0) -
							  (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, iCurLine - 1, 0);
		if (iPrevLineLength == 0) {
			int bitmask = (int)SendMessage(hwnd, SCI_MARKERGET, iCurLine - 1, 0);
			if (bitmask & 1) {
				SendMessage(hwnd, SCI_MARKERDELETE, iCurLine - 1, 0);
				SendMessage(hwnd, SCI_MARKERADD, iCurLine, 0);
			}
		}
	}
#endif

	if (iCurLine > 0/* && iLineLength <= 2*/) {
		int iPrevLineLength = (int)SendMessage(hwnd, SCI_LINELENGTH, iCurLine - 1, 0);
		if (iPrevLineLength < 2) {
			return;
		}
		pLineBuf = NP2HeapAlloc(2 * iPrevLineLength + 1 + iIndentWidth * 2 + 2 + 64);
		if (pLineBuf) {
			char *pPos;
			int indent = 0;
			int	iIndentLen = 0;
			int iIndentPos = iCurPos;
			int ch;
			const char *endPart = NULL;
			int commentStyle = 0;
			SciCall_GetLine(iCurLine - 1, pLineBuf);
			pLineBuf[iPrevLineLength] = '\0';
			ch = pLineBuf[iPrevLineLength - 2];
			if (ch == '\r') {
				ch = pLineBuf[iPrevLineLength - 3];
				iIndentLen = 1;
			}
			if (ch == '{' || ch == '[' || ch == '(') {
				indent = 2;
			} else if (ch == ':') { // case label/Python
				indent = 1;
			} else if (ch == '*' || ch == '!') { // indent block comment
				iIndentLen = iPrevLineLength - (2 + iIndentLen);
				if (iIndentLen >= 2 && pLineBuf[iIndentLen - 2] == '/' && pLineBuf[iIndentLen - 1] == '*') {
					indent = 1;
					commentStyle = 1;
				}
			}
			iIndentLen = 0;
			ch = SciCall_GetCharAt(SciCall_PositionFromLine(iCurLine));
			if (indent == 2 && !(ch == '}' || ch == ']' || ch == ')')) {
				indent = 1;
			} else if (!indent && (ch == '}' || ch == ']' || ch == ')')) {
				indent = 1;
			}
			for (pPos = pLineBuf; *pPos; pPos++) {
				if (*pPos != ' ' && *pPos != '\t') {
					if (!indent && IsWordStart(*pPos)) { // indent on keywords
						int style = SciCall_GetStyleAt(SciCall_PositionFromLine(iCurLine - 1) + iIndentLen);
						if (IsIndentKeywordStyle(style)) {
							endPart = EditKeywordIndent(pPos, &indent);
						}
					}
					if (indent) {
						memset(pPos, 0, iPrevLineLength - iIndentLen);
					}
					*pPos = '\0';
					break;
				}
				iIndentLen += 1;
			}
			if (indent) {
				int pad = iIndentWidth;
				iIndentPos += iIndentLen;
				ch = ' ';
				if (bTabIndents) {
					if (bTabsAsSpaces) {
						pad = iTabWidth;
						ch = ' ';
					} else {
						pad = 1;
						ch = '\t';
					}
				}
				if (commentStyle) {
					iIndentPos += 2;
					*pPos++ = ' ';
					*pPos++ = '*';
				} else {
					iIndentPos += pad;
					while (pad-- > 0) {
						*pPos++ = (char)ch;
					}
				}
				if (iEOLMode == SC_EOL_CRLF || iEOLMode == SC_EOL_CR) {
					*pPos++ = '\r';
				}
				if (iEOLMode == SC_EOL_CRLF || iEOLMode == SC_EOL_LF) {
					*pPos++ = '\n';
				}
				if (indent == 2) {
					lstrcpynA(pPos, pLineBuf, iIndentLen + 1);
					pPos += iIndentLen;
					if (endPart) {
						iIndentLen = lstrlenA(endPart);
						lstrcpynA(pPos, endPart, iIndentLen + 1);
						pPos += iIndentLen;
					}
				}
				*pPos = '\0';
			}
			if (*pLineBuf) {
				//int iPrevLineStartPos;
				//int iPrevLineEndPos;
				//int iPrevLineIndentPos;

				SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
				SendMessage(hwnd, SCI_ADDTEXT, lstrlenA(pLineBuf), (LPARAM)pLineBuf);
				if (indent) {
					if (indent == 1) {// remove new line
						iCurPos = iIndentPos + ((iEOLMode == SC_EOL_CRLF) ? 2 : 1);
						SendMessage(hwnd, SCI_SETSEL, iIndentPos, iCurPos);
						SendMessage(hwnd, SCI_REPLACESEL, 0, (LPARAM)"");
					}
					SendMessage(hwndEdit, SCI_SETSEL, iIndentPos, iIndentPos);
				}
				SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);

				//iPrevLineStartPos	 = (int)SendMessage(hwnd, SCI_POSITIONFROMLINE, (WPARAM)iCurLine-1, 0);
				//iPrevLineEndPos	 = (int)SendMessage(hwnd, SCI_GETLINEENDPOSITION, (WPARAM)iCurLine-1, 0);
				//iPrevLineIndentPos = (int)SendMessage(hwnd, SCI_GETLINEINDENTPOSITION, (WPARAM)iCurLine-1, 0);

				//if (iPrevLineEndPos == iPrevLineIndentPos) {
				//	SendMessage(hwnd, SCI_BEGINUNDOACTION, 0, 0);
				//	SendMessage(hwnd, SCI_SETTARGETSTART, (WPARAM)iPrevLineStartPos, 0);
				//	SendMessage(hwnd, SCI_SETTARGETEND, (WPARAM)iPrevLineEndPos, 0);
				//	SendMessage(hwnd, SCI_REPLACETARGET, 0, (LPARAM)"");
				//	SendMessage(hwnd, SCI_ENDUNDOACTION, 0, 0);
				//}
			}
			NP2HeapFree(pLineBuf);
			//int iIndent = (int)SendMessage(hwnd, SCI_GETLINEINDENTATION, (WPARAM)iCurLine, 0);
			//SendMessage(hwnd, SCI_SETLINEINDENTATION, (WPARAM)iCurLine, (LPARAM)iIndentBefore);
			//iIndentLen = /*- iIndent +*/ SendMessage(hwnd, SCI_GETLINEINDENTATION, (WPARAM)iCurLine, 0);
			//if (iIndentLen > 0)
			//	SendMessage(hwnd, SCI_SETSEL, (WPARAM)iAnchorPos+iIndentLen, (LPARAM)iCurPos+iIndentLen);
		}
	}
}


void EditToggleCommentLine(HWND hwnd) {
	BeginWaitCursor();
	switch (pLexCurrent->iLexer) {
	case SCLEX_CPP:
		switch (pLexCurrent->rid) {
		case NP2LEX_AWK:
		case NP2LEX_JAM:
			EditToggleLineComments(hwnd, L"#", TRUE);
			break;
		default:
			EditToggleLineComments(hwnd, L"//", FALSE);
			break;
		}
		break;
	case SCLEX_CSS:
	case SCLEX_PASCAL:
	case SCLEX_VERILOG:
	case SCLEX_FSHARP:
	case SCLEX_GRAPHVIZ:
	case SCLEX_JSON:
		EditToggleLineComments(hwnd, L"//", FALSE);
		break;
	case SCLEX_VBSCRIPT:
	case SCLEX_VB:
		EditToggleLineComments(hwnd, L"'", TRUE);
		break;
	case SCLEX_PYTHON:
	case SCLEX_RUBY:
	case SCLEX_SMALI:
	case SCLEX_MAKEFILE:
		EditToggleLineComments(hwnd, L"#", FALSE);
		break;
	case SCLEX_BASH:
		EditToggleLineComments(hwnd, ((np2LexLangIndex == IDM_LANG_M4)? L"dnl " : L"#"), FALSE);
		break;
	case SCLEX_PERL:
	case SCLEX_CONF:
	case SCLEX_TCL:
	case SCLEX_POWERSHELL:
	case SCLEX_CMAKE:
		EditToggleLineComments(hwnd, L"#", TRUE);
		break;
	case SCLEX_ASM:
	case SCLEX_PROPERTIES:
	case SCLEX_AU3:
	case SCLEX_INNOSETUP:
		EditToggleLineComments(hwnd, L";", TRUE);
		break;
	case SCLEX_SQL:
		EditToggleLineComments(hwnd, L"-- ", TRUE); // extra space
		break;
	case SCLEX_LUA:
	case SCLEX_VHDL:
		EditToggleLineComments(hwnd, L"--", TRUE);
		break;
	case SCLEX_BATCH:
		EditToggleLineComments(hwnd, L"@rem ", TRUE);
		break;
	case SCLEX_LATEX:
		EditToggleLineComments(hwnd, L"# ", TRUE);
		break;
	case SCLEX_MATLAB:
		if (pLexCurrent->rid == NP2LEX_JULIA) {
			EditToggleLineComments(hwnd, L"#", FALSE);
		} else if (pLexCurrent->rid == NP2LEX_SCILAB || np2LexLangIndex == IDM_LANG_SCILAB) {
			EditToggleLineComments(hwnd, L"//", FALSE);
		} else {
			EditToggleLineComments(hwnd, L"%", FALSE);
		}
		break;
	case SCLEX_LISP:
	case SCLEX_LLVM:
		EditToggleLineComments(hwnd, L";", FALSE);
		break;
	case SCLEX_VIM:
		EditToggleLineComments(hwnd, L"\" ", FALSE);
		break;
	case SCLEX_TEXINFO:
		EditToggleLineComments(hwnd, L"@c ", FALSE);
		break;
	default:
		break;
	}
	EndWaitCursor();
}

void EditEncloseSelectionNewLine(HWND hwnd, LPCWSTR pwszOpen, LPCWSTR pwszClose) {
	WCHAR start[64] = L"";
	WCHAR end[64] = L"";
	Sci_Position pos;
	int line;
	LPCWSTR lineEnd = (iEOLMode == SC_EOL_LF) ? L"LF" : ((iEOLMode == SC_EOL_CR) ? L"CR" : L"CR+LF");

	pos = SciCall_GetSelectionStart();
	line = SciCall_LineFromPosition(pos);
	if (pos != SciCall_PositionFromLine(line)) {
		lstrcat(start, lineEnd);
	}
	lstrcat(start, pwszOpen);
	lstrcat(start, lineEnd);

	pos = SciCall_GetSelectionEnd();
	line = SciCall_LineFromPosition(pos);
	if (pos != SciCall_PositionFromLine(line)) {
		lstrcat(end, lineEnd);
	}
	lstrcat(end, pwszClose);
	lstrcat(end, lineEnd);
	EditEncloseSelection(hwnd, start, end);
}

void EditToggleCommentBlock(HWND hwnd) {
	switch (pLexCurrent->iLexer) {
	case SCLEX_CPP:
		switch (pLexCurrent->rid) {
		case NP2LEX_AWK:
		case NP2LEX_JAM:
			break;
		default:
			EditEncloseSelection(hwnd, L"/*", L"*/");
			break;
		}
		break;
	case SCLEX_XML:
		EditEncloseSelection(hwnd, L"<!--", L"-->");
		break;
	case SCLEX_HTML:
		EditEncloseSelection(hwnd, L"<!--", L"-->");
		break;
	case SCLEX_CSS:
	case SCLEX_ASM:
	case SCLEX_VERILOG:
	case SCLEX_GRAPHVIZ:
	case SCLEX_JSON:
		EditEncloseSelection(hwnd, L"/*", L"*/");
		break;
	case SCLEX_PASCAL:
	case SCLEX_INNOSETUP:
		EditEncloseSelection(hwnd, L"{", L"}");
		break;
	case SCLEX_LUA:
		EditEncloseSelection(hwnd, L"--[[", L"]]");
		break;
	case SCLEX_FSHARP:
		EditEncloseSelection(hwnd, L"(*", L"*)");
		break;
	case SCLEX_LATEX:
		EditEncloseSelectionNewLine(hwnd, L"\\begin{comment}", L"\\end{comment}");
		break;
	case SCLEX_MATLAB:
		if (pLexCurrent->rid == NP2LEX_JULIA) {
			EditEncloseSelectionNewLine(hwnd, L"#=", L"=#");
		} else if (pLexCurrent->rid == NP2LEX_SCILAB || np2LexLangIndex == IDM_LANG_SCILAB) {
			EditEncloseSelectionNewLine(hwnd, L"/*", L"*/");
		} else {
			EditEncloseSelectionNewLine(hwnd, L"%{", L"%}");
		}
		break;
	default:
		break;
	}
}

void EditShowCallTips(HWND hwnd, Sci_Position position) {
	char *text;
	char *pLine;
	int iLine = (int)SendMessage(hwnd, SCI_LINEFROMPOSITION, (WPARAM)position, 0);
	int iDocLen = SciCall_GetLine(iLine, NULL); // get length
	pLine = NP2HeapAlloc(iDocLen + 1);
	SciCall_GetLine(iLine, pLine);
	text = NP2HeapAlloc(iDocLen + 1 + 128);
	wsprintfA(text, "ShowCallTips(%d, %d, %d)\n%s", iLine + 1, (int)position, iDocLen, pLine);
	SendMessage(hwnd, SCI_CALLTIPSHOW, position, (LPARAM)text);
	NP2HeapFree(pLine);
	NP2HeapFree(text);
}

// End of EditAutoC.c
