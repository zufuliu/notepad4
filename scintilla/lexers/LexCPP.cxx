// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for C, C++, C#, Java, Rescouce Script, Asymptote, D, Objective C/C++, PHP
//! JavaScript, JScript, ActionScript, haXe, Groovy, Scala, Jamfile, AWK, IDL/ODL/AIDL

#include <cassert>
#include <cstring>
#include <cctype>

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"
//#include "SparseState.h"

using namespace Scintilla;

#define		LEX_CPP		1	// C/C++
#define		LEX_JAVA	2	// Java
#define		LEX_CS		3	// C#
#define		LEX_JS		4	// JavaScript
#define		LEX_RC		5	// Resouce Script
#define		LEX_IDL		6	// Interface Definition Language
#define		LEX_D		7	// D
#define		LEX_ASY		8	// Asymptote
#define		LEX_OBJC	10	// Objective C/C++
#define		LEX_AS		11	// ActionScript
#define		LEX_HX		12	// haXe
#define		LEX_GROOVY	13	// Groovy Script
#define		LEX_SCALA	14	// Scala Script
#define		LEX_GO		15	// Go
//#define		LEX_AIDL	27	// Android Interface Definition Language
#define		LEX_PHP		29
#define		LEX_AWK		51	// Awk
#define		LEX_JAM		52	// Jamfile

static constexpr bool _hasPreprocessor(int lex) noexcept { // #[space]preprocessor
	return lex == LEX_CPP || lex == LEX_CS || lex == LEX_RC || lex == LEX_OBJC;
}
static constexpr bool _hasAnotation(int lex) noexcept { // @anotation
	return lex == LEX_JAVA || lex == LEX_GROOVY || lex == LEX_SCALA;
}
static constexpr bool _hasRegex(int lex) noexcept { // Javascript /regex/
	return lex == LEX_JS || lex == LEX_GROOVY || lex == LEX_AS || lex == LEX_HX || lex == LEX_AWK;
}
static constexpr bool _hasTripleVerbatim(int lex) noexcept {
	return lex == LEX_JAVA || lex == LEX_GROOVY || lex == LEX_SCALA;
}
static constexpr bool _sharpComment(int lex) noexcept {
	return lex == LEX_AWK || lex == LEX_JAM;
}
static constexpr bool _hasXML(int lex) noexcept {
	return lex == LEX_JS || lex == LEX_AS || lex == LEX_SCALA;
}
static constexpr bool _squareBraceAfterType(int lex) noexcept {
	return lex == LEX_JAVA || lex == LEX_CS || lex == LEX_JS || lex == LEX_AS || lex == LEX_HX || lex == LEX_GROOVY || lex == LEX_SCALA;
}
static constexpr bool IsDStrFix(int ch) noexcept {
	return ch == 'c' || ch == 'w' || ch == 'd';
}
static constexpr bool _use2ndKeyword(int lex) noexcept {
	return lex == LEX_OBJC;
}
static constexpr bool _use2ndKeyword2(int lex) noexcept {
	return lex == LEX_CPP || lex == LEX_OBJC;
}
#define	strequ(str1, str2)	(!strcmp(str1, str2))
static constexpr bool IsSpaceEquiv(int state) noexcept {
	// including SCE_C_DEFAULT, SCE_C_COMMENT, SCE_C_COMMENTLINE
	// SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG, SCE_C_COMMENTDOC_TAG_XML
	return (state < SCE_C_IDENTIFIER) || (state == SCE_C_DNESTEDCOMMENT);
}
static constexpr bool IsEscapeChar(int ch) noexcept {
	return ch == '\\' || ch == '\'' || ch == '\"'
		|| ch == '$'; // PHP
}

/*static const char* const cppWordLists[] = {
	"Primary keywords",		// SCE_C_WORD
	"Type keywords",		// SCE_C_WORD2
	"Preprocessor",			// SCE_C_PREPROCESSOR	#preprocessor
	"Directive",			// SCE_C_DIRECTIVE		@directive @anotation
	// global
	"Attribute",			// SCE_C_ATTRIBUTE		[attribute]
	"Class",				// SCE_C_CLASS
	"Interface",			// SCE_C_INTERFACE
	"Enumeration",			// SCE_C_ENUMERATION
	"Constant",				// SCE_C_CONSTANT
	"2nd Language Keyword"		// SCE_C_2NDWORD
	"2nd Language Type Keyword"	// SCE_C_2NDWORD2
	"Inline Asm Instruction"	// SCE_C_ASM_INSTRUCTION
	"Inline Asm Register"		// SCE_C_ASM_REGISTER
	0
};*/

#define LEX_BLOCK_MASK_ASM		0x0001
#define LEX_BLOCK_MASK_DEFINE	0x0002
#define LEX_BLOCK_MASK_TYPEDEF	0x0004
#define LEX_BLOCK_UMASK_ASM		0xFFFE
#define LEX_BLOCK_UMASK_ALL		0xFFFD

#define DOC_TAG_AT			1	/// @param \ref
#define DOC_TAG_INLINE_AT	2	/// {@code x+y}
#define DOC_TAG_OPEN_XML	3	/// <param name="path">file path
#define DOC_TAG_CLOSE_XML	4	/// </param>

static void ColouriseCppDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	const WordList &keywords = *keywordLists[0];
	const WordList &keywords2 = *keywordLists[1];
	const WordList &keywords3 = *keywordLists[2];
	const WordList &keywords4 = *keywordLists[3];
	// global
	const WordList &kwAttribute = *keywordLists[4];
	const WordList &kwClass = *keywordLists[5];
	const WordList &kwInterface = *keywordLists[6];
	const WordList &kwEnumeration = *keywordLists[7];
	const WordList &kwConstant = *keywordLists[8];
	// 2nd
	const WordList &kw2ndKeyword = *keywordLists[9];
	const WordList &kw2ndKeyword2 = *keywordLists[10];
	const WordList &kwAsmInstruction = *keywordLists[11];
	const WordList &kwAsmRegister = *keywordLists[12];

	static bool isObjCSource = false;
	const int lexType = styler.GetPropertyInt("lexer.lang.type", LEX_CPP);
	if (lexType == LEX_PHP && startPos == 0) {
		initStyle = SCE_C_XML_DEFAULT;
	}

	Sci_Position lineCurrent = styler.GetLine(startPos);
	const int curLineState = (lineCurrent > 0) ? styler.GetLineState(lineCurrent - 1) : 0;
	int lineState = (curLineState >> 24);
	int numCBrace = (curLineState >> 18) & 0x3F;
	int numSBrace = (curLineState >> 13) & 0x1F;
	int numRBrace = (curLineState >> 8) & 0x1F;
	int curNcLevel = (curLineState >> 4) & 0x0F;
	int numDTSBrace = (curLineState) & 0x0F;
#define _MakeState() ((lineState << 24)|(numCBrace << 18)|(numSBrace << 13)|(numRBrace << 8)|(curNcLevel << 4)|numDTSBrace)
#define _UpdateLineState()	styler.SetLineState(lineCurrent, _MakeState())
#define _UpdateCurLineState() lineCurrent = styler.GetLine(sc.currentPos); \
								styler.SetLineState(lineCurrent, _MakeState())
	static char heredoc[256];
	static Sci_PositionU heredoc_len;
	int outerStyle = SCE_C_DEFAULT;
	int varType = 0;
	int docTagType = 0;

	int chPrevNonWhite = ' ';
	int visibleChars = 0;
	bool isTypeDefine = false;
	bool lastWordWasUUID = false;
	bool lastWordWasGoto = false;
	bool lastWordWasAsm = false;
	bool lastWordWasAttr = false;
	int lastPPDefineWord = 0;
	bool continuationLine = false;
	bool isPragmaPreprocessor = false;
	bool isPreprocessorWord = false;
	bool isIncludePreprocessor = false;
	bool isMessagePreprocessor = false;
	bool followsReturn = false;
	bool followsPostfixOperator = false;
	bool isTripleSingle = false;
	bool isAssignStmt = false;
	bool inRERange = false;

	if (initStyle == SCE_C_COMMENTLINE || initStyle == SCE_C_COMMENTLINEDOC || initStyle == SCE_C_PREPROCESSOR) {
		// Set continuationLine if last character of previous line is '\'
		if (lineCurrent > 0) {
			const int chBack = styler.SafeGetCharAt(startPos - 1);
			const int chBack2 = styler.SafeGetCharAt(startPos - 2);
			int lineEndChar = '!';
			if (chBack2 == '\r' && chBack == '\n') {
				lineEndChar = styler.SafeGetCharAt(startPos - 3);
			} else if (chBack == '\n' || chBack == '\r') {
				lineEndChar = chBack2;
			}
			continuationLine = lineEndChar == '\\';
		}
	}

	// look back to set chPrevNonWhite properly for better regex colouring
	if (startPos > 0) {
		Sci_Position back = startPos;
		while (--back && IsSpaceEquiv(styler.StyleAt(back)));

		if (styler.StyleAt(back) == SCE_C_OPERATOR) {
			chPrevNonWhite = styler.SafeGetCharAt(back);
		}
	}

	StyleContext sc(startPos, length, initStyle, styler);
	if (startPos == 0 && sc.Match('#', '!')) {
		// Shell Shebang at beginning of file
		sc.SetState(SCE_C_COMMENTLINE);
	}

	for (; sc.More(); sc.Forward()) {

		if (sc.atLineStart) {
			if (sc.state == SCE_C_STRING || sc.state == SCE_C_CHARACTER) {
				// Prevent SCE_C_STRINGEOL from leaking back to previous line which
				// ends with a line continuation by locking in the state upto this position.
				sc.SetState(sc.state);
			}
			// Reset states to begining of colourise so no surprises
			// if different sets of lines lexed.
			visibleChars = 0;
			lastWordWasUUID = false;
			lastWordWasGoto = false;
			lastPPDefineWord = 0;
			followsReturn = false;
			followsPostfixOperator = false;

			isPreprocessorWord = false;
			isPragmaPreprocessor = false;
			isIncludePreprocessor = false;
			isMessagePreprocessor = false;
			if (!(lexType & LEX_AWK)) {
				inRERange = false;
			}

			if (!continuationLine) {
				lineState &= LEX_BLOCK_UMASK_ALL;
			}
			_UpdateCurLineState();
		}

		if (sc.atLineEnd) {
			_UpdateLineState();
			lineCurrent++;
		}

		// Determine if the current state should terminate.
		switch (sc.state) {
		case SCE_C_OPERATOR:
			sc.SetState(SCE_C_DEFAULT);
			break;
		case SCE_C_NUMBER:
			if (!iswordstart(sc.ch)) { //
				if ((sc.ch == '+' || sc.ch == '-') && (
					(sc.chPrev == 'E' || sc.chPrev == 'e') || (sc.chPrev == 'P' || sc.chPrev == 'p'))) {
					sc.Forward();
				} else if (sc.ch == '.' && sc.chNext != '.') {
					sc.ForwardSetState(SCE_C_DEFAULT);
				} else if (sc.ch == '\'' && lexType == LEX_CPP) {
					sc.Forward();
				} else {
					sc.SetState(SCE_C_DEFAULT);
				}
			}
			break;

		case SCE_C_IDENTIFIER:
			if (!(iswordstart(sc.ch) || (sc.ch == '-' && (lexType == LEX_JAM)))) {
				char s[256];
				if (lexType == LEX_PHP)
					sc.GetCurrentLowered(s, sizeof(s));
				else
					sc.GetCurrent(s, sizeof(s));

				followsReturn = false;
				//int len = static_cast<int>(strlen(s));
				//bool pps = (len > 4 && s[0] == '_' && s[1] == '_' && s[len - 1] == '_' && s[len - 2] == '_');
				//char tu[256] = {0};
				//if (pps)
				//	strncpy(tu, s + 2, len - 4);
				// __attribute__()
				const bool hasAttr = (lexType == LEX_CPP || lexType == LEX_OBJC || isObjCSource || lexType == LEX_CS);
				const bool mayAttr = lastWordWasAttr && (numRBrace > 0 || (lineState & LEX_BLOCK_MASK_DEFINE));
				const bool mayCSAttr = (lexType == LEX_CS) && numSBrace == 1 && numRBrace == 0;
				const int nextChar = sc.GetNextNSChar();

				if (lastPPDefineWord) {
					if (lastPPDefineWord == 2 && strcmp(s, "defined") == 0)
						sc.ChangeState(SCE_C_WORD);
					else if (sc.ch == '(')
						sc.ChangeState(SCE_C_MACRO2);
					else
						sc.ChangeState(SCE_C_MACRO);
					if (lastPPDefineWord != 2)
						lastPPDefineWord = 0;
				} else if (lexType == LEX_CS && isAssignStmt && keywords4.InList(s)) {
					sc.ChangeState(SCE_C_DIRECTIVE);
				} else if ((lineState & LEX_BLOCK_MASK_ASM) && kwAsmInstruction.InList(s)) {
					sc.ChangeState(SCE_C_ASM_INSTRUCTION);
					lastWordWasGoto = MakeLowerCase(s[0]) == 'j';
				} else if ((lineState & LEX_BLOCK_MASK_ASM) && kwAsmRegister.InList(s)) {
					sc.ChangeState(SCE_C_ASM_REGISTER);
				} else if ((lexType == LEX_D) && (lineState & LEX_BLOCK_MASK_ASM) && kw2ndKeyword.InList(s)) {
					sc.ChangeState(SCE_C_2NDWORD);
				} else if (((s[0] == '#' || s[0] == '!') && keywords3.InList(s + 1))
					|| (isPreprocessorWord && keywords3.InList(s))) {
					sc.ChangeState(SCE_C_PREPROCESSOR);
					lastWordWasAttr = false;
					if (isPreprocessorWord) {
						isPreprocessorWord = false;
						char *ppw = s;
						if (s[0] == '#')
							ppw = s + 1;
						isPragmaPreprocessor = strequ(ppw, "pragma") || strequ(ppw, "line");
						isIncludePreprocessor = strcmp(ppw, "include") >= 0 || strequ(ppw, "import") || strequ(ppw, "using");
						isMessagePreprocessor = strequ(ppw, "error") || strequ(ppw, "warning") || strequ(ppw, "message") ||
							strequ(ppw, "region") || strequ(ppw, "endregion");
						if (strequ(ppw, "define")) {
							lineState |= LEX_BLOCK_MASK_DEFINE;
							lastPPDefineWord = 1;
						} else if (strstr(ppw, "if")) {
							lastPPDefineWord = 2;
						} else if (strequ(ppw, "undef")) {
							lastPPDefineWord = 3;
						}
					}
				} else if (isPragmaPreprocessor) {
					isPragmaPreprocessor = false;
					sc.ChangeState(SCE_C_PREPROCESSOR);
					isMessagePreprocessor = strequ(s, "region") || strequ(s, "endregion") || strequ(s, "mark");
				} else if ((!hasAttr || mayAttr || mayCSAttr) && kwAttribute.InList(s)) {
					sc.ChangeState(SCE_C_ATTRIBUTE);
				} else if (keywords.InList(s)) {
					sc.ChangeState(SCE_C_WORD);
					if (isAssignStmt && chPrevNonWhite == '=' && (strequ(s, "function") || strequ(s, "new"))) {
						isAssignStmt = false;
					}
					// asm __asm _asm
					lastWordWasAsm = strequ(s, "asm") || strequ(s, "__asm");
					lastWordWasUUID = strequ(s, "uuid");
					lastWordWasGoto = strequ(s, "goto") || strequ(s, "__label__") || strequ(s, "break") || strequ(s, "continue");
					followsReturn = strequ(s, "return");
					if (!isTypeDefine)
						isTypeDefine = strequ(s, "typedef");
					if (!lastWordWasAttr)
						lastWordWasAttr = strequ(s, "__declspec") || strequ(s, "__attribute__");
				} else if (keywords2.InList(s)) {
					sc.ChangeState(SCE_C_WORD2);
				} else if (s[0] == '@' && _hasAnotation(lexType)) {
					sc.ChangeState(SCE_C_DIRECTIVE);
					while (iswordchar(sc.ch))
						sc.Forward();
				} else if (lexType != LEX_CS && s[0] == '@' && keywords4.InList(s + 1)) {
					sc.ChangeState(SCE_C_DIRECTIVE);
					if (lexType == LEX_CPP || lexType == LEX_OBJC || isObjCSource) {
						if (!isObjCSource)
							isObjCSource = true;
						if (!lastWordWasAttr)
							lastWordWasAttr = strequ(s + 1, "property");
					}
				} else if (kwClass.InList(s)) {
					sc.ChangeState(SCE_C_CLASS);
				} else if (kwInterface.InList(s)) {
					sc.ChangeState(SCE_C_INTERFACE);
				} else if (lexType != LEX_PHP && kwEnumeration.InList(s)) {
					sc.ChangeState(SCE_C_ENUMERATION);
				} else if (nextChar == '(') {
					if (kwConstant.InListPrefixed(s, '('))
						sc.ChangeState(SCE_C_MACRO2);
					else
						sc.ChangeState(SCE_C_FUNCTION);
				} else if (kwConstant.InList(s)) {
					sc.ChangeState(SCE_C_CONSTANT);
				} else if ((isObjCSource || _use2ndKeyword(lexType)) && kw2ndKeyword.InList(s)) {
					sc.ChangeState(SCE_C_2NDWORD);
				} else if ((isObjCSource || _use2ndKeyword2(lexType)) && kw2ndKeyword2.InList(s)) {
					sc.ChangeState(SCE_C_2NDWORD2);
				} else if (lastWordWasGoto && (numCBrace > 0)) {
					sc.ChangeState(SCE_C_LABEL);
					lastWordWasGoto = false;
				} else if (sc.ch == ':' && sc.chNext != ':' && !(isAssignStmt)
					&& (numCBrace > 0 && numSBrace == 0 && numRBrace == 0)
					&& visibleChars == static_cast<int>(strlen(s))) {
					sc.ChangeState(SCE_C_LABEL);
				} else if (iswordchar(s[0]) && (IsASpace(sc.ch) || sc.ch == '[' || sc.ch == ')' || sc.ch == '>'
					|| sc.ch == '*' || sc.ch == '&' || sc.ch == ':')) {
					bool is_class = false;
					Sci_PositionU pos = sc.currentPos;
					const int next_char = nextChar;

					if (sc.ch == ':' && sc.chNext == ':') { // C++, Java, PHP
						is_class = true;
					} else if (IsASpace(sc.ch) && iswordstart(next_char)) {
						is_class = true;
						if (isObjCSource && numSBrace > 0) {
							if (!IsUpperCase(s[0])) is_class = false;
						}
					} else if (next_char == ')' || next_char == '>' || next_char == '[' || next_char == '*' || next_char == '&') {
						while (IsASpace(styler.SafeGetCharAt(pos))) pos++;
						pos++;
						while (IsASpace(styler.SafeGetCharAt(pos))) pos++;
						int ch = styler.SafeGetCharAt(pos);
						const bool next_is_word = iswordstart(ch);
						if (next_char == ')' || next_char == '>') {
							if (next_is_word || (ch == '(')) {
								pos = sc.currentPos - strlen(s) - 1;
								while (IsASpace(styler.SafeGetCharAt(pos))) pos--;
								ch = styler.SafeGetCharAt(pos);
								if (next_char == '>' && (ch == '<' || ch == ',')) {
									is_class = true;
								} else if (next_char == ')' && ch == '(') {
									pos--;
									while (IsASpace(styler.SafeGetCharAt(pos))) pos--;
									ch = styler.SafeGetCharAt(pos);
									if (ch == '=' || ch == '(') {
										is_class = true;
									}
								}

							}
						} else if (next_is_word) {
							pos++;
							while (iswordchar(styler.SafeGetCharAt(pos))) pos++;
							while (IsASpace(styler.SafeGetCharAt(pos))) pos++;
							ch = styler.SafeGetCharAt(pos);
							if (ch == '=') {
								is_class = true;
							} else if (ch == ',' || ch == ';') {
								pos = sc.currentPos - strlen(s) - 1;
								while (IsASpace(styler.SafeGetCharAt(pos))) pos--;
								ch = styler.SafeGetCharAt(pos);
								if (iswordchar(ch) || ch == ';' || ch == '{') {
									is_class = true;
								}
							}
						} else if (ch == ')' || ch == '>' || ch == '*') {
							is_class = true;
						} else if (ch == ']' && _squareBraceAfterType(lexType)) {
							is_class = true;
						}
					}
					if (is_class) {
						sc.ChangeState(SCE_C_CLASS);
					}
				}

				if ((isIncludePreprocessor && (sc.ch == '<' || sc.ch == '\"'))
					|| (isMessagePreprocessor && !(sc.ch == '\n' || sc.ch == '\r'))) {
					sc.SetState(SCE_C_STRING);
					if (sc.ch == '\"') {
						isIncludePreprocessor = false;
						//isMessagePreprocessor = false;
					}
					sc.Forward();
				} else {
					sc.SetState(SCE_C_DEFAULT);
				}
			}
			break;

		case SCE_C_COMMENT:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_C_DEFAULT);
			}
			break;
		case SCE_C_COMMENTLINE:
			if (sc.atLineStart && !continuationLine) {
				sc.SetState(SCE_C_DEFAULT);
			} else if (lexType == LEX_PHP && sc.Match('?', '>')) {
				sc.SetState(SCE_C_XML_TAG);
				sc.Forward();
				sc.ForwardSetState(SCE_C_XML_DEFAULT);
			}
			break;
		case SCE_C_COMMENTDOC:
		case SCE_C_COMMENTLINEDOC:
			if (sc.state == SCE_C_COMMENTDOC && sc.Match('*', '/')) {
				docTagType = 0;
				outerStyle = SCE_C_DEFAULT;
				sc.Forward();
				sc.ForwardSetState(SCE_C_DEFAULT);
			} else if (sc.state == SCE_C_COMMENTLINEDOC && sc.atLineStart && !continuationLine) {
				docTagType = 0;
				outerStyle = SCE_C_DEFAULT;
				sc.SetState(SCE_C_DEFAULT);
			} else if (sc.state == SCE_C_COMMENTLINEDOC && lexType == LEX_PHP && sc.Match('?', '>')) {
				docTagType = 0;
				outerStyle = SCE_C_XML_DEFAULT;
				sc.SetState(SCE_C_XML_TAG);
				sc.Forward();
				sc.ForwardSetState(SCE_C_XML_DEFAULT);
			} else if (sc.ch == '{' && sc.chNext == '@' && IsAlpha(sc.GetRelative(2))) {
				docTagType = DOC_TAG_INLINE_AT;
				outerStyle = sc.state;
				sc.SetState(SCE_C_COMMENTDOC_TAG);
				sc.Forward();
			} else if ((sc.ch == '@' || sc.ch == '\\') && IsAlpha(sc.chNext) && (IsASpace(sc.chPrev)
				|| sc.chPrev == '*' || sc.chPrev == '/' || sc.chPrev == '!')) {
				docTagType = DOC_TAG_AT;
				outerStyle = sc.state;
				sc.SetState(SCE_C_COMMENTDOC_TAG);
			} else if (sc.ch == '<' && IsAlpha(sc.chNext)) {
				docTagType = DOC_TAG_OPEN_XML;
				outerStyle = sc.state;
				sc.SetState(SCE_C_COMMENTDOC_TAG_XML);
			} else if (sc.ch == '<' && sc.chNext == '/' && IsAlpha(sc.GetRelative(2))) {
				docTagType = DOC_TAG_CLOSE_XML;
				outerStyle = sc.state;
				sc.SetState(SCE_C_COMMENTDOC_TAG_XML);
				sc.Forward();
			} else if ((docTagType == DOC_TAG_INLINE_AT && sc.ch == '}')
				|| (docTagType == DOC_TAG_OPEN_XML && ((sc.ch == '/' && sc.chNext == '>') || sc.ch == '>'))) {
				docTagType = 0;
				sc.SetState((sc.ch == '}') ? SCE_C_COMMENTDOC_TAG : SCE_C_COMMENTDOC_TAG_XML);
				sc.Forward();
				if ((sc.chPrev == '/' && sc.ch == '>') || (sc.chPrev == '}' && sc.ch == '}'))
					sc.Forward();
				bool ignore = false;
				if (sc.ch == '<' && (IsAlpha(sc.chNext) || (sc.chNext == '/' && IsAlpha(sc.GetRelative(2))))) {
					ignore = true;
					sc.SetState(SCE_C_COMMENTDOC_TAG_XML);
					sc.Forward();
					docTagType = DOC_TAG_OPEN_XML;
					if (sc.ch == '/') {
						docTagType = DOC_TAG_CLOSE_XML;
						sc.Forward();
					}
				}
				if (!ignore) {
					sc.SetState(outerStyle);
				}
			}
			break;
		case SCE_C_COMMENTDOC_TAG:
		case SCE_C_COMMENTDOC_TAG_XML:
			if (!iswordstart(sc.ch) && sc.ch != '-') {
				bool ignore = false;
				if (sc.ch == '>' || sc.ch == '}') {
					docTagType = 0;
					sc.Forward();
					if (sc.ch == '<' && (IsAlpha(sc.chNext) || (sc.chNext == '/' && IsAlpha(sc.GetRelative(2))))) {
						ignore = true;
						sc.Forward();
						docTagType = DOC_TAG_OPEN_XML;
						if (sc.ch == '/') {
							docTagType = DOC_TAG_CLOSE_XML;
							sc.Forward();
						}
					} else if (sc.ch == '{' && sc.chNext == '@' && IsAlpha(sc.GetRelative(2))) {
						ignore = true;
						sc.SetState(SCE_C_COMMENTDOC_TAG);
						docTagType = DOC_TAG_INLINE_AT;
						sc.Forward();
					}
				}
				if (!ignore) {
					sc.SetState(outerStyle);
				}
			}
			break;
		case SCE_C_DNESTEDCOMMENT:
			if (sc.Match('+', '/')) {
				if (curNcLevel > 0)
					--curNcLevel;
				sc.Forward();
				if (curNcLevel == 0) {
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
			} else if (sc.Match('/', '+')) {
				++curNcLevel;
				sc.Forward();
			}
			break;

		case SCE_C_CHARACTER:
			if (sc.atLineEnd && !(lexType == LEX_PHP)) {
				sc.ChangeState(SCE_C_STRINGEOL);
			} else if (sc.ch == '\\' && IsEscapeChar(sc.chNext)) {
				sc.Forward();
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_C_DEFAULT);
			}
			break;
		case SCE_C_STRINGEOL:
			if (sc.atLineStart) {
				outerStyle = SCE_C_DEFAULT;
				sc.SetState(SCE_C_DEFAULT);
			}
			break;
		case SCE_C_STRING:
			if (sc.atLineEnd) {
				if (lexType == LEX_HX || lexType == LEX_ASY || lexType == LEX_JAM || lexType == LEX_PHP) {
					continue;
				}
				sc.ChangeState(SCE_C_STRINGEOL);
			} else if (isIncludePreprocessor && sc.ch == '>') {
				outerStyle = SCE_C_DEFAULT;
				sc.ForwardSetState(SCE_C_DEFAULT);
				isIncludePreprocessor = false;
			} else if (isMessagePreprocessor && sc.atLineEnd) {
				sc.ChangeState(SCE_C_STRINGEOL);
				isMessagePreprocessor = false;
			} else if (sc.ch == '\\' && IsEscapeChar(sc.chNext)) {
				sc.Forward();
			} else if (lexType == LEX_PHP && sc.chPrev != '\\' && (
				(sc.ch == '$' && iswordstart(sc.chNext)) ||
				(sc.ch == '$' && sc.chNext == '{') ||
				(sc.ch == '{' && sc.chNext == '$'))) {
				outerStyle = SCE_C_STRING;
				if (sc.ch == '{' || sc.chNext == '{') {
					varType = 2;
					++numDTSBrace;
					sc.SetState(SCE_C_VARIABLE2);
					sc.Forward(2);
				} else {
					sc.SetState(SCE_C_VARIABLE);
				}
			} else if (sc.ch == '\"') {
				if ((lexType == LEX_D) && IsDStrFix(sc.chNext))
					sc.Forward();
				outerStyle = SCE_C_DEFAULT;
				sc.ForwardSetState(SCE_C_DEFAULT);
			}
			break;
		case SCE_C_STRINGRAW:
			if (sc.ch == '\"') {
				Sci_PositionU pos = sc.currentPos;
				Sci_PositionU len = 0;
				bool rawEnd = false;
				while (pos > startPos && len < 16) {
					const char ch = styler.SafeGetCharAt(pos);
					if (ch == ')') {
						rawEnd = true;
						break;
					}
					if (IsASpace(ch) || ch == '\\' || ch == '(' || ch == ')') {
						break;
					}
					pos--;
					len++;
				}
				if (rawEnd) {
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
			}
			break;

		case SCE_C_DSTRINGX: // D
			if (sc.ch == '\"') {
				if (IsDStrFix(sc.chNext))
					sc.Forward();
				sc.ForwardSetState(SCE_C_DEFAULT);
			}
			break;
		case SCE_C_DSTRINGB: // D
			if (sc.ch == '`' && (lexType == LEX_JS || lexType == LEX_D || lexType == LEX_GO)) {
				if (lexType == LEX_D && IsDStrFix(sc.chNext))
					sc.Forward();
				sc.ForwardSetState(SCE_C_DEFAULT);
			}
			break;
		case SCE_C_DSTRINGQ: // D
			if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_C_DEFAULT);
			}
			break;
		case SCE_C_DSTRINGT: // D
			if (sc.ch == '}') {
				if (numDTSBrace > 0)
					--numDTSBrace;
				if (numDTSBrace == 0)
					sc.ForwardSetState(SCE_C_DEFAULT);
			} else if (sc.ch == '{') {
				++numDTSBrace;
			}
			break;

		case SCE_C_VARIABLE:
			if (!(iswordstart(sc.ch) || (sc.ch == '-' && lexType == LEX_JAM))) {
				if (lexType == LEX_PHP) {
					char s[256];
					sc.GetCurrentLowered(s, sizeof(s));
					if (kwEnumeration.InList(s)) {
						sc.ChangeState(SCE_C_ENUMERATION);
						if (outerStyle != SCE_C_DEFAULT && ((sc.ch == '-' && sc.chNext == '>') || sc.ch == '[')) {
							sc.SetState(SCE_C_VARIABLE);
							sc.Forward(2);
						}
					}
				}
				if (lexType == LEX_PHP && outerStyle != SCE_C_DEFAULT && ((sc.ch == '-' && sc.chNext == '>') || sc.ch == '[')) {
					sc.Forward(2);
				} else {
					if (outerStyle != SCE_C_DEFAULT && sc.ch == ']') {
						sc.Forward();
					}
					sc.SetState(outerStyle);
					if (outerStyle == SCE_C_STRING && sc.ch == '\\' && IsEscapeChar(sc.chNext)) {
						sc.Forward();
					} else if (outerStyle == SCE_C_STRING && sc.ch == '\"') {
						outerStyle = SCE_C_DEFAULT;
						sc.ForwardSetState(SCE_C_DEFAULT);
					}
				}
			}
			break;
		case SCE_C_VARIABLE2:
			if ((varType == 1 && sc.ch == ')') || (varType == 2 && sc.ch == '}')) {
				if (numDTSBrace > 0) {
					--numDTSBrace;
				}
				if (numDTSBrace == 0) {
					sc.Forward();
					if (varType == 2 && sc.ch == '}')
						sc.Forward();
					varType = 0;
					sc.SetState(outerStyle);
					if (outerStyle == SCE_C_STRING && sc.ch == '\"') {
						outerStyle = SCE_C_DEFAULT;
						sc.ForwardSetState(SCE_C_DEFAULT);
					}
				}
			} else if ((varType == 1 && sc.Match('$', '(')) || (varType == 2 && sc.Match('$', '{'))) {
				++numDTSBrace;
				sc.Forward();
			}
			break;

		case SCE_C_REGEX:
			if (sc.atLineStart && !(lexType & LEX_AWK)) {
				sc.SetState(SCE_C_DEFAULT);
			} else if (!inRERange && sc.ch == '/') {
				sc.Forward();
				while (IsLowerCase(sc.ch)) {
					sc.Forward();	 // gobble regex flags
				}
				sc.SetState(SCE_C_DEFAULT);
			} else if (sc.ch == '\\') {
				// Gobble up the escaped character
				sc.Forward();
			} else if (sc.ch == '[') {
				inRERange = true;
			} else if (sc.ch == ']') {
				inRERange = false;
			}
			break;
		case SCE_C_VERBATIM:
			if (isObjCSource && sc.ch == '\\' && IsEscapeChar(sc.chNext)) {
				sc.Forward();
			} else if (sc.ch == '\"') {
				if (sc.chNext == '\"') {
					sc.Forward();
				} else {
					if ((lexType == LEX_D) && IsDStrFix(sc.chNext))
						sc.Forward();
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
			}
			break;
		case SCE_C_TRIPLEVERBATIM:
			if (sc.ch == '\\' && IsEscapeChar(sc.chNext)) {
				sc.Forward();
			} else if (isTripleSingle && sc.Match(R"(''')")) {
				isTripleSingle = false;
				sc.Forward(2);
				sc.ForwardSetState(SCE_C_DEFAULT);
			} else if (sc.Match(R"(""")")) {
				sc.Forward(2);
				sc.ForwardSetState(SCE_C_DEFAULT);
			}
			break;

		case SCE_C_UUID:
			if (sc.ch == '\"') {
				sc.ChangeState(SCE_C_STRING);
				sc.Forward();
			}
			if (sc.ch == ')') {
				sc.SetState(SCE_C_DEFAULT);
			}
			break;

			// for Smali
		case SCE_C_2NDWORD2: // instruction
		case SCE_C_DIRECTIVE:
		case SCE_C_LABEL:
			if (!(iswordstart(sc.ch) || sc.ch == '-' || sc.ch == '.')) {
				sc.SetState(SCE_C_DEFAULT);
			}
			break;
		case SCE_C_ASM_REGISTER:
			if (!IsADigit(sc.ch)) {
				sc.SetState(SCE_C_DEFAULT);
			}
			break;
			//
		case SCE_C_XML_TAG:	// PHP, Scala, ActionScript, Javascript
			if (lexType == LEX_PHP) {
			} else {
				if (sc.Match('<', '/') || sc.Match('/', '>')) {
					if (curNcLevel > 0)
						--curNcLevel;
					sc.Forward();
					if (curNcLevel == 0) {
						while (sc.ch != '>') {
							sc.Forward();
						}
						sc.ForwardSetState(SCE_C_DEFAULT);
					}
				} else if (sc.ch == '<' && sc.chNext != '/') {
					++curNcLevel;
					sc.Forward();
				}
				break;
			}
			break;

		case SCE_C_HEREDOC:	// PHP
			if (sc.chPrev != '\\' && (
				(sc.ch == '$' && iswordstart(sc.chNext)) ||
				(sc.ch == '$' && sc.chNext == '{') ||
				(sc.ch == '{' && sc.chNext == '$'))) {
				outerStyle = SCE_C_HEREDOC;
				if (sc.ch == '{' || sc.chNext == '{') {
					varType = 2;
					++numDTSBrace;
					sc.SetState(SCE_C_VARIABLE2);
					sc.Forward(2);
				} else {
					sc.SetState(SCE_C_VARIABLE);
				}
			} else
				if ((visibleChars == 0) && styler.Match(sc.currentPos + 1, heredoc)) {
					sc.Forward(heredoc_len);
					outerStyle = SCE_C_DEFAULT;
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
			break;
		case SCE_C_NOWDOC:	// PHP
			if ((visibleChars == 0) && styler.Match(sc.currentPos + 1, heredoc)) {
				sc.Forward(heredoc_len);
				sc.ForwardSetState(SCE_C_DEFAULT);
			}
			break;
		}

		// Determine if a new state should be entered.
		if (sc.state == SCE_C_XML_DEFAULT) {
			if (lexType == LEX_PHP && sc.Match("<?php")) {
				sc.SetState(SCE_C_XML_TAG);
				sc.Forward(5);
				sc.SetState(SCE_C_DEFAULT);
			}
		} else
			if (sc.state == SCE_C_DEFAULT) {
				if (lexType == LEX_PHP && sc.Match("<?php")) {
					sc.SetState(SCE_C_XML_TAG);
					sc.Forward(5);
					sc.SetState(SCE_C_DEFAULT);
				} else
					if (((lexType == LEX_CS) && sc.Match('@', '\"'))
						|| ((lexType == LEX_D) && sc.Match('r', '\"'))) {
						sc.SetState(SCE_C_VERBATIM);
						sc.Forward();
					} else if (_hasTripleVerbatim(lexType) && sc.Match(R"(""")")) {
						sc.SetState(SCE_C_TRIPLEVERBATIM);
						sc.Forward(2);
					} else if (lexType == LEX_GROOVY && sc.Match(R"(''')")) {
						sc.SetState(SCE_C_TRIPLEVERBATIM);
						sc.Forward(2);
						isTripleSingle = true;
					} else if ((lexType == LEX_D) && sc.Match('x', '\"')) {
						sc.SetState(SCE_C_DSTRINGX);
						sc.Forward();
					} else if ((lexType == LEX_D) && sc.Match('q', '\"')) {
						sc.SetState(SCE_C_DSTRINGQ);
						sc.Forward();
					} else if ((lexType == LEX_D) && sc.Match('q', '{')) {
						++numDTSBrace;
						sc.SetState(SCE_C_DSTRINGT);
						sc.Forward();
					} else if (sc.ch == '`' && (lexType == LEX_JS || lexType == LEX_D || lexType == LEX_GO)) {
						sc.SetState(SCE_C_DSTRINGB);
					} else if (!_sharpComment(lexType) && sc.Match('/', '*')) {
						if (visibleChars == 0 && (sc.Match("/**") || sc.Match("/*!"))) {
							sc.SetState(SCE_C_COMMENTDOC);
						} else {
							sc.SetState(SCE_C_COMMENT);
						}
						sc.Forward();
					} else if ((!(_sharpComment(lexType)) && sc.Match('/', '/'))
						|| ((_sharpComment(lexType) || lexType == LEX_PHP) && sc.ch == '#')) {
						if (visibleChars == 0 && ((sc.Match("///") && !sc.Match("////")) || sc.Match("//!")))
							sc.SetState(SCE_C_COMMENTLINEDOC);
						else
							sc.SetState(SCE_C_COMMENTLINE);
					} else if (sc.ch == '$' && (lexType == LEX_JAM && sc.chNext == '(')) {
						++numDTSBrace;
						if (sc.chNext == '(')
							varType = 1;
						else
							varType = 2;
						sc.SetState(SCE_C_VARIABLE2);
						sc.Forward();
					} else if (IsADigit(sc.ch) || (sc.ch == '.' && IsADigit(sc.chNext))) {
						sc.SetState(SCE_C_NUMBER);
					} else if (sc.ch == '/') { // bug
						// RegExp only appears in assignment or function argument
						//if (_hasRegex(lexType) && (isAssignStmt || numRBrace > 0) && (strchr("([{=,:;!%^&*|?~+-", chPrevNonWhite) || followsReturn)
						const bool isJsRegex = (isAssignStmt && (chPrevNonWhite == '=' || chPrevNonWhite == ':')) 	// assignment
							|| (numRBrace > 0 && strchr("(,!&|", chPrevNonWhite))	// argument
							|| (strchr("};", chPrevNonWhite))
							|| followsReturn;
						if (_hasRegex(lexType) && isJsRegex
							&& !(chPrevNonWhite == '+' || chPrevNonWhite == '-' || followsPostfixOperator)) {
							sc.SetState(SCE_C_REGEX);	// JavaScript's RegEx
							followsReturn = false;
							inRERange = false;
						} else if ((lexType == LEX_D) && sc.chNext == '+') {
							++curNcLevel;
							sc.SetState(SCE_C_DNESTEDCOMMENT);
							sc.Forward();
						} else {
							sc.SetState(SCE_C_OPERATOR);
						}
					} else if ((lexType == LEX_CPP || lexType == LEX_RC) && (sc.ch == 'u' || sc.ch == 'U' || sc.ch == 'L')) {
						int offset = 0;
						bool is_raw = false;
						bool isuchar = false;
						if (sc.chNext == '\"' || sc.chNext == '\'') {
							offset = 1;
							isuchar = sc.chNext == '\'';
						} else if (sc.chNext == 'R' && sc.GetRelative(2) == '\"') {
							offset = 2;
							is_raw = true;
						} else if (sc.chNext == '8' && sc.GetRelative(2) == 'R' && sc.GetRelative(3) == '\"') {
							offset = 3;
							is_raw = true;
						}
						if (!offset) {
							sc.SetState(SCE_C_IDENTIFIER);
						} else {
							if (is_raw)
								sc.SetState(SCE_C_STRINGRAW);
							else if (isuchar)
								sc.SetState(SCE_C_CHARACTER);
							else
								sc.SetState(SCE_C_STRING);
							sc.Forward(offset);
						}
					} else if (lexType == LEX_CPP && sc.ch == 'R' && sc.chNext == '\"') {
						sc.SetState(SCE_C_STRINGRAW);
						sc.Forward();
					} else if (lexType == LEX_JAM && sc.ch == '\\' && IsEscapeChar(sc.chNext)) {
						sc.Forward();
					} else if (sc.ch == '\"') {
						isIncludePreprocessor = false;
						//isMessagePreprocessor = false;
						sc.SetState(SCE_C_STRING);
					} else if (sc.ch == '\'') {
						if (lexType == LEX_SCALA && !(sc.chNext == '\\' || (sc.chNext != '\'' && sc.GetRelative(2) == '\'')))
							sc.SetState(SCE_C_OPERATOR);
						else
							sc.SetState(SCE_C_CHARACTER);
					} else if (_hasPreprocessor(lexType) && sc.ch == '#') {
						if (lineState & LEX_BLOCK_MASK_DEFINE) {
							sc.SetState(SCE_C_OPERATOR);
							if (sc.chNext == '#' || sc.chNext == '@') {
								sc.Forward(2);
								if (iswordstart(sc.ch))
									sc.SetState(SCE_C_IDENTIFIER);
								else
									sc.SetState(SCE_C_DEFAULT);
							}
						} else if (visibleChars == 0) {
							isPreprocessorWord = true;
							if (isspacechar(sc.chNext)) {
								sc.SetState(SCE_C_PREPROCESSOR);
								sc.ForwardSetState(SCE_C_DEFAULT);
							} else if (iswordstart(sc.chNext)) {
								sc.SetState(SCE_C_IDENTIFIER);
							}
						}
					} else if (sc.ch == '$' && (lexType == LEX_HX || lexType == LEX_AWK || lexType == LEX_PHP)) {
						if (lexType == LEX_PHP && !iswordstart(sc.chNext))
							sc.SetState(SCE_C_OPERATOR);
						else
							sc.SetState(SCE_C_VARIABLE);
					} else if (iswordstart(sc.ch) || (iswordstart(sc.chNext) && (lexType != LEX_PHP) && (sc.ch == '@' ||
						(sc.ch == '#' && (lexType == LEX_D || lexType == LEX_HX || lexType == LEX_JAM))))) {
						sc.SetState(SCE_C_IDENTIFIER);
					} else if (sc.ch == '.') {
						sc.SetState(SCE_C_OPERATOR);
						if (sc.chNext == '.')
							sc.Forward();
					} else if (lexType == LEX_PHP && sc.Match('?', '>')) {
						sc.SetState(SCE_C_XML_TAG);
						sc.Forward();
						sc.ForwardSetState(SCE_C_XML_DEFAULT);
					} else if (sc.ch == '<') {
						if (_hasXML(lexType) && chPrevNonWhite == '=') {
							sc.SetState(SCE_C_XML_TAG);
							if (sc.chNext != '/') {
								++curNcLevel;
							}
							sc.Forward();
						} else if (isIncludePreprocessor) {
							sc.SetState(SCE_C_STRING);
						} else if (lexType == LEX_PHP && sc.chNext == '<' && sc.GetRelative(2) == '<') { // <<<EOT, <<<"EOT", <<<'EOT'
							if (sc.GetRelative(3) == '\'')
								sc.SetState(SCE_C_NOWDOC);
							else
								sc.SetState(SCE_C_HEREDOC);
							sc.Forward(3);
							if (sc.ch == '\'' || sc.ch == '\"')
								sc.Forward();
							heredoc_len = LexGetRange(sc.currentPos + 1, styler, iswordstart, heredoc, sizeof(heredoc));
							sc.Forward(heredoc_len);
							if (sc.ch == '\'' || sc.ch == '\"')
								sc.Forward();
						} else {
							sc.SetState(SCE_C_OPERATOR);
						}
					} else if (isoperator(sc.ch) || ((lexType == LEX_CS || lexType == LEX_D || lexType == LEX_JS) && sc.ch == '$') || sc.ch == '@'
						|| (lexType == LEX_PHP && sc.ch == '\\')) {
						sc.SetState(SCE_C_OPERATOR);
						isPragmaPreprocessor = false;
						if ((sc.ch == '+' && sc.chPrev == '+') || (sc.ch == '-' && sc.chPrev == '-'))
							followsPostfixOperator = true;
						else
							followsPostfixOperator = false;

						if (lastWordWasUUID && sc.ch == '(') {
							sc.ForwardSetState(SCE_C_UUID);
							lastWordWasUUID = false;
							isAssignStmt = false;
						}
						if (sc.ch == '=' && !(sc.chPrev == '=' || sc.chNext == '=')) {
							isAssignStmt = true;
						}
						if (sc.ch == ';') {
							isAssignStmt = false;
							lastWordWasGoto = false;
							followsReturn = false;
							//if (numCBrace == 0)
							isTypeDefine = false;
						}
						if (sc.ch == '{') {
							if (lastWordWasAsm) {
								lineState |= LEX_BLOCK_MASK_ASM;
								lastWordWasAsm = false;
							}
							//if (isTypeDefine) {
							//	lineState |= LEX_BLOCK_MASK_TYPEDEF;
							//	isTypeDefine = false;
							//}
						} else if (sc.ch == '}') {
							lastWordWasGoto = false;
							followsReturn = false;
							lastWordWasAsm = false;
							if (lineState & LEX_BLOCK_MASK_ASM) {
								lineState &= LEX_BLOCK_UMASK_ASM;
							}
						}

						if (!(lineState & LEX_BLOCK_MASK_DEFINE)) {
							if (sc.ch == '{') {
								++numCBrace;
							} else if (sc.ch == '}') {
								if (numCBrace > 0)
									--numCBrace;
							} else if (sc.ch == '(') {
								++numRBrace;
							} else if (sc.ch == ')') {
								if (numRBrace > 0)
									--numRBrace;
								if (lastWordWasAttr && numRBrace == 0)
									lastWordWasAttr = false;
							} else if (sc.ch == '[') {
								++numSBrace;
							} else if (sc.ch == ']') {
								if (numSBrace > 0)
									--numSBrace;
							}
						}
					}
			}

		// Handle line continuation generically.
		if (sc.ch == '\\') {
			if (sc.chNext == '\n' || sc.chNext == '\r') {
				lineCurrent++;
				sc.Forward();
				if (sc.ch == '\r' && sc.chNext == '\n') {
					sc.Forward();
				}
				continuationLine = true;
				continue;
			}
		}

		if (!(isspacechar(sc.ch) || IsSpaceEquiv(sc.state))) {
			chPrevNonWhite = sc.ch;
			visibleChars++;
		}
		continuationLine = false;
	}

	sc.Complete();
}

static bool IsCppDefineLine(Sci_Position line, LexAccessor &styler, Sci_Position &DefinePos) noexcept {
	Sci_Position pos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	pos = LexSkipSpaceTab(pos, endPos, styler);
	if (styler[pos] == '#' && styler.StyleAt(pos) == SCE_C_PREPROCESSOR) {
		pos++;
		pos = LexSkipSpaceTab(pos, endPos, styler);
		if (styler.Match(pos, "define") && IsASpaceOrTab(styler[pos + 6])) {
			DefinePos = pos + 6;
			return true;
		}
	}
	return false;
}
// also used in LexAsm.cxx
bool IsCppInDefine(Sci_Position currentPos, LexAccessor &styler) noexcept {
	Sci_Position line = styler.GetLine(currentPos);
	Sci_Position pos;
	if (IsCppDefineLine(line, styler, pos)) {
		if (pos < currentPos)
			return true;
	}
	line--;
	while (line > 0 && IsBackslashLine(line, styler) && !IsCppDefineLine(line, styler, pos))
		line--;
	if (line >= 0 && IsCppDefineLine(line, styler, pos) && IsBackslashLine(line, styler)) {
		return true;
	}
	return false;
}
static bool IsCppFoldingLine(Sci_Position line, LexAccessor &styler, int kind) noexcept {
	const Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	Sci_Position pos = LexSkipSpaceTab(startPos, endPos, styler);
	const int stl = styler.StyleAt(pos);
	char ch = styler[pos];
	switch (kind) {
		//case 0:
		//	return (ch == '/' || ch == '#') && (stl == SCE_C_COMMENTLINE || stl == SCE_C_COMMENTLINEDOC);
	case 1:
		return stl == SCE_C_WORD && (ch == 'i' || ch == 'u' || ch == 't') &&
			(styler.Match(pos, "using") || styler.Match(pos, "import") || styler.Match(pos, "typedef"));
	case 2:
		return stl == SCE_C_DIRECTIVE && (ch == '@') &&
			(styler.Match(pos, "@property") || styler.Match(pos, "@synthesize"));
	default:
		if (!(stl == SCE_C_PREPROCESSOR && (ch == '#' || ch == '!')))
			return false;
		pos++;
		pos = LexSkipSpaceTab(pos, endPos, styler);
		ch = styler[pos];
		if (kind == 3 && (ch == 'i' || ch == 'u')) {
			return styler.Match(pos, "include") || styler.Match(pos, "using") || styler.Match(pos, "import");
		} else if (kind == 4 && ch == 'd') {
			return styler.Match(pos, "define") && !IsBackslashLine(line, styler); // multi-line #define
		} else if (kind == 5 && ch == 'u') {
			return styler.Match(pos, "undef");
		}
		return false;
	}
}
#define IsCommentLine(line)		IsLexCommentLine(line, styler, MultiStyle(SCE_C_COMMENTLINE, SCE_C_COMMENTLINEDOC))
//#define IsCommentLine(line)		IsCppFoldingLine(line, styler, 0)
#define IsUsingLine(line)		IsCppFoldingLine(line, styler, 1)
//#define IsPropertyLine(line)	IsCppFoldingLine(line, styler, 2)
#define IsIncludeLine(line)		IsCppFoldingLine(line, styler, 3)
#define IsDefineLine(line)		IsCppFoldingLine(line, styler, 4)
#define IsUnDefLine(line)		IsCppFoldingLine(line, styler, 5)

static constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_C_COMMENT || style == SCE_C_COMMENTDOC
		|| style == SCE_C_COMMENTDOC_TAG || style == SCE_C_COMMENTDOC_TAG_XML;
}
static constexpr bool IsHear_NowDocStyle(int style) noexcept {
	return style == SCE_C_HEREDOC || style == SCE_C_NOWDOC;
}
static bool IsOpenBraceLine(Sci_Position line, LexAccessor &styler) noexcept {
	// above line
	Sci_Position startPos = styler.LineStart(line - 1);
	Sci_Position endPos = styler.LineStart(line) - 1;
	Sci_Position pos = LexSkipSpaceTab(startPos, endPos, styler);
	char ch = styler[pos];
	int stl = styler.StyleAt(pos);
	if (ch == '\r' || ch == '\n' || IsSpaceEquiv(stl) || (stl == SCE_C_PREPROCESSOR || stl == SCE_C_XML_TAG))
		return false;
	while (endPos >= pos && isspacechar(styler[endPos]))
		endPos--;
	ch = styler[endPos];
	stl = styler.StyleAt(endPos);
	if (stl == SCE_C_OPERATOR && !(ch == ')' || ch == '>' || ch == '=' || ch == ':' || ch == ']' || ch == '^' || ch == '.' || ch == ';'))
		return false;
	//if (styler[endPos-1] == '\n' || styler[endPos-1] == '\r'
	//	|| (styler.StyleAt(endPos) == SCE_C_OPERATOR
	//		&& !(ch == ')' || ch == '>' || ch == '=' || ch == ':' || ch == ']')))
		//return false;
	// current line
	startPos = styler.LineStart(line);
	endPos = styler.LineStart(line + 1) - 1;
	pos = LexSkipSpaceTab(startPos, endPos, styler);
	// only '{' line
	if (styler.StyleAt(pos) == SCE_C_OPERATOR && styler[pos] == '{') {
		return true;
		//pos++;
		//pos = LexSkipSpaceTab(pos, endPos, styler);
		//ch = styler[pos];
		//stl = styler.StyleAt(pos);
		//return  (ch == '\n') || (ch == '\r') || IsSpaceEquiv(stl);
	}
	return false;
}

static void FoldCppDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const bool foldComment = styler.GetPropertyInt("fold.comment", 1) != 0;
	const bool foldPreprocessor = styler.GetPropertyInt("fold.preprocessor", 1) != 0;
	//const bool foldAtElse = styler.GetPropertyInt("fold.at.else", 0) != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 0) != 0;

	const int lexType = styler.GetPropertyInt("lexer.lang.type", LEX_CPP);
	const bool hasPreprocessor = _hasPreprocessor(lexType);

	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	//int levelMinCurrent = levelCurrent;
	int levelNext = levelCurrent;

	int chNext = styler[startPos];
	int style = initStyle;
	int styleNext = styler.StyleAt(startPos);
	bool isObjCProtocol = false;

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
		if (foldComment && IsStreamCommentStyle(style) && !IsCommentLine(lineCurrent)) {
			if (!IsStreamCommentStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamCommentStyle(styleNext) && !atEOL) {
				levelNext--;
			}
		}
		if (lexType == LEX_D && foldComment && style == SCE_C_DNESTEDCOMMENT) {
			if (ch == '/' && chNext == '+')
				levelNext++;
			else if (ch == '+' && chNext == '/')
				levelNext--;
		}
		if (lexType == LEX_PHP && IsHear_NowDocStyle(style)) {
			if (!IsHear_NowDocStyle(stylePrev)) {
				levelNext++;
			} else if (!IsHear_NowDocStyle(styleNext) && !atEOL) {
				levelNext--;
			}
		}
		if (style == SCE_C_TRIPLEVERBATIM || style == SCE_C_VERBATIM) {
			if (stylePrev != style) {
				levelNext++;
			} else if (styleNext != style && !atEOL) {
				levelNext--;
			}
		}

		if (atEOL && IsUsingLine(lineCurrent)) {
			if (!IsUsingLine(lineCurrent - 1) && IsUsingLine(lineCurrent + 1))
				levelNext++;
			else if (IsUsingLine(lineCurrent - 1) && !IsUsingLine(lineCurrent + 1))
				levelNext--;
		}
		//if (hasPreprocessor && atEOL && IsPropertyLine(lineCurrent)) {
		//	if(!IsPropertyLine(lineCurrent-1) && IsPropertyLine(lineCurrent+1))
		//		levelNext++;
		//	else if(IsPropertyLine(lineCurrent-1) && !IsPropertyLine(lineCurrent+1))
		//		levelNext--;
		//}
		if (hasPreprocessor && atEOL && IsIncludeLine(lineCurrent)) {
			if (!IsIncludeLine(lineCurrent - 1) && IsIncludeLine(lineCurrent + 1))
				levelNext++;
			else if (IsIncludeLine(lineCurrent - 1) && !IsIncludeLine(lineCurrent + 1))
				levelNext--;
		}
		if (hasPreprocessor && atEOL && IsDefineLine(lineCurrent)) {
			if (!IsDefineLine(lineCurrent - 1) && IsDefineLine(lineCurrent + 1))
				levelNext++;
			else if (IsDefineLine(lineCurrent - 1) && !IsDefineLine(lineCurrent + 1))
				levelNext--;
		}
		if (hasPreprocessor && atEOL && IsUnDefLine(lineCurrent)) {
			if (!IsUnDefLine(lineCurrent - 1) && IsUnDefLine(lineCurrent + 1))
				levelNext++;
			else if (IsUnDefLine(lineCurrent - 1) && !IsUnDefLine(lineCurrent + 1))
				levelNext--;
		}

		if (atEOL && !IsStreamCommentStyle(style) && IsBackslashLine(lineCurrent, styler) && !IsBackslashLine(lineCurrent - 1, styler)) {
			levelNext++;
		}
		if (atEOL && !IsStreamCommentStyle(style) && !IsBackslashLine(lineCurrent, styler) && IsBackslashLine(lineCurrent - 1, styler)) {
			levelNext--;
		}
		if (atEOL && !(hasPreprocessor && IsCppInDefine(i, styler)) && IsOpenBraceLine(lineCurrent + 1, styler)) {
			levelNext++;
		}

		if ((hasPreprocessor || lexType == LEX_HX) && foldPreprocessor && (ch == '#') && style == SCE_C_PREPROCESSOR) {
			Sci_Position pos = LexSkipSpaceTab(i + 1, endPos, styler);
			if (styler.Match(pos, "if") || styler.Match(pos, "region")) {
				levelNext++;
			} else if (styler.Match(pos, "end")) {
				levelNext--;
			} else if (styler.Match(pos, "pragma")) { // #pragma region, #pragma endregion
				pos = LexSkipSpaceTab(pos + 7, endPos, styler);
				if (styler.StyleAt(pos) == SCE_C_PREPROCESSOR) {
					if (styler.Match(pos, "region")) {
						levelNext++;
					} else if (styler.Match(pos, "endregion")) {
						levelNext--;
					}
				}
			}
		}

		// Objctive C/C++
		if ((lexType == LEX_CPP || lexType == LEX_OBJC) && ch == '@' && style == SCE_C_DIRECTIVE) {
			if (styler.Match(i + 1, "interface") || styler.Match(i + 1, "implementation")) {
				levelNext++;
			} else if (styler.Match(i + 1, "end")) {
				levelNext--;
				isObjCProtocol = false;
			} else if (styler.Match(i + 1, "protocol")) {
				if (LexGetNextChar(i + 9, styler) != '(') { // @protocol()
					isObjCProtocol = true;
					levelNext++;
				}
			}
		}

		if (style == SCE_C_OPERATOR && !(hasPreprocessor && IsCppInDefine(i, styler))) {
			// maybe failed in multi-line define section, MFC's afx.h is a example
			if (ch == '{' && !(lineCurrent > 0 && visibleChars == 0 && IsOpenBraceLine(lineCurrent, styler))) {
				levelNext++;
				//if (levelMinCurrent > levelNext) {
				//	levelMinCurrent = levelNext;
				//}
			} else if (ch == '}') {
				levelNext--;
			} else if (ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == ']' || ch == ')') {
				levelNext--;
			}
			if (isObjCProtocol && ch == ';') {
				isObjCProtocol = false;
				levelNext--;
			}
		}

		// Go
		if ((lexType == LEX_JS || lexType == LEX_GO) && style == SCE_C_DSTRINGB) {
			if (ch == '`') {
				if (styleNext == SCE_C_DSTRINGB)
					levelNext++;
				else if (stylePrev == SCE_C_DSTRINGB)
					levelNext--;
			}
		}

		// Resource Script
		if (style == SCE_C_WORD && stylePrev != SCE_C_WORD) {
			if (lexType == LEX_RC && (ch == 'B' || ch == 'E')) {
				if (styler.Match(i, "BEGIN")) {
					levelNext++;
				} else if (styler.Match(i, "END")) {
					levelNext--;
				}
			}
		}

		if (style == SCE_C_XML_TAG) {
			if (styler.Match(i, "</") || styler.Match(i, "/>") || styler.Match(i, "?>"))
				levelNext--;
			else if (ch == '<' && chNext != '/')
				levelNext++;
		}

		if (!isspacechar(ch))
			visibleChars++;
		if (atEOL || (i == endPos - 1)) {
			const int levelUse = levelCurrent;
			//if (foldAtElse) {
			//	levelUse = levelMinCurrent;
			//}
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
			//levelMinCurrent = levelCurrent;
			visibleChars = 0;
			isObjCProtocol = false;
		}
	}
}

LexerModule lmCPP(SCLEX_CPP, ColouriseCppDoc, "cpp", FoldCppDoc);
