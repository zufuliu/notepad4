// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
//
// Dark mode integration using darkmodelib (https://github.com/ozone10/win32-darkmodelib)
// Licensed under MPL-2.0.
#pragma once

#include <windows.h>

// Initialize dark mode support. Call once at startup after LoadSettings().
void DarkMode_Init() noexcept;

// Clean up dark mode resources. Call during WM_DESTROY.
void DarkMode_Cleanup() noexcept;

// Apply dark mode to a top-level window (title bar, menu bar).
void DarkMode_ApplyToWindow(HWND hwnd, bool useWin11Features = true) noexcept;

// Apply dark mode to the main window bars.
void DarkMode_ApplyToBars(HWND hwnd, HWND hwndToolbar, HWND hwndReBar, HWND hwndStatus) noexcept;

// Apply dark mode to a dialog (WM_INITDIALOG handler).
void DarkMode_ApplyToDialog(HWND hDlg) noexcept;

// Apply dark-mode colors to a TreeView before it is populated.
void DarkMode_ApplyToTreeView(HWND hwndTreeView) noexcept;

// Show the comctl32 Customize Toolbar dialog with dark-mode theming applied.
void DarkMode_CustomizeToolbar(HWND hwndToolbar) noexcept;

// Update dark mode after style theme change. Call from Style_OnStyleThemeChanged().
void DarkMode_OnThemeChanged(int newTheme) noexcept;

// Handle WM_SETTINGCHANGE for system dark/light mode changes.
// Returns true if the message was handled.
bool DarkMode_HandleSettingChange(HWND hwnd, LPARAM lParam) noexcept;

// Check if dark mode UI is currently active.
bool DarkMode_IsEnabled() noexcept;

// Fill a dialog whose main content area should differ from its footer/button area.
void DarkMode_FillDialogWithFooter(HWND hwnd, HDC hdc, HWND hwndMainArea) noexcept;

// Handle custom WM_CTLCOLORSTATIC code paths that cannot use the generic dialog subclass.
LRESULT DarkMode_OnCtlColorDlgStaticText(HDC hdc, bool isTextEnabled) noexcept;

// Broadcast WM_THEMECHANGED to the window and all its descendants so that
// themed controls (including scroll bars) re-open their theme handles.
void DarkMode_BroadcastThemeChanged(HWND hwnd) noexcept;
