// Scintilla source code edit control
/** @file LexHTML.cxx
 ** Lexer for HTML.
 **/
// Copyright 1998-2005 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cassert>
#include <cstring>
#include <cctype>

#include <string>
#include <map>
#include <set>

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

namespace {

#define SCE_HA_JS (SCE_HJA_START - SCE_HJ_START)
#define SCE_HA_VBS (SCE_HBA_START - SCE_HB_START)
#define SCE_HA_PYTHON (SCE_HPA_START - SCE_HP_START)

enum script_type { eScriptNone = 0, eScriptJS, eScriptVBS, eScriptPython, eScriptPHP, eScriptXML, eScriptSGML, eScriptSGMLblock, eScriptComment };
enum script_mode { eHtml = 0, eNonHtmlScript, eNonHtmlPreProc, eNonHtmlScriptPreProc };

constexpr bool IsEmpty(const char *text) noexcept {
	return *text == '\0';
}

inline void GetTextSegment(Accessor &styler, Sci_PositionU start, Sci_PositionU end, char *s, size_t len) noexcept {
	styler.GetRangeLowered(start, end + 1, s, len);
}

const char *GetNextWord(Accessor &styler, Sci_PositionU start, char *s, size_t sLen) noexcept {
	Sci_PositionU i = 0;
	for (; i < sLen - 1; i++) {
		const char ch = styler.SafeGetCharAt(start + i);
		if ((i == 0) && !iswordstart(ch))
			break;
		if ((i > 0) && !iswordchar(ch))
			break;
		s[i] = ch;
	}
	s[i] = '\0';

	return s;
}

script_type segIsScriptingIndicator(Accessor &styler, Sci_PositionU start, Sci_PositionU end, script_type prevValue) {
	char s[128];
	GetTextSegment(styler, start, end, s, sizeof(s));
	//Platform::DebugPrintf("Scripting indicator [%s]\n", s);
	if (strstr(s, "src"))	// External script
		return eScriptNone;
	if (strstr(s, "vbs"))
		return eScriptVBS;
	if (strstr(s, "pyth"))
		return eScriptPython;
	if (strstr(s, "javas") || strstr(s, "ecmas") || strstr(s, "module"))
		return eScriptJS;
	if (strstr(s, "jscr"))
		return eScriptJS;
	if (strstr(s, "php"))
		return eScriptPHP;
	if (strstr(s, "xml")) {
		const char *xml = strstr(s, "xml");
		for (const char *t = s; t < xml; t++) {
			if (!IsASpace(*t)) {
				return prevValue;
			}
		}
		return eScriptXML;
	}

	return prevValue;
}

int PrintScriptingIndicatorOffset(Accessor &styler, Sci_PositionU start, Sci_PositionU end) noexcept {
	int iResult = 0;
	char s[8];
	GetTextSegment(styler, start, end, s, sizeof(s));
	if (0 == strncmp(s, "php", 3)) {
		iResult = 3;
	}

	return iResult;
}

script_type ScriptOfState(int state) noexcept {
	if ((state >= SCE_HP_START) && (state <= SCE_HP_IDENTIFIER)) {
		return eScriptPython;
	}
	if ((state >= SCE_HB_START) && (state <= SCE_HB_STRINGEOL)) {
		return eScriptVBS;
	}
	if ((state >= SCE_HJ_START) && (state <= SCE_HJ_REGEX)) {
		return eScriptJS;
	}
	if ((state >= SCE_HPHP_DEFAULT) && (state <= SCE_HPHP_COMMENTLINE)) {
		return eScriptPHP;
	}
	if ((state >= SCE_H_SGML_DEFAULT) && (state < SCE_H_SGML_BLOCK_DEFAULT)) {
		return eScriptSGML;
	}
	if (state == SCE_H_SGML_BLOCK_DEFAULT) {
		return eScriptSGMLblock;
	}
	return eScriptNone;
}

constexpr int statePrintForState(int state, script_mode inScriptType) noexcept {
	if (state >= SCE_HJ_START) {
		if ((state >= SCE_HP_START) && (state <= SCE_HP_IDENTIFIER)) {
			return state + ((inScriptType == eNonHtmlScript) ? 0 : SCE_HA_PYTHON);
		}
		if ((state >= SCE_HB_START) && (state <= SCE_HB_STRINGEOL)) {
			return state + ((inScriptType == eNonHtmlScript) ? 0 : SCE_HA_VBS);
		}
		if ((state >= SCE_HJ_START) && (state <= SCE_HJ_REGEX)) {
			return state + ((inScriptType == eNonHtmlScript) ? 0 : SCE_HA_JS);
		}
	}

	return state;
}

constexpr int stateForPrintState(int StateToPrint) noexcept {
	if ((StateToPrint >= SCE_HPA_START) && (StateToPrint <= SCE_HPA_IDENTIFIER)) {
		return StateToPrint - SCE_HA_PYTHON;
	}
	if ((StateToPrint >= SCE_HBA_START) && (StateToPrint <= SCE_HBA_STRINGEOL)) {
		return StateToPrint - SCE_HA_VBS;
	}
	if ((StateToPrint >= SCE_HJA_START) && (StateToPrint <= SCE_HJA_REGEX)) {
		return StateToPrint - SCE_HA_JS;
	}

	return StateToPrint;
}

bool IsNumber(Sci_PositionU start, Accessor &styler) noexcept {
	return IsADigit(styler[start]) || (styler[start] == '.') ||
		(styler[start] == '-') || (styler[start] == '#');
}

constexpr bool isStringState(int state) noexcept {
	switch (state) {
	case SCE_HJ_DOUBLESTRING:
	case SCE_HJ_SINGLESTRING:
	case SCE_HJA_DOUBLESTRING:
	case SCE_HJA_SINGLESTRING:
	case SCE_HB_STRING:
	case SCE_HBA_STRING:
	case SCE_HP_STRING:
	case SCE_HP_CHARACTER:
	case SCE_HP_TRIPLE:
	case SCE_HP_TRIPLEDOUBLE:
	case SCE_HPA_STRING:
	case SCE_HPA_CHARACTER:
	case SCE_HPA_TRIPLE:
	case SCE_HPA_TRIPLEDOUBLE:
	case SCE_HPHP_HSTRING:
	case SCE_HPHP_HEREDOC:
	case SCE_HPHP_SIMPLESTRING:
	case SCE_HPHP_NOWDOC:
	case SCE_HPHP_HSTRING_VARIABLE:
	case SCE_HPHP_COMPLEX_VARIABLE:
		return true;
	default:
		return false;
	}
}

constexpr bool stateAllowsTermination(int state) noexcept {
	bool allowTermination = !isStringState(state);
	if (allowTermination) {
		switch (state) {
		case SCE_HB_COMMENTLINE:
		case SCE_HPHP_COMMENT:
		case SCE_HP_COMMENTLINE:
		case SCE_HPA_COMMENTLINE:
			allowTermination = false;
		}
	}
	return allowTermination;
}

// not really well done, since it's only comments that should lex the %> and <%
constexpr bool isCommentASPState(int state) noexcept {
	return state == SCE_HJ_COMMENT
		|| state == SCE_HJ_COMMENTLINE
		|| state == SCE_HJ_COMMENTDOC
		|| state == SCE_HB_COMMENTLINE
		|| state == SCE_HP_COMMENTLINE
		|| state == SCE_HPHP_COMMENT
		|| state == SCE_HPHP_COMMENTLINE;
}

void classifyAttribHTML(Sci_PositionU start, Sci_PositionU end, const WordList &keywords, Accessor &styler) {
	const bool wordIsNumber = IsNumber(start, styler);
	char chAttr = SCE_H_ATTRIBUTEUNKNOWN;
	if (wordIsNumber) {
		chAttr = SCE_H_NUMBER;
	} else {
		char s[128];
		GetTextSegment(styler, start, end, s, sizeof(s));
		if (keywords.InList(s))
			chAttr = SCE_H_ATTRIBUTE;
	}
	if ((chAttr == SCE_H_ATTRIBUTEUNKNOWN) && !keywords)
		// No keywords -> all are known
		chAttr = SCE_H_ATTRIBUTE;
	styler.ColourTo(end, chAttr);
}

// https://html.spec.whatwg.org/multipage/custom-elements.html#custom-elements-core-concepts
bool isHTMLCustomElement(const char *tag, size_t length) noexcept {
	// check valid HTML custom element name: starts with an ASCII lower alpha and contains hyphen.
	// IsAlpha() is used for `html.tags.case.sensitive=1`.
	if (length < 2 || !IsAlpha(tag[0])) {
		return false;
	}
	if (strchr(tag, '-') == nullptr) {
		return false;
	}
	return true;
}

int classifyTagHTML(Sci_PositionU start, Sci_PositionU end,
	const WordList &keywords, Accessor &styler, bool &tagDontFold,
	bool caseSensitive, bool isXml, bool allowScripts) {
	char withSpace[126 + 2] = " ";
	const char *tag = withSpace + 1;
	// Copy after the '<' and stop before space
	Sci_PositionU i = 1;
	for (Sci_PositionU cPos = start; cPos <= end && i < sizeof(withSpace) - 2; cPos++) {
		const char ch = styler[cPos];
		if (IsASpace(ch)) {
			break;
		}
		if ((ch != '<') && (ch != '/')) {
			withSpace[i++] = caseSensitive ? ch : MakeLowerCase(ch);
		}
	}

	//The following is only a quick hack, to see if this whole thing would work
	//we first need the tagname with a trailing space...
	withSpace[i] = ' ';
	withSpace[i + 1] = '\0';

	// if the current language is XML, I can fold any tag
	// if the current language is HTML, I don't want to fold certain tags (input, meta, etc.)
	//...to find it in the list of no-container-tags
	tagDontFold = (!isXml) && (nullptr != strstr(
		// void elements
		" area base basefont br col command embed frame hr img input isindex keygen link meta param source track wbr "
		// end tag may omittd
		" p "
		, withSpace));

	//now we can remove the trailing space
	withSpace[i] = '\0';

	// No keywords -> all are known
	char chAttr = SCE_H_TAGUNKNOWN;
	bool customElement = false;
	if (tag[0] == '!') {
		chAttr = SCE_H_SGML_DEFAULT;
	} else if (!keywords || keywords.InList(tag)) {
		chAttr = SCE_H_TAG;
	} else if (!isXml && isHTMLCustomElement(tag, i - 1)) {
		customElement = true;
		chAttr = SCE_H_TAG;
	}
	if (chAttr != SCE_H_TAGUNKNOWN) {
		styler.ColourTo(end, chAttr);
	}
	if (chAttr == SCE_H_TAG && !customElement) {
		if (allowScripts && 0 == strcmp(tag, "script")) {
			// check to see if this is a self-closing tag by sniffing ahead
			bool isSelfClose = false;
			for (Sci_PositionU cPos = end; cPos <= end + 200; cPos++) {
				const char ch = styler.SafeGetCharAt(cPos);
				if (ch == '\0' || ch == '>')
					break;
				else if (ch == '/' && styler.SafeGetCharAt(cPos + 1) == '>') {
					isSelfClose = true;
					break;
				}
			}

			// do not enter a script state if the tag self-closed
			if (!isSelfClose)
				chAttr = SCE_H_SCRIPT;
		} else if (!isXml && 0 == strcmp(tag, "comment")) {
			chAttr = SCE_H_COMMENT;
		}
	}
	return chAttr;
}

void classifyWordHTJS(Sci_PositionU start, Sci_PositionU end,
	const WordList &keywords, Accessor &styler, script_mode inScriptType) {
	char s[127 + 1];
	styler.GetRange(start, end + 1, s, sizeof(s));
	char chAttr = SCE_HJ_WORD;
	if (keywords.InList(s)) {
		chAttr = SCE_HJ_KEYWORD;
	}
	styler.ColourTo(end, statePrintForState(chAttr, inScriptType));
}

int classifyWordHTVB(Sci_PositionU start, Sci_PositionU end, const WordList &keywords, Accessor &styler, script_mode inScriptType) {
	char chAttr = SCE_HB_IDENTIFIER;
	char s[128];
	GetTextSegment(styler, start, end, s, sizeof(s));
	if (keywords.InList(s)) {
		chAttr = SCE_HB_WORD;
		if (strcmp(s, "rem") == 0)
			chAttr = SCE_HB_COMMENTLINE;
	}
	styler.ColourTo(end, statePrintForState(chAttr, inScriptType));
	if (chAttr == SCE_HB_COMMENTLINE)
		return SCE_HB_COMMENTLINE;
	else
		return SCE_HB_DEFAULT;
}

void classifyWordHTPy(Sci_PositionU start, Sci_PositionU end, const WordList &keywords, Accessor &styler, char *prevWord, script_mode inScriptType, bool isMako) {
	char s[127 + 1];
	styler.GetRange(start, end + 1, s, sizeof(s));
	char chAttr = SCE_HP_IDENTIFIER;
	if (0 == strcmp(prevWord, "class"))
		chAttr = SCE_HP_CLASSNAME;
	else if (0 == strcmp(prevWord, "def"))
		chAttr = SCE_HP_DEFNAME;
	else if (keywords.InList(s))
		chAttr = SCE_HP_WORD;
	else if (isMako && 0 == strcmp(s, "block"))
		chAttr = SCE_HP_WORD;
	styler.ColourTo(end, statePrintForState(chAttr, inScriptType));
	strcpy(prevWord, s);
}

// Update the word colour to default or keyword
// Called when in a PHP word
void classifyWordHTPHP(Sci_PositionU start, Sci_PositionU end, const WordList &keywords, Accessor &styler) {
	char chAttr = SCE_HPHP_DEFAULT;
	char s[128];
	GetTextSegment(styler, start, end, s, sizeof(s));
	if (keywords.InList(s)) {
		chAttr = SCE_HPHP_WORD;
	}
	styler.ColourTo(end, chAttr);
}

bool isWordHSGML(Sci_PositionU start, Sci_PositionU end, const WordList &keywords, Accessor &styler) noexcept {
	char s[63 + 1];
	styler.GetRange(start, end + 1, s, sizeof(s));
	return keywords.InList(s);
}

inline bool isWordCdata(Sci_PositionU start, Accessor &styler) noexcept {
	return styler.Match(start, "[CDATA[");
}

// Return the first state to reach when entering a scripting language
constexpr int StateForScript(script_type scriptLanguage) noexcept {
	switch (scriptLanguage) {
	case eScriptVBS:
		return SCE_HB_START;
	case eScriptPython:
		return SCE_HP_START;
	case eScriptPHP:
		return SCE_HPHP_DEFAULT;
	case eScriptXML:
		return SCE_H_TAGUNKNOWN;
	case eScriptSGML:
		return SCE_H_SGML_DEFAULT;
	case eScriptComment:
		return SCE_H_COMMENT;
	default :
		return SCE_HJ_START;
	}
}

constexpr int defaultStateForSGML(script_type scriptLanguage) noexcept {
	return (scriptLanguage == eScriptSGMLblock)? SCE_H_SGML_BLOCK_DEFAULT : SCE_H_SGML_DEFAULT;
}

bool issgmlwordchar(int ch) noexcept {
	return !IsASCII(ch) ||
		(isalnum(ch) || ch == '.' || ch == '_' || ch == ':' || ch == '!' || ch == '#' || ch == '[');
}

constexpr bool InTagState(int state) noexcept {
	return state == SCE_H_TAG || state == SCE_H_TAGUNKNOWN ||
	       state == SCE_H_SCRIPT ||
	       state == SCE_H_ATTRIBUTE || state == SCE_H_ATTRIBUTEUNKNOWN ||
	       state == SCE_H_NUMBER || state == SCE_H_OTHER ||
	       state == SCE_H_DOUBLESTRING || state == SCE_H_SINGLESTRING;
}

constexpr bool IsCommentState(const int state) noexcept {
	return state == SCE_H_COMMENT || state == SCE_H_SGML_COMMENT;
}

constexpr bool IsScriptCommentState(const int state) noexcept {
	return state == SCE_HJ_COMMENT || state == SCE_HJ_COMMENTLINE || state == SCE_HJA_COMMENT ||
		   state == SCE_HJA_COMMENTLINE || state == SCE_HB_COMMENTLINE || state == SCE_HBA_COMMENTLINE;
}

bool isMakoBlockEnd(const int ch, const int chNext, const char *blockType) noexcept {
	if (IsEmpty(blockType)) {
		return ((ch == '%') && (chNext == '>'));
	} else if ((0 == strcmp(blockType, "inherit")) ||
		(0 == strcmp(blockType, "namespace")) ||
		(0 == strcmp(blockType, "include")) ||
		(0 == strcmp(blockType, "page"))) {
		return ((ch == '/') && (chNext == '>'));
	} else if (0 == strcmp(blockType, "%")) {
		if (ch == '/' && IsEOLChar(chNext))
			return true;
		else
			return IsEOLChar(ch);
	} else if (0 == strcmp(blockType, "{")) {
		return ch == '}';
	} else {
		return (ch == '>');
	}
}

bool isDjangoBlockEnd(const int ch, const int chNext, const char *blockType) noexcept {
	if (IsEmpty(blockType)) {
		return false;
	} else if (0 == strcmp(blockType, "%")) {
		return ((ch == '%') && (chNext == '}'));
	} else if (0 == strcmp(blockType, "{")) {
		return ((ch == '}') && (chNext == '}'));
	} else {
		return false;
	}
}

constexpr bool isPHPStringState(int state) noexcept {
	return
	    (state == SCE_HPHP_HSTRING) ||
	    (state == SCE_HPHP_SIMPLESTRING) ||
	    (state == SCE_HPHP_HSTRING_VARIABLE) ||
	    (state == SCE_HPHP_HEREDOC) ||
	    (state == SCE_HPHP_NOWDOC) ||
	    (state == SCE_HPHP_COMPLEX_VARIABLE);
}

Sci_Position FindPhpStringDelimiter(char *phpStringDelimiter, const int phpStringDelimiterSize, Sci_Position i, const Sci_Position lengthDoc, Accessor &styler, int &heardocQuotes) noexcept {
	Sci_Position j;
	const Sci_Position beginning = i - 1;
	bool isValidSimpleString = false;
	heardocQuotes = 0;

	while (i < lengthDoc && (styler[i] == ' ' || styler[i] == '\t')) {
		i++;
	}

	char ch = styler.SafeGetCharAt(i);
	const char chNext = styler.SafeGetCharAt(i + 1);
	if (!IsIdentifierStartEx(ch)) {
		if ((ch == '\'' || ch == '\"') && IsIdentifierStartEx(chNext)) {
			i++;
			ch = chNext;
			heardocQuotes = (ch == '\'') ? 1 : 2;
		} else {
			phpStringDelimiter[0] = '\0';
			return beginning;
		}
	}
	phpStringDelimiter[0] = ch;
	i++;

	for (j = i; j < lengthDoc && !IsEOLChar(styler[j]); j++) {
		if (!IsIdentifierCharEx(styler[j])) {
			if (heardocQuotes && (styler[j] == '\'' || styler[j] == '\"') && IsEOLChar(styler.SafeGetCharAt(j + 1))) {
				isValidSimpleString = true;
				j++;
				break;
			} else {
				phpStringDelimiter[0] = '\0';
				return beginning;
			}
		}
		if (j - i < phpStringDelimiterSize - 2)
			phpStringDelimiter[j - i + 1] = styler[j];
		else
			i++;
	}
	if (heardocQuotes && !isValidSimpleString) {
		phpStringDelimiter[0] = '\0';
		return beginning;
	}
	phpStringDelimiter[j - i + 1 - (heardocQuotes ? 1 : 0)] = '\0';
	return j - 1;
}

void ColouriseHyperTextDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists,
                                  Accessor &styler, bool isXml) {
	const WordList &keywords_Tag = *keywordLists[0];
	const WordList &keywords_JS = *keywordLists[1];
	const WordList &keywords_VBS = *keywordLists[2];
	const WordList &keywords_PY = *keywordLists[3];
	const WordList &keywords_PHP = *keywordLists[4];
	const WordList &keywords_SGML = *keywordLists[5]; // SGML (DTD)
	const WordList &keywords_Attr = *keywordLists[6];

	styler.StartAt(startPos);
	char prevWord[256];
	prevWord[0] = '\0';
	char phpStringDelimiter[256]; // PHP is not limited in length, we are
	phpStringDelimiter[0] = '\0';
	int outerStyle = SCE_H_DEFAULT;
	int StateToPrint = initStyle;
	int state = stateForPrintState(StateToPrint);
	char makoBlockType[256];
	makoBlockType[0] = '\0';
	int makoComment = 0;
	char djangoBlockType[2];
	djangoBlockType[0] = '\0';

	// If inside a tag, it may be a script tag, so reread from the start of line starting tag to ensure any language tags are seen
	if (InTagState(state)) {
		while ((startPos > 0) && (InTagState(styler.StyleAt(startPos - 1)))) {
			const Sci_Position backLineStart = styler.LineStart(styler.GetLine(startPos-1));
			length += startPos - backLineStart;
			startPos = backLineStart;
		}
		state = SCE_H_DEFAULT;
	}
	// String can be heredoc, must find a delimiter first. Reread from beginning of line containing the string, to get the correct lineState
	if (isPHPStringState(state)) {
		while (startPos > 0 && (isPHPStringState(state) || !IsEOLChar(styler[startPos - 1]))) {
			startPos--;
			length++;
			state = styler.StyleAt(startPos);
		}
		if (startPos == 0)
			state = SCE_H_DEFAULT;
	}
	styler.StartAt(startPos);

	/* Nothing handles getting out of these, so we need not start in any of them.
	 * As we're at line start and they can't span lines, we'll re-detect them anyway */
	switch (state) {
		case SCE_H_QUESTION:
		case SCE_H_XMLSTART:
		case SCE_H_XMLEND:
		case SCE_H_ASP:
			state = SCE_H_DEFAULT;
			break;
	}

	Sci_Position lineCurrent = styler.GetLine(startPos);
	int lineState;
	if (lineCurrent > 0) {
		lineState = styler.GetLineState(lineCurrent-1);
	} else {
		// Default client and ASP scripting language is JavaScript
		lineState = eScriptJS << 8;

		// property asp.default.language
		//	Script in ASP code is initially assumed to be in JavaScript.
		//	To change this to VBScript set asp.default.language to 2. Python is 3.
		lineState |= styler.GetPropertyInt("asp.default.language", eScriptJS) << 4;
	}
	script_mode inScriptType = static_cast<script_mode>((lineState >> 0) & 0x03); // 2 bits of scripting mode
	bool tagOpened = (lineState >> 2) & 0x01; // 1 bit to know if we are in an opened tag
	bool tagClosing = (lineState >> 3) & 0x01; // 1 bit to know if we are in a closing tag
	bool tagDontFold = false; //some HTML tags should not be folded
	script_type aspScript = static_cast<script_type>((lineState >> 4) & 0x0F); // 4 bits of script name
	script_type clientScript = static_cast<script_type>((lineState >> 8) & 0x0F); // 4 bits of script name
	int beforePreProc = (lineState >> 12) & 0xFF; // 8 bits of state

	script_type scriptLanguage = ScriptOfState(state);
	// If eNonHtmlScript coincides with SCE_H_COMMENT, assume eScriptComment
	if (inScriptType == eNonHtmlScript && state == SCE_H_COMMENT) {
		scriptLanguage = eScriptComment;
	}
	script_type beforeLanguage = ScriptOfState(beforePreProc);

	// property fold.html
	//	Folding is turned on or off for HTML and XML files with this option.
	//	The fold option must also be on for folding to occur.
	const bool foldHTML = styler.GetPropertyInt("fold.html", 0) != 0;

	const bool fold = foldHTML && styler.GetPropertyInt("fold", 0);

	// property fold.html.preprocessor
	//	Folding is turned on or off for scripts embedded in HTML files with this option.
	//	The default is on.
	const bool foldHTMLPreprocessor = foldHTML && styler.GetPropertyInt("fold.html.preprocessor", 1);

	const bool foldCompact = styler.GetPropertyInt("fold.compact", 1) != 0;

	// property fold.hypertext.comment
	//	Allow folding for comments in scripts embedded in HTML.
	//	The default is off.
	const bool foldComment = fold && styler.GetPropertyInt("fold.hypertext.comment", 0) != 0;

	// property fold.hypertext.heredoc
	//	Allow folding for heredocs in scripts embedded in HTML.
	//	The default is off.
	const bool foldHeredoc = fold && styler.GetPropertyInt("fold.hypertext.heredoc", 0) != 0;

	// property fold.xml.at.tag.open
	//	Enable folding for XML at the start of open tag.
	//	The default is on.
	const bool foldXmlAtTagOpen = isXml && fold && styler.GetPropertyInt("fold.xml.at.tag.open", 1) != 0;

	// property html.tags.case.sensitive
	//	For XML and HTML, setting this property to 1 will make tags match in a case
	//	sensitive way which is the expected behaviour for XML and XHTML.
	const bool caseSensitive = styler.GetPropertyInt("html.tags.case.sensitive", 0) != 0;

	// property lexer.xml.allow.scripts
	//	Set to 0 to disable scripts in XML.
	const bool allowScripts = styler.GetPropertyInt("lexer.xml.allow.scripts", 1) != 0;

	// property lexer.html.mako
	//	Set to 1 to enable the mako template language.
	const bool isMako = styler.GetPropertyInt("lexer.html.mako", 0) != 0;

	// property lexer.html.django
	//	Set to 1 to enable the django template language.
	const bool isDjango = styler.GetPropertyInt("lexer.html.django", 0) != 0;

	const CharacterSet setHTMLWord(CharacterSet::setAlphaNum, ".-_:!#", 0x80, true);
	const CharacterSet setTagContinue(CharacterSet::setAlphaNum, ".-_:!#[]", 0x80, true);
	const CharacterSet setAttributeContinue(CharacterSet::setAlphaNum, ".-_:!#/", 0x80, true);
	// characters not allowed in unquoted attribute value
	const CharacterSet setNonAttrValue(CharacterSet::setNone, "\"\'\\`=<> \t\n\v\f\r");
	// TODO: also handle + and - (except if they're part of ++ or --) and return keywords
	const CharacterSet setOKBeforeJSRE(CharacterSet::setNone, "([{=,:;!%^&*|?~");

	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	int visibleChars = 0;
	int lineStartVisibleChars = 0;

	int chPrev = ' ';
	int ch = ' ';
	int chPrevNonWhite = ' ';
	// look back to set chPrevNonWhite properly for better regex colouring
	if (scriptLanguage == eScriptJS && startPos > 0) {
		Sci_Position back = startPos;
		int style = 0;
		while (--back) {
			style = styler.StyleAt(back);
			if (style < SCE_HJ_DEFAULT || style > SCE_HJ_COMMENTDOC)
				// includes SCE_HJ_COMMENT & SCE_HJ_COMMENTLINE
				break;
		}
		if (style == SCE_HJ_SYMBOLS) {
			chPrevNonWhite = static_cast<unsigned char>(styler.SafeGetCharAt(back));
		}
	}

	styler.StartSegment(startPos);
	const Sci_Position lengthDoc = startPos + length;
	for (Sci_Position i = startPos; i < lengthDoc; i++) {
		const int chPrev2 = chPrev;
		chPrev = ch;
		if (!IsASpace(ch) && state != SCE_HJ_COMMENT &&
			state != SCE_HJ_COMMENTLINE && state != SCE_HJ_COMMENTDOC)
			chPrevNonWhite = ch;
		ch = static_cast<unsigned char>(styler[i]);
		int chNext = static_cast<unsigned char>(styler.SafeGetCharAt(i + 1));
		const int chNext2 = static_cast<unsigned char>(styler.SafeGetCharAt(i + 2));

		// Handle DBCS codepages
		if (styler.IsLeadByte(static_cast<unsigned char>(ch))) {
			chPrev = ' ';
			i += 1;
			continue;
		}

		if ((!IsASpace(ch) || !foldCompact) && fold)
			visibleChars++;
		if (!IsASpace(ch))
			lineStartVisibleChars++;

		// decide what is the current state to print (depending of the script tag)
		StateToPrint = statePrintForState(state, inScriptType);

		// handle script folding
		if (fold) {
			switch (scriptLanguage) {
			case eScriptJS:
			case eScriptPHP:
				//not currently supported				case eScriptVBS:

				if ((state != SCE_HPHP_COMMENT) && (state != SCE_HPHP_COMMENTLINE) && (state != SCE_HJ_COMMENT) && (state != SCE_HJ_COMMENTLINE) && (state != SCE_HJ_COMMENTDOC) && (!isStringState(state))) {
				//Platform::DebugPrintf("state=%d, StateToPrint=%d, initStyle=%d\n", state, StateToPrint, initStyle);
				//if ((state == SCE_HPHP_OPERATOR) || (state == SCE_HPHP_DEFAULT) || (state == SCE_HJ_SYMBOLS) || (state == SCE_HJ_START) || (state == SCE_HJ_DEFAULT)) {
					if (ch == '#') {
						Sci_Position j = i + 1;
						while ((j < lengthDoc) && IsASpaceOrTab(styler.SafeGetCharAt(j))) {
							j++;
						}
						if (styler.Match(j, "region") || styler.Match(j, "if")) {
							levelCurrent++;
						} else if (styler.Match(j, "end")) {
							levelCurrent--;
						}
					} else if ((ch == '{') || (ch == '}') || (foldComment && (ch == '/') && (chNext == '*'))) {
						levelCurrent += (((ch == '{') || (ch == '/')) ? 1 : -1);
					}
				} else if (((state == SCE_HPHP_COMMENT) || (state == SCE_HJ_COMMENT)) && foldComment && (ch == '*') && (chNext == '/')) {
					levelCurrent--;
				}
				break;
			case eScriptPython:
				if (state != SCE_HP_COMMENTLINE && !isMako) {
					if ((ch == ':') && ((chNext == '\n') || (chNext == '\r' && chNext2 == '\n'))) {
						levelCurrent++;
					} else if ((ch == '\n') && !((chNext == '\r') && (chNext2 == '\n')) && (chNext != '\n')) {
						// check if the number of tabs is lower than the level
						int Findlevel = (levelCurrent & ~SC_FOLDLEVELBASE) * 8;
						for (Sci_Position j = 0; Findlevel > 0; j++) {
							const char chTmp = styler.SafeGetCharAt(i + j + 1);
							if (chTmp == '\t') {
								Findlevel -= 8;
							} else if (chTmp == ' ') {
								Findlevel--;
							} else {
								break;
							}
						}

						if (Findlevel > 0) {
							levelCurrent -= Findlevel / 8;
							if (Findlevel % 8)
								levelCurrent--;
						}
					}
				}
				break;
			default:
				break;
			}
		}

		if ((ch == '\r' && chNext != '\n') || (ch == '\n')) {
			// Trigger on CR only (Mac style) or either on LF from CR+LF (Dos/Win) or on LF alone (Unix)
			// Avoid triggering two times on Dos/Win
			// New line -> record any line state onto /next/ line
			if (fold) {
				int lev = levelPrev;
				if (visibleChars == 0)
					lev |= SC_FOLDLEVELWHITEFLAG;
				if ((levelCurrent > levelPrev) && (visibleChars > 0))
					lev |= SC_FOLDLEVELHEADERFLAG;

				styler.SetLevel(lineCurrent, lev);
				visibleChars = 0;
				levelPrev = levelCurrent;
			}
			styler.SetLineState(lineCurrent,
			                    ((inScriptType & 0x03) << 0) |
			                    ((tagOpened ? 1 : 0) << 2) |
			                    ((tagClosing ? 1 : 0) << 3) |
			                    ((aspScript & 0x0F) << 4) |
			                    ((clientScript & 0x0F) << 8) |
			                    ((beforePreProc & 0xFF) << 12));
			lineCurrent++;
			lineStartVisibleChars = 0;
		}

		// handle start of Mako comment line
		if (isMako && ch == '#' && chNext == '#') {
			makoComment = 1;
			state = SCE_HP_COMMENTLINE;
		}

		// handle end of Mako comment line
		else if (isMako && makoComment && IsEOLChar(ch)) {
			makoComment = 0;
			styler.ColourTo(i - 1, StateToPrint);
			if (scriptLanguage == eScriptPython) {
				state = SCE_HP_DEFAULT;
			} else {
				state = SCE_H_DEFAULT;
			}
		}

		// Allow falling through to mako handling code if newline is going to end a block
		if (((ch == '\r' && chNext != '\n') || (ch == '\n')) &&
			(!isMako || (0 != strcmp(makoBlockType, "%")))) {
		}
		// Ignore everything in mako comment until the line ends
		else if (isMako && makoComment) {
		}

		// generic end of script processing
		else if ((inScriptType == eNonHtmlScript) && (ch == '<') && (chNext == '/')) {
			// Check if it's the end of the script tag (or any other HTML tag)
			switch (state) {
				// in these cases, you can embed HTML tags (to confirm !!!!!!!!!!!!!!!!!!!!!!)
			case SCE_H_DOUBLESTRING:
			case SCE_H_SINGLESTRING:
			case SCE_HJ_COMMENT:
			case SCE_HJ_COMMENTDOC:
			//case SCE_HJ_COMMENTLINE: // removed as this is a common thing done to hide
			// the end of script marker from some JS interpreters.
			case SCE_HB_COMMENTLINE:
			case SCE_HBA_COMMENTLINE:
			case SCE_HJ_DOUBLESTRING:
			case SCE_HJ_SINGLESTRING:
			case SCE_HJ_REGEX:
			case SCE_HB_STRING:
			case SCE_HBA_STRING:
			case SCE_HP_STRING:
			case SCE_HP_TRIPLE:
			case SCE_HP_TRIPLEDOUBLE:
			case SCE_HPHP_HSTRING:
			case SCE_HPHP_HEREDOC:
			case SCE_HPHP_SIMPLESTRING:
			case SCE_HPHP_NOWDOC:
			case SCE_HPHP_COMMENT:
			case SCE_HPHP_COMMENTLINE:
				break;
			default :
				// check if the closing tag is a script tag
				if (const char *tag =
						(state == SCE_HJ_COMMENTLINE || isXml) ? "script" :
						((state == SCE_H_COMMENT) ? "comment" : nullptr)) {
					Sci_Position j = i + 2;
					char chr;
					do {
						chr = *tag++;
					} while (chr != 0 && chr == MakeLowerCase(styler.SafeGetCharAt(j++)));
					if (chr != 0) break;
				}
				// closing tag of the script (it's a closing HTML tag anyway)
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_TAGUNKNOWN;
				inScriptType = eHtml;
				scriptLanguage = eScriptNone;
				clientScript = eScriptJS;
				i += 2;
				visibleChars += 2;
				tagClosing = true;
				if (foldXmlAtTagOpen) {
					levelCurrent--;
				}
				continue;
			}
		}

		/////////////////////////////////////
		// handle the start of PHP pre-processor = Non-HTML
		else if ((state != SCE_H_ASPAT) &&
		         !isStringState(state) &&
		         (state != SCE_HPHP_COMMENT) &&
		         (state != SCE_HPHP_COMMENTLINE) &&
		         (ch == '<') &&
		         (chNext == '?') &&
				 !IsScriptCommentState(state)) {
 			beforeLanguage = scriptLanguage;
			scriptLanguage = segIsScriptingIndicator(styler, i + 2, i + 6, isXml ? eScriptXML : eScriptPHP);
			if ((scriptLanguage != eScriptPHP) && (isStringState(state) || (state==SCE_H_COMMENT))) continue;
			styler.ColourTo(i - 1, StateToPrint);
			beforePreProc = state;
			i++;
			visibleChars++;
			i += PrintScriptingIndicatorOffset(styler, styler.GetStartSegment() + 2, i + 6);
			if (scriptLanguage == eScriptXML)
				styler.ColourTo(i, SCE_H_XMLSTART);
			else
				styler.ColourTo(i, SCE_H_QUESTION);
			state = StateForScript(scriptLanguage);
			if (inScriptType == eNonHtmlScript)
				inScriptType = eNonHtmlScriptPreProc;
			else
				inScriptType = eNonHtmlPreProc;
			// Fold whole script, but not if the XML first tag (all XML-like tags in this case)
			if (foldHTMLPreprocessor && (scriptLanguage != eScriptXML)) {
				levelCurrent++;
			}
			// should be better
			ch = static_cast<unsigned char>(styler.SafeGetCharAt(i));
			continue;
		}

		// handle the start Mako template Python code
		else if (isMako && scriptLanguage == eScriptNone && ((ch == '<' && chNext == '%') ||
															 (lineStartVisibleChars == 1 && ch == '%') ||
															 (lineStartVisibleChars == 1 && ch == '/' && chNext == '%') ||
															 (ch == '$' && chNext == '{') ||
															 (ch == '<' && chNext == '/' && chNext2 == '%'))) {
			if (ch == '%' || ch == '/')
				strcpy(makoBlockType, "%");
			else if (ch == '$')
				strcpy(makoBlockType, "{");
			else if (chNext == '/')
				GetNextWord(styler, i + 3, makoBlockType, sizeof(makoBlockType));
			else
				GetNextWord(styler, i + 2, makoBlockType, sizeof(makoBlockType));
			styler.ColourTo(i - 1, StateToPrint);
			beforePreProc = state;
			if (inScriptType == eNonHtmlScript)
				inScriptType = eNonHtmlScriptPreProc;
			else
				inScriptType = eNonHtmlPreProc;

			if (chNext == '/') {
				i += 2;
				visibleChars += 2;
			} else if (ch != '%') {
				i++;
				visibleChars++;
			}
			state = SCE_HP_START;
			scriptLanguage = eScriptPython;
			styler.ColourTo(i, SCE_H_ASP);

			if (ch != '%' && ch != '$' && ch != '/') {
				i += static_cast<int>(strlen(makoBlockType));
				visibleChars += static_cast<int>(strlen(makoBlockType));
				if (keywords_PY.InList(makoBlockType))
					styler.ColourTo(i, SCE_HP_WORD);
				else
					styler.ColourTo(i, SCE_H_TAGUNKNOWN);
			}

			ch = static_cast<unsigned char>(styler.SafeGetCharAt(i));
			continue;
		}

		// handle the start/end of Django comment
		else if (isDjango && state != SCE_H_COMMENT && (ch == '{' && chNext == '#')) {
			styler.ColourTo(i - 1, StateToPrint);
			beforePreProc = state;
			beforeLanguage = scriptLanguage;
			if (inScriptType == eNonHtmlScript)
				inScriptType = eNonHtmlScriptPreProc;
			else
				inScriptType = eNonHtmlPreProc;
			i += 1;
			visibleChars += 1;
			scriptLanguage = eScriptComment;
			state = SCE_H_COMMENT;
			styler.ColourTo(i, SCE_H_ASP);
			ch = static_cast<unsigned char>(styler.SafeGetCharAt(i));
			continue;
		} else if (isDjango && state == SCE_H_COMMENT && (ch == '#' && chNext == '}')) {
			styler.ColourTo(i - 1, StateToPrint);
			i += 1;
			visibleChars += 1;
			styler.ColourTo(i, SCE_H_ASP);
			state = beforePreProc;
			if (inScriptType == eNonHtmlScriptPreProc)
				inScriptType = eNonHtmlScript;
			else
				inScriptType = eHtml;
			scriptLanguage = beforeLanguage;
			continue;
		}

		// handle the start Django template code
		else if (isDjango && scriptLanguage != eScriptPython && scriptLanguage != eScriptComment && (ch == '{' && (chNext == '%' ||  chNext == '{'))) {
			if (chNext == '%')
				strcpy(djangoBlockType, "%");
			else
				strcpy(djangoBlockType, "{");
			styler.ColourTo(i - 1, StateToPrint);
			beforePreProc = state;
			if (inScriptType == eNonHtmlScript)
				inScriptType = eNonHtmlScriptPreProc;
			else
				inScriptType = eNonHtmlPreProc;

			i += 1;
			visibleChars += 1;
			state = SCE_HP_START;
			beforeLanguage = scriptLanguage;
			scriptLanguage = eScriptPython;
			styler.ColourTo(i, SCE_H_ASP);

			ch = static_cast<unsigned char>(styler.SafeGetCharAt(i));
			continue;
		}

		// handle the start of ASP pre-processor = Non-HTML
		else if (!isMako && !isDjango && !isCommentASPState(state) && (ch == '<') && (chNext == '%') && !isPHPStringState(state)) {
			styler.ColourTo(i - 1, StateToPrint);
			beforePreProc = state;
			if (inScriptType == eNonHtmlScript)
				inScriptType = eNonHtmlScriptPreProc;
			else
				inScriptType = eNonHtmlPreProc;

			if (chNext2 == '@') {
				i += 2; // place as if it was the second next char treated
				visibleChars += 2;
				state = SCE_H_ASPAT;
			} else if ((chNext2 == '-') && (styler.SafeGetCharAt(i + 3) == '-')) {
				styler.ColourTo(i + 3, SCE_H_ASP);
				state = SCE_H_XCCOMMENT;
				scriptLanguage = eScriptVBS;
				continue;
			} else {
				if (chNext2 == '=') {
					i += 2; // place as if it was the second next char treated
					visibleChars += 2;
				} else {
					i++; // place as if it was the next char treated
					visibleChars++;
				}

				state = StateForScript(aspScript);
			}
			scriptLanguage = eScriptVBS;
			styler.ColourTo(i, SCE_H_ASP);
			// fold whole script
			if (foldHTMLPreprocessor)
				levelCurrent++;
			// should be better
			ch = static_cast<unsigned char>(styler.SafeGetCharAt(i));
			continue;
		}

		/////////////////////////////////////
		// handle the start of SGML language (DTD)
		else if (((scriptLanguage == eScriptNone) || (scriptLanguage == eScriptXML)) &&
				 (chPrev == '<') &&
				 (ch == '!') &&
				 (StateToPrint != SCE_H_CDATA) &&
				 (!IsCommentState(StateToPrint)) &&
				 (!IsScriptCommentState(StateToPrint))) {
			beforePreProc = state;
			styler.ColourTo(i - 2, StateToPrint);
			if ((chNext == '-') && (chNext2 == '-')) {
				state = SCE_H_COMMENT; // wait for a pending command
				styler.ColourTo(i + 2, SCE_H_COMMENT);
				i += 2; // follow styling after the --
			} else if (isWordCdata(i + 1, styler)) {
				state = SCE_H_CDATA;
			} else {
				styler.ColourTo(i, SCE_H_SGML_DEFAULT); // <! is default
				scriptLanguage = eScriptSGML;
				state = SCE_H_SGML_COMMAND; // wait for a pending command
			}
			// fold whole tag (-- when closing the tag)
			if (foldHTMLPreprocessor || state == SCE_H_COMMENT || state == SCE_H_CDATA)
				levelCurrent++;
			continue;
		}

		// handle the end of Mako Python code
		else if (isMako &&
			     ((inScriptType == eNonHtmlPreProc) || (inScriptType == eNonHtmlScriptPreProc)) &&
				 (scriptLanguage != eScriptNone) && stateAllowsTermination(state) &&
				 isMakoBlockEnd(ch, chNext, makoBlockType)) {
			if (state == SCE_H_ASPAT) {
				aspScript = segIsScriptingIndicator(styler,
				                                    styler.GetStartSegment(), i - 1, aspScript);
			}
			if (state == SCE_HP_WORD) {
				classifyWordHTPy(styler.GetStartSegment(), i - 1, keywords_PY, styler, prevWord, inScriptType, isMako);
			} else {
				styler.ColourTo(i - 1, StateToPrint);
			}
			if (0 != strcmp(makoBlockType, "%") && (0 != strcmp(makoBlockType, "{")) && ch != '>') {
				i++;
				visibleChars++;
		    }
			else if (0 == strcmp(makoBlockType, "%") && ch == '/') {
				i++;
				visibleChars++;
			}
			if (0 != strcmp(makoBlockType, "%") || ch == '/') {
				styler.ColourTo(i, SCE_H_ASP);
			}
			state = beforePreProc;
			if (inScriptType == eNonHtmlScriptPreProc)
				inScriptType = eNonHtmlScript;
			else
				inScriptType = eHtml;
			scriptLanguage = eScriptNone;
			continue;
		}

		// handle the end of Django template code
		else if (isDjango &&
			     ((inScriptType == eNonHtmlPreProc) || (inScriptType == eNonHtmlScriptPreProc)) &&
				 (scriptLanguage != eScriptNone) && stateAllowsTermination(state) &&
				 isDjangoBlockEnd(ch, chNext, djangoBlockType)) {
			if (state == SCE_H_ASPAT) {
				aspScript = segIsScriptingIndicator(styler,
				                                    styler.GetStartSegment(), i - 1, aspScript);
			}
			if (state == SCE_HP_WORD) {
				classifyWordHTPy(styler.GetStartSegment(), i - 1, keywords_PY, styler, prevWord, inScriptType, isMako);
			} else {
				styler.ColourTo(i - 1, StateToPrint);
			}
			i += 1;
			visibleChars += 1;
			styler.ColourTo(i, SCE_H_ASP);
			state = beforePreProc;
			if (inScriptType == eNonHtmlScriptPreProc)
				inScriptType = eNonHtmlScript;
			else
				inScriptType = eHtml;
			scriptLanguage = beforeLanguage;
			continue;
		}

		// handle the end of a pre-processor = Non-HTML
		else if ((!isMako && !isDjango && ((inScriptType == eNonHtmlPreProc) || (inScriptType == eNonHtmlScriptPreProc)) &&
				  (((scriptLanguage != eScriptNone) && stateAllowsTermination(state))) &&
				  (((ch == '%') || (ch == '?')) && (chNext == '>'))) ||
		         ((scriptLanguage == eScriptSGML) && (ch == '>') && (state != SCE_H_SGML_COMMENT))) {
			if (state == SCE_H_ASPAT) {
				aspScript = segIsScriptingIndicator(styler,
				                                    styler.GetStartSegment(), i - 1, aspScript);
			}
			// Bounce out of any ASP mode
			switch (state) {
			case SCE_HJ_WORD:
				classifyWordHTJS(styler.GetStartSegment(), i - 1, keywords_JS, styler, inScriptType);
				break;
			case SCE_HB_WORD:
				classifyWordHTVB(styler.GetStartSegment(), i - 1, keywords_VBS, styler, inScriptType);
				break;
			case SCE_HP_WORD:
				classifyWordHTPy(styler.GetStartSegment(), i - 1, keywords_PY, styler, prevWord, inScriptType, isMako);
				break;
			case SCE_HPHP_WORD:
				classifyWordHTPHP(styler.GetStartSegment(), i - 1, keywords_PHP, styler);
				break;
			case SCE_H_XCCOMMENT:
				styler.ColourTo(i - 1, state);
				break;
			default :
				styler.ColourTo(i - 1, StateToPrint);
				break;
			}
			if (scriptLanguage != eScriptSGML) {
				i++;
				visibleChars++;
			}
			if (ch == '%')
				styler.ColourTo(i, SCE_H_ASP);
			else if (scriptLanguage == eScriptXML)
				styler.ColourTo(i, SCE_H_XMLEND);
			else if (scriptLanguage == eScriptSGML)
				styler.ColourTo(i, SCE_H_SGML_DEFAULT);
			else
				styler.ColourTo(i, SCE_H_QUESTION);
			state = beforePreProc;
			if (inScriptType == eNonHtmlScriptPreProc)
				inScriptType = eNonHtmlScript;
			else
				inScriptType = eHtml;
			// Unfold all scripting languages, except for XML tag
			if (foldHTMLPreprocessor && (scriptLanguage != eScriptXML)) {
				levelCurrent--;
			}
			scriptLanguage = beforeLanguage;
			continue;
		}
		/////////////////////////////////////

		switch (state) {
		case SCE_H_DEFAULT:
			if (ch == '<') {
				// in HTML, fold on tag open and unfold on tag close
				tagOpened = true;
				tagClosing = (chNext == '/');
				if (foldXmlAtTagOpen && !(chNext == '/' || chNext == '?' || chNext == '!' || chNext == '-' || chNext == '%')) {
					levelCurrent++;
				}
				if (foldXmlAtTagOpen && chNext == '/') {
					levelCurrent--;
				}
				styler.ColourTo(i - 1, StateToPrint);
				if (chNext != '!')
					state = SCE_H_TAGUNKNOWN;
			} else if (ch == '&') {
				styler.ColourTo(i - 1, SCE_H_DEFAULT);
				state = SCE_H_ENTITY;
			}
			break;
		case SCE_H_SGML_DEFAULT:
		case SCE_H_SGML_BLOCK_DEFAULT:
//			if (scriptLanguage == eScriptSGMLblock)
//				StateToPrint = SCE_H_SGML_BLOCK_DEFAULT;

			if (ch == '\"') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_SGML_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_SGML_SIMPLESTRING;
			} else if ((ch == '-') && (chPrev == '-')) {
				if (static_cast<Sci_Position>(styler.GetStartSegment()) <= (i - 2)) {
					styler.ColourTo(i - 2, StateToPrint);
				}
				state = SCE_H_SGML_COMMENT;
			} else if (IsASCII(ch) && isalpha(ch) && (chPrev == '%')) {
				styler.ColourTo(i - 2, StateToPrint);
				state = SCE_H_SGML_ENTITY;
			} else if (ch == '#') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_SGML_SPECIAL;
			} else if (ch == '[') {
				styler.ColourTo(i - 1, StateToPrint);
				scriptLanguage = eScriptSGMLblock;
				state = SCE_H_SGML_BLOCK_DEFAULT;
			} else if (ch == ']') {
				if (scriptLanguage == eScriptSGMLblock) {
					styler.ColourTo(i, StateToPrint);
					scriptLanguage = eScriptSGML;
				} else {
					styler.ColourTo(i - 1, StateToPrint);
					styler.ColourTo(i, SCE_H_SGML_ERROR);
				}
				state = SCE_H_SGML_DEFAULT;
			} else if (scriptLanguage == eScriptSGMLblock) {
				if ((ch == '!') && (chPrev == '<')) {
					styler.ColourTo(i - 2, StateToPrint);
					styler.ColourTo(i, SCE_H_SGML_DEFAULT);
					state = SCE_H_SGML_COMMAND;
				} else if (ch == '>') {
					styler.ColourTo(i - 1, StateToPrint);
					styler.ColourTo(i, SCE_H_SGML_DEFAULT);
				}
			}
			break;
		case SCE_H_SGML_COMMAND:
			if ((ch == '-') && (chPrev == '-')) {
				styler.ColourTo(i - 2, StateToPrint);
				state = SCE_H_SGML_COMMENT;
			} else if (!issgmlwordchar(ch)) {
				if (isWordHSGML(styler.GetStartSegment(), i - 1, keywords_SGML, styler)) {
					styler.ColourTo(i - 1, StateToPrint);
					state = SCE_H_SGML_1ST_PARAM;
				} else {
					state = SCE_H_SGML_ERROR;
				}
			}
			break;
		case SCE_H_SGML_1ST_PARAM:
			// wait for the beginning of the word
			if ((ch == '-') && (chPrev == '-')) {
				styler.ColourTo(i - 2, defaultStateForSGML(scriptLanguage));
				state = SCE_H_SGML_1ST_PARAM_COMMENT;
			} else if (issgmlwordchar(ch)) {
				styler.ColourTo(i - 1, defaultStateForSGML(scriptLanguage));
				// find the length of the word
				int size = 1;
				while (setHTMLWord.Contains(static_cast<unsigned char>(styler.SafeGetCharAt(i + size))))
					size++;
				styler.ColourTo(i + size - 1, StateToPrint);
				i += size - 1;
				visibleChars += size - 1;
				ch = static_cast<unsigned char>(styler.SafeGetCharAt(i));
				state = defaultStateForSGML(scriptLanguage);
				continue;
			} else if (ch == '\"' || ch == '\'') {
				styler.ColourTo(i - 1, defaultStateForSGML(scriptLanguage));
				state = (ch == '\"')? SCE_H_SGML_DOUBLESTRING : SCE_H_SGML_SIMPLESTRING;
			}
			break;
		case SCE_H_SGML_ERROR:
			if ((ch == '-') && (chPrev == '-')) {
				styler.ColourTo(i - 2, StateToPrint);
				state = SCE_H_SGML_COMMENT;
			} else if (ch == '\"' || ch == '\'') {
				styler.ColourTo(i - 1, defaultStateForSGML(scriptLanguage));
				state = (ch == '\"')? SCE_H_SGML_DOUBLESTRING : SCE_H_SGML_SIMPLESTRING;
			}
			break;
		case SCE_H_SGML_DOUBLESTRING:
			if (ch == '\"') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_SGML_DEFAULT;
			}
			break;
		case SCE_H_SGML_SIMPLESTRING:
			if (ch == '\'') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_SGML_DEFAULT;
			}
			break;
		case SCE_H_SGML_COMMENT:
			if ((ch == '-') && (chPrev == '-')) {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_SGML_DEFAULT;
			}
			break;
		case SCE_H_CDATA:
			if ((chPrev2 == ']') && (chPrev == ']') && (ch == '>')) {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_DEFAULT;
				levelCurrent--;
			}
			break;
		case SCE_H_COMMENT:
			if ((scriptLanguage != eScriptComment) && (chPrev2 == '-') && (chPrev == '-') && (ch == '>')) {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_DEFAULT;
				levelCurrent--;
			}
			break;
		case SCE_H_SGML_1ST_PARAM_COMMENT:
			if ((ch == '-') && (chPrev == '-')) {
				styler.ColourTo(i, SCE_H_SGML_COMMENT);
				state = SCE_H_SGML_1ST_PARAM;
			}
			break;
		case SCE_H_SGML_SPECIAL:
			if (!IsUpperCase(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				if (IsASCII(ch) && isalnum(ch)) {
					state = SCE_H_SGML_ERROR;
				} else {
					state = SCE_H_SGML_DEFAULT;
				}
			}
			break;
		case SCE_H_SGML_ENTITY:
			if (ch == ';') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_SGML_DEFAULT;
			} else if (!(IsASCII(ch) && isalnum(ch)) && ch != '-' && ch != '.') {
				styler.ColourTo(i, SCE_H_SGML_ERROR);
				state = SCE_H_SGML_DEFAULT;
			}
			break;
		case SCE_H_ENTITY:
			if (ch == ';') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_DEFAULT;
			}
			if (ch != '#' && !(IsASCII(ch) && isalnum(ch))	// Should check that '#' follows '&', but it is unlikely anyway...
				&& ch != '.' && ch != '-' && ch != '_' && ch != ':') { // valid in XML
				if (!IsASCII(ch))	// Possibly start of a multibyte character so don't allow this byte to be in entity style
					styler.ColourTo(i-1, SCE_H_TAGUNKNOWN);
				else
					styler.ColourTo(i, SCE_H_TAGUNKNOWN);
				state = SCE_H_DEFAULT;
			}
			break;
		case SCE_H_TAGUNKNOWN:
			if (!setTagContinue.Contains(ch) && !((ch == '/') && (chPrev == '<'))) {
				int eClass = classifyTagHTML(styler.GetStartSegment(),
					i - 1, keywords_Tag, styler, tagDontFold, caseSensitive, isXml, allowScripts);
				if (eClass == SCE_H_SCRIPT || eClass == SCE_H_COMMENT) {
					if (!tagClosing) {
						inScriptType = eNonHtmlScript;
						scriptLanguage = eClass == SCE_H_SCRIPT ? clientScript : eScriptComment;
					} else {
						scriptLanguage = eScriptNone;
					}
					eClass = SCE_H_TAG;
				}
				if (ch == '>') {
					styler.ColourTo(i, eClass);
					if (inScriptType == eNonHtmlScript) {
						state = StateForScript(scriptLanguage);
					} else {
						state = SCE_H_DEFAULT;
					}
					tagOpened = false;
					if (!(foldXmlAtTagOpen || tagDontFold)) {
						if (tagClosing) {
							levelCurrent--;
						} else {
							levelCurrent++;
						}
					}
					tagClosing = false;
				} else if (ch == '/' && chNext == '>') {
					if (eClass == SCE_H_TAGUNKNOWN) {
						styler.ColourTo(i + 1, SCE_H_TAGUNKNOWN);
					} else {
						styler.ColourTo(i - 1, StateToPrint);
						styler.ColourTo(i + 1, SCE_H_TAG); // SCE_H_TAGEND
					}
					i++;
					ch = chNext;
					state = SCE_H_DEFAULT;
					tagOpened = false;
					if (foldXmlAtTagOpen) {
						levelCurrent--;
					}
				} else {
					if (eClass != SCE_H_TAGUNKNOWN) {
						if (eClass == SCE_H_SGML_DEFAULT) {
							state = SCE_H_SGML_DEFAULT;
						} else {
							state = SCE_H_OTHER;
						}
					} else if (true) {
						styler.ColourTo(i - 1, eClass);
						state = SCE_H_OTHER;
					}
				}
			}
			break;
		case SCE_H_ATTRIBUTE:
			if (!setAttributeContinue.Contains(ch)) {
				if (inScriptType == eNonHtmlScript) {
					const int scriptLanguagePrev = scriptLanguage;
					clientScript = segIsScriptingIndicator(styler, styler.GetStartSegment(), i - 1, scriptLanguage);
					scriptLanguage = clientScript;
					if ((scriptLanguagePrev != scriptLanguage) && (scriptLanguage == eScriptNone))
						inScriptType = eHtml;
				}
				classifyAttribHTML(styler.GetStartSegment(), i - 1, keywords_Attr, styler);
				if (ch == '>') {
					styler.ColourTo(i, SCE_H_TAG);
					if (inScriptType == eNonHtmlScript) {
						state = StateForScript(scriptLanguage);
					} else {
						state = SCE_H_DEFAULT;
					}
					tagOpened = false;
					if (!(foldXmlAtTagOpen || tagDontFold)) {
						if (tagClosing) {
							levelCurrent--;
						} else {
							levelCurrent++;
						}
					}
					tagClosing = false;
				} else if (ch == '=') {
					styler.ColourTo(i, SCE_H_OTHER);
					state = SCE_H_VALUE;
				} else {
					state = SCE_H_OTHER;
				}
			}
			break;
		case SCE_H_OTHER:
			if (ch == '>') {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i, SCE_H_TAG);
				if (inScriptType == eNonHtmlScript) {
					state = StateForScript(scriptLanguage);
				} else {
					state = SCE_H_DEFAULT;
				}
				tagOpened = false;
				if (!(foldXmlAtTagOpen || tagDontFold)) {
					if (tagClosing) {
						levelCurrent--;
					} else {
						levelCurrent++;
					}
				}
				tagClosing = false;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_SINGLESTRING;
			} else if (ch == '=') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_H_VALUE;
			} else if (ch == '/' && chNext == '>') {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i + 1, SCE_H_TAG); // SCE_H_TAGEND
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
				tagOpened = false;
				if (foldXmlAtTagOpen) {
					levelCurrent--;
				}
			} else if (ch == '?' && chNext == '>') {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i + 1, SCE_H_XMLSTART); // SCE_H_XMLEND
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
			} else if (setHTMLWord.Contains(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_H_ATTRIBUTE;
			}
			break;
		case SCE_H_DOUBLESTRING:
			if (ch == '\"') {
				if (inScriptType == eNonHtmlScript) {
					scriptLanguage = segIsScriptingIndicator(styler, styler.GetStartSegment(), i, scriptLanguage);
				}
				styler.ColourTo(i, SCE_H_DOUBLESTRING);
				state = SCE_H_OTHER;
			}
			break;
		case SCE_H_SINGLESTRING:
			if (ch == '\'') {
				if (inScriptType == eNonHtmlScript) {
					scriptLanguage = segIsScriptingIndicator(styler, styler.GetStartSegment(), i, scriptLanguage);
				}
				styler.ColourTo(i, SCE_H_SINGLESTRING);
				state = SCE_H_OTHER;
			}
			break;
		case SCE_H_VALUE:
			if (setNonAttrValue.Contains(ch)) {
				if (ch == '\"' && chPrev == '=') {
					// Should really test for being first character
					state = SCE_H_DOUBLESTRING;
				} else if (ch == '\'' && chPrev == '=') {
					state = SCE_H_SINGLESTRING;
				} else {
					if (IsNumber(styler.GetStartSegment(), styler)) {
						styler.ColourTo(i - 1, SCE_H_NUMBER);
					} else {
						styler.ColourTo(i - 1, StateToPrint);
					}
					if (ch == '>') {
						styler.ColourTo(i, SCE_H_TAG);
						if (inScriptType == eNonHtmlScript) {
							state = StateForScript(scriptLanguage);
						} else {
							state = SCE_H_DEFAULT;
						}
						tagOpened = false;
						if (!tagDontFold) {
							if (tagClosing) {
								levelCurrent--;
							} else {
								levelCurrent++;
							}
						}
						tagClosing = false;
					} else {
						state = SCE_H_OTHER;
					}
				}
			}
			break;
		case SCE_HJ_DEFAULT:
		case SCE_HJ_START:
		case SCE_HJ_SYMBOLS:
			if (IsNumberStart(ch, chNext)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_NUMBER;
			} else if (IsIdentifierStartEx(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_WORD;
			} else if (ch == '/' && chNext == '*') {
				styler.ColourTo(i - 1, StateToPrint);
				if (chNext2 == '*')
					state = SCE_HJ_COMMENTDOC;
				else
					state = SCE_HJ_COMMENT;
				if (chNext2 == '/') {
					// Eat the * so it isn't used for the end of the comment
					i++;
				}
			} else if (ch == '/' && chNext == '/') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
			} else if (ch == '/' && setOKBeforeJSRE.Contains(chPrevNonWhite)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_REGEX;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_SINGLESTRING;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
			           styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
			} else if ((ch == '-') && (chNext == '-') && (chNext2 == '>')) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
				i += 2;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i, statePrintForState(SCE_HJ_SYMBOLS, inScriptType));
				state = SCE_HJ_DEFAULT;
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HJ_START) {
					styler.ColourTo(i - 1, StateToPrint);
					state = SCE_HJ_DEFAULT;
				}
			}
			break;
		case SCE_HJ_WORD:
		case SCE_HJ_NUMBER:
			if (!(IsIdentifierCharEx(ch) || (state == SCE_HJ_NUMBER && IsNumberContinue(chPrev, ch, chNext)))) {
				if (state == SCE_HJ_NUMBER) {
					styler.ColourTo(i - 1, statePrintForState(SCE_HJ_NUMBER, inScriptType));
				} else {
					classifyWordHTJS(styler.GetStartSegment(), i - 1, keywords_JS, styler, inScriptType);
				}
				//styler.ColourTo(i - 1, eHTJSKeyword);
				state = SCE_HJ_DEFAULT;
				if (ch == '/' && chNext == '*') {
					if (chNext2 == '*')
						state = SCE_HJ_COMMENTDOC;
					else
						state = SCE_HJ_COMMENT;
				} else if (ch == '/' && chNext == '/') {
					state = SCE_HJ_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_HJ_DOUBLESTRING;
				} else if (ch == '\'') {
					state = SCE_HJ_SINGLESTRING;
				} else if ((ch == '-') && (chNext == '-') && (chNext2 == '>')) {
					styler.ColourTo(i - 1, StateToPrint);
					state = SCE_HJ_COMMENTLINE;
					i += 2;
				} else if (isoperator(ch)) {
					styler.ColourTo(i, statePrintForState(SCE_HJ_SYMBOLS, inScriptType));
					state = (ch == '.')? SCE_HJ_WORD : SCE_HJ_DEFAULT;
				}
			}
			break;
		case SCE_HJ_COMMENT:
		case SCE_HJ_COMMENTDOC:
			if (ch == '/' && chPrev == '*') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HJ_DEFAULT;
				ch = ' ';
			}
			break;
		case SCE_HJ_COMMENTLINE:
			if (IsEOLChar(ch)) {
				styler.ColourTo(i - 1, statePrintForState(SCE_HJ_COMMENTLINE, inScriptType));
				state = SCE_HJ_DEFAULT;
				ch = ' ';
			}
			break;
		case SCE_HJ_DOUBLESTRING:
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
				}
			} else if (ch == '\"') {
				styler.ColourTo(i, statePrintForState(SCE_HJ_DOUBLESTRING, inScriptType));
				state = SCE_HJ_DEFAULT;
			} else if ((inScriptType == eNonHtmlScript) && (ch == '-') && (chNext == '-') && (chNext2 == '>')) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
				i += 2;
			} else if (IsEOLChar(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_STRINGEOL;
			}
			break;
		case SCE_HJ_SINGLESTRING:
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
				}
			} else if (ch == '\'') {
				styler.ColourTo(i, statePrintForState(SCE_HJ_SINGLESTRING, inScriptType));
				state = SCE_HJ_DEFAULT;
			} else if ((inScriptType == eNonHtmlScript) && (ch == '-') && (chNext == '-') && (chNext2 == '>')) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
				i += 2;
			} else if (IsEOLChar(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				if (chPrev != '\\' && (chPrev2 != '\\' || chPrev != '\r' || ch != '\n')) {
					state = SCE_HJ_STRINGEOL;
				}
			}
			break;
		case SCE_HJ_STRINGEOL:
			if (!IsEOLChar(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HJ_DEFAULT;
			} else if (!IsEOLChar(chNext)) {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HJ_DEFAULT;
			}
			break;
		case SCE_HJ_REGEX:
			if (ch == '\r' || ch == '\n' || ch == '/') {
				if (ch == '/') {
					while (IsLowerCase(chNext)) {   // gobble regex flags
						i++;
						ch = chNext;
						chNext = static_cast<unsigned char>(styler.SafeGetCharAt(i + 1));
					}
				}
				styler.ColourTo(i, StateToPrint);
				state = SCE_HJ_DEFAULT;
			} else if (ch == '\\') {
				// Gobble up the quoted character
				if (chNext == '\\' || chNext == '/') {
					i++;
					ch = chNext;
					chNext = static_cast<unsigned char>(styler.SafeGetCharAt(i + 1));
				}
			}
			break;
		case SCE_HB_DEFAULT:
		case SCE_HB_START:
			if (IsNumberStart(ch, chNext)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_NUMBER;
			} else if (IsIdentifierStartEx(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_WORD;
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_STRING;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
			           styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_COMMENTLINE;
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i, statePrintForState(SCE_HB_DEFAULT, inScriptType));
				state = SCE_HB_DEFAULT;
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HB_START) {
					styler.ColourTo(i - 1, StateToPrint);
					state = SCE_HB_DEFAULT;
				}
			}
			break;
		case SCE_HB_WORD:
		case SCE_HB_NUMBER:
			if (!(IsIdentifierCharEx(ch) || (state == SCE_HB_NUMBER && IsNumberContinue(chPrev, ch, chNext)))) {
				if (state == SCE_HB_NUMBER) {
					styler.ColourTo(i - 1, statePrintForState(SCE_HB_NUMBER, inScriptType));
					state = SCE_HB_DEFAULT;
				} else {
					state = classifyWordHTVB(styler.GetStartSegment(), i - 1, keywords_VBS, styler, inScriptType);
				}
				if (state == SCE_HB_DEFAULT) {
					if (ch == '\"') {
						state = SCE_HB_STRING;
					} else if (ch == '\'') {
						state = SCE_HB_COMMENTLINE;
					} else if (isoperator(ch)) {
						styler.ColourTo(i, statePrintForState(SCE_HB_DEFAULT, inScriptType));
						state = (ch == '.')? SCE_HB_WORD : SCE_HB_DEFAULT;
					}
				}
			}
			break;
		case SCE_HB_STRING:
			if (ch == '\"') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HB_DEFAULT;
			} else if (IsEOLChar(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_STRINGEOL;
			}
			break;
		case SCE_HB_COMMENTLINE:
			if (IsEOLChar(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_DEFAULT;
			}
			break;
		case SCE_HB_STRINGEOL:
			if (!IsEOLChar(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HB_DEFAULT;
			} else if (!IsEOLChar(chNext)) {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HB_DEFAULT;
			}
			break;
		case SCE_HP_DEFAULT:
		case SCE_HP_START:
			if (IsNumberStart(ch, chNext)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HP_NUMBER;
			} else if (IsIdentifierStartEx(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HP_WORD;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
			           styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HP_COMMENTLINE;
			} else if (ch == '#') {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HP_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColourTo(i - 1, StateToPrint);
				if (chNext == '\"' && chNext2 == '\"') {
					i += 2;
					state = SCE_HP_TRIPLEDOUBLE;
					ch = ' ';
					chPrev = ' ';
					chNext = static_cast<unsigned char>(styler.SafeGetCharAt(i + 1));
				} else {
					//state = statePrintForState(SCE_HP_STRING, inScriptType);
					state = SCE_HP_STRING;
				}
			} else if (ch == '\'') {
				styler.ColourTo(i - 1, StateToPrint);
				if (chNext == '\'' && chNext2 == '\'') {
					i += 2;
					state = SCE_HP_TRIPLE;
					ch = ' ';
					chPrev = ' ';
					chNext = static_cast<unsigned char>(styler.SafeGetCharAt(i + 1));
				} else {
					state = SCE_HP_CHARACTER;
				}
			} else if (isoperator(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				styler.ColourTo(i, statePrintForState(SCE_HP_OPERATOR, inScriptType));
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HP_START) {
					styler.ColourTo(i - 1, StateToPrint);
					state = SCE_HP_DEFAULT;
				}
			}
			break;
		case SCE_HP_WORD:
		case SCE_HP_NUMBER:
			if (!(IsIdentifierCharEx(ch) || (state == SCE_HP_NUMBER && IsNumberContinue(chPrev, ch, chNext)))) {
				if (state == SCE_HP_NUMBER) {
					styler.ColourTo(i - 1, statePrintForState(SCE_HP_NUMBER, inScriptType));
				} else {
					classifyWordHTPy(styler.GetStartSegment(), i - 1, keywords_PY, styler, prevWord, inScriptType, isMako);
				}
				state = SCE_HP_DEFAULT;
				if (ch == '#') {
					state = SCE_HP_COMMENTLINE;
				} else if (ch == '\"') {
					if (chNext == '\"' && chNext2 == '\"') {
						i += 2;
						state = SCE_HP_TRIPLEDOUBLE;
						ch = ' ';
						chPrev = ' ';
						chNext = static_cast<unsigned char>(styler.SafeGetCharAt(i + 1));
					} else {
						state = SCE_HP_STRING;
					}
				} else if (ch == '\'') {
					if (chNext == '\'' && chNext2 == '\'') {
						i += 2;
						state = SCE_HP_TRIPLE;
						ch = ' ';
						chPrev = ' ';
						chNext = static_cast<unsigned char>(styler.SafeGetCharAt(i + 1));
					} else {
						state = SCE_HP_CHARACTER;
					}
				} else if (isoperator(ch)) {
					styler.ColourTo(i, statePrintForState(SCE_HP_OPERATOR, inScriptType));
					state = (ch == '.')? SCE_HP_WORD : SCE_HP_DEFAULT;
				}
			}
			break;
		case SCE_HP_COMMENTLINE:
			if (IsEOLChar(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HP_DEFAULT;
			}
			break;
		case SCE_HP_STRING:
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
					ch = chNext;
					chNext = static_cast<unsigned char>(styler.SafeGetCharAt(i + 1));
				}
			} else if (ch == '\"') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HP_DEFAULT;
			}
			break;
		case SCE_HP_CHARACTER:
			if (ch == '\\') {
				if (chNext == '\"' || chNext == '\'' || chNext == '\\') {
					i++;
					ch = chNext;
					chNext = static_cast<unsigned char>(styler.SafeGetCharAt(i + 1));
				}
			} else if (ch == '\'') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HP_DEFAULT;
			}
			break;
		case SCE_HP_TRIPLE:
			if (ch == '\'' && chPrev == '\'' && chPrev2 == '\'') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HP_DEFAULT;
			}
			break;
		case SCE_HP_TRIPLEDOUBLE:
			if (ch == '\"' && chPrev == '\"' && chPrev2 == '\"') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HP_DEFAULT;
			}
			break;
			///////////// start - PHP state handling
		case SCE_HPHP_WORD:
		case SCE_HPHP_NUMBER:
			if (!(IsIdentifierCharEx(ch) || (state == SCE_HPHP_NUMBER && IsNumberContinue(chPrev, ch, chNext)))) {
				if (state == SCE_HPHP_NUMBER) {
					styler.ColourTo(i - 1, SCE_HPHP_NUMBER);
					state = SCE_HPHP_DEFAULT;
				} else {
					classifyWordHTPHP(styler.GetStartSegment(), i - 1, keywords_PHP, styler);
				}
				if (ch == '/' && chNext == '*') {
					i++;
					state = SCE_HPHP_COMMENT;
				} else if (ch == '/' && chNext == '/') {
					i++;
					state = SCE_HPHP_COMMENTLINE;
				} else if (ch == '#') {
					state = SCE_HPHP_COMMENTLINE;
				} else if (ch == '\"') {
					state = SCE_HPHP_HSTRING;
					strcpy(phpStringDelimiter, "\"");
				} else if (styler.Match(i, "<<<")) {
					int heardocQuotes = 0;
					i = FindPhpStringDelimiter(phpStringDelimiter, sizeof(phpStringDelimiter), i + 3, lengthDoc, styler, heardocQuotes);
					if (!IsEmpty(phpStringDelimiter)) {
						state = ((heardocQuotes == 1) ? SCE_HPHP_NOWDOC : SCE_HPHP_HEREDOC);
						if (foldHeredoc) levelCurrent++;
					}
				} else if (ch == '\'') {
					state = SCE_HPHP_SIMPLESTRING;
					strcpy(phpStringDelimiter, "\'");
				} else if (ch == '$' && IsIdentifierStartEx(chNext)) {
					state = SCE_HPHP_VARIABLE;
				} else if (isoperator(ch)) {
					styler.ColourTo(i, SCE_HPHP_OPERATOR);
					state = (ch == '.')? SCE_HPHP_WORD : SCE_HPHP_OPERATOR;
				} else {
					state = SCE_HPHP_DEFAULT;
				}
			}
			break;
		case SCE_HPHP_VARIABLE:
			if (!IsIdentifierCharEx(chNext)) {
				styler.ColourTo(i, SCE_HPHP_VARIABLE);
				state = SCE_HPHP_DEFAULT;
			}
			break;
		case SCE_HPHP_COMMENT:
			if (ch == '/' && chPrev == '*') {
				styler.ColourTo(i, StateToPrint);
				state = SCE_HPHP_DEFAULT;
			}
			break;
		case SCE_HPHP_COMMENTLINE:
			if (IsEOLChar(ch)) {
				styler.ColourTo(i - 1, StateToPrint);
				state = SCE_HPHP_DEFAULT;
			}
			break;
		case SCE_HPHP_HSTRING:
		case SCE_HPHP_HEREDOC:
			if (ch == '\\' && (phpStringDelimiter[0] == '\"' || chNext == '$' || chNext == '{')) {
				// skip the next char
				i++;
			} else if (((ch == '{' && chNext == '$') || (ch == '$' && chNext == '{'))
				&& IsIdentifierStartEx(chNext2)) {
				styler.ColourTo(i - 1, StateToPrint);
				outerStyle = state;
				state = SCE_HPHP_COMPLEX_VARIABLE;
			} else if (ch == '$' && IsIdentifierStartEx(chNext)) {
				styler.ColourTo(i - 1, StateToPrint);
				outerStyle = state;
				state = SCE_HPHP_HSTRING_VARIABLE;
			} else if (styler.Match(i, phpStringDelimiter)) {
				if (phpStringDelimiter[0] == '\"') {
					styler.ColourTo(i, StateToPrint);
					state = SCE_HPHP_DEFAULT;
				} else if (IsEOLChar(chPrev)) {
					const int psdLength = static_cast<int>(strlen(phpStringDelimiter));
					const char chAfterPsd = styler.SafeGetCharAt(i + psdLength);
					const char chAfterPsd2 = styler.SafeGetCharAt(i + psdLength + 1);
					if (IsEOLChar(chAfterPsd) ||
						(chAfterPsd == ';' && IsEOLChar(chAfterPsd2))) {
							i += (((i + psdLength) < lengthDoc) ? psdLength : lengthDoc) - 1;
						styler.ColourTo(i, StateToPrint);
						state = SCE_HPHP_DEFAULT;
						if (foldHeredoc) levelCurrent--;
					}
				}
			}
			break;
		case SCE_HPHP_SIMPLESTRING:
		case SCE_HPHP_NOWDOC:
			if (phpStringDelimiter[0] == '\'') {
				if (ch == '\\') {
					// skip the next char
					i++;
				} else if (ch == '\'') {
					styler.ColourTo(i, StateToPrint);
					state = SCE_HPHP_DEFAULT;
				}
			} else if (IsEOLChar(chPrev) && styler.Match(i, phpStringDelimiter)) {
				const int psdLength = static_cast<int>(strlen(phpStringDelimiter));
				const char chAfterPsd = styler.SafeGetCharAt(i + psdLength);
				const char chAfterPsd2 = styler.SafeGetCharAt(i + psdLength + 1);
				if (IsEOLChar(chAfterPsd) ||
				(chAfterPsd == ';' && IsEOLChar(chAfterPsd2))) {
					i += (((i + psdLength) < lengthDoc) ? psdLength : lengthDoc) - 1;
					styler.ColourTo(i, StateToPrint);
					state = SCE_HPHP_DEFAULT;
					if (foldHeredoc) levelCurrent--;
				}
			}
			break;
		case SCE_HPHP_HSTRING_VARIABLE:
			if (!IsIdentifierCharEx(chNext) && !((chNext == '-' && chNext2 =='>') || (ch == '-' && chNext == '>'))) { // $this->variable
				styler.ColourTo(i, StateToPrint);
				state = outerStyle;
			}
			break;
		case SCE_HPHP_COMPLEX_VARIABLE:
			if (ch == '}') {
				styler.ColourTo(i, StateToPrint);
				state = outerStyle;
			}
			break;
		case SCE_HPHP_OPERATOR:
		case SCE_HPHP_DEFAULT:
			styler.ColourTo(i - 1, StateToPrint);
			if (IsNumberStart(ch, chNext)) {
				state = SCE_HPHP_NUMBER;
			} else if (IsIdentifierStartEx(ch)) {
				state = SCE_HPHP_WORD;
			} else if (ch == '/' && chNext == '*') {
				i++;
				state = SCE_HPHP_COMMENT;
			} else if (ch == '/' && chNext == '/') {
				i++;
				state = SCE_HPHP_COMMENTLINE;
			} else if (ch == '#') {
				state = SCE_HPHP_COMMENTLINE;
			} else if (ch == '\"') {
				state = SCE_HPHP_HSTRING;
				strcpy(phpStringDelimiter, "\"");
			} else if (styler.Match(i, "<<<")) {
				int heardocQuotes = 0;
				i = FindPhpStringDelimiter(phpStringDelimiter, sizeof(phpStringDelimiter), i + 3, lengthDoc, styler, heardocQuotes);
				if (!IsEmpty(phpStringDelimiter)) {
					state = ((heardocQuotes == 1) ? SCE_HPHP_NOWDOC : SCE_HPHP_HEREDOC);
					if (foldHeredoc) levelCurrent++;
				}
			} else if (ch == '\'') {
				state = SCE_HPHP_SIMPLESTRING;
				strcpy(phpStringDelimiter, "\'");
			} else if (ch == '$' && IsIdentifierStartEx(chNext)) {
				state = SCE_HPHP_VARIABLE;
			} else if (isoperator(ch) || ch == '@' || ch == '$') {
				state = SCE_HPHP_OPERATOR;
			} else if ((state == SCE_HPHP_OPERATOR) && (IsASpace(ch))) {
				state = SCE_HPHP_DEFAULT;
			}
			break;
			///////////// end - PHP state handling
		}

		// Some of the above terminated their lexeme but since the same character starts
		// the same class again, only reenter if non empty segment.

		const bool nonEmptySegment = i >= static_cast<Sci_Position>(styler.GetStartSegment());
		if (state == SCE_HB_DEFAULT) {    // One of the above succeeded
			if ((ch == '\"') && (nonEmptySegment)) {
				state = SCE_HB_STRING;
			} else if (ch == '\'') {
				state = SCE_HB_COMMENTLINE;
			} else if (iswordstart(ch)) {
				state = SCE_HB_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i, SCE_HB_DEFAULT);
			}
		} else if (state == SCE_HBA_DEFAULT) {    // One of the above succeeded
			if ((ch == '\"') && (nonEmptySegment)) {
				state = SCE_HBA_STRING;
			} else if (ch == '\'') {
				state = SCE_HBA_COMMENTLINE;
			} else if (iswordstart(ch)) {
				state = SCE_HBA_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i, SCE_HBA_DEFAULT);
			}
		} else if (state == SCE_HJ_DEFAULT) {    // One of the above succeeded
			if (ch == '/' && chNext == '*') {
				if (styler.SafeGetCharAt(i + 2) == '*')
					state = SCE_HJ_COMMENTDOC;
				else
					state = SCE_HJ_COMMENT;
			} else if (ch == '/' && chNext == '/') {
				state = SCE_HJ_COMMENTLINE;
			} else if ((ch == '\"') && (nonEmptySegment)) {
				state = SCE_HJ_DOUBLESTRING;
			} else if ((ch == '\'') && (nonEmptySegment)) {
				state = SCE_HJ_SINGLESTRING;
			} else if (iswordstart(ch)) {
				state = SCE_HJ_WORD;
			} else if (isoperator(ch)) {
				styler.ColourTo(i, statePrintForState(SCE_HJ_SYMBOLS, inScriptType));
			}
		}
	}

	switch (state) {
	case SCE_HJ_WORD:
		classifyWordHTJS(styler.GetStartSegment(), lengthDoc - 1, keywords_JS, styler, inScriptType);
		break;
	case SCE_HB_WORD:
		classifyWordHTVB(styler.GetStartSegment(), lengthDoc - 1, keywords_VBS, styler, inScriptType);
		break;
	case SCE_HP_WORD:
		classifyWordHTPy(styler.GetStartSegment(), lengthDoc - 1, keywords_PY, styler, prevWord, inScriptType, isMako);
		break;
	case SCE_HPHP_WORD:
		classifyWordHTPHP(styler.GetStartSegment(), lengthDoc - 1, keywords_PHP, styler);
		break;
	default:
		StateToPrint = statePrintForState(state, inScriptType);
		if (static_cast<Sci_Position>(styler.GetStartSegment()) < lengthDoc)
			styler.ColourTo(lengthDoc - 1, StateToPrint);
		break;
	}

	// Fill in the real level of the next line, keeping the current flags as they will be filled in later
	if (fold) {
		const int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
		styler.SetLevel(lineCurrent, levelPrev | flagsNext);
	}
	styler.Flush();
}

void ColouriseXMLDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	// Passing in true because we're lexing XML
	ColouriseHyperTextDoc(startPos, length, initStyle, keywordLists, styler, true);
}

void ColouriseHTMLDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	// Passing in false because we're notlexing XML
	if (startPos == 0) {
		const int php = styler.GetPropertyInt("lexer.lang.type", 0);
		if (php) {
			initStyle = SCE_HPHP_DEFAULT;
		}
	}
	ColouriseHyperTextDoc(startPos, length, initStyle, keywordLists, styler, false);
}

/*void ColourisePHPScriptDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	if (startPos == 0) {
		initStyle = SCE_HPHP_DEFAULT;
	}
	ColouriseHTMLDoc(startPos, length, initStyle, keywordLists, styler);
}*/

/*const char * const htmlWordListDesc[] = {
	"HTML elements and attributes",
	"JavaScript keywords",
	"VBScript keywords",
	"Python keywords",
	"PHP keywords",
	"SGML and DTD keywords",
	0,
};*/

/*const char * const phpscriptWordListDesc[] = {
	"", //Unused
	"", //Unused
	"", //Unused
	"", //Unused
	"PHP keywords",
	"", //Unused
	0,
};*/

}

LexerModule lmHTML(SCLEX_HTML, ColouriseHTMLDoc, "hypertext");
LexerModule lmXML(SCLEX_XML, ColouriseXMLDoc, "xml");
//LexerModule lmPHPSCRIPT(SCLEX_PHPSCRIPT, ColourisePHPScriptDoc, "phpscript");
