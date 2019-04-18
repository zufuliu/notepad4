// Scintilla source code edit control
/** @file LexAccessor.cxx
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cassert>
#include <cctype>

#include "ILexer.h"
#include "LexAccessor.h"
#include "CharacterSet.h"

using namespace Scintilla;

LexAccessor::LexAccessor(IDocument *pAccess_) noexcept :
	pAccess(pAccess_), startPos(extremePosition), endPos(0),
	codePage(pAccess->CodePage()),
	encodingType(enc8bit),
	lenDoc(pAccess->Length()),
	validLen(0),
	startSeg(0),
	startPosStyling(0),
	documentVersion(pAccess->Version()) {
	// Prevent warnings by static analyzers about uninitialized buf and styleBuf.
	buf[0] = 0;
	styleBuf[0] = 0;
	if (codePage) {
		encodingType = (codePage == 65001) ? encUnicode : encDBCS;
	}
}
void LexAccessor::Fill(Sci_Position position) noexcept {
	startPos = position - slopSize;
	if (startPos + bufferSize > lenDoc)
		startPos = lenDoc - bufferSize;
	if (startPos < 0)
		startPos = 0;
	endPos = startPos + bufferSize;
	if (endPos > lenDoc)
		endPos = lenDoc;

	pAccess->GetCharRange(buf, startPos, endPos - startPos);
	buf[endPos - startPos] = '\0';
}
char LexAccessor::operator[](Sci_Position position) noexcept {
	if (position < startPos || position >= endPos) {
		Fill(position);
	}
	return buf[position - startPos];
}
char LexAccessor::SafeGetCharAt(Sci_Position position) noexcept {
	if (position < startPos || position >= endPos) {
		Fill(position);
		if (position < startPos || position >= endPos) {
			// Position is outside range of document
			return '\0';
		}
	}
	return buf[position - startPos];
}
void LexAccessor::Flush() {
	if (validLen > 0) {
		pAccess->SetStyles(validLen, styleBuf);
		startPosStyling += validLen;
		validLen = 0;
	}
}
void LexAccessor::StartAt(Sci_PositionU start) noexcept {
	pAccess->StartStyling(start);
	startPosStyling = start;
}
void LexAccessor::ColourTo(Sci_PositionU pos, int chAttr) {
	// Only perform styling if non empty range
	if (pos != startSeg - 1) {
		assert(pos >= startSeg);
		if (pos < startSeg) {
			return;
		}

		if (validLen + (pos - startSeg + 1) >= bufferSize)
			Flush();
		const unsigned char attr = static_cast<unsigned char>(chAttr);
		if (validLen + (pos - startSeg + 1) >= bufferSize) {
			// Too big for buffer so send directly
			pAccess->SetStyleFor(pos - startSeg + 1, attr);
		} else {
			for (Sci_PositionU i = startSeg; i <= pos; i++) {
				assert((startPosStyling + validLen) < Length());
				styleBuf[validLen++] = attr;
			}
		}
	}
	startSeg = pos + 1;
}
void LexAccessor::IndicatorFill(Sci_Position start, Sci_Position end, int indicator, int value) {
	pAccess->DecorationSetCurrentIndicator(indicator);
	pAccess->DecorationFillRange(start, value, end - start);
}

////////////////////////////////////////////////////////////////////////////////
namespace Scintilla {

bool IsLexAtEOL(Sci_Position pos, LexAccessor &styler) noexcept {
	return (styler[pos] == '\n') || ((styler[pos] == '\r') && styler.SafeGetCharAt(pos + 1) != '\n');
}
Sci_Position LexSkipSpaceTab(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler) noexcept {
	for (Sci_Position i = startPos; i < endPos; i++) {
		if (!IsASpaceOrTab(styler.SafeGetCharAt(i)))
			return i;
	}
	return endPos;
}
Sci_Position LexLineSkipSpaceTab(Sci_Position line, LexAccessor &styler) noexcept {
	const Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	return LexSkipSpaceTab(startPos, endPos, styler);
}
bool IsLexSpaceToEOL(LexAccessor &styler, Sci_Position startPos) noexcept {
	const Sci_Position line = styler.GetLine(startPos);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	startPos = LexSkipSpaceTab(startPos, endPos, styler);
	return startPos == endPos;
}
bool IsLexEmptyLine(LexAccessor &styler, Sci_Position line) noexcept {
	Sci_Position startPos = styler.LineStart(line);
	const Sci_Position endPos = styler.LineStart(line + 1) - 1;
	startPos = LexSkipSpaceTab(startPos, endPos, styler);
	return startPos == endPos;
}
bool IsLexLineStartsWith(Sci_Position line, LexAccessor &styler, const char* word, bool matchCase, int style) noexcept {
	const Sci_Position pos = LexLineSkipSpaceTab(line, styler);
	return styler.StyleAt(pos) == style &&
		(matchCase ? styler.Match(pos, word) : LexMatchIgnoreCase(pos, styler, word));
}

bool IsLexCommentLine(Sci_Position line, LexAccessor &styler, int style) noexcept {
	const Sci_Position pos = LexLineSkipSpaceTab(line, styler);
	const int stl = styler.StyleAt(pos);
	//char ch = styler.SafeGetCharAt(pos);
	if (stl == 0 && stl != style)
		return false;
	while (style > 0 && stl != (style & 0xFF)) {
		style >>= 8;
	}
	return !!style;
}
bool IsBackslashLine(Sci_Position line, LexAccessor &styler) noexcept {
	const Sci_Position pos = styler.LineStart(line + 1) - 1;
	return (pos >= 2) && (styler[pos] == '\n')
		&& (styler[pos - 1] == '\\' || (styler[pos - 1] == '\r' && styler[pos - 2] == '\\'));
}


Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler) noexcept {
	for (Sci_Position i = startPos; i < endPos; i++) {
		if (!(isspacechar(styler.SafeGetCharAt(i))))
			return i;
	}
	return endPos;
}
Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, bool IsStreamCommentStyle(int)) noexcept {
	for (Sci_Position i = startPos; i < endPos; i++) {
		if (!(isspacechar(styler.SafeGetCharAt(i)) || IsStreamCommentStyle(styler.StyleAt(i))))
			return i;
	}
	return endPos;
}
Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler,
	bool IsStreamCommentStyle(int), const CharacterSet &charSet) noexcept {
	for (Sci_Position i = startPos; i < endPos; i++) {
		const char ch = styler[i];
		if (!(isspacechar(styler.SafeGetCharAt(i)) || IsStreamCommentStyle(styler.StyleAt(i))
			|| charSet.Contains(ch)))
			return i;
	}
	return endPos;
}

bool LexMatch(Sci_Position pos, LexAccessor &styler, const char *s) noexcept {
	for (Sci_Position i = 0; *s; i++) {
		if (*s != styler.SafeGetCharAt(pos + i))
			return false;
		s++;
	}
	return true;
}
bool LexMatchIgnoreCase(Sci_Position pos, LexAccessor &styler, const char *s) noexcept {
	for (Sci_Position i = 0; *s; i++) {
		if (MakeLowerCase(*s) != MakeLowerCase(styler.SafeGetCharAt(pos + i)))
			return false;
		s++;
	}
	return true;
}

Sci_PositionU LexGetRange(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, char *s, Sci_PositionU len) noexcept {
	Sci_PositionU i = 0;
	char ch = styler.SafeGetCharAt(startPos + i);
	Sci_PositionU _endPos = endPos - startPos + 1;
	if (_endPos > len - 1)
		_endPos = len - 1;
	while (i < _endPos) {
		s[i] = ch;
		i++;
		ch = styler.SafeGetCharAt(startPos + i);
	}
	s[i] = '\0';
	return i;
}
Sci_PositionU LexGetRangeLowered(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, char *s, Sci_PositionU len) noexcept {
	Sci_PositionU i = 0;
	char ch = MakeLowerCase(styler.SafeGetCharAt(startPos + i));
	Sci_PositionU _endPos = endPos - startPos + 1;
	if (_endPos > len - 1)
		_endPos = len - 1;
	while (i < _endPos) {
		s[i] = ch;
		i++;
		ch = MakeLowerCase(styler.SafeGetCharAt(startPos + i));
	}
	s[i] = '\0';
	return i;
}

Sci_PositionU LexGetRange(Sci_Position startPos, LexAccessor &styler, bool IsWordChar(int), char *s, Sci_PositionU len) noexcept {
	Sci_PositionU i = 0;
	char ch = styler.SafeGetCharAt(startPos + i);
	while ((i < len - 1) && IsWordChar(static_cast<unsigned char>(ch))) {
		s[i] = ch;
		i++;
		ch = styler.SafeGetCharAt(startPos + i);
	}
	s[i] = '\0';
	return i;
}
Sci_PositionU LexGetRangeLowered(Sci_Position startPos, LexAccessor &styler, bool IsWordChar(int), char *s, Sci_PositionU len) noexcept {
	Sci_PositionU i = 0;
	char ch = MakeLowerCase(styler.SafeGetCharAt(startPos + i));
	while ((i < len - 1) && IsWordChar(static_cast<unsigned char>(ch))) {
		s[i] = ch;
		i++;
		ch = MakeLowerCase(styler.SafeGetCharAt(startPos + i));
	}
	s[i] = '\0';
	return i;
}
Sci_PositionU LexGetRange(Sci_Position startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept {
	Sci_PositionU i = 0;
	char ch = styler.SafeGetCharAt(startPos + i);
	while ((i < len - 1) && charSet.Contains(ch)) {
		s[i] = ch;
		i++;
		ch = styler.SafeGetCharAt(startPos + i);
	}
	s[i] = '\0';
	return i;
}
Sci_PositionU LexGetRangeLowered(Sci_Position startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept {
	Sci_PositionU i = 0;
	char ch = MakeLowerCase(styler.SafeGetCharAt(startPos + i));
	while ((i < len - 1) && charSet.Contains(ch)) {
		s[i] = ch;
		i++;
		ch = MakeLowerCase(styler.SafeGetCharAt(startPos + i));
	}
	s[i] = '\0';
	return i;
}

char LexGetPrevChar(Sci_Position endPos, LexAccessor &styler) noexcept {
	while (IsASpaceOrTab(styler.SafeGetCharAt(--endPos))){ }
	return styler.SafeGetCharAt(endPos);
}
char LexGetNextChar(Sci_Position startPos, LexAccessor &styler) noexcept {
	while (IsASpaceOrTab(styler.SafeGetCharAt(startPos++))){ }
	return styler.SafeGetCharAt(startPos - 1);
}

}
