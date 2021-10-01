// This file is part of Notepad2.
// See License.txt for details about distribution and modification.

#include <cstdint>
#include <cassert>

#include <string>
#include <string_view>
#include <vector>

#include "ILexer.h"
#include "LexAccessor.h"
#include "CharacterSet.h"
#include "LexerUtils.h"

using namespace Lexilla;

namespace Lexilla {

bool LexAccessor::MatchIgnoreCase(Sci_Position pos, const char *s) noexcept {
	for (; *s; s++, pos++) {
		if (*s != MakeLowerCase(SafeGetCharAt(pos))) {
			return false;
		}
	}
	return true;
}

void LexAccessor::GetRange(Sci_PositionU startPos_, Sci_PositionU endPos_, char *s, Sci_PositionU len) noexcept {
	assert(startPos_ <= endPos_ && len != 0 && s != nullptr);
	endPos_ = sci::min(endPos_, startPos_ + len - 1);
	len = endPos_ - startPos_;
	if (startPos_ >= static_cast<Sci_PositionU>(startPos) && endPos_ <= static_cast<Sci_PositionU>(endPos)) {
		const char * const p = buf + (startPos_ - startPos);
		memcpy(s, p, len);
	} else {
		pAccess->GetCharRange(s, startPos_, len);
	}
	s[len] = '\0';
}

void LexAccessor::GetRangeLowered(Sci_PositionU startPos_, Sci_PositionU endPos_, char *s, Sci_PositionU len) noexcept {
	GetRange(startPos_, endPos_, s, len);
	while (*s) {
		if (*s >= 'A' && *s <= 'Z') {
			*s += 'a' - 'A';
		}
		++s;
	}
}

std::string LexAccessor::GetRange(Sci_PositionU startPos_, Sci_PositionU endPos_) {
	assert(startPos_ < endPos_);
	const Sci_PositionU len = endPos_ - startPos_;
	std::string s(len, '\0');
	GetRange(startPos_, endPos_, s.data(), len);
	return s;
}

std::string LexAccessor::GetRangeLowered(Sci_PositionU startPos_, Sci_PositionU endPos_) {
	assert(startPos_ < endPos_);
	const Sci_PositionU len = endPos_ - startPos_;
	std::string s(len, '\0');
	GetRangeLowered(startPos_, endPos_, s.data(), len);
	return s;
}

Sci_Position LexLineSkipSpaceTab(Sci_Line line, LexAccessor &styler) noexcept {
	Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	for (; startPos < endPos; startPos++) {
		if (!IsSpaceOrTab(styler.SafeGetCharAt(startPos))) {
			break;
		}
	}
	return startPos;
}

bool IsLexSpaceToEOL(LexAccessor &styler, Sci_Position startPos) noexcept {
	const Sci_Line line = styler.GetLine(startPos);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	for (; startPos < endPos; startPos++) {
		const char ch = styler.SafeGetCharAt(startPos);
		if (!IsSpaceOrTab(ch)) {
			return IsEOLChar(ch);
		}
	}
	return true;
}

bool IsLexEmptyLine(LexAccessor &styler, Sci_Line line) noexcept {
	Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	for (; startPos < endPos; startPos++) {
		const char ch = styler.SafeGetCharAt(startPos);
		if (!IsSpaceOrTab(ch)) {
			return IsEOLChar(ch);
		}
	}
	return true;
}

bool IsLexLineStartsWith(Sci_Line line, LexAccessor &styler, const char *word, bool matchCase, int style) noexcept {
	Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	for (; startPos < endPos; startPos++) {
		if (!IsSpaceOrTab(styler.SafeGetCharAt(startPos))) {
			return styler.StyleAt(startPos) == style &&
			(matchCase ? styler.Match(startPos, word) : styler.MatchIgnoreCase(startPos, word));
		}
	}
	return false;
}

bool IsLexCommentLine(Sci_Line line, LexAccessor &styler, int style) noexcept {
	Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	for (; startPos < endPos; startPos++) {
		if (!IsSpaceOrTab(styler.SafeGetCharAt(startPos))) {
			const int stl = styler.StyleAt(startPos);
			//char ch = styler.SafeGetCharAt(startPos);
			if (stl == 0 && stl != style) {
				return false;
			}
			while (style != 0 && stl != (style & 0xFF)) {
				style >>= 8;
			}
			return style != 0;
		}
	}
	return false;
}

Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler) noexcept {
	for (; startPos < endPos; startPos++) {
		if (!IsWhiteSpace(styler[startPos])) {
			break;
		}
	}
	return startPos;
}

Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, bool IsStreamCommentStyle(int) noexcept) noexcept {
	for (; startPos < endPos; startPos++) {
		if (!(IsWhiteSpace(styler.SafeGetCharAt(startPos)) || IsStreamCommentStyle(styler.StyleAt(startPos)))) {
			break;
		}
	}
	return startPos;
}

Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler,
	bool IsStreamCommentStyle(int), const CharacterSet &charSet) noexcept {
	for (; startPos < endPos; startPos++) {
		const char ch = styler.SafeGetCharAt(startPos);
		if (!(IsWhiteSpace(ch) || charSet.Contains(ch) || IsStreamCommentStyle(styler.StyleAt(startPos)))) {
			break;
		}
	}
	return startPos;
}

Sci_PositionU LexGetRange(Sci_Position startPos, LexAccessor &styler, bool IsWordChar(int) noexcept, char *s, Sci_PositionU len) noexcept {
	Sci_PositionU i = 0;
	char ch = styler.SafeGetCharAt(startPos);
	while ((i < len - 1) && IsWordChar(static_cast<unsigned char>(ch))) {
		s[i] = ch;
		i++;
		startPos++;
		ch = styler.SafeGetCharAt(startPos);
	}
	s[i] = '\0';
	return i;
}

Sci_PositionU LexGetRangeLowered(Sci_Position startPos, LexAccessor &styler, bool IsWordChar(int) noexcept, char *s, Sci_PositionU len) noexcept {
	Sci_PositionU i = 0;
	char ch = MakeLowerCase(styler.SafeGetCharAt(startPos));
	while ((i < len - 1) && IsWordChar(static_cast<unsigned char>(ch))) {
		s[i] = ch;
		i++;
		startPos++;
		ch = MakeLowerCase(styler.SafeGetCharAt(startPos));
	}
	s[i] = '\0';
	return i;
}

Sci_PositionU LexGetRange(Sci_Position startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept {
	Sci_PositionU i = 0;
	char ch = styler.SafeGetCharAt(startPos);
	while ((i < len - 1) && charSet.Contains(ch)) {
		s[i] = ch;
		i++;
		startPos++;
		ch = styler.SafeGetCharAt(startPos);
	}
	s[i] = '\0';
	return i;
}

Sci_PositionU LexGetRangeLowered(Sci_Position startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept {
	Sci_PositionU i = 0;
	char ch = MakeLowerCase(styler.SafeGetCharAt(startPos));
	while ((i < len - 1) && charSet.Contains(ch)) {
		s[i] = ch;
		i++;
		startPos++;
		ch = MakeLowerCase(styler.SafeGetCharAt(startPos));
	}
	s[i] = '\0';
	return i;
}

int PackLineState(const std::vector<int>& states) noexcept {
	return PackLineState<DefaultNestedStateValueBit, DefaultMaxNestedStateCount, DefaultNestedStateCountBit, DefaultNestedStateBaseStyle>(states);
}

void UnpackLineState(int lineState, std::vector<int>& states) {
	UnpackLineState<DefaultNestedStateValueBit, DefaultMaxNestedStateCount, DefaultNestedStateCountBit, DefaultNestedStateBaseStyle>(lineState, states);
}

void BacktrackToStart(const LexAccessor &styler, int stateMask, Sci_PositionU &startPos, Sci_Position &lengthDoc, int &initStyle) noexcept {
	const Sci_Line currentLine = styler.GetLine(startPos);
	if (currentLine != 0) {
		Sci_Line line = currentLine - 1;
		int lineState = styler.GetLineState(line);
		while ((lineState & stateMask) != 0 && line != 0) {
			--line;
			lineState = styler.GetLineState(line);
		}
		if ((lineState & stateMask) == 0) {
			++line;
		}
		if (line != currentLine) {
			const Sci_Position endPos = startPos + lengthDoc;
			startPos = (line == 0)? 0 : styler.LineStart(line);
			lengthDoc = endPos - startPos;
			initStyle = (startPos == 0)? 0 : styler.StyleAt(startPos - 1);
		}
	}
}

void LookbackNonWhite(LexAccessor &styler, Sci_PositionU startPos, int maxSpaceStyle, int &chPrevNonWhite, int &stylePrevNonWhite) noexcept {
	Sci_PositionU back = startPos - 1;
	while (back) {
		const int style = styler.StyleAt(back);
		if (style > maxSpaceStyle) {
			chPrevNonWhite = static_cast<unsigned char>(styler.SafeGetCharAt(back));
			stylePrevNonWhite = style;
			break;
		}
		--back;
	}
}

Sci_PositionU CheckBraceOnNextLine(LexAccessor &styler, Sci_Line line, int operatorStyle, int maxSpaceStyle, int ignoreStyle) noexcept {
	// check brace on next line
	Sci_Position startPos = styler.LineStart(line + 1);
	Sci_Position bracePos = startPos;
	char ch;
	while (IsASpaceOrTab(ch = styler[bracePos])) {
		++bracePos;
	}
	if (ch != '{') {
		return 0;
	}

	int style = styler.StyleAt(bracePos);
	if (style != operatorStyle) {
		return 0;
	}

	// check current line
	Sci_Position endPos = startPos - 1;
	startPos = styler.LineStart(line);

	// ignore current line, e.g. current line is preprocessor.
	if (ignoreStyle) {
		while (startPos < endPos) {
			style = styler.StyleAt(startPos);
			if (style > maxSpaceStyle) {
				break;
			}
			++startPos;
		}
		if (style == ignoreStyle) {
			return 0;
		}
	}

	while (endPos >= startPos) {
		style = styler.StyleAt(endPos);
		if (style > maxSpaceStyle) {
			break;
		}
		--endPos;
	}
	if (endPos < startPos) {
		// current line is empty or comment
		return 0;
	}
	if (style == operatorStyle) {
		ch = styler[endPos];
		/*
		function(param)
			{ body }

		if (expr)
			{ body }
		else
			{ body }

		switch (expr)
			{ body }

		class name<T>
			{ body }

		var name =
			{ body }
		var name = new type[]
			{ body }

		case constant:
			{ body }

		ActionScript:
			function name(param:*):*
				{ body }
		C++:
			[lambda-capture]
				{ body }
		C#:
			=> { lambda }
		Java:
			-> { lambda }
		Objective-C:
			^{ block }
		Rust:
			fn name() -> optional?
				{ body }
		Scala:
			class name[T]
				{ body }
		*/
		if (!AnyOf(ch, ')', '>', '=', ':', ']', '^', '?', '*')) {
			return 0;
		}
	}

	/*
		class name
			{ body }

		try
			{ body }
		catch (exception)
			{ body }
	*/
	return bracePos;
}

}
