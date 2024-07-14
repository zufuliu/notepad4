// Scintilla source code edit control
/** @file Debugging.h
 ** Assert and debug trace functions.
 ** Implemented in each platform layer.
 **/
// Copyright 1998-2009 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define NP2_unreachable()	__builtin_unreachable()
#else
#define NP2_unreachable()	__assume(0)
#endif

namespace Scintilla::Internal {

// official Scintilla use dynamic_cast, which requires RTTI.
// When RTTI is enabled, MSVC defines _CPPRTTI,
// GCC/Clang defines __cpp_rtti (similar to C++20 feature testing macros).
#if defined(NDEBUG) && !((defined(_MSC_VER) && defined(_CPPRTTI)) || (!defined(_MSC_VER) && defined(__cpp_rtti)))
#define USE_RTTI	0
#else
#define USE_RTTI	1
#endif

template<typename DerivedPointer, class Base>
inline DerivedPointer down_cast(Base *ptr) noexcept {
#if USE_RTTI
	return dynamic_cast<DerivedPointer>(ptr);
#else
	return static_cast<DerivedPointer>(ptr);
#endif
}

template<typename DerivedReference, class Base>
inline DerivedReference down_cast(Base &ref) noexcept {
#if USE_RTTI
	return dynamic_cast<DerivedReference>(ref);
#else
	return static_cast<DerivedReference>(ref);
#endif
}

#if defined(__clang__)
	#if __has_feature(attribute_analyzer_noreturn)
		#define CLANG_ANALYZER_NORETURN __attribute__((analyzer_noreturn))
	#else
		#define CLANG_ANALYZER_NORETURN
	#endif
#else
	#define CLANG_ANALYZER_NORETURN
#endif

#if defined(__SANITIZE_ADDRESS__)
	#define USE_ADDRESS_SANITIZER	1
#elif defined(__clang__)
	#if __has_feature(address_sanitizer)
		#define USE_ADDRESS_SANITIZER	1
	#else
		#define USE_ADDRESS_SANITIZER	0
	#endif
#else
	#define USE_ADDRESS_SANITIZER	0
#endif

/**
 * Platform namespace used to segregate debugging functions.
 */
namespace Platform {

void DebugDisplay(const char *s) noexcept;
#if defined(__GNUC__) || defined(__clang__)
void DebugPrintf(const char *format, ...) noexcept __attribute__((format(printf, 1, 2)));
#else
void DebugPrintf(const char *format, ...) noexcept;
#endif
bool ShowAssertionPopUps(bool assertionPopUps_) noexcept;
void Assert(const char *c, const char *file, int line) noexcept CLANG_ANALYZER_NORETURN;

}

#ifdef NDEBUG
#define PLATFORM_ASSERT(c) ((void)0)
#elif defined(TRACE)
#define PLATFORM_ASSERT(c) ((c) ? (void)(0) : Scintilla::Internal::Platform::Assert(#c, __FILE__, __LINE__))
#else
#define PLATFORM_ASSERT(c)	assert(c)
#endif

}
