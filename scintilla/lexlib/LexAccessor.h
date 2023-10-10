// Scintilla source code edit control
/** @file LexAccessor.h
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Lexilla {

// same as EncodingFamily in Document.h
enum class EncodingType { eightBit, unicode, dbcs };

class LexAccessor {
	enum {
		extremePosition = 0x7FFFFFFF
	};
	Scintilla::IDocument * const pAccess;
	/** @a bufferSize is a trade off between time taken to copy the characters
	 * and retrieval overhead.
	 * @a slopSize positions the buffer before the desired position
	 * in case there is some backtracking. */
	enum {
		bufferSize = 4096,
		slopSize = bufferSize / 8,
	};
	char buf[bufferSize + 4];
	const EncodingType encodingType;
	Sci_Position startPos = 0;
	Sci_Position endPos = 0;
	//const int codePage;
	//const int documentVersion;
	const Sci_Position lenDoc;
	unsigned char styleBuf[bufferSize];
	Sci_Position validLen = 0;
	Sci_PositionU startSeg = 0;
	Sci_Position startPosStyling = 0;

	void Fill(Sci_Position position) noexcept {
		Sci_Position m = lenDoc - bufferSize;
		startPos = position - slopSize;
		startPos = sci::min(startPos, m);
		startPos = sci::max<Sci_Position>(startPos, 0);
		endPos = startPos + bufferSize;
		endPos = sci::min(endPos, lenDoc);

		m = endPos - startPos;
		pAccess->GetCharRange(buf, startPos, m);
		buf[m] = '\0';
	}

	static constexpr EncodingType EncodingTypeForCodePage(int codePage) noexcept {
		return (codePage == 65001) ? EncodingType::unicode : (codePage ? EncodingType::dbcs : EncodingType::eightBit);
	}

public:
	explicit LexAccessor(Scintilla::IDocument *pAccess_) noexcept :
		pAccess(pAccess_),
		//codePage(pAccess->CodePage()),
		//documentVersion(pAccess->Version()),
		encodingType(EncodingTypeForCodePage(pAccess->CodePage())),
		lenDoc(pAccess->Length()) {
		// Prevent warnings by static analyzers about uninitialized buf and styleBuf.
		// zero unused padding to prevent potential out of bounds bug.
		memset(buf, 0, 4);
		memset(buf + bufferSize, 0, 4);
		memset(styleBuf, 0, 4);
	}
	char operator[](Sci_Position position) noexcept {
		if (position < startPos || position >= endPos) {
			Fill(position);
		}
		return buf[position - startPos];
	}
	constexpr Scintilla::IDocument *MultiByteAccess() const noexcept {
		return pAccess;
	}
	Sci_Position GetRelativePosition(Sci_Position positionStart, Sci_Position characterOffset) const noexcept {
		return pAccess->GetRelativePosition(positionStart, characterOffset);
	}
	int GetCharacterAndWidth(Sci_Position position, Sci_Position *pWidth = nullptr) const noexcept {
		return pAccess->GetCharacterAndWidth(position, pWidth);
	}
	int GetCharacterAt(Sci_Position position) const noexcept {
		return pAccess->GetCharacterAndWidth(position, nullptr);
	}
	Scintilla::CharacterClass GetCharacterClass(unsigned int character) const noexcept {
		// NOTE: '_' is classified as word in CharClassify::SetDefaultCharClasses()
		return pAccess->GetCharacterClass(character);
	}

	/** Safe version of operator[], returning a defined value for invalid position. */
	char SafeGetCharAt(Sci_Position position) noexcept {
		if (position < startPos || position >= endPos) {
			Fill(position);
			if (position < startPos || position >= endPos) {
				// Position is outside range of document
				//! different from official Lexilla which returns space.
				return '\0';
			}
		}
		return buf[position - startPos];
	}
	unsigned char SafeGetUCharAt(Sci_Position position) noexcept {
		return SafeGetCharAt(position);
	}
#if 0
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
	unsigned char SafeGetUCharAt(Sci_Position position, char chDefault) noexcept {
		return SafeGetCharAt(position, chDefault);
	}
#endif
	bool IsLeadByte(unsigned char ch) const noexcept {
		return encodingType == EncodingType::dbcs && (ch & 0x80) != 0 && pAccess->IsDBCSLeadByte(ch);
	}
	constexpr EncodingType Encoding() const noexcept {
		return encodingType;
	}

	bool Match(Sci_Position pos, const char *s) noexcept {
		for (; *s; s++, pos++) {
			if (*s != (*this)[pos]) {
				return false;
			}
		}
		return true;
	}
	bool MatchIgnoreCase(Sci_Position pos, const char *s) noexcept;
	bool MatchLowerCase(Sci_Position pos, const char *s) noexcept;

	// Get first len - 1 characters in range [startPos_, endPos_).
	void GetRange(Sci_PositionU startPos_, Sci_PositionU endPos_, char *s, Sci_PositionU len) noexcept;
	void GetRangeLowered(Sci_PositionU startPos_, Sci_PositionU endPos_, char *s, Sci_PositionU len) noexcept;
	// Get all characters in range [startPos_, endPos_).
	std::string GetRange(Sci_PositionU startPos_, Sci_PositionU endPos_);
	std::string GetRangeLowered(Sci_PositionU startPos_, Sci_PositionU endPos_);

	// Flush() must be called first when used in Colourise() or Lex() function.
	unsigned char StyleAt(Sci_Position position) const noexcept {
		return pAccess->StyleAt(position);
	}
	unsigned char StyleIndexAt(Sci_Position position) const noexcept {
		return pAccess->StyleAt(position);
	}
	// only used in Colourise() or Lex() function, validLen is always zero in Fold() function.
	// Return style value from buffer when in buffer, else retrieve from document.
	// This is faster and can avoid calls to Flush() as that may be expensive.
	unsigned char BufferStyleAt(Sci_Position position) const noexcept {
		const Sci_Position index = position - startPosStyling;
		if (index >= 0 && index < validLen) {
			return styleBuf[index];
		}
		return pAccess->StyleAt(position);
	}

	Sci_Line GetLine(Sci_Position position) const noexcept {
		return pAccess->LineFromPosition(position);
	}
	Sci_Position LineStart(Sci_Line line) const noexcept {
		return pAccess->LineStart(line);
	}
	Sci_Position LineEnd(Sci_Line line) const noexcept {
		return pAccess->LineEnd(line);
	}
	int LevelAt(Sci_Line line) const noexcept {
		return pAccess->GetLevel(line);
	}
	constexpr Sci_Position Length() const noexcept {
		return lenDoc;
	}
	Sci_PositionU StyleEndPos(Sci_PositionU startPos_, Sci_PositionU length) const noexcept {
		length += startPos_;
		if (length == static_cast<Sci_PositionU>(lenDoc)) {
			++length;
		}
		return length;
	}
	void Flush() {
		if (validLen > 0) {
			pAccess->SetStyles(validLen, styleBuf);
			startPosStyling += validLen;
			validLen = 0;
		}
	}
	int GetLineState(Sci_Line line) const noexcept {
		return pAccess->GetLineState(line);
	}
	int SetLineState(Sci_Line line, int state) {
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
#if 0
	[[deprecated]]
	void ColourTo(Sci_PositionU pos, int chAttr) {
		ColorTo(pos + 1, chAttr);
	}
#endif
	void ColorTo(Sci_PositionU endPos_, int chAttr) {
		// Only perform styling if non empty range
		assert(endPos_ <= static_cast<Sci_PositionU>(Length()));
		if (endPos_ > startSeg) {
			const Sci_PositionU len = endPos_ - startSeg;
			if (validLen + len >= bufferSize) {
				Flush();
			}
			const auto attr = static_cast<unsigned char>(chAttr);
			if (validLen + len >= bufferSize) {
				// Too big for buffer so send directly
				pAccess->SetStyleFor(len, attr);
			} else {
				for (Sci_PositionU i = 0; i < len; i++) {
					assert((startPosStyling + validLen) < Length());
					styleBuf[validLen++] = attr;
				}
			}
		}
		startSeg = endPos_;
	}
	void SetLevel(Sci_Line line, int level) {
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

constexpr bool IsWhiteSpace(int ch) noexcept {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

bool IsLexCommentLine(LexAccessor &styler, Sci_Line line, int style) noexcept;

inline bool IsBackslashLine(LexAccessor &styler, Sci_Line line) noexcept {
#if 1
	const Sci_Position pos = styler.LineStart(line + 1) - 1;
	return (pos >= 2) && (styler[pos] == '\n')
		&& (styler[pos - 1] == '\\' || (styler[pos - 1] == '\r' && styler[pos - 2] == '\\'));
#else
	const Sci_Position pos = styler.LineEnd(line);
	return pos > 0 && styler[pos - 1] == '\\';
#endif
}

bool IsLexLineStartsWith(LexAccessor &styler, Sci_Line line, const char *word, bool matchCase, int style) noexcept;

Sci_Position LexLineSkipSpaceTab(LexAccessor &styler, Sci_Line line) noexcept;

inline Sci_Position LexSkipSpaceTab(LexAccessor &styler, Sci_Position startPos, Sci_Position endPos) noexcept {
	for (; startPos < endPos; startPos++) {
		const char ch = styler.SafeGetCharAt(startPos);
		if (!(ch == ' ' || ch == '\t')) {
			break;
		}
	}
	return startPos;
}

Sci_Position LexSkipWhiteSpace(LexAccessor &styler, Sci_Position startPos, Sci_Position endPos) noexcept;
Sci_Position LexSkipWhiteSpace(LexAccessor &styler, Sci_Position startPos, Sci_Position endPos, bool IsStreamCommentStyle(int) noexcept) noexcept;
Sci_Position LexSkipWhiteSpace(LexAccessor &styler, Sci_Position startPos, Sci_Position endPos,
	bool IsStreamCommentStyle(int), const CharacterSet &charSet) noexcept;
bool IsLexSpaceToEOL(LexAccessor &styler, Sci_Position startPos) noexcept;
bool IsLexEmptyLine(LexAccessor &styler, Sci_Line line) noexcept;

Sci_PositionU LexGetRange(LexAccessor &styler, Sci_Position startPos, bool IsWordChar(int) noexcept, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRangeLowered(LexAccessor &styler, Sci_Position startPos, bool IsWordChar(int) noexcept, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRange(LexAccessor &styler, Sci_Position startPos, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRangeLowered(LexAccessor &styler, Sci_Position startPos, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept;

inline unsigned char LexGetPrevChar(LexAccessor &styler, Sci_Position endPos) noexcept {
	do {
		--endPos;
		const unsigned char ch = styler.SafeGetCharAt(endPos);
		if (!IsWhiteSpace(ch)) {
			return ch;
		}
	} while (true);
}

inline unsigned char LexGetNextChar(LexAccessor &styler, Sci_Position startPos) noexcept {
	do {
		const unsigned char ch = styler.SafeGetCharAt(startPos);
		if (!IsWhiteSpace(ch)) {
			return ch;
		}
		++startPos;
	} while (true);
}

inline unsigned char LexGetNextChar(LexAccessor &styler, Sci_Position startPos, Sci_Position endPos) noexcept {
	for (; startPos < endPos; startPos++) {
		const unsigned char ch = styler[startPos];
		if (!IsWhiteSpace(ch)) {
			return ch;
		}
	}
	return '\0';
}

inline int GetMatchedDelimiterCount(LexAccessor &styler, Sci_PositionU pos, int delimiter) noexcept {
	int count = 1;
	while (true) {
		const uint8_t ch = styler.SafeGetCharAt(++pos);
		if (ch == delimiter) {
			++count;
		} else {
			break;
		}
	}
	return count;
}

void BacktrackToStart(const LexAccessor &styler, int stateMask, Sci_PositionU &startPos, Sci_Position &lengthDoc, int &initStyle) noexcept;
Sci_PositionU LookbackNonWhite(LexAccessor &styler, Sci_PositionU startPos, unsigned maxSpaceStyle, int &chPrevNonWhite, int &stylePrevNonWhite) noexcept;
Sci_PositionU CheckBraceOnNextLine(LexAccessor &styler, Sci_Line line, int operatorStyle, int maxSpaceStyle, int ignoreStyle = 0) noexcept;

}
