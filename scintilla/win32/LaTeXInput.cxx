// This file is part of Notepad2.
// See License.txt for details about distribution and modification.

#include <cstdint>
//#include <cstdio>

//#include <chrono>

//#include "ElapsedPeriod.h"
#include "LaTeXInput.h"

namespace {

struct InputSequence {
	uint16_t magic;		// length | (sequence[0] << 8)
	uint16_t offset;	// sequence offset + 1, i.e. excludes sequence[0]
	uint32_t character;
};

template <typename T, uint32_t N>
constexpr uint32_t array_size([[maybe_unused]] const T (&a)[N]) noexcept {
	return N;
}

}

#include "LaTeXInputData.h"

uint32_t GetLaTeXInputUnicodeCharacter(const char *sequence, size_t length) {
#if EnableLaTeXLikeEmojiInput
	static_assert(LaTeXHashMultiplier == EmojiHashMultiplier);
	const char firstChar = *sequence;
	if (firstChar != ':') {
		if (length < MinLaTeXInputSequenceLength || length > MaxLaTeXInputSequenceLength) {
			return 0;
		}
	} else {
		if (length < MinEmojiInputSequenceLength || length > MaxEmojiInputSequenceLength) {
			return 0;
		}
		length -= EmojiInputSequencePrefixLength;
		sequence += EmojiInputSequencePrefixLength;
		const char ch = sequence[length - EmojiInputSequenceSuffixLength];
		if (ch == ':') {
			length -= EmojiInputSequenceSuffixLength;
		}
	}
#else
	if (length < MinLaTeXInputSequenceLength || length > MaxLaTeXInputSequenceLength) {
		return 0;
	}
#endif

	//const Scintilla::Internal::ElapsedPeriod elapsed;
#if EnableLaTeXLikeEmojiInput
	const char *sequenceString;
	const InputSequence *sequenceList;
#endif
	const char * const end = sequence + length;

	// djb2 hash
	uint32_t value = 0;
	const char *ptr = sequence;
	do {
		value = value*LaTeXHashMultiplier + static_cast<uint8_t>(*ptr++);
	} while (ptr < end);
#if EnableLaTeXLikeEmojiInput
	if (firstChar != ':') {
		value %= array_size(LaTeXHashTable);
		value = LaTeXHashTable[value];
		sequenceString = LaTeXInputSequenceString;
		sequenceList = LaTeXInputSequenceList;
	} else {
		value %= array_size(EmojiHashTable);
		value = EmojiHashTable[value];
		sequenceString = EmojiInputSequenceString;
		sequenceList = EmojiInputSequenceList;
	}
#else
	value %= array_size(LaTeXHashTable);
	value = LaTeXHashTable[value];
#endif
	if (value) {
#if !EnableLaTeXLikeEmojiInput
		const char * const sequenceString = LaTeXInputSequenceString;
		const InputSequence * const sequenceList = LaTeXInputSequenceList;
#endif

		// magic field in InputSequence.
		length |= static_cast<uint8_t>(*sequence) << 8;
		uint32_t start = value >> 4;
		const uint32_t stop = start + (value & 15);
		value = 0;

		do {
			if (sequenceList[start].magic == length) {
				const char *s = sequenceString + sequenceList[start].offset;
				ptr = sequence + 1;
				while (ptr < end && *ptr == *s) {
					++ptr;
					++s;
				}
				if (ptr == end) {
					value = sequenceList[start].character;
					break;
				}
			}
			++start;
		} while (start < stop);
	}

	//const double duration = elapsed.Duration()*1e6;
	//printf("LaTeXInput(%s) => %04X, %.3f\n", sequence, value, duration);
	return value;
}
