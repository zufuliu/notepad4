// Scintilla source code edit control
/** @file UniqueString.h
 ** Define UniqueString, a unique_ptr based string type for storage in containers
 ** and an allocator for UniqueString.
 ** Define UniqueStringSet which holds a set of strings, used to avoid holding many copies
 ** of font names.
 **/
// Copyright 2017 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

constexpr bool IsNullOrEmpty(const char *text) noexcept {
	return text == nullptr || *text == '\0';
}

#if (__cplusplus > 201703L || (defined(_MSVC_LANG) && _MSVC_LANG > 201703L)) && ( \
	(defined(_MSC_VER) && _MSC_VER >= 1920 && (defined(_WIN64) || !defined(__clang__))) || \
	(defined(_LIBCPP_VERSION) && _LIBCPP_VERSION >= 16000) || \
	(!defined(_LIBCPP_VERSION) && defined(__GNUC__) && __GNUC__ >= 11) )
using std::make_unique_for_overwrite; // requires C++20 library support
#else
// https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique
template<class T>
std::enable_if_t<std::is_array_v<T>, std::unique_ptr<T>>
make_unique_for_overwrite(std::size_t n) {
	return std::unique_ptr<T>(new std::remove_extent_t<T>[n]);
}
#endif

template <typename T>
inline std::unique_ptr<T[]> UniqueCopy(const T *data, size_t length) {
	std::unique_ptr<T[]> copy = make_unique_for_overwrite<T[]>(length);
	memcpy(copy.get(), data, length * sizeof(T));
	return copy;
}

using UniqueString = std::unique_ptr<const char[]>;

/// Equivalent to strdup but produces a std::unique_ptr<const char[]> allocation to go
/// into collections.
UniqueString UniqueStringCopy(const char *text);

// A set of strings that always returns the same pointer for each string.

class UniqueStringSet {
private:
	std::vector<UniqueString> strings;
public:
	UniqueStringSet() noexcept;
	void Clear() noexcept;
	const char *Save(const char *text);
};

}
