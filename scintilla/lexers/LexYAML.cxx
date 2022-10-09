// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for YAML.

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
#include "LexerModule.h"

using namespace Lexilla;

namespace {

struct EscapeSequence {
	int digitsLeft = 0;

	// highlight any character as escape sequence.
	void resetEscapeState(int chNext) noexcept {
		digitsLeft = 1;
		if (chNext == 'x') {
			digitsLeft = 3;
		} else if (chNext == 'u') {
			digitsLeft = 5;
		} else if (chNext == 'U') {
			digitsLeft = 9;
		}
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

constexpr bool IsYAMLFlowIndicator(int ch) noexcept {
	// c-flow-indicator
	return ch == ',' || AnyOf<'[', ']', '{', '}'>(ch);
}

constexpr bool IsYAMLOperator(int ch, int braceCount) noexcept {
	// remaining c-indicator
	return ch == '{' || ch == '[' || ch == '@' || ch == '`'
		|| (braceCount && (ch == ',' || ch == '}' || ch == ']'));
}

constexpr bool IsYAMLAnchorChar(int ch) noexcept {
	// ns-anchor-char ::= ns-char - c-flow-indicator
	return IsGraphic(ch) && !IsYAMLFlowIndicator(ch);
}

constexpr bool IsYAMLDateTime(int ch, int chNext) noexcept {
	return ((ch == '-' || ch == ':' || ch == '.') && IsADigit(chNext))
		|| (ch == ' ' && (chNext == '-' || IsADigit(chNext)));
}

bool IsYAMLText(StyleContext& sc, int braceCount, const WordList *kwList) {
	const int state = sc.state;
	const Sci_Position endPos = braceCount? sc.styler.Length() : sc.lineStartNext;
	const unsigned char chNext = LexGetNextChar(sc.styler, sc.currentPos, endPos);
	if (chNext == ':') {
		// possible key
		sc.ChangeState(SCE_YAML_TEXT);
		return true;
	}
	if (chNext == '\0'
		|| (chNext == '#' && isspacechar(sc.ch))
		|| (braceCount && (chNext == ',' || chNext == '}' || chNext == ']'))) {
		if (state == SCE_YAML_IDENTIFIER) {
			char s[8];
			sc.GetCurrentLowered(s, sizeof(s));
			if (kwList->InList(s)) {
				sc.ChangeState(SCE_YAML_KEYWORD);
				sc.SetState(SCE_YAML_DEFAULT);
			}
		} else {
			sc.SetState(SCE_YAML_DEFAULT);
		}
	}
	if (sc.state == state) {
		sc.ChangeState(SCE_YAML_TEXT);
		return true;
	}
	return false;
}

bool IsYAMLTextBlockEnd(int state, int &indentCount, int textIndentCount,
	Sci_Position pos, Sci_Position lineStartNext, LexAccessor &styler) noexcept {
	const Sci_Position endPos = styler.Length();
	do {
		char ch = '\n';
		int indentation = 0;
		while (pos < endPos && (ch = styler[pos]) == ' ') {
			++pos;
			++indentation;
		}
		if (pos < lineStartNext) {
			indentCount = indentation;
		}
		if (state != SCE_YAML_BLOCK_SCALAR && ch == '#') {
			return true;
		}
		if (indentation != 0 && indentation != textIndentCount) {
			return indentation < textIndentCount;
		}
		if (state == SCE_YAML_TEXT) {
			return true;
		}
		if (!IsEOLChar(ch)) {
			return true;
		}
		// skip to next line
		while (pos < endPos && IsEOLChar(styler[pos])) {
			++pos;
		}
	} while (pos < endPos);
	return true;
}

enum class YAMLLineType {
	None = 0,
	EmptyLine,
	CommentLine,
	BlockSequence,
};

constexpr int GetIndentCount(int lineState) noexcept {
	return lineState >> 19;
}

constexpr YAMLLineType GetLineType(int lineState) noexcept {
	return static_cast<YAMLLineType>(lineState & 3);
}

void ColouriseYAMLDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int visibleChars = 0;
	bool indentEnded = true;
	bool hasKey = false;
	int indentCount = 0;
	int indentBefore = 0;
	int textIndentCount = 0;
	int braceCount = 0;
	YAMLLineType lineType = YAMLLineType::None;
	int lineStatePrev = 0;
	EscapeSequence escSeq;

	// backtrack to previous line for better coloring for indented text on typing.
	if (initStyle == SCE_YAML_INDENTED_TEXT || initStyle == SCE_YAML_TEXT) {
		const Sci_Position endPos = startPos + lengthDoc;
		const Sci_Line currentLine = styler.GetLine(startPos);
		startPos = (currentLine == 0)? 0 : styler.LineStart(currentLine - 1);
		lengthDoc = endPos - startPos;
		initStyle = (startPos == 0)? SCE_YAML_DEFAULT : styler.StyleAt(startPos - 1);
	}

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		lineStatePrev = styler.GetLineState(sc.currentLine - 1);
		/*
		2: lineType
		8: braceCount
		9: textIndentCount
		12: indentCount
		*/
		braceCount = (lineStatePrev >> 2) & 0xff;
		textIndentCount = (lineStatePrev >> 10) & 0x1ff;
	}

	while (sc.More()) {
		if (sc.atLineStart) {
			visibleChars = 0;
			indentBefore = 0;
			indentCount = 0;
			indentEnded = false;
			hasKey = false;

			if (sc.state == SCE_YAML_BLOCK_SCALAR || (sc.state == SCE_YAML_TEXT && !braceCount) || sc.state == SCE_YAML_INDENTED_TEXT) {
				indentEnded = true;
				if (IsYAMLTextBlockEnd(sc.state, indentCount, textIndentCount, sc.currentPos, sc.lineStartNext, styler)) {
					textIndentCount = 0;
					sc.SetState(SCE_YAML_DEFAULT);
					sc.Forward(indentCount);
				} else {
					if (sc.state == SCE_YAML_TEXT) {
						sc.ChangeState(SCE_YAML_INDENTED_TEXT);
					}
					sc.Forward(indentCount);
					// inside block scalar or indented text
					indentCount = textIndentCount + 1;
				}
			} else if (sc.state == SCE_YAML_STRING_SQ || sc.state == SCE_YAML_STRING_DQ) {
				// multiline quoted string
				indentCount = GetIndentCount(lineStatePrev);
				if (indentCount == textIndentCount) {
					++indentCount;
				}
				indentEnded = true;
				visibleChars = 1;
			}
		}

		switch (sc.state) {
		case SCE_YAML_OPERATOR:
			sc.SetState(SCE_YAML_DEFAULT);
			break;

		case SCE_YAML_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				if (IsYAMLDateTime(sc.ch, sc.chNext)) {
					sc.ChangeState(SCE_YAML_DATETIME);
				} else if (IsYAMLText(sc, braceCount, nullptr)) {
					continue;
				}
			}
			break;

		case SCE_YAML_DATETIME:
			if (!(IsIdentifierChar(sc.ch) || IsYAMLDateTime(sc.ch, sc.chNext))) {
				 if (IsYAMLText(sc, braceCount, nullptr)) {
					continue;
				}
			}
			break;

		case SCE_YAML_IDENTIFIER:
			if (!IsAlpha(sc.ch)) {
				if (IsYAMLText(sc, braceCount, &keywordLists[0])) {
					continue;
				}
			}
			break;

		case SCE_YAML_TEXT:
			if (sc.ch == ':') {
				if ((!hasKey || braceCount) && isspacechar(sc.chNext)) {
					hasKey = true;
					sc.ChangeState(SCE_YAML_KEY);
					sc.SetState(SCE_YAML_OPERATOR);
				}
			} else if (braceCount && IsYAMLFlowIndicator(sc.ch)) {
				sc.SetState(SCE_YAML_OPERATOR);
				if (sc.ch == '{' || sc.ch == '[') {
					++braceCount;
				} else if (sc.ch == '}' || sc.ch == ']') {
					--braceCount;
				}
			} else if (sc.ch == '#' && isspacechar(sc.chPrev)) {
				sc.SetState(SCE_YAML_COMMENT);
			}
			break;

		case SCE_YAML_ANCHOR:
		case SCE_YAML_ALIAS:
			if (!IsYAMLAnchorChar(sc.ch)) {
				sc.SetState(SCE_YAML_DEFAULT);
			}
			break;

		case SCE_YAML_TAG:
		case SCE_YAML_VERBATIM_TAG:
			if (sc.state == SCE_YAML_VERBATIM_TAG && sc.ch == '>') {
				sc.ForwardSetState(SCE_YAML_DEFAULT);
			} else if (IsInvalidUrlChar(sc.ch)) {
				sc.SetState(SCE_YAML_DEFAULT);
			}
			break;

		case SCE_YAML_STRING_SQ:
			if (sc.ch == '\'') {
				if (sc.chNext == '\'') {
					sc.SetState(SCE_YAML_ESCAPECHAR);
					sc.Advance(2);
					sc.SetState(SCE_YAML_STRING_SQ);
					continue;
				}

				sc.Forward();
				if (sc.GetDocNextChar() == ':') {
					hasKey = true;
					sc.ChangeState(SCE_YAML_KEY);
				}
				sc.SetState((sc.ch == ':')? SCE_YAML_OPERATOR : SCE_YAML_DEFAULT);
			}
			break;

		case SCE_YAML_STRING_DQ:
			if (sc.ch == '\\' && !IsEOLChar(sc.chNext)) {
				escSeq.resetEscapeState(sc.chNext);
				sc.SetState(SCE_YAML_ESCAPECHAR);
				sc.Forward();
			} else if (sc.ch == '\"') {
				sc.Forward();
				if (sc.GetDocNextChar() == ':') {
					hasKey = true;
					sc.ChangeState(SCE_YAML_KEY);
				}
				sc.SetState((sc.ch == ':')? SCE_YAML_OPERATOR : SCE_YAML_DEFAULT);
			}
			break;

		case SCE_YAML_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(SCE_YAML_STRING_DQ);
				continue;
			}
			break;

		case SCE_YAML_COMMENT:
		case SCE_YAML_DOCUMENT:
		case SCE_YAML_DIRECTIVE:
			if (sc.atLineStart) {
				sc.SetState(SCE_YAML_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_YAML_DEFAULT) {
			if (sc.ch == '%' && sc.atLineStart) {
				sc.SetState(SCE_YAML_DIRECTIVE);
			} else if (sc.ch == '#' && (visibleChars == 0 || isspacechar(sc.chPrev))) {
				sc.SetState(SCE_YAML_COMMENT);
				if (visibleChars == 0 && lineType == YAMLLineType::None) {
					indentCount = 0;
					lineType = YAMLLineType::CommentLine;
				}
			} else if (sc.atLineStart && (sc.Match('-', '-', '-') || sc.Match('.', '.', '.'))) {
				// reset document state
				braceCount = 0;
				visibleChars = 1;
				sc.SetState(SCE_YAML_DOCUMENT);
				sc.Advance(3);
				const int chNext = sc.GetLineNextChar();
				if (chNext != '\0') {
					sc.SetState(SCE_YAML_DEFAULT);
					// no `continue;` here: a white space is required after document marker.
				}
			} else if (sc.ch == '\'') {
				textIndentCount = indentCount;
				sc.SetState(SCE_YAML_STRING_SQ);
			} else if (sc.ch == '\"') {
				textIndentCount = indentCount;
				sc.SetState(SCE_YAML_STRING_DQ);
			} else if ((sc.ch == '&' || sc.ch == '*') && IsYAMLAnchorChar(sc.chNext)) {
				sc.SetState((sc.ch == '&')? SCE_YAML_ANCHOR : SCE_YAML_ALIAS);
			} else if (sc.ch == '!') {
				if (sc.chNext == '<') {
					sc.SetState(SCE_YAML_VERBATIM_TAG);
					sc.Advance(2);
				} else {
					sc.SetState(SCE_YAML_TAG);
				}
			} else if (sc.ch == '|' || sc.ch == '>') {
				// ignore block scalar header or comment
				if (lineType == YAMLLineType::BlockSequence) {
					textIndentCount = hasKey ? indentCount : indentBefore;
					++textIndentCount;
				} else {
					textIndentCount = indentCount;
				}
				sc.SetState(SCE_YAML_BLOCK_SCALAR);
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_YAML_NUMBER);
			} else if (IsAlpha(sc.ch) || (sc.ch == '.' && IsAlpha(sc.chNext))) {
				sc.SetState(SCE_YAML_IDENTIFIER);
			} else if (IsYAMLOperator(sc.ch, braceCount) || (sc.ch == '?' && sc.chNext == ' ') || (sc.ch == ':' && isspacechar(sc.chNext))) {
				sc.SetState(SCE_YAML_OPERATOR);
				if (sc.ch == '{' || sc.ch == '[') {
					++braceCount;
				} else if (sc.ch == '}' || sc.ch == ']') {
					--braceCount;
				}
			} else if (sc.ch == '+' || sc.ch == '-' || sc.ch == '.') {
				if ((sc.ch == '-' && isspacechar(sc.chNext))) {
					sc.SetState(SCE_YAML_OPERATOR);
					sc.ForwardSetState(SCE_YAML_DEFAULT);
					if (visibleChars == 0 && lineType == YAMLLineType::None) {
						// spaces after '-' are indentation white space when '- ' followed by key.
						indentBefore = indentCount;
						indentEnded = false;
						lineType = YAMLLineType::BlockSequence;
					} else {
						++visibleChars;
					}
				} else if (IsADigit(sc.chNext) || (sc.ch != '.' && sc.chNext == '.')) {
					// [+-]number, [+-].[inf | nan]
					sc.SetState(SCE_YAML_OPERATOR);
				} else {
					sc.SetState(SCE_YAML_TEXT);
				}
			} else if (!isspacechar(sc.ch)) {
				sc.SetState(SCE_YAML_TEXT);
			}
		}

		if (visibleChars == 0) {
			if (sc.ch == ' ') {
				if (!indentEnded) {
					++indentCount;
				}
			} else {
				indentEnded = true;
				if (!isspacechar(sc.ch)) {
					++visibleChars;
				}
			}
		}
		if (sc.atLineEnd) {
			if (sc.state == SCE_YAML_TEXT && !braceCount) {
				if (lineType == YAMLLineType::BlockSequence) {
					textIndentCount = hasKey ? indentCount : indentBefore;
					++textIndentCount;
				} else {
					textIndentCount = indentCount;
				}
			} else if (lineType == YAMLLineType::BlockSequence) {
				// temporary fix for unindented block sequence:
				// children content should be indented at least two levels (for '- ') greater than current line,
				// thus increase one indentation level doesn't break code folding.
				if (!hasKey) {
					indentCount = indentBefore + 1;
				}
			} else if (visibleChars == 0 && !(sc.state == SCE_YAML_BLOCK_SCALAR || sc.state == SCE_YAML_INDENTED_TEXT)) {
				indentCount = 0;
				lineType = YAMLLineType::EmptyLine;
			}

			lineStatePrev = (braceCount << 2) | (textIndentCount << 10) | (indentCount << 19) | static_cast<int>(lineType);
			styler.SetLineState(sc.currentLine, lineStatePrev);
			lineType = YAMLLineType::None;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int indentCount;
	YAMLLineType lineType;
	constexpr explicit FoldLineState(int lineState) noexcept:
		indentCount(GetIndentCount(lineState)),
		lineType(GetLineType(lineState)) {
	}
	constexpr bool Empty() const noexcept {
		return lineType == YAMLLineType::EmptyLine || lineType == YAMLLineType::CommentLine;
	}
};

// code folding based on LexNull
void FoldYAMLDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList, Accessor &styler) {
	const Sci_Position maxPos = startPos + lengthDoc;
	const Sci_Line docLines = styler.GetLine(styler.Length());
	const Sci_Line maxLines = (maxPos == styler.Length()) ? docLines : styler.GetLine(maxPos - 1);

	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState stateCurrent(styler.GetLineState(lineCurrent));
	while (lineCurrent > 0) {
		lineCurrent--;
		stateCurrent = FoldLineState(styler.GetLineState(lineCurrent));
		if (!stateCurrent.Empty()) {
			break;
		}
	}

	while (lineCurrent <= maxLines) {
		Sci_Position lineNext = lineCurrent + 1;
		FoldLineState stateNext = stateCurrent;
		if (lineNext <= docLines) {
			stateNext = FoldLineState(styler.GetLineState(lineNext));
		}
		if (stateNext.Empty()) {
			stateNext.indentCount = stateCurrent.indentCount;
		}
		while ((lineNext < docLines) && stateNext.Empty()) {
			lineNext++;
			stateNext = FoldLineState(styler.GetLineState(lineNext));
		}

		int lev = stateCurrent.indentCount + SC_FOLDLEVELBASE;
		if (!stateCurrent.Empty()) {
			if (stateCurrent.indentCount < stateNext.indentCount) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
		}

		styler.SetLevel(lineCurrent, lev);
		lineCurrent++;

		if (lineCurrent < lineNext) {
			const int levelAfterBlank = stateNext.indentCount;
			const int skipLevel = levelAfterBlank + SC_FOLDLEVELBASE;

			YAMLLineType prevLineType = stateCurrent.lineType;
			YAMLLineType nextLineType = GetLineType(styler.GetLineState(lineCurrent));
			int prevLevel = skipLevel;
			// comment on first line
			if (prevLineType == YAMLLineType::CommentLine) {
				nextLineType = prevLineType;
				prevLineType = YAMLLineType::None;
				--lineCurrent;
			}
			for (; lineCurrent < lineNext; lineCurrent++) {
				int level = skipLevel;
				const YAMLLineType currentLineType = nextLineType;
				nextLineType = GetLineType(styler.GetLineState(lineCurrent + 1));
				if (currentLineType == YAMLLineType::CommentLine) {
					if (nextLineType == YAMLLineType::CommentLine && prevLineType != YAMLLineType::CommentLine) {
						level |= SC_FOLDLEVELHEADERFLAG;
					} else if (prevLevel & SC_FOLDLEVELHEADERFLAG) {
						level++;
					} else {
						level = prevLevel;
					}
				}

				styler.SetLevel(lineCurrent, level);
				prevLineType = currentLineType;
				prevLevel = level;
			}
		}

		stateCurrent = stateNext;
	}
}

}

LexerModule lmYAML(SCLEX_YAML, ColouriseYAMLDoc, "yaml", FoldYAMLDoc);
