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
#include <map>

#include "PropSetSimple.h"

using namespace Lexilla;

namespace {

typedef std::map<std::string, std::string> mapss;

inline mapss *PropsFromPointer(void *impl) noexcept {
	return static_cast<mapss *>(impl);
}

#if 0
constexpr bool IsASpaceCharacter(char ch) noexcept {
	return (ch == ' ') || ((ch >= 0x09) && (ch <= 0x0d));
}
#endif

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

bool PropSetSimple::Set(const char *key, const char *val) {
	if (!*key) {	// Empty keys are not supported
		return false;
	}

	mapss *props = PropsFromPointer(impl);
	const auto [it, success] = props->emplace(key, val);
	if (!success) {
		it->second = val;
	}
	return true;
}

#if 0
void PropSetSimple::Set(const char *keyVal) {
	while (IsASpaceCharacter(*keyVal)) {
		keyVal++;
	}
	const char *endVal = keyVal;
	while (*endVal && (*endVal != '\n')) {
		endVal++;
	}
	const char *eqAt = strchr(keyVal, '=');
	if (eqAt) {
		Set(keyVal, eqAt + 1, eqAt - keyVal, endVal - eqAt - 1);
	} else if (*keyVal) {	// No '=' so assume '=1'
		Set(keyVal, "1", endVal - keyVal, 1);
	}
}

void PropSetSimple::SetMultiple(const char *s) {
	const char *eol = strchr(s, '\n');
	while (eol) {
		Set(s);
		s = eol + 1;
		eol = strchr(s, '\n');
	}
	Set(s);
}
#endif

const char *PropSetSimple::Get(const char *key) const {
	const mapss *props = PropsFromPointer(impl);
	const auto it = props->find(key);
	if (it != props->end()) {
		return it->second.c_str();
	}
	return "";
}

#if 0
namespace {

// There is some inconsistency between GetExpanded("foo") and Expand("$(foo)").
// A solution is to keep a stack of variables that have been expanded, so that
// recursive expansions can be skipped.  For now I'll just use the C++ stack
// for that, through a recursive function and a simple chain of pointers.

struct VarChain {
	VarChain(const char *var_ = nullptr, const VarChain *link_ = nullptr) noexcept : var(var_), link(link_) {}

	bool contains(const char *testVar) const noexcept {
		return (var && (0 == strcmp(var, testVar)))
			|| (link && link->contains(testVar));
	}

	const char *var;
	const VarChain *link;
};

int ExpandAllInPlace(const PropSetSimple &props, std::string &withVars, int maxExpands, const VarChain &blankVars) {
	size_t varStart = withVars.find("$(");
	while ((varStart != std::string::npos) && (maxExpands > 0)) {
		const size_t varEnd = withVars.find(')', varStart + 2);
		if (varEnd == std::string::npos) {
			break;
		}

		// For consistency, when we see '$(ab$(cde))', expand the inner variable first,
		// regardless whether there is actually a degenerate variable named 'ab$(cde'.
		size_t innerVarStart = withVars.find("$(", varStart + 2);
		while ((innerVarStart != std::string::npos) && (innerVarStart > varStart) && (innerVarStart < varEnd)) {
			varStart = innerVarStart;
			innerVarStart = withVars.find("$(", varStart + 2);
		}

		std::string var(withVars, varStart + 2, varEnd - varStart - 2);
		std::string val = props.Get(var.c_str());

		if (blankVars.contains(var.c_str())) {
			val = ""; // treat blankVar as an empty string (e.g. to block self-reference)
		}

		if (--maxExpands >= 0) {
			maxExpands = ExpandAllInPlace(props, val, maxExpands, VarChain(var.c_str(), &blankVars));
		}

		withVars.erase(varStart, varEnd - varStart + 1);
		withVars.insert(varStart, val.c_str(), val.length());

		varStart = withVars.find("$(");
	}

	return maxExpands;
}

}

size_t PropSetSimple::GetExpanded(const char *key, char *result) const {
	const char *val = Get(key);
	const size_t n = strlen(val);
	if (result) {
		memcpy(result, val, n + 1);
	}
	return n;	// Not including NUL
}
#endif

int PropSetSimple::GetInt(const char *key, size_t lenKey, int defaultValue) const {
	const mapss *props = PropsFromPointer(impl);
	const auto it = props->find(std::string(key, lenKey));
	if (it != props->end() && !it->second.empty()) {
		defaultValue = atoi(it->second.c_str());
	}
	return defaultValue;
}
