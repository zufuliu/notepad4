/** @file Decoration.cxx
 ** Visual elements added over text.
 **/
// Copyright 1998-2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>

#include <stdexcept>
#include <algorithm>

#include "Platform.h"

#include "Scintilla.h"
#include "Position.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "Decoration.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

Decoration::Decoration(int indicator_) : indicator(indicator_), next(0) {
}

Decoration::~Decoration() {
}

bool Decoration::Empty() const {
	return (rs.Runs() == 1) && (rs.AllSameAs(0));
}

DecorationList::DecorationList() : currentIndicator(0), currentValue(1), current(0),
	lengthDocument(0), root(0), clickNotified(false) {
}

DecorationList::~DecorationList() {
	Decoration *deco = root;
	while (deco) {
		Decoration *decoNext = deco->Next();
		delete deco;
		deco = decoNext;
	}
	root = 0;
	current = 0;
}

Decoration *DecorationList::DecorationFromIndicator(int indicator) {
	for (Decoration *deco=root; deco; deco = deco->Next()) {
		if (deco->Indicator() == indicator) {
			return deco;
		}
	}
	return 0;
}

Decoration *DecorationList::Create(int indicator, int length) {
	currentIndicator = indicator;
	Decoration *decoNew = new Decoration(indicator);
	decoNew->rs.InsertSpace(0, length);

	Decoration *decoPrev = 0;
	Decoration *deco = root;

	while (deco && (deco->Indicator() < indicator)) {
		decoPrev = deco;
		deco = deco->Next();
	}
	if (decoPrev == 0) {
		decoNew->next = root;
		root = decoNew;
	} else {
		decoNew->next = deco;
		decoPrev->next = decoNew;
	}
	return decoNew;
}

void DecorationList::Delete(int indicator) {
	Decoration *decoToDelete = 0;
	if (root) {
		if (root->Indicator() == indicator) {
			decoToDelete = root;
			root = root->Next();
		} else {
			Decoration *deco = root;
			while (deco && deco->Next() && !decoToDelete) {
				// decoNext ensures no warning from MSVC Code Analysis
				Decoration *decoNext = deco->Next();
				if (decoNext && deco->Next()->Indicator() == indicator) {
					decoToDelete = decoNext;
					deco->next = decoToDelete->Next();
				} else {
					deco = deco->Next();
				}
			}
		}
	}
	if (decoToDelete) {
		delete decoToDelete;
		current = 0;
	}
}

void DecorationList::SetCurrentIndicator(int indicator) {
	currentIndicator = indicator;
	current = DecorationFromIndicator(indicator);
	currentValue = 1;
}

void DecorationList::SetCurrentValue(int value) {
	currentValue = value ? value : 1;
}

bool DecorationList::FillRange(int &position, int value, int &fillLength) {
	if (!current) {
		current = DecorationFromIndicator(currentIndicator);
		if (!current) {
			current = Create(currentIndicator, lengthDocument);
		}
	}
	const bool changed = current->rs.FillRange(position, value, fillLength);
	if (current->Empty()) {
		Delete(currentIndicator);
	}
	return changed;
}

void DecorationList::InsertSpace(int position, int insertLength) {
	const bool atEnd = position == lengthDocument;
	lengthDocument += insertLength;
	for (Decoration *deco=root; deco; deco = deco->Next()) {
		deco->rs.InsertSpace(position, insertLength);
		if (atEnd) {
			deco->rs.FillRange(position, 0, insertLength);
		}
	}
}

void DecorationList::DeleteRange(int position, int deleteLength) {
	lengthDocument -= deleteLength;
	Decoration *deco;
	for (deco=root; deco; deco = deco->Next()) {
		deco->rs.DeleteRange(position, deleteLength);
	}
	DeleteAnyEmpty();
}

void DecorationList::DeleteAnyEmpty() {
	Decoration *deco = root;
	while (deco) {
		if ((lengthDocument == 0) || deco->Empty()) {
			Delete(deco->Indicator());
			deco = root;
		} else {
			deco = deco->Next();
		}
	}
}

int DecorationList::AllOnFor(int position) const {
	int mask = 0;
	for (Decoration *deco=root; deco; deco = deco->Next()) {
		if (deco->rs.ValueAt(position)) {
			if (deco->Indicator() < INDIC_IME) {
				mask |= 1 << deco->Indicator();
			}
		}
	}
	return mask;
}

int DecorationList::ValueAt(int indicator, int position) {
	const Decoration *deco = DecorationFromIndicator(indicator);
	if (deco) {
		return deco->rs.ValueAt(position);
	}
	return 0;
}

int DecorationList::Start(int indicator, int position) {
	const Decoration *deco = DecorationFromIndicator(indicator);
	if (deco) {
		return deco->rs.StartRun(position);
	}
	return 0;
}

int DecorationList::End(int indicator, int position) {
	const Decoration *deco = DecorationFromIndicator(indicator);
	if (deco) {
		return deco->rs.EndRun(position);
	}
	return 0;
}
