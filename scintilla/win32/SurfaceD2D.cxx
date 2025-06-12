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

using namespace Scintilla;
using namespace Scintilla::Internal;

namespace {

//#pragma comment(lib, "D2D1.lib")
extern "C" const GUID __declspec(selectany) IID_IDWriteFactory2 = // 0439fc60-ca44-4994-8dee-3a9af7b732ec
{ 0x0439fc60, 0xca44, 0x4994, { 0x8d, 0xee, 0x3a, 0x9a, 0xf7, 0xb7, 0x32, 0xec } };

IDWriteGdiInterop *gdiInterop = nullptr;
D2D1_DRAW_TEXT_OPTIONS d2dDrawTextOptions = D2D1_DRAW_TEXT_OPTIONS_NONE;

HMODULE hDLLD2D {};
HMODULE hDLLDWrite {};

#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
HMODULE hDLLD3D{};
PFN_D3D11_CREATE_DEVICE fnDCD {};
#endif

#if USE_WIN32_INIT_ONCE
BOOL CALLBACK LoadD2DOnce([[maybe_unused]] PINIT_ONCE initOnce, [[maybe_unused]] PVOID parameter, [[maybe_unused]] PVOID *lpContext) noexcept
#else
void LoadD2DOnce() noexcept
#endif
{
	// Availability of SetDefaultDllDirectories implies Windows 8+ or
	// that KB2533623 has been installed so LoadLibraryEx can be called
	// with LOAD_LIBRARY_SEARCH_SYSTEM32.

	using D2D1CreateFactorySig = HRESULT(WINAPI *)(D2D1_FACTORY_TYPE factoryType, REFIID riid,
		CONST D2D1_FACTORY_OPTIONS *pFactoryOptions, IUnknown **factory);
	using DWriteCreateFactorySig = HRESULT(WINAPI *)(DWRITE_FACTORY_TYPE factoryType, REFIID iid,
		IUnknown **factory);

	hDLLD2D = ::LoadLibraryEx(L"d2d1.dll", {}, kSystemLibraryLoadFlags);
	if (hDLLD2D) {
		D2D1CreateFactorySig fnD2DCF = DLLFunction<D2D1CreateFactorySig>(hDLLD2D, "D2D1CreateFactory");
		if (fnD2DCF) {
#ifdef NDEBUG
			// A multi threaded factory in case Scintilla is used with multiple GUI threads
			// fnD2DCF(D2D1_FACTORY_TYPE_MULTI_THREADED,
			// A single threaded factory as Scintilla always draw on the GUI thread
			fnD2DCF(D2D1_FACTORY_TYPE_SINGLE_THREADED,
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
				__uuidof(ID2D1Factory1),
#else
				__uuidof(ID2D1Factory),
#endif
				nullptr,
				reinterpret_cast<IUnknown**>(&pD2DFactory));
#else
			D2D1_FACTORY_OPTIONS options = {};
			options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
			fnD2DCF(D2D1_FACTORY_TYPE_SINGLE_THREADED,
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
				__uuidof(ID2D1Factory1),
#else
				__uuidof(ID2D1Factory),
#endif
				&options,
				reinterpret_cast<IUnknown**>(&pD2DFactory));
#endif
		}
	}

	hDLLDWrite = ::LoadLibraryEx(L"dwrite.dll", {}, kSystemLibraryLoadFlags);
	if (hDLLDWrite) {
		DWriteCreateFactorySig fnDWCF = DLLFunction<DWriteCreateFactorySig>(hDLLDWrite, "DWriteCreateFactory");
		if (fnDWCF) {
			HRESULT hr = fnDWCF(DWRITE_FACTORY_TYPE_SHARED,
				IID_IDWriteFactory2,
				reinterpret_cast<IUnknown**>(&pIDWriteFactory));
			if (SUCCEEDED(hr)) {
				// D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT
				d2dDrawTextOptions = static_cast<D2D1_DRAW_TEXT_OPTIONS>(0x00000004);
			} else {
				hr = fnDWCF(DWRITE_FACTORY_TYPE_SHARED,
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
					__uuidof(IDWriteFactory1),
#else
					__uuidof(IDWriteFactory),
#endif
					reinterpret_cast<IUnknown**>(&pIDWriteFactory));
			}
			if (SUCCEEDED(hr)) {
				pIDWriteFactory->GetGdiInterop(&gdiInterop);
			}
		}
	}

#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
	hDLLD3D = ::LoadLibraryEx(TEXT("D3D11.DLL"), {}, kSystemLibraryLoadFlags);
	if (hDLLD3D) {
		fnDCD = DLLFunction<PFN_D3D11_CREATE_DEVICE>(hDLLD3D, "D3D11CreateDevice");
		if (!fnDCD) {
			// Platform::DebugPrintf("Direct3D does not have D3D11CreateDevice\n");
		}
	} else {
		// Platform::DebugPrintf("Direct3D not loaded\n");
	}
#endif

#if USE_WIN32_INIT_ONCE
	return TRUE;
#endif
}

constexpr D2D1_TEXT_ANTIALIAS_MODE DWriteMapFontQuality(FontQuality extraFontFlag) noexcept {
	constexpr UINT mask = (D2D1_TEXT_ANTIALIAS_MODE_DEFAULT << static_cast<int>(FontQuality::QualityDefault))
		| (D2D1_TEXT_ANTIALIAS_MODE_ALIASED << (2 * static_cast<int>(FontQuality::QualityNonAntialiased)))
		| (D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE << (2 * static_cast<int>(FontQuality::QualityAntialiased)))
		| (D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE << (2 * static_cast<int>(FontQuality::QualityLcdOptimized)))
		;
	return static_cast<D2D1_TEXT_ANTIALIAS_MODE>((mask >> (2 * static_cast<int>(extraFontFlag & FontQuality::QualityMask))) & 3);
}

bool GetDWriteFontProperties(const LOGFONTW &lf, std::wstring &wsFamily,
	DWRITE_FONT_WEIGHT &weight, DWRITE_FONT_STYLE &style, DWRITE_FONT_STRETCH &stretch) {
	bool success = false;
	if (gdiInterop) {
		ComPtr<IDWriteFont> font;
		HRESULT hr = gdiInterop->CreateFontFromLOGFONT(&lf, font.GetAddressOf());
		if (SUCCEEDED(hr)) {
			weight = font->GetWeight();
			style = font->GetStyle();
			stretch = font->GetStretch();

			ComPtr<IDWriteFontFamily> family;
			hr = font->GetFontFamily(family.GetAddressOf());
			if (SUCCEEDED(hr)) {
				ComPtr<IDWriteLocalizedStrings> names;
				hr = family->GetFamilyNames(names.GetAddressOf());
				if (SUCCEEDED(hr)) {
					UINT32 index = 0;
					BOOL exists = false;
					names->FindLocaleName(L"en-us", &index, &exists);
					if (!exists) {
						index = 0;
					}

					UINT32 length = 0;
					names->GetStringLength(index, &length);

					wsFamily.resize(length + 1);
					names->GetString(index, wsFamily.data(), length + 1);
					success = wsFamily[0] != L'\0';
				}
			}
		}
	}
	return success;
}

}

namespace Scintilla::Internal {

FontWin::FontWin(const FontParameters &fp) {
	extraFontFlag = fp.extraFontFlag;
	// The negative is to allow for leading
	lf.lfHeight = -std::abs(std::lround(fp.size));
	lf.lfWeight = static_cast<LONG>(fp.weight);
	lf.lfItalic = fp.italic ? 1 : 0;
	lf.lfCharSet = static_cast<BYTE>(fp.characterSet);
	lf.lfQuality = Win32MapFontQuality(fp.extraFontFlag);
	UTF16FromUTF8(fp.faceName, lf.lfFaceName, LF_FACESIZE);
	if (fp.technology == Technology::Default) {
		hfont = ::CreateFontIndirectW(&lf);
	} else {
		std::wstring wsFamily;
		DWRITE_FONT_WEIGHT weight = static_cast<DWRITE_FONT_WEIGHT>(fp.weight);
		DWRITE_FONT_STYLE style = fp.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;
		DWRITE_FONT_STRETCH stretch = static_cast<DWRITE_FONT_STRETCH>(fp.stretch);
		if (!GetDWriteFontProperties(lf, wsFamily, weight, style, stretch)) {
			wsFamily = WStringFromUTF8(fp.faceName);
		}

		const std::wstring wsLocale = WStringFromUTF8(fp.localeName);
		const FLOAT fHeight = static_cast<FLOAT>(fp.size);
		HRESULT hr = pIDWriteFactory->CreateTextFormat(wsFamily.c_str(), nullptr,
			weight, style, stretch, fHeight, wsLocale.c_str(), pTextFormat.GetAddressOf());
		if (hr == E_INVALIDARG) {
			// Possibly a bad locale name like "/" so try "en-us".
			hr = pIDWriteFactory->CreateTextFormat(wsFamily.c_str(), nullptr,
					weight, style, stretch, fHeight, L"en-us", pTextFormat.ReleaseAndGetAddressOf());
		}
		if (SUCCEEDED(hr)) {
			pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

			if (const TextLayout pTextLayout = LayoutCreate(L"X", pTextFormat.Get())) {
				constexpr int maxLines = 2;
				DWRITE_LINE_METRICS lineMetrics[maxLines]{};
				UINT32 lineCount = 0;
				hr = pTextLayout->GetLineMetrics(lineMetrics, maxLines, &lineCount);
				if (SUCCEEDED(hr)) {
					yAscent = lineMetrics[0].baseline;
					yDescent = lineMetrics[0].height - lineMetrics[0].baseline;

					FLOAT emHeight;
					hr = pTextLayout->GetFontSize(0, &emHeight);
					if (SUCCEEDED(hr)) {
						yInternalLeading = lineMetrics[0].height - emHeight;
					}
				}
				pTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_UNIFORM, lineMetrics[0].height, lineMetrics[0].baseline);
			}
		}
	}
}

}

namespace {

constexpr D2D1_RECT_F RectangleFromPRectangle(PRectangle rc) noexcept {
	return {
		static_cast<FLOAT>(rc.left),
		static_cast<FLOAT>(rc.top),
		static_cast<FLOAT>(rc.right),
		static_cast<FLOAT>(rc.bottom)
	};
}

#if NP2_USE_AVX2
static_assert(sizeof(PRectangle) == sizeof(__m256d));
static_assert(sizeof(D2D1_RECT_F) == sizeof(__m128));

inline D2D1_RECT_F RectangleFromPRectangleEx(PRectangle prc) noexcept {
	D2D1_RECT_F rc;
	const __m256d f64x4 = _mm256_load_pd(reinterpret_cast<double *>(&prc));
	const __m128 f32x4 = _mm256_cvtpd_ps(f64x4);
	_mm_storeu_ps(reinterpret_cast<float *>(&rc), f32x4);
	return rc;
}

#else
constexpr D2D1_RECT_F RectangleFromPRectangleEx(PRectangle prc) noexcept {
	return RectangleFromPRectangle(prc);
}
#endif

constexpr D2D1_POINT_2F DPointFromPoint(Point point) noexcept {
	return { static_cast<FLOAT>(point.x), static_cast<FLOAT>(point.y) };
}

#if NP2_USE_SSE2
static_assert(sizeof(Point) == sizeof(__m128d));
static_assert(sizeof(D2D1_POINT_2F) == sizeof(__m64));

inline D2D1_POINT_2F DPointFromPointEx(Point point) noexcept {
	D2D1_POINT_2F pt;
	const __m128d f64x2 = _mm_load_pd(reinterpret_cast<double *>(&point));
	const __m128 f32x2 = _mm_cvtpd_ps(f64x2);
	_mm_storel_pi(reinterpret_cast<__m64 *>(&pt), f32x2);
	return pt;
}

#else
constexpr D2D1_POINT_2F DPointFromPointEx(Point point) noexcept {
	return DPointFromPoint(point);
}
#endif

constexpr unsigned int SupportsD2D =
	(1 << static_cast<int>(Supports::LineDrawsFinal)) |
	(1 << static_cast<int>(Supports::FractionalStrokeWidth)) |
	(1 << static_cast<int>(Supports::TranslucentStroke)) |
	(1 << static_cast<int>(Supports::PixelModification)) |
	(1 << static_cast<int>(Supports::ThreadSafeMeasureWidths));

#if NP2_USE_SSE2
static_assert(sizeof(D2D_COLOR_F) == sizeof(__m128));

inline D2D_COLOR_F ColorFromColourAlpha(ColourRGBA colour) noexcept {
#if NP2_USE_AVX2
	const __m128i i32x4 = unpack_color_epi32_sse4_si32(colour.AsInteger());
#else
	const __m128i i32x4 = unpack_color_epi32_sse2_si32(colour.AsInteger());
#endif
	__m128 f32x4 = _mm_cvtepi32_ps(i32x4);
	f32x4 = _mm_div_ps(f32x4, _mm_set1_ps(255.0f));
	D2D_COLOR_F color;
	_mm_storeu_ps(reinterpret_cast<float *>(&color), f32x4);
	return color;
}

#else
constexpr D2D_COLOR_F ColorFromColourAlpha(ColourRGBA colour) noexcept {
	return D2D_COLOR_F {
		colour.GetRedComponent(),
		colour.GetGreenComponent(),
		colour.GetBlueComponent(),
		colour.GetAlphaComponent()
	};
}
#endif

constexpr D2D1_RECT_F RectangleInset(D2D1_RECT_F rect, FLOAT inset) noexcept {
	return D2D1_RECT_F {
		rect.left + inset,
		rect.top + inset,
		rect.right - inset,
		rect.bottom - inset
	};
}

class BlobInline;

class SurfaceD2D final : public Surface {
	// Text measuring surface: both pRenderTarget and pBitmapRenderTarget are null.
	// Window surface: pRenderTarget is valid but not pBitmapRenderTarget.
	// Bitmap drawing surface: both pRenderTarget and pBitmapRenderTarget are valid and the same.
	ComPtr<ID2D1RenderTarget> pRenderTarget;
	ComPtr<ID2D1BitmapRenderTarget> pBitmapRenderTarget;
	BrushSolid pBrush;

	SurfaceMode mode;
	int clipsActive = 0;

	static constexpr FontQuality invalidFontQuality = FontQuality::QualityMask;
	FontQuality fontQuality = invalidFontQuality;
	int logPixelsY = USER_DEFAULT_SCREEN_DPI;
	IDWriteRenderingParams *defaultRenderingParams = nullptr;
	IDWriteRenderingParams *customRenderingParams = nullptr;

	void Clear() noexcept;
	void SetFontQuality(FontQuality extraFontFlag) noexcept;
	HRESULT GetBitmap(ID2D1Bitmap **ppBitmap);

public:
	SurfaceD2D() noexcept = default;
	SurfaceD2D(ID2D1RenderTarget *pRenderTargetCompatible, int width, int height, SurfaceMode mode_, int logPixelsY_,
		IDWriteRenderingParams *defaultRenderingParams_, IDWriteRenderingParams *customRenderingParams_) noexcept;
	// Deleted so SurfaceD2D objects can not be copied.
	SurfaceD2D(const SurfaceD2D &) = delete;
	SurfaceD2D(SurfaceD2D &&) = delete;
	SurfaceD2D &operator=(const SurfaceD2D &) = delete;
	SurfaceD2D &operator=(SurfaceD2D &&) = delete;
	~SurfaceD2D() noexcept override;

	void Init(WindowID wid) noexcept override;
	void Init(SurfaceID sid, WindowID wid, bool printing = false) noexcept override;
	std::unique_ptr<Surface> AllocatePixMap(int width, int height) override;

	void SetMode(SurfaceMode mode_) noexcept override;
	void SetRenderingParams(void *defaultRenderingParams_, void *customRenderingParams_) noexcept override;

	void Release() noexcept override;
	bool SupportsFeature(Supports feature) const noexcept override;
	bool Initialised() const noexcept override;

	void D2DPenColourAlpha(ColourRGBA fore) noexcept;
	int LogPixelsY() const noexcept override;
	int PixelDivisions() const noexcept override;
	int DeviceHeightFont(int points) const noexcept override;
	void SCICALL LineDraw(Point start, Point end, Stroke stroke) override;
	static Geometry GeometricFigure(const Point *pts, size_t npts, D2D1_FIGURE_BEGIN figureBegin) noexcept;
	void SCICALL PolyLine(const Point *pts, size_t npts, Stroke stroke) override;
	void SCICALL Polygon(const Point *pts, size_t npts, FillStroke fillStroke) override;
	void SCICALL RectangleDraw(PRectangle rc, FillStroke fillStroke) override;
	void SCICALL RectangleFrame(PRectangle rc, Stroke stroke) override;
	void SCICALL FillRectangle(PRectangle rc, Fill fill) override;
	void SCICALL FillRectangleAligned(PRectangle rc, Fill fill) override;
	void SCICALL FillRectangle(PRectangle rc, Surface &surfacePattern) override;
	void SCICALL RoundedRectangle(PRectangle rc, FillStroke fillStroke) override;
	void SCICALL AlphaRectangle(PRectangle rc, XYPOSITION cornerSize, FillStroke fillStroke) override;
	void SCICALL GradientRectangle(PRectangle rc, const std::vector<ColourStop> &stops, GradientOptions options) override;
	void SCICALL DrawRGBAImage(PRectangle rc, int width, int height, const unsigned char *pixelsImage) override;
	void SCICALL Ellipse(PRectangle rc, FillStroke fillStroke) override;
	void SCICALL Stadium(PRectangle rc, FillStroke fillStroke, Ends ends) override;
	void SCICALL Copy(PRectangle rc, Point from, Surface &surfaceSource) override;

	std::unique_ptr<IScreenLineLayout> Layout(const IScreenLine *screenLine) override;

	void SCICALL DrawTextCommon(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, int codePageOverride, UINT fuOptions);

	void SCICALL DrawTextNoClip(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void SCICALL DrawTextClipped(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void SCICALL DrawTextTransparent(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore) override;
	void SCICALL MeasureWidths(const Font *font_, std::string_view text, XYPOSITION *positions) override;
	XYPOSITION WidthText(const Font *font_, std::string_view text) override;

	void SCICALL DrawTextNoClipUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void SCICALL DrawTextClippedUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void SCICALL DrawTextTransparentUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore) override;
	void SCICALL MeasureWidthsUTF8(const Font *font_, std::string_view text, XYPOSITION *positions) override;
	XYPOSITION WidthTextUTF8(const Font *font_, std::string_view text) override;

	XYPOSITION Ascent(const Font *font_) noexcept override;
	XYPOSITION Descent(const Font *font_) noexcept override;
	XYPOSITION InternalLeading(const Font *font_) noexcept override;
	XYPOSITION Height(const Font *font_) noexcept override;
	XYPOSITION AverageCharWidth(const Font *font_) override;

	void SCICALL SetClip(PRectangle rc) noexcept override;
	void PopClip() noexcept override;
	void FlushCachedState() noexcept override;

	void FlushDrawing() noexcept override;
};

SurfaceD2D::SurfaceD2D(ID2D1RenderTarget *pRenderTargetCompatible, int width, int height, SurfaceMode mode_, int logPixelsY_,
	IDWriteRenderingParams *defaultRenderingParams_, IDWriteRenderingParams *customRenderingParams_) noexcept {
	const D2D1_SIZE_F desiredSize = D2D1::SizeF(static_cast<float>(width), static_cast<float>(height));
	D2D1_PIXEL_FORMAT desiredFormat;
#ifdef __MINGW32__
	desiredFormat.format = DXGI_FORMAT_UNKNOWN;
#else
	desiredFormat = pRenderTargetCompatible->GetPixelFormat();
#endif
	desiredFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
	const HRESULT hr = pRenderTargetCompatible->CreateCompatibleRenderTarget(
		&desiredSize, nullptr, &desiredFormat, D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE, pBitmapRenderTarget.GetAddressOf());
	if (SUCCEEDED(hr)) {
		pRenderTarget = pBitmapRenderTarget;
		pRenderTarget->BeginDraw();
	}
	mode = mode_;
	logPixelsY = logPixelsY_;
	defaultRenderingParams = defaultRenderingParams_;
	customRenderingParams = customRenderingParams_;
}

SurfaceD2D::~SurfaceD2D() noexcept {
	Clear();
}

void SurfaceD2D::Clear() noexcept {
	pBrush = nullptr;
	if (pRenderTarget) {
		while (clipsActive) {
			pRenderTarget->PopAxisAlignedClip();
			clipsActive--;
		}
		if (pBitmapRenderTarget) {
			pRenderTarget->EndDraw();
		}
	}
	pRenderTarget = nullptr;
	pBitmapRenderTarget = nullptr;
}

void SurfaceD2D::Release() noexcept {
	Clear();
}

bool SurfaceD2D::SupportsFeature(Supports feature) const noexcept {
	return (SupportsD2D >> static_cast<int>(feature)) & true;
}

bool SurfaceD2D::Initialised() const noexcept {
	return pRenderTarget;
}

void SurfaceD2D::Init(WindowID wid) noexcept {
	fontQuality = invalidFontQuality;
	logPixelsY = DpiForWindow(wid);
}

void SurfaceD2D::Init(SurfaceID sid, WindowID wid, bool /*printing*/) noexcept {
	// printing always using GDI
	pRenderTarget = static_cast<ID2D1RenderTarget *>(sid);
	fontQuality = invalidFontQuality;
	logPixelsY = DpiForWindow(wid);
}

std::unique_ptr<Surface> SurfaceD2D::AllocatePixMap(int width, int height) {
	return std::make_unique<SurfaceD2D>(pRenderTarget.Get(), width, height, mode, logPixelsY, defaultRenderingParams, customRenderingParams);
}

void SurfaceD2D::SetMode(SurfaceMode mode_) noexcept {
	mode = mode_;
}

void SurfaceD2D::SetRenderingParams(void *defaultRenderingParams_, void *customRenderingParams_) noexcept {
	defaultRenderingParams = static_cast<IDWriteRenderingParams *>(defaultRenderingParams_);
	customRenderingParams = static_cast<IDWriteRenderingParams *>(customRenderingParams_);
}

HRESULT SurfaceD2D::GetBitmap(ID2D1Bitmap **ppBitmap) {
	PLATFORM_ASSERT(pBitmapRenderTarget);
	return pBitmapRenderTarget->GetBitmap(ppBitmap);
}

void SurfaceD2D::D2DPenColourAlpha(ColourRGBA fore) noexcept {
	if (pRenderTarget) {
		const D2D_COLOR_F col = ColorFromColourAlpha(fore);
		if (pBrush) {
			pBrush->SetColor(col);
		} else {
			const HRESULT hr = pRenderTarget->CreateSolidColorBrush(col, &pBrush);
			if (!SUCCEEDED(hr)) {
				pBrush = nullptr;
			}
		}
	}
}

void SurfaceD2D::SetFontQuality(FontQuality extraFontFlag) noexcept {
	if (fontQuality != extraFontFlag) {
		fontQuality = extraFontFlag;
		const D2D1_TEXT_ANTIALIAS_MODE aaMode = DWriteMapFontQuality(extraFontFlag);
		if (aaMode == D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE && customRenderingParams) {
			pRenderTarget->SetTextRenderingParams(customRenderingParams);
		} else if (defaultRenderingParams) {
			pRenderTarget->SetTextRenderingParams(defaultRenderingParams);
		}
		pRenderTarget->SetTextAntialiasMode(aaMode);
	}
}

int SurfaceD2D::LogPixelsY() const noexcept {
	return logPixelsY;
}

int SurfaceD2D::PixelDivisions() const noexcept {
	// Win32 uses device pixels.
	return 1;
}

int SurfaceD2D::DeviceHeightFont(int points) const noexcept {
	return ::MulDiv(points, logPixelsY, pointsPerInch);
}

constexpr FLOAT mitreLimit = 4.0f;

void SurfaceD2D::LineDraw(Point start, Point end, Stroke stroke) {
	D2DPenColourAlpha(stroke.colour);

	D2D1_STROKE_STYLE_PROPERTIES strokeProps {};
	strokeProps.startCap = D2D1_CAP_STYLE_SQUARE;
	strokeProps.endCap = D2D1_CAP_STYLE_SQUARE;
	strokeProps.dashCap = D2D1_CAP_STYLE_FLAT;
	strokeProps.lineJoin = D2D1_LINE_JOIN_MITER;
	strokeProps.miterLimit = mitreLimit;
	strokeProps.dashStyle = D2D1_DASH_STYLE_SOLID;
	strokeProps.dashOffset = 0;

	// get the stroke style to apply
	if (const StrokeStyle pStrokeStyle = StrokeStyleCreate(strokeProps)) {
		pRenderTarget->DrawLine(
			DPointFromPointEx(start),
			DPointFromPoint(end), pBrush.Get(), stroke.WidthF(), pStrokeStyle.Get());
	}
}

Geometry SurfaceD2D::GeometricFigure(const Point *pts, size_t npts, D2D1_FIGURE_BEGIN figureBegin) noexcept {
	Geometry geometry = GeometryCreate();
	if (geometry) {
		if (const GeometrySink sink = GeometrySinkCreate(geometry.Get())) {
			sink->BeginFigure(DPointFromPointEx(pts[0]), figureBegin);
			for (size_t i = 1; i < npts; i++) {
				sink->AddLine(DPointFromPointEx(pts[i]));
			}
			sink->EndFigure((figureBegin == D2D1_FIGURE_BEGIN_FILLED) ?
				D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
			sink->Close();
		}
	}
	return geometry;
}

void SurfaceD2D::PolyLine(const Point *pts, size_t npts, Stroke stroke) {
	PLATFORM_ASSERT(pRenderTarget && (npts > 1));
	if (!pRenderTarget || (npts <= 1)) {
		return;
	}

	const Geometry geometry = GeometricFigure(pts, npts, D2D1_FIGURE_BEGIN_HOLLOW);
	PLATFORM_ASSERT(geometry);
	if (!geometry) {
		return;
	}

	D2DPenColourAlpha(stroke.colour);
	D2D1_STROKE_STYLE_PROPERTIES strokeProps {};
	strokeProps.startCap = D2D1_CAP_STYLE_ROUND;
	strokeProps.endCap = D2D1_CAP_STYLE_ROUND;
	strokeProps.dashCap = D2D1_CAP_STYLE_FLAT;
	strokeProps.lineJoin = D2D1_LINE_JOIN_MITER;
	strokeProps.miterLimit = mitreLimit;
	strokeProps.dashStyle = D2D1_DASH_STYLE_SOLID;
	strokeProps.dashOffset = 0;

	// get the stroke style to apply
	if (const StrokeStyle pStrokeStyle = StrokeStyleCreate(strokeProps)) {
		pRenderTarget->DrawGeometry(geometry.Get(), pBrush.Get(), stroke.WidthF(), pStrokeStyle.Get());
	}
}

void SurfaceD2D::Polygon(const Point *pts, size_t npts, FillStroke fillStroke) {
	PLATFORM_ASSERT(pRenderTarget && (npts > 2));
	if (pRenderTarget) {
		const Geometry geometry = GeometricFigure(pts, npts, D2D1_FIGURE_BEGIN_FILLED);
		PLATFORM_ASSERT(geometry);
		if (geometry) {
			D2DPenColourAlpha(fillStroke.fill.colour);
			pRenderTarget->FillGeometry(geometry.Get(), pBrush.Get());
			D2DPenColourAlpha(fillStroke.stroke.colour);
			pRenderTarget->DrawGeometry(geometry.Get(), pBrush.Get(), fillStroke.stroke.WidthF());
		}
	}
}

void SurfaceD2D::RectangleDraw(PRectangle rc, FillStroke fillStroke) {
	if (!pRenderTarget)
		return;
	const D2D1_RECT_F rect = RectangleFromPRectangle(rc);
	const D2D1_RECT_F rectFill = RectangleInset(rect, fillStroke.stroke.WidthF());
	const float halfStroke = fillStroke.stroke.WidthF() / 2.0f;
	const D2D1_RECT_F rectOutline = RectangleInset(rect, halfStroke);

	D2DPenColourAlpha(fillStroke.fill.colour);
	pRenderTarget->FillRectangle(&rectFill, pBrush.Get());
	D2DPenColourAlpha(fillStroke.stroke.colour);
	pRenderTarget->DrawRectangle(&rectOutline, pBrush.Get(), fillStroke.stroke.WidthF());
}

void SurfaceD2D::RectangleFrame(PRectangle rc, Stroke stroke) {
	if (pRenderTarget) {
		const XYPOSITION halfStroke = stroke.width / 2.0f;
		const D2D1_RECT_F rectangle1 = RectangleFromPRectangle(rc.Inset(halfStroke));
		D2DPenColourAlpha(stroke.colour);
		pRenderTarget->DrawRectangle(&rectangle1, pBrush.Get(), stroke.WidthF());
	}
}

void SurfaceD2D::FillRectangle(PRectangle rc, Fill fill) {
	if (pRenderTarget) {
		D2DPenColourAlpha(fill.colour);
		const D2D1_RECT_F rectangle = RectangleFromPRectangleEx(rc);
		pRenderTarget->FillRectangle(&rectangle, pBrush.Get());
	}
}

void SurfaceD2D::FillRectangleAligned(PRectangle rc, Fill fill) {
	FillRectangle(PixelAlign(rc, 1), fill);
}

void SurfaceD2D::FillRectangle(PRectangle rc, Surface &surfacePattern) {
	SurfaceD2D *psurfOther = down_cast<SurfaceD2D *>(&surfacePattern);
	PLATFORM_ASSERT(psurfOther);
	ComPtr<ID2D1Bitmap> pBitmap;
	HRESULT hr = psurfOther->GetBitmap(pBitmap.GetAddressOf());
	if (SUCCEEDED(hr) && pBitmap) {
		ComPtr<ID2D1BitmapBrush> pBitmapBrush;
		const D2D1_BITMAP_BRUSH_PROPERTIES brushProperties =
			D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		// Create the bitmap brush.
		hr = pRenderTarget->CreateBitmapBrush(pBitmap.Get(), brushProperties, pBitmapBrush.GetAddressOf());
		if (SUCCEEDED(hr) && pBitmapBrush) {
			pRenderTarget->FillRectangle(RectangleFromPRectangleEx(rc), pBitmapBrush.Get());
		}
	}
}

void SurfaceD2D::RoundedRectangle(PRectangle rc, FillStroke fillStroke) {
	if (pRenderTarget) {
		const FLOAT minDimension = static_cast<FLOAT>(std::min(rc.Width(), rc.Height())) / 2.0f;
		const FLOAT radius = std::min(4.0f, minDimension);
		if (fillStroke.fill.colour == fillStroke.stroke.colour) {
			const D2D1_ROUNDED_RECT roundedRectFill = {
				RectangleFromPRectangle(rc),
				radius, radius };
			D2DPenColourAlpha(fillStroke.fill.colour);
			pRenderTarget->FillRoundedRectangle(roundedRectFill, pBrush.Get());
		} else {
			const D2D1_ROUNDED_RECT roundedRectFill = {
				RectangleFromPRectangle(rc.Inset(1.0)),
				radius-1, radius-1 };
			D2DPenColourAlpha(fillStroke.fill.colour);
			pRenderTarget->FillRoundedRectangle(roundedRectFill, pBrush.Get());

			const D2D1_ROUNDED_RECT roundedRect = {
				RectangleFromPRectangle(rc.Inset(0.5)),
				radius, radius };
			D2DPenColourAlpha(fillStroke.stroke.colour);
			pRenderTarget->DrawRoundedRectangle(roundedRect, pBrush.Get(), fillStroke.stroke.WidthF());
		}
	}
}

void SurfaceD2D::AlphaRectangle(PRectangle rc, XYPOSITION cornerSize, FillStroke fillStroke) {
	const D2D1_RECT_F rect = RectangleFromPRectangle(rc);
	const D2D1_RECT_F rectFill = RectangleInset(rect, fillStroke.stroke.WidthF());
	const float halfStroke = fillStroke.stroke.WidthF() / 2.0f;
	const D2D1_RECT_F rectOutline = RectangleInset(rect, halfStroke);
	if (pRenderTarget) {
		if (cornerSize == 0) {
			// When corner size is zero, draw square rectangle to prevent blurry pixels at corners
			D2DPenColourAlpha(fillStroke.fill.colour);
			pRenderTarget->FillRectangle(rectFill, pBrush.Get());

			D2DPenColourAlpha(fillStroke.stroke.colour);
			pRenderTarget->DrawRectangle(rectOutline, pBrush.Get(), fillStroke.stroke.WidthF());
		} else {
			const float cornerSizeF = static_cast<float>(cornerSize);
			const D2D1_ROUNDED_RECT roundedRectFill = {
				rectFill, cornerSizeF - 1.0f, cornerSizeF - 1.0f };
			D2DPenColourAlpha(fillStroke.fill.colour);
			pRenderTarget->FillRoundedRectangle(roundedRectFill, pBrush.Get());

			const D2D1_ROUNDED_RECT roundedRect = { rectOutline, cornerSizeF, cornerSizeF };
			D2DPenColourAlpha(fillStroke.stroke.colour);
			pRenderTarget->DrawRoundedRectangle(roundedRect, pBrush.Get(), fillStroke.stroke.WidthF());
		}
	}
}

void SurfaceD2D::GradientRectangle(PRectangle rc, const std::vector<ColourStop> &stops, GradientOptions options) {
	if (pRenderTarget) {
		D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES lgbp {
			DPointFromPoint(Point(rc.left, rc.top)), {}
		};
		switch (options) {
		case GradientOptions::leftToRight:
			lgbp.endPoint = DPointFromPoint(Point(rc.right, rc.top));
			break;
		case GradientOptions::topToBottom:
		default:
			lgbp.endPoint = DPointFromPoint(Point(rc.left, rc.bottom));
			break;
		}

		std::vector<D2D1_GRADIENT_STOP> gradientStops;
		for (const ColourStop &stop : stops) {
			gradientStops.push_back({ static_cast<FLOAT>(stop.position), ColorFromColourAlpha(stop.colour) });
		}
		ComPtr<ID2D1GradientStopCollection> pGradientStops;
		HRESULT hr = pRenderTarget->CreateGradientStopCollection(
			gradientStops.data(), static_cast<UINT32>(gradientStops.size()), pGradientStops.GetAddressOf());
		if (FAILED(hr) || !pGradientStops) {
			return;
		}
		ComPtr<ID2D1LinearGradientBrush> pBrushLinear;
		hr = pRenderTarget->CreateLinearGradientBrush(
			lgbp, pGradientStops.Get(), pBrushLinear.GetAddressOf());
		if (SUCCEEDED(hr) && pBrushLinear) {
			const D2D1_RECT_F rectangle = RectangleFromPRectangle(PRectangle(
				std::round(rc.left), rc.top, std::round(rc.right), rc.bottom));
			pRenderTarget->FillRectangle(&rectangle, pBrushLinear.Get());
		}
	}
}

void SurfaceD2D::DrawRGBAImage(PRectangle rc, int width, int height, const unsigned char *pixelsImage) {
	if (pRenderTarget) {
		if (rc.Width() > width)
			rc.left += std::floor((rc.Width() - width) / 2);
		rc.right = rc.left + width;
		if (rc.Height() > height)
			rc.top += std::floor((rc.Height() - height) / 2);
		rc.bottom = rc.top + height;

		std::vector<unsigned char> image(RGBAImage::bytesPerPixel * height * width);
		RGBAImage::BGRAFromRGBA(image.data(), pixelsImage, static_cast<ptrdiff_t>(height) * width);

		ComPtr<ID2D1Bitmap> bitmap;
		const D2D1_SIZE_U size = D2D1::SizeU(width, height);
		constexpr D2D1_BITMAP_PROPERTIES props = { {DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_PREMULTIPLIED}, 72.0, 72.0 };
		const HRESULT hr = pRenderTarget->CreateBitmap(size, image.data(),
			width * 4, &props, bitmap.GetAddressOf());
		if (SUCCEEDED(hr)) {
			const D2D1_RECT_F rcDestination = RectangleFromPRectangleEx(rc);
			pRenderTarget->DrawBitmap(bitmap.Get(), rcDestination);
		}
	}
}

void SurfaceD2D::Ellipse(PRectangle rc, FillStroke fillStroke) {
	if (!pRenderTarget)
		return;
	const D2D1_POINT_2F centre = DPointFromPoint(rc.Centre());

	const FLOAT radiusFill = static_cast<FLOAT>((rc.Width() / 2.0f) - fillStroke.stroke.width);
	const D2D1_ELLIPSE ellipseFill = { centre, radiusFill, radiusFill };

	D2DPenColourAlpha(fillStroke.fill.colour);
	pRenderTarget->FillEllipse(ellipseFill, pBrush.Get());

	const FLOAT radiusOutline = static_cast<FLOAT>((rc.Width() / 2.0f) - (fillStroke.stroke.width / 2.0f));
	const D2D1_ELLIPSE ellipseOutline = { centre, radiusOutline, radiusOutline };

	D2DPenColourAlpha(fillStroke.stroke.colour);
	pRenderTarget->DrawEllipse(ellipseOutline, pBrush.Get(), fillStroke.stroke.WidthF());
}

void SurfaceD2D::Stadium(PRectangle rc, FillStroke fillStroke, Ends ends) {
	if (!pRenderTarget)
		return;
	if (rc.Width() < rc.Height()) {
		// Can't draw nice ends so just draw a rectangle
		RectangleDraw(rc, fillStroke);
		return;
	}
	const FLOAT radius = static_cast<FLOAT>(rc.Height() / 2.0);
	const FLOAT radiusFill = radius - fillStroke.stroke.WidthF();
	const FLOAT halfStroke = fillStroke.stroke.WidthF() / 2.0f;
	if (ends == Surface::Ends::semiCircles) {
		const D2D1_RECT_F rect = RectangleFromPRectangle(rc);
		const D2D1_ROUNDED_RECT roundedRectFill = { RectangleInset(rect, fillStroke.stroke.WidthF()),
			radiusFill, radiusFill };
		D2DPenColourAlpha(fillStroke.fill.colour);
		pRenderTarget->FillRoundedRectangle(roundedRectFill, pBrush.Get());

		const D2D1_ROUNDED_RECT roundedRect = { RectangleInset(rect, halfStroke),
			radius, radius };
		D2DPenColourAlpha(fillStroke.stroke.colour);
		pRenderTarget->DrawRoundedRectangle(roundedRect, pBrush.Get(), fillStroke.stroke.WidthF());
	} else {
		const Ends leftSide = static_cast<Ends>(static_cast<int>(ends) & 0xf);
		const Ends rightSide = static_cast<Ends>(static_cast<int>(ends) & 0xf0);
		PRectangle rcInner = rc;
		rcInner.left += radius;
		rcInner.right -= radius;
		const Geometry pathGeometry = GeometryCreate();
		if (!pathGeometry)
			return;
		if (const GeometrySink pSink = GeometrySinkCreate(pathGeometry.Get())) {
			switch (leftSide) {
			case Ends::leftFlat:
				pSink->BeginFigure(DPointFromPoint(Point(rc.left + halfStroke, rc.top + halfStroke)), D2D1_FIGURE_BEGIN_FILLED);
				pSink->AddLine(DPointFromPoint(Point(rc.left + halfStroke, rc.bottom - halfStroke)));
				break;
			case Ends::leftAngle:
				pSink->BeginFigure(DPointFromPoint(Point(rcInner.left + halfStroke, rc.top + halfStroke)), D2D1_FIGURE_BEGIN_FILLED);
				pSink->AddLine(DPointFromPoint(Point(rc.left + halfStroke, rc.Centre().y)));
				pSink->AddLine(DPointFromPoint(Point(rcInner.left + halfStroke, rc.bottom - halfStroke)));
				break;
			case Ends::semiCircles:
			default: {
				pSink->BeginFigure(DPointFromPoint(Point(rcInner.left + halfStroke, rc.top + halfStroke)), D2D1_FIGURE_BEGIN_FILLED);
				D2D1_ARC_SEGMENT segment{};
				segment.point = DPointFromPoint(Point(rcInner.left + halfStroke, rc.bottom - halfStroke));
				segment.size = D2D1::SizeF(radiusFill, radiusFill);
				segment.rotationAngle = 0.0f;
				segment.sweepDirection = D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
				segment.arcSize = D2D1_ARC_SIZE_SMALL;
				pSink->AddArc(segment);
			}
			break;
			}

			switch (rightSide) {
			case Ends::rightFlat:
				pSink->AddLine(DPointFromPoint(Point(rc.right - halfStroke, rc.bottom - halfStroke)));
				pSink->AddLine(DPointFromPoint(Point(rc.right - halfStroke, rc.top + halfStroke)));
				break;
			case Ends::rightAngle:
				pSink->AddLine(DPointFromPoint(Point(rcInner.right - halfStroke, rc.bottom - halfStroke)));
				pSink->AddLine(DPointFromPoint(Point(rc.right - halfStroke, rc.Centre().y)));
				pSink->AddLine(DPointFromPoint(Point(rcInner.right - halfStroke, rc.top + halfStroke)));
				break;
			case Ends::semiCircles:
			default: {
				pSink->AddLine(DPointFromPoint(Point(rcInner.right - halfStroke, rc.bottom - halfStroke)));
				D2D1_ARC_SEGMENT segment{};
				segment.point = DPointFromPoint(Point(rcInner.right - halfStroke, rc.top + halfStroke));
				segment.size = D2D1::SizeF(radiusFill, radiusFill);
				segment.rotationAngle = 0.0f;
				segment.sweepDirection = D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
				segment.arcSize = D2D1_ARC_SIZE_SMALL;
				pSink->AddArc(segment);
			}
			break;
			}

			pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			pSink->Close();
		}

		D2DPenColourAlpha(fillStroke.fill.colour);
		pRenderTarget->FillGeometry(pathGeometry.Get(), pBrush.Get());
		D2DPenColourAlpha(fillStroke.stroke.colour);
		pRenderTarget->DrawGeometry(pathGeometry.Get(), pBrush.Get(), fillStroke.stroke.WidthF());
	}
}

void SurfaceD2D::Copy(PRectangle rc, Point from, Surface &surfaceSource) {
	SurfaceD2D &surfOther = down_cast<SurfaceD2D &>(surfaceSource);
	ComPtr<ID2D1Bitmap> pBitmap;
	const HRESULT hr = surfOther.GetBitmap(pBitmap.GetAddressOf());
	if (SUCCEEDED(hr) && pBitmap) {
		const D2D1_RECT_F rcDestination = RectangleFromPRectangle(rc);
		const D2D1_RECT_F rcSource = RectangleFromPRectangle(PRectangle(
			from.x, from.y, from.x + rc.Width(), from.y + rc.Height()));
		pRenderTarget->DrawBitmap(pBitmap.Get(), rcDestination, 1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, rcSource);
	}
}

class BlobInline final : public IDWriteInlineObject {
	XYPOSITION width;

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, PVOID *ppv) noexcept override;
	STDMETHODIMP_(ULONG)AddRef() noexcept override;
	STDMETHODIMP_(ULONG)Release() noexcept override;

	// IDWriteInlineObject
	STDMETHODIMP Draw(
		void *clientDrawingContext,
		IDWriteTextRenderer *renderer,
		FLOAT originX,
		FLOAT originY,
		BOOL isSideways,
		BOOL isRightToLeft,
		IUnknown *clientDrawingEffect
	) noexcept override;
	STDMETHODIMP GetMetrics(DWRITE_INLINE_OBJECT_METRICS *metrics) noexcept override;
	STDMETHODIMP GetOverhangMetrics(DWRITE_OVERHANG_METRICS *overhangs) noexcept override;
	STDMETHODIMP GetBreakConditions(DWRITE_BREAK_CONDITION *breakConditionBefore, DWRITE_BREAK_CONDITION *breakConditionAfter) noexcept override;
public:
	explicit BlobInline(XYPOSITION width_ = 0.0f) noexcept : width(width_) {}
};

/// Implement IUnknown
STDMETHODIMP BlobInline::QueryInterface(REFIID riid, PVOID *ppv) noexcept {
	if (!ppv) {
		return E_POINTER;
	}
	// Never called so not checked.
	*ppv = nullptr;
	if (riid == IID_IUnknown)
		*ppv = this;
	if (riid == __uuidof(IDWriteInlineObject))
		*ppv = this;
	if (!*ppv)
		return E_NOINTERFACE;
	return S_OK;
}

STDMETHODIMP_(ULONG) BlobInline::AddRef() noexcept {
	// Lifetime tied to Platform methods so ignore any reference operations.
	return 1;
}

STDMETHODIMP_(ULONG) BlobInline::Release() noexcept {
	// Lifetime tied to Platform methods so ignore any reference operations.
	return 1;
}

/// Implement IDWriteInlineObject
STDMETHODIMP BlobInline::Draw(
	void*,
	IDWriteTextRenderer*,
	FLOAT,
	FLOAT,
	BOOL,
	BOOL,
	IUnknown*) noexcept {
	// Since not performing drawing, not necessary to implement
	// Could be implemented by calling back into platform-independent code.
	// This would allow more of the drawing to be mediated through DirectWrite.
	return S_OK;
}

STDMETHODIMP BlobInline::GetMetrics(DWRITE_INLINE_OBJECT_METRICS *metrics) noexcept {
	if (!metrics) {
		return E_POINTER;
	}
	metrics->width = static_cast<FLOAT>(width);
	metrics->height = 2;
	metrics->baseline = 1;
	metrics->supportsSideways = FALSE;
	return S_OK;
}

STDMETHODIMP BlobInline::GetOverhangMetrics(DWRITE_OVERHANG_METRICS *overhangs) noexcept {
	if (!overhangs) {
		return E_POINTER;
	}
	overhangs->left = 0;
	overhangs->top = 0;
	overhangs->right = 0;
	overhangs->bottom = 0;
	return S_OK;
}

STDMETHODIMP BlobInline::GetBreakConditions(DWRITE_BREAK_CONDITION *breakConditionBefore, DWRITE_BREAK_CONDITION *breakConditionAfter) noexcept {
	if (!breakConditionBefore || !breakConditionAfter) {
		return E_POINTER;
	}
	// Since not performing 2D layout, not necessary to implement
	*breakConditionBefore = DWRITE_BREAK_CONDITION_NEUTRAL;
	*breakConditionAfter = DWRITE_BREAK_CONDITION_NEUTRAL;
	return S_OK;
}

class ScreenLineLayout final : public IScreenLineLayout {
	std::string text;
	std::wstring buffer;
	std::vector<BlobInline> blobs;
	// textLayout depends on blobs so must be declared after blobs so it is destroyed before blobs.
	TextLayout textLayout;
	static void FillTextLayoutFormats(const IScreenLine *screenLine, IDWriteTextLayout *textLayout, std::vector<BlobInline> &blobs);
	static std::wstring ReplaceRepresentation(std::string_view text);
	static UINT32 GetPositionInLayout(std::string_view text, size_t position) noexcept;
public:
	explicit ScreenLineLayout(const IScreenLine *screenLine);
	// Deleted so ScreenLineLayout objects can not be copied
	ScreenLineLayout(const ScreenLineLayout &) = delete;
	ScreenLineLayout(ScreenLineLayout &&) = delete;
	ScreenLineLayout &operator=(const ScreenLineLayout &) = delete;
	ScreenLineLayout &operator=(ScreenLineLayout &&) = delete;
	~ScreenLineLayout() noexcept override = default;
	size_t PositionFromX(XYPOSITION xDistance, bool charPosition) noexcept override;
	XYPOSITION XFromPosition(size_t caretPosition) noexcept override;
	std::vector<Interval> FindRangeIntervals(size_t start, size_t end) override;
};

// Each char can have its own style, so we fill the textLayout with the textFormat of each char

void ScreenLineLayout::FillTextLayoutFormats(const IScreenLine *screenLine, IDWriteTextLayout *textLayout, std::vector<BlobInline> &blobs) {
	// Reserve enough entries up front so they are not moved and the pointers handed
	// to textLayout remain valid.
	const ptrdiff_t numRepresentations = screenLine->RepresentationCount();
	std::string_view text = screenLine->Text();
	const ptrdiff_t numTabs = std::count(std::begin(text), std::end(text), '\t');
	blobs.reserve(numRepresentations + numTabs);

	UINT32 layoutPosition = 0;

	for (size_t bytePosition = 0; bytePosition < screenLine->Length();) {
		const unsigned char uch = screenLine->Text()[bytePosition];
		const unsigned int byteCount = UTF8BytesOfLead(uch);
		const UINT32 codeUnits = UTF16LengthFromUTF8ByteCount(byteCount);
		const DWRITE_TEXT_RANGE textRange = { layoutPosition, codeUnits };

		XYPOSITION representationWidth = screenLine->RepresentationWidth(bytePosition);
		if ((representationWidth == 0.0f) && (screenLine->Text()[bytePosition] == '\t')) {
			D2D1_POINT_2F realPt {};
			DWRITE_HIT_TEST_METRICS realCaretMetrics {};
			textLayout->HitTestTextPosition(
				layoutPosition,
				false, // trailing if false, else leading edge
				&realPt.x,
				&realPt.y,
				&realCaretMetrics
			);

			const XYPOSITION nextTab = screenLine->TabPositionAfter(realPt.x);
			representationWidth = nextTab - realPt.x;
		}
		if (representationWidth > 0.0f) {
			blobs.emplace_back(representationWidth);
			textLayout->SetInlineObject(&blobs.back(), textRange);
		};

		const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(screenLine->FontOfPosition(bytePosition));

		const UINT32 fontFamilyNameSize = pfm->pTextFormat->GetFontFamilyNameLength();
		std::wstring fontFamilyName(fontFamilyNameSize + 1, L'\0');

		const HRESULT hrFamily = pfm->pTextFormat->GetFontFamilyName(fontFamilyName.data(), fontFamilyNameSize + 1);
		if (SUCCEEDED(hrFamily)) {
			textLayout->SetFontFamilyName(fontFamilyName.c_str(), textRange);
		}

		textLayout->SetFontSize(pfm->pTextFormat->GetFontSize(), textRange);
		textLayout->SetFontWeight(pfm->pTextFormat->GetFontWeight(), textRange);
		textLayout->SetFontStyle(pfm->pTextFormat->GetFontStyle(), textRange);

		const UINT32 localeNameSize = pfm->pTextFormat->GetLocaleNameLength();
		std::wstring localeName(localeNameSize + 1, L'\0');

		const HRESULT hrLocale = pfm->pTextFormat->GetLocaleName(localeName.data(), localeNameSize);
		if (SUCCEEDED(hrLocale)) {
			textLayout->SetLocaleName(localeName.c_str(), textRange);
		}

		textLayout->SetFontStretch(pfm->pTextFormat->GetFontStretch(), textRange);

		IDWriteFontCollection *fontCollection = nullptr;
		if (SUCCEEDED(pfm->pTextFormat->GetFontCollection(&fontCollection))) {
			textLayout->SetFontCollection(fontCollection, textRange);
		}

		bytePosition += byteCount;
		layoutPosition += codeUnits;
	}

}

/* Convert to a wide character string and replace tabs with X to stop DirectWrite tab expansion */

std::wstring ScreenLineLayout::ReplaceRepresentation(std::string_view text) {
	const TextWide wideText(text, CpUtf8);
	std::wstring ws(wideText.data(), wideText.length());
	std::replace(ws.begin(), ws.end(), L'\t', L'X');
	return ws;
}

// Finds the position in the wide character version of the text.

UINT32 ScreenLineLayout::GetPositionInLayout(std::string_view text, size_t position) noexcept {
	const std::string_view textUptoPosition = text.substr(0, position);
	return static_cast<UINT32>(UTF16Length(textUptoPosition));
}

ScreenLineLayout::ScreenLineLayout(const IScreenLine *screenLine) {
	// If the text is empty, then no need to go through this function
	if (!screenLine || !screenLine->Length()) {
		return;
	}

	text = screenLine->Text();

	// Get textFormat
	const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(screenLine->FontOfPosition(0));
	if (!pfm->pTextFormat) {
		return;
	}

	// Convert the string to wstring and replace the original control characters with their representative chars.
	buffer = ReplaceRepresentation(screenLine->Text());

	// Create a text layout
	textLayout = LayoutCreate(
		buffer,
		pfm->pTextFormat.Get(),
		static_cast<FLOAT>(screenLine->Width()),
		static_cast<FLOAT>(screenLine->Height()));
	if (!textLayout) {
		return;
	}

	// Fill the textLayout chars with their own formats
	FillTextLayoutFormats(screenLine, textLayout.Get(), blobs);
}

// Get the position from the provided x

size_t ScreenLineLayout::PositionFromX(XYPOSITION xDistance, bool charPosition) noexcept {
	if (!textLayout) {
		return 0;
	}

	// Returns the text position corresponding to the mouse (x, y).
	// If hitting the trailing side of a cluster, return the
	// leading edge of the following text position.

	BOOL isTrailingHit = FALSE;
	BOOL isInside = FALSE;
	DWRITE_HIT_TEST_METRICS caretMetrics {};

	textLayout->HitTestPoint(
		static_cast<FLOAT>(xDistance),
		0.0f,
		&isTrailingHit,
		&isInside,
		&caretMetrics
	);

	DWRITE_HIT_TEST_METRICS hitTestMetrics {};
	if (isTrailingHit) {
		FLOAT caretX = 0.0f;
		FLOAT caretY = 0.0f;

		// Uses hit-testing to align the current caret position to a whole cluster,
		// rather than residing in the middle of a base character + diacritic,
		// surrogate pair, or character + UVS.

		// Align the caret to the nearest whole cluster.
		textLayout->HitTestTextPosition(
			caretMetrics.textPosition,
			false,
			&caretX,
			&caretY,
			&hitTestMetrics
		);
	}

	size_t pos;
	if (charPosition) {
		pos = isTrailingHit ? hitTestMetrics.textPosition : caretMetrics.textPosition;
	} else {
		pos = isTrailingHit ? hitTestMetrics.textPosition + hitTestMetrics.length : caretMetrics.textPosition;
	}

	// Get the character position in original string
	return UTF8PositionFromUTF16Position(text, pos);
}

// Finds the point of the caret position

XYPOSITION ScreenLineLayout::XFromPosition(size_t caretPosition) noexcept {
	if (!textLayout) {
		return 0.0;
	}
	// Convert byte positions to wchar_t positions
	const UINT32 position = GetPositionInLayout(text, caretPosition);

	// Translate text character offset to point (x, y).
	DWRITE_HIT_TEST_METRICS caretMetrics {};
	D2D1_POINT_2F pt {};

	textLayout->HitTestTextPosition(
		position,
		false, // trailing if false, else leading edge
		&pt.x,
		&pt.y,
		&caretMetrics
	);

	return pt.x;
}

// Find the selection range rectangles

std::vector<Interval> ScreenLineLayout::FindRangeIntervals(size_t start, size_t end) {
	std::vector<Interval> ret;

	if (!textLayout || (start == end)) {
		return ret;
	}

	// Convert byte positions to wchar_t positions
	const UINT32 startPos = GetPositionInLayout(text, start);
	const UINT32 endPos = GetPositionInLayout(text, end);

	// Find selection range length
	const UINT32 rangeLength = (endPos > startPos) ? (endPos - startPos) : (startPos - endPos);

	// Determine actual number of hit-test ranges
	UINT32 hitTestCount = 2;	// Simple selection often produces just 2 hits

	// First try with 2 elements and if more needed, allocate.
	std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(hitTestCount);
	textLayout->HitTestTextRange(
		startPos,
		rangeLength,
		0, // x
		0, // y
		hitTestMetrics.data(),
		hitTestCount,
		&hitTestCount
	);

	if (hitTestCount == 0) {
		return ret;
	}

	if (hitTestMetrics.size() < hitTestCount) {
		// Allocate enough room to return all hit-test metrics.
		hitTestMetrics.resize(hitTestCount);
		textLayout->HitTestTextRange(
			startPos,
			rangeLength,
			0, // x
			0, // y
			hitTestMetrics.data(),
			hitTestCount,
			&hitTestCount
		);
	}

	// Get the selection ranges behind the text.
	for (UINT32 i = 0; i < hitTestCount; ++i) {
		// Store selection rectangle
		const DWRITE_HIT_TEST_METRICS &htm = hitTestMetrics[i];
		const Interval selectionInterval { htm.left, htm.left + htm.width };
		ret.push_back(selectionInterval);
	}

	return ret;
}

std::unique_ptr<IScreenLineLayout> SurfaceD2D::Layout(const IScreenLine *screenLine) {
	return std::make_unique<ScreenLineLayout>(screenLine);
}

void SurfaceD2D::DrawTextCommon(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, int codePageOverride, UINT fuOptions) {
	const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(font_);
	if (pfm->pTextFormat) {
		// Use Unicode calls
		const int codePageDraw = codePageOverride ? codePageOverride : mode.codePage;
		const TextWide tbuf(text, codePageDraw);

		SetFontQuality(pfm->extraFontFlag);
		if (fuOptions & ETO_CLIPPED) {
			const D2D1_RECT_F rcClip = RectangleFromPRectangle(rc);
			pRenderTarget->PushAxisAlignedClip(rcClip, D2D1_ANTIALIAS_MODE_ALIASED);
		}
		//pfm->pTextFormat->SetReadingDirection(mode.bidiR2L ? DWRITE_READING_DIRECTION_RIGHT_TO_LEFT : DWRITE_READING_DIRECTION_LEFT_TO_RIGHT);

		// Explicitly creating a text layout appears a little faster
		const TextLayout pTextLayout = LayoutCreate(
			tbuf.AsView(),
			pfm->pTextFormat.Get(),
			static_cast<FLOAT>(rc.Width()),
			static_cast<FLOAT>(rc.Height()));
		if (pTextLayout) {
			const D2D1_POINT_2F origin = DPointFromPoint(Point(rc.left, ybase - pfm->yAscent));
			//DWRITE_TEXT_METRICS textMetrics{};
			//pTextLayout->GetMetrics(&textMetrics);
			//const FLOAT width = textMetrics.widthIncludingTrailingWhitespace + 24.0f;
			//const FLOAT width = static_cast<FLOAT>(rc.Width());
			//D2D1::Matrix3x2F invertX = D2D1::Matrix3x2F(-1, 0, 0, 1, 0, 0);
			//D2D1::Matrix3x2F moveX = D2D1::Matrix3x2F::Translation(width, 0);
			//pRenderTarget->SetTransform(invertX * moveX);
			//pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(180.0f, DPointFromPoint(rc.Centre())));
			//pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(180.0f, origin));
			pRenderTarget->DrawTextLayout(origin, pTextLayout.Get(), pBrush.Get(), d2dDrawTextOptions);
			//pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		}

		if (fuOptions & ETO_CLIPPED) {
			pRenderTarget->PopAxisAlignedClip();
		}
	}
}

void SurfaceD2D::DrawTextNoClip(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	if (pRenderTarget) {
		FillRectangleAligned(rc, back);
		D2DPenColourAlpha(fore);
		DrawTextCommon(rc, font_, ybase, text, 0, ETO_OPAQUE);
	}
}

void SurfaceD2D::DrawTextClipped(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	if (pRenderTarget) {
		FillRectangleAligned(rc, back);
		D2DPenColourAlpha(fore);
		DrawTextCommon(rc, font_, ybase, text, 0, ETO_OPAQUE | ETO_CLIPPED);
	}
}

void SurfaceD2D::DrawTextTransparent(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore) {
	// Avoid drawing spaces in transparent mode
	for (const char ch : text) {
		if (ch != ' ') {
			if (pRenderTarget) {
				D2DPenColourAlpha(fore);
				DrawTextCommon(rc, font_, ybase, text, 0, 0);
			}
			return;
		}
	}
}

class TextWideD2D {
	XYPOSITION *positions = nullptr;
	wchar_t * buffer;
	UINT len;	// Using UINT instead of size_t as most Win32 APIs take UINT.
	// reuse bufferStandard for both text and position, since only one is active
	XYPOSITION bufferStandard[stackBufferLength];
public:
	TextWideD2D(std::string_view text, int codePage): buffer {reinterpret_cast<wchar_t *>(bufferStandard)} {
		if (text.length() > stackBufferLength*sizeof(XYPOSITION)/sizeof(wchar_t)) {
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
	XYPOSITION *position() noexcept {
		return positions;
	}
	void allocate() {
		positions = bufferStandard;
		if (len > stackBufferLength) {
			positions = new XYPOSITION[len];
		}
		memset(positions, 0, len*sizeof(XYPOSITION));
	}
	[[nodiscard]] std::wstring_view AsView() const noexcept {
		return std::wstring_view{buffer, len};
	}

	// Deleted so TextWideD2D objects can not be copied.
	TextWideD2D(const TextWideD2D &) = delete;
	TextWideD2D(TextWide &&) = delete;
	TextWideD2D &operator=(const TextWideD2D &) = delete;
	TextWideD2D &operator=(TextWideD2D &&) = delete;

	~TextWideD2D() noexcept {
		if (buffer != static_cast<void *>(bufferStandard)) {
			delete[]buffer;
		}
		if (positions != bufferStandard) {
			delete[]positions;
		}
	}
};

HRESULT MeasurePositions(const Font *font_, TextWideD2D &tbuf) {
	const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(font_);
	if (!pfm->pTextFormat) {
		// Unexpected failure like no access to DirectWrite so give up.
		return E_FAIL;
	}

	// Create a layout
	const TextLayout pTextLayout = LayoutCreate(tbuf.AsView(), pfm->pTextFormat.Get());
	if (!pTextLayout) {
		return E_FAIL;
	}

	tbuf.allocate();
	VarBuffer<DWRITE_CLUSTER_METRICS, stackBufferLength> clusterMetrics(tbuf.length());
	UINT32 count = 0;
	const HRESULT hrGetCluster = pTextLayout->GetClusterMetrics(clusterMetrics.data(), tbuf.length(), &count);
	if (!SUCCEEDED(hrGetCluster)) {
		return hrGetCluster;
	}
	// A cluster may be more than one WCHAR, such as for "ffi" which is a ligature in the Candara font
	XYPOSITION position = 0.0;
	UINT ti = 0;
	XYPOSITION * const poses = tbuf.position();
	for (UINT32 ci = 0; ci < count; ci++) {
		const int length = clusterMetrics[ci].length;
		const XYPOSITION width = clusterMetrics[ci].width;
		for (int inCluster = 0; inCluster < length; inCluster++) {
			const XYPOSITION proportion = static_cast<XYPOSITION>(inCluster + 1) / length;
			poses[ti++] = position + width * proportion;
		}
		position += width;
	}
	PLATFORM_ASSERT(ti == tbuf.length());
	return S_OK;
}

void SurfaceD2D::MeasureWidths(const Font *font_, std::string_view text, XYPOSITION *positions) {
	TextWideD2D tbuf(text, mode.codePage);
	if (FAILED(MeasurePositions(font_, tbuf))) {
		return;
	}
	const XYPOSITION * const poses = tbuf.position();
	if (mode.codePage == CpUtf8) {
		// Map the widths given for UTF-16 characters back onto the UTF-8 input string
		size_t i = 0;
		for (UINT ui = 0; ui < tbuf.length(); ui++) {
			const unsigned char uch = text[i];
			const unsigned int byteCount = UTF8BytesOfLead(uch);
			if (byteCount == 4) {	// Non-BMP
				ui++;
			}
			for (unsigned int bytePos = 0; (bytePos < byteCount) && (i < text.length()) && (ui < tbuf.length()); bytePos++) {
				positions[i++] = poses[ui];
			}
		}
		const XYPOSITION lastPos = (i > 0) ? positions[i - 1] : 0.0;
		while (i < text.length()) {
			positions[i++] = lastPos;
		}
	} else {
		if (mode.codePage) {
			// May be one or two bytes per position
			UINT ui = 0;
			for (size_t i = 0; i < text.length() && ui < tbuf.length();) {
				positions[i] = poses[ui];
				const unsigned char uch = text[i++];
				if (!UTF8IsAscii(uch) && DBCSIsLeadByte(mode.codePage, uch)) {
					positions[i] = poses[ui];
					i += 1;
				}
				ui++;
			}
		} else {
			// One char per position
			PLATFORM_ASSERT(text.length() == tbuf.length());
			for (UINT kk = 0; kk < tbuf.length(); kk++) {
				positions[kk] = poses[kk];
			}
		}
	}
}

XYPOSITION SurfaceD2D::WidthText(const Font *font_, std::string_view text) {
	FLOAT width = 1.0;
	const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(font_);
	if (pfm->pTextFormat) {
		const TextWide tbuf(text, mode.codePage);
		// Create a layout
		if (const TextLayout pTextLayout = LayoutCreate(tbuf.AsView(), pfm->pTextFormat.Get())) {
			DWRITE_TEXT_METRICS textMetrics;
			if (SUCCEEDED(pTextLayout->GetMetrics(&textMetrics)))
				width = textMetrics.widthIncludingTrailingWhitespace;
		}
	}
	return width;
}

void SurfaceD2D::DrawTextNoClipUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	if (pRenderTarget) {
		FillRectangleAligned(rc, back);
		D2DPenColourAlpha(fore);
		DrawTextCommon(rc, font_, ybase, text, CpUtf8, ETO_OPAQUE);
	}
}

void SurfaceD2D::DrawTextClippedUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	if (pRenderTarget) {
		FillRectangleAligned(rc, back);
		D2DPenColourAlpha(fore);
		DrawTextCommon(rc, font_, ybase, text, CpUtf8, ETO_OPAQUE | ETO_CLIPPED);
	}
}

void SurfaceD2D::DrawTextTransparentUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore) {
	// Avoid drawing spaces in transparent mode
	for (const char ch : text) {
		if (ch != ' ') {
			if (pRenderTarget) {
				D2DPenColourAlpha(fore);
				DrawTextCommon(rc, font_, ybase, text, CpUtf8, 0);
			}
			return;
		}
	}
}

void SurfaceD2D::MeasureWidthsUTF8(const Font *font_, std::string_view text, XYPOSITION *positions) {
	TextWideD2D tbuf(text, CpUtf8);
	if (FAILED(MeasurePositions(font_, tbuf))) {
		return;
	}
	// Map the widths given for UTF-16 characters back onto the UTF-8 input string
	size_t i = 0;
	const XYPOSITION * const poses = tbuf.position();
	for (UINT ui = 0; ui < tbuf.length(); ui++) {
		const unsigned char uch = text[i];
		const unsigned int byteCount = UTF8BytesOfLead(uch);
		if (byteCount == 4) {	// Non-BMP
			ui++;
			PLATFORM_ASSERT(ui < tbuf.length());
		}
		for (unsigned int bytePos = 0; (bytePos < byteCount) && (i < text.length()) && (ui < tbuf.length()); bytePos++) {
			positions[i++] = poses[ui];
		}
	}
	const XYPOSITION lastPos = (i > 0) ? positions[i - 1] : 0.0;
	while (i < text.length()) {
		positions[i++] = lastPos;
	}
}

XYPOSITION SurfaceD2D::WidthTextUTF8(const Font * font_, std::string_view text) {
	FLOAT width = 1.0;
	const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(font_);
	if (pfm->pTextFormat) {
		const TextWide tbuf(text, CpUtf8);
		// Create a layout
		if (const TextLayout pTextLayout = LayoutCreate(tbuf.AsView(), pfm->pTextFormat.Get())) {
			DWRITE_TEXT_METRICS textMetrics;
			if (SUCCEEDED(pTextLayout->GetMetrics(&textMetrics)))
				width = textMetrics.widthIncludingTrailingWhitespace;
		}
	}
	return width;
}

XYPOSITION SurfaceD2D::Ascent(const Font *font_) noexcept {
	const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(font_);
	return std::ceil(pfm->yAscent);
}

XYPOSITION SurfaceD2D::Descent(const Font *font_) noexcept {
	const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(font_);
	return std::ceil(pfm->yDescent);
}

XYPOSITION SurfaceD2D::InternalLeading(const Font *font_) noexcept {
	const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(font_);
	return std::floor(pfm->yInternalLeading);
}

XYPOSITION SurfaceD2D::Height(const Font *font_) noexcept {
	const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(font_);
	return std::ceil(pfm->yAscent) + std::ceil(pfm->yDescent);
}

XYPOSITION SurfaceD2D::AverageCharWidth(const Font *font_) {
	FLOAT width = 1.0;
	const FontDirectWrite *pfm = down_cast<const FontDirectWrite *>(font_);
	if (pfm->pTextFormat) {
		// Create a layout
		constexpr std::wstring_view wsvAllAlpha = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		if (const TextLayout pTextLayout = LayoutCreate(wsvAllAlpha, pfm->pTextFormat.Get())) {
			DWRITE_TEXT_METRICS textMetrics;
			if (SUCCEEDED(pTextLayout->GetMetrics(&textMetrics)))
				width = textMetrics.width / wsvAllAlpha.length();
		}
	}
	return width;
}

void SurfaceD2D::SetClip(PRectangle rc) noexcept {
	if (pRenderTarget) {
		const D2D1_RECT_F rcClip = RectangleFromPRectangle(rc);
		pRenderTarget->PushAxisAlignedClip(rcClip, D2D1_ANTIALIAS_MODE_ALIASED);
		clipsActive++;
	}
}

void SurfaceD2D::PopClip() noexcept {
	if (pRenderTarget) {
		PLATFORM_ASSERT(clipsActive > 0);
		pRenderTarget->PopAxisAlignedClip();
		clipsActive--;
	}
}

void SurfaceD2D::FlushCachedState() noexcept {
}

void SurfaceD2D::FlushDrawing() noexcept {
	if (pRenderTarget) {
		pRenderTarget->Flush();
	}
}

}

namespace Scintilla::Internal {

#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
IDWriteFactory1 *pIDWriteFactory = nullptr;
ID2D1Factory1 *pD2DFactory = nullptr;
#else
IDWriteFactory *pIDWriteFactory = nullptr;
ID2D1Factory *pD2DFactory = nullptr;
#endif

#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
HRESULT CreateD3D(D3D11Device &device) noexcept {
	device = nullptr;
	if (!fnDCD) {
		return E_FAIL;
	}

	static constexpr D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// Create device.
	// Try for a hardware device but, if that fails, fall back to the Warp software rasterizer.
	ComPtr<ID3D11Device> upDevice;
	HRESULT hr = S_OK;
	constexpr D3D_DRIVER_TYPE typesToTry[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP };
	for (const D3D_DRIVER_TYPE type : typesToTry) {
		hr = fnDCD(nullptr,
			type,
			{},
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			upDevice.GetAddressOf(),
			nullptr,
			nullptr);
		if (SUCCEEDED(hr)) {
			// Convert from D3D11 to D3D11.1
			hr = upDevice.As(&device);
			if (FAILED(hr)) {
				// Platform::DebugPrintf("Failed to create D3D11.1 device 0x%lx\n", hr);
			}
			break;
		}
		// Platform::DebugPrintf("Failed to create D3D11 device 0x%lx\n", hr);
	}

	return hr;
}
#endif

bool LoadD2D() noexcept {
#if USE_STD_CALL_ONCE
	static std::once_flag once;
	try {
		std::call_once(once, LoadD2DOnce);
	} catch (...) {
		// ignore
	}
#elif USE_WIN32_INIT_ONCE
	static INIT_ONCE once = INIT_ONCE_STATIC_INIT;
	::InitOnceExecuteOnce(&once, LoadD2DOnce, nullptr, nullptr);
#else
	static LONG once = 0;
	if (::InterlockedCompareExchange(&once, 1, 0) == 0) {
		LoadD2DOnce();
	}
#endif
	return pIDWriteFactory && pD2DFactory;
}

void ReleaseD2D() noexcept {
	ReleaseUnknown(gdiInterop);
	ReleaseUnknown(pIDWriteFactory);
	ReleaseUnknown(pD2DFactory);
	ReleaseLibrary(hDLLDWrite);
	ReleaseLibrary(hDLLD2D);
#if _WIN32_WINNT >= _WIN32_WINNT_WIN7
	ReleaseLibrary(hDLLD3D);
#endif
}

HRESULT CreateDCRenderTarget(const D2D1_RENDER_TARGET_PROPERTIES *renderTargetProperties, DCRenderTarget &dcRT) noexcept {
	return pD2DFactory->CreateDCRenderTarget(renderTargetProperties, dcRT.ReleaseAndGetAddressOf());
}

BrushSolid BrushSolidCreate(ID2D1RenderTarget *pTarget, COLORREF colour) noexcept {
	BrushSolid brush;
	const D2D_COLOR_F col = ColorFromColourAlpha(ColourRGBA::FromRGB(colour));
	if (FAILED(pTarget->CreateSolidColorBrush(col, brush.GetAddressOf()))) {
		return {};
	}
	return brush;
}

Geometry GeometryCreate() noexcept {
	Geometry geometry;
	if (FAILED(pD2DFactory->CreatePathGeometry(geometry.GetAddressOf()))) {
		return {};
	}
	return geometry;
}

GeometrySink GeometrySinkCreate(ID2D1PathGeometry *geometry) noexcept {
	GeometrySink sink;
	if (FAILED(geometry->Open(sink.GetAddressOf()))) {
		return {};
	}
	return sink;
}

StrokeStyle StrokeStyleCreate(const D2D1_STROKE_STYLE_PROPERTIES &strokeStyleProperties) noexcept {
	StrokeStyle strokeStyle;
	const HRESULT hr = pD2DFactory->CreateStrokeStyle(
		strokeStyleProperties, nullptr, 0, strokeStyle.GetAddressOf());
	if (FAILED(hr)) {
		return {};
	}
	return strokeStyle;
}

TextLayout LayoutCreate(std::wstring_view wsv, IDWriteTextFormat *pTextFormat, FLOAT maxWidth, FLOAT maxHeight) noexcept {
	TextLayout layout;
	const HRESULT hr = pIDWriteFactory->CreateTextLayout(wsv.data(), static_cast<UINT32>(wsv.length()),
		pTextFormat, maxWidth, maxHeight, layout.GetAddressOf());
	if (FAILED(hr)) {
		return {};
	}
	return layout;
}

std::shared_ptr<Font> Font::Allocate(const FontParameters &fp) {
	return std::make_shared<FontWin>(fp);
}

std::unique_ptr<Surface> Surface::Allocate(Technology technology) {
	if (technology != Technology::Default) {
		return std::make_unique<SurfaceD2D>();
	}
	return SurfaceGDI_Allocate();
}

}
