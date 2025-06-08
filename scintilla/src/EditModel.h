// Scintilla source code edit control
/** @file EditModel.h
 ** Defines the editor state that must be visible to EditView.
 **/
// Copyright 1998-2014 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

/**
*/
class Caret {
public:
	bool active;
	bool on;
	int period;

	Caret() noexcept;
};

enum class UndoRedo { undo, redo };

// Selection stack is sparse so use a map

struct SelectionWithScroll final {
	std::string selection;
	Sci::Line topLine = 0;
};

using SelectionStack = std::map<int, SelectionWithScroll>;

struct SelectionHistory final {
	int indexCurrent = 0;
	std::string ssCurrent;
	SelectionStack stack;
};

struct ModelState final : ViewState {
	SelectionHistory historyForUndo;
	SelectionHistory historyForRedo;
	void RememberSelectionForUndo(int index, const Selection &sel);
	void ForgetSelectionForUndo() noexcept;
	void RememberSelectionOntoStack(int index, Sci::Line topLine);
	void RememberSelectionForRedoOntoStack(int index, const Selection &sel, Sci::Line topLine);
	SelectionWithScroll SelectionFromStack(int index, UndoRedo history) const;
	void TruncateUndo(int index) override;
};

using ModelStateShared = std::shared_ptr<ModelState>;

class EditModel {
public:
	Document *pdoc;
	bool inOverstrike;
	bool trackLineWidth;
	bool hasFocus;
	bool primarySelection;
	int xOffset;		///< Horizontal scrolled amount in pixels

	const std::unique_ptr<SpecialRepresentations> reprs;
	Caret caret;
	SelectionPosition posDrag;
	Sci::Position braces[2];
	int bracesMatchStyle;
	int highlightGuideColumn;
	Selection sel;
	std::string copySeparator;

	Scintilla::IMEInteraction imeInteraction;
	Scintilla::Bidirectional bidirectional;

	Scintilla::FoldFlag foldFlags;
	Scintilla::FoldDisplayTextStyle foldDisplayTextStyle;
	UniqueString defaultFoldDisplayText;
	std::unique_ptr<IContractionState> pcs;
	// Hotspot support
	Range hotspot;
	bool hotspotSingleLine;
	bool needRedoRemembered = false;
	Sci::Position hoverIndicatorPos;

	Scintilla::ChangeHistoryOption changeHistoryOption = Scintilla::ChangeHistoryOption::Disabled;
	Scintilla::UndoSelectionHistoryOption undoSelectionHistoryOption = UndoSelectionHistoryOption::Disabled;
	ModelStateShared modelState;

	// Wrapping support
	int wrapWidth;
	uint32_t hardwareConcurrency;
	uint32_t minParallelLayoutLength;
	uint32_t maxParallelLayoutLength;
	ActionDuration durationWrapOneUnit;
	static constexpr uint32_t IdleLineWrapTime = 250;
	static constexpr uint32_t MaxPaintTextTime = 16; // 60Hz
	static constexpr uint32_t ParallelLayoutBlockSize = 4096;
	void *idleTaskTimer;

	EditModel();
	// Deleted so EditModel objects can not be copied.
	EditModel(const EditModel &) = delete;
	EditModel(EditModel &&) = delete;
	EditModel &operator=(const EditModel &) = delete;
	EditModel &operator=(EditModel &&) = delete;
	virtual ~EditModel();
	virtual Sci::Line TopLineOfMain() const noexcept = 0;
	virtual Point GetVisibleOriginInMain() const noexcept = 0;
	virtual Sci::Line LinesOnScreen() const noexcept = 0;
	virtual void OnLineWrapped(Sci::Line lineDoc, int linesWrapped, int option) = 0;
	bool BidirectionalEnabled() const noexcept;
	bool BidirectionalR2L() const noexcept {
		return bidirectional == Scintilla::Bidirectional::R2L;
	}
	SurfaceMode CurrentSurfaceMode() const noexcept;
	void SetDefaultFoldDisplayText(const char *text);
	const char *GetDefaultFoldDisplayText() const noexcept;
	const char *GetFoldDisplayText(Sci::Line lineDoc, bool partialLine) const noexcept;
	InSelection LineEndInSelection(Sci::Line lineDoc) const noexcept;
	[[nodiscard]] MarkerMask GetMark(Sci::Line line) const noexcept;

	void EnsureModelState();
	void ChangeUndoSelectionHistory(Scintilla::UndoSelectionHistoryOption undoSelectionHistoryOptionNew) noexcept;

	void SetIdleTaskTime(uint32_t milliseconds) const noexcept;
	bool IdleTaskTimeExpired() const noexcept;
	void UpdateParallelLayoutThreshold() noexcept;
};

}
