// Scintilla source code edit control
/** @file UniConversion.cxx
 ** Functions to handle UTF-8 and UTF-16 strings.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>

#include <stdexcept>
#include <string>
#include <string_view>

#include "UniConversion.h"

using namespace Scintilla;

namespace Scintilla {

size_t UTF8Length(std::wstring_view wsv) noexcept {
	size_t len = 0;
	for (size_t i = 0; i < wsv.length() && wsv[i];) {
		const unsigned int uch = wsv[i];
		if (uch < 0x80) {
			len++;
		} else if (uch < 0x800) {
			len += 2;
		} else if ((uch >= SURROGATE_LEAD_FIRST) &&
			(uch <= SURROGATE_TRAIL_LAST)) {
			len += 4;
			i++;
		} else {
			len += 3;
		}
		i++;
	}
	return len;
}

size_t UTF8PositionFromUTF16Position(std::string_view u8Text, size_t positionUTF16) noexcept {
	size_t positionUTF8 = 0;
	for (size_t lengthUTF16 = 0; (positionUTF8 < u8Text.length()) && (lengthUTF16 < positionUTF16);) {
		const unsigned char uch = u8Text[positionUTF8];
		const unsigned int byteCount = UTF8BytesOfLead(uch);
		lengthUTF16 += UTF16LengthFromUTF8ByteCount(byteCount);
		positionUTF8 += byteCount;
	}

	return positionUTF8;
}

void UTF8FromUTF16(std::wstring_view wsv, char *putf, size_t len) noexcept {
	size_t k = 0;
	for (size_t i = 0; i < wsv.length() && wsv[i];) {
		const unsigned int uch = wsv[i];
		if (uch < 0x80) {
			putf[k++] = static_cast<char>(uch);
		} else if (uch < 0x800) {
			putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
			putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
		} else if ((uch >= SURROGATE_LEAD_FIRST) &&
			(uch <= SURROGATE_TRAIL_LAST)) {
			// Half a surrogate pair
			i++;
			const unsigned int xch = 0x10000 + ((uch & 0x3ff) << 10) + (wsv[i] & 0x3ff);
			putf[k++] = static_cast<char>(0xF0 | (xch >> 18));
			putf[k++] = static_cast<char>(0x80 | ((xch >> 12) & 0x3f));
			putf[k++] = static_cast<char>(0x80 | ((xch >> 6) & 0x3f));
			putf[k++] = static_cast<char>(0x80 | (xch & 0x3f));
		} else {
			putf[k++] = static_cast<char>(0xE0 | (uch >> 12));
			putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
			putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
		}
		i++;
	}
	if (k < len)
		putf[k] = '\0';
}

void UTF8FromUTF32Character(int uch, char *putf) noexcept {
	size_t k = 0;
	if (uch < 0x80) {
		putf[k++] = static_cast<char>(uch);
	} else if (uch < 0x800) {
		putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
		putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
	} else if (uch < 0x10000) {
		putf[k++] = static_cast<char>(0xE0 | (uch >> 12));
		putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
		putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
	} else {
		putf[k++] = static_cast<char>(0xF0 | (uch >> 18));
		putf[k++] = static_cast<char>(0x80 | ((uch >> 12) & 0x3f));
		putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
		putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
	}
	putf[k] = '\0';
}

size_t UTF16Length(std::string_view svu8) noexcept {
	size_t ulen = 0;
	size_t i = 0;
	unsigned int byteCount = 0;
	while (i < svu8.length()) {
		const unsigned char ch = svu8[i];
		byteCount = UTF8BytesOfLead(ch);
		i += byteCount;
		ulen += UTF16LengthFromUTF8ByteCount(byteCount);
	}

	// Invalid 4-bytes UTF-8 lead byte at string end.
	//if ((byteCount == 4) && (i != len)) {
	//	ulen--;
	//}

	/*
	Branchless version of above two tests:
	For 4-bytes UTF-8 lead byte, when the loop is finished, i must
	in [len, len + 1, len + 2, len + 3], thus (i ^ len) is in [0, 1, 2, 3],
	where 0 (i.e. i == len) is the only valid case.
	*/
	const unsigned mask = (1 << (byteCount & 4)) - 1;
	if (mask & (i ^ svu8.length())) {
		ulen--;
	}
	return ulen;
}

size_t UTF16FromUTF8(std::string_view svu8, wchar_t *tbuf, size_t tlen) {
	size_t ui = 0;
	for (size_t i = 0; i < svu8.length();) {
		unsigned char ch = svu8[i];
		const unsigned int byteCount = UTF8BytesOfLead(ch);
		unsigned int value;

		if (i + byteCount > svu8.length()) {
			// Trying to read past end but still have space to write
			if (ui < tlen) {
				tbuf[ui] = ch;
				ui++;
			}
			break;
		}

		const size_t outLen = UTF16LengthFromUTF8ByteCount(byteCount);
		if (ui + outLen > tlen) {
			throw std::runtime_error("UTF16FromUTF8: attempted write beyond end");
		}

		i++;
		switch (byteCount) {
		case 1:
			tbuf[ui] = ch;
			break;
		case 2:
			value = (ch & 0x1F) << 6;
			ch = svu8[i++];
			value += ch & 0x3F;
			tbuf[ui] = static_cast<wchar_t>(value);
			break;
		case 3:
			value = (ch & 0xF) << 12;
			ch = svu8[i++];
			value += (ch & 0x3F) << 6;
			ch = svu8[i++];
			value += ch & 0x3F;
			tbuf[ui] = static_cast<wchar_t>(value);
			break;
		default:
			// Outside the BMP so need two surrogates
			value = (ch & 0x7) << 18;
			ch = svu8[i++];
			value += (ch & 0x3F) << 12;
			ch = svu8[i++];
			value += (ch & 0x3F) << 6;
			ch = svu8[i++];
			value += ch & 0x3F;
			tbuf[ui] = static_cast<wchar_t>(((value - 0x10000) >> 10) + SURROGATE_LEAD_FIRST);
			ui++;
			tbuf[ui] = static_cast<wchar_t>((value & 0x3ff) + SURROGATE_TRAIL_FIRST);
			break;
		}
		ui++;
	}
	return ui;
}

size_t UTF32Length(std::string_view svu8) noexcept {
	size_t ulen = 0;
	for (size_t i = 0; i < svu8.length();) {
		const unsigned char ch = svu8[i];
		const unsigned int byteCount = UTF8BytesOfLead(ch);
		i += byteCount;
		ulen++;
	}
	return ulen;
}

size_t UTF32FromUTF8(std::string_view svu8, unsigned int *tbuf, size_t tlen) {
	size_t ui = 0;
	for (size_t i = 0; i < svu8.length();) {
		unsigned char ch = svu8[i];
		const unsigned int byteCount = UTF8BytesOfLead(ch);
		unsigned int value;

		if (i + byteCount > svu8.length()) {
			// Trying to read past end but still have space to write
			if (ui < tlen) {
				tbuf[ui] = ch;
				ui++;
			}
			break;
		}

		if (ui == tlen) {
			throw std::runtime_error("UTF32FromUTF8: attempted write beyond end");
		}

		i++;
		switch (byteCount) {
		case 1:
			value = ch;
			break;
		case 2:
			value = (ch & 0x1F) << 6;
			ch = svu8[i++];
			value += ch & 0x3F;
			break;
		case 3:
			value = (ch & 0xF) << 12;
			ch = svu8[i++];
			value += (ch & 0x3F) << 6;
			ch = svu8[i++];
			value += ch & 0x3F;
			break;
		default:
			value = (ch & 0x7) << 18;
			ch = svu8[i++];
			value += (ch & 0x3F) << 12;
			ch = svu8[i++];
			value += (ch & 0x3F) << 6;
			ch = svu8[i++];
			value += ch & 0x3F;
			break;
		}
		tbuf[ui] = value;
		ui++;
	}
	return ui;
}

std::wstring WStringFromUTF8(std::string_view svu8) {
	if constexpr (sizeof(wchar_t) == 2) {
		const size_t len16 = UTF16Length(svu8);
		std::wstring ws(len16, 0);
		UTF16FromUTF8(svu8, &ws[0], len16);
		return ws;
	} else {
		const size_t len32 = UTF32Length(svu8);
		std::wstring ws(len32, 0);
		UTF32FromUTF8(svu8, reinterpret_cast<unsigned int *>(&ws[0]), len32);
		return ws;
	}
}

unsigned int UTF16FromUTF32Character(unsigned int val, wchar_t *tbuf) noexcept {
	if (val < SUPPLEMENTAL_PLANE_FIRST) {
		tbuf[0] = static_cast<wchar_t>(val);
		return 1;
	} else {
		tbuf[0] = static_cast<wchar_t>(((val - SUPPLEMENTAL_PLANE_FIRST) >> 10) + SURROGATE_LEAD_FIRST);
		tbuf[1] = static_cast<wchar_t>((val & 0x3ff) + SURROGATE_TRAIL_FIRST);
		return 2;
	}
}

// https://en.wikipedia.org/wiki/UTF-8
// https://tools.ietf.org/html/rfc3629
// UTF-8, a transformation format of ISO 10646
//  4. Syntax of UTF-8 Byte Sequences
// https://www.unicode.org/versions/Unicode12.0.0/
//  3.9 Unicode Encoding Forms
//      Table 3-7. Well-Formed UTF-8 Byte Sequences
/*
UTF8-octets = *(UTF8-char)
UTF8-char   = UTF8-1 / UTF8-2 / UTF8-3 / UTF8-4
             Interval   Number
UTF8-1       = 00-7F    0
UTF8-tail    = 80-BF    (1, 2, 3)
               80-8F    1
               90-9F    2
               A0-BF    3
               80-9F    (1, 2)
               90-BF    (2, 3)
UTF8-2       = C2-DF    4       UTF8-tail
UTF8-3       = E0       5       A0-BF       3       UTF8-tail
               E1-EC    6       UTF8-tail           UTF8-tail
               ED       7       80-9F       (1, 2)  UTF8-tail
               EE-EF    6       UTF8-tail           UTF8-tail
UTF8-4       = F0       8       90-BF       (2, 3)  UTF8-tail   UTF8-tail
               F1-F3    9       UTF8-tail           UTF8-tail   UTF8-tail
               F4       10      80-8F       1       UTF8-tail   UTF8-tail
UTF8-invalid = C0-C1    11
               F5-FF    11

UTF8-2:
    4  | (1, 2, 3)
UTF8-3:
    5  | 3          | (1, 2, 3)
    6  | (1, 2, 3)  | (1, 2, 3)
    7  | (1, 2)     | (1, 2, 3)
UTF8-4:
    8  | (2, 3)     | (1, 2, 3) | (1, 2, 3)
    9  | (1, 2, 3)  | (1, 2, 3) | (1, 2, 3)
    10 | 1          | (1, 2, 3) | (1, 2, 3)

Table content:
bit 0-2: octet count from lead byte, invalid lead byte is treated as isolated single byte.
            table[ch] & 7
bit 3:   tail byte:
            table[ch] & 8
bit 4-7: interval number:
            table[ch] >> 4
*/

enum {
	UTF8_3ByteMask1  = (1 << 5)  | (1 << 3) | (1 << (3 + 1)),
	UTF8_3ByteMask2  = (1 << 6)  /* tail */ | (1 << (3 + 1)),
	UTF8_3ByteMask31 = (1 << 7)  | (1 << 1) | (1 << (3 + 1)),
	UTF8_3ByteMask32 = (1 << 7)  | (1 << 2) | (1 << (3 + 1)),

	UTF8_4ByteMask11 = (1 << 8)  | (1 << 2) | (1 << (3 + 1)) | (1 << (3 + 2)),
	UTF8_4ByteMask12 = (1 << 8)  | (1 << 3) | (1 << (3 + 1)) | (1 << (3 + 2)),
	UTF8_4ByteMask2  = (1 << 9)  /* tail */ | (1 << (3 + 1)) | (1 << (3 + 2)),
	UTF8_4ByteMask3  = (1 << 10) | (1 << 1) | (1 << (3 + 1)) | (1 << (3 + 2)),
};

#define mask_match(mask, test)	(((mask) & (test)) == (test))

const unsigned char UTF8ClassifyTable[256] = {
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 00 - 0F
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 10 - 1F
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 20 - 2F
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 30 - 3F
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 40 - 4F
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 50 - 5F
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 60 - 6F
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, // 70 - 7F
0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, // 80 - 8F
0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, 0x29, // 90 - 9F
0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, // A0 - AF
0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, // B0 - BF
0xB1, 0xB1, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, // C0 - CF
0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, // D0 - DF
0x53, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x73, 0x63, 0x63, // E0 - EF
0x84, 0x94, 0x94, 0x94, 0xA4, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, 0xB1, // F0 - FF
};

// Return both the width of the first character in the string and a status
// saying whether it is valid or invalid.
// Most invalid sequences return a width of 1 so are treated as isolated bytes but
// the non-characters *FFFE, *FFFF and FDD0 .. FDEF return 3 or 4 as they can be
// reasonably treated as code points in some circumstances. They will, however,
// not have associated glyphs.
int UTF8ClassifyMulti(const unsigned char *us, size_t len) noexcept {
	// For the rules: https://www.cl.cam.ac.uk/~mgk25/unicode.html#utf-8
	unsigned int mask = UTF8ClassifyTable[us[0]];
	const size_t byteCount = mask & UTF8ClassifyMaskOctetCount;
	if (byteCount == 1 || byteCount > len) {
		// Invalid lead byte
		return UTF8MaskInvalid | 1;
	}

	const unsigned int second = UTF8ClassifyTable[us[1]];
	if (!(second & UTF8ClassifyMaskTrailByte)) {
		// Invalid tail byte
		return UTF8MaskInvalid | 1;
	}

	switch (byteCount) {
	case 2:
		return 2;

	case 3:
		mask = 1 << (mask >> 4);
		mask |= 1 << (second >> 4);
		mask |= (UTF8ClassifyTable[us[2]] & UTF8ClassifyMaskTrailByte) << 1;
		if (mask_match(mask, UTF8_3ByteMask1) || mask_match(mask, UTF8_3ByteMask2)
			|| mask_match(mask, UTF8_3ByteMask31) || mask_match(mask, UTF8_3ByteMask32)) {
			const unsigned int codePoint = ((us[0] & 0xF) << 12) | ((us[1] & 0x3F) << 6) | (us[2] & 0x3F);
			if (codePoint == 0xFFFE || codePoint == 0xFFFF || (codePoint >= 0xFDD0 && codePoint <= 0xFDEF)) {
				// U+FFFE or U+FFFF, FDD0 .. FDEF non-character
				return UTF8MaskInvalid | 3;
			}
			return 3;
		}
		break;

	default:
		mask = 1 << (mask >> 4);
		mask |= 1 << (second >> 4);
		mask |= (UTF8ClassifyTable[us[2]] & UTF8ClassifyMaskTrailByte) << 1;
		mask |= (UTF8ClassifyTable[us[3]] & UTF8ClassifyMaskTrailByte) << 2;
		if (mask_match(mask, UTF8_4ByteMask11) || mask_match(mask, UTF8_4ByteMask12)
			|| mask_match(mask, UTF8_4ByteMask2) || mask_match(mask, UTF8_4ByteMask3)) {
			unsigned int codePoint = ((us[1] & 0x3F) << 12) | ((us[2] & 0x3F) << 6) | (us[3] & 0x3F);
			codePoint &= 0xFFFF;
			if (codePoint == 0xFFFE || codePoint == 0xFFFF) {
				// *FFFE or *FFFF non-character
				return UTF8MaskInvalid | 4;
			}
			return 4;
		}
		break;
	}

	return UTF8MaskInvalid | 1;
}

bool UTF8IsValid(std::string_view svu8) noexcept {
	const unsigned char *us = reinterpret_cast<const unsigned char *>(svu8.data());
	size_t remaining = svu8.length();
	while (remaining > 0) {
		const int utf8Status = UTF8Classify(us, remaining);
		if (utf8Status & UTF8MaskInvalid) {
			return false;
		} else {
			const int lenChar = utf8Status & UTF8MaskWidth;
			us += lenChar;
			remaining -= lenChar;
		}
	}
	return remaining == 0;
}

// Replace invalid bytes in UTF-8 with the replacement character
std::string FixInvalidUTF8(const std::string &text) {
	std::string result;
	const char *s = text.c_str();
	size_t remaining = text.size();
	while (remaining > 0) {
		const int utf8Status = UTF8Classify(reinterpret_cast<const unsigned char *>(s), remaining);
		if (utf8Status & UTF8MaskInvalid) {
			// Replacement character 0xFFFD = UTF8:"efbfbd".
			result.append("\xef\xbf\xbd");
			s++;
			remaining--;
		} else {
			const size_t len = utf8Status & UTF8MaskWidth;
			result.append(s, len);
			s += len;
			remaining -= len;
		}
	}
	return result;
}

}
