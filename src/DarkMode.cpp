// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
//
// Dark mode integration using darkmodelib (https://github.com/ozone10/win32-darkmodelib)
// Licensed under MPL-2.0.

#include "Darkmodelib.h"
#include "DarkMode.h"
#include "EditLexer.h"

// Declared in Styles.cpp
extern int np2StyleTheme;

// Thread-level hook to intercept WM_INITDIALOG and apply dark mode to dialogs
static HHOOK s_hCallWndProcRetHook = nullptr;

static LRESULT CALLBACK DarkMode_CallWndRetProc(int nCode, WPARAM wParam, LPARAM lParam) noexcept {
	if (nCode == HC_ACTION) {
		const CWPRETSTRUCT *cwpret = reinterpret_cast<const CWPRETSTRUCT *>(lParam);
		if (cwpret->message == WM_INITDIALOG) {
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
		dmlib::setDarkModeConfigEx(static_cast<UINT>(dmlib::DarkModeType::dark));
	} else {
		dmlib::setDarkModeConfigEx(static_cast<UINT>(dmlib::DarkModeType::light));
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

void DarkMode_ApplyToDialog(HWND hDlg) noexcept {
	dmlib::setDarkWndNotifySafeEx(hDlg, false, true);
}

void DarkMode_OnThemeChanged(int newTheme) noexcept {
	if (newTheme == StyleTheme_Dark) {
		dmlib::setDarkModeConfigEx(static_cast<UINT>(dmlib::DarkModeType::dark));
	} else {
		dmlib::setDarkModeConfigEx(static_cast<UINT>(dmlib::DarkModeType::light));
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
