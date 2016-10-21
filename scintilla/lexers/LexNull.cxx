// Scintilla source code edit control
/** @file LexOthers.cxx
 ** Lexers for batch files, diff results, properties files, make files and error lists.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

static void ColouriseNullDoc(Sci_PositionU startPos, Sci_Position length, int, WordList *[], Accessor &styler) {
	// Null language means all style bytes are 0 so just mark the end - no need to fill in.
	if (length > 0) {
		styler.StartAt(startPos + length - 1);
		styler.StartSegment(startPos + length - 1);
		styler.ColourTo(startPos + length - 1, 0);
	}
}

static void FoldNullDoc(Sci_PositionU startPos, Sci_Position length, int, WordList *[], Accessor &styler) {
	if (styler.GetPropertyInt("fold") == 0)
		return;
	const Sci_Position maxPos = startPos + length;
	const Sci_Position maxLines = (maxPos == styler.Length()) ? styler.GetLine(maxPos) : styler.GetLine(maxPos - 1);	// Requested last line
	const Sci_Position docLines = styler.GetLine(styler.Length());	// Available last line

	const bool foldCompact = styler.GetPropertyInt("fold.compact") != 0;

	// Backtrack to previous non-blank line so we can determine indent level
	// for any white space lines
	// and so we can fix any preceding fold level (which is why we go back
	// at least one line in all cases)
	int spaceFlags = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int indentCurrent = Accessor::LexIndentAmount(styler, lineCurrent, &spaceFlags, NULL);
	while (lineCurrent > 0) {
		lineCurrent--;
		indentCurrent = Accessor::LexIndentAmount(styler, lineCurrent, &spaceFlags, NULL);
		if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG))
			break;
	}
	int indentCurrentLevel = indentCurrent & SC_FOLDLEVELNUMBERMASK;

	// Set up initial loop state
	startPos = styler.LineStart(lineCurrent);

	// Process all characters to end of requested range
	// Cap processing in all cases
	// to end of document (in case of unclosed quote at end).
	while ((lineCurrent <= docLines) && ((lineCurrent <= maxLines))) {
		// Gather info
		int lev = indentCurrent;
		int lineNext = lineCurrent + 1;
		int indentNext = indentCurrent;
		if (lineNext <= docLines) {
			// Information about next line is only available if not at end of document
			indentNext = Accessor::LexIndentAmount(styler, lineNext, &spaceFlags, NULL);
		}
		indentCurrentLevel = indentCurrent & SC_FOLDLEVELNUMBERMASK;
		if (indentNext & SC_FOLDLEVELWHITEFLAG)
			indentNext = SC_FOLDLEVELWHITEFLAG | indentCurrentLevel;

		// Skip past any blank lines for next indent level info
		while ((lineNext < docLines) && (indentNext & SC_FOLDLEVELWHITEFLAG)) {
			lineNext++;
			indentNext = Accessor::LexIndentAmount(styler, lineNext, &spaceFlags, NULL);
		}

		const int levelAfterBlank = indentNext & SC_FOLDLEVELNUMBERMASK;
		const int levelBeforeBlank = Maximum(indentCurrentLevel, levelAfterBlank);

		// Now set all the indent levels on the lines we skipped
		// Do this from end to start. Once we encounter one line
		// which is indented more than the line after the end of
		// the blank-block, use the level of the block before

		int skipLine = lineNext;
		int skipLevel = levelAfterBlank;

		while (--skipLine > lineCurrent) {
			int skipLineIndent = Accessor::LexIndentAmount(styler, skipLine, &spaceFlags, NULL);

			if (foldCompact) {
				if ((skipLineIndent & SC_FOLDLEVELNUMBERMASK) > levelAfterBlank)
					skipLevel = levelBeforeBlank;

				int whiteFlag = skipLineIndent & SC_FOLDLEVELWHITEFLAG;
				styler.SetLevel(skipLine, skipLevel | whiteFlag);
			} else {
				if ((skipLineIndent & SC_FOLDLEVELNUMBERMASK) > levelAfterBlank &&
					!(skipLineIndent & SC_FOLDLEVELWHITEFLAG))
					skipLevel = levelBeforeBlank;

				styler.SetLevel(skipLine, skipLevel);
			}
		}

		// Set fold header
		if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG)) {
			if ((indentCurrent & SC_FOLDLEVELNUMBERMASK) < (indentNext & SC_FOLDLEVELNUMBERMASK))
				lev |= SC_FOLDLEVELHEADERFLAG;
		}

		// Set fold level for this line and move to next line
		styler.SetLevel(lineCurrent, foldCompact ? lev : lev & ~SC_FOLDLEVELWHITEFLAG);
		indentCurrent = indentNext;
		lineCurrent = lineNext;
	}

	// NOTE: Cannot set level of last line here because indentCurrent doesn't have
	// header flag set; the loop above is crafted to take care of this case!
	//styler.SetLevel(lineCurrent, indentCurrent);
}

LexerModule lmNull(SCLEX_NULL, ColouriseNullDoc, "null", FoldNullDoc);
