// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include <windows.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>

#include "../include/VectorISA.h"
#include "../include/GraphicUtils.h"
#include "TestUtils.h"

// cl /EHsc /std:c++20 /DNDEBUG /O2 /FAcs /GS- /GR- /Gv /W4 /arch:AVX2 GraphicTest2.cpp
// clang-cl /EHsc /std:c++20 /DNDEBUG /O2 /FA /GS- /GR- /Gv /W4 -march=x86-64-v3 -fsanitize=address GraphicTest2.cpp
// g++ -S -std=gnu++20 -DNDEBUG -O3 -fno-rtti -Wall -Wextra -march=x86-64-v3 GraphicTest2.cpp

namespace {
constexpr size_t bytesPerPixel = 4;
constexpr unsigned char AlphaMultiplied(unsigned char value, unsigned char alpha) noexcept {
	return (value * alpha) / 255;
}
inline uint32_t RGBQuadToUInt32(RGBQUAD quad) noexcept {
	return *reinterpret_cast<const uint32_t *>(&quad);
}

#if NP2_USE_SSE2
void mm128_dump_epi16(const char *msg, __m128i i16x8) noexcept {
	uint16_t buf[16];
	_mm_storeu_si128((__m128i *)buf, i16x8);
	printf("%s: %04X %04X %04X %04X  %04X %04X %04X %04X\n", msg,
		buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
}
#endif

#if NP2_USE_AVX2
void mm256_dump_epi16(const char *msg, __m256i i16x16) noexcept {
	uint16_t buf[16];
	_mm256_storeu_si256((__m256i *)buf, i16x16);
	printf("%s: %04X %04X %04X %04X  %04X %04X %04X %04X  %04X %04X %04X %04X  %04X %04X %04X %04X\n", msg,
		buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
		buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
}
#endif

#if NP2_USE_AVX512
void mm512_dump_epi16(const char *msg, __m512i i16x32) noexcept {
	uint16_t buf[32];
	_mm512_storeu_si512(buf, i16x32);
	printf("%s: %04X %04X %04X %04X  %04X %04X %04X %04X  %04X %04X %04X %04X  %04X %04X %04X %04X  %04X %04X %04X %04X  %04X %04X %04X %04X  %04X %04X %04X %04X  %04X %04X %04X %04X\n", msg,
		buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
		buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15],
		buf[16], buf[17], buf[18], buf[19], buf[20], buf[21], buf[22], buf[23],
		buf[24], buf[25], buf[26], buf[27], buf[28], buf[28], buf[30], buf[31]);
}
#endif
}

void ProcessScalar(uint32_t *output, const uint32_t *input, size_t count, uint32_t crDest, uint8_t alpha) noexcept {
}

#if 0//NP2_USE_SSE2
void ProcessSSE2(uint32_t *output, const uint32_t *input, size_t count, uint32_t crDest, uint8_t alpha) noexcept {
}
#endif

#if NP2_USE_AVX2
void ProcessAVX2(uint32_t *output, const uint32_t *input, size_t count, uint32_t crDest, uint8_t alpha) noexcept {
}
#endif

#if NP2_USE_AVX512
void ProcessAVX512(uint32_t *output, const uint32_t *input, size_t count, uint32_t crDest, uint8_t alpha) noexcept {
}
#endif

bool TestProcess(const std::vector<uint32_t> &input, uint32_t crDest, uint8_t alpha) noexcept {
	const size_t count = input.size();
	std::vector<uint32_t> scalar(count);
	std::vector<uint32_t> output(count);
	ProcessScalar(scalar.data(), input.data(), count, crDest, alpha);
	// ProcessAVX2(scalar.data(), input.data(), count, crDest, alpha);
#if NP2_USE_AVX512
	ProcessAVX512(output.data(), input.data(), count, crDest, alpha);
#elif NP2_USE_AVX2
	ProcessAVX2(output.data(), input.data(), count, crDest, alpha);
#elif NP2_USE_SSE2
	ProcessSSE2(output.data(), input.data(), count, crDest, alpha);
#endif

	// printf("input : %08X %08X %08X %08X  %08X %08X %08X %08X\n", input[0], input[1], input[2], input[3], input[4], input[5], input[6], input[7]);
	// printf("scalar: %08X %08X %08X %08X  %08X %08X %08X %08X\n", scalar[0], scalar[1], scalar[2], scalar[3], scalar[4], scalar[5], scalar[6], scalar[7]);
	// printf("output: %08X %08X %08X %08X  %08X %08X %08X %08X\n", output[0], output[1], output[2], output[3], output[4], output[5], output[6], output[7]);
	for (size_t i = 0; i < count; i++) {
		if (scalar[i] != output[i]) {
			const size_t k = i / 8;
			printf("fail %zu/%zu: %08X => %08X, %08X\n", k, i & 7, input[i], scalar[i], output[i]);
			printf("input : %08X %08X %08X %08X  %08X %08X %08X %08X\n",
				input[k], input[k + 1], input[k + 2], input[k + 3], input[k + 4], input[k + 5], input[k + 6], input[k + 7]);
			printf("scalar: %08X %08X %08X %08X  %08X %08X %08X %08X\n",
				scalar[k], scalar[k + 1], scalar[k + 2], scalar[k + 3], scalar[k + 4], scalar[k + 5], scalar[k + 6], scalar[k + 7]);
			printf("output: %08X %08X %08X %08X  %08X %08X %08X %08X\n",
				output[k], output[k + 1], output[k + 2], output[k + 3], output[k + 4], output[k + 5], output[k + 6], output[k + 7]);
			return false;
		}
	}
	return true;
}

int __cdecl main(int argc, char *argv[]) {
	if (argc > 1) {
		argc = atoi(argv[1]);
	}

	// LCGRandom random(static_cast<unsigned int>(reinterpret_cast<uintptr_t>(argv)));
	PCG32Random random(reinterpret_cast<uintptr_t>(argv), reinterpret_cast<uintptr_t>(argv[0]));
	for (int j = 0; j < argc; j++) {
		size_t count = random.Next() & 0xff;
		count = NP2_align_up(count, 64);
		// count = 8;
		std::vector<uint32_t> input(count);
		for (size_t i = 0; i < count; i++) {
			input[i] = random.Next();
		}
#if 0
		input[0] = 0xU;
		input[1] = 0xU;
		input[2] = 0xU;
		input[3] = 0xU;
		input[4] = 0xU;
		input[5] = 0xU;
		input[6] = 0xU;
		input[7] = 0xU;
#endif
		const uint32_t crDest = random.Next();
		const uint8_t alpha = (random.Next() & 0xff) | 1;
		if (!TestProcess(input, crDest, alpha)) {
			argc = j + 1;
			break;
		}
	}

	printf("done: %d\n", argc);
	return 0;
}
