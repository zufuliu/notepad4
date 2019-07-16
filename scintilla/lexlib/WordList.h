// Scintilla source code edit control
/** @file WordList.h
 ** Hold a list of words.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef WORDLIST_H
#define WORDLIST_H

namespace Scintilla {

/**
 */
class WordList {
	// Each word contains at least one character - a empty word acts as sentinel at the end.
	char **words;
	char *list;
	int len;
	bool onlyLineEnds;	///< Delimited by any white space or only line ends
	int starts[256];
public:
	explicit WordList(bool onlyLineEnds_ = false) noexcept;
	~WordList();
	operator bool() const noexcept;
	bool operator!=(const WordList &other) const noexcept;
	int Length() const noexcept;
	void Clear() noexcept;
	void Set(const char *s);
	void Reset(WordList &other) noexcept;
	bool InList(const char *s) const noexcept;
	bool InListAbbreviated(const char *s, char marker) const noexcept;
	bool InListAbridged(const char *s, char marker) const noexcept;
	const char *WordAt(int n) const noexcept;
};

}

#endif
