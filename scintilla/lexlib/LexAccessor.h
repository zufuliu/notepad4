// Scintilla source code edit control
/** @file LexAccessor.h
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef LEXACCESSOR_H
#define LEXACCESSOR_H

namespace Scintilla {

class CharacterSet;
class LexAccessor;

#define MultiStyle(stl1, stl2) \
	((stl1) | ((stl2) << 8))
#define MultiStyle4(stl1, stl2, stl3, stl4) \
	((stl1) | ((stl2) << 8) | ((stl3) << 16) | ((stl4) << 24))
bool IsLexCommentLine(Sci_Position line, LexAccessor &styler, int style) noexcept;

bool IsBackslashLine(Sci_Position line, LexAccessor &styler) noexcept;

bool IsLexLineStartsWith(Sci_Position line, LexAccessor &styler, const char* word, bool matchCase, int style) noexcept;

Sci_Position LexLineSkipSpaceTab(Sci_Position line, LexAccessor &styler) noexcept;
Sci_Position LexSkipSpaceTab  (Sci_Position startPos, Sci_Position endPos, LexAccessor &styler) noexcept;
Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler) noexcept;
Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, bool IsStreamCommentStyle(int)) noexcept;
Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler,
											bool IsStreamCommentStyle(int), const CharacterSet &charSet) noexcept;
bool IsLexSpaceToEOL(LexAccessor &styler, Sci_Position startPos) noexcept;
bool IsLexEmptyLine(LexAccessor &styler, Sci_Position line) noexcept;

Sci_PositionU LexGetRange			(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRangeLowered	(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRange			(Sci_Position startPos, LexAccessor &styler, bool IsWordChar(int), char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRangeLowered	(Sci_Position startPos, LexAccessor &styler, bool IsWordChar(int), char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRange			(Sci_Position startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRangeLowered	(Sci_Position startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept;

char LexGetPrevChar     (Sci_Position endPos, LexAccessor &styler) noexcept;
char LexGetNextChar		(Sci_Position startPos, LexAccessor &styler) noexcept;
bool IsLexAtEOL			(Sci_Position pos, LexAccessor &styler) noexcept;
bool LexMatch			(Sci_Position pos, LexAccessor &styler, const char *s) noexcept;
bool LexMatchIgnoreCase (Sci_Position pos, LexAccessor &styler, const char *s) noexcept;

enum EncodingType { enc8bit, encUnicode, encDBCS };

class LexAccessor {
public:
	enum {
		extremePosition = 0x7FFFFFFF
	};
private:
	IDocument * pAccess;
	/** @a bufferSize is a trade off between time taken to copy the characters
	 * and retrieval overhead.
	 * @a slopSize positions the buffer before the desired position
	 * in case there is some backtracking. */
	enum {
		bufferSize = 4096, slopSize = bufferSize / 8
	};
	char buf[bufferSize + 1];
	Sci_Position startPos;
	Sci_Position endPos;
	int codePage;
	enum EncodingType encodingType;
	Sci_Position lenDoc;
	unsigned char styleBuf[bufferSize];
	Sci_Position validLen;
	Sci_PositionU startSeg;
	Sci_Position startPosStyling;
	int documentVersion;

	void Fill(Sci_Position position) noexcept;

public:
	explicit LexAccessor(IDocument *pAccess_) noexcept;
	char operator[](Sci_Position position) noexcept;
	constexpr IDocument *MultiByteAccess() const noexcept {
		return pAccess;
	}
	/** Safe version of operator[], returning a defined value for invalid position. */
	char SafeGetCharAt(Sci_Position position) noexcept;
	bool IsLeadByte(char ch) const noexcept {
		return pAccess->IsDBCSLeadByte(ch);
	}
	constexpr EncodingType Encoding() const noexcept {
		return encodingType;
	}

	bool Match(Sci_Position pos, const char *s) noexcept {
		return LexMatch(pos, *this, s);
	}
	unsigned char StyleAt(Sci_Position position) const noexcept {
		return pAccess->StyleAt(position);
	}
	Sci_Position GetLine(Sci_Position position) const noexcept {
		return pAccess->LineFromPosition(position);
	}
	Sci_Position LineStart(Sci_Position line) const noexcept {
		return pAccess->LineStart(line);
	}
	Sci_Position LineEnd(Sci_Position line) const noexcept {
		return pAccess->LineEnd(line);
	}
	int LevelAt(Sci_Position line) const noexcept {
		return pAccess->GetLevel(line);
	}
	constexpr Sci_Position Length() const noexcept {
		return lenDoc;
	}
	void Flush();
	int GetLineState(Sci_Position line) const {
		return pAccess->GetLineState(line);
	}
	int SetLineState(Sci_Position line, int state) {
		return pAccess->SetLineState(line, state);
	}
	// Style setting
	void StartAt(Sci_PositionU start) noexcept;
	Sci_PositionU GetStartSegment() const noexcept {
		return startSeg;
	}
	void StartSegment(Sci_PositionU pos) noexcept {
		startSeg = pos;
	}
	void ColourTo(Sci_PositionU pos, int chAttr);
	void SetLevel(Sci_Position line, int level) {
		pAccess->SetLevel(line, level);
	}
	void IndicatorFill(Sci_Position start, Sci_Position end, int indicator, int value);

	void ChangeLexerState(Sci_Position start, Sci_Position end) {
		pAccess->ChangeLexerState(start, end);
	}
};

struct LexicalClass {
	int value;
	const char *name;
	const char *tags;
	const char *description;
};

}

#endif
