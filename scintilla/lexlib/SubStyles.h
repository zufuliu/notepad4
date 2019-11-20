// Scintilla source code edit control
/** @file SubStyles.h
 ** Manage substyles for a lexer.
 **/
// Copyright 2012 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla {

class WordClassifier {
	int baseStyle;
	int firstStyle;
	int lenStyles;
	std::map<std::string, int> wordToStyle;

public:

	explicit WordClassifier(int baseStyle_) noexcept : baseStyle(baseStyle_), firstStyle(0), lenStyles(0) {
	}

	void Allocate(int firstStyle_, int lenStyles_) noexcept {
		firstStyle = firstStyle_;
		lenStyles = lenStyles_;
		wordToStyle.clear();
	}

	int Base() const noexcept {
		return baseStyle;
	}

	int Start() const noexcept {
		return firstStyle;
	}

	int Last() const noexcept {
		return firstStyle + lenStyles - 1;
	}

	int Length() const noexcept {
		return lenStyles;
	}

	void Clear() noexcept {
		firstStyle = 0;
		lenStyles = 0;
		wordToStyle.clear();
	}

	int ValueFor(const std::string &s) const {
		const auto it = wordToStyle.find(s);
		if (it != wordToStyle.end())
			return it->second;
		else
			return -1;
	}

	bool IncludesStyle(int style) const noexcept {
		return (style >= firstStyle) && (style < (firstStyle + lenStyles));
	}

	void RemoveStyle(int style) {
		auto it = wordToStyle.begin();
		while (it != wordToStyle.end()) {
			if (it->second == style) {
				it = wordToStyle.erase(it);
			} else {
				++it;
			}
		}
	}

	void SetIdentifiers(int style, const char *identifiers) {
		RemoveStyle(style);
		while (*identifiers) {
			const char *cpSpace = identifiers;
			while (*cpSpace && !(*cpSpace == ' ' || *cpSpace == '\t' || *cpSpace == '\r' || *cpSpace == '\n'))
				cpSpace++;
			if (cpSpace > identifiers) {
				std::string word(identifiers, cpSpace - identifiers);
				wordToStyle[word] = style;
			}
			identifiers = cpSpace;
			if (*identifiers)
				identifiers++;
		}
	}
};

class SubStyles {
	int classifications;
	const char *baseStyles;
	int styleFirst;
	int stylesAvailable;
	int secondaryDistance;
	int allocated;
	std::vector<WordClassifier> classifiers;

	int BlockFromBaseStyle(int baseStyle) const noexcept {
		for (int b = 0; b < classifications; b++) {
			if (baseStyle == baseStyles[b])
				return b;
		}
		return -1;
	}

	int BlockFromStyle(int style) const noexcept {
		int b = 0;
		for (const auto &classifier : classifiers) {
			if (classifier.IncludesStyle(style))
				return b;
			b++;
		}
		return -1;
	}

public:

	SubStyles(const char *baseStyles_, int styleFirst_, int stylesAvailable_, int secondaryDistance_) :
		classifications(0),
		baseStyles(baseStyles_),
		styleFirst(styleFirst_),
		stylesAvailable(stylesAvailable_),
		secondaryDistance(secondaryDistance_),
		allocated(0) {
		while (baseStyles[classifications]) {
			classifiers.emplace_back(baseStyles[classifications]);
			classifications++;
		}
	}

	int Allocate(int styleBase, int numberStyles) {
		const int block = BlockFromBaseStyle(styleBase);
		if (block >= 0) {
			if ((allocated + numberStyles) > stylesAvailable)
				return -1;
			const int startBlock = styleFirst + allocated;
			allocated += numberStyles;
			classifiers[block].Allocate(startBlock, numberStyles);
			return startBlock;
		} else {
			return -1;
		}
	}

	int Start(int styleBase) const noexcept {
		const int block = BlockFromBaseStyle(styleBase);
		return (block >= 0) ? classifiers[block].Start() : -1;
	}

	int Length(int styleBase) const noexcept {
		const int block = BlockFromBaseStyle(styleBase);
		return (block >= 0) ? classifiers[block].Length() : 0;
	}

	int BaseStyle(int subStyle) const noexcept {
		const int block = BlockFromStyle(subStyle);
		if (block >= 0)
			return classifiers[block].Base();
		else
			return subStyle;
	}

	int DistanceToSecondaryStyles() const noexcept {
		return secondaryDistance;
	}

	int FirstAllocated() const noexcept {
		int start = 257;
		for (const auto &classifier : classifiers) {
			if (start > classifier.Start())
				start = classifier.Start();
		}
		return (start < 256) ? start : -1;
	}

	int LastAllocated() const noexcept {
		int last = -1;
		for (const auto &classifier : classifiers) {
			if (last < classifier.Last())
				last = classifier.Last();
		}
		return last;
	}

	void SetIdentifiers(int style, const char *identifiers) {
		const int block = BlockFromStyle(style);
		if (block >= 0)
			classifiers[block].SetIdentifiers(style, identifiers);
	}

	void Free() noexcept {
		allocated = 0;
		for (auto &classifier : classifiers)
			classifier.Clear();
	}

	const WordClassifier &Classifier(int baseStyle) const {
		const int block = BlockFromBaseStyle(baseStyle);
		return classifiers[block >= 0 ? block : 0];
	}
};

}
