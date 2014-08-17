// Scintilla source code edit control
/** @file MarginView.h
 ** Defines the appearance of the editor margin.
 **/
// Copyright 1998-2014 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef MARGINVIEW_H
#define MARGINVIEW_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

void DrawWrapMarker(Surface *surface, const PRectangle &rcPlace, bool isEndMarker, const ColourDesired &wrapColour);

/**
* MarginView draws the margins.
*/
class MarginView {
public:
	Surface *pixmapSelMargin;
	Surface *pixmapSelPattern;
	Surface *pixmapSelPatternOffset1;
	// Highlight current folding block
	HighlightDelimiter highlightDelimiter;

	MarginView();

	void DropGraphics(bool freeObjects);
	void AllocateGraphics(const ViewStyle &vsDraw);
	void RefreshPixMaps(Surface *surfaceWindow, WindowID wid, const ViewStyle &vsDraw);
	void PaintMargin(Surface *surface, int topLine, const PRectangle &rc, const PRectangle &rcMargin,
		const EditModel &model, const ViewStyle &vs);
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
