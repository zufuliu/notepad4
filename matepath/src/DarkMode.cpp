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

DialogHook dialogHook {nullptr, DialogRefData_None};
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
	if (!dialogHook.hook) {
		dialogHook.Start();
	}
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
	if (dwRefData < DialogRefData_NotCenter) {
		CenterDlgInParent(hwnd);
		if (dwRefData == DialogRefData_MsgBox) {
			SnapToDefaultButton(hwnd);
		}
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
}
