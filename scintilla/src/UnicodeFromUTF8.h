// Scintilla source code edit control
/** @file UnicodeFromUTF8.h
 ** Lexer infrastructure.
 **/
// Copyright 2013 by Neil Hodgson <neilh@scintilla.org>
// This file is in the public domain.

#ifndef UNICODEFROMUTF8_H
#define UNICODEFROMUTF8_H

namespace Scintilla {

static inline int UnicodeFromUTF8(const unsigned char *us) {
	switch (UTF8BytesOfLead[us[0]]) {
	case 1:
		return us[0];
	case 2:
		return ((us[0] & 0x1F) << 6) + (us[1] & 0x3F);
	case 3:
		return ((us[0] & 0xF) << 12) + ((us[1] & 0x3F) << 6) + (us[2] & 0x3F);
	case 4:
		return ((us[0] & 0x7) << 18) + ((us[1] & 0x3F) << 12) + ((us[2] & 0x3F) << 6) + (us[3] & 0x3F);
	}
	return us[0];
}

}

#endif
