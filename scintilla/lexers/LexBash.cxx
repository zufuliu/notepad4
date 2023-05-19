// Scintilla source code edit control
/** @file LexBash.cxx
 ** Lexer for Bash.
 **/
// Copyright 2004-2012 by Neil Hodgson <neilh@scintilla.org>
// Adapted from LexPerl by Kein-Hong Man 2004
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

using namespace Lexilla;

namespace {

//KeywordIndex++Autogenerated -- start of section automatically generated
enum {
	KeywordIndex_Keyword = 0,
	KeywordIndex_BashStruct = 1,
};
//KeywordIndex--Autogenerated -- end of section automatically generated

#define HERE_DELIM_MAX			256

// define this if you want 'invalid octals' to be marked as errors
// usually, this is not a good idea, permissive lexing is better
#undef PEDANTIC_OCTAL

#define BASH_BASE_ERROR			65
#define BASH_BASE_DECIMAL		66
#define BASH_BASE_HEX			67
#ifdef PEDANTIC_OCTAL
#define BASH_BASE_OCTAL			68
#define	BASH_BASE_OCTAL_ERROR	69
#endif

// state constants for parts of a bash command segment
enum class CmdState {
	Body,
	Start,
	Word,
	Test,
	Arithmetic,
	Delimiter,
};

enum class TestExprType {
	Test,			// test
	DoubleBracket,	// [[]]
	SingleBracket,	// []
};

// state constants for nested delimiter pairs, used by
// SCE_SH_STRING, SCE_SH_PARAM and SCE_SH_BACKTICKS processing
enum class QuoteStyle {
	Literal,		// ''
	CString,		// $''
	String,			// ""
	LString,		// $""
	Backtick,		// ``, $``
	Parameter,		// ${}
	Command,		// $()
	Arithmetic,		// $(()), $[]
};

#define BASH_QUOTE_STACK_MAX	7

constexpr int translateBashDigit(int ch) noexcept {
	if (ch >= '0' && ch <= '9') {
		return ch - '0';
	}
	if (ch >= 'a' && ch <= 'z') {
		return ch - 'a' + 10;
	}
	if (ch >= 'A' && ch <= 'Z') {
		return ch - 'A' + 36;
	}
	if (ch == '@') {
		return 62;
	}
	if (ch == '_') {
		return 63;
	}
	return BASH_BASE_ERROR;
}

int getBashNumberBase(const char *s) noexcept {
	int i = 0;
	int base = 0;
	while (*s) {
		base = base * 10 + (*s++ - '0');
		i++;
	}
	if (base > 64 || i > 2) {
		return BASH_BASE_ERROR;
	}
	return base;
}

constexpr int opposite(int ch) noexcept {
	if (ch == '(') return ')';
	if (ch == '[') return ']';
	if (ch == '{') return '}';
	if (ch == '<') return '>';
	return ch;
}

int GlobScan(StyleContext &sc) noexcept {
	// forward scan for zsh globs, disambiguate versus bash arrays
	// complex expressions may still fail, e.g. unbalanced () '' "" etc
	int c;
	int sLen = 0;
	int pCount = 0;
	int hash = 0;
	while ((c = sc.GetRelativeCharacter(++sLen)) != 0) {
		if (IsASpace(c)) {
			return 0;
		} else if (c == '\'' || c == '\"') {
			if (hash != 2) {
				return 0;
			}
		} else if (c == '#' && hash == 0) {
			hash = (sLen == 1) ? 2 : 1;
		} else if (c == '(') {
			pCount++;
		} else if (c == ')') {
			if (pCount == 0) {
				if (hash) {
					return sLen;
				}
				return 0;
			}
			pCount--;
		}
	}
	return 0;
}

constexpr bool IsBashWordChar(int ch) noexcept {
	// note that [+-] are often parts of identifiers in shell scripts
	return IsIdentifierChar(ch) || ch == '.' || ch == '+' || ch == '-';
}

constexpr bool IsBashMetaCharacter(int ch) noexcept {
	return ch <= 32 || AnyOf(ch, '|', '&', ';', '(', ')', '<', '>');
}

constexpr bool IsBashOperator(int ch) noexcept {
	return AnyOf(ch, '^', '&', '%', '(', ')', '-', '+', '=', '|', '{', '}', '[', ']', ':', ';', '>', ',', '*', '<', '?', '!', '.', '~', '@');
}

constexpr bool IsBashOperatorLast(int ch) noexcept {
	return IsAGraphic(ch) && !(ch == '/'); // remaining graphic characters
}

constexpr bool IsBashSingleCharOperator(int ch) noexcept {
	return AnyOf(ch, 'r', 'w', 'x', 'o', 'R', 'W', 'X', 'O', 'e', 'z', 's', 'f', 'd', 'l', 'p', 'S', 'b', 'c', 't', 'u', 'g', 'k', 'T', 'B', 'M', 'A', 'C', 'a', 'h', 'G', 'L', 'N', 'n');
}

constexpr bool IsBashParamChar(int ch) noexcept {
	return IsIdentifierChar(ch);
}

constexpr bool IsBashHereDoc(int ch) noexcept {
	return IsAlpha(ch) || AnyOf(ch, '_', '\\', '-', '+', '!', '%', '*', ',', '.', '/', ':', '?', '@', '[', ']', '^', '`', '{', '}', '~');
}

constexpr bool IsBashHereDoc2(int ch) noexcept {
	return IsAlphaNumeric(ch) || AnyOf(ch, '_', '-', '+', '!', '%', '*', ',', '.', '/', ':', '=', '?', '@', '[', ']', '^', '`', '{', '}', '~');
}

constexpr bool IsBashLeftShift(int ch) noexcept {
	return IsADigit(ch) || ch == '$';
}

constexpr bool IsBashCmdDelimiter(int ch, int chNext) noexcept {
	if (chNext == 0) {
		return AnyOf(ch, '|', '&', ';', '(', ')', '{', '}');
	}
	return (ch == chNext && (ch == '|' || ch == '&' || ch == ';'))
		|| (ch == '|' && chNext == '&');
}

class QuoteCls {	// Class to manage quote pairs (simplified vs LexPerl)
public:
	int Count = 0;
	int Up = '\0';
	int Down = '\0';
	QuoteStyle Style = QuoteStyle::Literal;
	int Outer = SCE_SH_DEFAULT;
	void Clear() noexcept {
		Count = 0;
		Up	  = '\0';
		Down  = '\0';
		Style = QuoteStyle::Literal;
		Outer = SCE_SH_DEFAULT;
	}
	void Start(int u, QuoteStyle s, int outer) noexcept {
		Count = 1;
		Up    = u;
		Down  = opposite(Up);
		Style = s;
		Outer = outer;
	}
};

class QuoteStackCls {	// Class to manage quote pairs that nest
public:
	int Depth = 0;
	int State = SCE_SH_DEFAULT;
	QuoteCls Current;
	QuoteCls Stack[BASH_QUOTE_STACK_MAX];
	[[nodiscard]] bool Empty() const noexcept {
		return Current.Up == '\0';
	}
	void Start(int u, QuoteStyle s, int outer) noexcept {
		if (Empty()) {
			Current.Start(u, s, outer);
		} else {
			Push(u, s, outer);
		}
	}
	void Push(int u, QuoteStyle s, int outer) noexcept {
		if (Depth >= BASH_QUOTE_STACK_MAX) {
			return;
		}
		Stack[Depth] = Current;
		Depth++;
		Current.Start(u, s, outer);
	}
	void Pop() noexcept {
		if (Depth <= 0) {
			return;
		}
		Depth--;
		Current = Stack[Depth];
	}
	void Clear() noexcept {
		Depth = 0;
		State = SCE_SH_DEFAULT;
		Current.Clear();
	}
	bool CountDown(StyleContext &sc, CmdState &cmdState) {
		Current.Count--;
		if (Current.Count == 1 && sc.Match(')', ')')) {
			Current.Count--;
			sc.Forward();
		}
		if (Current.Count == 0) {
			cmdState = CmdState::Body;
			const int outer = Current.Outer;
			if (Depth > 0) {
				Pop();
			} else {
				Clear();
			}
			sc.ForwardSetState(outer);
			return true;
		}
		return false;
	}
	void Expand(StyleContext &sc, CmdState &cmdState) {
		const int state = sc.state;
		QuoteStyle style = QuoteStyle::Literal;
		State = state;
		sc.SetState(SCE_SH_SCALAR);
		sc.Forward();
		if (sc.ch == '{') {
			style = QuoteStyle::Parameter;
			sc.ChangeState(SCE_SH_PARAM);
		} else if (sc.ch == '\'') {
			style = QuoteStyle::CString;
			sc.ChangeState(SCE_SH_STRING_DQ);
		} else if (sc.ch == '"') {
			style = QuoteStyle::LString;
			sc.ChangeState(SCE_SH_STRING_DQ);
		} else if (sc.ch == '(' || sc.ch == '[') {
			sc.ChangeState(SCE_SH_OPERATOR);
			if (sc.ch == '[' || sc.chNext == '(') {
				style = QuoteStyle::Arithmetic;
				cmdState = CmdState::Arithmetic;
			} else {
				style = QuoteStyle::Command;
				cmdState = CmdState::Delimiter;
			}
			if (sc.ch == '(' && state == SCE_SH_DEFAULT && Depth == 0) {
				// optimized to avoid track nested delimiter pairs
				return;
			}
		} else if (sc.ch == '`') {	// $` seen in a configure script, valid?
			style = QuoteStyle::Backtick;
			sc.ChangeState(SCE_SH_BACKTICKS);
		} else {
			// scalar has no delimiter pair
			return;
		}
		Start(sc.ch, style, state);
		sc.Forward();
	}
};

void ColouriseBashDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	class HereDocCls {	// Class to manage HERE document elements
	public:
		int State = 0;		// 0: '<<' encountered
		// 1: collect the delimiter
		// 2: here doc text (lines after the delimiter)
		int Quote = '\0';		// the char after '<<'
		bool Quoted = false;		// true if Quote in ('\'','"','`')
		bool Escaped = false;		// backslash in delimiter, common in configure script
		bool Indent = false;		// indented delimiter (for <<-)
		int DelimiterLength = 0;	// strlen(Delimiter)
		char Delimiter[HERE_DELIM_MAX]{};	// the Delimiter
		void Append(int ch) noexcept {
			Delimiter[DelimiterLength++] = static_cast<char>(ch);
			Delimiter[DelimiterLength] = '\0';
		}
	};
	HereDocCls HereDoc;

	QuoteStackCls QuoteStack;

	int numBase = 0;
	int digit;
	const Sci_PositionU endPos = startPos + length;
	CmdState cmdState = CmdState::Start;
	TestExprType testExprType = TestExprType::Test;

	// Always backtracks to the start of a line that is not a continuation
	// of the previous line (i.e. start of a bash command segment)
	Sci_Line ln = styler.GetLine(startPos);
	if (ln > 0 && startPos == static_cast<Sci_PositionU>(styler.LineStart(ln))) {
		ln--;
	}
	for (;;) {
		startPos = styler.LineStart(ln);
		if (ln == 0 || styler.GetLineState(ln) == static_cast<int>(CmdState::Start)) {
			break;
		}
		ln--;
	}
	initStyle = SCE_SH_DEFAULT;
	StyleContext sc(startPos, endPos - startPos, initStyle, styler);

	while (sc.More()) {
		// handle line continuation, updates per-line stored state
		if (sc.atLineStart) {
			if (sc.state == SCE_SH_STRING_DQ
				|| sc.state == SCE_SH_BACKTICKS
				|| sc.state == SCE_SH_STRING_SQ
				|| sc.state == SCE_SH_HERE_Q
				|| sc.state == SCE_SH_COMMENTLINE
				|| sc.state == SCE_SH_PARAM) {
				// force backtrack while retaining cmdState
				styler.SetLineState(sc.currentLine, static_cast<int>(CmdState::Body));
			} else {
				if (sc.currentLine > 0) {
					if ((sc.GetRelative(-3) == '\\' && sc.GetRelative(-2) == '\r' && sc.chPrev == '\n')
						|| sc.GetRelative(-2) == '\\') {	// handle '\' line continuation
						   // retain last line's state
					} else {
						cmdState = CmdState::Start;
					}
				}
				// force backtrack when nesting
				const CmdState state = QuoteStack.Empty() ? cmdState : CmdState::Body;
				styler.SetLineState(sc.currentLine, static_cast<int>(state));
			}
		}

		// controls change of cmdState at the end of a non-whitespace element
		// states Body|Test|Arithmetic persist until the end of a command segment
		// state Word persist, but ends with 'in' or 'do' construct keywords
		CmdState cmdStateNew = CmdState::Body;
		if (cmdState == CmdState::Test || cmdState == CmdState::Arithmetic || cmdState == CmdState::Word) {
			cmdStateNew = cmdState;
		}
		const int stylePrev = sc.state;

		// Determine if the current state should terminate.
		switch (sc.state) {
		case SCE_SH_OPERATOR:
			sc.SetState(SCE_SH_DEFAULT);
			if (cmdState == CmdState::Delimiter) {		// if command delimiter, start new command
				cmdStateNew = CmdState::Start;
			} else if (sc.chPrev == '\\') {			// propagate command state if line continued
				cmdStateNew = cmdState;
			}
			break;
		case SCE_SH_WORD:
			// "." never used in Bash variable names but used in file names
			if (!IsBashWordChar(sc.ch) || sc.Match('+', '=')) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				// allow keywords ending in a whitespace or command delimiter
				const bool keywordEnds = IsASpace(sc.ch) || IsBashCmdDelimiter(sc.ch, 0);
				// 'in' or 'do' may be construct keywords
				if (cmdState == CmdState::Word) {
					if (StrEqual(s, "in") && keywordEnds) {
						cmdStateNew = CmdState::Body;
					} else if (StrEqual(s, "do") && keywordEnds) {
						cmdStateNew = CmdState::Start;
					} else {
						sc.ChangeState(SCE_SH_IDENTIFIER);
					}
					sc.SetState(SCE_SH_DEFAULT);
					break;
				}
				// a 'test' keyword starts a test expression
				if (StrEqual(s, "test")) {
					if (cmdState == CmdState::Start && keywordEnds) {
						cmdStateNew = CmdState::Test;
						testExprType = TestExprType::Test;
					} else {
						sc.ChangeState(SCE_SH_IDENTIFIER);
					}
				}
				// detect bash construct keywords
				else if (keywordLists[KeywordIndex_BashStruct].InList(s)) {
					if (cmdState == CmdState::Start && keywordEnds) {
						cmdStateNew = CmdState::Start;
					} else {
						sc.ChangeState(SCE_SH_IDENTIFIER);
					}
				}
				// 'for'|'case'|'select' needs 'in'|'do' to be highlighted later
				else if (StrEqualsAny(s, "for", "case", "select")) {
					if (cmdState == CmdState::Start && keywordEnds) {
						cmdStateNew = CmdState::Word;
					} else {
						sc.ChangeState(SCE_SH_IDENTIFIER);
					}
				}
				// disambiguate option items and file test operators
				else if (s[0] == '-') {
					if (cmdState != CmdState::Test) {
						sc.ChangeState(SCE_SH_IDENTIFIER);
					}
				}
				// disambiguate keywords and identifiers
				else if (cmdState != CmdState::Start
					|| !(keywordLists[KeywordIndex_Keyword].InList(s) && keywordEnds)) {
					sc.ChangeState(SCE_SH_IDENTIFIER);
				}

				// m4
				if (StrEqual(s, "dnl")) {
					sc.ChangeState(SCE_SH_COMMENTLINE);
					if (sc.atLineEnd) {
						sc.SetState(SCE_SH_DEFAULT);
					}
				} else {
					const int nextState = sc.Match('+', '=') ? SCE_SH_OPERATOR : SCE_SH_DEFAULT;
					sc.SetState(nextState);
				}
			}
			break;
		case SCE_SH_IDENTIFIER:
			if (sc.chPrev == '\\') {	// for escaped chars
				sc.ForwardSetState(SCE_SH_DEFAULT);
			} else if (!IsBashWordChar(sc.ch)) {
				sc.SetState(SCE_SH_DEFAULT);
			} else if (cmdState == CmdState::Arithmetic && !IsIdentifierStart(sc.ch)) {
				sc.SetState(SCE_SH_DEFAULT);
			}
			break;
		case SCE_SH_NUMBER:
			digit = translateBashDigit(sc.ch);
			if (numBase == BASH_BASE_DECIMAL) {
				if (sc.ch == '#') {
					char s[10];
					sc.GetCurrent(s, sizeof(s));
					numBase = getBashNumberBase(s);
					if (numBase != BASH_BASE_ERROR) {
						break;
					}
				} else if (IsADigit(sc.ch)) {
					break;
				}
			} else if (numBase == BASH_BASE_HEX) {
				if (IsHexDigit(sc.ch)) {
					break;
				}
#ifdef PEDANTIC_OCTAL
			} else if (numBase == BASH_BASE_OCTAL ||
				numBase == BASH_BASE_OCTAL_ERROR) {
				if (digit <= 7) {
					break;
				}
				if (digit <= 9) {
					numBase = BASH_BASE_OCTAL_ERROR;
					break;
				}
#endif
			} else if (numBase == BASH_BASE_ERROR) {
				if (digit <= 9) {
					break;
				}
			} else {	// DD#DDDD number style handling
				if (digit != BASH_BASE_ERROR) {
					if (numBase <= 36) {
						// case-insensitive if base<=36
						if (digit >= 36) {
							digit -= 26;
						}
					}
					if (digit < numBase) {
						break;
					}
					if (digit <= 9) {
						numBase = BASH_BASE_ERROR;
						break;
					}
				}
			}
			// fallthrough when number is at an end or error
			if (numBase == BASH_BASE_ERROR
#ifdef PEDANTIC_OCTAL
				|| numBase == BASH_BASE_OCTAL_ERROR
#endif
				) {
				sc.ChangeState(SCE_SH_ERROR);
			}
			sc.SetState(SCE_SH_DEFAULT);
			break;
		case SCE_SH_COMMENTLINE:
			if (sc.MatchLineEnd() && sc.chPrev != '\\') {
				sc.SetState(SCE_SH_DEFAULT);
			}
			break;
		case SCE_SH_HERE_DELIM:
			// From Bash info:
			// ---------------
			// Specifier format is: <<[-]WORD
			// Optional '-' is for removal of leading tabs from here-doc.
			// Whitespace acceptable after <<[-] operator
			//
			if (HereDoc.State == 0) { // '<<' encountered
				HereDoc.Quote = sc.chNext;
				HereDoc.Quoted = false;
				HereDoc.Escaped = false;
				HereDoc.DelimiterLength = 0;
				HereDoc.Delimiter[0] = '\0';
				if (sc.chNext == '\'' || sc.chNext == '\"') {	// a quoted here-doc delimiter (' or ")
					sc.Forward();
					HereDoc.Quoted = true;
					HereDoc.State = 1;
				} else if (IsBashHereDoc(sc.chNext) ||
					(sc.chNext == '=' && cmdState != CmdState::Arithmetic)) {
					// an unquoted here-doc delimiter, no special handling
					HereDoc.State = 1;
				} else if (sc.chNext == '<') {	// HERE string <<<
					sc.Forward();
					sc.ForwardSetState(SCE_SH_DEFAULT);
				} else if (IsASpace(sc.chNext)) {
					// eat whitespace
				} else if (IsBashLeftShift(sc.chNext) ||
					(sc.chNext == '=' && cmdState == CmdState::Arithmetic)) {
					// left shift <<$var or <<= cases
					sc.ChangeState(SCE_SH_OPERATOR);
					sc.ForwardSetState(SCE_SH_DEFAULT);
				} else {
					// symbols terminates; deprecated zero-length delimiter
					HereDoc.State = 1;
				}
			} else if (HereDoc.State == 1) { // collect the delimiter
				// * if single quoted, there's no escape
				// * if double quoted, there are \\ and \" escapes
				if ((HereDoc.Quote == '\'' && sc.ch != HereDoc.Quote) ||
					(HereDoc.Quoted && sc.ch != HereDoc.Quote && sc.ch != '\\') ||
					(HereDoc.Quote != '\'' && sc.chPrev == '\\') ||
					(IsBashHereDoc2(sc.ch))) {
					HereDoc.Append(sc.ch);
				} else if (HereDoc.Quoted && sc.ch == HereDoc.Quote) {	// closing quote => end of delimiter
					sc.ForwardSetState(SCE_SH_DEFAULT);
				} else if (sc.ch == '\\') {
					HereDoc.Escaped = true;
					if (HereDoc.Quoted && sc.chNext != HereDoc.Quote && sc.chNext != '\\') {
						// in quoted prefixes only \ and the quote eat the escape
						HereDoc.Append(sc.ch);
					} else {
						// skip escape prefix
					}
				} else if (!HereDoc.Quoted) {
					sc.SetState(SCE_SH_DEFAULT);
				}
				if (HereDoc.DelimiterLength >= HERE_DELIM_MAX - 1) {	// force blowup
					sc.SetState(SCE_SH_ERROR);
					HereDoc.State = 0;
				}
			}
			break;
		case SCE_SH_HERE_Q:
			// HereDoc.State == 2
			if (sc.atLineStart) {
				sc.SetState(SCE_SH_HERE_Q);
				if (HereDoc.Indent) { // tabulation prefix
					while (sc.ch == '\t') {
						sc.Forward();
					}
				}
				if ((HereDoc.DelimiterLength == 0 && sc.MatchLineEnd())
					|| (styler.Match(sc.currentPos, HereDoc.Delimiter) && IsEOLChar(sc.GetRelative(HereDoc.DelimiterLength)))) {
					sc.Forward(HereDoc.DelimiterLength);
					sc.SetState(SCE_SH_DEFAULT);
					break;
				}
			}
			if (!HereDoc.Quoted && !HereDoc.Escaped) {
				if (sc.ch == '\\') {
					sc.Forward();
				} else if (sc.ch == '`') {
					QuoteStack.Start(sc.ch, QuoteStyle::Backtick, sc.state);
					sc.SetState(SCE_SH_BACKTICKS);
				} else if (sc.ch == '$') {
					QuoteStack.Expand(sc, cmdState);
					continue;
				}
			}
			break;
		case SCE_SH_SCALAR:	// variable names
			if (!IsBashParamChar(sc.ch)) {
				if (sc.LengthCurrent() == 1) {
					// Special variable: $(, $_ etc.
					sc.Forward();
				}
				sc.SetState(QuoteStack.State);
				continue;
			}
			break;
		case SCE_SH_STRING_DQ:	// delimited styles, can nest
		case SCE_SH_PARAM: // ${parameter}
		case SCE_SH_BACKTICKS:
			if (sc.ch == '\\') {
				sc.Forward();
			} else if (sc.ch == QuoteStack.Current.Down) {
				if (QuoteStack.CountDown(sc, cmdState)) {
					continue;
				}
			} else if (sc.ch == QuoteStack.Current.Up) {
				QuoteStack.Current.Count++;
			} else {
				if (QuoteStack.Current.Style == QuoteStyle::String ||
					QuoteStack.Current.Style == QuoteStyle::LString
					) {	// do nesting for "string", $"locale-string"
					if (sc.ch == '`') {
						QuoteStack.Push(sc.ch, QuoteStyle::Backtick, sc.state);
						sc.SetState(SCE_SH_BACKTICKS);
					} else if (sc.ch == '$') {
						QuoteStack.Expand(sc, cmdState);
						continue;
					}
				} else if (QuoteStack.Current.Style == QuoteStyle::Command
					|| QuoteStack.Current.Style == QuoteStyle::Parameter
					|| QuoteStack.Current.Style == QuoteStyle::Backtick
					) {	// do nesting for $(command), `command`, ${parameter}
					if (sc.ch == '\'') {
						QuoteStack.State = sc.state;
						sc.SetState(SCE_SH_STRING_SQ);
					} else if (sc.ch == '\"') {
						QuoteStack.Push(sc.ch, QuoteStyle::String, sc.state);
						sc.SetState(SCE_SH_STRING_DQ);
					} else if (sc.ch == '`') {
						QuoteStack.Push(sc.ch, QuoteStyle::Backtick, sc.state);
						sc.SetState(SCE_SH_BACKTICKS);
					} else if (sc.ch == '$') {
						QuoteStack.Expand(sc, cmdState);
						continue;
					}
				}
			}
			break;
		case SCE_SH_STRING_SQ: // singly-quoted strings
			if (sc.ch == '\'') {
				sc.ForwardSetState(QuoteStack.State);
				continue;
			}
			break;
		}

		// Must check end of HereDoc state 1 before default state is handled
		if (HereDoc.State == 1 && sc.MatchLineEnd()) {
			// Begin of here-doc (the line after the here-doc delimiter):
			// Lexically, the here-doc starts from the next line after the >>, but the
			// first line of here-doc seem to follow the style of the last EOL sequence
			HereDoc.State = 2;
			if (HereDoc.Quoted) {
				if (sc.state == SCE_SH_HERE_DELIM) {
					// Missing quote at end of string! Syntax error in bash 4.3
					// Mark this bit as an error, do not colour any here-doc
					sc.ChangeState(SCE_SH_ERROR);
					sc.SetState(SCE_SH_DEFAULT);
				} else {
					// HereDoc.Quote always == '\''
					sc.SetState(SCE_SH_HERE_Q);
				}
			} else if (HereDoc.DelimiterLength == 0) {
				// no delimiter, illegal (but '' and "" are legal)
				sc.ChangeState(SCE_SH_ERROR);
				sc.SetState(SCE_SH_DEFAULT);
			} else {
				sc.SetState(SCE_SH_HERE_Q);
			}
		}

		// update cmdState about the current command segment
		if (stylePrev != SCE_SH_DEFAULT && sc.state == SCE_SH_DEFAULT) {
			cmdState = cmdStateNew;
		}
		// Determine if a new state should be entered.
		if (sc.state == SCE_SH_DEFAULT) {
			if (sc.ch == '\\') {
				// Bash can escape any non-newline as a literal
				sc.SetState(SCE_SH_IDENTIFIER);
				if (IsEOLChar(sc.chNext)) {
					//sc.SetState(SCE_SH_OPERATOR);
					sc.SetState(SCE_SH_DEFAULT);
				}
			} else if (IsADigit(sc.ch)) {
				sc.SetState(SCE_SH_NUMBER);
				numBase = BASH_BASE_DECIMAL;
				if (sc.ch == '0') {	// hex, octal
					if (sc.chNext == 'x' || sc.chNext == 'X') {
						numBase = BASH_BASE_HEX;
						sc.Forward();
					} else if (IsADigit(sc.chNext)) {
#ifdef PEDANTIC_OCTAL
						numBase = BASH_BASE_OCTAL;
#else
						numBase = BASH_BASE_HEX;
#endif
					}
				}
			} else if (IsIdentifierStart(sc.ch)) {
				sc.SetState(SCE_SH_WORD);
			//} else if (sc.ch == '#' || (sc.ch == '/' && sc.chNext == '/')) {
			} else if (sc.ch == '#') {
				if (stylePrev != SCE_SH_WORD && stylePrev != SCE_SH_IDENTIFIER &&
					IsBashMetaCharacter(sc.chPrev)) {
					sc.SetState(SCE_SH_COMMENTLINE);
				} else {
					sc.SetState(SCE_SH_WORD);
				}
				// handle some zsh features within arithmetic expressions only
				if (cmdState == CmdState::Arithmetic) {
					if (sc.chPrev == '[') {	// [#8] [##8] output digit setting
						sc.SetState(SCE_SH_WORD);
						if (sc.chNext == '#') {
							sc.Forward();
						}
					} else if (sc.Match('#', '#', '^') && IsUpperCase(sc.GetRelative(3))) {	// ##^A
						sc.SetState(SCE_SH_IDENTIFIER);
						sc.Advance(3);
					} else if (sc.chNext == '#' && !IsASpace(sc.GetRelative(2))) {	// ##a
						sc.SetState(SCE_SH_IDENTIFIER);
						sc.Advance(2);
					} else if (IsIdentifierStart(sc.chNext)) {	// #name
						sc.SetState(SCE_SH_IDENTIFIER);
					}
				}
			} else if (sc.ch == '\"') {
				QuoteStack.Start(sc.ch, QuoteStyle::String, SCE_SH_DEFAULT);
				sc.SetState(SCE_SH_STRING_DQ);
			} else if (sc.ch == '\'') {
				QuoteStack.State = SCE_SH_DEFAULT;
				sc.SetState(SCE_SH_STRING_SQ);
			} else if (sc.ch == '`') {
				QuoteStack.Start(sc.ch, QuoteStyle::Backtick, SCE_SH_DEFAULT);
				sc.SetState(SCE_SH_BACKTICKS);
			} else if (sc.ch == '$') {
				QuoteStack.Expand(sc, cmdState);
				continue;
			} else if (cmdState != CmdState::Arithmetic && sc.Match('<', '<')) {
				sc.SetState(SCE_SH_HERE_DELIM);
				HereDoc.State = 0;
				if (sc.GetRelative(2) == '-') {	// <<- indent case
					HereDoc.Indent = true;
					sc.Forward();
				} else {
					HereDoc.Indent = false;
				}
			} else if (sc.ch == '-'	&&	// one-char file test operators
				IsBashSingleCharOperator(sc.chNext) &&
				!IsBashWordChar(sc.GetRelative(2)) &&
				IsASpace(sc.chPrev)) {
				sc.SetState(SCE_SH_WORD);
				sc.Forward();
			} else if (IsBashOperatorLast(sc.ch)) {
				sc.SetState(SCE_SH_OPERATOR);
				// arithmetic expansion and command substitution
				if (QuoteStack.Current.Style >= QuoteStyle::Command) {
					if (sc.ch == QuoteStack.Current.Down) {
						if (QuoteStack.CountDown(sc, cmdState)) {
							continue;
						}
					} else if (sc.ch == QuoteStack.Current.Up) {
						QuoteStack.Current.Count++;
					}
				}
				// globs have no whitespace, do not appear in arithmetic expressions
				if (cmdState != CmdState::Arithmetic && sc.ch == '(' && sc.chNext != '(') {
					const int i = GlobScan(sc);
					if (i > 1) {
						sc.SetState(SCE_SH_IDENTIFIER);
						sc.Forward(i + 1);
						continue;
					}
				}
				// handle opening delimiters for test/arithmetic expressions - ((,[[,[
				if (cmdState == CmdState::Start || cmdState == CmdState::Body) {
					if (sc.Match('(', '(')) {
						cmdState = CmdState::Arithmetic;
						sc.Forward();
					} else if (sc.Match('[', '[') && IsASpace(sc.GetRelative(2))) {
						cmdState = CmdState::Test;
						testExprType = TestExprType::DoubleBracket;
						sc.Forward();
					} else if (sc.ch == '[' && IsASpace(sc.chNext)) {
						cmdState = CmdState::Test;
						testExprType = TestExprType::SingleBracket;
					}
				}
				// special state -- for ((x;y;z)) in ... looping
				if (cmdState == CmdState::Word && sc.Match('(', '(')) {
					cmdState = CmdState::Arithmetic;
					sc.Forward(2);
					continue;
				}
				// handle command delimiters in command START|BODY|WORD state, also TEST if 'test'
				if (cmdState == CmdState::Start
					|| cmdState == CmdState::Body
					|| cmdState == CmdState::Word
					|| (cmdState == CmdState::Test && testExprType == TestExprType::Test)) {
					bool isCmdDelim = false;
					if (IsBashOperator(sc.chNext)) {
						isCmdDelim = IsBashCmdDelimiter(sc.ch, sc.chNext);
						if (isCmdDelim) {
							sc.Forward();
						}
					}
					if (!isCmdDelim) {
						isCmdDelim = IsBashCmdDelimiter(sc.ch, 0);
					}
					if (isCmdDelim) {
						cmdState = CmdState::Delimiter;
						sc.Forward();
						continue;
					}
				}
				// handle closing delimiters for test/arithmetic expressions - )),]],]
				if (cmdState == CmdState::Arithmetic && sc.Match(')', ')')) {
					cmdState = CmdState::Body;
					sc.Forward();
				} else if (cmdState == CmdState::Test && IsASpace(sc.chPrev)) {
					if (sc.Match(']', ']') && testExprType == TestExprType::DoubleBracket) {
						sc.Forward();
						cmdState = CmdState::Body;
					} else if (sc.ch == ']' && testExprType == TestExprType::SingleBracket) {
						cmdState = CmdState::Body;
					}
				}
			}
		}// sc.state

		sc.Forward();
	}
	if (sc.state == SCE_SH_HERE_Q) {
		styler.ChangeLexerState(sc.currentPos, styler.Length());
	}
	sc.Complete();
}

#define IsCommentLine(line)	IsLexCommentLine(styler, line, SCE_SH_COMMENTLINE)

void FoldBashDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList /*keywordLists*/, Accessor &styler) {
	const bool isCShell = styler.GetPropertyBool("lexer.lang");

	const Sci_PositionU endPos = startPos + length;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	lineStartNext = sci::min(lineStartNext, endPos);
	int levelPrev = styler.LevelAt(lineCurrent) & SC_FOLDLEVELNUMBERMASK;
	int levelCurrent = levelPrev;
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;

	char word[8]; // foreach
	constexpr int MaxFoldWordLength = sizeof(word) - 1;
	int wordlen = 0;

	while (startPos < endPos) {
		const char ch = styler[startPos];
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(++startPos);

		switch (style) {
		case SCE_SH_WORD:
			if (wordlen < MaxFoldWordLength) {
				word[wordlen++] = ch;
			}
			if (styleNext != SCE_SH_WORD) {
				word[wordlen] = '\0';
				wordlen = 0;
				if (isCShell) {
					if (StrEqualsAny(word, "if", "foreach", "switch", "while")) {
						levelCurrent++;
					} else if (StrEqualsAny(word, "end", "endif", "endsw")) {
						levelCurrent--;
					}
				} else {
					if (StrEqualsAny(word, "if", "case", "do")) {
						levelCurrent++;
					} else if (StrEqualsAny(word, "fi", "esac", "done")) {
						levelCurrent--;
					}
				}
			}
			break;

		case SCE_SH_OPERATOR:
			if (ch == '{' || ch == '[') {
				levelCurrent++;
			} else if (ch == '}' || ch == ']') {
				levelCurrent--;
			}
			break;

		case SCE_SH_HERE_DELIM:
			if (stylePrev != SCE_SH_HERE_DELIM) {
				if (ch == '<' && styler[startPos + 1] != '<') {
					levelCurrent++;
				}
			}
			break;

		case SCE_SH_HERE_Q:
			if (style == SCE_SH_HERE_Q && styleNext == SCE_SH_DEFAULT) {
				levelCurrent--;
			}
			break;
		}

		if (startPos == lineStartNext) {
			// Comment folding
			if (IsCommentLine(lineCurrent)) {
				levelCurrent += IsCommentLine(lineCurrent + 1) - IsCommentLine(lineCurrent - 1);
			}

			int lev = levelPrev;
			if ((levelCurrent > levelPrev)) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}
			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
			lineStartNext = sci::min(lineStartNext, endPos);
			levelPrev = levelCurrent;
		}
	}
}

}

LexerModule lmBash(SCLEX_BASH, ColouriseBashDoc, "bash", FoldBashDoc);
