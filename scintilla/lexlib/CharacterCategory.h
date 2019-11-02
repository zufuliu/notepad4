// Scintilla source code edit control
/** @file CharacterCategory.h
 ** Returns the Unicode general category of a character.
 **/
// Copyright 2013 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef CHARACTERCATEGORY_H
#define CHARACTERCATEGORY_H

namespace Scintilla {

enum CharacterCategory {
	ccLu, ccLl, ccLt, ccLm, ccLo,
	ccMn, ccMc, ccMe,
	ccNd, ccNl, ccNo,
	ccPc, ccPd, ccPs, ccPe, ccPi, ccPf, ccPo,
	ccSm, ccSc, ccSk, ccSo,
	ccZs, ccZl, ccZp,
	ccCc, ccCf, ccCs, ccCo, ccCn
};

// `character` argument must be UTF-32 code point, otherwise the result is undefined.
// see https://sourceforge.net/p/scintilla/feature-requests/1259/ for these changes.
#define CHARACTERCATEGORY_USE_BINARY_SEARCH		0
// most calls already checked for ASCII, optimization for Latin-1 may not benefit a lot.
#define CHARACTERCATEGORY_OPTIMIZE_LATIN1		0

CharacterCategory CategoriseCharacter(int character) noexcept;

// Common definitions of allowable characters in identifiers from UAX #31.
bool IsIdStart(int character) noexcept;
bool IsIdContinue(int character) noexcept;
bool IsXidStart(int character) noexcept;
bool IsXidContinue(int character) noexcept;

class CharacterCategoryMap {
private:
	std::vector<unsigned char> dense;
public:
	CharacterCategoryMap();
	CharacterCategory CategoryFor(int character) const noexcept {
		if (static_cast<size_t>(character) < dense.size()) {
			return static_cast<CharacterCategory>(dense[character]);
		}
		// binary search through ranges
		return CategoriseCharacter(character);
	}
	int Size() const noexcept;
	void Optimize(int countCharacters);
};

}

#endif
