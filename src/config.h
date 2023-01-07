// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
#pragma once

//! Enable customize toolbar labels
// When enabled, user can change toolbar button tooltip text
// in the "Toolbar Labels" (without quotes) section in Notepad2.ini
#define NP2_ENABLE_CUSTOMIZE_TOOLBAR_LABELS		0

//! Enable building with HiDPI toolbar images.
// When disabled, only building with 16x16 Toolbar16.bmp
#define NP2_ENABLE_HIDPI_TOOLBAR_IMAGE			1

//! Enable the .LOG feature
// When enabled and Notepad2 opens a file starts with ".LOG" (without quotes, in upper case),
// Notepad2 will append current time to the file.
// This is a hiden feature in Windows Notepad.
#define NP2_ENABLE_DOT_LOG_FEATURE				0

//! Enable localization with satellite resource DLLs.
#define NP2_ENABLE_APP_LOCALIZATION_DLL			1
//! Enable test localization dialog layout with default UI font for target locale.
#define NP2_ENABLE_TEST_LOCALIZATION_LAYOUT		0

//! Enable localization for scheme/lexer names.
#define NP2_ENABLE_LOCALIZE_LEXER_NAME			1
//! Enable localization for scheme/lexer style names.
#define NP2_ENABLE_LOCALIZE_STYLE_NAME			1
