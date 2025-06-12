// Scintilla source code edit control
/** @file PlatWin.cxx
 ** Implementation of platform facilities on Windows.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cmath>
#include <climits>

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <map>
#include <optional>
#include <algorithm>
#include <iterator>
#include <memory>
//#include <mutex>

#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>

#include "ScintillaTypes.h"
#include "ILexer.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"
#include "VectorISA.h"
#include "GraphicUtils.h"
#include "UniqueString.h"
#include "XPM.h"
#include "CharClassify.h"
#include "UniConversion.h"

#include "WinTypes.h"
#include "PlatWin.h"
#include "SurfaceD2D.h"

extern UINT g_uSystemDPI;

using namespace Scintilla;

#if !NP2_HAS_GETDPIFORWINDOW
namespace {

using GetDpiForWindowSig = UINT (WINAPI *)(HWND hwnd);
GetDpiForWindowSig fnGetDpiForWindow = nullptr;

#ifndef DPI_ENUMS_DECLARED
#define MDT_EFFECTIVE_DPI	0
#endif

using GetDpiForMonitorSig = HRESULT (WINAPI *)(HMONITOR hmonitor, /*MONITOR_DPI_TYPE*/int dpiType, UINT *dpiX, UINT *dpiY);
HMODULE hShcoreDLL {};
GetDpiForMonitorSig fnGetDpiForMonitor = nullptr;

using GetSystemMetricsForDpiSig = int (WINAPI *)(int nIndex, UINT dpi);
GetSystemMetricsForDpiSig fnGetSystemMetricsForDpi = nullptr;

using AdjustWindowRectExForDpiSig = BOOL (WINAPI *)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);
AdjustWindowRectExForDpiSig fnAdjustWindowRectExForDpi = nullptr;

}

extern "C"
void Scintilla_LoadDpiForWindow(void) {
	using Scintilla::Internal::DLLFunction;

	HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
	fnGetDpiForWindow = DLLFunction<GetDpiForWindowSig>(user32, "GetDpiForWindow");
	fnGetSystemMetricsForDpi = DLLFunction<GetSystemMetricsForDpiSig>(user32, "GetSystemMetricsForDpi");
	fnAdjustWindowRectExForDpi = DLLFunction<AdjustWindowRectExForDpiSig>(user32, "AdjustWindowRectExForDpi");

	using GetDpiForSystemSig = UINT (WINAPI *)(void);
	GetDpiForSystemSig fnGetDpiForSystem = DLLFunction<GetDpiForSystemSig>(user32, "GetDpiForSystem");
	if (fnGetDpiForSystem) {
		g_uSystemDPI = fnGetDpiForSystem();
	} else {
		HDC hDC = ::GetDC({});
		g_uSystemDPI = ::GetDeviceCaps(hDC, LOGPIXELSY);
		::ReleaseDC({}, hDC);
	}

	if (!fnGetDpiForWindow) {
		hShcoreDLL = ::LoadLibraryExW(L"shcore.dll", {}, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (hShcoreDLL) {
			fnGetDpiForMonitor = DLLFunction<GetDpiForMonitorSig>(hShcoreDLL, "GetDpiForMonitor");
		}
	}
}

UINT GetWindowDPI(HWND hwnd) noexcept {
	if (fnGetDpiForWindow) {
		return fnGetDpiForWindow(hwnd);
	}
	if (fnGetDpiForMonitor) {
		HMONITOR hMonitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		UINT dpiX = 0;
		UINT dpiY = 0;
		if (fnGetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) == S_OK) {
			return dpiY;
		}
	}
	return g_uSystemDPI;
}

int SystemMetricsForDpi(int nIndex, UINT dpi) noexcept {
	if (fnGetSystemMetricsForDpi) {
		return fnGetSystemMetricsForDpi(nIndex, dpi);
	}

	int value = ::GetSystemMetrics(nIndex);
	if (dpi != g_uSystemDPI) {
		value = ::MulDiv(value, dpi, g_uSystemDPI);
	}
	return value;
}

BOOL AdjustWindowRectForDpi(LPRECT lpRect, DWORD dwStyle, DWORD dwExStyle, UINT dpi) noexcept {
	if (fnAdjustWindowRectExForDpi) {
		return fnAdjustWindowRectExForDpi(lpRect, dwStyle, FALSE, dwExStyle, dpi);
	}
	return ::AdjustWindowRectEx(lpRect, dwStyle, FALSE, dwExStyle);
}
#endif

namespace Scintilla::Internal {

HINSTANCE hinstPlatformRes {};

int64_t QueryPerformanceFrequency() noexcept {
	LARGE_INTEGER freq;
	::QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}

int64_t QueryPerformanceCounter() noexcept {
	LARGE_INTEGER count;
	::QueryPerformanceCounter(&count);
	return count.QuadPart;
}

namespace {

constexpr BITMAPV5HEADER BitMapHeader(int width, int height) noexcept {
	constexpr int pixelBits = 32;

	// Divide each pixel up in the expected BGRA manner.
	// Compatible with DXGI_FORMAT_B8G8R8A8_UNORM.
	constexpr DWORD maskRed = 0x00FF0000U;
	constexpr DWORD maskGreen = 0x0000FF00U;
	constexpr DWORD maskBlue = 0x000000FFU;
	constexpr DWORD maskAlpha = 0xFF000000U;

	BITMAPV5HEADER bi{};
	bi.bV5Size = sizeof(BITMAPV5HEADER);
	bi.bV5Width = width;
	bi.bV5Height = height;
	bi.bV5Planes = 1;
	bi.bV5BitCount = pixelBits;
	bi.bV5Compression = BI_BITFIELDS;
	// The following mask specification specifies a supported 32 BPP alpha format for Windows XP.
	bi.bV5RedMask = maskRed;
	bi.bV5GreenMask = maskGreen;
	bi.bV5BlueMask = maskBlue;
	bi.bV5AlphaMask = maskAlpha;
	return bi;
}

HBITMAP BitMapSection(HDC hdc, int width, int height, DWORD **pixels) noexcept {
	const BITMAPV5HEADER bi = BitMapHeader(width, height);
	void *image = nullptr;
	HBITMAP hbm = ::CreateDIBSection(hdc, reinterpret_cast<const BITMAPINFO *>(&bi), DIB_RGB_COLORS, &image, {}, 0);
	if (pixels) {
		*pixels = static_cast<DWORD *>(image);
	}
	return hbm;
}

}

GDIBitMap::~GDIBitMap() noexcept {
	Release();
}

void GDIBitMap::Create(HDC hdcBase, int width, int height, DWORD **pixels) noexcept {
	Release();

	hdc = CreateCompatibleDC(hdcBase);
	if (!hdc) {
		return;
	}

	hbm = BitMapSection(hdc, width, height, pixels);
	if (!hbm) {
		return;
	}
	hbmOriginal = SelectBitmap(hdc, hbm);
}

void GDIBitMap::Release() noexcept {
	if (hbmOriginal) {
		// Deselect HBITMAP from HDC so it may be deleted.
		SelectBitmap(hdc, hbmOriginal);
	}
	hbmOriginal = {};
	if (hbm) {
		::DeleteObject(hbm);
		hbm = {};
	}
	if (hdc) {
		::DeleteDC(hdc);
		hdc = {};
	}
}

HBITMAP GDIBitMap::Extract() noexcept {
	// Deselect HBITMAP from HDC but keep so can delete.
	// The caller will make a copy, not take ownership.
	HBITMAP ret = hbm;
	if (hbmOriginal) {
		SelectBitmap(hdc, hbmOriginal);
		hbmOriginal = {};
	}
	return ret;
}

void Window::Destroy() noexcept {
	if (wid)
		::DestroyWindow(HwndFromWindowID(wid));
	wid = nullptr;
}

PRectangle Window::GetPosition() const noexcept {
	RECT rc;
	::GetWindowRect(HwndFromWindowID(wid), &rc);
	return PRectangleFromRectEx(rc);
}

void Window::SetPosition(PRectangle rc) noexcept {
	::SetWindowPos(HwndFromWindowID(wid),
		{}, static_cast<int>(rc.left), static_cast<int>(rc.top),
		static_cast<int>(rc.Width()), static_cast<int>(rc.Height()), SWP_NOZORDER | SWP_NOACTIVATE);
}

namespace {

RECT RectFromMonitor(HMONITOR hMonitor) noexcept {
	MONITORINFO mi = {};
	mi.cbSize = sizeof(mi);
	if (GetMonitorInfo(hMonitor, &mi)) {
		return mi.rcWork;
	}
	RECT rc {};
	if (::SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0) == 0) {
		rc.left = 0;
		rc.top = 0;
		rc.right = 0;
		rc.bottom = 0;
	}
	return rc;
}

}

void Window::SetPositionRelative(PRectangle rc, const Window *relativeTo) noexcept {
	const DWORD style = GetWindowStyle(HwndFromWindowID(wid));
	if (style & WS_POPUP) {
		POINT ptOther {};
		::ClientToScreen(HwndFromWindow(*relativeTo), &ptOther);
		rc.Move(static_cast<XYPOSITION>(ptOther.x), static_cast<XYPOSITION>(ptOther.y));

		const RECT rcMonitor = RectFromPRectangleEx(rc);

		HMONITOR hMonitor = MonitorFromRect(&rcMonitor, MONITOR_DEFAULTTONEAREST);
		// If hMonitor is NULL, that's just the main screen anyways.
		const RECT rcWork = RectFromMonitor(hMonitor);

		if (rcWork.left < rcWork.right) {
			// Now clamp our desired rectangle to fit inside the work area
			// This way, the menu will fit wholly on one screen. An improvement even
			// if you don't have a second monitor on the left... Menu's appears half on
			// one screen and half on the other are just U.G.L.Y.!
			if (rc.right > rcWork.right)
				rc.Move(rcWork.right - rc.right, 0);
			if (rc.bottom > rcWork.bottom)
				rc.Move(0, rcWork.bottom - rc.bottom);
			if (rc.left < rcWork.left)
				rc.Move(rcWork.left - rc.left, 0);
			if (rc.top < rcWork.top)
				rc.Move(0, rcWork.top - rc.top);
		}
	}
	SetPosition(rc);
}

PRectangle Window::GetClientPosition() const noexcept {
	RECT rc {};
	if (wid)
		::GetClientRect(HwndFromWindowID(wid), &rc);
	return PRectangleFromRectEx(rc);
}

void Window::Show(bool show) const noexcept {
	if (show)
		::ShowWindow(HwndFromWindowID(wid), SW_SHOWNOACTIVATE);
	else
		::ShowWindow(HwndFromWindowID(wid), SW_HIDE);
}

void Window::InvalidateAll() noexcept {
	::InvalidateRect(HwndFromWindowID(wid), nullptr, FALSE);
}

void Window::InvalidateRectangle(PRectangle rc) noexcept {
	const RECT rcw = RectFromPRectangleEx(rc);
	::InvalidateRect(HwndFromWindowID(wid), &rcw, FALSE);
}

#if 1 // flip system array cursor
namespace {

void FlipBitmap(HBITMAP bitmap, int width, int height) noexcept {
	HDC hdc = ::CreateCompatibleDC({});
	if (hdc) {
		HBITMAP prevBmp = SelectBitmap(hdc, bitmap);
		::StretchBlt(hdc, width - 1, 0, -width, height, hdc, 0, 0, width, height, SRCCOPY);
		SelectBitmap(hdc, prevBmp);
		::DeleteDC(hdc);
	}
}

}

HCURSOR LoadReverseArrowCursor(HCURSOR cursor, UINT dpi) noexcept {
	bool created = false;
	// https://learn.microsoft.com/en-us/answers/questions/815036/windows-cursor-size
	constexpr DWORD defaultCursorBaseSize = 32;
	constexpr DWORD maxCursorBaseSize = 16*(1 + 15); // 16*(1 + CursorSize)
	DWORD cursorBaseSize = 0;
	HKEY hKey {};
	LSTATUS status = ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Cursors", 0, KEY_QUERY_VALUE, &hKey);
	if (status == ERROR_SUCCESS) {
		DWORD baseSize = 0;
		DWORD type = REG_NONE;
		DWORD size = sizeof(DWORD);
		status = ::RegQueryValueExW(hKey, L"CursorBaseSize", nullptr, &type, reinterpret_cast<LPBYTE>(&baseSize), &size);
		if (status == ERROR_SUCCESS && type == REG_DWORD) {
			// CursorBaseSize is multiple of 16
			cursorBaseSize = std::min(baseSize & ~15, maxCursorBaseSize);
		}
	}

	if (dpi != g_uSystemDPI || cursorBaseSize > defaultCursorBaseSize) {
		int width;
		int height;
		if (cursorBaseSize > defaultCursorBaseSize) {
			width = ::MulDiv(cursorBaseSize, dpi, USER_DEFAULT_SCREEN_DPI);
			height = width;
		} else {
			width = SystemMetricsForDpi(SM_CXCURSOR, dpi);
			height = SystemMetricsForDpi(SM_CYCURSOR, dpi);
		}
		if (hKey) {
			// workaround CopyImage() for system cursor
			// https://learn.microsoft.com/en-us/answers/questions/1315176/how-to-copy-system-cursors-properly
			WCHAR cursorPath[MAX_PATH]{};
			DWORD size = sizeof(cursorPath);
			DWORD type = REG_NONE;
#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
			status = ::RegGetValueW(hKey, nullptr, L"Arrow", RRF_RT_REG_SZ, &type, cursorPath, &size);
			if (status == ERROR_SUCCESS && type == REG_SZ) {
				HCURSOR load = static_cast<HCURSOR>(::LoadImage({}, cursorPath, IMAGE_CURSOR, width, height, LR_LOADFROMFILE));
				if (load) {
					created = true;
					cursor = load;
				}
			}
#else
			status = ::RegQueryValueExW(hKey, L"Arrow", nullptr, &type, reinterpret_cast<LPBYTE>(cursorPath), &size);
			if (status == ERROR_SUCCESS && (type == REG_SZ || type == REG_EXPAND_SZ)) {
				LPCWSTR path = cursorPath;
				WCHAR expansion[MAX_PATH];
				if (type == REG_EXPAND_SZ) {
					size = ::ExpandEnvironmentStringsW(cursorPath, expansion, MAX_PATH);
					if (size > 0 && size <= MAX_PATH) {
						path = expansion;
					}
				}
				HCURSOR load = static_cast<HCURSOR>(::LoadImage({}, path, IMAGE_CURSOR, width, height, LR_LOADFROMFILE));
				if (load) {
					created = true;
					cursor = load;
				}
			}
#endif // _WIN32_WINNT_VISTA
		}
		HCURSOR copy = static_cast<HCURSOR>(::CopyImage(cursor, IMAGE_CURSOR, width, height, LR_COPYFROMRESOURCE | LR_COPYRETURNORG));
		if (copy && copy != cursor) {
			if (created) {
				::DestroyCursor(cursor);
			}
			created = true;
			cursor = copy;
		}
	}

	if (hKey) {
		::RegCloseKey(hKey);
	}

	ICONINFO info;
	HCURSOR reverseArrowCursor {};
	if (::GetIconInfo(cursor, &info)) {
		BITMAP bmp{};
		if (::GetObject(info.hbmMask, sizeof(bmp), &bmp)) {
			FlipBitmap(info.hbmMask, bmp.bmWidth, bmp.bmHeight);
			if (info.hbmColor) {
				FlipBitmap(info.hbmColor, bmp.bmWidth, bmp.bmHeight);
			}
			info.xHotspot = bmp.bmWidth - 1 - info.xHotspot;

			reverseArrowCursor = ::CreateIconIndirect(&info);
		}

		::DeleteObject(info.hbmMask);
		if (info.hbmColor) {
			::DeleteObject(info.hbmColor);
		}
	}

	if (created) {
		::DestroyCursor(cursor);
	}
	return reverseArrowCursor;
}

#else // draw reverse arrow cursor
namespace {

std::optional<DWORD> RegGetDWORD(HKEY hKey, LPCWSTR valueName) noexcept {
	DWORD value = 0;
	DWORD type = REG_NONE;
	DWORD size = sizeof(DWORD);
	const LSTATUS status = ::RegQueryValueExW(hKey, valueName, nullptr, &type, reinterpret_cast<LPBYTE>(&value), &size);
	if (status == ERROR_SUCCESS && type == REG_DWORD) {
		return value;
	}
	return {};
}

class CursorHelper {
	GDIBitMap bm;
	DWORD *pixels = nullptr;
	const int width;
	const int height;
	const float scale;
	static constexpr float baseSize = 32.0f;

	static constexpr float arrow[][2] = {
		{ 32.0f - 12.73606f,32.0f - 19.04075f },
		{ 32.0f - 7.80159f, 32.0f - 19.04075f },
		{ 32.0f - 9.82813f, 32.0f - 14.91828f },
		{ 32.0f - 6.88341f, 32.0f - 13.42515f },
		{ 32.0f - 4.62301f, 32.0f - 18.05872f },
		{ 32.0f - 1.26394f, 32.0f - 14.78295f },
		{ 32.0f - 1.26394f, 32.0f - 30.57485f },
	};

public:
	~CursorHelper() = default;

	CursorHelper(int width_, int height_) noexcept : width{width_}, height{height_}, scale{ static_cast<float>(width) / baseSize } {
		// https://learn.microsoft.com/en-us/windows/win32/menurc/using-cursors#creating-a-cursor
		bm.Create({}, width, height, &pixels);
	}

	[[nodiscard]] explicit operator bool() const noexcept {
		return static_cast<bool>(bm);
	}

	HCURSOR Create() noexcept {
		HCURSOR cursor {};
		// Create an empty mask bitmap.
		HBITMAP hMonoBitmap = ::CreateBitmap(width, height, 1, 1, nullptr);
		if (hMonoBitmap) {
			// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createiconindirect
			// hBitmap should not already be selected into a device context
			HBITMAP hBitmap = bm.Extract();
			ICONINFO info = {false, static_cast<DWORD>(width - 1), 0, hMonoBitmap, hBitmap};
			cursor = ::CreateIconIndirect(&info);
			::DeleteObject(hMonoBitmap);
		}
		return cursor;
	}

	bool DrawD2D(COLORREF fillColour, COLORREF strokeColour) noexcept {
		if (!LoadD2D()) {
			return false;
		}

		const D2D1_RENDER_TARGET_PROPERTIES drtp = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED });

		DCRenderTarget pTarget;
		HRESULT hr = CreateDCRenderTarget(&drtp, pTarget);
		if (FAILED(hr) || !pTarget) {
			return false;
		}

		const RECT rc = {0, 0, width, height};
		hr = pTarget->BindDC(bm.DC(), &rc);
		if (FAILED(hr)) {
			return false;
		}

		pTarget->BeginDraw();
		// Clear to transparent
		// pTarget->Clear(D2D1::ColorF(0.0, 0.0, 0.0, 0.0));

		// Draw something on the bitmap section.
		constexpr size_t nPoints = std::size(arrow);
		D2D1_POINT_2F points[nPoints];
		for (size_t i = 0; i < nPoints; i++) {
			points[i].x = arrow[i][0] * scale;
			points[i].y = arrow[i][1] * scale;
		}

		const Geometry geometry = GeometryCreate();
		if (!geometry) {
			return false;
		}

		const GeometrySink sink = GeometrySinkCreate(geometry.Get());
		if (!sink) {
			return false;
		}

		sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
		for (size_t i = 1; i < nPoints; i++) {
			sink->AddLine(points[i]);
		}
		sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		hr = sink->Close();
		if (FAILED(hr)) {
			return false;
		}

		if (const BrushSolid pBrushFill = BrushSolidCreate(pTarget.Get(), fillColour)) {
			pTarget->FillGeometry(geometry.Get(), pBrushFill.Get());
		}

		if (const BrushSolid pBrushStroke = BrushSolidCreate(pTarget.Get(), strokeColour)) {
			pTarget->DrawGeometry(geometry.Get(), pBrushStroke.Get(), scale);
		}

		hr = pTarget->EndDraw();
		return SUCCEEDED(hr);
	}

	void Draw(COLORREF fillColour, COLORREF strokeColour) noexcept {
		if (DrawD2D(fillColour, strokeColour)) {
			return;
		}

		// Draw something on the DIB section.
		constexpr size_t nPoints = std::size(arrow);
		POINT points[nPoints];
		for (size_t i = 0; i < nPoints; i++) {
			points[i].x = std::lround(arrow[i][0] * scale);
			points[i].y = std::lround(arrow[i][1] * scale);
		}

		const DWORD penWidth = std::lround(scale);
		HPEN pen{};
		if (penWidth > 1) {
			const LOGBRUSH brushParameters { BS_SOLID, strokeColour, 0 };
			pen = ::ExtCreatePen(PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_MITER,
				penWidth,
				&brushParameters,
				0,
				nullptr);
		} else {
			pen = ::CreatePen(PS_INSIDEFRAME, 1, strokeColour);
		}

		HPEN penOld = SelectPen(bm.DC(), pen);
		HBRUSH brush = ::CreateSolidBrush(fillColour);
		HBRUSH brushOld = SelectBrush(bm.DC(), brush);
		::Polygon(bm.DC(), points, static_cast<int>(nPoints));
		SelectPen(bm.DC(), penOld);
		SelectBrush(bm.DC(), brushOld);
		::DeleteObject(pen);
		::DeleteObject(brush);

		// Set the alpha values for each pixel in the cursor.
		constexpr DWORD opaque = 0xFF000000U;
		for (int i = 0; i < width*height; i++) {
			if (*pixels != 0) {
				*pixels |= opaque;
			}
			pixels++;
		}
	}
};

}

HCURSOR LoadReverseArrowCursor(HCURSOR cursor, UINT dpi) noexcept {
	// https://learn.microsoft.com/en-us/answers/questions/815036/windows-cursor-size
	constexpr DWORD defaultCursorBaseSize = 32;
	constexpr DWORD maxCursorBaseSize = 16*(1 + 15); // 16*(1 + CursorSize)
	DWORD cursorBaseSize = 0;
	HKEY hKey {};
	LSTATUS status = ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Control Panel\\Cursors", 0, KEY_QUERY_VALUE, &hKey);
	if (status == ERROR_SUCCESS) {
		if (auto baseSize = RegGetDWORD(hKey, L"CursorBaseSize")) {
			// CursorBaseSize is multiple of 16
			cursorBaseSize = std::min(*baseSize & ~15, maxCursorBaseSize);
		}
		::RegCloseKey(hKey);
	}

	int width;
	int height;
	if (cursorBaseSize > defaultCursorBaseSize) {
		width = ::MulDiv(cursorBaseSize, dpi, USER_DEFAULT_SCREEN_DPI);
		height = width;
	} else {
		width = SystemMetricsForDpi(SM_CXCURSOR, dpi);
		height = SystemMetricsForDpi(SM_CYCURSOR, dpi);
		PLATFORM_ASSERT(width == height);
	}

	CursorHelper cursorHelper(width, height);
	if (!cursorHelper) {
		return {};
	}

	COLORREF fillColour = RGB(0xff, 0xff, 0xfe);
	COLORREF strokeColour = RGB(0, 0, 1);
	status = ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Accessibility", 0, KEY_QUERY_VALUE, &hKey);
	if (status == ERROR_SUCCESS) {
		if (auto cursorType = RegGetDWORD(hKey, L"CursorType")) {
			switch (*cursorType) {
			case 1: // black
			case 4: // black
				std::swap(fillColour, strokeColour);
				break;
			case 6: // custom
				if (auto cursorColor = RegGetDWORD(hKey, L"CursorColor")) {
					fillColour = *cursorColor;
				}
				break;
			default: // 0, 3 white, 2, 5 invert
				break;
			}
		}
		::RegCloseKey(hKey);
	}

	cursorHelper.Draw(fillColour, strokeColour);
	cursor = cursorHelper.Create();
	return cursor;
}
#endif // reverse arrow cursor

namespace {

void ChooseCursor(LPCTSTR cursor) noexcept {
	::SetCursor(::LoadCursor({}, cursor));
}

void ChooseCursor(Window::Cursor curs) noexcept {
	switch (curs) {
	case Window::Cursor::text:
		ChooseCursor(IDC_IBEAM);
		break;
	case Window::Cursor::up:
		ChooseCursor(IDC_UPARROW);
		break;
	case Window::Cursor::wait:
		ChooseCursor(IDC_WAIT);
		break;
	case Window::Cursor::horizontal:
		ChooseCursor(IDC_SIZEWE);
		break;
	case Window::Cursor::vertical:
		ChooseCursor(IDC_SIZENS);
		break;
	case Window::Cursor::hand:
		ChooseCursor(IDC_HAND);
		break;
	case Window::Cursor::reverseArrow:
	case Window::Cursor::arrow:
	case Window::Cursor::invalid:	// Should not occur, but just in case.
	default:
		ChooseCursor(IDC_ARROW);
		break;
	}
}

}

void Window::SetCursor(Cursor curs) noexcept {
	ChooseCursor(curs);
}

/* Returns rectangle of monitor pt is on, both rect and pt are in Window's
   coordinates */
PRectangle Window::GetMonitorRect(Point pt) const noexcept {
	const PRectangle rcPosition = GetPosition();
	const POINT ptDesktop = { static_cast<LONG>(pt.x + rcPosition.left),
		static_cast<LONG>(pt.y + rcPosition.top) };
	HMONITOR hMonitor = MonitorFromPoint(ptDesktop, MONITOR_DEFAULTTONEAREST);

	const RECT rcWork = RectFromMonitor(hMonitor);
	if (rcWork.left < rcWork.right) {
		const PRectangle rcMonitor(
			rcWork.left - rcPosition.left,
			rcWork.top - rcPosition.top,
			rcWork.right - rcPosition.left,
			rcWork.bottom - rcPosition.top);
		return rcMonitor;
	}
	return PRectangle();
}

void Menu::CreatePopUp() noexcept {
	Destroy();
	mid = ::CreatePopupMenu();
}

void Menu::Destroy() noexcept {
	if (mid)
		::DestroyMenu(static_cast<HMENU>(mid));
	mid = {};
}

void Menu::Show(Point pt, const Window &w) noexcept {
	::TrackPopupMenu(static_cast<HMENU>(mid),
		TPM_RIGHTBUTTON, static_cast<int>(pt.x - 4), static_cast<int>(pt.y), 0,
		HwndFromWindow(w), nullptr);
	Destroy();
}

ColourRGBA ColourFromSys(int nIndex) noexcept {
	const DWORD colourValue = ::GetSysColor(nIndex);
	return ColourRGBA::FromRGB(colourValue);
}

ColourRGBA Platform::Chrome() noexcept {
	return ColourFromSys(COLOR_3DFACE);
}

ColourRGBA Platform::ChromeHighlight() noexcept {
	return ColourFromSys(COLOR_3DHIGHLIGHT);
}

const char *Platform::DefaultFont() noexcept {
	return "Verdana";
}

int Platform::DefaultFontSize() noexcept {
	return 10;
}

unsigned int Platform::DoubleClickTime() noexcept {
	return ::GetDoubleClickTime();
}

//#define TRACE

#ifdef TRACE
void Platform::DebugDisplay(const char *s) noexcept {
	::OutputDebugStringA(s);
}

void Platform::DebugPrintf(const char *format, ...) noexcept {
	char buffer[2000];
	va_list pArguments;
	va_start(pArguments, format);
	vsprintf(buffer, format, pArguments);
	va_end(pArguments);
	Platform::DebugDisplay(buffer);
}

static bool assertionPopUps = true;

bool Platform::ShowAssertionPopUps(bool assertionPopUps_) noexcept {
	const bool ret = assertionPopUps;
	assertionPopUps = assertionPopUps_;
	return ret;
}

void Platform::Assert(const char *c, const char *file, int line) noexcept {
	char buffer[2000]{};
	sprintf(buffer, "Assertion [%s] failed at %s %d%s", c, file, line, assertionPopUps ? "" : "\r\n");
	if (assertionPopUps) {
		const int idButton = ::MessageBoxA({}, buffer, "Assertion failure",
			MB_ABORTRETRYIGNORE | MB_ICONHAND | MB_SETFOREGROUND | MB_TASKMODAL);
		if (idButton == IDRETRY) {
			::DebugBreak();
		} else if (idButton == IDIGNORE) {
			// all OK
		} else {
			abort();
		}
	} else {
		Platform::DebugDisplay(buffer);
		::DebugBreak();
		abort();
	}
}

#else
void Platform::DebugDisplay(const char *) noexcept {
}

void Platform::DebugPrintf(const char *, ...) noexcept {
}

bool Platform::ShowAssertionPopUps(bool) noexcept {
	return false;
}

void Platform::Assert(const char *, const char *, int) noexcept {
}
#endif // TRACE

void Platform_Initialise(void *hInstance) noexcept {
	hinstPlatformRes = static_cast<HINSTANCE>(hInstance);
	//LoadDpiForWindow() is moved into wWinMain().
	ListBoxX_Register();
}

void Platform_Finalise(bool fromDllMain) noexcept {
	if (!fromDllMain) {
		ReleaseD2D();
#if !NP2_HAS_GETDPIFORWINDOW
		ReleaseLibrary(hShcoreDLL);
#endif
	}
	ListBoxX_Unregister();
}

}
