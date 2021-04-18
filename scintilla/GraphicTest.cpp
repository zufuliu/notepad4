// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "include/VectorISA.h"
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

static inline uint32_t RGBQuadToUInt32(RGBQUAD quad) noexcept {
	return *(uint32_t *)((void *)(&quad));
}

union DualPixel {
	uint32_t pixel32[2];
	uint64_t pixel64;
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

#if NP2_USE_AVX2
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

bool ReadPixelData(const char *path, std::vector<uint32_t> &data) {
	FILE *fp = fopen(path, "rb");
	if (!fp) {
		return false;
	}
	fseek(fp, 0, SEEK_END);
	ptrdiff_t len = ftell(fp);
	rewind(fp);
	if (len > 0) {
		data.resize(len / sizeof(uint32_t));
		const ptrdiff_t rd_len = fread(data.data(), 1, len, fp);
		if (rd_len != len) {
			len = -1;
		}
	}
	fclose(fp);
	return len > 0;
}
#endif // dump

#if 0 // MixedWith
ColourAlpha MixedWith(ColourAlpha colour, ColourAlpha other) noexcept {
	const unsigned int red = (colour.GetRed() + other.GetRed()) / 2;
	const unsigned int green = (colour.GetGreen() + other.GetGreen()) / 2;
	const unsigned int blue = (colour.GetBlue() + other.GetBlue()) / 2;
	const unsigned int alpha = (colour.GetAlpha() + other.GetAlpha()) / 2;
	return ColourAlpha(red, green, blue, alpha);
}

#if NP2_USE_AVX2
ColourAlpha MixedWith_avx2(ColourAlpha colour, ColourAlpha other) noexcept {
	__m128i i32x4Color = mm_unpack_color_avx2_si32(colour.AsInteger());
	__m128i i32x4Other = mm_unpack_color_avx2_si32(other.AsInteger());
	i32x4Color = _mm_add_epi32(i32x4Color, i32x4Other);
	i32x4Color = _mm_srli_epi32(i32x4Color, 1);
	const uint32_t color = mm_pack_color_si32(i32x4Color);
	return ColourAlpha(color);
}
#endif

ColourAlpha MixedWith_sse2(ColourAlpha colour, ColourAlpha other) noexcept {
	__m128i i32x4Color = mm_unpack_color_sse2_si32(colour.AsInteger());
	__m128i i32x4Other = mm_unpack_color_sse2_si32(other.AsInteger());
	i32x4Color = _mm_add_epi32(i32x4Color, i32x4Other);
	i32x4Color = _mm_srli_epi32(i32x4Color, 1);
	const uint32_t color = mm_pack_color_si32(i32x4Color);
	return ColourAlpha(color);
}
#endif // MixedWith

#if 0 // AlphaBlend
ColourDesired AlphaBlend(ColourDesired fore, ColourDesired back, unsigned int alpha) noexcept {
	const unsigned int red = (fore.GetRed()*alpha + back.GetRed()*(255 ^ alpha)) / 255;
	const unsigned int green = (fore.GetGreen()*alpha + back.GetGreen()*(255 ^ alpha)) / 255;
	const unsigned int blue = (fore.GetBlue()*alpha + back.GetBlue()*(255 ^ alpha)) / 255;
	return ColourDesired(red, green, blue);
}
#if NP2_USE_AVX2
ColourDesired AlphaBlend_avx2(ColourDesired fore, ColourDesired back, unsigned int alpha) noexcept {
	__m128i i32x4Fore = mm_unpack_color_avx2_si32(fore.AsInteger());
	__m128i i32x4Back = mm_unpack_color_avx2_si32(back.AsInteger());
	__m128i i32x4Alpha = _mm_set1_epi32(alpha);
	i32x4Fore = mm_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint32_t color = mm_pack_color_si32(i32x4Fore);
	return ColourDesired(color);
}
#endif
ColourDesired AlphaBlend_sse2(ColourDesired fore, ColourDesired back, unsigned int alpha) noexcept {
	__m128i i32x4Fore = mm_unpack_color_sse2_si32(fore.AsInteger());
	__m128i i32x4Back = mm_unpack_color_sse2_si32(back.AsInteger());
	__m128i i32x4Alpha = _mm_set1_epi32(alpha);
	i32x4Fore = mm_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint32_t color = mm_pack_color_si32(i32x4Fore);
	return ColourDesired(color);
}
#endif // AlphaBlend

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
#if NP2_USE_AVX2
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
#endif
uint32_t RGBQuadMultiplied_sse2(ColourAlpha colour) noexcept {
	const uint32_t rgba = bswap32(colour.AsInteger());
	__m128i i32x4Color = mm_unpack_color_sse2_si32(rgba);
	__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Color, 0);
	i32x4Color = _mm_mullo_epi16(i32x4Color, i32x4Alpha);
	i32x4Color = mm_divlo_epu16_by_255(i32x4Color);

	const uint32_t color = bgr_from_abgr_si32(i32x4Color);
	return color | (rgba << 24);
}
#endif // RGBQuadMultiplied

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
#if NP2_USE_AVX2
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
#endif
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

		const uint32_t color = bgr_from_abgr_si32(i32x4Color);
		*pbgra++ = (color | (rgba << 24));
	}
}
#endif // BGRAFromRGBA

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
#if NP2_USE_AVX2
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
#endif
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
	const uint32_t color = bgr_from_abgr_si32(i32x4Fore);
	return color | (alpha << 24);
}
#endif // Proportional

#if 0 // BitmapMergeAlpha
uint32_t BitmapMergeAlpha(ColourAlpha fore, ColourAlpha back) noexcept {
	unsigned int alpha = fore.GetAlpha();
	const unsigned int red = (fore.GetRed()*alpha + back.GetRed()*(255 ^ alpha)) / 255;
	const unsigned int green = (fore.GetGreen()*alpha + back.GetGreen()*(255 ^ alpha)) / 255;
	const unsigned int blue = (fore.GetBlue()*alpha + back.GetBlue()*(255 ^ alpha)) / 255;
	return ColourAlpha(red, green, blue, 0xff).AsInteger();
}
#if NP2_USE_AVX2
uint32_t BitmapMergeAlpha_avx2(const uint32_t *fore, COLORREF back) noexcept {
	__m128i i32x4Fore = mm_unpack_color_avx2_ptr32(fore);
	__m128i i32x4Back = rgba_to_bgra_avx2_si32(back);
	__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Fore, 0xff);
	i32x4Fore = mm_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint32_t color = mm_pack_color_si32(i32x4Fore);
	return color | 0xff000000U;
}

uint64_t BitmapMergeAlpha_avx2_x2(const uint64_t *fore, COLORREF back) noexcept {
	__m256i i32x4Fore = mm256_unpack_color_ptr64(fore);
	__m256i i32x4Back = mm256_rgba_to_bgra_si32(back);
	__m256i i32x4Alpha = _mm256_shuffle_epi32(i32x4Fore, 0xff);
	i32x4Fore = mm256_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint64_t color = mm256_pack_color_si64(i32x4Fore);
	return color | UINT64_C(0xff000000ff000000);
}
#endif
uint32_t BitmapMergeAlpha_sse2(uint32_t fore, COLORREF back) noexcept {
	__m128i i32x4Fore = mm_unpack_color_sse2_si32(fore);
	__m128i i32x4Back = rgba_to_bgra_sse2_si32(back);
	__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Fore, 0xff);
	i32x4Fore = mm_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint32_t color = mm_pack_color_si32(i32x4Fore);
	return color | 0xff000000U;
}
#endif // BitmapMergeAlpha

#if 0 // BitmapAlphaBlend
uint32_t BitmapAlphaBlend(ColourAlpha fore, ColourAlpha back, unsigned int alpha) noexcept {
	const unsigned int red = (fore.GetRed()*alpha + back.GetRed()*(255 ^ alpha)) / 255;
	const unsigned int green = (fore.GetGreen()*alpha + back.GetGreen()*(255 ^ alpha)) / 255;
	const unsigned int blue = (fore.GetBlue()*alpha + back.GetBlue()*(255 ^ alpha)) / 255;
	return ColourAlpha(red, green, blue, fore.GetAlpha()).AsInteger();
}
#if NP2_USE_AVX2
uint32_t BitmapAlphaBlend_avx2(const uint32_t *fore, COLORREF back, unsigned int alpha) noexcept {
	const __m128i origin = mm_unpack_color_avx2_ptr32(fore);
	__m128i i32x4Back = rgba_to_bgra_avx2_si32(back);
	__m128i i32x4Alpha = _mm_set1_epi32(alpha);
	__m128i i32x4Fore = mm_alpha_blend_epi32(origin, i32x4Back, i32x4Alpha);
	i32x4Fore = _mm_blend_epi32(origin, i32x4Fore, 7);
	return mm_pack_color_si32(i32x4Fore);
}

uint64_t BitmapAlphaBlend_avx2_x2(const uint64_t *fore, COLORREF back, unsigned int alpha) noexcept {
	const __m256i origin = mm256_unpack_color_ptr64(fore);
	__m256i i32x8Back = mm256_rgba_to_bgra_si32(back);
	__m256i i32x8Alpha = _mm256_set1_epi32(alpha);
	__m256i i32x8Fore = mm256_alpha_blend_epi32(origin, i32x8Back, i32x8Alpha);
	i32x8Fore = _mm256_blend_epi32(origin, i32x8Fore, 0x77);
	return mm256_pack_color_si64(i32x8Fore);
}
#endif
uint32_t BitmapAlphaBlend_sse2(uint32_t fore, COLORREF back, unsigned int alpha) noexcept {
	__m128i i32x4Fore = mm_unpack_color_sse2_si32(fore);
	__m128i i32x4Back = rgba_to_bgra_sse2_si32(back);
	__m128i i32x4Alpha = _mm_set1_epi32(alpha);
	i32x4Fore = mm_alpha_blend_epi32(i32x4Fore, i32x4Back, i32x4Alpha);
	const uint32_t color = bgr_from_bgra_si32(i32x4Fore);
	return color | (fore & 0xff000000);
}

#if 0 // TestBitmapAlphaBlend
void TestBitmapAlphaBlend(const char *path, const uint32_t crDest, const BYTE alpha) {
	printf("BitmapAlphaBlend(%s, %08x, %02x)\n", path, crDest, alpha);
	std::vector<uint32_t> data;
	if (!ReadPixelData(path, data)) {
		printf("read %s fail\n", path);
		return;
	}

	const size_t pixelCount = data.size();
	std::vector<uint32_t> scale;
	{ // scale
		scale.resize(pixelCount);
		const RGBQUAD *prgba = reinterpret_cast<RGBQUAD *>(data.data());

		const WORD red = GetRValue(crDest) * (255 ^ alpha);
		const WORD green = GetGValue(crDest) * (255 ^ alpha);
		const WORD blue = GetBValue(crDest) * (255 ^ alpha);
		for (size_t x = 0; x < pixelCount; x++, prgba++) {
			RGBQUAD quad = *prgba;
			quad.rgbRed = (BYTE)(((quad.rgbRed * alpha) + red) / 255);
			quad.rgbGreen = (BYTE)(((quad.rgbGreen * alpha) + green) / 255);
			quad.rgbBlue = (BYTE)(((quad.rgbBlue * alpha) + blue) / 255);
			scale[x] = RGBQuadToUInt32(quad);
		}
	}

	{ // sse2 1x1
		std::vector<uint32_t> xmm;
		xmm.resize(pixelCount);
		const uint32_t *prgba = data.data();

		const __m128i i32x4Alpha = _mm_set1_epi32(alpha);
		__m128i i32x4Back = rgba_to_bgra_sse2_si32(crDest);
		i32x4Back = _mm_mullo_epi16(i32x4Back, mm_xor_alpha_epi32(i32x4Alpha));
		for (size_t x = 0; x < pixelCount; x++, prgba++) {
			const uint32_t origin = *prgba;
			__m128i i32x4Fore = mm_unpack_color_sse2_si32(origin);
			i32x4Fore = _mm_mullo_epi16(i32x4Fore, i32x4Alpha);
			i32x4Fore = _mm_add_epi32(i32x4Fore, i32x4Back);
			i32x4Fore = mm_divlo_epu16_by_255(i32x4Fore);
			uint32_t color = bgr_from_bgra_si32(i32x4Fore);
			xmm[x] = color | (origin & 0xff000000U);
		}
		for (size_t x = 0; x < pixelCount; x++) {
			if (scale[x] != xmm[x]) {
				printf("sse2 1x1 fail %4zu %08x, %08x %08x\n", x, data[x], scale[x], xmm[x]);
				break;
			}
		}
	}
	{ // sse2 1x4
		std::vector<uint32_t> xmm;
		xmm.resize(pixelCount);
		constexpr size_t offset = 0;
		const __m128i *prgba = (const __m128i *)(data.data() + offset);
		__m128i *dest = (__m128i *)(xmm.data() + offset);

		const __m128i i32x4Alpha = _mm_set1_epi32(alpha);
		__m128i i32x4Back = rgba_to_bgra_sse2_si32(crDest);
		i32x4Back = _mm_mullo_epi16(i32x4Back, mm_xor_alpha_epi32(i32x4Alpha));
		for (size_t x = offset; x < pixelCount; x += 4, prgba++, dest++) {
			const __m128i origin = _mm_loadu_si128(prgba);
			__m128i i32x4Fore = mm_unpacklo_color_sse2_si128(origin);
			i32x4Fore = _mm_mullo_epi16(i32x4Fore, i32x4Alpha);
			i32x4Fore = _mm_add_epi32(i32x4Fore, i32x4Back);
			i32x4Fore = mm_divlo_epu16_by_255(i32x4Fore);
			__m128i i32x4_12 = mm_pack_color_si128(i32x4Fore);

			__m128i color42 = _mm_shuffle_epi32(origin, 0x31);
			i32x4Fore = mm_unpacklo_color_sse2_si128(color42);
			i32x4Fore = _mm_mullo_epi16(i32x4Fore, i32x4Alpha);
			i32x4Fore = _mm_add_epi32(i32x4Fore, i32x4Back);
			i32x4Fore = mm_divlo_epu16_by_255(i32x4Fore);
			i32x4_12 = _mm_unpacklo_epi32(i32x4_12, mm_pack_color_si128(i32x4Fore));

			i32x4Fore = mm_unpackhi_color_sse2_si128(origin);
			i32x4Fore = _mm_mullo_epi16(i32x4Fore, i32x4Alpha);
			i32x4Fore = _mm_add_epi32(i32x4Fore, i32x4Back);
			i32x4Fore = mm_divlo_epu16_by_255(i32x4Fore);
			__m128i i32x4_34 = mm_pack_color_si128(i32x4Fore);

			i32x4Fore = mm_unpackhi_color_sse2_si128(color42);
			i32x4Fore = _mm_mullo_epi16(i32x4Fore, i32x4Alpha);
			i32x4Fore = _mm_add_epi32(i32x4Fore, i32x4Back);
			i32x4Fore = mm_divlo_epu16_by_255(i32x4Fore);
			i32x4_34 = _mm_unpacklo_epi32(i32x4_34, mm_pack_color_si128(i32x4Fore));

			i32x4_34 = _mm_unpacklo_epi64(i32x4_12, i32x4_34);
			i32x4_34 = _mm_and_si128(_mm_set1_epi32(0x00ffffff), i32x4_34);
			i32x4_12 = _mm_andnot_si128(_mm_set1_epi32(0x00ffffff), origin);
			i32x4_34 = _mm_or_si128(i32x4_12, i32x4_34);
			_mm_storeu_si128(dest, i32x4_34);
		}
		for (size_t x = offset; x < pixelCount; x++) {
			if (scale[x] != xmm[x]) {
				printf("sse2 1x4 fail at %4zu %08x, %08x %08x\n", x, data[x], scale[x], xmm[x]);
				break;
			}
		}
	}
#if NP2_USE_AVX2
	{ // avx2 1x1
		std::vector<uint32_t> xmm;
		xmm.resize(pixelCount);
		const uint32_t *prgba = data.data();
		uint32_t *dest = xmm.data();

		const __m128i i32x4Alpha = _mm_set1_epi32(alpha);
		__m128i i32x4Back = rgba_to_bgra_sse2_si32(crDest);
		i32x4Back = _mm_mullo_epi16(i32x4Back, mm_xor_alpha_epi32(i32x4Alpha));
		for (size_t x = 0; x < pixelCount; x++, prgba++, dest++) {
			const __m128i origin = mm_unpack_color_avx2_ptr32(prgba);
			__m128i i32x4Fore = _mm_mullo_epi16(origin, i32x4Alpha);
			i32x4Fore = _mm_add_epi32(i32x4Fore, i32x4Back);
			i32x4Fore = mm_divlo_epu16_by_255(i32x4Fore);
			i32x4Fore = _mm_blend_epi32(origin, i32x4Fore, 7);
			i32x4Fore = mm_pack_color_si128(i32x4Fore);
			mm_storeu_si32(dest, i32x4Fore);
		}
		for (size_t x = 0; x < pixelCount; x++) {
			if (scale[x] != xmm[x]) {
				printf("avx2 1x1 fail %4zu %08x, %08x %08x\n", x, data[x], scale[x], xmm[x]);
				break;
			}
		}
	}
	{ // avx2 2x1
		std::vector<uint32_t> ymm;
		ymm.resize(pixelCount);
		const uint64_t *prgba = (uint64_t *)data.data();
		uint64_t *dest = (uint64_t *)ymm.data();

		const __m256i i32x8Alpha = _mm256_set1_epi32(alpha);
		__m256i i32x8Back = mm256_rgba_to_bgra_si32(crDest);
		i32x8Back = _mm256_mullo_epi16(i32x8Back, mm256_xor_alpha_epi32(i32x8Alpha));
		for (size_t x = 0; x < pixelCount; x += 2, prgba++, dest++) {
			const __m256i origin = mm256_unpack_color_ptr64(prgba);
			__m256i i32x8Fore = _mm256_mullo_epi16(origin, i32x8Alpha);
			i32x8Fore = _mm256_add_epi32(i32x8Fore, i32x8Back);
			i32x8Fore = mm256_divlo_epu16_by_255(i32x8Fore);
			i32x8Fore = _mm256_blend_epi32(origin, i32x8Fore, 0x77);
			__m128i i32x4Fore = mm256_pack_color_si128(i32x8Fore);
			_mm_storel_epi64((__m128i *)dest, i32x4Fore);
		}
		for (size_t x = 0; x < pixelCount; x++) {
			if (scale[x] != ymm[x]) {
				printf("avx2 2x1 fail %4zu %08x, %08x %08x\n", x, data[x], scale[x], ymm[x]);
				break;
			}
		}
	}
#endif // NP2_USE_AVX2
}
#endif // TestBitmapAlphaBlend
#endif // BitmapAlphaBlend

#if 0 // BitmapGrayScale
uint32_t BitmapGrayScale(ColourAlpha fore) noexcept {
	uint32_t gray = (fore.GetRed() * 38 + fore.GetGreen() * 75 + fore.GetBlue() * 15) >> 7;
	gray = ((gray * 0x80) + (0xD0 * (255 ^ 0x80))) >> 8;
	return (fore.AsInteger() & 0xff000000U) | (gray * 0x010101);
}
#if NP2_USE_AVX2
uint32_t BitmapGrayScale_avx2(const uint32_t fore) noexcept {
	__m128i i32x4Color = mm_unpack_color_avx2_si32(fore);
	i32x4Color = _mm_mullo_epi16(i32x4Color, _mm_set_epi32(0, 38, 75, 15));
	uint32_t gray = mm_hadd_epi32_si32(i32x4Color);
	gray >>= 7;
	gray = ((gray * 0x80) + (0xD0 * (255 ^ 0x80))) >> 8;
	return (fore & 0xff000000U) | (gray * 0x010101);
}
#endif
#endif // BitmapGrayScale

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
#if NP2_USE_AVX2
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
#endif // VerifyContrast

int __cdecl main() {
	constexpr uint32_t alpha = 0x60;
	constexpr uint32_t back = 0x05f3f2f1;
	constexpr uint32_t quad = 0x9b5170a3;
	constexpr uint32_t quad2 = 0x8c8172b2;
	const uint32_t fore = ColorFromRGBQuad(quad);
	const uint32_t fore2 = ColorFromRGBQuad(quad2);
	constexpr DualPixel dualPixels = { { quad, quad2 } };

#if 0 // MixedWith
	printf("MixedWith(%08x, %08x):\n", fore, back);
	printf("    scale %08x\n", MixedWith(fore, back).AsInteger());
#if NP2_USE_AVX2
	printf("     avx2 %08x\n", MixedWith_avx2(fore, back).AsInteger());
#endif
	printf("     sse2 %08x\n", MixedWith_sse2(fore, back).AsInteger());
#endif // MixedWith

#if 0 // AlphaBlend
	printf("AlphaBlend(%08x, %08x, %02x):\n", fore, back, alpha);
	printf("    scale %08x\n", AlphaBlend(fore, back, alpha).AsInteger());
#if NP2_USE_AVX2
	printf("     avx2 %08x\n", AlphaBlend_avx2(fore, back, alpha).AsInteger());
#endif
	printf("     sse2 %08x\n", AlphaBlend_sse2(fore, back, alpha).AsInteger());
#endif // AlphaBlend

#if 0 // RGBQuadMultiplied
	printf("RGBQuadMultiplied(%08x):\n", fore);
	printf("    scale %08x\n", RGBQuadMultiplied(fore));
#if NP2_USE_AVX2
	printf("    avx21 %08x\n", RGBQuadMultiplied_avx21(fore));
	printf("    avx22 %08x\n", RGBQuadMultiplied_avx22(fore));
#endif
	printf("     sse2 %08x\n", RGBQuadMultiplied_sse2(fore));
#endif // RGBQuadMultiplied

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
#if NP2_USE_AVX2
	BGRAFromRGBA_avx2(pixelsBGRA, pixelsRGBA, pixelCount);
	printf("     avx2 %08x %08x\n", bgra[0], bgra[1]);
#endif
	BGRAFromRGBA_sse2(pixelsBGRA, pixelsRGBA, pixelCount);
	printf("     sse2 %08x %08x\n", bgra[0], bgra[1]);
#endif // BGRAFromRGBA

#if 0 // Proportional
	constexpr double proportion = alpha/255.0f;
	printf("Proportional(%08x, %08x, %f):\n", fore, back, proportion);
	printf("    scale %08x\n", Proportional(ColourAlpha(fore), ColourAlpha(back), proportion));
#if NP2_USE_AVX2
	printf("    avx21 %08x\n", Proportional_avx21(fore, back, proportion));
	printf("    avx22 %08x\n", Proportional_avx22(fore, back, proportion));
#endif
	printf("     sse2 %08x\n", Proportional_sse2(fore, back, proportion));
#endif // Proportional

#if 0 // BitmapMergeAlpha
	printf("BitmapMergeAlpha 1(%08x / %08x, %08x):\n", quad, fore, back);
	printf("    scale %08x\n", RGBQuadFromColor(BitmapMergeAlpha(fore, back)));
#if NP2_USE_AVX2
	printf("     avx2 %08x\n", BitmapMergeAlpha_avx2(&quad, back));
#endif
	printf("     sse2 %08x\n", BitmapMergeAlpha_sse2(quad, back));
	{
		printf("BitmapMergeAlpha 2(%08x / %08x, %08x):\n", quad2, fore2, back);
		printf("    scale %08x\n", RGBQuadFromColor(BitmapMergeAlpha(fore2, back)));
#if NP2_USE_AVX2
		printf("     avx2 %08x\n", BitmapMergeAlpha_avx2(&quad2, back));
		DualPixel two;
		two.pixel64 = BitmapMergeAlpha_avx2_x2(&dualPixels.pixel64, back);
		printf("  avx2 x2 %08x %08x\n", two.pixel32[0], two.pixel32[1]);
#endif
		printf("     sse2 %08x\n", BitmapMergeAlpha_sse2(quad2, back));
	}
#endif // BitmapMergeAlpha

#if 0 // BitmapAlphaBlend
	printf("BitmapAlphaBlend 1(%08x / %08x, %08x, %02x):\n", quad, fore, back, alpha);
	printf("    scale %08x\n", RGBQuadFromColor(BitmapAlphaBlend(fore, back, alpha)));
#if NP2_USE_AVX2
	printf("     avx2 %08x\n", BitmapAlphaBlend_avx2(&quad, back, alpha));
#endif
	printf("     sse2 %08x\n", BitmapAlphaBlend_sse2(quad, back, alpha));
	{
		printf("BitmapAlphaBlend 2(%08x / %08x, %08x, %02x):\n", quad2, fore2, back, alpha);
		printf("    scale %08x\n", RGBQuadFromColor(BitmapAlphaBlend(fore2, back, alpha)));
#if NP2_USE_AVX2
		printf("     avx2 %08x\n", BitmapAlphaBlend_avx2(&quad2, back, alpha));
		DualPixel two;
		two.pixel64 = BitmapAlphaBlend_avx2_x2(&dualPixels.pixel64, back, alpha);
		printf("  avx2 x2 %08x %08x\n", two.pixel32[0], two.pixel32[1]);
#endif
		printf("     sse2 %08x\n", BitmapAlphaBlend_sse2(quad2, back, alpha));
	}
	TestBitmapAlphaBlend("bitmap.dat", back, alpha);
#endif // BitmapAlphaBlend

#if 0 // BitmapGrayScale
	printf("BitmapGrayScale(%08x / %08x):\n", quad, fore);
	printf("    scale %08x\n", BitmapGrayScale(fore));
#if NP2_USE_AVX2
	printf("     avx2 %08x\n", BitmapGrayScale_avx2(quad));
#endif
#endif // BitmapGrayScale

#if 0 // VerifyContrast
	const Contrast cr1 = VerifyContrast(fore & 0xffffff, back & 0xffffff);
	printf("VerifyContrast(%08x, %08x):\n", fore, back);
	printf("    scale %d, %d\n", cr1.diff, cr1.scale);
#if NP2_USE_AVX2
	const Contrast cr2 = VerifyContrast_avx2(fore & 0xffffff, back & 0xffffff);
	printf("     avx2 %d, %d\n", cr2.diff, cr2.scale);
#endif
#endif // VerifyContrast

	return 0;
}
