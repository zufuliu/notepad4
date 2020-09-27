// Scintilla source code edit control
/** @file CellBuffer.cxx
 ** Manages a buffer of cells.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdarg>

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <memory>
#include <chrono>

#include "Platform.h"
#include "VectorISA.h"

#include "Scintilla.h"
#include "Position.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "CellBuffer.h"
#include "UniConversion.h"
#include "ElapsedPeriod.h"

namespace Scintilla {

struct CountWidths {
	// Measures the number of characters in a string divided into those
	// from the Base Multilingual Plane and those from other planes.
	Sci::Position countBasePlane;
	Sci::Position countOtherPlanes;
	CountWidths(Sci::Position countBasePlane_ = 0, Sci::Position countOtherPlanes_ = 0) noexcept :
		countBasePlane(countBasePlane_),
		countOtherPlanes(countOtherPlanes_) {
	}
	CountWidths operator-() const noexcept {
		return CountWidths(-countBasePlane, -countOtherPlanes);
	}
	Sci::Position WidthUTF32() const noexcept {
		// All code points take one code unit in UTF-32.
		return countBasePlane + countOtherPlanes;
	}
	Sci::Position WidthUTF16() const noexcept {
		// UTF-16 takes 2 code units for other planes
		return countBasePlane + 2 * countOtherPlanes;
	}
	void CountChar(int lenChar) noexcept {
		if (lenChar == 4) {
			countOtherPlanes++;
		} else {
			countBasePlane++;
		}
	}
};

class ILineVector {
public:
	virtual void Init() = 0;
	virtual void SetPerLine(PerLine *pl) noexcept = 0;
	virtual void InsertText(Sci::Line line, Sci::Position delta) noexcept = 0;
	virtual void InsertLine(Sci::Line line, Sci::Position position, bool lineStart) = 0;
	virtual void InsertLines(Sci::Line line, const Sci::Position *positions, size_t lines, bool lineStart) = 0;
	virtual void SetLineStart(Sci::Line line, Sci::Position position) noexcept = 0;
	virtual void RemoveLine(Sci::Line line) = 0;
	virtual Sci::Line Lines() const noexcept = 0;
	virtual void AllocateLines(Sci::Line lines) = 0;
	virtual Sci::Line LineFromPosition(Sci::Position pos) const noexcept = 0;
	virtual Sci::Position LineStart(Sci::Line line) const noexcept = 0;
	virtual void InsertCharacters(Sci::Line line, CountWidths delta) noexcept = 0;
	virtual void SetLineCharactersWidth(Sci::Line line, CountWidths width) noexcept = 0;
	virtual int LineCharacterIndex() const noexcept = 0;
	virtual bool AllocateLineCharacterIndex(int lineCharacterIndex, Sci::Line lines) = 0;
	virtual bool ReleaseLineCharacterIndex(int lineCharacterIndex) = 0;
	virtual Sci::Position IndexLineStart(Sci::Line line, int lineCharacterIndex) const noexcept = 0;
	virtual Sci::Line LineFromPositionIndex(Sci::Position pos, int lineCharacterIndex) const noexcept = 0;
	virtual ~ILineVector() = default;
};

}

using namespace Scintilla;

template <typename POS>
class LineStartIndex {
public:
	int refCount;
	Partitioning<POS> starts;

	LineStartIndex() : refCount(0), starts(4) {
		// Minimal initial allocation
	}
	// Deleted so LineStartIndex objects can not be copied.
	LineStartIndex(const LineStartIndex &) = delete;
	LineStartIndex(LineStartIndex &&) = delete;
	void operator=(const LineStartIndex &) = delete;
	void operator=(LineStartIndex &&) = delete;
	virtual ~LineStartIndex() = default;
	bool Allocate(Sci::Line lines) {
		refCount++;
		Sci::Position length = starts.Length();
		for (Sci::Line line = starts.Partitions(); line < lines; line++) {
			// Produce an ascending sequence that will be filled in with correct widths later
			length++;
			starts.InsertPartition(static_cast<POS>(line), static_cast<POS>(length));
		}
		return refCount == 1;
	}
	bool Release() {
		if (refCount == 1) {
			starts.DeleteAll();
		}
		refCount--;
		return refCount == 0;
	}
	bool Active() const noexcept {
		return refCount > 0;
	}
	Sci::Position LineWidth(Sci::Line line) const noexcept {
		return starts.PositionFromPartition(static_cast<POS>(line) + 1) -
			starts.PositionFromPartition(static_cast<POS>(line));
	}
	void SetLineWidth(Sci::Line line, Sci::Position width) noexcept {
		const Sci::Position widthCurrent = LineWidth(line);
		starts.InsertText(static_cast<POS>(line), static_cast<POS>(width - widthCurrent));
	}
	void AllocateLines(Sci::Line lines) {
		if (lines > starts.Partitions()) {
			starts.ReAllocate(lines);
		}
	}
	void InsertLines(Sci::Line line, Sci::Line lines) {
		// Insert multiple lines with each temporarily 1 character wide.
		// The line widths will be fixed up by later measuring code.
		const POS lineAsPos = static_cast<POS>(line);
		const POS lineStart = starts.PositionFromPartition(lineAsPos - 1) + 1;
		for (POS l = 0; l < static_cast<POS>(lines); l++) {
			starts.InsertPartition(lineAsPos + l, lineStart + l);
		}
	}
};

template <typename POS>
class LineVector : public ILineVector {
	Partitioning<POS> starts;
	PerLine *perLine;
	LineStartIndex<POS> startsUTF16;
	LineStartIndex<POS> startsUTF32;
	int activeIndices;

	void SetActiveIndices() noexcept {
		activeIndices = (startsUTF32.Active() ? SC_LINECHARACTERINDEX_UTF32 : 0)
			| (startsUTF16.Active() ? SC_LINECHARACTERINDEX_UTF16 : 0);
	}

public:
	LineVector() : starts(256), perLine(nullptr), activeIndices(0) {
	}
	// Deleted so LineVector objects can not be copied.
	LineVector(const LineVector &) = delete;
	LineVector(LineVector &&) = delete;
	LineVector &operator=(const LineVector &) = delete;
	LineVector &operator=(LineVector &&) = delete;
	~LineVector() override = default;
	void Init() override {
		starts.DeleteAll();
		if (perLine) {
			perLine->Init();
		}
		startsUTF32.starts.DeleteAll();
		startsUTF16.starts.DeleteAll();
	}
	void SetPerLine(PerLine *pl) noexcept override {
		perLine = pl;
	}
	void InsertText(Sci::Line line, Sci::Position delta) noexcept override {
		starts.InsertText(static_cast<POS>(line), static_cast<POS>(delta));
	}
	void InsertLine(Sci::Line line, Sci::Position position, bool lineStart) override {
		const POS lineAsPos = static_cast<POS>(line);
		starts.InsertPartition(lineAsPos, static_cast<POS>(position));
		if (activeIndices) {
			if (activeIndices & SC_LINECHARACTERINDEX_UTF32) {
				startsUTF32.InsertLines(line, 1);
			}
			if (activeIndices & SC_LINECHARACTERINDEX_UTF16) {
				startsUTF16.InsertLines(line, 1);
			}
		}
		if (perLine) {
			if ((line > 0) && lineStart) {
				line--;
			}
			perLine->InsertLine(line);
		}
	}
	void InsertLines(Sci::Line line, const Sci::Position *positions, size_t lines, bool lineStart) override {
		const POS lineAsPos = static_cast<POS>(line);
		if constexpr (sizeof(Sci::Position) == sizeof(POS)) {
			starts.InsertPartitions(lineAsPos, positions, lines);
		} else {
			starts.InsertPartitionsWithCast(lineAsPos, positions, lines);
		}
		if (activeIndices) {
			if (activeIndices & SC_LINECHARACTERINDEX_UTF32) {
				startsUTF32.InsertLines(line, lines);
			}
			if (activeIndices & SC_LINECHARACTERINDEX_UTF16) {
				startsUTF16.InsertLines(line, lines);
			}
		}
		if (perLine) {
			if ((line > 0) && lineStart) {
				line--;
			}
			perLine->InsertLines(line, lines);
		}
	}
	void SetLineStart(Sci::Line line, Sci::Position position) noexcept override {
		starts.SetPartitionStartPosition(static_cast<POS>(line), static_cast<POS>(position));
	}
	void RemoveLine(Sci::Line line) override {
		starts.RemovePartition(static_cast<POS>(line));
		if (activeIndices & SC_LINECHARACTERINDEX_UTF32) {
			startsUTF32.starts.RemovePartition(static_cast<POS>(line));
		}
		if (activeIndices & SC_LINECHARACTERINDEX_UTF16) {
			startsUTF16.starts.RemovePartition(static_cast<POS>(line));
		}
		if (perLine) {
			perLine->RemoveLine(line);
		}
	}
	Sci::Line Lines() const noexcept override {
		return static_cast<Sci::Line>(starts.Partitions());
	}
	void AllocateLines(Sci::Line lines) override {
		if (lines > Lines()) {
			starts.ReAllocate(lines);
			if (activeIndices & SC_LINECHARACTERINDEX_UTF32) {
				startsUTF32.AllocateLines(lines);
			}
			if (activeIndices & SC_LINECHARACTERINDEX_UTF16) {
				startsUTF16.AllocateLines(lines);
			}
		}
	}
	Sci::Line LineFromPosition(Sci::Position pos) const noexcept override {
		return static_cast<Sci::Line>(starts.PartitionFromPosition(static_cast<POS>(pos)));
	}
	Sci::Position LineStart(Sci::Line line) const noexcept override {
		return starts.PositionFromPartition(static_cast<POS>(line));
	}
	void InsertCharacters(Sci::Line line, CountWidths delta) noexcept override {
		if (activeIndices & SC_LINECHARACTERINDEX_UTF32) {
			startsUTF32.starts.InsertText(static_cast<POS>(line), static_cast<POS>(delta.WidthUTF32()));
		}
		if (activeIndices & SC_LINECHARACTERINDEX_UTF16) {
			startsUTF16.starts.InsertText(static_cast<POS>(line), static_cast<POS>(delta.WidthUTF16()));
		}
	}
	void SetLineCharactersWidth(Sci::Line line, CountWidths width) noexcept override {
		if (activeIndices & SC_LINECHARACTERINDEX_UTF32) {
			assert(startsUTF32.starts.Partitions() == starts.Partitions());
			startsUTF32.SetLineWidth(line, width.WidthUTF32());
		}
		if (activeIndices & SC_LINECHARACTERINDEX_UTF16) {
			assert(startsUTF16.starts.Partitions() == starts.Partitions());
			startsUTF16.SetLineWidth(line, width.WidthUTF16());
		}
	}

	int LineCharacterIndex() const noexcept override {
		return activeIndices;
	}
	bool AllocateLineCharacterIndex(int lineCharacterIndex, Sci::Line lines) override {
		const int activeIndicesStart = activeIndices;
		if ((lineCharacterIndex & SC_LINECHARACTERINDEX_UTF32) != 0) {
			startsUTF32.Allocate(lines);
			assert(startsUTF32.starts.Partitions() == starts.Partitions());
		}
		if ((lineCharacterIndex & SC_LINECHARACTERINDEX_UTF16) != 0) {
			startsUTF16.Allocate(lines);
			assert(startsUTF16.starts.Partitions() == starts.Partitions());
		}
		SetActiveIndices();
		return activeIndicesStart != activeIndices;
	}
	bool ReleaseLineCharacterIndex(int lineCharacterIndex) override {
		const int activeIndicesStart = activeIndices;
		if ((lineCharacterIndex & SC_LINECHARACTERINDEX_UTF32) != 0) {
			startsUTF32.Release();
		}
		if ((lineCharacterIndex & SC_LINECHARACTERINDEX_UTF16) != 0) {
			startsUTF16.Release();
		}
		SetActiveIndices();
		return activeIndicesStart != activeIndices;
	}
	Sci::Position IndexLineStart(Sci::Line line, int lineCharacterIndex) const noexcept override {
		if (lineCharacterIndex == SC_LINECHARACTERINDEX_UTF32) {
			return startsUTF32.starts.PositionFromPartition(static_cast<POS>(line));
		} else {
			return startsUTF16.starts.PositionFromPartition(static_cast<POS>(line));
		}
	}
	Sci::Line LineFromPositionIndex(Sci::Position pos, int lineCharacterIndex) const noexcept override {
		if (lineCharacterIndex == SC_LINECHARACTERINDEX_UTF32) {
			return static_cast<Sci::Line>(startsUTF32.starts.PartitionFromPosition(static_cast<POS>(pos)));
		} else {
			return static_cast<Sci::Line>(startsUTF16.starts.PartitionFromPosition(static_cast<POS>(pos)));
		}
	}
};

Action::Action() noexcept {
	at = startAction;
	position = 0;
	lenData = 0;
	mayCoalesce = false;
}

Action::~Action() = default;

void Action::Create(actionType at_, Sci::Position position_, const char *data_, Sci::Position lenData_, bool mayCoalesce_) {
	data = nullptr;
	position = position_;
	at = at_;
	if (lenData_) {
		data = std::make_unique<char[]>(lenData_);
		memcpy(data.get(), data_, lenData_);
	}
	lenData = lenData_;
	mayCoalesce = mayCoalesce_;
}

void Action::Clear() noexcept {
	data = nullptr;
	lenData = 0;
}

// The undo history stores a sequence of user operations that represent the user's view of the
// commands executed on the text.
// Each user operation contains a sequence of text insertion and text deletion actions.
// All the user operations are stored in a list of individual actions with 'start' actions used
// as delimiters between user operations.
// Initially there is one start action in the history.
// As each action is performed, it is recorded in the history. The action may either become
// part of the current user operation or may start a new user operation. If it is to be part of the
// current operation, then it overwrites the current last action. If it is to be part of a new
// operation, it is appended after the current last action.
// After writing the new action, a new start action is appended at the end of the history.
// The decision of whether to start a new user operation is based upon two factors. If a
// compound operation has been explicitly started by calling BeginUndoAction and no matching
// EndUndoAction (these calls nest) has been called, then the action is coalesced into the current
// operation. If there is no outstanding BeginUndoAction call then a new operation is started
// unless it looks as if the new action is caused by the user typing or deleting a stream of text.
// Sequences that look like typing or deletion are coalesced into a single user operation.

UndoHistory::UndoHistory() {

	actions.resize(3);
	maxAction = 0;
	currentAction = 0;
	undoSequenceDepth = 0;
	savePoint = 0;
	tentativePoint = -1;

	actions[currentAction].Create(startAction);
}

UndoHistory::~UndoHistory() = default;

void UndoHistory::EnsureUndoRoom() {
	// Have to test that there is room for 2 more actions in the array
	// as two actions may be created by the calling function
	if (static_cast<size_t>(currentAction) >= (actions.size() - 2)) {
		// Run out of undo nodes so extend the array
		actions.resize(actions.size() * 2);
	}
}

const char *UndoHistory::AppendAction(actionType at, Sci::Position position, const char *data, Sci::Position lengthData,
	bool &startSequence, bool mayCoalesce) {
	EnsureUndoRoom();
	//Platform::DebugPrintf("%% %d action %d %d %d\n", at, position, lengthData, currentAction);
	//Platform::DebugPrintf("^ %d action %d %d\n", actions[currentAction - 1].at,
	//	actions[currentAction - 1].position, actions[currentAction - 1].lenData);
	if (currentAction < savePoint) {
		savePoint = -1;
	}
	int oldCurrentAction = currentAction;
	if (currentAction >= 1) {
		if (0 == undoSequenceDepth) {
			// Top level actions may not always be coalesced
			int targetAct = -1;
			const Action *actPrevious = &(actions[currentAction + targetAct]);
			// Container actions may forward the coalesce state of Scintilla Actions.
			while ((actPrevious->at == containerAction) && actPrevious->mayCoalesce) {
				targetAct--;
				actPrevious = &(actions[currentAction + targetAct]);
			}
			// See if current action can be coalesced into previous action
			// Will work if both are inserts or deletes and position is same
			if ((currentAction == savePoint) || (currentAction == tentativePoint)) {
				currentAction++;
			} else if (!actions[currentAction].mayCoalesce) {
				// Not allowed to coalesce if this set
				currentAction++;
			} else if (!mayCoalesce || !actPrevious->mayCoalesce) {
				currentAction++;
			} else if (at == containerAction || actions[currentAction].at == containerAction) {
				;	// A coalescible containerAction
			} else if ((at != actPrevious->at) && (actPrevious->at != startAction)) {
				currentAction++;
			} else if ((at == insertAction) &&
				(position != (actPrevious->position + actPrevious->lenData))) {
				// Insertions must be immediately after to coalesce
				currentAction++;
			} else if (at == removeAction) {
				if ((lengthData == 1) || (lengthData == 2)) {
					if ((position + lengthData) == actPrevious->position) {
						; // Backspace -> OK
					} else if (position == actPrevious->position) {
						; // Delete -> OK
					} else {
						// Removals must be at same position to coalesce
						currentAction++;
					}
				} else {
					// Removals must be of one character to coalesce
					currentAction++;
				}
			} else {
				// Action coalesced.
			}

		} else {
			// Actions not at top level are always coalesced unless this is after return to top level
			if (!actions[currentAction].mayCoalesce)
				currentAction++;
		}
	} else {
		currentAction++;
	}
	startSequence = oldCurrentAction != currentAction;
	const int actionWithData = currentAction;
	actions[currentAction].Create(at, position, data, lengthData, mayCoalesce);
	currentAction++;
	actions[currentAction].Create(startAction);
	maxAction = currentAction;
	return actions[actionWithData].data.get();
}

void UndoHistory::BeginUndoAction() {
	EnsureUndoRoom();
	if (undoSequenceDepth == 0) {
		if (actions[currentAction].at != startAction) {
			currentAction++;
			actions[currentAction].Create(startAction);
			maxAction = currentAction;
		}
		actions[currentAction].mayCoalesce = false;
	}
	undoSequenceDepth++;
}

void UndoHistory::EndUndoAction() {
	PLATFORM_ASSERT(undoSequenceDepth > 0);
	EnsureUndoRoom();
	undoSequenceDepth--;
	if (0 == undoSequenceDepth) {
		if (actions[currentAction].at != startAction) {
			currentAction++;
			actions[currentAction].Create(startAction);
			maxAction = currentAction;
		}
		actions[currentAction].mayCoalesce = false;
	}
}

void UndoHistory::DropUndoSequence() noexcept {
	undoSequenceDepth = 0;
}

void UndoHistory::DeleteUndoHistory() {
	for (int i = 1; i < maxAction; i++) {
		actions[i].Clear();
	}
	maxAction = 0;
	currentAction = 0;
	actions[currentAction].Create(startAction);
	savePoint = 0;
	tentativePoint = -1;
}

void UndoHistory::SetSavePoint() noexcept {
	savePoint = currentAction;
}

bool UndoHistory::IsSavePoint() const noexcept {
	return savePoint == currentAction;
}

void UndoHistory::TentativeStart() noexcept {
	tentativePoint = currentAction;
}

void UndoHistory::TentativeCommit() noexcept {
	tentativePoint = -1;
	// Truncate undo history
	maxAction = currentAction;
}

bool UndoHistory::TentativeActive() const noexcept {
	return tentativePoint >= 0;
}

int UndoHistory::TentativeSteps() noexcept {
	// Drop any trailing startAction
	if (actions[currentAction].at == startAction && currentAction > 0)
		currentAction--;
	if (tentativePoint >= 0)
		return currentAction - tentativePoint;
	else
		return -1;
}

bool UndoHistory::CanUndo() const noexcept {
	return (currentAction > 0) && (maxAction > 0);
}

int UndoHistory::StartUndo() noexcept {
	// Drop any trailing startAction
	if (actions[currentAction].at == startAction && currentAction > 0)
		currentAction--;

	// Count the steps in this action
	int act = currentAction;
	while (actions[act].at != startAction && act > 0) {
		act--;
	}
	return currentAction - act;
}

const Action &UndoHistory::GetUndoStep() const noexcept {
	return actions[currentAction];
}

void UndoHistory::CompletedUndoStep() noexcept {
	currentAction--;
}

bool UndoHistory::CanRedo() const noexcept {
	return maxAction > currentAction;
}

int UndoHistory::StartRedo() noexcept {
	// Drop any leading startAction
	if (currentAction < maxAction && actions[currentAction].at == startAction)
		currentAction++;

	// Count the steps in this action
	int act = currentAction;
	while (act < maxAction && actions[act].at != startAction) {
		act++;
	}
	return act - currentAction;
}

const Action &UndoHistory::GetRedoStep() const noexcept {
	return actions[currentAction];
}

void UndoHistory::CompletedRedoStep() noexcept {
	currentAction++;
}

CellBuffer::CellBuffer(bool hasStyles_, bool largeDocument_) :
	hasStyles(hasStyles_), largeDocument(largeDocument_) {
	readOnly = false;
	utf8Substance = false;
	utf8LineEnds = 0;
	collectingUndo = true;
	if (largeDocument)
		plv = std::make_unique<LineVector<Sci::Position>>();
	else
		plv = std::make_unique<LineVector<int>>();
}

CellBuffer::~CellBuffer() = default;

char CellBuffer::CharAt(Sci::Position position) const noexcept {
	return substance.ValueAt(position);
}

unsigned char CellBuffer::UCharAt(Sci::Position position) const noexcept {
	return substance.ValueAt(position);
}

void CellBuffer::GetCharRange(char *buffer, Sci::Position position, Sci::Position lengthRetrieve) const noexcept {
	if (lengthRetrieve <= 0)
		return;
	if (position < 0)
		return;
	if ((position + lengthRetrieve) > substance.Length()) {
		//Platform::DebugPrintf("Bad GetCharRange %.0f for %.0f of %.0f\n",
		//					static_cast<double>(position),
		//					static_cast<double>(lengthRetrieve),
		//					static_cast<double>(substance.Length()));
		return;
	}
	substance.GetRange(buffer, position, lengthRetrieve);
}

char CellBuffer::StyleAt(Sci::Position position) const noexcept {
	return hasStyles ? style.ValueAt(position) : '\0';
}

void CellBuffer::GetStyleRange(unsigned char *buffer, Sci::Position position, Sci::Position lengthRetrieve) const noexcept {
	if (lengthRetrieve < 0)
		return;
	if (position < 0)
		return;
	if (!hasStyles) {
		std::fill_n(buffer, lengthRetrieve, static_cast<unsigned char>(0));
		return;
	}
	if ((position + lengthRetrieve) > style.Length()) {
		//Platform::DebugPrintf("Bad GetStyleRange %.0f for %.0f of %.0f\n",
		//					static_cast<double>(position),
		//					static_cast<double>(lengthRetrieve),
		//					static_cast<double>(style.Length()));
		return;
	}
	style.GetRange(reinterpret_cast<char *>(buffer), position, lengthRetrieve);
}

const char *CellBuffer::BufferPointer() {
	return substance.BufferPointer();
}

const char *CellBuffer::RangePointer(Sci::Position position, Sci::Position rangeLength) noexcept {
	return substance.RangePointer(position, rangeLength);
}

const char *CellBuffer::StyleRangePointer(Sci::Position position, Sci::Position rangeLength) noexcept {
	return hasStyles ? style.RangePointer(position, rangeLength) : nullptr;
}

Sci::Position CellBuffer::GapPosition() const noexcept {
	return substance.GapPosition();
}

// The char* returned is to an allocation owned by the undo history
const char *CellBuffer::InsertString(Sci::Position position, const char *s, Sci::Position insertLength, bool &startSequence) {
	// InsertString and DeleteChars are the bottleneck though which all changes occur
	const char *data = s;
	if (!readOnly) {
		if (collectingUndo) {
			// Save into the undo/redo stack, but only the characters - not the formatting
			// This takes up about half load time
			data = uh.AppendAction(insertAction, position, s, insertLength, startSequence);
		}

		BasicInsertString(position, s, insertLength);
	}
	return data;
}

bool CellBuffer::SetStyleAt(Sci::Position position, char styleValue) noexcept {
	if (!hasStyles) {
		return false;
	}
	const char curVal = style.ValueAt(position);
	if (curVal != styleValue) {
		style.SetValueAt(position, styleValue);
		return true;
	} else {
		return false;
	}
}

bool CellBuffer::SetStyleFor(Sci::Position position, Sci::Position lengthStyle, char styleValue) noexcept {
	if (!hasStyles) {
		return false;
	}
	bool changed = false;
	PLATFORM_ASSERT(lengthStyle == 0 ||
		(lengthStyle > 0 && lengthStyle + position <= style.Length()));
	while (lengthStyle--) {
		const char curVal = style.ValueAt(position);
		if (curVal != styleValue) {
			style.SetValueAt(position, styleValue);
			changed = true;
		}
		position++;
	}
	return changed;
}

// The char* returned is to an allocation owned by the undo history
const char *CellBuffer::DeleteChars(Sci::Position position, Sci::Position deleteLength, bool &startSequence) {
	// InsertString and DeleteChars are the bottleneck though which all changes occur
	PLATFORM_ASSERT(deleteLength > 0);
	const char *data = nullptr;
	if (!readOnly) {
		if (collectingUndo) {
			// Save into the undo/redo stack, but only the characters - not the formatting
			// The gap would be moved to position anyway for the deletion so this doesn't cost extra
			data = substance.RangePointer(position, deleteLength);
			data = uh.AppendAction(removeAction, position, data, deleteLength, startSequence);
		}

		BasicDeleteChars(position, deleteLength);
	}
	return data;
}

Sci::Position CellBuffer::Length() const noexcept {
	return substance.Length();
}

void CellBuffer::Allocate(Sci::Position newSize) {
	substance.ReAllocate(newSize);
	if (hasStyles) {
		style.ReAllocate(newSize);
	}
}

bool CellBuffer::EnsureStyleBuffer(bool hasStyles_) {
	if (hasStyles != hasStyles_) {
		hasStyles = hasStyles_;
		if (hasStyles_) {
			style.InsertValue(0, substance.Length(), 0);
		} else {
			style.DeleteAll();
		}
		return true;
	}
	return false;
}

void CellBuffer::SetUTF8Substance(bool utf8Substance_) noexcept {
	utf8Substance = utf8Substance_;
}

void CellBuffer::SetLineEndTypes(int utf8LineEnds_) {
	if (utf8LineEnds != utf8LineEnds_) {
		const int indexes = plv->LineCharacterIndex();
		utf8LineEnds = utf8LineEnds_;
		ResetLineEnds();
		AllocateLineCharacterIndex(indexes);
	}
}

bool CellBuffer::ContainsLineEnd(const char *s, Sci::Position length) const noexcept {
	unsigned char chBeforePrev = 0;
	unsigned char chPrev = 0;
	for (Sci::Position i = 0; i < length; i++) {
		const unsigned char ch = s[i];
		if ((ch == '\r') || (ch == '\n')) {
			return true;
		}
		if (utf8LineEnds && !UTF8IsAscii(ch)) {
			if (UTF8IsMultibyteLineEnd(chBeforePrev, chPrev, ch)) {
				return true;
			}
		}
		chBeforePrev = chPrev;
		chPrev = ch;
	}
	return false;
}

void CellBuffer::SetPerLine(PerLine *pl) noexcept {
	plv->SetPerLine(pl);
}

int CellBuffer::LineCharacterIndex() const noexcept {
	return plv->LineCharacterIndex();
}

void CellBuffer::AllocateLineCharacterIndex(int lineCharacterIndex) {
	if (utf8Substance) {
		if (plv->AllocateLineCharacterIndex(lineCharacterIndex, Lines())) {
			// Changed so recalculate whole file
			RecalculateIndexLineStarts(0, Lines() - 1);
		}
	}
}

void CellBuffer::ReleaseLineCharacterIndex(int lineCharacterIndex) {
	plv->ReleaseLineCharacterIndex(lineCharacterIndex);
}

Sci::Line CellBuffer::Lines() const noexcept {
	return plv->Lines();
}

void CellBuffer::AllocateLines(Sci::Line lines) {
	plv->AllocateLines(lines);
}

Sci::Position CellBuffer::LineStart(Sci::Line line) const noexcept {
	if (line < 0)
		return 0;
	else if (line >= Lines())
		return Length();
	else
		return plv->LineStart(line);
}

Sci::Line CellBuffer::LineFromPosition(Sci::Position pos) const noexcept {
	return plv->LineFromPosition(pos);
}

Sci::Position CellBuffer::IndexLineStart(Sci::Line line, int lineCharacterIndex) const noexcept {
	return plv->IndexLineStart(line, lineCharacterIndex);
}

Sci::Line CellBuffer::LineFromPositionIndex(Sci::Position pos, int lineCharacterIndex) const noexcept {
	return plv->LineFromPositionIndex(pos, lineCharacterIndex);
}

bool CellBuffer::IsReadOnly() const noexcept {
	return readOnly;
}

void CellBuffer::SetReadOnly(bool set) noexcept {
	readOnly = set;
}

bool CellBuffer::IsLarge() const noexcept {
	return largeDocument;
}

bool CellBuffer::HasStyles() const noexcept {
	return hasStyles;
}

void CellBuffer::SetSavePoint() noexcept {
	uh.SetSavePoint();
}

bool CellBuffer::IsSavePoint() const noexcept {
	return uh.IsSavePoint();
}

void CellBuffer::TentativeStart() noexcept {
	uh.TentativeStart();
}

void CellBuffer::TentativeCommit() noexcept {
	uh.TentativeCommit();
}

int CellBuffer::TentativeSteps() noexcept {
	return uh.TentativeSteps();
}

bool CellBuffer::TentativeActive() const noexcept {
	return uh.TentativeActive();
}

// Without undo

void CellBuffer::InsertLine(Sci::Line line, Sci::Position position, bool lineStart) {
	plv->InsertLine(line, position, lineStart);
}

void CellBuffer::RemoveLine(Sci::Line line) {
	plv->RemoveLine(line);
}

bool CellBuffer::UTF8LineEndOverlaps(Sci::Position position) const noexcept {
	const unsigned char bytes[] = {
		static_cast<unsigned char>(substance.ValueAt(position - 2)),
		static_cast<unsigned char>(substance.ValueAt(position - 1)),
		static_cast<unsigned char>(substance.ValueAt(position)),
		static_cast<unsigned char>(substance.ValueAt(position + 1)),
	};
	return UTF8IsSeparator(bytes) || UTF8IsSeparator(bytes + 1) || UTF8IsNEL(bytes + 1);
}

bool CellBuffer::UTF8IsCharacterBoundary(Sci::Position position) const {
	assert(position >= 0 && position <= Length());
	if (position > 0) {
		std::string back;
		for (int i = 0; i < UTF8MaxBytes; i++) {
			const Sci::Position posBack = position - i;
			if (posBack < 0) {
				return false;
			}
			back.insert(0, 1, substance.ValueAt(posBack));
			if (!UTF8IsTrailByte(back.front())) {
				if (i > 0) {
					// Have reached a non-trail
					const int cla = UTF8Classify(back);
					if ((cla & UTF8MaskInvalid) || (cla != i)) {
						return false;
					}
				}
				break;
			}
		}
	}
	if (position < Length()) {
		const unsigned char fore = substance.ValueAt(position);
		if (UTF8IsTrailByte(fore)) {
			return false;
		}
	}
	return true;
}

void CellBuffer::ResetLineEnds() {
	// Reinitialize line data -- too much work to preserve
	const Sci::Line lines = plv->Lines();
	plv->Init();
	plv->AllocateLines(lines);

	constexpr Sci::Position position = 0;
	const Sci::Position length = Length();
	Sci::Line lineInsert = 1;
	constexpr bool atLineStart = true;
	plv->InsertText(lineInsert - 1, length);
	unsigned char chBeforePrev = 0;
	unsigned char chPrev = 0;
	for (Sci::Position i = 0; i < length; i++) {
		const unsigned char ch = substance.ValueAt(position + i);
		if (ch == '\r') {
			InsertLine(lineInsert, (position + i) + 1, atLineStart);
			lineInsert++;
		} else if (ch == '\n') {
			if (chPrev == '\r') {
				// Patch up what was end of line
				plv->SetLineStart(lineInsert - 1, (position + i) + 1);
			} else {
				InsertLine(lineInsert, (position + i) + 1, atLineStart);
				lineInsert++;
			}
		} else if (utf8LineEnds && !UTF8IsAscii(ch)) {
			if (UTF8IsMultibyteLineEnd(chBeforePrev, chPrev, ch)) {
				InsertLine(lineInsert, (position + i) + 1, atLineStart);
				lineInsert++;
			}
		}
		chBeforePrev = chPrev;
		chPrev = ch;
	}
}

namespace {

CountWidths CountCharacterWidthsUTF8(std::string_view sv) noexcept {
	CountWidths cw;
	size_t remaining = sv.length();
	while (remaining > 0) {
		const int utf8Status = UTF8Classify(sv);
		const int lenChar = utf8Status & UTF8MaskWidth;
		cw.CountChar(lenChar);
		sv.remove_prefix(lenChar);
		remaining -= lenChar;
	}
	return cw;
}

}

bool CellBuffer::MaintainingLineCharacterIndex() const noexcept {
	return plv->LineCharacterIndex() != SC_LINECHARACTERINDEX_NONE;
}

void CellBuffer::RecalculateIndexLineStarts(Sci::Line lineFirst, Sci::Line lineLast) {
	std::string text;
	Sci::Position posLineEnd = LineStart(lineFirst);
	for (Sci::Line line = lineFirst; line <= lineLast; line++) {
		// Find line start and end, retrieve text of line, count characters and update line width
		const Sci::Position posLineStart = posLineEnd;
		posLineEnd = LineStart(line + 1);
		const Sci::Position width = posLineEnd - posLineStart;
		text.resize(width);
		GetCharRange(text.data(), posLineStart, width);
		const CountWidths cw = CountCharacterWidthsUTF8(text);
		plv->SetLineCharactersWidth(line, cw);
	}
}

void CellBuffer::BasicInsertString(const Sci::Position position, const char * const s, const Sci::Position insertLength) {
	if (insertLength == 0)
		return;
	PLATFORM_ASSERT(insertLength > 0);

	const unsigned char chAfter = substance.ValueAt(position);
	bool breakingUTF8LineEnd = false;
	if (utf8LineEnds && UTF8IsTrailByte(chAfter)) {
		breakingUTF8LineEnd = UTF8LineEndOverlaps(position);
	}

	const Sci::Line linePosition = plv->LineFromPosition(position);
	Sci::Line lineInsert = linePosition + 1;

	// A simple insertion is one that inserts valid text on a single line at a character boundary
	bool simpleInsertion = false;

	const bool maintainingIndex = MaintainingLineCharacterIndex();

	// Check for breaking apart a UTF-8 sequence and inserting invalid UTF-8
	if (utf8Substance && maintainingIndex) {
		// Actually, don't need to check that whole insertion is valid just that there
		// are no potential fragments at ends.
		simpleInsertion = UTF8IsCharacterBoundary(position) &&
			UTF8IsValid(std::string_view(s, insertLength));
	}

	substance.InsertFromArray(position, s, 0, insertLength);
	if (hasStyles) {
		style.InsertValue(position, insertLength, 0);
	}

	const bool atLineStart = plv->LineStart(lineInsert - 1) == position;
	// Point all the lines after the insertion point further along in the buffer
	plv->InsertText(lineInsert - 1, insertLength);
	unsigned char chBeforePrev = substance.ValueAt(position - 2);
	unsigned char chPrev = substance.ValueAt(position - 1);
	if (chPrev == '\r' && chAfter == '\n') {
		// Splitting up a crlf pair at position
		InsertLine(lineInsert, position, false);
		lineInsert++;
	}
	if (breakingUTF8LineEnd) {
		RemoveLine(lineInsert);
	}

#if defined(_WIN64)
	constexpr size_t PositionBlockSize = 256;
#else
	constexpr size_t PositionBlockSize = 128;
#endif

	//ElapsedPeriod period;
	Sci::Position positions[PositionBlockSize];
	size_t nPositions = 0;
	const Sci::Line lineStart = lineInsert;

	// s may not NULL-terminated, ensure *ptr == '\n' or *next == '\n' is valid.
	const char * const end = s + insertLength - 1;
	const char *ptr = s;
	unsigned char ch;

	if (chPrev == '\r' && *ptr == '\n') {
		++ptr;
		// Patch up what was end of line
		plv->SetLineStart(lineInsert - 1, (position + ptr - s));
		simpleInsertion = false;
	}

	if (utf8LineEnds) {
		uint8_t eolTable[256]{};
		eolTable[static_cast<uint8_t>('\n')] = 1;
		eolTable[static_cast<uint8_t>('\r')] = 2;
		// see UniConversion.h for LS, PS and NEL
		eolTable[0x85] = 4;
		eolTable[0xa8] = 3;
		eolTable[0xa9] = 3;

		while (ptr < end) {
			// skip to line end
			ch = *ptr++;
			uint8_t type;
			while ((type = eolTable[ch]) == 0 && ptr < end) {
				chBeforePrev = chPrev;
				chPrev = ch;
				ch = *ptr++;
			}
			switch (type) {
			case 2: // '\r'
				if (*ptr == '\n') {
					++ptr;
				}
				[[fallthrough]];
			case 1: // '\n'
				positions[nPositions++] = position + ptr - s;
				if (nPositions == PositionBlockSize) {
					plv->InsertLines(lineInsert, positions, nPositions, atLineStart);
					lineInsert += nPositions;
					nPositions = 0;
				}
				break;
			case 3:
			case 4:
				// LS, PS and NEL
				if ((type == 3 && chPrev == 0x80 && chBeforePrev == 0xe2) || (type == 4 && chPrev == 0xc2)) {
					positions[nPositions++] = position + ptr - s;
					if (nPositions == PositionBlockSize) {
						plv->InsertLines(lineInsert, positions, nPositions, atLineStart);
						lineInsert += nPositions;
						nPositions = 0;
					}
				}
				break;
			}

			chBeforePrev = chPrev;
			chPrev = ch;
		}
	} else {
#if NP2_USE_AVX2
		constexpr uint32_t LAST_CR_MASK = (1U << (sizeof(__m256i) - 1));
		const __m256i vectCR = _mm256_set1_epi8('\r');
		const __m256i vectLF = _mm256_set1_epi8('\n');
		while (ptr + sizeof(__m256i) <= end) {
			const __m256i chunk = _mm256_loadu_si256((__m256i *)ptr);
			uint32_t maskCR = _mm256_movemask_epi8(_mm256_cmpeq_epi8(chunk, vectCR));
			uint32_t maskLF = _mm256_movemask_epi8(_mm256_cmpeq_epi8(chunk, vectLF));

			const char *next = ptr + sizeof(__m256i);
			bool lastCR = false;
			if (maskCR) {
				if (maskCR & LAST_CR_MASK) {
					maskCR &= LAST_CR_MASK - 1;
					lastCR = true;
					if (*next == '\n') {
						// CR+LF across boundary
						++next;
					}
				}

				// maskCR and maskLF never have some bit set. after shifting maskCR by 1 bit,
				// the bits both set in maskCR and maskLF represents CR+LF;
				// the bits only set in maskCR or maskLF represents individual CR or LF.
				const uint32_t maskCRLF = (maskCR << 1) & maskLF; // CR+LF
				const uint32_t maskCR_LF = (maskCR << 1) ^ maskLF;// CR alone or LF alone
				maskLF = maskCR_LF & maskLF; // LF alone
				//maskCR = maskCR_LF ^ maskLF; // CR alone (with one position offset)
				// each set bit now represent end location of CR or LF in each line endings.
				maskLF |= maskCRLF | ((maskCR_LF ^ maskLF) >> 1);
			}
			if (maskLF) {
				if (nPositions >= PositionBlockSize - 32) {
					plv->InsertLines(lineInsert, positions, nPositions, atLineStart);
					lineInsert += nPositions;
					nPositions = 0;
				}

				Sci::Position offset = position + ptr - s;
				do {
					const uint32_t trailing = np2_ctz(maskLF);
					maskLF >>= trailing;
					//! shift 32 bit is undefined behavior: (0x80000000 >> 32) == 0x80000000.
					maskLF >>= 1;
					offset += trailing + 1;
					positions[nPositions++] = offset;
				} while (maskLF);
			}

			ptr = next;
			if (lastCR) {
				if (nPositions == PositionBlockSize) {
					plv->InsertLines(lineInsert, positions, nPositions, atLineStart);
					lineInsert += nPositions;
					nPositions = 0;
				}
				positions[nPositions++] = position + ptr - s;
			}
		}
		// end NP2_USE_AVX2
#elif NP2_USE_SSE2
		constexpr uint32_t LAST_CR_MASK = (1U << (2*sizeof(__m128i) - 1));
		const __m128i vectCR = _mm_set1_epi8('\r');
		const __m128i vectLF = _mm_set1_epi8('\n');
		while (ptr + 2*sizeof(__m128i) <= end) {
			// unaligned loading: line starts at random position.
			const __m128i chunk1 = _mm_loadu_si128((__m128i *)ptr);
			const __m128i chunk2 = _mm_loadu_si128((__m128i *)(ptr + sizeof(__m128i)));
			uint32_t maskCR = _mm_movemask_epi8(_mm_cmpeq_epi8(chunk1, vectCR));
			uint32_t maskLF = _mm_movemask_epi8(_mm_cmpeq_epi8(chunk1, vectLF));
			maskCR |= ((uint32_t)_mm_movemask_epi8(_mm_cmpeq_epi8(chunk2, vectCR))) << sizeof(__m128i);
			maskLF |= ((uint32_t)_mm_movemask_epi8(_mm_cmpeq_epi8(chunk2, vectLF))) << sizeof(__m128i);

			const char *next = ptr + 2*sizeof(__m128i);
			bool lastCR = false;
			if (maskCR) {
				if (maskCR & LAST_CR_MASK) {
					maskCR &= LAST_CR_MASK - 1;
					lastCR = true;
					if (*next == '\n') {
						// CR+LF across boundary
						++next;
					}
				}

				// maskCR and maskLF never have some bit set. after shifting maskCR by 1 bit,
				// the bits both set in maskCR and maskLF represents CR+LF;
				// the bits only set in maskCR or maskLF represents individual CR or LF.
				const uint32_t maskCRLF = (maskCR << 1) & maskLF; // CR+LF
				const uint32_t maskCR_LF = (maskCR << 1) ^ maskLF;// CR alone or LF alone
				maskLF = maskCR_LF & maskLF; // LF alone
				//maskCR = maskCR_LF ^ maskLF; // CR alone (with one position offset)
				// each set bit now represent end location of CR or LF in each line endings.
				maskLF |= maskCRLF | ((maskCR_LF ^ maskLF) >> 1);
			}
			if (maskLF) {
				if (nPositions >= PositionBlockSize - 32) {
					plv->InsertLines(lineInsert, positions, nPositions, atLineStart);
					lineInsert += nPositions;
					nPositions = 0;
				}

				Sci::Position offset = position + ptr - s;
				do {
					const uint32_t trailing = np2_ctz(maskLF);
					maskLF >>= trailing;
					//! shift 32 bit is undefined behavior: (0x80000000 >> 32) == 0x80000000.
					maskLF >>= 1;
					offset += trailing + 1;
					positions[nPositions++] = offset;
				} while (maskLF);
			}

			ptr = next;
			if (lastCR) {
				if (nPositions == PositionBlockSize) {
					plv->InsertLines(lineInsert, positions, nPositions, atLineStart);
					lineInsert += nPositions;
					nPositions = 0;
				}
				positions[nPositions++] = position + ptr - s;
			}
		}
		// end NP2_USE_SSE2
#endif

		constexpr uint32_t mask = (1 << '\r') | (1 << '\n');
		while (ptr < end) {
			// skip to line end
			ch = 0;
			while (ptr < end && ((ch = *ptr++) > '\r' || ((mask >> ch) & 1) == 0)) {
				// nop
			}
			switch (ch) {
			case '\r':
				if (*ptr == '\n') {
					++ptr;
				}
				[[fallthrough]];
			case '\n':
				if (nPositions == PositionBlockSize) {
					plv->InsertLines(lineInsert, positions, nPositions, atLineStart);
					lineInsert += nPositions;
					nPositions = 0;
				}
				positions[nPositions++] = position + ptr - s;
				break;
			}
		}
	}

	if (nPositions != 0) {
		plv->InsertLines(lineInsert, positions, nPositions, atLineStart);
		lineInsert += nPositions;
	}

	ch = *end;
	if (ptr == end) {
		++ptr;
		if (ch == '\r' || ch == '\n') {
			InsertLine(lineInsert, (position + ptr - s), atLineStart);
			lineInsert++;
		} else if (utf8LineEnds && !UTF8IsAscii(ch)) {
			if (UTF8IsMultibyteLineEnd(chBeforePrev, chPrev, ch)) {
				InsertLine(lineInsert, (position + ptr - s), atLineStart);
				lineInsert++;
			}
		}
	}

	//const double duration = period.Duration()*1e3;
	//printf("%s avx2=%d, cache=%d, perLine=%d, duration=%f\n", __func__, NP2_USE_AVX2,
	//	(int)PositionBlockSize, InsertString_WithoutPerLine, duration);

	// Joining two lines where last insertion is cr and following substance starts with lf
	if (chAfter == '\n') {
		if (ch == '\r') {
			// End of line already in buffer so drop the newly created one
			RemoveLine(lineInsert - 1);
			simpleInsertion = false;
		}
	} else if (utf8LineEnds && !UTF8IsAscii(chAfter)) {
		chBeforePrev = chPrev;
		chPrev = ch;
		// May have end of UTF-8 line end in buffer and start in insertion
		for (int j = 0; j < UTF8SeparatorLength - 1; j++) {
			const unsigned char chAt = substance.ValueAt(position + insertLength + j);
			const unsigned char back3[3] = { chBeforePrev, chPrev, chAt };
			if (UTF8IsSeparator(back3)) {
				InsertLine(lineInsert, (position + insertLength + j) + 1, atLineStart);
				lineInsert++;
			}
			if ((j == 0) && UTF8IsNEL(back3 + 1)) {
				InsertLine(lineInsert, (position + insertLength + j) + 1, atLineStart);
				lineInsert++;
			}
			chBeforePrev = chPrev;
			chPrev = chAt;
		}
	}
	if (maintainingIndex) {
		if (simpleInsertion && (lineInsert == lineStart)) {
			const CountWidths cw = CountCharacterWidthsUTF8(std::string_view(s, insertLength));
			plv->InsertCharacters(linePosition, cw);
		} else {
			RecalculateIndexLineStarts(linePosition, lineInsert - 1);
		}
	}
}

void CellBuffer::BasicDeleteChars(const Sci::Position position, const Sci::Position deleteLength) {
	if (deleteLength == 0)
		return;

	Sci::Line lineRecalculateStart = INVALID_POSITION;

	if ((position == 0) && (deleteLength == substance.Length())) {
		// If whole buffer is being deleted, faster to reinitialise lines data
		// than to delete each line.
		plv->Init();
	} else {
		// Have to fix up line positions before doing deletion as looking at text in buffer
		// to work out which lines have been removed

		const Sci::Line linePosition = plv->LineFromPosition(position);
		Sci::Line lineRemove = linePosition + 1;

		plv->InsertText(lineRemove - 1, -(deleteLength));
		const unsigned char chPrev = substance.ValueAt(position - 1);
		const unsigned char chBefore = chPrev;
		unsigned char chNext = substance.ValueAt(position);

		// Check for breaking apart a UTF-8 sequence
		// Needs further checks that text is UTF-8 or that some other break apart is occurring
		if (utf8Substance && MaintainingLineCharacterIndex()) {
			const Sci::Position posEnd = position + deleteLength;
			const Sci::Line lineEndRemove = plv->LineFromPosition(posEnd);
			const bool simpleDeletion =
				(linePosition == lineEndRemove) &&
				UTF8IsCharacterBoundary(position) && UTF8IsCharacterBoundary(posEnd);
			if (simpleDeletion) {
				std::string text(deleteLength, '\0');
				GetCharRange(text.data(), position, deleteLength);
				if (UTF8IsValid(text)) {
					// Everything is good
					const CountWidths cw = CountCharacterWidthsUTF8(text);
					plv->InsertCharacters(linePosition, -cw);
				} else {
					lineRecalculateStart = linePosition;
				}
			} else {
				lineRecalculateStart = linePosition;
			}
		}

		bool ignoreNL = false;
		if (chPrev == '\r' && chNext == '\n') {
			// Move back one
			plv->SetLineStart(lineRemove, position);
			lineRemove++;
			ignoreNL = true; 	// First \n is not real deletion
		}
		if (utf8LineEnds && UTF8IsTrailByte(chNext)) {
			if (UTF8LineEndOverlaps(position)) {
				RemoveLine(lineRemove);
			}
		}

		unsigned char ch = chNext;
		for (Sci::Position i = 0; i < deleteLength; i++) {
			chNext = substance.ValueAt(position + i + 1);
			if (ch == '\r') {
				if (chNext != '\n') {
					RemoveLine(lineRemove);
				}
			} else if (ch == '\n') {
				if (ignoreNL) {
					ignoreNL = false; 	// Further \n are real deletions
				} else {
					RemoveLine(lineRemove);
				}
			} else if (utf8LineEnds && !UTF8IsAscii(ch)) {
				const unsigned char next3[3] = { ch, chNext,
					static_cast<unsigned char>(substance.ValueAt(position + i + 2)) };
				if (UTF8IsSeparator(next3) || UTF8IsNEL(next3)) {
					RemoveLine(lineRemove);
				}
			}

			ch = chNext;
		}
		// May have to fix up end if last deletion causes cr to be next to lf
		// or removes one of a crlf pair
		const char chAfter = substance.ValueAt(position + deleteLength);
		if (chBefore == '\r' && chAfter == '\n') {
			// Using lineRemove-1 as cr ended line before start of deletion
			RemoveLine(lineRemove - 1);
			plv->SetLineStart(lineRemove - 1, position + 1);
		}
	}
	substance.DeleteRange(position, deleteLength);
	if (lineRecalculateStart >= 0) {
		RecalculateIndexLineStarts(lineRecalculateStart, lineRecalculateStart);
	}
	if (hasStyles) {
		style.DeleteRange(position, deleteLength);
	}
}

bool CellBuffer::SetUndoCollection(bool collectUndo) noexcept {
	collectingUndo = collectUndo;
	uh.DropUndoSequence();
	return collectingUndo;
}

bool CellBuffer::IsCollectingUndo() const noexcept {
	return collectingUndo;
}

void CellBuffer::BeginUndoAction() {
	uh.BeginUndoAction();
}

void CellBuffer::EndUndoAction() {
	uh.EndUndoAction();
}

void CellBuffer::AddUndoAction(Sci::Position token, bool mayCoalesce) {
	bool startSequence;
	uh.AppendAction(containerAction, token, nullptr, 0, startSequence, mayCoalesce);
}

void CellBuffer::DeleteUndoHistory() {
	uh.DeleteUndoHistory();
}

bool CellBuffer::CanUndo() const noexcept {
	return uh.CanUndo();
}

int CellBuffer::StartUndo() noexcept {
	return uh.StartUndo();
}

const Action &CellBuffer::GetUndoStep() const noexcept {
	return uh.GetUndoStep();
}

void CellBuffer::PerformUndoStep() {
	const Action &actionStep = uh.GetUndoStep();
	if (actionStep.at == insertAction) {
		if (substance.Length() < actionStep.lenData) {
			throw std::runtime_error(
				"CellBuffer::PerformUndoStep: deletion must be less than document length.");
		}
		BasicDeleteChars(actionStep.position, actionStep.lenData);
	} else if (actionStep.at == removeAction) {
		BasicInsertString(actionStep.position, actionStep.data.get(), actionStep.lenData);
	}
	uh.CompletedUndoStep();
}

bool CellBuffer::CanRedo() const noexcept {
	return uh.CanRedo();
}

int CellBuffer::StartRedo() noexcept {
	return uh.StartRedo();
}

const Action &CellBuffer::GetRedoStep() const noexcept {
	return uh.GetRedoStep();
}

void CellBuffer::PerformRedoStep() {
	const Action &actionStep = uh.GetRedoStep();
	if (actionStep.at == insertAction) {
		BasicInsertString(actionStep.position, actionStep.data.get(), actionStep.lenData);
	} else if (actionStep.at == removeAction) {
		BasicDeleteChars(actionStep.position, actionStep.lenData);
	}
	uh.CompletedRedoStep();
}
