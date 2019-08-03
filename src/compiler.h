#ifndef NOTEPAD2_COMPILER_H_
#define NOTEPAD2_COMPILER_H_

#if defined(__cplusplus)
#undef NULL
#define NULL	nullptr
#endif

#if (defined(__GNUC__) || defined(__clang__)) && !defined(__cplusplus)
// https://stackoverflow.com/questions/19452971/array-size-macro-that-rejects-pointers
// trigger error for pointer: GCC: void value not ignored as it ought to be. Clang: invalid operands to binary expression.
#define COUNTOF(ar)	_Generic(&(ar), __typeof__((ar)[0]) **: (void)0, default: _countof(ar))
// trigger warning for non-literal string: GCC: division by zero [-Wdiv-by-zero]. Clang: division by zero is undefined [-Wdivision-by-zero].
#if !defined(__clang__)
#define CSTRLEN(s)	(__builtin_constant_p(s) ? (_countof(s) - 1) : (1 / 0))
#else
// Clang complains when above CSTRLEN() is used in certain macros, such as EDITLEXER_HOLE()
#define CSTRLEN(s)	(COUNTOF(s) - 1)
#endif
#else
// C++ template based version of _countof(), or plain old unsafe version
#define COUNTOF(ar)	_countof(ar)
#define CSTRLEN(s)	(_countof(s) - 1)
#endif

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

#if defined(__GNUC__) && !defined(__cplusplus)
#if defined(__NO_INLINE__) // O0
#define NP2_inline	static inline
#else
#define NP2_inline	extern inline __attribute__((__gnu_inline__, __artificial__))
#endif
#else
#define NP2_inline	inline
#endif

// force compile C as CPP: /TP for MSVC and clang-cl, -x c++ for GCC and clang
#define NP2_FORCE_COMPILE_C_AS_CPP	0

#if defined(__cplusplus) && !defined(__clang__)
#define NP2_USE_DESIGNATED_INITIALIZER	0
#else
#define NP2_USE_DESIGNATED_INITIALIZER	1
#endif

// sdkddkver.h
#ifndef _WIN32_WINNT_VISTA
#define _WIN32_WINNT_VISTA					0x0600
#endif
#ifndef _WIN32_WINNT_WIN7
#define _WIN32_WINNT_WIN7					0x0601
#endif
#ifndef _WIN32_WINNT_WIN8
#define _WIN32_WINNT_WIN8					0x0602
#endif
#ifndef _WIN32_WINNT_WINBLUE
#define _WIN32_WINNT_WINBLUE				0x0603
#endif
#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10					0x0A00
#endif

#endif // NOTEPAD2_COMPILER_H_
