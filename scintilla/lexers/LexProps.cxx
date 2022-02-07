// Scintilla source code edit control
/** @file LexProps.cxx
 ** Lexers for properties files.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cassert>
#include <cstring>

#include <string>
#include <string_view>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Lexilla;

namespace {

#define ENABLE_FOLD_PROPS_COMMENT	1

constexpr bool IsCommentChar(unsigned char ch) noexcept {
	return ch == '#' || ch == ';' || ch == '!';
}

constexpr bool IsAssignChar(unsigned char ch) noexcept {
	return ch == '=' || ch == ':';
}

void ColourisePropsDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	// property lexer.props.allow.initial.spaces
	//	For properties files, set to 0 to style all lines that start with whitespace in the default style.
	//	This is not suitable for SciTE .properties files which use indentation for flow control but
	//	can be used for RFC2822 text where indentation is used for continuation lines.
	const bool allowInitialSpaces = styler.GetPropertyBool("lexer.props.allow.initial.spaces", true);

	const Sci_Position endPos = startPos + lengthDoc;
	const Sci_Line maxLines = styler.GetLine((endPos == styler.Length()) ? endPos : endPos - 1);

	styler.StartAt(startPos);
	styler.StartSegment(startPos);

	Sci_Line lineCurrent = styler.GetLine(startPos);
	Sci_PositionU lineStartCurrent = styler.LineStart(lineCurrent);
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);

#if !ENABLE_FOLD_PROPS_COMMENT
	const bool fold = styler.GetPropertyBool("fold");
	int prevLevel = (lineCurrent > 0) ? styler.LevelAt(lineCurrent - 1) : SC_FOLDLEVELBASE;
#endif

	while (lineCurrent <= maxLines) {
		const Sci_PositionU lineEndPos = lineStartNext - 1;
		Sci_PositionU i = lineStartCurrent;
		unsigned char ch = styler[i];
		if (allowInitialSpaces) {
			while (i < lineEndPos && isspacechar(ch)) {
				ch = styler[++i];
			}
		}

		initStyle = SCE_PROPS_DEFAULT;
#if ENABLE_FOLD_PROPS_COMMENT
		bool changed = false;
#endif
		if (IsCommentChar(ch)) {
			initStyle = SCE_PROPS_COMMENT;
		} else if (ch == '[') {
			initStyle = SCE_PROPS_SECTION;
		} else if (ch == '@') {
			++i;
			styler.ColorTo(i, SCE_PROPS_DEFVAL);
			const char chNext = styler[i];
			if (IsAssignChar(chNext)) {
				styler.ColorTo(i + 1, SCE_PROPS_ASSIGNMENT);
			}
		} else if (allowInitialSpaces || !isspacechar(ch)) {
			while (i < lineStartNext) {
				ch = styler[i];
				if (IsAssignChar(ch)) {
					styler.ColorTo(i, SCE_PROPS_KEY);
					++i;
					styler.ColorTo(i, SCE_PROPS_ASSIGNMENT);
					break;
				}
				// ignore trail byte in DBCS character
				i += styler.IsLeadByte(ch) ? 2 : 1;
			}

			unsigned char chPrev = ch;
			while (i < lineStartNext) {
				ch = styler[i];
				if (IsCommentChar(ch) && IsASpaceOrTab(chPrev))	{
					styler.ColorTo(i, SCE_PROPS_DEFAULT);
					initStyle = SCE_PROPS_COMMENT;
#if ENABLE_FOLD_PROPS_COMMENT
					changed = true;
#endif
					break;
				}
				chPrev = ch;
				// ignore trail byte in DBCS character
				i += styler.IsLeadByte(ch) ? 2 : 1;
			}
		}

		styler.ColorTo(lineStartNext, initStyle);
#if ENABLE_FOLD_PROPS_COMMENT
		styler.SetLineState(lineCurrent, changed ? SCE_PROPS_DEFAULT : initStyle);
#else
		if (fold) {
			int nextLevel;
			if (initStyle == SCE_PROPS_SECTION) {
				nextLevel = SC_FOLDLEVELBASE | SC_FOLDLEVELHEADERFLAG;
			} else if (prevLevel & SC_FOLDLEVELHEADERFLAG) {
				nextLevel = (prevLevel & SC_FOLDLEVELNUMBERMASK) + 1;
			} else {
				nextLevel = prevLevel;
			}
			if (nextLevel != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, nextLevel);
			}
			prevLevel = nextLevel;
		}
#endif
		lineStartCurrent = lineStartNext;
		lineCurrent++;
		lineStartNext = styler.LineStart(lineCurrent + 1);
	}
}

#if ENABLE_FOLD_PROPS_COMMENT
void FoldPropsDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList, Accessor &styler) {
	const Sci_Position endPos = startPos + lengthDoc;
	const Sci_Line maxLines = styler.GetLine((endPos == styler.Length()) ? endPos : endPos - 1);

	Sci_Line lineCurrent = styler.GetLine(startPos);

	int prevLevel = SC_FOLDLEVELBASE;
	bool prevComment = false;
	bool prev2Comment = false;
	if (lineCurrent > 0) {
		prevLevel = styler.LevelAt(lineCurrent - 1);
		prevComment = styler.GetLineState(lineCurrent - 1) == SCE_PROPS_COMMENT;
		prev2Comment = lineCurrent > 1 && styler.GetLineState(lineCurrent - 2) == SCE_PROPS_COMMENT;
	}

	bool commentHead = prevComment && (prevLevel & SC_FOLDLEVELHEADERFLAG);
	while (lineCurrent <= maxLines) {
		int nextLevel;
		const int initStyle = styler.GetLineState(lineCurrent);

		const bool currentComment = initStyle == SCE_PROPS_COMMENT;
		if (currentComment) {
			commentHead = !prevComment;
			if (prevLevel & SC_FOLDLEVELHEADERFLAG) {
				nextLevel = (prevLevel & SC_FOLDLEVELNUMBERMASK) + 1;
			} else {
				nextLevel = prevLevel;
			}
			nextLevel |= commentHead ? SC_FOLDLEVELHEADERFLAG : 0;
		} else {
			if (initStyle == SCE_PROPS_SECTION) {
				nextLevel = SC_FOLDLEVELBASE | SC_FOLDLEVELHEADERFLAG;
			} else {
				if (commentHead) {
					nextLevel = prevLevel & SC_FOLDLEVELNUMBERMASK;
				} else if (prevLevel & SC_FOLDLEVELHEADERFLAG) {
					nextLevel = (prevLevel & SC_FOLDLEVELNUMBERMASK) + 1;
				} else if (prevComment && prev2Comment) {
					nextLevel = prevLevel - 1;
				} else {
					nextLevel = prevLevel;
				}
			}

			if (commentHead) {
				commentHead = false;
				styler.SetLevel(lineCurrent - 1, prevLevel & SC_FOLDLEVELNUMBERMASK);
			}
		}

		if (nextLevel != styler.LevelAt(lineCurrent)) {
			styler.SetLevel(lineCurrent, nextLevel);
		}

		prevLevel = nextLevel;
		prev2Comment = prevComment;
		prevComment = currentComment;
		lineCurrent++;
	}
}
#endif

}

#if ENABLE_FOLD_PROPS_COMMENT
LexerModule lmProps(SCLEX_PROPERTIES, ColourisePropsDoc, "props", FoldPropsDoc);
#else
LexerModule lmProps(SCLEX_PROPERTIES, ColourisePropsDoc, "props");
#endif
