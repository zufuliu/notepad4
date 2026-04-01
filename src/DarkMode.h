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

// Apply dark mode to a top-level window (title bar, menu bar, control subclassing).
void DarkMode_ApplyToWindow(HWND hwnd, bool useWin11Features = true) noexcept;

// Apply dark mode to a dialog (WM_INITDIALOG handler).
void DarkMode_ApplyToDialog(HWND hDlg) noexcept;

// Update dark mode after style theme change. Call from Style_OnStyleThemeChanged().
void DarkMode_OnThemeChanged(int newTheme) noexcept;

// Handle WM_SETTINGCHANGE for system dark/light mode changes.
// Returns true if the message was handled.
bool DarkMode_HandleSettingChange(HWND hwnd, LPARAM lParam) noexcept;

// Check if dark mode UI is currently active.
bool DarkMode_IsEnabled() noexcept;
