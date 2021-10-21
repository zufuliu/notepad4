// Scintilla source code edit control
/** @file StyleContext.h
 ** Lexer infrastructure.
 **/
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// This file is in the public domain.
#pragma once

namespace Lexilla {

// All languages handled so far can treat all characters >= 0x80 as one class
// which just continues the current token or starts an identifier if in default.
// DBCS treated specially as the second character can be < 0x80 and hence
// syntactically significant. UTF-8 avoids this as all trail bytes are >= 0x80
class StyleContext final {
public:
	LexAccessor &styler;
private:
	const Sci_PositionU endPos;
#if 0
	// Used for optimizing GetRelativeCharacter
	Sci_PositionU posRelative = 0;
	Sci_PositionU currentPosLastRelative = SIZE_MAX;
	Sci_Position offsetRelative = 0;
#endif

	void GetNextChar() noexcept {
		if (!multiByteAccess) {
			chNext = static_cast<unsigned char>(styler.SafeGetCharAt(currentPos + 1));
		} else {
			chNext = styler.GetCharacterAndWidth(currentPos + width, &widthNext);
		}
		// End of line determined from line end position, allowing CR, LF,
		// CRLF and Unicode line ends as set by document.
		atLineEnd = currentPos >= lineStartNext - (currentLine < lineDocEnd);
	}

public:
	Sci_PositionU currentPos;
	Sci_Line currentLine;
	const Sci_Line lineDocEnd;
	//Sci_PositionU lineEnd;
	Sci_PositionU lineStartNext;
	const bool multiByteAccess;
	bool atLineStart;
	bool atLineEnd;
	int state;
	int chPrev;
	int ch;
	int chNext;
	Sci_Position width = 1;
	Sci_Position widthNext = 1;

	StyleContext(Sci_PositionU startPos, Sci_PositionU length,
		int initStyle, LexAccessor &styler_) noexcept :
	styler(styler_),
	endPos(styler.StyleEndPos(startPos, length)),
	currentLine(styler.GetLine(startPos)),
	lineDocEnd(styler.GetLine(styler.Length())),
	multiByteAccess(styler.Encoding() == EncodingType::dbcs),
	state(initStyle) {
		styler.StartAt(startPos);
		styler.StartSegment(startPos);
		//lineEnd = styler.LineEnd(currentLine);
		lineStartNext = styler.LineStart(currentLine + 1);
		atLineStart = static_cast<Sci_PositionU>(styler.LineStart(currentLine)) == startPos;
		SeekTo(startPos);
	}
	// Deleted so StyleContext objects can not be copied.
	StyleContext(const StyleContext &) = delete;
	StyleContext(StyleContext &&) = delete;
	StyleContext &operator=(const StyleContext &) = delete;
	StyleContext &operator=(StyleContext &&) = delete;
	void Complete() {
#ifndef NDEBUG
		styler.ColorTo(sci::min<Sci_PositionU>(currentPos, styler.Length()), state);
#else
		styler.ColorTo(currentPos, state);
#endif
		styler.Flush();
	}
	bool More() const noexcept {
		return currentPos < endPos;
	}
	void Forward() noexcept {
		if (currentPos < endPos) {
			atLineStart = atLineEnd;
			if (atLineStart) {
				currentLine++;
				//lineEnd = styler.LineEnd(currentLine);
				lineStartNext = styler.LineStart(currentLine + 1);
			}
			chPrev = ch;
			currentPos += width;
			ch = chNext;
			width = widthNext;
			GetNextChar();
		} else {
			atLineStart = false;
			atLineEnd = true;
			chPrev = 0;
			ch = 0;
			chNext = 0;
		}
	}
	void Forward(Sci_Position nb) noexcept {
		for (Sci_Position i = 0; i < nb; i++) {
			Forward();
		}
	}
	void ForwardBytes(Sci_Position nb) noexcept {
		const Sci_PositionU forwardPos = currentPos + nb;
		while (forwardPos > currentPos) {
			const Sci_PositionU currentPosStart = currentPos;
			Forward();
			if (currentPos == currentPosStart) {
				// Reached end
				return;
			}
		}
	}
	void ChangeState(int state_) noexcept {
		state = state_;
	}
	void SetState(int state_) {
#ifndef NDEBUG
		styler.ColorTo(sci::min<Sci_PositionU>(currentPos, styler.Length()), state);
#else
		styler.ColorTo(currentPos, state);
#endif
		state = state_;
	}
	void ForwardSetState(int state_) {
		Forward();
		SetState(state_);
	}
	Sci_Position LengthCurrent() const noexcept {
		return currentPos - styler.GetStartSegment();
	}
	int GetRelative(Sci_Position n) const noexcept {
		return static_cast<unsigned char>(styler.SafeGetCharAt(currentPos + n));
	}
#if 0
	[[deprecated]]
	int GetRelative(Sci_Position n, char chDefault) const noexcept {
		return static_cast<unsigned char>(styler.SafeGetCharAt(currentPos + n, chDefault));
	}
#endif
	int GetRelativeCharacter(Sci_Position n) noexcept {
		if (n == 0) {
			return ch;
		}
#if 0
		if (multiByteAccess) {
			if ((currentPosLastRelative != currentPos) ||
				((n > 0) && ((offsetRelative < 0) || (n < offsetRelative))) ||
				((n < 0) && ((offsetRelative > 0) || (n > offsetRelative)))) {
				posRelative = currentPos;
				offsetRelative = 0;
			}
			const Sci_Position diffRelative = n - offsetRelative;
			const Sci_Position posNew = styler.GetRelativePosition(posRelative, diffRelative);
			const int chReturn = styler.GetCharacterAndWidth(posNew, nullptr);
			posRelative = posNew;
			currentPosLastRelative = currentPos;
			offsetRelative = n;
			return chReturn;
		}
#endif
		// fast version for single byte encodings
		return static_cast<unsigned char>(styler.SafeGetCharAt(currentPos + n));
	}
	bool AtDocumentEnd() const noexcept {
		return currentPos == static_cast<Sci_PositionU>(styler.Length());
	}
	bool MatchLineEnd() const noexcept {
		//return currentPos == lineEnd;
		return atLineEnd;
	}
#if 0
	[[deprecated]]
	bool Match(char ch0) const noexcept {
		return ch == static_cast<unsigned char>(ch0);
	}
#endif
	bool Match(char ch0, char ch1) const noexcept {
		return (ch == static_cast<unsigned char>(ch0)) && (chNext == static_cast<unsigned char>(ch1));
	}
	bool Match(char ch0, char ch1, char ch2) const noexcept {
		return Match(ch0, ch1) && ch2 == styler.SafeGetCharAt(currentPos + 2);
	}
	bool Match(char ch0, char ch1, char ch2, char ch3) const noexcept {
		return Match(ch0, ch1, ch2) && ch3 == styler.SafeGetCharAt(currentPos + 3);
	}

	bool MatchNext(char ch0, char ch1) const noexcept {
		return chNext == static_cast<unsigned char>(ch0) && ch1 == styler.SafeGetCharAt(currentPos + 2);
	}
	bool MatchNext(char ch0, char ch1, char ch2) const noexcept {
		return MatchNext(ch0, ch1) && ch2 == styler.SafeGetCharAt(currentPos + 3);
	}

	bool Match(const char *s) const noexcept {
		if (ch != static_cast<unsigned char>(*s)) {
			return false;
		}
		s++;
		if (!*s) {
			return true;
		}
		if (chNext != static_cast<unsigned char>(*s)) {
			return false;
		}
		s++;
		for (Sci_PositionU pos = currentPos + 2; *s; s++, pos++) {
			if (*s != styler.SafeGetCharAt(pos)) {
				return false;
			}
		}
		return true;
	}
	bool MatchIgnoreCase(const char *s) const noexcept;

	void GetCurrent(char *s, Sci_PositionU len) const noexcept {
		styler.GetRange(styler.GetStartSegment(), currentPos, s, len);
	}
	void GetCurrentLowered(char *s, Sci_PositionU len) const noexcept {
		styler.GetRangeLowered(styler.GetStartSegment(), currentPos, s, len);
	}

	void SeekTo(Sci_PositionU startPos) noexcept {
		currentPos = startPos;
		chPrev = 0;
		if (!multiByteAccess) {
			ch = static_cast<unsigned char>(styler[startPos]);
			chNext = static_cast<unsigned char>(styler.SafeGetCharAt(startPos + 1));
		} else {
			ch =  styler.GetCharacterAndWidth(startPos, &widthNext);
			width = widthNext;
			chNext = styler.GetCharacterAndWidth(startPos + width, &widthNext);
		}
		atLineEnd = startPos >= lineStartNext - (currentLine < lineDocEnd);
	}

	void Rewind() noexcept {
		SeekTo(styler.GetStartSegment());
	}

	void BackTo(Sci_PositionU startPos) {
		assert(startPos < styler.GetStartSegment());
		styler.Flush();
		styler.StartAt(startPos);
		styler.StartSegment(startPos);
		SeekTo(startPos);
	}

	void Advance(Sci_Position nb) noexcept {
		if (nb) {
			SeekTo(currentPos + nb);
		}
	}

	bool LineEndsWith(char ch0) const noexcept {
		return chPrev == static_cast<unsigned char>(ch0)
			|| (chPrev == '\r' && ch == '\n' && currentPos >= 2 && ch0 == styler[currentPos - 2]);
	}

	int GetLineLastChar() const noexcept {
		if (chPrev == '\r' && ch == '\n' && currentPos >= 2) {
			return static_cast<unsigned char>(styler[currentPos - 2]);
		}
		return chPrev;
	}

	int GetDocNextChar(bool ignoreCurrent = false) const noexcept {
		if (!ignoreCurrent && !IsWhiteSpace(ch)) {
			return ch;
		}
		if (!IsWhiteSpace(chNext)) {
			return chNext;
		}
		return LexGetNextChar(currentPos + 2, styler);
	}

	int GetLineNextChar(bool ignoreCurrent = false) const noexcept {
		if (!ignoreCurrent && !IsWhiteSpace(ch)) {
			return ch;
		}
		if (currentPos + 1 == lineStartNext) {
			return '\0';
		}
		if (!IsWhiteSpace(chNext)) {
			return chNext;
		}
		return LexGetNextChar(currentPos + 2, lineStartNext, styler);
	}
};

bool HighlightTaskMarker(StyleContext &sc, int &visibleChars, int visibleCharsBefore, int markerStyle);

}
