// Scintilla source code edit control
/** @file DBCS.cxx
 ** Functions to handle DBCS double byte encodings like Shift-JIS.
 **/
// Copyright 2017 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include "DBCS.h"

using namespace Scintilla;

namespace Scintilla {

bool DBCSIsLeadByte(int codePage, char ch) noexcept {
	// Byte ranges found in Wikipedia articles with relevant search strings in each case
	const unsigned char uch = ch;
	switch (codePage) {
	case 932:
		// Shift_jis
		return ((uch >= 0x81) && (uch <= 0x9F)) ||
			((uch >= 0xE0) && (uch <= 0xFC));
		// Lead bytes F0 to FC may be a Microsoft addition.
	case 936:
		// GBK
		return (uch >= 0x81) && (uch <= 0xFE);
	case 949:
		// Korean Wansung KS C-5601-1987
		return (uch >= 0x81) && (uch <= 0xFE);
	case 950:
		// Big5
		return (uch >= 0x81) && (uch <= 0xFE);
	case 1361:
		// Korean Johab KS C-5601-1992
		return
			((uch >= 0x84) && (uch <= 0xD3)) ||
			((uch >= 0xD8) && (uch <= 0xDE)) ||
			((uch >= 0xE0) && (uch <= 0xF9));
	}
	return false;
}

bool DBCSIsLeadByteInvalid(int codePage, char ch) noexcept {
	const unsigned char lead = ch;
	switch (codePage) {
	case 932:
		// Shift_jis
		return
			(lead == 0x85) ||
			(lead == 0x86) ||
			(lead == 0xEB) ||
			(lead == 0xEC) ||
			(lead == 0xEF) ||
			(lead == 0xFA) ||
			(lead == 0xFB) ||
			(lead == 0xFC);
	case 936:
		// GBK
		return (lead == 0x80) || (lead == 0xFF);
	case 949:
		// Korean Wansung KS C-5601-1987
		return (lead == 0x80) || (lead == 0xC9) || (lead >= 0xFE);
	case 950:
		// Big5
		return
			((lead >= 0x80) && (lead <= 0xA0)) ||
			(lead == 0xC8) ||
			(lead >= 0xFA);
	case 1361:
		// Korean Johab KS C-5601-1992
		return
			((lead >= 0x80) && (lead <= 0x83)) ||
			((lead >= 0xD4) && (lead <= 0xD8)) ||
			(lead == 0xDF) ||
			(lead >= 0xFA);
	}
	return false;
}

bool DBCSIsTrailByteInvalid(int codePage, char ch) noexcept {
	const unsigned char trail = ch;
	switch (codePage) {
	case 932:
		// Shift_jis
		return
			(trail <= 0x3F) ||
			(trail == 0x7F) ||
			(trail >= 0xFD);
	case 936:
		// GBK
		return
			(trail <= 0x3F) ||
			(trail == 0x7F) ||
			(trail == 0xFF);
	case 949:
		// Korean Wansung KS C-5601-1987
		return
			(trail <= 0x40) ||
			((trail >= 0x5B) && (trail <= 0x60)) ||
			((trail >= 0x7B) && (trail <= 0x80)) ||
			(trail == 0xFF);
	case 950:
		// Big5
		return
			(trail <= 0x3F) ||
			((trail >= 0x7F) && (trail <= 0xA0)) ||
			(trail == 0xFF);
	case 1361:
		// Korean Johab KS C-5601-1992
		return
			(trail <= 0x30) ||
			(trail == 0x7F) ||
			(trail == 0x80) ||
			(trail == 0xFF);
	}
	return false;
}

}
