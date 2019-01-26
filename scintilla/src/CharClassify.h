// Scintilla source code edit control
/** @file CharClassify.h
 ** Character classifications used by Document and RESearch.
 **/
// Copyright 2006-2009 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef CHARCLASSIFY_H
#define CHARCLASSIFY_H

namespace Scintilla {

class CharClassify {
public:
	CharClassify() noexcept;

	enum cc {
		ccSpace, ccNewLine, ccWord, ccPunctuation, ccCJKWord
	};
	void SetDefaultCharClasses(bool includeWordClass) noexcept;
	void SetCharClasses(const unsigned char *chars, cc newCharClass) noexcept;
	void SetCharClassesEx(const unsigned char *chars, int length) noexcept;
	int GetCharsOfClass(cc characterClass, unsigned char *buffer) const noexcept;
	cc GetClass(unsigned char ch) const noexcept {
		return static_cast<cc>(charClass[ch]);
	}
	bool IsWord(unsigned char ch) const noexcept {
		return static_cast<cc>(charClass[ch]) == ccWord;
	}

	static cc ClassifyCharacter(unsigned int ch) noexcept;

private:
	enum {
		maxChar = 256
	};
	unsigned char charClass[maxChar];    // not type cc to save space
};

class DBCSCharClassify {
public:
	static const DBCSCharClassify* Get(int codePage);

	bool IsLeadByte(char ch) const noexcept {
		return leadByte[static_cast<unsigned char>(ch)];
	}
	bool IsLeadByteInvalid(char ch) const noexcept {
		return invalidLeadByte[static_cast<unsigned char>(ch)];
	}
	bool IsTrailByteInvalid(char ch) const noexcept {
		return invalidTrailByte[static_cast<unsigned char>(ch)];
	}

	CharClassify::cc ClassifyCharacter(unsigned int ch) const noexcept {
		return classifyCharacter(ch);
	}

private:
	DBCSCharClassify(int codePage_) noexcept;

	typedef CharClassify::cc (*ClassifyCharacterSig)(unsigned int ch) noexcept;

	int codePage;
	ClassifyCharacterSig classifyCharacter;
	bool leadByte[256];
	bool invalidLeadByte[256];
	bool invalidTrailByte[256];
};

}

#endif
