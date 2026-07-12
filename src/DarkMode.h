// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#pragma once

enum DialogRefData {
	DialogRefData_None,
	DialogRefData_MsgBox,
	DialogRefData_NotCenter,
	DialogRefData_RightBottom,
	DialogRefData_MaxValue,
};

void DialogHook_Start(DWORD_PTR dwRefData) noexcept;
void DialogHook_Stop() noexcept;
void DarkMode_Init() noexcept;
void DarkMode_Cleanup() noexcept;
void DarkMode_InitDialog(HWND hwnd, DWORD_PTR dwRefData = DialogRefData_None) noexcept;
void DarkMode_InitTreeView(HWND hwndTV) noexcept;
void DarkMode_InitFileListView(HWND hwndLV) noexcept;
