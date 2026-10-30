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
		bufferSize = 4096,
	};
	Scintilla::IDocument * const pAccess;
	Scintilla::SplitView cbView;
	Scintilla::SplitView styleView;
	const EncodingType encodingType;
	unsigned char styleBuf[bufferSize];
	Sci_PositionU validLen = 0;
	Sci_PositionU startSeg = 0;
	Sci_Position startPosStyling = 0;

	static constexpr EncodingType EncodingTypeForCodePage(int codePage) noexcept {
		return (codePage == 65001) ? EncodingType::unicode : (codePage ? EncodingType::dbcs : EncodingType::eightBit);
	}

public:
	explicit LexAccessor(Scintilla::IDocument *pAccess_) noexcept :
		pAccess(pAccess_),
		encodingType(EncodingTypeForCodePage(pAccess->CodePage())) {
		pAccess->GetBufferView(cbView, styleView);
		memset(styleBuf, 0, sizeof(int));
	}
	char operator[](Sci_Position position) const noexcept {
		return cbView[position];
	}
	Scintilla::IDocument *MultiByteAccess() const noexcept {
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
	char SafeGetCharAt(Sci_Position position) const noexcept {
		return cbView.CharAt(position);
	}
	unsigned char SafeGetUCharAt(Sci_Position position) const noexcept {
		return SafeGetCharAt(position);
	}
#if 0
	[[deprecated]]
	char SafeGetCharAt(Sci_Position position, char chDefault) const noexcept {
		return cbView.CharAt(position, chDefault);
	}
	[[deprecated]]
	unsigned char SafeGetUCharAt(Sci_Position position, char chDefault) const noexcept {
		return SafeGetCharAt(position, chDefault);
	}
#endif
	bool IsLeadByte(unsigned char ch) const noexcept {
		return encodingType == EncodingType::dbcs && (ch & 0x80) != 0 && pAccess->IsDBCSLeadByte(ch);
	}
	EncodingType Encoding() const noexcept {
		return encodingType;
	}

	bool Match(Sci_Position pos, const char *s) const noexcept {
		for (; *s; s++, pos++) {
			if (*s != (*this)[pos]) {
				return false;
			}
		}
		return true;
	}
	bool MatchIgnoreCase(Sci_Position pos, const char *s) const noexcept;
	bool MatchLowerCase(Sci_Position pos, const char *s) const noexcept;

	// Get first len - 1 characters in range [startPos_, endPos_).
	void GetRange(Sci_PositionU startPos_, Sci_PositionU endPos_, char *s, Sci_PositionU len) const noexcept;
	void GetRangeLowered(Sci_PositionU startPos_, Sci_PositionU endPos_, char *s, Sci_PositionU len) const noexcept;
	// Get all characters in range [startPos_, endPos_).
	void GetRange(Sci_PositionU startPos_, Sci_PositionU endPos_, std::string &s) const;
	void GetRangeLowered(Sci_PositionU startPos_, Sci_PositionU endPos_, std::string &s) const;

	// Flush() must be called first when used in Colourise() or Lex() function.
	unsigned char StyleAt(Sci_Position position) const noexcept {
		return styleView.UCharAt(position);
	}
	unsigned char StyleIndexAt(Sci_Position position) const noexcept {
		return styleView.UCharAt(position);
	}
	// only used in Colourise() or Lex() function, validLen is always zero in Fold() function.
	// Return style value from buffer when in buffer, else retrieve from document.
	// This is faster and can avoid calls to Flush() as that may be expensive.
	unsigned char BufferStyleAt(Sci_Position position) const noexcept {
		const Sci_PositionU index = position - startPosStyling;
		if (index < validLen) {
			return styleBuf[index];
		}
		return StyleAt(position);
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
	Sci_Position Length() const noexcept {
		return cbView.length;
	}
	Sci_PositionU StyleEndPos(Sci_PositionU startPos_, Sci_PositionU length) const noexcept {
		length += startPos_;
		if (length == cbView.length) {
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
	void ColorTo(Sci_PositionU endPos_, unsigned chAttr);
	void SetLevel(Sci_Line line, int level) {
		pAccess->SetLevel(line, level);
	}
	// Avoids some overhead when level same as before
	void SetLevelIfDifferent(Sci_Line line, int level) {
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

// Fold level setting

constexpr int FoldLevelShift = 16;
constexpr int FoldLevelStart(int levelPrevious) noexcept {
	return levelPrevious >> FoldLevelShift;
}

constexpr int FoldLevelForCurrentNext(int levelCurrent, int levelNext) noexcept {
	return levelCurrent | (levelNext << FoldLevelShift);
}

// Where lexer uses current/next but only has one value, commonly at end of range
constexpr int FoldLevelForCurrent(int levelCurrent) noexcept {
	return FoldLevelForCurrentNext(levelCurrent, levelCurrent);
}

constexpr int FoldLevelFlags(int levelLine, int levelNext, bool white, bool headerPermitted = true) noexcept {
	int flags = 0;
	if (white) {
		flags |= SC_FOLDLEVELWHITEFLAG;
	}
	if ((levelLine < levelNext) && headerPermitted) {
		flags |= SC_FOLDLEVELHEADERFLAG;
	}
	return flags;
}

constexpr int MultiStyle(int style1, int style2) noexcept {
	return style1 | (style2 << 8);
}

constexpr bool IsWhiteSpace(int ch) noexcept {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}

bool IsLexCommentLine(const LexAccessor &styler, Sci_Line line, int style) noexcept;

inline bool IsBackslashLine(const LexAccessor &styler, Sci_Line line) noexcept {
#if 1
	const Sci_Position pos = styler.LineStart(line + 1) - 1;
	return (pos >= 2) && (styler[pos] == '\n')
		&& (styler[pos - 1] == '\\' || (styler[pos - 1] == '\r' && styler[pos - 2] == '\\'));
#else
	const Sci_Position pos = styler.LineEnd(line);
	return pos > 0 && styler[pos - 1] == '\\';
#endif
}

bool IsLexLineStartsWith(const LexAccessor &styler, Sci_Line line, const char *word, bool matchCase, int style) noexcept;

Sci_Position LexLineSkipSpaceTab(const LexAccessor &styler, Sci_Line line) noexcept;

inline Sci_Position LexSkipSpaceTab(const LexAccessor &styler, Sci_Position startPos, Sci_Position endPos) noexcept {
	for (; startPos < endPos; startPos++) {
		const char ch = styler.SafeGetCharAt(startPos);
		if (ch != ' ' && ch != '\t') {
			break;
		}
	}
	return startPos;
}

Sci_Position LexSkipWhiteSpace(const LexAccessor &styler, Sci_Position startPos, Sci_Position endPos) noexcept;
Sci_Position LexSkipWhiteSpace(const LexAccessor &styler, Sci_Position startPos, Sci_Position endPos, bool IsStreamCommentStyle(int) noexcept) noexcept;
Sci_Position LexSkipWhiteSpace(const LexAccessor &styler, Sci_Position startPos, Sci_Position endPos,
	bool IsStreamCommentStyle(int), const CharacterSet &charSet) noexcept;
bool IsLexSpaceToEOL(const LexAccessor &styler, Sci_Position startPos) noexcept;
bool IsLexEmptyLine(const LexAccessor &styler, Sci_Line line) noexcept;

Sci_PositionU LexGetRange(const LexAccessor &styler, Sci_Position startPos, bool IsWordChar(int) noexcept, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRangeLowered(const LexAccessor &styler, Sci_Position startPos, bool IsWordChar(int) noexcept, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRange(const LexAccessor &styler, Sci_Position startPos, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept;
Sci_PositionU LexGetRangeLowered(const LexAccessor &styler, Sci_Position startPos, const CharacterSet &charSet, char *s, Sci_PositionU len) noexcept;

inline unsigned char LexGetPrevChar(const LexAccessor &styler, Sci_Position endPos) noexcept {
	do {
		--endPos;
		const unsigned char ch = styler.SafeGetCharAt(endPos);
		if (!IsWhiteSpace(ch)) {
			return ch;
		}
	} while (true);
}

inline unsigned char LexGetNextChar(const LexAccessor &styler, Sci_Position startPos) noexcept {
	do {
		const unsigned char ch = styler.SafeGetCharAt(startPos);
		if (!IsWhiteSpace(ch)) {
			return ch;
		}
		++startPos;
	} while (true);
}

inline unsigned char LexGetNextChar(const LexAccessor &styler, Sci_Position startPos, Sci_Position endPos) noexcept {
	for (; startPos < endPos; startPos++) {
		const unsigned char ch = styler[startPos];
		if (!IsWhiteSpace(ch)) {
			return ch;
		}
	}
	return '\0';
}

struct MatchedDelimiterCount {
	int count;
	int chNext;
};

inline MatchedDelimiterCount GetMatchedDelimiterCountEx(const LexAccessor &styler, Sci_PositionU pos, int delimiter) noexcept {
	int count = 0;
	int chNext;
	do {
		++count;
		++pos;
		chNext = styler.SafeGetUCharAt(pos);
	} while (chNext == delimiter);
	return {count, chNext};
}

inline int GetMatchedDelimiterCount(const LexAccessor &styler, Sci_PositionU pos, int delimiter) noexcept {
	int count = 0;
	int chNext;
	do {
		++count;
		++pos;
		chNext = styler.SafeGetUCharAt(pos);
	} while (chNext == delimiter);
	return count;
}

void BacktrackToStart(const LexAccessor &styler, int stateMask, Sci_PositionU &startPos, Sci_Position &lengthDoc, int &initStyle) noexcept;
Sci_PositionU LookbackNonWhite(const LexAccessor &styler, Sci_PositionU startPos, unsigned maxSpaceStyle, int &chPrevNonWhite, int &stylePrevNonWhite) noexcept;
Sci_PositionU CheckBraceOnNextLine(const LexAccessor &styler, Sci_Line line, int operatorStyle, int maxSpaceStyle, int ignoreStyle = 0) noexcept;

}
