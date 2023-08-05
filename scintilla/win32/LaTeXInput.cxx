// This file is part of Notepad2.
// See License.txt for details about distribution and modification.

#include <cstdint>

#include "LaTeXInput.h"

namespace {

struct InputSequence {
	uint16_t magic;		// length | (sequence[0] << 8)
	uint16_t offset;	// sequence offset + 1, i.e. excludes sequence[0]
	uint32_t character;
};

template <typename T, size_t N>
constexpr uint32_t array_size([[maybe_unused]] const T (&a)[N]) noexcept {
	return static_cast<uint32_t>(N);
}

}

#include "LaTeXInputData.h"

#if 0
uint32_t GetLaTeXInputUnicodeCharacter(const char *sequence, size_t length) {
#if NP2_ENABLE_LATEX_LIKE_EMOJI_INPUT
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

#if NP2_ENABLE_LATEX_LIKE_EMOJI_INPUT
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
#if NP2_ENABLE_LATEX_LIKE_EMOJI_INPUT
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
#if !NP2_ENABLE_LATEX_LIKE_EMOJI_INPUT
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

	return value;
}

#else
uint32_t GetLaTeXInputUnicodeCharacter(const char *sequence, size_t length) {
	uint8_t firstChar = sequence[0];
#if NP2_ENABLE_LATEX_LIKE_EMOJI_INPUT
	uint32_t start;
	uint32_t offset;
	const char *sequenceString;
	const uint32_t *sequenceList;
	if (firstChar != ':') {
		if (length < MinLaTeXInputSequenceLength || length > MaxLaTeXInputSequenceLength) {
			return 0;
		}
		offset = firstChar - LaTeXMinInitialCharacter;
		if (offset > LaTeXMaxInitialCharacter - LaTeXMinInitialCharacter) {
			return 0;
		}
		start = LaTeXIndexTable[offset];
		if (start == 0) {
			return 0;
		}

		offset = LaTeXOffsetTable[offset];
		sequenceList = LaTeXInputSequenceList;
		sequenceString = LaTeXInputSequenceString;
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

		firstChar = sequence[0];
		offset = firstChar - EmojiMinInitialCharacter;
		if (offset > EmojiMaxInitialCharacter - EmojiMinInitialCharacter) {
			return 0;
		}
		start = EmojiIndexTable[offset];
		if (start == 0) {
			return 0;
		}

		offset = EmojiOffsetTable[offset];
		sequenceList = EmojiInputSequenceList;
		sequenceString = EmojiInputSequenceString;
	}
#else
	if (length < MinLaTeXInputSequenceLength || length > MaxLaTeXInputSequenceLength) {
		return 0;
	}
	uint32_t offset = firstChar - LaTeXMinInitialCharacter;
	if (offset > LaTeXMaxInitialCharacter - LaTeXMinInitialCharacter) {
		return 0;
	}
	uint32_t start = LaTeXIndexTable[offset];
	if (start == 0) {
		return 0;
	}

	offset = LaTeXOffsetTable[offset];
	const char * const sequenceString = LaTeXInputSequenceString;
	const uint32_t * const sequenceList = LaTeXInputSequenceList;
#endif

	const char * const end = sequence + length;
	uint32_t magic = static_cast<uint32_t>(length) + 1;
	firstChar = end[-1];
	magic |= ((firstChar - LaTeXFirstSubCharacter) & LaTeXFirstSubMask) << LaTeXLengthBit;
	if (length > 2) {
		firstChar = end[-2];
		magic |= ((firstChar - LaTeXSecondSubCharacter) & LaTeXSecondSubMask) << (LaTeXLengthBit + LaTeXFirstSubBit);
	}

	const uint32_t stop = start >> 16;
	start &= 0xffff;
	do {
		uint32_t value = sequenceList[start];
		if ((value & LaTeXHashMask) == magic) {
			const char *s = sequenceString + offset + 1;
			const char *ptr = sequence + 1;
			while (ptr < end && *ptr == *s) {
				++ptr;
				++s;
			}
			if (ptr == end) {
				value >>= LaTeXHashBit;
				if (value < LaTeXDoubleCharacterCount) {
					value = sequenceList[value];
				} else if (value > 0xffff) {
					// UTF-32 to UTF-16
					const uint32_t trail = 0xDC00 + (value & 0x3FF);
					value = 0xD800 - (0x10000 >> 10) + (value >> 10);
					value |= trail << 16;
				}
				return value;
			}
		}
		offset += value & LaTeXLengthMask;
		++start;
	} while (start < stop);
	return 0;
}
#endif
