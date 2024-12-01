// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#define _CRT_SECURE_NO_WARNINGS
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "../include/VectorISA.h"
#include "TestUtils.h"

// cl /EHsc /std:c++20 /DNDEBUG /O2 /FAcs /GS- /GR- /Gv /W4 /arch:AVX2 BraceMatchTest.cpp
// clang-cl /EHsc /std:c++20 /DNDEBUG /O2 /FA /GS- /GR- /Gv /W4 -march=x86-64-v3 -fsanitize=address BraceMatchTest.cpp
// g++ -S -std=gnu++20 -DNDEBUG -O3 -fno-rtti -Wall -Wextra -march=x86-64-v3 BraceMatchTest.cpp
constexpr char chBrace = '{';
constexpr char chSeek = '}';
constexpr uint32_t maxLength = 512;

void FindAllBraceForward(const SplitView &cbView, ptrdiff_t position, const ptrdiff_t length, uint32_t (&result)[maxLength]) noexcept {
	unsigned j = 0;
#if NP2_USE_AVX2
	const __m256i mmBrace = _mm256_set1_epi8(chBrace);
	const __m256i mmSeek = _mm256_set1_epi8(chSeek);
	const ptrdiff_t maxPos = length - 2*sizeof(__m256i);
	const ptrdiff_t segmentLength = cbView.length1;
	const ptrdiff_t segmentEndPos = min(maxPos, segmentLength - 1);
	while (position <= maxPos) {
		const bool scanFirst = IsValidIndex(position, segmentLength);
		const ptrdiff_t endPos = scanFirst ? segmentEndPos : maxPos;
		const char * const segment = scanFirst ? cbView.segment1 : cbView.segment2;
		const __m256i *ptr = reinterpret_cast<const __m256i *>(segment + position);
		ptrdiff_t index = position;
		uint64_t mask = 0;
		do {
			const __m256i chunk1 = _mm256_loadu_si256(ptr);
			const __m256i chunk2 = _mm256_loadu_si256(ptr + 1);
			mask = mm256_movemask_epi8(_mm256_or_si256(_mm256_cmpeq_epi8(chunk1, mmBrace), _mm256_cmpeq_epi8(chunk1, mmSeek)));
			mask |= static_cast<uint64_t>(mm256_movemask_epi8(_mm256_or_si256(_mm256_cmpeq_epi8(chunk2, mmBrace), _mm256_cmpeq_epi8(chunk2, mmSeek)))) << sizeof(__m256i);
			if (mask != 0) {
				index = position;
				position += 2*sizeof(__m256i);
				break;
			}
			ptr += 2;
			position += 2*sizeof(__m256i);
		} while (position <= endPos);
		if (position > segmentLength && index < segmentLength) {
			position = segmentLength;
			const uint32_t offset = static_cast<uint32_t>(position - index);
			mask = bit_zero_high_u64(mask, offset);
		}
		while (mask) {
			const uint64_t trailing = np2::ctz(mask);
			index += trailing;
			mask >>= trailing;
			result[j++] = static_cast<uint32_t>(index + 1);
			index++;
			mask >>= 1;
		}
	}

#elif NP2_USE_SSE2
	const __m128i mmBrace = _mm_set1_epi8(chBrace);
	const __m128i mmSeek = _mm_set1_epi8(chSeek);
	const ptrdiff_t maxPos = length - 2*sizeof(__m128i);
	const ptrdiff_t segmentLength = cbView.length1;
	const ptrdiff_t segmentEndPos = min(maxPos, segmentLength - 1);
	while (position <= maxPos) {
		const bool scanFirst = IsValidIndex(position, segmentLength);
		const ptrdiff_t endPos = scanFirst ? segmentEndPos : maxPos;
		const char * const segment = scanFirst ? cbView.segment1 : cbView.segment2;
		const __m128i *ptr = reinterpret_cast<const __m128i *>(segment + position);
		ptrdiff_t index = position;
		uint32_t mask = 0;
		do {
			const __m128i chunk1 = _mm_loadu_si128(ptr);
			const __m128i chunk2 = _mm_loadu_si128(ptr + 1);
			mask = mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(chunk1, mmBrace), _mm_cmpeq_epi8(chunk1, mmSeek)));
			mask |= mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(chunk2, mmBrace), _mm_cmpeq_epi8(chunk2, mmSeek))) << sizeof(__m128i);
			if (mask != 0) {
				index = position;
				position += 2*sizeof(__m128i);
				break;
			}
			ptr += 2;
			position += 2*sizeof(__m128i);
		} while (position <= endPos);
		if (position > segmentLength && index < segmentLength) {
			position = segmentLength;
			const uint32_t offset = static_cast<uint32_t>(position - index);
			mask = bit_zero_high_u32(mask, offset);
		}
		while (mask) {
			const uint32_t trailing = np2::ctz(mask);
			index += trailing;
			mask >>= trailing;
			result[j++] = static_cast<uint32_t>(index + 1);
			index++;
			mask >>= 1;
		}
	}
#endif

	while (position < length) {
		const char chAtPos = cbView.CharAt(position);
		if (chAtPos == chBrace || chAtPos == chSeek) {
			result[j++] = static_cast<uint32_t>(position + 1);
		}
		++position;
	}
}

void FindAllBraceBackward(const SplitView &cbView, ptrdiff_t position, uint32_t (&result)[maxLength]) noexcept {
	unsigned j = 0;
#if NP2_USE_AVX2
	const __m256i mmBrace = _mm256_set1_epi8(chBrace);
	const __m256i mmSeek = _mm256_set1_epi8(chSeek);
	constexpr ptrdiff_t minPos = 2*sizeof(__m256i) - 1;
	const ptrdiff_t segmentLength = cbView.length1;
	const ptrdiff_t segmentEndPos = max(minPos, segmentLength);
	while (position >= minPos) {
		const bool scanFirst = IsValidIndex(position, segmentLength);
		const ptrdiff_t endPos = scanFirst ? minPos : segmentEndPos;
		const char * const segment = scanFirst ? cbView.segment1 : cbView.segment2;
		const __m256i *ptr = reinterpret_cast<const __m256i *>(segment + position + 1);
		ptrdiff_t index = position;
		uint64_t mask = 0;
		do {
			const __m256i chunk1 = _mm256_loadu_si256(ptr - 1);
			const __m256i chunk2 = _mm256_loadu_si256(ptr - 2);
			mask = mm256_movemask_epi8(_mm256_or_si256(_mm256_cmpeq_epi8(chunk2, mmBrace), _mm256_cmpeq_epi8(chunk2, mmSeek)));
			mask |= static_cast<uint64_t>(mm256_movemask_epi8(_mm256_or_si256(_mm256_cmpeq_epi8(chunk1, mmBrace), _mm256_cmpeq_epi8(chunk1, mmSeek)))) << sizeof(__m256i);
			if (mask != 0) {
				index = position;
				position -= 2*sizeof(__m256i);
				break;
			}
			ptr -= 2;
			position -= 2*sizeof(__m256i);
		} while (position >= endPos);
		if (index >= segmentLength && position < segmentLength) {
			position = segmentLength - 1;
			//const uint32_t offset = 63 & ~static_cast<uint32_t>(index - segmentLength);
			const uint32_t offset = 63 & static_cast<uint32_t>(position - index);
			mask = (mask >> offset) << offset;
		}
		while (mask) {
			const uint64_t leading = np2::clz(mask);
			index -= leading;
			mask <<= leading;
			result[j++] = static_cast<uint32_t>(index + 1);
			index--;
			mask <<= 1;
		}
	}

#elif NP2_USE_SSE2
	const __m128i mmBrace = _mm_set1_epi8(chBrace);
	const __m128i mmSeek = _mm_set1_epi8(chSeek);
	constexpr ptrdiff_t minPos = 2*sizeof(__m128i) - 1;
	const ptrdiff_t segmentLength = cbView.length1;
	const ptrdiff_t segmentEndPos = max(minPos, segmentLength);
	while (position >= minPos) {
		const bool scanFirst = IsValidIndex(position, segmentLength);
		const ptrdiff_t endPos = scanFirst ? minPos : segmentEndPos;
		const char * const segment = scanFirst ? cbView.segment1 : cbView.segment2;
		const __m128i *ptr = reinterpret_cast<const __m128i *>(segment + position + 1);
		ptrdiff_t index = position;
		uint32_t mask = 0;
		do {
			const __m128i chunk1 = _mm_loadu_si128(ptr - 1);
			const __m128i chunk2 = _mm_loadu_si128(ptr - 2);
			mask = mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(chunk2, mmBrace), _mm_cmpeq_epi8(chunk2, mmSeek)));
			mask |= mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(chunk1, mmBrace), _mm_cmpeq_epi8(chunk1, mmSeek))) << sizeof(__m128i);
			if (mask != 0) {
				index = position;
				position -= 2*sizeof(__m128i);
				break;
			}
			ptr -= 2;
			position -= 2*sizeof(__m128i);
		} while (position >= endPos);
		if (index >= segmentLength && position < segmentLength) {
			position = segmentLength - 1;
			//const uint32_t offset = 31 & ~static_cast<uint32_t>(index - segmentLength);
			const uint32_t offset = 31 & static_cast<uint32_t>(position - index);
			mask = (mask >> offset) << offset;
		}
		while (mask) {
			const uint32_t leading = np2::clz(mask);
			index -= leading;
			mask <<= leading;
			result[j++] = static_cast<uint32_t>(index + 1);
			index--;
			mask <<= 1;
		}
	}
#endif

	while (position >= 0) {
		const char chAtPos = cbView.CharAt(position);
		if (chAtPos == chBrace || chAtPos == chSeek) {
			result[j++] = static_cast<uint32_t>(position + 1);
		}
		--position;
	}
}

bool TestFindBrace(const SplitView &cbView, ptrdiff_t position, ptrdiff_t length, bool forward) noexcept {
	uint32_t result[maxLength]{};
	uint32_t naive[maxLength]{};
	unsigned j = 0;
	if (forward) {
		FindAllBraceForward(cbView, position, length, result);
		while (position < length) {
			const char chAtPos = cbView.CharAt(position);
			if (chAtPos == chBrace || chAtPos == chSeek) {
				naive[j++] = static_cast<uint32_t>(position + 1);
			}
			++position;
		}
	} else {
		FindAllBraceBackward(cbView, position, result);
		while (position >= 0) {
			const char chAtPos = cbView.CharAt(position);
			if (chAtPos == chBrace || chAtPos == chSeek) {
				naive[j++] = static_cast<uint32_t>(position + 1);
			}
			--position;
		}
	}

	const char *tag = forward ? "forward" : "backward";
	bool same = true;
	for (j = 0; j < maxLength; j++) {
		const uint32_t lhs = naive[j];
		const uint32_t rhs = result[j];
		if (lhs != rhs) {
			same = false;
			printf("%s fail %u: (%u, '%c'), (%u, '%c')\n", tag, j, lhs, cbView.CharAt(lhs - 1), rhs, cbView.CharAt(rhs - 1));
		}
	}
	return same;
}

int __cdecl main(int argc, char *argv[]) {
	if (argc > 1) {
		argc = atoi(argv[1]);
	}

	LCGRandom random(static_cast<unsigned int>(reinterpret_cast<uintptr_t>(argv)));
	//PCG32Random random(reinterpret_cast<uintptr_t>(argv), reinterpret_cast<uintptr_t>(argv[0]));
	constexpr uint32_t padding = 32;
	char buffer[padding + maxLength + padding + 1]{};
	memset(buffer, chBrace, padding);
	memset(buffer + padding + maxLength, chSeek, padding);

#if 0
	{
		strcpy(buffer, "");
		constexpr uint32_t gapPosition = 0;
		constexpr uint32_t gapLength = 0;
		constexpr uint32_t position = 0;
		constexpr uint32_t length = 0;
		constexpr bool hasGap = gapPosition != 0 && gapLength != 0;
		const SplitView cbView {
			buffer + padding,
			hasGap ? gapPosition : length,
			buffer + padding + (hasGap ? gapLength : 0),
			length,
		};
		printf("doc: (%u, %u), gap: (%u, %u)\n", position, length, gapPosition, gapLength);
		//TestFindBrace(cbView, position, length, true);
		//TestFindBrace(cbView, position, length, false);
		argc = 0;
	}
#endif

	for (int j = 0; j < argc; j++) {
		for (uint32_t i = 0; i < maxLength; i += 4) {
			const uint32_t value = random.Next();
			buffer[i + padding + 0] = "0{12[3(45)6]78}9"[value & 15];
			buffer[i + padding + 1] = "0{12[3(45)6]78}9"[(value >> 4) & 15];
			buffer[i + padding + 2] = "0{12[3(45)6]78}9"[(value >> 8) & 15];
			buffer[i + padding + 3] = "0{12[3(45)6]78}9"[(value >> 12) & 15];
		}

		const uint32_t gapPosition = random.Next() & (maxLength/2 - 1);
		const uint32_t gapLength = random.Next() & (maxLength/2 - 1);
		uint32_t position = random.Next() & (maxLength - 1);
		const bool hasGap = gapPosition != 0 && gapLength != 0;
		const uint32_t length = maxLength - (hasGap ? gapLength : 0);
		if (position >= length) {
			position = length - 1;
		}
		memset(buffer + padding + gapPosition, chBrace, gapLength);
		const SplitView cbView {
			buffer + padding,
			hasGap ? gapPosition : length,
			buffer + padding + (hasGap ? gapLength : 0),
			length,
		};

		if (!TestFindBrace(cbView, position, length, true)) {
			printf("%4d: (%u, %u), gap: (%u, %u)\n%s\n", j, position, length, gapPosition, gapLength, buffer);
			break;
		}
		if (!TestFindBrace(cbView, position, length, false)) {
			printf("%4d: (%u, %u), gap: (%u, %u)\n%s\n", j, position, length, gapPosition, gapLength, buffer);
			break;
		}
	}
	printf("done: %d\n", argc);
	return 0;
}
