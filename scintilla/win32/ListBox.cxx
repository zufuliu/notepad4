// Scintilla source code edit control
/** @file ListBox.cxx
 ** Implementation of list box on Windows.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cmath>
#include <climits>

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <optional>
#include <algorithm>
#include <iterator>
#include <memory>
//#include <mutex>

#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>
#include <shlwapi.h>

#include "ScintillaTypes.h"
#include "ILexer.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"
#include "VectorISA.h"
#include "GraphicUtils.h"
#include "UniqueString.h"
#include "XPM.h"
#include "UniConversion.h"

#include "WinTypes.h"
#include "PlatWin.h"
#include "SurfaceD2D.h"

using namespace Scintilla;
using namespace Scintilla::Internal;

namespace {

// These are reasonable initial guesses that may be refined by measurement or calls.
constexpr SIZE sizeList { 150, 80 };
constexpr int commonLineHeight = 10;
constexpr unsigned int commonCharacterWidth = 8;
constexpr int commonItemLength = 12;
constexpr int defaultVisibleRows = 9;

struct ListItemData {
	const char *text;
	unsigned int len;
	int pixId;
};

class LineToItem {
	std::unique_ptr<char[]> words;
	std::vector<ListItemData> data;

public:
	void Clear() noexcept {
		words.reset();
		data.clear();
	}

	[[nodiscard]] ListItemData Get(size_t index) const noexcept {
		if (index < data.size()) {
			return data[index];
		}
		ListItemData missing = { "", 0, -1 };
		return missing;
	}
	[[nodiscard]] int Count() const noexcept {
		return static_cast<int>(data.size());
	}

	void AllocItem(const char *text, unsigned int len, int pixId) {
		const ListItemData lid = { text, len, pixId };
		data.push_back(lid);
	}

	char *SetWords(const char *s, size_t length) {
		words = UniqueCopy(s, length + 1);
		return words.get();
	}
};

inline ColourRGBA ColourOfElement(std::optional<ColourRGBA> colour, UINT nIndex) noexcept {
	if (colour.has_value()) {
		return colour.value().Opaque();
	}
	return ColourFromSys(nIndex);
}

struct LBGraphics {
	GDIBitMap bm;
	std::unique_ptr<Surface> pixmapLine;
	DCRenderTarget pBMDCTarget;

	void Release() noexcept {
		pixmapLine.reset();
		pBMDCTarget = nullptr;
		bm.Release();
	}
};

#define LISTBOXX_USE_FAKE_FRAME		0
#define LISTBOXX_USE_WIDEST_ITEM	1

constexpr const WCHAR *ListBoxX_ClassName = L"ListBoxX";
#if LISTBOXX_USE_FAKE_FRAME
constexpr int ListBoxXFakeFrameSize = 4;
#endif

}

class ListBoxX final : public ListBox {
	int lineHeight = commonLineHeight;
	HFONT fontCopy {};
	std::shared_ptr<FontWin> fontWin;
	Technology technology = Technology::Default;
	RGBAImageSet images;
	LineToItem lti;
	HWND lb {};
	int codePage = 0;
	int desiredVisibleRows = defaultVisibleRows;
	unsigned int maxItemCharacters = 0;
	unsigned int aveCharWidth = commonCharacterWidth;
	ColourRGBA colorText;
	ColourRGBA colorBackground;
	ColourRGBA colorHighlightText;
	ColourRGBA colorHighlightBack;
	Window *parent = nullptr;
	int ctrlID = 0;
	UINT dpi = USER_DEFAULT_SCREEN_DPI;
	IListBoxDelegate *delegate = nullptr;
#if LISTBOXX_USE_WIDEST_ITEM
	const char *widestItem = nullptr;
#endif
	unsigned int maxCharWidth = 1;
	WPARAM resizeHit = 0;
	PRectangle rcPreSize;
	Point dragOffset;
	Point location;	// Caret location at which the list is opened
	MouseWheelDelta wheelDelta;
	DWORD frameStyle = WS_THICKFRAME;

	LBGraphics graphics;

	HWND GetHWND() const noexcept;
	void AppendListItem(const char *text, const char *numword, unsigned int len);
	void AdjustWindowRect(PRectangle *rc) const noexcept;
	int ItemHeight() const noexcept;
	int MinClientWidth() const noexcept;
	int TextOffset() const noexcept;
	POINT GetClientExtent() const noexcept;
	POINT MinTrackSize() const noexcept;
	POINT MaxTrackSize() const noexcept;
	void SetRedraw(bool on) noexcept;
	void OnDoubleClick();
	void OnSelChange();
	void ResizeToCursor();
	void StartResize(WPARAM) noexcept;
	LRESULT NcHitTest(WPARAM, LPARAM) const noexcept;
	void CentreItem(int n) noexcept;
	void AllocateBitMap();
	static LRESULT CALLBACK ControlWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	static constexpr POINT ItemInset {0, 0};	// Padding around whole item
	static constexpr POINT TextInset {2, 0};	// Padding around text
	static constexpr POINT ImageInset {1, 0};	// Padding around image

public:
	ListBoxX() noexcept = default;
	ListBoxX(const ListBoxX &) = delete;
	ListBoxX(ListBoxX &&) = delete;
	ListBoxX &operator=(const ListBoxX &) = delete;
	ListBoxX &operator=(ListBoxX &&) = delete;
	~ListBoxX() noexcept override {
		if (fontCopy) {
			::DeleteObject(fontCopy);
		}
		graphics.Release();
	}
	void SetFont(std::shared_ptr<Font> font) noexcept override;
	void SCICALL Create(Window &parent_, int ctrlID_, Point location_, int lineHeight_, int codePage_, Technology technology_) noexcept override;
	void SetAverageCharWidth(int width) noexcept override;
	void SetVisibleRows(int rows) noexcept override;
	int GetVisibleRows() const noexcept override;
	PRectangle GetDesiredRect() override;
	int CaretFromEdge() const noexcept override;
	void Clear() noexcept override;
	void Append(const char *s, int type = -1) const noexcept override;
	int Length() const noexcept override;
	void Select(int n) override;
	int GetSelection() const noexcept override;
	int Find(const char *prefix) const noexcept override;
	std::string GetValue(int n) const override;
	void RegisterImage(int type, const char *xpm_data) override;
	void RegisterRGBAImage(int type, int width, int height, const unsigned char *pixelsImage) override;
	void ClearRegisteredImages() noexcept override;
	void SetDelegate(IListBoxDelegate *lbDelegate) noexcept override;
	void SetList(const char *list, char separator, char typesep) override;
	void SCICALL SetOptions(const ListOptions &options_) noexcept override;
	void Draw(const DRAWITEMSTRUCT *pDrawItem);
	LRESULT WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
};

std::unique_ptr<ListBox> ListBox::Allocate() {
	return std::make_unique<ListBoxX>();
}

void ListBoxX::Create(Window &parent_, int ctrlID_, Point location_, int lineHeight_, int codePage_, Technology technology_) noexcept {
	parent = &parent_;
	ctrlID = ctrlID_;
	location = location_;
	lineHeight = lineHeight_;
	codePage = codePage_;
	technology = technology_;
	HWND hwndParent = HwndFromWindow(*parent);
	HINSTANCE hinstanceParent = GetWindowInstance(hwndParent);
	// Window created as popup so not clipped within parent client area
	constexpr int startPosition = 100;	// Arbitrary as will be moved immediately
	wid = ::CreateWindowEx(
		WS_EX_WINDOWEDGE, ListBoxX_ClassName, L"",
		WS_POPUP | frameStyle,
		startPosition, startPosition, sizeList.cx, sizeList.cy, hwndParent,
		{},
		hinstanceParent,
		this);

	dpi = GetWindowDPI(hwndParent);
	POINT locationw = POINTFromPointEx(location);
	::MapWindowPoints(hwndParent, {}, &locationw, 1);
	location = PointFromPOINTEx(locationw);
}

void ListBoxX::SetFont(std::shared_ptr<Font> font) noexcept {
#if USE_RTTI
	fontWin = std::dynamic_pointer_cast<FontWin>(font);
#else
	fontWin = std::static_pointer_cast<FontWin>(font);
#endif
	const FontWin *pfm = fontWin.get();
	if (pfm) {
		if (fontCopy) {
			::DeleteObject(fontCopy);
		}
		fontCopy = pfm->HFont();
		SetWindowFont(lb, fontCopy, FALSE);
		graphics.Release();
	}
}

void ListBoxX::SetAverageCharWidth(int width) noexcept {
	aveCharWidth = width;
}

void ListBoxX::SetVisibleRows(int rows) noexcept {
	desiredVisibleRows = rows;
}

int ListBoxX::GetVisibleRows() const noexcept {
	return desiredVisibleRows;
}

HWND ListBoxX::GetHWND() const noexcept {
	return HwndFromWindowID(GetID());
}

PRectangle ListBoxX::GetDesiredRect() {
	PRectangle rcDesired = GetPosition();

	int rows = Length();
	if ((rows == 0) || (rows > desiredVisibleRows))
		rows = desiredVisibleRows;
	rcDesired.bottom = rcDesired.top + ItemHeight() * rows;

	int width = MinClientWidth();

	int textSize = 0;

	// Make a measuring surface
	const std::unique_ptr<Surface> surfaceItem(Surface::Allocate(technology));
	surfaceItem->Init(GetID());
	surfaceItem->SetMode(SurfaceMode(codePage/*, false*/));

	// Find the widest item in pixels
#if LISTBOXX_USE_WIDEST_ITEM
	if (widestItem) {
		textSize = static_cast<int>(std::ceil(
			surfaceItem->WidthText(fontWin.get(), std::string_view(widestItem, maxItemCharacters))));
	}
#else
	const int items = lti.Count();
	for (int i = 0; i < items; i++) {
		const ListItemData item = lti.Get(i);
		const int itemTextSize = static_cast<int>(std::ceil(
			surfaceItem->WidthText(fontWin.get(), item.text)));
		textSize = std::max(textSize, itemTextSize);
	}
#endif

	maxCharWidth = static_cast<int>(std::ceil(surfaceItem->WidthText(fontWin.get(), "W")));
	const int averageCharWidth = static_cast<int>(surfaceItem->AverageCharWidth(fontWin.get()));

	width = std::max(width, textSize);
	width = std::max<int>(width, (maxItemCharacters + 1) * averageCharWidth);

	rcDesired.right = rcDesired.left + TextOffset() + width + (TextInset.x * 2);
	if (Length() > rows) {
		rcDesired.right += SystemMetricsForDpi(SM_CXVSCROLL, dpi);
	}

	AdjustWindowRect(&rcDesired);
	return rcDesired;
}

int ListBoxX::TextOffset() const noexcept {
	const int pixWidth = images.GetWidth();
	return pixWidth == 0 ? ItemInset.x : ItemInset.x + pixWidth + (ImageInset.x * 2);
}

int ListBoxX::CaretFromEdge() const noexcept {
	PRectangle rc;
	AdjustWindowRect(&rc);
	return TextOffset() + static_cast<int>(TextInset.x + (0 - rc.left) - 1);
}

void ListBoxX::Clear() noexcept {
	ListBox_ResetContent(lb);
	maxItemCharacters = 0;
#if LISTBOXX_USE_WIDEST_ITEM
	widestItem = nullptr;
#endif
	lti.Clear();
}

void ListBoxX::Append(const char *, int) const noexcept {
	// This method is no longer called in Scintilla
	PLATFORM_ASSERT(false);
}

int ListBoxX::Length() const noexcept {
	return lti.Count();
}

void ListBoxX::Select(int n) {
	// We are going to scroll to centre on the new selection and then select it, so disable
	// redraw to avoid flicker caused by a painting new selection twice in unselected and then
	// selected states
	SetRedraw(false);
	CentreItem(n);
	ListBox_SetCurSel(lb, n);
	OnSelChange();
	SetRedraw(true);
}

int ListBoxX::GetSelection() const noexcept {
	return ListBox_GetCurSel(lb);
}

// This is not actually called at present
int ListBoxX::Find(const char *) const noexcept {
	return LB_ERR;
}

std::string ListBoxX::GetValue(int n) const {
	const ListItemData item = lti.Get(n);
	return std::string(item.text, item.len);
}

void ListBoxX::RegisterImage(int type, const char *xpm_data) {
	const XPM xpmImage(xpm_data);
	images.AddImage(type, std::make_unique<RGBAImage>(xpmImage));
}

void ListBoxX::RegisterRGBAImage(int type, int width, int height, const unsigned char *pixelsImage) {
	images.AddImage(type, std::make_unique<RGBAImage>(width, height, 1.0f, pixelsImage));
}

void ListBoxX::ClearRegisteredImages() noexcept {
	images.Clear();
}

void ListBoxX::Draw(const DRAWITEMSTRUCT *pDrawItem) {
	if ((pDrawItem->itemAction != ODA_SELECT) && (pDrawItem->itemAction != ODA_DRAWENTIRE)) {
		return;
	}

	if (!graphics.pixmapLine) {
		AllocateBitMap();
		if (!graphics.pixmapLine) {
			// Failed to allocate, so release fully and give up
			graphics.Release();
			return;
		}
	}
	if (graphics.pBMDCTarget) {
		graphics.pBMDCTarget->BeginDraw();
	}

	const PRectangle rcItemBase = PRectangleFromRectEx(pDrawItem->rcItem);
	const PRectangle rcItem(0, 0, rcItemBase.Width(), rcItemBase.Height());
	PRectangle rcBox = rcItem;
	rcBox.left += TextOffset();
	ColourRGBA colourFore;
	ColourRGBA colourBack;
	if (pDrawItem->itemState & ODS_SELECTED) {
		PRectangle rcImage = rcItem;
		rcImage.right = rcBox.left;
		// The image is not highlighted
		graphics.pixmapLine->FillRectangle(rcImage, colorBackground);
		colourBack = colorHighlightBack;
		graphics.pixmapLine->FillRectangle(rcBox, colourBack);
		colourFore = colorHighlightText;
	} else {
		colourBack = colorBackground;
		graphics.pixmapLine->FillRectangle(rcItem, colourBack);
		colourFore = colorText;
	}

	const ListItemData item = lti.Get(pDrawItem->itemID);
	const int pixId = item.pixId;
	const char *text = item.text;
	const unsigned int len = item.len;

	const PRectangle rcText = rcBox.Inset(Point(TextInset.x, TextInset.y));

	const double ascent = graphics.pixmapLine->Ascent(fontWin.get());
	graphics.pixmapLine->DrawTextClipped(rcText, fontWin.get(), rcText.top + ascent, std::string_view(text, len), colourFore, colourBack);

	// Draw the image, if any
	const RGBAImage *pimage = images.Get(pixId);
	if (pimage) {
		const XYPOSITION left = rcItem.left + ItemInset.x + ImageInset.x;
		PRectangle rcImage = rcItem;
		rcImage.left = left;
		rcImage.right = rcImage.left + images.GetWidth();
		graphics.pixmapLine->DrawRGBAImage(rcImage,
			pimage->GetWidth(), pimage->GetHeight(), pimage->Pixels());
	}

	if (graphics.pBMDCTarget) {
		const HRESULT hrEnd = graphics.pBMDCTarget->EndDraw();
		if (FAILED(hrEnd)) {
			return;
		}
	}

	// Blit from hMemDC to hDC
	const SIZE extent = SizeOfRect(pDrawItem->rcItem);
	::BitBlt(pDrawItem->hDC, pDrawItem->rcItem.left, pDrawItem->rcItem.top, extent.cx, extent.cy, graphics.bm.DC(), 0, 0, SRCCOPY);
}

void ListBoxX::AppendListItem(const char *text, const char *numword, unsigned int len) {
	int pixId = -1;
	if (numword) {
		pixId = 0;
		char ch;
		while ((ch = *++numword) != '\0') {
			pixId = 10 * pixId + (ch - '0');
		}
	}

	lti.AllocItem(text, len, pixId);
	if (maxItemCharacters < len) {
		maxItemCharacters = len;
#if LISTBOXX_USE_WIDEST_ITEM
		widestItem = text;
#endif
	}
}

void ListBoxX::SetDelegate(IListBoxDelegate *lbDelegate) noexcept {
	delegate = lbDelegate;
}

void ListBoxX::SetList(const char *list, const char separator, const char typesep) {
	// Turn off redraw while populating the list - this has a significant effect, even if
	// the listbox is not visible.
	SetRedraw(false);
	Clear();
	const size_t size = strlen(list);
	char *words = lti.SetWords(list, size);
	const char *startword = words;
	char *numword = nullptr;
	char * const end = words + size;
	for (; words < end; words++) {
		if (words[0] == separator) {
			words[0] = '\0';
			const char *endword = words;
			if (numword) {
				endword = numword;
				*numword = '\0';
			}
			AppendListItem(startword, numword, static_cast<unsigned int>(endword - startword));
			startword = words + 1;
			numword = nullptr;
		} else if (words[0] == typesep) {
			numword = words;
		}
	}
	if (startword) {
		const char *endword = end;
		if (numword) {
			endword = numword;
			*numword = '\0';
		}
		AppendListItem(startword, numword, static_cast<unsigned int>(endword - startword));
	}

	// Finally populate the listbox itself with the correct number of items
	const int count = lti.Count();
	::SendMessage(lb, LB_INITSTORAGE, count, 0);
	for (intptr_t j = 0; j < count; j++) {
		ListBox_AddItemData(lb, j + 1);
	}
	SetRedraw(true);
}

void ListBoxX::SetOptions(const ListOptions &options_) noexcept {
	colorText = ColourOfElement(options_.fore, COLOR_WINDOWTEXT);
	colorBackground = ColourOfElement(options_.back, COLOR_WINDOW);
	colorHighlightText = ColourOfElement(options_.foreSelected, COLOR_HIGHLIGHTTEXT);
	colorHighlightBack = ColourOfElement(options_.backSelected, COLOR_HIGHLIGHT);
	frameStyle = FlagSet(options_.options, AutoCompleteOption::FixedSize) ? WS_BORDER : WS_THICKFRAME;
}

void ListBoxX::AdjustWindowRect(PRectangle *rc) const noexcept {
	RECT rcw = RectFromPRectangleEx(*rc);
	AdjustWindowRectForDpi(&rcw, frameStyle, WS_EX_WINDOWEDGE, dpi);
	*rc = PRectangleFromRectEx(rcw);
#if LISTBOXX_USE_FAKE_FRAME
	*rc = rc->Inflate(ListBoxXFakeFrameSize, ListBoxXFakeFrameSize);
#endif
}

int ListBoxX::ItemHeight() const noexcept {
	const int itemHeight = lineHeight + (TextInset.y * 2);
	const int pixHeight = images.GetHeight() + (ImageInset.y * 2);
	return std::max(itemHeight, pixHeight);
}

int ListBoxX::MinClientWidth() const noexcept {
	return commonItemLength * (aveCharWidth + aveCharWidth / 3);
}

POINT ListBoxX::MinTrackSize() const noexcept {
	PRectangle rc = PRectangle::FromInts(0, 0, MinClientWidth(), ItemHeight());
	AdjustWindowRect(&rc);
	POINT ret = { static_cast<LONG>(rc.Width()), static_cast<LONG>(rc.Height()) };
	return ret;
}

POINT ListBoxX::MaxTrackSize() const noexcept {
	const int width = (maxCharWidth * maxItemCharacters) + (TextInset.x * 2) +
		TextOffset() + SystemMetricsForDpi(SM_CXVSCROLL, dpi);
	PRectangle rc = PRectangle::FromInts(0, 0,
		std::max(MinClientWidth(), width),
		ItemHeight() * lti.Count());
	AdjustWindowRect(&rc);
	POINT ret = { static_cast<LONG>(rc.Width()), static_cast<LONG>(rc.Height()) };
	return ret;
}

void ListBoxX::SetRedraw(bool on) noexcept {
	::SendMessage(lb, WM_SETREDRAW, on, 0);
	if (on) {
		::RedrawWindow(lb, {}, {}, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
	}
}

void ListBoxX::ResizeToCursor() {
	PRectangle rc = GetPosition();
	POINT ptw;
	::GetCursorPos(&ptw);
	const Point pt = PointFromPOINT(ptw) + dragOffset;

	switch (resizeHit) {
	case HTLEFT:
		rc.left = pt.x;
		break;
	case HTRIGHT:
		rc.right = pt.x;
		break;
	case HTTOP:
		rc.top = pt.y;
		break;
	case HTTOPLEFT:
		rc.top = pt.y;
		rc.left = pt.x;
		break;
	case HTTOPRIGHT:
		rc.top = pt.y;
		rc.right = pt.x;
		break;
	case HTBOTTOM:
		rc.bottom = pt.y;
		break;
	case HTBOTTOMLEFT:
		rc.bottom = pt.y;
		rc.left = pt.x;
		break;
	case HTBOTTOMRIGHT:
		rc.bottom = pt.y;
		rc.right = pt.x;
		break;
	default:
		break;
	}

	const POINT ptMin = MinTrackSize();
	const POINT ptMax = MaxTrackSize();
	// We don't allow the left edge to move at present, but just in case
	rc.left = Clamp(rc.left, rcPreSize.right - ptMax.x, rcPreSize.right - ptMin.x);
	rc.top = Clamp(rc.top, rcPreSize.bottom - ptMax.y, rcPreSize.bottom - ptMin.y);
	rc.right = Clamp(rc.right, rcPreSize.left + ptMin.x, rcPreSize.left + ptMax.x);
	rc.bottom = Clamp(rc.bottom, rcPreSize.top + ptMin.y, rcPreSize.top + ptMax.y);

	SetPosition(rc);
}

void ListBoxX::StartResize(WPARAM hitCode) noexcept {
	rcPreSize = GetPosition();
	POINT cursorPos;
	::GetCursorPos(&cursorPos);

	switch (hitCode) {
	case HTRIGHT:
	case HTBOTTOM:
	case HTBOTTOMRIGHT:
		dragOffset.x = rcPreSize.right - cursorPos.x;
		dragOffset.y = rcPreSize.bottom - cursorPos.y;
		break;

	case HTTOPRIGHT:
		dragOffset.x = rcPreSize.right - cursorPos.x;
		dragOffset.y = rcPreSize.top - cursorPos.y;
		break;

		// Note that the current hit test code prevents the left edge cases ever firing
		// as we don't want the left edge to be movable
	case HTLEFT:
	case HTTOP:
	case HTTOPLEFT:
		dragOffset.x = rcPreSize.left - cursorPos.x;
		dragOffset.y = rcPreSize.top - cursorPos.y;
		break;
	case HTBOTTOMLEFT:
		dragOffset.x = rcPreSize.left - cursorPos.x;
		dragOffset.y = rcPreSize.bottom - cursorPos.y;
		break;

	default:
		return;
	}

	::SetCapture(GetHWND());
	resizeHit = hitCode;
}

LRESULT ListBoxX::NcHitTest(WPARAM wParam, LPARAM lParam) const noexcept {
	const PRectangle rc = GetPosition();

	LRESULT hit = ::DefWindowProc(GetHWND(), WM_NCHITTEST, wParam, lParam);
	// There is an apparent bug in the DefWindowProc hit test code whereby it will
	// return HTTOPXXX if the window in question is shorter than the default
	// window caption height + frame, even if one is hovering over the bottom edge of
	// the frame, so workaround that here
	if (hit >= HTTOP && hit <= HTTOPRIGHT) {
		const int minHeight = SystemMetricsForDpi(SM_CYMINTRACK, dpi);
		const int yPos = GET_Y_LPARAM(lParam);
		if ((rc.Height() < minHeight) && (yPos > ((rc.top + rc.bottom) / 2))) {
			hit += HTBOTTOM - HTTOP;
		}
	}
	else if ((hit < HTSIZEFIRST || hit > HTSIZELAST) && (frameStyle == WS_BORDER)) {
		const int cx = SystemMetricsForDpi(SM_CXVSCROLL, dpi);
#if !LISTBOXX_USE_FAKE_FRAME
		const PRectangle rcInner = rc.Deflate(SystemMetricsForDpi(SM_CXBORDER, dpi), SystemMetricsForDpi(SM_CYBORDER, dpi));
#else
		const PRectangle rcInner = rc.Deflate(ListBoxXFakeFrameSize, ListBoxXFakeFrameSize);
#endif
		const int xPos = GET_X_LPARAM(lParam);
		const int yPos = GET_Y_LPARAM(lParam);
		/*
		HTTOPLEFT    13 | HTTOP    12 | HTTOPRIGHT    14         4 | 3 | 5
		HTLEFT       10 | HTNOWHERE 0 | HTRIGHT       11  -9 =>  1 | 0 | 2
		HTBOTTOMLEFT 16 | HTBOTTOM 15 | HTBOTTOMRIGHT 17         7 | 6 | 8
		*/
		const int x = (xPos <= rcInner.left) ? 1 : ((xPos >= rcInner.right - cx) ? 2 : 0);
		int y = (yPos <= rcInner.top) ? 3 : ((yPos >= rcInner.bottom) ? 6 : 0);
		if (y == 0 && x == 2) {
			if (location.y < rc.top) {
				y = (yPos >= rcInner.bottom - cx) ? 6 : 0;
			} else {
				y = (yPos <= rcInner.top + cx) ? 3 : 0;
			}
		}
		const int h = x + y;
		hit = h ? (9 + h) : HTERROR;
	}

	// Never permit resizing that moves the left edge. Allow movement of top or bottom edge
	// depending on whether the list is above or below the caret
	switch (hit) {
	case HTLEFT:
	case HTTOPLEFT:
	case HTBOTTOMLEFT:
		hit = HTERROR;
		break;

	case HTTOP:
	case HTTOPRIGHT: {
		// Valid only if caret below list
		if (location.y < rc.top)
			hit = HTERROR;
	}
	break;

	case HTBOTTOM:
	case HTBOTTOMRIGHT: {
		// Valid only if caret above list
		if (rc.bottom <= location.y)
			hit = HTERROR;
	}
	break;

	default:
		break;
	}

	return hit;
}

void ListBoxX::OnDoubleClick() {
	if (delegate) {
		ListBoxEvent event(ListBoxEvent::EventType::doubleClick);
		delegate->ListNotify(&event);
	}
}

void ListBoxX::OnSelChange() {
	if (delegate) {
		ListBoxEvent event(ListBoxEvent::EventType::selectionChange);
		delegate->ListNotify(&event);
	}
}

POINT ListBoxX::GetClientExtent() const noexcept {
	RECT rc;
	::GetWindowRect(HwndFromWindowID(wid), &rc);
	POINT ret{ rc.right - rc.left, rc.bottom - rc.top };
	return ret;
}

void ListBoxX::CentreItem(int n) noexcept {
	// If below mid point, scroll up to centre, but with more items below if uneven
	if (n >= 0) {
		const POINT extent = GetClientExtent();
		const int visible = extent.y / ItemHeight();
		if (visible < Length()) {
			const int top = ListBox_GetTopIndex(lb);
			const int half = (visible - 1) / 2;
			if (n > (top + half))
				ListBox_SetTopIndex(lb, n - half);
		}
	}
}

void ListBoxX::AllocateBitMap() {
	const SIZE extent { GetClientExtent().x, ItemHeight() };

	graphics.bm.Create({}, extent.cx, -extent.cy, nullptr);
	if (!graphics.bm) {
		return;
	}

	// Make a surface
	graphics.pixmapLine = Surface::Allocate(technology);
	graphics.pixmapLine->SetMode(SurfaceMode(codePage/*, false*/));

	if (technology != Technology::Default) {
		// if (!LoadD2D()) {
		// 	return;
		// }

		const D2D1_RENDER_TARGET_PROPERTIES drtp = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED });

		HRESULT hr = CreateDCRenderTarget(&drtp, graphics.pBMDCTarget);
		if (FAILED(hr) || !graphics.pBMDCTarget) {
			return;
		}

		const RECT rcExtent = { 0, 0, extent.cx, extent.cy };
		hr = graphics.pBMDCTarget->BindDC(graphics.bm.DC(), &rcExtent);
		if (SUCCEEDED(hr)) {
			graphics.pixmapLine->Init(graphics.pBMDCTarget.Get(), GetID());
		}
		return;
	}

	// Either technology == Technology::Default or USE_D2D turned off
	graphics.pixmapLine->Init(graphics.bm.DC(), GetID());
}

LRESULT CALLBACK ListBoxX::ControlWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR /*dwRefData*/) {
	try {
		ListBoxX * const lbx = PointerFromWindow<ListBoxX *>(::GetParent(hWnd));
		switch (iMessage) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_MOUSEACTIVATE:
			// This prevents the view activating when the scrollbar is clicked
			return MA_NOACTIVATE;

		case WM_LBUTTONDOWN: {
			// We must take control of selection to prevent the ListBox activating
			// the popup
			const LRESULT lResult = ::SendMessage(hWnd, LB_ITEMFROMPOINT, 0, lParam);
			if (HIWORD(lResult) == 0) {
				ListBox_SetCurSel(hWnd, LOWORD(lResult));
				if (lbx) {
					lbx->OnSelChange();
				}
			}
		}
		return 0;

		case WM_LBUTTONUP:
			return 0;

		case WM_LBUTTONDBLCLK: {
			if (lbx) {
				lbx->OnDoubleClick();
			}
		}
		return 0;

		case WM_MBUTTONDOWN:
			// disable the scroll wheel button click action
			return 0;

		case WM_NCDESTROY:
			RemoveWindowSubclass(hWnd, ControlWndProc, uIdSubclass);
			break;
		}
	} catch (...) {
	}
	return ::DefSubclassProc(hWnd, iMessage, wParam, lParam);
}

LRESULT ListBoxX::WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_CREATE: {
		HINSTANCE hinstanceParent = GetWindowInstance(HwndFromWindow(*parent));
		// Note that LBS_NOINTEGRALHEIGHT is specified to fix cosmetic issue when resizing the list
		// but has useful side effect of speeding up list population significantly
		lb = ::CreateWindowEx(
			0, WC_LISTBOXW, L"",
			WS_CHILD | WS_VSCROLL | WS_VISIBLE |
			LBS_OWNERDRAWFIXED | LBS_NODATA | LBS_NOINTEGRALHEIGHT,
			0, 0, sizeList.cx, sizeList.cy, hWnd,
			AsPointer<HMENU>(static_cast<ptrdiff_t>(ctrlID)),
			hinstanceParent,
			nullptr);
		::SetWindowSubclass(lb, ControlWndProc, 0, 0);
	}
	break;

	case WM_SIZE:
		if (lb) {
			graphics.Release();	// Bitmap must be reallocated to new size.
			SetRedraw(false);
			::SetWindowPos(lb, {}, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
			// Ensure the selection remains visible
			CentreItem(GetSelection());
			SetRedraw(true);
		}
		break;

	case WM_PAINT: {
		const Painter painter(hWnd);
	}
	break;

	case WM_COMMAND:
		// This is not actually needed now - the registered double click action is used
		// directly to action a choice from the list.
		::SendMessage(HwndFromWindow(*parent), iMessage, wParam, lParam);
		break;

	case WM_MEASUREITEM: {
		MEASUREITEMSTRUCT *pMeasureItem = AsPointer<MEASUREITEMSTRUCT *>(lParam);
		pMeasureItem->itemHeight = ItemHeight();
	}
	break;

	case WM_DRAWITEM:
		Draw(AsPointer<DRAWITEMSTRUCT *>(lParam));
		break;

	case WM_DESTROY:
		lb = {};
		SetWindowPointer(hWnd, nullptr);
		return ::DefWindowProc(hWnd, iMessage, wParam, lParam);

	case WM_ERASEBKGND:
		// To reduce flicker we can elide background erasure since this window is
		// completely covered by its child.
		return TRUE;

	case WM_GETMINMAXINFO: {
		MINMAXINFO *minMax = AsPointer<MINMAXINFO*>(lParam);
		minMax->ptMaxTrackSize = MaxTrackSize();
		minMax->ptMinTrackSize = MinTrackSize();
	}
	break;

	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;

#if LISTBOXX_USE_FAKE_FRAME
	case WM_NCPAINT: {
		HDC hDC = ::GetWindowDC(hWnd);
		RECT rect;
		::GetClientRect(hWnd, &rect);

		// outer frame
		rect.right += 2*ListBoxXFakeFrameSize;
		rect.bottom += 2*ListBoxXFakeFrameSize;
		const int width = rect.right - rect.left;
		const int height = rect.bottom - rect.top;

		// inner border
		RECT client = rect;
		::InflateRect(&client, -ListBoxXFakeFrameSize + 1, -ListBoxXFakeFrameSize + 1);

		HDC hMemDC = CreateCompatibleDC(hDC);
		const BITMAPINFO bpih = { {sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB, 0, 0, 0, 0, 0}, {{0, 0, 0, 0}} };
		HBITMAP hbmMem = CreateDIBSection(hMemDC, &bpih, DIB_RGB_COLORS, nullptr, nullptr, 0);

		if (hbmMem) {
			HBITMAP hbmOld = SelectBitmap(hMemDC, hbmMem);
			BLENDFUNCTION merge = { AC_SRC_OVER, 0, 0, AC_SRC_ALPHA };

			GdiAlphaBlend(hDC, rect.left, rect.top, width, height, hMemDC, 0, 0, width, height, merge);

			SelectBitmap(hMemDC, hbmOld);
			::DeleteObject(hbmMem);
		}
		::DeleteDC(hMemDC);

		//HPEN hPen = ::CreatePen(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOWFRAME));
		HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		HPEN hPenOld = SelectPen(hDC, hPen);
		::Rectangle(hDC, client.left, client.top, client.right, client.bottom);
		::SelectObject(hDC, hPenOld);
		::DeleteObject(hPen);

		::ReleaseDC(hWnd, hDC);
		return 0;
	}

	case WM_NCCALCSIZE: {
		LPRECT rect = AsPointer<LPRECT>(lParam);
		::InflateRect(rect, -ListBoxXFakeFrameSize, -ListBoxXFakeFrameSize);
		return 0;
	}
#endif

	case WM_NCHITTEST:
		return NcHitTest(wParam, lParam);

	case WM_NCLBUTTONDOWN:
		// We have to implement our own window resizing because the DefWindowProc
		// implementation insists on activating the resized window
		StartResize(wParam);
		return 0;

	case WM_MOUSEMOVE: {
		if (resizeHit == 0) {
			return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
		}
		ResizeToCursor();
	}
	break;

	case WM_LBUTTONUP:
	case WM_CANCELMODE:
		if (resizeHit != 0) {
			resizeHit = 0;
			::ReleaseCapture();
		}
		return ::DefWindowProc(hWnd, iMessage, wParam, lParam);

	case WM_MOUSEWHEEL:
		if (wheelDelta.Accumulate(wParam)) {
			const int nRows = GetVisibleRows();
			int linesToScroll = std::clamp(nRows - 1, 1, 3);
			linesToScroll *= wheelDelta.Actions();
			const int top = std::max(0, ListBox_GetTopIndex(lb) + linesToScroll);
			ListBox_SetTopIndex(lb, top);
		}
		break;

	default:
		return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK ListBoxX::StaticWndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	if (iMessage == WM_CREATE) {
		const CREATESTRUCT *pCreate = AsPointer<CREATESTRUCT *>(lParam);
		SetWindowPointer(hWnd, pCreate->lpCreateParams);
	}
	// Find C++ object associated with window.
	if (ListBoxX *lbx = PointerFromWindow<ListBoxX *>(hWnd)) {
		return lbx->WndProc(hWnd, iMessage, wParam, lParam);
	}
	return ::DefWindowProc(hWnd, iMessage, wParam, lParam);
}

namespace Scintilla::Internal {

bool ListBoxX_Register() noexcept {
	WNDCLASSEX wndclassc {};
	wndclassc.cbSize = sizeof(wndclassc);
	// We need CS_HREDRAW and CS_VREDRAW because of the ellipsis that might be drawn for
	// truncated items in the list and the appearance/disappearance of the vertical scroll bar.
	// The list repaint is double-buffered to avoid the flicker this would otherwise cause.
	wndclassc.style = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
	wndclassc.cbWndExtra = sizeof(LONG_PTR);
	wndclassc.hInstance = hinstPlatformRes;
	wndclassc.lpfnWndProc = ListBoxX::StaticWndProc;
	wndclassc.hCursor = ::LoadCursor({}, IDC_ARROW);
	wndclassc.lpszClassName = ListBoxX_ClassName;

	return ::RegisterClassEx(&wndclassc) != 0;
}

void ListBoxX_Unregister() noexcept {
	if (hinstPlatformRes) {
		::UnregisterClass(ListBoxX_ClassName, hinstPlatformRes);
	}
}

}
