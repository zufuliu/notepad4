// Scintilla source code edit control
/** @file PropSetSimple.cxx
 ** A basic string to string map.
 **/
// Copyright 1998-2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

// Maintain a dictionary of properties

#include <cstdlib>
#include <cstring>

#include <string>
#include <string_view>
#include <map>

#include "PropSetSimple.h"

using namespace Lexilla;

bool PropSetSimple::Set(std::string_view key, std::string_view val) {
	const auto it = props.find(key);
	if (it != props.end()) {
		if (it->second == val) {
			return false;
		}
		it->second = val;
	} else {
		props.emplace(key, val);
	}
	return true;
}

const char *PropSetSimple::Get(std::string_view key) const {
	const auto it = props.find(key);
	if (it != props.end()) {
		return it->second.c_str();
	}
	return "";
}

int PropSetSimple::GetInt(const char *key, size_t keyLen, int defaultValue) const {
	const auto it = props.find(std::string_view(key, keyLen));
	if (it != props.end()) {
		defaultValue = atoi(it->second.c_str());
	}
	return defaultValue;
}
