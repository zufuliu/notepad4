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

namespace {

typedef std::map<std::string, std::string> mapss;

inline mapss *PropsFromPointer(void *impl) noexcept {
	return static_cast<mapss *>(impl);
}

}

PropSetSimple::PropSetSimple() {
	mapss *props = new mapss;
	impl = static_cast<void *>(props);
}

PropSetSimple::~PropSetSimple() {
	mapss *props = PropsFromPointer(impl);
	delete props;
	impl = nullptr;
}

bool PropSetSimple::Set(std::string_view key, std::string_view val) {
	mapss *props = PropsFromPointer(impl);
	const auto [it, updated] = props->emplace(key, val);
	if (!updated) {
		it->second = val;
	}
	return true;
}

const char *PropSetSimple::Get(const char *key) const {
	const mapss *props = PropsFromPointer(impl);
	const auto it = props->find(key);
	if (it != props->end()) {
		return it->second.c_str();
	}
	return "";
}

int PropSetSimple::GetInt(std::string_view key, int defaultValue) const {
	const mapss *props = PropsFromPointer(impl);
	const auto it = props->find(std::string(key));
	if (it != props->end() && !it->second.empty()) {
		defaultValue = atoi(it->second.c_str());
	}
	return defaultValue;
}
