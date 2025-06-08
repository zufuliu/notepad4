// Scintilla source code edit control
/** @file SurfaceGDI.cxx
 ** Implementation of drawing to GDI on Windows.
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

using namespace Scintilla;
using namespace Scintilla::Internal;

namespace {

constexpr Supports SupportsGDI =
	Supports::PixelModification;

class SurfaceGDI final : public Surface {
	HDC hdc{};
	HPEN pen{};
	HPEN penOld{};
	HBRUSH brush{};
	HBRUSH brushOld{};
	HFONT fontOld{};
	HBITMAP bitmap{};
	HBITMAP bitmapOld{};

	SurfaceMode mode;
	bool hdcOwned = false;
	int logPixelsY = USER_DEFAULT_SCREEN_DPI;

	static constexpr int maxWidthMeasure = INT_MAX;
	// There appears to be a 16 bit string length limit in GDI on NT.
	static constexpr int maxLenText = 65535;

	void PenColour(ColourRGBA fore, XYPOSITION widthStroke) noexcept;
	void BrushColour(ColourRGBA back) noexcept;
	void SetFont(const Font *font_) noexcept;
	void Clear() noexcept;

public:
	SurfaceGDI() noexcept = default;
	SurfaceGDI(HDC hdcCompatible, int width, int height, SurfaceMode mode_, int logPixelsY_) noexcept;
	// Deleted so SurfaceGDI objects can not be copied.
	SurfaceGDI(const SurfaceGDI &) = delete;
	SurfaceGDI(SurfaceGDI &&) = delete;
	SurfaceGDI &operator=(const SurfaceGDI &) = delete;
	SurfaceGDI &operator=(SurfaceGDI &&) = delete;

	~SurfaceGDI() noexcept override;

	void Init(WindowID wid) noexcept override;
	void Init(SurfaceID sid, WindowID wid, bool printing = false) noexcept override;
	std::unique_ptr<Surface> AllocatePixMap(int width, int height) override;

	void SetMode(SurfaceMode mode_) noexcept override;
	void SetRenderingParams(void *defaultRenderingParams, void *customRenderingParams) noexcept override;

	void Release() noexcept override;
	bool SupportsFeature(Supports feature) const noexcept override;
	bool Initialised() const noexcept override;
	int LogPixelsY() const noexcept override;
	int PixelDivisions() const noexcept override;
	int DeviceHeightFont(int points) const noexcept override;
	void SCICALL LineDraw(Point start, Point end, Stroke stroke) noexcept override;
	void SCICALL PolyLine(const Point *pts, size_t npts, Stroke stroke) override;
	void SCICALL Polygon(const Point *pts, size_t npts, FillStroke fillStroke) override;
	void SCICALL RectangleDraw(PRectangle rc, FillStroke fillStroke) noexcept override;
	void SCICALL RectangleFrame(PRectangle rc, Stroke stroke) noexcept override;
	void SCICALL FillRectangle(PRectangle rc, Fill fill) noexcept override;
	void SCICALL FillRectangleAligned(PRectangle rc, Fill fill) noexcept override;
	void SCICALL FillRectangle(PRectangle rc, Surface &surfacePattern) noexcept override;
	void SCICALL RoundedRectangle(PRectangle rc, FillStroke fillStroke) noexcept override;
	void SCICALL AlphaRectangle(PRectangle rc, XYPOSITION cornerSize, FillStroke fillStroke) noexcept override;
	void SCICALL GradientRectangle(PRectangle rc, const std::vector<ColourStop> &stops, GradientOptions options) override;
	void SCICALL DrawRGBAImage(PRectangle rc, int width, int height, const unsigned char *pixelsImage) noexcept override;
	void SCICALL Ellipse(PRectangle rc, FillStroke fillStroke) noexcept override;
	void SCICALL Stadium(PRectangle rc, FillStroke fillStroke, Ends ends) noexcept override;
	void SCICALL Copy(PRectangle rc, Point from, Surface &surfaceSource) noexcept override;

	std::unique_ptr<IScreenLineLayout> Layout(const IScreenLine *screenLine) noexcept override;

	void SCICALL DrawTextCommon(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, UINT fuOptions);
	void SCICALL DrawTextNoClip(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void SCICALL DrawTextClipped(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void SCICALL DrawTextTransparent(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore) override;
	void SCICALL MeasureWidths(const Font *font_, std::string_view text, XYPOSITION *positions) override;
	XYPOSITION WidthText(const Font *font_, std::string_view text) override;

	void SCICALL DrawTextCommonUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, UINT fuOptions);
	void SCICALL DrawTextNoClipUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void SCICALL DrawTextClippedUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore, ColourRGBA back) override;
	void SCICALL DrawTextTransparentUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, ColourRGBA fore) override;
	void SCICALL MeasureWidthsUTF8(const Font *font_, std::string_view text, XYPOSITION *positions) override;
	XYPOSITION WidthTextUTF8(const Font *font_, std::string_view text) override;

	XYPOSITION Ascent(const Font *font_) noexcept override;
	XYPOSITION Descent(const Font *font_) noexcept override;
	XYPOSITION InternalLeading(const Font *font_) noexcept override;
	XYPOSITION Height(const Font *font_) noexcept override;
	XYPOSITION AverageCharWidth(const Font *font_) noexcept override;

	void SCICALL SetClip(PRectangle rc) noexcept override;
	void PopClip() noexcept override;
	void FlushCachedState() noexcept override;
	void FlushDrawing() noexcept override;
};

SurfaceGDI::SurfaceGDI(HDC hdcCompatible, int width, int height, SurfaceMode mode_, int logPixelsY_) noexcept {
	hdc = ::CreateCompatibleDC(hdcCompatible);
	bitmap = ::CreateCompatibleBitmap(hdcCompatible, width, height);
	bitmapOld = SelectBitmap(hdc, bitmap);
	::SetTextAlign(hdc, TA_BASELINE);
	mode = mode_;
	hdcOwned = true;
	logPixelsY = logPixelsY_;
}

SurfaceGDI::~SurfaceGDI() noexcept {
	Clear();
}

void SurfaceGDI::Clear() noexcept {
	if (penOld) {
		::SelectObject(hdc, penOld);
		::DeleteObject(pen);
		penOld = {};
	}
	pen = {};
	if (brushOld) {
		::SelectObject(hdc, brushOld);
		::DeleteObject(brush);
		brushOld = {};
	}
	brush = {};
	if (fontOld) {
		// Fonts are not deleted as they are owned by a Font object
		::SelectObject(hdc, fontOld);
		fontOld = {};
	}
	if (bitmapOld) {
		::SelectObject(hdc, bitmapOld);
		::DeleteObject(bitmap);
		bitmapOld = {};
	}
	bitmap = {};
	if (hdcOwned) {
		::DeleteDC(hdc);
		hdc = {};
		hdcOwned = false;
	}
}

void SurfaceGDI::Release() noexcept {
	Clear();
}

bool SurfaceGDI::SupportsFeature(Supports feature) const noexcept {
	return SupportsGDI == feature;
}

bool SurfaceGDI::Initialised() const noexcept {
	return hdc != nullptr;
}

void SurfaceGDI::Init(WindowID wid) noexcept {
	hdc = ::CreateCompatibleDC({});
	hdcOwned = true;
	::SetTextAlign(hdc, TA_BASELINE);
	logPixelsY = DpiForWindow(wid);
}

void SurfaceGDI::Init(SurfaceID sid, WindowID wid, bool printing) noexcept {
	hdc = static_cast<HDC>(sid);
	::SetTextAlign(hdc, TA_BASELINE);
	// Windows on screen are scaled but printers are not.
	//const bool printing = (::GetDeviceCaps(hdc, TECHNOLOGY) != DT_RASDISPLAY);
	logPixelsY = printing ? ::GetDeviceCaps(hdc, LOGPIXELSY) : DpiForWindow(wid);
}

std::unique_ptr<Surface> SurfaceGDI::AllocatePixMap(int width, int height) {
	return std::make_unique<SurfaceGDI>(hdc, width, height, mode, logPixelsY);
}

void SurfaceGDI::SetMode(SurfaceMode mode_) noexcept {
	mode = mode_;
}

void SurfaceGDI::SetRenderingParams([[maybe_unused]] void *defaultRenderingParams, [[maybe_unused]] void *customRenderingParams) noexcept {
}

void SurfaceGDI::PenColour(ColourRGBA fore, XYPOSITION widthStroke) noexcept {
	if (pen) {
		::SelectObject(hdc, penOld);
		::DeleteObject(pen);
		pen = {};
		penOld = {};
	}
	const DWORD penWidth = std::lround(widthStroke);
	const COLORREF penColour = fore.OpaqueRGB();
	if (penWidth > 1) {
		const LOGBRUSH brushParameters { BS_SOLID, penColour, 0 };
		pen = ::ExtCreatePen(PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_MITER,
			penWidth,
			&brushParameters,
			0,
			nullptr);
	} else {
		pen = ::CreatePen(PS_INSIDEFRAME, 1, penColour);
	}
	penOld = SelectPen(hdc, pen);
}

void SurfaceGDI::BrushColour(ColourRGBA back) noexcept {
	if (brush) {
		::SelectObject(hdc, brushOld);
		::DeleteObject(brush);
		brush = {};
		brushOld = {};
	}
	brush = ::CreateSolidBrush(back.OpaqueRGB());
	brushOld = SelectBrush(hdc, brush);
}

void SurfaceGDI::SetFont(const Font *font_) noexcept {
	const FontGDI *pfm = down_cast<const FontGDI *>(font_);
	PLATFORM_ASSERT(pfm);
	if (fontOld) {
		SelectFont(hdc, pfm->hfont);
	} else {
		fontOld = SelectFont(hdc, pfm->hfont);
	}
}

int SurfaceGDI::LogPixelsY() const noexcept {
	return logPixelsY;
}

int SurfaceGDI::PixelDivisions() const noexcept {
	// Win32 uses device pixels.
	return 1;
}

int SurfaceGDI::DeviceHeightFont(int points) const noexcept {
	return ::MulDiv(points, logPixelsY, pointsPerInch);
}

void SurfaceGDI::LineDraw(Point start, Point end, Stroke stroke) noexcept {
	PenColour(stroke.colour, stroke.width);
	::MoveToEx(hdc, static_cast<int>(start.x), static_cast<int>(start.y), nullptr);
	::LineTo(hdc, static_cast<int>(end.x), static_cast<int>(end.y));
}

void SurfaceGDI::PolyLine(const Point *pts, size_t npts, Stroke stroke) {
	PLATFORM_ASSERT(npts > 1);
	if (npts <= 1) {
		return;
	}
	PenColour(stroke.colour, stroke.width);
	std::vector<POINT> outline;
	std::transform(pts, pts + npts, std::back_inserter(outline), POINTFromPointEx);
	::Polyline(hdc, outline.data(), static_cast<int>(npts));
}

void SurfaceGDI::Polygon(const Point *pts, size_t npts, FillStroke fillStroke) {
	PenColour(fillStroke.stroke.colour.WithoutAlpha(), fillStroke.stroke.width);
	BrushColour(fillStroke.fill.colour.WithoutAlpha());
	std::vector<POINT> outline;
	std::transform(pts, pts + npts, std::back_inserter(outline), POINTFromPointEx);
	::Polygon(hdc, outline.data(), static_cast<int>(npts));
}

void SurfaceGDI::RectangleDraw(PRectangle rc, FillStroke fillStroke) noexcept {
	RectangleFrame(rc, fillStroke.stroke);
	FillRectangle(rc.Inset(fillStroke.stroke.width), fillStroke.fill.colour);
}

void SurfaceGDI::RectangleFrame(PRectangle rc, Stroke stroke) noexcept {
	BrushColour(stroke.colour);
	const RECT rcw = RectFromPRectangleEx(rc);
	::FrameRect(hdc, &rcw, brush);
}

void SurfaceGDI::FillRectangle(PRectangle rc, Fill fill) noexcept {
	if (fill.colour.IsOpaque()) {
		// Using ExtTextOut rather than a FillRect ensures that no dithering occurs.
		// There is no need to allocate a brush either.
		const RECT rcw = RectFromPRectangleEx(rc);
		::SetBkColor(hdc, fill.colour.OpaqueRGB());
		::ExtTextOut(hdc, rcw.left, rcw.top, ETO_OPAQUE, &rcw, TEXT(""), 0, nullptr);
	} else {
		AlphaRectangle(rc, 0, FillStroke(fill.colour));
	}
}

void SurfaceGDI::FillRectangleAligned(PRectangle rc, Fill fill) noexcept {
	FillRectangle(PixelAlign(rc, 1), fill);
}

void SurfaceGDI::FillRectangle(PRectangle rc, Surface &surfacePattern) noexcept {
	HBRUSH br{};
	if (const SurfaceGDI *psgdi = down_cast<SurfaceGDI *>(&surfacePattern); psgdi && psgdi->bitmap) {
		br = ::CreatePatternBrush(psgdi->bitmap);
	} else {	// Something is wrong so display in red
		br = ::CreateSolidBrush(RGB(0xff, 0, 0));
	}
	const RECT rcw = RectFromPRectangleEx(rc);
	::FillRect(hdc, &rcw, br);
	::DeleteObject(br);
}

void SurfaceGDI::RoundedRectangle(PRectangle rc, FillStroke fillStroke) noexcept {
	PenColour(fillStroke.stroke.colour, fillStroke.stroke.width);
	BrushColour(fillStroke.fill.colour);
	const RECT rcw = RectFromPRectangleEx(rc);
	constexpr int cornerSize = 8;
	::RoundRect(hdc,
		rcw.left + 1, rcw.top,
		rcw.right - 1, rcw.bottom,
		cornerSize, cornerSize);
}

#if NP2_USE_AVX2
inline DWORD RGBQuadMultiplied(ColourRGBA colour) noexcept {
	__m128i i16x4Color = rgba_to_bgra_epi16_sse4_si32(colour.AsInteger());
	const __m128i i16x4Alpha = _mm_shufflelo_epi16(i16x4Color, 0xff);
	i16x4Color = _mm_mullo_epi16(i16x4Color, i16x4Alpha);
	i16x4Color = mm_div_epu16_by_255(i16x4Color);
	i16x4Color = _mm_blend_epi16(i16x4Alpha, i16x4Color, 7);
	return pack_color_epi16_sse2_si32(i16x4Color);
}

#elif NP2_USE_SSE2
inline DWORD RGBQuadMultiplied(ColourRGBA colour) noexcept {
	const uint32_t rgba = bswap32(colour.AsInteger());
	__m128i i16x4Color = unpack_color_epi16_sse2_si32(rgba);
	const __m128i i16x4Alpha = _mm_shufflelo_epi16(i16x4Color, 0);
	i16x4Color = _mm_mullo_epi16(i16x4Color, i16x4Alpha);
	i16x4Color = mm_div_epu16_by_255(i16x4Color);

	const uint32_t color = bgr_from_abgr_epi16_sse2_si32(i16x4Color);
	return color | (rgba << 24);
}

#else
// make a GDI RGBQUAD as DWORD.
template <typename T>
constexpr DWORD RGBQuad(T alpha, T red, T green, T blue) noexcept {
	return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

template <typename T>
constexpr T AlphaScaled(T component, T alpha) noexcept {
	return static_cast<T>(component * alpha / 255);
}

template <typename T>
constexpr DWORD RGBQuadMultiplied(T alpha, T red, T green, T blue) noexcept {
	red = AlphaScaled(red, alpha);
	green = AlphaScaled(green, alpha);
	blue = AlphaScaled(blue, alpha);
	return RGBQuad(alpha, red, green, blue);
}

constexpr DWORD RGBQuadMultiplied(ColourRGBA colour) noexcept {
	return RGBQuadMultiplied(colour.GetAlpha(), colour.GetRed(), colour.GetGreen(), colour.GetBlue());
}
#endif

// DIBSection is bitmap with some drawing operations used by SurfaceGDI.
class DIBSection {
	GDIBitMap bm;
	DWORD *pixels = nullptr;
	const SIZE size;
public:
	DIBSection(HDC hdc, SIZE size_) noexcept;
	explicit operator bool() const noexcept {
		return bm && pixels;
	}
	[[nodiscard]] DWORD *Pixels() const noexcept {
		return pixels;
	}
	[[nodiscard]] unsigned char *Bytes() const noexcept {
		return reinterpret_cast<unsigned char *>(pixels);
	}
	[[nodiscard]] HDC DC() const noexcept {
		return bm.DC();
	}
	void SetPixel(LONG x, LONG y, DWORD value) noexcept {
		PLATFORM_ASSERT(x >= 0);
		PLATFORM_ASSERT(y >= 0);
		PLATFORM_ASSERT(x < size.cx);
		PLATFORM_ASSERT(y < size.cy);
		pixels[(y * size.cx) + x] = value;
	}
	void SetSymmetric(LONG x, LONG y, DWORD value) noexcept;
};

DIBSection::DIBSection(HDC hdc, SIZE size_) noexcept : size{size_} {
	// -size.y makes bitmap start from top
	bm.Create(hdc, size.cx, -size.cy, &pixels);
}

void DIBSection::SetSymmetric(LONG x, LONG y, DWORD value) noexcept {
	// Plot a point symmetrically to all 4 quadrants
	const LONG xSymmetric = size.cx - 1 - x;
	const LONG ySymmetric = size.cy - 1 - y;
	SetPixel(x, y, value);
	SetPixel(xSymmetric, y, value);
	SetPixel(x, ySymmetric, value);
	SetPixel(xSymmetric, ySymmetric, value);
}

#if NP2_USE_AVX2
inline DWORD Proportional(ColourRGBA a, ColourRGBA b, XYPOSITION t) noexcept {
	__m128i i32x4Fore = rgba_to_abgr_epi32_sse4_si32(a.AsInteger());
	const __m128i i32x4Back = rgba_to_abgr_epi32_sse4_si32(b.AsInteger());
	// a + t * (b - a)
	__m128 f32x4Fore = _mm_cvtepi32_ps(_mm_sub_epi32(i32x4Back, i32x4Fore));
	f32x4Fore = _mm_mul_ps(f32x4Fore, _mm_set1_ps(static_cast<float>(t)));
	f32x4Fore = _mm_add_ps(f32x4Fore, _mm_cvtepi32_ps(i32x4Fore));
	// component * alpha / 255
	const __m128 f32x4Alpha = _mm_broadcastss_ps(f32x4Fore);
	f32x4Fore = _mm_mul_ps(f32x4Fore, f32x4Alpha);
	f32x4Fore = _mm_div_ps(f32x4Fore, _mm_set1_ps(255.0f));
	f32x4Fore = mm_alignr_ps(f32x4Alpha, f32x4Fore, 1);

	i32x4Fore = _mm_cvttps_epi32(f32x4Fore);
	return pack_color_epi32_sse2_si32(i32x4Fore);
}

#elif NP2_USE_SSE2
inline DWORD Proportional(ColourRGBA a, ColourRGBA b, XYPOSITION t) noexcept {
	__m128i i32x4Fore = rgba_to_abgr_epi32_sse2_si32(a.AsInteger());
	const __m128i i32x4Back = rgba_to_abgr_epi32_sse2_si32(b.AsInteger());
	// a + t * (b - a)
	__m128 f32x4Fore = _mm_cvtepi32_ps(_mm_sub_epi32(i32x4Back, i32x4Fore));
	f32x4Fore = _mm_mul_ps(f32x4Fore, _mm_set1_ps(static_cast<float>(t)));
	f32x4Fore = _mm_add_ps(f32x4Fore, _mm_cvtepi32_ps(i32x4Fore));
	// component * alpha / 255
	const uint32_t alpha = _mm_cvttss_si32(f32x4Fore);
	const __m128 f32x4Alpha = _mm_shuffle_ps(f32x4Fore, f32x4Fore, 0);
	f32x4Fore = _mm_mul_ps(f32x4Fore, f32x4Alpha);
	f32x4Fore = _mm_div_ps(f32x4Fore, _mm_set1_ps(255.0f));

	i32x4Fore = _mm_cvttps_epi32(f32x4Fore);
	const uint32_t color = bgr_from_abgr_epi32_sse2_si32(i32x4Fore);
	return color | (alpha << 24);
}

#else
constexpr uint32_t Proportional(uint8_t a, uint8_t b, XYPOSITION t) noexcept {
	return static_cast<uint32_t>(a + t * (b - a));
}

constexpr DWORD Proportional(ColourRGBA a, ColourRGBA b, XYPOSITION t) noexcept {
	const uint32_t red = Proportional(a.GetRed(), b.GetRed(), t);
	const uint32_t green = Proportional(a.GetGreen(), b.GetGreen(), t);
	const uint32_t blue = Proportional(a.GetBlue(), b.GetBlue(), t);
	const uint32_t alpha = Proportional(a.GetAlpha(), b.GetAlpha(), t);
	return RGBQuadMultiplied(alpha, red, green, blue);
}
#endif

DWORD GradientValue(const std::vector<ColourStop> &stops, XYPOSITION proportion) noexcept {
	for (size_t stop = 0; stop < stops.size() - 1; stop++) {
		// Loop through each pair of stops
		const XYPOSITION positionStart = stops[stop].position;
		const XYPOSITION positionEnd = stops[stop + 1].position;
		if ((proportion >= positionStart) && (proportion <= positionEnd)) {
			const XYPOSITION proportionInPair = (proportion - positionStart) / (positionEnd - positionStart);
			return Proportional(stops[stop].colour, stops[stop + 1].colour, proportionInPair);
		}
	}
	// Loop should always find a value
	return 0;
}

constexpr BLENDFUNCTION mergeAlpha = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

void SurfaceGDI::AlphaRectangle(PRectangle rc, XYPOSITION cornerSize, FillStroke fillStroke) noexcept {
	// TODO: Implement strokeWidth
	const RECT rcw = RectFromPRectangle(rc);
	const SIZE size = SizeOfRect(rcw);

	if (size.cx > 0) {
		DIBSection section(hdc, size);
		if (section) {
			// Ensure not distorted too much by corners when small
			const LONG corner = std::min(static_cast<LONG>(cornerSize), (std::min(size.cx, size.cy) / 2) - 2);

			constexpr DWORD valEmpty = 0;
			const DWORD valFill = RGBQuadMultiplied(fillStroke.fill.colour);
			const DWORD valOutline = RGBQuadMultiplied(fillStroke.stroke.colour);

			// Draw a framed rectangle
			for (int y = 0; y < size.cy; y++) {
				for (int x = 0; x < size.cx; x++) {
					if ((x == 0) || (x == size.cx - 1) || (y == 0) || (y == size.cy - 1)) {
						section.SetPixel(x, y, valOutline);
					} else {
						section.SetPixel(x, y, valFill);
					}
				}
			}

			// Make the corners transparent
			for (LONG c = 0; c < corner; c++) {
				for (LONG x = 0; x < c + 1; x++) {
					section.SetSymmetric(x, c - x, valEmpty);
				}
			}

			// Draw the corner frame pieces
			for (LONG x = 1; x < corner; x++) {
				section.SetSymmetric(x, corner - x, valOutline);
			}

			GdiAlphaBlend(hdc, rcw.left, rcw.top, size.cx, size.cy, section.DC(), 0, 0, size.cx, size.cy, mergeAlpha);
		}
	} else {
		BrushColour(fillStroke.stroke.colour);
		::FrameRect(hdc, &rcw, brush);
	}
}

void SurfaceGDI::GradientRectangle(PRectangle rc, const std::vector<ColourStop> &stops, GradientOptions options) {
	const RECT rcw = RectFromPRectangle(rc);
	const SIZE size = SizeOfRect(rcw);

	DIBSection section(hdc, size);
	if (section) {
		if (options == GradientOptions::topToBottom) {
			for (LONG y = 0; y < size.cy; y++) {
				// Find y/height proportional colour
				const XYPOSITION proportion = y / (rc.Height() - 1.0f);
				const DWORD valFill = GradientValue(stops, proportion);
				for (LONG x = 0; x < size.cx; x++) {
					section.SetPixel(x, y, valFill);
				}
			}
		} else {
			for (LONG x = 0; x < size.cx; x++) {
				// Find x/width proportional colour
				const XYPOSITION proportion = x / (rc.Width() - 1.0f);
				const DWORD valFill = GradientValue(stops, proportion);
				for (LONG y = 0; y < size.cy; y++) {
					section.SetPixel(x, y, valFill);
				}
			}
		}

		GdiAlphaBlend(hdc, rcw.left, rcw.top, size.cx, size.cy, section.DC(), 0, 0, size.cx, size.cy, mergeAlpha);
	}
}

void SurfaceGDI::DrawRGBAImage(PRectangle rc, int width, int height, const unsigned char *pixelsImage) noexcept {
	if (rc.Width() > 0) {
		if (rc.Width() > width)
			rc.left += std::floor((rc.Width() - width) / 2);
		rc.right = rc.left + width;
		if (rc.Height() > height)
			rc.top += std::floor((rc.Height() - height) / 2);
		rc.bottom = rc.top + height;

		const SIZE size{ width, height };
		const DIBSection section(hdc, size);
		if (section) {
			RGBAImage::BGRAFromRGBA(section.Bytes(), pixelsImage, width * height);
			GdiAlphaBlend(hdc, static_cast<int>(rc.left), static_cast<int>(rc.top),
				static_cast<int>(rc.Width()), static_cast<int>(rc.Height()), section.DC(),
				0, 0, width, height, mergeAlpha);
		}
	}
}

void SurfaceGDI::Ellipse(PRectangle rc, FillStroke fillStroke) noexcept {
	PenColour(fillStroke.stroke.colour, fillStroke.stroke.width);
	BrushColour(fillStroke.fill.colour);
	const RECT rcw = RectFromPRectangleEx(rc);
	::Ellipse(hdc, rcw.left, rcw.top, rcw.right, rcw.bottom);
}

void SurfaceGDI::Stadium(PRectangle rc, FillStroke fillStroke, [[maybe_unused]] Ends ends) noexcept {
	// TODO: Implement properly - the rectangle is just a placeholder
	RectangleDraw(rc, fillStroke);
}

void SurfaceGDI::Copy(PRectangle rc, Point from, Surface &surfaceSource) noexcept {
	::BitBlt(hdc,
		static_cast<int>(rc.left), static_cast<int>(rc.top),
		static_cast<int>(rc.Width()), static_cast<int>(rc.Height()),
		down_cast<SurfaceGDI &>(surfaceSource).hdc,
		static_cast<int>(from.x), static_cast<int>(from.y), SRCCOPY);
}

std::unique_ptr<IScreenLineLayout> SurfaceGDI::Layout(const IScreenLine *) noexcept {
	return {};
}

using TextPositionsGDI = VarBuffer<int, stackBufferLength>;

void SurfaceGDI::DrawTextCommon(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, UINT fuOptions) {
	SetFont(font_);
	const RECT rcw = RectFromPRectangleEx(rc);
	const int x = static_cast<int>(rcw.left);
	const int yBaseInt = static_cast<int>(ybase);

	if (mode.codePage == CpUtf8) {
		const TextWide tbuf(text, CpUtf8);
		::ExtTextOutW(hdc, x, yBaseInt, fuOptions, &rcw, tbuf.data(), tbuf.length(), nullptr);
	} else {
		::ExtTextOutA(hdc, x, yBaseInt, fuOptions, &rcw, text.data(), static_cast<UINT>(text.length()), nullptr);
	}
}

void SurfaceGDI::DrawTextNoClip(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	::SetTextColor(hdc, fore.OpaqueRGB());
	::SetBkColor(hdc, back.OpaqueRGB());
	DrawTextCommon(rc, font_, ybase, text, ETO_OPAQUE);
}

void SurfaceGDI::DrawTextClipped(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	::SetTextColor(hdc, fore.OpaqueRGB());
	::SetBkColor(hdc, back.OpaqueRGB());
	DrawTextCommon(rc, font_, ybase, text, ETO_OPAQUE | ETO_CLIPPED);
}

void SurfaceGDI::DrawTextTransparent(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore) {
	// Avoid drawing spaces in transparent mode
	for (const char ch : text) {
		if (ch != ' ') {
			::SetTextColor(hdc, fore.OpaqueRGB());
			::SetBkMode(hdc, TRANSPARENT);
			DrawTextCommon(rc, font_, ybase, text, 0);
			::SetBkMode(hdc, OPAQUE);
			return;
		}
	}
}

void SurfaceGDI::MeasureWidths(const Font *font_, std::string_view text, XYPOSITION *positions) {
	SetFont(font_);
	SIZE sz {};
	int fit = 0;
	int i = 0;
	const int len = static_cast<int>(text.length());
	TextPositionsGDI poses;
	if (mode.codePage == CpUtf8) {
		const TextWide tbuf(text, CpUtf8);
		poses.allocate(tbuf.length());
		if (!::GetTextExtentExPointW(hdc, tbuf.data(), tbuf.length(), maxWidthMeasure, &fit, poses.data(), &sz)) {
			// Failure
			return;
		}
		// Map the widths given for UTF-16 characters back onto the UTF-8 input string
		for (int ui = 0; ui < fit; ui++) {
			const unsigned char uch = text[i];
			const unsigned int byteCount = UTF8BytesOfLead(uch);
			if (byteCount == 4) {	// Non-BMP
				ui++;
			}
			const XYPOSITION pos = static_cast<XYPOSITION>(poses[ui]);
			for (unsigned int bytePos = 0; (bytePos < byteCount) && (i < len); bytePos++) {
				positions[i++] = pos;
			}
		}
	} else {
		poses.allocate(len);
		if (!::GetTextExtentExPointA(hdc, text.data(), len, maxWidthMeasure, &fit, poses.data(), &sz)) {
			// Eeek - a NULL DC or other foolishness could cause this.
			return;
		}
		while (i < fit) {
			positions[i] = static_cast<XYPOSITION>(poses[i]);
			i++;
		}
	}
	// If any positions not filled in then use the last position for them
	const XYPOSITION lastPos = (fit > 0) ? positions[fit - 1] : 0.0f;
	std::fill(positions + i, positions + text.length(), lastPos);
}

XYPOSITION SurfaceGDI::WidthText(const Font *font_, std::string_view text) {
	SetFont(font_);
	SIZE sz {};
	if (mode.codePage != CpUtf8) {
		::GetTextExtentPoint32A(hdc, text.data(), std::min(static_cast<int>(text.length()), maxLenText), &sz);
	} else {
		const TextWide tbuf(text, CpUtf8);
		::GetTextExtentPoint32W(hdc, tbuf.data(), tbuf.length(), &sz);
	}
	return static_cast<XYPOSITION>(sz.cx);
}

void SurfaceGDI::DrawTextCommonUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text, UINT fuOptions) {
	SetFont(font_);
	const RECT rcw = RectFromPRectangleEx(rc);
	const int x = static_cast<int>(rcw.left);
	const int yBaseInt = static_cast<int>(ybase);

	const TextWide tbuf(text, CpUtf8);
	::ExtTextOutW(hdc, x, yBaseInt, fuOptions, &rcw, tbuf.data(), tbuf.length(), nullptr);
}

void SurfaceGDI::DrawTextNoClipUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	::SetTextColor(hdc, fore.OpaqueRGB());
	::SetBkColor(hdc, back.OpaqueRGB());
	DrawTextCommonUTF8(rc, font_, ybase, text, ETO_OPAQUE);
}

void SurfaceGDI::DrawTextClippedUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore, ColourRGBA back) {
	::SetTextColor(hdc, fore.OpaqueRGB());
	::SetBkColor(hdc, back.OpaqueRGB());
	DrawTextCommonUTF8(rc, font_, ybase, text, ETO_OPAQUE | ETO_CLIPPED);
}

void SurfaceGDI::DrawTextTransparentUTF8(PRectangle rc, const Font *font_, XYPOSITION ybase, std::string_view text,
	ColourRGBA fore) {
	// Avoid drawing spaces in transparent mode
	for (const char ch : text) {
		if (ch != ' ') {
			::SetTextColor(hdc, fore.OpaqueRGB());
			::SetBkMode(hdc, TRANSPARENT);
			DrawTextCommonUTF8(rc, font_, ybase, text, 0);
			::SetBkMode(hdc, OPAQUE);
			return;
		}
	}
}

void SurfaceGDI::MeasureWidthsUTF8(const Font *font_, std::string_view text, XYPOSITION *positions) {
	SetFont(font_);
	SIZE sz = { 0,0 };
	int fit = 0;
	int i = 0;
	const int len = static_cast<int>(text.length());
	const TextWide tbuf(text, CpUtf8);
	TextPositionsGDI poses(tbuf.length());
	if (!::GetTextExtentExPointW(hdc, tbuf.data(), tbuf.length(), maxWidthMeasure, &fit, poses.data(), &sz)) {
		// Failure
		return;
	}
	// Map the widths given for UTF-16 characters back onto the UTF-8 input string
	for (int ui = 0; ui < fit; ui++) {
		const unsigned char uch = text[i];
		const unsigned int byteCount = UTF8BytesOfLead(uch);
		if (byteCount == 4) {	// Non-BMP
			ui++;
		}
		for (unsigned int bytePos = 0; (bytePos < byteCount) && (i < len); bytePos++) {
			positions[i++] = static_cast<XYPOSITION>(poses[ui]);
		}
	}
	// If any positions not filled in then use the last position for them
	const XYPOSITION lastPos = (fit > 0) ? positions[fit - 1] : 0.0f;
	std::fill(positions + i, positions + text.length(), lastPos);
}

XYPOSITION SurfaceGDI::WidthTextUTF8(const Font *font_, std::string_view text) {
	SetFont(font_);
	SIZE sz = { 0,0 };
	const TextWide tbuf(text, CpUtf8);
	::GetTextExtentPoint32W(hdc, tbuf.data(), tbuf.length(), &sz);
	return static_cast<XYPOSITION>(sz.cx);
}

XYPOSITION SurfaceGDI::Ascent(const Font *font_) noexcept {
	SetFont(font_);
	TEXTMETRIC tm;
	::GetTextMetrics(hdc, &tm);
	return static_cast<XYPOSITION>(tm.tmAscent);
}

XYPOSITION SurfaceGDI::Descent(const Font *font_) noexcept {
	SetFont(font_);
	TEXTMETRIC tm;
	::GetTextMetrics(hdc, &tm);
	return static_cast<XYPOSITION>(tm.tmDescent);
}

XYPOSITION SurfaceGDI::InternalLeading(const Font *font_) noexcept {
	SetFont(font_);
	TEXTMETRIC tm;
	::GetTextMetrics(hdc, &tm);
	return static_cast<XYPOSITION>(tm.tmInternalLeading);
}

XYPOSITION SurfaceGDI::Height(const Font *font_) noexcept {
	SetFont(font_);
	TEXTMETRIC tm;
	::GetTextMetrics(hdc, &tm);
	return static_cast<XYPOSITION>(tm.tmHeight);
}

XYPOSITION SurfaceGDI::AverageCharWidth(const Font *font_) noexcept {
	SetFont(font_);
	TEXTMETRIC tm;
	::GetTextMetrics(hdc, &tm);
	return static_cast<XYPOSITION>(tm.tmAveCharWidth);
}

void SurfaceGDI::SetClip(PRectangle rc) noexcept {
	::SaveDC(hdc);
	::IntersectClipRect(hdc, static_cast<int>(rc.left), static_cast<int>(rc.top),
		static_cast<int>(rc.right), static_cast<int>(rc.bottom));
}

void SurfaceGDI::PopClip() noexcept {
	::RestoreDC(hdc, -1);
}

void SurfaceGDI::FlushCachedState() noexcept {
	pen = {};
	brush = {};
}

void SurfaceGDI::FlushDrawing() noexcept {
}

}

namespace Scintilla::Internal {

std::unique_ptr<Surface> SurfaceGDI_Allocate() {
	return std::make_unique<SurfaceGDI>();
}

}
