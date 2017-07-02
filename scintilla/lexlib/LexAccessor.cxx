// Scintilla source code edit control
/** @file LexAccessor.cxx
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "LexAccessor.h"
#include "CharacterSet.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

LexAccessor::LexAccessor(IDocument *pAccess_) :
		pAccess(pAccess_), startPos(extremePosition), endPos(0),
		codePage(pAccess->CodePage()),
		encodingType(enc8bit),
		lenDoc(pAccess->Length()),
		validLen(0),
		startSeg(0), startPosStyling(0),
		documentVersion(pAccess->Version()) {
		// Prevent warnings by static analyzers about uninitialized buf and styleBuf.
		buf[0] = 0;
		styleBuf[0] = 0;
		switch (codePage) {
		case 65001:
			encodingType = encUnicode;
			break;
		case 932:
		case 936:
		case 949:
		case 950:
		case 1361:
			encodingType = encDBCS;
		}
}
void LexAccessor::Fill(Sci_Position position) {
	startPos = position - slopSize;
	if (startPos + bufferSize > lenDoc)
		startPos = lenDoc - bufferSize;
	if (startPos < 0)
		startPos = 0;
	endPos = startPos + bufferSize;
	if (endPos > lenDoc)
		endPos = lenDoc;

	pAccess->GetCharRange(buf, startPos, endPos-startPos);
	buf[endPos-startPos] = '\0';
}
char LexAccessor::operator[](Sci_Position position) {
	if (position < startPos || position >= endPos) {
		Fill(position);
	}
	return buf[position - startPos];
}
char LexAccessor::SafeGetCharAt(Sci_Position position, char chDefault) {
	if (position < startPos || position >= endPos) {
		Fill(position);
		if (position < startPos || position >= endPos) {
			// Position is outside range of document
			return chDefault;
		}
	}
	return buf[position - startPos];
}
bool LexAccessor::Match(Sci_Position pos, const char *s) {
	return LexMatch(pos, *this, s);
}
Sci_Position LexAccessor::LineEnd(Sci_Position line) {
	return pAccess->LineEnd(line);
}
void LexAccessor::Flush() {
	if (validLen > 0) {
		pAccess->SetStyles(validLen, styleBuf);
		startPosStyling += validLen;
		validLen = 0;
	}
}
void LexAccessor::StartAt(Sci_PositionU start) {
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
		if (validLen + (pos - startSeg + 1) >= bufferSize) {
			// Too big for buffer so send directly
			pAccess->SetStyleFor(pos - startSeg + 1, static_cast<unsigned char>(chAttr));
		} else {
			for (Sci_PositionU i = startSeg; i <= pos; i++) {
				assert((startPosStyling + validLen) < Length());
				styleBuf[validLen++] = static_cast<unsigned char>(chAttr);
			}
		}
	}
	startSeg = pos+1;
}
void LexAccessor::IndicatorFill(Sci_Position start, Sci_Position end, int indicator, int value) {
	pAccess->DecorationSetCurrentIndicator(indicator);
	pAccess->DecorationFillRange(start, value, end - start);
}

////////////////////////////////////////////////////////////////////////////////
#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

bool IsLexAtEOL(Sci_Position pos, LexAccessor &styler) {
	return (styler[pos] == '\n') || ((styler[pos] == '\r') && styler.SafeGetCharAt(pos + 1) != '\n');
}
Sci_Position LexSkipSpaceTab(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler) {
	for (Sci_Position i = startPos; i < endPos; i++) {
		if (!IsASpaceOrTab(styler.SafeGetCharAt(i, '\0')))
			return i;
	}
	return endPos;
}
Sci_Position LexLineSkipSpaceTab(Sci_Position line, LexAccessor &styler) {
	Sci_Position startPos = styler.LineStart(line);
	Sci_Position endPos = styler.LineStart(line + 1) - 1;
	return LexSkipSpaceTab(startPos, endPos, styler);
}
bool IsLexLineStartsWith(Sci_Position line, LexAccessor &styler, const char* word, bool matchCase, int style) {
	Sci_Position pos = LexLineSkipSpaceTab(line, styler);
	return styler.StyleAt(pos) == style &&
		(matchCase ? styler.Match(pos, word) : LexMatchIgnoreCase(pos, styler, word));
}

bool IsLexCommentLine(Sci_Position line, LexAccessor &styler, int style) {
	Sci_Position pos = LexLineSkipSpaceTab(line, styler);
	int stl = styler.StyleAt(pos);
	//char ch = styler.SafeGetCharAt(pos);
	if (stl == 0 && stl != style)
		return false;
	while (style > 0 && stl != (style & 0xFF)) style >>= 8;
	return !!style;
}
bool IsBackslashLine(Sci_Position line, LexAccessor &styler) {
	Sci_Position pos = styler.LineStart(line + 1) - 1;
	return (pos >= 2) && (styler[pos] == '\n')
		&& (styler[pos-1] == '\\' || (styler[pos-1] == '\r' && styler[pos-2] == '\\'));
}


Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler) {
	for (Sci_Position i = startPos; i < endPos; i++) {
		if (!(isspacechar(styler.SafeGetCharAt(i, '\0'))))
			return i;
	}
	return endPos;
}
Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, bool IsStreamCommentStyle(Sci_Position)) {
	for (Sci_Position i = startPos; i < endPos; i++) {
		if (!(isspacechar(styler.SafeGetCharAt(i, '\0')) || IsStreamCommentStyle(styler.StyleAt(i))))
			return i;
	}
	return endPos;
}
Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler,
					  bool IsStreamCommentStyle(Sci_Position), const CharacterSet &charSet) {
	for (Sci_Position i = startPos; i < endPos; i++) {
		char ch = styler[i];
		if (!(isspacechar(styler.SafeGetCharAt(i, '\0')) || IsStreamCommentStyle(styler.StyleAt(i))
			|| charSet.Contains(ch)))
			return i;
	}
	return endPos;
}

bool LexMatch(Sci_Position pos, LexAccessor &styler, const char *s) {
	for (Sci_Position i = 0; *s; i++) {
		if (static_cast<unsigned char>(*s) !=
			static_cast<unsigned char>(styler.SafeGetCharAt(pos + i)))
			return false;
		s++;
	}
	return true;
}
bool LexMatchIgnoreCase(Sci_Position pos, LexAccessor &styler, const char *s) {
	for (Sci_Position i = 0; *s; i++) {
		if (static_cast<unsigned char>(tolower(*s)) !=
			static_cast<unsigned char>(tolower(styler.SafeGetCharAt(pos + i))))
			return false;
		s++;
	}
	return true;
}

int LexGetRange(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, char *s, int len) {
	int i = 0;
	char ch = styler.SafeGetCharAt(startPos + i);
	Sci_Position _endPos = endPos - startPos + 1;
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
int LexGetRangeLowered(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, char *s, int len) {
	int i = 0;
	char ch = static_cast<char>(tolower(styler.SafeGetCharAt(startPos + i)));
	Sci_Position _endPos = endPos - startPos + 1;
	if (_endPos > len - 1)
        _endPos = len - 1;
	while (i < _endPos) {
		s[i] = ch;
		i++;
		ch = static_cast<char>(tolower(styler.SafeGetCharAt(startPos + i)));
	}
	s[i] = '\0';
	return i;
}

int LexGetRange(Sci_Position startPos, LexAccessor &styler, bool IsWordChar(int), char *s, int len) {
	int i = 0;
	char ch = styler.SafeGetCharAt(startPos + i);
	while ((i < len - 1) && IsWordChar(ch)) {
		s[i] = ch;
		i++;
		ch = styler.SafeGetCharAt(startPos + i);
	}
	s[i] = '\0';
	return i;
}
int LexGetRangeLowered(Sci_Position startPos, LexAccessor &styler, bool IsWordChar(int), char *s, int len) {
	int i = 0;
	char ch = static_cast<char>(tolower(styler.SafeGetCharAt(startPos + i)));
	while ((i < len - 1) && IsWordChar(ch)) {
		s[i] = ch;
		i++;
		ch = static_cast<char>(tolower(styler.SafeGetCharAt(startPos + i)));
	}
	s[i] = '\0';
	return i;
}
int LexGetRange(Sci_Position startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, int len) {
	int i = 0;
	char ch = styler.SafeGetCharAt(startPos + i);
	while ((i < len - 1) && charSet.Contains(ch)) {
		s[i] = ch;
		i++;
		ch = styler.SafeGetCharAt(startPos + i);
	}
	s[i] = '\0';
	return i;
}
int LexGetRangeLowered(Sci_Position startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, int len) {
	int i = 0;
	char ch = static_cast<char>(tolower(styler.SafeGetCharAt(startPos + i)));
	while ((i < len - 1) && charSet.Contains(ch)) {
		s[i] = ch;
		i++;
		ch = static_cast<char>(tolower(styler.SafeGetCharAt(startPos + i)));
	}
	s[i] = '\0';
	return i;
}

char LexGetPrevChar(Sci_Position endPos, LexAccessor &styler) {
	while (IsASpaceOrTab(styler.SafeGetCharAt(--endPos, '\0')));
	return styler.SafeGetCharAt(endPos);
}
char LexGetNextChar(Sci_Position startPos, LexAccessor &styler) {
	while (IsASpaceOrTab(styler.SafeGetCharAt(startPos++, '\0')));
	return styler.SafeGetCharAt(startPos - 1);
}

#ifdef SCI_NAMESPACE
}
#endif
