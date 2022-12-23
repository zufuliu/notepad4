// Edit AutoCompletion

#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <limits.h>
#include <inttypes.h>
#include <stdio.h>
#include "SciCall.h"
#include "VectorISA.h"
#include "Helpers.h"
#include "Edit.h"
#include "Styles.h"
#include "resource.h"
#include "EditAutoC_Data0.h"
#include "LaTeXInput.h"

#define NP2_AUTOC_USE_STRING_ORDER	1
#define NP2_AUTOC_USE_WORD_POINTER	0	// used for debug
// scintilla/src/AutoComplete.h AutoComplete::maxItemLen
#define NP2_AUTOC_MAX_WORD_LENGTH	(1024 - 3 - 1 - 16)	// SP + '(' + ')' + '\0'
#define NP2_AUTOC_WORD_BUFFER_SIZE	1024
#define NP2_AUTOC_INIT_BUFFER_SIZE	(4096)

// memory buffer
struct WordListBuffer;
struct WordListBuffer {
	struct WordListBuffer *next;
};

struct WordNode;
struct WordList {
	int (__cdecl *WL_strcmp)(LPCSTR, LPCSTR);
	int (__cdecl *WL_strncmp)(LPCSTR, LPCSTR, size_t);
#if NP2_AUTOC_USE_STRING_ORDER
	uint32_t (*WL_OrderFunc)(const void *, uint32_t);
#endif

	struct WordNode *pListHead;
	LPCSTR pWordStart;
	UINT iStartLen;
#if NP2_AUTOC_USE_STRING_ORDER
	UINT orderStart;
#endif
	UINT nWordCount;
	UINT nTotalLen;

	UINT offset;
	UINT capacity;
	struct WordListBuffer *buffer;
};

// TODO: replace _stricmp() and _strnicmp() with other functions
// which correctly case insensitively compares UTF-8 string and ANSI string.

#if NP2_AUTOC_USE_STRING_ORDER
#define NP2_AUTOC_ORDER_LENGTH	4

uint32_t WordList_Order(const void *pWord, uint32_t len) {
#if 0
	uint32_t high = 0;
	const uint8_t *ptr = (const uint8_t *)pWord;
	len = min_u(len, NP2_AUTOC_ORDER_LENGTH);
	for (uint32_t i = 0; i < len; i++) {
		high = (high << 8) | *ptr++;
	}
	if (len < NP2_AUTOC_ORDER_LENGTH) {
		NP2_assume(len != 0); // suppress [clang-analyzer-core.uninitialized.Assign]
		high <<= (NP2_AUTOC_ORDER_LENGTH - len)*8;
	}

#else
	uint32_t high = loadle_u32(pWord);
	if (len < NP2_AUTOC_ORDER_LENGTH) {
		high = bit_zero_high_u32(high, len*8);
	}
	high = bswap32(high);
#endif
	return high;
}

uint32_t WordList_OrderCase(const void *pWord, uint32_t len) {
#if 1
	uint32_t high = 0;
	const uint8_t *ptr = (const uint8_t *)pWord;
	len = min_u(len, NP2_AUTOC_ORDER_LENGTH);
	for (uint32_t i = 0; i < len; i++) {
		uint8_t ch = *ptr++;
		// convert to lower case to match _stricmp() / strcasecmp().
		if (ch >= 'A' && ch <= 'Z') {
			ch = ch + 'a' - 'A';
		}
		high = (high << 8) | ch;
	}
	if (len < NP2_AUTOC_ORDER_LENGTH) {
		NP2_assume(len != 0); // suppress [clang-analyzer-core.uninitialized.Assign]
		high <<= (NP2_AUTOC_ORDER_LENGTH - len)*8;
	}

#else
	uint32_t high = loadle_u32(pWord);
	high |= 0x20202020U; // only works for ASCII letters
	if (len < NP2_AUTOC_ORDER_LENGTH) {
		high = bit_zero_high_u32(high, len*8);
	}
	high = bswap32(high);
#endif
	return high;
}
#endif

// Tree
struct WordNode {
	union {
		struct WordNode *link[2];
		struct {
			struct WordNode *left;
			struct WordNode *right;
		};
	};
#if NP2_AUTOC_USE_WORD_POINTER
	char *word;
#endif
#if NP2_AUTOC_USE_STRING_ORDER
	UINT order;
#endif
	UINT len;
	UINT level;
};

#define NP2_TREE_HEIGHT_LIMIT	32
// store word right after the node as most word are short.
#define WordNode_GetWord(node)		((char *)(node) + sizeof(struct WordNode))
// TODO: since the tree is sorted, nodes greater than some level can be deleted to reduce total words.
// or only limit word count in WordList_GetList().

// Andersson Tree, source from https://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_andersson.aspx
// see also https://en.wikipedia.org/wiki/AA_tree
#define aa_tree_skew(t) \
	if ((t)->level && (t)->left && (t)->level == (t)->left->level) {\
		struct WordNode *save = (t)->left;					\
		(t)->left = save->right;							\
		save->right = (t);									\
		(t) = save;											\
	}
#define aa_tree_split(t) \
	if ((t)->level && (t)->right && (t)->right->right && (t)->level == (t)->right->right->level) {\
		struct WordNode *save = (t)->right;					\
		(t)->right = save->left;							\
		save->left = (t);									\
		(t) = save;											\
		++(t)->level;										\
	}

#define WordList_AddNode(pWList)	((struct WordNode *)((char *)((pWList)->buffer) + (pWList)->offset))
static inline void WordList_AddBuffer(struct WordList *pWList) {
	struct WordListBuffer *buffer = (struct WordListBuffer *)NP2HeapAlloc(pWList->capacity);
	buffer->next = pWList->buffer;
	pWList->offset = NP2_align_up(sizeof(struct WordListBuffer), NP2_alignof(struct WordNode));
	pWList->buffer = buffer;
}

void WordList_AddWord(struct WordList *pWList, LPCSTR pWord, UINT len) {
	struct WordNode *root = pWList->pListHead;
#if NP2_AUTOC_USE_STRING_ORDER
	const UINT order = (pWList->iStartLen > NP2_AUTOC_ORDER_LENGTH) ? 0 : pWList->WL_OrderFunc(pWord, len);
#endif
	if (root == NULL) {
		struct WordNode *node = WordList_AddNode(pWList);
		char *word = WordNode_GetWord(node);
		memcpy(word, pWord, len);
#if NP2_AUTOC_USE_WORD_POINTER
		node->word = word;
#endif
#if NP2_AUTOC_USE_STRING_ORDER
		node->order = order;
#endif
		node->len = len;
		node->level = 1;
		root = node;
	} else {
		struct WordNode *iter = root;
		struct WordNode *path[NP2_TREE_HEIGHT_LIMIT] = { NULL };
		int top = 0;
		int dir;

		// find a spot and save the path
		for (;;) {
			path[top++] = iter;
#if NP2_AUTOC_USE_STRING_ORDER
			dir = (int)(iter->order - order);
			if (dir == 0 && (len > NP2_AUTOC_ORDER_LENGTH || iter->len > NP2_AUTOC_ORDER_LENGTH)) {
				dir = pWList->WL_strcmp(WordNode_GetWord(iter), pWord);
			}
#else
			dir = pWList->WL_strcmp(WordNode_GetWord(iter), pWord);
#endif
			if (dir == 0) {
				return;
			}
			dir = dir < 0;
			if (iter->link[dir] == NULL) {
				break;
			}
			iter = iter->link[dir];
		}

		if (pWList->capacity < pWList->offset + len + 1 + sizeof(struct WordNode)) {
			pWList->capacity <<= 1;
			WordList_AddBuffer(pWList);
		}

		struct WordNode *node = WordList_AddNode(pWList);
		char *word = WordNode_GetWord(node);
		memcpy(word, pWord, len);
#if NP2_AUTOC_USE_WORD_POINTER
		node->word = word;
#endif
#if NP2_AUTOC_USE_STRING_ORDER
		node->order = order;
#endif
		node->len = len;
		node->level = 1;
		iter->link[dir] = node;

		// walk back and rebalance
		while (--top >= 0) {
			// which child?
			if (top != 0) {
				dir = path[top - 1]->right == path[top];
			}
			aa_tree_skew(path[top]);
			aa_tree_split(path[top]);
			// fix the parent
			if (top != 0) {
				path[top - 1]->link[dir] = path[top];
			} else {
				root = path[top];
			}
		}
	}

	pWList->pListHead = root;
	pWList->nWordCount++;
	pWList->nTotalLen += len + 1;
	pWList->offset += NP2_align_up(len + 1 + sizeof(struct WordNode), NP2_alignof(struct WordNode));
}

void WordList_Free(struct WordList *pWList) {
	struct WordListBuffer *buffer = pWList->buffer;
	while (buffer) {
		struct WordListBuffer * const next = buffer->next;
		NP2HeapFree(buffer);
		buffer = next;
	}
}

char* WordList_GetList(struct WordList *pWList) {
	struct WordNode *root = pWList->pListHead;
	struct WordNode *path[NP2_TREE_HEIGHT_LIMIT] = { NULL };
	int top = 0;
	char *buf = (char *)NP2HeapAlloc(pWList->nTotalLen + 1);// additional separator
	char * const pList = buf;

	while (root || top > 0) {
		if (root) {
			path[top++] = root;
			root = root->left;
		} else {
			root = path[--top];
			memcpy(buf, WordNode_GetWord(root), root->len);
			buf += root->len;
			*buf++ = '\n'; // the separator char
			root = root->right;
		}
	}
	// trim last separator char
	if (buf != pList) {
		*(--buf) = '\0';
	}
	return pList;
}

void WordList_Init(struct WordList *pWList, LPCSTR pRoot, UINT iRootLen, bool bIgnoreCase) {
	memset(pWList, 0, sizeof(struct WordList));
	pWList->pWordStart = pRoot;
	pWList->iStartLen = iRootLen;

	if (bIgnoreCase) {
		pWList->WL_strcmp = _stricmp;
		pWList->WL_strncmp = _strnicmp;
#if NP2_AUTOC_USE_STRING_ORDER
		pWList->WL_OrderFunc = WordList_OrderCase;
#endif
	} else {
		pWList->WL_strcmp = strcmp;
		pWList->WL_strncmp = strncmp;
#if NP2_AUTOC_USE_STRING_ORDER
		pWList->WL_OrderFunc = WordList_Order;
#endif
	}
#if NP2_AUTOC_USE_STRING_ORDER
	pWList->orderStart = pWList->WL_OrderFunc(pRoot, iRootLen);
#endif

	pWList->capacity = NP2_AUTOC_INIT_BUFFER_SIZE;
	WordList_AddBuffer(pWList);
}

static inline void WordList_UpdateRoot(struct WordList *pWList, LPCSTR pRoot, UINT iRootLen) {
	pWList->pWordStart = pRoot;
	pWList->iStartLen = iRootLen;
#if NP2_AUTOC_USE_STRING_ORDER
	pWList->orderStart = pWList->WL_OrderFunc(pRoot, iRootLen);
#endif
}

static inline bool WordList_StartsWith(const struct WordList *pWList, LPCSTR pWord) {
#if NP2_AUTOC_USE_STRING_ORDER
	if (pWList->iStartLen > NP2_AUTOC_ORDER_LENGTH) {
		return pWList->WL_strncmp(pWList->pWordStart, pWord, pWList->iStartLen) == 0;
	}
	if (pWList->orderStart != pWList->WL_OrderFunc(pWord, pWList->iStartLen)) {
		return false;
	}
	return true;
#else
	return pWList->WL_strncmp(pWList->pWordStart, pWord, pWList->iStartLen) == 0;
#endif
}

static inline bool WordList_IsSeparator(uint8_t ch) {
#if defined(_WIN64)
	// directly complied into bit test
#if defined(__clang__) || defined(__GNUC__)
	if (ch > ';') {
		return ch == '^';
	}
	const uint64_t mask = UINT32_MAX
		| (UINT64_C(1) << ' ')
		| (UINT64_C(1) << '(')
		| (UINT64_C(1) << ')')
		| (UINT64_C(1) << ',')
		| (UINT64_C(1) << '.')
		//| (UINT64_C(1) << ':')
		| (UINT64_C(1) << ';');
	return (mask >> ch) & true;
#else
	return ch <= ' '
		|| ch == '('
		|| ch == ')'
		|| ch == ','
		|| ch == '.'
		//|| ch == ':'
		|| ch == ';'
		|| ch == '^';
#endif
#else
	if (ch <= ' ') {
		return true;
	}
	ch -= '(';
	if (ch > ';' - '(') {
		return ch == '^' - '(';
	}
	const uint32_t mask = (1 << ('(' - '('))
		| (1 << (')' - '('))
		| (1 << (',' - '('))
		| (1 << ('.' - '('))
		//| (1 << (':' - '('))
		| (1 << (';' - '('));
	return (mask >> ch) & true;
#endif
}

void WordList_AddListEx(struct WordList *pWList, LPCSTR pList) {
	//StopWatch watch;
	//StopWatch_Start(watch);
	char word[NP2_AUTOC_WORD_BUFFER_SIZE];
	const UINT iStartLen = pWList->iStartLen;
	UINT len = 0;
	bool ok = false;
	while (true) {
		uint8_t ch;
		LPCSTR sub = pList;
		do {
			ch = *sub++;
		} while (!WordList_IsSeparator(ch));

		UINT lenSub = (UINT)(sub - pList - 1);
		lenSub = min_u(NP2_AUTOC_MAX_WORD_LENGTH - len, lenSub);
		memcpy(word + len, pList, lenSub);
		len += lenSub;
		pList = sub;
		if (len >= iStartLen) {
			if (ch == '(') {
				word[len++] = '(';
				word[len++] = ')';
			}
			word[len] = '\0';
			if (ok || WordList_StartsWith(pWList, word)) {
				WordList_AddWord(pWList, word, len);
				ok = ch == '.';
			}
		}
		if (ch == '\0') {
			break;
		}
		if (ch == '^') {
			word[len++] = ' ';
		} else if (!ok && ch != '.') {
			len = 0;
		} else {
			word[len++] = '.';
		}
	}

	//StopWatch_Stop(watch);
	//const double duration = StopWatch_Get(&watch);
	//printf("%s duration=%.6f\n", __func__, duration);
}

static inline void WordList_AddList(struct WordList *pWList, LPCSTR pList) {
	if (StrNotEmptyA(pList)) {
		WordList_AddListEx(pWList, pList);
	}
}

void WordList_AddSubWord(struct WordList *pWList, LPSTR pWord, UINT wordLength, UINT iRootLen) {
	/*
	when pRoot is 'b', split 'bugprone-branch-clone' as following:
	1. first hyphen: 'bugprone-branch-clone' => 'bugprone', 'branch-clone'.
	2. second hyphen: 'bugprone-branch-clone' => 'bugprone-branch'; 'branch-clone' => 'branch'.
	*/

	LPCSTR words[8];
	UINT starts[8];
	UINT count = 0;

	for (UINT i = 0; i < wordLength - 1; i++) {
		const char ch = pWord[i];
		if (ch == '.' || ch == '-' || ch == ':') {
			if (i >= iRootLen) {
				pWord[i] = '\0';
				WordList_AddWord(pWList, pWord, i);
				for (UINT j = 0; j < count; j++) {
					const UINT subLen = i - starts[j];
					if (subLen >= iRootLen) {
						WordList_AddWord(pWList, words[j], subLen);
					}
				}
				pWord[i] = ch;
			}
			if (ch != '.' && (pWord[i + 1] == '>' || pWord[i + 1] == ':')) {
				++i;
			}

			const UINT subLen = wordLength - (i + 1);
			LPCSTR pSubRoot = pWord + i + 1;
			if (subLen >= iRootLen && WordList_StartsWith(pWList, pSubRoot)) {
				WordList_AddWord(pWList, pSubRoot, subLen);
				if (count < COUNTOF(words)) {
					words[count] = pSubRoot;
					starts[count] = i + 1;
					++count;
				}
			}
		}
	}
}


static inline bool IsCppCommentStyle(int style) {
	return style == SCE_C_COMMENT
		|| style == SCE_C_COMMENTLINE
		|| style == SCE_C_COMMENTDOC
		|| style == SCE_C_COMMENTLINEDOC
		|| style == SCE_C_COMMENTDOC_TAG
		|| style == SCE_C_COMMENTDOC_TAG_XML;
}

static inline bool IsSpecialStart(int ch) {
	return ch == ':' || ch == '.' || ch == '#' || ch == '@'
		|| ch == '<' || ch == '\\' || ch == '/' || ch == '-'
		|| ch == '>' || ch == '$' || ch == '%';
}

static inline bool IsSpecialStartChar(int ch, int chPrev) {
	return (ch == '.')	// member
		|| (ch == '#')	// preprocessor
		|| (ch == '@') // Java/PHP/Doxygen Doc Tag
		// ObjC Keyword, Java Annotation, Python Decorator, Cobra Directive
		|| (ch == '<') // HTML/XML Tag, C# Doc Tag
		|| (ch == '\\')// Doxygen Doc Tag, LaTeX Command
		|| (ch == ':') // CSS pseudo class
		|| (ch == '$') // variable
		|| (ch == '`') // VHDL, Verilog directive
		|| (ch == '\'')// VHDL attribute
		|| (chPrev == '\\' && (ch == '^' || ch == ':'))// LaTeX input, Emoji input
		// TODO: show emoji list after typing ':'.
		|| (chPrev == '<' && ch == '/')	// HTML/XML Close Tag
		|| (chPrev == '-' && ch == '>')	// member(C/C++/PHP)
		|| (chPrev == ':' && ch == ':');// namespace(C++), static member(C++/Java8/PHP)
}

//=============================================================================
//
// EditCompleteWord()
// Auto-complete words
//
extern EditAutoCompletionConfig autoCompletionConfig;

// CharClassify::SetDefaultCharClasses()
// tools/GenerateTable.py
static const uint32_t DefaultWordCharSet[8] = {
0x00000000U, 0x03ff0000U, 0x87fffffeU, 0x07fffffeU,
0xffffffffU, 0xffffffffU, 0xffffffffU, 0xffffffffU
};
// word character set for pLexCurrent
static uint32_t CurrentWordCharSet[8];
static uint32_t CharacterPrefixMask[8];
static uint32_t RawStringStyleMask[8];
static uint32_t GenericTypeStyleMask[8];
static uint32_t IgnoreWordStyleMask[8];

// from scintilla/lexlib/DocUtils.h
#define js_style(style)		((style) + SCE_PHP_LABEL + 1)
#define css_style(style)	((style) + SCE_PHP_LABEL + SCE_JS_LABEL + 2)

static inline bool IsDefaultWordChar(uint32_t ch) {
	return BitTestEx(DefaultWordCharSet, ch);
}

bool IsDocWordChar(uint32_t ch) {
	return BitTestEx(CurrentWordCharSet, ch);
}

static inline bool IsCharacterPrefix(int ch) {
	return BitTestEx(CharacterPrefixMask, ch);
}

static inline bool IsRawStringStyle(int style) {
	return BitTestEx(RawStringStyleMask, style);
}

static inline bool IsGenericTypeStyle(int style) {
	return BitTestEx(GenericTypeStyleMask, style);
}

static inline bool IsWordStyleToIgnore(int style) {
	return BitTestEx(IgnoreWordStyleMask, style);
}

bool IsAutoCompletionWordCharacter(uint32_t ch) {
	if (ch < 0x80) {
		return IsDocWordChar(ch);
	}
	const CharacterClass cc = SciCall_GetCharacterClass(ch);
	return cc == CharacterClass_Word;
}

static inline bool IsEscapeCharacter(int ch) {
	return ch == '0'	// '\0'
		|| ch == 'a'	// '\a'
		|| ch == 'b'	// '\b'
		|| ch == 'e'	// '\e', GNU extension
		|| ch == 'f'	// '\f'
		|| ch == 'n'	// '\n'
		|| ch == 'r'	// '\r'
		|| ch == 't'	// '\t'
		|| ch == 'v'	// '\v'
		// other
		|| ch == '$';	// PHP variable
	// x u U ignored as they need to be followed with multiple hex digits.
}

// https://en.wikipedia.org/wiki/Printf_format_string
static inline bool IsPrintfFormatSpecifier(int ch) {
	return IsAlpha(ch);
}

static bool IsEscapeCharOrFormatSpecifier(Sci_Position before, int ch, int chPrev, int style, bool punctuation) {
	// style for chPrev, style for ch is zero on typing
	const int stylePrev = SciCall_GetStyleIndexAt(before);
	if (stylePrev == 0) {
		return false;
	}
	if (chPrev == '%') {
		if (!IsPrintfFormatSpecifier(ch)) {
			return false;
		}
		if (style != 0 && pLexCurrent->formatSpecifierStyle) {
			return stylePrev == pLexCurrent->formatSpecifierStyle;
		}
		// legacy lexer without format specifier highlighting
		if (pLexCurrent->lexerAttr & LexerAttr_PrintfFormatSpecifier) {
			return !(stylePrev == pLexCurrent->operatorStyle || stylePrev == pLexCurrent->operatorStyle2);
		}
		return false;
	}

	if (style != 0 && pLexCurrent->escapeCharacterStyle) {
		if (stylePrev != pLexCurrent->escapeCharacterStyle) {
			if (pLexCurrent->iLexer != SCLEX_PHPSCRIPT
				|| !(stylePrev == js_style(SCE_JS_ESCAPECHAR) || stylePrev == css_style(SCE_CSS_ESCAPECHAR))) {
				return false;
			}
		}
	} else if (!punctuation) {
		// legacy lexer without escape character highlighting
		if (!IsEscapeCharacter(ch)) {
			return false;
		}
	}

	if (!IsRawStringStyle(stylePrev)) {
		int chPrev2 = 0;
		const Sci_Position before2 = SciCall_PositionBefore(before);
		if (before2 + 1 == before) {
			chPrev2 = SciCall_GetCharAt(before2);
		}
		// simply treat chPrev == chPrev2 as escape escapeCharacterStart self
		return chPrev != chPrev2;
	}

	return false;
}

static inline bool NeedSpaceAfterKeyword(const char *word, Sci_Position length) {
	const char *p = strstr(
		" if for try using while elseif switch foreach synchronized "
		, word);
	return p != NULL && p[-1] == ' ' && p[length] == ' ';
}

enum {
	// see LexInno.cxx
	InnoLineStatePreprocessor = 1 << 5,
	InnoLineStateCodeSection = 1 << 6,
};

//KeywordIndex++Autogenerated -- start of section automatically generated
enum {
	APDLKeywordIndex_SlashCommand = 2,
	APDLKeywordIndex_StarCommand = 3,
	AutoHotkeyKeywordIndex_Directive = 1,
	AutoHotkeyKeywordIndex_CompilerDirective = 2,
	AutoIt3KeywordIndex_Macro = 2,
	AutoIt3KeywordIndex_Directive = 4,
	AutoIt3KeywordIndex_Special = 5,
	CPPKeywordIndex_Preprocessor = 2,
	CPPKeywordIndex_Directive = 3,
	CSSKeywordIndex_AtRule = 1,
	CSSKeywordIndex_PseudoClass = 2,
	CSSKeywordIndex_PseudoElement = 3,
	CSharpKeywordIndex_Preprocessor = 3,
	CSharpKeywordIndex_CommentTag = 10,
	DKeywordIndex_Preprocessor = 2,
	DKeywordIndex_Attribute = 3,
	DartKeywordIndex_Metadata = 4,
	GraphVizKeywordIndex_HtmlLabel = 1,
	GroovyKeywordIndex_Annotation = 7,
	GroovyKeywordIndex_Groovydoc = 9,
	HTMLKeywordIndex_Tag = 0,
	HTMLKeywordIndex_Attribute = 6,
	HTMLKeywordIndex_EventHandler = 7,
	HTMLKeywordIndex_Value = 8,
	HaxeKeywordIndex_Preprocessor = 1,
	HaxeKeywordIndex_CommentTag = 8,
	InnoKeywordIndex_Directive = 4,
	JavaKeywordIndex_Annotation = 7,
	JavaKeywordIndex_Javadoc = 9,
	JavaScriptKeywordIndex_Decorator = 7,
	JavaScriptKeywordIndex_Jsdoc = 10,
	JuliaKeywordIndex_CodeFolding = 1,
	JuliaKeywordIndex_Macro = 6,
	KotlinKeywordIndex_Annotation = 4,
	KotlinKeywordIndex_Kdoc = 6,
	PHPKeywordIndex_Phpdoc = 11,
	PowerShellKeywordIndex_PredefinedVariable = 4,
	PythonKeywordIndex_Decorator = 7,
	RebolKeywordIndex_Directive = 1,
	ScalaKeywordIndex_Annotation = 3,
	ScalaKeywordIndex_Scaladoc = 5,
	SmaliKeywordIndex_Directive = 9,
	SwiftKeywordIndex_Directive = 1,
	SwiftKeywordIndex_Attribute = 2,
	VBKeywordIndex_Preprocessor = 3,
	VHDLKeywordIndex_Directive = 3,
	VHDLKeywordIndex_Attribute = 4,
	VerilogKeywordIndex_Directive = 3,
	VerilogKeywordIndex_SystemTaskAndFunction = 4,
};
//KeywordIndex--Autogenerated -- end of section automatically generated

extern EDITLEXER lexCPP;
extern EDITLEXER lexCSS;
extern EDITLEXER lexHTML;
extern EDITLEXER lexJavaScript;
extern EDITLEXER lexPHP;
extern EDITLEXER lexPython;
extern EDITLEXER lexVBScript;
extern HANDLE idleTaskTimer;

typedef enum HtmlTextBlock {
	HtmlTextBlock_Tag,
	HtmlTextBlock_CDATA,
	HtmlTextBlock_SGML,
	HtmlTextBlock_JavaScript,
	HtmlTextBlock_VBScript,
	HtmlTextBlock_Python,
	HtmlTextBlock_PHP,
	HtmlTextBlock_CSS,
} HtmlTextBlock;

static HtmlTextBlock GetCurrentHtmlTextBlockEx(int iLexer, int iCurrentStyle) {
	if (iLexer == SCLEX_PHPSCRIPT) {
		if (iCurrentStyle >= css_style(SCE_CSS_DEFAULT)) {
			return HtmlTextBlock_CSS;
		}
		if (iCurrentStyle >= js_style(SCE_JS_DEFAULT)) {
			return HtmlTextBlock_JavaScript;
		}
		if (iCurrentStyle >= SCE_PHP_DEFAULT) {
			return HtmlTextBlock_PHP;
		}
	}
	if (iCurrentStyle == SCE_H_CDATA) {
		return HtmlTextBlock_CDATA;
	}
	if ((iCurrentStyle >= SCE_HJ_START && iCurrentStyle <= SCE_HJ_TEMPLATELITERAL)
		|| (iCurrentStyle >= SCE_HJA_START && iCurrentStyle <= SCE_HJA_TEMPLATELITERAL)) {
		return HtmlTextBlock_JavaScript;
	}
	if ((iCurrentStyle >= SCE_HB_START && iCurrentStyle <= SCE_HB_OPERATOR)
		|| (iCurrentStyle >= SCE_HBA_START && iCurrentStyle <= SCE_HBA_OPERATOR)) {
		return HtmlTextBlock_VBScript;
	}
	if ((iCurrentStyle >= SCE_H_SGML_DEFAULT && iCurrentStyle <= SCE_H_SGML_BLOCK_DEFAULT)) {
		return HtmlTextBlock_SGML;
	}
	return HtmlTextBlock_Tag;
}

static HtmlTextBlock GetCurrentHtmlTextBlock(int iLexer) {
	const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
	const int iCurrentStyle = SciCall_GetStyleIndexAt(iCurrentPos);
	return GetCurrentHtmlTextBlockEx(iLexer, iCurrentStyle);
}

void EscapeRegex(LPSTR pszOut, LPCSTR pszIn) {
	char ch;
	while ((ch = *pszIn++) != '\0') {
		if (ch == '.'		// any character
			|| ch == '^'	// start of line
			|| ch == '$'	// end of line
			|| ch == '?'	// 0 or 1 times
			|| ch == '*'	// 0 or more times
			|| ch == '+'	// 1 or more times
			|| ch == '[' || ch == ']'
			|| ch == '(' || ch == ')'
			) {
			*pszOut++ = '\\';
		}
		*pszOut++ = ch;
	}
	*pszOut++ = '\0';
}

static void AutoC_AddDocWord(struct WordList *pWList, bool bIgnoreCase, char prefix) {
	LPCSTR const pRoot = pWList->pWordStart;
	const int iRootLen = pWList->iStartLen;

	// optimization for small string
	char onStack[128];
	char *pFind;
	if (iRootLen * 2 + 32 < (int)sizeof(onStack)) {
		memset(onStack, 0, sizeof(onStack));
		pFind = onStack;
	} else {
		pFind = (char *)NP2HeapAlloc(iRootLen * 2 + 32);
	}

	if (prefix) {
		char buf[2] = { prefix, '\0' };
		EscapeRegex(pFind, buf);
	}
	if (iRootLen == 0) {
		// find an identifier
		strcat(pFind, "[A-Za-z0-9_]");
		strcat(pFind, "\\i?");
	} else {
		if (IsDefaultWordChar((uint8_t)pRoot[0])) {
			strcat(pFind, "\\h");
		}
		EscapeRegex(pFind + strlen(pFind), pRoot);
		if (IsDefaultWordChar((uint8_t)pRoot[iRootLen - 1])) {
			strcat(pFind, "\\i?");
		} else {
			strcat(pFind, "\\i");
		}
	}

	const Sci_Position iCurrentPos = SciCall_GetCurrentPos() - iRootLen - (prefix ? 1 : 0);
	const Sci_Position iDocLen = SciCall_GetLength();
	const int findFlag = SCFIND_REGEXP | SCFIND_POSIX | (bIgnoreCase ? 0 : SCFIND_MATCHCASE);
	struct Sci_TextToFindFull ft = { { 0, iDocLen }, pFind, { 0, 0 } };

	Sci_Position iPosFind = SciCall_FindTextFull(findFlag, &ft);
	HANDLE timer = idleTaskTimer;
	WaitableTimer_Set(timer, autoCompletionConfig.dwScanWordsTimeout);

	while (iPosFind >= 0 && iPosFind < iDocLen && WaitableTimer_Continue(timer)) {
		Sci_Position wordEnd = iPosFind + iRootLen;
		const int style = SciCall_GetStyleIndexAt(wordEnd - 1);
		wordEnd = ft.chrgText.cpMax;
		if (iPosFind != iCurrentPos && !IsWordStyleToIgnore(style)) {
			// find all word after '::', '->', '.' and '-'
			bool bSubWord = false;
			while (wordEnd < iDocLen) {
				const int ch = SciCall_GetCharAt(wordEnd);
				if (!(ch == ':' || ch == '.' || ch == '-')) {
					if (ch == '!' && pLexCurrent->iLexer == SCLEX_RUST && style == SCE_RUST_MACRO) {
						// macro: println!()
						++wordEnd;
					}
					break;
				}

				const Sci_Position before = wordEnd;
				Sci_Position width = 0;
				int chNext = SciCall_GetCharacterAndWidth(wordEnd + 1, &width);
				if ((ch == '-' && chNext == '>') || (ch == ':' && chNext == ':')) {
					chNext = SciCall_GetCharacterAndWidth(wordEnd + 2, &width);
					if (IsAutoCompletionWordCharacter(chNext)) {
						wordEnd += 2;
					}
				} else if (ch == '.' || (ch == '-' && style == SciCall_GetStyleIndexAt(wordEnd))) {
					if (IsAutoCompletionWordCharacter(chNext)) {
						++wordEnd;
					}
				}
				if (wordEnd == before) {
					break;
				}

				while (wordEnd < iDocLen && (chNext < 0x80 && !IsDefaultWordChar(chNext))) {
					wordEnd += width;
					chNext = SciCall_GetCharacterAndWidth(wordEnd, &width);
					if (!IsAutoCompletionWordCharacter(chNext)) {
						break;
					}
				}

				wordEnd = SciCall_WordEndPosition(wordEnd, true);
				if (wordEnd - iPosFind > NP2_AUTOC_MAX_WORD_LENGTH) {
					wordEnd = before;
					break;
				}
				bSubWord = true;
			}

			if (wordEnd - iPosFind >= iRootLen) {
				char wordBuf[NP2_AUTOC_WORD_BUFFER_SIZE];
				char *pWord = wordBuf + NP2DefaultPointerAlignment;
				bool bChanged = false;
				struct Sci_TextRangeFull tr = { { iPosFind, min_pos(iPosFind + NP2_AUTOC_MAX_WORD_LENGTH, wordEnd) }, pWord };
				int wordLength = (int)SciCall_GetTextRangeFull(&tr);

				const Sci_Position before = SciCall_PositionBefore(iPosFind);
				if (before + 1 == iPosFind) {
					const int chPrev = SciCall_GetCharAt(before);
					// word after escape character or format specifier
					if (chPrev == '%' || chPrev == pLexCurrent->escapeCharacterStart) {
						if (IsEscapeCharOrFormatSpecifier(before, (uint8_t)pWord[0], chPrev, style, false)) {
							pWord++;
							--wordLength;
							bChanged = true;
						}
					}
				}
				if (prefix && prefix == pWord[0]) {
					pWord++;
					--wordLength;
					bChanged = true;
				}

				//if (pLexCurrent->iLexer == SCLEX_PHPSCRIPT && wordLength >= 2 && pWord[0] == '$' && pWord[1] == '$') {
				//	pWord++;
				//	--wordLength;
				//	bChanged = true;
				//}
				while (wordLength > 0 && (pWord[wordLength - 1] == '-' || pWord[wordLength - 1] == ':' || pWord[wordLength - 1] == '.')) {
					--wordLength;
					pWord[wordLength] = '\0';
				}
				if (bChanged) {
					memcpy(wordBuf, pWord, wordLength + 1);
					pWord = wordBuf;
				}

				bChanged = wordLength >= iRootLen && WordList_StartsWith(pWList, pWord);
				if (bChanged && !(pWord[0] == ':' && pWord[1] != ':')) {
					bool space = false;
					if (!(pLexCurrent->iLexer == SCLEX_CPP && style == SCE_C_MACRO)) {
						while (IsASpaceOrTab(SciCall_GetCharAt(wordEnd))) {
							space = true;
							wordEnd++;
						}
					}

					const int chWordEnd = SciCall_GetCharAt(wordEnd);
					if ((pLexCurrent->iLexer == SCLEX_JULIA || pLexCurrent->iLexer == SCLEX_RUST) && chWordEnd == '!') {
						const int chNext = SciCall_GetCharAt(wordEnd + 1);
						if (chNext == '(') {
							wordEnd += 2;
							pWord[wordLength++] = '!';
							pWord[wordLength++] = '(';
							pWord[wordLength++] = ')';
						}
					}
					else if (chWordEnd == '(') {
						if (space && NeedSpaceAfterKeyword(pWord, wordLength)) {
							pWord[wordLength++] = ' ';
						}

						pWord[wordLength++] = '(';
						pWord[wordLength++] = ')';
						wordEnd++;
					}

					if (wordLength >= iRootLen) {
						pWord[wordLength] = '\0';
						WordList_AddWord(pWList, pWord, wordLength);
						if (bSubWord) {
							WordList_AddSubWord(pWList, pWord, wordLength, iRootLen);
						}
					}
				}
			}
		}

		ft.chrg.cpMin = wordEnd;
		iPosFind = SciCall_FindTextFull(findFlag, &ft);
	}

	if (pFind != onStack) {
		NP2HeapFree(pFind);
	}
}

static void AutoC_AddKeyword(struct WordList *pWList, int iCurrentStyle) {
	const int iLexer = pLexCurrent->iLexer;
	if (iLexer != SCLEX_PHPSCRIPT) {
		uint64_t attr = pLexCurrent->keywordAttr;
		for (UINT i = 0; i < KEYWORDSET_MAX + 1; attr >>= 4, i++) {
			const char *pKeywords = pLexCurrent->pKeyWords->pszKeyWords[i];
			if (!(attr & KeywordAttr_NoAutoComp) && StrNotEmptyA(pKeywords)) {
				WordList_AddListEx(pWList, pKeywords);
			}
		}
	}

	// additional keywords
	if (np2_LexKeyword && !(iLexer == SCLEX_CPP && !IsCppCommentStyle(iCurrentStyle))) {
		WordList_AddList(pWList, (*np2_LexKeyword)[0]);
		WordList_AddList(pWList, (*np2_LexKeyword)[1]);
		WordList_AddList(pWList, (*np2_LexKeyword)[2]);
		WordList_AddList(pWList, (*np2_LexKeyword)[3]);
	}

	// embedded script
	LPCEDITLEXER pLex = NULL;
	if (iLexer == SCLEX_HTML || iLexer == SCLEX_PHPSCRIPT) {
		const int block = GetCurrentHtmlTextBlockEx(iLexer, iCurrentStyle);
		switch (block) {
		case HtmlTextBlock_JavaScript:
			pLex = &lexJavaScript;
			break;
		case HtmlTextBlock_VBScript:
			pLex = &lexVBScript;
			break;
		case HtmlTextBlock_Python:
			pLex = &lexPython;
			break;
		case HtmlTextBlock_PHP:
			pLex = &lexPHP;
			break;
		case HtmlTextBlock_CSS:
			pLex = &lexCSS;
			break;
		default:
			break;
		}
	} else if (pLexCurrent->rid == NP2LEX_TYPESCRIPT) {
		pLex = &lexJavaScript;
	}
	if (pLex != NULL) {
		uint64_t attr = pLex->keywordAttr;
		for (UINT i = 0; i < KEYWORDSET_MAX + 1; attr >>= 4, i++) {
			const char *pKeywords = pLex->pKeyWords->pszKeyWords[i];
			if (!(attr & KeywordAttr_NoAutoComp) && StrNotEmptyA(pKeywords)) {
				WordList_AddListEx(pWList, pKeywords);
			}
		}
	}
	if (iLexer == SCLEX_PHPSCRIPT || iLexer == SCLEX_JAVASCRIPT || iLexer == SCLEX_MARKDOWN) {
		WordList_AddListEx(pWList, lexHTML.pKeyWords->pszKeyWords[HTMLKeywordIndex_Tag]);
		WordList_AddListEx(pWList, lexHTML.pKeyWords->pszKeyWords[HTMLKeywordIndex_Attribute]);
		WordList_AddListEx(pWList, lexHTML.pKeyWords->pszKeyWords[HTMLKeywordIndex_EventHandler]);
		WordList_AddListEx(pWList, lexHTML.pKeyWords->pszKeyWords[HTMLKeywordIndex_Value]);
	}
}

typedef enum AddWordResult {
	AddWordResult_None,
	AddWordResult_Finish,
	AddWordResult_IgnoreLexer,
} AddWordResult;

static AddWordResult AutoC_AddSpecWord(struct WordList *pWList, int iCurrentStyle, int iPrevStyle, int ch, int chPrev) {
#if EnableLaTeXLikeEmojiInput
	if ((ch == '\\' || (chPrev == '\\' && (ch == '^' || ch == ':'))) && autoCompletionConfig.bLaTeXInputMethod) {
		if (ch != ':') {
			WordList_AddListEx(pWList, LaTeXInputSequenceString);
		} else {
			WordList_AddListEx(pWList, EmojiInputSequenceString);
		}
	}
#else
	if ((ch == '\\' || (chPrev == '\\' && ch == '^')) && autoCompletionConfig.bLaTeXInputMethod) {
		WordList_AddListEx(pWList, LaTeXInputSequenceString);
	}
#endif

	LPCEDITLEXER pLex = pLexCurrent;
	int rid = pLex->rid;
	if (rid == NP2LEX_PHP) {
		if (iCurrentStyle >= css_style(SCE_CSS_DEFAULT)) {
			iCurrentStyle -= css_style(SCE_CSS_DEFAULT);
			iPrevStyle -= css_style(SCE_CSS_DEFAULT);
			rid = NP2LEX_CSS;
			pLex = &lexCSS;
		} else if (iCurrentStyle >= js_style(SCE_JS_DEFAULT)) {
			iCurrentStyle -= js_style(SCE_JS_DEFAULT);
			iPrevStyle -= js_style(SCE_JS_DEFAULT);
			rid = NP2LEX_JAVASCRIPT;
			pLex = &lexJavaScript;
		} else if (iCurrentStyle < SCE_PHP_DEFAULT) {
			rid = NP2LEX_HTML;
			pLex = &lexHTML;
		}
	}

	switch (rid) {
	case NP2LEX_AUTOHOTKEY:
		if (ch == '#' && iCurrentStyle == SCE_AHK_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[AutoHotkeyKeywordIndex_Directive]);
			return AddWordResult_Finish;
		}
		if (ch == '@' && (iCurrentStyle == SCE_AHK_COMMENTLINE || iCurrentStyle == SCE_AHK_COMMENTBLOCK)) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[AutoHotkeyKeywordIndex_CompilerDirective]);
			return AddWordResult_Finish;
		}
		break;

	case NP2LEX_AUTOIT3:
		if (ch == '#' && iCurrentStyle == SCE_AU3_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[AutoIt3KeywordIndex_Directive]);
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[AutoIt3KeywordIndex_Special]);
			return AddWordResult_Finish;
		}
		if (ch == '@' && iCurrentStyle == SCE_AU3_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[AutoIt3KeywordIndex_Macro]);
			return AddWordResult_Finish;
		}
		break;

	case NP2LEX_ABAQUS:
	case NP2LEX_APDL:
		if (iCurrentStyle == 0 && (ch == '*' || ch == '/')) {
			const int index = (ch == '/') ? APDLKeywordIndex_SlashCommand : APDLKeywordIndex_StarCommand;
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[index]);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_CSS:
		if (ch == '@' && iCurrentStyle == SCE_CSS_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[CSSKeywordIndex_AtRule]);
			return AddWordResult_IgnoreLexer;
		}
		if (ch == ':' && (iCurrentStyle == SCE_CSS_DEFAULT || iCurrentStyle == SCE_CSS_OPERATOR)) {
			if (chPrev == ':') {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[CSSKeywordIndex_PseudoElement]);
				return AddWordResult_IgnoreLexer;
			}
			if (!(iPrevStyle == SCE_CSS_PROPERTY || iPrevStyle == SCE_CSS_UNKNOWN_PROPERTY)) {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[CSSKeywordIndex_PseudoClass]);
				return AddWordResult_IgnoreLexer;
			}
		}
		break;

	case NP2LEX_CPP:
	case NP2LEX_RESOURCESCRIPT:
		if (IsCppCommentStyle(iCurrentStyle) && np2_LexKeyword) {
			if ((ch == '@' || ch == '\\') && (np2_LexKeyword == &kwDoxyDoc)) {
				WordList_AddList(pWList, (*np2_LexKeyword)[0]);
				WordList_AddList(pWList, (*np2_LexKeyword)[1]);
				WordList_AddList(pWList, (*np2_LexKeyword)[2]);
				WordList_AddList(pWList, (*np2_LexKeyword)[3]);
				return AddWordResult_Finish;
			}
		} else if (iCurrentStyle == SCE_C_DEFAULT) {
			if (ch == '#') { // #preprocessor
				const char *pKeywords = pLex->pKeyWords->pszKeyWords[CPPKeywordIndex_Preprocessor];
				if (StrNotEmptyA(pKeywords)) {
					WordList_AddListEx(pWList, pKeywords);
					return AddWordResult_Finish;
				}
			} else if (ch == '@') { // @directive, @annotation, @decorator
				const char *pKeywords = pLex->pKeyWords->pszKeyWords[CPPKeywordIndex_Directive];
				if (StrNotEmptyA(pKeywords)) {
					WordList_AddListEx(pWList, pKeywords);
					// user defined annotation
					return AddWordResult_IgnoreLexer;
				}
			}
			//else if (chPrev == ':' && ch == ':') {
			//	WordList_AddList(pWList, "C++/namespace C++/Java8/PHP/static SendMessage()");
			//}
			//else if (chPrev == '-' && ch == '>') {
			//	WordList_AddList(pWList, "C/C++pointer PHP-variable");
			//}
		}
		break;

	case NP2LEX_CSHARP:
		if (ch == '#' && iCurrentStyle == SCE_CSHARP_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[CSharpKeywordIndex_Preprocessor]);
			return AddWordResult_Finish;
		}
		if ((ch == '<' || (chPrev == '<' && ch == '/')) && (iCurrentStyle > SCE_CSHARP_DEFAULT && iCurrentStyle < SCE_CSHARP_TASKMARKER)) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[CSharpKeywordIndex_CommentTag]);
			return AddWordResult_Finish;
		}
		break;

	case NP2LEX_CONFIG:
	case NP2LEX_HTML:
	case NP2LEX_XML:
		if (ch == '<' || (chPrev == '<' && ch == '/')) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[HTMLKeywordIndex_Tag]);
			if (rid == NP2LEX_XML) {
				if (np2_LexKeyword) { // XML Tag
					WordList_AddList(pWList, (*np2_LexKeyword)[0]);
				}
			}
			return AddWordResult_IgnoreLexer; // application defined tags
		}
		break;

	case NP2LEX_MARKDOWN:
		if (ch == '<' || (chPrev == '<' && ch == '/')) {
			WordList_AddList(pWList, lexHTML.pKeyWords->pszKeyWords[HTMLKeywordIndex_Tag]);
			return AddWordResult_IgnoreLexer; // custom tags
		}
		break;

	case NP2LEX_DLANG:
		if ((ch == '#' || ch == '@') && iCurrentStyle == SCE_D_DEFAULT) {
			const int index = (ch == '#') ? DKeywordIndex_Preprocessor : DKeywordIndex_Attribute;
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[index]);
			return AddWordResult_Finish;
		}
		break;

	case NP2LEX_DART:
		if (ch == '@' && iCurrentStyle == SCE_DART_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[DartKeywordIndex_Metadata]);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_FORTRAN:
		if (ch == '#' && iCurrentStyle == SCE_F_PREPROCESSOR) {
			WordList_AddList(pWList, lexCPP.pKeyWords->pszKeyWords[CPPKeywordIndex_Preprocessor]);
			return AddWordResult_Finish;
		}
		break;

	case NP2LEX_HAXE:
		if (ch == '#' && iCurrentStyle == SCE_HAXE_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[HaxeKeywordIndex_Preprocessor]);
			return AddWordResult_Finish;
		}
		if (ch == '@' && iCurrentStyle == SCE_HAXE_COMMENTBLOCKDOC) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[HaxeKeywordIndex_CommentTag]);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_INNOSETUP:
		if (ch == '#' && (iCurrentStyle == SCE_INNO_DEFAULT || iCurrentStyle == SCE_INNO_INLINE_EXPANSION)) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[InnoKeywordIndex_Directive]);
			return (iCurrentStyle == SCE_INNO_DEFAULT) ? AddWordResult_Finish : AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_GRAPHVIZ:
		if (ch == '<' || (chPrev == '<' && ch == '/')) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[GraphVizKeywordIndex_HtmlLabel]);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_GRADLE:
	case NP2LEX_GROOVY:
	case NP2LEX_JAVA:
		if (ch == '@') {
			NP2_static_assert(JavaKeywordIndex_Annotation == GroovyKeywordIndex_Annotation);
			NP2_static_assert(JavaKeywordIndex_Javadoc == GroovyKeywordIndex_Groovydoc);
			if (iCurrentStyle == SCE_JAVA_DEFAULT) {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[JavaKeywordIndex_Annotation]);
				return AddWordResult_IgnoreLexer;
			}
			if (iCurrentStyle >= SCE_JAVA_COMMENTBLOCKDOC && iCurrentStyle <= SCE_JAVA_TASKMARKER) {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[JavaKeywordIndex_Javadoc]);
				return AddWordResult_Finish;
			}
		}
		break;

	case NP2LEX_ACTIONSCRIPT:
	case NP2LEX_JAVASCRIPT:
	case NP2LEX_TYPESCRIPT:
		if (ch == '@' || (ch == '<' && rid == NP2LEX_TYPESCRIPT)) {
			if (iCurrentStyle >= SCE_JS_COMMENTLINE && iCurrentStyle <= SCE_JS_TASKMARKER) {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[JavaScriptKeywordIndex_Jsdoc]);
				return AddWordResult_Finish;
			}
#if 0
			if (ch == '@' && iCurrentStyle == SCE_JS_DEFAULT) {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[JavaScriptKeywordIndex_Decorator]);
				return AddWordResult_IgnoreLexer;
			}
#endif
		}
		break;

	case NP2LEX_JULIA:
		if (ch == '@' && iCurrentStyle == SCE_JULIA_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[JuliaKeywordIndex_Macro]);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_KOTLIN:
		if (ch == '@') {
			if (iCurrentStyle == SCE_KOTLIN_DEFAULT) {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[KotlinKeywordIndex_Annotation]);
				return AddWordResult_IgnoreLexer;
			}
			if (iCurrentStyle >= SCE_KOTLIN_COMMENTLINE && iCurrentStyle <= SCE_KOTLIN_TASKMARKER) {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[KotlinKeywordIndex_Kdoc]);
				return AddWordResult_Finish;
			}
		}
		break;

	case NP2LEX_LATEX:
	case NP2LEX_TEXINFO:
		if ((ch == '\\' || (chPrev == '\\' && ch == '^')) && !autoCompletionConfig.bLaTeXInputMethod) {
			WordList_AddListEx(pWList, LaTeXInputSequenceString);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_PHP:
		if (ch == '@') {
			if (iCurrentStyle >= SCE_PHP_COMMENTLINE && iCurrentStyle <= SCE_PHP_TASKMARKER) {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[PHPKeywordIndex_Phpdoc]);
				return AddWordResult_Finish;
			}
		}
		break;

	case NP2LEX_POWERSHELL:
		if ((ch == '$' || ch == '@')) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[PowerShellKeywordIndex_PredefinedVariable]);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_PYTHON:
		if (ch == '@' && iCurrentStyle == SCE_PY_DEFAULT) {
			const char *pKeywords = pLex->pKeyWords->pszKeyWords[PythonKeywordIndex_Decorator];
			if (StrNotEmptyA(pKeywords)) {
				WordList_AddListEx(pWList, pKeywords);
				return AddWordResult_IgnoreLexer;
			}
		}
		break;

	case NP2LEX_REBOL:
		if (ch == '#' && iCurrentStyle == SCE_REBOL_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[RebolKeywordIndex_Directive]);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_SCALA:
		if (ch == '@') {
			if (iCurrentStyle == SCE_SCALA_DEFAULT) {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[ScalaKeywordIndex_Annotation]);
				return AddWordResult_IgnoreLexer;
			}
			if (iCurrentStyle >= SCE_SCALA_COMMENTLINE && iCurrentStyle <= SCE_SCALA_TASKMARKER) {
				WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[ScalaKeywordIndex_Scaladoc]);
				return AddWordResult_Finish;
			}
		}
		break;

	case NP2LEX_SMALI:
		if (ch == '.' && iCurrentStyle == SCE_C_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[SmaliKeywordIndex_Directive]);
			return AddWordResult_Finish;
		}
		break;

	case NP2LEX_SWIFT:
		if ((ch == '@' || ch == '#') && iCurrentStyle == SCE_SWIFT_DEFAULT) {
			const int index = (ch == '#') ? SwiftKeywordIndex_Directive : SwiftKeywordIndex_Attribute;
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[index]);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_VHDL:
		if (ch == '`' && iCurrentStyle == SCE_VHDL_DEFAULT) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[VHDLKeywordIndex_Directive]);
			return AddWordResult_IgnoreLexer;
		}
		if (ch == '\'' && iCurrentStyle == SCE_VHDL_OPERATOR) {
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[VHDLKeywordIndex_Attribute]);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_VERILOG:
		if ((ch == '$' || ch == '`') && iCurrentStyle == SCE_V_DEFAULT) {
			const int index = (ch == '`') ? VerilogKeywordIndex_Directive : VerilogKeywordIndex_SystemTaskAndFunction;
			WordList_AddList(pWList, pLex->pKeyWords->pszKeyWords[index]);
			return AddWordResult_IgnoreLexer;
		}
		break;

	case NP2LEX_VISUALBASIC:
		if (ch == '#' && iCurrentStyle == SCE_B_DEFAULT) {
			const char *pKeywords = pLex->pKeyWords->pszKeyWords[VBKeywordIndex_Preprocessor];
			if (StrNotEmptyA(pKeywords)) {
				WordList_AddListEx(pWList, pKeywords);
				return AddWordResult_Finish;
			}
		}
		break;
	}
	return AddWordResult_None;
}

void EditCompleteUpdateConfig(void) {
	int i = 0;
	const int mask = autoCompletionConfig.fAutoCompleteFillUpMask;
	if (mask & AutoCompleteFillUpSpace) {
		autoCompletionConfig.szAutoCompleteFillUp[i++] = ' ';
	}

	const BOOL punctuation = mask & AutoCompleteFillUpPunctuation;
	int k = 0;
	for (UINT j = 0; j < COUNTOF(autoCompletionConfig.wszAutoCompleteFillUp); j++) {
		const WCHAR c = autoCompletionConfig.wszAutoCompleteFillUp[j];
		if (c == L'\0') {
			break;
		}
		if (IsPunctuation(c)) {
			autoCompletionConfig.wszAutoCompleteFillUp[k++] = c;
			if (punctuation) {
				autoCompletionConfig.szAutoCompleteFillUp[i++] = (char)c;
			}
		}
	}

	autoCompletionConfig.szAutoCompleteFillUp[i] = '\0';
	autoCompletionConfig.wszAutoCompleteFillUp[k] = L'\0';
}

static bool EditCompleteWordCore(int iCondition, bool autoInsert) {
	const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
	const int iCurrentStyle = SciCall_GetStyleIndexAt(iCurrentPos);
	const Sci_Line iLine = SciCall_LineFromPosition(iCurrentPos);
	const Sci_Position iLineStartPos = SciCall_PositionFromLine(iLine);

	// word before current position
	Sci_Position iStartWordPos = iCurrentPos;
	do {
		Sci_Position before = iStartWordPos;
		iStartWordPos = SciCall_WordStartPosition(before, true);
		const bool nonWord = iStartWordPos == before;
		before = SciCall_PositionBefore(iStartWordPos);
		if (nonWord) {
			// non-word
			if (before + 1 != iStartWordPos) {
				break;
			}

			const int ch = SciCall_GetCharAt(before);
			if (!IsDocWordChar(ch) || IsSpecialStartChar(ch, '\0')) {
				break;
			}

			iStartWordPos = before;
		} else {
			const Sci_Position iPos = SciCall_WordEndPosition(before, true);
			if (iPos == iStartWordPos) {
				// after CJK word
				break;
			}
		}
	} while (iStartWordPos > iLineStartPos);
	if (iStartWordPos == iCurrentPos) {
		return false;
	}

	// beginning of word
	int ch = SciCall_GetCharAt(iStartWordPos);

	int chPrev = '\0';
	int chPrev2 = '\0';
	if (ch < 0x80 && iStartWordPos > iLineStartPos) {
		const Sci_Position before = SciCall_PositionBefore(iStartWordPos);
		if (before + 1 == iStartWordPos) {
			chPrev = SciCall_GetCharAt(before);
			const Sci_Position before2 = SciCall_PositionBefore(before);
			if (before2 >= iLineStartPos && before2 + 1 == before) {
				chPrev2 = SciCall_GetCharAt(before2);
			}
			// word after escape character or format specifier
			if (chPrev == '%' || chPrev == pLexCurrent->escapeCharacterStart) {
				const int style = SciCall_GetStyleIndexAt(iStartWordPos);
				if (IsEscapeCharOrFormatSpecifier(before, ch, chPrev, style, false)) {
					++iStartWordPos;
					ch = SciCall_GetCharAt(iStartWordPos);
					chPrev = '\0';
				}
			}
		}
	}

	int iRootLen = autoCompletionConfig.iMinWordLength;
	if (ch >= '0' && ch <= '9') {
		if (autoCompletionConfig.iMinNumberLength <= 0) { // ignore number
			return false;
		}

		iRootLen = autoCompletionConfig.iMinNumberLength;
		if (ch == '0') {
			// number prefix
			const int chNext = UnsafeLower(SciCall_GetCharAt(iStartWordPos + 1));
			if (chNext == 'x' || chNext == 'b' || chNext == 'o') {
				iRootLen += 2;
			}
		}
	}

	if (iCurrentPos - iStartWordPos < iRootLen) {
		return false;
	}

	// preprocessor like: # space preprocessor
	if ((pLexCurrent->lexerAttr & LexerAttr_CppPreprocessor) && (chPrev == '#' || IsASpaceOrTab(chPrev))) {
		Sci_Position before = iStartWordPos - 1;
		if (chPrev != '#') {
			while (before >= iLineStartPos) {
				chPrev = SciCall_GetCharAt(before);
				if (!IsASpaceOrTab(chPrev)) {
					break;
				}
				--before;
			}
		}
		if (chPrev == '#') {
			if (before > iLineStartPos) {
				--before;
				while (before >= iLineStartPos && IsASpaceOrTab(SciCall_GetCharAt(before))) {
					--before;
				}
				if (before >= iLineStartPos) {
					chPrev = '\0';
				}
			}
			ch = chPrev;
		}
		chPrev = '\0';
	} else if (IsSpecialStartChar(chPrev, chPrev2)) {
		ch = chPrev;
		chPrev = chPrev2;
	}

	// optimization for small string
	char onStack[64];
	char *pRoot;
	if (iCurrentPos - iStartWordPos + 1 < (Sci_Position)sizeof(onStack)) {
		memset(onStack, 0, sizeof(onStack));
		pRoot = onStack;
	} else {
		pRoot = (char *)NP2HeapAlloc(iCurrentPos - iStartWordPos + 1);
	}

	struct Sci_TextRangeFull tr = { { iStartWordPos, iCurrentPos }, pRoot };
	SciCall_GetTextRangeFull(&tr);
	iRootLen = (int)strlen(pRoot);

#if 0
	StopWatch watch;
	StopWatch_Start(watch);
#endif

	bool bIgnoreLexer = (pRoot[0] >= '0' && pRoot[0] <= '9'); // number
	const bool bIgnoreCase = bIgnoreLexer || autoCompletionConfig.bIgnoreCase;
	struct WordList pWList;
	WordList_Init(&pWList, pRoot, iRootLen, bIgnoreCase);
	bool bIgnoreDoc = false;
	char prefix = '\0';

	if (!bIgnoreLexer && IsSpecialStartChar(ch, chPrev)) {
		int iPrevStyle = 0;
		if (ch == ':' && chPrev != ':') {
			const Sci_Position iPos = SciCall_WordStartPosition(iStartWordPos - 1, false);
			iPrevStyle = SciCall_GetStyleIndexAt(iPos);
		}

		const AddWordResult result = AutoC_AddSpecWord(&pWList, iCurrentStyle, iPrevStyle, ch, chPrev);
		if (result == AddWordResult_Finish) {
			bIgnoreLexer = true;
			bIgnoreDoc = true;
		} else if (result == AddWordResult_IgnoreLexer) {
			bIgnoreLexer = true;
			// HTML/XML Tag
			if (ch == '/' || ch == '>') {
				ch = '<';
			}
			prefix = (char)ch;
		}
	}

	bool retry = false;
	const bool bScanWordsInDocument = autoCompletionConfig.bScanWordsInDocument;
	do {
		if (!bIgnoreLexer) {
			// keywords
			AutoC_AddKeyword(&pWList, iCurrentStyle);
		}
		if (bScanWordsInDocument) {
			if (!bIgnoreDoc || pWList.nWordCount == 0) {
				AutoC_AddDocWord(&pWList, bIgnoreCase, prefix);
			}
			if (prefix && pWList.nWordCount == 0) {
				prefix = '\0';
				AutoC_AddDocWord(&pWList, bIgnoreCase, prefix);
			}
		}

		retry = false;
		if (pWList.nWordCount == 0 && iRootLen != 0) {
			const char *pSubRoot = strpbrk(pWList.pWordStart, ":.#@<\\/->$%");
			if (pSubRoot) {
				while (IsSpecialStart(*pSubRoot)) {
					pSubRoot++;
				}
				if (*pSubRoot) {
					iRootLen = (int)strlen(pSubRoot);
					WordList_UpdateRoot(&pWList, pSubRoot, iRootLen);
					retry = true;
					bIgnoreLexer = false;
					bIgnoreDoc = false;
					prefix = '\0';
				}
			}
		}
	} while (retry);

#if 0
	StopWatch_Stop(watch);
	const double elapsed = StopWatch_Get(&watch);
	printf("Notepad2 AddDocWord(%u, %u): %.6f\n", pWList.nWordCount, pWList.nTotalLen, elapsed);
#endif

	const bool bShow = pWList.nWordCount > 0 && !(pWList.nWordCount == 1 && pWList.nTotalLen == (UINT)(iRootLen + 1));
	const bool bUpdated = (autoCompletionConfig.iPreviousItemCount == 0)
		// deleted some words. leave some words that no longer matches current input at the top.
		|| (iCondition == AutoCompleteCondition_OnCharAdded && autoCompletionConfig.iPreviousItemCount - pWList.nWordCount > autoCompletionConfig.iVisibleItemCount)
		// added some words. TODO: check top matched items before updating, if top items not changed, delay the update.
		|| (iCondition == AutoCompleteCondition_OnCharDeleted && autoCompletionConfig.iPreviousItemCount < pWList.nWordCount);

	if (bShow && bUpdated) {
		autoCompletionConfig.iPreviousItemCount = pWList.nWordCount;
		char *pList = WordList_GetList(&pWList);
		SciCall_AutoCSetOptions(SC_AUTOCOMPLETE_FIXED_SIZE);
		SciCall_AutoCSetOrder(SC_ORDER_PRESORTED); // pre-sorted
		SciCall_AutoCSetIgnoreCase(bIgnoreCase); // case sensitivity
		//if (bIgnoreCase) {
		//	SciCall_AutoCSetCaseInsensitiveBehaviour(SC_CASEINSENSITIVEBEHAVIOUR_IGNORECASE);
		//}
		//SciCall_AutoCSetSeparator('\n');
		//SciCall_AutoCSetTypeSeparator('\t');
		SciCall_AutoCSetFillUps(autoCompletionConfig.szAutoCompleteFillUp);
		//SciCall_AutoCSetDropRestOfWord(true); // delete orginal text: pRoot
		SciCall_AutoCSetMaxHeight(min_u(pWList.nWordCount, autoCompletionConfig.iVisibleItemCount)); // visible rows
		SciCall_AutoCSetCancelAtStart(false); // don't cancel the list when deleting character
		SciCall_AutoCSetChooseSingle(autoInsert);
		SciCall_AutoCShow(pWList.iStartLen, pList);
		NP2HeapFree(pList);
	}

	if (pRoot != onStack) {
		NP2HeapFree(pRoot);
	}
	WordList_Free(&pWList);
	return bShow;
}

void EditCompleteWord(int iCondition, bool autoInsert) {
	if (iCondition == AutoCompleteCondition_OnCharAdded) {
		if (autoCompletionConfig.iPreviousItemCount <= 2*autoCompletionConfig.iVisibleItemCount) {
			return;
		}
		// too many words in auto-completion list, recreate it.
	}

	if (iCondition == AutoCompleteCondition_Normal) {
		autoCompletionConfig.iPreviousItemCount = 0; // recreate list
	}

	const bool bShow = EditCompleteWordCore(iCondition, autoInsert);
	if (!bShow) {
		autoCompletionConfig.iPreviousItemCount = 0;
		if (iCondition != AutoCompleteCondition_Normal) {
			SciCall_AutoCCancel();
		}
	}
}

static bool CanAutoCloseSingleQuote(int chPrev, int iCurrentStyle) {
	const int iLexer = pLexCurrent->iLexer;
	if (iCurrentStyle == 0) {
		if (iLexer == SCLEX_VISUALBASIC || iLexer == SCLEX_VBSCRIPT) {
			return false; // comment
		}
		if (iLexer == SCLEX_HTML) {
			const HtmlTextBlock block = GetCurrentHtmlTextBlockEx(iLexer, iCurrentStyle);
			if (block == HtmlTextBlock_VBScript) {
				return false;
			}
		}
	} else {
		if (iCurrentStyle == pLexCurrent->noneSingleQuotedStyle) {
			return false;
		}
	}

	// someone's, don't
	if (IsDefaultWordChar(chPrev)) {
		// character or string prefix
		if (pLexCurrent->lexerAttr & LexerAttr_CharacterPrefix) {
			const int chPrev2 = SciCall_GetCharAt(SciCall_GetCurrentPos() - 3);
			if (IsDefaultWordChar(chPrev2)) {
				switch (iLexer) {
				case SCLEX_CPP:
					return chPrev2 == 'u' && chPrev == '8';

				case SCLEX_FORTRAN:
					return chPrev == '_';

				case SCLEX_PYTHON:
					return UnsafeLower(chPrev) != UnsafeLower(chPrev2)
						&& IsCharacterPrefix(chPrev) && IsCharacterPrefix(chPrev2);
				}
			} else {
				return IsCharacterPrefix(chPrev);
			}
		}
		return false;
	}

	if (iLexer == SCLEX_RUST || iLexer == SCLEX_REBOL || iLexer == SCLEX_VERILOG) {
		// TODO: Rust lifetime, REBOL symbol, Verilog literal
		return false;
	}

	return true;
}

bool EditIsOpenBraceMatched(Sci_Position pos, Sci_Position startPos) {
	// SciCall_GetEndStyled() is SciCall_GetCurrentPos() - 1
	// only find close brace with same style in next 4KiB text
	SciCall_EnsureStyledTo(pos + 1024*4);
	// find next close brace
	const Sci_Position iPos = SciCall_BraceMatchNext(pos, startPos);
	if (iPos >= 0) {
		// style may not matched when iPos > SciCall_GetEndStyled() (e.g. iPos on next line), see Document::BraceMatch()
#if 0
		SciCall_EnsureStyledTo(iPos + 1);
#endif
		// TODO: retry when style not matched
		if (SciCall_GetStyleIndexAt(pos) == SciCall_GetStyleIndexAt(iPos)) {
			// check whether next close brace already matched
			return pos == 0 || SciCall_BraceMatchNext(iPos, SciCall_PositionBefore(pos)) < 0;
		}
	}
	return false;
}

void EditAutoCloseBraceQuote(int ch) {
	const Sci_Position iCurPos = SciCall_GetCurrentPos();
	const int chPrev = SciCall_GetCharAt(iCurPos - 2);
	const int chNext = SciCall_GetCharAt(iCurPos);
	const int iPrevStyle = SciCall_GetStyleIndexAt(iCurPos - 2);
	const int iNextStyle = SciCall_GetStyleIndexAt(iCurPos);

	if (iPrevStyle != 0) {
		const int charStyle = pLexCurrent->characterLiteralStyle;
		if (charStyle != 0) {
			// within character literal
			if (iPrevStyle == charStyle && iNextStyle == charStyle) {
				return;
			}
			if (pLexCurrent->iLexer == SCLEX_RUST && (iPrevStyle == SCE_RUST_BYTE_CHARACTER && iNextStyle == SCE_RUST_BYTE_CHARACTER)) {
				return;
			}
		}

		// escape sequence
		if (ch != ',' && (chPrev != '\0' && chPrev == pLexCurrent->escapeCharacterStart)) {
			const int style = SciCall_GetStyleIndexAt(iCurPos - 1);
			if (IsEscapeCharOrFormatSpecifier(iCurPos - 2, ch, chPrev, style, true)) {
				return;
			}
		}
	}

	const int mask = autoCompletionConfig.fAutoInsertMask;
	char fillChar = '\0';
	bool closeBrace = false;
	switch (ch) {
	case '(':
		if (mask & AutoInsertParenthesis) {
			fillChar = ')';
			closeBrace = true;
		}
		break;
	case '[':
		if ((mask & AutoInsertSquareBracket) && !(pLexCurrent->iLexer == SCLEX_SMALI)) { // JVM array type
			fillChar = ']';
			closeBrace = true;
		}
		break;
	case '{':
		if (mask & AutoInsertBrace) {
			fillChar = '}';
			closeBrace = true;
		}
		break;
	case '<':
		if ((mask & AutoInsertAngleBracket) && IsGenericTypeStyle(iPrevStyle)) {
			// geriatric type, template
			fillChar = '>';
		}
		break;
	case '\"':
		if ((mask & AutoInsertDoubleQuote)) {
			fillChar = '\"';
		}
		break;
	case '\'':
		if ((mask & AutoInsertSingleQuote) && CanAutoCloseSingleQuote(chPrev, iPrevStyle)) {
			fillChar = '\'';
		}
		break;
	case '`':
		//if (pLexCurrent->iLexer == SCLEX_BASH
		//|| pLexCurrent->iLexer == SCLEX_JULIA
		//|| pLexCurrent->iLexer == SCLEX_MAKEFILE
		//|| pLexCurrent->iLexer == SCLEX_SQL
		//) {
		//	fillChar = '`';
		//} else if (0) {
		//	fillChar = '\'';
		//}
		if ((mask & AutoInsertBacktick) && pLexCurrent->iLexer != SCLEX_VERILOG && pLexCurrent->iLexer != SCLEX_VHDL) {
			fillChar = '`';
		}
		break;
	case ',':
		if ((mask & AutoInsertSpaceAfterComma) && !(chNext == ' ' || chNext == '\t' || (chPrev == '\'' && chNext == '\'') || (chPrev == '\"' && chNext == '\"'))) {
			fillChar = ' ';
		}
		break;
	default:
		break;
	}

	if (fillChar) {
		if (closeBrace && EditIsOpenBraceMatched(iCurPos - 1, iCurPos)) {
			return;
		}
		// TODO: auto escape quotes inside string
		//else if (ch == fillChar) {
		//}

		const char tchIns[4] = { fillChar };
		SciCall_ReplaceSel(tchIns);
		const Sci_Position iCurrentPos = (ch == ',') ? iCurPos + 1 : iCurPos;
		SciCall_SetSel(iCurrentPos, iCurrentPos);
		if (closeBrace) {
			// fix brace matching
			SciCall_EnsureStyledTo(iCurPos + 1);
		}
	}
}

static inline bool IsHtmlVoidTag(const char *word, int length) {
	// same as htmlVoidTagList in scintilla/lexlib/DocUtils.h
	const char *p = StrStrIA(
		// void elements
		" area base basefont br col command embed frame hr img input isindex keygen link meta param source track wbr "
		// end tag may omittd
		" p "
		, word);
	return p != NULL && p[-1] == ' ' && p[length] == ' ';
}

void EditAutoCloseXMLTag(void) {
	char tchBuf[512];
	const Sci_Position iCurPos = SciCall_GetCurrentPos();
	const Sci_Position iStartPos = max_pos(0, iCurPos - (COUNTOF(tchBuf) - 1));
	const Sci_Position iSize = iCurPos - iStartPos;
	bool shouldAutoClose = false;
	bool autoClosed = false;

	if (iSize >= 3 && autoCompletionConfig.bCloseTags) {
		shouldAutoClose = true;
		int iCurrentStyle = SciCall_GetStyleIndexAt(iCurPos);
		const int iLexer = pLexCurrent->iLexer;
		if ((pLexCurrent->lexerAttr & LexerAttr_AngleBracketGeneric)
			&& (iCurrentStyle == 0 || iCurrentStyle == pLexCurrent->operatorStyle || iCurrentStyle == pLexCurrent->operatorStyle2)) {
			shouldAutoClose = false;
		} else if (iLexer == SCLEX_CPP || iLexer == SCLEX_INNOSETUP || iLexer == SCLEX_VERILOG) {
			// C++ like #include <path>
			const int preprocessor = (iLexer == SCLEX_CPP) ? SCE_C_PREPROCESSOR : ((iLexer == SCLEX_VERILOG) ? SCE_V_DIRECTIVE : SCE_INNO_PREPROCESSOR);
			const Sci_Line iLine = SciCall_LineFromPosition(iCurPos);
			Sci_Position iCurrentLinePos = SciCall_PositionFromLine(iLine);
			int ch = 0;
			while (iCurrentLinePos < iCurPos) {
				ch = SciCall_GetCharAt(iCurrentLinePos);
				if (!IsASpaceOrTab(ch)) {
					break;
				}
				iCurrentLinePos++;
			}
			if (ch == ((iLexer == SCLEX_VERILOG) ? '`' : '#')) {
				iCurrentStyle = SciCall_GetStyleIndexAt(iCurrentLinePos);
				if (iCurrentStyle == preprocessor) {
					shouldAutoClose = false;
				}
			}
		}
	}

	if (shouldAutoClose) {
		struct Sci_TextRangeFull tr = { { iStartPos, iCurPos }, tchBuf };
		SciCall_GetTextRangeFull(&tr);

		if (tchBuf[iSize - 2] != '/') {
			char tchIns[516] = "</";
			int cchIns = 2;
			const char *pBegin = tchBuf;
			const char *pCur = tchBuf + iSize - 2;

			while (pCur > pBegin && *pCur != '<' && *pCur != '>') {
				--pCur;
			}

			if (*pCur == '<') {
				pCur++;
				while (IsHtmlTagChar(*pCur)) {
					tchIns[cchIns++] = *pCur;
					pCur++;
				}
			}

			tchIns[cchIns++] = '>';
			tchIns[cchIns] = '\0';

			shouldAutoClose = cchIns > 3;
			if (shouldAutoClose && pLexCurrent->iLexer == SCLEX_HTML) {
				tchIns[cchIns - 1] = '\0';
				shouldAutoClose = !IsHtmlVoidTag(tchIns + 2, cchIns - 3);
			}
			if (shouldAutoClose) {
				tchIns[cchIns - 1] = '>';
				autoClosed = true;
				SciCall_ReplaceSel(tchIns);
				SciCall_SetSel(iCurPos, iCurPos);
			}
		}
	}

	if (!autoClosed && autoCompletionConfig.bCompleteWord) {
		const Sci_Position iPos = SciCall_GetCurrentPos();
		if (SciCall_GetCharAt(iPos - 2) == '-') {
			EditCompleteWord(AutoCompleteCondition_Normal, false); // obj->field, obj->method
		}
	}
}

typedef enum AutoIndentType {
	AutoIndentType_None,
	AutoIndentType_IndentOnly,
	AutoIndentType_IndentAndClose,
} AutoIndentType;

static const char *EditKeywordIndent(LPCEDITLEXER pLex, const char *head, AutoIndentType *indent) {
	char word[16] = "";
	char word_low[16] = "";
	int length = 0;
	const char *endPart = NULL;
	*indent = AutoIndentType_None;

	while (*head && length < 15) {
		const char lower = UnsafeLower(*head);
		if (lower < 'a' || lower > 'z') {
			break;
		}
		word[length] = *head;
		word_low[length] = lower;
		++length;
		++head;
	}

	switch (pLex->rid) {
	//case NP2LEX_AUTOIT3:
	case NP2LEX_BASH:
		if (np2LexLangIndex == IDM_LEXER_CSHELL) {
			if (StrEqualExA(word, "if")) {
				*indent = AutoIndentType_IndentAndClose;
				endPart = "endif";
			} else if (StrEqualExA(word, "switch")) {
				*indent = AutoIndentType_IndentAndClose;
				endPart = "endsw";
			} else if (StrEqualExA(word, "foreach") || StrEqualExA(word, "while")) {
				*indent = AutoIndentType_IndentAndClose;
				endPart = "end";
			}
		} else {
			if (StrEqualExA(word, "if")) {
				*indent = AutoIndentType_IndentAndClose;
				endPart = "fi";
			} else if (StrEqualExA(word, "case")) {
				*indent = AutoIndentType_IndentAndClose;
				endPart = "esac";
			} else if (StrEqualExA(word, "do")) {
				*indent = AutoIndentType_IndentAndClose;
				endPart = "done";
			}
		}
		break;

	case NP2LEX_CMAKE:
		if (StrEqualExA(word, "function")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "endfunction()";
		} else if (StrEqualExA(word, "macro")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "endmacro()";
		} else if (StrEqualExA(word, "if")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "endif()";
		} else if (StrEqualExA(word, "foreach")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "endforeach()";
		} else if (StrEqualExA(word, "while")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "endwhile()";
		}
		break;
	//case NP2LEX_CPP:
	//case NP2LEX_RESOURCESCRIPT:

	//case NP2LEX_INNOSETUP:

	case NP2LEX_JULIA: {
		LPCSTR pKeywords = pLex->pKeyWords->pszKeyWords[JuliaKeywordIndex_CodeFolding];
		LPCSTR p = strstr(pKeywords, word);
		if (p == pKeywords || (p != NULL &&  p[-1] == ' ')) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "end";
		}
	} break;

	case NP2LEX_LUA:
		if (StrEqualExA(word, "function") || StrEqualExA(word, "if") || StrEqualExA(word, "do")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "end";
		}
		break;

	case NP2LEX_MAKEFILE:
		if (StrEqualExA(word, "if")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "endif";
		} else if (StrEqualExA(word, "define")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "endef";
		} else if (StrEqualExA(word, "for")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "endfor";
		}
		break;
	case NP2LEX_MATLAB:
		if (StrEqualExA(word, "function")) {
			*indent = AutoIndentType_IndentOnly;
			// 'end' is optional
		} else if (StrEqualExA(word, "if") || StrEqualExA(word, "for") || StrEqualExA(word, "while") || StrEqualExA(word, "switch") || StrEqualExA(word, "try")) {
			*indent = AutoIndentType_IndentAndClose;
			if (np2LexLangIndex == IDM_LEXER_OCTAVE) {
				if (StrEqualExA(word, "if")) {
					endPart = "endif";
				} else if (StrEqualExA(word, "for")) {
					endPart = "endfor";
				} else if (StrEqualExA(word, "while")) {
					endPart = "endwhile";
				} else if (StrEqualExA(word, "switch")) {
					endPart = "endswitch";
				} else if (StrEqualExA(word, "try")) {
					endPart = "end_try_catch";
				}
			}
			if (endPart == NULL) {
				endPart = "end";
			}
		}
		break;

	//case NP2LEX_NSIS:
	//case NP2LEX_PASCAL:
	case NP2LEX_RUBY:
		if (StrEqualExA(word, "if") || StrEqualExA(word, "do") || StrEqualExA(word, "while") || StrEqualExA(word, "for")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "end";
		}
		break;

	case NP2LEX_SQL:
		if (StrEqualExA(word_low, "if")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "END IF;";
		} else if (StrEqualExA(word_low, "while")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "END WHILE;";
		} else if (StrEqualExA(word_low, "repeat")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "END REPEAT;";
		} else if (StrEqualExA(word_low, "loop") || StrEqualExA(word_low, "for")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "END LOOP;";
		} else if (StrEqualExA(word_low, "case")) {
			*indent = AutoIndentType_IndentAndClose;
			endPart = "END CASE;";
		} else if (StrEqualExA(word_low, "begin")) {
			*indent = AutoIndentType_IndentAndClose;
			if (StrStrIA(head, "transaction") != NULL) {
				endPart = "COMMIT;";
			} else {
				endPart = "END";
			}
		} else if (StrEqualExA(word_low, "start")) {
			if (StrStrIA(head, "transaction") != NULL) {
				*indent = AutoIndentType_IndentAndClose;
				endPart = "COMMIT;";
			}
		}
		break;

	//case NP2LEX_VISUALBASIC:
	//case NP2LEX_VBSCRIPT:
	//case NP2LEX_VERILOG:
	//case NP2LEX_VHDL:
	}
	return endPart;
}

extern FILEVARS fvCurFile;

void EditAutoIndent(void) {
	const Sci_Position iCurPos = SciCall_GetCurrentPos();
	//const Sci_Position iAnchorPos = SciCall_GetAnchor();
	const Sci_Line iCurLine = SciCall_LineFromPosition(iCurPos);
	//const Sci_Position iLineLength = SciCall_GetLineLength(iCurLine);
	//const Sci_Position iIndentBefore = SciCall_GetLineIndentation(iCurLine - 1);

	// Move bookmark along with line if inserting lines (pressing return at beginning of line) because Scintilla does not do this for us
	if (iCurLine > 0) {
		const Sci_Position iPrevLineLength = SciCall_GetLineEndPosition(iCurLine - 1) - SciCall_PositionFromLine(iCurLine - 1);
		if (iPrevLineLength == 0) {
			const Sci_MarkerMask bitmask = SciCall_MarkerGet(iCurLine - 1);
			if (bitmask & MarkerBitmask_Bookmark) {
				SciCall_MarkerDelete(iCurLine - 1, MarkerNumber_Bookmark);
				SciCall_MarkerAdd(iCurLine, MarkerNumber_Bookmark);
			}
		}
	}

	if (iCurLine > 0/* && iLineLength <= 2*/) {
		const Sci_Position iPrevLineLength = SciCall_GetLineLength(iCurLine - 1);
		if (iPrevLineLength < 2) {
			return;
		}
		char *pLineBuf = (char *)NP2HeapAlloc(2 * iPrevLineLength + 1 + fvCurFile.iIndentWidth * 2 + 2 + 64);
		if (pLineBuf == NULL) {
			return;
		}

		const int iEOLMode = SciCall_GetEOLMode();
		AutoIndentType indent = AutoIndentType_None;
		Sci_Position iIndentLen = 0;
		int commentStyle = 0;
		SciCall_GetLine(iCurLine - 1, pLineBuf);
		pLineBuf[iPrevLineLength] = '\0';

		int ch = (uint8_t)pLineBuf[iPrevLineLength - 2];
		if (ch == '\r') {
			ch = (uint8_t)pLineBuf[iPrevLineLength - 3];
			iIndentLen = 1;
		}
		if (ch == '{' || ch == '[' || ch == '(') {
			indent = AutoIndentType_IndentAndClose;
		} else if (ch == ':') { // case label/Python
			indent = AutoIndentType_IndentOnly;
		} else if (ch == '*' || ch == '!') { // indent block comment
			iIndentLen = iPrevLineLength - (2 + iIndentLen);
			if (iIndentLen >= 2 && pLineBuf[iIndentLen - 2] == '/' && pLineBuf[iIndentLen - 1] == '*') {
				indent = AutoIndentType_IndentOnly;
				commentStyle = 1;
			}
		}

		iIndentLen = 0;
		ch = SciCall_GetCharAt(SciCall_PositionFromLine(iCurLine));
		const bool closeBrace = (ch == '}' || ch == ']' || ch == ')');
		if (indent == AutoIndentType_IndentAndClose && !closeBrace) {
			indent = AutoIndentType_IndentOnly;
		}

		char *pPos;
		const char *endPart = NULL;
		for (pPos = pLineBuf; *pPos; pPos++) {
			if (!IsASpaceOrTab(*pPos)) {
				if (indent == AutoIndentType_None && IsAlpha(*pPos)) { // indent on keywords
					const int style = SciCall_GetStyleIndexAt(SciCall_PositionFromLine(iCurLine - 1) + iIndentLen);
					if (style != 0 && style == pLexCurrent->autoIdentWordStyle) {
						endPart = EditKeywordIndent(pLexCurrent, pPos, &indent);
					}
				}
				if (indent != AutoIndentType_None) {
					memset(pPos, 0, iPrevLineLength - iIndentLen);
				}
				*pPos = '\0';
				break;
			}
			iIndentLen += 1;
		}

		if (indent == AutoIndentType_IndentAndClose && endPart) {
			const int level = SciCall_GetFoldLevel(iCurLine);
			if (!(level & SC_FOLDLEVELHEADERFLAG)) {
				const Sci_Line parent = SciCall_GetFoldParent(iCurLine);
				if (parent >= 0 && parent + 1 == iCurLine) {
					const Sci_Line child = SciCall_GetLastChild(parent);
					// TODO: check endPart is on this line
					if (SciCall_GetLineLength(child)) {
						indent = AutoIndentType_IndentOnly;
					}
				} else {
					indent = AutoIndentType_None;
				}
			}
		}

		Sci_Position iIndentPos = iCurPos;
		if (indent != AutoIndentType_None) {
			int pad = fvCurFile.iIndentWidth;
			iIndentPos += iIndentLen;
			ch = ' ';
			if (fvCurFile.bTabIndents) {
				if (fvCurFile.bTabsAsSpaces) {
					pad = fvCurFile.iTabWidth;
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
			if (indent == AutoIndentType_IndentAndClose) {
				switch (iEOLMode) {
				default: // SC_EOL_CRLF
					*pPos++ = '\r';
					*pPos++ = '\n';
					break;
				case SC_EOL_LF:
					*pPos++ = '\n';
					break;
				case SC_EOL_CR:
					*pPos++ = '\r';
					break;
				}
				strncpy(pPos, pLineBuf, iIndentLen + 1);
				pPos += iIndentLen;
				if (endPart) {
					iIndentLen = strlen(endPart);
					memcpy(pPos, endPart, iIndentLen);
					pPos += iIndentLen;
				}
			}
			*pPos = '\0';
		}

		if (*pLineBuf) {
			SciCall_AddText(strlen(pLineBuf), pLineBuf);
			if (indent != AutoIndentType_None) {
				SciCall_SetSel(iIndentPos, iIndentPos);
			}

			//const Sci_Position iPrevLineStartPos = SciCall_PositionFromLine(iCurLine - 1);
			//const Sci_Position iPrevLineEndPos = SciCall_GetLineEndPosition(iCurLine - 1);
			//const Sci_Position iPrevLineIndentPos = SciCall_GetLineIndentPosition(iCurLine - 1);

			//if (iPrevLineEndPos == iPrevLineIndentPos) {
			//	SciCall_SetTargetRange(iPrevLineStartPos, iPrevLineEndPos);
			//	SciCall_ReplaceTarget(0, "");
			//}
		}

		NP2HeapFree(pLineBuf);
		//const Sci_Position iIndent = SciCall_GetLineIndentation(iCurLine);
		//SciCall_SetLineIndentation(iCurLine, iIndentBefore);
		//iIndentLen = SciCall_GetLineIndentation(iCurLine);
		//if (iIndentLen > 0)
		//	SciCall_SetSel(iAnchorPos + iIndentLen, iCurPos + iIndentLen);
	}
}

void EditToggleCommentLine(void) {
	switch (pLexCurrent->rid) {
	case NP2LEX_ASM: {
		LPCWSTR ch;
		switch (autoCompletionConfig.iAsmLineCommentChar) {
		case AsmLineCommentCharSemicolon:
		default:
			ch = L";";
			break;
		case AsmLineCommentCharSharp:
			ch = L"# ";
			break;
		case AsmLineCommentCharSlash:
			ch = L"//";
			break;
		case AsmLineCommentCharAt:
			ch = L"@ ";
			break;
		}
		EditToggleLineComments(ch, false);
	}
	break;

	case NP2LEX_BASH:
		if (np2LexLangIndex == IDM_LEXER_M4) {
			EditToggleLineComments(L"dnl ", false);
		} else {
			EditToggleLineComments(L"#", false);
		}
		break;

	case NP2LEX_HTML:
	case NP2LEX_PHP:
	case NP2LEX_XML: {
		const int block = GetCurrentHtmlTextBlock(pLexCurrent->iLexer);
		switch (block) {
		case HtmlTextBlock_VBScript:
			EditToggleLineComments(L"'", false);
			break;

		case HtmlTextBlock_Python:
			EditToggleLineComments(L"#", false);
			break;

		case HtmlTextBlock_CDATA:
		case HtmlTextBlock_JavaScript:
		case HtmlTextBlock_PHP:
			EditToggleLineComments(L"//", false);
			break;

		default:
			break;
		}
	}
	break;

	case NP2LEX_INNOSETUP: {
		const int lineState = SciCall_GetLineState(SciCall_LineFromPosition(SciCall_GetSelectionStart()));
		if (lineState & InnoLineStateCodeSection) {
			EditToggleLineComments(L"//", false);
		} else {
			EditToggleLineComments(L";", false);
		}
	}
	break;

	case NP2LEX_MATLAB:
		if (np2LexLangIndex == IDM_LEXER_SCILAB) {
			EditToggleLineComments(L"//", false);
		} else {
			EditToggleLineComments(L"%", false);
		}
		break;

//CommentLine++Autogenerated -- start of section automatically generated
	case NP2LEX_ABAQUS:
		EditToggleLineComments(L"**", false);
		break;

	case NP2LEX_ACTIONSCRIPT:
	case NP2LEX_ASYMPTOTE:
	case NP2LEX_BLOCKDIAG:
	case NP2LEX_CIL:
	case NP2LEX_CPP:
	case NP2LEX_CSHARP:
	case NP2LEX_CSS:
	case NP2LEX_DART:
	case NP2LEX_DLANG:
	case NP2LEX_FSHARP:
	case NP2LEX_GO:
	case NP2LEX_GRADLE:
	case NP2LEX_GRAPHVIZ:
	case NP2LEX_GROOVY:
	case NP2LEX_HAXE:
	case NP2LEX_JAVA:
	case NP2LEX_JAVASCRIPT:
	case NP2LEX_JSON:
	case NP2LEX_KOTLIN:
	case NP2LEX_PASCAL:
	case NP2LEX_RESOURCESCRIPT:
	case NP2LEX_RUST:
	case NP2LEX_SCALA:
	case NP2LEX_SWIFT:
	case NP2LEX_TYPESCRIPT:
	case NP2LEX_VERILOG:
		EditToggleLineComments(L"//", false);
		break;

	case NP2LEX_APDL:
	case NP2LEX_FORTRAN:
		EditToggleLineComments(L"!", false);
		break;

	case NP2LEX_AUTOHOTKEY:
	case NP2LEX_AUTOIT3:
	case NP2LEX_INI:
	case NP2LEX_LISP:
	case NP2LEX_LLVM:
	case NP2LEX_REBOL:
		EditToggleLineComments(L";", false);
		break;

	case NP2LEX_AVISYNTH:
	case NP2LEX_AWK:
	case NP2LEX_CMAKE:
	case NP2LEX_COFFEESCRIPT:
	case NP2LEX_CONFIG:
	case NP2LEX_GN:
	case NP2LEX_JAMFILE:
	case NP2LEX_JULIA:
	case NP2LEX_MAKEFILE:
	case NP2LEX_NSIS:
	case NP2LEX_PERL:
	case NP2LEX_POWERSHELL:
	case NP2LEX_PYTHON:
	case NP2LEX_RLANG:
	case NP2LEX_RUBY:
	case NP2LEX_SMALI:
	case NP2LEX_TCL:
	case NP2LEX_TOML:
	case NP2LEX_YAML:
		EditToggleLineComments(L"#", false);
		break;

	case NP2LEX_BATCH:
		EditToggleLineComments(L"@rem ", true);
		break;

	case NP2LEX_LATEX:
		EditToggleLineComments(L"%", false);
		break;

	case NP2LEX_LUA:
	case NP2LEX_VHDL:
		EditToggleLineComments(L"--", false);
		break;

	case NP2LEX_SQL:
		EditToggleLineComments(L"-- ", false);
		break;

	case NP2LEX_TEXINFO:
		EditToggleLineComments(L"@c ", false);
		break;

	case NP2LEX_VBSCRIPT:
	case NP2LEX_VISUALBASIC:
		EditToggleLineComments(L"\'", false);
		break;

	case NP2LEX_VIM:
		EditToggleLineComments(L"\"", false);
		break;

	case NP2LEX_WASM:
		EditToggleLineComments(L";;", false);
		break;

//CommentLine--Autogenerated -- end of section automatically generated
	}
}

void EditEncloseSelectionNewLine(LPCWSTR pwszOpen, LPCWSTR pwszClose) {
	WCHAR start[64] = L"";
	WCHAR end[64] = L"";
	const int iEOLMode = SciCall_GetEOLMode();
	LPCWSTR lineEnd = (iEOLMode == SC_EOL_LF) ? L"\n" : ((iEOLMode == SC_EOL_CR) ? L"\r" : L"\r\n");

	Sci_Position pos = SciCall_GetSelectionStart();
	Sci_Line line = SciCall_LineFromPosition(pos);
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
	EditEncloseSelection(start, end);
}

void EditToggleCommentBlock(void) {
	switch (pLexCurrent->rid) {
	case NP2LEX_BLOCKDIAG:
	case NP2LEX_GRAPHVIZ: {
		const int lineState = SciCall_GetLineState(SciCall_LineFromPosition(SciCall_GetSelectionStart()));
		if (lineState) {
			EditEncloseSelection(L"<!--", L"-->");
		} else {
			EditEncloseSelection(L"/*", L"*/");
		}
	} break;

	case NP2LEX_HTML:
	case NP2LEX_PHP:
	case NP2LEX_XML: {
		const int block = GetCurrentHtmlTextBlock(pLexCurrent->iLexer);
		switch (block) {
		case HtmlTextBlock_Tag:
			EditEncloseSelection(L"<!--", L"-->");
			break;

		case HtmlTextBlock_CDATA:
		case HtmlTextBlock_JavaScript:
		case HtmlTextBlock_PHP:
		case HtmlTextBlock_CSS:
			EditEncloseSelection(L"/*", L"*/");
			break;

		case HtmlTextBlock_SGML:
			// A brief SGML tutorial
			// https://www.w3.org/TR/WD-html40-970708/intro/sgmltut.html
			EditEncloseSelection(L"--", L"--");
			break;

		default:
			break;
		}
	}
	break;

	case NP2LEX_INNOSETUP: {
		const int lineState = SciCall_GetLineState(SciCall_LineFromPosition(SciCall_GetSelectionStart()));
		if (lineState &  InnoLineStateCodeSection) {
			EditEncloseSelection(L"{", L"}");
		} else if (lineState & InnoLineStatePreprocessor) {
			EditEncloseSelection(L"/*", L"*/");
		}
	}
	break;

	case NP2LEX_MATLAB:
		if (np2LexLangIndex == IDM_LEXER_SCILAB) {
			EditEncloseSelection(L"/*", L"*/");
		} else {
			EditEncloseSelectionNewLine(L"%{", L"%}");
		}
		break;

//CommentBlock++Autogenerated -- start of section automatically generated
	case NP2LEX_ACTIONSCRIPT:
	case NP2LEX_ASM:
	case NP2LEX_ASYMPTOTE:
	case NP2LEX_AUTOHOTKEY:
	case NP2LEX_AVISYNTH:
	case NP2LEX_CIL:
	case NP2LEX_CPP:
	case NP2LEX_CSHARP:
	case NP2LEX_CSS:
	case NP2LEX_DART:
	case NP2LEX_DLANG:
	case NP2LEX_GO:
	case NP2LEX_GRADLE:
	case NP2LEX_GROOVY:
	case NP2LEX_HAXE:
	case NP2LEX_JAVA:
	case NP2LEX_JAVASCRIPT:
	case NP2LEX_JSON:
	case NP2LEX_KOTLIN:
	case NP2LEX_NSIS:
	case NP2LEX_RESOURCESCRIPT:
	case NP2LEX_RUST:
	case NP2LEX_SCALA:
	case NP2LEX_SQL:
	case NP2LEX_SWIFT:
	case NP2LEX_TYPESCRIPT:
	case NP2LEX_VERILOG:
	case NP2LEX_VHDL:
		EditEncloseSelection(L"/*", L"*/");
		break;

	case NP2LEX_AUTOIT3:
		EditEncloseSelectionNewLine(L"#cs", L"#ce");
		break;

	case NP2LEX_CMAKE:
		EditEncloseSelection(L"#[[", L"]]");
		break;

	case NP2LEX_COFFEESCRIPT:
		EditEncloseSelection(L"###", L"###");
		break;

	case NP2LEX_FORTRAN:
		EditEncloseSelectionNewLine(L"#if 0", L"#endif");
		break;

	case NP2LEX_FSHARP:
		EditEncloseSelection(L"(*", L"*)");
		break;

	case NP2LEX_JAMFILE:
	case NP2LEX_LISP:
		EditEncloseSelection(L"#|", L"|#");
		break;

	case NP2LEX_JULIA:
		EditEncloseSelection(L"#=", L"=#");
		break;

	case NP2LEX_LATEX:
		EditEncloseSelectionNewLine(L"\\begin{comment}", L"\\end{comment}");
		break;

	case NP2LEX_LUA:
		EditEncloseSelection(L"--[[", L"--]]");
		break;

	case NP2LEX_MARKDOWN:
		EditEncloseSelection(L"<!--", L"-->");
		break;

	case NP2LEX_PASCAL:
		EditEncloseSelection(L"{", L"}");
		break;

	case NP2LEX_POWERSHELL:
		EditEncloseSelection(L"<#", L"#>");
		break;

	case NP2LEX_REBOL:
		EditEncloseSelectionNewLine(L"comment {", L"}");
		break;

	case NP2LEX_RLANG:
		EditEncloseSelectionNewLine(L"if (FALSE) {", L"}");
		break;

	case NP2LEX_TCL:
		EditEncloseSelectionNewLine(L"if (0) {", L"}");
		break;

	case NP2LEX_WASM:
		EditEncloseSelection(L"(;", L";)");
		break;

//CommentBlock--Autogenerated -- end of section automatically generated
	}
}

// see Style_SniffShebang() in Styles.c
void EditInsertScriptShebangLine(void) {
	const char *prefix = "#!/usr/bin/env ";
	const char *name = NULL;

	switch (pLexCurrent->rid) {
	case NP2LEX_BASH:
		switch (np2LexLangIndex) {
		case IDM_LEXER_CSHELL:
			prefix = "#!/bin/csh";
			break;

		case IDM_LEXER_M4:
			name = "m4";
			break;

		default:
			prefix = "#!/bin/bash";
			break;
		}
		break;

//ScriptShebang++Autogenerated -- start of section automatically generated
	case NP2LEX_AWK:
		name = "awk";
		break;

	case NP2LEX_GROOVY:
		name = "groovy";
		break;

	case NP2LEX_JAVASCRIPT:
		name = "node";
		break;

	case NP2LEX_LUA:
		name = "lua";
		break;

	case NP2LEX_PERL:
		name = "perl";
		break;

	case NP2LEX_PHP:
		name = "php";
		break;

	case NP2LEX_PYTHON:
		name = "python3";
		break;

	case NP2LEX_RLANG:
		name = "Rscript";
		break;

	case NP2LEX_RUBY:
		name = "ruby";
		break;

	case NP2LEX_SCALA:
		name = "scala";
		break;

	case NP2LEX_TCL:
		name = "wish";
		break;

//ScriptShebang--Autogenerated -- end of section automatically generated
	}

	char line[128];
	strcpy(line, prefix);
	if (name != NULL) {
		strcat(line, name);
	}

	const Sci_Position iCurrentPos = SciCall_GetCurrentPos();
	if (iCurrentPos == 0 && (name != NULL || pLexCurrent->iLexer == SCLEX_BASH)) {
		const int iEOLMode = SciCall_GetEOLMode();
		LPCSTR lineEnd = (iEOLMode == SC_EOL_LF) ? "\n" : ((iEOLMode == SC_EOL_CR) ? "\r" : "\r\n");
		strcat(line, lineEnd);
	}
	SciCall_ReplaceSel(line);
}

void EditShowCallTips(Sci_Position position) {
	const Sci_Line iLine = SciCall_LineFromPosition(position);
	const Sci_Position iDocLen = SciCall_GetLineLength(iLine);
	char *pLine = (char *)NP2HeapAlloc(iDocLen + 1);
	SciCall_GetLine(iLine, pLine);
	StrTrimA(pLine, " \t\r\n");
	char *text = (char *)NP2HeapAlloc(iDocLen + 1 + 128);
#if defined(_WIN64)
	sprintf(text, "ShowCallTips(%" PRId64 ", %" PRId64 ", %" PRId64 ")\n\n\002%s", iLine + 1, position, iDocLen, pLine);
#else
	sprintf(text, "ShowCallTips(%d, %d, %d)\n\n\002%s", (int)(iLine + 1), (int)position, (int)iDocLen, pLine);
#endif
	SciCall_CallTipUseStyle(fvCurFile.iTabWidth);
	SciCall_CallTipShow(position, text);
	NP2HeapFree(pLine);
	NP2HeapFree(text);
}

void InitAutoCompletionCache(LPCEDITLEXER pLex) {
	np2_LexKeyword = NULL;
	memset(CharacterPrefixMask, 0, sizeof(CharacterPrefixMask));
	memset(RawStringStyleMask, 0, sizeof(RawStringStyleMask));
	memset(GenericTypeStyleMask, 0, sizeof(GenericTypeStyleMask));
	memset(IgnoreWordStyleMask, 0, sizeof(IgnoreWordStyleMask));
	memcpy(CurrentWordCharSet, DefaultWordCharSet, sizeof(DefaultWordCharSet));
	//CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));

	switch (pLex->rid) {
//Cache++Autogenerated -- start of section automatically generated
	case NP2LEX_2NDTEXTFILE:
	case NP2LEX_ANSI:
	case NP2LEX_BLOCKDIAG:
	case NP2LEX_CSV:
	case NP2LEX_GRAPHVIZ:
	case NP2LEX_LISP:
	case NP2LEX_SMALI:
	case NP2LEX_TEXTFILE:
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		break;

	case NP2LEX_ACTIONSCRIPT:
	case NP2LEX_TYPESCRIPT:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		GenericTypeStyleMask[SCE_JS_CLASS >> 5] |= (1U << (SCE_JS_CLASS & 31));
		GenericTypeStyleMask[SCE_JS_INTERFACE >> 5] |= (1U << (SCE_JS_INTERFACE & 31));
		GenericTypeStyleMask[SCE_JS_ENUM >> 5] |= (1U << (SCE_JS_ENUM & 31));
		GenericTypeStyleMask[SCE_JS_WORD2 >> 5] |= (1U << (SCE_JS_WORD2 & 31));
		break;

	case NP2LEX_ASM:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['%' >> 5] |= (1 << ('%' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		break;

	case NP2LEX_AUTOIT3:
	case NP2LEX_JAVASCRIPT:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		break;

	case NP2LEX_AWK:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		break;

	case NP2LEX_BASH:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		RawStringStyleMask[SCE_SH_STRING_SQ >> 5] |= (1U << (SCE_SH_STRING_SQ & 31));
		break;

	case NP2LEX_BATCH:
	case NP2LEX_GN:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		break;

	case NP2LEX_CIL:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		break;

	case NP2LEX_CPP:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		CharacterPrefixMask['L' >> 5] |= (1 << ('L' & 31));
		CharacterPrefixMask['U' >> 5] |= (1 << ('U' & 31));
		CharacterPrefixMask['u' >> 5] |= (1 << ('u' & 31));
		RawStringStyleMask[SCE_C_STRINGRAW >> 5] |= (1U << (SCE_C_STRINGRAW & 31));
		RawStringStyleMask[SCE_C_COMMENTDOC_TAG >> 5] |= (1U << (SCE_C_COMMENTDOC_TAG & 31));
		GenericTypeStyleMask[SCE_C_CLASS >> 5] |= (1U << (SCE_C_CLASS & 31));
		GenericTypeStyleMask[SCE_C_INTERFACE >> 5] |= (1U << (SCE_C_INTERFACE & 31));
		GenericTypeStyleMask[SCE_C_STRUCT >> 5] |= (1U << (SCE_C_STRUCT & 31));
		GenericTypeStyleMask[SCE_C_WORD2 >> 5] |= (1U << (SCE_C_WORD2 & 31));
		IgnoreWordStyleMask[SCE_C_WORD >> 5] |= (1U << (SCE_C_WORD & 31));
		IgnoreWordStyleMask[SCE_C_WORD2 >> 5] |= (1U << (SCE_C_WORD2 & 31));
		IgnoreWordStyleMask[SCE_C_PREPROCESSOR >> 5] |= (1U << (SCE_C_PREPROCESSOR & 31));
		IgnoreWordStyleMask[SCE_C_ASM_REGISTER >> 5] |= (1U << (SCE_C_ASM_REGISTER & 31));
		IgnoreWordStyleMask[SCE_C_ASM_INSTRUCTION >> 5] |= (1U << (SCE_C_ASM_INSTRUCTION & 31));
		np2_LexKeyword = &kwDoxyDoc;
		break;

	case NP2LEX_CSHARP:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		RawStringStyleMask[SCE_CSHARP_VERBATIM_STRING >> 5] |= (1U << (SCE_CSHARP_VERBATIM_STRING & 31));
		RawStringStyleMask[SCE_CSHARP_INTERPOLATED_VERBATIM_STRING >> 5] |= (1U << (SCE_CSHARP_INTERPOLATED_VERBATIM_STRING & 31));
		RawStringStyleMask[SCE_CSHARP_RAWSTRING_SL >> 5] |= (1U << (SCE_CSHARP_RAWSTRING_SL & 31));
		RawStringStyleMask[SCE_CSHARP_INTERPOLATED_RAWSTRING_SL >> 5] |= (1U << (SCE_CSHARP_INTERPOLATED_RAWSTRING_SL & 31));
		RawStringStyleMask[SCE_CSHARP_RAWSTRING_ML >> 5] |= (1U << (SCE_CSHARP_RAWSTRING_ML & 31));
		RawStringStyleMask[SCE_CSHARP_INTERPOLATED_RAWSTRING_ML >> 5] |= (1U << (SCE_CSHARP_INTERPOLATED_RAWSTRING_ML & 31));
		GenericTypeStyleMask[SCE_CSHARP_CLASS >> 5] |= (1U << (SCE_CSHARP_CLASS & 31));
		GenericTypeStyleMask[SCE_CSHARP_INTERFACE >> 5] |= (1U << (SCE_CSHARP_INTERFACE & 31));
		GenericTypeStyleMask[SCE_CSHARP_STRUCT >> 5] |= (1U << (SCE_CSHARP_STRUCT & 31));
		GenericTypeStyleMask[SCE_CSHARP_ENUM >> 5] |= (1U << (SCE_CSHARP_ENUM & 31));
		GenericTypeStyleMask[SCE_CSHARP_WORD2 >> 5] |= (1U << (SCE_CSHARP_WORD2 & 31));
		break;

	case NP2LEX_CSS:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		break;

	case NP2LEX_DART:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		CharacterPrefixMask['r' >> 5] |= (1 << ('r' & 31));
		RawStringStyleMask[SCE_DART_RAWSTRING_SQ >> 5] |= (1U << (SCE_DART_RAWSTRING_SQ & 31));
		RawStringStyleMask[SCE_DART_RAWSTRING_DQ >> 5] |= (1U << (SCE_DART_RAWSTRING_DQ & 31));
		RawStringStyleMask[SCE_DART_TRIPLE_RAWSTRING_SQ >> 5] |= (1U << (SCE_DART_TRIPLE_RAWSTRING_SQ & 31));
		RawStringStyleMask[SCE_DART_TRIPLE_RAWSTRING_DQ >> 5] |= (1U << (SCE_DART_TRIPLE_RAWSTRING_DQ & 31));
		GenericTypeStyleMask[SCE_DART_CLASS >> 5] |= (1U << (SCE_DART_CLASS & 31));
		GenericTypeStyleMask[SCE_DART_ENUM >> 5] |= (1U << (SCE_DART_ENUM & 31));
		GenericTypeStyleMask[SCE_DART_WORD2 >> 5] |= (1U << (SCE_DART_WORD2 & 31));
		break;

	case NP2LEX_DLANG:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		RawStringStyleMask[SCE_D_RAWSTRING >> 5] |= (1U << (SCE_D_RAWSTRING & 31));
		RawStringStyleMask[SCE_D_STRING_BT >> 5] |= (1U << (SCE_D_STRING_BT & 31));
		break;

	case NP2LEX_FORTRAN:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['%' >> 5] |= (1 << ('%' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CharacterPrefixMask['B' >> 5] |= (1 << ('B' & 31));
		CharacterPrefixMask['O' >> 5] |= (1 << ('O' & 31));
		CharacterPrefixMask['Z' >> 5] |= (1 << ('Z' & 31));
		CharacterPrefixMask['b' >> 5] |= (1 << ('b' & 31));
		CharacterPrefixMask['o' >> 5] |= (1 << ('o' & 31));
		CharacterPrefixMask['z' >> 5] |= (1 << ('z' & 31));
		break;

	case NP2LEX_FSHARP:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		np2_LexKeyword = &kwNETDoc;
		break;

	case NP2LEX_GO:
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		RawStringStyleMask[SCE_GO_RAW_STRING >> 5] |= (1U << (SCE_GO_RAW_STRING & 31));
		break;

	case NP2LEX_GRADLE:
	case NP2LEX_GROOVY:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		GenericTypeStyleMask[SCE_GROOVY_CLASS >> 5] |= (1U << (SCE_GROOVY_CLASS & 31));
		GenericTypeStyleMask[SCE_GROOVY_INTERFACE >> 5] |= (1U << (SCE_GROOVY_INTERFACE & 31));
		GenericTypeStyleMask[SCE_GROOVY_TRAIT >> 5] |= (1U << (SCE_GROOVY_TRAIT & 31));
		GenericTypeStyleMask[SCE_GROOVY_ENUM >> 5] |= (1U << (SCE_GROOVY_ENUM & 31));
		break;

	case NP2LEX_HAXE:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		GenericTypeStyleMask[SCE_HAXE_CLASS >> 5] |= (1U << (SCE_HAXE_CLASS & 31));
		GenericTypeStyleMask[SCE_HAXE_INTERFACE >> 5] |= (1U << (SCE_HAXE_INTERFACE & 31));
		GenericTypeStyleMask[SCE_HAXE_ENUM >> 5] |= (1U << (SCE_HAXE_ENUM & 31));
		break;

	case NP2LEX_HTML:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		break;

	case NP2LEX_INNOSETUP:
	case NP2LEX_VISUALBASIC:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		break;

	case NP2LEX_JAVA:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		GenericTypeStyleMask[SCE_JAVA_CLASS >> 5] |= (1U << (SCE_JAVA_CLASS & 31));
		GenericTypeStyleMask[SCE_JAVA_INTERFACE >> 5] |= (1U << (SCE_JAVA_INTERFACE & 31));
		GenericTypeStyleMask[SCE_JAVA_ENUM >> 5] |= (1U << (SCE_JAVA_ENUM & 31));
		break;

	case NP2LEX_JULIA:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		RawStringStyleMask[SCE_JULIA_RAWSTRING >> 5] |= (1U << (SCE_JULIA_RAWSTRING & 31));
		RawStringStyleMask[SCE_JULIA_TRIPLE_RAWSTRING >> 5] |= (1U << (SCE_JULIA_TRIPLE_RAWSTRING & 31));
		break;

	case NP2LEX_KOTLIN:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		RawStringStyleMask[SCE_KOTLIN_RAWSTRING >> 5] |= (1U << (SCE_KOTLIN_RAWSTRING & 31));
		GenericTypeStyleMask[SCE_KOTLIN_CLASS >> 5] |= (1U << (SCE_KOTLIN_CLASS & 31));
		GenericTypeStyleMask[SCE_KOTLIN_INTERFACE >> 5] |= (1U << (SCE_KOTLIN_INTERFACE & 31));
		GenericTypeStyleMask[SCE_KOTLIN_ENUM >> 5] |= (1U << (SCE_KOTLIN_ENUM & 31));
		break;

	case NP2LEX_LLVM:
	case NP2LEX_WASM:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['%' >> 5] |= (1 << ('%' & 31));
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		break;

	case NP2LEX_MAKEFILE:
	case NP2LEX_NSIS:
		CurrentWordCharSet['!' >> 5] |= (1 << ('!' & 31));
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		break;

	case NP2LEX_PERL:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		RawStringStyleMask[SCE_PL_STRING_SQ >> 5] |= (1U << (SCE_PL_STRING_SQ & 31));
		break;

	case NP2LEX_PHP:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		RawStringStyleMask[SCE_PHP_STRING_SQ >> 5] |= (1U << (SCE_PHP_STRING_SQ & 31));
		RawStringStyleMask[SCE_PHP_NOWDOC >> 5] |= (1U << (SCE_PHP_NOWDOC & 31));
		break;

	case NP2LEX_POWERSHELL:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		CurrentWordCharSet['?' >> 5] |= (1U << ('?' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		RawStringStyleMask[SCE_POWERSHELL_STRING_SQ >> 5] |= (1U << (SCE_POWERSHELL_STRING_SQ & 31));
		RawStringStyleMask[SCE_POWERSHELL_HERE_STRING_SQ >> 5] |= (1U << (SCE_POWERSHELL_HERE_STRING_SQ & 31));
		break;

	case NP2LEX_PYTHON:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		CharacterPrefixMask['B' >> 5] |= (1 << ('B' & 31));
		CharacterPrefixMask['F' >> 5] |= (1 << ('F' & 31));
		CharacterPrefixMask['R' >> 5] |= (1 << ('R' & 31));
		CharacterPrefixMask['U' >> 5] |= (1 << ('U' & 31));
		CharacterPrefixMask['b' >> 5] |= (1 << ('b' & 31));
		CharacterPrefixMask['f' >> 5] |= (1 << ('f' & 31));
		CharacterPrefixMask['r' >> 5] |= (1 << ('r' & 31));
		CharacterPrefixMask['u' >> 5] |= (1 << ('u' & 31));
		RawStringStyleMask[SCE_PY_RAWSTRING_SQ >> 5] |= (1U << (SCE_PY_RAWSTRING_SQ & 31));
		RawStringStyleMask[SCE_PY_RAWSTRING_DQ >> 5] |= (1U << (SCE_PY_RAWSTRING_DQ & 31));
		RawStringStyleMask[SCE_PY_TRIPLE_RAWSTRING_SQ >> 5] |= (1U << (SCE_PY_TRIPLE_RAWSTRING_SQ & 31));
		RawStringStyleMask[SCE_PY_TRIPLE_RAWSTRING_DQ >> 5] |= (1U << (SCE_PY_TRIPLE_RAWSTRING_DQ & 31));
		RawStringStyleMask[SCE_PY_RAWFMTSTRING_SQ >> 5] |= (1U << (SCE_PY_RAWFMTSTRING_SQ & 31));
		RawStringStyleMask[SCE_PY_RAWFMTSTRING_DQ >> 5] |= (1U << (SCE_PY_RAWFMTSTRING_DQ & 31));
		RawStringStyleMask[SCE_PY_TRIPLE_RAWFMTSTRING_SQ >> 5] |= (1U << (SCE_PY_TRIPLE_RAWFMTSTRING_SQ & 31));
		RawStringStyleMask[SCE_PY_TRIPLE_RAWFMTSTRING_DQ >> 5] |= (1U << (SCE_PY_TRIPLE_RAWFMTSTRING_DQ & 31));
		RawStringStyleMask[SCE_PY_RAWBYTES_SQ >> 5] |= (1U << (SCE_PY_RAWBYTES_SQ & 31));
		RawStringStyleMask[SCE_PY_RAWBYTES_DQ >> 5] |= (1U << (SCE_PY_RAWBYTES_DQ & 31));
		RawStringStyleMask[SCE_PY_TRIPLE_RAWBYTES_SQ >> 5] |= (1U << (SCE_PY_TRIPLE_RAWBYTES_SQ & 31));
		RawStringStyleMask[SCE_PY_TRIPLE_RAWBYTES_DQ >> 5] |= (1U << (SCE_PY_TRIPLE_RAWBYTES_DQ & 31));
		IgnoreWordStyleMask[SCE_PY_WORD >> 5] |= (1U << (SCE_PY_WORD & 31));
		IgnoreWordStyleMask[SCE_PY_WORD2 >> 5] |= (1U << (SCE_PY_WORD2 & 31));
		IgnoreWordStyleMask[SCE_PY_BUILTIN_CONSTANT >> 5] |= (1U << (SCE_PY_BUILTIN_CONSTANT & 31));
		IgnoreWordStyleMask[SCE_PY_BUILTIN_FUNCTION >> 5] |= (1U << (SCE_PY_BUILTIN_FUNCTION & 31));
		IgnoreWordStyleMask[SCE_PY_ATTRIBUTE >> 5] |= (1U << (SCE_PY_ATTRIBUTE & 31));
		IgnoreWordStyleMask[SCE_PY_OBJECT_FUNCTION >> 5] |= (1U << (SCE_PY_OBJECT_FUNCTION & 31));
		break;

	case NP2LEX_REBOL:
		CurrentWordCharSet['!' >> 5] |= (1 << ('!' & 31));
		CurrentWordCharSet['&' >> 5] |= (1 << ('&' & 31));
		CurrentWordCharSet['*' >> 5] |= (1 << ('*' & 31));
		CurrentWordCharSet['+' >> 5] |= (1 << ('+' & 31));
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['=' >> 5] |= (1 << ('=' & 31));
		CurrentWordCharSet['?' >> 5] |= (1U << ('?' & 31));
		CurrentWordCharSet['~' >> 5] |= (1 << ('~' & 31));
		break;

	case NP2LEX_RESOURCESCRIPT:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CharacterPrefixMask['L' >> 5] |= (1 << ('L' & 31));
		CharacterPrefixMask['U' >> 5] |= (1 << ('U' & 31));
		CharacterPrefixMask['u' >> 5] |= (1 << ('u' & 31));
		RawStringStyleMask[SCE_C_STRINGRAW >> 5] |= (1U << (SCE_C_STRINGRAW & 31));
		RawStringStyleMask[SCE_C_COMMENTDOC_TAG >> 5] |= (1U << (SCE_C_COMMENTDOC_TAG & 31));
		IgnoreWordStyleMask[SCE_C_WORD >> 5] |= (1U << (SCE_C_WORD & 31));
		IgnoreWordStyleMask[SCE_C_WORD2 >> 5] |= (1U << (SCE_C_WORD2 & 31));
		IgnoreWordStyleMask[SCE_C_PREPROCESSOR >> 5] |= (1U << (SCE_C_PREPROCESSOR & 31));
		break;

	case NP2LEX_RLANG:
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CharacterPrefixMask['R' >> 5] |= (1 << ('R' & 31));
		CharacterPrefixMask['r' >> 5] |= (1 << ('r' & 31));
		RawStringStyleMask[SCE_R_RAWSTRING_SQ >> 5] |= (1U << (SCE_R_RAWSTRING_SQ & 31));
		RawStringStyleMask[SCE_R_RAWSTRING_DQ >> 5] |= (1U << (SCE_R_RAWSTRING_DQ & 31));
		break;

	case NP2LEX_RUBY:
		CurrentWordCharSet['!' >> 5] |= (1 << ('!' & 31));
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['?' >> 5] |= (1U << ('?' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		RawStringStyleMask[SCE_RB_STRING_SQ >> 5] |= (1U << (SCE_RB_STRING_SQ & 31));
		break;

	case NP2LEX_RUST:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		CharacterPrefixMask['b' >> 5] |= (1 << ('b' & 31));
		RawStringStyleMask[SCE_RUST_RAW_STRING >> 5] |= (1U << (SCE_RUST_RAW_STRING & 31));
		RawStringStyleMask[SCE_RUST_RAW_BYTESTRING >> 5] |= (1U << (SCE_RUST_RAW_BYTESTRING & 31));
		GenericTypeStyleMask[SCE_RUST_TYPE >> 5] |= (1U << (SCE_RUST_TYPE & 31));
		GenericTypeStyleMask[SCE_RUST_STRUCT >> 5] |= (1U << (SCE_RUST_STRUCT & 31));
		GenericTypeStyleMask[SCE_RUST_TRAIT >> 5] |= (1U << (SCE_RUST_TRAIT & 31));
		GenericTypeStyleMask[SCE_RUST_ENUMERATION >> 5] |= (1U << (SCE_RUST_ENUMERATION & 31));
		GenericTypeStyleMask[SCE_RUST_UNION >> 5] |= (1U << (SCE_RUST_UNION & 31));
		break;

	case NP2LEX_SCALA:
	case NP2LEX_TCL:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		break;

	case NP2LEX_SQL:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		CharacterPrefixMask['B' >> 5] |= (1 << ('B' & 31));
		CharacterPrefixMask['Q' >> 5] |= (1 << ('Q' & 31));
		CharacterPrefixMask['X' >> 5] |= (1 << ('X' & 31));
		CharacterPrefixMask['b' >> 5] |= (1 << ('b' & 31));
		CharacterPrefixMask['q' >> 5] |= (1 << ('q' & 31));
		CharacterPrefixMask['x' >> 5] |= (1 << ('x' & 31));
		IgnoreWordStyleMask[SCE_SQL_WORD >> 5] |= (1U << (SCE_SQL_WORD & 31));
		IgnoreWordStyleMask[SCE_SQL_WORD2 >> 5] |= (1U << (SCE_SQL_WORD2 & 31));
		IgnoreWordStyleMask[SCE_SQL_USER1 >> 5] |= (1U << (SCE_SQL_USER1 & 31));
		IgnoreWordStyleMask[SCE_SQL_HEX >> 5] |= (1U << (SCE_SQL_HEX & 31));
		IgnoreWordStyleMask[SCE_SQL_HEX2 >> 5] |= (1U << (SCE_SQL_HEX2 & 31));
		break;

	case NP2LEX_SWIFT:
		CurrentWordCharSet['#' >> 5] |= (1 << ('#' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['@' >> 5] |= (1 << ('@' & 31));
		GenericTypeStyleMask[SCE_SWIFT_CLASS >> 5] |= (1U << (SCE_SWIFT_CLASS & 31));
		GenericTypeStyleMask[SCE_SWIFT_STRUCT >> 5] |= (1U << (SCE_SWIFT_STRUCT & 31));
		GenericTypeStyleMask[SCE_SWIFT_PROTOCOL >> 5] |= (1U << (SCE_SWIFT_PROTOCOL & 31));
		GenericTypeStyleMask[SCE_SWIFT_ENUM >> 5] |= (1U << (SCE_SWIFT_ENUM & 31));
		break;

	case NP2LEX_VERILOG:
		CurrentWordCharSet['$' >> 5] |= (1 << ('$' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		CurrentWordCharSet['`' >> 5] |= (1 << ('`' & 31));
		break;

	case NP2LEX_VHDL:
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet['`' >> 5] |= (1 << ('`' & 31));
		RawStringStyleMask[SCE_VHDL_STRING >> 5] |= (1U << (SCE_VHDL_STRING & 31));
		break;

	case NP2LEX_VIM:
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		RawStringStyleMask[SCE_VIM_STRING_SQ >> 5] |= (1U << (SCE_VIM_STRING_SQ & 31));
		break;

	case NP2LEX_XML:
		CurrentWordCharSet['-' >> 5] |= (1 << ('-' & 31));
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		CurrentWordCharSet[':' >> 5] |= (1 << (':' & 31));
		break;

	default:
		CurrentWordCharSet['.' >> 5] |= (1 << ('.' & 31));
		break;

//Cache--Autogenerated -- end of section automatically generated
	}

	UpdateLexerExtraKeywords();
}
