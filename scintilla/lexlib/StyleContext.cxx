// Scintilla source code edit control
/** @file StyleContext.cxx
 ** Lexer infrastructure.
 **/
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// This file is in the public domain.

#include <cstdint>
#include <cassert>
#include <cstring>

#include "ILexer.h"

#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "StringUtils.h"

using namespace Scintilla;

namespace Scintilla {

bool StyleContext::MatchIgnoreCase(const char *s) const noexcept {
	if (MakeLowerCase(ch) != static_cast<unsigned char>(*s)) {
		return false;
	}
	s++;
	if (MakeLowerCase(chNext) != static_cast<unsigned char>(*s)) {
		return false;
	}
	s++;
	for (Sci_PositionU pos = currentPos + 2; *s; s++, pos++) {
		if (*s != MakeLowerCase(styler.SafeGetCharAt(pos))) {
			return false;
		}
	}
	return true;
}

namespace {

constexpr bool IsTaskMarkerPrev(int chPrev) noexcept {
	return chPrev <= 32 || AnyOf(chPrev, '/', '*', '!', '#');
}

constexpr bool IsTaskMarkerStart(int visibleChars, int visibleCharsBefore, int chPrev, int ch, int chNext) noexcept {
	return (visibleChars == 0 || (visibleChars <= visibleCharsBefore + 3 && IsTaskMarkerPrev(chPrev)))
		&& IsUpperCase(ch) && IsUpperCase(chNext);
}

}

bool HighlightTaskMarker(StyleContext &sc, int &visibleChars, int visibleCharsBefore, int markerStyle) {
	if (IsTaskMarkerStart(visibleChars, visibleCharsBefore, sc.chPrev, sc.ch, sc.chNext)) {
		Sci_PositionU pos = sc.currentPos + 2;
		unsigned char ch;
		while (IsUpperCase(ch = sc.styler[pos])) {
			++pos;
		}

		bool marker = false;
		const int len = static_cast<int>(pos - sc.currentPos);
		if (ch == ':' || ch == '(') {
			// highlight first uppercase word after comment characters as task marker.
			marker = true;
		} else if (ch <= 32 && len >= 3 && len < 16 && AnyOf(sc.ch, 'T', 'F', 'N', 'X')) {
			char s[8];
			sc.styler.GetRange(sc.currentPos, pos, s, sizeof(s));
			marker = StrEqualsAny(s, "TODO", "FIXME", "NOTE", "XXX", "TBD")
				|| StrStartsWith(s, "NOLINT"); // clang-tidy: NOLINT, NOLINTNEXTLINE
		}

		visibleChars += len;
		const int state = sc.state;
		sc.SetState(markerStyle);
		sc.Forward(len);
		if (marker) {
			sc.SetState(state);
		} else {
			sc.ChangeState(state);
		}
		return true;
	}
	return false;
}

}
