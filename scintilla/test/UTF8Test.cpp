// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#define _CRT_SECURE_NO_WARNINGS
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <initializer_list>
#include <string_view>

#include "../include/VectorISA.h"

// cl /utf-8 /EHsc /std:c++20 /DNDEBUG /O2 /FAcs /GS- /GR- /Gv /W4 /arch:AVX2 UTF8Test.cpp
// clang-cl /utf-8 /EHsc /std:c++20 /DNDEBUG /O2 /FA /GS- /GR- /Gv /W4 -march=x86-64-v3 UTF8Test.cpp
// g++ -S -std=gnu++20 -DNDEBUG -O3 -fno-rtti -Wall -Wextra -march=x86-64-v3 UTF8Test.cpp

namespace {

void dump(const char *name, const char *s, size_t len) noexcept {
	printf("const char %s[%zu] = {", name, len);
	const char *end = s + len;
	while (end > s && end[-1] == '\0') {
		--end;
	}
	while (s < end) {
		const uint8_t ch = *s++;
		if (ch == 0) {
			printf("0, ");
		} else if (ch >= 32 && ch < 127) {
			printf("'%c', ", static_cast<char>(ch));
		} else {
			const uint8_t index = ch - '\a';
			if (index <= '\r' - '\a') {
				printf("'\\%c', ", "abtnvfr"[index]);
			} else {
				printf("'\\x%02x', ", ch);
			}
		}
	}
	printf("};\n");
}

}

#if 0
#include "../src/UniConversion.cxx"
bool IsUTF8(const char *s, uint32_t len) noexcept {
	return Scintilla::Internal::UTF8IsValid({s, len});
}
#endif

namespace {

struct FixedTest {
	bool expected;
	int lineno;
	std::string_view test;
};

const FixedTest FIXED_TESTS[] = {
// https://github.com/lemire/fastvalidate-utf-8/blob/master/tests/unit.c
{true, __LINE__, "a"},
{true, __LINE__, "\xc3\xb1"},
{true, __LINE__, "\xe2\x82\xa1"},
{true, __LINE__, "\xf0\x90\x8c\xbc"},
{true, __LINE__, "안녕하세요, 세상"},
{true, __LINE__, "\xc2\x80"},
{true, __LINE__, "\xf0\x90\x80\x80"},
{true, __LINE__, "\xee\x80\x80"},

{false, __LINE__, "\xc3\x28"},
{false, __LINE__, "\xa0\xa1"},
{false, __LINE__, "\xe2\x28\xa1"},
{false, __LINE__, "\xe2\x82\x28"},
{false, __LINE__, "\xf0\x28\x8c\xbc"},
{false, __LINE__, "\xf0\x90\x28\xbc"},
{false, __LINE__, "\xf0\x28\x8c\x28"},
{false, __LINE__, "\xc0\x9f"},
{false, __LINE__, "\xf5\xff\xff\xff"},
{false, __LINE__, "\xed\xa0\x81"},
{false, __LINE__, "\xf8\x90\x80\x80\x80"},
{false, __LINE__, "123456789012345\xed"},
{false, __LINE__, "123456789012345\xf1"},
{false, __LINE__, "123456789012345\xc2"},
{false, __LINE__, "\xC2\x7F"},
{false, __LINE__, "\xce"},
{false, __LINE__, "\xce\xba\xe1"},
{false, __LINE__, "\xce\xba\xe1\xbd"},
{false, __LINE__, "\xce\xba\xe1\xbd\xb9\xcf"},
{false, __LINE__, "\xce\xba\xe1\xbd\xb9\xcf\x83\xce"},
{false, __LINE__, "\xce\xba\xe1\xbd\xb9\xcf\x83\xce\xbc\xce"},
{false, __LINE__, "\xdf"},
{false, __LINE__, "\xef\xbf"},

// https://github.com/protocolbuffers/protobuf/blob/main/third_party/utf8_range/utf8_validity_test.cc
{true, __LINE__, "abcd"},
{true, __LINE__, {"a\0cd", 4}},
{true, __LINE__, "ab\xc2\x81"},
{true, __LINE__, "a\xe2\x81\x81"},
{true, __LINE__, "\xf2\x81\x81\x81"},

{false, __LINE__, "abc\x80"},
{false, __LINE__, "abc\xc2"},
{false, __LINE__, "ab\xe2\x81"},
{false, __LINE__, "a\xf2\x81\x81"},
{false, __LINE__, "ab\xc0\x81"},
{false, __LINE__, "a\xe0\x81\x81"},
{false, __LINE__, "\xf0\x81\x81\x81"},
{false, __LINE__, "\xf4\xbf\xbf\xbf"},

{false, __LINE__, "\xED\xA0\x80"},
{false, __LINE__, "\xED\xBF\xBF"},

{false, __LINE__, "\xc0\x80"},
{false, __LINE__, "\xc1\xbf"},
{false, __LINE__, "\xe0\x80\x80"},
{false, __LINE__, "\xe0\x9f\xbf"},
{false, __LINE__, "\xf0\x80\x80\x80"},
{false, __LINE__, "\xf0\x83\xbf\xbf"},
{false, __LINE__, "\xc7\xc8\xcd\xcb"},

// https://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt

// TRAILING_TESTS
{true, __LINE__, "\x40"},
{true, __LINE__, "\xC2\x80"},
{true, __LINE__, "\xE0\xA0\x80"},
{true, __LINE__, "\xE1\x80\x80"},
{true, __LINE__, "\xED\x80\x80"},
{true, __LINE__, "\xF4\x8F\x80\x80"},
{false, __LINE__, "\xC2"},
{false, __LINE__, "\xE1\x80"},
{false, __LINE__, "\xF4\x80\x80"},
};

// https://github.com/zwegner/faster-utf8-validator/blob/master/test.lua
// https://github.com/zwegner/faster-utf8-validator/blob/wip/test.lua
struct ByteRange {
	uint8_t lower;
	uint8_t upper;
};

constexpr ByteRange ANY = { 0, 0xFF };
constexpr ByteRange ASCII = { 0, 0x7F };
constexpr ByteRange CONT = { 0x80, 0xBF };

struct RangeTest {
	bool expected;
	int lineno;
	std::initializer_list<ByteRange> test;
};

const RangeTest TEST_CASES[] = {
	// ASCII
	{  true, __LINE__, {ASCII, ASCII, ASCII, ASCII} },

	// 2-byte sequences
	{ false, __LINE__, {{ 0xC2, 0xDF }} },
	{ false, __LINE__, {{ 0xC2, 0xDF }, ASCII} },
	{  true, __LINE__, {{ 0xC2, 0xDF }, CONT} },
	{ false, __LINE__, {{ 0xC2, 0xDF }, { 0xC0, 0xFF}} },
	{ false, __LINE__, {{ 0xC2, 0xDF }, CONT, CONT} },
	{ false, __LINE__, {{ 0xC2, 0xDF }, CONT, CONT, CONT} },

	// 3-byte sequences
	{ false, __LINE__, {{ 0xE1, 0xEC }}, },
	{ false, __LINE__, {{ 0xE1, 0xEC }, CONT} },
	{  true, __LINE__, {{ 0xE1, 0xEC }, CONT, CONT} },
	{  true, __LINE__, {{ 0xE1, 0xEC }, CONT, CONT, ASCII} },
	{  true, __LINE__, {{ 0xEE, 0xEF }, CONT, CONT} },
	{ false, __LINE__, {{ 0xE1, 0xEC }, CONT, ASCII} },
	{ false, __LINE__, {{ 0xE1, 0xEC }, CONT, CONT, CONT} },

	// 4-byte sequences
	{ false, __LINE__, {{ 0xF1, 0xF3 }} },
	{ false, __LINE__, {{ 0xF1, 0xF3 }, CONT} },
	{ false, __LINE__, {{ 0xF1, 0xF3 }, CONT, CONT} },
	{  true, __LINE__, {{ 0xF1, 0xF3 }, CONT, CONT, CONT} },
	{ false, __LINE__, {{ 0xF1, 0xF3 }, CONT, CONT, ASCII} },
	{  true, __LINE__, {{ 0xF1, 0xF3 }, CONT, CONT, CONT, ASCII} },
	{ false, __LINE__, {{ 0xF1, 0xF3 }, CONT, CONT, CONT, CONT} },

	// Stray continuation bytes
	{ false, __LINE__, {CONT, ANY} },
	{ false, __LINE__, {ASCII, CONT} },
	{ false, __LINE__, {ASCII, CONT, CONT} },
	{ false, __LINE__, {ASCII, CONT, CONT, CONT} },
	{ false, __LINE__, {ASCII, CONT, CONT, CONT, CONT} },

	// No C0/C1 bytes (overlong)
	{ false, __LINE__, {{ 0xC0, 0xC1 }, ANY} },
	{ false, __LINE__, {{ 0xC0, 0xC1 }, ANY, ANY} },
	{ false, __LINE__, {{ 0xC0, 0xC1 }, ANY, ANY, ANY} },

	// No E0 followed by 80..9F (overlong)
	{ false, __LINE__, {{ 0xE0, 0xE0 }, { 0x00, 0x9F }, CONT} },
	{  true, __LINE__, {{ 0xE0, 0xE0 }, { 0xA0, 0xBF }, CONT} },

	// No surrogate pairs
	{  true, __LINE__, {{ 0xE1, 0xEC }, CONT, CONT} },
	{  true, __LINE__, {{ 0xED, 0xED }, { 0x80, 0x9F }, CONT} },
	{ false, __LINE__, {{ 0xED, 0xED }, { 0xA0, 0xBF }, CONT} },
	{  true, __LINE__, {{ 0xEE, 0xEF }, CONT, CONT} },

	// No F0 followed by 80..8F (overlong)
	{ false, __LINE__, {{ 0xF0, 0xF0 }, { 0x80, 0x8F }, CONT, CONT} },
	{  true, __LINE__, {{ 0xF0, 0xF0 }, { 0x90, 0xBF }, CONT, CONT} },

	// No code points above U+10FFFF
	{  true, __LINE__, {{ 0xF4, 0xF4 }, { 0x80, 0x8F }, CONT, CONT} },
	{ false, __LINE__, {{ 0xF4, 0xF4 }, { 0x90, 0xBF }, CONT, CONT} },

	// No bytes above F4
	{ false, __LINE__, {{ 0xF5, 0xFF }, ANY} },
	{ false, __LINE__, {{ 0xF5, 0xFF }, ANY, ANY} },
	{ false, __LINE__, {{ 0xF5, 0xFF }, ANY, ANY, ANY} },

	// No consecutive leader bytes
	{ false, __LINE__, {{ 0xC0, 0xFF }, { 0xC0, 0xFF }, CONT} },
	{ false, __LINE__, {{ 0xC0, 0xFF }, { 0xC0, 0xFF }, CONT, CONT} },
	{ false, __LINE__, {{ 0xC0, 0xFF }, { 0xC0, 0xFF }, CONT, CONT, CONT} },

	// Various other cases that probably won't fail, but are here to check that
	// we at least check every permutation of two bytes in a row.
	{ false, __LINE__, {ASCII, { 0xC0, 0xC1 }, CONT} },
	{  true, __LINE__, {ASCII, { 0xC2, 0xDF }, CONT} },
	{ false, __LINE__, {ASCII, { 0xE0, 0xE0 }, { 0x00, 0x9F }, CONT} },
	{  true, __LINE__, {ASCII, { 0xE0, 0xE0 }, { 0xA0, 0xBF }, CONT} },
	{  true, __LINE__, {ASCII, { 0xE1, 0xEC }, CONT, CONT} },
	{  true, __LINE__, {ASCII, { 0xED, 0xED }, { 0x80, 0x9F }, CONT} },
	{ false, __LINE__, {ASCII, { 0xED, 0xED }, { 0xA0, 0xBF }, CONT} },
	{  true, __LINE__, {ASCII, { 0xEE, 0xEF }, CONT, CONT} },
	{ false, __LINE__, {ASCII, { 0xF0, 0xF0 }, { 0x80, 0x8F }, CONT, CONT} },
	{  true, __LINE__, {ASCII, { 0xF0, 0xF0 }, { 0x90, 0xBF }, CONT, CONT} },
	{  true, __LINE__, {ASCII, { 0xF1, 0xF3 }, CONT, CONT, CONT} },
	{  true, __LINE__, {ASCII, { 0xF4, 0xF4 }, { 0x80, 0x8F }, CONT, CONT} },
	{ false, __LINE__, {ASCII, { 0xF5, 0xFF }, CONT, CONT, CONT} },
	{ false, __LINE__, {{ 0xC0, 0xFF }, ASCII} },
};

struct TrailingTest {
	bool expected;
	int lineno;
	std::initializer_list<uint8_t> test;
};

const TrailingTest TRAILING_TESTS[] = {
	{  true, __LINE__, {}},
	{  true, __LINE__, {0x40} },
	{  true, __LINE__, {0xC2, 0x80} },
	{  true, __LINE__, {0xE0, 0xA0, 0x80} },
	{  true, __LINE__, {0xE1, 0x80, 0x80} },
	{  true, __LINE__, {0xED, 0x80, 0x80} },
	{  true, __LINE__, {0xF4, 0x8F, 0x80, 0x80} },
	{ false, __LINE__, {0xC2}, },
	{ false, __LINE__, {0xE1, 0x80} },
	{ false, __LINE__, {0xF4, 0x80, 0x80} },
};

}

namespace {

void run_fixed_test() noexcept {
	for (const auto &test : FIXED_TESTS) {
		const bool result = IsUTF8(test.test.data(), static_cast<uint32_t>(test.test.size()));
		if (result != test.expected) {
			printf("fixed test fail %d: result=%d, expected=%d\n", test.lineno, result, test.expected);
			dump("s", test.test.data(), test.test.size());
			break;
		}
	}
}

void run_range_test() noexcept {
	for (const auto &test : TEST_CASES) {
		// Loop through various frame shifts, to make sure we catch any issues due
		// to vector alignment
		for (size_t k = 0; k <= 200; k++) {
			char buffer[256]{};
			// Loop through first byte
			const auto first = *test.test.begin();
			for (int b = first.lower; b <= first.upper; b++) {
				buffer[k] = static_cast<char>(b);
				// Find maximum range of values in remaining bytes
				for (int offset = 0; offset <= 255; offset++) {
					bool any_valid = false;
					size_t index = 0;
					for (const auto range : test.test) {
						const size_t i = index + k;
						index++;
						const int ch = range.lower + offset;
						if (ch > range.upper) {
							buffer[i] = range.upper;
						} else {
							buffer[i] = static_cast<char>(ch);
							any_valid = true;
						}
					}
					// Break if we've run through the range of all bytes
					if (!any_valid && (index != 0 || offset > 0)) {
						break;
					}
					// Run the validators
					const bool result = IsUTF8(buffer, sizeof(buffer));
					if (result != test.expected) {
						printf("range test fail %d(%zu, %d, %d): result=%d, expected=%d\n", test.lineno, k, b, offset, result, test.expected);
						dump("s", buffer + k, index);
						dump("buffer", buffer, sizeof(buffer));
						return;
					}
				}
			}
		}
	}
}

void run_trailing_test() noexcept {
	for (const auto &test : TRAILING_TESTS) {
		for (size_t pre = 0; pre <= 120; pre++) {
			for (size_t post = 0; post <= 120; post++) {
				char buffer[256];
				const size_t len = pre + test.test.size() + post;
				// Fill in invalid bytes everywhere
				memset(buffer, 0xFF, sizeof(buffer));
				// Fill in valid bytes in the range being tested
				memset(buffer + 1, 0x20, len + 1);
				// Fill in the test sequence
				memcpy(buffer + 1 + pre, test.test.begin(), test.test.size());
				const bool result = IsUTF8(buffer + 1, static_cast<uint32_t>(len));
				if (result != test.expected) {
					printf("trailing test fail %d(%zu, %zu): result=%d, expected=%d\n", test.lineno, pre, post, result, test.expected);
					dump("s", buffer + 1, len);
					return;
				}
			}
		}
	}
}

}

int __cdecl main() {
	run_fixed_test();
	run_range_test();
	run_trailing_test();
	// printf("%d\n", IsUTF8(s, sizeof(s)));
	// printf("%d\n", IsUTF8(buffer, sizeof(buffer)));
	printf("done\n");
	return 0;
}
