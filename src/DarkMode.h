// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#pragma once

enum DialogRefData {
	DialogRefData_CenterParent,
	DialogRefData_MessageBox,
	DialogRefData_RightBottom,
	DialogRefData_DefaultPosition,
	DialogRefData_CustomizeToolbar,
	DialogRefData_MaxValue,
};

void DialogHook_Start(DWORD_PTR dwRefData) noexcept;
void DialogHook_Stop() noexcept;
void DarkMode_Init() noexcept;
void DarkMode_Cleanup() noexcept;
void DarkMode_InitDialog(HWND hwnd, DWORD_PTR dwRefData = DialogRefData_CenterParent) noexcept;
void DarkMode_InitTreeView(HWND hwndTV) noexcept;
void DarkMode_InitFileListView(HWND hwndLV) noexcept;
void DarkMode_ApplyToWindow(HWND hwnd, bool useWin11Features = true) noexcept;
void DarkMode_ApplyToBars(HWND hwnd, HWND hwndToolbar, HWND hwndReBar, HWND hwndStatus) noexcept;
void DarkMode_OnToolbarBeginAdjust() noexcept;
void DarkMode_OnToolbarEndAdjust() noexcept;
void DarkMode_OnThemeChanged(int newTheme) noexcept;
bool DarkMode_IsEnabled() noexcept;
void DarkMode_FillDialogWithFooter(HWND hwnd, HDC hdc, HWND hwndMainArea) noexcept;
LRESULT DarkMode_OnCtlColorDlgStaticText(HDC hdc, bool isTextEnabled) noexcept;
void DarkMode_BroadcastThemeChanged(HWND hwnd) noexcept;
