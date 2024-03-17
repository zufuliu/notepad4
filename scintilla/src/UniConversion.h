// Scintilla source code edit control
/** @file UniConversion.h
 ** Functions to handle UTF-8 and UTF-16 strings.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

constexpr int UTF8MaxBytes = 4;

constexpr int unicodeReplacementChar = 0xFFFD;

size_t UTF8Length(std::wstring_view wsv) noexcept;
size_t UTF8PositionFromUTF16Position(std::string_view u8Text, size_t positionUTF16) noexcept;
void UTF8FromUTF16(std::wstring_view wsv, char *putf, size_t len) noexcept;
void UTF8FromUTF32Character(int uch, char *putf) noexcept;
size_t UTF16Length(std::string_view svu8) noexcept;
size_t UTF16FromUTF8(std::string_view svu8, wchar_t *tbuf, size_t tlen) noexcept;
size_t UTF32Length(std::string_view svu8) noexcept;
size_t UTF32FromUTF8(std::string_view svu8, unsigned int *tbuf, size_t tlen) noexcept;
// WStringFromUTF8 does the right thing when wchar_t is 2 or 4 bytes so
// works on both Windows and Unix.
std::wstring WStringFromUTF8(std::string_view svu8);
bool UTF8IsValid(std::string_view svu8) noexcept;
std::string FixInvalidUTF8(const std::string &text);

extern const unsigned char UTF8ClassifyTable[256];

enum {
	UTF8ClassifyMaskOctetCount = 7,
	UTF8ClassifyMaskTrailByte = 8,
};

inline int UTF8BytesOfLead(unsigned char ch) noexcept {
	return UTF8ClassifyTable[ch] & UTF8ClassifyMaskOctetCount;
}

inline int UnicodeFromUTF8(const unsigned char *us) noexcept {
	switch (UTF8BytesOfLead(us[0])) {
	case 1:
		return us[0];
	case 2:
		return ((us[0] & 0x1F) << 6) | (us[1] & 0x3F);
	case 3:
		return ((us[0] & 0xF) << 12) | ((us[1] & 0x3F) << 6) | (us[2] & 0x3F);
	default:
		return ((us[0] & 0x7) << 18) | ((us[1] & 0x3F) << 12) | ((us[2] & 0x3F) << 6) | (us[3] & 0x3F);
	}
}

constexpr bool UTF8IsTrailByte(unsigned char ch) noexcept {
	return (ch >= 0x80) && (ch < 0xc0);
}

constexpr bool IsASCIICharacter(unsigned int ch) noexcept {
	return ch < 0x80;
}

constexpr bool UTF8IsAscii(unsigned char ch) noexcept {
	return static_cast<signed char>(ch) >= 0;
}

constexpr bool UTF8IsAscii(char ch) noexcept {
	return static_cast<signed char>(ch) >= 0;
}

enum {
	UTF8MaskWidth = 0x7, UTF8MaskInvalid = 0x8
};
int UTF8ClassifyMulti(const unsigned char *us, size_t len) noexcept;
inline int UTF8Classify(const unsigned char *us, size_t len) noexcept {
	if (UTF8IsAscii(us[0])) {
		return 1;
	}
	return UTF8ClassifyMulti(us, len);
}
inline int UTF8Classify(const char *s, size_t len) noexcept {
	return UTF8Classify(reinterpret_cast<const unsigned char *>(s), len);
}

inline int UTF8Classify(std::string_view sv) noexcept {
	return UTF8Classify(sv.data(), sv.length());
}

// Similar to UTF8Classify but returns a length of 1 for invalid bytes
// instead of setting the invalid flag
inline int UTF8DrawBytes(const char *us, size_t len) noexcept {
	if (static_cast<unsigned char>(us[0]) < 0xc2) {
		return 1;
	}

	const int utf8StatusNext = UTF8ClassifyMulti(reinterpret_cast<const unsigned char *>(us), len);
	return (utf8StatusNext & UTF8MaskInvalid) ? 1 : (utf8StatusNext & UTF8MaskWidth);
}

// Line separator is U+2028 \xe2\x80\xa8
// Paragraph separator is U+2029 \xe2\x80\xa9
constexpr int UTF8SeparatorLength = 3;
constexpr bool UTF8IsSeparator(const unsigned char *us) noexcept {
	return (us[0] == 0xe2) && (us[1] == 0x80) && ((us[2] == 0xa8) || (us[2] == 0xa9));
}

// NEL is U+0085 \xc2\x85
constexpr int UTF8NELLength = 2;
constexpr bool UTF8IsNEL(const unsigned char *us) noexcept {
	return (us[0] == 0xc2) && (us[1] == 0x85);
}

// Is the sequence of 3 char a UTF-8 line end? Only the last two char are tested for a NEL.
constexpr bool UTF8IsMultibyteLineEnd(unsigned char ch0, unsigned char ch1, unsigned char ch2) noexcept {
	return ((ch0 == 0xe2) && (ch1 == 0x80) && ((ch2 == 0xa8) || (ch2 == 0xa9)))
		|| ((ch1 == 0xc2) && (ch2 == 0x85));
}

enum {
	SURROGATE_LEAD_FIRST = 0xD800,
	SURROGATE_LEAD_LAST = 0xDBFF,
	SURROGATE_TRAIL_FIRST = 0xDC00,
	SURROGATE_TRAIL_LAST = 0xDFFF,
	SUPPLEMENTAL_PLANE_FIRST = 0x10000,
};

constexpr unsigned int UTF16CharLength(wchar_t uch) noexcept {
	return ((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_LEAD_LAST)) ? 2 : 1;
}

constexpr unsigned int UTF16LengthFromUTF8ByteCount(unsigned int byteCount) noexcept {
	return (byteCount < 4) ? 1 : 2;
}

inline unsigned int UTF16FromUTF32Character(unsigned int val, wchar_t *tbuf) noexcept {
	if (val < SUPPLEMENTAL_PLANE_FIRST) {
		tbuf[0] = static_cast<wchar_t>(val);
		return 1;
	}
	tbuf[0] = static_cast<wchar_t>(((val - SUPPLEMENTAL_PLANE_FIRST) >> 10) + SURROGATE_LEAD_FIRST);
	tbuf[1] = static_cast<wchar_t>((val & 0x3ff) + SURROGATE_TRAIL_FIRST);
	return 2;
}

}
