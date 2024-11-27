// Scintilla source code edit control
/** @file RESearch.h
 ** Interface to the regular expression search library.
 **/
// Written by Neil Hodgson <neilh@scintilla.org>
// Based on the work of Ozan S. Yigit.
// This file is in the public domain.
#pragma once

namespace Scintilla::Internal {

class CharacterIndexer {
public:
	virtual char CharAt(Sci::Position index) const noexcept = 0;
	virtual Sci::Position MovePositionOutsideChar(Sci::Position pos, int moveDir) const noexcept = 0;
};

class RESearch {
public:
	explicit RESearch(const CharClassify *charClassTable) noexcept;
	// No dynamic allocation so default copy constructor and assignment operator are OK.
	void Clear() noexcept;
	const char *Compile(const char *pattern, size_t length, Scintilla::FindOption flags);
	int Execute(const CharacterIndexer &ci, Sci::Position lp, Sci::Position endp);
	void SetLineRange(Sci::Position startPos, Sci::Position endPos) noexcept {
		lineStartPos = startPos;
		lineEndPos = endPos;
	}

	static constexpr int MAXTAG = 10;
	static constexpr int NOTFOUND = -1;

	using MatchPositions = std::array<Sci::Position, MAXTAG>;
	MatchPositions bopat;
	MatchPositions eopat;

private:
	static constexpr int MAXNFA = 4096;
	// The following constants are not meant to be changeable.
	static constexpr int MAXCHR = 256;
	static constexpr int CHRBIT = 8;
	static constexpr int BITBLK = MAXCHR / CHRBIT;

	void ChSet(unsigned char c) noexcept;
	void ChSetWithCase(unsigned char c, bool caseSensitive) noexcept;
	int GetBackslashExpression(const char *pattern, int &incr) noexcept;

	const char *DoCompile(const char *pattern, size_t length, Scintilla::FindOption flags) noexcept;
	Sci::Position PMatch(const CharacterIndexer &ci, Sci::Position lp, Sci::Position endp, const char *ap);

	// positions to match line start and line end
	Sci::Position lineStartPos;
	Sci::Position lineEndPos;
	char nfa[MAXNFA];    /* automaton */
	int sta;

	// cache for previous pattern to avoid recompile
	Scintilla::FindOption previousFlags;
	std::string cachedPattern;

	unsigned char bittab[BITBLK]; /* bit table for CCL pre-set bits */
	const CharClassify *charClass;
	bool iswordc(unsigned char x) const noexcept {
		return charClass->IsWord(x);
	}
};

}
