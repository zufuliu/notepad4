// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#pragma once

template <typename T, typename V>
inline T AsPointer(V value) noexcept {
#if defined(__clang__)
	static_assert(__is_pointer(T) && __is_integral(V) && sizeof(V) == sizeof(nullptr));
#endif
	return reinterpret_cast<T>(value);
}

template <typename T, typename V>
inline T AsInteger(V value) noexcept {
#if defined(__clang__)
	static_assert(__is_pointer(V) && __is_integral(T));
#endif
	return reinterpret_cast<T>(value);
}

// suppress clang-tidy [bugprone-multi-level-implicit-pointer-conversion] warning
template <typename T>
inline void* AsVoidPointer(T** pp) noexcept {
	return static_cast<void *>(pp);
}

// C++ template based version of _countof()
#define COUNTOF(ar)	_countof(ar)
#define CSTRLEN(s)	(_countof(s) - 1)

// https://docs.microsoft.com/en-us/cpp/preprocessor/pragma-directives-and-the-pragma-keyword
#if defined(__GNUC__) || defined(__clang__)
#define NP2_COMPILER_WARNING_PUSH	_Pragma("GCC diagnostic push")
#define NP2_COMPILER_WARNING_POP	_Pragma("GCC diagnostic pop")
#else
#define NP2_COMPILER_WARNING_PUSH	__pragma(warning(push))
#define NP2_COMPILER_WARNING_POP	__pragma(warning(pop))
#endif

// C4996：was declared deprecated
#if defined(__GNUC__) || defined(__clang__)
#define NP2_IGNORE_WARNING_DEPRECATED_DECLARATIONS	_Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#else
#define NP2_IGNORE_WARNING_DEPRECATED_DECLARATIONS	__pragma(warning(disable: 4996))
#endif

#define PP_CONCAT_(x, y)	x##y
#define PP_CONCAT(x, y)		PP_CONCAT_(x, y)

// sdkddkver.h
#ifndef _WIN32_WINNT_VISTA
#define _WIN32_WINNT_VISTA				0x0600
#endif
#ifndef _WIN32_WINNT_WIN7
#define _WIN32_WINNT_WIN7				0x0601
#endif
#ifndef _WIN32_WINNT_WIN8
#define _WIN32_WINNT_WIN8				0x0602
#endif
#ifndef _WIN32_WINNT_WINBLUE
#define _WIN32_WINNT_WINBLUE			0x0603
#endif
#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10				0x0A00
#endif
