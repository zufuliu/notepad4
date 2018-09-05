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

UINT Style_GetDefaultFoldState(void) {
	switch (pLexCurrent->rid) {
	case NP2LEX_DEFAULT:
	case NP2LEX_ANSI:
		return (1 << 0) | (1 << 4);
	case NP2LEX_CPP:
	case NP2LEX_CSHARP:
	case NP2LEX_XML:
	case NP2LEX_HTML:
	case NP2LEX_JSON:
		return (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
	case NP2LEX_JAVA:
	case NP2LEX_RC:
	case NP2LEX_SCALA:
	case NP2LEX_RUBY:
		return (1 << 0) | (1 << 1) | (1 << 2);
	case NP2LEX_INI:
	case NP2LEX_DIFF:
		return (1 << 0);
	case NP2LEX_PYTHON:
		return (1 << 0) | (1 << 4) | (1 << 8);
	}
	return (1 << 0) | (1 << 1);
}

void FoldToggleNode(int line, FOLD_ACTION *pAction, BOOL *fToggled) {
	const BOOL fExpanded = SciCall_GetFoldExpanded(line);
	FOLD_ACTION action = *pAction;
	if (action == FOLD_ACTION_SNIFF) {
		action = fExpanded ? FOLD_ACTION_FOLD : FOLD_ACTION_EXPAND;
	}

	if ((action == FOLD_ACTION_FOLD && fExpanded) || (action == FOLD_ACTION_EXPAND && !fExpanded)) {
		SciCall_ToggleFold(line);
		if (*fToggled == FALSE || *pAction == FOLD_ACTION_SNIFF) {
			const BOOL after = SciCall_GetFoldExpanded(line);
			if (fExpanded != after) {
				*fToggled = TRUE;
				if (*pAction == FOLD_ACTION_SNIFF) {
					*pAction = action;
				}
			}
		}
	}
}

void FoldToggleAll(FOLD_ACTION action) {
	BOOL fToggled = FALSE;
	const int lineCount = SciCall_GetLineCount();

	for (int line = 0; line < lineCount; ++line) {
		int level = SciCall_GetFoldLevel(line);
		if (level & SC_FOLDLEVELHEADERFLAG) {
			FoldToggleNode(line, &action, &fToggled);
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

void FoldToggleLevel(int lev, FOLD_ACTION action) {
	BOOL fToggled = FALSE;
	const int lineCount = SciCall_GetLineCount();

	switch (pLexCurrent->iLexer) {
	case SCLEX_NULL:
	case SCLEX_PYTHON:
		lev = lev * 4;
		break;
	}
	lev += SC_FOLDLEVELBASE;

	for (int line = 0; line < lineCount; ++line) {
		int level = SciCall_GetFoldLevel(line);
		if (level & SC_FOLDLEVELHEADERFLAG) {
			level &= SC_FOLDLEVELNUMBERMASK;
			if (lev == level) {
				FoldToggleNode(line, &action, &fToggled);
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

void FoldToggleCurrent(FOLD_ACTION action) {
	BOOL fToggled = FALSE;
	int line = SciCall_LineFromPosition(SciCall_GetCurrentPos());
	int level = SciCall_GetFoldLevel(line);

	if (!(level & SC_FOLDLEVELHEADERFLAG)) {
#if 1
		// Document::GetFoldParent(Sci::Line line)
		line = SciCall_GetFoldParent(line);
#else
		int lev = level & SC_FOLDLEVELNUMBERMASK;
		switch (pLexCurrent->iLexer) {
		case SCLEX_NULL:
		case SCLEX_PYTHON:
			lev -= 4;
			break;
		default:
			lev -= 1;
			break;
		}

		--line;
		while (line >= 0) {
			level = SciCall_GetFoldLevel(line);
			if (level & SC_FOLDLEVELHEADERFLAG) {
				level &= SC_FOLDLEVELNUMBERMASK;
				if (lev == level) {
					break;
				}
			}
			--line;
		}
#endif
		if (line < 0) {
			return;
		}
	}

	FoldToggleNode(line, &action, &fToggled);
	if (fToggled) {
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_SLOP | CARET_STRICT | CARET_EVEN, 5);
		SciCall_ScrollCaret();
		SciCall_SetXCaretPolicy(CARET_SLOP | CARET_EVEN, 50);
		SciCall_SetYCaretPolicy(CARET_EVEN, 0);
	}
}

void FoldToggleDefault(FOLD_ACTION action) {
	BOOL fToggled = FALSE;
	const UINT state = Style_GetDefaultFoldState();
	const int lineCount = SciCall_GetLineCount();

	for (int line = 0; line < lineCount; ++line) {
		int level = SciCall_GetFoldLevel(line);
		if (level & SC_FOLDLEVELHEADERFLAG) {
			level &= SC_FOLDLEVELNUMBERMASK;
			level -= SC_FOLDLEVELBASE;
			if (state & (1U << level)) {
				FoldToggleNode(line, &action, &fToggled);
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

void FoldPerformAction(int ln, int mode, FOLD_ACTION action) {
	BOOL fToggled = FALSE;
	if (mode & (FOLD_CHILDREN | FOLD_SIBLINGS)) {
		// ln/lvNode: line and level of the source of this fold action
		const int lnNode = ln;
		const int lvNode = SciCall_GetFoldLevel(lnNode) & SC_FOLDLEVELNUMBERMASK;
		const int lnTotal = SciCall_GetLineCount();

		// lvStop: the level over which we should not cross
		int lvStop = lvNode;

		if (mode & FOLD_SIBLINGS) {
			ln = SciCall_GetFoldParent(lnNode) + 1;	 // -1 + 1 = 0 if no parent
			--lvStop;
		}

		for (; ln < lnTotal; ++ln) {
			int lv = SciCall_GetFoldLevel(ln);
			BOOL fHeader = (lv & SC_FOLDLEVELHEADERFLAG) != 0;
			lv &= SC_FOLDLEVELNUMBERMASK;

			if (lv < lvStop || (lv == lvStop && fHeader && ln != lnNode)) {
				return;
			}
			if (fHeader && (lv == lvNode || (lv > lvNode && (mode & FOLD_CHILDREN)))) {
				FoldToggleNode(ln, &action, &fToggled);
			}
		}
	} else {
		FoldToggleNode(ln, &action, &fToggled);
	}
}

void FoldClick(int ln, int mode) {
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

void FoldAltArrow(int key, int mode) {
	// Because Alt-Shift is already in use (and because the sibling fold feature
	// is not as useful from the keyboard), only the Ctrl modifier is supported

	if (bShowCodeFolding && (mode & (SCMOD_ALT | SCMOD_SHIFT)) == SCMOD_ALT) {
		int ln = SciCall_LineFromPosition(SciCall_GetCurrentPos());

		// Jump to the next visible fold point
		if (key == SCK_DOWN && !(mode & SCMOD_CTRL)) {
			const int lnTotal = SciCall_GetLineCount();
			for (ln = ln + 1; ln < lnTotal; ++ln) {
				if ((SciCall_GetFoldLevel(ln) & SC_FOLDLEVELHEADERFLAG) && SciCall_GetLineVisible(ln)) {
					EditJumpTo(hwndEdit, ln + 1, 0);
					return;
				}
			}
		} else if (key == SCK_UP && !(mode & SCMOD_CTRL)) {// Jump to the previous visible fold point
			for (ln = ln - 1; ln >= 0; --ln) {
				if ((SciCall_GetFoldLevel(ln) & SC_FOLDLEVELHEADERFLAG) && SciCall_GetLineVisible(ln)) {
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
