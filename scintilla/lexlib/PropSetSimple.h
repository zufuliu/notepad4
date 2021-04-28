// Scintilla source code edit control
/** @file PropSetSimple.h
 ** A basic string to string map.
 **/
// Copyright 1998-2009 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Lexilla {

class PropSetSimple final {
	void *impl;
	void Set(const char *keyVal);
public:
	PropSetSimple();
	~PropSetSimple();
	bool Set(const char *key, const char *val);
	//void Set(const char *keyVal);
	//void SetMultiple(const char *s);
	const char *Get(const char *key) const;
	//size_t GetExpanded(const char *key, char *result) const;
	int GetInt(const char *key, size_t lenKey, int defaultValue = 0) const;

	template <size_t N>
	int GetInt(const char (&key)[N], int defaultValue = 0) const {
		return GetInt(key, N - 1, defaultValue);
	}
};

}
