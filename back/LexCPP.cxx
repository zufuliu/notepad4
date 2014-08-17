// Lexer for C, C++, C#, Java, Rescouce Script, Asymptote, D, Objective C/C++
// JavaScript, JScript, ActionScript, haXe, Groovy, Scala, Jamfile, AWK, IDL/ODL/AIDL

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
//#include "SparseState.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

#define		LEX_CPP		1	// C/C++
#define		LEX_JAVA	2	// Java
#define 	LEX_CS		3	// C#
#define 	LEX_JS		4	// JavaScript
#define 	LEX_RC		5	// Resouce Script
#define		LEX_IDL		6	// Interface Definition Language
#define 	LEX_D		7	// D
#define 	LEX_ASY		8	// Asymptote
#define		LEX_CLI		9	// Commaon Language Infrastructure
#define		LEX_OBJC	10	// Objective C/C++
#define 	LEX_AS		11	// ActionScript
#define 	LEX_HX		12	// haXe
#define		LEX_GROOVY	13	// Groovy Script
#define		LEX_SCALA	14	// Scala Script
//#define		LEX_AIDL	27	// Android Interface Definition Language
#define		LEX_AWK		51	// Awk
#define		LEX_JAM		52	// Jamfile
#define		LEX_DOT		53	// GraphViz Dot
#define		LEX_NSIS	56	// Nsis Script

static bool _hasPreprocessor(int lex) { // #[space]preprocessor
	return lex == LEX_CPP || lex == LEX_CS || lex == LEX_RC || lex == LEX_OBJC || lex == LEX_CLI || lex == LEX_IDL;
}
static inline bool _hasAnotation(int lex) { // @anotation
	return lex == LEX_JAVA || lex == LEX_GROOVY || lex == LEX_SCALA;
}
static bool _hasRegex(int lex) { // Javascript /regex/
	return lex == LEX_JS || lex == LEX_GROOVY || lex == LEX_AS || lex == LEX_HX || lex == LEX_AWK;
}
static bool _hasVerbatim(int lex) { // @"string", NSString
	return lex == LEX_CS || lex == LEX_CPP || lex == LEX_OBJC || lex == LEX_CLI;
}
static inline bool _hasTripleVerbatim(int lex) {
	return lex == LEX_GROOVY || lex == LEX_SCALA;
}
static inline bool _sharpComment(int lex) {
	return lex == LEX_AWK || lex == LEX_JAM;
}
static inline bool _hasComplex(int lex) {
	return lex == LEX_CPP || lex == LEX_OBJC || lex == LEX_D;
}
static inline bool _hasXML(int lex) {
	return lex == LEX_JS || lex == LEX_AS || lex == LEX_SCALA;
}
#define _allowUnderscore	_hasComplex
static bool _hasHexFloat(int lex) {
	return _hasComplex(lex) || _hasAnotation(lex);
}
#define	_hasBinNum	_hasHexFloat
static bool _hasOctNum(int lex) {
	return !(lex == LEX_JS || lex == LEX_AS || lex == LEX_HX || lex == LEX_ASY || lex == LEX_DOT);
}
static bool _hasHexNum(int lex) {
	return !(lex == LEX_ASY || lex == LEX_DOT);
}
static bool _hasLong(int lex) {
	return _hasPreprocessor(lex) || _hasAnotation(lex) || lex == LEX_D;
}
static inline bool _hasUnsigned(int lex) {
	return _hasPreprocessor(lex) || lex == LEX_D || lex == LEX_NSIS;
}
static inline bool _hasLongLong(int lex) {
	return lex == LEX_CPP || lex == LEX_OBJC;
}
static inline bool _has_iInt(int lex) {
	return lex == LEX_CPP || lex == LEX_IDL;
}
static bool IsFltFix(int ch, int lex) {
	ch = MakeUpperCase(ch);
	if (ch >= 0x80)
		return false;
	switch (lex) {
		case LEX_CPP:	case LEX_OBJC:
			return ch == 'F' || ch == 'L' || ch == 'Q' || ch == 'W'; break;
		case LEX_JAVA:	case LEX_GROOVY:	case LEX_SCALA:
			return ch == 'F' || ch == 'D'; break;
		case LEX_CS:	case LEX_CLI:
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
static inline bool _use2ndKeyword(int lex) {
	return lex == LEX_OBJC || lex == LEX_CLI;
}
static inline bool _use2ndKeyword2(int lex) {
	return lex == LEX_CPP || lex == LEX_OBJC || lex == LEX_CLI;
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

#define LEX_BLOCK_MASK_ASM		0x0001
#define LEX_BLOCK_MASK_DEFINE	0x0002
#define LEX_BLOCK_MASK_TYPEDEF	0x0004
#define LEX_BLOCK_UMASK_ASM		0xFFFE
#define LEX_BLOCK_UMASK_ALL		0xFFFD

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
	const int lexType = styler.GetPropertyInt("lexer.lang.type", LEX_CPP);

	int lineCurrent = styler.GetLine(startPos);
	int curLineState =  (lineCurrent > 0) ? styler.GetLineState(lineCurrent-1) : 0;
	int lineState = 	(curLineState >> 24);
	int numCBrace =		(curLineState >> 18) & 0x3F;
	int numSBrace = 	(curLineState >> 13) & 0x1F;
	int numRBrace = 	(curLineState >>  8) & 0x1F;
	int curNcLevel = 	(curLineState >>  4) & 0x0F;
	int numDTSBrace =	(curLineState      ) & 0x0F;
#define _MakeState() ((lineState << 24)|(numCBrace << 18)|(numSBrace << 13)|(numRBrace << 8)|(curNcLevel << 4)|numDTSBrace)
#define _UpdateLineState()	styler.SetLineState(lineCurrent, _MakeState())
#define _UpdateCurLineState() lineCurrent = styler.GetLine(sc.currentPos); \
								styler.SetLineState(lineCurrent, _MakeState())

	CharacterSet setWord(CharacterSet::setAlphaNum, "_", 0x80, true);
	CharacterSet setWordStart(CharacterSet::setAlpha, "_@", 0x80, true);
	CharacterSet setOKBeforeRE(CharacterSet::setNone, "([{=,:;!%^&*|?~+-");
	CharacterSet setCouldBePostOp(CharacterSet::setNone, "+-");
	if (lexType == LEX_D || lexType == LEX_HX)
		setWordStart.Add('#');
	if (lexType == LEX_JAM)
		setWord.Add('-');

	int chPrevNonWhite = ' ';
	int visibleChars = 0;
	bool isTypeDefine = false;
	bool lastWordWasUUID = false;
	bool lastWordWasGoto = false;
	bool lastWordWasAsm = false;
	bool lastWordWasAttr = false;
	bool continuationLine = false;
	bool isPragmaPreprocessor = false;
	bool isPreprocessorWord = false;
	bool isIncludePreprocessor = false;
	bool isMessagePreprocessor = false;
	int radix = 0;
	bool isFloatNumber = false;
	bool followsReturn = false;
	bool followsPostfixOperator = false;
	bool isTripleSingle = false;
	int varType = 0;

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
				lineState &= LEX_BLOCK_UMASK_ALL;
			}
			_UpdateCurLineState();
		}

		if (sc.atLineEnd) {
			_UpdateLineState();
			lineCurrent++;
		}

		// Handle line continuation generically.
		/*if (sc.ch == '\\') {
			if (sc.chNext == '\n' || sc.chNext == '\r') {
				lineCurrent++;
				sc.Forward();
				if (sc.ch == '\r' && sc.chNext == '\n') {
					sc.Forward();
				}
				continuationLine = true;
				continue;
			}
		}*/

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
							else {
								sc.ChangeState(SCE_C_DEFAULT);
								while (sc.ch == '.' || sc.ch == '_' || IsADigit(sc.ch, 10))
									sc.Forward();
							}
						} else {
							if ((radix == 10 && IsADigit(sc.chNext,10))
								|| (_hasHexFloat(lexType) && radix == 16 && IsADigit(sc.chNext, 16))) {
								isFloatNumber = true;
								sc.Forward();
							} else if (radix == 10 || (radix == 16 && _hasHexFloat(lexType))){
								if (IsFltFix(sc.chNext, lexType)) {
									sc.Forward(2);
									if (_hasComplex(lexType) && IsCmpFix(sc.ch))
										sc.Forward();
									sc.SetState(SCE_C_DEFAULT);
								} else if (_hasComplex(lexType) && IsCmpFix(sc.chNext)) {
									sc.Forward(2);
									if (IsFltFix(sc.ch, lexType))
										sc.Forward();
									sc.SetState(SCE_C_DEFAULT);
								} else if (lexType == LEX_GROOVY && (sc.chNext == 'g' || sc.chNext == 'G')) {
									sc.Forward(2);
									sc.SetState(SCE_C_DEFAULT);
								} else {
									if (!iswordchar(sc.chNext))
										sc.Forward();
									sc.SetState(SCE_C_DEFAULT);
								}
							} else {
								sc.SetState(SCE_C_DEFAULT);
							}
						}
					} else if (sc.ch == '_') {
						if (_allowUnderscore(lexType))
							sc.Forward();
						else if (lexType == LEX_JAVA && (IsADigit(sc.chPrev, radix) || sc.chPrev == '_')
							&& (IsADigit(sc.chNext, radix) || sc.chNext == '_'))
							sc.Forward();
						else {
							sc.ChangeState(SCE_C_DEFAULT);
							while (sc.ch == '.' || sc.ch == '_' || IsADigit(sc.ch, 10))
								sc.Forward();
						}
					} else if (((radix == 10 && (sc.ch == 'e' || sc.ch == 'E'))
						|| (_hasHexFloat(lexType) && radix == 16 && (sc.ch == 'p' || sc.ch == 'P')))
						&& (IsADigit(sc.chNext, radix)
							|| ((sc.chNext == '+' || sc.chNext == '-') && IsADigit(sc.GetRelative(2), radix)))) {
						isFloatNumber = true;
						sc.Forward();
						if (sc.ch == '+' || sc.ch == '-')
							sc.Forward();
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
							if (lexType == LEX_GROOVY && (sc.ch == 'g' || sc.ch == 'G'))
								sc.Forward();
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
						} else if  ((lineState & LEX_BLOCK_MASK_ASM) && (sc.ch == 'h' || sc.ch == 'H' || sc.ch == 'b' || sc.ch == 'B')) {
							sc.ForwardSetState(SCE_C_DEFAULT);
						} else if (radix == 8 && IsADigit(sc.ch, 10)) {
							sc.ChangeState(SCE_C_DEFAULT);
							while (sc.ch == '.' || sc.ch == '_' || IsADigit(sc.ch, 10))
								sc.Forward();
						} else {
							if (lexType == LEX_GROOVY && (sc.ch == 'i' || sc.ch == 'I' || sc.ch == 'g' || sc.ch == 'G'))
								sc.Forward();
							sc.SetState(SCE_C_DEFAULT);
						}
					}
				}
				break;

			case SCE_C_IDENTIFIER:
_label_identifier:
				if (!setWord.Contains(sc.ch)) {
					char s[256] = {0};
					if (lexType == LEX_DOT || lexType == LEX_NSIS)
						sc.GetCurrentLowered(s, sizeof(s));
					else
						sc.GetCurrent(s, sizeof(s));

					followsReturn = false;
					int len = static_cast<int>(strlen(s));
					char tu[256] = {0};
					if (len > 4 && s[0] == '_' && s[1] == '_' && s[len - 1] == '_' && s[len - 2] == '_')
						strncpy(tu, &s[2], len - 4);
					bool hasAttr = (lexType == LEX_CPP || lexType == LEX_OBJC || isObjCSource);
					bool mayAttr = lastWordWasAttr && (numRBrace > 0 || lineState & LEX_BLOCK_MASK_DEFINE);

					if ((lineState & LEX_BLOCK_MASK_ASM) && kwAsmInstruction.InList(s)) {
						sc.ChangeState(SCE_C_ASM_INSTRUCTION);
						lastWordWasGoto = s[0] == 'j';
					} else if ((lineState & LEX_BLOCK_MASK_ASM) && kwAsmRegister.InList(s)) {
						sc.ChangeState(SCE_C_ASM_REGISTER);
					} else if ((lexType == LEX_D) && (lineState & LEX_BLOCK_MASK_ASM) && kw2ndKeyword.InList(s)) {
						sc.ChangeState(SCE_C_2NDWORD);
					} else if (((s[0] == '#' || s[0] == '!') && keywords3.InList(&s[1]))
						|| (isPreprocessorWord && keywords3.InList(s))) {
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
								lineState |= LEX_BLOCK_MASK_DEFINE;
						}
					} else if (isPragmaPreprocessor) {
						isPragmaPreprocessor = false;
						sc.ChangeState(SCE_C_PREPROCESSOR);
						isMessagePreprocessor = (s[0] == 'r' && strequ(&s[1], "egion"))
							|| (s[0] == 'e' && strequ(&s[1], "ndregion"))
							|| (s[0] == 'm' && strequ(&s[1], "ark"));
					} else if ((!hasAttr && kwAttribute.InList(s))
						|| (hasAttr && mayAttr && (kwAttribute.InList(s) || kwAttribute.InList(tu)))) {
							sc.ChangeState(SCE_C_GATTRIBUTE);
					} else if (keywords.InList(s)) {
						if ((sc.GetRelative(-len-1) == '.')
							|| ((lexType == LEX_OBJC || isObjCSource) && LexGetNextChar(sc.currentPos, styler) == ']')
							|| (lexType == LEX_NSIS && visibleChars != len)) {
							sc.ChangeState(SCE_C_DEFAULT);
						} else {
							sc.ChangeState(SCE_C_WORD);
							// asm __asm _asm
							lastWordWasAsm = (s[0] == 'a' && strequ(&s[1], "sm")) || (s[0] == '_' && strequ(&s[1], "_asm"));
							lastWordWasUUID = (s[0] == 'u' && strequ(&s[1], "uid"));
							lastWordWasGoto = (s[0] == 'g' && strequ(&s[1], "oto")) || (s[0] == '_' && strequ(&s[1], "_label__"))
								|| (s[0] == 'b' && strequ(&s[1], "reak")) || (s[0] == 'c' && strequ(&s[1], "ontinue"));
							followsReturn = (s[0] == 'r' && strequ(&s[1], "eturn"));
							if (!isTypeDefine)
								isTypeDefine = (s[0] == 't') && (strequ(&s[1], "ypedef"));
							if (!lastWordWasAttr)
								lastWordWasAttr = (s[0] == '_' && s[1] == '_') && (strequ(&s[2], "declspec") || strequ(&s[2], "attribute__"));
						}
					} else if (keywords2.InList(s)) {
						sc.ChangeState(SCE_C_WORD2);
					} else if (s[0] == '@' && _hasAnotation(lexType)) {
						sc.ChangeState(SCE_C_DIRECTIVE);
						while (iswordchar(sc.ch))
							sc.Forward();
					} else if (s[0] == '@' && keywords4.InList(&s[1])) {
						sc.ChangeState(SCE_C_DIRECTIVE);
						if (lexType == LEX_CPP || lexType == LEX_OBJC) {
							isObjCSource = true;
							lastWordWasAttr = (s[1] == 'p') && strequ(&s[2], "roperty");
						}
					} else if (kwClass.InList(s)) {
						sc.ChangeState(SCE_C_GCLASS);
					} else if (kwInterface.InList(s)) {
						sc.ChangeState(SCE_C_GINTERFACE);
					} else if (kwEnumeration.InList(s)) {
						sc.ChangeState(SCE_C_GENUMERATION);
					} else if (kwConstant.InList(s)) {
						sc.ChangeState(SCE_C_GCONSTANT);
					} else if ((isObjCSource || _use2ndKeyword(lexType)) && kw2ndKeyword.InList(s)) {
						sc.ChangeState(SCE_C_2NDWORD);
					} else if ((isObjCSource || isILAsmSource || _use2ndKeyword2(lexType)) && kw2ndKeyword2.InList(s)) {
						sc.ChangeState(SCE_C_2NDWORD2);
					} else if (isILAsmSource && kwAsmInstruction.InList(s)) {
						sc.ChangeState(SCE_C_2NDWORD);
					} else if (lastWordWasGoto && (numCBrace > 0 || lexType == LEX_NSIS)) {
						sc.ChangeState(SCE_C_LABEL);
						lastWordWasGoto = false;
					} else if (sc.ch == ':' && sc.chNext != ':'
						&& ((numCBrace > 0 && numSBrace == 0 && numRBrace == 0) || lexType == LEX_NSIS)
						&& visibleChars == static_cast<int>(strlen(s))) {
						sc.ChangeState(SCE_C_LABEL);
					} else if (lexType == LEX_DOT) {
						int pos = sc.currentPos;
						while (IsASpaceOrTab(styler[pos++]));
						if (styler[pos-1] == '=')
							sc.ChangeState(SCE_C_GATTRIBUTE);
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

			case SCE_C_CHARACTER:
				if (sc.atLineEnd) {
					sc.ChangeState(SCE_C_STRINGEOL);
				} else if (!(lexType == LEX_NSIS) &&
					(sc.ch == '\\' && (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\'))) {
					sc.Forward();
				} else if (sc.ch == '\'') {
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
				break;
			case SCE_C_STRING:
				if (sc.atLineEnd) {
					if (lexType == LEX_HX || lexType == LEX_ASY || lexType == LEX_JAM)
						continue;
					else
						sc.SetState(SCE_C_STRINGEOL);
				} else if (isIncludePreprocessor && sc.ch == '>') {
					sc.ForwardSetState(SCE_C_DEFAULT);
					isIncludePreprocessor = false;
				} else if (isMessagePreprocessor && sc.atLineEnd) {
					sc.SetState(SCE_C_STRINGEOL);
					isMessagePreprocessor = false;
				} else if (!(lexType == LEX_NSIS) &&
					(sc.ch == '\\' && (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\'))) {
					sc.Forward();
				} else if (sc.ch == '\"') {
					if ((lexType == LEX_D) && IsDStrFix(sc.chNext))
						sc.Forward();
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
				break;
			case SCE_C_STRINGRAW:
				if (sc.ch == '\"') {
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

			case SCE_C_DSTRINGX:
				if (sc.ch == '\"') {
					if (IsDStrFix(sc.chNext))
						sc.Forward();
					sc.ForwardSetState(SCE_C_DEFAULT);
				}
				break;
			case SCE_C_DSTRINGB:
				if (sc.ch == '`') {
					if (lexType == LEX_D && IsDStrFix(sc.chNext))
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
				if ((varType == 1 && sc.ch == ')') || (varType == 2 && sc.ch == '}')) {
					if (numDTSBrace > 0)
						--numDTSBrace;
					if (numDTSBrace == 0) {
						varType = 0;
						sc.ForwardSetState(SCE_C_DEFAULT);
					}
				} else if ((varType == 1 && sc.Match('$', '(')) || (varType == 2 && sc.Match('$', '{'))) {
					++numDTSBrace;
					sc.Forward();
				}
				break;
			case SCE_C_VARIABLE:
				if (!setWord.Contains(sc.ch)) {
					sc.SetState(SCE_C_DEFAULT);
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
				if (isObjCSource && sc.ch == '\\' && (sc.chNext == '\\' || sc.chNext == '\"' || sc.chNext == '\'')) {
					sc.Forward();
				} else if (sc.ch == '\"') {
					if (isObjCSource) {
						sc.ForwardSetState(SCE_C_DEFAULT);
					} else if (sc.chNext == '\"') {
						sc.Forward();
					} else {
						if ((lexType == LEX_D) && IsDStrFix(sc.chNext))
							sc.Forward();
						sc.ForwardSetState(SCE_C_DEFAULT);
					}
				}
				break;
			case SCE_C_TRIPLEVERBATIM:
				if (sc.ch == '\\' && (sc.chNext == '\"' || sc.chNext == '\'' || sc.chNext == '\\')) {
					sc.Forward();
				} else if (isTripleSingle && sc.Match("\'\'\'")) {
					isTripleSingle = false;
					sc.Forward(2);
					sc.ForwardSetState(SCE_C_DEFAULT);
				} else if (sc.Match("\"\"\"")) {
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
			case SCE_C_XML_TAG:
				if (sc.Match('<', '/') || sc.Match('/', '>')) {
					if (curNcLevel > 0)
						--curNcLevel;
					sc.Forward();
					if (curNcLevel == 0) {
						while (sc.ch != '>')
							sc.Forward();
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
			if ((_hasVerbatim(lexType) && sc.Match('@', '\"'))
				|| ((lexType == LEX_D) && sc.Match('r', '\"'))) {
				sc.SetState(SCE_C_VERBATIM);
				sc.Forward();
			} else if (_hasTripleVerbatim(lexType) && sc.Match("\"\"\"")) {
				sc.SetState(SCE_C_TRIPLEVERBATIM);
				sc.Forward(2);
			} else if (lexType == LEX_GROOVY && sc.Match("\'\'\'")) {
				sc.SetState(SCE_C_TRIPLEVERBATIM);
				sc.Forward(2);
				isTripleSingle = true;
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
			} else if ((lexType == LEX_D || lexType == LEX_NSIS) && sc.ch == '`') {
				sc.SetState(SCE_C_DSTRINGB);
			} else if (!_sharpComment(lexType) && sc.Match('/', '*')) {
				sc.SetState(SCE_C_COMMENT);
				sc.Forward();
			} else if ((!(_sharpComment(lexType) || lexType == LEX_NSIS) && sc.Match('/', '/'))
				|| (lineCurrent == 0 && sc.Match('#', '!'))
				|| ((_sharpComment(lexType) || lexType == LEX_NSIS) && sc.ch == '#')
				|| (lexType == LEX_NSIS && sc.ch == ';')) {
				sc.SetState(SCE_C_COMMENTLINE);
			} else if (sc.ch == '$' && (((lexType == LEX_JAM || lexType == LEX_NSIS) && sc.chNext == '(')
				|| (lexType == LEX_NSIS && sc.chNext == '{'))) {
				++numDTSBrace;
				if (sc.chNext == '(')
					varType = 1;
				else
					varType = 2;
				sc.SetState(SCE_C_VARIABLE2);
				sc.Forward();
			} else if (sc.ch == '0') {
				sc.SetState(SCE_C_NUMBER);
				if ((_hasHexNum(lexType) && (sc.chNext == 'x' || sc.chNext == 'X')) || (lineState & LEX_BLOCK_MASK_ASM)) {
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
				if (_hasRegex(lexType) && (setOKBeforeRE.Contains(chPrevNonWhite) || followsReturn)
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
			} else if ((lexType == LEX_CPP || lexType == LEX_RC) && (sc.ch == 'u' || sc.ch == 'U' || sc.ch == 'L')) {
				int offset = 0;
				bool raw = false;
				bool isuchar = false;
				if (sc.chNext == '\"' || sc.chNext == '\'') {
					offset = 1;
					isuchar = sc.chNext == '\'';
				} else if (sc.chNext == 'R' && sc.GetRelative(2) == '\"') {
					offset = 2;
					raw = true;
				} else if (sc.chNext == '8' && sc.GetRelative(2) == 'R' && sc.GetRelative(3) == '\"') {
					offset = 3;
					raw = true;
				}
				if (!offset) {
					sc.SetState(SCE_C_IDENTIFIER);
				} else {
					if (raw)
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
			} else if (lexType == LEX_JAM && sc.ch == '\\' && (sc.chNext == '\\' || sc.chNext == '\"' || sc.chNext == '\'')) {
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
			} else if (sc.ch == '$' && (lexType == LEX_HX || lexType == LEX_AWK || lexType == LEX_NSIS)) {
				sc.SetState(SCE_C_VARIABLE);
			} else if (setWordStart.Contains(sc.ch)
				|| (lexType == LEX_NSIS && sc.ch == '!' && visibleChars == 0 && iswordstart(sc.chNext))) {
				sc.SetState(SCE_C_IDENTIFIER);
			} else if (sc.ch == '.') {
				sc.SetState(SCE_C_OPERATOR);
				if (sc.chNext == '.')
					sc.Forward();
				// for IL Asm
				if ((lexType == LEX_CS || lexType == LEX_CLI) && iswordstart(sc.chNext)) {
					char sw[128];
					int i = LexGetRange(sc.currentPos + 1, styler, iswordstart, sw, sizeof(sw)) + 1;
					if (kw2ndKeyword.InList(sw)) {
						isILAsmSource = true;
						lastWordWasUUID = strequ(sw, "publickeytoken");
						sc.SetState(SCE_C_DIRECTIVE);
						sc.Forward(i);
						if (isoperator(static_cast<char>(sc.ch)))
							sc.SetState(SCE_C_OPERATOR);
						else
							sc.SetState(SCE_C_DEFAULT);
					}
				}
			} else if (sc.ch == '<') {
				if (_hasXML(lexType) && chPrevNonWhite == '=') {
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
			} else if (isoperator(static_cast<char>(sc.ch)) || ((lexType == LEX_D) && sc.ch == '$')) {
				sc.SetState(SCE_C_OPERATOR);
				isPragmaPreprocessor = false;
				if ((sc.ch == '+' && sc.chPrev == '+') || (sc.ch == '-' && sc.chPrev == '-'))
					followsPostfixOperator = true;
				else
					followsPostfixOperator = false;

				if (lastWordWasUUID && sc.ch == '(') {
					sc.ForwardSetState(SCE_C_UUID);
					lastWordWasUUID = false;
				}
				if (sc.ch == ';') {
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

				if(!(lineState & LEX_BLOCK_MASK_DEFINE)) {
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
			} else {
				radix = 0;
				isFloatNumber = false;
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

	if (sc.state == SCE_C_IDENTIFIER)
		goto _label_identifier;

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
#define LexMatchIC(pos, str)	LexMatchIgnoreCase(pos, styler, str)
static bool IsCppFoldingLine(int line, LexAccessor &styler, int kind) {
	int startPos = styler.LineStart(line);
	int endPos = styler.LineStart(line + 1) - 1;
	int pos = LexSkipSpaceTab(startPos, endPos, styler);
	int stl = styler.StyleAt(pos);
	char ch = styler[pos];
	if (kind == 0 && stl == SCE_C_COMMENTLINE && (ch == '/' || ch == '#' || ch == ';')) {
		return true;
	} else if (kind == 1 && stl == SCE_C_WORD && (ch == 'i' || ch == 'u' || ch == 't' || ch == '@')) {
		return styler.Match(pos, "using") || styler.Match(pos, "import") || styler.Match(pos, "typedef");
	//} else if (kind == 2 && stl == SCE_C_DIRECTIVE && (ch == '@')) {
	//	return styler.Match(pos, "@property") || styler.Match(pos, "@synthesize");
	} else if (kind > 2 && (ch == '#' || ch == '!') && stl == SCE_C_PREPROCESSOR) {
		pos++;
		pos = LexSkipSpaceTab(pos, endPos, styler);
		ch = styler[pos];
		if (kind == 3 && (ch == 'i' || ch == 'u')) {
			return LexMatchIC(pos, "include") || styler.Match(pos, "using") || styler.Match(pos, "import");
		} else if (kind == 4 && ch == 'd') {
			return LexMatchIC(pos, "define") && !IsBackslashLine(line, styler); // multi-line #define
		} else if (kind == 5 && ch == 'u') {
			return LexMatchIC(pos, "undef");
		}
	}
	return false;
}
#define IsCommentLine(line)		IsCppFoldingLine(line, styler, 0)
#define IsUsingLine(line)		IsCppFoldingLine(line, styler, 1)
//#define IsPropertyLine(line)	IsCppFoldingLine(line, styler, 2)
#define IsIncludeLine(line)		IsCppFoldingLine(line, styler, 3)
#define IsDefineLine(line)		IsCppFoldingLine(line, styler, 4)
#define IsUnDefLine(line)		IsCppFoldingLine(line, styler, 5)

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

static void FoldCppDoc(unsigned int startPos, int length, int initStyle, WordList *[], Accessor &styler) {
	if (styler.GetPropertyInt("fold") == 0)
		return;
	const bool foldComment = styler.GetPropertyInt("fold.comment", 1) != 0;
	const bool foldPreprocessor = styler.GetPropertyInt("fold.preprocessor", 1) != 0;
	//const bool foldAtElse = styler.GetPropertyInt("fold.at.else", 0) != 0;
	const bool foldCompact = styler.GetPropertyInt("fold.compact", 0) != 0;

	const int lexType = styler.GetPropertyInt("lexer.lang.type", LEX_CPP);
	const bool hasPreprocessor = _hasPreprocessor(lexType) || lexType == LEX_NSIS;

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
	bool isObjCProtocol = false;

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
		//if (hasPreprocessor && atEOL && IsPropertyLine(lineCurrent)) {
		//	if(!IsPropertyLine(lineCurrent-1) && IsPropertyLine(lineCurrent+1))
		//		levelNext++;
		//	else if(IsPropertyLine(lineCurrent-1) && !IsPropertyLine(lineCurrent+1))
		//		levelNext--;
		//}
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
		if (atEOL && !(hasPreprocessor && IsCppInDefine(i, styler)) && IsLeftBraceLine(lineCurrent+1, styler)) {
			levelNext++;
		}

		if ((hasPreprocessor || lexType == LEX_HX) && foldPreprocessor && (ch == '#' || ch == '!') && style == SCE_C_PREPROCESSOR) {
			int pos = LexSkipSpaceTab(i+1, endPos, styler);
			if (LexMatchIC(pos, "if") || styler.Match(pos, "region")) {
				levelNext++;
			} else if (LexMatchIC(pos, "end")) {
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
			} else if (lexType == LEX_NSIS && stylePrev != SCE_C_PREPROCESSOR) {
				if (LexMatchIC(pos, "macroend")) {
					levelNext--;
				} else if (LexMatchIC(pos, "macro")) {
					levelNext++;
				}
			}
		}

		// Objctive C/C++
		if ((lexType == LEX_CPP || lexType == LEX_OBJC) && ch == '@' && style == SCE_C_DIRECTIVE) {
			if (styler.Match(i+1, "interface") || styler.Match(i+1, "implementation")) {
				levelNext++;
			} else if (styler.Match(i+1, "end")) {
				levelNext--;
				isObjCProtocol = false;
			} else if (styler.Match(i+1, "protocol")) {
				//char ch = LexGetNextChar(i+9, styler);
				if (LexGetNextChar(i+9, styler) != '(') { // @protocol()
					isObjCProtocol = true;
					levelNext++;
				}
			}
		}

		if (lexType != LEX_NSIS && style == SCE_C_OPERATOR && !(hasPreprocessor && IsCppInDefine(i, styler))) {
			// maybe failed in multi-line define section, MFC's afx.h is a example
			if (ch == '{' && !(lineCurrent > 0 && visibleChars == 0 && IsLeftBraceLine(lineCurrent, styler))) {
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

		// Resource Script
		if (style == SCE_C_WORD && stylePrev != SCE_C_WORD) {
			if (lexType == LEX_RC && (ch == 'B' || ch == 'E')) {
				if (styler.Match(i, "BEGIN")) {
					levelNext++;
				} else if (styler.Match(i, "END")) {
					levelNext--;
				}
			} else if (lexType == LEX_NSIS) {
				if (LexMatchIC(i, "functionend") || LexMatchIC(i, "sectionend")
					|| LexMatchIC(i, "sectiongroupend") || LexMatchIC(i, "pageexend")) {
					levelNext--;
				} else if (LexMatchIC(i, "function") || LexMatchIC(i, "section") || LexMatchIC(i, "pageex")) {
					levelNext++;
				}
			}
		}

		if (style == SCE_C_XML_TAG) {
			if (styler.Match(i, "</") || styler.Match(i, "/>"))
				levelNext--;
			else if (ch == '<' && chNext != '/')
				levelNext++;
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
			isObjCProtocol = false;
		}
	}
}

LexerModule lmCPP(SCLEX_CPP, ColouriseCppDoc, "cpp", FoldCppDoc, 0, 8);
