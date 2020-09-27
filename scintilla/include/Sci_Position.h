// Scintilla source code edit control
/** @file Sci_Position.h
 ** Define the Sci_Position type used in Scintilla's external interfaces.
 ** These need to be available to clients written in C so are not in a C++ namespace.
 **/
// Copyright 2015 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

#if defined(__cplusplus)
#include <cstddef>
#else
#include <stddef.h>
#endif

// Basic signed type used throughout interface
typedef ptrdiff_t Sci_Position;

// Unsigned variant used for ILexer::Lex and ILexer::Fold
typedef size_t Sci_PositionU;

// For Sci_CharacterRange, previously defined as long to be compatible with Win32 CHARRANGE.
// long is 32-bit on LLP64 system (e.g. 64-bit Windows), thus can not be used to access text beyond 2 GiB.
// ScintillaWin::EditMessage() added translations between CHARRANGE and Sci_CharacterRange.
// Using legacy Win32 EM_* messages and related structures to interact with Scintilla
// is deprecated, supporting for WIN32 EM_* messages will be removed in the future.
typedef Sci_Position Sci_PositionCR;

#ifdef _WIN32
	#define SCI_METHOD __stdcall
#else
	#define SCI_METHOD
#endif
