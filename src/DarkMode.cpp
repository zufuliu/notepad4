// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
//
// Dark mode integration using darkmodelib (https://github.com/ozone10/win32-darkmodelib)
// Licensed under MPL-2.0.

#include "DarkModeSubclass.h"
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
	DarkMode::setBackgroundColor(HEXRGB(0x2D2D2D));       // toolbar, rebar
	DarkMode::setCtrlBackgroundColor(HEXRGB(0x383838));    // interactive controls (edit, combo)
	DarkMode::setHotBackgroundColor(HEXRGB(0x454545));     // hover states
	DarkMode::setDlgBackgroundColor(HEXRGB(0x252526));     // dialog backgrounds
	DarkMode::setTextColor(HEXRGB(0xD4D4D4));              // match editor text
	DarkMode::setDarkerTextColor(HEXRGB(0xC0C0C0));
	DarkMode::setDisabledTextColor(HEXRGB(0x808080));
	DarkMode::setEdgeColor(HEXRGB(0x555555));
	DarkMode::setHotEdgeColor(HEXRGB(0x9B9B9B));
	DarkMode::setDisabledEdgeColor(HEXRGB(0x484848));
	DarkMode::updateThemeBrushesAndPens();

	// View colors for list boxes, list views, tree views
	DarkMode::setViewBackgroundColor(HEXRGB(0x1E1E1E));
	DarkMode::setViewTextColor(HEXRGB(0xD4D4D4));
	DarkMode::setViewGridlinesColor(HEXRGB(0x3C3C3C));
	DarkMode::setHeaderBackgroundColor(HEXRGB(0x2D2D2D));
	DarkMode::setHeaderTextColor(HEXRGB(0xD4D4D4));
	DarkMode::setHeaderEdgeColor(HEXRGB(0x555555));
	DarkMode::updateViewBrushesAndPens();

	// Override system colors used by system-drawn list boxes (e.g. toolbar customize dialog)
	DarkMode::setSysColor(COLOR_WINDOW, HEXRGB(0x1E1E1E));
	DarkMode::setSysColor(COLOR_WINDOWTEXT, HEXRGB(0xD4D4D4));
	DarkMode::setSysColor(COLOR_BTNFACE, HEXRGB(0x3C3C3C));
}

void DarkMode_Init() noexcept {
	DarkMode::initDarkMode();

	if (np2StyleTheme == StyleTheme_Dark) {
		DarkMode::setDarkModeConfigEx(static_cast<UINT>(DarkMode::DarkModeType::dark));
	} else {
		DarkMode::setDarkModeConfigEx(static_cast<UINT>(DarkMode::DarkModeType::light));
	}
	DarkMode::setDefaultColors(true);

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
	DarkMode::setDarkTitleBarEx(hwnd, useWin11Features);
	DarkMode::setWindowMenuBarSubclass(hwnd);
	DarkMode::setWindowSettingChangeSubclass(hwnd);
}

void DarkMode_ApplyToDialog(HWND hDlg) noexcept {
	DarkMode::setDarkWndNotifySafeEx(hDlg, false, true);
}

void DarkMode_OnThemeChanged(int newTheme) noexcept {
	if (newTheme == StyleTheme_Dark) {
		DarkMode::setDarkModeConfigEx(static_cast<UINT>(DarkMode::DarkModeType::dark));
	} else {
		DarkMode::setDarkModeConfigEx(static_cast<UINT>(DarkMode::DarkModeType::light));
	}
	DarkMode::setDefaultColors(true);

	if (newTheme == StyleTheme_Dark) {
		DarkMode_SetCustomColors();
	} else {
		// Reset overridden system colors to actual system values
		DarkMode::setSysColor(COLOR_WINDOW, GetSysColor(COLOR_WINDOW));
		DarkMode::setSysColor(COLOR_WINDOWTEXT, GetSysColor(COLOR_WINDOWTEXT));
		DarkMode::setSysColor(COLOR_BTNFACE, GetSysColor(COLOR_BTNFACE));
	}
}

bool DarkMode_HandleSettingChange([[maybe_unused]] HWND hwnd, LPARAM lParam) noexcept {
	return DarkMode::handleSettingChange(lParam);
}

bool DarkMode_IsEnabled() noexcept {
	return DarkMode::isExperimentalActive();
}
