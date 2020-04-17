// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
#pragma once

#ifndef NP2_noexcept
	#if defined(__cplusplus)
		#define NP2_noexcept noexcept
	#else
		#define NP2_noexcept
	#endif
#endif

// https://docs.microsoft.com/en-us/cpp/intrinsics/compiler-intrinsics
#include <intrin.h>

#if defined(__aarch64__) || defined(_ARM64_) || defined(_M_ARM64)
	#define NP2_TARGET_ARM64	1
	#define NP2_TARGET_ARM32	0
	#define NP2_USE_SSE2		0
	#define NP2_USE_AVX2		0
	// TODO: use ARM Neon
#elif defined(__arm__) || defined(_ARM_) || defined(_M_ARM)
	#define NP2_TARGET_ARM64	0
	#define NP2_TARGET_ARM32	1
	#define NP2_USE_SSE2		0
	#define NP2_USE_AVX2		0
	// TODO: use ARM Neon
#else
	#define NP2_TARGET_ARM64	0
	#define NP2_TARGET_ARM32	0
	#ifndef NP2_USE_SSE2
		// SSE2 enabled by default
		#define NP2_USE_SSE2	1
	#endif

	// Clang & GCC use -mavx2 -mpopcnt -mbmi (to enable tzcnt).
	// MSVC use /arch:AVX2
	#if defined(_WIN64) && !defined(NP2_USE_AVX2)
		#if defined(__AVX2__)
			#define NP2_USE_AVX2	1
		#else
			#define NP2_USE_AVX2	0
		#endif
	#else
		#define NP2_USE_AVX2	0
	#endif // NP2_USE_AVX2

	// TODO: use __isa_enabled/__isa_available in MSVC build to dynamic enable AVX2 code.
	//#if defined(_MSC_VER) || (defined(__has_include) && __has_include(<isa_availability.h>))
	//	#include <isa_availability.h>
	//#else
	//#endif

	// TODO: Function Multiversioning https://gcc.gnu.org/wiki/FunctionMultiVersioning
#endif

#if NP2_USE_AVX2
#define NP2_ALIGNED_LOAD_ALIGNMENT	sizeof(__m256i)
#elif NP2_USE_SSE2
#define NP2_ALIGNED_LOAD_ALIGNMENT	sizeof(__m128i)
#elif defined(_WIN64)
#define NP2_ALIGNED_LOAD_ALIGNMENT	sizeof(uint64_t)
#else
#define NP2_ALIGNED_LOAD_ALIGNMENT	sizeof(uint32_t)
#endif

// for C++20, use functions from <bit> header.

// count trailing zero bits
#if defined(__clang__) || defined(__GNUC__)
	#define np2_ctz		__builtin_ctz
#elif defined(_MSC_VER) && !(NP2_TARGET_ARM64 || NP2_TARGET_ARM32)
	//! NOTE: TZCNT is compatible with BSF; but LZCNT is not compatible with BSR, LZCNT = 31 - BSR.
	#define np2_ctz		_tzcnt_u32
#else
	static __forceinline uint32_t np2_ctz(uint32_t value) NP2_noexcept {
		unsigned long trailing;
		_BitScanForward(&trailing, value);
		return trailing;
	}

	// https://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightParallel
#endif

// count bits set
#if defined(__clang__) || defined(__GNUC__)
	#define np2_popcount	__builtin_popcount
#elif NP2_USE_AVX2
	#define np2_popcount	_mm_popcnt_u32
#else
	// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	// Bit Twiddling Hacks copyright 1997-2005 Sean Eron Anderson
	static __forceinline uint32_t bth_popcount(uint32_t v) NP2_noexcept {
		v = v - ((v >> 1) & 0x55555555U);
		v = (v & 0x33333333U) + ((v >> 2) & 0x33333333U);
		return (((v + (v >> 4)) & 0x0F0F0F0FU) * 0x01010101U) >> 24;
	}

	//#define np2_popcount	__popcnt
	#define np2_popcount	bth_popcount
#endif
