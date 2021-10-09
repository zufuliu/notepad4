// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for AutoHotkey.

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
#include "StringUtils.h"

using namespace Lexilla;

namespace {

enum {
	AHKLineStateMaskLineComment = 1, // line comment
	AHKLineStateMaskSectionComment = 2, // section comment
};

enum class AHKSectionState {
	None = 0,
	InitQuote,
	InitSpace,
	Header,
	Section,
};

enum AHKSectionOption {
	AHKSectionOption_None = 0,
	AHKSectionOption_Accent = 1,
	AHKSectionOption_Percent = 2,	// v1
	AHKSectionOption_Comma = 4,		// v1
	AHKSectionOption_Comment = 8,
};

constexpr bool IsAHKNumber(int base, int chPrev, int ch, int chNext) noexcept {
	return IsADigit(ch)
		|| ((base == 16) ? IsHexDigit(ch) : IsNumberContinue(chPrev, ch, chNext));
}

constexpr bool IsHotStringOptionChar(int ch) noexcept {
	// https://lexikos.github.io/v2/docs/Hotstrings.htm#Options
	return IsAlphaNumeric(ch) || ch == '*' || ch == '?' || ch == ' ';
}

constexpr unsigned char Digit(unsigned char ch) noexcept {
	return ch - '0';
}

constexpr int Digit(unsigned char ch, unsigned char chNext) noexcept {
	return Digit(ch)*10 + Digit(chNext);
}

inline bool IsSpecialKey(const char (&buffer)[128], size_t length) noexcept {
	switch (length) {
	case 2: // F1 - F9
		if (buffer[0] == 'f' && Between(buffer[1], '1', '9')) {
			return true;
		}
		break;
	case 3: // F10 - F24
		if (buffer[0] == 'f' && Between(Digit(buffer[1], buffer[2]), 10, 24)) {
			return true;
		}
		break;
	case 4: // VKnn, Joy1 - Joy9
		if (StrStartsWith(buffer, "vk")) {
			if (IsHexDigit(buffer[2]) && IsHexDigit(buffer[3])) {
				return true;
			}
		} else if (StrStartsWith(buffer, "joy")) {
			if (Between(buffer[3], '1', '9')) {
				return true;
			}
		}
		break;
	case 5: // SCnnn, Joy10 - Joy32
		if (StrStartsWith(buffer, "sc")) {
			if (IsHexDigit(buffer[2]) && IsHexDigit(buffer[3]) && IsHexDigit(buffer[4])) {
				return true;
			}
		} else if (StrStartsWith(buffer, "joy")) {
			if (Between(Digit(buffer[3], buffer[4]), 10, 32)) {
				return true;
			}
		}
		break;
	}
	return false;
}

inline bool MaybeStartsQuotedSection(const StyleContext &sc) noexcept {
	int chNext = sc.GetLineNextChar(true);
	if (chNext == '\0' || (chNext == ';' && chNext != sc.chNext)) {
		chNext = LexGetNextChar(sc.lineStartNext, sc.styler);
		return chNext == '(' || chNext == ';';
	}
	return false;
}

constexpr bool IsSectionOptionStart(int ch) noexcept {
	return AnyOf(ch, 0, 'J', 'j', 'L', 'l', 'R', 'r', 'C', 'c', ';', '`', '%', ',');
}

inline bool IsSectionOptionStart(const StyleContext &sc) noexcept {
	const int chNext = sc.GetLineNextChar(true);
	return IsSectionOptionStart(chNext);
}

constexpr bool IsSectionCommentOption(const char (&buffer)[128], size_t length) noexcept {
	constexpr size_t comment = CStrLen("comments");
	return length <= comment && __builtin_memcmp(buffer, "comments", length) == 0;
}

// https://lexikos.github.io/v2/docs/commands/Format.htm
constexpr bool IsFormatSpecifier(char ch) noexcept {
	return AnyOf(ch, 'a', 'A',
					'c',
					'd',
					'e', 'E',
					'f',
					'g', 'G',
					'i',
					'o',
					'p',
					's',
					'u',
					'x', 'X');
}

inline Sci_Position CheckFormatSpecifier(const StyleContext &sc, LexAccessor &styler) noexcept {
	if (sc.chNext == '}') {
		return 2;
	}
	if (!(IsADigit(sc.chNext) || sc.chNext == ':')) {
		return 0;
	}

	Sci_PositionU pos = sc.currentPos + 1;
	char ch = styler.SafeGetCharAt(pos);
	// index
	while (IsADigit(ch)) {
		ch = styler.SafeGetCharAt(++pos);
	}
	if (ch == ':') {
		// Flags
		ch = styler.SafeGetCharAt(++pos);
		while (AnyOf(ch, '-', '+', '0', ' ', '#')) {
			ch = styler.SafeGetCharAt(++pos);
		}
		// Width
		while (IsADigit(ch)) {
			ch = styler.SafeGetCharAt(++pos);
		}
		// .Precision
		if (ch == '.') {
			ch = styler.SafeGetCharAt(++pos);
			while (IsADigit(ch)) {
				ch = styler.SafeGetCharAt(++pos);
			}
		}
		// ULT
		if (AnyOf(ch, 'U', 'L', 'T', 'l', 't')) {
			ch = styler.SafeGetCharAt(++pos);
		}
		// Type
		if (IsFormatSpecifier(ch)) {
			ch = styler.SafeGetCharAt(++pos);
		}
	}
	if (ch == '}') {
		return pos - sc.currentPos + 1;
	}
	return 0;
}

bool HighlightBrace(StyleContext &sc, int &outerStyle) {
	const int state = sc.state;
	if (sc.ch == '{' && AnyOf(sc.chNext, '!', '#', '+', '^', '{', '}') && sc.GetRelative(2) == '}') {
		// https://lexikos.github.io/v2/docs/commands/Send.htm
		sc.SetState(SCE_AHK_SENTKEY);
		sc.Advance(2);
		sc.ForwardSetState(state);
		return true;
	}
	if (sc.ch == sc.chNext) {
		sc.SetState(SCE_AHK_ESCAPECHAR);
		sc.Forward();
		sc.ForwardSetState(state);
		return true;
	}
	if (sc.ch == '{') {
		if (state != SCE_AHK_HOTSTRING_VALUE) {
			const Sci_Position length = CheckFormatSpecifier(sc, sc.styler);
			if (length != 0) {
				sc.SetState(SCE_AHK_FORMAT_SPECIFIER);
				sc.Advance(length);
				sc.SetState(state);
				return true;
			}
		}
		if (IsAlpha(sc.chNext)) {
			outerStyle = state;
			sc.SetState(SCE_AHK_SENTKEY);
			sc.Forward();
			return true;
		}
	}
	return false;
}

inline bool ScanHotKey(const StyleContext &sc, LexAccessor &styler, Sci_PositionU &length) noexcept {
	char chPrev = '\0';
	int count = 1;
	int modifier = 1;
	// TODO: improve this code
	Sci_PositionU pos = sc.currentPos;
	while (pos < sc.lineStartNext) {
		const char ch = styler[pos++];
		if (ch < ' ' || ch >= '\x7f') {
			return false;
		}
		if (ch == ' ') {
			if (!(chPrev == ' ' || chPrev == '&')) {
				++modifier;
				if (modifier == 3) {
					return false;
				}
			}
		} else if (ch == '&') {
			modifier = 1;
			++count;
			if (count == 3) {
				return false;
			}
		} else if (ch == ':' && chPrev == ':') {
			length = pos - sc.currentPos;
			return true;
		}
		chPrev = ch;
	}
	return false;
}

void ColouriseAHKDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int kwType = SCE_AHK_DEFAULT;
	int visibleChars = 0;
	int outerStyle = SCE_AHK_DEFAULT;
	int lineStateLineComment = 0;
	AHKSectionState sectionState = AHKSectionState::None;
	int sectionOption = AHKSectionOption_None;
	Sci_PositionU backPos = 0;
	int stringQuoteChar = 0;
	bool hotString = false;

	if (startPos != 0) {
		constexpr int mask = AHKLineStateMaskSectionComment | (static_cast<int>(AHKSectionState::Header) << 1);
		BacktrackToStart(styler, mask, startPos, lengthDoc, initStyle);
	}
	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		2: lineStateLineComment
		3: section state
		4: section option
		*/
		sectionState = static_cast<AHKSectionState>((lineState >> 2) & 7);
		sectionOption = (lineState >> 5) & 15;
	}

	while (sc.More()) {
		if (sc.atLineStart) {
			switch (sectionState) {
			case AHKSectionState::InitQuote:
				sectionState = AHKSectionState::InitSpace;
				break;
			case AHKSectionState::Header:
				sectionState = AHKSectionState::Section;
				break;
			default:
				break;
			}
		}

		switch (sc.state) {
		case SCE_AHK_OPERATOR:
			sc.SetState(SCE_AHK_DEFAULT);
			break;

		case SCE_AHK_NUMBER:
			if (!IsAHKNumber(stringQuoteChar, sc.chPrev, sc.ch, sc.chNext)) {
				if (IsIdentifierCharEx(sc.ch)) {
					sc.ChangeState(SCE_AHK_IDENTIFIER);
				} else {
					sc.SetState(SCE_AHK_DEFAULT);
				}
			}
			break;

		case SCE_AHK_IDENTIFIER:
		case SCE_AHK_DYNAMIC_VARIABLE:
		case SCE_AHK_DIRECTIVE_SHARP:
		case SCE_AHK_SECTION_OPTION:
			if (!IsIdentifierCharEx(sc.ch)) {
				if (sc.state == SCE_AHK_DYNAMIC_VARIABLE) {
					if (sc.ch == '%') {
						sc.Forward();
					}
					sc.SetState(outerStyle);
					continue;
				}

				char s[128];
				sc.GetCurrentLowered(s, sizeof(s));
				if (sc.state == SCE_AHK_IDENTIFIER) {
					const int chNext = sc.GetLineNextChar();
					if (keywordLists[0]->InList(s) || (kwType == SCE_AHK_KEYWORD && StrEqualsAny(s, "files", "parse", "read", "reg"))) {
						sc.ChangeState(SCE_AHK_KEYWORD);
						kwType = SCE_AHK_DEFAULT;
						if (StrEqual(s, "loop")) {
							if (AnyOf(chNext, 'F', 'f', 'P', 'p', 'R', 'r')) {
								kwType = SCE_AHK_KEYWORD;
							}
						} else if (StrEqual(s, "class")) {
							if (IsIdentifierStartEx(chNext)) {
								kwType = SCE_AHK_CLASS;
							}
						}
					} else if (kwType == SCE_AHK_CLASS || keywordLists[3]->InList(s)) {
						sc.ChangeState(SCE_AHK_CLASS);
					} else if (keywordLists[4]->InList(s)) {
						sc.ChangeState(SCE_AHK_BUILTIN_VARIABLE);
					} else if (keywordLists[6]->InListPrefixed(s, '(')) {
						sc.ChangeState(SCE_AHK_BUILTIN_FUNCTION);
					} else if (chNext == '(') {
						sc.ChangeState(SCE_AHK_FUNCTION);
					} else if (keywordLists[5]->InList(s) || IsSpecialKey(s, sc.LengthCurrent())) {
						sc.ChangeState(SCE_AHK_KEY);
					} else if (sc.ch == ':') {
						if (sc.chNext != '=' && visibleChars == sc.LengthCurrent()) {
							sc.ChangeState(SCE_AHK_LABEL);
							sc.Forward();
						}
					}
					if (sc.state != SCE_AHK_KEYWORD) {
						kwType = SCE_AHK_DEFAULT;
					}
					sc.SetState(SCE_AHK_DEFAULT);
				} else if (sc.state == SCE_AHK_SECTION_OPTION) {
					if (StrStartsWith(s, "join")) {
						while (IsGraphic(sc.ch)) {
							if (sc.ch == '`' && !IsEOLChar(sc.chNext)) {
								sc.SetState(SCE_AHK_ESCAPECHAR);
								sc.Forward();
								sc.ForwardSetState(SCE_AHK_SECTION_OPTION);
							} else {
								sc.Forward();
							}
						}
					} else if (s[0] == 'c' && IsSectionCommentOption(s, sc.LengthCurrent())) {
						sectionOption |= AHKSectionOption_Comment;
					} else if ((sectionOption & (AHKSectionOption_Comment - 1)) == 0) {
						if (!(StrStartsWith(s, "ltrim") || StrStartsWith(s, "rtrim"))) {
							// unknown option
							sectionState = AHKSectionState::None;
							sectionOption = AHKSectionOption_None;
							sc.BackTo(backPos);
							sc.ChangeState(SCE_AHK_OPERATOR);
							break;
						}
					}
					sc.SetState(outerStyle);
					continue;
				} else {
					hotString = StrEqual(s, "#hotstring");
					sc.SetState(SCE_AHK_DEFAULT);
				}
			}
			break;

		case SCE_AHK_DIRECTIVE_AT:
			if (!(IsIdentifierChar(sc.ch) || sc.ch == '-')) {
				char s[12];
				sc.GetCurrentLowered(s, sizeof(s));
				if (StrStartsWith(s, "@ahk2exe-")) {
					sc.SetState(outerStyle);
				} else {
					sc.ChangeState(outerStyle);
				}
				continue;
			}
			break;

		case SCE_AHK_STRING_SQ:
		case SCE_AHK_STRING_DQ:
			if (sc.ch == '`' && !IsEOLChar(sc.chNext)) {
				const int state = sc.state;
				sc.SetState(SCE_AHK_ESCAPECHAR);
				sc.Forward();
				sc.ForwardSetState(state);
				continue;
			}
			if (sc.ch == '%' && IsIdentifierCharEx(sc.chNext)) {
				outerStyle = sc.state;
				sc.SetState(SCE_AHK_DYNAMIC_VARIABLE);
			} else if (sc.ch == '{' || sc.ch == '}') {
				if (HighlightBrace(sc, outerStyle)) {
					continue;
				}
			} else if (sc.ch == stringQuoteChar) {
				if (sc.ch == sc.chNext) {
					const int state = sc.state;
					sc.SetState(SCE_AHK_ESCAPECHAR);
					sc.Forward();
					sc.ForwardSetState(state);
					continue;
				}
				sc.ForwardSetState(SCE_AHK_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_AHK_DEFAULT);
			}
			break;

		case SCE_AHK_SENTKEY:
			if (!(IsIdentifierChar(sc.ch) || sc.ch == ' ')) {
				if (sc.ch == '+' && (sc.chPrev == 'U' || sc.chPrev == 'u')) {
					sc.Forward();
				} else if (sc.ch == '}') {
					sc.ForwardSetState(outerStyle);
					continue;
				} else {
					sc.Rewind();
					sc.ChangeState(outerStyle);
				}
			}
			break;

		case SCE_AHK_HOTSTRING_OPTION:
			if (!IsHotStringOptionChar(sc.ch)) {
				if (sc.ch == ':' && IsGraphic(sc.chNext)) {
					sc.ForwardSetState(SCE_AHK_HOTSTRING_KEY);
				} else {
					sc.Rewind();
					sc.ChangeState(SCE_AHK_OPERATOR);
				}
			}
			break;

		case SCE_AHK_HOTSTRING_KEY:
			if (sc.ch == '`' && !IsEOLChar(sc.chNext)) {
				sc.SetState(SCE_AHK_ESCAPECHAR);
				sc.Forward();
				sc.ForwardSetState(SCE_AHK_HOTSTRING_KEY);
				continue;
			}
			if (sc.Match(':', ':')) {
				hotString = true;
				sc.SetState(SCE_AHK_HOTSTRING_OPTION);
				sc.Forward(2);
				sc.SetState(SCE_AHK_DEFAULT);
			} else if (!IsGraphic(sc.ch)) {
				sc.Rewind();
				sc.ChangeState(SCE_AHK_DEFAULT);
			}
			break;

		case SCE_AHK_HOTSTRING_VALUE:
			if (sc.ch == '`' && !IsEOLChar(sc.chNext)) {
				sc.SetState(SCE_AHK_ESCAPECHAR);
				sc.Forward();
				sc.ForwardSetState(SCE_AHK_HOTSTRING_VALUE);
				continue;
			}
			if (sc.ch == '{' || sc.ch == '}') {
				if (HighlightBrace(sc, outerStyle)) {
					continue;
				}
			} else if (IsASpaceOrTab(sc.ch)) {
				const int chNext = sc.GetLineNextChar(true);
				if (chNext == ';' || chNext == '\0') {
					sc.SetState(SCE_AHK_DEFAULT);
				}
			} else if (sc.atLineStart) {
				sc.SetState(SCE_AHK_DEFAULT);
			}
			break;

		// Continuation section
		case SCE_AHK_SECTION_SQ:
		case SCE_AHK_SECTION_DQ:
		case SCE_AHK_SECTION_NQ:
			switch (sectionState) {
			case AHKSectionState::InitQuote:
				if (sc.ch == ';' && IsASpaceOrTab(sc.chPrev)) {
					outerStyle = sc.state;
					lineStateLineComment = AHKLineStateMaskSectionComment;
					sc.SetState(SCE_AHK_SECTION_COMMENT);
				}
				break;

			case AHKSectionState::InitSpace:
				if (visibleChars == 0) {
					if (sc.ch == ';') {
						outerStyle = sc.state;
						lineStateLineComment = AHKLineStateMaskSectionComment;
						sc.SetState(SCE_AHK_SECTION_COMMENT);
					} else if (sc.ch == '(' && IsSectionOptionStart(sc)) {
						sectionState = AHKSectionState::Header;
						sectionOption = AHKSectionOption_None;
						backPos = sc.currentPos;
					} else if (!isspacechar(sc.ch)) {
						sc.ChangeState(SCE_AHK_DEFAULT);
					}
				}
				break;

			case AHKSectionState::Header:
				switch (sc.ch) {
				case '`':
					sectionOption |= AHKSectionOption_Accent;
					break;
				case '%':
					sectionOption |= AHKSectionOption_Percent;
					break;
				case ',':
					sectionOption |= AHKSectionOption_Comma;
					break;
				default:
					if (sc.ch == ';' && IsASpaceOrTab(sc.chPrev)) {
						outerStyle = sc.state;
						lineStateLineComment = AHKLineStateMaskSectionComment;
						sc.SetState(SCE_AHK_SECTION_COMMENT);
					} else if (IsAlpha(sc.ch)) {
						outerStyle = sc.state;
						sc.SetState(SCE_AHK_SECTION_OPTION);
					} else if ((sectionOption & (AHKSectionOption_Comment - 1)) == 0) {
						if (!isspacechar(sc.ch)) {
							// unknown option
							sectionState = AHKSectionState::None;
							sectionOption = AHKSectionOption_None;
							sc.BackTo(backPos);
							sc.ChangeState(SCE_AHK_OPERATOR);
						}
					}
					break;
				}
				break;

			case AHKSectionState::Section:
				switch (sc.ch) {
				case ')':
					if (visibleChars == 0) {
						if ((sc.state == SCE_AHK_SECTION_SQ && sc.chNext == '\'')
							|| (sc.state == SCE_AHK_SECTION_DQ && sc.chNext == '\"')) {
							sc.Forward();
						}
						sc.ForwardSetState(SCE_AHK_DEFAULT);
					}
					break;

				case '%':
					if ((sectionOption & AHKSectionOption_Percent) == AHKSectionOption_None) {
						if (IsIdentifierStartEx(sc.chNext)) {
							outerStyle = sc.state;
							sc.SetState(SCE_AHK_DYNAMIC_VARIABLE);
						}
					}
					break;

				case ';':
					if ((sectionOption & AHKSectionOption_Comment) != AHKSectionOption_None) {
						if (visibleChars == 0 || IsASpaceOrTab(sc.chPrev)) {
							outerStyle = sc.state;
							lineStateLineComment = AHKLineStateMaskSectionComment;
							sc.SetState(SCE_AHK_SECTION_COMMENT);
						}
					}
					break;

				case '`':
					if ((sectionOption & AHKSectionOption_Comment) == AHKSectionOption_None) {
						if (!IsEOLChar(sc.chNext)) {
							const int state = sc.state;
							sc.SetState(SCE_AHK_ESCAPECHAR);
							sc.Forward();
							sc.ForwardSetState(state);
							continue;
						}
					}
					break;

				case '{':
				case '}':
					if (HighlightBrace(sc, outerStyle)) {
						continue;
					}
					break;
				}
				break;
			default:
				break;
			}
			break;

		case SCE_AHK_SECTION_COMMENT:
			if (sectionState == AHKSectionState::InitQuote) {
				if (sc.ch == '`') {
					sc.Forward();
				} else if (sc.ch == stringQuoteChar) {
					sectionState = AHKSectionState::None;
					sectionOption = AHKSectionOption_None;
					sc.BackTo(backPos);
					sc.ChangeState((stringQuoteChar == '\'') ? SCE_AHK_STRING_SQ : SCE_AHK_STRING_DQ);
					break;
				}
			}
			if (sc.atLineStart) {
				sc.SetState(outerStyle);
				continue;
			}
			break;

		case SCE_AHK_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_AHK_DEFAULT);
			}
			break;

		case SCE_AHK_COMMENTBLOCK:
			if (sc.Match('*', '/') && (visibleChars == 0 || LexGetNextChar(sc.currentPos + 2, sc.lineStartNext, styler) == 0)) {
				sc.Forward();
				sc.ForwardSetState(SCE_AHK_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_AHK_DEFAULT) {
			if (hotString && !(isspacechar(sc.ch) || (sc.ch == ';' && IsASpaceOrTab(sc.chPrev)))) {
				hotString = false;
				sc.SetState(SCE_AHK_HOTSTRING_VALUE);
			} else if (sc.ch == ';') {
				if (visibleChars == 0 || IsASpaceOrTab(sc.chPrev)) {
					if (visibleChars == 0) {
						lineStateLineComment = AHKLineStateMaskLineComment;
					}
					sc.SetState(SCE_AHK_COMMENTLINE);
					if (visibleChars == 0 && sc.chNext == '@' && (sc.GetRelative(2) | 0x20) == 'a') {
						outerStyle = SCE_AHK_COMMENTLINE;
						sc.ForwardSetState(SCE_AHK_DIRECTIVE_AT);
					}
				}
			} else if (sc.Match('/', '*') && visibleChars == 0) {
				sc.SetState(SCE_AHK_COMMENTBLOCK);
				sc.Forward();
				if (sc.chNext == '@' && (sc.GetRelative(2) | 0x20) == 'a') {
					outerStyle = SCE_AHK_COMMENTBLOCK;
					sc.ForwardSetState(SCE_AHK_DIRECTIVE_AT);
				}
			} else if (sc.ch == '\"' || sc.ch == '\'') {
				stringQuoteChar = sc.ch;
				int state;
				if (MaybeStartsQuotedSection(sc)) {
					sectionState = AHKSectionState::InitQuote;
					sectionOption = AHKSectionOption_None;
					backPos = sc.currentPos;
					state = (stringQuoteChar == '\'') ? SCE_AHK_SECTION_SQ : SCE_AHK_SECTION_DQ;
				} else {
					state = (stringQuoteChar == '\'') ? SCE_AHK_STRING_SQ : SCE_AHK_STRING_DQ;
				}
				sc.SetState(state);
			} else if (sc.ch == ':') {
				sc.SetState((sc.chNext == ':' || IsHotStringOptionChar(sc.chNext)) ? SCE_AHK_HOTSTRING_OPTION : SCE_AHK_OPERATOR);
			} else if (visibleChars == 0 && IsAGraphic(sc.ch) && ScanHotKey(sc, styler, backPos)) {
				sc.SetState(SCE_AHK_HOTKEY);
				sc.Advance(backPos);
				sc.SetState(SCE_AHK_DEFAULT);
				continue;
			} else if (sc.ch == '0' && (sc.chNext == 'x' || sc.chNext == 'X')) {
				stringQuoteChar = 16;
				sc.SetState(SCE_AHK_NUMBER);
				sc.Forward();
			} else if (IsADigit(sc.ch)) {
				stringQuoteChar = 10;
				sc.SetState(SCE_AHK_NUMBER);
			} else if (IsIdentifierStartEx(sc.ch)) {
				sc.SetState(SCE_AHK_IDENTIFIER);
			} else if (sc.ch == '%' && IsIdentifierCharEx(sc.chNext)) {
				outerStyle = SCE_AHK_DEFAULT;
				sc.SetState(SCE_AHK_DYNAMIC_VARIABLE);
			} else if (sc.ch == '#' && visibleChars == 0 && IsAlpha(sc.chNext)) {
				sc.SetState(SCE_AHK_DIRECTIVE_SHARP);
			} else if (sc.ch == '`' && !IsEOLChar(sc.chNext)) {
				// v1 legacy unquoted text
				sc.SetState(SCE_AHK_ESCAPECHAR);
				sc.Forward();
				sc.ForwardSetState(SCE_AHK_DEFAULT);
				continue;
			} else if (isoperator(sc.ch)) {
				int state;
				if (sc.ch == '(' && visibleChars == 0 && IsSectionOptionStart(sc)) {
					sectionState = AHKSectionState::Header;
					sectionOption = AHKSectionOption_None;
					backPos = sc.currentPos;
					state = SCE_AHK_SECTION_NQ;
				} else {
					state = SCE_AHK_OPERATOR;
				}
				sc.SetState(state);
			}
		}

		if (!isspacechar(sc.ch)) {
			++visibleChars;
		}
		if (sc.atLineEnd) {
			const int lineState = lineStateLineComment
				| (static_cast<int>(sectionState) << 2)
				| (sectionOption << 5);
			styler.SetLineState(sc.currentLine, lineState);
			visibleChars = 0;
			stringQuoteChar = 0;
			lineStateLineComment = 0;
			hotString = false;
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_AHK_TASKMARKER;
}

constexpr int GetLineCommentState(int lineState) noexcept {
	return lineState & AHKLineStateMaskLineComment;
}

constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_AHK_COMMENTBLOCK
		|| style == SCE_AHK_DIRECTIVE_AT
		|| style == SCE_AHK_TASKMARKER;
}

constexpr bool IsMultilineStringStyle(int style) noexcept {
	return style == SCE_AHK_SECTION_SQ
		|| style == SCE_AHK_SECTION_DQ
		|| style == SCE_AHK_SECTION_NQ
		|| style == SCE_AHK_SECTION_COMMENT
		|| style == SCE_AHK_SECTION_OPTION
		|| style == SCE_AHK_ESCAPECHAR
		|| style == SCE_AHK_FORMAT_SPECIFIER
		|| style == SCE_AHK_SENTKEY
		|| style == SCE_AHK_DYNAMIC_VARIABLE;
}

void FoldAHKDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList /*keywordLists*/, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = GetLineCommentState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_AHK_OPERATOR, SCE_AHK_TASKMARKER);
		if (bracePos) {
			startPos = bracePos + 1; // skip the brace
		}
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = GetLineCommentState(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;
	int visibleChars = 0;

	int style = initStyle;
	int styleNext = styler.StyleAt(startPos);

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		switch (style) {
		case SCE_AHK_COMMENTBLOCK:
			if (!IsStreamCommentStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamCommentStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_AHK_SECTION_SQ:
		case SCE_AHK_SECTION_DQ:
		case SCE_AHK_SECTION_NQ:
			if (!IsMultilineStringStyle(stylePrev)) {
				levelNext++;
			} else if (!IsMultilineStringStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_AHK_OPERATOR: {
			const char ch = styler[i];
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		} break;
		}

		if (visibleChars == 0 && !IsSpaceEquiv(style)) {
			++visibleChars;
		}
		if (i == lineEndPos) {
			const int lineCommentNext = GetLineCommentState(styler.GetLineState(lineCurrent + 1));
			if (lineCommentCurrent) {
				levelNext += lineCommentNext - lineCommentPrev;
			} else if (visibleChars) {
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_AHK_OPERATOR, SCE_AHK_TASKMARKER);
				if (bracePos) {
					levelNext++;
					i = bracePos; // skip the brace
					style = SCE_AHK_OPERATOR;
					styleNext = styler.StyleAt(i + 1);
				}
			}

			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}

			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
			lineEndPos = sci::min(lineStartNext, endPos) - 1;
			levelCurrent = levelNext;
			lineCommentPrev = lineCommentCurrent;
			lineCommentCurrent = lineCommentNext;
			visibleChars = 0;
		}
	}
}

}

LexerModule lmAHK(SCLEX_AHK, ColouriseAHKDoc, "ahk", FoldAHKDoc);
