// Scintilla source code edit control
/** @file UniConversion.cxx
 ** Functions to handle UTF-8 and UTF-16 strings.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>

#include <stdexcept>
#include <string>

#include "UniConversion.h"

using namespace Scintilla;

namespace Scintilla {

size_t UTF8Length(const wchar_t *uptr, size_t tlen) {
	size_t len = 0;
	for (size_t i = 0; i < tlen && uptr[i];) {
		const unsigned int uch = uptr[i];
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

void UTF8FromUTF16(const wchar_t *uptr, size_t tlen, char *putf, size_t len) {
	size_t k = 0;
	for (size_t i = 0; i < tlen && uptr[i];) {
		const unsigned int uch = uptr[i];
		if (uch < 0x80) {
			putf[k++] = static_cast<char>(uch);
		} else if (uch < 0x800) {
			putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
			putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
		} else if ((uch >= SURROGATE_LEAD_FIRST) &&
			(uch <= SURROGATE_TRAIL_LAST)) {
			// Half a surrogate pair
			i++;
			const unsigned int xch = 0x10000 + ((uch & 0x3ff) << 10) + (uptr[i] & 0x3ff);
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

size_t UTF16Length(const char *s, size_t len) {
	size_t ulen = 0;
	const unsigned char *us = reinterpret_cast<const unsigned char *>(s);
	size_t i = 0;
	unsigned int byteCount = 0;
	while (i < len) {
		const unsigned char ch = us[i];
		byteCount = UTF8BytesOfLead(ch);
		i += byteCount;
		ulen += UTF16LengthFromUTF8ByteCount(byteCount < 4);
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
	if (mask & (i ^ len)) {
		ulen--;
	}
	return ulen;
}

size_t UTF16FromUTF8(const char *s, size_t len, wchar_t *tbuf, size_t tlen) {
	size_t ui = 0;
	const unsigned char *us = reinterpret_cast<const unsigned char *>(s);
	for (size_t i = 0; i < len;) {
		unsigned char ch = us[i];
		const unsigned int byteCount = UTF8BytesOfLead(ch);
		unsigned int value;

		if (i + byteCount > len) {
			// Trying to read past end but still have space to write
			if (ui < tlen) {
				tbuf[ui] = ch;
				ui++;
			}
			break;
		}

		const size_t outLen = (byteCount < 4) ? 1 : 2;
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
			ch = us[i++];
			value += ch & 0x3F;
			tbuf[ui] = static_cast<wchar_t>(value);
			break;
		case 3:
			value = (ch & 0xF) << 12;
			ch = us[i++];
			value += (ch & 0x3F) << 6;
			ch = us[i++];
			value += ch & 0x3F;
			tbuf[ui] = static_cast<wchar_t>(value);
			break;
		default:
			// Outside the BMP so need two surrogates
			value = (ch & 0x7) << 18;
			ch = us[i++];
			value += (ch & 0x3F) << 12;
			ch = us[i++];
			value += (ch & 0x3F) << 6;
			ch = us[i++];
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

size_t UTF32FromUTF8(const char *s, size_t len, unsigned int *tbuf, size_t tlen) {
	size_t ui = 0;
	const unsigned char *us = reinterpret_cast<const unsigned char *>(s);
	for (size_t i = 0; i < len;) {
		unsigned char ch = us[i];
		const unsigned int byteCount = UTF8BytesOfLead(ch);
		unsigned int value;

		if (i + byteCount > len) {
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
			ch = us[i++];
			value += ch & 0x3F;
			break;
		case 3:
			value = (ch & 0xF) << 12;
			ch = us[i++];
			value += (ch & 0x3F) << 6;
			ch = us[i++];
			value += ch & 0x3F;
			break;
		default:
			value = (ch & 0x7) << 18;
			ch = us[i++];
			value += (ch & 0x3F) << 12;
			ch = us[i++];
			value += (ch & 0x3F) << 6;
			ch = us[i++];
			value += ch & 0x3F;
			break;
		}
		tbuf[ui] = value;
		ui++;
	}
	return ui;
}

unsigned int UTF16FromUTF32Character(unsigned int val, wchar_t *tbuf) {
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
// https://www.unicode.org/versions/Unicode10.0.0/
//  3.9 Unicode Encoding Forms
//      Table 3-7. Well-Formed UTF-8 Byte Sequences
/*
UTF8-octets = *(UTF8-char)
UTF8-char   = UTF8-1 / UTF8-2 / UTF8-3 / UTF8-4
UTF8-1      = 00-7F
UTF8-2      = C2-DF         UTF8-tail   4
UTF8-3      = E0        5   A0-BF       11  UTF8-tail
              E1-EC     6   UTF8-tail   4   UTF8-tail
              ED        7   80-9F       12  UTF8-tail
              EE-EF     6   UTF8-tail   4   UTF8-tail
UTF8-4      = F0        8   90-BF       13  UTF8-tail   UTF8-tail
              F1-F3     9   UTF8-tail   4   UTF8-tail   UTF8-tail
              F4        10  80-8F       14  UTF8-tail   UTF8-tail
UTF8-tail   = 80-BF     4

bit 0-2: octet count, invalid bytes is treated as isolated single byte.
bit 3: leading byte
bit 4: tailing byte

UTF8-2:
    (1 << 3)  | (1 << 4)
UTF8-3:
    (1 << 5)  | (1 << 11)
    (1 << 6)  | (1 << 4)
    (1 << 7)  | (1 << 12)
UTF8-4:
    (1 << 8)  | (1 << 13)
    (1 << 9)  | (1 << 4)
    (1 << 10) | (1 << 14)
*/

enum {
	UTF8_3ByteMask1 = (1 << 5)  | (1 << 11) | ((1 << 4) << 11),
	UTF8_3ByteMask2 = (1 << 6)  | (1 << 4)  | ((1 << 4) << 11),
	UTF8_3ByteMask3 = (1 << 7)  | (1 << 12) | ((1 << 4) << 11),

	UTF8_4ByteMask1 = (1 << 8)  | (1 << 13) | ((1 << 4) << 11) | ((1 << 4) << 12),
	UTF8_4ByteMask2 = (1 << 9)  | (1 << 4)  | ((1 << 4) << 11) | ((1 << 4) << 12),
	UTF8_4ByteMask3 = (1 << 10) | (1 << 14) | ((1 << 4) << 11) | ((1 << 4) << 12),
};

#define mask_match(mask, test)	((mask & test) == test)

const unsigned short UTF8ClassifyTable[256] = {
0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, // 00 - 0F
0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, // 10 - 1F
0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, // 20 - 2F
0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, // 30 - 3F
0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, // 40 - 4F
0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, // 50 - 5F
0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, // 60 - 6F
0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, 0x0009, // 70 - 7F
0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, 0x5011, // 80 - 8F
0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, 0x3011, // 90 - 9F
0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, // A0 - AF
0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, 0x2811, // B0 - BF
0x0001, 0x0001, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, // C0 - CF
0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, 0x000A, // D0 - DF
0x002B, 0x004B, 0x004B, 0x004B, 0x004B, 0x004B, 0x004B, 0x004B, 0x004B, 0x004B, 0x004B, 0x004B, 0x004B, 0x008B, 0x004B, 0x004B, // E0 - EF
0x010C, 0x020C, 0x020C, 0x020C, 0x040C, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, // F0 - FF
};

// Return both the width of the first character in the string and a status
// saying whether it is valid or invalid.
// Most invalid sequences return a width of 1 so are treated as isolated bytes but
// the non-characters *FFFE, *FFFF and FDD0 .. FDEF return 3 or 4 as they can be
// reasonably treated as code points in some circumstances. They will, however,
// not have associated glyphs.
int UTF8Classify(const unsigned char *us, int len) {
	// For the rules: http://www.cl.cam.ac.uk/~mgk25/unicode.html#utf-8
	if (*us < 0x80) {
		// Single bytes easy
		return 1;
	}

	unsigned int mask = UTF8ClassifyTable[*us];
	const int byteCount = mask & UTF8ClassifyMaskOctetCount;
	if (byteCount == 1 || byteCount > len) {
		return UTF8MaskInvalid | 1;
	}

	mask |= UTF8ClassifyTable[us[1]];
	switch (byteCount) {
	case 2:
		if (mask & UTF8ClassifyMaskTrailByte) {
			return 2;
		}
		break;

	case 3:
		mask |= (UTF8ClassifyTable[us[2]] & UTF8ClassifyMaskTrailByte) << 11;
		if (mask_match(mask, UTF8_3ByteMask1) || mask_match(mask, UTF8_3ByteMask2) || mask_match(mask, UTF8_3ByteMask3)) {
			mask = ((us[0] & 0xF) << 12) | ((us[1] & 0x3F) << 6) | (us[2] & 0x3F);
			if (mask == 0xFFFE || mask == 0xFFFF || (mask >= 0xFDD0 && mask <= 0xFDEF)) {
				// U+FFFE or U+FFFF, FDD0 .. FDEF non-character
				return UTF8MaskInvalid | 3;
			}
			return 3;
		}
		break;

	default:
		mask |= (UTF8ClassifyTable[us[2]] & UTF8ClassifyMaskTrailByte) << 11;
		mask |= (UTF8ClassifyTable[us[3]] & UTF8ClassifyMaskTrailByte) << 12;
		if (mask_match(mask, UTF8_4ByteMask1) || mask_match(mask, UTF8_4ByteMask2) || mask_match(mask, UTF8_4ByteMask3)) {
			mask = ((us[1] & 0x3F) << 12) | ((us[2] & 0x3F) << 6) | (us[3] & 0x3F);
			mask &= 0xFFFF;
			if (mask == 0xFFFE || mask == 0xFFFF) {
				// *FFFE or *FFFF non-character
				return UTF8MaskInvalid | 4;
			}
			return 4;
		}
		break;
	}

	return UTF8MaskInvalid | 1;
}

int UTF8DrawBytes(const unsigned char *us, int len) {
	const int utf8StatusNext = UTF8Classify(us, len);
	return (utf8StatusNext & UTF8MaskInvalid) ? 1 : (utf8StatusNext & UTF8MaskWidth);
}

// Replace invalid bytes in UTF-8 with the replacement character
std::string FixInvalidUTF8(const std::string &text) {
	std::string result;
	const unsigned char *us = reinterpret_cast<const unsigned char *>(text.c_str());
	size_t remaining = text.size();
	while (remaining > 0) {
		const int utf8Status = UTF8Classify(us, static_cast<int>(remaining));
		if (utf8Status & UTF8MaskInvalid) {
			// Replacement character 0xFFFD = UTF8:"efbfbd".
			result.append("\xef\xbf\xbd");
			us++;
			remaining--;
		} else {
			const int len = utf8Status & UTF8MaskWidth;
			result.append(reinterpret_cast<const char *>(us), len);
			us += len;
			remaining -= len;
		}
	}
	return result;
}

}
