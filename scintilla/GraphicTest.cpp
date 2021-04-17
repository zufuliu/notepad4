#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "include/VectorISA.h"
#if !(defined(__GNUC__) || defined(__clang__))
#undef NP2_USE_AVX2
#define NP2_USE_AVX2	1
#endif
#include "include/GraphicUtils.h"
#if defined(_MSC_VER)
#pragma comment(lib, "gdi32.lib")
#endif

// cl /EHsc /std:c++17 /DNDEBUG /Ox /Ot /FAcs /GS- /GR- /Gv /W4 /arch:AVX2 GraphicTest.cpp
// clang-cl /EHsc /std:c++17 /DNDEBUG /Ox /Ot /FA /GS- /GR- /Gv /W4 -march=x86-64-v3 GraphicTest.cpp
// g++ -S -std=gnu++17 -DNDEBUG -O3 -fno-rtti -Wall -Wextra -mavx2 -mpopcnt -mbmi -mbmi2 -mlzcnt -mmovbe GraphicTest.cpp

struct ColourAlpha {
	uint32_t color;

	constexpr ColourAlpha(uint32_t value) noexcept : color(value) {}
	constexpr ColourAlpha(uint32_t red, uint32_t green, uint32_t blue) noexcept
		: color(red | (green << 8) | (blue << 16)) {}
	constexpr ColourAlpha(uint32_t red, uint32_t green, uint32_t blue, uint32_t alpha) noexcept
		: color(red | (green << 8) | (blue << 16) | (alpha << 24)) {}
	constexpr uint32_t AsInteger() const noexcept {
		return color;
	}
	constexpr uint8_t GetRed() const noexcept {
		return color & 0xff;
	}
	constexpr uint8_t GetGreen() const noexcept {
		return (color >> 8) & 0xff;
	}
	constexpr uint8_t GetBlue() const noexcept {
		return (color >> 16) & 0xff;
	}
	constexpr uint8_t GetAlpha() const noexcept {
		return (color >> 24) & 0xff;
	}
};
using ColourDesired = ColourAlpha;

union TwoPixel {
	uint32_t pixel[2];
	uint64_t combine;
};

#if 0 // dump
void mm_dump_epi8(const char *msg, __m128i i32x4) noexcept {
	uint8_t dummy[16];
	_mm_storeu_si128((__m128i *)dummy, i32x4);
	printf("%s: {%02x %02x %02x %02x}{%02x %02x %02x %02x}{%02x %02x %02x %02x}{%02x %02x %02x %02x}\n", msg,
		dummy[15], dummy[14], dummy[13], dummy[12], dummy[11], dummy[10], dummy[9], dummy[8],
		dummy[7],  dummy[6],  dummy[5],  dummy[4],  dummy[3],  dummy[2],  dummy[1], dummy[0]);
}

void mm_dump_ps(const char *msg, __m128 f32x4) noexcept {
	float dummy[4];
	_mm_storeu_ps(dummy, f32x4);
	printf("%s: {%f, %f, %f, %f}\n", msg, dummy[0], dummy[1], dummy[2], dummy[3]);
}

void mm256_dump_epi8(const char *msg, __m256i i32x8) noexcept {
	uint8_t dummy[32];
	_mm256_storeu_si256((__m256i *)dummy, i32x8);
	printf("%s: {%02x %02x %02x %02x}{%02x %02x %02x %02x}{%02x %02x %02x %02x}{%02x %02x %02x %02x} "
		"{%02x %02x %02x %02x}{%02x %02x %02x %02x}{%02x %02x %02x %02x}{%02x %02x %02x %02x}\n", msg,
		dummy[31], dummy[30], dummy[29], dummy[28], dummy[27], dummy[26], dummy[25], dummy[24],
		dummy[23], dummy[22], dummy[21], dummy[20], dummy[19], dummy[18], dummy[17], dummy[16],
		dummy[15], dummy[14], dummy[13], dummy[12], dummy[11], dummy[10], dummy[9],  dummy[8],
		dummy[7],  dummy[6],  dummy[5],  dummy[4],  dummy[3],  dummy[2],  dummy[1],  dummy[0]);
}
#endif

#if 0 // MixedWith
ColourAlpha MixedWith(ColourAlpha colour, ColourAlpha other) noexcept {
	const unsigned int red = (colour.GetRed() + other.GetRed()) / 2;
	const unsigned int green = (colour.GetGreen() + other.GetGreen()) / 2;
	const unsigned int blue = (colour.GetBlue() + other.GetBlue()) / 2;
	const unsigned int alpha = (colour.GetAlpha() + other.GetAlpha()) / 2;
	return ColourAlpha(red, green, blue, alpha);
}

ColourAlpha MixedWith_avx2(ColourAlpha colour, ColourAlpha other) noexcept {
	__m128i i32x4Color = mm_unpack_color_avx2_si32(colour.AsInteger());
	__m128i i32x4Other = mm_unpack_color_avx2_si32(other.AsInteger());
	i32x4Color = _mm_add_epi32(i32x4Color, i32x4Other);
	i32x4Color = _mm_srli_epi32(i32x4Color, 1);
	const uint32_t color = mm_pack_color_si32(i32x4Color);
	return ColourAlpha(color);
}

ColourAlpha MixedWith_sse2(ColourAlpha colour, ColourAlpha other) noexcept {
	__m128i i32x4Color = mm_unpack_color_sse2_si32(colour.AsInteger());
	__m128i i32x4Other = mm_unpack_color_sse2_si32(other.AsInteger());
	i32x4Color = _mm_add_epi32(i32x4Color, i32x4Other);
	i32x4Color = _mm_srli_epi32(i32x4Color, 1);
	const uint32_t color = mm_pack_color_si32(i32x4Color);
	return ColourAlpha(color);
}
#endif

#if 0 // AlphaBlend
ColourDesired AlphaBlend(ColourDesired fore, ColourDesired back, unsigned int alpha) noexcept {
	const unsigned int red = (fore.GetRed()*alpha + back.GetRed()*(255 ^ alpha)) / 255;
	const unsigned int green = (fore.GetGreen()*alpha + back.GetGreen()*(255 ^ alpha)) / 255;
	const unsigned int blue = (fore.GetBlue()*alpha + back.GetBlue()*(255 ^ alpha)) / 255;
	return ColourDesired(red, green, blue);
}

ColourDesired AlphaBlend_avx2(ColourDesired fore, ColourDesired back, unsigned int alpha) noexcept {
	__m128i i32x4Fore = mm_unpack_color_avx2_si32(fore.AsInteger());
	__m128i i32x4Back = mm_unpack_color_avx2_si32(back.AsInteger());
	__m128i i32x4Alpha = _mm_set1_epi32(alpha);
	i32x4Fore = mm_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint32_t color = mm_pack_color_si32(i32x4Fore);
	return ColourDesired(color);
}

ColourDesired AlphaBlend_sse2(ColourDesired fore, ColourDesired back, unsigned int alpha) noexcept {
	__m128i i32x4Fore = mm_unpack_color_sse2_si32(fore.AsInteger());
	__m128i i32x4Back = mm_unpack_color_sse2_si32(back.AsInteger());
	__m128i i32x4Alpha = _mm_set1_epi32(alpha);
	i32x4Fore = mm_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint32_t color = mm_pack_color_si32(i32x4Fore);
	return ColourDesired(color);
}
#endif

#if 0 // RGBQuadMultiplied
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

uint32_t RGBQuadMultiplied(ColourAlpha colour) noexcept {
	return RGBQuadMultiplied(colour.GetAlpha(), colour.GetRed(), colour.GetGreen(), colour.GetBlue());
}

uint32_t RGBQuadMultiplied_avx21(ColourAlpha colour) noexcept {
	__m128i i32x4Color = rgba_to_abgr_avx2_si32(colour.AsInteger());
	__m128i i32x4Alpha = _mm_broadcastd_epi32(i32x4Color);
	i32x4Color = _mm_mullo_epi16(i32x4Color, i32x4Alpha);
	i32x4Color = mm_divlo_epu16_by_255(i32x4Color);
	i32x4Color = _mm_alignr_epi8(i32x4Alpha, i32x4Color, 4);
	return mm_pack_color_si32(i32x4Color);
}

uint32_t RGBQuadMultiplied_avx22(ColourAlpha colour) noexcept {
	__m128i i32x4Color = rgba_to_bgra_avx2_si32(colour.AsInteger());
	__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Color, 0xff);
	i32x4Color = _mm_mullo_epi16(i32x4Color, i32x4Alpha);
	i32x4Color = mm_divlo_epu16_by_255(i32x4Color);
	i32x4Color = _mm_blend_epi32(i32x4Alpha, i32x4Color, 7);
	return mm_pack_color_si32(i32x4Color);
}

uint32_t RGBQuadMultiplied_sse2(ColourAlpha colour) noexcept {
	const uint32_t rgba = bswap32(colour.AsInteger());
	__m128i i32x4Color = mm_unpack_color_sse2_si32(rgba);
	__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Color, 0);
	i32x4Color = _mm_mullo_epi16(i32x4Color, i32x4Alpha);
	i32x4Color = mm_divlo_epu16_by_255(i32x4Color);

	const uint32_t color = bgr_from_abgr_s132(i32x4Color);
	return color | (rgba << 24);
}
#endif

#if 0 // BGRAFromRGBA
void BGRAFromRGBA(unsigned char *pixelsBGRA, const unsigned char *pixelsRGBA, size_t count) noexcept {
	constexpr size_t bytesPerPixel = 4;
	for (size_t i = 0; i < count; i++) {
		const unsigned char alpha = pixelsRGBA[3];
		// Input is RGBA, output is BGRA with premultiplied alpha
		pixelsBGRA[2] = pixelsRGBA[0] * alpha / 255;
		pixelsBGRA[1] = pixelsRGBA[1] * alpha / 255;
		pixelsBGRA[0] = pixelsRGBA[2] * alpha / 255;
		pixelsBGRA[3] = alpha;
		pixelsRGBA += bytesPerPixel;
		pixelsBGRA += bytesPerPixel;
	}
}

void BGRAFromRGBA_avx2(unsigned char *pixelsBGRA, const unsigned char *pixelsRGBA, size_t count) noexcept {
	constexpr size_t bytesPerPixel = 4;
	count /= bytesPerPixel;
	uint32_t *pbgra = reinterpret_cast<uint32_t *>(pixelsBGRA);
	const uint32_t *prgba = reinterpret_cast<const uint32_t *>(pixelsRGBA);
	for (size_t i = 0; i < count; i++, pbgra++) {
		__m128i i32x4Color = mm_unpack_color_avx2_si32(loadbe_u32(prgba++));
		__m128i i32x4Alpha = _mm_broadcastd_epi32(i32x4Color);
		i32x4Color = _mm_mullo_epi16(i32x4Color, i32x4Alpha);
		i32x4Color = mm_divlo_epu16_by_255(i32x4Color);
		i32x4Color = _mm_alignr_epi8(i32x4Alpha, i32x4Color, 4);

		i32x4Color = mm_pack_color_si128(i32x4Color);
		mm_storeu_si32(pbgra, i32x4Color);
	}
}

void BGRAFromRGBA_sse2(unsigned char *pixelsBGRA, const unsigned char *pixelsRGBA, size_t count) noexcept {
	constexpr size_t bytesPerPixel = 4;
	count /= bytesPerPixel;
	uint32_t *pbgra = reinterpret_cast<uint32_t *>(pixelsBGRA);
	const uint32_t *prgba = reinterpret_cast<const uint32_t *>(pixelsRGBA);
	for (size_t i = 0; i < count; i++, pbgra++) {
		const uint32_t rgba = bswap32(*prgba++);
		__m128i i32x4Color = mm_unpack_color_sse2_si32(rgba);
		__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Color, 0);
		i32x4Color = _mm_mullo_epi16(i32x4Color, i32x4Alpha);
		i32x4Color = mm_divlo_epu16_by_255(i32x4Color);

		const uint32_t color = bgr_from_abgr_s132(i32x4Color);
		*pbgra++ = (color | (rgba << 24));
	}
}
#endif

#if 0 // Proportional
constexpr uint32_t Proportional(uint8_t a, uint8_t b, double t) noexcept {
	return static_cast<uint32_t>(a + t * (b - a));
}

uint32_t Proportional(ColourAlpha a, ColourAlpha b, double t) noexcept {
	const uint32_t red = Proportional(a.GetRed(), b.GetRed(), t);
	const uint32_t green = Proportional(a.GetGreen(), b.GetGreen(), t);
	const uint32_t blue = Proportional(a.GetBlue(), b.GetBlue(), t);
	const uint32_t alpha = Proportional(a.GetAlpha(), b.GetAlpha(), t);
	return RGBQuadMultiplied(alpha, red, green, blue);
}

uint32_t Proportional_avx21(ColourAlpha a, ColourAlpha b, double t) noexcept {
	__m128i i32x4Fore = rgba_to_abgr_avx2_si32(a.AsInteger());
	__m128i i32x4Back = rgba_to_abgr_avx2_si32(b.AsInteger());
	// a + t * (b - a)
	__m128 f32x4Fore = _mm_cvtepi32_ps(_mm_sub_epi32(i32x4Back, i32x4Fore));
	f32x4Fore = _mm_mul_ps(f32x4Fore, _mm_set1_ps((float)t));
	f32x4Fore = _mm_add_ps(f32x4Fore, _mm_cvtepi32_ps(i32x4Fore));
	// component * alpha / 255
	__m128 f32x4Alpha = _mm_broadcastss_ps(f32x4Fore);
	f32x4Fore = _mm_mul_ps(f32x4Fore, f32x4Alpha);
	f32x4Fore = _mm_div_ps(f32x4Fore, _mm_set1_ps(255.0f));
	f32x4Fore = mm_alignr_ps(f32x4Alpha, f32x4Fore, 1);

	i32x4Fore = _mm_cvttps_epi32(f32x4Fore);
	return mm_pack_color_si32(i32x4Fore);
}

uint32_t Proportional_avx22(ColourAlpha a, ColourAlpha b, double t) noexcept {
	__m128i i32x4Fore = rgba_to_bgra_avx2_si32(a.AsInteger());
	__m128i i32x4Back = rgba_to_bgra_avx2_si32(b.AsInteger());
	// a + t * (b - a)
	__m128 f32x4Fore = _mm_cvtepi32_ps(_mm_sub_epi32(i32x4Back, i32x4Fore));
	f32x4Fore = _mm_mul_ps(f32x4Fore, _mm_set1_ps((float)t));
	f32x4Fore = _mm_add_ps(f32x4Fore, _mm_cvtepi32_ps(i32x4Fore));
	// component * alpha / 255
	__m128 f32x4Alpha = _mm_shuffle_ps(f32x4Fore, f32x4Fore, 0xff);
	f32x4Fore = _mm_mul_ps(f32x4Fore, f32x4Alpha);
	f32x4Fore = _mm_div_ps(f32x4Fore, _mm_set1_ps(255.0f));
	f32x4Fore = _mm_blend_ps(f32x4Alpha, f32x4Fore, 7);

	i32x4Fore = _mm_cvttps_epi32(f32x4Fore);
	return mm_pack_color_si32(i32x4Fore);
}

uint32_t Proportional_sse2(ColourAlpha a, ColourAlpha b, double t) noexcept {
	__m128i i32x4Fore = rgba_to_abgr_sse2_si32(a.AsInteger());
	__m128i i32x4Back = rgba_to_abgr_sse2_si32(b.AsInteger());
	// a + t * (b - a)
	__m128 f32x4Fore = _mm_cvtepi32_ps(_mm_sub_epi32(i32x4Back, i32x4Fore));
	f32x4Fore = _mm_mul_ps(f32x4Fore, _mm_set1_ps((float)t));
	f32x4Fore = _mm_add_ps(f32x4Fore, _mm_cvtepi32_ps(i32x4Fore));
	// component * alpha / 255
	const uint32_t alpha = _mm_cvttss_si32(f32x4Fore);
	__m128 f32x4Alpha = _mm_shuffle_ps(f32x4Fore, f32x4Fore, 0);
	f32x4Fore = _mm_mul_ps(f32x4Fore, f32x4Alpha);
	f32x4Fore = _mm_div_ps(f32x4Fore, _mm_set1_ps(255.0f));

	i32x4Fore = _mm_cvttps_epi32(f32x4Fore);
	const uint32_t color = bgr_from_abgr_s132(i32x4Fore);
	return color | (alpha << 24);
}
#endif

#if 0 // BitmapMergeAlpha
uint32_t BitmapMergeAlpha(ColourAlpha fore, ColourAlpha back) noexcept {
	unsigned int alpha = fore.GetAlpha();
	const unsigned int red = (fore.GetRed()*alpha + back.GetRed()*(255 ^ alpha)) / 255;
	const unsigned int green = (fore.GetGreen()*alpha + back.GetGreen()*(255 ^ alpha)) / 255;
	const unsigned int blue = (fore.GetBlue()*alpha + back.GetBlue()*(255 ^ alpha)) / 255;
	return ColourAlpha(red, green, blue, 0xff).AsInteger();
}

uint32_t BitmapMergeAlpha_avx2(const uint32_t *fore, COLORREF back) noexcept {
	__m128i i32x4Fore = mm_unpack_color_avx2_ptr32(fore);
	__m128i i32x4Back = rgba_to_bgra_avx2_si32(back);
	__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Fore, 0xff);
	i32x4Fore = mm_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint32_t color = mm_pack_color_si32(i32x4Fore);
	return color | 0xff000000U;
}

uint64_t BitmapMergeAlpha_mm256(const uint64_t *fore, COLORREF back) noexcept {
	__m256i i32x4Fore = mm256_unpack_color_ptr64(fore);
	__m256i i32x4Back = mm256_rgba_to_bgra_si32(back);
	__m256i i32x4Alpha = _mm256_shuffle_epi32(i32x4Fore, 0xff);
	i32x4Fore = mm256_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint64_t color = mm256_pack_color_si64(i32x4Fore);
	return color | UINT64_C(0xff000000ff000000);
}

uint32_t BitmapMergeAlpha_sse2(uint32_t fore, COLORREF back) noexcept {
	__m128i i32x4Fore = mm_unpack_color_sse2_si32(fore);
	__m128i i32x4Back = rgba_to_bgra_sse2_si32(back);
	__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Fore, 0xff);
	i32x4Fore = mm_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint32_t color = mm_pack_color_si32(i32x4Fore);
	return color | 0xff000000U;
}
#endif

#if 0 // BitmapAlphaBlend
uint32_t BitmapAlphaBlend(ColourAlpha fore, ColourAlpha back, unsigned int alpha) noexcept {
	const unsigned int red = (fore.GetRed()*alpha + back.GetRed()*(255 ^ alpha)) / 255;
	const unsigned int green = (fore.GetGreen()*alpha + back.GetGreen()*(255 ^ alpha)) / 255;
	const unsigned int blue = (fore.GetBlue()*alpha + back.GetBlue()*(255 ^ alpha)) / 255;
	return ColourAlpha(red, green, blue, fore.GetAlpha()).AsInteger();
}

uint32_t BitmapAlphaBlend_avx2(const uint32_t *fore, COLORREF back, unsigned int alpha) noexcept {
	const __m128i origin = mm_unpack_color_avx2_ptr32(fore);
	__m128i i32x4Back = rgba_to_bgra_avx2_si32(back);
	__m128i i32x4Alpha = _mm_set1_epi32(alpha);
	__m128i i32x4Fore = mm_alpha_blend_epi32(origin, i32x4Back, i32x4Alpha);
	i32x4Fore = _mm_blend_epi32(origin, i32x4Fore, 7);
	return mm_pack_color_si32(i32x4Fore);
}

uint64_t BitmapAlphaBlend_mm256(const uint64_t *fore, COLORREF back, unsigned int alpha) noexcept {
	const __m256i origin = mm256_unpack_color_ptr64(fore);
	__m256i i32x8Back = mm256_rgba_to_bgra_si32(back);
	__m256i i32x8Alpha = _mm256_set1_epi32(alpha);
	__m256i i32x8Fore = mm256_alpha_blend_epi32(origin, i32x8Back, i32x8Alpha);
	i32x8Fore = _mm256_blend_epi32(origin, i32x8Fore, 0x77);
	return mm256_pack_color_si64(i32x8Fore);
}

uint32_t BitmapAlphaBlend_sse2(uint32_t fore, COLORREF back, unsigned int alpha) noexcept {
	__m128i i32x4Fore = mm_unpack_color_sse2_si32(fore);
	__m128i i32x4Back = rgba_to_bgra_sse2_si32(back);
	__m128i i32x4Alpha = _mm_set1_epi32(alpha);
	i32x4Fore = mm_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint32_t color = bgr_from_bgra_s132(i32x4Fore);
	return color | (fore & 0xff000000);
}
#endif

#if 0 // BitmapGrayScale
uint32_t BitmapGrayScale(ColourAlpha fore) noexcept {
	uint32_t gray = (fore.GetRed() * 38 + fore.GetGreen() * 75 + fore.GetBlue() * 15) >> 7;
	gray = ((gray * 0x80) + (0xD0 * (255 ^ 0x80))) >> 8;
	return (fore.AsInteger() & 0xff000000U) | (gray * 0x010101);
}

uint32_t BitmapGrayScale_avx2(const uint32_t fore) noexcept {
	__m128i i32x4Color = mm_unpack_color_avx2_si32(fore);
	i32x4Color = _mm_mullo_epi16(i32x4Color, _mm_set_epi32(0, 38, 75, 15));
	uint32_t gray = mm_hadd_epi32_si32(i32x4Color);
	gray >>= 7;
	gray = ((gray * 0x80) + (0xD0 * (255 ^ 0x80))) >> 8;
	return (fore & 0xff000000U) | (gray * 0x010101);
}
#endif

#if 0 // VerifyContrast
struct Contrast {
	int diff;
	int scale;
};

Contrast VerifyContrast(COLORREF cr1, COLORREF cr2) noexcept {
	const BYTE r1 = GetRValue(cr1);
	const BYTE g1 = GetGValue(cr1);
	const BYTE b1 = GetBValue(cr1);
	const BYTE r2 = GetRValue(cr2);
	const BYTE g2 = GetGValue(cr2);
	const BYTE b2 = GetBValue(cr2);

	const int diff = abs(r1 - r2) + abs(b1 - b2) + abs(g1 - g2);
	const int scale = abs((3 * r1 + 5 * g1 + 1 * b1) - (3 * r2 + 6 * g2 + 1 * b2));
	return { diff, scale };
}

Contrast VerifyContrast_avx2(COLORREF cr1, COLORREF cr2) noexcept {
	__m128i i32x4Fore = mm_unpack_color_avx2_si32(cr1);
	__m128i i32x4Back = mm_unpack_color_avx2_si32(cr2);

	__m128i diff = _mm_sad_epu8(i32x4Fore, i32x4Back);
	const int value = mm_hadd_epi32_si32(diff);

	i32x4Fore = _mm_mullo_epi16(i32x4Fore, _mm_setr_epi32(3, 5, 1, 0));
	i32x4Back = _mm_mullo_epi16(i32x4Back, _mm_setr_epi32(3, 6, 1, 0));
	diff = _mm_sub_epi32(i32x4Fore, i32x4Back);
	const int scale = mm_hadd_epi32_si32(diff);
	return { value, abs(scale) };
}
#endif

int __cdecl main() {
	constexpr uint32_t alpha = 0x60;
	constexpr uint32_t back = 0x05f3f2f1;
	constexpr uint32_t quad = 0x9b5170a3;
	constexpr uint32_t quad2 = 0x8c8172b2;
	const uint32_t fore = ColorFromRGBQuad(quad);
	const uint32_t fore2 = ColorFromRGBQuad(quad2);
	constexpr TwoPixel twoPixel = { { quad, quad2 } };

#if 0 // MixedWith
	printf("MixedWith(%08x, %08x):\n", fore, back);
	printf("    scale %08x\n", MixedWith(fore, back).AsInteger());
	printf("     avx2 %08x\n", MixedWith_avx2(fore, back).AsInteger());
	printf("     sse2 %08x\n", MixedWith_sse2(fore, back).AsInteger());
#endif

#if 0 // AlphaBlend
	printf("AlphaBlend(%08x, %08x, %02x):\n", fore, back, alpha);
	printf("    scale %08x\n", AlphaBlend(fore, back, alpha).AsInteger());
	printf("     avx2 %08x\n", AlphaBlend_avx2(fore, back, alpha).AsInteger());
	printf("     sse2 %08x\n", AlphaBlend_sse2(fore, back, alpha).AsInteger());
#endif

#if 0 // RGBQuadMultiplied
	printf("RGBQuadMultiplied(%08x):\n", fore);
	printf("    scale %08x\n", RGBQuadMultiplied(fore));
	printf("    avx21 %08x\n", RGBQuadMultiplied_avx21(fore));
	printf("    avx22 %08x\n", RGBQuadMultiplied_avx22(fore));
	printf("     sse2 %08x\n", RGBQuadMultiplied_sse2(fore));
#endif

#if 0 // BGRAFromRGBA
	const uint32_t rgba[2] = { fore, fore2 };
	uint32_t bgra[2];
	const unsigned char *pixelsRGBA = reinterpret_cast<const unsigned char *>(rgba);
	unsigned char *pixelsBGRA = reinterpret_cast<unsigned char *>(bgra);
	constexpr size_t pixelCount = sizeof(rgba);
	printf("BGRAFromRGBA(%08x, %08x)\n", rgba[0], rgba[1]);
	BGRAFromRGBA(pixelsBGRA, pixelsRGBA, pixelCount);
	printf("   single %08x %08x\n", RGBQuadMultiplied(rgba[0]), RGBQuadMultiplied(rgba[1]));
	printf("    scale %08x %08x\n", bgra[0], bgra[1]);
	BGRAFromRGBA_avx2(pixelsBGRA, pixelsRGBA, pixelCount);
	printf("     avx2 %08x %08x\n", bgra[0], bgra[1]);
	BGRAFromRGBA_sse2(pixelsBGRA, pixelsRGBA, pixelCount);
	printf("     sse2 %08x %08x\n", bgra[0], bgra[1]);
#endif

#if 0 // Proportional
	constexpr double proportion = alpha/255.0f;
	printf("Proportional(%08x, %08x, %f):\n", fore, back, proportion);
	printf("    scale %08x\n", Proportional(ColourAlpha(fore), ColourAlpha(back), proportion));
	printf("    avx21 %08x\n", Proportional_avx21(fore, back, proportion));
	printf("    avx22 %08x\n", Proportional_avx22(fore, back, proportion));
	printf("     sse2 %08x\n", Proportional_sse2(fore, back, proportion));
#endif

#if 0 // BitmapMergeAlpha
	printf("BitmapMergeAlpha 1(%08x / %08x, %08x):\n", quad, fore, back);
	printf("    scale %08x\n", RGBQuadFromColor(BitmapMergeAlpha(fore, back)));
	printf("     avx2 %08x\n", BitmapMergeAlpha_avx2(&quad, back));
	printf("     sse2 %08x\n", BitmapMergeAlpha_sse2(quad, back));
	{
		printf("BitmapMergeAlpha 2(%08x / %08x, %08x):\n", quad2, fore2, back);
		printf("    scale %08x\n", RGBQuadFromColor(BitmapMergeAlpha(fore2, back)));
		printf("     avx2 %08x\n", BitmapMergeAlpha_avx2(&quad2, back));
		printf("     sse2 %08x\n", BitmapMergeAlpha_sse2(quad2, back));
		TwoPixel two;
		two.combine = BitmapMergeAlpha_mm256(&twoPixel.combine, back);
		printf("  avx2 x2 %08x %08x\n", two.pixel[0], two.pixel[1]);
	}
#endif

#if 0 // BitmapAlphaBlend
	printf("BitmapAlphaBlend 1(%08x / %08x, %08x, %02x):\n", quad, fore, back, alpha);
	printf("    scale %08x\n", RGBQuadFromColor(BitmapAlphaBlend(fore, back, alpha)));
	printf("     avx2 %08x\n", BitmapAlphaBlend_avx2(&quad, back, alpha));
	printf("     sse2 %08x\n", BitmapAlphaBlend_sse2(quad, back, alpha));
	{
		printf("BitmapAlphaBlend 2(%08x / %08x, %08x, %02x):\n", quad2, fore2, back, alpha);
		printf("    scale %08x\n", RGBQuadFromColor(BitmapAlphaBlend(fore2, back, alpha)));
		printf("     avx2 %08x\n", BitmapAlphaBlend_avx2(&quad2, back, alpha));
		printf("     sse2 %08x\n", BitmapAlphaBlend_sse2(quad2, back, alpha));
		TwoPixel two;
		two.combine = BitmapAlphaBlend_mm256(&twoPixel.combine, back, alpha);
		printf("  avx2 x2 %08x %08x\n", two.pixel[0], two.pixel[1]);
	}
#endif

#if 0 // BitmapGrayScale
	printf("BitmapGrayScale(%08x / %08x):\n", quad, fore);
	printf("    scale %08x\n", BitmapGrayScale(fore));
	printf("     avx2 %08x\n", BitmapGrayScale_avx2(quad));
#endif

#if 0 // VerifyContrast
	const Contrast cr1 = VerifyContrast(fore & 0xffffff, back & 0xffffff);
	const Contrast cr2 = VerifyContrast_avx2(fore & 0xffffff, back & 0xffffff);
	printf("VerifyContrast(%08x, %08x):\n", fore, back);
	printf("    scale %d, %d\n", cr1.diff, cr1.scale);
	printf("     avx2 %d, %d\n", cr2.diff, cr2.scale);
#endif

	return 0;
}
