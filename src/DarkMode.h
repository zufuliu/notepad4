// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#pragma once

enum DialogRefData {
	DialogRefData_CenterParent,
	DialogRefData_MessageBox,
	DialogRefData_RightBottom,
	DialogRefData_DefaultPosition,
	DialogRefData_CustomizeToolbar,
	DialogRefData_ChooseColor,
	DialogRefData_ChooseFont,
	DialogRefData_PrintPageSetup,
	DialogRefData_MaxValue,
};

enum DarkModeStatus {
	DarkModeStatus_None = 0,
	DarkModeStatus_Active = 1,
	DarkModeStatus_Supported = 4,
	DarkModeStatus_SupportDarkTheme = 8,
	DarkModeStatus_PriorWindows10_1903 = 16,
	DarkModeStatus_IsWindows11 = 32,
	DarkModeStatus_GlobalInitialized = 64,
};

#define WM_UAHDRAWMENU		0x0091 // lParam is UAHMENU
#define WM_UAHDRAWMENUITEM	0x0092 // lParam is UAHDRAWMENUITEM

extern UINT g_DarkModeStatus;
inline BOOL DarkMode_IsActive() noexcept {
	return g_DarkModeStatus & DarkModeStatus_Active;
}
inline BOOL DarkMode_IsSupported() noexcept {
	return g_DarkModeStatus & DarkModeStatus_Supported;
}
inline BOOL DarkMode_IsWindows11() noexcept {
	return g_DarkModeStatus & DarkModeStatus_IsWindows11;
}
inline BOOL DarkMode_SupportDarkTheme() noexcept {
	return g_DarkModeStatus & DarkModeStatus_SupportDarkTheme;
}

void DialogHook_Start(DWORD_PTR dwRefData) noexcept;
void DialogHook_Stop() noexcept;
void DarkMode_Init(const OSVERSIONINFOW &version) noexcept;
void DarkMode_Cleanup() noexcept;
void DarkMode_SetAppDarkMode(BOOL dark) noexcept;
void DarkMode_SetDarkTitleBar(HWND hwnd, BOOL dark) noexcept;
void DarkMode_HandleSettingChange(HWND hwnd, LPARAM lParam) noexcept;
int DarkMode_ResolveStyleTheme(int theme) noexcept;
void DarkMode_OnDarkModeChange(HWND hwnd) noexcept;
void DarkMode_ApplyToBars(HWND hwndToolbar, HWND hwndStatus) noexcept;
void DarkMode_OnToolbarBeginAdjust() noexcept;
void DarkMode_OnToolbarEndAdjust() noexcept;
LRESULT DarkMode_DrawUAHMenu(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam) noexcept;
void DarkMode_DrawUAHMenuNCBottomLine(HWND hwnd) noexcept;
LRESULT DarkMode_RebarCustomDraw(LPARAM lParam) noexcept;
LRESULT DarkMode_ToolbarCustomDraw(LPARAM lParam) noexcept;
void DarkMode_InitDialog(HWND hwnd, DWORD_PTR dwRefData = DialogRefData_CenterParent) noexcept;
void DarkMode_InitTreeView(HWND hwndTV) noexcept;
void DarkMode_InitFileListView(HWND hwndLV) noexcept;
#define DarkMode_UseDarkScrollBar(hwnd)		SetWindowTheme((hwnd), L"DarkMode_Explorer", nullptr)
