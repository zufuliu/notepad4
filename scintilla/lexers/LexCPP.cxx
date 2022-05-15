// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for C/C++, Rescouce Script, Objective C/C++, Scala, IDL/ODL

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

#define		LEX_CPP		0	// C/C++
#define		LEX_RC		1	// Resouce Script
#define		LEX_OBJC	2	// Objective C/C++
#define		LEX_SCALA	3	// Scala Script

constexpr bool HasPreprocessor(int lex) noexcept { // #[space]preprocessor
	return lex == LEX_CPP || lex == LEX_RC || lex == LEX_OBJC;
}
constexpr bool HasAnotation(int lex) noexcept { // @anotation
	return lex == LEX_SCALA;
}
constexpr bool HasTripleVerbatim(int lex) noexcept {
	return lex == LEX_SCALA;
}
constexpr bool HasXML(int lex) noexcept {
	return lex == LEX_SCALA;
}
constexpr bool SquareBraceAfterType(int lex) noexcept {
	return lex == LEX_SCALA;
}
constexpr bool Use2ndKeyword(int lex) noexcept {
	return lex == LEX_OBJC;
}
constexpr bool Use2ndKeyword2(int lex) noexcept {
	return lex == LEX_CPP || lex == LEX_OBJC;
}
constexpr bool IsSpaceEquiv(int state) noexcept {
	// including SCE_C_DEFAULT, SCE_C_COMMENT, SCE_C_COMMENTLINE
	// SCE_C_COMMENTDOC, SCE_C_COMMENTLINEDOC, SCE_C_COMMENTDOC_TAG, SCE_C_COMMENTDOC_TAG_XML
	return (state < SCE_C_IDENTIFIER);
}
constexpr bool IsEscapeChar(int ch) noexcept {
	return ch == '\\' || ch == '\'' || ch == '\"';
}

/*const char* const cppWordLists[] = {
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

void ColouriseCppDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
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
	const int lexType = styler.GetPropertyInt("lexer.lang", LEX_CPP);

	Sci_Line lineCurrent = styler.GetLine(startPos);
	const int curLineState = (lineCurrent > 0) ? styler.GetLineState(lineCurrent - 1) : 0;
	int lineState = (curLineState >> 24);
	int numCBrace = (curLineState >> 18) & 0x3F;
	int numSBrace = (curLineState >> 13) & 0x1F;
	int numRBrace = (curLineState >> 8) & 0x1F;
	int curNcLevel = (curLineState >> 4) & 0x0F;
	int numDTSBrace = (curLineState) & 0x0F;
#define MakeState() ((lineState << 24)|(numCBrace << 18)|(numSBrace << 13)|(numRBrace << 8)|(curNcLevel << 4)|numDTSBrace)
#define UpdateLineState()	styler.SetLineState(lineCurrent, MakeState())
#define UpdateCurLineState() lineCurrent = styler.GetLine(sc.currentPos); \
								styler.SetLineState(lineCurrent, MakeState())
	int outerStyle = SCE_C_DEFAULT;
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
	bool isAssignStmt = false;

	if (initStyle == SCE_C_COMMENTLINE || initStyle == SCE_C_COMMENTLINEDOC || initStyle == SCE_C_PREPROCESSOR) {
		// Set continuationLine if last character of previous line is '\'
		if (lineCurrent > 0) {
			const char chBack = styler.SafeGetCharAt(startPos - 1);
			const char chBack2 = styler.SafeGetCharAt(startPos - 2);
			char lineEndChar = '!';
			if (chBack2 == '\r' && chBack == '\n') {
				lineEndChar = styler.SafeGetCharAt(startPos - 3);
			} else if (chBack == '\n' || chBack == '\r') {
				lineEndChar = chBack2;
			}
			continuationLine = lineEndChar == '\\';
		}
	}

	StyleContext sc(startPos, length, initStyle, styler);
	if (startPos == 0 && sc.Match('#', '!')) {
		// Shell Shebang at beginning of file
		sc.SetState(SCE_C_COMMENTLINE);
		sc.Forward();
	}

	while (sc.More()) {

		if (sc.atLineStart) {
			if (sc.state == SCE_C_STRING || sc.state == SCE_C_CHARACTER) {
				// Prevent SCE_C_STRINGEOL from leaking back to previous line which
				// ends with a line continuation by locking in the state upto this position.
				sc.SetState(sc.state);
			}
			// Reset states to begining of colourise so no surprises
			// if different sets of lines lexed.
			visibleChars = 0;
			docTagType = 0;
			lastWordWasUUID = false;
			lastWordWasGoto = false;
			lastPPDefineWord = 0;

			isPreprocessorWord = false;
			isPragmaPreprocessor = false;
			isIncludePreprocessor = false;
			isMessagePreprocessor = false;

			if (!continuationLine) {
				lineState &= LEX_BLOCK_UMASK_ALL;
			}
			UpdateCurLineState();
		}

		if (sc.atLineEnd) {
			UpdateLineState();
			lineCurrent++;
		}

		// Determine if the current state should terminate.
		switch (sc.state) {
		case SCE_C_OPERATOR:
			sc.SetState(SCE_C_DEFAULT);
			break;
		case SCE_C_NUMBER:
			if (!IsDecimalNumberEx(sc.chPrev, sc.ch, sc.chNext)) {
				if (sc.ch == '\'' && lexType == LEX_CPP) {
					sc.Forward();
				} else {
					sc.SetState(SCE_C_DEFAULT);
				}
			}
			break;

		case SCE_C_IDENTIFIER:
			if (!iswordstart(sc.ch)) {
				char s[256];
				sc.GetCurrent(s, sizeof(s));

				//int len = static_cast<int>(strlen(s));
				//bool pps = (len > 4 && s[0] == '_' && s[1] == '_' && s[len - 1] == '_' && s[len - 2] == '_');
				//char tu[256]{};
				//if (pps)
				//	strncpy(tu, s + 2, len - 4);
				// __attribute__()
				const bool hasAttr = (lexType == LEX_CPP || lexType == LEX_OBJC || isObjCSource);
				const bool mayAttr = lastWordWasAttr && (numRBrace > 0 || (lineState & LEX_BLOCK_MASK_DEFINE));
				const int nextChar = sc.GetDocNextChar();

				if (lastPPDefineWord) {
					if (lastPPDefineWord == 2 && StrEqual(s, "defined"))
						sc.ChangeState(SCE_C_WORD);
					else if (sc.ch == '(')
						sc.ChangeState(SCE_C_MACRO2);
					else
						sc.ChangeState(SCE_C_MACRO);
					if (lastPPDefineWord != 2)
						lastPPDefineWord = 0;
				} else if ((lineState & LEX_BLOCK_MASK_ASM) && kwAsmInstruction.InList(s)) {
					sc.ChangeState(SCE_C_ASM_INSTRUCTION);
					lastWordWasGoto = MakeLowerCase(s[0]) == 'j';
				} else if ((lineState & LEX_BLOCK_MASK_ASM) && kwAsmRegister.InList(s)) {
					sc.ChangeState(SCE_C_ASM_REGISTER);
				} else if (((s[0] == '#' || s[0] == '!') && keywords3.InList(s + 1))
					|| (isPreprocessorWord && keywords3.InList(s))) {
					sc.ChangeState(SCE_C_PREPROCESSOR);
					lastWordWasAttr = false;
					if (isPreprocessorWord) {
						isPreprocessorWord = false;
						char *ppw = s;
						if (s[0] == '#')
							ppw = s + 1;
						isPragmaPreprocessor = StrEqualsAny(ppw, "pragma", "line");
						isIncludePreprocessor = StrStartsWith(ppw, "include") || StrEqualsAny(ppw, "import", "using");
						isMessagePreprocessor = StrEqualsAny(ppw, "error", "warning", "message", "region", "endregion");
						if (StrEqual(ppw, "define")) {
							lineState |= LEX_BLOCK_MASK_DEFINE;
							lastPPDefineWord = 1;
						} else if (strstr(ppw, "if")) {
							lastPPDefineWord = 2;
						} else if (StrEqual(ppw, "undef")) {
							lastPPDefineWord = 3;
						}
					}
				} else if (isPragmaPreprocessor) {
					isPragmaPreprocessor = false;
					sc.ChangeState(SCE_C_PREPROCESSOR);
					isMessagePreprocessor = StrEqualsAny(s, "region", "endregion", "mark");
				} else if ((!hasAttr || mayAttr) && kwAttribute.InList(s)) {
					sc.ChangeState(SCE_C_ATTRIBUTE);
				} else if (keywords.InList(s)) {
					sc.ChangeState(SCE_C_WORD);
					if (isAssignStmt && chPrevNonWhite == '=' && StrEqualsAny(s, "function", "new")) {
						isAssignStmt = false;
					}
					// asm __asm _asm
					lastWordWasAsm = StrEqualsAny(s, "asm", "__asm");
					lastWordWasUUID = StrEqual(s, "uuid");
					lastWordWasGoto = StrEqualsAny(s, "goto", "__label__", "break", "continue");
					if (!isTypeDefine)
						isTypeDefine = StrEqual(s, "typedef");
					if (!lastWordWasAttr)
						lastWordWasAttr = StrEqualsAny(s, "__declspec", "__attribute__");
				} else if (keywords2.InList(s)) {
					sc.ChangeState(SCE_C_WORD2);
				} else if (s[0] == '@' && HasAnotation(lexType)) {
					sc.ChangeState(SCE_C_DIRECTIVE);
					while (iswordchar(sc.ch))
						sc.Forward();
				} else if (s[0] == '@' && keywords4.InList(s + 1)) {
					sc.ChangeState(SCE_C_DIRECTIVE);
					if (lexType == LEX_CPP || lexType == LEX_OBJC || isObjCSource) {
						if (!isObjCSource)
							isObjCSource = true;
						if (!lastWordWasAttr)
							lastWordWasAttr = StrEqual(s + 1, "property");
					}
				} else if (kwClass.InList(s)) {
					sc.ChangeState(SCE_C_CLASS);
				} else if (kwInterface.InList(s)) {
					sc.ChangeState(SCE_C_INTERFACE);
				} else if (kwEnumeration.InList(s)) {
					sc.ChangeState(SCE_C_ENUMERATION);
				} else if (nextChar == '(') {
					if (kwConstant.InListPrefixed(s, '('))
						sc.ChangeState(SCE_C_MACRO2);
					else
						sc.ChangeState(SCE_C_FUNCTION);
				} else if (kwConstant.InList(s)) {
					sc.ChangeState(SCE_C_CONSTANT);
				} else if ((isObjCSource || Use2ndKeyword(lexType)) && kw2ndKeyword.InList(s)) {
					sc.ChangeState(SCE_C_2NDWORD);
				} else if ((isObjCSource || Use2ndKeyword2(lexType)) && kw2ndKeyword2.InList(s)) {
					sc.ChangeState(SCE_C_2NDWORD2);
				} else if (lastWordWasGoto && (numCBrace > 0)) {
					sc.ChangeState(SCE_C_LABEL);
					lastWordWasGoto = false;
				} else if (sc.ch == ':' && sc.chNext != ':' && !(isAssignStmt)
					&& (numCBrace > 0 && numSBrace == 0 && numRBrace == 0)
					&& visibleChars == sc.LengthCurrent()) {
					sc.ChangeState(SCE_C_LABEL);
				} else if (iswordchar(s[0]) && (IsASpace(sc.ch) || sc.ch == '[' || sc.ch == ')' || sc.ch == '>'
					|| sc.ch == '*' || sc.ch == '&' || sc.ch == ':')) {
					bool is_class = false;
					Sci_PositionU pos = sc.currentPos;
					const int next_char = nextChar;

					if (sc.ch == ':' && sc.chNext == ':') { // C++
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
						unsigned char ch = styler.SafeGetCharAt(pos);
						const bool next_is_word = iswordstart(ch);
						if (next_char == ')' || next_char == '>') {
							if (next_is_word || (ch == '(')) {
								pos = styler.GetStartSegment() - 1;
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
								pos = styler.GetStartSegment() - 1;
								while (IsASpace(styler.SafeGetCharAt(pos))) pos--;
								ch = styler.SafeGetCharAt(pos);
								if (iswordchar(ch) || ch == ';' || ch == '{') {
									is_class = true;
								}
							}
						} else if (ch == ')' || ch == '>' || ch == '*') {
							is_class = true;
						} else if (ch == ']' && SquareBraceAfterType(lexType)) {
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
			} else if (sc.ch == '{' && sc.chNext == '@' && IsAlpha(sc.GetRelative(2))) {
				docTagType = DOC_TAG_INLINE_AT;
				outerStyle = sc.state;
				sc.SetState(SCE_C_COMMENTDOC_TAG);
				sc.Forward();
			} else if ((sc.ch == '@' || sc.ch == '\\') && IsAlpha(sc.chNext) && IsCommentTagPrev(sc.chPrev)) {
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

		case SCE_C_CHARACTER:
			if (sc.atLineEnd) {
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
			} else if (sc.ch == '\"') {
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

		case SCE_C_TRIPLEVERBATIM:
			if (sc.ch == '\\' && IsEscapeChar(sc.chNext)) {
				sc.Forward();
			} else if (sc.Match('"', '"', '"')) {
				sc.Advance(2);
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
		case SCE_C_XML_TAG:	// Scala
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

		// Determine if a new state should be entered.
			if (sc.state == SCE_C_DEFAULT) {
					if (HasTripleVerbatim(lexType) && sc.Match('"', '"', '"')) {
						sc.SetState(SCE_C_TRIPLEVERBATIM);
						sc.Advance(2);
					} else if (sc.ch == '/' && (sc.chNext == '/' || sc.chNext == '*')) {
						const int chNext = sc.chNext;
						sc.SetState((chNext == '/') ? SCE_C_COMMENTLINE : SCE_C_COMMENT);
						sc.Forward(2);
						if (sc.ch == '!' || (sc.ch == chNext && sc.chNext != chNext)) {
							sc.ChangeState((chNext == '/') ? SCE_C_COMMENTLINEDOC : SCE_C_COMMENTDOC);
						}
						continue;
					} else if (IsNumberStart(sc.ch, sc.chNext)) {
						sc.SetState(SCE_C_NUMBER);
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
							sc.Advance(offset);
						}
					} else if (lexType == LEX_CPP && sc.ch == 'R' && sc.chNext == '\"') {
						sc.SetState(SCE_C_STRINGRAW);
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
					} else if (HasPreprocessor(lexType) && sc.ch == '#') {
						if (lineState & LEX_BLOCK_MASK_DEFINE) {
							sc.SetState(SCE_C_OPERATOR);
							if (sc.chNext == '#' || sc.chNext == '@') {
								sc.Advance(2);
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
					} else if (iswordstart(sc.ch) || (iswordstart(sc.chNext) && (sc.ch == '@'))) {
						sc.SetState(SCE_C_IDENTIFIER);
					} else if (sc.ch == '.') {
						sc.SetState(SCE_C_OPERATOR);
						if (sc.chNext == '.')
							sc.Forward();
					} else if (sc.ch == '<') {
						if (HasXML(lexType) && chPrevNonWhite == '=') {
							sc.SetState(SCE_C_XML_TAG);
							if (sc.chNext != '/') {
								++curNcLevel;
							}
							sc.Forward();
						} else if (isIncludePreprocessor) {
							sc.SetState(SCE_C_STRING);
						} else {
							sc.SetState(SCE_C_OPERATOR);
						}
					} else if (isoperator(sc.ch) || sc.ch == '@') {
						sc.SetState(SCE_C_OPERATOR);
						isPragmaPreprocessor = false;

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
				sc.Forward();
				continuationLine = true;
				continue;
			}
		}

		if (!(isspacechar(sc.ch) || IsSpaceEquiv(sc.state))) {
			chPrevNonWhite = sc.ch;
			visibleChars++;
		}
		continuationLine = false;
		sc.Forward();
	}

	sc.Complete();
}

bool IsCppDefineLine(LexAccessor &styler, Sci_Line line, Sci_Position &DefinePos) noexcept {
	Sci_Position pos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	pos = LexSkipSpaceTab(styler, pos, endPos);
	if (styler[pos] == '#' && styler.StyleAt(pos) == SCE_C_PREPROCESSOR) {
		pos++;
		pos = LexSkipSpaceTab(styler, pos, endPos);
		if (styler.Match(pos, "define") && IsASpaceOrTab(styler[pos + 6])) {
			DefinePos = pos + 6;
			return true;
		}
	}
	return false;
}

}

// also used in LexAsm.cxx
bool Lexilla::IsCppInDefine(LexAccessor &styler, Sci_Position currentPos) noexcept {
	Sci_Line line = styler.GetLine(currentPos);
	Sci_Position pos;
	if (IsCppDefineLine(styler, line, pos)) {
		if (pos < currentPos)
			return true;
	}
	line--;
	while (line > 0 && IsBackslashLine(styler, line) && !IsCppDefineLine(styler, line, pos))
		line--;
	if (line >= 0 && IsCppDefineLine(styler, line, pos) && IsBackslashLine(styler, line)) {
		return true;
	}
	return false;
}

namespace {

bool IsCppFoldingLine(LexAccessor &styler, Sci_Line line, int kind) noexcept {
	if (line < 0) {
		return false;
	}
	const Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	Sci_Position pos = LexSkipSpaceTab(styler, startPos, endPos);
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
		pos = LexSkipSpaceTab(styler, pos, endPos);
		ch = styler[pos];
		if (kind == 3 && (ch == 'i' || ch == 'u')) {
			return styler.Match(pos, "include") || styler.Match(pos, "using") || styler.Match(pos, "import");
		} else if (kind == 4 && ch == 'd') {
			return styler.Match(pos, "define") && !IsBackslashLine(styler, line); // multi-line #define
		} else if (kind == 5 && ch == 'u') {
			return styler.Match(pos, "undef");
		}
		return false;
	}
}
#define IsCommentLine(line)		IsLexCommentLine(styler, line, MultiStyle(SCE_C_COMMENTLINE, SCE_C_COMMENTLINEDOC))
//#define IsCommentLine(line)		IsCppFoldingLine(styler, line, 0)
#define IsUsingLine(line)		IsCppFoldingLine(styler, line, 1)
//#define IsPropertyLine(line)	IsCppFoldingLine(styler, line, 2)
#define IsIncludeLine(line)		IsCppFoldingLine(styler, line, 3)
#define IsDefineLine(line)		IsCppFoldingLine(styler, line, 4)
#define IsUnDefLine(line)		IsCppFoldingLine(styler, line, 5)

constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_C_COMMENT || style == SCE_C_COMMENTDOC;
}
constexpr bool IsInnerCommentStyle(int style) noexcept {
	return style == SCE_C_COMMENT
		|| style == SCE_C_COMMENTDOC
		|| style == SCE_C_COMMENTDOC_TAG
		|| style == SCE_C_COMMENTDOC_TAG_XML;
}
bool IsOpenBraceLine(LexAccessor &styler, Sci_Line line) noexcept {
	// above line
	Sci_Position startPos = styler.LineStart(line - 1);
	Sci_Position endPos = styler.LineStart(line) - 1;
	Sci_Position pos = LexSkipSpaceTab(styler, startPos, endPos);
	char ch = styler[pos];
	int stl = styler.StyleAt(pos);
	if (IsEOLChar(ch) || IsSpaceEquiv(stl) || (stl == SCE_C_PREPROCESSOR || stl == SCE_C_XML_TAG))
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
	pos = LexSkipSpaceTab(styler, startPos, endPos);
	// only '{' line
	if (styler.StyleAt(pos) == SCE_C_OPERATOR && styler[pos] == '{') {
		return true;
		//pos++;
		//pos = LexSkipSpaceTab(pos, endPos, styler);
		//ch = styler[pos];
		//stl = styler.StyleAt(pos);
		//return (ch == '\n') || (ch == '\r') || IsSpaceEquiv(stl);
	}
	return false;
}

void FoldCppDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList, Accessor &styler) {
	const int lexType = styler.GetPropertyInt("lexer.lang", LEX_CPP);
	const bool hasPreprocessor = HasPreprocessor(lexType);

	const Sci_PositionU endPos = startPos + length;
	int visibleChars = 0;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	int style = initStyle;
	int styleEOL = initStyle;
	int styleNext = styler.StyleAt(startPos);
	bool isObjCProtocol = false;
	bool lineCommentCurrent = IsCommentLine(lineCurrent);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);
		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');

		{
			if (lineCommentCurrent) {
				if (atEOL) {
					levelNext += IsCommentLine(lineCurrent + 1) - IsCommentLine(lineCurrent - 1);
				}
			}
			else if (IsStreamCommentStyle(style)) {
				if (!IsInnerCommentStyle(stylePrev)) {
					levelNext++;
				} else if (!IsInnerCommentStyle(styleNext)) {
					levelNext--;
				}
			}
		}
		if (style == SCE_C_TRIPLEVERBATIM) {
			if (stylePrev != style) {
				levelNext++;
			} else if (styleNext != style) {
				levelNext--;
			}
		}

		if (atEOL && IsUsingLine(lineCurrent)) {
			levelNext += IsUsingLine(lineCurrent + 1) - IsUsingLine(lineCurrent - 1);
		}
		if (hasPreprocessor && atEOL) {
			if (IsIncludeLine(lineCurrent)) {
				levelNext += IsIncludeLine(lineCurrent + 1) - IsIncludeLine(lineCurrent - 1);
			}
			else if (IsDefineLine(lineCurrent)) {
				levelNext += IsDefineLine(lineCurrent + 1) - IsDefineLine(lineCurrent - 1);
			}
			else if (IsUnDefLine(lineCurrent)) {
				levelNext += IsUnDefLine(lineCurrent + 1) - IsUnDefLine(lineCurrent - 1);
			}
			//else if (IsPropertyLine(lineCurrent)) {
			//	levelNext += IsPropertyLine(lineCurrent + 1) - IsPropertyLine(lineCurrent - 1);
			//}
		}

		if (atEOL && !IsStreamCommentStyle(style) && !IsStreamCommentStyle(styleEOL)) {
			levelNext += IsBackslashLine(styler, lineCurrent) - IsBackslashLine(styler, lineCurrent - 1);
		}
		if (atEOL && !(hasPreprocessor && IsCppInDefine(styler, i)) && IsOpenBraceLine(styler, lineCurrent + 1)) {
			levelNext++;
		}

		if (hasPreprocessor && ch == '#' && style == SCE_C_PREPROCESSOR) {
			Sci_Position pos = LexSkipSpaceTab(styler, i + 1, endPos);
			if (styler.Match(pos, "if") || styler.Match(pos, "region")) {
				levelNext++;
			} else if (styler.Match(pos, "end")) {
				levelNext--;
			} else if (styler.Match(pos, "pragma")) { // #pragma region, #pragma endregion
				pos = LexSkipSpaceTab(styler, pos + 7, endPos);
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
				if (LexGetNextChar(styler, i + 9) != '(') { // @protocol()
					isObjCProtocol = true;
					levelNext++;
				}
			}
		}

		if (style == SCE_C_OPERATOR && !(hasPreprocessor && IsCppInDefine(styler, i))) {
			// maybe failed in multi-line define section, MFC's afx.h is a example
			if (ch == '{' && !(lineCurrent > 0 && visibleChars == 0 && IsOpenBraceLine(styler, lineCurrent))) {
				levelNext++;
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

		if (visibleChars == 0 && !isspacechar(ch))
			visibleChars++;
		if (atEOL || (i == endPos - 1)) {
			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext)
				lev |= SC_FOLDLEVELHEADERFLAG;
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			levelCurrent = levelNext;
			visibleChars = 0;
			isObjCProtocol = false;
			styleEOL = style;
			lineCommentCurrent = IsCommentLine(lineCurrent);
		}
	}
}

}

LexerModule lmCPP(SCLEX_CPP, ColouriseCppDoc, "cpp", FoldCppDoc);
