// This file is part of Notepad4.
// See License.txt for details about distribution and modification.

namespace {

struct ToolbarCustomization {
	HWND hwnd;
	const TBBUTTON *buttons;
	UINT count;
};

ToolbarCustomization customization;

bool UseSystemDialogColors() noexcept {
	HIGHCONTRAST contrast{sizeof(HIGHCONTRAST)};
	return !SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0)
		|| (contrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

bool ReadDialogRowText(const DRAWITEMSTRUCT &item, bool combo, LPWSTR text, int capacity) noexcept {
	const LRESULT length = SendMessage(item.hwndItem, combo ? CB_GETLBTEXTLEN : LB_GETTEXTLEN, item.itemID, 0);
	return length > 0 && length < capacity
		&& SendMessage(item.hwndItem, combo ? CB_GETLBTEXT : LB_GETTEXT, item.itemID, AsInteger<LPARAM>(text)) == length;
}

bool PaintDialogRow(const DRAWITEMSTRUCT &item, LPCWSTR text, HIMAGELIST images = nullptr,
	int image = -1, COLORREF swatch = CLR_INVALID) noexcept {
	const UINT dpi = GetWindowDPI(item.hwndItem);
	const int padding = MulDiv(4, dpi, USER_DEFAULT_SCREEN_DPI);
	const int height = item.rcItem.bottom - item.rcItem.top;
	int iconWidth = 0;
	int iconHeight = 0;
	if (images) {
		if (height <= 0 || !ImageList_GetIconSize(images, &iconWidth, &iconHeight)
			|| image < 0 || image >= ImageList_GetImageCount(images)) {
			return false;
		}
		if (iconHeight > height) {
			iconWidth = max(1, MulDiv(iconWidth, height, iconHeight));
			iconHeight = height;
		}
	}

	HDC dc = item.hDC;
	const int saved = SaveDC(dc);
	if (!saved) {
		return false;
	}
	const RECT &row = item.rcItem;
	IntersectClipRect(dc, row.left, row.top, row.right, row.bottom);
	const bool selected = (item.itemState & ODS_SELECTED) != 0;
	const bool disabled = (item.itemState & ODS_DISABLED) != 0 || !IsWindowEnabled(item.hwndItem);
	FillRect(dc, &row, selected ? GetSysColorBrush(COLOR_HIGHLIGHT) : GetDarkBrush(DarkBrush_CtrlBackgroundBrush));
	const COLORREF color = selected ? GetSysColor(COLOR_HIGHLIGHTTEXT)
		: disabled ? DarkColor_DisabledTextColor : DarkColor_TextColor;
	SetTextColor(dc, color);
	SetBkMode(dc, TRANSPARENT);
	if (const auto font = SendMessage(item.hwndItem, WM_GETFONT, 0, 0)) {
		SelectObject(dc, AsPointer<HFONT>(font));
	}

	const DWORD exStyle = GetWindowExStyle(item.hwndItem);
	const bool physicalRight = (exStyle & (WS_EX_RIGHT | WS_EX_LAYOUTRTL)) != 0;
	const bool rightAligned = physicalRight != ((GetLayout(dc) & LAYOUT_RTL) != 0);
	RECT label{row.left + padding, row.top, row.right - padding, row.bottom};
	const UINT drawFlags = DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS
		| ((exStyle & WS_EX_RTLREADING) ? DT_RTLREADING : 0);
	if (item.CtlType == ODT_LISTBOX || swatch != CLR_INVALID) {
		const int width = images ? iconWidth : MulDiv(swatch != CLR_INVALID ? 28 : 18, dpi, USER_DEFAULT_SCREEN_DPI);
		RECT icon = label;
		if (rightAligned) {
			icon.left = icon.right - width;
			label.right = icon.left - padding;
		} else {
			icon.right = icon.left + width;
			label.left = icon.right + padding;
		}
		if (images) {
			ImageList_DrawEx(images, image, dc, icon.left, row.top + (height - iconHeight)/2,
				iconWidth, iconHeight, CLR_NONE, CLR_NONE, ILD_TRANSPARENT | ILD_SCALE);
		} else if (swatch != CLR_INVALID) {
			InflateRect(&icon, 0, MulDiv(-2, dpi, USER_DEFAULT_SCREEN_DPI));
			SetDCBrushColor(dc, swatch);
			FillRect(dc, &icon, GetStockBrush(DC_BRUSH));
			FrameRect(dc, &icon, GetDarkBrush(DarkBrush_EdgeBrush));
		}
	}
	DrawText(dc, text, -1, &label, drawFlags | (rightAligned ? DT_RIGHT : DT_LEFT));
	if ((item.itemState & (ODS_FOCUS | ODS_NOFOCUSRECT)) == ODS_FOCUS) {
		DrawFocusRect(dc, &row);
	}
	RestoreDC(dc, saved);
	return true;
}

bool DrawToolbarCustomizationRow(HWND dialog, const DRAWITEMSTRUCT &item) noexcept {
	constexpr DWORD required = LBS_OWNERDRAWFIXED | LBS_HASSTRINGS;
	if (!customization.hwnd || item.CtlType != ODT_LISTBOX || item.itemID == UINT(-1)
		|| GetParent(item.hwndItem) != dialog || (GetWindowStyle(item.hwndItem) & required) != required) {
		return false;
	}
	WCHAR text[256];
	if (!ReadDialogRowText(item, false, text, COUNTOF(text))) {
		return false;
	}
	const TBBUTTON *match = nullptr;
	for (UINT index = 0; index < customization.count; ++index) {
		const auto &button = customization.buttons[index];
		WCHAR label[128];
		if (button.idCommand && GetString(button.idCommand, label, COUNTOF(label)) && lstrcmp(text, label) == 0) {
			if (match) {
				return PaintDialogRow(item, text);
			}
			match = &button;
		}
	}
	if (!match) {
		// System-generated rows such as separators have a label but no toolbar image.
		return PaintDialogRow(item, text);
	}

	int image = match->iBitmap;
	TBBUTTONINFO info{sizeof(TBBUTTONINFO)};
	info.dwMask = TBIF_IMAGE;
	if (SendMessage(customization.hwnd, TB_GETBUTTONINFO, match->idCommand, AsInteger<LPARAM>(&info)) >= 0) {
		image = info.iImage;
	}
	const auto images = AsPointer<HIMAGELIST>(SendMessage(customization.hwnd, TB_GETIMAGELIST, 0, 0));
	return (images && PaintDialogRow(item, text, images, image))
		|| PaintDialogRow(item, text);
}

}

void DarkMode_OnToolbarBeginAdjust(HWND toolbar, const TBBUTTON *buttons, UINT count) noexcept {
	customization = {toolbar, buttons, count};
	DialogHook_Start(DialogRefData_CustomizeToolbar);
}

void DarkMode_OnToolbarEndAdjust() noexcept {
	customization = {};
	DialogHook_Stop();
}
