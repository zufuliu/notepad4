// Scintilla source code edit control
/** @file StyleContext.h
 ** Lexer infrastructure.
 **/
// Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// This file is in the public domain.

#ifndef STYLECONTEXT_H
#define STYLECONTEXT_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

// All languages handled so far can treat all characters >= 0x80 as one class
// which just continues the current token or starts an identifier if in default.
// DBCS treated specially as the second character can be < 0x80 and hence
// syntactically significant. UTF-8 avoids this as all trail bytes are >= 0x80
class StyleContext {
public:
	LexAccessor &styler;
private:
	IDocument *multiByteAccess;
	Sci_PositionU endPos;
	Sci_PositionU lengthDocument;

	// Used for optimizing GetRelativeCharacter
	Sci_PositionU posRelative;
	Sci_PositionU currentPosLastRelative;
	Sci_Position offsetRelative;

	void GetNextChar();

public:
	Sci_PositionU currentPos;
	Sci_Position currentLine;
	Sci_Position lineDocEnd;
	Sci_Position lineStartNext;
	bool atLineStart;
	bool atLineEnd;
	int state;
	int chPrev;
	int ch;
	Sci_Position width;
	int chNext;
	Sci_Position widthNext;

	StyleContext(Sci_PositionU startPos, Sci_PositionU length,
				int initStyle, LexAccessor &styler_, unsigned char chMask='\377');
	// Deleted so StyleContext objects can not be copied.
	StyleContext(const StyleContext &) = delete;
	StyleContext &operator=(const StyleContext &) = delete;
	void Complete();
	bool More() const {
		return currentPos < endPos;
	}
	void Forward();
	void Forward(Sci_Position nb);
	void ForwardBytes(Sci_Position nb);
	void ChangeState(int state_) {
		state = state_;
	}
	void SetState(int state_);
	void ForwardSetState(int state_);
	Sci_Position LengthCurrent() const {
		return currentPos - styler.GetStartSegment();
	}
	int GetRelative(Sci_Position n) const {
		return static_cast<unsigned char>(styler.SafeGetCharAt(currentPos+n));
	}
	int GetRelativeCharacter(Sci_Position n);
	bool Match(char ch0) const {
		return ch == static_cast<unsigned char>(ch0);
	}
	bool Match(char ch0, char ch1) const {
		return (ch == static_cast<unsigned char>(ch0)) && (chNext == static_cast<unsigned char>(ch1));
	}
	bool Match(const char *s) const {
        return LexMatch(currentPos, styler, s);
	}
	bool MatchIgnoreCase(const char *s) const {
		return LexMatchIgnoreCase(currentPos, styler, s);
	}
	// Non-inline
	Sci_Position GetCurrent(char *s, Sci_PositionU len) const {
		return LexGetRange(styler.GetStartSegment(), currentPos - 1, styler, s, len);
	}
	Sci_Position GetCurrentLowered(char *s, Sci_PositionU len) const {
		return LexGetRangeLowered(styler.GetStartSegment(), currentPos - 1, styler, s, len);
	}
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
