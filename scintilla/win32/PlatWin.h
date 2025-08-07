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

#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32	0x00000800
#endif

#if _WIN32_WINNT >= _WIN32_WINNT_WIN8
#define kSystemLibraryLoadFlags		LOAD_LIBRARY_SEARCH_SYSTEM32
#else
extern DWORD kSystemLibraryLoadFlags;
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WIN8) && defined(_MSC_VER) && defined(__clang__)
// fix error for RoTransformError() used in winrt\wrl\event.h
BOOL WINAPI RoTransformError(HRESULT oldError, HRESULT newError, /*HSTRING*/ void *message);
#endif
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
#include <d2d1_1.h>
#include <d3d11_1.h>
#include <dwrite_1.h>
#else
#include <d2d1.h>
#include <dwrite.h>
#endif

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
#if _WIN32_WINNT >= _WIN32_WINNT_WIN10
#define GetWindowDPI(hwnd)						GetDpiForWindow(hwnd)
#define SystemMetricsForDpi(nIndex, dpi)		GetSystemMetricsForDpi((nIndex), (dpi))
#define AdjustWindowRectForDpi(lpRect, dwStyle, dwExStyle, dpi) \
		::AdjustWindowRectExForDpi((lpRect), (dwStyle), FALSE, (dwExStyle), (dpi))

#else
extern UINT GetWindowDPI(HWND hwnd) noexcept;
extern int SystemMetricsForDpi(int nIndex, UINT dpi) noexcept;
extern BOOL AdjustWindowRectForDpi(LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, UINT dpi) noexcept;
#endif

extern WCHAR defaultTextFontName[LF_FACESIZE];

namespace Scintilla::Internal {

constexpr FLOAT dpiDefault = USER_DEFAULT_SCREEN_DPI;

// Used for defining font size with LOGFONT
constexpr int pointsPerInch = 72;

bool ListBoxX_Register() noexcept;
void ListBoxX_Unregister() noexcept;
std::unique_ptr<Surface> SurfaceGDI_Allocate();

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
	const __m128i i32x4 = _mm_load_si128(reinterpret_cast<__m128i *>(&rc));
	const __m256d f64x4 = _mm256_cvtepi32_pd(i32x4);
	_mm256_storeu_pd(reinterpret_cast<double *>(&prc), f64x4);
	return prc;
}

inline RECT RectFromPRectangleEx(PRectangle prc) noexcept {
	RECT rc;
	const __m256d f64x4 = _mm256_load_pd(reinterpret_cast<double *>(&prc));
	const __m128i i32x4 = _mm256_cvttpd_epi32(f64x4);
	_mm_storeu_si128(reinterpret_cast<__m128i *>(&rc), i32x4);
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

constexpr SIZE SizeOfRect(RECT rc) noexcept {
 	return { rc.right - rc.left, rc.bottom - rc.top };
}

#if NP2_USE_SSE2
static_assert(sizeof(Point) == sizeof(__m128d));
static_assert(sizeof(POINT) == sizeof(__int64));

inline POINT POINTFromPointEx(Point point) noexcept {
	POINT pt;
	const __m128d f64x2 = _mm_load_pd(reinterpret_cast<double *>(&point));
	const __m128i i32x2 = _mm_cvttpd_epi32(f64x2);
	_mm_storeu_si64(&pt, i32x2);
	return pt;
}

inline Point PointFromPOINTEx(POINT point) noexcept {
	Point pt;
	const __m128i i32x2 = _mm_loadu_si64(&point);
	const __m128d f64x2 = _mm_cvtepi32_pd(i32x2);
	_mm_storeu_pd(reinterpret_cast<double *>(&pt), f64x2);
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

extern HINSTANCE hinstPlatformRes;

ColourRGBA ColourFromSys(int nIndex) noexcept;

constexpr HWND HwndFromWindowID(WindowID wid) noexcept {
	return static_cast<HWND>(wid);
}

inline HWND HwndFromWindow(const Window &w) noexcept {
	return HwndFromWindowID(w.GetID());
}

template <class T>
inline T PointerFromWindow(HWND hWnd) noexcept {
	return AsPointer<T>(::GetWindowLongPtr(hWnd, 0));
}

inline void SetWindowPointer(HWND hWnd, void *ptr) noexcept {
	::SetWindowLongPtr(hWnd, 0, AsInteger<LONG_PTR>(ptr));
}

inline UINT DpiForWindow(WindowID wid) noexcept {
	return GetWindowDPI(HwndFromWindowID(wid));
}

HCURSOR LoadReverseArrowCursor(HCURSOR cursor, UINT dpi) noexcept;

// Encapsulate WM_PAINT handling so that EndPaint is always called even with unexpected returns or exceptions.
struct Painter {
	HWND hWnd{};
	PAINTSTRUCT ps{};
	explicit Painter(HWND hWnd_) noexcept : hWnd(hWnd_) {
		::BeginPaint(hWnd, &ps);
	}
	~Painter() {
		::EndPaint(hWnd, &ps);
	}
};

class MouseWheelDelta {
	int wheelDelta = 0;
public:
	bool Accumulate(WPARAM wParam) noexcept {
		wheelDelta -= GET_WHEEL_DELTA_WPARAM(wParam);
		return std::abs(wheelDelta) >= WHEEL_DELTA;
	}
	int Actions() noexcept {
		const int actions = wheelDelta / WHEEL_DELTA;
		wheelDelta = wheelDelta % WHEEL_DELTA;
		return actions;
	}
};

constexpr BYTE Win32MapFontQuality(FontQuality extraFontFlag) noexcept {
	constexpr UINT mask = (DEFAULT_QUALITY << static_cast<int>(FontQuality::QualityDefault))
		| (NONANTIALIASED_QUALITY << (4 *static_cast<int>(FontQuality::QualityNonAntialiased)))
		| (ANTIALIASED_QUALITY << (4 * static_cast<int>(FontQuality::QualityAntialiased)))
		| (CLEARTYPE_QUALITY << (4 * static_cast<int>(FontQuality::QualityLcdOptimized)))
		;
	return static_cast<BYTE>((mask >> (4*static_cast<int>(extraFontFlag & FontQuality::QualityMask))) & 15);
}

// Both GDI and DirectWrite can produce a HFONT for use in list boxes
struct FontWin final : public Font {
	HFONT hfont{};
	ComPtr<IDWriteTextFormat> pTextFormat;
	FontQuality extraFontFlag;
	static constexpr FLOAT minimalAscent = 2.0f;
	FLOAT yAscent = minimalAscent;
	FLOAT yDescent = 1.0f;
	FLOAT yInternalLeading = 0.0f;
	LOGFONTW lf {};
	explicit FontWin(const FontParameters &fp);
	FontWin(const FontWin &) = delete;
	FontWin(FontWin &&) = delete;
	FontWin &operator=(const FontWin &) = delete;
	FontWin &operator=(FontWin &&) = delete;

	~FontWin() noexcept override {
		if (hfont) {
			::DeleteObject(hfont);
		}
	}
	[[nodiscard]] HFONT HFont() const noexcept {
		return ::CreateFontIndirectW(&lf);
	}
};

/* dummy types to minimize differences between official Scintilla.
FontDirectWrite::HFont() will create wild font when font family name
is different from typeface name, e.g. Source Code Pro Semibold.
*/
using FontGDI = FontWin;
using FontDirectWrite = FontWin;

// Buffer to hold strings and string position arrays without always allocating on heap.
// May sometimes have string too long to allocate on stack. So use a fixed stack-allocated buffer
// when less than safe size otherwise allocate on heap and free automatically.
template<typename T, size_t lengthStandard>
class VarBuffer {
	T *buffer;
	T bufferStandard[lengthStandard];
public:
	VarBuffer() noexcept {
		buffer = bufferStandard;
	}
	explicit VarBuffer(size_t length): buffer{bufferStandard} {
		allocate(length);
	}
	void allocate(size_t length) {
		if (length > lengthStandard) {
			buffer = new T[length];
		}
		static_assert(__is_standard_layout(T));
		memset(buffer, 0, length*sizeof(T));
	}
	const T *data() const noexcept {
		return buffer;
	}
	T *data() noexcept {
		return buffer;
	}
	const T& operator[](size_t index) const noexcept {
		return buffer[index];
	}
	T& operator[](size_t index) noexcept {
		return buffer[index];
	}

	// Deleted so VarBuffer objects can not be copied.
	VarBuffer(const VarBuffer &) = delete;
	VarBuffer(VarBuffer &&) = delete;
	VarBuffer &operator=(const VarBuffer &) = delete;
	VarBuffer &operator=(VarBuffer &&) = delete;

	~VarBuffer() noexcept {
		if (buffer != bufferStandard) {
			delete[]buffer;
		}
	}
};

constexpr size_t stackBufferLength = 480; // max value to keep stack usage under 4096
class TextWide {
	wchar_t *buffer;
	UINT len;	// Using UINT instead of size_t as most Win32 APIs take UINT.
	wchar_t bufferStandard[stackBufferLength];
public:
	TextWide(std::string_view text, int codePage): buffer {bufferStandard} {
		if (text.length() > stackBufferLength) {
			buffer = new wchar_t[text.length()];
		}
		if (codePage == CpUtf8) {
			len = static_cast<UINT>(UTF16FromUTF8(text, buffer, text.length()));
		} else {
			// Support Asian string display in 9x English
			len = ::MultiByteToWideChar(codePage, 0, text.data(), static_cast<int>(text.length()),
				buffer, static_cast<int>(text.length()));
		}
	}
	const wchar_t *data() const noexcept {
		return buffer;
	}
	UINT length() const noexcept {
		return len;
	}
	[[nodiscard]] std::wstring_view AsView() const noexcept {
		return std::wstring_view{buffer, len};
	}

	// Deleted so TextWide objects can not be copied.
	TextWide(const TextWide &) = delete;
	TextWide(TextWide &&) = delete;
	TextWide &operator=(const TextWide &) = delete;
	TextWide &operator=(TextWide &&) = delete;

	~TextWide() noexcept {
		if (buffer != bufferStandard) {
			delete[]buffer;
		}
	}
};

// Manage the lifetime of a memory HBITMAP and its HDC so there are no leaks.
class GDIBitMap {
	HDC hdc{};
	HBITMAP hbm{};
	HBITMAP hbmOriginal{};

public:
	GDIBitMap() noexcept = default;
	// Deleted so GDIBitMap objects can not be copied.
	GDIBitMap(const GDIBitMap &) = delete;
	GDIBitMap(GDIBitMap &&) = delete;
	// Move would be OK but not needed yet
	GDIBitMap &operator=(const GDIBitMap &) = delete;
	GDIBitMap &operator=(GDIBitMap &&) = delete;
	~GDIBitMap() noexcept;

	void Create(HDC hdcBase, int width, int height, DWORD **pixels) noexcept;
	void Release() noexcept;
	HBITMAP Extract() noexcept;

	[[nodiscard]] HDC DC() const noexcept {
		return hdc;
	}
	[[nodiscard]] explicit operator bool() const noexcept {
		return hdc && hbm;
	}
};

}
