// Scintilla source code edit control
/** @file EditModel.h
 ** Defines the editor state that must be visible to EditorView.
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

class EditModel {
public:
	Document *pdoc;
	bool inOverstrike;
	bool trackLineWidth;
	int xOffset;		///< Horizontal scrolled amount in pixels

	SpecialRepresentations reprs;
	Caret caret;
	SelectionPosition posDrag;
	Sci::Position braces[2];
	int bracesMatchStyle;
	int highlightGuideColumn;
	bool hasFocus;
	bool primarySelection;
	Selection sel;

	Scintilla::IMEInteraction imeInteraction;
	Scintilla::Bidirectional bidirectional;

	Scintilla::FoldFlag foldFlags;
	Scintilla::FoldDisplayTextStyle foldDisplayTextStyle;
	UniqueString defaultFoldDisplayText;
	std::unique_ptr<IContractionState> pcs;
	// Hotspot support
	Range hotspot;
	bool hotspotSingleLine;
	Sci::Position hoverIndicatorPos;

	// Wrapping support
	int wrapWidth;
	uint32_t hardwareConcurrency;
	uint32_t minParallelLayoutLength;
	uint32_t maxParallelLayoutLength;
	ActionDuration durationWrapOneUnit;
	ActionDuration durationWrapOneThread;
	static constexpr uint32_t IdleLineWrapTime = 250;
	static constexpr uint32_t ActiveLineWrapTime = 500;
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
	virtual void OnLineWrapped(Sci::Line lineDoc, int linesWrapped) = 0;
	bool BidirectionalEnabled() const noexcept;
	bool BidirectionalR2L() const noexcept {
		return bidirectional == Scintilla::Bidirectional::R2L;
	}
	SurfaceMode CurrentSurfaceMode() const noexcept;
	void SetDefaultFoldDisplayText(const char *text);
	const char *GetDefaultFoldDisplayText() const noexcept;
	const char *GetFoldDisplayText(Sci::Line lineDoc, bool partialLine) const noexcept;
	InSelection LineEndInSelection(Sci::Line lineDoc) const noexcept;
	void SetIdleTaskTime(uint32_t milliseconds) const noexcept;
	bool IdleTaskTimeExpired() const noexcept;
	void UpdateParallelLayoutThreshold() noexcept;
};

}
