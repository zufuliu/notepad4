// Scintilla source code edit control
/** @file EditModel.cxx
 ** Defines the editor state that must be visible to EditorView.
 **/
// Copyright 1998-2014 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cmath>

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <map>
#include <optional>
#include <algorithm>
#include <memory>

#include "ParallelSupport.h"
#include "ScintillaTypes.h"
#include "ILoader.h"
#include "ILexer.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"

//#include "CharacterCategory.h"

#include "Position.h"
#include "UniqueString.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "ContractionState.h"
#include "CellBuffer.h"
#include "Indicator.h"
#include "LineMarker.h"
#include "Style.h"
#include "ViewStyle.h"
#include "CharClassify.h"
#include "Decoration.h"
#include "CaseFolder.h"
#include "Document.h"
#include "UniConversion.h"
#include "Selection.h"
#include "PositionCache.h"
#include "EditModel.h"

using namespace Scintilla;
using namespace Scintilla::Internal;

Caret::Caret() noexcept :
	active(false), on(false), period(500) {}

void ModelState::RememberSelectionForUndo(int index, const Selection &sel) {
	historyForUndo.indexCurrent = index;
	historyForUndo.ssCurrent = sel.ToString();
}

void ModelState::ForgetSelectionForUndo() noexcept {
	historyForUndo.indexCurrent = -1;
}

void ModelState::RememberSelectionOntoStack(int index, Sci::Line topLine) {
	if ((historyForUndo.indexCurrent >= 0) && (index == historyForUndo.indexCurrent + 1)) {
		// Don't overwrite initial selection save if most recent action was coalesced
		historyForUndo.stack[index] = { historyForUndo.ssCurrent, topLine };
	}
}

void ModelState::RememberSelectionForRedoOntoStack(int index, const Selection &sel, Sci::Line topLine) {
	historyForRedo.stack[index] = { sel.ToString(), topLine };
}

SelectionWithScroll ModelState::SelectionFromStack(int index, UndoRedo history) const {
	const SelectionHistory &sh = history == UndoRedo::undo ? historyForUndo : historyForRedo;
	const SelectionStack::const_iterator it = sh.stack.find(index);
	if (it != sh.stack.end()) {
		return it->second;
	}
	return {};
}

void ModelState::TruncateUndo(int index) {
	const SelectionStack::const_iterator itUndo = historyForUndo.stack.find(index);
	historyForUndo.stack.erase(itUndo, historyForUndo.stack.end());
	const SelectionStack::const_iterator itRedo = historyForRedo.stack.find(index);
	historyForRedo.stack.erase(itRedo, historyForRedo.stack.end());
}

EditModel::EditModel() :
	reprs{std::make_unique<SpecialRepresentations>()},
	pcs{ContractionStateCreate(false)},
	durationWrapOneUnit(0.01 / 64) {
	inOverstrike = false;
	trackLineWidth = false;
	hasFocus = false;
	primarySelection = true;
	xOffset = 0;
	posDrag = SelectionPosition(Sci::invalidPosition);
	braces[0] = Sci::invalidPosition;
	braces[1] = Sci::invalidPosition;
	bracesMatchStyle = StyleBraceBad;
	highlightGuideColumn = 0;
	imeInteraction = IMEInteraction::Windowed;
	bidirectional = Bidirectional::Disabled;
	foldFlags = FoldFlag::None;
	foldDisplayTextStyle = FoldDisplayTextStyle::Hidden;
	hotspot = Range(Sci::invalidPosition);
	hotspotSingleLine = true;
	hoverIndicatorPos = Sci::invalidPosition;
	wrapWidth = LineLayout::wrapWidthInfinite;
	// before setting a lexer, style buffer is useless.
	pdoc = new Document(DocumentOption::StylesNone);
	pdoc->AddRef();

	SYSTEM_INFO info;
	GetNativeSystemInfo(&info);
	hardwareConcurrency = info.dwNumberOfProcessors;
	idleTaskTimer = CreateWaitableTimer(nullptr, true, nullptr);
	SetIdleTaskTime(IdleLineWrapTime);
	UpdateParallelLayoutThreshold();
}

EditModel::~EditModel() {
	pdoc->SetViewState(this, {});
	pdoc->Release();
	pdoc = nullptr;
	CloseHandle(idleTaskTimer);
}

bool EditModel::BidirectionalEnabled() const noexcept {
	return (bidirectional != Bidirectional::Disabled) &&
		(CpUtf8 == pdoc->dbcsCodePage);
}

SurfaceMode EditModel::CurrentSurfaceMode() const noexcept {
	return { pdoc->dbcsCodePage/*, BidirectionalR2L()*/ };
}

void EditModel::SetDefaultFoldDisplayText(const char *text) {
	defaultFoldDisplayText = UniqueStringCopy(text);
}

const char *EditModel::GetDefaultFoldDisplayText() const noexcept {
	return defaultFoldDisplayText.get();
}

const char *EditModel::GetFoldDisplayText(Sci::Line lineDoc, bool partialLine) const noexcept {
	if (!partialLine && (foldDisplayTextStyle == FoldDisplayTextStyle::Hidden || pcs->GetExpanded(lineDoc))) {
		return nullptr;
	}

#if EnablePerLineFoldDisplayText
	const char *text = pcs->GetFoldDisplayText(lineDoc);
	return text ? text : defaultFoldDisplayText.get();
#else
	return defaultFoldDisplayText.get();
#endif
}

InSelection EditModel::LineEndInSelection(Sci::Line lineDoc) const noexcept {
	const Sci::Position posAfterLineEnd = pdoc->LineStart(lineDoc + 1);
	return sel.InSelectionForEOL(posAfterLineEnd);
}

MarkerMask EditModel::GetMark(Sci::Line line) const noexcept {
	return pdoc->GetMark(line, FlagSet(changeHistoryOption, ChangeHistoryOption::Markers));
}

void EditModel::EnsureModelState() {
	if (!modelState && (undoSelectionHistoryOption != UndoSelectionHistoryOption::Disabled)) {
		if (auto vss = pdoc->GetViewState(this)) {
#if USE_RTTI
			modelState = std::dynamic_pointer_cast<ModelState>(vss);
#else
			modelState = std::static_pointer_cast<ModelState>(vss);
#endif
		} else {
			modelState = std::make_shared<ModelState>();
			pdoc->SetViewState(this, std::static_pointer_cast<ViewState>(modelState));
		}
	}
}

void EditModel::ChangeUndoSelectionHistory(Scintilla::UndoSelectionHistoryOption undoSelectionHistoryOptionNew) noexcept {
	undoSelectionHistoryOption = undoSelectionHistoryOptionNew;
	if (undoSelectionHistoryOption == UndoSelectionHistoryOption::Disabled) {
		modelState.reset();
		pdoc->SetViewState(this, {});
	}
}

void EditModel::SetIdleTaskTime(uint32_t milliseconds) const noexcept {
	LARGE_INTEGER dueTime;
	dueTime.QuadPart = -INT64_C(10*1000)*milliseconds; // convert to 100ns
	SetWaitableTimer(idleTaskTimer, &dueTime, 0, nullptr, nullptr, false);
}

bool EditModel::IdleTaskTimeExpired() const noexcept {
	return WaitableTimerExpired(idleTaskTimer);
}

void EditModel::UpdateParallelLayoutThreshold() noexcept {
	const uint32_t idleLength = durationWrapOneUnit.ActionsInAllowedTime(0.2);
	minParallelLayoutLength = std::max(ParallelLayoutBlockSize, idleLength/64); // (1.5ms ~ 2ms)*2
	maxParallelLayoutLength = idleLength*hardwareConcurrency;
}
