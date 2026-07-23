// This file is part of Notepad4.
// See License.txt for details about distribution and modification.

#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#include "config.h"
#include "Helpers.h"
#include "Darkmodelib.h"
#include "DarkMode.h"
#include "Dialogs.h"
#include "EditLexer.h"
#include "DmlibDpi.h"
#include "DmlibHook.h"
#include "resource.h"

extern int np2StyleTheme;
extern HWND hwndEdit;
extern HINSTANCE g_hInstance;

static constexpr LPCWSTR DarkMode_DialogAppliedProp = L"Notepad4.DarkModeApplied";
static constexpr LPCWSTR DarkMode_DialogManagedProp = L"Notepad4.DarkModeManaged";
static constexpr UINT_PTR DarkMode_DialogSubclassId = 1;
static constexpr UINT_PTR DarkMode_ResizeGripSubclassId = 1;
static constexpr UINT_PTR DarkMode_MessageBoxSubclassId = 1;

static bool DarkMode_IsHighContrast() noexcept {
	HIGHCONTRAST highContrast{ sizeof(HIGHCONTRAST) };
	return SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &highContrast, 0)
		&& (highContrast.dwFlags & HCF_HIGHCONTRASTON) != 0;
}

static UINT DarkMode_TypeForStyleTheme(int theme) noexcept {
	if (DarkMode_IsHighContrast()) {
		return static_cast<UINT>(dmlib::DarkModeType::classic);
	}
	return static_cast<UINT>((theme == StyleTheme_Dark) ? dmlib::DarkModeType::dark : dmlib::DarkModeType::light);
}

static constexpr COLORREF HEXRGB(DWORD rrggbb) noexcept {
	return ((rrggbb & 0xFF0000) >> 16) | (rrggbb & 0x00FF00) | ((rrggbb & 0x0000FF) << 16);
}

static void DarkMode_SetCustomColors() noexcept {
	dmlib::setBackgroundColor(HEXRGB(0x2D2D2D));
	dmlib::setCtrlBackgroundColor(HEXRGB(0x383838));
	dmlib::setHotBackgroundColor(HEXRGB(0x454545));
	dmlib::setDlgBackgroundColor(HEXRGB(0x252526));
	dmlib::setTextColor(HEXRGB(0xD4D4D4));
	dmlib::setDarkerTextColor(HEXRGB(0xC0C0C0));
	dmlib::setDisabledTextColor(HEXRGB(0x808080));
	dmlib::setEdgeColor(HEXRGB(0x555555));
	dmlib::setHotEdgeColor(HEXRGB(0x9B9B9B));
	dmlib::setDisabledEdgeColor(HEXRGB(0x484848));
	dmlib::updateThemeBrushesAndPens();

	dmlib::setViewBackgroundColor(HEXRGB(0x1E1E1E));
	dmlib::setViewTextColor(HEXRGB(0xD4D4D4));
	dmlib::setViewGridlinesColor(HEXRGB(0x3C3C3C));
	dmlib::setHeaderBackgroundColor(HEXRGB(0x2D2D2D));
	dmlib::setHeaderTextColor(HEXRGB(0xD4D4D4));
	dmlib::setHeaderEdgeColor(HEXRGB(0x555555));
	dmlib::updateViewBrushesAndPens();

	dmlib::setSysColor(COLOR_WINDOW, HEXRGB(0x1E1E1E));
	dmlib::setSysColor(COLOR_WINDOWTEXT, HEXRGB(0xD4D4D4));
	dmlib::setSysColor(COLOR_BTNFACE, HEXRGB(0x3C3C3C));
}

static void DarkMode_PaintResizeGrip(HWND hwnd, HDC hdc) noexcept {
	RECT rc{};
	GetClientRect(hwnd, &rc);
	FillRect(hdc, &rc, dmlib::getDlgBackgroundBrush());

	HTHEME hTheme = OpenThemeData(hwnd, VSCLASS_STATUS);
	if (hTheme != nullptr) {
		SIZE size{};
		GetThemePartSize(hTheme, hdc, SP_GRIPPER, 0, &rc, TS_DRAW, &size);
		rc.left = rc.right - size.cx;
		rc.top = rc.bottom - size.cy;
		DrawThemeBackground(hTheme, hdc, SP_GRIPPER, 0, &rc, nullptr);
		CloseThemeData(hTheme);
	}
}

static LRESULT CALLBACK DarkMode_ResizeGripSubclass(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR idSubclass, DWORD_PTR /*refData*/) noexcept {
	switch (msg) {
	case WM_NCDESTROY:
		RemoveWindowSubclass(hwnd, DarkMode_ResizeGripSubclass, idSubclass);
		break;

	case WM_ERASEBKGND:
		if (dmlib::isExperimentalActive()) {
			return TRUE;
		}
		break;

	case WM_PAINT:
		if (dmlib::isExperimentalActive()) {
			PAINTSTRUCT ps{};
			HDC hdc = BeginPaint(hwnd, &ps);
			DarkMode_PaintResizeGrip(hwnd, hdc);
			EndPaint(hwnd, &ps);
			return 0;
		}
		break;

	case WM_PRINT:
	case WM_PRINTCLIENT:
		if (dmlib::isExperimentalActive()) {
			DarkMode_PaintResizeGrip(hwnd, AsPointer<HDC>(wParam));
			return 0;
		}
		break;

	case WM_THEMECHANGED:
	case WM_WINDOWPOSCHANGED:
		if (dmlib::isExperimentalActive()) {
			InvalidateRect(hwnd, nullptr, TRUE);
		}
		break;
	}
	return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static void DarkMode_ApplyToResizeGrip(HWND hwndDlg, int id) noexcept {
	HWND hwndGrip = GetDlgItem(hwndDlg, id);
	if (hwndGrip != nullptr && (GetWindowLongPtr(hwndGrip, GWL_STYLE) & SBS_SIZEGRIP) == SBS_SIZEGRIP) {
		SetWindowSubclass(hwndGrip, DarkMode_ResizeGripSubclass, DarkMode_ResizeGripSubclassId, 0);
	}
}

static LRESULT CALLBACK DarkMode_DialogSubclass(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR idSubclass, DWORD_PTR /*refData*/) noexcept {
	switch (msg) {
	case WM_NCDESTROY:
		RemoveWindowSubclass(hwnd, DarkMode_DialogSubclass, idSubclass);
		break;

	case WM_CTLCOLORSTATIC: {
		if (dmlib::isExperimentalActive()) {
			HWND hwndCtl = AsPointer<HWND>(lParam);
			const DWORD id = GetWindowLong(hwndCtl, GWLP_ID);
			if (id >= IDC_MODIFY_LINE_DLN_NP && id <= IDC_MODIFY_LINE_ZCN_ZP) {
				return dmlib::onCtlColorDlgLinkText(AsPointer<HDC>(wParam), true);
			}
		}
	} break;

	case WM_CTLCOLOREDIT:
		if (dmlib::isExperimentalActive()) {
			return dmlib::onCtlColorCtrl(AsPointer<HDC>(wParam));
		}
		break;

	case WM_CTLCOLORLISTBOX:
		if (dmlib::isExperimentalActive()) {
			return dmlib::onCtlColorListbox(wParam, lParam);
		}
		break;
	}
	return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK DarkMode_MessageBoxSubclass(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
	UINT_PTR idSubclass, DWORD_PTR /*refData*/) noexcept {
	switch (msg) {
	case WM_NCDESTROY:
		RemoveWindowSubclass(hwnd, DarkMode_MessageBoxSubclass, idSubclass);
		break;

	case WM_ERASEBKGND:
		if (dmlib::isExperimentalActive()) {
			return TRUE;
		}
		break;

	case WM_PAINT:
		if (dmlib::isExperimentalActive()) {
			PAINTSTRUCT ps{};
			HDC hdc = BeginPaint(hwnd, &ps);
			RECT rcClient{};
			GetClientRect(hwnd, &rcClient);
			FillRect(hdc, &rcClient, dmlib::getBackgroundBrush());

			RECT rcFooterUnit{ 0, 0, 0, 26 };
			MapDialogRect(hwnd, &rcFooterUnit);
			RECT rcContent{ rcClient };
			rcContent.bottom -= rcFooterUnit.bottom;
			if (rcContent.bottom < rcContent.top) {
				rcContent.bottom = rcContent.top;
			}
			FillRect(hdc, &rcContent, dmlib::getDlgBackgroundBrush());
			EndPaint(hwnd, &ps);
			return 0;
		}
		break;

	case WM_CTLCOLORSTATIC:
		if (dmlib::isExperimentalActive()) {
			return dmlib::onCtlColorDlgStaticText(AsPointer<HDC>(wParam), true);
		}
		break;
	}
	return DefSubclassProc(hwnd, msg, wParam, lParam);
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

void DarkMode_Init() noexcept {
	dmlib::initDarkMode();
	dmlib::setDarkModeConfigEx(DarkMode_TypeForStyleTheme(np2StyleTheme));
	dmlib::setDefaultColors(true);
	if (np2StyleTheme == StyleTheme_Dark && dmlib::isExperimentalActive()) {
		DarkMode_SetCustomColors();
	}
}

void DarkMode_Cleanup() noexcept {
}

static void DarkMode_ApplyDialogStyling(HWND hwnd, DWORD_PTR dwRefData) noexcept {
	if (hwnd == nullptr || !dmlib::isExperimentalActive()) {
		return;
	}

	dmlib::setDarkWndNotifySafeEx(hwnd, false, true);
	if (GetProp(hwnd, DarkMode_DialogAppliedProp) == nullptr) {
		DarkMode_ApplyToResizeGrip(hwnd, IDC_RESIZEGRIP);
		DarkMode_ApplyToResizeGrip(hwnd, IDC_RESIZEGRIP2);
		DarkMode_ApplyToResizeGrip(hwnd, IDC_RESIZEGRIP3);
		SetWindowSubclass(hwnd, DarkMode_DialogSubclass, DarkMode_DialogSubclassId, 0);
		SetProp(hwnd, DarkMode_DialogAppliedProp, reinterpret_cast<HANDLE>(1));
	}
	if (dwRefData == DialogRefData_MessageBox) {
		SetWindowSubclass(hwnd, DarkMode_MessageBoxSubclass, DarkMode_MessageBoxSubclassId, 0);
		InvalidateRect(hwnd, nullptr, TRUE);
	}
}

NP2_noinline
void DarkMode_InitDialog(HWND hwnd, DWORD_PTR dwRefData) noexcept {
	if (hwnd != nullptr) {
		SetProp(hwnd, DarkMode_DialogManagedProp, reinterpret_cast<HANDLE>(dwRefData + 1));
	}
	DarkMode_ApplyDialogStyling(hwnd, dwRefData);

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
	if (dmlib::isExperimentalActive()) {
		TreeView_SetTextColor(hwndTV, dmlib::getViewTextColor());
		TreeView_SetBkColor(hwndTV, dmlib::getViewBackgroundColor());
		dmlib::setTreeViewWindowThemeEx(hwndTV, true);
		dmlib::setDarkTooltips(hwndTV, static_cast<int>(dmlib::ToolTipsType::treeview));
	} else if (DarkMode_IsHighContrast()) {
		SetWindowTheme(hwndTV, nullptr, nullptr);
	} else {
		SetWindowTheme(hwndTV, L"Explorer", nullptr);
	}
}

NP2_noinline
void DarkMode_InitFileListView(HWND hwndLV) noexcept {
	InitWindowCommon(hwndLV);
	ListView_SetExtendedListViewStyle(hwndLV, /*LVS_EX_FULLROWSELECT|*/LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP);
	// SetWindowTheme(hwndLV, L"Explorer", nullptr);

	LVCOLUMN lvc{};
	lvc.mask = LVCF_FMT | LVCF_TEXT;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(hwndLV, 0, &lvc);

	if (dmlib::isExperimentalActive()) {
		dmlib::setDarkListView(hwndLV);
		dmlib::setDarkTooltips(hwndLV, static_cast<int>(dmlib::ToolTipsType::listview));
	}
}

void DarkMode_ApplyToWindow(HWND hwnd, bool useWin11Features) noexcept {
	dmlib::setDarkTitleBarEx(hwnd, useWin11Features);
	dmlib::setWindowMenuBarSubclass(hwnd);
}

void DarkMode_ApplyToBars(HWND hwnd, HWND hwndToolbar, HWND hwndReBar, HWND hwndStatus) noexcept {
	dmlib::setDarkLineAbovePanelToolbar(hwndToolbar);
	dmlib::setStatusBarCtrlSubclass(hwndStatus);
	dmlib::setWindowEraseBgSubclass(hwndReBar);
	dmlib::setWindowNotifyCustomDrawSubclass(hwnd);
	dmlib::setDarkScrollBar(hwndEdit);
	dmlib::setDarkTooltips(hwndToolbar, static_cast<int>(dmlib::ToolTipsType::toolbar));
}

void DarkMode_OnToolbarBeginAdjust() noexcept {
	DialogHook_Start(DialogRefData_CustomizeToolbar);
	if (dmlib::isExperimentalActive()) {
		dmlib_hook::hookSysColor();
	}
}

void DarkMode_OnToolbarEndAdjust() noexcept {
	dmlib_hook::unhookSysColor();
	DialogHook_Stop();
}

void DarkMode_OnThemeChanged(int newTheme) noexcept {
	dmlib::setDarkModeConfigEx(DarkMode_TypeForStyleTheme(newTheme));
	dmlib::setDefaultColors(true);
	if (newTheme == StyleTheme_Dark && dmlib::isExperimentalActive()) {
		DarkMode_SetCustomColors();
	} else {
		dmlib::setSysColor(COLOR_WINDOW, GetSysColor(COLOR_WINDOW));
		dmlib::setSysColor(COLOR_WINDOWTEXT, GetSysColor(COLOR_WINDOWTEXT));
		dmlib::setSysColor(COLOR_BTNFACE, GetSysColor(COLOR_BTNFACE));
	}
}

static BOOL CALLBACK DarkMode_SendThemeChangedProc(HWND hwnd, LPARAM /*lParam*/) noexcept {
	WCHAR className[16] = L"";
	GetClassName(hwnd, className, COUNTOF(className));
	if (StrEqual(className, WC_TREEVIEW)) {
		if (dmlib::isExperimentalActive()) {
			TreeView_SetTextColor(hwnd, dmlib::getViewTextColor());
			TreeView_SetBkColor(hwnd, dmlib::getViewBackgroundColor());
			dmlib::setTreeViewWindowThemeEx(hwnd, true);
		} else if (DarkMode_IsHighContrast()) {
			TreeView_SetTextColor(hwnd, GetSysColor(COLOR_WINDOWTEXT));
			TreeView_SetBkColor(hwnd, GetSysColor(COLOR_WINDOW));
			LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
			if ((style & TVS_TRACKSELECT) == TVS_TRACKSELECT) {
				SetWindowLongPtr(hwnd, GWL_STYLE, style & ~TVS_TRACKSELECT);
			}
			SetWindowTheme(hwnd, nullptr, nullptr);
		} else {
			TreeView_SetTextColor(hwnd, GetSysColor(COLOR_WINDOWTEXT));
			TreeView_SetBkColor(hwnd, GetSysColor(COLOR_WINDOW));
			dmlib::setTreeViewWindowThemeEx(hwnd, true);
		}
	} else if (StrEqual(className, WC_LISTVIEW)) {
		if (dmlib::isExperimentalActive()) {
			ListView_SetTextColor(hwnd, dmlib::getViewTextColor());
			ListView_SetTextBkColor(hwnd, dmlib::getViewBackgroundColor());
			ListView_SetBkColor(hwnd, dmlib::getViewBackgroundColor());
			dmlib::setDarkListView(hwnd);
		} else {
			ListView_SetTextColor(hwnd, GetSysColor(COLOR_WINDOWTEXT));
			ListView_SetTextBkColor(hwnd, GetSysColor(COLOR_WINDOW));
			ListView_SetBkColor(hwnd, GetSysColor(COLOR_WINDOW));
			SetWindowTheme(hwnd, DarkMode_IsHighContrast() ? nullptr : L"Explorer", nullptr);
		}
	}
	SendMessage(hwnd, WM_THEMECHANGED, 0, 0);
	return TRUE;
}

static bool DarkMode_IsOwnedBy(HWND hwnd, HWND hwndOwner) noexcept {
	for (HWND owner = GetWindow(hwnd, GW_OWNER); owner != nullptr; owner = GetWindow(owner, GW_OWNER)) {
		if (owner == hwndOwner || GetAncestor(owner, GA_ROOT) == hwndOwner) {
			return true;
		}
	}
	return false;
}

static BOOL CALLBACK DarkMode_RefreshTopLevelProc(HWND hwnd, LPARAM lParam) noexcept {
	const HWND hwndMain = AsPointer<HWND>(lParam);
	const HANDLE managed = GetProp(hwnd, DarkMode_DialogManagedProp);
	const bool isAppWindow = AsPointer<HINSTANCE>(GetWindowLongPtr(hwnd, GWLP_HINSTANCE)) == g_hInstance;
	if (hwnd != hwndMain && DarkMode_IsOwnedBy(hwnd, hwndMain) && (isAppWindow || managed != nullptr)) {
		const DWORD_PTR dwRefData = (managed != nullptr)
			? reinterpret_cast<DWORD_PTR>(managed) - 1
			: DialogRefData_DefaultPosition;
		if (dmlib::isExperimentalActive()) {
			DarkMode_ApplyDialogStyling(hwnd, dwRefData);
		} else if (DarkMode_IsHighContrast()) {
			dmlib::setDarkTitleBarEx(hwnd, true);
			dmlib::setChildCtrlsTheme(hwnd);
		} else {
			dmlib::setDarkWndNotifySafeEx(hwnd, false, true);
		}
		SendMessage(hwnd, WM_THEMECHANGED, 0, 0);
		EnumChildWindows(hwnd, DarkMode_SendThemeChangedProc, 0);
		RedrawWindow(hwnd, nullptr, nullptr,
			RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_FRAME);
	}
	return TRUE;
}

void DarkMode_BroadcastThemeChanged(HWND hwnd) noexcept {
	if (hwnd != nullptr) {
		SendMessage(hwnd, WM_THEMECHANGED, 0, 0);
		EnumChildWindows(hwnd, DarkMode_SendThemeChangedProc, 0);
		EnumThreadWindows(GetCurrentThreadId(), DarkMode_RefreshTopLevelProc, AsInteger<LPARAM>(hwnd));
	}
}

bool DarkMode_IsEnabled() noexcept {
	return dmlib::isExperimentalActive();
}

void DarkMode_FillDialogWithFooter(HWND hwnd, HDC hdc, HWND hwndMainArea) noexcept {
	if (!dmlib::isExperimentalActive()) {
		return;
	}

	RECT rcClient{};
	GetClientRect(hwnd, &rcClient);
	FillRect(hdc, &rcClient, dmlib::getBackgroundBrush());
	if (hwndMainArea != nullptr) {
		RECT rcMain{};
		GetWindowRect(hwndMainArea, &rcMain);
		MapWindowPoints(HWND_DESKTOP, hwnd, reinterpret_cast<LPPOINT>(&rcMain), 2);
		FillRect(hdc, &rcMain, dmlib::getDlgBackgroundBrush());
	}
}

LRESULT DarkMode_OnCtlColorDlgStaticText(HDC hdc, bool isTextEnabled) noexcept {
	return dmlib::onCtlColorDlgStaticText(hdc, isTextEnabled);
}

// ---------------------------------------------------------------------------
// External DPI provider for darkmodelib.
//
// Notepad4 already loads the per-monitor DPI v2 functions via
// Scintilla_LoadDpiForWindow() and exposes them through GetWindowDPI(),
// SystemMetricsForDpi(), AdjustWindowRectForDpi() and g_uSystemDPI. To avoid
// duplicating that loader inside the vendored library, DmlibDpi.cpp is compiled
// out of Notepad4's build and the dmlib_dpi functions the library uses are
// implemented here, backed by Notepad4's helpers. Only the two
// functions Notepad4 does not already load (SystemParametersInfoForDpi,
// OpenThemeDataForDpi) are resolved here, with fallbacks for pre-Windows 10.
//
// Calls to Notepad4's helpers are written with a leading "::" so the
// Windows-10 function-like macros expand to the global Win32 APIs instead of
// recursing into these dmlib_dpi members.
// ---------------------------------------------------------------------------
namespace {
using SystemParametersInfoForDpiSig = BOOL (WINAPI *)(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni, UINT dpi);
using OpenThemeDataForDpiSig = HTHEME (WINAPI *)(HWND hwnd, LPCWSTR pszClassList, UINT dpi);
SystemParametersInfoForDpiSig pfnSystemParametersInfoForDpi = nullptr;
OpenThemeDataForDpiSig pfnOpenThemeDataForDpi = nullptr;
}

bool dmlib_dpi::InitDpiAPI() noexcept {
	// The core per-monitor DPI functions are already loaded by Notepad4
	// (Scintilla_LoadDpiForWindow). Resolve only the two extras darkmodelib
	// needs that Notepad4 does not load itself.
	pfnSystemParametersInfoForDpi = DLLFunctionEx<SystemParametersInfoForDpiSig>(L"user32.dll", "SystemParametersInfoForDpi");
	pfnOpenThemeDataForDpi = DLLFunctionEx<OpenThemeDataForDpiSig>(L"uxtheme.dll", "OpenThemeDataForDpi");
	return true;
}

UINT dmlib_dpi::GetDpiForSystem() noexcept {
	return g_uSystemDPI;
}

UINT dmlib_dpi::GetDpiForWindow(HWND hWnd) noexcept {
	if (hWnd != nullptr) {
		const UINT dpi = ::GetWindowDPI(hWnd);
		if (dpi > 0) {
			return dpi;
		}
	}
	return g_uSystemDPI;
}

int dmlib_dpi::GetSystemMetricsForDpi(int nIndex, UINT dpi) noexcept {
	return ::SystemMetricsForDpi(nIndex, dpi);
}

BOOL dmlib_dpi::AdjustWindowRectExForDpi(LPRECT lpRect, DWORD dwStyle, BOOL /*bMenu*/, DWORD dwExStyle, UINT dpi) noexcept {
	// darkmodelib only ever calls this with bMenu = FALSE, matching the wrapper.
	return ::AdjustWindowRectForDpi(lpRect, dwStyle, dwExStyle, dpi);
}

HTHEME dmlib_dpi::OpenThemeDataForDpi(HWND hwnd, LPCWSTR pszClassList, UINT dpi) noexcept {
	if (pfnOpenThemeDataForDpi != nullptr) {
		return pfnOpenThemeDataForDpi(hwnd, pszClassList, dpi);
	}
	return ::OpenThemeData(hwnd, pszClassList);
}

LOGFONT dmlib_dpi::getSysFontForDpi(UINT dpi, FontType type) noexcept {
	LOGFONT lf{};
	NONCLIENTMETRICS ncm{};
	ncm.cbSize = sizeof(NONCLIENTMETRICS);

	const BOOL ok = (pfnSystemParametersInfoForDpi != nullptr)
		? pfnSystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0, dpi)
		: ::SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

	if (ok == TRUE) {
		switch (type) {
		case FontType::menu:      lf = ncm.lfMenuFont; break;
		case FontType::status:    lf = ncm.lfStatusFont; break;
		case FontType::message:   lf = ncm.lfMessageFont; break;
		case FontType::caption:   lf = ncm.lfCaptionFont; break;
		case FontType::smcaption: lf = ncm.lfSmCaptionFont; break;
		}
	} else { // should not happen, fallback
		HFONT hf = static_cast<HFONT>(::GetStockObject(DEFAULT_GUI_FONT));
		::GetObjectW(hf, sizeof(LOGFONT), &lf);
		lf.lfHeight = scaleFontForDpi(lf.lfHeight, dpi);
	}
	return lf;
}
