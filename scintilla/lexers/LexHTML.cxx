// Scintilla source code edit control
/** @file LexHTML.cxx
 ** Lexer for HTML.
 **/
// Copyright 1998-2005 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

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
#include "DocUtils.h"

using namespace Lexilla;

namespace {

#define SCE_HA_JS (SCE_HJA_START - SCE_HJ_START)
#define SCE_HA_VBS (SCE_HBA_START - SCE_HB_START)

enum script_type { eScriptNone = 0, eScriptJS, eScriptVBS, eScriptXML, eScriptSGML, eScriptSGMLblock, eScriptComment };
enum script_mode { eHtml = 0, eNonHtmlScript, eNonHtmlPreProc, eNonHtmlScriptPreProc };

inline void GetTextSegment(LexAccessor &styler, Sci_PositionU start, Sci_PositionU end, char *s, size_t len) noexcept {
	styler.GetRangeLowered(start, end, s, len);
}

script_type segIsScriptingIndicator(LexAccessor &styler, Sci_PositionU start, Sci_PositionU end, script_type prevValue) {
	char s[128];
	GetTextSegment(styler, start, end, s, sizeof(s));
	//Platform::DebugPrintf("Scripting indicator [%s]\n", s);
	if (strstr(s, "vbs"))
		return eScriptVBS;
	// https://html.spec.whatwg.org/multipage/scripting.html#attr-script-type
	// https://mimesniff.spec.whatwg.org/#javascript-mime-type
	if (strstr(s, "javas") || strstr(s, "ecmas") || strstr(s, "module") || strstr(s, "jscr"))
		return eScriptJS;
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

script_type ScriptOfState(int state) noexcept {
	if ((state >= SCE_HB_START && state <= SCE_HB_OPERATOR) || state == SCE_H_ASPAT || state == SCE_H_XCCOMMENT) {
		return eScriptVBS;
	}
	if ((state >= SCE_HJ_START) && (state <= SCE_HJ_TEMPLATELITERAL)) {
		return eScriptJS;
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
		if ((state >= SCE_HB_START) && (state <= SCE_HB_OPERATOR)) {
			return state + ((inScriptType == eNonHtmlScript) ? 0 : SCE_HA_VBS);
		}
		if ((state >= SCE_HJ_START) && (state <= SCE_HJ_REGEX)) {
			return state + ((inScriptType == eNonHtmlScript) ? 0 : SCE_HA_JS);
		}
	}

	return state;
}

constexpr int stateForPrintState(int StateToPrint) noexcept {
	if ((StateToPrint >= SCE_HBA_START) && (StateToPrint <= SCE_HBA_OPERATOR)) {
		return StateToPrint - SCE_HA_VBS;
	}
	if ((StateToPrint >= SCE_HJA_START) && (StateToPrint <= SCE_HJA_TEMPLATELITERAL)) {
		return StateToPrint - SCE_HA_JS;
	}

	return StateToPrint;
}

constexpr bool IsNumberChar(char ch) noexcept {
	return IsADigit(ch) || ch == '.' || ch == '-' || ch == '#';
}

constexpr bool IsEntityChar(int ch) noexcept {
	// Only allow [A-Za-z0-9.#-_:] in entities
	return IsAlphaNumeric(ch) || AnyOf(ch, '#', '.', '-', '_', ':');
}

constexpr bool isStringState(int state) noexcept {
	switch (state) {
	case SCE_HJ_DOUBLESTRING:
	case SCE_HJ_SINGLESTRING:
	case SCE_HJ_TEMPLATELITERAL:
	case SCE_HJA_DOUBLESTRING:
	case SCE_HJA_SINGLESTRING:
	case SCE_HJA_TEMPLATELITERAL:
	case SCE_HB_STRING:
	case SCE_HBA_STRING:
		return true;
	default:
		return false;
	}
}

constexpr bool stateAllowsTermination(int state, int ch) noexcept {
	if (!isStringState(state)) {
		if (state == SCE_H_ASP || state == SCE_H_ASPAT || state == SCE_H_XCCOMMENT || state >= SCE_HJ_START) {
			return ch == '%'; // ASP, JSP %>
		}
		return ch == '?'; // XML ?>
	}
	return false;
}

// not really well done, since it's only comments that should lex the %> and <%
constexpr bool isCommentASPState(int state) noexcept {
	return state == SCE_HJ_COMMENT
		|| state == SCE_HJ_COMMENTLINE
		|| state == SCE_HJ_COMMENTDOC
		|| state == SCE_HB_COMMENTLINE;
}

bool classifyAttribHTML(script_mode inScriptType, Sci_PositionU start, Sci_PositionU end, const WordList &keywords, const WordList &keywordsEvent, LexAccessor &styler) {
	char chAttr = SCE_H_ATTRIBUTEUNKNOWN;
	bool isLanguageType = false;
	if (IsNumberChar(styler[start])) {
		chAttr = SCE_H_NUMBER;
	} else {
		char s[128];
		GetTextSegment(styler, start, end, s, sizeof(s));
		if (keywords.InList(s) || keywordsEvent.InList(s))
			chAttr = SCE_H_ATTRIBUTE;
		if (inScriptType == eNonHtmlScript) {
			// see https://html.spec.whatwg.org/multipage/scripting.html
			if (StrEqualsAny(s, "type", "language")) {
				isLanguageType = true;
			}
		}
	}
	if ((chAttr == SCE_H_ATTRIBUTEUNKNOWN) && !keywords)
		// No keywords -> all are known
		chAttr = SCE_H_ATTRIBUTE;
	styler.ColorTo(end, chAttr);
	return isLanguageType;
}

// https://html.spec.whatwg.org/multipage/custom-elements.html#custom-elements-core-concepts
bool isHTMLCustomElement(const char *tag, size_t length, bool dashColon) noexcept {
	// check valid HTML custom element name: starts with an ASCII lower alpha and contains hyphen.
	// IsAlpha() is used for `html.tags.case.sensitive=1`.
	if (length < 2 || !IsAlpha(tag[0])) {
		return false;
	}
	return dashColon;
}

int classifyTagHTML(Sci_PositionU start, Sci_PositionU end, const WordList &keywords, LexAccessor &styler, bool &tagDontFold,
	bool isXml, bool allowScripts) {
	char withSpace[126 + 2] = " ";
	const char *tag = withSpace + 1;
	// Copy after the '<' and stop before space
	Sci_PositionU i = 1;
	bool dashColon = false;
	for (Sci_PositionU cPos = start; cPos < end && i < sizeof(withSpace) - 2; cPos++) {
		const char ch = styler[cPos];
		if (static_cast<unsigned char>(ch) <= ' ') {
			break;
		}
		if ((ch != '<') && (ch != '/')) {
			withSpace[i++] = isXml ? ch : MakeLowerCase(ch);
			if (ch == ':' || ch == '-') {
				dashColon = true;
			}
		}
	}

	//The following is only a quick hack, to see if this whole thing would work
	//we first need the tagname with a trailing space...
	withSpace[i] = ' ';
	withSpace[i + 1] = '\0';

	// if the current language is XML, I can fold any tag
	// if the current language is HTML, I don't want to fold certain tags (input, meta, etc.)
	//...to find it in the list of no-container-tags
	tagDontFold = (!isXml) && (nullptr != strstr(htmlVoidTagList, withSpace));

	//now we can remove the trailing space
	withSpace[i] = '\0';

	// No keywords -> all are known
	char chAttr = SCE_H_TAGUNKNOWN;
	bool customElement = false;
	if (tag[0] == '!') {
		chAttr = SCE_H_SGML_DEFAULT;
	} else if (!keywords || keywords.InList(tag)) {
		chAttr = SCE_H_TAG;
	} else if (!isXml && isHTMLCustomElement(tag, i - 1, dashColon)) {
		customElement = true;
		chAttr = SCE_H_TAG;
	}
	if (chAttr != SCE_H_TAGUNKNOWN) {
		styler.ColorTo(end, chAttr);
	}
	if (chAttr == SCE_H_TAG && !customElement) {
		if (allowScripts && StrEqual(tag, "script")) {
			// check to see if this is a self-closing tag by sniffing ahead
			bool isSelfClose = false;
			for (Sci_PositionU cPos = end - 1; cPos < end + 200; cPos++) {
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
		} else if (!isXml && StrEqual(tag, "comment")) {
			// IE only comment tag
			chAttr = SCE_H_COMMENT;
		}
	}
	return chAttr;
}

void classifyWordHTJS(Sci_PositionU start, Sci_PositionU end, const WordList &keywords, LexAccessor &styler, script_mode inScriptType) {
	char s[127 + 1];
	styler.GetRange(start, end, s, sizeof(s));
	char chAttr = SCE_HJ_WORD;
	if (keywords.InList(s)) {
		chAttr = SCE_HJ_KEYWORD;
	}
	styler.ColorTo(end, statePrintForState(chAttr, inScriptType));
}

int classifyWordHTVB(Sci_PositionU start, Sci_PositionU end, const WordList &keywords, LexAccessor &styler, script_mode inScriptType) {
	char chAttr = SCE_HB_IDENTIFIER;
	char s[128];
	GetTextSegment(styler, start, end, s, sizeof(s));
	if (keywords.InList(s)) {
		chAttr = SCE_HB_WORD;
		if (StrEqual(s, "rem"))
			chAttr = SCE_HB_COMMENTLINE;
	}
	styler.ColorTo(end, statePrintForState(chAttr, inScriptType));
	if (chAttr == SCE_HB_COMMENTLINE)
		return SCE_HB_COMMENTLINE;
	else
		return SCE_HB_DEFAULT;
}

bool isWordHSGML(Sci_PositionU start, Sci_PositionU end, const WordList &keywords, LexAccessor &styler) noexcept {
	char s[63 + 1];
	styler.GetRange(start, end + 1, s, sizeof(s));
	return keywords.InList(s);
}

inline bool isWordCdata(Sci_PositionU start, LexAccessor &styler) noexcept {
	return styler.Match(start, "[CDATA[");
}

// Return the first state to reach when entering a scripting language
constexpr int StateForScript(script_type scriptLanguage) noexcept {
	switch (scriptLanguage) {
	case eScriptVBS:
		return SCE_HB_START;
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

constexpr bool issgmlwordchar(int ch) noexcept {
	return ch >= 0x80 ||
		(IsAlphaNumeric(ch) || ch == '.' || ch == '_' || ch == ':' || ch == '!' || ch == '#' || ch == '[');
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

constexpr bool IsHTMLWordChar(int ch) noexcept {
	return IsAlphaNumeric(ch) || AnyOf(ch, '.', '-', '_', ':', '!', '#') || ch >= 0x80;
}

constexpr bool IsTagContinue(int ch) noexcept {
	return IsAlphaNumeric(ch) || AnyOf(ch, '.', '-', '_', ':', '!', '#', '[', ']') || ch >= 0x80;
}

constexpr bool IsAttributeContinue(int ch) noexcept {
	return IsAlphaNumeric(ch) || AnyOf(ch, '.', '-', '_', ':', '!', '#', '/') || ch >= 0x80;
}

constexpr bool IsOKBeforeJSRE(int ch) noexcept {
	// TODO: also handle + and - (except if they're part of ++ or --) and return keywords
	return AnyOf(ch, '(', '[', '{', '=', ',', ':', ';', '!', '%', '^', '&', '*', '|', '?', '~', '>');
}

void ColouriseHyperTextDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler, bool isXml) {
	const WordList &keywordsTag = keywordLists[0];
	const WordList &keywordsJS = keywordLists[1];
	const WordList &keywordsVBS = keywordLists[2];
	const WordList &keywordsSGML = keywordLists[5]; // SGML (DTD)
	const WordList &keywordsAttr = keywordLists[6];
	const WordList &keywordsEvent = keywordLists[7];

	styler.StartAt(startPos);
	int StateToPrint = initStyle;
	int state = stateForPrintState(StateToPrint);

	// If inside a tag, it may be a script tag, so reread from the start of line starting tag to ensure any language tags are seen
	if (InTagState(state)) {
		while ((startPos > 0) && (InTagState(styler.StyleAt(startPos - 1)))) {
			const Sci_Position backLineStart = styler.LineStart(styler.GetLine(startPos-1));
			length += startPos - backLineStart;
			startPos = backLineStart;
		}
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

	Sci_Line lineCurrent = styler.GetLine(startPos);
	int lineState;
	if (lineCurrent > 0) {
		lineState = styler.GetLineState(lineCurrent-1);
	} else {
		// Default client and ASP scripting language is JavaScript
		lineState = eScriptJS << 8;

		// property asp.default.language
		//	Script in ASP code is initially assumed to be in JavaScript.
		//	To change this to VBScript set asp.default.language to 2.
		lineState |= styler.GetPropertyInt("asp.default.language", eScriptJS) << 4;
	}
	script_mode inScriptType = static_cast<script_mode>((lineState >> 0) & 0x03); // 2 bits of scripting mode
	bool tagOpened = (lineState >> 2) & 0x01; // 1 bit to know if we are in an opened tag
	bool tagClosing = (lineState >> 3) & 0x01; // 1 bit to know if we are in a closing tag
	bool tagDontFold = false; //some HTML tags should not be folded
	script_type aspScript = static_cast<script_type>((lineState >> 4) & 0x0F); // 4 bits of script name
	script_type clientScript = static_cast<script_type>((lineState >> 8) & 0x0F); // 4 bits of script name
	int beforePreProc = (lineState >> 12) & 0xFF; // 8 bits of state
	bool isLanguageType = (lineState >> 20) & 1; // type or language attribute for script tag

	script_type scriptLanguage = ScriptOfState(state);
	// If eNonHtmlScript coincides with SCE_H_COMMENT, assume eScriptComment
	if (inScriptType == eNonHtmlScript && state == SCE_H_COMMENT) {
		scriptLanguage = eScriptComment;
	}
	script_type beforeLanguage = ScriptOfState(beforePreProc);

	// property fold.html
	//	Folding is turned on or off for HTML and XML files with this option.
	//	The fold option must also be on for folding to occur.
	constexpr bool foldHTML = true;//styler.GetPropertyBool("fold.html", true);

	const bool fold = foldHTML & styler.GetPropertyInt("fold");

	// property fold.html.preprocessor
	//	Folding is turned on or off for scripts embedded in HTML files with this option.
	//	The default is on.
	constexpr bool foldHTMLPreprocessor = foldHTML;// && styler.GetPropertyBool("fold.html.preprocessor", true);

	// property fold.xml.at.tag.open
	//	Enable folding for XML at the start of open tag.
	//	The default is on.
	const bool foldXmlAtTagOpen = isXml & fold;// && styler.GetPropertyBool("fold.xml.at.tag.open", true);

	// property lexer.xml.allow.scripts
	//	Set to 0 to disable scripts in XML.
	const bool allowScripts = styler.GetPropertyBool("lexer.xml.allow.scripts", true);

	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;

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
			chPrevNonWhite = styler.SafeGetUCharAt(back);
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
		int chNext = styler.SafeGetUCharAt(i + 1);
		const int chNext2 = styler.SafeGetUCharAt(i + 2);

		// Handle DBCS codepages
		if (styler.IsLeadByte(static_cast<unsigned char>(ch))) {
			chPrev = ' ';
			i += 1;
			continue;
		}

		// decide what is the current state to print (depending of the script tag)
		StateToPrint = statePrintForState(state, inScriptType);

		// handle script folding
		if (fold) {
			switch (scriptLanguage) {
			case eScriptJS:
				//not currently supported				case eScriptVBS:

				if (state == SCE_HJ_COMMENT || state == SCE_HJ_COMMENTDOC) {
					if (ch == '*' && chNext == '/') {
						levelCurrent--;
					}
				} else if (!(state == SCE_HJ_COMMENTLINE || isStringState(state) || state == SCE_HJ_REGEX)) {
				//Platform::DebugPrintf("state=%d, StateToPrint=%d, initStyle=%d\n", state, StateToPrint, initStyle);
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
					} else if (ch == '{' || ch == '[' || ch == '(' || (ch == '/' && chNext == '*')) {
						levelCurrent++;
					} else if (ch == '}' || ch == ']' || ch == ')') {
						levelCurrent--;
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
				if ((levelCurrent > levelPrev))
					lev |= SC_FOLDLEVELHEADERFLAG;

				styler.SetLevel(lineCurrent, lev);
				levelPrev = levelCurrent;
			}
			styler.SetLineState(lineCurrent,
			                    ((inScriptType & 0x03) << 0) |
			                    ((tagOpened ? 1 : 0) << 2) |
			                    ((tagClosing ? 1 : 0) << 3) |
			                    ((aspScript & 0x0F) << 4) |
			                    ((clientScript & 0x0F) << 8) |
			                    ((beforePreProc & 0xFF) << 12) |
			                    ((isLanguageType ? 1 : 0) << 20));
			lineCurrent++;
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
			//case SCE_HB_COMMENTLINE:
			case SCE_HBA_COMMENTLINE:
			case SCE_HJ_DOUBLESTRING:
			case SCE_HJ_SINGLESTRING:
			case SCE_HJ_TEMPLATELITERAL:
			case SCE_HJ_REGEX:
			case SCE_HB_STRING:
			case SCE_HBA_STRING:
				break;
			default :
				// check if the closing tag is a script tag
				{
					const bool match = (state == SCE_HJ_COMMENTLINE || state == SCE_HB_COMMENTLINE || isXml) ? styler.MatchLowerCase(i + 2, "script")
						: ((state == SCE_H_COMMENT) ? styler.MatchLowerCase(i + 2, "comment") : true);
					if (!match) {
						break;
					}
				}
				// closing tag of the script (it's a closing HTML tag anyway)
				styler.ColorTo(i, StateToPrint);
				state = SCE_H_TAGUNKNOWN;
				inScriptType = eHtml;
				scriptLanguage = eScriptNone;
				clientScript = eScriptJS;
				isLanguageType = false;
				i += 2;
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
		         (ch == '<') &&
		         (chNext == '?') &&
				 !IsScriptCommentState(state)) {
 			beforeLanguage = scriptLanguage;
			scriptLanguage = segIsScriptingIndicator(styler, i + 2, i + 7, eScriptXML);
			if ((isStringState(state) || (state==SCE_H_COMMENT))) continue;
			styler.ColorTo(i, StateToPrint);
			beforePreProc = state;
			i++;
			if (scriptLanguage == eScriptXML)
				styler.ColorTo(i + 1, SCE_H_XMLSTART);
			else
				styler.ColorTo(i + 1, SCE_H_QUESTION);
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
			ch = styler.SafeGetUCharAt(i);
			continue;
		}

		// handle the start of ASP pre-processor = Non-HTML
		else if (!isCommentASPState(state) && (ch == '<') && (chNext == '%')) {
			styler.ColorTo(i, StateToPrint);
			beforePreProc = state;
			if (inScriptType == eNonHtmlScript)
				inScriptType = eNonHtmlScriptPreProc;
			else
				inScriptType = eNonHtmlPreProc;
			// fold whole script
			if (foldHTMLPreprocessor) {
				levelCurrent++;
			}
			if (chNext2 == '@') {
				i += 2; // place as if it was the second next char treated
				state = SCE_H_ASPAT;
				scriptLanguage = eScriptVBS;
			} else if ((chNext2 == '-') && (styler.SafeGetCharAt(i + 3) == '-')) {
				styler.ColorTo(i + 4, SCE_H_ASP);
				state = SCE_H_XCCOMMENT;
				scriptLanguage = eScriptVBS;
				continue;
			} else {
				if (chNext2 == '=') {
					i += 2; // place as if it was the second next char treated
				} else {
					i++; // place as if it was the next char treated
				}

				state = StateForScript(aspScript);
				scriptLanguage = aspScript;
			}
			styler.ColorTo(i + 1, SCE_H_ASP);
			// should be better
			ch = styler.SafeGetUCharAt(i);
			continue;
		}

		/////////////////////////////////////
		// handle the start of SGML language (DTD)
		else if (((scriptLanguage == eScriptNone) || (scriptLanguage == eScriptXML)) &&
				 (chPrev == '<') &&
				 (ch == '!') &&
				 (StateToPrint != SCE_H_CDATA) &&
				 (!isStringState(StateToPrint)) &&
				 (!IsCommentState(StateToPrint)) &&
				 (!IsScriptCommentState(StateToPrint))) {
			beforePreProc = state;
			styler.ColorTo(i - 1, StateToPrint);
			if ((chNext == '-') && (chNext2 == '-')) {
				state = SCE_H_COMMENT; // wait for a pending command
				styler.ColorTo(i + 3, SCE_H_COMMENT);
				i += 2; // follow styling after the --
			} else if (isWordCdata(i + 1, styler)) {
				state = SCE_H_CDATA;
			} else {
				styler.ColorTo(i + 1, SCE_H_SGML_DEFAULT); // <! is default
				scriptLanguage = eScriptSGML;
				state = SCE_H_SGML_COMMAND; // wait for a pending command
			}
			// fold whole tag (-- when closing the tag)
			if constexpr (foldHTMLPreprocessor || state == SCE_H_COMMENT || state == SCE_H_CDATA)
				levelCurrent++;
			continue;
		}

		// handle the end of a pre-processor = Non-HTML
		else if ((((inScriptType == eNonHtmlPreProc) || (inScriptType == eNonHtmlScriptPreProc)) &&
				  ((scriptLanguage != eScriptNone) && (chNext == '>') && stateAllowsTermination(state, ch))) ||
		         ((scriptLanguage == eScriptSGML) && (ch == '>') && (state != SCE_H_SGML_COMMENT))) {
			if (state == SCE_H_ASPAT) {
				aspScript = segIsScriptingIndicator(styler, styler.GetStartSegment(), i, aspScript);
			}
			// Bounce out of any ASP mode
			switch (state) {
			case SCE_HJ_WORD:
				classifyWordHTJS(styler.GetStartSegment(), i, keywordsJS, styler, inScriptType);
				break;
			case SCE_HB_WORD:
				classifyWordHTVB(styler.GetStartSegment(), i, keywordsVBS, styler, inScriptType);
				break;
			case SCE_H_XCCOMMENT:
				styler.ColorTo(i, state);
				break;
			default :
				styler.ColorTo(i, StateToPrint);
				break;
			}
			if (scriptLanguage != eScriptSGML) {
				i++;
			}
			if (ch == '%')
				styler.ColorTo(i + 1, SCE_H_ASP);
			else if (scriptLanguage == eScriptXML)
				styler.ColorTo(i + 1, SCE_H_XMLEND);
			else if (scriptLanguage == eScriptSGML)
				styler.ColorTo(i + 1, SCE_H_SGML_DEFAULT);
			else
				styler.ColorTo(i + 1, SCE_H_QUESTION);
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
				styler.ColorTo(i, StateToPrint);
				if (chNext != '!')
					state = SCE_H_TAGUNKNOWN;
			} else if (ch == '&' && (IsAlpha(chNext) || chNext == '#')) {
				styler.ColorTo(i, SCE_H_DEFAULT);
				state = SCE_H_ENTITY;
			}
			break;
		case SCE_H_SGML_DEFAULT:
		case SCE_H_SGML_BLOCK_DEFAULT:
//			if (scriptLanguage == eScriptSGMLblock)
//				StateToPrint = SCE_H_SGML_BLOCK_DEFAULT;

			if (ch == '\"') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_H_SGML_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_H_SGML_SIMPLESTRING;
			} else if ((ch == '-') && (chPrev == '-')) {
				if (static_cast<Sci_Position>(styler.GetStartSegment()) <= (i - 2)) {
					styler.ColorTo(i - 1, StateToPrint);
				}
				state = SCE_H_SGML_COMMENT;
			} else if (IsAlpha(ch) && (chPrev == '%')) {
				styler.ColorTo(i - 1, StateToPrint);
				state = SCE_H_SGML_ENTITY;
			} else if (ch == '#') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_H_SGML_SPECIAL;
			} else if (ch == '[') {
				styler.ColorTo(i, StateToPrint);
				scriptLanguage = eScriptSGMLblock;
				state = SCE_H_SGML_BLOCK_DEFAULT;
			} else if (ch == ']') {
				if (scriptLanguage == eScriptSGMLblock) {
					styler.ColorTo(i + 1, StateToPrint);
					scriptLanguage = eScriptSGML;
				} else {
					styler.ColorTo(i, StateToPrint);
					styler.ColorTo(i + 1, SCE_H_SGML_ERROR);
				}
				state = SCE_H_SGML_DEFAULT;
			} else if (scriptLanguage == eScriptSGMLblock) {
				if ((ch == '!') && (chPrev == '<')) {
					styler.ColorTo(i - 1, StateToPrint);
					styler.ColorTo(i + 1, SCE_H_SGML_DEFAULT);
					state = SCE_H_SGML_COMMAND;
				} else if (ch == '>') {
					styler.ColorTo(i, StateToPrint);
					styler.ColorTo(i + 1, SCE_H_SGML_DEFAULT);
				}
			}
			break;
		case SCE_H_SGML_COMMAND:
			if ((ch == '-') && (chPrev == '-')) {
				styler.ColorTo(i - 1, StateToPrint);
				state = SCE_H_SGML_COMMENT;
			} else if (!issgmlwordchar(ch)) {
				if (isWordHSGML(styler.GetStartSegment(), i - 1, keywordsSGML, styler)) {
					styler.ColorTo(i, StateToPrint);
					state = SCE_H_SGML_1ST_PARAM;
				} else {
					state = SCE_H_SGML_ERROR;
				}
			}
			break;
		case SCE_H_SGML_1ST_PARAM:
			// wait for the beginning of the word
			if ((ch == '-') && (chPrev == '-')) {
				styler.ColorTo(i - 1, defaultStateForSGML(scriptLanguage));
				state = SCE_H_SGML_1ST_PARAM_COMMENT;
			} else if (issgmlwordchar(ch)) {
				styler.ColorTo(i, defaultStateForSGML(scriptLanguage));
				// find the length of the word
				int size = 1;
				while (IsHTMLWordChar(styler.SafeGetUCharAt(i + size)))
					size++;
				styler.ColorTo(i + size, StateToPrint);
				i += size - 1;
				ch = styler.SafeGetUCharAt(i);
				state = defaultStateForSGML(scriptLanguage);
				continue;
			} else if (ch == '\"' || ch == '\'') {
				styler.ColorTo(i, defaultStateForSGML(scriptLanguage));
				state = (ch == '\"')? SCE_H_SGML_DOUBLESTRING : SCE_H_SGML_SIMPLESTRING;
			}
			break;
		case SCE_H_SGML_ERROR:
			if ((ch == '-') && (chPrev == '-')) {
				styler.ColorTo(i - 1, StateToPrint);
				state = SCE_H_SGML_COMMENT;
			} else if (ch == '\"' || ch == '\'') {
				styler.ColorTo(i, defaultStateForSGML(scriptLanguage));
				state = (ch == '\"')? SCE_H_SGML_DOUBLESTRING : SCE_H_SGML_SIMPLESTRING;
			}
			break;
		case SCE_H_SGML_DOUBLESTRING:
		case SCE_H_SGML_SIMPLESTRING:
			if (ch == ((state == SCE_H_SGML_DOUBLESTRING) ? '\"' : '\'')) {
				styler.ColorTo(i + 1, StateToPrint);
				state = SCE_H_SGML_DEFAULT;
			}
			break;
		case SCE_H_SGML_COMMENT:
			if ((ch == '-') && (chPrev == '-')) {
				styler.ColorTo(i + 1, StateToPrint);
				state = SCE_H_SGML_DEFAULT;
			}
			break;
		case SCE_H_CDATA:
			if ((chPrev2 == ']') && (chPrev == ']') && (ch == '>')) {
				styler.ColorTo(i + 1, StateToPrint);
				state = SCE_H_DEFAULT;
				levelCurrent--;
			}
			break;
		case SCE_H_COMMENT:
			if ((scriptLanguage != eScriptComment) && (chPrev2 == '-') && (chPrev == '-') && (ch == '>')) {
				styler.ColorTo(i + 1, StateToPrint);
				state = SCE_H_DEFAULT;
				levelCurrent--;
			}
			break;
		case SCE_H_SGML_1ST_PARAM_COMMENT:
			if ((ch == '-') && (chPrev == '-')) {
				styler.ColorTo(i + 1, SCE_H_SGML_COMMENT);
				state = SCE_H_SGML_1ST_PARAM;
			}
			break;
		case SCE_H_SGML_SPECIAL:
			if (!IsUpperCase(ch)) {
				styler.ColorTo(i, StateToPrint);
				if (IsAlphaNumeric(ch)) {
					state = SCE_H_SGML_ERROR;
				} else {
					state = SCE_H_SGML_DEFAULT;
				}
			}
			break;
		case SCE_H_SGML_ENTITY:
			if (!(IsAlphaNumeric(ch) || ch == '-' || ch == '.')) {
				styler.ColorTo(i + 1, ((ch == ';') ? StateToPrint : SCE_H_SGML_ERROR));
				state = SCE_H_SGML_DEFAULT;
			}
			break;
		case SCE_H_ENTITY:
			if (!IsEntityChar(ch)) {
				styler.ColorTo(i + (ch == ';'), ((ch == ';') ? StateToPrint : SCE_H_TAGUNKNOWN));
				state = SCE_H_DEFAULT;
				if (ch != ';' && IsAGraphic(ch)) {
					// Retreat one byte so the character that is invalid inside entity
					// may start something else like a tag.
					--i;
					continue;
				}
			}
			break;
		case SCE_H_TAGUNKNOWN:
			if (!IsTagContinue(ch) && !((ch == '/') && (chPrev == '<'))) {
				int eClass = classifyTagHTML(styler.GetStartSegment(),
					i, keywordsTag, styler, tagDontFold, isXml, allowScripts);
				if (eClass == SCE_H_SCRIPT || eClass == SCE_H_COMMENT) {
					if (!tagClosing) {
						inScriptType = eNonHtmlScript;
						scriptLanguage = eClass == SCE_H_SCRIPT ? clientScript : eScriptComment;
					} else {
						scriptLanguage = eScriptNone;
					}
					isLanguageType = false;
					eClass = SCE_H_TAG;
				}
				if (ch == '>') {
					styler.ColorTo(i + 1, eClass);
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
						styler.ColorTo(i + 2, SCE_H_TAGUNKNOWN);
					} else {
						styler.ColorTo(i, StateToPrint);
						styler.ColorTo(i + 2, SCE_H_TAG); // SCE_H_TAGEND
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
						styler.ColorTo(i, eClass);
						state = SCE_H_OTHER;
					}
				}
			}
			break;
		case SCE_H_ATTRIBUTE:
			if (!IsAttributeContinue(ch)) {
				isLanguageType = classifyAttribHTML(inScriptType, styler.GetStartSegment(), i, keywordsAttr, keywordsEvent, styler);
				if (ch == '>') {
					styler.ColorTo(i + 1, SCE_H_TAG);
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
					styler.ColorTo(i + 1, SCE_H_OTHER);
					state = SCE_H_VALUE;
				} else {
					state = SCE_H_OTHER;
				}
			}
			break;
		case SCE_H_OTHER:
			if (ch == '>') {
				styler.ColorTo(i, StateToPrint);
				styler.ColorTo(i + 1, SCE_H_TAG);
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
				styler.ColorTo(i, StateToPrint);
				state = SCE_H_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_H_SINGLESTRING;
			} else if (ch == '=') {
				styler.ColorTo(i + 1, StateToPrint);
				state = SCE_H_VALUE;
			} else if (ch == '/' && chNext == '>') {
				styler.ColorTo(i, StateToPrint);
				styler.ColorTo(i + 2, SCE_H_TAG); // SCE_H_TAGEND
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
				tagOpened = false;
				if (foldXmlAtTagOpen) {
					levelCurrent--;
				}
			} else if (ch == '?' && chNext == '>') {
				styler.ColorTo(i, StateToPrint);
				styler.ColorTo(i + 2, SCE_H_XMLSTART); // SCE_H_XMLEND
				i++;
				ch = chNext;
				state = SCE_H_DEFAULT;
			} else if (IsHTMLWordChar(ch)) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_H_ATTRIBUTE;
			}
			break;
		case SCE_H_DOUBLESTRING:
		case SCE_H_SINGLESTRING:
			if (ch == ((state == SCE_H_DOUBLESTRING) ? '\"' : '\'')) {
				if (isLanguageType) {
					scriptLanguage = segIsScriptingIndicator(styler, styler.GetStartSegment(), i + 1, scriptLanguage);
					clientScript = scriptLanguage;
					isLanguageType = false;
				}
				styler.ColorTo(i + 1, state);
				state = SCE_H_OTHER;
			}
			break;
		case SCE_H_VALUE:
			if (IsHtmlInvalidAttrChar(ch)) {
				if (ch == '\"' && chPrev == '=') {
					// Should really test for being first character
					state = SCE_H_DOUBLESTRING;
				} else if (ch == '\'' && chPrev == '=') {
					state = SCE_H_SINGLESTRING;
				} else {
					if (IsNumberChar(styler[styler.GetStartSegment()])) {
						styler.ColorTo(i, SCE_H_NUMBER);
					} else {
						styler.ColorTo(i, StateToPrint);
					}
					if (ch == '>') {
						styler.ColorTo(i + 1, SCE_H_TAG);
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
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_NUMBER;
			} else if (IsIdentifierStartEx(ch)) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_WORD;
			} else if (ch == '/' && chNext == '*') {
				styler.ColorTo(i, StateToPrint);
				if (chNext2 == '*')
					state = SCE_HJ_COMMENTDOC;
				else
					state = SCE_HJ_COMMENT;
				if (chNext2 == '/') {
					// Eat the * so it isn't used for the end of the comment
					i++;
				}
			} else if (ch == '/' && chNext == '/') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
			} else if (ch == '/' && IsOKBeforeJSRE(chPrevNonWhite)) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_REGEX;
			} else if (ch == '\"') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_DOUBLESTRING;
			} else if (ch == '\'') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_SINGLESTRING;
			} else if (ch == '`') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_TEMPLATELITERAL;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
			           styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
			} else if ((ch == '-') && (chNext == '-') && (chNext2 == '>')) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_COMMENTLINE;
				i += 2;
			} else if (isoperator(ch)) {
				styler.ColorTo(i, StateToPrint);
				styler.ColorTo(i + 1, statePrintForState(SCE_HJ_SYMBOLS, inScriptType));
				state = SCE_HJ_DEFAULT;
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HJ_START) {
					styler.ColorTo(i, StateToPrint);
					state = SCE_HJ_DEFAULT;
				}
			}
			break;
		case SCE_HJ_WORD:
		case SCE_HJ_NUMBER:
			if (!(IsIdentifierCharEx(ch) || (state == SCE_HJ_NUMBER && IsNumberContinue(chPrev, ch, chNext)))) {
				if (state == SCE_HJ_NUMBER) {
					styler.ColorTo(i, statePrintForState(SCE_HJ_NUMBER, inScriptType));
				} else {
					classifyWordHTJS(styler.GetStartSegment(), i, keywordsJS, styler, inScriptType);
				}
				//styler.ColorTo(i, eHTJSKeyword);
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
				} else if (ch == '`') {
					state = SCE_HJ_TEMPLATELITERAL;
				} else if ((ch == '-') && (chNext == '-') && (chNext2 == '>')) {
					styler.ColorTo(i, StateToPrint);
					state = SCE_HJ_COMMENTLINE;
					i += 2;
				} else if (isoperator(ch)) {
					styler.ColorTo(i + 1, statePrintForState(SCE_HJ_SYMBOLS, inScriptType));
					state = (ch == '.')? SCE_HJ_WORD : SCE_HJ_DEFAULT;
				}
			}
			break;
		case SCE_HJ_COMMENT:
		case SCE_HJ_COMMENTDOC:
			if (ch == '/' && chPrev == '*') {
				styler.ColorTo(i + 1, StateToPrint);
				state = SCE_HJ_DEFAULT;
				ch = ' ';
			}
			break;
		case SCE_HJ_COMMENTLINE:
			if (IsEOLChar(ch)) {
				styler.ColorTo(i, statePrintForState(SCE_HJ_COMMENTLINE, inScriptType));
				state = SCE_HJ_DEFAULT;
				ch = ' ';
			}
			break;
		case SCE_HJ_DOUBLESTRING:
		case SCE_HJ_SINGLESTRING:
		case SCE_HJ_TEMPLATELITERAL:
			if (ch == '\\') {
				if (IsAGraphic(chNext)) {
					i++;
				}
			} else if (ch == ((state == SCE_HJ_DOUBLESTRING) ? '\"' : ((state == SCE_HJ_SINGLESTRING) ? '\'' : '`'))) {
				styler.ColorTo(i + 1, statePrintForState(state, inScriptType));
				state = SCE_HJ_DEFAULT;
			} else if (state != SCE_HJ_TEMPLATELITERAL && IsEOLChar(ch)) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_STRINGEOL;
			}
			break;
		case SCE_HJ_STRINGEOL:
			if (!IsEOLChar(ch)) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HJ_DEFAULT;
			} else if (!IsEOLChar(chNext)) {
				styler.ColorTo(i + 1, StateToPrint);
				state = SCE_HJ_DEFAULT;
			}
			break;
		case SCE_HJ_REGEX:
			if (ch == '\r' || ch == '\n' || ch == '/') {
				if (ch == '/') {
					while (IsLowerCase(chNext)) {   // gobble regex flags
						i++;
						ch = chNext;
						chNext = styler.SafeGetUCharAt(i + 1);
					}
				}
				styler.ColorTo(i + 1, StateToPrint);
				state = SCE_HJ_DEFAULT;
			} else if (ch == '\\') {
				// Gobble up the quoted character
				if (chNext == '\\' || chNext == '/') {
					i++;
					ch = chNext;
					chNext = styler.SafeGetUCharAt(i + 1);
				}
			}
			break;

		case SCE_HB_DEFAULT:
		case SCE_HB_START:
		case SCE_HB_OPERATOR:
			if (IsNumberStart(ch, chNext)) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HB_NUMBER;
			} else if (IsIdentifierStartEx(ch)) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HB_WORD;
			} else if (ch == '\'') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HB_COMMENTLINE;
			} else if (ch == '\"') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HB_STRING;
			} else if ((ch == '<') && (chNext == '!') && (chNext2 == '-') &&
			           styler.SafeGetCharAt(i + 3) == '-') {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HB_COMMENTLINE;
			} else if (isoperator(ch)) {
				styler.ColorTo(i, StateToPrint);
				styler.ColorTo(i + 1, statePrintForState(SCE_HB_OPERATOR, inScriptType));
				state = SCE_HB_DEFAULT;
			} else if ((ch == ' ') || (ch == '\t')) {
				if (state == SCE_HB_START) {
					styler.ColorTo(i, StateToPrint);
					state = SCE_HB_DEFAULT;
				}
			}
			break;
		case SCE_HB_WORD:
		case SCE_HB_NUMBER:
			if (!(IsIdentifierCharEx(ch) || (state == SCE_HB_NUMBER && IsNumberContinue(chPrev, ch, chNext)))) {
				if (state == SCE_HB_NUMBER) {
					styler.ColorTo(i, statePrintForState(SCE_HB_NUMBER, inScriptType));
					state = SCE_HB_DEFAULT;
				} else {
					state = classifyWordHTVB(styler.GetStartSegment(), i, keywordsVBS, styler, inScriptType);
				}
				if (state == SCE_HB_DEFAULT) {
					if (ch == '\"') {
						state = SCE_HB_STRING;
					} else if (ch == '\'') {
						state = SCE_HB_COMMENTLINE;
					} else if (isoperator(ch)) {
						styler.ColorTo(i + 1, statePrintForState(SCE_HB_OPERATOR, inScriptType));
						state = (ch == '.')? SCE_HB_WORD : SCE_HB_DEFAULT;
					}
				}
			}
			break;
		case SCE_HB_STRING:
			if (ch == '\"') {
				styler.ColorTo(i + 1, StateToPrint);
				state = SCE_HB_DEFAULT;
			} else if (IsEOLChar(ch)) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HB_STRINGEOL;
			}
			break;
		case SCE_HB_COMMENTLINE:
			if (IsEOLChar(ch)) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HB_DEFAULT;
			}
			break;
		case SCE_HB_STRINGEOL:
			if (!IsEOLChar(ch)) {
				styler.ColorTo(i, StateToPrint);
				state = SCE_HB_DEFAULT;
			} else if (!IsEOLChar(chNext)) {
				styler.ColorTo(i + 1, StateToPrint);
				state = SCE_HB_DEFAULT;
			}
			break;
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
				styler.ColorTo(i + 1, SCE_HB_OPERATOR);
			}
		} else if (state == SCE_HBA_DEFAULT) {    // One of the above succeeded
			if ((ch == '\"') && (nonEmptySegment)) {
				state = SCE_HBA_STRING;
			} else if (ch == '\'') {
				state = SCE_HBA_COMMENTLINE;
			} else if (iswordstart(ch)) {
				state = SCE_HBA_WORD;
			} else if (isoperator(ch)) {
				styler.ColorTo(i + 1, SCE_HBA_OPERATOR);
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
			} else if ((ch == '`') && (nonEmptySegment)) {
				state = SCE_HJ_TEMPLATELITERAL;
			} else if (iswordstart(ch)) {
				state = SCE_HJ_WORD;
			} else if (isoperator(ch)) {
				styler.ColorTo(i + 1, statePrintForState(SCE_HJ_SYMBOLS, inScriptType));
			}
		}
	}

	switch (state) {
	case SCE_HJ_WORD:
		classifyWordHTJS(styler.GetStartSegment(), lengthDoc, keywordsJS, styler, inScriptType);
		break;
	case SCE_HB_WORD:
		classifyWordHTVB(styler.GetStartSegment(), lengthDoc, keywordsVBS, styler, inScriptType);
		break;
	default:
		StateToPrint = statePrintForState(state, inScriptType);
		if (static_cast<Sci_Position>(styler.GetStartSegment()) < lengthDoc)
			styler.ColorTo(lengthDoc, StateToPrint);
		break;
	}

	// Fill in the real level of the next line, keeping the current flags as they will be filled in later
	if (fold) {
		const int flagsNext = styler.LevelAt(lineCurrent) & ~SC_FOLDLEVELNUMBERMASK;
		styler.SetLevel(lineCurrent, levelPrev | flagsNext);
	}
}

void ColouriseXMLDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	// Passing in true because we're lexing XML
	ColouriseHyperTextDoc(startPos, length, initStyle, keywordLists, styler, true);
}

void ColouriseHTMLDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	// Passing in false because we're not lexing XML
	ColouriseHyperTextDoc(startPos, length, initStyle, keywordLists, styler, false);
}

}

LexerModule lmHTML(SCLEX_HTML, ColouriseHTMLDoc, "hypertext");
LexerModule lmXML(SCLEX_XML, ColouriseXMLDoc, "xml");
