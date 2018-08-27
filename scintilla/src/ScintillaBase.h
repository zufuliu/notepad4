// Scintilla source code edit control
/** @file ScintillaBase.h
 ** Defines an enhanced subclass of Editor with calltips, autocomplete and context menu.
 **/
// Copyright 1998-2002 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef SCINTILLABASE_H
#define SCINTILLABASE_H

namespace Scintilla {

#ifdef SCI_LEXER
class LexState;
#endif

/**
 */
class ScintillaBase : public Editor, IListBoxDelegate {
protected:
	/** Enumeration of commands and child windows. */
	enum {
		idCallTip = 1,
		idAutoComplete = 2,

		idcmdUndo = 10,
		idcmdRedo = 11,
		idcmdCut = 12,
		idcmdCopy = 13,
		idcmdPaste = 14,
		idcmdDelete = 15,
		idcmdSelectAll = 16
	};

	enum {
		maxLenInputIME = 200
	};

	int displayPopupMenu;
	Menu popup;
	AutoComplete ac;

	CallTip ct;

	int listType;			///< 0 is an autocomplete list
	int maxListWidth;		/// Maximum width of list, in average character widths
	int multiAutoCMode; /// Mode for autocompleting when multiple selections are present

#ifdef SCI_LEXER
	LexState *DocumentLexState();
#endif

	ScintillaBase() noexcept;
	~ScintillaBase() override;
	void Initialise() noexcept override {}
	void Finalise() noexcept override;

	void AddCharUTF(const char *s, unsigned int len, bool treatAsDBCS = false) override;
	void Command(int cmdId);
	void CancelModes() noexcept override;
	int KeyCommand(unsigned int iMessage) override;

	void AutoCompleteInsert(Sci::Position startPos, Sci::Position removeLen, const char *text, Sci::Position textLen);
	void AutoCompleteStart(Sci::Position lenEntered, const char *list);
	void AutoCompleteCancel() noexcept;
	void AutoCompleteMove(int delta);
	int AutoCompleteGetCurrent() const noexcept;
	int AutoCompleteGetCurrentText(char *buffer) const;
	void AutoCompleteCharacterAdded(char ch);
	void AutoCompleteCharacterDeleted();
	void AutoCompleteCompleted(char ch, unsigned int completionMethod);
	void AutoCompleteMoveToCurrentWord();
	void AutoCompleteSelection();
	void ListNotify(ListBoxEvent *plbe) override;

	void CallTipClick() noexcept;
	void CallTipShow(const Point &pt, const char *defn);
	virtual void CreateCallTipWindow(const PRectangle &rc) noexcept = 0;

	virtual void AddToPopUp(const char *label, int cmd = 0, bool enabled = true) noexcept = 0;
	bool ShouldDisplayPopup(const Point &ptInWindowCoordinates) const noexcept;
	void ContextMenu(const Point &pt);

	void ButtonDownWithModifiers(const Point &pt, unsigned int curTime, int modifiers) override;
	void RightButtonDownWithModifiers(const Point &pt, unsigned int curTime, int modifiers) override;

	void NotifyStyleToNeeded(Sci::Position endStyleNeeded) override;
	void NotifyLexerChanged(Document *doc, void *userData) override;

public:
	// Deleted so ScintillaBase objects can not be copied.
	ScintillaBase(const ScintillaBase &) = delete;
	ScintillaBase(ScintillaBase &&) = delete;
	ScintillaBase &operator=(const ScintillaBase &) = delete;
	ScintillaBase &operator=(ScintillaBase &&) = delete;
	// Public so scintilla_send_message can use it
	sptr_t WndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam) override;
};

}

#endif
