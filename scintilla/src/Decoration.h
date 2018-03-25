/** @file Decoration.h
 ** Visual elements added over text.
 **/
// Copyright 1998-2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef DECORATION_H
#define DECORATION_H

namespace Scintilla {

class Decoration {
	int indicator;
public:
	RunStyles<Sci::Position, int> rs;

	explicit Decoration(int indicator_);
	~Decoration();

	bool Empty() const;
	int Indicator() const {
		return indicator;
	}
};

class DecorationList {
	int currentIndicator;
	int currentValue;
	Decoration *current;	// Cached so FillRange doesn't have to search for each call.
	Sci::Position lengthDocument;
	// Ordered by indicator
	std::vector<std::unique_ptr<Decoration>> decorationList;
	std::vector<const Decoration*> decorationView;	// Read-only view of decorationList
	bool clickNotified;

	Decoration *DecorationFromIndicator(int indicator);
	Decoration *Create(int indicator, Sci::Position length);
	void Delete(int indicator);
	void DeleteAnyEmpty();
	void SetView();
public:

	DecorationList();
	~DecorationList();

	const std::vector<const Decoration*> &View() const { return decorationView; }

	void SetCurrentIndicator(int indicator);
	int GetCurrentIndicator() const { return currentIndicator; }

	void SetCurrentValue(int value);
	int GetCurrentValue() const { return currentValue; }

	// Returns true if some values may have changed
	bool FillRange(Sci::Position &position, int value, Sci::Position &fillLength);

	void InsertSpace(Sci::Position position, Sci::Position insertLength);
	void DeleteRange(Sci::Position position, Sci::Position deleteLength);

	void DeleteLexerDecorations();

	int AllOnFor(Sci::Position position) const;
	int ValueAt(int indicator, Sci::Position position);
	Sci::Position Start(int indicator, Sci::Position position);
	Sci::Position End(int indicator, Sci::Position position);

	bool ClickNotified() const {
		return clickNotified;
	}
	void SetClickNotified(bool notified) {
		clickNotified = notified;
	}
};

}

#endif
