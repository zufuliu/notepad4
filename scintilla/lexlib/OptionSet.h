// Scintilla source code edit control
/** @file OptionSet.h
 ** Manage descriptive information about an options struct for a lexer.
 ** Hold the names, positions, and descriptions of boolean, integer and string options and
 ** allow setting options and retrieving metadata about the options.
 **/
// Copyright 2010 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Lexilla {

inline std::string JoinWordListDescriptions(const char *const wordListDescriptions[]) {
	std::string wordLists;
	if (wordListDescriptions) {
		for (size_t wl = 0; wordListDescriptions[wl]; wl++) {
			if (wl > 0)
				wordLists += '\n';
			wordLists += wordListDescriptions[wl];
		}
	}
	return wordLists;
}

// Allow OptionSet<T> to be called without knowing T
struct OptionSetInterface {
	[[nodiscard]] virtual const char *PropertyNames() const noexcept = 0;
	[[nodiscard]] virtual int PropertyType(const char *name) const = 0;
	[[nodiscard]] virtual const char *DescribeProperty(const char *name) const = 0;
	[[nodiscard]] virtual const char *PropertyGet(const char *name) const = 0;
	[[nodiscard]] virtual const char *DescribeWordListSets() const noexcept = 0;
};

template <typename T>
class OptionSet : public OptionSetInterface {
	using Target = T;
	using plcob = bool T::*;
	using plcoi = int T::*;
	using plcos = std::string T::*;
	struct Option {
		int opType;
		union {
			plcob pb;
			plcoi pi;
			plcos ps;
		};
		const char *description;
		std::string value;
		Option() noexcept :
			opType(SC_TYPE_BOOLEAN), pb(nullptr), description("") {
		}
		Option(plcob pb_, const char *description_ = "") noexcept :
			opType(SC_TYPE_BOOLEAN), pb(pb_), description(description_) {
		}
		Option(plcoi pi_, const char *description_) noexcept :
			opType(SC_TYPE_INTEGER), pi(pi_), description(description_) {
		}
		Option(plcos ps_, const char *description_) noexcept :
			opType(SC_TYPE_STRING), ps(ps_), description(description_) {
		}
		bool Set(T *base, const char *val) {
			value = val;
			switch (opType) {
			case SC_TYPE_BOOLEAN: {
				bool option = atoi(val) != 0;
				if ((*base).*pb != option) {
					(*base).*pb = option;
					return true;
				}
				break;
			}
			case SC_TYPE_INTEGER: {
				int option = atoi(val);
				if ((*base).*pi != option) {
					(*base).*pi = option;
					return true;
				}
				break;
			}
			case SC_TYPE_STRING: {
				const std::string_view sv = val;
				if ((*base).*ps != sv) {
					(*base).*ps = sv;
					return true;
				}
				break;
			}
			}
			return false;
		}
		[[nodiscard]] const char *Get() const noexcept {
			return value.c_str();
		}
	};
	using OptionMap = std::map<std::string_view, Option, std::less<>>;
	OptionMap nameToDef;
	std::string names;
	std::string wordLists;

	void AddProperty(const char *name, Option option) {
		const std::string_view sv = name;
		nameToDef[sv] = std::move(option);
		if (!names.empty()) {
			names += '\n';
		}
		names += sv;
	}
	const Option *GetProperty(const char *name) const {
		const std::string_view sv = name;
		const auto it = nameToDef.find(sv);
		if (it != nameToDef.end()) {
			return &it->second;
		}
		return nullptr;
	}
public:
	virtual ~OptionSet() = default;

	void DefineProperty(const char *name, plcob pb, const char *description = "") {
		AddProperty(name, Option(pb, description));
	}
	void DefineProperty(const char *name, plcoi pi, const char *description = "") {
		AddProperty(name, Option(pi, description));
	}
	void DefineProperty(const char *name, plcos ps, const char *description = "") {
		AddProperty(name, Option(ps, description));
	}
	template <typename E>
#if defined(__cpp_concepts)
	requires std::is_enum_v<E>
#endif
	void DefineProperty(const char *name, E T::*pe, const char *description = "") {
		static_assert(std::is_enum_v<E>);
		plcoi pi = reinterpret_cast<plcoi>(pe);
		static_assert(sizeof(pe) == sizeof(pi));
		AddProperty(name, Option(pi, description));
	}
	[[nodiscard]] const char *PropertyNames() const noexcept final {
		return names.c_str();
	}
	[[nodiscard]] int PropertyType(const char *name) const final {
		if (const Option *option = GetProperty(name)) {
			return option->opType;
		}
		return SC_TYPE_BOOLEAN;
	}
	[[nodiscard]] const char *DescribeProperty(const char *name) const final {
		if (const Option *option = GetProperty(name)) {
			return option->description;
		}
		return "";
	}

	bool PropertySet(T *base, const char *name, const char *val) {
		if (Option *option = const_cast<Option *>(GetProperty(name))) {
			return option->Set(base, val);
		}
		return false;
	}

	[[nodiscard]] const char *PropertyGet(const char *name) const final {
		if (const Option *option = GetProperty(name)) {
			return option->Get();
		}
		return nullptr;
	}

	void DefineWordListSets(const char *const wordListDescriptions[]) {
		wordLists = JoinWordListDescriptions(wordListDescriptions);
	}

	const char *DescribeWordListSets() const noexcept final {
		return wordLists.c_str();
	}
};

}
