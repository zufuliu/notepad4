// Scintilla source code edit control
/** @file LexNull.cxx
 ** Lexer for no language. Used for plain text and unrecognized files.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cassert>
#include <cstring>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Scintilla;

namespace {

void ColouriseNullDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int, LexerWordList, Accessor &styler) {
	// Null language means all style bytes are 0 so just mark the end - no need to fill in.
#if 0
	styler.StartAt(startPos + lengthDoc);
#else
	if (lengthDoc > 0) {
		styler.StartAt(startPos + lengthDoc - 1);
		styler.StartSegment(startPos + lengthDoc - 1);
		styler.ColourTo(startPos + lengthDoc - 1, 0);
	}
#endif
}

// code folding based on Python
void FoldNullDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /* initStyle */, LexerWordList, Accessor &styler) {
	const Sci_Position maxPos = startPos + lengthDoc;
	const Sci_Position docLines = styler.GetLine(styler.Length());	// Available last line
	const Sci_Position maxLines = (maxPos == styler.Length()) ? docLines : styler.GetLine(maxPos - 1);	// Requested last line

	const bool foldCompact = styler.GetPropertyInt("fold.compact") != 0;

	// Backtrack to previous non-blank line so we can determine indent level
	// for any white space lines
	// and so we can fix any preceding fold level (which is why we go back
	// at least one line in all cases)
	int spaceFlags = 0;
	Sci_Position lineCurrent = styler.GetLine(startPos);
	int indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, nullptr);
	while (lineCurrent > 0) {
		lineCurrent--;
		indentCurrent = styler.IndentAmount(lineCurrent, &spaceFlags, nullptr);
		if (!(indentCurrent & SC_FOLDLEVELWHITEFLAG))
			break;
	}

	// Process all characters to end of requested range
	// Cap processing in all cases
	// to end of document (in case of unclosed quote at end).
	while (lineCurrent <= maxLines) {
		// Gather info
		int lev = indentCurrent;
		Sci_Position lineNext = lineCurrent + 1;
		int indentNext = indentCurrent;
		if (lineNext <= docLines) {
			// Information about next line is only available if not at end of document
			indentNext = styler.IndentAmount(lineNext, &spaceFlags, nullptr);
		}
		const int indentCurrentLevel = indentCurrent & SC_FOLDLEVELNUMBERMASK;
		if (indentNext & SC_FOLDLEVELWHITEFLAG)
			indentNext = SC_FOLDLEVELWHITEFLAG | indentCurrentLevel;

		// Skip past any blank lines for next indent level info
		while ((lineNext < docLines) && (indentNext & SC_FOLDLEVELWHITEFLAG)) {
			lineNext++;
			indentNext = styler.IndentAmount(lineNext, &spaceFlags, nullptr);
		}

		const int levelAfterBlank = indentNext & SC_FOLDLEVELNUMBERMASK;
		const int levelBeforeBlank = (indentCurrentLevel > levelAfterBlank) ? indentCurrentLevel : levelAfterBlank;

		// Now set all the indent levels on the lines we skipped
		// Do this from end to start. Once we encounter one line
		// which is indented more than the line after the end of
		// the blank-block, use the level of the block before

		Sci_Position skipLine = lineNext;
		int skipLevel = levelAfterBlank;

		while (--skipLine > lineCurrent) {
			const int skipLineIndent = styler.IndentAmount(skipLine, &spaceFlags, nullptr);

			if (foldCompact) {
				if ((skipLineIndent & SC_FOLDLEVELNUMBERMASK) > levelAfterBlank)
					skipLevel = levelBeforeBlank;

				const int whiteFlag = skipLineIndent & SC_FOLDLEVELWHITEFLAG;
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

}

LexerModule lmNull(SCLEX_NULL, ColouriseNullDoc, "null", FoldNullDoc);
