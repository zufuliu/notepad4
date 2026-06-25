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

namespace { // DialogHook

struct DialogHook {
	HHOOK hook;
	DWORD_PTR dwRefData;

	void Start() noexcept {
		hook = SetWindowsHookEx(WH_CALLWNDPROCRET, DialogHook::HookProc, nullptr, GetCurrentThreadId());
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
LRESULT CALLBACK DialogHook::HookProc(int nCode, WPARAM wParam, LPARAM lParam) noexcept {
	if (nCode == HC_ACTION) {
		const auto *cwpret = AsPointer<const CWPRETSTRUCT *>(lParam);
		if (cwpret->message == WM_INITDIALOG) {
			const auto current = dialogHook;
			memset(&dialogHook, 0, sizeof(dialogHook));
			UnhookWindowsHookEx(current.hook);
			if (current.dwRefData > DialogRefData_MaxValue) {
				SetWindowSubclass(cwpret->hwnd, FileDialog::SubProc, 0, current.dwRefData);
			} else {
				DarkMode_InitDialog(cwpret->hwnd, current.dwRefData);
			}
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
}

void DarkMode_Cleanup() noexcept {
}

NP2_noinline
void DarkMode_InitDialog(HWND hwnd, DWORD_PTR dwRefData) noexcept {
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
	SetWindowTheme(hwndTV, L"Explorer", nullptr);
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
