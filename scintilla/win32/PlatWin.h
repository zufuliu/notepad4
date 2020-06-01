// Scintilla source code edit control
/** @file PlatWin.h
 ** Implementation of platform facilities on Windows.
 **/
// Copyright 1998-2011 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

// sdkddkver.h
#ifndef _WIN32_WINNT_VISTA
#define _WIN32_WINNT_VISTA				0x0600
#endif
#ifndef _WIN32_WINNT_WIN7
#define _WIN32_WINNT_WIN7				0x0601
#endif
#ifndef _WIN32_WINNT_WIN8
#define _WIN32_WINNT_WIN8				0x0602
#endif
#ifndef _WIN32_WINNT_WINBLUE
#define _WIN32_WINNT_WINBLUE			0x0603
#endif
#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10				0x0A00
#endif

#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI		96
#endif

#if !defined(DISABLE_D2D)
#define USE_D2D		1
#endif

#if defined(USE_D2D)
#if defined(_MSC_BUILD) && (VER_PRODUCTVERSION_W <= _WIN32_WINNT_WIN7)
#pragma warning(push)
#pragma warning(disable: 4458)
// d2d1helper.h(677,19): warning C4458:  declaration of 'a' hides class member
#endif
#include <d2d1.h>
#include <dwrite.h>
#if defined(_MSC_BUILD) && (VER_PRODUCTVERSION_W <= _WIN32_WINNT_WIN7)
#pragma warning(pop)
#endif
#endif

// force compile C as CPP
#define NP2_FORCE_COMPILE_C_AS_CPP		0

// since Windows 10, version 1607
#if defined(__aarch64__) || defined(_ARM64_) || defined(_M_ARM64)
// 1709 was the first version for Windows 10 on ARM64.
#define NP2_TARGET_ARM64	1
#define GetWindowDPI(hwnd)						GetDpiForWindow(hwnd)
#define SystemMetricsForDpi(nIndex, dpi)		GetSystemMetricsForDpi((nIndex), (dpi))
#define DpiAdjustWindowRect(lpRect, dwStyle, dwExStyle, dpi) \
		::AdjustWindowRectExForDpi((lpRect), (dwStyle), FALSE, (dwExStyle), (dpi))

#else
#define NP2_TARGET_ARM64	0
#if NP2_FORCE_COMPILE_C_AS_CPP
#define NP2_noexcept noexcept
extern UINT GetWindowDPI(HWND hwnd) noexcept;
extern int SystemMetricsForDpi(int nIndex, UINT dpi) noexcept;
extern BOOL DpiAdjustWindowRect(LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, UINT dpi) noexcept;
#else
#define NP2_noexcept
extern "C" UINT GetWindowDPI(HWND hwnd);
extern "C" int SystemMetricsForDpi(int nIndex, UINT dpi);
extern "C" BOOL DpiAdjustWindowRect(LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, UINT dpi);
#endif
#endif

namespace Scintilla {

extern void Platform_Initialise(void *hInstance) noexcept;
extern void Platform_Finalise(bool fromDllMain) noexcept;

constexpr RECT RectFromPRectangle(PRectangle prc) noexcept {
	RECT rc = { static_cast<LONG>(prc.left), static_cast<LONG>(prc.top),
		static_cast<LONG>(prc.right), static_cast<LONG>(prc.bottom) };
	return rc;
}

constexpr POINT POINTFromPoint(Point pt) noexcept {
	return POINT { static_cast<LONG>(pt.x), static_cast<LONG>(pt.y) };
}

constexpr Point PointFromPOINT(POINT pt) noexcept {
	return Point::FromInts(pt.x, pt.y);
}

constexpr HWND HwndFromWindowID(WindowID wid) noexcept {
	return static_cast<HWND>(wid);
}

inline HWND HwndFromWindow(const Window &w) noexcept {
	return HwndFromWindowID(w.GetID());
}

inline void *PointerFromWindow(HWND hWnd) noexcept {
	return reinterpret_cast<void *>(::GetWindowLongPtr(hWnd, 0));
}

inline void SetWindowPointer(HWND hWnd, void *ptr) noexcept {
	::SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(ptr));
}

/// Find a function in a DLL and convert to a function pointer.
/// This avoids undefined and conditionally defined behaviour.
template<typename T>
inline T DLLFunction(HMODULE hModule, LPCSTR lpProcName) noexcept {
#if 1
#if defined(__GNUC__) && __GNUC__ >= 8
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wcast-function-type"
	return reinterpret_cast<T>(::GetProcAddress(hModule, lpProcName));
	#pragma GCC diagnostic pop
#else
	return reinterpret_cast<T>(::GetProcAddress(hModule, lpProcName));
#endif
#else
	if (!hModule) {
		return nullptr;
	}
	FARPROC function = ::GetProcAddress(hModule, lpProcName);
	static_assert(sizeof(T) == sizeof(function));
	T fp;
	memcpy(&fp, &function, sizeof(T));
	return fp;
#endif
}

template<typename T>
inline T DLLFunctionEx(LPCWSTR lpDllName, LPCSTR lpProcName) noexcept {
	return DLLFunction<T>(::GetModuleHandleW(lpDllName), lpProcName);
}

inline UINT DpiForWindow(WindowID wid) noexcept {
	return GetWindowDPI(HwndFromWindowID(wid));
}

HCURSOR LoadReverseArrowCursor(UINT dpi) noexcept;

#if defined(USE_D2D)
extern bool LoadD2D() noexcept;
extern ID2D1Factory *pD2DFactory;
extern IDWriteFactory *pIDWriteFactory;
#endif

}
