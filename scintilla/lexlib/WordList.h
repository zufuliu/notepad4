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
	// words in [start, end) starts with same character.
	struct Range {
		int start;
		int end;
	};
	Range ranges[128];	// only ASCII, most word starts with character in '_a-zA-Z'
public:
	explicit WordList() noexcept;
	~WordList();
	operator bool() const noexcept;
	bool operator!=(const WordList &other) const noexcept;
	bool operator==(const WordList &other) const noexcept {
		return !(*this != other);
	}
	int Length() const noexcept;
	void Clear() noexcept;
	bool Set(const char *s);
	bool InList(const char *s) const noexcept;
	bool InListPrefixed(const char *s, char marker) const noexcept;
	bool InListAbbreviated(const char *s, char marker) const noexcept;
	bool InListAbridged(const char *s, char marker) const noexcept;
	const char *WordAt(int n) const noexcept;
};

}

#endif
