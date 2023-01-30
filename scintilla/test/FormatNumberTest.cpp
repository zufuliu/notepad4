// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdint>
#include <cinttypes>
#include <cstring>
#include <intrin.h>

#include "../include/VectorISA.h"

// cl /EHsc /std:c++20 /DNDEBUG /Ox /Ot /FAcs /GS- /GR- /Gv /W4 /arch:AVX2 FormatNumberTest.cpp
// clang-cl /EHsc /std:c++20 /DNDEBUG /Ox /Ot /FA /GS- /GR- /Gv /W4 -march=x86-64-v3 FormatNumberTest.cpp
// g++ -S -std=gnu++20 -DNDEBUG -O3 -fno-rtti -Wall -Wextra -march=x86-64-v3 FormatNumberTest.cpp

// https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10
/*
LOG2_10 = math.log2(10)
[(j, int((1 << j)/LOG2_10)) for j in range(17) if all(int(i/LOG2_10) == i*int((1 << j)/LOG2_10) >> j for i in range(65))]

[(8, 77), (9, 154), (10, 308), (11, 616), (12, 1233), (13, 2466), (14, 4932), (15, 9864), (16, 19728)]

[i for i in range(1024) if int(i/LOG2_10) != i*77 >> 8]

[i for i in range(1024) if int(i/LOG2_10) != i*1233 >> 12]
*/

namespace {

constexpr uint64_t PowersOf10[20 + 1] = {
	UINT64_C(0),		UINT64_C(10),		UINT64_C(100),		UINT64_C(1000),		UINT64_C(10000),
	UINT64_C(100000),	UINT64_C(1000000),	UINT64_C(10000000),	UINT64_C(100000000),UINT64_C(1000000000),
	UINT64_C(10000000000),					UINT64_C(100000000000),					UINT64_C(1000000000000),
	UINT64_C(10000000000000),				UINT64_C(100000000000000),				UINT64_C(1000000000000000),
	UINT64_C(10000000000000000),			UINT64_C(100000000000000000),			UINT64_C(1000000000000000000),
	UINT64_C(10000000000000000000),			UINT64_MAX
};

#if 0
inline uint32_t GetWidth(uint64_t value) noexcept {
	//const uint32_t t = (64 - static_cast<uint32_t>(np2_clz64(value)))*1233 >> 12;
	//const uint32_t t = (64 - static_cast<uint32_t>(np2_clz64(value)))*77 >> 8;
	const uint32_t t = np2_ilog10_lower64(value);
	return t - (value < PowersOf10[t]) + 1;
}

#else
inline uint32_t GetWidth(uint64_t value) noexcept {
	//return ((64 - static_cast<uint32_t>(np2_clz64(value)))*77 >> 8) + 1;
	return np2_ilog10_upper64(value);
}
#endif

inline uint32_t GetComma(uint64_t value) noexcept {
	const uint32_t width = GetWidth(value);
	return width + (width - 1)/3;
}

}

struct FormatResult {
	int offset;
	int len;
};

FormatResult FormatNumber(char *number, uint64_t value) noexcept {
	if (value < 10) {
		number[0] = (char)(value + '0');
		number[1] = '\0';
		return {0, 1};
	}

	char * const end = number + GetWidth(value);
	char *ptr = end;
	*ptr = '\0';
	do {
		*--ptr = (char)((value % 10) + '0');
		value /= 10;
	} while (value != 0);
	const int offset = int(ptr - number);
	const int len = (offset >= 0) ? int(end - ptr) : 0;
	if (ptr != number) {
		do {
			*number++ = *ptr++;
		} while (ptr <= end);
	}
	return {offset, len};
}

FormatResult FormatComma(char *number, uint64_t value) noexcept {
	if (value < 10) {
		number[0] = (char)(value + '0');
		number[1] = '\0';
		return {0, 1};
	}

	char * const end = number + GetComma(value);
	char *ptr = end;
	int count = 0;
	*ptr = '\0';
	do {
		if (count == 3) {
			count = 0;
			*--ptr = ',';
		}
		++count;
		*--ptr = (char)((value % 10) + '0');
		value /= 10;
	} while (value != 0);
	const int offset = int(ptr - number);
	const int len = (offset >= 0) ? int(end - ptr) : 0;
	if (ptr != number) {
		do {
			*number++ = *ptr++;
		} while (ptr <= end);
	}
	return {offset, len};
}

int __cdecl main() {
	char number[33]{};
	char buf[32]{};

	for (int bit = 0; bit < 64; bit++) {
		const uint64_t value = (UINT64_C(1) << bit) - 1;
		memset(number, 'x', 32);
		const auto [offset, len1] = FormatNumber(number, value);
		const int len2 = sprintf(buf, "%" PRIu64, value);
		const char *status = (offset == 0 && len1 == len2) ? "" : "FAIL";
		printf("%s => %s (%d, %d, %d) %s\n", buf, number, len1, len2, offset, status);
	}
	for (const uint64_t value : PowersOf10) {
		memset(number, 'x', 32);
		const auto [offset, len1] = FormatNumber(number, value);
		const int len2 = sprintf(buf, "%" PRIu64, value);
		const char *status = (offset == 0 && len1 == len2) ? "" : "FAIL";
		printf("%s => %s (%d, %d, %d) %s\n", buf, number, len1, len2, offset, status);
	}
	for (const uint64_t value : PowersOf10) {
		memset(number, 'x', 32);
		const auto [offset, len1] = FormatNumber(number, value - 1);
		const int len2 = sprintf(buf, "%" PRIu64, value - 1);
		const char *status = (offset == 0 && len1 == len2) ? "" : "FAIL";
		printf("%s => %s (%d, %d, %d) %s\n", buf, number, len1, len2, offset, status);
	}

	for (int bit = 0; bit < 64; bit++) {
		const uint64_t value = (UINT64_C(1) << bit) - 1;
		memset(number, 'x', 32);
		const auto [offset, len1] = FormatComma(number, value);
		int len2 = sprintf(buf, "%" PRIu64, value);
		len2 += (len2 - 1)/3;
		const char *status = (offset == 0 && len1 == len2) ? "" : "FAIL";
		printf("%s => %s (%d, %d, %d) %s\n", buf, number, len1, len2, offset, status);
	}
	for (const uint64_t value : PowersOf10) {
		memset(number, 'x', 32);
		const auto [offset, len1] = FormatComma(number, value);
		int len2 = sprintf(buf, "%" PRIu64, value);
		len2 += (len2 - 1)/3;
		const char *status = (offset == 0 && len1 == len2) ? "" : "FAIL";
		printf("%s => %s (%d, %d, %d) %s\n", buf, number, len1, len2, offset, status);
	}
	for (const uint64_t value : PowersOf10) {
		memset(number, 'x', 32);
		const auto [offset, len1] = FormatComma(number, value - 1);
		int len2 = sprintf(buf, "%" PRIu64, value - 1);
		len2 += (len2 - 1)/3;
		const char *status = (offset == 0 && len1 == len2) ? "" : "FAIL";
		printf("%s => %s (%d, %d, %d) %s\n", buf, number, len1, len2, offset, status);
	}
}
