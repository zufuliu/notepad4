// Scintilla source code edit control
/** @file PerLine.h
 ** Manages data associated with each line of the document
 **/
// Copyright 1998-2009 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef PERLINE_H
#define PERLINE_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

/**
 * This holds the marker identifier and the marker type to display.
 * MarkerHandleNumbers are members of lists.
 */
struct MarkerHandleNumber {
	int handle;
	int number;
	MarkerHandleNumber *next;
};

/**
 * A marker handle set contains any number of MarkerHandleNumbers.
 */
class MarkerHandleSet {
	MarkerHandleNumber *root;

public:
	MarkerHandleSet();
	~MarkerHandleSet();
	int Length() const;
	int MarkValue() const;	///< Bit set of marker numbers.
	bool Contains(int handle) const;
	bool InsertHandle(int handle, int markerNum);
	void RemoveHandle(int handle);
	bool RemoveNumber(int markerNum, bool all);
	void CombineWith(MarkerHandleSet *other);
};

class LineMarkers : public PerLine {
	SplitVector<MarkerHandleSet *> markers;
	/// Handles are allocated sequentially and should never have to be reused as 32 bit ints are very big.
	int handleCurrent;
public:
	LineMarkers() : handleCurrent(0) {
	}
	virtual ~LineMarkers();
	virtual void Init();
	virtual void InsertLine(Sci::Line line);
	virtual void RemoveLine(Sci::Line line);

	int MarkValue(Sci::Line line);
	Sci::Line MarkerNext(Sci::Line lineStart, int mask) const;
	int AddMark(Sci::Line line, int markerNum, Sci::Line lines);
	void MergeMarkers(Sci::Line line);
	bool DeleteMark(Sci::Line line, int markerNum, bool all);
	void DeleteMarkFromHandle(int markerHandle);
	Sci::Line LineFromHandle(int markerHandle);
};

class LineLevels : public PerLine {
	SplitVector<int> levels;
public:
	virtual ~LineLevels();
	virtual void Init();
	virtual void InsertLine(Sci::Line line);
	virtual void RemoveLine(Sci::Line line);

	void ExpandLevels(Sci::Line sizeNew=-1);
	void ClearLevels();
	int SetLevel(Sci::Line line, int level, Sci::Line lines);
	int GetLevel(Sci::Line line) const;
};

class LineState : public PerLine {
	SplitVector<int> lineStates;
public:
	LineState() {
	}
	virtual ~LineState();
	virtual void Init();
	virtual void InsertLine(Sci::Line line);
	virtual void RemoveLine(Sci::Line line);

	int SetLineState(Sci::Line line, int state);
	int GetLineState(Sci::Line line);
	Sci::Line GetMaxLineState() const;
};

class LineAnnotation : public PerLine {
	SplitVector<char *> annotations;
public:
	LineAnnotation() {
	}
	virtual ~LineAnnotation();
	virtual void Init();
	virtual void InsertLine(Sci::Line line);
	virtual void RemoveLine(Sci::Line line);

	bool MultipleStyles(Sci::Line line) const;
	int Style(Sci::Line line) const;
	const char *Text(Sci::Line line) const;
	const unsigned char *Styles(Sci::Line line) const;
	void SetText(Sci::Line line, const char *text);
	void ClearAll();
	void SetStyle(Sci::Line line, int style);
	void SetStyles(Sci::Line line, const unsigned char *styles);
	int Length(Sci::Line line) const;
	int Lines(Sci::Line line) const;
};

typedef std::vector<int> TabstopList;

class LineTabstops : public PerLine {
	SplitVector<TabstopList *> tabstops;
public:
	LineTabstops() {
	}
	virtual ~LineTabstops();
	virtual void Init();
	virtual void InsertLine(Sci::Line line);
	virtual void RemoveLine(Sci::Line line);

	bool ClearTabstops(Sci::Line line);
	bool AddTabstop(Sci::Line line, int x);
	int GetNextTabstop(Sci::Line line, int x) const;
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
