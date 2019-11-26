// Scintilla source code edit control
/** @file LexAccessor.h
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla {

enum EncodingType { enc8bit, encUnicode, encDBCS };

class LexAccessor {
public:
	enum {
		extremePosition = 0x7FFFFFFF
	};
private:
	IDocument * const pAccess;
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
	const int codePage;
	const enum EncodingType encodingType;
	const Sci_Position lenDoc;
	unsigned char styleBuf[bufferSize];
	Sci_Position validLen;
	Sci_PositionU startSeg;
	Sci_Position startPosStyling;
	const int documentVersion;

	void Fill(Sci_Position position) noexcept {
		startPos = position - slopSize;
		if (startPos + bufferSize > lenDoc) {
			startPos = lenDoc - bufferSize;
		}
		if (startPos < 0) {
			startPos = 0;
		}
		endPos = startPos + bufferSize;
		if (endPos > lenDoc) {
			endPos = lenDoc;
		}

		pAccess->GetCharRange(buf, startPos, endPos - startPos);
		buf[endPos - startPos] = '\0';
	}

public:
	explicit LexAccessor(IDocument * pAccess_) noexcept :
		pAccess(pAccess_), startPos(extremePosition), endPos(0),
		codePage(pAccess->CodePage()),
		encodingType((codePage == 65001) ? encUnicode : (codePage ? encDBCS : enc8bit)),
		lenDoc(pAccess->Length()),
		validLen(0),
		startSeg(0),
		startPosStyling(0),
		documentVersion(pAccess->Version()) {
		// Prevent warnings by static analyzers about uninitialized buf and styleBuf.
		buf[0] = 0;
		styleBuf[0] = 0;
	}
	char operator[](Sci_Position position) noexcept {
		if (position < startPos || position >= endPos) {
			Fill(position);
		}
		return buf[position - startPos];
	}
	constexpr IDocument *MultiByteAccess() const noexcept {
		return pAccess;
	}
	/** Safe version of operator[], returning a defined value for invalid position. */
	char SafeGetCharAt(Sci_Position position) noexcept {
		if (position < startPos || position >= endPos) {
			Fill(position);
			if (position < startPos || position >= endPos) {
				// Position is outside range of document
				return '\0';
			}
		}
		return buf[position - startPos];
	}
	[[deprecated]]
	char SafeGetCharAt(Sci_Position position, char chDefault) noexcept {
		if (position < startPos || position >= endPos) {
			Fill(position);
			if (position < startPos || position >= endPos) {
				// Position is outside range of document
				return chDefault;
			}
		}
		return buf[position - startPos];
	}
	bool IsLeadByte(unsigned char ch) const noexcept {
		return encodingType == encDBCS && ch > 0x80 && pAccess->IsDBCSLeadByte(ch);
	}
	constexpr EncodingType Encoding() const noexcept {
		return encodingType;
	}

	bool Match(Sci_Position pos, const char *s) noexcept {
		for (; *s; s++, pos++) {
			if (*s != SafeGetCharAt(pos)) {
				return false;
			}
		}
		return true;
	}
	bool MatchIgnoreCase(Sci_Position pos, const char *s) noexcept;

	// Get first len - 1 characters in range [startPos_, endPos_).
	void GetRange(Sci_PositionU startPos_, Sci_PositionU endPos_, char *s, Sci_PositionU len) noexcept;
	void GetRangeLowered(Sci_PositionU startPos_, Sci_PositionU endPos_, char *s, Sci_PositionU len) noexcept;

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
	void Flush() {
		if (validLen > 0) {
			pAccess->SetStyles(validLen, styleBuf);
			startPosStyling += validLen;
			validLen = 0;
		}
	}
	int GetLineState(Sci_Position line) const noexcept {
		return pAccess->GetLineState(line);
	}
	int SetLineState(Sci_Position line, int state) {
		return pAccess->SetLineState(line, state);
	}
	// Style setting
	void StartAt(Sci_PositionU start) noexcept {
		pAccess->StartStyling(start);
		startPosStyling = start;
	}
	Sci_PositionU GetStartSegment() const noexcept {
		return startSeg;
	}
	void StartSegment(Sci_PositionU pos) noexcept {
		startSeg = pos;
	}
	void ColourTo(Sci_PositionU pos, int chAttr) {
		// Only perform styling if non empty range
		if (pos != startSeg - 1) {
			assert(pos >= startSeg);
			if (pos < startSeg) {
				return;
			}

			if (validLen + (pos - startSeg + 1) >= bufferSize) {
				Flush();
			}
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
	void SetLevel(Sci_Position line, int level) {
		pAccess->SetLevel(line, level);
	}
	void IndicatorFill(Sci_Position start, Sci_Position end, int indicator, int value) {
		pAccess->DecorationSetCurrentIndicator(indicator);
		pAccess->DecorationFillRange(start, value, end - start);
	}

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

class CharacterSet;

constexpr int MultiStyle(int style1, int style2) noexcept {
	return style1 | (style2 << 8);
}

constexpr bool IsSpaceOrTab(int ch) noexcept {
	return ch == ' ' || ch == '\t';
}

constexpr bool IsWhiteSpace(int ch) noexcept {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

bool IsLexCommentLine(Sci_Position line, LexAccessor &styler, int style) noexcept;

inline bool IsBackslashLine(Sci_Position line, LexAccessor &styler) noexcept {
	const Sci_Position pos = styler.LineStart(line + 1) - 1;
	return (pos >= 2) && (styler[pos] == '\n')
		&& (styler[pos - 1] == '\\' || (styler[pos - 1] == '\r' && styler[pos - 2] == '\\'));
}

bool IsLexLineStartsWith(Sci_Position line, LexAccessor &styler, const char *word, bool matchCase, int style) noexcept;

Sci_Position LexLineSkipSpaceTab(Sci_Position line, LexAccessor &styler) noexcept;

inline Sci_Position LexSkipSpaceTab(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler) noexcept {
	for (Sci_Position i = startPos; i < endPos; i++) {
		if (!IsSpaceOrTab(styler.SafeGetCharAt(i))) {
			return i;
		}
	}
	return endPos;
}

Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler) noexcept;
Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler, bool IsStreamCommentStyle(int) noexcept) noexcept;
Sci_Position LexSkipWhiteSpace(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler,
	bool IsStreamCommentStyle(int), const CharacterSet &charSet) noexcept;
bool IsLexSpaceToEOL(LexAccessor &styler, Sci_Position startPos) noexcept;
bool IsLexEmptyLine(LexAccessor &styler, Sci_Position line) noexcept;

Sci_PositionU LexGetRange(Sci_Position startPos, LexAccessor &styler, bool IsWordChar(int) noexcept, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRangeLowered(Sci_Position startPos, LexAccessor &styler, bool IsWordChar(int) noexcept, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRange(Sci_Position startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRangeLowered(Sci_Position startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept;

inline char LexGetPrevChar(Sci_Position endPos, LexAccessor &styler) noexcept {
	do {
		--endPos;
		const char ch = styler.SafeGetCharAt(endPos);
		if (!IsWhiteSpace(ch)) {
			return ch;
		}
	} while (true);
}

inline char LexGetNextChar(Sci_Position startPos, LexAccessor &styler) noexcept {
	do {
		const char ch = styler.SafeGetCharAt(startPos);
		if (!IsWhiteSpace(ch)) {
			return ch;
		}
		++startPos;
	} while (true);
}

inline char LexGetNextChar(Sci_Position startPos, Sci_Position endPos, LexAccessor &styler) noexcept {
	while (startPos < endPos) {
		const char ch = styler[startPos];
		if (!IsWhiteSpace(ch)) {
			return ch;
		}
		++startPos;
	}
	return '\0';
}

}
