// Scintilla source code edit control
/** @file WordList.cxx
 ** Hold a list of words.
 **/
// Copyright 1998-2002 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cassert>
#include <cstring>

#include <algorithm>
#include <iterator>

#include "WordList.h"

using namespace Scintilla;

/**
 * Creates an array that points into each word in the string and puts \0 terminators
 * after each word.
 */
static char **ArrayFromWordList(char *wordlist, size_t slen, int *len) {
	int prev = true;
	int words = 0;
	// For rapid determination of whether a character is a separator, build
	// a look up table, for ASCII only.
	bool wordSeparator[128] = {};	// Initialise all to false.
	wordSeparator[static_cast<unsigned int>('\r')] = true;
	wordSeparator[static_cast<unsigned int>('\n')] = true;
	wordSeparator[static_cast<unsigned int>(' ')] = true;
	wordSeparator[static_cast<unsigned int>('\t')] = true;

	char * const end = wordlist + slen;
	char *s = wordlist;
	while (s < end) {
		const unsigned char ch = *s++;
		const bool curr = wordSeparator[ch];
		if (!curr && prev) {
			words++;
		}
		prev = curr;
	}

	char **keywords = new char *[words + 1];
	int wordsStore = 0;
	if (words) {
		prev = '\0';
		s = wordlist;
		while (s < end) {
			unsigned char ch = *s;
			if (!wordSeparator[ch]) {
				if (!prev) {
					keywords[wordsStore] = s;
					wordsStore++;
				}
			} else {
				*s = '\0';
				ch = '\0';
			}
			prev = ch;
			++s;
		}
	}

	assert(wordsStore < (words + 1));
	keywords[wordsStore] = end;
	*len = wordsStore;
	return keywords;
}

WordList::WordList() noexcept :
	words(nullptr), list(nullptr), len(0) {
	// Prevent warnings by static analyzers about uninitialized ranges.
	ranges[0] = {};
}

WordList::~WordList() {
	Clear();
}

WordList::operator bool() const noexcept {
	return len != 0;
}

bool WordList::operator!=(const WordList &other) const noexcept {
	if (len != other.len) {
		return true;
	}
	for (int i = 0; i < len; i++) {
		if (strcmp(words[i], other.words[i]) != 0) {
			return true;
		}
	}
	return false;
}

int WordList::Length() const noexcept {
	return len;
}

void WordList::Clear() noexcept {
	if (words) {
		delete[]list;
		delete[]words;
	}
	words = nullptr;
	list = nullptr;
	len = 0;
}

bool WordList::Set(const char *s) {
	// omitted comparison for Notepad2, we don't care whether the list is same as before or not.
	// 1. when we call SciCall_SetKeywords(), the document or styles already changed.
	// 2. the comparison is expensive than rebuild the list, especially for a long list.

	Clear();
	const size_t lenS = strlen(s) + 1;
	list = new char[lenS];
	memcpy(list, s, lenS);
	words = ArrayFromWordList(list, lenS - 1, &len);
	std::sort(words, words + len, [](const char *a, const char *b) noexcept {
		return strcmp(a, b) < 0;
	});

	memset(ranges, 0, sizeof(ranges));
	for (int i = 0; i < len;) {
		const unsigned char indexChar = words[i][0];
		const int start = i++;
		while (words[i][0] == indexChar) {
			++i;
		}
		ranges[indexChar] = {start, i};
	}
	return true;
}

/** Check whether a string is in the list.
 * List elements are either exact matches or prefixes.
 * Prefix elements start with '^' and match all strings that start with the rest of the element
 * so '^GTK_' matches 'GTK_X', 'GTK_MAJOR_VERSION', and 'GTK_'.
 */
bool WordList::InList(const char *s) const noexcept {
	if (nullptr == words) {
		return false;
	}
	const unsigned char firstChar = s[0];
	if (firstChar > 0x7F) {
		return false;
	}
	Range r = ranges[firstChar];
	if (r.end) {
		int count = r.end - r.start;
		if (count < 5) {
			for (int j = r.start; j < r.end; j++) {
				const char *a = words[j] + 1;
				const char *b = s + 1;
				while (*a && *a == *b) {
					a++;
					b++;
				}
				if (!*a && !*b) {
					return true;
				}
			}
		} else {
			int j = r.start;
			do {
				const int step = count/2;
				const int mid = j + step;
				const char *a = words[mid] + 1;
				const char *b = s + 1;
				while (*a && *a == *b) {
					a++;
					b++;
				}
				const int diff = static_cast<unsigned char>(*a) - static_cast<unsigned char>(*b);
				if (diff == 0) {
					return true;
				}
				if (diff < 0) {
					j = mid + 1;
					count -= step + 1;
				} else {
					count = step;
				}
			} while (count > 0);
		}
	}

	r = ranges[static_cast<unsigned char>('^')];
	if (r.end) {
		for (int j = r.start; j < r.end; j++) {
			const char *a = words[j] + 1;
			const char *b = s;
			while (*a && *a == *b) {
				a++;
				b++;
			}
			if (!*a) {
				return true;
			}
		}
	}
	return false;
}

/**
 * similar to InList, but word s can be a prefix of keyword.
 * mainly used to test whether a function is built-in or not.
 * e.g. for keyword definition "sin(x)", InListPrefixed("sin", '(') => true
 * InList(s) == InListPrefixed(s, '\0')
 */
bool WordList::InListPrefixed(const char *s, const char marker) const noexcept {
	if (nullptr == words) {
		return false;
	}
	const unsigned char firstChar = s[0];
	if (firstChar > 0x7F) {
		return false;
	}
	Range r = ranges[firstChar];
	if (r.end) {
		int count = r.end - r.start;
		if (count < 5) {
			for (int j = r.start; j < r.end; j++) {
				const char *a = words[j] + 1;
				const char *b = s + 1;
				while (*a && *a == *b) {
					a++;
					b++;
				}
				if ((!*a || *a == marker) && !*b) {
					return true;
				}
			}
		} else {
			int j = r.start;
			do {
				const int step = count/2;
				const int mid = j + step;
				const char *a = words[mid] + 1;
				const char *b = s + 1;
				while (*a && *a == *b) {
					a++;
					b++;
				}
				const int diff = static_cast<unsigned char>(*a) - static_cast<unsigned char>(*b);
				if (diff == 0 || diff == static_cast<unsigned char>(marker)) {
					return true;
				}
				if (diff < 0) {
					j = mid + 1;
					count -= step + 1;
				} else {
					count = step;
				}
			} while (count > 0);
		}
	}

	r = ranges[static_cast<unsigned char>('^')];
	if (r.end) {
		for (int j = r.start; j < r.end; j++) {
			const char *a = words[j] + 1;
			const char *b = s;
			while (*a && *a == *b) {
				a++;
				b++;
			}
			if (!*a) {
				return true;
			}
		}
	}
	return false;
}

/** similar to InList, but word s can be a substring of keyword.
 * eg. the keyword define is defined as def~ine. This means the word must start
 * with def to be a keyword, but also defi, defin and define are valid.
 * The marker is ~ in this case.
 */
bool WordList::InListAbbreviated(const char *s, const char marker) const noexcept {
	if (nullptr == words) {
		return false;
	}
	const unsigned char firstChar = s[0];
	if (firstChar > 0x7F) {
		return false;
	}
	Range r = ranges[firstChar];
	if (r.end) {
		for (int j = r.start; j < r.end; j++) {
			bool isSubword = false;
			const char *a = words[j] + 1;
			const char *b = s + 1;
			if (*a == marker) {
				isSubword = true;
				a++;
			}
			while (*a && *a == *b) {
				a++;
				if (*a == marker) {
					isSubword = true;
					a++;
				}
				b++;
			}
			if ((!*a || isSubword) && !*b) {
				return true;
			}
		}
	}

	r = ranges[static_cast<unsigned char>('^')];
	if (r.end) {
		for (int j = r.start; j < r.end; j++) {
			const char *a = words[j] + 1;
			const char *b = s;
			while (*a && *a == *b) {
				a++;
				b++;
			}
			if (!*a) {
				return true;
			}
		}
	}
	return false;
}

/** similar to InListAbbreviated, but word s can be a abridged version of a keyword.
* eg. the keyword is defined as "after.~:". This means the word must have a prefix (begins with) of
* "after." and suffix (ends with) of ":" to be a keyword, Hence "after.field:" , "after.form.item:" are valid.
* Similarly "~.is.valid" keyword is suffix only... hence "field.is.valid" , "form.is.valid" are valid.
* The marker is ~ in this case.
* No multiple markers check is done and wont work.
*/
bool WordList::InListAbridged(const char *s, const char marker) const noexcept {
	if (nullptr == words) {
		return false;
	}
	const unsigned char firstChar = s[0];
	if (firstChar > 0x7F) {
		return false;
	}
	Range r = ranges[firstChar];
	if (r.end) {
		for (int j = r.start; j < r.end; j++) {
			const char *a = words[j];
			const char *b = s;
			while (*a && *a == *b) {
				a++;
				if (*a == marker) {
					a++;
					const size_t suffixLengthA = strlen(a);
					const size_t suffixLengthB = strlen(b);
					if (suffixLengthA >= suffixLengthB) {
						break;
					}
					b = b + suffixLengthB - suffixLengthA - 1;
				}
				b++;
			}
			if (!*a && !*b) {
				return true;
			}
		}
	}

	r = ranges[static_cast<unsigned char>(marker)];
	if (r.end) {
		for (int j = r.start; j < r.end; j++) {
			const char *a = words[j] + 1;
			const char *b = s;
			const size_t suffixLengthA = strlen(a);
			const size_t suffixLengthB = strlen(b);
			if (suffixLengthA > suffixLengthB) {
				continue;
			}
			b = b + suffixLengthB - suffixLengthA;

			while (*a && *a == *b) {
				a++;
				b++;
			}
			if (!*a && !*b) {
				return true;
			}
		}
	}

	return false;
}

const char *WordList::WordAt(int n) const noexcept {
	return words[n];
}

