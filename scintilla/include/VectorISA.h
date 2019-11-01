// This file is part of Notepad2.
// See License.txt for details about distribution and modification.

#ifndef NOTEPAD2_VECTOR_ISA_H_
#define NOTEPAD2_VECTOR_ISA_H_

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

	#include <intrin.h>
	// TODO: use __isa_enabled/__isa_available in MSVC build to dynamic enable AVX2 code.
	//#if defined(_MSC_VER) || (defined(__has_include) && __has_include(<isa_availability.h>))
	//	#include <isa_availability.h>
	//#else
	//#endif

	// TODO: Function Multiversioning https://gcc.gnu.org/wiki/FunctionMultiVersioning
#endif

#endif // NOTEPAD2_VECTOR_ISA_H_

// end of VectorISA.h
