// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Markdown

#include <cassert>
#include <cstring>

#include <string>
#include <string_view>
#include <vector>

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
#include "LexerUtils.h"

using namespace Scintilla;
using namespace Lexilla;

namespace {

// CommonMark Spec https://spec.commonmark.org/
// https://spec.commonmark.org/dingus/
// GitHub Flavored Markdown Spec https://github.github.com/gfm/
// https://docs.github.com/en/get-started/writing-on-github
// GitLab Flavored Markdown https://docs.gitlab.com/ee/user/markdown.html
// https://gitlab.com/gitlab-org/gitlab/blob/master/doc/user/markdown.md
// Pandoc’s Markdown https://pandoc.org/MANUAL.html#pandocs-markdown
// MultiMarkdown https://github.com/fletcher/MultiMarkdown-6
// https://fletcher.github.io/MultiMarkdown-6/
// PHP Markdown Extra https://michelf.ca/projects/php-markdown/extra/

enum class Markdown {
	GitHub,
	GitLab,
	Pandoc,
};

enum class HtmlTagState {
	None,
	Question,
	Open,
	Value,
};

enum class AutoLink {
	None,
	Angle,
	Scheme,
	Domain,
	Path,
};

enum {
	LineStateEmptyLine = 1 << 2,
	LineStateNestedStateLine = 1 << 3,
	LineStateListItemFirstLine = 1 << 4,
	LineStateBlockEndLine = 1 << 5,
	LineStateSetextFirstLine = 1 << 6,
};

constexpr bool IsHeaderStyle(int state) noexcept {
	return state >= SCE_MARKDOWN_HEADER1 && state <= SCE_MARKDOWN_SETEXT_H2;
}

constexpr bool IsInlineStyle(int state) noexcept {
	return state >= SCE_MARKDOWN_ESCAPECHAR || state == SCE_H_TAG || state == SCE_H_ENTITY;
}

constexpr bool StyleNeedsBacktrack(int state) noexcept {
	return state == SCE_MARKDOWN_SETEXT_H1 || SCE_MARKDOWN_SETEXT_H2
		|| state == SCE_MARKDOWN_CODE_SPAN;
}

constexpr bool IsMarkdownSpace(int ch) noexcept {
	return IsSpaceOrTab(ch) || IsEOLChar(ch);
}

constexpr bool IsBlockStartChar(int ch) noexcept {
	return ch == '>' // block quote
		|| ch == '#' // ATX header
		|| ch == '<' // html block
		|| ch == '*' // thematic break, bullet list
		|| ch == '-' // thematic break, bullet list, setext header
		|| ch == '_' // thematic break
		|| ch == '=' // setext header
		|| ch == '+' // bullet list
		|| ch == '`' // fenced code block
		|| ch == '~' // fenced code block
		|| ch == '\t'// indented code block or list item, paragraph continuation text
		|| ch == '[' // link reference definition
		|| IsADigit(ch);// ordered list
}

inline uint8_t GetCharAfterIndent(LexAccessor &styler, Sci_PositionU &startPos, int indentCount) noexcept {
	Sci_PositionU pos = startPos;
	uint8_t ch = styler.SafeGetCharAt(pos);
	while (ch == ' ' && indentCount != 0) {
		--indentCount;
		ch = styler.SafeGetCharAt(++pos);
	}
	startPos = pos;
	return ch;
}

inline uint8_t GetCharAfterDelimiter(LexAccessor &styler, Sci_PositionU &startPos, int delimiter) noexcept {
	Sci_PositionU pos = startPos;
	uint8_t ch;
	do {
		ch = styler.SafeGetCharAt(++pos);
	} while (ch == delimiter);
	startPos = pos;
	return ch;
}

// 5.2 List items
// https://html.spec.whatwg.org/multipage/grouping-content.html#the-ol-element
// https://pandoc.org/MANUAL.html#ordered-lists
enum class OrderedListType {
	None,
	Decimal,
	LowerRoman,
	UpperRoman,
	LowerAlpha,
	UpperAlpha,
	NumberSign, // Pandoc #.
	Parenthesis = 8,
};

constexpr bool IsUpperRoman(int ch) {
	return AnyOf(ch, 'I', 'V', 'X', 'L', 'C', 'D', 'M');
}

constexpr bool IsLowerRoman(int ch) {
	return AnyOf(ch, 'i', 'v', 'x', 'l', 'c', 'd', 'm');
}

// 6.2 Emphasis and strong emphasis
// 6.5 Strikethrough (extension), handled similar to `**` except no nesting
struct DelimiterRun {
	CharacterClass ccPrev = CharacterClass::space;
	CharacterClass ccNext = CharacterClass::space;

	constexpr bool IsLeftFlanking() const noexcept {
		// (1) not followed by Unicode whitespace
		if (ccNext >= CharacterClass::punctuation) {
			// (2b) followed by a Unicode punctuation character
			if (ccNext == CharacterClass::punctuation) {
				// and preceded by Unicode whitespace or a Unicode punctuation character
				return ccPrev <= CharacterClass::punctuation;
			}
			// (2a) not followed by a Unicode punctuation character,
			return true;
		}
		return false;
	}
	constexpr bool IsRightFlanking() const noexcept {
		// (1) not preceded by Unicode whitespace
		if (ccPrev >= CharacterClass::punctuation) {
			// (2b) preceded by a Unicode punctuation character
			if (ccPrev == CharacterClass::punctuation) {
				// and followed by Unicode whitespace or a Unicode punctuation character
				return ccNext <= CharacterClass::punctuation;
			}
			// (2a) not preceded by a Unicode punctuation character
			return true;
		}
		return false;
	}
	constexpr bool CanOpen(int delimiter) const noexcept {
		// is part of a left-flanking delimiter run
		if (IsLeftFlanking()) {
			if (delimiter != '_') {
				return true; // `*`, `**`, `~~`
			}
			// (a) not part of a right-flanking delimiter run or
			// (b) part of a right-flanking delimiter run preceded by a Unicode punctuation character.
			if (ccPrev == CharacterClass::punctuation || !IsRightFlanking()) {
				return true;
			}
		}
		return false;
	}
	constexpr bool CanClose(int delimiter) const noexcept {
		// is part of a right-flanking delimiter run
		if (IsRightFlanking()) {
			if (delimiter != '_') {
				return true; // `*`, `**`, `~~`
			}
			// (a) not part of a left-flanking delimiter run or
			// (b) part of a left-flanking delimiter run followed by a Unicode punctuation character.
			if (ccNext == CharacterClass::punctuation || !IsLeftFlanking()) {
				return true;
			}
		}
		return false;
	}
};

struct MarkdownLexer {
	StyleContext sc;
	std::vector<int> nestedState;
	std::vector<Sci_PositionU> backPos;

	HtmlTagState tagState = HtmlTagState::None; // html tag, link title
	int delimiterCount = 0; // code fence, autoLink
	int bracketCount = 0; // link text
	int parenCount = 0; // link	destination, link title, autoLink
	AutoLink autoLink = AutoLink::None;
	const Markdown markdown;

	MarkdownLexer(Sci_PositionU startPos, Sci_PositionU lengthDoc, int initStyle, Accessor &styler_) noexcept :
		sc(startPos, lengthDoc, initStyle, styler_),
		markdown{static_cast<Markdown>(styler_.GetPropertyInt("lexer.lang"))} {}

	void SaveOuterStyle(int style) {
		nestedState.push_back(style);
	}
	int TakeOuterStyle() {
		return TakeAndPop(nestedState);
	}
	void DropOuterStyle() {
		nestedState.pop_back();
	}

	void SaveOuterStart(Sci_PositionU startPos) {
		backPos.push_back(startPos);
	}
	Sci_PositionU TakeOuterStart() {
		return TakeAndPop(backPos);
	}
	void DropOuterStart() {
		backPos.pop_back();
	}

	bool IsParagraphEnd(Sci_PositionU startPos) const noexcept;
	int HighlightBlockText();
	void HighlightInlineText();
	void HighlightIndentedText();

	int GetCurrentDelimiterRun(DelimiterRun &delimiterRun) const noexcept;
	bool HighlightEmphasis();

	bool HighlightLinkText();
	bool HighlightLinkDestination();

	AutoLink RoughCheckAutoLink() const noexcept;
	bool HighlightAutoLink();

	void HighlightDelimiterRow();
	bool HighlightInlineDiff();
};

// 4.1 Thematic breaks
bool CheckThematicBreak(LexAccessor &styler, Sci_PositionU pos, int delimiter) noexcept {
	int count = 1;
	while (true) {
		const uint8_t ch = styler.SafeGetCharAt(++pos);
		if (ch == delimiter) {
			++count;
		} else if (!IsSpaceOrTab(ch)) {
			if (IsEOLChar(ch))  {
				break;
			}
			return false;
		}
	}
	return count >= 3;
}

// 4.2 ATX headings
int CheckATXHeading(LexAccessor &styler, Sci_PositionU pos) noexcept {
	int level = 1;
	while (true) {
		const uint8_t ch = styler.SafeGetCharAt(++pos);
		if (ch == '#') {
			++level;
		} else {
			if (level <= 6 && IsMarkdownSpace(ch)) {
				return level;
			}
			break;
		}
	}
	return 0;
}

// 4.3 Setext headings
int CheckSetextHeading(LexAccessor &styler, Sci_PositionU pos) noexcept {
	const uint8_t marker = GetCharAfterIndent(styler, pos, 3);
	if (marker == '=' || marker == '-') {
		uint8_t ch;
		do {
			ch = styler.SafeGetCharAt(++pos);
		} while (ch == marker);
		while (IsSpaceOrTab(ch)) {
			ch = styler.SafeGetCharAt(++pos);
		}
		if (IsEOLChar(ch)) {
			return (marker == '=') ? 1 : 2;
		}
	}
	return 0;
}

// 4.5 Fenced code blocks
int GetMatchedDelimiterCount(LexAccessor &styler, Sci_PositionU pos, int delimiter) noexcept {
	int count = 1;
	while (true) {
		const uint8_t ch = styler.SafeGetCharAt(++pos);
		if (ch == delimiter) {
			++count;
		} else {
			break;
		}
	}
	return count;
}

// 4.10 Tables (extension)
// https://pandoc.org/MANUAL.html#tables
void MarkdownLexer::HighlightDelimiterRow() {
	bool pipe = false;
	bool minus = false;
	bool plus = false;
	bool equal = false;
	Sci_PositionU pos = sc.currentPos;
	while (pos < sc.lineStartNext) {
		const uint8_t ch = sc.styler.SafeGetCharAt(pos++);
		switch (ch) {
		case '|':
			pipe = true;
			break;
		case ':':
			break;
		case '-':
			minus = true;
			break;
		case '+':
			plus = true;
			break;
		case '=':
			equal = true;
			break;
		default:
			if (!IsMarkdownSpace(ch)) {
				if (sc.ch == ':' && (sc.chNext == ':' || IsSpaceOrTab(sc.chNext))) {
					// Pandoc fenced divs
					// Pandoc, MultiMarkdown, PHP Markdown Extra definition list
					SaveOuterStyle(SCE_MARKDOWN_DEFAULT);
					sc.SetState(SCE_MARKDOWN_DELIMITER);
				}
				return;
			}
			break;
		}
	}

	if (markdown == Markdown::Pandoc) {
		// pipe table or grid table
		pipe = (pipe | plus) & (minus | equal);
	} else {
		pipe &= minus;
	}
	if (pipe) {
		int style = SCE_MARKDOWN_DELIMITER_ROW;
		if (sc.ch == '|' || sc.ch == '+') {
			SaveOuterStyle(style);
			style = SCE_MARKDOWN_DELIMITER;
		}
		sc.SetState(style);
	}
}

OrderedListType CheckOrderedList(LexAccessor &styler, Sci_PositionU pos, int current, int chNext) noexcept {
	if (current == '#') { // Pandoc
		if (chNext == '.') {
			const uint8_t after = styler.SafeGetCharAt(pos + 2);
			if (IsMarkdownSpace(after)) {
				return OrderedListType::NumberSign;
			}
		}
		return OrderedListType::None;
	}

	int marker = current;
	if (marker == '(') { // Pandoc
		marker = chNext;
		++pos;
		chNext = static_cast<uint8_t>(styler.SafeGetCharAt(pos + 1));
	}

	// only support (1) single uppercase or lowercase letter.
	// (2) less than 10 decimal numbers or roman numerals.
	OrderedListType type;
	int count = 1;

	if (IsADigit(marker)) {
		type = OrderedListType::Decimal;
		if (IsADigit(chNext)) {
			++count;
			++pos;
		}
	} else if (IsLowerCase(marker)) {
		type = OrderedListType::LowerAlpha;
		++pos;
		if (IsLowerRoman(marker) && IsLowerRoman(chNext)) {
			type = OrderedListType::LowerRoman;
			++count;
		}
	} else if (IsUpperCase(marker)) {
		type = OrderedListType::UpperAlpha;
		++pos;
		if (IsUpperRoman(marker) && IsUpperRoman(chNext)) {
			type = OrderedListType::UpperRoman;
			++count;
		}
	} else {
		return OrderedListType::None;
	}

	if (type < OrderedListType::LowerAlpha) {
		while (true) {
			chNext = static_cast<uint8_t>(styler.SafeGetCharAt(++pos));
			const bool handled = (type == OrderedListType::Decimal) ? IsADigit(chNext)
				: ((type == OrderedListType::LowerRoman) ? IsLowerRoman(chNext) : IsUpperRoman(chNext));
			if (handled) {
				++count;
				if (count == 10) {
					return OrderedListType::None;
				}
			} else {
				break;
			}
		}
	}

	if ((chNext == '.' && current != '(') || chNext == ')') {
		const uint8_t after = styler.SafeGetCharAt(++pos);
		if (IsMarkdownSpace(after)) {
			if (type == OrderedListType::UpperAlpha && chNext == '.' && count == 1) {
				// single uppercase letter ended with period requires at least two spaces.
				// see https://pandoc.org/MANUAL.html#fn1
				if (!(IsSpaceOrTab(after) && IsMarkdownSpace(styler.SafeGetCharAt(pos + 1)))) {
					return OrderedListType::None;
				}
			}
			if (current == '(') {
				type = static_cast<OrderedListType>(static_cast<int>(type) | (static_cast<int>(OrderedListType::Parenthesis)));
			}
			return type;
		}
	}
	return OrderedListType::None;
}

// 4.4 Indented code blocks
void MarkdownLexer::HighlightIndentedText() {
	// indented code block

	// paragraph continuation text

	// indented list item
	if (sc.ch == '+' || sc.ch == '-' || sc.ch == '*') {
		if (IsMarkdownSpace(sc.chNext)) {
			sc.SetState(SCE_MARKDOWN_BULLET_LIST);
		}
	} else {
		const OrderedListType listType = CheckOrderedList(sc.styler, sc.currentPos, sc.ch, sc.chNext);
		if (listType != OrderedListType::None) {
			const int style = (listType == OrderedListType::Decimal || markdown == Markdown::Pandoc) ? SCE_MARKDOWN_ORDERED_LIST : SCE_MARKDOWN_EXT_ORDERED_LIST;
			sc.SetState(style);
		}
	}
}

int MarkdownLexer::GetCurrentDelimiterRun(DelimiterRun &delimiterRun) const noexcept {
	int chPrev = sc.chPrev;
	Sci_PositionU pos = sc.currentPos;
	// unlike official Lexilla, for performance reason our StyleContext
	// for UTF-8 encoding is byte oriented instead of character oriented.
	if ((chPrev & 0x80) != 0 && sc.styler.Encoding() == EncodingType::unicode) {
		Sci_Position width = -1; // get full character before currentPos
		chPrev = sc.styler.GetCharacterAndWidth(pos - 1, &width);
	}

	int chNext = GetCharAfterDelimiter(sc.styler, pos, sc.ch);
	if (chNext & 0x80) {
		chNext = sc.styler.GetCharacterAndWidth(pos);
	}

	// not same as Unicode whitespace or punctuation character defined in "2.1 Characters and lines",
	// which can be implemented for UTF-8 encoding with CategoriseCharacter() from CharacterCategory.h
	delimiterRun.ccPrev = (chPrev == '_') ? CharacterClass::punctuation : sc.styler.GetCharacterClass(chPrev);
	delimiterRun.ccNext = (chNext == '_') ? CharacterClass::punctuation : sc.styler.GetCharacterClass(chNext);
	// returns length of the delimiter run
	return static_cast<int>(pos - sc.currentPos);
}

bool MarkdownLexer::HighlightEmphasis() {
	if (sc.ch == ']' && bracketCount != 0) {
		sc.ChangeState(TakeOuterStyle());
		sc.BackTo(TakeOuterStart());
		sc.Forward();
		return true;
	}

	const int delimiter = static_cast<uint8_t>((sc.state == SCE_MARKDOWN_STRIKEOUT) ? '~'
		: ((sc.state == SCE_MARKDOWN_EM_ASTERISK || sc.state == SCE_MARKDOWN_STRONG_ASTERISK) ? '*' : '_'));
	if (sc.ch == delimiter && (sc.state != SCE_MARKDOWN_STRIKEOUT || sc.chNext == '~')) {
		DelimiterRun delimiterRun;
		const int length = GetCurrentDelimiterRun(delimiterRun);

		const bool closed = delimiterRun.CanClose(delimiter);
		const int current = sc.state;
		if (current != SCE_MARKDOWN_STRIKEOUT) {
			// TODO: fix longest match failure for `***strong** in emph* t`
			if (length == 1 && current >= SCE_MARKDOWN_STRONG_ASTERISK) {
				// inner emphasis with `*`
				HighlightInlineText();
				return false;
			}
			if (!closed || (current < SCE_MARKDOWN_STRONG_ASTERISK && length == 2)) {
				if (delimiterRun.CanOpen(delimiter)) {
					// inner strong emphasis with `**`
					HighlightInlineText();
					return false;
				}
			}
		}

		const int outer = TakeOuterStyle();
		const Sci_PositionU startPos = TakeOuterStart();
		if (closed) {
			sc.Forward();
			if (current >= SCE_MARKDOWN_STRONG_ASTERISK) {
				sc.Forward();
			}
			sc.SetState(outer);
		} else {
			sc.ChangeState(outer);
			sc.BackTo(startPos);
			sc.Forward();
			if (current == SCE_MARKDOWN_STRIKEOUT) {
				sc.Forward();
			}
		}
		return true;
	}

	HighlightInlineText();
	return false;
}

// 4.7 Link reference definitions
inline bool IsLinkReferenceDefinition(LexAccessor &styler, Sci_Line line, Sci_PositionU startPos) noexcept {
	Sci_PositionU pos = styler.LineStart(line);
	const uint8_t ch = GetCharAfterIndent(styler, pos, 3);
	return ch == '[' && pos == startPos;
}

bool MarkdownLexer::HighlightLinkText() {
	if (sc.ch == ']') {
		sc.Forward();
		Sci_PositionU startPos = 0;
		--bracketCount;
		if (bracketCount == 0) {
			startPos = TakeOuterStart();
			sc.SetState(TakeOuterStyle());
		}
		if (sc.ch == '(' || (sc.ch == ':' && bracketCount == 0 && nestedState.empty() && sc.state == SCE_MARKDOWN_DEFAULT
			&& IsLinkReferenceDefinition(sc.styler, sc.currentLine, startPos))) {
			startPos = sc.currentPos;
			int chNext = sc.GetLineNextChar(true);
			if (chNext == '\0' && bracketCount == 0) {
				// link destination on next line
				startPos = sc.lineStartNext;
				const Sci_PositionU endPos = sc.styler.LineStart(sc.currentLine + 2);
				chNext = LexGetNextChar(sc.styler, startPos, endPos);
			}
			if (chNext != '\0') {
				const int current = sc.state;
				const int style = (chNext == '<') ? SCE_MARKDOWN_ANGLE_LINK
					: ((sc.ch == '(') ? SCE_MARKDOWN_PAREN_LINK : SCE_MARKDOWN_PLAIN_LINK);
				tagState = (startPos == sc.currentPos) ? HtmlTagState::None : HtmlTagState::Open;
				parenCount = sc.ch == '(';
				if (sc.ch != '(') {
					sc.SetState(SCE_MARKDOWN_DELIMITER);
					sc.ForwardSetState(current);
				}
				SaveOuterStyle(current);
				SaveOuterStart(sc.currentPos);
				sc.SetState(style);
				sc.Forward();
				if (tagState == HtmlTagState::None) {
					// skip whitespace between link text and destination
					while (IsSpaceOrTab(sc.ch)) {
						sc.Forward();
					}
					if (sc.ch == '<') {
						sc.Forward();
					}
				}
			}
		}
		return true;
	}

	if (IsEOLChar(sc.ch)) {
		bracketCount = 0;
		sc.ChangeState(TakeOuterStyle());
		sc.BackTo(TakeOuterStart());
		sc.Forward();
		return true;
	}
	if (sc.ch == '[' || sc.Match('!', '[')) {
		++bracketCount;
		if (sc.ch == '!') {
			sc.Forward();
		}
	} else {
		HighlightInlineText();
	}
	return false;
}

// 6.3 Links
bool MarkdownLexer::HighlightLinkDestination() {
	if (sc.ch == '\\' || sc.ch == '&' || sc.ch == ':') {
		HighlightInlineText();
		return false;
	}

	if (tagState >= HtmlTagState::Open && sc.atLineStart) {
		while (IsSpaceOrTab(sc.ch)) {
			sc.Forward();
		}
		switch (sc.state) {
		case SCE_MARKDOWN_ANGLE_LINK:
			if (tagState == HtmlTagState::Open && sc.ch == '<') {
				sc.Forward();
			}
			break;
		case SCE_MARKDOWN_LINK_TITLE_SQ:
			if (tagState == HtmlTagState::Value && sc.ch == '\'') {
				sc.Forward();
			}
			break;
		case SCE_MARKDOWN_LINK_TITLE_DQ:
			if (tagState == HtmlTagState::Value && sc.ch == '\"') {
				sc.Forward();
			}
			break;
		}
	}

	enum class LinkState {
		None,
		Finish,
		Title,
		Invalid,
	};

	LinkState linkState = LinkState::None;
	switch (sc.state) {
	case SCE_MARKDOWN_ANGLE_LINK:
		if (sc.ch == '>') {
			sc.Forward();
			while (IsSpaceOrTab(sc.ch)) {
				sc.Forward();
			}
			if (parenCount != 0 && sc.ch == ')') {
				--parenCount;
				linkState = LinkState::Finish;
			} else {
				linkState = (sc.chPrev != '>' || IsEOLChar(sc.ch)) ? LinkState::Title : LinkState::Invalid;
			}
		} else if (sc.ch == '<' || IsEOLChar(sc.ch)) {
			linkState = LinkState::Invalid;
		}
		break;

	case SCE_MARKDOWN_PLAIN_LINK:
	case SCE_MARKDOWN_PAREN_LINK:
	case SCE_MARKDOWN_LINK_TITLE_PAREN:
		if (sc.ch == '(') {
			++parenCount;
		} else if (sc.ch == ')') {
			--parenCount;
			if (parenCount == 0 && sc.state != SCE_MARKDOWN_PLAIN_LINK) {
				linkState = LinkState::Finish;
			}
		} else if (sc.state != SCE_MARKDOWN_LINK_TITLE_PAREN && !IsGraphic(sc.ch)) {
			linkState = LinkState::Invalid;
			if (IsMarkdownSpace(sc.ch)) {
				while (IsSpaceOrTab(sc.ch)) {
					sc.Forward();
				}
				if (sc.ch == ')' && parenCount == 1 && sc.state == SCE_MARKDOWN_PAREN_LINK) {
					--parenCount;
					linkState = LinkState::Finish;
				} else {
					linkState = LinkState::Title;
				}
			}
		}
		break;

	case SCE_MARKDOWN_LINK_TITLE_SQ:
		if (sc.ch == '\'') {
			linkState = LinkState::Finish;
		}
		break;

	case SCE_MARKDOWN_LINK_TITLE_DQ:
		if (sc.ch == '\"') {
			linkState = LinkState::Finish;
		}
		break;
	}

	switch (linkState) {
	case LinkState::None:
		return false;

	case LinkState::Finish: {
		tagState = HtmlTagState::None;
		parenCount = 0;
		const int current = sc.state;
		const int outer = TakeOuterStyle();
		const Sci_PositionU startPos = TakeOuterStart();
		sc.ForwardSetState(outer);
		if (current == SCE_MARKDOWN_LINK_TITLE_PAREN || current == SCE_MARKDOWN_LINK_TITLE_SQ || current == SCE_MARKDOWN_LINK_TITLE_DQ) {
			while (IsSpaceOrTab(sc.ch)) {
				sc.Forward();
			}
			if (sc.ch == ')') {
				// use same style for enclosing link parenthesis
				const int style = sc.styler.StyleAtEx(startPos);
				sc.SetState(style);
				sc.Forward();
				sc.SetState(outer);
			}
		}
	} break;

	case LinkState::Title: {
		int marker = sc.ch;
		if (IsEOLChar(marker)) {
			// link title on next line
			const Sci_PositionU endPos = sc.styler.LineStart(sc.currentLine + 2);
			marker = LexGetNextChar(sc.styler, sc.lineStartNext, endPos);
		}
		if (marker == '\'' || marker == '\"' || marker == '(') {
			const int style = (marker == '(') ? SCE_MARKDOWN_LINK_TITLE_PAREN
				: ((marker == '\'') ? SCE_MARKDOWN_LINK_TITLE_SQ : SCE_MARKDOWN_LINK_TITLE_DQ);
			sc.SetState(style);
			parenCount = sc.ch == '(';
			if (marker != sc.ch) {
				tagState = HtmlTagState::Value;
			} else {
				sc.Forward();
			}
			break;
		}
		if (IsEOLChar(sc.ch)) {
			if (marker == '\0' || IsParagraphEnd(sc.lineStartNext)) {
				// no link title
				tagState = HtmlTagState::None;
				sc.SetState(TakeOuterStyle());
				DropOuterStart();
				break;
			}
		}
	}
		[[fallthrough]];
	default: // invalid link destination or title
		tagState = HtmlTagState::None;
		parenCount = 0;
		sc.ChangeState(TakeOuterStyle());
		sc.BackTo(TakeOuterStart());
		sc.Forward();
		break;
	}
	return true;
}

// 6.9 Autolinks (extension)
constexpr bool IsSchemeNameChar(int ch) noexcept {
	return IsIdentifierChar(ch) || ch == '+' || ch == '-';
}

constexpr bool IsDomainNameChar(int ch) noexcept {
	return IsIdentifierChar(ch) || ch == '-';
}

AutoLink MarkdownLexer::RoughCheckAutoLink() const noexcept {
	Sci_PositionU pos = sc.currentPos;
	if (sc.Match('w', 'w')
		&& sc.styler.SafeGetCharAt(pos + 2) == 'w'
		&& sc.styler.SafeGetCharAt(pos + 3) == '.'
		&& IsDomainNameChar(sc.styler.SafeGetCharAt(pos + 4))) {
		return AutoLink::Domain;
	}
	if (IsSchemeNameChar(sc.chNext) || sc.chNext == '.') {
		int length = 2;
		pos += 2;
		while (true) {
			const uint8_t ch = sc.styler.SafeGetCharAt(pos++);
			if (IsSchemeNameChar(ch) || ch == '.' ) {
				++length;
				if (length > 32) {
					break;
				}
			} else {
				if (ch == ':' && sc.styler.SafeGetCharAt(pos) == '/'
					&& sc.styler.SafeGetCharAt(pos + 1) == '/'
					&& IsDomainNameChar(sc.styler.SafeGetCharAt(pos + 2))) {
					return AutoLink::Scheme;
				}
				break;
			}
		}
	}
	return AutoLink::None;
}

bool MarkdownLexer::HighlightAutoLink() {
	bool invalid = false;
	switch (autoLink) {
	case AutoLink::Angle:
		if (sc.ch == '>') {
			sc.ForwardSetState(TakeOuterStyle());
			return true;
		}
		if (sc.ch == '<' || !IsGraphic(sc.ch)) {
			invalid = true;
		}
		break;

	case AutoLink::Scheme:
		if (sc.ch == '.' && IsSchemeNameChar(sc.chNext)) {
			sc.Forward();
		} else if (!IsSchemeNameChar(sc.ch)) {
			if (sc.Match(':', '/', '/') && IsDomainNameChar(sc.GetRelative(3))) {
				tagState = HtmlTagState::Question;
				delimiterCount = 0;
				autoLink = AutoLink::Domain;
				sc.Advance(3);
			} else {
				invalid = true;
			}
		}
		break;

	case AutoLink::Domain:
		if (sc.ch == '.' && IsDomainNameChar(sc.chNext)) {
			++delimiterCount;
			sc.Forward();
		} else if (!IsDomainNameChar(sc.ch)) {
			invalid = (delimiterCount == 0 && tagState == HtmlTagState::None)
				|| (sc.ch == ':' && !IsADigit(sc.chNext));
			tagState = HtmlTagState::None;
			delimiterCount = 0;
			if (!invalid) {
				if (sc.ch == ':' || ((sc.ch == '/' || sc.ch == '?') && !IsInvalidUrlChar(sc.chNext))) {
					parenCount = 0;
					autoLink = AutoLink::Path;
				} else {
					if (sc.ch == '/') {
						sc.Forward();
					}
					sc.SetState(TakeOuterStyle());
					return true;
				}
			}
		}
		break;

	case AutoLink::Path:
		if (sc.ch == '(') {
			++parenCount;
		} else if (sc.ch == ')') {
			--parenCount;
		}
		if (IsInvalidUrlChar(sc.chNext)) {
			if (sc.ch == '/' || (sc.ch == ')' && parenCount == 0) || !IsPunctuation(sc.ch)) {
				sc.Forward();
			}
			parenCount = 0;
			autoLink = AutoLink::None;
			sc.SetState(TakeOuterStyle());
			return true;
		}
		break;

	default:
		break;
	}
	if (invalid) {
		parenCount = 0;
		autoLink = AutoLink::None;
		sc.ChangeState(TakeOuterStyle());
		sc.Rewind();
		sc.Forward();
		return true;
	}
	return false;
}

// 6.6 Raw HTML
constexpr bool IsHtmlTagStart(int ch) noexcept {
	return IsAlpha(ch);
}

constexpr bool IsHtmlTagChar(int ch) noexcept {
	return IsAlphaNumeric(ch) || ch == '-';
}

constexpr bool IsHtmlAttrStart(int ch) noexcept {
	return IsIdentifierStart(ch) || ch == ':';
}

constexpr bool IsHtmlAttrChar(int ch) noexcept {
	return IsIdentifierChar(ch) || ch == ':' || ch == '.' || ch == '-';
}

constexpr bool IsInvalidAttrChar(int ch) noexcept {
	// characters not allowed in unquoted attribute value
	return ch <= 32 || ch == 127 || AnyOf(ch, '"', '\'', '\\', '`', '=', '<', '>');
}

constexpr bool IsHtmlBlockStartChar(int ch) noexcept {
	return ch == '!' || ch == '?' || ch == '/' || IsHtmlTagStart(ch);
}

// https://pandoc.org/MANUAL.html#math
constexpr bool IsMathOpenDollar(int chNext) {
	return !IsASpace(chNext);
}

constexpr bool IsMathCloseDollar(int chPrev, int chNext) {
	return !IsASpace(chPrev) && !IsADigit(chNext);
}

bool MarkdownLexer::IsParagraphEnd(Sci_PositionU startPos) const noexcept {
	Sci_PositionU pos = startPos;
	uint8_t ch = GetCharAfterIndent(sc.styler, pos, 3);
	switch (ch) {
	case '\r':	// empty line
	case '\n':	// empty line
	case '>':	// block quote
		return true;

	case '#':
		ch = sc.styler.SafeGetCharAt(pos + 1);
		if (IsMarkdownSpace(ch)) {
			return true;
		}
		if (ch == '#') {
			return CheckATXHeading(sc.styler, pos);
		}
		break;

	case '<':
		ch = sc.styler.SafeGetCharAt(++pos);
		return IsHtmlBlockStartChar(ch);

	case '+':
	case '-':
	case '*':
	case '_':
		if (ch != '+' && CheckThematicBreak(sc.styler, pos, ch)) {
			return true;
		}
		if (ch != '_') { // bullet list
			ch = sc.styler.SafeGetCharAt(++pos);
			return IsMarkdownSpace(ch);
		}
		[[fallthrough]];
	case '=':
		return CheckSetextHeading(sc.styler, pos);

	case '`':
	case '~': // fenced code block
		return GetMatchedDelimiterCount(sc.styler, pos, ch) >= 3;

	case '[': // link reference definition
		ch = sc.styler.SafeGetCharAt(++pos);
		return !IsEOLChar(ch);

	case '$':
		if (markdown == Markdown::Pandoc && pos - startPos == 1) {
			ch = sc.styler.SafeGetCharAt(++pos);
			return ch == '$';
		}
		break;
	}

	// check for indented list item
	while (IsSpaceOrTab(ch)) {
		ch = sc.styler.SafeGetCharAt(++pos);
	}
	if (IsEOLChar(ch)) {
		return true; // empty line
	}

	const uint8_t chNext = sc.styler.SafeGetCharAt(pos + 1);
	if (ch == '+' || ch == '-' || ch == '*') {
		return IsMarkdownSpace(chNext);
	}
	const OrderedListType listType = CheckOrderedList(sc.styler, pos, ch, chNext);
	return listType == OrderedListType::Decimal
		|| (listType != OrderedListType::None && markdown == Markdown::Pandoc);
}

int MarkdownLexer::HighlightBlockText() {
	switch (sc.ch) {
	case '>':
		if (markdown == Markdown::GitLab && sc.atLineStart && sc.MatchNext('>', '>')) {
			sc.SetState(SCE_MARKDOWN_MULTI_BLOCKQUOTE);
		} else {
			sc.SetState(SCE_MARKDOWN_BLOCKQUOTE);
		}
		return 0;

	case '+':
	case '-':
	case '*':
	case '_':
		if (sc.ch != '+' && CheckThematicBreak(sc.styler, sc.currentPos, sc.ch)) {
			sc.SetState(SCE_MARKDOWN_HRULE);
			return 0;
		}
		if (sc.ch != '_' && IsMarkdownSpace(sc.chNext)) {
			sc.SetState(SCE_MARKDOWN_BULLET_LIST);
			return 0;
		}
		break;

	case '#':
		if (IsMarkdownSpace(sc.chNext)) {
			sc.SetState(SCE_MARKDOWN_HEADER1);
			return SCE_MARKDOWN_HEADER1;
		}
		if (sc.chNext == '#')  {
			int headerLevel = CheckATXHeading(sc.styler, sc.currentPos);
			if (headerLevel != 0) {
				headerLevel += SCE_MARKDOWN_HEADER1 - 1;
				sc.SetState(headerLevel);
				return headerLevel;
			}
		}
		break;

	case '`':
	case '~':
		if (sc.ch == sc.chNext) {
			Sci_PositionU pos = sc.currentPos;
			int chNext = GetCharAfterDelimiter(sc.styler, pos, sc.ch);
			const int count = static_cast<int>(pos - sc.currentPos);
			if (count >= 3) {
				delimiterCount = count;
				int style = (sc.ch == '`') ? SCE_MARKDOWN_BACKTICK_BLOCK : SCE_MARKDOWN_TILDE_BLOCK;
				// check info string
				chNext |= 0x20;
				if (chNext == 'm' || chNext == 'l')  {
					char info[8]{};
					sc.styler.GetRangeLowered(pos, sc.lineStartNext, info, sizeof(info));
					if (StrStartsWith(info, "math") || StrStartsWith(info, "latex")) {
						style += 1;
					}
				}
				sc.SetState(style);
				return 0;
			}
		}
		break;

	case '<':
		if (IsHtmlBlockStartChar(sc.chNext)) {
			HighlightInlineText();
			return 0;
		}
		break;

	case '$':
		if (markdown == Markdown::Pandoc && sc.atLineStart && sc.chNext == '$') {
			sc.SetState(SCE_MARKDOWN_DISPLAY_MATH);
			return 0;
		}
		break;
	}

	const OrderedListType listType = CheckOrderedList(sc.styler, sc.currentPos, sc.ch, sc.chNext);
	if (listType == OrderedListType::Decimal
		|| (listType != OrderedListType::None && markdown == Markdown::Pandoc)) {
		sc.SetState(SCE_MARKDOWN_ORDERED_LIST);
		return 0;
	}
	// setext heading has lower precedence than other blocks
	int headerLevel = CheckSetextHeading(sc.styler, sc.lineStartNext);
	if (headerLevel != 0) {
		headerLevel += SCE_MARKDOWN_SETEXT_H1 - 1;
		sc.SetState(headerLevel);
		HighlightInlineText();
		return headerLevel;
	}
	if (listType != OrderedListType::None) {
		// nonstandard ordered list
		sc.SetState(SCE_MARKDOWN_EXT_ORDERED_LIST);
		return 0;
	}

	switch (sc.ch) {
	case '~':
		if (IsSpaceOrTab(sc.chNext)) {
			// Pandoc definition list
			SaveOuterStyle(SCE_MARKDOWN_DEFAULT);
			sc.SetState(SCE_MARKDOWN_DELIMITER);
		}
		break;

	case '+':
	case '=':
		if (markdown == Markdown::Pandoc) {
			HighlightDelimiterRow();
		}
		break;

	case '|':
	case ':':
	case '-':
		HighlightDelimiterRow();
		break;
	}
	return 0;
}

void MarkdownLexer::HighlightInlineText() {
	bool handled = false;
	const int current = sc.state;
	switch (sc.ch) {
	case '\\':
		if (IsPunctuation(sc.chNext)) {
			sc.SetState(SCE_MARKDOWN_ESCAPECHAR);
			sc.Forward();
		}
		break;

	case '&':
		if (IsAlpha(sc.chNext)) {
			sc.SetState(SCE_H_ENTITY);
		} else if (sc.chNext == '#') {
			const int chNext = sc.GetRelative(2);
			if (IsADigit(chNext) || ((chNext == 'x' || chNext == 'X') && IsHexDigit(sc.GetRelative(3)))) {
				sc.SetState(SCE_H_ENTITY);
				sc.Forward();
			}
		}
		break;

	case '<':
		// 4.6 HTML blocks
		if (sc.chNext == '!') {
			const int chNext = sc.GetRelative(2);
			if (chNext == '-' && sc.GetRelative(3) == '-') {
				sc.SetState(SCE_H_COMMENT);
				sc.Advance(3);
			} else if (chNext == '[' && sc.styler.Match(sc.currentPos + 3, "CDATA[")) {
				// <![CDATA[ ]]>
				sc.SetState(SCE_H_CDATA);
				sc.Advance(8);
			} else if (IsAlpha(chNext)) {
				// <!DOCTYPE html>
				tagState = HtmlTagState::Open;
				// highlighted as tag instead of SGML
				sc.SetState(SCE_H_TAG);
				sc.Forward();
			} else {
				autoLink = AutoLink::Angle;
				sc.SetState(SCE_MARKDOWN_AUTOLINK);
			}
		} else if (sc.chNext == '?') {
			// <?php ?>
			tagState = HtmlTagState::Question;
			sc.SetState(SCE_H_QUESTION);
		} else if (IsHtmlTagStart(sc.chNext) || (sc.chNext == '/' && IsHtmlTagStart(sc.GetRelative(2)))) {
			sc.SetState(SCE_H_TAG);
			if (sc.chNext == '/') {
				tagState = HtmlTagState::None;
				sc.Forward();
			} else {
				tagState = HtmlTagState::Open;
			}
		} else if (!IsInvalidUrlChar(sc.chNext)) {
			autoLink = AutoLink::Angle;
			sc.SetState(SCE_MARKDOWN_AUTOLINK);
		}
		break;

	case '`':
		delimiterCount = GetMatchedDelimiterCount(sc.styler, sc.currentPos, '`');
		sc.SetState(SCE_MARKDOWN_CODE_SPAN);
		sc.Advance(delimiterCount - 1);
		break;

	case '*':
	case '_':
	case '~':
		if (sc.ch != '~' || sc.chNext == '~') {
			DelimiterRun delimiterRun;
			GetCurrentDelimiterRun(delimiterRun);
			const int delimiter = sc.ch;
			if (delimiterRun.CanOpen(delimiter)) {
				handled = true;
				int style;
				if (delimiter == '~') {
					style = SCE_MARKDOWN_STRIKEOUT;
				} else {
					style = (delimiter == '*') ? SCE_MARKDOWN_EM_ASTERISK : SCE_MARKDOWN_EM_UNDERSCORE;
					if (delimiter == sc.chNext) {
						style += SCE_MARKDOWN_STRONG_ASTERISK - SCE_MARKDOWN_EM_ASTERISK;
					}
				}
				SaveOuterStart(sc.currentPos);
				sc.SetState(style);
			}
			if (delimiter == sc.chNext) {
				sc.Forward(); // longest match
			}
		} else if (markdown == Markdown::Pandoc && !IsMarkdownSpace(sc.chNext)) {
			sc.SetState(SCE_MARKDOWN_SUBSCRIPT);
		}
		break;

	case '{':
	case '[':
		if (markdown == Markdown::GitLab && current == SCE_MARKDOWN_DEFAULT && (sc.chNext == '+' || sc.chNext == '-')) {
			// https://docs.gitlab.com/ee/user/markdown.html#inline-diff
			int style = (sc.chNext == '+') ? SCE_MARKDOWN_DIFF_ADD_CURLY : SCE_MARKDOWN_DIFF_DEL_CURLY;
			if (sc.ch == '[') {
				style += SCE_MARKDOWN_DIFF_ADD_SQUARE - SCE_MARKDOWN_DIFF_ADD_CURLY;
			}
			SaveOuterStart(sc.currentPos);
			sc.SetState(style);
			sc.Forward();
		} else if (sc.ch == '[') {
			if (current == SCE_MARKDOWN_DEFAULT && IsSpaceOrTab(sc.chPrev)
				&& (sc.chNext == ' ' || sc.chNext == 'x' || sc.chNext == 'X')
				&& sc.GetRelative(2) == ']' && IsMarkdownSpace(sc.GetRelative(3))) {
				// task list after list marker
				sc.SetState(SCE_MARKDOWN_TASK_LIST);
				sc.Advance(2);
			} else if (!(sc.chPrev == '!' || IsEOLChar(sc.chNext))) {
				// chPrev == '!' means parsing image link failed
				bracketCount = 1;
				SaveOuterStart(sc.currentPos);
				sc.SetState(SCE_MARKDOWN_LINK_TEXT);
			}
		}
		break;

	case '!':
		if (sc.chNext == '[') {
			bracketCount = 1;
			SaveOuterStart(sc.currentPos);
			sc.SetState(SCE_MARKDOWN_LINK_TEXT);
			sc.Forward();
		}
		break;

	case ':':
		if (IsAlphaNumeric(sc.chNext) || ((sc.chNext == '+' || sc.chNext == '-') && IsADigit(sc.GetRelative(2)))) {
			sc.SetState(SCE_MARKDOWN_EMOJI);
			if (sc.chNext == '+') {
				sc.Forward();
			}
		} else if (sc.chNext == ':' || sc.chPrev == ':') {
			sc.SetState(SCE_MARKDOWN_DELIMITER); // Pandoc fenced divs
		}
		break;

	case '|': // table pipe, Pandoc line block
		sc.SetState(SCE_MARKDOWN_DELIMITER);
		break;

	case '$':
		if ((markdown == Markdown::GitLab && sc.chNext == '`')
			|| (markdown == Markdown::Pandoc && IsMathOpenDollar(sc.chNext))) {
			sc.SetState(SCE_MARKDOWN_INLINE_MATH);
			if (markdown == Markdown::GitLab) {
				sc.Forward();
			}
		}
		break;

	case '(':
		if (markdown == Markdown::Pandoc && sc.chNext == '@') {
			sc.SetState(SCE_MARKDOWN_EXAMPLE_LIST);
			sc.Forward();
		}
		break;

	case '^':
		if (markdown == Markdown::Pandoc && !IsMarkdownSpace(sc.chNext)) {
			sc.SetState(SCE_MARKDOWN_SUPERSCRIPT);
		}
		break;

	case '@':
		if (IsIdentifierChar(sc.chNext) && !IsIdentifierChar(sc.chPrev)) {
			sc.SetState(SCE_MARKDOWN_CITATION_AT);
		}
		break;

	default:
		if (bracketCount == 0 && IsAlpha(sc.ch) && !IsIdentifierChar(sc.chPrev)) {
			const AutoLink result = RoughCheckAutoLink();
			if (result != AutoLink::None) {
				tagState = HtmlTagState::None;
				delimiterCount = 0;
				autoLink = result;
				sc.SetState(SCE_MARKDOWN_AUTOLINK);
				if (result == AutoLink::Domain) {
					sc.Advance(3);
				}
			}
		}
		break;
	}
	if (handled || (current != sc.state && IsInlineStyle(sc.state))) {
		SaveOuterStyle(current);
	}
}

bool MarkdownLexer::HighlightInlineDiff() {
	if (sc.ch == '\\' || sc.ch == '&' || sc.ch == ':') {
		HighlightInlineText();
	} else if (IsEOLChar(sc.ch) || sc.ch == '`' || (sc.ch == '<' && IsHtmlBlockStartChar(sc.chNext))) {
		sc.ChangeState(nestedState.back());
		sc.BackTo(backPos.back());
		if (sc.ch == '[') {
			bracketCount = 1;
			sc.SetState(SCE_MARKDOWN_LINK_TEXT);
		} else {
			DropOuterStyle();
			DropOuterStart();
			sc.Forward();
			return true;
		}
	} else if (sc.ch == ((sc.state < SCE_MARKDOWN_DIFF_DEL_CURLY) ? '+' : '-')) {
		if (sc.chNext == (((sc.state - SCE_MARKDOWN_DIFF_DEL_CURLY) & 1) ? ']' : '}')) {
			DropOuterStart();
			sc.Forward(2);
			sc.SetState(TakeOuterStyle());
			return true;
		}
	}
	return false;
}

void ColouriseMarkdownDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList /*keywordLists*/, Accessor &styler) {
	if (startPos != 0) {
		BacktrackToStart(styler, LineStateNestedStateLine, startPos, lengthDoc, initStyle);
		Sci_PositionU pos = startPos;
		const uint8_t ch = GetCharAfterIndent(styler, pos, 4);
		if (IsBlockStartChar(ch) || pos - startPos == 4) {
			// backtrack to previous line for better coloring on typing.
			const Sci_PositionU endPos = startPos + lengthDoc;
			const Sci_Line currentLine = styler.GetLine(startPos);
			startPos = styler.LineStart(currentLine - 1);
			lengthDoc = endPos - startPos;
			initStyle = (startPos == 0) ? 0 : styler.StyleAt(startPos - 1);
		}
	}

	MarkdownLexer lexer(startPos, lengthDoc, initStyle, styler);
	const bool fold = styler.GetPropertyBool("fold");

	StyleContext &sc = lexer.sc;
	uint32_t lineState = 0;
	int visibleChars = 0;
	int indentCurrent = 0;
	// parent block indentation count for indented code block,
	// indented code block ends on the line when indentParent - indentCurrent < 4
	//int indentParent = 0;
	int headerLevel = 0;
	int prevLevel = SC_FOLDLEVELBASE;

	if (sc.currentLine > 0) {
		prevLevel = styler.LevelAt(sc.currentLine - 1);
		lineState = styler.GetLineState(sc.currentLine - 1);
		lexer.tagState = static_cast<HtmlTagState>(lineState & 3);
		lexer.delimiterCount = (lineState >> 8) & 0xff;
		/*
		2: tagState
		1: empty line
		1: nestedState
		1: list item first line
		1: block end line
		1: setext first line
		1: unused
		8: delimiterCount
		8: indentCurrent
		8: indentParent
		*/
	}
	if (startPos == 0) {
		switch (sc.ch) {
		case '-': // YAML or others
			if (sc.MatchNext('-', '-')) {
				initStyle = SCE_MARKDOWN_METADATA_YAML;
			}
			break;

		case '+': // TOML
		case ';': // JSON
			if (lexer.markdown == Markdown::GitLab && sc.ch == sc.chNext && sc.ch == sc.GetRelative(2)) {
				initStyle = (sc.ch == '+') ? SCE_MARKDOWN_METADATA_TOML : SCE_MARKDOWN_METADATA_JSON;
			}
			break;

		case '%':// title block
			if (lexer.markdown == Markdown::Pandoc) {
				initStyle = SCE_MARKDOWN_TITLE_BLOCK;
			}
			break;
		}
		if (initStyle != SCE_MARKDOWN_DEFAULT) {
			sc.SetState(initStyle);
			sc.Forward();
		}
	}

	while (sc.More()) {
		if (sc.atLineStart) {
			visibleChars = 0;
			indentCurrent = 0;
			headerLevel = 0;
			if (lineState & LineStateBlockEndLine) {
				lineState &= ~LineStateBlockEndLine;
				lexer.delimiterCount = 0;
				sc.SetState(SCE_MARKDOWN_DEFAULT);
			}
		}

		switch (sc.state) {
		// block
		case SCE_MARKDOWN_HEADER1:
		case SCE_MARKDOWN_HEADER2:
		case SCE_MARKDOWN_HEADER3:
		case SCE_MARKDOWN_HEADER4:
		case SCE_MARKDOWN_HEADER5:
		case SCE_MARKDOWN_HEADER6:
			if (sc.atLineStart) {
				sc.SetState(SCE_MARKDOWN_DEFAULT);
			} else {
				lexer.HighlightInlineText();
			}
			break;

		case SCE_MARKDOWN_SETEXT_H1:
		case SCE_MARKDOWN_SETEXT_H2:
			if (lineState & LineStateSetextFirstLine) {
				lexer.HighlightInlineText();
			} else if (visibleChars == 0 && sc.ch == ((sc.state == SCE_MARKDOWN_SETEXT_H1) ? '=' : '-')) {
				lineState |= LineStateBlockEndLine;
			}
			break;

		case SCE_MARKDOWN_HRULE:
		case SCE_MARKDOWN_DELIMITER_ROW:
			if (sc.atLineStart) {
				sc.SetState(SCE_MARKDOWN_DEFAULT);
			} else if (sc.ch == '|' || sc.ch == '+') {
				lexer.SaveOuterStyle(SCE_MARKDOWN_DELIMITER_ROW);
				sc.SetState(SCE_MARKDOWN_DELIMITER);
			}
			break;

		case SCE_MARKDOWN_BACKTICK_BLOCK:
		case SCE_MARKDOWN_BACKTICK_MATH:
		case SCE_MARKDOWN_TILDE_BLOCK:
		case SCE_MARKDOWN_TILDE_MATH:
			if (visibleChars == 0 && indentCurrent < 4
				&& sc.ch == ((sc.state <= SCE_MARKDOWN_BACKTICK_MATH) ? '`' : '~')) {
				const int count = GetMatchedDelimiterCount(styler, sc.currentPos, sc.ch);
				if (count >= lexer.delimiterCount) {
					lineState |= LineStateBlockEndLine;
				}
			}
			break;

		case SCE_MARKDOWN_BLOCKQUOTE:
			if (sc.atLineStart && lexer.IsParagraphEnd(sc.currentPos)) {
				sc.SetState(SCE_MARKDOWN_DEFAULT);
			} else {
				lexer.HighlightInlineText();
			}
			break;

		case SCE_MARKDOWN_MULTI_BLOCKQUOTE:
			if (sc.atLineStart && sc.Match('>', '>', '>')) {
				lineState |= LineStateBlockEndLine;
			} else {
				lexer.HighlightInlineText();
			}
			break;

		case SCE_MARKDOWN_BULLET_LIST:
		case SCE_MARKDOWN_ORDERED_LIST:
		case SCE_MARKDOWN_EXT_ORDERED_LIST:
			if (!IsGraphic(sc.ch)) {
				sc.SetState(SCE_MARKDOWN_DEFAULT);
			}
			break;

		case SCE_MARKDOWN_METADATA_YAML:
		case SCE_MARKDOWN_METADATA_TOML:
		case SCE_MARKDOWN_METADATA_JSON:
			if (sc.atLineStart) {
				const int delimiter = static_cast<uint8_t>((sc.state == SCE_MARKDOWN_METADATA_YAML) ? '-'
					: ((sc.state == SCE_MARKDOWN_METADATA_TOML) ? '+' : ';'));
				if ((sc.ch == delimiter && sc.chNext == delimiter && sc.GetRelative(2) == delimiter)
					|| (sc.state == SCE_MARKDOWN_METADATA_YAML && sc.Match('.', '.', '.'))) {
					// `...` YAML document end marker, used by Pandoc
					lineState |= LineStateBlockEndLine;
				}
			}
			break;

		case SCE_MARKDOWN_TITLE_BLOCK:
			if (sc.atLineStart && !(sc.ch == '%' || (sc.ch == ' ' && sc.GetLineNextChar(true) != '\0'))) {
				sc.SetState(SCE_MARKDOWN_DEFAULT);
			} else {
				lexer.HighlightInlineText();
			}
			break;

		case SCE_MARKDOWN_DISPLAY_MATH:
			if (sc.atLineStart && sc.Match('$', '$')) {
				lineState |= LineStateBlockEndLine;
			}
			break;

		// inline
		case SCE_MARKDOWN_ESCAPECHAR:
		case SCE_MARKDOWN_DELIMITER:
		case SCE_MARKDOWN_TASK_LIST:
			sc.SetState(lexer.TakeOuterStyle());
			continue;

		case SCE_MARKDOWN_EM_ASTERISK:
		case SCE_MARKDOWN_EM_UNDERSCORE:
		case SCE_MARKDOWN_STRONG_ASTERISK:
		case SCE_MARKDOWN_STRONG_UNDERSCORE:
		case SCE_MARKDOWN_STRIKEOUT:
			if (lexer.HighlightEmphasis()) {
				continue;
			}
			break;

		case SCE_MARKDOWN_LINK_TEXT:
			if (lexer.HighlightLinkText()) {
				continue;
			}
			break;

		case SCE_MARKDOWN_ANGLE_LINK:
		case SCE_MARKDOWN_PLAIN_LINK:
		case SCE_MARKDOWN_PAREN_LINK:
		case SCE_MARKDOWN_LINK_TITLE_PAREN:
		case SCE_MARKDOWN_LINK_TITLE_SQ:
		case SCE_MARKDOWN_LINK_TITLE_DQ:
			if (lexer.HighlightLinkDestination()) {
				continue;
			}
			break;

		case SCE_MARKDOWN_AUTOLINK:
			if (lexer.HighlightAutoLink()) {
				continue;
			}
			break;

		case SCE_MARKDOWN_CODE_SPAN:
			if (sc.ch == '`') {
				const int count = GetMatchedDelimiterCount(styler, sc.currentPos, '`');
				sc.Advance(count - 1);
				if (count == lexer.delimiterCount) {
					lexer.delimiterCount = 0;
					sc.ForwardSetState(lexer.TakeOuterStyle());
					continue;
				}
			}
			break;

		case SCE_MARKDOWN_INLINE_MATH:
			if ((lexer.markdown == Markdown::GitLab) ? sc.Match('`', '$')
				: (sc.ch == '$' && IsMathCloseDollar(sc.chPrev, sc.chNext))) {
				if (lexer.markdown == Markdown::GitLab) {
					sc.Forward();
				}
				sc.ForwardSetState(lexer.TakeOuterStyle());
				continue;
			}
			if (sc.ch == '$' || sc.ch == '`' || sc.atLineEnd) {
				sc.ChangeState(lexer.TakeOuterStyle());
				sc.Rewind();
				sc.Forward();
				continue;
			}
			break;

		case SCE_MARKDOWN_EMOJI:
		case SCE_MARKDOWN_EXAMPLE_LIST:
		case SCE_MARKDOWN_CITATION_AT:
			if ((sc.ch == ':' && sc.state == SCE_MARKDOWN_EMOJI)
				|| (sc.ch == ')' && sc.state == SCE_MARKDOWN_EXAMPLE_LIST)) {
				sc.ForwardSetState(lexer.TakeOuterStyle());
				continue;
			}
			if (!(IsIdentifierChar(sc.ch) || sc.ch == '-')) {
				const int outer = lexer.TakeOuterStyle();
				if (sc.state == SCE_MARKDOWN_CITATION_AT) {
					sc.SetState(outer);
				} else {
					sc.ChangeState(outer);
					sc.Rewind();
					sc.Forward();
				}
				continue;
			}
			break;

		case SCE_MARKDOWN_SUPERSCRIPT:
		case SCE_MARKDOWN_SUBSCRIPT:
			if (sc.ch == '\\' && IsSpaceOrTab(sc.chNext)) {
				sc.Forward();
			} else if ((sc.ch == '^' && sc.state == SCE_MARKDOWN_SUPERSCRIPT)
				|| (sc.ch == '~' && sc.state == SCE_MARKDOWN_SUBSCRIPT)) {
				sc.ForwardSetState(lexer.TakeOuterStyle());
				continue;
			} else if (!IsGraphic(sc.ch)) {
				sc.Rewind();
				if (IsAlphaNumeric(sc.chNext)) { // MultiMarkdown
					const int style = sc.state + SCE_MARKDOWN_SHORT_SUPERSCRIPT - SCE_MARKDOWN_SUPERSCRIPT;
					sc.ChangeState(style);
				} else {
					sc.ChangeState(lexer.TakeOuterStyle());
					sc.Forward();
					continue;
				}
			}
			break;

		case SCE_MARKDOWN_SHORT_SUPERSCRIPT:
		case SCE_MARKDOWN_SHORT_SUBSCRIPT:
			if (!IsAlphaNumeric(sc.ch)) {
				sc.SetState( lexer.TakeOuterStyle());
				continue;
			}
			break;

		case SCE_MARKDOWN_DIFF_ADD_CURLY:
		case SCE_MARKDOWN_DIFF_ADD_SQUARE:
		case SCE_MARKDOWN_DIFF_DEL_CURLY:
		case SCE_MARKDOWN_DIFF_DEL_SQUARE:
			if (lexer.HighlightInlineDiff()) {
				continue;
			}
			break;

		// basic html
		case SCE_H_TAG:
			if (sc.ch == '>' || sc.Match('/', '>')) {
				lexer.tagState = HtmlTagState::None;
				sc.Forward((sc.ch == '/') ? 2 : 1);
				sc.SetState(lexer.TakeOuterStyle());
				continue;
			}
			if (!IsHtmlTagChar(sc.ch)) {
				if (IsASpace(sc.ch)) {
					// tag attribute
					sc.SetState(SCE_MARKDOWN_DEFAULT);
				} else {
					lexer.tagState = HtmlTagState::None;
					lexer.autoLink = AutoLink::Angle;
					sc.ChangeState(SCE_MARKDOWN_AUTOLINK);
					continue;
				}
			}
			break;

		case SCE_H_ENTITY:
			if (sc.ch == ';') {
				sc.ForwardSetState(lexer.TakeOuterStyle());
				continue;
			}
			// https://html.spec.whatwg.org/entities.json
			if (!IsAlphaNumeric(sc.ch)) {
				sc.ChangeState(lexer.TakeOuterStyle());
				sc.Rewind();
				sc.Forward();
				continue;
			}
			break;

		case SCE_H_ATTRIBUTE:
			if (!IsHtmlAttrChar(sc.ch)) {
				sc.SetState(SCE_MARKDOWN_DEFAULT);
			}
			break;

		case SCE_H_VALUE:
			if (IsInvalidAttrChar(sc.ch)) {
				sc.SetState(SCE_MARKDOWN_DEFAULT);
			}
			break;

		case SCE_H_SINGLESTRING:
			if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_MARKDOWN_DEFAULT);
			}
			break;

		case SCE_H_DOUBLESTRING:
			if (sc.ch == '\"') {
				sc.ForwardSetState(SCE_MARKDOWN_DEFAULT);
			}
			break;

		case SCE_H_OTHER:
			sc.SetState(SCE_MARKDOWN_DEFAULT);
			break;

		case SCE_H_COMMENT:
			if (sc.Match('-', '-', '>')) {
				sc.Advance(3);
				sc.SetState(SCE_MARKDOWN_DEFAULT);
			}
			break;

		case SCE_H_CDATA:
			if (sc.Match(']', ']', '>')) {
				sc.Forward(3);
				if (sc.GetLineNextChar() == '\0') {
					lineState |= LineStateBlockEndLine;
				} else {
					sc.SetState(SCE_MARKDOWN_DEFAULT);
				}
			}
			break;

		case SCE_H_QUESTION:
			if (sc.Match('?', '>') || IsASpace(sc.ch)) {
				if (sc.ch == '?') {
					lexer.tagState = HtmlTagState::None;
					sc.Forward(2);
				}
				sc.SetState(SCE_MARKDOWN_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_MARKDOWN_DEFAULT) {
			switch (lexer.tagState) {
			case HtmlTagState::None:
				if (sc.ch > ' ') {
					if (visibleChars == 0) {
						if (indentCurrent < 4) {
							headerLevel = lexer.HighlightBlockText();
							if (headerLevel == SCE_MARKDOWN_SETEXT_H1 || headerLevel == SCE_MARKDOWN_SETEXT_H2) {
								lineState |= LineStateSetextFirstLine;
							}
						} else {
							lexer.HighlightIndentedText();
						}
						if (sc.state == SCE_MARKDOWN_BULLET_LIST || sc.state == SCE_MARKDOWN_ORDERED_LIST) {
							lineState |= LineStateListItemFirstLine;
						}
					}
					if (sc.state == SCE_MARKDOWN_DEFAULT) {
						lexer.HighlightInlineText();
					}
				}
				break;

			case HtmlTagState::Question:
				if (sc.Match('?', '>')) {
					lexer.tagState = HtmlTagState::None;
					sc.SetState(SCE_H_QUESTION);
					sc.Forward(2);
					sc.SetState(SCE_MARKDOWN_DEFAULT);
					continue;
				}
				break;

			default:
				if (sc.ch == '>' || sc.Match('/', '>')) {
					lexer.tagState = HtmlTagState::None;
					sc.SetState(SCE_H_TAG);
					sc.Forward((sc.ch == '/') ? 2 : 1);
					sc.SetState(lexer.TakeOuterStyle());
					continue;
				}
				if (sc.ch == '\'') {
					sc.SetState(SCE_H_SINGLESTRING);
				} else if (sc.ch == '\"') {
					sc.SetState(SCE_H_DOUBLESTRING);
				} else if (sc.ch == '=') {
					sc.SetState(SCE_H_OTHER);
				} else if (lexer.tagState == HtmlTagState::Open && IsHtmlAttrStart(sc.ch)) {
					sc.SetState(SCE_H_ATTRIBUTE);
				} else if (!IsInvalidAttrChar(sc.ch)) {
					sc.SetState(SCE_H_VALUE);
				}
				if (sc.state != SCE_MARKDOWN_DEFAULT) {
					lexer.tagState = (sc.state == SCE_H_OTHER) ? HtmlTagState::Value : HtmlTagState::Open;
				}
				break;
			}
		}

		if (visibleChars == 0) {
			if (sc.ch == ' ') {
				indentCurrent += 1;
			} else if (sc.ch == '\t') {
				indentCurrent = (indentCurrent/4 + 1)*4;
			} else if (!IsASpace(sc.ch)) {
				visibleChars = 1;
			}
		}
		if (sc.atLineEnd) {
			if (fold) {
				int nextLevel;
				if (headerLevel) {
					headerLevel -= (headerLevel <= SCE_MARKDOWN_HEADER6) ? SCE_MARKDOWN_HEADER1 : SCE_MARKDOWN_SETEXT_H1;
					headerLevel += SC_FOLDLEVELBASE;
					nextLevel = headerLevel | SC_FOLDLEVELHEADERFLAG;
					if (nextLevel == prevLevel || (prevLevel != SC_FOLDLEVELBASE && (lineState & LineStateEmptyLine) != 0)) {
						styler.SetLevel(sc.currentLine - 1, headerLevel);
					}
				} else if (prevLevel & SC_FOLDLEVELHEADERFLAG) {
					nextLevel = (prevLevel & SC_FOLDLEVELNUMBERMASK) + 1;
				} else {
					nextLevel = prevLevel;
				}
				styler.SetLevel(sc.currentLine, nextLevel);
				prevLevel = nextLevel;
			}

			lineState = (lineState & (LineStateListItemFirstLine | LineStateBlockEndLine))
				| static_cast<int>(lexer.tagState)
				| (lexer.delimiterCount << 8);
			if (visibleChars == 0) {
				lineState |= LineStateEmptyLine;
			} else {
				lineState |= (indentCurrent << 16);
			}
			if (lexer.tagState >= HtmlTagState::Open || StyleNeedsBacktrack(sc.state)) {
				lineState |= LineStateNestedStateLine;
			}
			if (!lexer.nestedState.empty()) {
				const int outer = lexer.nestedState.front();
				if (IsHeaderStyle(outer) || (lexer.tagState == HtmlTagState::None && lexer.IsParagraphEnd(sc.lineStartNext))) {
					lexer.nestedState.clear();
					lexer.backPos.clear();
					sc.SetState(outer);
				} else {
					lineState |= LineStateNestedStateLine;
				}
			}
			styler.SetLineState(sc.currentLine, static_cast<int>(lineState));
			lineState &= ~LineStateListItemFirstLine;
		}
		sc.Forward();
	}

	sc.Complete();
}

}

LexerModule lmMarkdown(SCLEX_MARKDOWN, ColouriseMarkdownDoc, "markdown");
