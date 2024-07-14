// Scintilla source code edit control
/** @file Sci_Position.h
 ** Define the Sci_Position type used in Scintilla's external interfaces.
 ** These need to be available to clients written in C so are not in a C++ namespace.
 **/
// Copyright 2015 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

#include <cstddef>

// Basic signed type used throughout interface
typedef ptrdiff_t Sci_Position;
typedef ptrdiff_t Sci_Line;

// Unsigned variant used for ILexer::Lex and ILexer::Fold
typedef size_t Sci_PositionU;

#ifdef _WIN32
	#define SCI_METHOD __stdcall
#else
	#define SCI_METHOD
#endif

namespace sci {

template <typename T>
constexpr T min(T x, T y) noexcept {
	return (x < y) ? x : y;
}

template <typename T>
constexpr T max(T x, T y) noexcept {
	return (x > y) ? x : y;
}

template <typename T>
constexpr T clamp(T x, T lower, T upper) noexcept {
	return (x < lower) ? lower : (x > upper) ? upper : x;
}

}
