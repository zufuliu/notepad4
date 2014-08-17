// Scintilla source code edit control
/** @file LexAccessor.h
 ** Interfaces between Scintilla and lexers.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef LEXACCESSOR_H
#define LEXACCESSOR_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

class CharacterSet;
class LexAccessor;

#define MultiStyle(stl1, stl2) \
	((stl1) | ((stl2) << 8))
#define MultiStyle4(stl1, stl2, stl3, stl4) \
	((stl1) | ((stl2) << 8) | ((stl3) << 16) | ((stl4) << 24))
int IsLexCommentLine(int line, LexAccessor &styler, int style);

int IsBackslashLine(int line, LexAccessor &styler);

bool IsLexLineStartsWith(int line, LexAccessor &styler, const char* word, bool matchCase, int style);

int LexLineSkipSpaceTab(int line, LexAccessor &styler);
int LexSkipSpaceTab  (int startPos, int endPos, LexAccessor &styler);
int LexSkipWhiteSpace(int startPos, int endPos, LexAccessor &styler);
int LexSkipWhiteSpace(int startPos, int endPos, LexAccessor &styler, bool IsStreamCommentStyle(int));
int LexSkipWhiteSpace(int startPos, int endPos, LexAccessor &styler,
											bool IsStreamCommentStyle(int), const CharacterSet &charSet);
int LexGetRange			(int startPos, int endPos, LexAccessor &styler, char *s, int len);
int LexGetRangeLowered	(int startPos, int endPos, LexAccessor &styler, char *s, int len);
int LexGetRange			(int startPos, LexAccessor &styler, bool IsWordChar(int), char *s, int len);
int LexGetRangeLowered	(int startPos, LexAccessor &styler, bool IsWordChar(int), char *s, int len);
int LexGetRange			(int startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, int len);
int LexGetRangeLowered	(int startPos, LexAccessor &styler, const CharacterSet &charSet, char *s, int len);

char LexGetPrevChar     (int endPos, LexAccessor &styler);
char LexGetNextChar		(int startPos, LexAccessor &styler);
bool IsLexAtEOL			(int pos, LexAccessor &styler);
bool LexMatch			(int pos, LexAccessor &styler, const char *s);
bool LexMatchIgnoreCase (int pos, LexAccessor &styler, const char *s);

enum EncodingType { enc8bit, encUnicode, encDBCS };

class LexAccessor {
public:
	enum {extremePosition=0x7FFFFFFF};
private:
	IDocument *pAccess;
	/** @a bufferSize is a trade off between time taken to copy the characters
	 * and retrieval overhead.
	 * @a slopSize positions the buffer before the desired position
	 * in case there is some backtracking. */
	enum {bufferSize=4096, slopSize=bufferSize/8};
	char buf[bufferSize+1];
	int startPos;
	int endPos;
	int codePage;
	enum EncodingType encodingType;
	int lenDoc;
	unsigned char styleBuf[bufferSize];
	int validLen;
	unsigned int startSeg;
	int startPosStyling;
	int documentVersion;

	void Fill(int position);

public:
	explicit LexAccessor(IDocument *pAccess_);
	char operator[](int position);
	IDocumentWithLineEnd *MultiByteAccess() const {
		if (documentVersion >= dvLineEnd) {
			return static_cast<IDocumentWithLineEnd *>(pAccess);
		}
		return 0;
	}
	/** Safe version of operator[], returning a defined value for invalid position. */
	char SafeGetCharAt(int position, char chDefault='\0');
	bool IsLeadByte(char ch) const {
		return pAccess->IsDBCSLeadByte(ch);
	}
	EncodingType Encoding() const {
		return encodingType;
	}

	bool Match(int pos, const char *s);
	unsigned char StyleAt(int position) const {
		return static_cast<unsigned char>(pAccess->StyleAt(position));
	}
	int GetLine(int position) const {
		return pAccess->LineFromPosition(position);
	}
	int LineStart(int line) const {
		return pAccess->LineStart(line);
	}
	int LineEnd(int line);
	int LevelAt(int line) const {
		return pAccess->GetLevel(line);
	}
	int Length() const {
		return lenDoc;
	}
	void Flush();
	int GetLineState(int line) const {
		return pAccess->GetLineState(line);
	}
	int SetLineState(int line, int state) {
		return pAccess->SetLineState(line, state);
	}
	// Style setting
	void StartAt(unsigned int start);
	unsigned int GetStartSegment() const {
		return startSeg;
	}
	void StartSegment(unsigned int pos) {
		startSeg = pos;
	}
	void ColourTo(unsigned int pos, int chAttr);
	void SetLevel(int line, int level) {
		pAccess->SetLevel(line, level);
	}
	void IndicatorFill(int start, int end, int indicator, int value);

	void ChangeLexerState(int start, int end) {
		pAccess->ChangeLexerState(start, end);
	}
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
