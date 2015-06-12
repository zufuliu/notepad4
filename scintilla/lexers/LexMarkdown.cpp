// Lexer for Markdown

#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "CharacterSet.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "LexerModule.h"

static void ColouriseMarkdownDoc(unsigned int startPos, int length, int initStyle, WordList *keywordLists[], Accessor &styler) {
	int state = initStyle;
	int chPrev, ch = 0, chNext = styler[startPos];
	styler.StartAt(startPos);
	styler.StartSegment(startPos);
	unsigned int endPos = startPos + length;
	if (endPos == (unsigned)styler.Length())
		++endPos;

	int lineCurrent = styler.GetLine(startPos);
	char buf[MAX_WORD_LENGTH + 1] = {0};
	int wordLen = 0;

	for (unsigned int i = startPos; i < endPos; i++) {
		chPrev = ch;
		ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);

		const bool atEOL = (ch == '\r' && chNext != '\n') || (ch == '\n');
		const bool atLineStart = i == (unsigned)styler.LineStart(lineCurrent);
		if (atEOL || i == endPos-1) {
			lineCurrent++;
		}

		switch (state) {
		}

		if (state == SCE_C_DEFAULT) {
		}
	}

	// Colourise remaining document
	styler.ColourTo(endPos - 1, state);
}

LexerModule lmMarkdown(SCLEX_MARKDOWN, ColouriseMarkdownDoc, "md", 0);
