//==============================================================================
//
// Folding Functions
//
//
typedef enum {
	FOLD_ACTION_EXPAND	= 1,
	FOLD_ACTION_SNIFF	= 0,
	FOLD_ACTION_FOLD	= -1
} FOLD_ACTION;

#define FOLD_CHILDREN SCMOD_CTRL
#define FOLD_SIBLINGS SCMOD_SHIFT

UINT stateToggleFoldLevel = 0;

UINT Style_GetDefaultFoldState()
{
	switch (pLexCurrent->rid) {
	case NP2LEX_DEFAULT:
	case NP2LEX_ANSI:
		return 0;
	case NP2LEX_CPP:
	case NP2LEX_CSHARP:
	case NP2LEX_XML:
	case NP2LEX_HTML:
	case NP2LEX_JSON:
		return (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);
	case NP2LEX_JAVA:
	case NP2LEX_RC:
	case NP2LEX_SCALA:
	case NP2LEX_RUBY:
		return (1 << 1) | (1 << 2) | (1 << 3);
	case NP2LEX_INI:
	case NP2LEX_DIFF:
		return (1 << 1);
	case NP2LEX_PYTHON:
		return (1 << 1) | (1 << 5) | (1 << 9);
	}
	return (1 << 1) | (1 << 2);
}

BOOL FoldToggleNode(int line, FOLD_ACTION action)
{
	BOOL fExpanded = SciCall_GetFoldExpanded(line);

	if ((action == FOLD_ACTION_FOLD && fExpanded) || (action == FOLD_ACTION_EXPAND && !fExpanded)) {
		SciCall_ToggleFold(line);
		return TRUE;
	}

	return FALSE;
}

void FoldToggleAll(FOLD_ACTION action)
{
	BOOL fToggled = FALSE;
	int lineCount = SciCall_GetLineCount();
	int line;
	int maxLevel = 0;

	for (line = 0; line < lineCount; ++line) {
		int level = SciCall_GetFoldLevel(line);
		if (level & SC_FOLDLEVELHEADERFLAG) {
			if (action == FOLD_ACTION_SNIFF) {
				action = SciCall_GetFoldExpanded(line) ? FOLD_ACTION_FOLD : FOLD_ACTION_EXPAND;
			}
			if (FoldToggleNode(line, action)) {
				fToggled = TRUE;
			}
			level -= SC_FOLDLEVELBASE;
			level &= SC_FOLDLEVELNUMBERMASK;
			if (level <= 10 && maxLevel < level) {
				maxLevel = level;
			}
		}
	}

	stateToggleFoldLevel = 0;
	if (fToggled) {
		if (action == FOLD_ACTION_FOLD) {
			++maxLevel;
			stateToggleFoldLevel = (1 << (maxLevel + 1)) - 1;
		}
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 5);
		SciCall_ScrollCaret();
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_EVEN, 0);
	}
}

void FoldToggleLevel(int lev, FOLD_ACTION action)
{
	BOOL fToggled = FALSE;
	int lineCount = SciCall_GetLineCount();
	int line;

	for (line = 0; line < lineCount; ++line) {
		int level = SciCall_GetFoldLevel(line);
		if (level & SC_FOLDLEVELHEADERFLAG) {
			level -= SC_FOLDLEVELBASE;
			if (lev == ((level & SC_FOLDLEVELNUMBERMASK))) {
				if (action == FOLD_ACTION_SNIFF) {
					action = SciCall_GetFoldExpanded(line) ? FOLD_ACTION_FOLD : FOLD_ACTION_EXPAND;
				}
				if (FoldToggleNode(line, action)) {
					fToggled = TRUE;
				}
			}
		}
	}

	++lev;
	if (fToggled && action == FOLD_ACTION_FOLD) {
		stateToggleFoldLevel |= (1 << lev);
	} else {
		stateToggleFoldLevel &= ~(1 << lev);
	}
	if (fToggled) {
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 5);
		SciCall_ScrollCaret();
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_EVEN, 0);
	}
}

void FoldToggleDefault(FOLD_ACTION action)
{
	UINT state;
	BOOL fToggled = FALSE;
	int lineCount;
	int line;
	stateToggleFoldLevel = 0;

	state = Style_GetDefaultFoldState();
	if (state == 0)		return;
	lineCount = SciCall_GetLineCount();
	if (lineCount < 2)	return;

	for (line = 0; line < lineCount; ++line) {
		int level = SciCall_GetFoldLevel(line);
		if (level & SC_FOLDLEVELHEADERFLAG) {
			level -= SC_FOLDLEVELBASE;
			level &= SC_FOLDLEVELNUMBERMASK;
			++level;
			if (state & (1 << level)) {
				if (action == FOLD_ACTION_SNIFF) {
					action = SciCall_GetFoldExpanded(line) ? FOLD_ACTION_FOLD : FOLD_ACTION_EXPAND;
				}
				if (FoldToggleNode(line, action)) {
					fToggled = TRUE;
				}
				if (fToggled && action == FOLD_ACTION_FOLD) {
					stateToggleFoldLevel |= (1 << level);
				} else {
					stateToggleFoldLevel &= ~(1 << level);
				}
			}
		}
	}

	if (fToggled) {
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 5);
		SciCall_ScrollCaret();
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_EVEN, 0);
	}
}

void FoldPerformAction(int ln, int mode, FOLD_ACTION action)
{
	if (action == FOLD_ACTION_SNIFF) {
		action = SciCall_GetFoldExpanded(ln) ? FOLD_ACTION_FOLD : FOLD_ACTION_EXPAND;
	}

	if (mode & (FOLD_CHILDREN | FOLD_SIBLINGS)) {
		// ln/lvNode: line and level of the source of this fold action
		int lnNode = ln;
		int lvNode = SciCall_GetFoldLevel(lnNode) & SC_FOLDLEVELNUMBERMASK;
		int lnTotal = SciCall_GetLineCount();

		// lvStop: the level over which we should not cross
		int lvStop = lvNode;

		if (mode & FOLD_SIBLINGS) {
			ln = SciCall_GetFoldParent(lnNode) + 1;	 // -1 + 1 = 0 if no parent
			--lvStop;
		}

		for (; ln < lnTotal; ++ln) {
			int lv = SciCall_GetFoldLevel(ln);
			BOOL fHeader = lv & SC_FOLDLEVELHEADERFLAG;
			lv &= SC_FOLDLEVELNUMBERMASK;

			if (lv < lvStop || (lv == lvStop && fHeader && ln != lnNode)) {
				return;
			} else if (fHeader && (lv == lvNode || (lv > lvNode && mode & FOLD_CHILDREN))) {
				FoldToggleNode(ln, action);
			}
		}
	} else {
		FoldToggleNode(ln, action);
	}
}

void FoldClick(int ln, int mode)
{
	static struct {
		int ln;
		int mode;
		DWORD dwTickCount;
	} prev;

	BOOL fGotoFoldPoint = mode & FOLD_SIBLINGS;

	if (!(SciCall_GetFoldLevel(ln) & SC_FOLDLEVELHEADERFLAG)) {
		// Not a fold point: need to look for a double-click
		if (prev.ln == ln && prev.mode == mode && (GetTickCount() - prev.dwTickCount <= GetDoubleClickTime())) {
			prev.ln = -1; // Prevent re-triggering on a triple-click
			ln = SciCall_GetFoldParent(ln);

			if (ln >= 0 && SciCall_GetFoldExpanded(ln)) {
				fGotoFoldPoint = TRUE;
			} else {
				return;
			}
		} else {
			// Save the info needed to match this click with the next click
			prev.ln = ln;
			prev.mode = mode;
			prev.dwTickCount = GetTickCount();
			return;
		}
	}

	FoldPerformAction(ln, mode, FOLD_ACTION_SNIFF);
	if (fGotoFoldPoint) {
		EditJumpTo(hwndEdit, ln + 1, 0);
	}
}

void FoldAltArrow(int key, int mode)
{
	// Because Alt-Shift is already in use (and because the sibling fold feature
	// is not as useful from the keyboard), only the Ctrl modifier is supported

	if (bShowCodeFolding && (mode & (SCMOD_ALT | SCMOD_SHIFT)) == SCMOD_ALT) {
		int ln = SciCall_LineFromPosition(SciCall_GetCurrentPos());

		// Jump to the next visible fold point
		if (key == SCK_DOWN && !(mode & SCMOD_CTRL)) {
			int lnTotal = SciCall_GetLineCount();
			for (ln = ln + 1; ln < lnTotal; ++ln) {
				if (SciCall_GetFoldLevel(ln) & SC_FOLDLEVELHEADERFLAG && SciCall_GetLineVisible(ln)) {
					EditJumpTo(hwndEdit, ln + 1, 0);
					return;
				}
			}
		} else if (key == SCK_UP && !(mode & SCMOD_CTRL)) {// Jump to the previous visible fold point
			for (ln = ln - 1; ln >= 0; --ln) {
				if (SciCall_GetFoldLevel(ln) & SC_FOLDLEVELHEADERFLAG && SciCall_GetLineVisible(ln)) {
					EditJumpTo(hwndEdit, ln + 1, 0);
					return;
				}
			}
		} else if (SciCall_GetFoldLevel(ln) & SC_FOLDLEVELHEADERFLAG) {// Perform a fold/unfold operation
			if (key == SCK_LEFT) {
				FoldPerformAction(ln, mode, FOLD_ACTION_FOLD);
			}
			if (key == SCK_RIGHT) {
				FoldPerformAction(ln, mode, FOLD_ACTION_EXPAND);
			}
		}
	}
}
