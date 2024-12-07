// Scintilla source code edit control
/** @file AutoComplete.h
 ** Defines the auto completion list box.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

/**
 */
class AutoComplete {
	bool active = false;
	char separator = '\n';
	char typesep = '\t'; // Type separator
	std::string stopChars;
	std::string fillUpChars;
	std::vector<int> sortMatrix;

public:

	bool ignoreCase = false;
	bool chooseSingle = false;
	AutoCompleteOption options = Scintilla::AutoCompleteOption::Normal;
	const std::unique_ptr<ListBox> lb;
	Sci::Position posStart = 0;
	Sci::Position startLen = 0;
	/// Should autocompletion be cancelled if editor's currentPos <= startPos?
	bool cancelAtStartPos = true;
	bool autoHide = true;
	bool dropRestOfWord = false;
	Scintilla::CaseInsensitiveBehaviour ignoreCaseBehaviour = Scintilla::CaseInsensitiveBehaviour::RespectCase;
	int widthLBDefault = 100;
	int heightLBDefault = 100;
	/** Ordering::PreSorted:   Assume the list is presorted; selection will fail if it is not alphabetical<br />
	 *  Ordering::PerformSort: Sort the list alphabetically; start up performance cost for sorting<br />
	 *  Ordering::Custom:      Handle non-alphabetical entries; start up performance cost for generating a sorted lookup table
	 */
	Scintilla::Ordering autoSort = Scintilla::Ordering::PreSorted;

	AutoComplete();
	// Deleted so AutoComplete objects can not be copied.
	AutoComplete(const AutoComplete &) = delete;
	AutoComplete(AutoComplete &&) = delete;
	AutoComplete &operator=(const AutoComplete &) = delete;
	AutoComplete &operator=(AutoComplete &&) = delete;
	~AutoComplete();

	/// Is the auto completion list displayed?
	bool Active() const noexcept {
		return active;
	}

	/// Display the auto completion list positioned to be near a character position
	void SCICALL Start(Window &parent, int ctrlID, Sci::Position position, Point location,
		Sci::Position startLen_, int lineHeight, bool unicodeMode, Scintilla::Technology technology,
		ListOptions listOptions) noexcept;

	/// The stop chars are characters which, when typed, cause the auto completion list to disappear
	void SetStopChars(const char *stopChars_);
	bool IsStopChar(char ch) const noexcept;

	/// The fillup chars are characters which, when typed, fill up the selected word
	void SetFillUpChars(const char *fillUpChars_);
	bool IsFillUpChar(char ch) const noexcept;

	/// The separator character is used when interpreting the list in SetList
	void SetSeparator(char separator_) noexcept {
		separator = separator_;
	}
	char GetSeparator() const noexcept {
		return separator;
	}

	/// The typesep character is used for separating the word from the type
	void SetTypesep(char separator_) noexcept {
		typesep = separator_;
	}
	char GetTypesep() const noexcept {
		return typesep;
	}

	/// The list string contains a sequence of words separated by the separator character
	void SetList(const char *list);

	/// Return the position of the currently selected list item
	int GetSelection() const noexcept;

	/// Return the value of an item in the list
	std::string GetValue(int item) const;

	void Show(bool show) const;
	void Cancel() noexcept;

	/// Move the current list element by delta, scrolling appropriately
	void Move(int delta) const;

	/// Select a list element that starts with word as the current element
	void Select(const char *word);
};

}
