// Scintilla source code edit control
/** @file PropSetSimple.h
 ** A basic string to string map.
 **/
// Copyright 1998-2009 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Lexilla {

#define PropSetSimpleUseMap		0

class PropSetSimple final {
#if PropSetSimpleUseMap
	std::map<std::string, std::string, std::less<>> props;
#else
	std::vector<std::pair<std::string, std::string>> props;
#endif
public:
	bool Set(std::string_view key, std::string_view val);
	const char *Get(std::string_view key) const;
	int GetInt(const char *key, size_t keyLen, int defaultValue = 0) const;

	template <size_t N>
	int GetInt(const char (&key)[N], int defaultValue = 0) const {
		return GetInt(key, N - 1, defaultValue);
	}
};

}
