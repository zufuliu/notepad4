//==============================================================================
//
// Folding Functions
//
//
typedef enum {
	FOLD_ACTION_FOLD	= 0, // SC_FOLDACTION_CONTRACT
	FOLD_ACTION_EXPAND	= 1, // SC_FOLDACTION_EXPAND
	FOLD_ACTION_SNIFF	= 2, // SC_FOLDACTION_TOGGLE
} FOLD_ACTION;

#define FOLD_CHILDREN SCMOD_CTRL
#define FOLD_SIBLINGS SCMOD_SHIFT

#define MAX_EDIT_TOGGLE_FOLD_LEVEL		10
struct EditFoldStack {
	int level_count; // 1-based level number at current header line
	int level_stack[MAX_EDIT_TOGGLE_FOLD_LEVEL];
};

static void EditFoldStack_Push(struct EditFoldStack *foldStack, int level) {
	while (foldStack->level_count != 0 && level <= foldStack->level_stack[foldStack->level_count - 1]) {
		--foldStack->level_count;
	}

	foldStack->level_stack[foldStack->level_count] = level;
	++foldStack->level_count;
}

UINT Style_GetDefaultFoldState(int *maxLevel) {
	switch (pLexCurrent->rid) {
	case NP2LEX_TEXTFILE:
	case NP2LEX_2NDTEXTFILE:
	case NP2LEX_ANSI:
		*maxLevel = 2;
		return (1 << 1) | (1 << 2);
	case NP2LEX_CPP:
	case NP2LEX_CSHARP:
	case NP2LEX_XML:
	case NP2LEX_HTML:
	case NP2LEX_JSON:
		*maxLevel = 3;
		return (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
	case NP2LEX_JAVA:
	case NP2LEX_RC:
	case NP2LEX_SCALA:
	case NP2LEX_RUBY:
		*maxLevel = 2;
		return (1 << 0) | (1 << 1) | (1 << 2);
	case NP2LEX_INI:
		*maxLevel = 0;
		return (1 << 0);
	case NP2LEX_DIFF:
		*maxLevel = 2;
		return (1 << 0) | (1 << 2);
	case NP2LEX_PYTHON:
		*maxLevel = 3;
		return (1 << 1) | (1 << 2) | (1 << 3);
	default:
		*maxLevel = 1;
		return (1 << 0) | (1 << 1);
	}
}

void FoldToggleNode(int line, FOLD_ACTION *pAction, BOOL *fToggled) {
	const BOOL fExpanded = SciCall_GetFoldExpanded(line);
	FOLD_ACTION action = *pAction;
	if (action == FOLD_ACTION_SNIFF) {
		action = fExpanded ? FOLD_ACTION_FOLD : FOLD_ACTION_EXPAND;
	}

	if (action ^ fExpanded) {
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
		const int level = SciCall_GetFoldLevel(line);
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
	int line = 0;

	switch (pLexCurrent->iLexer) {
	case SCLEX_PYTHON:
	case SCLEX_NULL: {
		struct EditFoldStack foldStack = { 0, { 0 }};
		++lev;
		while (line < lineCount) {
			int level = SciCall_GetFoldLevel(line);
			if (level & SC_FOLDLEVELHEADERFLAG) {
				level &= SC_FOLDLEVELNUMBERMASK;
				EditFoldStack_Push(&foldStack, level);
				if (lev == foldStack.level_count) {
					FoldToggleNode(line, &action, &fToggled);
					line = SciCall_GetLastChild(line);
				}
			}
			++line;
		}
	}
	break;

	default:
		lev += SC_FOLDLEVELBASE;
		while (line < lineCount) {
			int level = SciCall_GetFoldLevel(line);
			if (level & SC_FOLDLEVELHEADERFLAG) {
				level &= SC_FOLDLEVELNUMBERMASK;
				if (lev == level) {
					FoldToggleNode(line, &action, &fToggled);
					line = SciCall_GetLastChild(line);
				}
			}
			++line;
		}
		break;
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
	const int level = SciCall_GetFoldLevel(line);

	if (!(level & SC_FOLDLEVELHEADERFLAG)) {
		line = SciCall_GetFoldParent(line);
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
	int maxLevel = 0;
	const UINT state = Style_GetDefaultFoldState(&maxLevel);
	const int lineCount = SciCall_GetLineCount();
	int line = 0;

	switch (pLexCurrent->iLexer) {
	case SCLEX_PYTHON:
	case SCLEX_NULL: {
		struct EditFoldStack foldStack = { 0, { 0 }};
		while (line < lineCount) {
			int level = SciCall_GetFoldLevel(line);
			if (level & SC_FOLDLEVELHEADERFLAG) {
				level &= SC_FOLDLEVELNUMBERMASK;
				EditFoldStack_Push(&foldStack, level);
				level = foldStack.level_count;
				if (state & (1U << level)) {
					FoldToggleNode(line, &action, &fToggled);
					if (level == maxLevel) {
						line = SciCall_GetLastChild(line);
					}
				}
			}
			++line;
		}
	}
	break;

	default:
		while (line < lineCount) {
			int level = SciCall_GetFoldLevel(line);
			if (level & SC_FOLDLEVELHEADERFLAG) {
				level &= SC_FOLDLEVELNUMBERMASK;
				level -= SC_FOLDLEVELBASE;
				if (state & (1U << level)) {
					FoldToggleNode(line, &action, &fToggled);
					if (level == maxLevel) {
						line = SciCall_GetLastChild(line);
					}
				}
			}
			++line;
		}
		break;
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
			const BOOL fHeader = (lv & SC_FOLDLEVELHEADERFLAG) != 0;
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
