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

// official Scintilla use std::call_once(), which increases binary about 12 KiB.
#define USE_STD_CALL_ONCE		0
#if !USE_STD_CALL_ONCE && (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
// use InitOnceExecuteOnce()
#define USE_WIN32_INIT_ONCE		0
#else
// fallback to InterlockedCompareExchange(). it's not same as std::call_once(),
// but should safe in Windows message handlers (for WM_CREATE and SCI_SETTECHNOLOGY).
#define USE_WIN32_INIT_ONCE		0
#endif

// since Windows 10, version 1607
#if (defined(__aarch64__) || defined(_ARM64_) || defined(_M_ARM64)) && !defined(__MINGW32__)
// 1709 was the first version for Windows 10 on ARM64.
#define NP2_HAS_GETDPIFORWINDOW					1
#define GetWindowDPI(hwnd)						GetDpiForWindow(hwnd)
#define SystemMetricsForDpi(nIndex, dpi)		GetSystemMetricsForDpi((nIndex), (dpi))
#define AdjustWindowRectForDpi(lpRect, dwStyle, dwExStyle, dpi) \
		::AdjustWindowRectExForDpi((lpRect), (dwStyle), FALSE, (dwExStyle), (dpi))

#else
#define NP2_HAS_GETDPIFORWINDOW					0
#if NP2_FORCE_COMPILE_C_AS_CPP
#define NP2F_noexcept noexcept
extern UINT GetWindowDPI(HWND hwnd) noexcept;
extern int SystemMetricsForDpi(int nIndex, UINT dpi) noexcept;
extern BOOL AdjustWindowRectForDpi(LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, UINT dpi) noexcept;
#else
#define NP2F_noexcept
extern "C" UINT GetWindowDPI(HWND hwnd);
extern "C" int SystemMetricsForDpi(int nIndex, UINT dpi);
extern "C" BOOL AdjustWindowRectForDpi(LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, UINT dpi);
#endif
#endif

#if NP2_FORCE_COMPILE_C_AS_CPP
extern WCHAR defaultTextFontName[LF_FACESIZE];
#else
extern "C" WCHAR defaultTextFontName[LF_FACESIZE];
#endif

namespace Scintilla {

extern void Platform_Initialise(void *hInstance) noexcept;
extern void Platform_Finalise(bool fromDllMain) noexcept;

constexpr RECT RectFromPRectangle(PRectangle prc) noexcept {
	RECT rc = { static_cast<LONG>(prc.left), static_cast<LONG>(prc.top),
		static_cast<LONG>(prc.right), static_cast<LONG>(prc.bottom) };
	return rc;
}

constexpr PRectangle PRectangleFromRect(RECT rc) noexcept {
	return PRectangle::FromInts(rc.left, rc.top, rc.right, rc.bottom);
}

#if NP2_USE_AVX2
static_assert(sizeof(PRectangle) == sizeof(__m256d));
static_assert(sizeof(RECT) == sizeof(__m128i));

inline PRectangle PRectangleFromRectEx(RECT rc) noexcept {
	PRectangle prc;
	__m128i i32x4 = _mm_load_si128((__m128i *)(&rc));
	__m256d f64x4 = _mm256_cvtepi32_pd(i32x4);
	_mm256_storeu_pd((double *)(&prc), f64x4);
	return prc;
}

inline RECT RectFromPRectangleEx(PRectangle prc) noexcept {
	RECT rc;
	__m256d f64x4 = _mm256_load_pd((double *)(&prc));
	__m128i i32x4 = _mm256_cvttpd_epi32(f64x4);
	_mm_storeu_si128((__m128i *)(&rc), i32x4);
	return rc;
}

#else
constexpr PRectangle PRectangleFromRectEx(RECT rc) noexcept {
	return PRectangleFromRect(rc);
}

constexpr RECT RectFromPRectangleEx(PRectangle prc) noexcept {
	return RectFromPRectangle(prc);
}
#endif

constexpr POINT POINTFromPoint(Point pt) noexcept {
	return POINT { static_cast<LONG>(pt.x), static_cast<LONG>(pt.y) };
}

constexpr Point PointFromPOINT(POINT pt) noexcept {
	return Point::FromInts(pt.x, pt.y);
}

#if NP2_USE_SSE2
static_assert(sizeof(Point) == sizeof(__m128d));
static_assert(sizeof(POINT) == sizeof(__int64));

inline POINT POINTFromPointEx(Point point) noexcept {
	POINT pt;
	__m128d f64x2 = _mm_load_pd((double *)(&point));
	__m128i i32x2 = _mm_cvttpd_epi32(f64x2);
	_mm_storeu_si64(&pt, i32x2);
	return pt;
}

inline Point PointFromPOINTEx(POINT point) noexcept {
	Point pt;
	__m128i i32x2 = _mm_loadu_si64(&point);
	__m128d f64x2 = _mm_cvtepi32_pd(i32x2);
	_mm_storeu_pd((double *)(&pt), f64x2);
	return pt;
}

#else
constexpr POINT POINTFromPointEx(Point point) noexcept {
	return POINTFromPoint(point);
}

constexpr Point PointFromPOINTEx(POINT point) noexcept {
	return PointFromPOINT(point);
}
#endif

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

// Release an IUnknown* and set to nullptr.
// While IUnknown::Release must be noexcept, it isn't marked as such so produces
// warnings which are avoided by the catch.
template <class T>
inline void ReleaseUnknown(T *&ppUnknown) noexcept {
	if (ppUnknown) {
#if 0
		ppUnknown->Release();
#else
		try {
			ppUnknown->Release();
		} catch (...) {
			// Never occurs
			NP2_unreachable();
		}
#endif
		ppUnknown = nullptr;
	}
}

inline UINT DpiForWindow(WindowID wid) noexcept {
	return GetWindowDPI(HwndFromWindowID(wid));
}

HCURSOR LoadReverseArrowCursor(UINT dpi) noexcept;

constexpr BYTE Win32MapFontQuality(int extraFontFlag) noexcept {
	switch (extraFontFlag & SC_EFF_QUALITY_MASK) {

	case SC_EFF_QUALITY_NON_ANTIALIASED:
		return NONANTIALIASED_QUALITY;

	case SC_EFF_QUALITY_ANTIALIASED:
		return ANTIALIASED_QUALITY;

	case SC_EFF_QUALITY_LCD_OPTIMIZED:
		return CLEARTYPE_QUALITY;

	default:
		return DEFAULT_QUALITY;
	}
}

#if defined(USE_D2D)
extern bool LoadD2D() noexcept;
extern ID2D1Factory *pD2DFactory;
extern IDWriteFactory *pIDWriteFactory;
#endif

}
