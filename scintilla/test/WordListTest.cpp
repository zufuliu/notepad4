// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#define _CRT_SECURE_NO_WARNINGS
#include <cstdint>
#include <cstring>
#include <cstdio>
#include "../include/VectorISA.h"

// cl /utf-8 /EHsc /std:c++20 /DNDEBUG /O2 /FAcs /GS- /GR- /Gv /W4 /arch:AVX2 WordListTest.cpp
// clang-cl /utf-8 /EHsc /std:c++20 /DNDEBUG /O2 /FA /GS- /GR- /Gv /W4 -march=x86-64-v3 WordListTest.cpp
// g++ -S -std=gnu++20 -DNDEBUG -O3 -fno-rtti -Wall -Wextra -march=x86-64-v3 WordListTest.cpp

void TestArrayFromWordList() noexcept {
	char wordlist[256];
	const __m256i space = _mm256_set1_epi8(-(' ' + 1));
	constexpr uint32_t gap = sizeof(__m256i) + 2;
	uint32_t total = 0;
	uint32_t diff[1 + 1 + 4]{};

	for (uint32_t indexA = 0; indexA < gap; indexA++) {
		memset(wordlist, 0, sizeof(wordlist));
		wordlist[indexA] = 'A';
		const uint32_t startA = indexA + 1;
		for (uint32_t indexB = startA; indexB < startA + gap; indexB++) {
			memset(wordlist + startA, 0, sizeof(wordlist) - startA);
			wordlist[indexB] = 'B';
			const uint32_t startB = indexB + 1;
			for (uint32_t indexC = startB; indexC < startB + gap; indexC++) {
				memset(wordlist + startB, 0, sizeof(wordlist) - startB);
				wordlist[indexC] = 'C';
				for (uint32_t length = 1; length < indexC + gap; length++) {
					const char * const end = wordlist + length;

					uint32_t wordsStore = 0;
					uint8_t prev = 1;
					const char *s = wordlist;
					do {
						const unsigned char ch = *s++;
						if (prev <= ' ' && ch > ' ') {
							wordsStore++;
						}
						prev = ch;
					} while (s < end);

					uint32_t words = 0;
					prev = 1;
					s = wordlist;
					do {
						const __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(s));
						uint32_t mask = mm256_movemask_epi8(_mm256_add_epi8(chunk, space));
						uint32_t mask2;
						prev = _addcarry_u32(prev, mask, mask, &mask2);
						words += np2_popcount(andn_u32(mask, mask2));
						s += sizeof(__m256i);
					} while (s < end);

					++total;
					int delta = words - wordsStore;
					if (delta < 0) {
						delta = -1;
					} else if (delta > 4) {
						delta = 4;
					}
					diff[delta + 1] += 1;
				}
			}
		}
	}
	printf("total: %u, less: %u, equal: %u, greater: %u %u %u %u\n", total, diff[0], diff[1], diff[2], diff[3], diff[4], diff[5]);
}

int __cdecl main() {
	TestArrayFromWordList();
	printf("done\n");
	return 0;
}
