// Scintilla source code edit control
/** @file ContractionState.cxx
 ** Manages visibility of lines for folding and wrapping.
 **/
// Copyright 1998-2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cassert>
#include <cstring>

#include <stdexcept>
#include <algorithm>

#include "Platform.h"

#include "Position.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "SparseVector.h"
#include "ContractionState.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

ContractionState::ContractionState() : visible(0), expanded(0), heights(0), foldDisplayTexts(0), displayLines(0), linesInDocument(1) {
	//InsertLine(0);
}

ContractionState::~ContractionState() {
	Clear();
}

void ContractionState::EnsureData() {
	if (OneToOne()) {
		visible = new RunStyles();
		expanded = new RunStyles();
		heights = new RunStyles();
		foldDisplayTexts = new SparseVector<const char *>();
		displayLines = new Partitioning(4);
		InsertLines(0, linesInDocument);
	}
}

void ContractionState::Clear() {
	delete visible;
	visible = 0;
	delete expanded;
	expanded = 0;
	delete heights;
	heights = 0;
	delete foldDisplayTexts;
	foldDisplayTexts = 0;
	delete displayLines;
	displayLines = 0;
	linesInDocument = 1;
}

Sci::Line ContractionState::LinesInDoc() const {
	if (OneToOne()) {
		return linesInDocument;
	} else {
		return displayLines->Partitions() - 1;
	}
}

Sci::Line ContractionState::LinesDisplayed() const {
	if (OneToOne()) {
		return linesInDocument;
	} else {
		return displayLines->PositionFromPartition(LinesInDoc());
	}
}

Sci::Line ContractionState::DisplayFromDoc(Sci::Line lineDoc) const {
	if (OneToOne()) {
		return (lineDoc <= linesInDocument) ? lineDoc : linesInDocument;
	} else {
		if (lineDoc > displayLines->Partitions())
			lineDoc = displayLines->Partitions();
		return displayLines->PositionFromPartition(lineDoc);
	}
}

Sci::Line ContractionState::DisplayLastFromDoc(Sci::Line lineDoc) const {
	return DisplayFromDoc(lineDoc) + GetHeight(lineDoc) - 1;
}

Sci::Line ContractionState::DocFromDisplay(Sci::Line lineDisplay) const {
	if (OneToOne()) {
		return lineDisplay;
	} else {
		if (lineDisplay <= 0) {
			return 0;
		}
		if (lineDisplay > LinesDisplayed()) {
			return displayLines->PartitionFromPosition(LinesDisplayed());
		}
		Sci::Line lineDoc = displayLines->PartitionFromPosition(lineDisplay);
		PLATFORM_ASSERT(GetVisible(lineDoc));
		return lineDoc;
	}
}

void ContractionState::InsertLine(Sci::Line lineDoc) {
	if (OneToOne()) {
		linesInDocument++;
	} else {
		visible->InsertSpace(lineDoc, 1);
		visible->SetValueAt(lineDoc, 1);
		expanded->InsertSpace(lineDoc, 1);
		expanded->SetValueAt(lineDoc, 1);
		heights->InsertSpace(lineDoc, 1);
		heights->SetValueAt(lineDoc, 1);
		foldDisplayTexts->InsertSpace(lineDoc, 1);
		foldDisplayTexts->SetValueAt(lineDoc, NULL);
		Sci::Line lineDisplay = DisplayFromDoc(lineDoc);
		displayLines->InsertPartition(lineDoc, lineDisplay);
		displayLines->InsertText(lineDoc, 1);
	}
}

void ContractionState::InsertLines(Sci::Line lineDoc, Sci::Line lineCount) {
	for (int l = 0; l < lineCount; l++) {
		InsertLine(lineDoc + l);
	}
	Check();
}

void ContractionState::DeleteLine(Sci::Line lineDoc) {
	if (OneToOne()) {
		linesInDocument--;
	} else {
		if (GetVisible(lineDoc)) {
			displayLines->InsertText(lineDoc, -heights->ValueAt(lineDoc));
		}
		displayLines->RemovePartition(lineDoc);
		visible->DeleteRange(lineDoc, 1);
		expanded->DeleteRange(lineDoc, 1);
		heights->DeleteRange(lineDoc, 1);
		foldDisplayTexts->DeletePosition(lineDoc);
	}
}

void ContractionState::DeleteLines(Sci::Line lineDoc, Sci::Line lineCount) {
	for (Sci::Line l = 0; l < lineCount; l++) {
		DeleteLine(lineDoc);
	}
	Check();
}

bool ContractionState::GetVisible(Sci::Line lineDoc) const {
	if (OneToOne()) {
		return true;
	} else {
		if (lineDoc >= visible->Length())
			return true;
		return visible->ValueAt(lineDoc) == 1;
	}
}

bool ContractionState::SetVisible(Sci::Line lineDocStart, Sci::Line lineDocEnd, bool isVisible) {
	if (OneToOne() && isVisible) {
		return false;
	} else {
		EnsureData();
		Sci::Line delta = 0;
		Check();
		if ((lineDocStart <= lineDocEnd) && (lineDocStart >= 0) && (lineDocEnd < LinesInDoc())) {
			for (Sci::Line line = lineDocStart; line <= lineDocEnd; line++) {
				if (GetVisible(line) != isVisible) {
					int difference = isVisible ? heights->ValueAt(line) : -heights->ValueAt(line);
					visible->SetValueAt(line, isVisible ? 1 : 0);
					displayLines->InsertText(line, difference);
					delta += difference;
				}
			}
		} else {
			return false;
		}
		Check();
		return delta != 0;
	}
}

bool ContractionState::HiddenLines() const {
	if (OneToOne()) {
		return false;
	} else {
		return !visible->AllSameAs(1);
	}
}

const char *ContractionState::GetFoldDisplayText(Sci::Line lineDoc) const {
	Check();
	return foldDisplayTexts->ValueAt(lineDoc);
}

bool ContractionState::SetFoldDisplayText(Sci::Line lineDoc, const char *text) {
	EnsureData();
	const char *foldText = foldDisplayTexts->ValueAt(lineDoc);
	if (!foldText || 0 != strcmp(text, foldText)) {
		foldDisplayTexts->SetValueAt(lineDoc, text);
		Check();
		return true;
	} else {
		Check();
		return false;
	}
}

bool ContractionState::GetExpanded(Sci::Line lineDoc) const {
	if (OneToOne()) {
		return true;
	} else {
		Check();
		return expanded->ValueAt(lineDoc) == 1;
	}
}

bool ContractionState::SetExpanded(Sci::Line lineDoc, bool isExpanded) {
	if (OneToOne() && isExpanded) {
		return false;
	} else {
		EnsureData();
		if (isExpanded != (expanded->ValueAt(lineDoc) == 1)) {
			expanded->SetValueAt(lineDoc, isExpanded ? 1 : 0);
			Check();
			return true;
		} else {
			Check();
			return false;
		}
	}
}

bool ContractionState::GetFoldDisplayTextShown(Sci::Line lineDoc) const {
	return !GetExpanded(lineDoc) && GetFoldDisplayText(lineDoc);
}

Sci::Line ContractionState::ContractedNext(Sci::Line lineDocStart) const {
	if (OneToOne()) {
		return -1;
	} else {
		Check();
		if (!expanded->ValueAt(lineDocStart)) {
			return lineDocStart;
		} else {
			Sci::Line lineDocNextChange = expanded->EndRun(lineDocStart);
			if (lineDocNextChange < LinesInDoc())
				return lineDocNextChange;
			else
				return -1;
		}
	}
}

int ContractionState::GetHeight(Sci::Line lineDoc) const {
	if (OneToOne()) {
		return 1;
	} else {
		return heights->ValueAt(lineDoc);
	}
}

// Set the number of display lines needed for this line.
// Return true if this is a change.
bool ContractionState::SetHeight(Sci::Line lineDoc, int height) {
	if (OneToOne() && (height == 1)) {
		return false;
	} else if (lineDoc < LinesInDoc()) {
		EnsureData();
		if (GetHeight(lineDoc) != height) {
			if (GetVisible(lineDoc)) {
				displayLines->InsertText(lineDoc, height - GetHeight(lineDoc));
			}
			heights->SetValueAt(lineDoc, height);
			Check();
			return true;
		} else {
			Check();
			return false;
		}
	} else {
		return false;
	}
}

void ContractionState::ShowAll() {
	Sci::Line lines = LinesInDoc();
	Clear();
	linesInDocument = lines;
}

// Debugging checks

void ContractionState::Check() const {
#ifdef CHECK_CORRECTNESS
	for (Sci::Line vline = 0; vline < LinesDisplayed(); vline++) {
		const Sci::Line lineDoc = DocFromDisplay(vline);
		PLATFORM_ASSERT(GetVisible(lineDoc));
	}
	for (Sci::Line lineDoc = 0; lineDoc < LinesInDoc(); lineDoc++) {
		const Sci::Line displayThis = DisplayFromDoc(lineDoc);
		const Sci::Line displayNext = DisplayFromDoc(lineDoc + 1);
		const Sci::Line height = displayNext - displayThis;
		PLATFORM_ASSERT(height >= 0);
		if (GetVisible(lineDoc)) {
			PLATFORM_ASSERT(GetHeight(lineDoc) == height);
		} else {
			PLATFORM_ASSERT(0 == height);
		}
	}
#endif
}
