// Lexer for C, C++, C#, Java, Rescouce Script, Asymptote, D, Objective C/C++
// JavaScript, JScript, ActionScript, haXe, Groovy, Jamfile, AWK, IDL/ODL/AIDL

#include <string.h>
#include <assert.h>
#include <ctype.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "CharacterSet.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "LexerModule.h"
#include "SparseState.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

#define		LEX_CPP		1	// C/C++
#define		LEX_JAVA	2	// Java
#define 	LEX_CS		3	// C#
#define 	LEX_JS		4	// JavaScript
#define 	LEX_RC		5	// Resouce Script
#define 	LEX_D		6	// D
#define 	LEX_ASY		7	// Asymptote
#define		LEX_OBJC	21	// Objective C/C++
#define		LEX_GROOVY	22	// Groovy Script
#define 	LEX_AS		23	// ActionScript
#define 	LEX_HX		24	// haXe
#define		LEX_ILASM	25	// Commaon Language Infrastructure
#define		LEX_IDL		26	// Interface Definition Language
#define		LEX_AIDL	27	// Android Interface Definition Language
#define		LEX_JAM		28	// Jamfile
#define		LEX_AWK		29	// Awk
#define		LEX_DOT		30	// GraphViz Dot

static bool _hasPreprocessor(int lex) { // #[space]preprocessor
	switch (lex) {
		case LEX_CPP:	case LEX_OBJC:	case LEX_RC:	case LEX_CS:	case LEX_ILASM:	case LEX_IDL:
			return true;
	}
	return false;
}
static bool _hasAnotation(int lex) { // @anotation
	return lex == LEX_JAVA || lex == LEX_GROOVY || lex == LEX_AIDL;
}
static bool _hasRegex(int lex) { // /regex/
	switch (lex) {
		case LEX_JS:	case LEX_AS:	case LEX_HX:	case LEX_GROOVY:	case LEX_AWK:
			return true;
	}
	return false;
}
static bool _hasVerbatim(int lex) { // @"string"
	return lex == LEX_CS || lex == LEX_CPP || lex == LEX_OBJC;
}
static bool _sharpComment(int lex) {
	return lex == LEX_AWK || lex == LEX_JAM;
}
static bool _hasComplex(int lex) {
	return lex == LEX_CPP || lex == LEX_OBJC || lex == LEX_D;
}
#define _allowUnderscore	_hasComplex
static bool _hasHexFloat(int lex) {
	return _hasComplex(lex) || _hasAnotation(lex);
}
#define	_hasBinNum	_hasHexFloat
static bool _hasOctNum(int lex) {
	return _hasHexFloat(lex) || lex == LEX_RC;
}
static bool _hasHexNum(int lex) {
	return _hasOctNum(lex) || _hasRegex(lex);
}
static bool _hasLong(int lex) {
	return _hasPreprocessor(lex) || _hasAnotation(lex) || lex == LEX_D;
}
static bool _hasUnsigned(int lex) {
	return _hasPreprocessor(lex) || lex == LEX_D;
}
static bool _hasLongLong(int lex) {
	return lex == LEX_CPP || lex == LEX_OBJC;
}
static bool _has_iInt(int lex) {
	return lex == LEX_CPP || lex == LEX_IDL;
}
static bool IsFltFix(int ch, int lex) {
	ch = MakeUpperCase(ch);
	if (ch >= 0x80)
		return false;
	switch (lex) {
		case LEX_CPP:	case LEX_OBJC:
			return ch == 'F' || ch == 'L' || ch == 'Q' || ch == 'W'; break;
		case LEX_JAVA:	case LEX_GROOVY:	case LEX_AIDL:
			return ch == 'F' || ch == 'D'; break;
		case LEX_CS:	case LEX_ILASM:
			return ch == 'F' || ch == 'D' || ch == 'M';	break;
		case LEX_RC:
			return ch == 'F'; break;
		case LEX_D:		case LEX_IDL:
			return ch == 'F' || ch == 'L';	break;
		default:
			return false;
	}
}
static bool IsCmpFix(int ch) {
	return (ch < 0x80) && (ch == 'i' || ch == 'j' || ch == 'I' || ch == 'J');
}
static bool IsDStrFix(int ch) {
	return (ch < 0x80) && (ch == 'c' || ch == 'w' || ch == 'd');
}
static bool _use2ndOnly(int lex) {
	return lex == LEX_DOT;
}
static bool _hasAttribute(int lex) {
	return lex == LEX_CS || lex == LEX_ILASM;
}
static bool _use2ndKeyword(int lex) {
	return lex == LEX_OBJC || lex == LEX_GROOVY || lex == LEX_ILASM || lex == LEX_DOT;
}
static bool _use2ndKeyword2(int lex) {
	return lex == LEX_OBJC || lex == LEX_AIDL || lex == LEX_ILASM;
}
#define	strequ(str1, str2)	(strcmp(str1, str2) == 0)
static bool IsSpaceEquiv(int state) {
	// including SCE_C_DEFAULT, SCE_C_COMMENT, SCE_C_COMMENTLINE
	return (state < SCE_C_IDENTIFIER) || (state == SCE_C_DNESTEDCOMMENT);
}

/*static const char* const cppWordLists[] = {
	"Primary keywords",		// SCE_C_WORD
	"Type keywords",		// SCE_C_WORD2
	"Preprocessor",			// SCE_C_PREPROCESSOR	#preprocessor
	"Directive",			// SCE_C_DIRECTIVE		@directive @anotation
	// global
	"Attribute",			// SCE_C_GATTRIBUTE		[attribute]
	"Class",				// SCE_C_GCLASS
	"Interface",			// SCE_C_GINTERFACE
	"Enumeration",			// SCE_C_GENUMERATION
	"Constant",				// SCE_C_GCONSTANT
	"2nd Language Keyword"		// SCE_C_2NDWORD
	"2nd Language Type Keyword"	// SCE_C_2NDWORD2
	"Inline Asm Instruction"	// SCE_C_ASM_INSTRUCTION
	"Inline Asm Register"		// SCE_C_ASM_REGISTER
	0
};*/

#define SCE_C_VARIABLE2	60

static void ColouriseCppDoc(unsigned int startPos, int length, int initStyle, WordList *keywordLists[], Accessor &styler) {
	WordList &keywords		= *keywordLists[0];
	WordList &keywords2		= *keywordLists[1];
	WordList &keywords3		= *keywordLists[2];
	WordList &keywords4		= *keywordLists[3];
	// global
	WordList &kwAttribute	= *keywordLists[4];
	WordList &kwClass		= *keywordLists[5];
	WordList &kwInterface	= *keywordLists[6];
	WordList &kwEnumeration	= *keywordLists[7];
	WordList &kwConstant	= *keywordLists[8];
	// 2nd
	WordList &kw2ndKeyword	= *keywordLists[9];
	WordList &kw2ndKeyword2	= *keywordLists[10];
	WordList &kwAsmInstruction	 = *keywordLists[11];
	WordList &kwAsmRegister		 = *keywordLists[12];

	static bool isObjCSource = false;
	static bool isILAsmSource = false;
	const int lexType = styler.GetPropertyInt("lexer.2nd.type", LEX_CPP);
	//const bool stylingWithinPreprocessor = true;
	//const bool triplequotedStrings = false;
	//const bool hashquotedStrings = false;

	int lineCurrent = styler.GetLine(startPos);
	SparseState<std::string> rawStringTerminators;
	std::string rawStringTerminator = rawStringTerminators.ValueAt(lineCurrent-1);
	SparseState<std::string> rawSTNew(lineCurrent);
	int curLineState =  (lineCurrent > 0) ? styler.GetLineState(lineCurrent-1) : 0;
	int lineState = 	(curLineState >> 24);
	int numCBrace =		(curLineState >> 18) & 0x3F;
	int numSBrace = 	(curLineState >> 13) & 0x1F;
	int numRBrace = 	(curLineState >>  8) & 0x1F;
	int curNcLevel = 	(curLineState >>  4) & 0x0F;
	int numDTSBrace =	(curLineState      ) & 0x0F;
#define _MakeState() ((lineState << 24)|(numCBrace << 18)|(numSBrace << 13)|(numRBrace << 8)|(curNcLevel << 4)|numDTSBrace)
#define _UpdateLineState()	styler.SetLineState(lineCurrent, _MakeState())

	CharacterSet setWord(CharacterSet::setAlphaNum, "._", 0x80, true);
	CharacterSet setWordStart(CharacterSet::setAlpha, "_@", 0x80, true);
	CharacterSet setOKBeforeRE(CharacterSet::setNone, "([{=,:;!%^&*|?~+-");
	CharacterSet setCouldBePostOp(CharacterSet::setNone, "+-");
	//CharacterSet setDoxygen(CharacterSet::setAlpha, "$@\\&<>#{}[]");
	if (lexType == LEX_D || lexType == LEX_HX)
		setWordStart.Add('#');
	if (lexType == LEX_JAM)
		setWord.Add('-');

	int chPrevNonWhite = ' ';
	int visibleChars = 0;
	bool lastWordWasUUID = false;
	bool lastWordWasGoto = false;
	bool lastWordWasAsm = false;
	//int styleBeforeDCKeyword = SCE_C_DEFAULT;
	bool continuationLine = false;
	bool isPragmaPreprocessor = false;
	bool isPreprocessorWord = false;
	bool isIncludePreprocessor = false;
	bool isMessagePreprocessor = false;
	int radix = 0;
	bool isFloatNumber = false;
	bool followsReturn = false;
	bool followsPostfixOperator = false;

	if (initStyle == SCE_C_COMMENTLINE || initStyle == SCE_C_PREPROCESSOR) {
		// Set continuationLine if last character of previous line is '\'
		if (lineCurrent > 0) {
			int chBack = styler.SafeGetCharAt(startPos-1, 0);
			int chBack2 = styler.SafeGetCharAt(startPos-2, 0);
			int lineEndChar = '!';
			if (chBack2 == '\r' && chBack == '\n') {
				lineEndChar = styler.SafeGetCharAt(startPos-3, 0);
			} else if (chBack == '\n' || chBack == '\r') {
				lineEndChar = chBack2;
			}
			continuationLine = lineEndChar == '\\';
		}
	}

	// look back to set chPrevNonWhite properly for better regex colouring
	if (startPos > 0) {
		int back = startPos;
		while (--back && IsSpaceEquiv(styler.StyleAt(back)));

		if (styler.StyleAt(back) == SCE_C_OPERATOR) {
			chPrevNonWhite = styler.SafeGetCharAt(back);
		}
	}

	StyleContext sc(startPos, length, initStyle, styler, 127);

	for (; sc.More(); sc.Forward()) {

		if (sc.atLineStart) {
			if ((sc.state == SCE_C_STRING) || (sc.state == SCE_C_CHARACTER)) {
				// Prevent SCE_C_STRINGEOL from leaking back to previous line which
				// ends with a line continuation by locking in the state upto this position.
				sc.SetState(sc.state);
			}
			// Reset states to begining of colourise so no surprises
			// if different sets of lines lexed.
			visibleChars = 0;
			lastWordWasUUID = false;
			lastWordWasGoto = false;
			followsReturn = false;
			followsPostfixOperator = false;

			isPreprocessorWord = false;
			isPragmaPreprocessor = false;
			isIncludePreprocessor = false;
			isMessagePreprocessor = false;

			if (!continuationLine) {
				lineState &= 0xFFFD;
			}
		}

		if (sc.atLineEnd) {
			_UpdateLineState();

			lineCurrent++;
			if (rawStringTerminator != "") {
				rawSTNew.Set(lineCurrent-1, rawStringTerminator);
			}
		}

		// Handle line continuation generically.
		if (sc.ch == '\\') {
			if (sc.chNext == '\n' || sc.chNext == '\r') {
				sc.Forward();
				if (sc.ch == '\r' && sc.chNext == '\n') {
					sc.Forward();
				}
				continuationLine = true;
				continue;
			}
		}

		//const bool atLineEndBeforeSwitch = sc.atLineEnd;

		// Determine if the current state should terminate.
		switch (sc.state) {
			case SCE_C_OPERATOR:
				sc.SetState(SCE_C_DEFAULT);
				break;
			case SCE_C_NUMBER:
				if (!IsADigit(sc.ch, radix)) {
					if (sc.ch == '.' && sc.chNext != '.') {
						if (sc.chNext == '_') {
							if (_allowUnderscore(lexType)) // C/Asm
								sc.Forward();
							else
								sc.ChangeState(SCE_C_DEFAULT);
						} else {
							if ((radix == 10 && IsADigit(sc.chNext,10))
								|| (_hasHexFloat(lexType) && radix == 16 && IsADigit(sc.chNext, 16))) {
								isFloatNumber = true;
								sc.Forward();
							} else {
								sc.SetState(SCE_C_DEFAULT);
							}
						}
					} else if (((radix == 10
							&& (sc.ch == 'e' || sc.ch == 'E') && !(sc.chNext == 'e' || sc.chNext == 'E'))
						|| (_hasHexFloat(lexType) && radix == 16
							&& (sc.ch == 'p' || sc.ch == 'P') && !(sc.chNext == 'p' || sc.chNext == 'P')))
						&& (sc.chNext == '+' || sc.chNext == '-' || IsADigit(sc.chNext, radix))) {
						isFloatNumber = true;
						sc.Forward();
						if (sc.ch == '+' || sc.ch == '-')
							sc.Forward();
					} else if (sc.ch == '_') {
						if (_allowUnderscore(lexType))
							sc.Forward();
						else if (_hasAnotation(lexType) && (IsADigit(sc.chPrev, radix) || sc.chPrev == '_')
							&& (IsADigit(sc.chNext, radix) || sc.chNext == '_'))
							sc.Forward();
						else
							sc.ChangeState(SCE_C_DEFAULT);
					} else if (isFloatNumber) { // float
						isFloatNumber = false;
						if (IsFltFix(sc.ch, lexType)) {
							sc.Forward();
							if (_hasComplex(lexType) && IsCmpFix(sc.ch))
								sc.Forward();
							sc.SetState(SCE_C_DEFAULT);
						} else if (_hasComplex(lexType) && IsCmpFix(sc.ch)) {
							sc.Forward();
							if (IsFltFix(sc.ch, lexType))
								sc.Forward();
							sc.SetState(SCE_C_DEFAULT);
						} else {
							sc.SetState(SCE_C_DEFAULT);
						}
					} else { // integer
						if (_hasUnsigned(lexType) && (sc.ch == 'u' || sc.ch == 'U')) {
							sc.Forward();
							if (_hasLong(lexType) && (sc.ch == 'L' || sc.ch == 'l')) {
								sc.Forward();
								if (_hasLongLong(lexType) && (sc.ch == 'L' || sc.ch == 'l'))
									sc.Forward();
							} else if (_has_iInt(lexType) && (sc.ch == 'i' || sc.ch == 'I') && IsADigit(sc.chNext,10)) {
								sc.Forward();
							}
							sc.SetState(SCE_C_DEFAULT);
						} else if (_hasLong(lexType) && (sc.ch == 'L' || sc.ch == 'l')) {
							sc.Forward();
							if (_hasLongLong(lexType) && (sc.ch == 'L' || sc.ch == 'l') && !(sc.chNext == 'L' || sc.chNext == 'l'))
								sc.Forward();
							if (_hasUnsigned(lexType) && (sc.ch == 'u' || sc.ch == 'U'))
								sc.Forward();
							sc.SetState(SCE_C_DEFAULT);
						} else if (IsFltFix(sc.ch, lexType)) {
							sc.ForwardSetState(SCE_C_DEFAULT);
						} else if (_has_iInt(lexType ) && (sc.ch == 'i' || sc.ch == 'I') && IsADigit(sc.chNext,10)) {
							sc.Forward();
						} else if (_hasComplex(lexType) && IsCmpFix(sc.ch)) {
							sc.ForwardSetState(SCE_C_DEFAULT);
						} else if  ((lineState & 0x0001) && (sc.ch == 'h' || sc.ch == 'H' || sc.ch == 'b' || sc.ch == 'B')) {
							sc.ForwardSetState(SCE_C_DEFAULT);
						} else {
							sc.SetState(SCE_C_DEFAULT);
						}
					}
				}
				break;

			case SCE_C_IDENTIFIER:
_lable_identifier:
				if (!setWord.Contains(sc.ch) || sc.ch == '.') {
					char s[256];
					if (lexType == LEX_DOT)
						sc.GetCurrentLowered(s, sizeof(s));
					else
						sc.GetCurrent(s, sizeof(s));

					if (((lineState & 0x0001) || (lexType == LEX_HX)) && kwAsmInstruction.InList(s)) {
						sc.ChangeState(SCE_C_ASM_INSTRUCTION);
						lastWordWasGoto = s[0] == 'j';
					} else if (((lineState & 0x0001) || (lexType == LEX_HX)) && kwAsmRegister.InList(s)) {
						sc.ChangeState(SCE_C_ASM_REGISTER);
					} else if ((lexType == LEX_D) && (lineState & 0x0001) && kw2ndKeyword.InList(s)) {
						sc.ChangeState(SCE_C_2NDWORD);
					} else if ((s[0] == '#' && keywords3.InList(&s[1])) || (isPreprocessorWord && keywords3.InList(s))) {
						sc.ChangeState(SCE_C_PREPROCESSOR);
						if (isPreprocessorWord) {
							isPreprocessorWord = false;
							char *ppw = s;
							if (s[0] == '#')
								ppw = &s[1];
							isPragmaPreprocessor = (ppw[0] == 'p' && strequ(&ppw[1], "ragma"));
							isIncludePreprocessor = (ppw[0] == 'i' && (strcmp(&ppw[1], "nclude") >= 0 || strequ(&ppw[1], "mport")))
								|| (ppw[0] == 'u' && strequ(&ppw[1], "sing"));
							isMessagePreprocessor = (ppw[0] == 'e' && (strequ(&ppw[1], "rror") || strequ(&ppw[1], "ndregion")))
								|| (ppw[0] == 'w' && strequ(&ppw[1], "arning")) || (ppw[0] == 'r' && strequ(&ppw[1], "egion"))
								|| (ppw[0] == 'm' && strequ(&ppw[1], "essage"));
							if (ppw[0] == 'd' && strequ(&ppw[1], "efine"))
								lineState |= 0x0002;
						}
					} else if (isPragmaPreprocessor) {
						isPragmaPreprocessor = false;
						sc.ChangeState(SCE_C_PREPROCESSOR);
						isMessagePreprocessor = (s[0] == 'r' && strequ(&s[1], "egion"))
							|| (s[0] == 'e' && strequ(&s[1], "ndregion"));
					} else if (s[0] == '@' && keywords4.InList(&s[1])) {
						sc.ChangeState(SCE_C_DIRECTIVE);
						if (lexType == LEX_CPP || lexType == LEX_OBJC)
							isObjCSource = true;
					} else if (!_use2ndOnly(lexType) && keywords.InList(s)) {
						sc.ChangeState(SCE_C_WORD);
						lastWordWasAsm = (s[0] == 'a' && strequ(&s[1], "sm")) || (s[0] == '_' && strequ(&s[1], "_asm"));
						lastWordWasUUID = (s[0] == 'u' && strequ(&s[1], "uid"));
						lastWordWasGoto = (s[0] == 'g' && strequ(&s[1], "oto")) || (s[0] == '_' && strequ(&s[1], "_label__"))
							|| (s[0] == 'b' && strequ(&s[1], "reak")) || (s[0] == 'c' && strequ(&s[1], "ontinue"));
						followsReturn = (s[0] == 'r' && strequ(&s[1], "eturn"));
					} else if (!_use2ndOnly(lexType) && keywords2.InList(s)) {
						sc.ChangeState(SCE_C_WORD2);
					} else if ((lexType == LEX_CPP || lexType == LEX_JAM
						|| (_hasAttribute(lexType) && numSBrace > 0)) && kwAttribute.InList(s)) {
						sc.ChangeState(SCE_C_GATTRIBUTE);
					} else if (kwClass.InList(s)) {
						sc.ChangeState(SCE_C_GCLASS);
					} else if (kwInterface.InList(s) || (_hasAnotation(lexType) && keywords4.InList(s))) {
						sc.ChangeState(SCE_C_GINTERFACE);
					} else if (kwEnumeration.InList(s)) {
						sc.ChangeState(SCE_C_GENUMERATION);
					} else if (kwConstant.InList(s)) {
						sc.ChangeState(SCE_C_GCONSTANT);
					} else if ((isObjCSource || isILAsmSource || _use2ndKeyword(lexType)) && kw2ndKeyword.InList(s)) {
						sc.ChangeState(SCE_C_2NDWORD);
					} else if ((isObjCSource || isILAsmSource || _use2ndKeyword2(lexType)) && kw2ndKeyword2.InList(s)) {
						sc.ChangeState(SCE_C_2NDWORD2);
					} else if (lastWordWasGoto) {
						sc.ChangeState(SCE_C_LABEL);
						lastWordWasGoto = false;
					} else if (sc.ch == ':' && sc.chNext != ':' && numCBrace > 0 && numSBrace == 0 && numRBrace == 0
						&& visibleChars == static_cast<int>(strlen(s))) {
						sc.ChangeState(SCE_C_LABEL);
					} else if (lexType == LEX_DOT) {
						int pos = sc.currentPos;
						while (IsASpaceOrTab(styler[pos++]));
						if (styler[pos-1] == '=')
							sc.ChangeState(SCE_C_GATTRIBUTE);
					}

					const bool literalString = sc.ch == '\"';
					if ((lexType == LEX_CPP) && (literalString || sc.ch == '\'')) {
						size_t lenS = strlen(s);
						const bool raw = literalString && sc.chPrev == 'R';
						if (raw)
							s[lenS--] = '\0';
						bool valid =
							(lenS == 0) ||
							((lenS == 1) && ((s[0] == 'L') || (s[0] == 'u') || (s[0] == 'U'))) ||
							((lenS == 2) && literalString && (s[0] == 'u') && (s[1] == '8'));
						if (valid) {
							if (literalString)
								sc.ChangeState((raw ? SCE_C_STRINGRAW : SCE_C_STRING));
							else
								sc.ChangeState(SCE_C_CHARACTER);
						}
					}

					if ((isIncludePreprocessor || isMessagePreprocessor) && !sc.atLineEnd) {
						sc.SetState(SCE_C_STRING);
					} else if (lastWordWasUUID && sc.ch == '(' && sc.chNext != '\"') {
						sc.SetState(SCE_C_OPERATOR);
						sc.ForwardSetState(SCE_C_UUID);
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
			case SCE_C_DNESTEDCOMMENT:
				if (sc.Match('+', '/')) {
					if (curNcLevel > 0)
						--curNcLevel;
					sc.Forward();
					if (curNcLevel == 0) {
						sc.ForwardSetState(SCE_C_DEFAULT);
					}
				} else if (sc.Match('/','+')) {
					++curNcLevel;
					sc.Forward();
				}
				break;

			case SCE_C_STRING:
				if (sc.atLineEnd) {
					if (lexType == LEX_ASY || lexType == LEX_JAM)
						continue;
					else
						sc.ChangeState(SCE_C_STRINGEOL);
				} else if (isIncludePreprocessor) {
					if (sc.ch == '>') {
						sc.ForwardSetState(SCE_C_DEFAULT);
						isIncludePreprocessor = false;
					}
				} else if (isMessagePreprocessor) {
					if (sc.ch == '\"') {
						sc.Forward();
						isMessagePreprocessor = false;
					}
				} else if (sc.ch == '\\') {
					if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
						sc.Forward();
					}
				} else if (sc.ch == '\"') {
					if ((lexType == LEX_D) && IsDStrFix(sc.chNext))
						sc.Forward();
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
				break;
			//case SCE_C_HASHQUOTEDSTRING:
			//	if (sc.ch == '\\') {
			//		if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
			//			sc.Forward();
			//		}
			//	} else if (sc.ch == '\"') {
			//		sc.ForwardSetState(SCE_C_DEFAULT);
			//	}
			//	break;
			case SCE_C_STRINGRAW:
				if (sc.Match(rawStringTerminator.c_str())) {
					for (size_t termPos=rawStringTerminator.size(); termPos; termPos--)
						sc.Forward();
					sc.SetState(SCE_C_DEFAULT);
					rawStringTerminator = "";
				}
				break;

			case SCE_C_DSTRINGX:
				if (sc.ch == '\"') {
					if (IsDStrFix(sc.chNext))
						sc.Forward();
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
				break;
			case SCE_C_DSTRINGB:
				if (sc.ch == '`') {
					if (IsDStrFix(sc.chNext))
						sc.Forward();
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
				break;
			case SCE_C_DSTRINGQ:
				if (sc.ch == '\"') {
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
				break;
			case SCE_C_DSTRINGT:
				if (sc.ch == '}') {
					if (numDTSBrace > 0)
						--numDTSBrace;
					if (numDTSBrace == 0)
						sc.ForwardSetState(SCE_C_DEFAULT);
				} else if (sc.ch == '{') {
					++numDTSBrace;
				}
				break;

			case SCE_C_VARIABLE2:
				if (sc.ch == ')') {
					if (numDTSBrace > 0)
						--numDTSBrace;
					if (numDTSBrace == 0)
						sc.ForwardSetState(SCE_C_DEFAULT);
				} else if (sc.ch == '(') {
					++numDTSBrace;
				}
				break;
			case SCE_C_VARIABLE:
				if (!setWord.Contains(sc.ch) || sc.ch == '.') {
					sc.SetState(SCE_C_DEFAULT);
				}
				break;

			case SCE_C_CHARACTER:
				if (sc.atLineEnd) {
					sc.ChangeState(SCE_C_STRINGEOL);
				} else if (sc.ch == '\\') {
					if (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\') {
						sc.Forward();
					}
				} else if (sc.ch == '\'') {
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
				break;
			case SCE_C_REGEX:
				if (sc.atLineStart) {
					sc.SetState(SCE_C_DEFAULT);
				} else if (sc.ch == '/') {
					sc.Forward();
					while ((sc.ch < 0x80) && islower(sc.ch))
						sc.Forward();    // gobble regex flags
					sc.SetState(SCE_C_DEFAULT);
				} else if (sc.ch == '\\') {
					// Gobble up the quoted character
					if (sc.chNext == '\\' || sc.chNext == '/') {
						sc.Forward();
					}
				}
				break;
			case SCE_C_STRINGEOL:
				if (sc.atLineStart) {
					sc.SetState(SCE_C_DEFAULT);
				}
				break;
			case SCE_C_VERBATIM:
				if (sc.ch == '\"') {
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
				if (sc.Match("\"\"\"")) {
					sc.Forward(2);
					//while (sc.Match('"')) {
					//	sc.Forward();
					//}
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
				break;
			case SCE_C_UUID:
				if (sc.ch == ')')
					sc.SetState(SCE_C_DEFAULT);
				break;
		}

		//if (sc.atLineEnd && !atLineEndBeforeSwitch) {
			// State exit processing consumed characters up to end of line.
		//	lineCurrent++;
		//}

		// Determine if a new state should be entered.
		if (sc.state == SCE_C_DEFAULT) {
			if ((_hasVerbatim(lexType) && sc.Match('@', '\"')) || ((lexType == LEX_D) && sc.Match('r', '\"'))) {
				sc.SetState(SCE_C_VERBATIM);
				sc.Forward();
			} else if ((lexType == LEX_GROOVY) && sc.Match("\"\"\"")) {
				sc.SetState(SCE_C_TRIPLEVERBATIM);
				sc.Forward(2);
			//} else if (hashquotedStrings && sc.Match('#', "\"")) {
			//	sc.SetState(SCE_C_HASHQUOTEDSTRING);
			//	sc.Forward();
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
			} else if ((lexType == LEX_D) && sc.ch == '`') {
				sc.SetState(SCE_C_DSTRINGB);
				sc.Forward();
			} else if (!_sharpComment(lexType) && sc.Match('/', '*')) {
				sc.SetState(SCE_C_COMMENT);
				sc.Forward();
			} else if ((!_sharpComment(lexType) && sc.Match('/', '/'))
				|| (lineCurrent == 0 && sc.Match('#', '!')) || (_sharpComment(lexType) && sc.ch == '#')) {
				sc.SetState(SCE_C_COMMENTLINE);
			} else if (lexType == LEX_JAM && sc.Match('$', '(')) {
				++numDTSBrace;
				sc.SetState(SCE_C_VARIABLE2);
				sc.Forward();
			} else if (sc.ch == '0') {
				sc.SetState(SCE_C_NUMBER);
				if (_hasHexNum(lexType) && (sc.chNext == 'x' || sc.chNext == 'X')) {
					radix = 16;
					sc.Forward();
				} else if (_hasBinNum(lexType) && (sc.chNext == 'b' || sc.chNext == 'B')) {
					radix = 2;
					sc.Forward();
				} else if (_hasOctNum(lexType) && IsADigit(sc.chNext, 10)) {
					radix = 8;
				} else {
					radix = 10;
				}
			} else if (IsADigit(sc.ch)) {
				radix = 10;
				sc.SetState(SCE_C_NUMBER);
			} else if (sc.ch == '.' && IsADigit(sc.chNext)) {
				radix = 10;
				isFloatNumber = true;
				sc.SetState(SCE_C_NUMBER);
				sc.Forward();
			} else if (sc.ch == '/') { // bug
				if (_hasRegex(lexType) && (sc.chNext != '=')
					&& (setOKBeforeRE.Contains(chPrevNonWhite) || followsReturn)
					&& (!setCouldBePostOp.Contains(chPrevNonWhite) || !followsPostfixOperator)) {
					sc.SetState(SCE_C_REGEX);	// JavaScript's RegEx
					followsReturn = false;
				} else if ((lexType == LEX_D) && sc.chNext == '+'){
					++curNcLevel;
					sc.SetState(SCE_C_DNESTEDCOMMENT);
					sc.Forward();
				} else {
					sc.SetState(SCE_C_OPERATOR);
				}
			} else if (sc.ch == '\"') {
				if ((lexType == LEX_CPP || lexType == LEX_OBJC) && sc.chPrev == 'R') {
					styler.Flush();
					if ((styler.StyleAt(sc.currentPos - 1)) == SCE_C_STRINGRAW) {
						sc.SetState(SCE_C_STRINGRAW);
						rawStringTerminator = ")";
						for (int termPos = sc.currentPos + 1;; termPos++) {
							char chTerminator = styler.SafeGetCharAt(termPos, '(');
							if (chTerminator == '(')
								break;
							rawStringTerminator += chTerminator;
						}
						rawStringTerminator += '\"';
					} else {
						sc.SetState(SCE_C_STRING);
					}
				} else if (!(lexType == LEX_JAM && sc.chPrev == '\\')){
					sc.SetState(SCE_C_STRING);
				}
				isIncludePreprocessor = false;	// ensure that '>' won't end the string
			} else if (isIncludePreprocessor && (sc.ch == '<')) {
				sc.SetState(SCE_C_STRING);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_C_CHARACTER);
			} else if (_hasPreprocessor(lexType) && sc.ch == '#') {
				if (lineState & 0x0002) {
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
			} else if (sc.ch == '$' && (lexType == LEX_HX || lexType == LEX_AWK)) {
				sc.SetState(SCE_C_VARIABLE);
			} else if (setWordStart.Contains(sc.ch)) {
				sc.SetState(SCE_C_IDENTIFIER);
			} else if (isoperator(static_cast<char>(sc.ch)) || ((lexType == LEX_D) && sc.ch == '$')) {
				sc.SetState(SCE_C_OPERATOR);
				if (sc.ch == '.' && sc.chNext == '.')
					sc.Forward();
				if ((sc.ch == '+' && sc.chPrev == '+') || (sc.ch == '-' && sc.chPrev == '-'))
					followsPostfixOperator = true;
				else
					followsPostfixOperator = false;

				if (lastWordWasGoto && sc.ch == ';') {
					lastWordWasGoto = false;
					followsReturn = false;
				}
				if (lastWordWasAsm && sc.ch == '{') {
					lineState |= 0x0001;
					lastWordWasAsm = false;
				} else if (sc.ch == '}') {
					lastWordWasGoto = false;
					followsReturn = false;
					lastWordWasAsm = false;
					if (lineState & 0x0001) {
						lineState &= 0xFFFE;
					}
				}

				if(!(lineState & 0x0002)) {
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
					} else if (sc.ch == '[') {
						++numSBrace;
					} else if (sc.ch == ']') {
						if (numSBrace > 0)
							--numSBrace;
					}
				}

				// for IL Asm
				if ((lexType == LEX_CS || lexType == LEX_ILASM) && sc.ch == '.' && iswordstart(sc.chNext)) {
					char sw[128];
					int i = LexGetRange(sc.currentPos + 1, styler, iswordstart, sw, sizeof(sw)) + 1;
					if (keywords4.InList(sw)) {
						isILAsmSource = true;
						sc.SetState(SCE_C_DIRECTIVE);
						sc.Forward(i);
						if (isoperator(static_cast<char>(sc.ch)))
							sc.SetState(SCE_C_OPERATOR);
						else
							sc.SetState(SCE_C_DEFAULT);
					}
				}

			} else {
				radix = 0;
				isFloatNumber = false;
			}
		}

		if (!isspacechar(sc.ch) && !IsSpaceEquiv(sc.state)) {
			chPrevNonWhite = sc.ch;
			visibleChars++;
		}
		continuationLine = false;
	}
	const bool rawStringsChanged = rawStringTerminators.Merge(rawSTNew, lineCurrent);
	if (rawStringsChanged)
		styler.ChangeLexerState(startPos, startPos + length);

	if (sc.state == SCE_C_IDENTIFIER)
		goto _lable_identifier;

	sc.Complete();
}

static bool IsBackslash(int pos, LexAccessor &styler) {
	return (pos >= 2) && (styler[pos] == '\n')
		&& (styler[pos-1] == '\\' || (styler[pos-1] == '\r' && styler[pos-2] == '\\'));
}
// also used in LexAsm.cxx
bool IsBackslashLine(int line, LexAccessor &styler) {
	return IsBackslash(styler.LineStart(line + 1) - 1, styler);
}
static bool IsDefineLine(int line, LexAccessor &styler, int &DefinePos) {
	int pos = styler.LineStart(line);
	int endPos = styler.LineStart(line + 1) - 1;
	pos = LexSkipSpaceTab(pos, endPos, styler);
	if (styler[pos] == '#'){
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
bool IsCppInDefine(int currentPos, LexAccessor &styler) {
	int line = styler.GetLine(currentPos);
	int pos;
	if (IsDefineLine(line, styler, pos)) {
		if (pos < currentPos)
			return true;
	}
	line--;
	while (line > 0 && IsBackslashLine(line, styler) && !IsDefineLine(line, styler, pos))
		line--;
	if (line >= 0 && IsDefineLine(line, styler, pos) && IsBackslashLine(line, styler)) {
		return true;
	}
	return false;
}

#define CPP_MAX_FLOD_KIND 	3
static bool IsCppFoldingLine(int line, LexAccessor &styler, int kind) {
	int startPos = styler.LineStart(line);
	int endPos = styler.LineStart(line + 1) - 1;
	int pos = LexSkipSpaceTab(startPos, endPos, styler);
	int stl = styler.StyleAt(pos);
	char ch = styler[pos];
	if (kind == 0 && stl == SCE_C_COMMENTLINE && (ch == '/' || ch == '#')) {
		return true;
	} else if (kind == 1 && stl == SCE_C_WORD && (ch == 'i' || ch == 'u' || ch == 't')) {
		return styler.Match(pos, "using") || styler.Match(pos, "import") || styler.Match(pos, "typedef");
	} else if (kind > 1 && ch == '#' && stl == SCE_C_PREPROCESSOR) {
		pos++;
		pos = LexSkipSpaceTab(pos, endPos, styler);
		ch = styler[pos];
		if (kind == 2 && (ch == 'i' || ch == 'u')) {
			return styler.Match(pos, "include") || styler.Match(pos, "using") || styler.Match(pos, "import");
		} else if (kind == 3 && ch == 'd') {
			return styler.Match(pos, "define") && !IsBackslashLine(line, styler); // multi-line #define
		} else if (kind == 4 && ch == 'u') {
			return styler.Match(pos, "undef");
		}
	}
	return false;
}
#define IsCommentLine(line)		IsCppFoldingLine(line, styler, 0)
#define IsUsingLine(line)		IsCppFoldingLine(line, styler, 1)
#define IsIncludeLine(line)		IsCppFoldingLine(line, styler, 2)
#define IsDefineLine(line)		IsCppFoldingLine(line, styler, 3)
#define IsUnDefLine(line)		IsCppFoldingLine(line, styler, 4)

static inline bool IsStreamCommentStyle(int style) {
	return style == SCE_C_COMMENT;
}
static bool IsLeftBraceLine(int line, LexAccessor &styler) {
	// above line
	int startPos = styler.LineStart(line-1);
	int endPos = styler.LineStart(line)-1;
	int pos = LexSkipSpaceTab(startPos, endPos, styler);
	char ch = styler[pos];
	int stl = styler.StyleAt(pos);
	if (ch == '\r' || ch == '\n' || IsSpaceEquiv(stl) || (stl == SCE_C_PREPROCESSOR))
		return false;
	while (endPos > pos && isspacechar(styler[endPos]))
		endPos--;
	ch = styler[endPos];
	if (styler[endPos-1] == '\n' || styler[endPos-1] == '\r'
		|| (styler.StyleAt(endPos) == SCE_C_OPERATOR
			&& !(ch == ')' || ch == '>' || ch == '=' || ch == ':' || ch == ']')))
		return false;
	// current line
	startPos = styler.LineStart(line);
	endPos = styler.LineStart(line + 1) - 1;
	pos = LexSkipSpaceTab(startPos, endPos, styler);
	if (styler.StyleAt(pos) == SCE_C_OPERATOR && styler[pos] == '{') {
		pos++;
		pos = LexSkipSpaceTab(pos, endPos, styler);
		ch = styler[pos];
		stl = styler.StyleAt(pos);
		return  (ch == '\n') || (ch == '\r') || IsSpaceEquiv(stl);
	}
	return false;
}

static void FoldCppDoc(unsigned int startPos, int length, int initStyle, WordList *[], Accessor &styler) {
	if (styler.GetPropertyInt("fold") == 0)
		return;
	const bool foldComment = styler.GetPropertyInt("fold.comment", 1) != 0;
	const bool foldPreprocessor = styler.GetPropertyInt("fold.preprocessor", 1) != 0;
	//const bool foldAtElse = styler.GetPropertyInt("fold.at.else", 0) != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 0) != 0;

	const int lexType = styler.GetPropertyInt("lexer.2nd.type", LEX_CPP);
	const bool hasPreprocessor = _hasPreprocessor(lexType);

	unsigned int endPos = startPos + length;
	int visibleChars = 0;
	int lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0)
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
	//int levelMinCurrent = levelCurrent;
	int levelNext = levelCurrent;

	char chNext = styler[startPos];
	int style = initStyle;
	int styleNext = styler.StyleAt(startPos);

	for (unsigned int i = startPos; i < endPos; i++) {
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
		if (style == SCE_C_TRIPLEVERBATIM) {
			if (stylePrev != SCE_C_TRIPLEVERBATIM) {
				levelNext++;
			} else if (styleNext != SCE_C_TRIPLEVERBATIM && !atEOL) {
				levelNext--;
			}
		}
		if (foldComment && atEOL && IsCommentLine(lineCurrent)) {
			if(!IsCommentLine(lineCurrent - 1) && IsCommentLine(lineCurrent + 1))
				levelNext++;
			else if(IsCommentLine(lineCurrent - 1) && !IsCommentLine(lineCurrent + 1))
				levelNext--;
		}
		if (lexType == LEX_D && foldComment && style == SCE_C_DNESTEDCOMMENT) {
			if (ch == '/' && chNext == '+')
				levelNext++;
			else if (ch == '+' && chNext == '/')
				levelNext--;
		}

		if (atEOL && IsUsingLine(lineCurrent)) {
			if(!IsUsingLine(lineCurrent-1) && IsUsingLine(lineCurrent+1))
				levelNext++;
			else if(IsUsingLine(lineCurrent-1) && !IsUsingLine(lineCurrent+1))
				levelNext--;
		}
		if (hasPreprocessor && atEOL && IsIncludeLine(lineCurrent)) {
			if(!IsIncludeLine(lineCurrent-1) && IsIncludeLine(lineCurrent+1))
				levelNext++;
			else if(IsIncludeLine(lineCurrent-1) && !IsIncludeLine(lineCurrent+1))
				levelNext--;
		}
		if (hasPreprocessor && atEOL && IsDefineLine(lineCurrent)) {
			if(!IsDefineLine(lineCurrent-1) && IsDefineLine(lineCurrent+1))
				levelNext++;
			else if(IsDefineLine(lineCurrent-1) && !IsDefineLine(lineCurrent+1))
				levelNext--;
		}
		if (hasPreprocessor && atEOL && IsUnDefLine(lineCurrent)) {
			if(!IsUnDefLine(lineCurrent-1) && IsUnDefLine(lineCurrent+1))
				levelNext++;
			else if(IsUnDefLine(lineCurrent-1) && !IsUnDefLine(lineCurrent+1))
				levelNext--;
		}

		if (atEOL && IsBackslashLine(lineCurrent, styler) && !IsBackslashLine(lineCurrent-1, styler)) {
			levelNext++;
		}
		if (atEOL && !IsBackslashLine(lineCurrent, styler) && IsBackslashLine(lineCurrent-1, styler)) {
			levelNext--;
		}
		if (atEOL && IsLeftBraceLine(lineCurrent+1, styler)) {
			levelNext++;
		}

		if ((hasPreprocessor || (lexType == LEX_HX)) && foldPreprocessor && ch == '#' && style == SCE_C_PREPROCESSOR) {
			int pos = LexSkipSpaceTab(i+1, endPos, styler);
			if (styler.Match(pos, "if") || styler.Match(pos, "region")) {
				levelNext++;
			} else if (styler.Match(pos, "end")) {
				levelNext--;
			} else if (styler.Match(pos, "pragma")) { // #pragma region, #pragma endregion
				pos = LexSkipSpaceTab(pos+7, endPos, styler);
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
		if ((lexType == LEX_CPP || lexType == LEX_OBJC) && visibleChars == 0 && ch == '@' && style == SCE_C_DIRECTIVE) {
			if (styler.Match(i+1, "interface") || styler.Match(i+1, "implementation") || styler.Match(i+1, "protocol")) {
				levelNext++;
			} else if (styler.Match(i+1, "end")) {
				levelNext--;
			}
		}

		if (style == SCE_C_OPERATOR && !(hasPreprocessor && IsCppInDefine(i, styler))) {
			// maybe failed in multi-line define section, MFC's afx.h is a example
			if (ch == '{' && !(lineCurrent > 0 && IsLeftBraceLine(lineCurrent, styler))) {
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
		}

		// Resource Script
		if (lexType == LEX_RC && style == SCE_C_WORD) {
			if (styler.Match(i, "BEGIN")) {
				levelNext++;
			} else if (styler.Match(i, "END")) {
				levelNext--;
			}
		}

		if (!isspacechar(ch))
			visibleChars++;
		if (atEOL || (i == endPos-1)) {
			int levelUse = levelCurrent;
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
		}
	}
}

LexerModule lmCPP(SCLEX_CPP, ColouriseCppDoc, "cpp", FoldCppDoc, 0, 8);
