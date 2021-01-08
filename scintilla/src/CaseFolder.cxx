// Scintilla source code edit control
/** @file CaseFolder.cxx
 ** Classes for case folding.
 **/
// Copyright 1998-2013 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdexcept>
#include <string>

#include "CaseFolder.h"
#include "CaseConvert.h"

using namespace Scintilla;

template <typename T>
static constexpr T MakeLowerCase(T ch) noexcept {
	return (ch >= 'A' && ch <= 'Z') ? (ch - 'A' + 'a') : ch;
}

CaseFolder::~CaseFolder() = default;

CaseFolderTable::CaseFolderTable() noexcept {
	for (int iChar = 0; iChar < 256; iChar++) {
		mapping[iChar] = static_cast<char>(MakeLowerCase(iChar));
	}
}

CaseFolderTable::~CaseFolderTable() = default;

size_t CaseFolderTable::Fold(char *folded, size_t sizeFolded, const char *mixed, size_t lenMixed) {
	if (lenMixed > sizeFolded) {
		return 0;
	} else {
		for (size_t i = 0; i < lenMixed; i++) {
			folded[i] = mapping[static_cast<unsigned char>(mixed[i])];
		}
		return lenMixed;
	}
}

void CaseFolderTable::SetTranslation(char ch, char chTranslation) noexcept {
	mapping[static_cast<unsigned char>(ch)] = chTranslation;
}

CaseFolderUnicode::CaseFolderUnicode() {
	converter = ConverterFor(CaseConversionFold);
}

size_t CaseFolderUnicode::Fold(char *folded, size_t sizeFolded, const char *mixed, size_t lenMixed) {
	if ((lenMixed == 1) && (sizeFolded > 0)) {
		folded[0] = mapping[static_cast<unsigned char>(mixed[0])];
		return 1;
	} else {
		return converter->CaseConvertString(folded, sizeFolded, mixed, lenMixed);
	}
}
