// Scintilla source code edit control
/** @file ContractionState.cxx
 ** Manages visibility of lines for folding and wrapping.
 **/
// Copyright 1998-2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cassert>
#include <cstring>
//#include <cstdio>

#include <stdexcept>
#include <utility>
#include <string_view>
#include <vector>
#include <optional>
#include <algorithm>
#include <memory>

#include "Debugging.h"

#include "Position.h"
#include "UniqueString.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "SparseVector.h"
#include "ContractionState.h"
//#include "ElapsedPeriod.h"

using namespace Scintilla::Internal;

#define ContractionState_InsertLines_OneByOne	0
#define ContractionState_DeleteLines_OneByOne	0

namespace {

template <typename LINE>
class ContractionState final : public IContractionState {
	// These contain 1 element for every document line.
	std::unique_ptr<RunStyles<LINE, char>> visible;
	std::unique_ptr<RunStyles<LINE, char>> expanded;
	std::unique_ptr<RunStyles<LINE, int>> heights;
#if EnablePerLineFoldDisplayText
	std::unique_ptr<SparseVector<UniqueString>> foldDisplayTexts;
#endif
	std::unique_ptr<Partitioning<LINE>> displayLines;
	Sci::Line linesInDocument = 1;

	void EnsureData();

	[[nodiscard]] bool OneToOne() const noexcept {
		// True when each document line is exactly one display line so need for
		// complex data structures.
		return visible == nullptr;
	}

#if ContractionState_InsertLines_OneByOne
	void InsertLine(Sci::Line lineDoc);
#endif
#if ContractionState_DeleteLines_OneByOne
	void DeleteLine(Sci::Line lineDoc);
#endif

	// line_cast(): cast Sci::Line to either 32-bit or 64-bit value
	// This avoids warnings from Visual C++ Code Analysis and shortens code
	static constexpr LINE line_cast(Sci::Line line) noexcept {
		return static_cast<LINE>(line);
	}

public:
	ContractionState() noexcept;

	void Clear() noexcept override;

	[[nodiscard]] Sci::Line LinesInDoc() const noexcept override;
	[[nodiscard]] Sci::Line LinesDisplayed() const noexcept override;
	[[nodiscard]] Sci::Line DisplayFromDoc(Sci::Line lineDoc) const noexcept override;
	[[nodiscard]] Sci::Line DisplayFromDocSub(Sci::Line lineDoc, Sci::Line lineSub) const noexcept override;
	[[nodiscard]] Sci::Line DisplayLastFromDoc(Sci::Line lineDoc) const noexcept override;
	[[nodiscard]] Sci::Line DocFromDisplay(Sci::Line lineDisplay) const noexcept override;

	void InsertLines(Sci::Line lineDoc, Sci::Line lineCount) override;
	void DeleteLines(Sci::Line lineDoc, Sci::Line lineCount) override;

	[[nodiscard]] bool GetVisible(Sci::Line lineDoc) const noexcept override;
	bool SetVisible(Sci::Line lineDocStart, Sci::Line lineDocEnd, bool isVisible) override;
	[[nodiscard]] bool HiddenLines() const noexcept override;

#if EnablePerLineFoldDisplayText
	[[nodiscard]] const char *GetFoldDisplayText(Sci::Line lineDoc) const noexcept override;
	bool SetFoldDisplayText(Sci::Line lineDoc, const char *text) override;
#endif

	[[nodiscard]] bool GetExpanded(Sci::Line lineDoc) const noexcept override;
	bool SetExpanded(Sci::Line lineDoc, bool isExpanded) override;
	bool ExpandAll() override;
	[[nodiscard]] Sci::Line ContractedNext(Sci::Line lineDocStart) const noexcept override;

	[[nodiscard]] int GetHeight(Sci::Line lineDoc) const noexcept override;
	bool SetHeight(Sci::Line lineDoc, int height) override;

	void ShowAll() noexcept override;

#ifdef CHECK_CORRECTNESS
	void Check() const noexcept;
#else
	void Check() const noexcept {}
#endif
};

template <typename LINE>
ContractionState<LINE>::ContractionState() noexcept = default;

template <typename LINE>
void ContractionState<LINE>::EnsureData() {
	if (OneToOne()) {
		visible = std::make_unique<RunStyles<LINE, char>>();
		expanded = std::make_unique<RunStyles<LINE, char>>();
		heights = std::make_unique<RunStyles<LINE, int>>();
#if EnablePerLineFoldDisplayText
		foldDisplayTexts = std::make_unique<SparseVector<UniqueString>>();
#endif
		displayLines = std::make_unique<Partitioning<LINE>>(4);
		displayLines->ReAllocate(linesInDocument + 2);
		InsertLines(0, linesInDocument);
	}
}

#if ContractionState_InsertLines_OneByOne
template <typename LINE>
void ContractionState<LINE>::InsertLine(Sci::Line lineDoc) {
	if (OneToOne()) {
		linesInDocument++;
	} else {
		const LINE lineDocCast = line_cast(lineDoc);
		visible->InsertSpace(lineDocCast, 1);
		visible->SetValueAt(lineDocCast, 1);
		expanded->InsertSpace(lineDocCast, 1);
		expanded->SetValueAt(lineDocCast, 1);
		heights->InsertSpace(lineDocCast, 1);
		heights->SetValueAt(lineDocCast, 1);
#if EnablePerLineFoldDisplayText
		foldDisplayTexts->InsertSpace(lineDocCast, 1);
		foldDisplayTexts->SetValueAt(lineDocCast, nullptr);
#endif
		const Sci::Line lineDisplay = DisplayFromDoc(lineDoc);
		displayLines->InsertPartition(lineDocCast, line_cast(lineDisplay));
		displayLines->InsertText(lineDocCast, 1);
	}
}
#endif

#if ContractionState_DeleteLines_OneByOne
template <typename LINE>
void ContractionState<LINE>::DeleteLine(Sci::Line lineDoc) {
	if (OneToOne()) {
		linesInDocument--;
	} else {
		const LINE lineDocCast = line_cast(lineDoc);
		if (visible->ValueAt(lineDocCast)) {
			displayLines->InsertText(lineDocCast, -heights->ValueAt(lineDocCast));
		}
		displayLines->RemovePartition(lineDocCast);
		visible->DeleteRange(lineDocCast, 1);
		expanded->DeleteRange(lineDocCast, 1);
		heights->DeleteRange(lineDocCast, 1);
#if EnablePerLineFoldDisplayText
		foldDisplayTexts->DeletePosition(lineDocCast);
#endif
	}
}
#endif

template <typename LINE>
void ContractionState<LINE>::Clear() noexcept {
	visible.reset();
	expanded.reset();
	heights.reset();
#if EnablePerLineFoldDisplayText
	foldDisplayTexts.reset();
#endif
	displayLines.reset();
	linesInDocument = 1;
}

#define OneToMany_LinesInDoc()		(displayLines->Partitions() - 1)

template <typename LINE>
Sci::Line ContractionState<LINE>::LinesInDoc() const noexcept {
	if (OneToOne()) {
		return linesInDocument;
	}
	return displayLines->Partitions() - 1;
}

template <typename LINE>
Sci::Line ContractionState<LINE>::LinesDisplayed() const noexcept {
	if (OneToOne()) {
		return linesInDocument;
	}
	return displayLines->PositionFromPartition(OneToMany_LinesInDoc());
}

template <typename LINE>
Sci::Line ContractionState<LINE>::DisplayFromDoc(Sci::Line lineDoc) const noexcept {
	if (OneToOne()) {
		return std::min(lineDoc, linesInDocument);
	}
	const LINE lineDocCast = std::min(line_cast(lineDoc), displayLines->Partitions());
	return displayLines->PositionFromPartition(lineDocCast);
}

template <typename LINE>
Sci::Line ContractionState<LINE>::DisplayFromDocSub(Sci::Line lineDoc, Sci::Line lineSub) const noexcept {
	return DisplayFromDoc(lineDoc) +
		std::min(lineSub, static_cast<Sci::Line>(GetHeight(lineDoc) - 1));
}

template <typename LINE>
Sci::Line ContractionState<LINE>::DisplayLastFromDoc(Sci::Line lineDoc) const noexcept {
	//return DisplayFromDoc(lineDoc) + GetHeight(lineDoc) - 1;
	if (OneToOne()) {
		return std::min(lineDoc, linesInDocument);
	}
	LINE lineDocCast = std::min(line_cast(lineDoc), displayLines->Partitions());
	lineDocCast = displayLines->PositionFromPartition(lineDocCast);
	lineDocCast += heights->ValueAt(line_cast(lineDoc));
	return lineDocCast - 1;
}

template <typename LINE>
Sci::Line ContractionState<LINE>::DocFromDisplay(Sci::Line lineDisplay) const noexcept {
	if (OneToOne()) {
		return lineDisplay;
	}
	if (lineDisplay < 0) {
		return 0;
	}
	const Sci::Line linesDisplayed = displayLines->PositionFromPartition(OneToMany_LinesInDoc());
	lineDisplay = std::min(lineDisplay, linesDisplayed);
	const Sci::Line lineDoc = displayLines->PartitionFromPosition(line_cast(lineDisplay));
	PLATFORM_ASSERT(GetVisible(lineDoc));
	return lineDoc;
}

template <typename LINE>
void ContractionState<LINE>::InsertLines(Sci::Line lineDoc, Sci::Line lineCount) {
	if (OneToOne()) {
		linesInDocument += lineCount;
	} else {
		//const ElapsedPeriod period;
#if ContractionState_InsertLines_OneByOne
		for (Sci::Line l = 0; l < lineCount; l++) {
			InsertLine(lineDoc + l);
		}
#else
		{
			const LINE lineDocCast = line_cast(lineDoc);
			const LINE insertLength = line_cast(lineCount);
			visible->InsertSpace(lineDocCast, insertLength);
			visible->FillRange(lineDocCast, 1, insertLength);
			expanded->InsertSpace(lineDocCast, insertLength);
			expanded->FillRange(lineDocCast, 1, insertLength);
			heights->InsertSpace(lineDocCast, insertLength);
			heights->FillRange(lineDocCast, 1, insertLength);
#if EnablePerLineFoldDisplayText
			foldDisplayTexts->InsertSpace(lineDocCast, insertLength);
#endif
		}
		for (Sci::Line l = 0; l < lineCount; l++) {
			const LINE lineDocCast = line_cast(lineDoc + l);
			const LINE lineDisplay = displayLines->PositionFromPartition(std::min(lineDocCast, displayLines->Partitions()));
			displayLines->InsertPartition(lineDocCast, lineDisplay);
			displayLines->InsertText(lineDocCast, 1);
#if EnablePerLineFoldDisplayText
			foldDisplayTexts->SetValueAt(lineDocCast, nullptr);
#endif
		}

#endif // ContractionState_InsertLines_OneByOne
		//const double duration = period.Duration()*1e3;
		//printf("%s(%td, %td) duration: %f\n", __func__, lineDoc, lineCount, duration);
	}
	Check();
}

template <typename LINE>
void ContractionState<LINE>::DeleteLines(Sci::Line lineDoc, Sci::Line lineCount) {
	if (OneToOne()) {
		linesInDocument -= lineCount;
	} else {
		//const ElapsedPeriod period;
#if ContractionState_DeleteLines_OneByOne
		for (Sci::Line l = 0; l < lineCount; l++) {
			DeleteLine(lineDoc);
		}
#else
		const LINE lineDocCast = line_cast(lineDoc);
		for (Sci::Line l = 0; l < lineCount; l++) {
			const LINE line = line_cast(lineDoc + l);
			if (visible->ValueAt(line)) {
				displayLines->InsertText(lineDocCast, -heights->ValueAt(line));
			}
			displayLines->RemovePartition(lineDocCast);
#if EnablePerLineFoldDisplayText
			foldDisplayTexts->DeletePosition(lineDocCast);
#endif
		}
		{
			const LINE deleteLength = line_cast(lineCount);
			visible->DeleteRange(lineDocCast, deleteLength);
			expanded->DeleteRange(lineDocCast, deleteLength);
			heights->DeleteRange(lineDocCast, deleteLength);
		}
#endif // ContractionState_DeleteLines_OneByOne
		//const double duration = period.Duration()*1e3;
		//printf("%s(%td, %td) duration: %f\n", __func__, lineDoc, lineCount, duration);
	}
	Check();
}

template <typename LINE>
bool ContractionState<LINE>::GetVisible(Sci::Line lineDoc) const noexcept {
	if (OneToOne()) {
		return true;
	}
	if (lineDoc >= visible->Length()) {
		return true;
	}
	return visible->ValueAt(line_cast(lineDoc)) & true;
}

template <typename LINE>
bool ContractionState<LINE>::SetVisible(Sci::Line lineDocStart, Sci::Line lineDocEnd, bool isVisible) {
	if (OneToOne() && isVisible) {
		return false;
	}
	EnsureData();
	Check();
	if (InRangeInclusive(lineDocStart, lineDocEnd) && (lineDocEnd < OneToMany_LinesInDoc())) {
		bool changed = false;
		for (Sci::Line lineDoc = lineDocStart; lineDoc <= lineDocEnd; lineDoc++) {
			const LINE line = line_cast(lineDoc);
			const char lineVisible = visible->ValueAt(line);
			if (lineVisible != static_cast<char>(isVisible)) {
				changed = true;
				const int heightLine = heights->ValueAt(line);
				const int difference = isVisible ? heightLine : -heightLine;
				displayLines->InsertText(line, difference);
			}
		}
		if (changed) {
			visible->FillRange(line_cast(lineDocStart), static_cast<char>(isVisible),
				line_cast(lineDocEnd - lineDocStart) + 1);
		}
		Check();
		return changed;
	}
	return false;
}

template <typename LINE>
bool ContractionState<LINE>::HiddenLines() const noexcept {
	if (OneToOne()) {
		return false;
	}
	return !visible->AllSameAs(1);
}

#if EnablePerLineFoldDisplayText
template <typename LINE>
const char *ContractionState<LINE>::GetFoldDisplayText(Sci::Line lineDoc) const noexcept {
	Check();
	const UniqueString empty{};
	return foldDisplayTexts->ValueOr(lineDoc, empty).get();
}

template <typename LINE>
bool ContractionState<LINE>::SetFoldDisplayText(Sci::Line lineDoc, const char *text) {
	EnsureData();
	const UniqueString empty{};
	const char *foldText = foldDisplayTexts->ValueOr(lineDoc, empty).get();
	if (!foldText || !text || 0 != strcmp(text, foldText)) {
		UniqueString uns = UniqueStringCopy(text);
		foldDisplayTexts->SetValueAt(lineDoc, std::move(uns));
		Check();
		return true;
	}
	Check();
	return false;
}
#endif

template <typename LINE>
bool ContractionState<LINE>::GetExpanded(Sci::Line lineDoc) const noexcept {
	if (OneToOne()) {
		return true;
	}
	Check();
	return expanded->ValueAt(line_cast(lineDoc)) & true;
}

template <typename LINE>
bool ContractionState<LINE>::SetExpanded(Sci::Line lineDoc, bool isExpanded) {
	if (OneToOne() && isExpanded) {
		return false;
	}
	EnsureData();
	if (static_cast<char>(isExpanded) != expanded->ValueAt(line_cast(lineDoc))) {
		expanded->SetValueAt(line_cast(lineDoc), static_cast<char>(isExpanded));
		Check();
		return true;
	}
	Check();
	return false;
}

template <typename LINE>
bool ContractionState<LINE>::ExpandAll() {
	if (OneToOne()) {
		return false;
	}
	const LINE lines = expanded->Length();
	const bool changed = expanded->FillRange(0, 1, lines).changed;
	Check();
	return changed;
}

template <typename LINE>
Sci::Line ContractionState<LINE>::ContractedNext(Sci::Line lineDocStart) const noexcept {
	if (OneToOne()) {
		return -1;
	}
	Check();
	if (!expanded->ValueAt(line_cast(lineDocStart))) {
		return lineDocStart;
	}
	const Sci::Line lineDocNextChange = expanded->EndRun(line_cast(lineDocStart));
	if (lineDocNextChange < OneToMany_LinesInDoc()) {
		return lineDocNextChange;
	}
	return -1;
}

template <typename LINE>
int ContractionState<LINE>::GetHeight(Sci::Line lineDoc) const noexcept {
	if (OneToOne()) {
		return 1;
	}
	return heights->ValueAt(line_cast(lineDoc));
}

// Set the number of display lines needed for this line.
// Return true if this is a change.
template <typename LINE>
bool ContractionState<LINE>::SetHeight(Sci::Line lineDoc, int height) {
	if (OneToOne() && (height == 1)) {
		return false;
	}
	if (lineDoc < LinesInDoc()) {
		EnsureData();
		const int h = heights->ValueAt(line_cast(lineDoc));
		if (h != height) {
			if (visible->ValueAt(line_cast(lineDoc))) {
				displayLines->InsertText(line_cast(lineDoc), height - h);
			}
			heights->SetValueAt(line_cast(lineDoc), height);
			Check();
			return true;
		}
		Check();
	}
	return false;
}

template <typename LINE>
void ContractionState<LINE>::ShowAll() noexcept {
	const Sci::Line lines = LinesInDoc();
	Clear();
	linesInDocument = lines;
}

// Debugging checks

#ifdef CHECK_CORRECTNESS
template <typename LINE>
void ContractionState<LINE>::Check() const noexcept {
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
}
#endif

}

namespace Scintilla::Internal {

std::unique_ptr<IContractionState> ContractionStateCreate(bool largeDocument) {
	if (largeDocument) {
		return std::make_unique<ContractionState<Sci::Line>>();
	}
	return std::make_unique<ContractionState<int>>();
}

}
