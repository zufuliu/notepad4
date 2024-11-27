// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#include <cstdint>
#include <cstdlib>

template <typename T>
constexpr T min(T x, T y) noexcept {
	return (x < y) ? x : y;
}

template <typename T>
constexpr T max(T x, T y) noexcept {
	return (x > y) ? x : y;
}

constexpr bool IsValidIndex(size_t index, size_t length) noexcept {
	return index < length;
}

struct SplitView {
	const char *segment1 = nullptr;
	size_t length1 = 0;
	const char *segment2 = nullptr;
	size_t length = 0;

	char CharAt(size_t position) const noexcept {
		if (position < length1) {
			return segment1[position];
		}
		if (position < length) {
			return segment2[position];
		}
		return '\0';
	}
};

struct CRTRandom {
	CRTRandom(uint32_t seed) noexcept {
		srand(seed);
	}
	uint32_t Next() const noexcept {
		return rand();
	}
};

struct LCGRandom {
	uint32_t state;
	LCGRandom(uint32_t seed) noexcept: state{seed} {}
	uint32_t Next() noexcept {
		//state = state*214013 + 2531011; // msvc
		// https://pubs.opengroup.org/onlinepubs/9699919799/functions/rand.html
		state = state*1103515245 + 12345;
		return (state >> 16) & RAND_MAX;
	}
};

// https://www.pcg-random.org/download.html
struct PCG32Random {
	uint64_t state;
	uint64_t inc;
	PCG32Random(uint64_t seed, uint64_t seq) noexcept: state{seed}, inc{seq | 1} {}
	uint32_t Next() noexcept {
		const uint64_t oldstate = state;
		// Advance internal state
		state = oldstate * UINT64_C(6364136223846793005) + inc;
		// Calculate output function (XSH RR), uses old state for max ILP
		const uint32_t xorshifted = static_cast<uint32_t>(((oldstate >> 18) ^ oldstate) >> 27);
		const int rot = oldstate >> 59;
		return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
	}
};

// https://prng.di.unimi.it/

// https://lemire.me/blog/2018/07/02/predicting-the-truncated-xorshift32-random-number-generator/
