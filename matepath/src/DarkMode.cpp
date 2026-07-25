// This file is part of Notepad4.
// See License.txt for details about distribution and modification.

#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <vsstyle.h>
#include <vssym32.h>
#include "config.h"
#include "Helpers.h"
#include "DarkMode.h"
#include "Dialogs.h"

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
}

void DarkMode_Cleanup() noexcept {
}

NP2_noinline
void DarkMode_InitDialog(HWND hwnd, DWORD_PTR dwRefData) noexcept {
	if (dwRefData < DialogRefData_DefaultPosition) {
		CenterDlgInParent(hwnd);
		if (dwRefData == DialogRefData_MessageBox) {
			SnapToDefaultButton(hwnd);
		}
	}
}

NP2_noinline
void DarkMode_InitFileListView(HWND hwndLV, DWORD exStyle) noexcept {
	InitWindowCommon(hwndLV);
	exStyle |= LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP;
	ListView_SetExtendedListViewStyle(hwndLV, exStyle);
	if (exStyle & LVS_EX_FULLROWSELECT) {
		SetWindowTheme(hwndLV, L"Explorer", nullptr);
	}

	LVCOLUMN lvc{};
	lvc.mask = LVCF_FMT | LVCF_TEXT;
	lvc.fmt = LVCFMT_LEFT;
	ListView_InsertColumn(hwndLV, 0, &lvc);
}
