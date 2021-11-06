// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for C#, Vala.

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

using namespace Lexilla;

namespace {

constexpr bool IsVerbatimString(int state) noexcept {
	return state == SCE_CSHARP_VERBATIM_STRING
		|| state == SCE_CSHARP_INTERPOLATED_VERBATIM_STRING;
}

constexpr bool IsInterpolatedString(int state) noexcept {
	return state == SCE_CSHARP_INTERPOLATED_STRING
		|| state == SCE_CSHARP_INTERPOLATED_VERBATIM_STRING;
}

struct EscapeSequence {
	int outerState = SCE_CSHARP_DEFAULT;
	int digitsLeft = 0;

	// highlight any character as escape sequence.
	bool resetEscapeState(int state, int chNext) noexcept {
		if (IsEOLChar(chNext)) {
			return false;
		}
		outerState = state;
		digitsLeft = 1;
		if (chNext == 'x' || chNext == 'u') {
			digitsLeft = 5;
		} else if (chNext == 'U') {
			digitsLeft = 9;
		}
		return true;
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

struct InterpolatedStringState {
	int state;
	int parenCount;
};

enum {
	CSharpLineStateMaskLineComment = 1,
	CSharpLineStateMaskUsing = 1 << 1,
	CSharpLineStateMaskInterpolation = 1 << 2,
};

enum class PreprocessorKind {
	None,
	Init,
	Pragma,
	Message,
	Other,
};

enum class DocTagState {
	None,
	XmlOpen,
	XmlClose,
};

enum class KeywordType {
	None = SCE_CSHARP_DEFAULT,
	Attribute = SCE_CSHARP_ATTRIBUTE,
	Class = SCE_CSHARP_CLASS,
	Struct = SCE_CSHARP_STRUCT,
	Interface = SCE_CSHARP_INTERFACE,
	Enum = SCE_CSHARP_ENUM,
	Record = SCE_CSHARP_RECORD,
	Label = SCE_CSHARP_LABEL,
	Return = 0x40,
	While,
};

constexpr bool IsUnicodeEscape(int ch, int chNext) noexcept {
	return ch == '\\' && (chNext | 0x20) == 'u';
}

constexpr bool IsCsIdentifierStart(int ch, int chNext) noexcept {
	return IsIdentifierStartEx(ch) || IsUnicodeEscape(ch, chNext);
}

constexpr bool IsCsIdentifierChar(int ch, int chNext) noexcept {
	return IsIdentifierCharEx(ch) || IsUnicodeEscape(ch, chNext);
}

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_CSHARP_TASKMARKER;
}

// https://docs.microsoft.com/en-us/dotnet/standard/base-types/composite-formatting
constexpr bool IsInvalidFormatSpecifier(int ch) noexcept {
	// Custom format strings allows any characters
	return AnyOf(ch, '\r', '\n', '\"', '}', '{');
}

inline Sci_Position CheckFormatSpecifier(const StyleContext &sc, LexAccessor &styler) noexcept {
	Sci_PositionU pos = sc.currentPos;
	char ch = styler.SafeGetCharAt(pos);
	// [,alignment]
	if (ch == ',') {
		ch = styler.SafeGetCharAt(++pos);
		if (ch == '-') {
			ch = styler.SafeGetCharAt(++pos);
		}
		while (IsADigit(ch)) {
			ch = styler.SafeGetCharAt(++pos);
		}
	}
	// [:formatString]
	if (ch == ':') {
		ch = styler.SafeGetCharAt(++pos);
		const Sci_PositionU endPos = pos + 32;
		while (pos < endPos && !IsInvalidFormatSpecifier(ch)) {
			ch = styler.SafeGetCharAt(++pos);
		}
	}
	if (ch == '}') {
		return pos - sc.currentPos;
	}
	return 0;
}

inline bool IsInterpolatedStringEnd(const StyleContext &sc) noexcept {
	return sc.ch == '}' || sc.ch == ':'
		|| (sc.ch == ',' && (IsADigit(sc.chNext) || (sc.chNext == '-' && IsADigit(sc.GetRelative(2)))));
}

void ColouriseCSharpDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineType = 0;

	KeywordType kwType = KeywordType::None;
	int chBeforeIdentifier = 0;
	int parenCount = 0;
	PreprocessorKind ppKind = PreprocessorKind::None;

	int visibleChars = 0;
	int chBefore = 0;
	int visibleCharsBefore = 0;
	int chPrevNonWhite = 0;
	DocTagState docTagState = DocTagState::None;
	EscapeSequence escSeq;

	std::vector<InterpolatedStringState> nestedState;

	if (startPos != 0) {
		// backtrack to the line starts expression inside interpolated string literal.
		BacktrackToStart(styler, CSharpLineStateMaskInterpolation, startPos, lengthDoc, initStyle);
	}

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		parenCount = lineState >> 4;
	}
	if (startPos == 0 && sc.Match('#', '!')) {
		// Shell Shebang at beginning of file
		sc.SetState(SCE_CSHARP_COMMENTLINE);
		sc.Forward();
		lineStateLineType = CSharpLineStateMaskLineComment;
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_CSHARP_OPERATOR:
		case SCE_CSHARP_OPERATOR2:
			sc.SetState(SCE_CSHARP_DEFAULT);
			break;

		case SCE_CSHARP_NUMBER:
			if (!IsDecimalNumber(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_CSHARP_DEFAULT);
			}
			break;

		case SCE_CSHARP_IDENTIFIER:
		case SCE_CSHARP_PREPROCESSOR:
			if (!IsCsIdentifierChar(sc.ch, sc.chNext)) {
				char s[128];
				sc.GetCurrent(s, sizeof(s));
				switch (ppKind) {
				case PreprocessorKind::None:
					if (s[0] != '@') {
						if (keywordLists[0]->InList(s)) {
							sc.ChangeState(SCE_CSHARP_WORD);
							if (StrEqual(s, "using")) {
								if (visibleChars == sc.LengthCurrent()) {
									lineStateLineType = CSharpLineStateMaskUsing;
								}
							} else if (StrEqualsAny(s, "class", "new", "as", "is")) {
								kwType = KeywordType::Class;
							} else if (StrEqual(s, "struct")) {
								kwType = KeywordType::Struct;
							} else if (StrEqual(s, "interface")) {
								kwType = KeywordType::Interface;
							} else if (StrEqual(s, "enum")) {
								kwType = KeywordType::Enum;
							} else if (StrEqual(s, "record")) {
								kwType = KeywordType::Record;
							} else if (StrEqual(s, "goto")) {
								kwType = KeywordType::Label;
							} else if (StrEqualsAny(s, "return", "await", "yield")) {
								kwType = KeywordType::Return;
							} else if (StrEqualsAny(s, "if", "while")) {
								// to avoid treating following code as type cast:
								// if (identifier) expression, while (identifier) expression
								kwType = KeywordType::While;
							}
							if (kwType > KeywordType::None && kwType < KeywordType::Return) {
								const int chNext = sc.GetDocNextChar();
								if (!IsIdentifierStartEx(chNext)) {
									kwType = KeywordType::None;
								}
							}
						} else if (keywordLists[1]->InList(s) || keywordLists[2]->InList(s)) {
							sc.ChangeState(SCE_CSHARP_WORD2);
						} else if (keywordLists[5]->InList(s)) {
							sc.ChangeState(SCE_CSHARP_CLASS);
						} else if (keywordLists[6]->InList(s)) {
							sc.ChangeState(SCE_CSHARP_STRUCT);
						} else if (keywordLists[7]->InList(s)) {
							sc.ChangeState(SCE_CSHARP_INTERFACE);
						} else if (keywordLists[8]->InList(s)) {
							sc.ChangeState(SCE_CSHARP_ENUM);
						} else if (keywordLists[4]->InList(s)) {
							sc.ChangeState(SCE_CSHARP_ATTRIBUTE);
						} else if (keywordLists[9]->InList(s)) {
							sc.ChangeState(SCE_CSHARP_CONSTANT);
						}
					}
					break;

				case PreprocessorKind::Init:
					if (sc.state == SCE_CSHARP_IDENTIFIER) {
						sc.ChangeState(SCE_CSHARP_PREPROCESSOR);
					}
					if (sc.LengthCurrent() > 1) {
						const char *p = s;
						if (*p == '#')  {
							++p;
						}
						if (StrEqualsAny(p, "pragma", "line", "nullable")) {
							ppKind = PreprocessorKind::Pragma;
						} else if (StrEqualsAny(p, "error", "warning", "region", "endregion")) {
							ppKind = PreprocessorKind::Message;
						} else {
							ppKind = PreprocessorKind::Other;
						}
					} else if (!IsASpaceOrTab(sc.ch)) {
						ppKind = PreprocessorKind::Other;
					}
					break;

				case PreprocessorKind::Pragma:
					ppKind = PreprocessorKind::Other;
					sc.ChangeState(SCE_CSHARP_PREPROCESSOR_WORD);
					break;

				default:
					break;
				}

				if (ppKind == PreprocessorKind::None && sc.state == SCE_CSHARP_IDENTIFIER) {
					if (sc.ch == ':') {
						if (parenCount == 0 && visibleChars == sc.LengthCurrent()) {
							sc.ChangeState(SCE_CSHARP_LABEL);
						} else if (chBefore == '[') {
							// [target: Attribute]
							sc.ChangeState(SCE_CSHARP_ATTRIBUTE);
							kwType = KeywordType::Attribute;
						}
					} else if (sc.ch != '.') {
						if (kwType > KeywordType::None && kwType < KeywordType::Return) {
							sc.ChangeState(static_cast<int>(kwType));
						} else {
							const int chNext = sc.GetDocNextChar(sc.ch == '?' || sc.ch == ')');
							if (sc.ch == ')') {
								if (chBeforeIdentifier == '(' && (chNext == '(' || (kwType != KeywordType::While && IsIdentifierCharEx(chNext)))) {
									// (type)(expression)
									// (type)expression, (type)++identifier, (type)--identifier
									sc.ChangeState(SCE_CSHARP_CLASS);
								}
							} else if (chNext == '(') {
								if (kwType != KeywordType::Return && (IsIdentifierCharEx(chBefore) || chBefore == ']')) {
									// type method()
									// type[] method()
									// type<type> method()
									sc.ChangeState(SCE_CSHARP_FUNCTION_DEFINITION);
								} else {
									sc.ChangeState(SCE_CSHARP_FUNCTION);
								}
							} else if ((sc.ch == '[' && (sc.chNext == ']' || sc.chNext == ','))
								|| (chBeforeIdentifier == '<' && (chNext == '>' || chNext == '<'))
								|| IsIdentifierStartEx(chNext)) {
								// type[] identifier
								// type[,] identifier
								// type<type>
								// type<type?>
								// type<type<type>>
								// type identifier
								sc.ChangeState(IsInterfaceName(s[0], s[1]) ? SCE_CSHARP_INTERFACE : SCE_CSHARP_CLASS);
							}
						}
#if 0
						if (sc.state == SCE_CSHARP_IDENTIFIER && IsInterfaceName(s[0], s[1])) {
							sc.ChangeState(SCE_CSHARP_INTERFACE);
						}
#endif
					}
				}
				if (sc.state != SCE_CSHARP_WORD && sc.state != SCE_CSHARP_ATTRIBUTE && sc.ch != '.') {
					kwType = KeywordType::None;
				}
				sc.SetState(SCE_CSHARP_DEFAULT);
			}
			break;

		case SCE_CSHARP_PREPROCESSOR_MESSAGE:
			if (sc.atLineStart) {
				sc.SetState(SCE_CSHARP_DEFAULT);
			}
			break;

		case SCE_CSHARP_COMMENTLINE:
			if (sc.atLineStart) {
				sc.SetState(SCE_CSHARP_DEFAULT);
			} else {
				HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_CSHARP_TASKMARKER);
			}
			break;

		case SCE_CSHARP_COMMENTBLOCK:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_CSHARP_DEFAULT);
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_CSHARP_TASKMARKER)) {
				continue;
			}
			break;

		case SCE_CSHARP_COMMENTLINEDOC:
		case SCE_CSHARP_COMMENTBLOCKDOC:
			if (docTagState != DocTagState::None) {
				if (sc.Match('/', '>') || sc.ch == '>') {
					docTagState = DocTagState::None;
					const int state = sc.state;
					sc.SetState(SCE_CSHARP_COMMENTTAG_XML);
					sc.Forward((sc.ch == '/') ? 2 : 1);
					sc.SetState(state);
				}
			}
			if (sc.state == SCE_CSHARP_COMMENTLINEDOC) {
				if (sc.atLineStart) {
					sc.SetState(SCE_CSHARP_DEFAULT);
					break;
				}
			} else if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_CSHARP_DEFAULT);
				break;
			}
			if (docTagState == DocTagState::None) {
				if (sc.ch == '<') {
					if (IsAlpha(sc.chNext)) {
						docTagState = DocTagState::XmlOpen;
						escSeq.outerState = sc.state;
						sc.SetState(SCE_CSHARP_COMMENTTAG_XML);
					} else if (sc.chNext == '/' && IsAlpha(sc.GetRelative(2))) {
						docTagState = DocTagState::XmlClose;
						escSeq.outerState = sc.state;
						sc.SetState(SCE_CSHARP_COMMENTTAG_XML);
						sc.Forward();
					}
				} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_CSHARP_TASKMARKER)) {
					continue;
				}
			}
			break;

		case SCE_CSHARP_COMMENTTAG_XML:
			if (!(IsIdentifierChar(sc.ch) || sc.ch == '-' || sc.ch == ':')) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_CSHARP_CHARACTER:
			if (sc.atLineStart) {
				sc.SetState(SCE_CSHARP_DEFAULT);
			} else if (sc.ch == '\\') {
				if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
					sc.SetState(SCE_CSHARP_ESCAPECHAR);
					sc.Forward();
				}
			} else if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_CSHARP_DEFAULT);
			}
			break;

		case SCE_CSHARP_STRING:
		case SCE_CSHARP_INTERPOLATED_STRING:
		case SCE_CSHARP_VERBATIM_STRING:
		case SCE_CSHARP_INTERPOLATED_VERBATIM_STRING:
			if (sc.atLineStart && !IsVerbatimString(sc.state)) {
				sc.SetState(SCE_CSHARP_DEFAULT);
				break;
			}
			switch (sc.ch) {
			case '\\':
				if (!IsVerbatimString(sc.state)) {
					if (escSeq.resetEscapeState(sc.state, sc.chNext)) {
						sc.SetState(SCE_CSHARP_ESCAPECHAR);
						sc.Forward();
					}
				}
				break;

			case '\"':
				if (sc.chNext == '\"' && IsVerbatimString(sc.state)) {
					escSeq.outerState = sc.state;
					escSeq.digitsLeft = 1;
					sc.SetState(SCE_CSHARP_ESCAPECHAR);
					sc.Forward();
				} else {
					sc.ForwardSetState(SCE_CSHARP_DEFAULT);
					if (!nestedState.empty() && nestedState.back().state == sc.state) {
						nestedState.pop_back();
					}
				}
				break;

			case '{':
				if (sc.chNext == '{') {
					escSeq.outerState = sc.state;
					escSeq.digitsLeft = 1;
					sc.SetState(SCE_CSHARP_ESCAPECHAR);
					sc.Forward();
				} else if (IsInterpolatedString(sc.state)) {
					nestedState.push_back({sc.state, 0});
					sc.SetState(SCE_CSHARP_OPERATOR2);
					sc.ForwardSetState(SCE_CSHARP_DEFAULT);
				} else if (IsIdentifierCharEx(sc.chNext) || sc.chNext == '@' || sc.chNext == '$') {
					// standard format: {index,alignment:format}
					// third party string template library: {@identifier} {$identifier} {identifier}
					escSeq.outerState = sc.state;
					sc.SetState(SCE_CSHARP_PLACEHOLDER);
					if (sc.chNext == '@' || sc.chNext == '$') {
						sc.Forward();
					}
				}
				break;

			case '}':
				if (IsInterpolatedString(sc.state)) {
					const bool interpolating = !nestedState.empty();
					if (interpolating) {
						nestedState.pop_back();
					}
					if (interpolating || sc.chNext != '}') {
						const int state = sc.state;
						sc.SetState(SCE_CSHARP_OPERATOR2);
						sc.ForwardSetState(state);
						continue;
					}
				}
				if (sc.chNext == '}') {
					escSeq.outerState = sc.state;
					escSeq.digitsLeft = 1;
					sc.SetState(SCE_CSHARP_ESCAPECHAR);
					sc.Forward();
				}
				break;
			}
			break;

		case SCE_CSHARP_FORMAT_SPECIFIER:
			if (IsInvalidFormatSpecifier(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_CSHARP_PLACEHOLDER:
			if (!IsIdentifierCharEx(sc.ch)) {
				if (sc.ch != '}') {
					const Sci_Position length = CheckFormatSpecifier(sc, styler);
					if (length == 0) {
						sc.Rewind();
						sc.ChangeState(escSeq.outerState);
					} else {
						sc.SetState(SCE_CSHARP_FORMAT_SPECIFIER);
						sc.Advance(length);
						sc.SetState(SCE_CSHARP_PLACEHOLDER);
					}
				}
				sc.ForwardSetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_CSHARP_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;
		}

		if (sc.state == SCE_CSHARP_DEFAULT) {
			if (ppKind == PreprocessorKind::Message && !isspacechar(sc.ch)) {
				sc.SetState(SCE_CSHARP_PREPROCESSOR_MESSAGE);
			} else if (sc.ch == '/' && (sc.chNext == '/' || sc.chNext == '*')) {
				visibleCharsBefore = visibleChars;
				docTagState = DocTagState::None;
				const int chNext = sc.chNext;
				if (chNext == '/' && visibleChars == 0) {
					lineStateLineType = CSharpLineStateMaskLineComment;
				}
				sc.SetState((chNext == '/') ? SCE_CSHARP_COMMENTLINE : SCE_CSHARP_COMMENTBLOCK);
				sc.Forward(2);
				if (sc.ch == chNext && sc.chNext != chNext) {
					sc.ChangeState((chNext == '/') ? SCE_CSHARP_COMMENTLINEDOC : SCE_CSHARP_COMMENTBLOCKDOC);
				}
				continue;
			} else if (sc.ch == '$' || sc.ch == '@') {
				if (sc.chNext == '\"') {
					sc.SetState((sc.ch == '@') ? SCE_CSHARP_VERBATIM_STRING : SCE_CSHARP_INTERPOLATED_STRING);
					sc.Forward();
				} else if (sc.ch != sc.chNext) {
					const int chNext = sc.GetRelative(2);
					if ((sc.chNext == '$' || sc.chNext == '@') && chNext == '\"') {
						sc.SetState(SCE_CSHARP_INTERPOLATED_VERBATIM_STRING);
						sc.Advance(2);
					} else if (sc.ch == '@' && IsCsIdentifierStart(sc.chNext, chNext)) {
						chBefore = chPrevNonWhite;
						if (chPrevNonWhite != '.') {
							chBeforeIdentifier = chPrevNonWhite;
						}
						sc.SetState(SCE_CSHARP_IDENTIFIER);
					}
				}
			} else if (sc.ch == '\"') {
				sc.SetState(SCE_CSHARP_STRING);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_CSHARP_CHARACTER);
			} else if (visibleChars == 0 && sc.ch == '#') {
				ppKind = PreprocessorKind::Init;
				sc.SetState(SCE_CSHARP_PREPROCESSOR);
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_CSHARP_NUMBER);
			} else if (IsCsIdentifierStart(sc.ch, sc.chNext)) {
				chBefore = chPrevNonWhite;
				if (chPrevNonWhite != '.') {
					chBeforeIdentifier = chPrevNonWhite;
				}
				sc.SetState(SCE_CSHARP_IDENTIFIER);
			} else if (isoperator(sc.ch)) {
				const bool interpolating = !nestedState.empty();
				if (sc.ch == '(' || sc.ch == '[') {
					if (interpolating) {
						nestedState.back().parenCount += 1;
					} else {
						++parenCount;
					}
				} else if (sc.ch == ')' || sc.ch == ']') {
					if (interpolating) {
						InterpolatedStringState &state = nestedState.back();
						if (state.parenCount > 0) {
							--state.parenCount;
						}
					} else {
						if (parenCount > 0) {
							--parenCount;
						}
					}
				}
				if (interpolating) {
					const InterpolatedStringState &state = nestedState.back();
					if (state.parenCount == 0 && IsInterpolatedStringEnd(sc)) {
						if (sc.ch == '}') {
							sc.SetState(state.state);
						} else {
							escSeq.outerState = state.state;
							sc.SetState(SCE_CSHARP_FORMAT_SPECIFIER);
						}
						continue;
					}
				} else {
					if (kwType == KeywordType::None && sc.ch == '[') {
						if (visibleChars == 0 || !(chPrevNonWhite == ')' || chPrevNonWhite == ']' || IsIdentifierCharEx(chPrevNonWhite))) {
							kwType = KeywordType::Attribute;
						}
					} else if (kwType == KeywordType::Attribute && (sc.ch == '(' || sc.ch == ']')) {
						kwType = KeywordType::None;
					}
				}
				sc.SetState(interpolating ? SCE_CSHARP_OPERATOR2 : SCE_CSHARP_OPERATOR);
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
			if (!IsSpaceEquiv(sc.state)) {
				chPrevNonWhite = sc.ch;
			}
		}
		if (sc.atLineEnd) {
			int lineState = lineStateLineType | (parenCount << 4);
			if (!nestedState.empty()) {
				const int state = nestedState.back().state;
				if (!IsVerbatimString(state)) {
					nestedState.pop_back();
					sc.SetState(state);
				}
				if (!nestedState.empty()) {
					lineState |= CSharpLineStateMaskInterpolation;
				}
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineType = 0;
			visibleChars = 0;
			visibleCharsBefore = 0;
			docTagState = DocTagState::None;
			ppKind = PreprocessorKind::None;
			kwType = KeywordType::None;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int usingName;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & CSharpLineStateMaskLineComment),
		usingName((lineState >> 1) & 1) {
	}
};

constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_CSHARP_COMMENTBLOCK
		|| style == SCE_CSHARP_COMMENTBLOCKDOC
		|| style == SCE_CSHARP_COMMENTTAG_XML
		|| style == SCE_CSHARP_TASKMARKER;
}

constexpr bool IsMultilineStringStyle(int style) noexcept {
	return style == SCE_CSHARP_VERBATIM_STRING
		|| style == SCE_CSHARP_INTERPOLATED_VERBATIM_STRING
		|| style == SCE_CSHARP_OPERATOR2
		|| style == SCE_CSHARP_ESCAPECHAR
		|| style == SCE_CSHARP_FORMAT_SPECIFIER
		|| style == SCE_CSHARP_PLACEHOLDER;
}

void FoldCSharpDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_CSHARP_OPERATOR, SCE_CSHARP_TASKMARKER, SCE_CSHARP_PREPROCESSOR);
		if (bracePos) {
			startPos = bracePos + 1; // skip the brace
		}
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	char buf[12]; // endregion
	constexpr int MaxFoldWordLength = sizeof(buf) - 1;
	int wordLen = 0;

	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;
	int visibleChars = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		switch (style) {
		case SCE_CSHARP_COMMENTBLOCK:
		case SCE_CSHARP_COMMENTBLOCKDOC:
			if (!IsStreamCommentStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamCommentStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_CSHARP_VERBATIM_STRING:
		case SCE_CSHARP_INTERPOLATED_VERBATIM_STRING:
			if (!IsMultilineStringStyle(stylePrev)) {
				levelNext++;
			} else if (!IsMultilineStringStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_CSHARP_OPERATOR: {
			const char ch = styler[i];
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		} break;

		case SCE_CSHARP_PREPROCESSOR:
			if (wordLen < MaxFoldWordLength) {
				buf[wordLen++] = MakeLowerCase(styler[i]);
			}
			if (styleNext != style) {
				buf[wordLen] = '\0';
				wordLen = 0;
				const char *p = buf;
				if (*p == '#') {
					++p;
				}
				if (StrEqualsAny(p, "if", "region")) {
					levelNext++;
				} else if (StrStartsWith(p, "end")) {
					levelNext--;
				}
			}
			break;
		}

		if (visibleChars == 0 && !IsSpaceEquiv(style)) {
			++visibleChars;
		}
		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.usingName) {
				levelNext += foldNext.usingName - foldPrev.usingName;
			} else if (visibleChars) {
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_CSHARP_OPERATOR, SCE_CSHARP_TASKMARKER, SCE_CSHARP_PREPROCESSOR);
				if (bracePos) {
					levelNext++;
					i = bracePos; // skip the brace
					style = SCE_CSHARP_OPERATOR;
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
			foldPrev = foldCurrent;
			foldCurrent = foldNext;
			visibleChars = 0;
		}
	}
}

}

LexerModule lmCSharp(SCLEX_CSHARP, ColouriseCSharpDoc, "csharp", FoldCSharpDoc);
