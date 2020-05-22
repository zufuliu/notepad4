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
#else
	#define NP2_TARGET_ARM64	0
	#define NP2_TARGET_ARM32	0
	// SSE2 enabled by default
	#define NP2_USE_SSE2		1

	// Clang & GCC use -mavx2 -mpopcnt -mbmi (to enable tzcnt).
	// MSVC use /arch:AVX2
	#if defined(_WIN64) && defined(__AVX2__)
		#define NP2_USE_AVX2	1
	#else
		#define NP2_USE_AVX2	0
	#endif

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
#if !(NP2_TARGET_ARM64 || NP2_TARGET_ARM32)

// count trailing zero bits
//!NOTE: TZCNT is compatible with BSF for non-zero value;
//! but LZCNT is not compatible with BSR, LZCNT = 31 - BSR.
#if defined(__clang__) || defined(__GNUC__)
	#define np2_ctz(x)		__builtin_ctz(x)
#elif defined(_MSC_VER) || defined(__INTEL_COMPILER_BUILD_DATE)
	#define np2_ctz(x)		_tzcnt_u32(x)
#elif defined(_WIN32)
	// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=bsf
	static __forceinline uint32_t np2_ctz(uint32_t value) NP2_noexcept {
		unsigned long trailing;
		_BitScanForward(&trailing, value);
		return trailing;
	}
#else
	#define np2_ctz(x)		_bit_scan_forward(x)
#endif

// count bits set
#if defined(__clang__) || defined(__GNUC__)
	#define np2_popcount(x)	__builtin_popcount(x)
#elif NP2_USE_AVX2
	#define np2_popcount(x)	_mm_popcnt_u32(x)
#else
	// https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	// Bit Twiddling Hacks copyright 1997-2005 Sean Eron Anderson
	static __forceinline uint32_t bth_popcount(uint32_t v) NP2_noexcept {
		v = v - ((v >> 1) & 0x55555555U);
		v = (v & 0x33333333U) + ((v >> 2) & 0x33333333U);
		return (((v + (v >> 4)) & 0x0F0F0F0FU) * 0x01010101U) >> 24;
	}

	//#define np2_popcount(x)	__popcnt(x)
	#define np2_popcount(x)	bth_popcount(x)
#endif

#endif
