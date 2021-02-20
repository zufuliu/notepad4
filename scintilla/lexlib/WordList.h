// Scintilla source code edit control
/** @file WordList.h
 ** Hold a list of words.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla {

/**
 */
class WordList final {
public:
	using range_t = unsigned int;
private:
	// Each word contains at least one character - a empty word acts as sentinel at the end.
	char **words;
	char *list;
	range_t len;
	// words in [start, end) starts with same character, maximum word count limited to 0xffff.
	struct Range {
		range_t start;
		const range_t end;
		explicit constexpr Range(range_t range) noexcept : start(range & 0xffff), end(range >> 16) {}
		constexpr range_t Length() const noexcept {
			return end - start;
		}
		bool Next() noexcept {
			++start;
			return start < end;
		}
	};
	range_t ranges[128];	// only ASCII, most word starts with character in '_a-zA-Z'
public:
	explicit WordList() noexcept;
	~WordList();
	operator bool() const noexcept;
	bool operator!=(const WordList &other) const noexcept;
	bool operator==(const WordList &other) const noexcept {
		return !(*this != other);
	}
	range_t Length() const noexcept;
	void Clear() noexcept;
	bool Set(const char *s, bool toLower);
	bool InList(const char *s) const noexcept;
	bool InListPrefixed(const char *s, char marker) const noexcept;
	bool InListAbbreviated(const char *s, char marker) const noexcept;
	bool InListAbridged(const char *s, char marker) const noexcept;
	const char *WordAt(range_t n) const noexcept;
};

}
