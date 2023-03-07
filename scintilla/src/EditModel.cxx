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

EditModel::EditModel() : durationWrapOneUnit(0.01 / 64), durationWrapOneThread(0.01 / 16) {
	inOverstrike = false;
	trackLineWidth = false;
	xOffset = 0;
	posDrag = SelectionPosition(Sci::invalidPosition);
	braces[0] = Sci::invalidPosition;
	braces[1] = Sci::invalidPosition;
	bracesMatchStyle = StyleBraceBad;
	highlightGuideColumn = 0;
	hasFocus = false;
	primarySelection = true;
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
	pcs = ContractionStateCreate(pdoc->IsLarge());

	SYSTEM_INFO info;
	GetNativeSystemInfo(&info);
	hardwareConcurrency = info.dwNumberOfProcessors;
	idleTaskTimer = CreateWaitableTimer(nullptr, true, nullptr);
	UpdateParallelLayoutThreshold();
}

EditModel::~EditModel() {
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

void EditModel::SetIdleTaskTime(uint32_t milliseconds) const noexcept {
	LARGE_INTEGER dueTime;
	dueTime.QuadPart = -INT64_C(10*1000)*milliseconds; // convert to 100ns
	SetWaitableTimer(idleTaskTimer, &dueTime, 0, nullptr, nullptr, false);
}

bool EditModel::IdleTaskTimeExpired() const noexcept {
	return WaitableTimerExpired(idleTaskTimer);
}

void EditModel::UpdateParallelLayoutThreshold() noexcept {
	minParallelLayoutLength = durationWrapOneThread.ActionsInAllowedTime(0.01);
	const uint32_t idleLength = durationWrapOneUnit.ActionsInAllowedTime(0.2);
	maxParallelLayoutLength = std::max(minParallelLayoutLength*hardwareConcurrency, idleLength);
}
