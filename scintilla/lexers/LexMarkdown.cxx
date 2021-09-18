// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for Markdown

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
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

#if 0
using namespace Lexilla;

static void ColouriseMarkdownDoc(Sci_PositionU startPos, Sci_Position length, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int state = initStyle;
	int chPrev, ch = 0, chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	const Sci_PositionU endPos = startPos + length;

	Sci_Line lineCurrent = styler.GetLine(startPos);
	char buf[MAX_WORD_LENGTH + 1] = {0};
	int wordLen = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		chPrev = ch;
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineStart = i == (Sci_PositionU)styler.LineStart(lineCurrent);
		if (atEOL || i == endPos-1) {
			lineCurrent++;
		}

		switch (state) {
		}

		if (state == SCE_MARKDOWN_DEFAULT) {
		}
	}

	// Colourise remaining document
	styler.ColorTo(endPos, state);
}

LexerModule lmMarkdown(SCLEX_MARKDOWN, ColouriseMarkdownDoc, "md", 0);
#endif
