// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
/*
dark mode implementation based on following library and article:
https://github.com/ysc3839/win32-darkmode
https://github.com/ozone10/win32-darkmodelib
UAHMenuBar	https://github.com/adzm/win32-custom-menubar-aero-theme

https://www.zabkat.com/blog/darkmode-listview.htm
https://www.zabkat.com/blog/bpic/dark_themes.txt
https://github.com/nptr/msstyleEditor
*/

#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
// #include "config.h"
#include "Sci_Position.h"
#include "Helpers.h"
#include "DarkMode.h"
#include "Notepad4.h"
#include "Dialogs.h"
#include "Styles.h"

extern HWND hwndMain;
extern HWND hwndEdit;
extern HTHEME hThemeMenu;
UINT g_DarkModeStatus = DarkModeStatus_None;

namespace { // API

enum class IMMERSIVE_HC_CACHE_MODE {
	IHCM_USE_CACHED_VALUE,
	IHCM_REFRESH
};

// 1903 18362
enum class PreferredAppMode {
	Default,
	AllowDark,
	ForceDark,
	ForceLight,
	Max
};

enum WINDOWCOMPOSITIONATTRIB {
	WCA_UNDEFINED = 0,
	WCA_NCRENDERING_ENABLED = 1,
	WCA_NCRENDERING_POLICY = 2,
	WCA_TRANSITIONS_FORCEDISABLED = 3,
	WCA_ALLOW_NCPAINT = 4,
	WCA_CAPTION_BUTTON_BOUNDS = 5,
	WCA_NONCLIENT_RTL_LAYOUT = 6,
	WCA_FORCE_ICONIC_REPRESENTATION = 7,
	WCA_EXTENDED_FRAME_BOUNDS = 8,
	WCA_HAS_ICONIC_BITMAP = 9,
	WCA_THEME_ATTRIBUTES = 10,
	WCA_NCRENDERING_EXILED = 11,
	WCA_NCADORNMENTINFO = 12,
	WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
	WCA_VIDEO_OVERLAY_ACTIVE = 14,
	WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
	WCA_DISALLOW_PEEK = 16,
	WCA_CLOAK = 17,
	WCA_CLOAKED = 18,
	WCA_ACCENT_POLICY = 19,
	WCA_FREEZE_REPRESENTATION = 20,
	WCA_EVER_UNCLOAKED = 21,
	WCA_VISUAL_OWNER = 22,
	WCA_HOLOGRAPHIC = 23,
	WCA_EXCLUDED_FROM_DDA = 24,
	WCA_PASSIVEUPDATEMODE = 25,
	WCA_USEDARKMODECOLORS = 26,
	WCA_LAST = 27
};

struct WINDOWCOMPOSITIONATTRIBDATA {
	WINDOWCOMPOSITIONATTRIB Attrib;
	PVOID pvData;
	SIZE_T cbData;
};

// constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
// using DwmSetWindowAttributeSig = HRESULT (WINAPI *)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute) noexcept;
using SetWindowCompositionAttributeSig = BOOL (WINAPI *)(HWND hwnd, WINDOWCOMPOSITIONATTRIBDATA*) noexcept;
// 1809 17763
using AllowDarkModeForWindowSig = BOOL (WINAPI *)(HWND hwnd, BOOL allow) noexcept; // ordinal 133
using FlushMenuThemesSig = void (WINAPI *)() noexcept; // ordinal 136
using RefreshImmersiveColorPolicyStateSig = void (WINAPI *)() noexcept; // ordinal 104
using IsDarkModeAllowedForWindowSig = BOOL (WINAPI *)(HWND hwnd) noexcept; // ordinal 137
using GetIsImmersiveColorUsingHighContrastSig = BOOL (WINAPI *)(IMMERSIVE_HC_CACHE_MODE mode) noexcept; // ordinal 106
// 1903 18362
using SetPreferredAppModeSig = PreferredAppMode (WINAPI *)(PreferredAppMode appMode) noexcept; // ordinal 135, in 1903

// DwmSetWindowAttributeSig fnDwmSetWindowAttribute = nullptr;
SetWindowCompositionAttributeSig fnSetWindowCompositionAttribute = nullptr;
AllowDarkModeForWindowSig fnAllowDarkModeForWindow = nullptr;
SetPreferredAppModeSig fnSetPreferredAppMode = nullptr;
FlushMenuThemesSig fnFlushMenuThemes = nullptr;
RefreshImmersiveColorPolicyStateSig fnRefreshImmersiveColorPolicyState = nullptr;
IsDarkModeAllowedForWindowSig fnIsDarkModeAllowedForWindow = nullptr;
GetIsImmersiveColorUsingHighContrastSig fnGetIsImmersiveColorUsingHighContrast = nullptr;

constexpr DWORD win10Build1809 = 17763;
constexpr DWORD win10Build1903 = 18362;
// constexpr DWORD win10Build2004 = 19041;
constexpr DWORD win11Build21H2 = 22000;
constexpr DWORD win11Build24H2 = 26100;
constexpr DWORD win11Build25H2 = 26200;

constexpr bool CheckBuildNumber(DWORD buildNumber) noexcept {
	return buildNumber >= win10Build1809;
}

LPCWSTR getDarkModeThemeName() noexcept {
	return DarkMode_SupportDarkTheme() ? L"DarkMode_DarkTheme" : L"DarkMode_Explorer";
}

bool IsColorSchemeChangeMessage(LPARAM lParam) noexcept {
	return lParam == SPI_SETHIGHCONTRAST || (!IS_INTRESOURCE(lParam) && StrCaseEqual(AsPointer<LPCWSTR>(lParam), L"ImmersiveColorSet"));
}

bool DarkMode_AppsUseDarkMode() noexcept {
	if (DarkMode_IsSupported()) {
		// fnShouldAppsUseDarkMode (ordinal 132) is not reliable on 1903+?
		DWORD size = sizeof(DWORD);
		DWORD mode = FALSE;
		const auto status = RegGetValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &mode, &size);
		if (status == ERROR_SUCCESS && mode == FALSE) {
			HIGHCONTRASTW highContrast{};
			highContrast.cbSize = sizeof(highContrast);
			if (SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE)) {
				return (highContrast.dwFlags & HCF_HIGHCONTRASTON) == 0;
			}
			return true;
		}
	}
	return false;
}

}

namespace { // Color

enum {
	DarkColor_BackgroundColor = RGB(0x2D, 0x2D, 0x2D),
	DarkColor_CtrlBackgroundColor = RGB(0x38, 0x38, 0x38),
	DarkColor_HotBackgroundColor = RGB(0x45, 0x45, 0x45),
	DarkColor_DlgBackgroundColor = RGB(0x25, 0x25, 0x26),
	DarkColor_ErrorBackgroundColor = DarkColor_BackgroundColor,

	DarkColor_EdgeColor = RGB(0x55, 0x55, 0x55),
	DarkColor_HotEdgeColor = RGB(0x9B, 0x9B, 0x9B),
	DarkColor_DisabledEdgeColor = RGB(0x48, 0x48, 0x48),
	DarkColor_HighlightColor = RGB(0x60, 0xCD, 0xFF),

	DarkColor_TextColor = RGB(0xD4, 0xD4, 0xD4),
	DarkColor_DarkerTextColor = RGB(0xC0, 0xC0, 0xC0),
	DarkColor_DisabledTextColor = RGB(0x80, 0x80, 0x80),
	DarkColor_LinkTextColor = RGB(0x60, 0xCD, 0xFF),

	DarkColor_ViewTextColor = DarkColor_TextColor,
	DarkColor_ViewBackgroundColor = RGB(0x1E, 0x1E, 0x1E),
	DarkColor_GridLinesColor = RGB(0x3C, 0x3C, 0x3C),
	DarkColor_HeaderDividerColor = RGB(0x64, 0x64, 0x64),

	DarkColor_HeaderTextColor = DarkColor_TextColor,
	DarkColor_HeaderBackgroundColor = DarkColor_BackgroundColor,
	DarkColor_HeaderHotBackgroundColor = DarkColor_HotBackgroundColor,
	DarkColor_HeaderEdgeColor = DarkColor_EdgeColor,

	DarkColor_WINDOWTEXT = DarkColor_TextColor,
	DarkColor_WINDOW = DarkColor_ViewBackgroundColor,
	DarkColor_BTNFACE = DarkColor_GridLinesColor,
};

enum {
	DarkBrush_BackgroundBrush,
	DarkBrush_CtrlBackgroundBrush,
	DarkBrush_HotBackgroundBrush,
	DarkBrush_DlgBackgroundBrush,
	DarkBrush_EdgeBrush,
	DarkPen_EdgePen,
	DarkPen_HotEdgePen,
	DarkBrushPen_Total,
};

HGDIOBJ darkObject[DarkBrushPen_Total]{};
void InitDarkBrushAndPen() noexcept {
	darkObject[DarkBrush_BackgroundBrush] = CreateSolidBrush(DarkColor_BackgroundColor);
	darkObject[DarkBrush_CtrlBackgroundBrush] = CreateSolidBrush(DarkColor_CtrlBackgroundColor);
	darkObject[DarkBrush_HotBackgroundBrush] = CreateSolidBrush(DarkColor_HotBackgroundColor);
	darkObject[DarkBrush_DlgBackgroundBrush] = CreateSolidBrush(DarkColor_DlgBackgroundColor);
	darkObject[DarkBrush_EdgeBrush] = CreateSolidBrush(DarkColor_EdgeColor);
	darkObject[DarkPen_EdgePen] = CreatePen(PS_SOLID, 1, DarkColor_EdgeColor);
	darkObject[DarkPen_HotEdgePen] = CreatePen(PS_SOLID, 1, DarkColor_HotEdgeColor);
}

HBRUSH GetDarkBrush(UINT index) noexcept {
	return static_cast<HBRUSH>(darkObject[index]);
}

HPEN GetDarkPen(UINT index) noexcept {
	return static_cast<HPEN>(darkObject[index]);
}

}

namespace { // Paint

enum {
	MenuBarItemTextBufferSize = 128,
	ButtonTextBufferSize = MAX_PATH,
	GroupBoxTextBufferSize = MAX_PATH,
	StatusBarItemTextBufferSize = MAX_PATH + 128, // see FileIO()
};

constexpr int kWin11CornerRoundness = 4;
constexpr wchar_t kTriangleDown = 0x23F7; // U+23F7 Black Medium Down-Pointing Triangle

[[nodiscard]] bool isRectValid(const RECT& rc) noexcept {
	return rc.right > rc.left && rc.bottom > rc.top;
}

[[nodiscard]] bool isAnimationEnabled() noexcept {
	BOOL isEnabled = FALSE;
	return SystemParametersInfo(SPI_GETCLIENTAREAANIMATION, 0, &isEnabled, FALSE) && isEnabled;
}

void paintRoundRect(HDC hdc, const RECT &rect, HPEN hPen, HBRUSH hBrush, int width, int height) noexcept {
	HBRUSH holdBrush = SelectBrush(hdc, hBrush);
	HPEN oldPen = SelectPen(hdc, hPen);
	RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, width, height);
	SelectObject(hdc, holdBrush);
	SelectBrush(hdc, oldPen);
}

void paintFrameRect(HDC hdc, const RECT &rect, HPEN hPen) noexcept {
	paintRoundRect(hdc, rect, hPen, static_cast<HBRUSH>(GetStockObject(NULL_BRUSH)), 0, 0);
}

struct BufferedPaint {
	HDC hMemDC = nullptr;
	HBITMAP hMemBmp = nullptr;
	HBITMAP hOldBmp = nullptr;
	SIZE szBuffer{};
	bool ensureBuffer(HDC hdc, const RECT &rcClient) noexcept {
		const int width = rcClient.right - rcClient.left;
		const int height = rcClient.bottom - rcClient.top;
		if (szBuffer.cx != width || szBuffer.cy != height) {
			releaseBuffer();
			hMemDC = CreateCompatibleDC(hdc);
			hMemBmp = CreateCompatibleBitmap(hdc, width, height);
			hOldBmp = static_cast<HBITMAP>(SelectObject(hMemDC, hMemBmp));
			szBuffer = { width, height };
		}
		return hMemDC != nullptr && hMemBmp != nullptr;
	}
	void releaseBuffer() const noexcept {
		if (hMemDC != nullptr) {
			SelectObject(hMemDC, hOldBmp);
			DeleteObject(hMemBmp);
			DeleteDC(hMemDC);
		}
	}
	bool BeginPaint(HWND hwnd, HDC hdc, const PAINTSTRUCT &ps, RECT &rcClient) {
		if (isRectValid(ps.rcPaint)) {
			GetClientRect(hwnd, &rcClient);
			if (ensureBuffer(hdc, rcClient)) {
				SaveDC(hMemDC);
				IntersectClipRect(hMemDC,
					ps.rcPaint.left, ps.rcPaint.top,
					ps.rcPaint.right, ps.rcPaint.bottom
				);
				return true;
			}
		}
		return false;
	}
	void EndPaint(HDC hdc, const PAINTSTRUCT &ps) const noexcept {
		RestoreDC(hMemDC, -1);
		BitBlt(hdc,
			ps.rcPaint.left, ps.rcPaint.top,
			ps.rcPaint.right - ps.rcPaint.left,
			ps.rcPaint.bottom - ps.rcPaint.top,
			hMemDC,
			ps.rcPaint.left, ps.rcPaint.top,
			SRCCOPY);
	}
};

}

namespace { // UAHMenuBar

// window messages related to menu bar drawing
#define WM_UAHDESTROYWINDOW		0x0090 // handled by DefWindowProc
// #define WM_UAHDRAWMENU		0x0091 // lParam is UAHMENU
// #define WM_UAHDRAWMENUITEM	0x0092 // lParam is UAHDRAWMENUITEM
#define WM_UAHINITMENU			0x0093 // handled by DefWindowProc
#define WM_UAHMEASUREMENUITEM	0x0094 // lParam is UAHMEASUREMENUITEM
#define WM_UAHNCPAINTMENUPOPUP	0x0095 // handled by DefWindowProc

// describes the sizes of the menu bar or menu item
union UAHMENUITEMMETRICS {
	// cx appears to be 14 / 0xE less than rcItem's width!
	// cy 0x14 seems stable, i wonder if it is 4 less than rcItem's height which is always 24 atm
	struct {
		DWORD cx;
		DWORD cy;
	} rgsizeBar[2];
	struct {
		DWORD cx;
		DWORD cy;
	} rgsizePopup[4];
};

// not really used in our case but part of the other structures
struct UAHMENUPOPUPMETRICS {
	DWORD rgcx[4];
	DWORD fUpdateMaxWidths : 2; // from kernel symbols, padded to full dword
};

// hmenu is the main window menu; hdc is the context to draw in
struct UAHMENU {
	HMENU hmenu;
	HDC hdc;
	DWORD dwFlags; // no idea what these mean, in my testing it's either 0x00000a00 or sometimes 0x00000a10
};

// menu items are always referred to by iPosition here
struct UAHMENUITEM {
	[[maybe_unused]] int iPosition; // 0-based position of menu item in menubar
	UAHMENUITEMMETRICS umim;
	UAHMENUPOPUPMETRICS umpm;
};

// the DRAWITEMSTRUCT contains the states of the menu items, as well as
// the position index of the item in the menu, which is duplicated in
// the UAHMENUITEM's iPosition as well
struct UAHDRAWMENUITEM {
	DRAWITEMSTRUCT dis; // itemID looks uninitialized
	UAHMENU um;
	UAHMENUITEM umi;
};

// the MEASUREITEMSTRUCT is intended to be filled with the size of the item
// height appears to be ignored, but width can be modified
struct UAHMEASUREMENUITEM {
	MEASUREITEMSTRUCT mis;
	UAHMENU um;
	UAHMENUITEM umi;
};

void paintMenuBar(HWND hwnd, HDC hdc) noexcept {
	// get the menubar rect
	MENUBARINFO mbi{};
	mbi.cbSize = sizeof(MENUBARINFO);
	GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi);

	RECT rcWindow{};
	GetWindowRect(hwnd, &rcWindow);

	// the rcBar is offset by the window rect
	RECT rcBar{ mbi.rcBar };
	OffsetRect(&rcBar, -rcWindow.left, -rcWindow.top);

	rcBar.top -= 1;

	FillRect(hdc, &rcBar, GetDarkBrush(DarkBrush_BackgroundBrush));
}

void paintMenuBarItems(UAHDRAWMENUITEM& UDMI, HTHEME hTheme) noexcept {
	// get the menu item string
	WCHAR buffer[MenuBarItemTextBufferSize];
	memset(buffer, 0, sizeof(int));
	MENUITEMINFO mii{};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;
	mii.dwTypeData = buffer;
	mii.cch = COUNTOF(buffer);

	GetMenuItemInfoW(UDMI.um.hmenu, static_cast<UINT>(UDMI.umi.iPosition), TRUE, &mii);

	// get the item state for drawing

	DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

	int iTextStateId = MBI_NORMAL;
	int iBackgroundStateId = MBI_NORMAL;
	if ((UDMI.dis.itemState & ODS_SELECTED) != 0) {
		// clicked
		iTextStateId = MBI_PUSHED;
		iBackgroundStateId = MBI_PUSHED;
	} else if ((UDMI.dis.itemState & ODS_HOTLIGHT) != 0) {
		// hot tracking
		iTextStateId = ((UDMI.dis.itemState & ODS_INACTIVE) != 0) ? MBI_DISABLEDHOT : MBI_HOT;
		iBackgroundStateId = MBI_HOT;
	} else if ((UDMI.dis.itemState & (ODS_GRAYED | ODS_DISABLED | ODS_INACTIVE)) != 0) {
		// disabled / grey text / inactive
		iTextStateId = MBI_DISABLED;
		iBackgroundStateId = MBI_DISABLED;
	// } else if ((UDMI.dis.itemState & ODS_DEFAULT) != 0) {
	// 	// normal display
	// 	iTextStateId = MBI_NORMAL;
	// 	iBackgroundStateId = MBI_NORMAL;
	}

	if ((UDMI.dis.itemState & ODS_NOACCEL) != 0) {
		dwFlags |= DT_HIDEPREFIX;
	}

	switch (iBackgroundStateId) {
	case MBI_NORMAL:
	case MBI_DISABLED:
		FillRect(UDMI.um.hdc, &UDMI.dis.rcItem, GetDarkBrush(DarkBrush_BackgroundBrush));
		break;

	case MBI_HOT:
	case MBI_DISABLEDHOT:
		FillRect(UDMI.um.hdc, &UDMI.dis.rcItem, GetDarkBrush(DarkBrush_HotBackgroundBrush));
		break;

	case MBI_PUSHED:
	case MBI_DISABLEDPUSHED:
		FillRect(UDMI.um.hdc, &UDMI.dis.rcItem, GetDarkBrush(DarkBrush_CtrlBackgroundBrush));
		break;

	default:
		DrawThemeBackground(hTheme, UDMI.um.hdc, MENU_BARITEM, iBackgroundStateId, &UDMI.dis.rcItem, nullptr);
		break;
	}

	DTTOPTS dttopts{};
	dttopts.dwSize = sizeof(DTTOPTS);
	dttopts.dwFlags = DTT_TEXTCOLOR;
	switch (iTextStateId) {
	case MBI_NORMAL:
	case MBI_HOT:
	case MBI_PUSHED:
		dttopts.crText = DarkColor_TextColor;
		break;

	case MBI_DISABLED:
	case MBI_DISABLEDHOT:
	case MBI_DISABLEDPUSHED:
		dttopts.crText = DarkColor_DisabledTextColor;
		break;

	default:
		break;
	}

	DrawThemeTextEx(hTheme, UDMI.um.hdc, MENU_BARITEM, iTextStateId, buffer, static_cast<int>(mii.cch), dwFlags, &UDMI.dis.rcItem, &dttopts);
}

}

LRESULT DarkMode_DrawUAHMenu(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) noexcept {
	if (DarkMode_IsActive()) {
		if (umsg == WM_UAHDRAWMENU) {
			auto* pUDM = AsPointer<UAHMENU *>(lParam);
			paintMenuBar(hwnd, pUDM->hdc);
			return 0;
		}
		if (hThemeMenu == nullptr) {
			hThemeMenu = OpenThemeData(hwnd, VSCLASS_MENU);
		}
		if (hThemeMenu) {
			auto* pUDMI = AsPointer<UAHDRAWMENUITEM *>(lParam);
			paintMenuBarItems(*pUDMI, hThemeMenu);
			return 0;
		}
	}
	return DefWindowProc(hwnd, umsg, wParam, lParam);
}

void DarkMode_DrawUAHMenuNCBottomLine(HWND hwnd) noexcept {
	if (!DarkMode_IsActive()) {
		return;
	}
	MENUBARINFO mbi{};
	mbi.cbSize = sizeof(MENUBARINFO);
	if (!GetMenuBarInfo(hwnd, OBJID_MENU, 0, &mbi)) {
		return;
	}

	RECT rcClient{};
	GetClientRect(hwnd, &rcClient);
	MapWindowPoints(hwnd, nullptr, reinterpret_cast<POINT *>(&rcClient), 2);

	RECT rcWindow{};
	GetWindowRect(hwnd, &rcWindow);

	OffsetRect(&rcClient, -rcWindow.left, -rcWindow.top);

	// the rcBar is offset by the window rect
	RECT rcAnnoyingLine{ rcClient };
	rcAnnoyingLine.bottom = rcAnnoyingLine.top;
	rcAnnoyingLine.top--;

	HDC hdc = GetWindowDC(hwnd);
	FillRect(hdc, &rcAnnoyingLine, GetDarkBrush(DarkBrush_BackgroundBrush));
	ReleaseDC(hwnd, hdc);
}

namespace { // Button, GroupBox

constexpr bool buttonNeedsSubclass(DWORD type) noexcept {
	return type == BS_GROUPBOX
		|| type == BS_CHECKBOX || type == BS_AUTOCHECKBOX
		|| type == BS_3STATE || type == BS_AUTO3STATE
		|| type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON;
}

constexpr bool isGroupBox(UINT_PTR uIdSubclass) noexcept {
	return uIdSubclass == BS_GROUPBOX;
}

void renderButton(HWND hwnd, HDC hdc, HTHEME hTheme, DWORD style, DWORD nState, int iPartId, int iStateId, COLORREF textColor) noexcept {
	bool isFontCreated = false;
	HFONT hFont = nullptr;
	LOGFONT lf;
	if (SUCCEEDED(GetThemeFont(hTheme, hdc, iPartId, iStateId, TMT_FONT, &lf))) {
		hFont = CreateFontIndirect(&lf);
		isFontCreated = hFont != nullptr;
	}
	if (!isFontCreated) {
		hFont = GetWindowFont(hwnd);
	}

	DWORD dtFlags = DT_LEFT;
	if (style & BS_MULTILINE) {
		dtFlags |= DT_WORDBREAK;
	} else {
		dtFlags |= DT_SINGLELINE;
	}
	if (style & BS_CENTER) {
		dtFlags |= DT_CENTER;
	} else if (style & BS_RIGHT) {
		dtFlags |= DT_RIGHT;
	}
	if ((style & BS_VCENTER) != 0 || (style & (BS_MULTILINE | BS_BOTTOM | BS_TOP)) == 0) {
		dtFlags |= DT_VCENTER;
	} else if (style & BS_BOTTOM) {
		dtFlags |= DT_BOTTOM;
	}

	const auto uiState = static_cast<DWORD>(SendMessage(hwnd, WM_QUERYUISTATE, 0, 0));
	// hide prefix
	if ((uiState & UISF_HIDEACCEL) != 0) {
		dtFlags |= DT_HIDEPREFIX;
	}

	// Text and box part
	WCHAR buffer[ButtonTextBufferSize];
	memset(buffer, 0, sizeof(int));
	const int bufferLen = GetWindowText(hwnd, buffer, COUNTOF(buffer));
	RECT rcClient{};
	GetClientRect(hwnd, &rcClient);

	SIZE szBox{};
	GetThemePartSize(hTheme, hdc, iPartId, iStateId, nullptr, TS_DRAW, &szBox);

	RECT rcText{};
	GetThemeBackgroundContentRect(hTheme, hdc, iPartId, iStateId, &rcClient, &rcText);

	RECT rcBackground{ rcClient };
	if (!(style & BS_MULTILINE)) {
		rcBackground.top += (rcText.bottom - rcText.top - szBox.cy) / 2;
	}
	rcBackground.bottom = rcBackground.top + szBox.cy;
	rcBackground.right = rcBackground.left + szBox.cx;
	rcText.left = rcBackground.right + 3;

	HFONT hOldFont = SelectFont(hdc, hFont);
	DrawThemeParentBackground(hwnd, hdc, &rcClient);
	DrawThemeBackground(hTheme, hdc, iPartId, iStateId, &rcBackground, nullptr); // draw box

	DTTOPTS dtto{};
	dtto.dwSize = sizeof(DTTOPTS);
	dtto.dwFlags = DTT_TEXTCOLOR;
	dtto.crText = textColor;
	DrawThemeTextEx(hTheme, hdc, iPartId, iStateId, buffer, bufferLen, dtFlags, &rcText, &dtto);

	// Focus rect
	if (((nState & BST_FOCUS) == BST_FOCUS) && ((uiState & UISF_HIDEFOCUS) == 0)) {
		dtto.dwFlags |= DTT_CALCRECT;
		DrawThemeTextEx(hTheme, hdc, iPartId, iStateId, buffer, bufferLen, dtFlags | DT_CALCRECT, &rcText, &dtto);
		const RECT rcFocus{ rcText.left - 1, rcText.top, rcText.right + 1, rcText.bottom + 1 };
		DrawFocusRect(hdc, &rcFocus);
	}

	SelectFont(hdc, hOldFont);
	if (isFontCreated) {
		DeleteObject(hFont);
	}
}

DWORD paintButton(HWND hwnd, HDC hdc, HTHEME hTheme, DWORD prevStateId) noexcept {
	const DWORD style = GetWindowStyle(hwnd);
	const BOOL isEnabled = IsWindowEnabled(hwnd);
	const auto nState = static_cast<DWORD>(SendMessage(hwnd, BM_GETSTATE, 0, 0));
	const COLORREF textColor = isEnabled ? DarkColor_TextColor : DarkColor_DisabledTextColor;

	int iPartId = 0;
	DWORD iStateId = 0;

	// `enum CHECKBOXSTATES` in vsstyle.h
	constexpr int checkedOffset = CBS_CHECKEDNORMAL - CBS_UNCHECKEDNORMAL;
	constexpr int mixedOffset = CBS_MIXEDNORMAL - CBS_UNCHECKEDNORMAL;

	switch (style & BS_TYPEMASK) {
	case BS_CHECKBOX:
	case BS_AUTOCHECKBOX:
	case BS_3STATE:
	case BS_AUTO3STATE:
		iPartId = BP_CHECKBOX;
		if (!isEnabled) {
			iStateId = CBS_UNCHECKEDDISABLED;
		} else if ((nState & BST_PUSHED) == BST_PUSHED) {
			iStateId = CBS_UNCHECKEDPRESSED;
		} else if ((nState & BST_HOT) == BST_HOT) {
			iStateId = CBS_UNCHECKEDHOT;
		} else {
			iStateId = CBS_UNCHECKEDNORMAL;
		}
		if ((nState & BST_CHECKED) == BST_CHECKED) {
			iStateId += checkedOffset;
		} else if ((nState & BST_INDETERMINATE) == BST_INDETERMINATE) {
			iStateId += mixedOffset;
		}
		break;

	case BS_RADIOBUTTON:
	case BS_AUTORADIOBUTTON:
		iPartId = BP_RADIOBUTTON;
		if (!isEnabled) {
			iStateId = RBS_UNCHECKEDDISABLED;
		} else if ((nState & BST_PUSHED) == BST_PUSHED) {
			iStateId = RBS_UNCHECKEDPRESSED;
		} else if ((nState & BST_HOT) == BST_HOT) {
			iStateId = RBS_UNCHECKEDHOT;
		} else {
			iStateId = RBS_UNCHECKEDNORMAL;
		}
		if ((nState & BST_CHECKED) == BST_CHECKED) {
			iStateId += RBS_CHECKEDNORMAL - RBS_UNCHECKEDNORMAL;
		}
		break;

	default: // should never happen
		iPartId = BP_CHECKBOX;
		iStateId = CBS_UNCHECKEDDISABLED;
		break;
	}

	if (!isAnimationEnabled()) {
		renderButton(hwnd, hdc, hTheme, style, nState, iPartId, iStateId, textColor);
		return iStateId;
	}

	if (BufferedPaintRenderAnimation(hwnd, hdc)) {
		return prevStateId;
	}

	// Animation part - hover transition
	BP_ANIMATIONPARAMS animParams{};
	animParams.cbSize = sizeof(BP_ANIMATIONPARAMS);
	animParams.style = BPAS_LINEAR;
	if (iStateId != prevStateId) {
		GetThemeTransitionDuration(hTheme, iPartId, prevStateId, iStateId, TMT_TRANSITIONDURATIONS, &animParams.dwDuration);
	}

	RECT rcClient{};
	GetClientRect(hwnd, &rcClient);

	HDC hdcFrom = nullptr;
	HDC hdcTo = nullptr;
	if (HANIMATIONBUFFER hbpAnimation = BeginBufferedAnimation(hwnd, hdc, &rcClient, BPBF_COMPATIBLEBITMAP, nullptr, &animParams, &hdcFrom, &hdcTo)) {
		if (hdcFrom != nullptr) {
			renderButton(hwnd, hdcFrom, hTheme, style, nState, iPartId, prevStateId, textColor);
		}
		if (hdcTo != nullptr) {
			renderButton(hwnd, hdcTo, hTheme, style, nState, iPartId, iStateId, textColor);
		}
		EndBufferedAnimation(hbpAnimation, TRUE);
	} else {
		renderButton(hwnd, hdc, hTheme, style, nState, iPartId, iStateId, textColor);
	}
	return iStateId;
}

void paintGroupBox(HWND hwnd, HDC hdc, HTHEME hTheme) noexcept {
	const DWORD style = GetWindowStyle(hwnd);
	const BOOL isEnabled = IsWindowEnabled(hwnd);
	constexpr int iPartId = BP_GROUPBOX;
	const int iStateId = isEnabled ? GBS_NORMAL : GBS_DISABLED;

	bool isFontCreated = false;
	HFONT hFont = nullptr;
	LOGFONT lf;
	if (SUCCEEDED(GetThemeFont(hTheme, hdc, iPartId, iStateId, TMT_FONT, &lf))) {
		hFont = CreateFontIndirect(&lf);
		isFontCreated = hFont != nullptr;
	}
	if (!isFontCreated) {
		hFont = GetWindowFont(hwnd);
	}

	WCHAR buffer[GroupBoxTextBufferSize];
	memset(buffer, 0, sizeof(int));
	const int bufferLen = GetWindowText(hwnd, buffer, COUNTOF(buffer));
	const bool isCenter = (style & BS_CENTER) != 0;
	RECT rcClient{};
	GetClientRect(hwnd, &rcClient);
	rcClient.bottom -= 1;

	RECT rcText{ rcClient };
	RECT rcBackground{ rcClient };
	int textLen = bufferLen;
	if (textLen == 0) {
		// There is no text, use "M" to get metrics to move top edge down
		textLen = 1;
		buffer[0] = L'M';
		buffer[1] = L'\0';
	}

	// Text rectangle part
	SIZE szText{};
	HFONT hOldFont = SelectFont(hdc, hFont);
	GetTextExtentPoint32(hdc, buffer, textLen, &szText);
	rcBackground.top += szText.cy / 2U;
	if (bufferLen != 0) {
		const int centerPosX = isCenter ? ((rcClient.right - rcClient.left - szText.cx) / 2) : 7;
		rcText.left += centerPosX;
		rcText.bottom = rcText.top + szText.cy;
		rcText.right = rcText.left + szText.cx + 4;
		ExcludeClipRect(hdc, rcText.left, rcText.top, rcText.right, rcText.bottom);
	}

	RECT rcContent = rcBackground;
	GetThemeBackgroundContentRect(hTheme, hdc, BP_GROUPBOX, iStateId, &rcBackground, &rcContent);
	ExcludeClipRect(hdc, rcContent.left, rcContent.top, rcContent.right, rcContent.bottom);
	paintFrameRect(hdc, rcBackground, GetDarkPen(DarkPen_EdgePen)); // main frame
	SelectClipRgn(hdc, nullptr);

	// Text part
	if (bufferLen != 0) {
		InflateRect(&rcText, -2, 0);
		DTTOPTS dtto{};
		dtto.dwSize = sizeof(DTTOPTS);
		dtto.dwFlags = DTT_TEXTCOLOR;
		dtto.crText = isEnabled ? DarkColor_TextColor : DarkColor_DisabledTextColor;
		DWORD dtFlags = isCenter ? (DT_CENTER | DT_SINGLELINE) : (DT_LEFT | DT_SINGLELINE);
		if (SendMessage(hwnd, WM_QUERYUISTATE, 0, 0) != 0) {
			dtFlags |= DT_HIDEPREFIX;
		}
		DrawThemeTextEx(hTheme, hdc, BP_GROUPBOX, iStateId, buffer, bufferLen, dtFlags, &rcText, &dtto);
	}

	SelectFont(hdc, hOldFont);
	if (isFontCreated) {
		DeleteObject(hFont);
	}
}

LRESULT CALLBACK ButtonSubclass(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) noexcept {
	if (umsg == WM_NCDESTROY) {
		RemoveWindowSubclass(hwnd, ButtonSubclass, uIdSubclass);
	} else if (DarkMode_IsActive()) {
		switch (umsg) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_PRINTCLIENT:
		case WM_PAINT:
			if (auto hTheme = OpenThemeData(hwnd, VSCLASS_BUTTON)) {
				PAINTSTRUCT ps{};
				auto hdc = AsPointer<HDC>(wParam);
				if (hdc == nullptr) {
					hdc = BeginPaint(hwnd, &ps);
				}
				if (isGroupBox(uIdSubclass)) {
					paintGroupBox(hwnd, hdc, hTheme);
				} else {
					const DWORD prevStateId = static_cast<DWORD>(dwRefData);
					const DWORD iStateId = paintButton(hwnd, hdc, hTheme, prevStateId);
					if (iStateId != prevStateId) {
						SetWindowSubclass(hwnd, ButtonSubclass, uIdSubclass, iStateId);
					}
				}
				if (ps.hdc != nullptr) {
					EndPaint(hwnd, &ps);
				}
				CloseThemeData(hTheme);
				return 0;
			}
			break;

		case WM_SIZE:
		case WM_DESTROY:
			if (!isGroupBox(uIdSubclass) && isAnimationEnabled()) {
				BufferedPaintStopAllAnimations(hwnd);
			}
			break;

		case WM_ENABLE:
			if (isGroupBox(uIdSubclass)) {
				RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE);
			} else {
				// Skip the button's normal WNDPROC so it won't redraw out of WM_PAINT
				const auto result = DefSubclassProc(hwnd, umsg, wParam, lParam);
				InvalidateRect(hwnd, nullptr, FALSE);
				return result;
			}
			break;

		case WM_UPDATEUISTATE:
			if (!isGroupBox(uIdSubclass) && (HIWORD(wParam) & (UISF_HIDEACCEL | UISF_HIDEFOCUS)) != 0) {
				InvalidateRect(hwnd, nullptr, FALSE);
			}
			break;

		default:
			break;
		}
	}
	return DefSubclassProc(hwnd, umsg, wParam, lParam);
}

}

namespace { // StatusBar, ResizeGrip

void paintStatusBar(HWND hwnd, HDC hdc) noexcept {
	struct {
		int horizontal = 0; // horizontal not used
		int vertical = 0;
		int between = 0;
	} borders{};

	SendMessage(hwnd, SB_GETBORDERS, 0, AsInteger<LPARAM>(&borders));

	const auto oldPen = SelectPen(hdc, GetDarkPen(DarkPen_EdgePen));
	const auto hFont = GetWindowFont(hwnd);
	const auto hOldFont = SelectFont(hdc, hFont);

	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, DarkColor_TextColor);

	RECT rcClient{};
	GetClientRect(hwnd, &rcClient);
	FillRect(hdc, &rcClient, GetDarkBrush(DarkBrush_BackgroundBrush));

	const bool hasSizeGrip = !IsZoomed(hwndMain);
	const bool isSimple = SendMessage(hwnd, SB_ISSIMPLE, 0, 0) != FALSE;
	// Simple text is set with SB_SIMPLEID, but retrieved through part 0.
	const auto nParts = isSimple ? 1 : static_cast<int>(SendMessage(hwnd, SB_GETPARTS, 0, 0));

	SIZE szGrip{};
	HTHEME hTheme = nullptr;
	if (hasSizeGrip) {
		hTheme = OpenThemeData(hwnd, VSCLASS_STATUS);
		if (hTheme) {
			GetThemePartSize(hTheme, hdc, SP_GRIPPER, 0, &rcClient, TS_DRAW, &szGrip);
		}
	}

	RECT rcPart{};
	RECT rcIntersect{};
	// no edge before size grip
	const int iLastDiv = nParts - (hasSizeGrip ? 1 : 0);
	// Don't draw edge if there is only one part without size grip.
	const bool drawEdge = !isSimple && (nParts >= 2 || !hasSizeGrip);
	for (int i = 0; i < nParts; ++i) {
		if (isSimple) {
			// SB_GETRECT only exposes normal parts, even when simple mode is active.
			rcPart = rcClient;
			rcPart.right -= szGrip.cx;
		} else {
			SendMessage(hwnd, SB_GETRECT, i, AsInteger<LPARAM>(&rcPart));
		}
		if (!IntersectRect(&rcIntersect, &rcPart, &rcClient)) {
			continue;
		}

		if (drawEdge && (i < iLastDiv)) {
			const POINT edges[] = {
				{ rcPart.right - borders.between, rcPart.top + 1 },
				{ rcPart.right - borders.between, rcPart.bottom - 3 }
			};
			Polyline(hdc, edges, COUNTOF(edges));
		}

		rcPart.left += borders.between;
		rcPart.right -= borders.vertical;

		WCHAR str[StatusBarItemTextBufferSize];
		memset(str, 0, sizeof(int));
		const LRESULT retValLen = SendMessage(hwnd, SB_GETTEXT, i, AsInteger<LPARAM>(str));
		const DWORD cchText = LOWORD(retValLen);
		str[cchText] = L'\0';

		DrawText(hdc, str, cchText, &rcPart, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_NOPREFIX);
	}

#if 0 // for horizontal edge
	const POINT edgeHor[] = {
		{rcClient.left, rcClient.top},
		{rcClient.right, rcClient.top}
	};
	Polyline(hdc, edgeHor, COUNTOF(edgeHor));
#endif

	// draw optional size grip
	if (hTheme) {
		RECT rcGrip{ rcClient };
		rcGrip.left = rcGrip.right - szGrip.cx;
		rcGrip.top = rcGrip.bottom - szGrip.cy;
		DrawThemeBackground(hTheme, hdc, SP_GRIPPER, 0, &rcGrip, nullptr);
		CloseThemeData(hTheme);
	}

	SelectPen(hdc, oldPen);
	SelectFont(hdc, hOldFont);
}

void paintResizeGrip(HWND hwnd, HDC hdc) noexcept {
	RECT rc{};
	GetClientRect(hwnd, &rc);
	FillRect(hdc, &rc, GetDarkBrush(DarkBrush_DlgBackgroundBrush));

	// Dialog grips are scrollbar controls; use the same themed gripper as the status bar.
	if (HTHEME hTheme = OpenThemeData(hwnd, VSCLASS_STATUS)) {
		SIZE szGrip{};
		GetThemePartSize(hTheme, hdc, SP_GRIPPER, 0, &rc, TS_DRAW, &szGrip);
		RECT rcGrip{ rc };
		rcGrip.left = rcGrip.right - szGrip.cx;
		rcGrip.top = rcGrip.bottom - szGrip.cy;
		DrawThemeBackground(hTheme, hdc, SP_GRIPPER, 0, &rcGrip, nullptr);
		CloseThemeData(hTheme);
	}
}

LRESULT CALLBACK StatusBarSubclass(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) noexcept {
	auto * const paint = AsPointer<BufferedPaint *>(dwRefData);
	if (umsg == WM_NCDESTROY) {
		RemoveWindowSubclass(hwnd, StatusBarSubclass, uIdSubclass);
		paint->releaseBuffer();
		NP2HeapFree(paint);
	} else if (DarkMode_IsActive()) {
		switch (umsg) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_PAINT: {
			PAINTSTRUCT ps{};
			RECT rcClient{};
			HDC hdc = BeginPaint(hwnd, &ps);
			if (paint->BeginPaint(hwnd, hdc, ps, rcClient)) {
				paintStatusBar(hwnd, paint->hMemDC);
				paint->EndPaint(hdc, ps);
			}
			EndPaint(hwnd, &ps);
			return 0;
		}

		default:
			break;
		}
	}
	return DefSubclassProc(hwnd, umsg, wParam, lParam);
}

LRESULT CALLBACK ResizeGripSubclass(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) noexcept {
	UNREFERENCED_PARAMETER(dwRefData);
	if (umsg == WM_NCDESTROY) {
		RemoveWindowSubclass(hwnd, ResizeGripSubclass, uIdSubclass);
	} else if (DarkMode_IsActive()) {
		switch (umsg) {
		case WM_ERASEBKGND:
			return TRUE;

		case WM_PRINTCLIENT:
		case WM_PAINT: {
			PAINTSTRUCT ps{};
			auto hdc = AsPointer<HDC>(wParam);
			if (hdc == nullptr) {
				hdc = BeginPaint(hwnd, &ps);
			}
			paintResizeGrip(hwnd, hdc);
			if (ps.hdc != nullptr) {
				EndPaint(hwnd, &ps);
			}
			return 0;
		}

		default:
			break;
		}
	}
	return DefSubclassProc(hwnd, umsg, wParam, lParam);
}

}

namespace { // Rebar, Toolbar

LRESULT prePaintToolbarItem(LPNMTBCUSTOMDRAW lptbcd) noexcept {
	// Set colors
	lptbcd->hbrMonoDither = GetDarkBrush(DarkBrush_BackgroundBrush);
	lptbcd->hbrLines = GetDarkBrush(DarkBrush_EdgeBrush);
	lptbcd->hpenLines = GetDarkPen(DarkPen_EdgePen);
	lptbcd->clrText = DarkColor_DarkerTextColor;
	lptbcd->clrTextHighlight = DarkColor_TextColor;
	lptbcd->clrBtnFace = DarkColor_BackgroundColor;
	lptbcd->clrBtnHighlight = DarkColor_CtrlBackgroundColor;
	lptbcd->clrHighlightHotTrack = DarkColor_HotBackgroundColor;
	lptbcd->nStringBkMode = TRANSPARENT;
	lptbcd->nHLStringBkMode = TRANSPARENT;

	// Get styles and rectangles
	const bool isHot = (lptbcd->nmcd.uItemState & CDIS_HOT) == CDIS_HOT;
	const bool isChecked = (lptbcd->nmcd.uItemState & CDIS_CHECKED) == CDIS_CHECKED;

	RECT rcItem{ lptbcd->nmcd.rc };
	RECT rcDrop{};

	TBBUTTONINFOW tbi{};
	tbi.cbSize = sizeof(TBBUTTONINFOW);
	tbi.dwMask = TBIF_IMAGE | TBIF_STYLE;
	HWND hwnd = lptbcd->nmcd.hdr.hwndFrom;
	SendMessage(hwnd, TB_GETBUTTONINFO, lptbcd->nmcd.dwItemSpec, AsInteger<LPARAM>(&tbi));

	const bool isIcon = tbi.iImage != I_IMAGENONE;
	const bool isDropDown = ((tbi.fsStyle & BTNS_DROPDOWN) != 0) && isIcon; // has 2 "buttons"
	if (isDropDown) {
		const auto idx = SendMessage(hwnd, TB_COMMANDTOINDEX, lptbcd->nmcd.dwItemSpec, 0);
		SendMessage(hwnd, TB_GETITEMDROPDOWNRECT, static_cast<WPARAM>(idx), AsInteger<LPARAM>(&rcDrop));
		rcItem.right = rcDrop.left;
	}

	// Paint part
	if (isHot || isChecked) { // hot must have higher priority to overwrite checked state
		HPEN pen = isHot ? GetDarkPen(DarkPen_HotEdgePen) : GetDarkPen(DarkPen_EdgePen);
		HBRUSH brush = isHot ? GetDarkBrush(DarkBrush_HotBackgroundBrush) : GetDarkBrush(DarkBrush_CtrlBackgroundBrush);
		constexpr UINT maskHot = ~static_cast<UINT>(CDIS_CHECKED | CDIS_HOT); // clears states to use custom highlight;
		constexpr UINT maskChecked = ~static_cast<UINT>(CDIS_CHECKED); // clears state to use custom highlight
		lptbcd->nmcd.uItemState &= (isHot ? maskHot : maskChecked);

		HDC hdc = lptbcd->nmcd.hdc;
		if (!isIcon) {
			FillRect(hdc, &rcItem, brush);
		} else {
			const int roundness = DarkMode_IsWindows11() ? kWin11CornerRoundness + 1 : 0;
			paintRoundRect(hdc, rcItem, pen, brush, roundness, roundness);
			if (isDropDown) {
				paintRoundRect(hdc, rcDrop, pen, brush, roundness, roundness);
			}
		}
	}

	LRESULT retVal = TBCDRF_USECDCOLORS;
	if ((lptbcd->nmcd.uItemState & CDIS_SELECTED) != 0) {
		retVal |= TBCDRF_NOBACKGROUND;
	}
	if (isDropDown) {
		retVal |= CDRF_NOTIFYPOSTPAINT;
	}

	return retVal;
}

LRESULT postPaintToolbarItem(const NMTBCUSTOMDRAW *lptbcd) noexcept {
	TBBUTTONINFOW tbi{};
	tbi.cbSize = sizeof(TBBUTTONINFOW);
	tbi.dwMask = TBIF_IMAGE;
	HWND hwnd = lptbcd->nmcd.hdr.hwndFrom;
	const auto dwItemSpec = lptbcd->nmcd.dwItemSpec;
	SendMessage(hwnd, TB_GETBUTTONINFO, dwItemSpec, AsInteger<LPARAM>(&tbi));

	if (tbi.iImage == I_IMAGENONE) {
		return CDRF_DODEFAULT;
	}

	RECT rcArrow{};
	const auto idx = SendMessage(hwnd, TB_COMMANDTOINDEX, dwItemSpec, 0);
	SendMessage(hwnd, TB_GETITEMDROPDOWNRECT, static_cast<WPARAM>(idx), AsInteger<LPARAM>(&rcArrow));
	rcArrow.left += 1;
	rcArrow.bottom -= MulDiv(3, g_uCurrentDPI, USER_DEFAULT_SCREEN_DPI);

	HDC hdc = lptbcd->nmcd.hdc;
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, DarkColor_TextColor);

	HFONT hFont = GetWindowFont(hwnd);
	HFONT hOldFont = SelectFont(hdc, hFont);
	constexpr UINT dtFlags = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP | DT_NOPREFIX;
	constexpr WCHAR text[] = {kTriangleDown, L'\0'};
	DrawText(hdc, text, 1, &rcArrow, dtFlags);
	SelectFont(hdc, hOldFont);

	return CDRF_DODEFAULT;
}

}

LRESULT DarkMode_RebarCustomDraw(LPARAM lParam) noexcept {
	if (DarkMode_IsActive()) {
		auto * const lpnmcd = AsPointer<LPNMCUSTOMDRAW>(lParam);
		if (lpnmcd->dwDrawStage == CDDS_PREPAINT) {
			FillRect(lpnmcd->hdc, &lpnmcd->rc, GetDarkBrush(DarkBrush_DlgBackgroundBrush));
			return CDRF_SKIPDEFAULT;
		}
	}
	return FALSE;
}

LRESULT DarkMode_ToolbarCustomDraw(LPARAM lParam) noexcept {
	if (DarkMode_IsActive()) {
		auto * const lptbcd = AsPointer<LPNMTBCUSTOMDRAW>(lParam);
		switch (lptbcd->nmcd.dwDrawStage) {
		case CDDS_PREPAINT:
			FillRect(lptbcd->nmcd.hdc, &lptbcd->nmcd.rc, GetDarkBrush(DarkBrush_BackgroundBrush));
			return CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;

		case CDDS_ITEMPREPAINT:
			return prePaintToolbarItem(lptbcd);

		case CDDS_ITEMPOSTPAINT:
			return postPaintToolbarItem(lptbcd);

		default:
			break;
		}
	}
	return FALSE;
}

namespace { // Window

LRESULT CALLBACK DarkDialogProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) noexcept {
	if (umsg == WM_NCDESTROY) {
		RemoveWindowSubclass(hwnd, DarkDialogProc, uIdSubclass);
	} else if (DarkMode_IsActive()) {
		switch (umsg) {
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLORLISTBOX:
		// case WM_CTLCOLORBTN:
		// case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLOREDIT: {
			COLORREF back = DarkColor_DlgBackgroundColor;
			UINT index = DarkBrush_DlgBackgroundBrush;
			if (umsg == WM_CTLCOLOREDIT || umsg == WM_CTLCOLORLISTBOX) {
				back = DarkColor_CtrlBackgroundColor;
				index = DarkBrush_CtrlBackgroundBrush;
			} else if (umsg == WM_CTLCOLORSTATIC) {
				auto lpDialogFunc = AsPointer<WNDPROC>(GetWindowLongPtr(hwnd, DWLP_DLGPROC));
				static_assert(sizeof(INT_PTR) == sizeof(LRESULT));
				if (const auto result = CallWindowProc(lpDialogFunc, hwnd, umsg, wParam, lParam)) {
					if (dwRefData == DialogRefData_MessageBox) {
						back = DarkColor_BackgroundColor;
						index = DarkBrush_BackgroundBrush;
					} else {
						return result;
					}
				}
			}

			HDC hdc = AsPointer<HDC>(wParam);
			SetTextColor(hdc, DarkColor_TextColor);
			SetBkColor(hdc, back);
			return AsInteger<LONG_PTR>(GetDarkBrush(index));
		}

		case WM_PRINTCLIENT:
			return TRUE;

		case WM_PAINT:
			if (dwRefData == DialogRefData_MessageBox) {
				PAINTSTRUCT ps{};
				HDC hdc = BeginPaint(hwnd, &ps);
				RECT rc{};
				rc.bottom = 26;
				MapDialogRect(hwnd, &rc);
				const int bottom = rc.bottom;
				rc = ps.rcPaint;
				rc.bottom -= bottom;
				FillRect(hdc, &rc, GetDarkBrush(DarkBrush_BackgroundBrush));
				rc.top = rc.bottom;
				rc.bottom += bottom;
				FillRect(hdc, &rc, GetDarkBrush(DarkBrush_DlgBackgroundBrush));
				EndPaint(hwnd, &ps);
				return 0;
			}
			break;

		case WM_NOTIFY:
			break;

		default:
			break;
		}
	}
	return DefSubclassProc(hwnd, umsg, wParam, lParam);
}

enum DarkEnumFlag {
	DarkEnumFlag_None = 0,
	DarkEnumFlag_Active = 1,
	DarkEnumFlag_NeedDarkTheme = 2,
	DarkEnumFlag_HandleTreeList = 4,
};

struct DarkEnumParam {
	LPCWSTR theme;
	LPCWSTR treeListTheme;
	UINT flag;
	COLORREF textColor;
	COLORREF backColor;
};

BOOL CALLBACK DarkEnumChildProc(HWND hwnd, LPARAM lParam) noexcept {
	WCHAR className[16]{};
	const UINT len = GetClassName(hwnd, className, COUNTOF(className));
	const DarkEnumParam &enumParam = *AsPointer<const DarkEnumParam *>(lParam);
	LPCWSTR theme = enumParam.theme;
	if (StrEqualLen(className, len, WC_BUTTON)) {
		SetWindowTheme(hwnd, theme, nullptr);
		if (enumParam.flag & DarkEnumFlag_NeedDarkTheme) {
			const DWORD style = GetWindowStyle(hwnd);
			if ((style & BS_PUSHLIKE) == 0 && buttonNeedsSubclass(style & BS_TYPEMASK)) {
				SetWindowSubclass(hwnd, ButtonSubclass, style & BS_TYPEMASK, 0);
			}
		}
	} else if (StrEqualLen(className, len, WC_EDIT)) {
		if (enumParam.flag & DarkEnumFlag_NeedDarkTheme) {
			const DWORD style = GetWindowStyle(hwnd);
			if ((style & (WS_HSCROLL | WS_VSCROLL)) == 0) {
				theme = L"DarkMode_CFD";
			}
		}
		SetWindowTheme(hwnd, theme, nullptr);
	} else if (StrEqualLen(className, len, WC_LISTBOX)) {
		SetWindowTheme(hwnd, theme, nullptr);
	} else if (StrEqualLen(className, len, WC_COMBOBOX)) {
		COMBOBOXINFO info;
		info.cbSize = sizeof(COMBOBOXINFO);
		if (GetComboBoxInfo(hwnd, &info)) {
			SetWindowTheme(info.hwndList, theme, nullptr);
		}
		if (enumParam.flag & DarkEnumFlag_NeedDarkTheme) {
			theme = L"DarkMode_CFD";
		}
		SetWindowTheme(hwnd, theme, nullptr);
	} else if (StrEqualLen(className, len, WC_SCROLLBAR)) {
		// DarkMode_DarkTheme's status gripper paints its own background.
		SetWindowTheme(hwnd, L"Explorer", nullptr);
		SetWindowSubclass(hwnd, ResizeGripSubclass, 0, 0);
	} else if ((enumParam.flag & DarkEnumFlag_HandleTreeList) && StrEqualLen(className, len, WC_TREEVIEW)) {
		theme = enumParam.treeListTheme;
		const COLORREF textColor = enumParam.textColor;
		const COLORREF backColor = enumParam.backColor;
		SetWindowTheme(hwnd, theme, nullptr);
		TreeView_SetTextColor(hwnd, textColor);
		TreeView_SetBkColor(hwnd, backColor);
		if (HWND hwndCtrl = TreeView_GetToolTips(hwnd)) {
			SetWindowTheme(hwndCtrl, theme, nullptr);
		}
	} else if ((enumParam.flag & DarkEnumFlag_HandleTreeList) && StrEqualLen(className, len, WC_LISTVIEW)) {
		theme = enumParam.treeListTheme;
		const COLORREF textColor = enumParam.textColor;
		const COLORREF backColor = enumParam.backColor;
		SetWindowTheme(hwnd, theme, nullptr);
		ListView_SetTextColor(hwnd, textColor);
		ListView_SetTextBkColor(hwnd, backColor);
		ListView_SetBkColor(hwnd, backColor);
		if (HWND hwndCtrl = ListView_GetToolTips(hwnd)) {
			SetWindowTheme(hwndCtrl, theme, nullptr);
		}
	}
	return TRUE;
}

BOOL CALLBACK RefreshTopLevelProc(HWND hwnd, LPARAM lParam) noexcept {
	if (hwnd != hwndMain) {
		DWORD_PTR dwRefData = 0;
		const BOOL managed = GetWindowSubclass(hwnd, DarkDialogProc, 0, &dwRefData);
		if (managed || GetWindowInstance(hwnd) == g_hInstance) {
			if (!managed) {
				SetWindowSubclass(hwnd, DarkDialogProc, 0, dwRefData);
			}
			const DarkEnumParam &enumParam = *AsPointer<const DarkEnumParam *>(lParam);
			DarkMode_SetDarkTitleBar(hwnd, enumParam.flag & DarkEnumFlag_Active);
			EnumChildWindows(hwnd, DarkEnumChildProc, lParam);
			RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME);
		}
	}
	return TRUE;
}

}

namespace { // DialogHook

struct DialogHook {
	HHOOK hook;
	DWORD_PTR dwRefData;

	void Start() noexcept {
		hook = SetWindowsHookEx(WH_CBT, DialogHook::HookProc, nullptr, GetCurrentThreadId());
	}
	void Stop() noexcept {
		HHOOK current = hook;
		if (current) {
			hook = nullptr;
			UnhookWindowsHookEx(current);
		}
	}
	static LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) noexcept;
};

DialogHook dialogHook {nullptr, DialogRefData_CenterParent};
LRESULT CALLBACK InitDialogSubProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) noexcept {
	if (umsg == WM_INITDIALOG || umsg == WM_DESTROY) {
		RemoveWindowSubclass(hwnd, InitDialogSubProc, uIdSubclass);
	}
	const LRESULT result = DefSubclassProc(hwnd, umsg, wParam, lParam);
	if (umsg == WM_INITDIALOG) {
		if (dwRefData > DialogRefData_MaxValue) {
			SetWindowSubclass(hwnd, FileDialog::SubProc, 0, dwRefData);
		} else {
			DarkMode_InitDialog(hwnd, dwRefData);
		}
	}
	return result;
}

LRESULT CALLBACK DialogHook::HookProc(int nCode, WPARAM wParam, LPARAM lParam) noexcept {
	if (nCode == HCBT_CREATEWND) {
		const auto *lpcs = AsPointer<const CBT_CREATEWND *>(lParam)->lpcs;
		if (lpcs->lpszClass == WC_DIALOG) {
			const auto current = dialogHook;
			memset(&dialogHook, 0, sizeof(dialogHook));
			UnhookWindowsHookEx(current.hook);
			HWND hwnd = AsPointer<HWND>(wParam);
			SetWindowSubclass(hwnd, InitDialogSubProc, 0, current.dwRefData);
		}
	}
	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

}

void DialogHook_Start(DWORD_PTR dwRefData) noexcept {
	dialogHook.dwRefData = dwRefData;
	dialogHook.Start();
}

void DialogHook_Stop() noexcept {
	dialogHook.Stop();
}

void DarkMode_Init(const OSVERSIONINFOW &version) noexcept {
	const DWORD buildNumber = version.dwBuildNumber;
	if (version.dwMajorVersion == 10 && version.dwMinorVersion == 0 && CheckBuildNumber(buildNumber)) {
		fnSetWindowCompositionAttribute = DLLFunctionEx<SetWindowCompositionAttributeSig>(L"user32.dll", "SetWindowCompositionAttribute");
		HMODULE hUxtheme = GetModuleHandleW(L"uxtheme.dll");
		fnRefreshImmersiveColorPolicyState = DLLFunction<RefreshImmersiveColorPolicyStateSig>(hUxtheme, MAKEINTRESOURCEA(104));
		fnGetIsImmersiveColorUsingHighContrast = DLLFunction<GetIsImmersiveColorUsingHighContrastSig>(hUxtheme, MAKEINTRESOURCEA(106));
		fnAllowDarkModeForWindow = DLLFunction<AllowDarkModeForWindowSig>(hUxtheme, MAKEINTRESOURCEA(133));

		fnSetPreferredAppMode = DLLFunction<SetPreferredAppModeSig>(hUxtheme, MAKEINTRESOURCEA(135));
		fnFlushMenuThemes = DLLFunction<FlushMenuThemesSig>(hUxtheme, MAKEINTRESOURCEA(136));
		fnIsDarkModeAllowedForWindow = DLLFunction<IsDarkModeAllowedForWindowSig>(hUxtheme, MAKEINTRESOURCEA(137));

		if (fnRefreshImmersiveColorPolicyState &&
			fnGetIsImmersiveColorUsingHighContrast &&
			fnAllowDarkModeForWindow &&
			fnSetPreferredAppMode &&
			fnFlushMenuThemes &&
			fnIsDarkModeAllowedForWindow) {
			g_DarkModeStatus = DarkModeStatus_Supported;

			if (buildNumber > win11Build25H2) {
				g_DarkModeStatus |= DarkModeStatus_IsWindows11 | DarkModeStatus_SupportDarkTheme;
			} else if (buildNumber >= win11Build24H2) {
				g_DarkModeStatus |= DarkModeStatus_IsWindows11;
				constexpr DWORD minWin11Revision = 6899; // 2025-10-14
				DWORD size = sizeof(DWORD);
				DWORD revision = 0;
				const auto status = RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"UBR", RRF_RT_REG_DWORD, nullptr, &revision, &size);
				if (status == ERROR_SUCCESS && revision >= minWin11Revision) {
					g_DarkModeStatus |= DarkModeStatus_SupportDarkTheme;
				}
			} else if (buildNumber >= win11Build21H2) {
				g_DarkModeStatus |= DarkModeStatus_IsWindows11;
			} else if (buildNumber < win10Build1903) {
				g_DarkModeStatus |= DarkModeStatus_PriorWindows10_1903;
			}
			// if (buildNumber >= win10Build2004) {
			// 	fnDwmSetWindowAttribute = DLLFunctionEx<DwmSetWindowAttributeSig>(L"dwmapi.dll", "DwmSetWindowAttribute");
			// }
		}
	}
}

NP2_noinline
void DarkMode_SetAppDarkMode(BOOL dark) noexcept {
	if (dark) {
		if (!(g_DarkModeStatus & DarkModeStatus_PriorWindows10_1903)) {
			dark = static_cast<BOOL>(PreferredAppMode::ForceDark);
		}
		if (!(g_DarkModeStatus & DarkModeStatus_GlobalInitialized)) {
			g_DarkModeStatus |= DarkModeStatus_GlobalInitialized;
			InitDarkBrushAndPen();
		}
	}
	fnSetPreferredAppMode(static_cast<PreferredAppMode>(dark));
	fnFlushMenuThemes();
}

NP2_noinline
void DarkMode_SetDarkTitleBar(HWND hwnd, BOOL dark) noexcept {
	// if (fnDwmSetWindowAttribute) {
	// 	fnDwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
	// } else
	{
		fnAllowDarkModeForWindow(hwnd, dark);
		if (dark) {
			dark = fnIsDarkModeAllowedForWindow(hwnd);
		}
		if (g_DarkModeStatus & DarkModeStatus_PriorWindows10_1903) {
			SetPropW(hwnd, L"UseImmersiveDarkModeColors", AsPointer<HANDLE>(static_cast<INT_PTR>(dark)));
		} else if (fnSetWindowCompositionAttribute) {
			WINDOWCOMPOSITIONATTRIBDATA data = { WCA_USEDARKMODECOLORS, &dark, sizeof(dark) };
			fnSetWindowCompositionAttribute(hwnd, &data);
		}
	}
	if (!DarkMode_IsWindows11()) {
		// flash the window to fix title bar paint on Win10
		SendMessage(hwnd, WM_NCACTIVATE, FALSE, 0);
		SendMessage(hwnd, WM_NCACTIVATE, TRUE, 0);
	}
}

void DarkMode_Cleanup() noexcept {
	for (auto obj : darkObject) {
		if (obj) {
			DeleteObject(obj);
		}
	}
}

void DarkMode_HandleSettingChange(HWND hwnd, LPARAM lParam) noexcept {
	if (hThemeMenu) {
		CloseThemeData(hThemeMenu);
		hThemeMenu = nullptr;
	}
	if (DarkMode_IsSupported() && IsColorSchemeChangeMessage(lParam)) {
		fnRefreshImmersiveColorPolicyState();
		fnGetIsImmersiveColorUsingHighContrast(IMMERSIVE_HC_CACHE_MODE::IHCM_REFRESH);
		Style_OnStyleThemeChanged(hwnd, styleThemeSetting);
	}
}

NP2_noinline
int DarkMode_ResolveStyleTheme(int theme) noexcept {
	if (theme < StyleTheme_Light) {
		theme = DarkMode_AppsUseDarkMode()? StyleTheme_Dark : StyleTheme_Light;
	}
	if (theme != StyleTheme_Light && DarkMode_IsSupported()) {
		g_DarkModeStatus |= DarkModeStatus_Active;
	} else {
		g_DarkModeStatus &= ~DarkModeStatus_Active;
	}
	return theme;
}

NP2_noinline
void DarkMode_OnDarkModeChange(HWND hwnd) noexcept {
	const BOOL dark = DarkMode_IsActive();
	DarkEnumParam param {};
	if (dark) {
		param.theme = getDarkModeThemeName();
		param.treeListTheme = L"DarkMode_Explorer";
		param.textColor = DarkColor_ViewTextColor;
		param.backColor = DarkColor_ViewBackgroundColor;
		param.flag = DarkMode_SupportDarkTheme()? (DarkEnumFlag_Active | DarkEnumFlag_HandleTreeList) : (DarkEnumFlag_Active | DarkEnumFlag_HandleTreeList | DarkEnumFlag_NeedDarkTheme);
	} else {
		param.theme = L"Explorer";
		param.treeListTheme = L"Explorer";
		param.flag = DarkEnumFlag_HandleTreeList;
		param.textColor = GetSysColor(COLOR_WINDOWTEXT);
		param.backColor = GetSysColor(COLOR_WINDOW);
	}

	DarkMode_SetAppDarkMode(dark);
	DarkMode_SetDarkTitleBar(hwnd, dark);
	MsgThemeChanged(hwnd, 0, 0);
	SetWindowTheme(hwndEdit, (dark? L"DarkMode_Explorer" : L"Explorer"), nullptr);
	RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME);
	EnumThreadWindows(GetCurrentThreadId(), RefreshTopLevelProc, AsInteger<LPARAM>(&param));
}

void DarkMode_ApplyToBars(HWND hwndToolbar, HWND hwndStatus) noexcept {
	if (DarkMode_IsActive()) {
		COLORSCHEME scheme{};
		scheme.dwSize = sizeof(COLORSCHEME);
		scheme.clrBtnHighlight = DarkColor_BackgroundColor;
		scheme.clrBtnShadow = DarkColor_BackgroundColor;
		SendMessage(hwndToolbar, TB_SETCOLORSCHEME, 0, AsInteger<LPARAM>(&scheme));
		if (auto hwndCtrl = SendMessage(hwndToolbar, TB_GETTOOLTIPS, 0, 0)) {
			SetWindowTheme(AsPointer<HWND>(hwndCtrl), L"DarkMode_Explorer", nullptr);
		}
		auto * const paint = static_cast<BufferedPaint *>(NP2HeapAlloc(sizeof(BufferedPaint)));
		SetWindowSubclass(hwndStatus, StatusBarSubclass, 0, AsInteger<DWORD_PTR>(paint));
	}
}

void DarkMode_OnToolbarBeginAdjust() noexcept {
	DialogHook_Start(DialogRefData_CustomizeToolbar);
}

void DarkMode_OnToolbarEndAdjust() noexcept {
	DialogHook_Stop();
}

NP2_noinline
void DarkMode_InitDialog(HWND hwnd, DWORD_PTR dwRefData) noexcept {
	if (DarkMode_IsActive()) {
		DarkEnumParam param {};
		param.theme = getDarkModeThemeName();
		param.treeListTheme = L"DarkMode_Explorer";
		param.textColor = DarkColor_ViewTextColor;
		param.backColor = DarkColor_ViewBackgroundColor;
		param.flag = DarkMode_SupportDarkTheme()? DarkEnumFlag_Active : (DarkEnumFlag_Active | DarkEnumFlag_NeedDarkTheme);

		DarkMode_SetDarkTitleBar(hwnd, TRUE);
		SetWindowSubclass(hwnd, DarkDialogProc, 0, dwRefData);
		EnumChildWindows(hwnd, DarkEnumChildProc, AsInteger<LPARAM>(&param));
	} else if (DarkMode_IsSupported() && (dwRefData == DialogRefData_MessageBox || dwRefData > DialogRefData_DefaultPosition)) {
		// subclass system dialog to allow toggle dark mode
		SetWindowSubclass(hwnd, DarkDialogProc, 0, dwRefData);
	}
	if (dwRefData >= DialogRefData_DefaultPosition) {
		return;
	}
	if (dwRefData != DialogRefData_RightBottom) {
		CenterDlgInParent(hwnd);
		if (dwRefData == DialogRefData_MessageBox) {
			SnapToDefaultButton(hwnd);
		}
	} else {
		SetToRightBottom(hwnd);
	}
}

NP2_noinline
void DarkMode_InitTreeView(HWND hwndTV) noexcept {
	InitWindowCommon(hwndTV);
	TreeView_SetExtendedStyle(hwndTV, TVS_EX_DOUBLEBUFFER, TVS_EX_DOUBLEBUFFER);
	const bool dark = DarkMode_IsActive();
	LPCWSTR theme = dark ? L"DarkMode_Explorer" : L"Explorer";
	SetWindowTheme(hwndTV, theme, nullptr);
	if (dark) {
		TreeView_SetTextColor(hwndTV, DarkColor_ViewTextColor);
		TreeView_SetBkColor(hwndTV, DarkColor_ViewBackgroundColor);
		if (HWND hwndCtrl = TreeView_GetToolTips(hwndTV)) {
			SetWindowTheme(hwndCtrl, theme, nullptr);
		}
	}
}

NP2_noinline
void DarkMode_InitFileListView(HWND hwndLV) noexcept {
	InitWindowCommon(hwndLV);
	ListView_SetExtendedListViewStyle(hwndLV, /*LVS_EX_FULLROWSELECT|*/LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
	if (DarkMode_IsActive()) {
		ListView_SetTextColor(hwndLV, DarkColor_ViewTextColor);
		ListView_SetTextBkColor(hwndLV, DarkColor_ViewBackgroundColor);
		ListView_SetBkColor(hwndLV, DarkColor_ViewBackgroundColor);
		SetWindowTheme(hwndLV, L"DarkMode_Explorer", nullptr);
		if (HWND hwndCtrl = ListView_GetToolTips(hwndLV)) {
			SetWindowTheme(hwndCtrl, L"DarkMode_Explorer", nullptr);
		}
	}

	LVCOLUMN lvc{};
	lvc.mask = LVCF_FMT | LVCF_TEXT;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(hwndLV, 0, &lvc);
}
