// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
//
// Dark mode integration using darkmodelib (https://github.com/ozone10/win32-darkmodelib)
// Licensed under MPL-2.0.

#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commdlg.h>
#include <uxtheme.h>
#include "config.h"
#include "Helpers.h"
#include "Darkmodelib.h"
#include "DarkMode.h"
#include "EditLexer.h"
#include "DmlibDpi.h"

// Declared in Styles.cpp
extern int np2StyleTheme;
// Declared in Notepad4.cpp - this app's module handle
extern HINSTANCE g_hInstance;
// Declared in Notepad4.cpp - the Scintilla edit window
extern HWND hwndEdit;

// Thread-level hook to intercept WM_INITDIALOG and apply dark mode to dialogs
static HHOOK s_hCallWndProcRetHook = nullptr;

static constexpr UINT DarkMode_TypeForStyleTheme(int theme) noexcept {
	return static_cast<UINT>((theme == StyleTheme_Dark) ? dmlib::DarkModeType::dark : dmlib::DarkModeType::light);
}

static LRESULT CALLBACK DarkMode_CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam) noexcept {
	if (nCode == HC_ACTION) {
		const CWPRETSTRUCT *cwpret = reinterpret_cast<const CWPRETSTRUCT *>(lParam);
		// Only theme dialogs created by this app while dark mode is active.
		// This avoids touching system dialogs (e.g. the common file open/save
		// dialog) that are hosted in our process but owned by comdlg32/shell.
		if (cwpret->message == WM_INITDIALOG
			&& dmlib::isExperimentalActive()
			&& reinterpret_cast<HINSTANCE>(GetWindowLongPtr(cwpret->hwnd, GWLP_HINSTANCE)) == g_hInstance) {
			DarkMode_ApplyToDialog(cwpret->hwnd);
		}
	}
	return CallNextHookEx(s_hCallWndProcRetHook, nCode, wParam, lParam);
}

// Custom dark colors for Notepad4
// Creates visual hierarchy: editor (#1E1E1E) < toolbar/statusbar (#2D2D2D) < controls (#383838)
static constexpr COLORREF HEXRGB(DWORD rrggbb) noexcept {
	return ((rrggbb & 0xFF0000) >> 16) | (rrggbb & 0x00FF00) | ((rrggbb & 0x0000FF) << 16);
}

static void DarkMode_SetCustomColors() noexcept {
	dmlib::setBackgroundColor(HEXRGB(0x2D2D2D));       // toolbar, rebar
	dmlib::setCtrlBackgroundColor(HEXRGB(0x383838));    // interactive controls (edit, combo)
	dmlib::setHotBackgroundColor(HEXRGB(0x454545));     // hover states
	dmlib::setDlgBackgroundColor(HEXRGB(0x252526));     // dialog backgrounds
	dmlib::setTextColor(HEXRGB(0xD4D4D4));              // match editor text
	dmlib::setDarkerTextColor(HEXRGB(0xC0C0C0));
	dmlib::setDisabledTextColor(HEXRGB(0x808080));
	dmlib::setEdgeColor(HEXRGB(0x555555));
	dmlib::setHotEdgeColor(HEXRGB(0x9B9B9B));
	dmlib::setDisabledEdgeColor(HEXRGB(0x484848));
	dmlib::updateThemeBrushesAndPens();

	// View colors for list boxes, list views, tree views
	dmlib::setViewBackgroundColor(HEXRGB(0x1E1E1E));
	dmlib::setViewTextColor(HEXRGB(0xD4D4D4));
	dmlib::setViewGridlinesColor(HEXRGB(0x3C3C3C));
	dmlib::setHeaderBackgroundColor(HEXRGB(0x2D2D2D));
	dmlib::setHeaderTextColor(HEXRGB(0xD4D4D4));
	dmlib::setHeaderEdgeColor(HEXRGB(0x555555));
	dmlib::updateViewBrushesAndPens();

	// Override system colors used by system-drawn list boxes (e.g. toolbar customize dialog)
	dmlib::setSysColor(COLOR_WINDOW, HEXRGB(0x1E1E1E));
	dmlib::setSysColor(COLOR_WINDOWTEXT, HEXRGB(0xD4D4D4));
	dmlib::setSysColor(COLOR_BTNFACE, HEXRGB(0x3C3C3C));
}

void DarkMode_Init() noexcept {
	dmlib::initDarkMode();

	if (np2StyleTheme == StyleTheme_Dark) {
		dmlib::setDarkModeConfigEx(DarkMode_TypeForStyleTheme(np2StyleTheme));
	} else {
		dmlib::setDarkModeConfigEx(DarkMode_TypeForStyleTheme(np2StyleTheme));
	}
	dmlib::setDefaultColors(true);

	if (np2StyleTheme == StyleTheme_Dark) {
		DarkMode_SetCustomColors();
	}

	// Install a thread-level hook to automatically apply dark mode to all dialogs
	s_hCallWndProcRetHook = SetWindowsHookEx(WH_CALLWNDPROCRET, DarkMode_CallWndRetProc,
		nullptr, GetCurrentThreadId());
	// Hook failure is non-fatal: dialogs won't get automatic dark mode theming
}

void DarkMode_Cleanup() noexcept {
	if (s_hCallWndProcRetHook) {
		UnhookWindowsHookEx(s_hCallWndProcRetHook);
		s_hCallWndProcRetHook = nullptr;
	}
}

void DarkMode_ApplyToWindow(HWND hwnd, bool useWin11Features) noexcept {
	dmlib::setDarkTitleBarEx(hwnd, useWin11Features);
	dmlib::setWindowMenuBarSubclass(hwnd);
	dmlib::setWindowSettingChangeSubclass(hwnd);
}

void DarkMode_ApplyToBars(HWND hwnd, HWND hwndToolbar, HWND hwndReBar, HWND hwndStatus) noexcept {
	dmlib::setDarkLineAbovePanelToolbar(hwndToolbar);
	dmlib::setStatusBarCtrlSubclass(hwndStatus);
	dmlib::setWindowEraseBgSubclass(hwndReBar);
	dmlib::setWindowNotifyCustomDrawSubclass(hwnd);

	// Apply the dark/light "DarkMode_Explorer" theme directly to the Scintilla
	// edit window so its scroll bars follow the current mode. setDarkScrollBar()
	// sets the dark theme when dark mode is active and resets it otherwise, so it
	// works for both directions without any per-window hook state tracking.
	dmlib::setDarkScrollBar(hwndEdit);

	dmlib::setDarkTooltips(hwndToolbar, static_cast<int>(dmlib::ToolTipsType::toolbar));
}

void DarkMode_ApplyToDialog(HWND hDlg) noexcept {
	dmlib::setDarkWndNotifySafeEx(hDlg, false, true);
}

void DarkMode_OnThemeChanged(int newTheme) noexcept {
	if (newTheme == StyleTheme_Dark) {
		dmlib::setDarkModeConfigEx(DarkMode_TypeForStyleTheme(newTheme));
	} else {
		dmlib::setDarkModeConfigEx(DarkMode_TypeForStyleTheme(newTheme));
	}
	dmlib::setDefaultColors(true);

	if (newTheme == StyleTheme_Dark) {
		DarkMode_SetCustomColors();
	} else {
		// Reset overridden system colors to actual system values
		dmlib::setSysColor(COLOR_WINDOW, GetSysColor(COLOR_WINDOW));
		dmlib::setSysColor(COLOR_WINDOWTEXT, GetSysColor(COLOR_WINDOWTEXT));
		dmlib::setSysColor(COLOR_BTNFACE, GetSysColor(COLOR_BTNFACE));
	}
}

// Broadcast WM_THEMECHANGED to a window and all its descendants so that
// themed controls (including scroll bars) re-open their theme handles.
static BOOL CALLBACK DarkMode_SendThemeChangedProc(HWND hwnd, LPARAM /*lParam*/) noexcept {
	SendMessage(hwnd, WM_THEMECHANGED, 0, 0);
	return TRUE;
}

void DarkMode_BroadcastThemeChanged(HWND hwnd) noexcept {
	if (hwnd == nullptr) {
		return;
	}
	SendMessage(hwnd, WM_THEMECHANGED, 0, 0);
	EnumChildWindows(hwnd, DarkMode_SendThemeChangedProc, 0);
}

bool DarkMode_HandleSettingChange([[maybe_unused]] HWND hwnd, LPARAM lParam) noexcept {
	return dmlib::handleSettingChange(lParam);
}

bool DarkMode_IsEnabled() noexcept {
	return dmlib::isExperimentalActive();
}

int DarkMode_MessageBox(HWND hwnd, LPCWSTR text, LPCWSTR caption, UINT uType, WORD wLanguageId) noexcept {
	if (dmlib::isExperimentalActive()) {
		const HRESULT hr = dmlib::darkMessageBoxW(hwnd, text, caption, uType);
		if (hr > 0) {
			return static_cast<int>(hr);
		}
		// Fall through to MessageBoxEx on failure.
	}
	return MessageBoxEx(hwnd, text, caption, uType, wLanguageId);
}

// ---------------------------------------------------------------------------
// External DPI provider for darkmodelib (_DARKMODELIB_EXTERNAL_DPI)
//
// Notepad4 already loads the per-monitor DPI v2 functions via
// Scintilla_LoadDpiForWindow() and exposes them through GetWindowDPI(),
// SystemMetricsForDpi(), AdjustWindowRectForDpi() and g_uSystemDPI. To avoid
// duplicating that loader inside the vendored library, DmlibDpi.cpp is compiled
// out (via _DARKMODELIB_EXTERNAL_DPI) and the dmlib_dpi functions the library
// uses are implemented here, backed by Notepad4's helpers. Only the two
// functions Notepad4 does not already load (SystemParametersInfoForDpi,
// OpenThemeDataForDpi) are resolved here, with fallbacks for pre-Windows 10.
//
// Calls to Notepad4's helpers are written with a leading "::" so the
// Windows-10 function-like macros expand to the global Win32 APIs instead of
// recursing into these dmlib_dpi members.
// ---------------------------------------------------------------------------
#ifdef _DARKMODELIB_EXTERNAL_DPI

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

#endif // _DARKMODELIB_EXTERNAL_DPI
