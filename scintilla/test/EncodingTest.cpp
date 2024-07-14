// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cinttypes>

#include "../include/VectorISA.h"

// cl /EHsc /std:c++20 /DNDEBUG /O2 /FAcs /GS- /GR- /Gv /W4 /arch:AVX2 EncodingTest.cpp
// clang-cl /EHsc /std:c++20 /DNDEBUG /O2 /FA /GS- /GR- /Gv /W4 -march=x86-64-v3 EncodingTest.cpp
// g++ -S -std=gnu++20 -DNDEBUG -O3 -fno-rtti -Wall -Wextra -march=x86-64-v3 EncodingTest.cpp

#define CPI_DEFAULT					0
#define CPI_UNICODE					4
#define CPI_UNICODEBE				5

#if 0
namespace {

char DocUTF16[] = {
	'0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0,
	'A', 0, 'B', 0, 'C', 0, 'D', 0, 'E', 0, 'F', 0, 'G', 0, 'H', 0,
	'I', 0, 'J', 0, 'K', 0, 'L', 0, 'M', 0, 'N', 0, 'O', 0, 'P', 0,
	'Q', 0, 'R', 0, 'S', 0, 'T', 0, 'U', 0, 'V', 0, 'W', 0, 'X', 0,
	'a', 0, 'b', 0, 'c', 0, 'd', 0, 'e', 0, 'f', 0, 'g', 0, 'h', 0,
	'i', 0, 'j', 0, 'k', 0, 'l', 0, 'm', 0, 'n', 0, 'o', 0, 'p', 0,
	'q', 0, 'r', 0, 's', 0, 't', 0, 'u', 0, 'v', 0, 'w', 0, 'x', 0,
	'8', 0, '9', 0, 'Y', 0, 'Z', 0, 'y', 0, 'z', 0, '(', 0, ')', 0,
	'+', 1, '-', 2, '*', 3, '/', 4, '%', 5, '^', 6, '=', 7,
	'@', 8,	'<', 9, '>', '\x80', '\x81', '\x82',
};

class EndUTF16Doc {
	unsigned size;
	char ch1;
	char ch2;
public:
	explicit EndUTF16Doc(unsigned size_) noexcept: size{size_} {
		ch1 = DocUTF16[size];
		ch2 = DocUTF16[size + 1];
		DocUTF16[size] = '\xF0';
		DocUTF16[size + 1] = '\xF1';
	}
	~EndUTF16Doc() {
		DocUTF16[size] = ch1;
		DocUTF16[size + 1] = ch2;
	}
};

struct SwapUTF16Doc {
	SwapUTF16Doc() noexcept {
		_swab(DocUTF16, DocUTF16, sizeof(DocUTF16));
	}
	~SwapUTF16Doc() {
		_swab(DocUTF16, DocUTF16, sizeof(DocUTF16));
	}
};

static_assert((sizeof(DocUTF16) & 1) == 0);
constexpr unsigned maxUTF7Len = sizeof(DocUTF16) - 3;
constexpr unsigned maxLatinExtLen = sizeof(DocUTF16) - 4*2;
constexpr unsigned maxLatin1Len = maxLatinExtLen - 7*2;

#if 1 // UTF-7
constexpr bool GetUTF7(unsigned size) noexcept {
	return size <= maxUTF7Len;
}
void TestUTF7() {
	printf("UTF-7/%3u %02X\n", maxUTF7Len, static_cast<uint8_t>(DocUTF16[maxUTF7Len]));
	for (unsigned size = 1; size + 1 < sizeof(DocUTF16); size++) {
		const EndUTF16Doc dummy(size);
		const bool detect = CheckUTF7(DocUTF16, size) == nullptr;
		const bool expect = GetUTF7(size);
		if (detect != expect) {
			printf("UTF-7/%3u result=(%d, %d)\n", size, detect, expect);
			break;
		}
	}
}
#endif

#if 1 // UTF-16 Latin-1
constexpr int GetLatin1LE(unsigned size) noexcept {
	return size <= maxLatin1Len ? CPI_UNICODE : CPI_DEFAULT;
}
constexpr int GetLatin1BE(unsigned size) noexcept {
	return size <= maxLatin1Len ? CPI_UNICODEBE : CPI_DEFAULT;
}
void TestUTF16Latin1() {
	printf("Latin-1 UTF-16/%3u %02X %02X\n", maxLatin1Len, static_cast<uint8_t>(DocUTF16[maxLatin1Len]), static_cast<uint8_t>(DocUTF16[maxLatin1Len + 1]));
	for (unsigned size = 2; size + 2 <= sizeof(DocUTF16); size += 2) {
		const EndUTF16Doc dummy(size);
		int detect = DetectUTF16Latin1(DocUTF16, size);
		int expect = GetLatin1LE(size);
		if (detect != expect) {
			printf("Latin-1 UTF-16LE/%3u result=(%d, %d)\n", size, detect, expect);
			break;
		}

		const SwapUTF16Doc swap;
		detect = DetectUTF16Latin1(DocUTF16, size);
		expect = GetLatin1BE(size);
		if (detect != expect) {
			printf("Latin-1 UTF-16BE/%3u result=(%d, %d)\n", size, detect, expect);
			break;
		}
	}
}
#endif

#if 1 // UTF-16 Latin-Ext
constexpr int GetLatinExtLE(unsigned size) noexcept {
	return size <= maxLatinExtLen ? CPI_UNICODE : CPI_DEFAULT;
}
constexpr int GetLatinExtBE(unsigned size) noexcept {
	return size <= maxLatinExtLen ? CPI_UNICODEBE : CPI_DEFAULT;
}
void TestUTF16LatinExt() {
	printf("Latin-Ext UTF-16/%3u %02X %02X\n", maxLatinExtLen, static_cast<uint8_t>(DocUTF16[maxLatinExtLen]), static_cast<uint8_t>(DocUTF16[maxLatinExtLen + 1]));
	for (unsigned size = 2; size + 2 <= sizeof(DocUTF16); size += 2) {
		const EndUTF16Doc dummy(size);
		int detect = DetectUTF16LatinExt(DocUTF16, size);
		int expect = GetLatinExtLE(size);
		if (detect != expect) {
			printf("Latin-Ext UTF-16LE/%3u result=(%d, %d)\n", size, detect, expect);
			break;
		}

		const SwapUTF16Doc swap;
		detect = DetectUTF16LatinExt(DocUTF16, size);
		expect = GetLatinExtBE(size);
		if (detect != expect) {
			printf("Latin-Ext UTF-16BE/%3u result=(%d, %d)\n", size, detect, expect);
			break;
		}
	}
}
#endif

}

int __cdecl main() {
	printf("doc size=%u, UTF-7=%u, Latin-1=%u, Latin-Ext=%u\n",
		static_cast<unsigned>(sizeof(DocUTF16)), maxUTF7Len, maxLatin1Len, maxLatinExtLen);
	TestUTF7();
	TestUTF16Latin1();
	TestUTF16LatinExt();
	return 0;
}
#endif
