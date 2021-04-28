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
	constexpr uint32_t RGBAValue() const noexcept {
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
#if 0
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
#endif
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
ColourAlpha MixedWith_sse4(ColourAlpha colour, ColourAlpha other) noexcept {
	__m128i i16x4Color = unpack_color_epi16_sse4_si32(colour.RGBAValue());
	__m128i i16x4Other = unpack_color_epi16_sse4_si32(other.RGBAValue());
	i16x4Color = _mm_add_epi16(i16x4Color, i16x4Other);
	i16x4Color = _mm_srli_epi16(i16x4Color, 1);
	const uint32_t color = pack_color_epi16_sse2_si32(i16x4Color);
	return ColourAlpha(color);
}
#endif

ColourAlpha MixedWith_sse2(ColourAlpha colour, ColourAlpha other) noexcept {
	__m128i i16x4Color = unpack_color_epi16_sse2_si32(colour.RGBAValue());
	__m128i i16x4Other = unpack_color_epi16_sse2_si32(other.RGBAValue());
	i16x4Color = _mm_add_epi16(i16x4Color, i16x4Other);
	i16x4Color = _mm_srli_epi16(i16x4Color, 1);
	const uint32_t color = pack_color_epi16_sse2_si32(i16x4Color);
	return ColourAlpha(color);
}
#endif // MixedWith

#if 0 // AlphaBlend
ColourAlpha AlphaBlend(ColourAlpha fore, ColourAlpha back, unsigned int alpha) noexcept {
	const unsigned int red = (fore.GetRed()*alpha + back.GetRed()*(255 ^ alpha)) / 255;
	const unsigned int green = (fore.GetGreen()*alpha + back.GetGreen()*(255 ^ alpha)) / 255;
	const unsigned int blue = (fore.GetBlue()*alpha + back.GetBlue()*(255 ^ alpha)) / 255;
	return ColourAlpha(red, green, blue);
}
#if NP2_USE_AVX2
ColourAlpha AlphaBlend_sse4(ColourAlpha fore, ColourAlpha back, unsigned int alpha) noexcept {
	__m128i i16x4Fore = unpack_color_epi16_sse4_si32(fore.RGBAValue());
	__m128i i16x4Back = unpack_color_epi16_sse4_si32(back.RGBAValue());
	__m128i i16x4Alpha = mm_setlo_alpha_epi16(alpha);
	i16x4Fore = mm_alpha_blend_epi16(i16x4Fore, i16x4Back, i16x4Alpha);
	const uint32_t color = pack_color_epi16_sse2_si32(i16x4Fore);
	return ColourAlpha(color);
}
#endif
ColourAlpha AlphaBlend_sse2(ColourAlpha fore, ColourAlpha back, unsigned int alpha) noexcept {
	__m128i i16x4Fore = unpack_color_epi16_sse2_si32(fore.RGBAValue());
	__m128i i16x4Back = unpack_color_epi16_sse2_si32(back.RGBAValue());
	__m128i i16x4Alpha = mm_setlo_alpha_epi16(alpha);
	i16x4Fore = mm_alpha_blend_epi16(i16x4Fore, i16x4Back, i16x4Alpha);
	const uint32_t color = pack_color_epi16_sse2_si32(i16x4Fore);
	return ColourAlpha(color);
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
uint32_t RGBQuadMultiplied_sse4_blend16(ColourAlpha colour) noexcept {
	__m128i i16x4Color = rgba_to_bgra_epi16_sse4_si32(colour.RGBAValue());
	__m128i i16x4Alpha = _mm_shufflelo_epi16(i16x4Color, 0xff);
	i16x4Color = _mm_mullo_epi16(i16x4Color, i16x4Alpha);
	i16x4Color = mm_div_epu16_by_255(i16x4Color);
	i16x4Color = _mm_blend_epi16(i16x4Alpha, i16x4Color, 7);
	return pack_color_epi16_sse2_si32(i16x4Color);
}

uint32_t RGBQuadMultiplied_sse4_align32(ColourAlpha colour) noexcept {
	__m128i i32x4Color = rgba_to_abgr_epi32_sse4_si32(colour.RGBAValue());
	//__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Color, 0);
	__m128i i32x4Alpha = _mm_broadcastd_epi32(i32x4Color);
	i32x4Color = _mm_mullo_epi16(i32x4Color, i32x4Alpha);
	i32x4Color = mm_divlo_epu16_by_255(i32x4Color);
	i32x4Color = _mm_alignr_epi8(i32x4Alpha, i32x4Color, 4);
	return pack_color_epi32_sse2_si32(i32x4Color);
}

uint32_t RGBQuadMultiplied_sse4_blend32(ColourAlpha colour) noexcept {
	__m128i i32x4Color = rgba_to_bgra_epi32_sse4_si32(colour.RGBAValue());
	__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Color, 0xff);
	i32x4Color = _mm_mullo_epi16(i32x4Color, i32x4Alpha);
	i32x4Color = mm_divlo_epu16_by_255(i32x4Color);
	//i32x4Color = _mm_blend_epi16(i32x4Alpha, i32x4Color, 0x3f);
	i32x4Color = _mm_blend_epi32(i32x4Alpha, i32x4Color, 7);
	return pack_color_epi32_sse2_si32(i32x4Color);
}
#endif
uint32_t RGBQuadMultiplied_sse2(ColourAlpha colour) noexcept {
	const uint32_t rgba = bswap32(colour.RGBAValue());
	__m128i i16x4Color = unpack_color_epi16_sse2_si32(rgba);
	__m128i i16x4Alpha = _mm_shufflelo_epi16(i16x4Color, 0);
	i16x4Color = _mm_mullo_epi16(i16x4Color, i16x4Alpha);
	i16x4Color = mm_div_epu16_by_255(i16x4Color);

	const uint32_t color = bgr_from_abgr_epi16_sse2_si32(i16x4Color);
	return color | (rgba << 24);
}
#endif // RGBQuadMultiplied

#if 0 // BGRAFromRGBA
void BGRAFromRGBA(uint8_t *pixelsBGRA, const uint8_t *pixelsRGBA, size_t count) noexcept {
	constexpr size_t bytesPerPixel = 4;
	for (size_t i = 0; i < count; i++) {
		const uint8_t alpha = pixelsRGBA[3];
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
void BGRAFromRGBA_sse4_2x1(void *pixelsBGRA, const void *pixelsRGBA, size_t count) noexcept {
	constexpr size_t bytesPerPixel = 4;
	count /= (bytesPerPixel * 2);
	uint64_t *pbgra = static_cast<uint64_t *>(pixelsBGRA);
	const uint64_t *prgba = static_cast<const uint64_t *>(pixelsRGBA);
	for (size_t i = 0; i < count; i++, pbgra++) {
		__m128i i16x8Color = unpack_color_epi16_sse4_ptr64(prgba++);
		i16x8Color = _mm_shufflehi_epi16(_mm_shufflelo_epi16(i16x8Color, 0xc6), 0xc6);
		__m128i i16x8Alpha = _mm_shufflehi_epi16(_mm_shufflelo_epi16(i16x8Color, 0xff), 0xff);

		i16x8Color = _mm_mullo_epi16(i16x8Color, i16x8Alpha);
		i16x8Color = mm_div_epu16_by_255(i16x8Color);
		i16x8Color = _mm_blend_epi16(i16x8Alpha, i16x8Color, 0x77);

		i16x8Color = pack_color_epi16_sse2_si128(i16x8Color);
		_mm_storel_epi64((__m128i *)pbgra, i16x8Color);
	}
}

void BGRAFromRGBA_sse4_align32(void *pixelsBGRA, const void *pixelsRGBA, size_t count) noexcept {
	constexpr size_t bytesPerPixel = 4;
	count /= bytesPerPixel;
	uint32_t *pbgra = static_cast<uint32_t *>(pixelsBGRA);
	const uint32_t *prgba = static_cast<const uint32_t *>(pixelsRGBA);
	for (size_t i = 0; i < count; i++, pbgra++) {
		//__m128i i32x4Color = unpack_color_epi32_sse4_si32(bswap32(*prgba++));
		//__m128i i32x4Alpha = _mm_shuffle_epi32(i32x4Color, 0);
		__m128i i32x4Color = unpack_color_epi32_sse4_si32(loadbe_u32(prgba++));
		__m128i i32x4Alpha = _mm_broadcastd_epi32(i32x4Color);
		i32x4Color = _mm_mullo_epi16(i32x4Color, i32x4Alpha);
		i32x4Color = mm_divlo_epu16_by_255(i32x4Color);
		i32x4Color = _mm_alignr_epi8(i32x4Alpha, i32x4Color, 4);

		i32x4Color = pack_color_epi32_sse2_si128(i32x4Color);
		mm_storeu_si32(pbgra, i32x4Color);
	}
}
#endif
void BGRAFromRGBA_sse2_1x2(void *pixelsBGRA, const void *pixelsRGBA, size_t count) noexcept {
	constexpr size_t bytesPerPixel = 4;
	count /= (bytesPerPixel * 2);
	uint64_t *pbgra = static_cast<uint64_t *>(pixelsBGRA);
	const uint32_t *prgba = static_cast<const uint32_t *>(pixelsRGBA);
	for (size_t i = 0; i < count; i++, pbgra++) {
		__m128i i16x8Color = rgba_to_bgra_epi16_sse2_si32(*prgba++);
		__m128i color2 = rgba_to_bgra_epi16_sse2_si32(*prgba++);
		__m128i i16x8Alpha = _mm_shufflelo_epi16(i16x8Color, 0xff);

		i16x8Color = _mm_unpacklo_epi64(i16x8Color, color2);
		i16x8Alpha = _mm_unpacklo_epi64(i16x8Alpha, _mm_shufflelo_epi16(color2, 0xff));
		i16x8Color = _mm_mullo_epi16(i16x8Color, i16x8Alpha);
		i16x8Color = mm_div_epu16_by_255(i16x8Color);

		i16x8Alpha = _mm_and_si128(_mm_set_epi32(0x00ff0000, 0, 0x00ff0000, 0), i16x8Alpha);
		i16x8Color = _mm_andnot_si128(_mm_set_epi32(0x00ff0000, 0, 0x00ff0000, 0), i16x8Color);
		i16x8Color = _mm_or_si128(i16x8Alpha, i16x8Color);
		i16x8Color = pack_color_epi16_sse2_si128(i16x8Color);
		_mm_storel_epi64((__m128i *)pbgra, i16x8Color);
	}
}

void BGRAFromRGBA_sse2_1x1(void *pixelsBGRA, const void *pixelsRGBA, size_t count) noexcept {
	constexpr size_t bytesPerPixel = 4;
	count /= bytesPerPixel;
	uint32_t *pbgra = static_cast<uint32_t *>(pixelsBGRA);
	const uint32_t *prgba = static_cast<const uint32_t *>(pixelsRGBA);
	for (size_t i = 0; i < count; i++, pbgra++) {
		const uint32_t rgba = bswap32(*prgba++);
		__m128i i16x4Color = unpack_color_epi16_sse2_si32(rgba);
		__m128i i16x4Alpha = _mm_shufflelo_epi16(i16x4Color, 0);
		i16x4Color = _mm_mullo_epi16(i16x4Color, i16x4Alpha);
		i16x4Color = mm_div_epu16_by_255(i16x4Color);

		const uint32_t color = bgr_from_abgr_epi16_sse2_si32(i16x4Color);
		*pbgra = (color | (rgba << 24));
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
uint32_t Proportional_sse4_align(ColourAlpha a, ColourAlpha b, double t) noexcept {
	__m128i i32x4Fore = rgba_to_abgr_epi32_sse4_si32(a.RGBAValue());
	__m128i i32x4Back = rgba_to_abgr_epi32_sse4_si32(b.RGBAValue());
	// a + t * (b - a)
	__m128 f32x4Fore = _mm_cvtepi32_ps(_mm_sub_epi32(i32x4Back, i32x4Fore));
	f32x4Fore = _mm_mul_ps(f32x4Fore, _mm_set1_ps((float)t));
	f32x4Fore = _mm_add_ps(f32x4Fore, _mm_cvtepi32_ps(i32x4Fore));
	// component * alpha / 255
	//__m128 f32x4Alpha = _mm_shuffle_ps(f32x4Fore, f32x4Fore, 0);
	__m128 f32x4Alpha = _mm_broadcastss_ps(f32x4Fore);
	f32x4Fore = _mm_mul_ps(f32x4Fore, f32x4Alpha);
	f32x4Fore = _mm_div_ps(f32x4Fore, _mm_set1_ps(255.0f));
	f32x4Fore = mm_alignr_ps(f32x4Alpha, f32x4Fore, 1);

	i32x4Fore = _mm_cvttps_epi32(f32x4Fore);
	return pack_color_epi32_sse2_si32(i32x4Fore);
}

uint32_t Proportional_sse4_blend(ColourAlpha a, ColourAlpha b, double t) noexcept {
	__m128i i32x4Fore = rgba_to_bgra_epi32_sse4_si32(a.RGBAValue());
	__m128i i32x4Back = rgba_to_bgra_epi32_sse4_si32(b.RGBAValue());
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
	return pack_color_epi32_sse2_si32(i32x4Fore);
}
#endif
uint32_t Proportional_sse2(ColourAlpha a, ColourAlpha b, double t) noexcept {
	__m128i i32x4Fore = rgba_to_abgr_epi32_sse2_si32(a.RGBAValue());
	__m128i i32x4Back = rgba_to_abgr_epi32_sse2_si32(b.RGBAValue());
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
	const uint32_t color = bgr_from_abgr_epi32_sse2_si32(i32x4Fore);
	return color | (alpha << 24);
}
#endif // Proportional

#if 0 // BitmapMergeAlpha
uint32_t BitmapMergeAlpha(ColourAlpha fore, ColourAlpha back) noexcept {
	unsigned int alpha = fore.GetAlpha();
	const unsigned int red = (fore.GetRed()*alpha + back.GetRed()*(255 ^ alpha)) / 255;
	const unsigned int green = (fore.GetGreen()*alpha + back.GetGreen()*(255 ^ alpha)) / 255;
	const unsigned int blue = (fore.GetBlue()*alpha + back.GetBlue()*(255 ^ alpha)) / 255;
	return ColourAlpha(red, green, blue, 0xff).RGBAValue();
}
#if NP2_USE_AVX2
uint32_t BitmapMergeAlpha_sse4(const uint32_t *fore, COLORREF back) noexcept {
	__m128i i16x4Fore = unpack_color_epi16_sse4_ptr32(fore);
	__m128i i16x4Back = rgba_to_bgra_epi16_sse4_si32(back);
	__m128i i16x4Alpha = _mm_shufflelo_epi16(i16x4Fore, 0xff);
	i16x4Fore = mm_alpha_blend_epi16(i16x4Fore, i16x4Back, i16x4Alpha);
	const uint32_t color = pack_color_epi16_sse2_si32(i16x4Fore);
	return color | 0xff000000U;
}

uint64_t BitmapMergeAlpha_sse4_x2(const uint64_t *fore, COLORREF back) noexcept {
	__m128i i16x8Fore = unpack_color_epi16_sse4_ptr64(fore);
	__m128i i16x8Back = rgba_to_bgra_epi16x8_sse4_si32(back);
	__m128i i16x8Alpha = _mm_shufflehi_epi16(_mm_shufflelo_epi16(i16x8Fore, 0xff), 0xff);
	i16x8Fore = mm_alpha_blend_epi16(i16x8Fore, i16x8Back, i16x8Alpha);
	const uint64_t color = pack_color_epi16_sse2_si64(i16x8Fore);
	return color | UINT64_C(0xff000000ff000000);
}
#endif
uint32_t BitmapMergeAlpha_sse2(uint32_t fore, COLORREF back) noexcept {
	__m128i i16x4Fore = unpack_color_epi16_sse2_si32(fore);
	__m128i i16x4Back = rgba_to_bgra_epi16_sse2_si32(back);
	__m128i i16x4Alpha = _mm_shufflelo_epi16(i16x4Fore, 0xff);
	i16x4Fore = mm_alpha_blend_epi16(i16x4Fore, i16x4Back, i16x4Alpha);
	const uint32_t color = pack_color_epi16_sse2_si32(i16x4Fore);
	return color | 0xff000000U;
}
#endif // BitmapMergeAlpha

#if 0 // BitmapAlphaBlend
uint32_t BitmapAlphaBlend(ColourAlpha fore, ColourAlpha back, BYTE alpha) noexcept {
	const unsigned int red = (fore.GetRed()*alpha + back.GetRed()*(255 ^ alpha)) / 255;
	const unsigned int green = (fore.GetGreen()*alpha + back.GetGreen()*(255 ^ alpha)) / 255;
	const unsigned int blue = (fore.GetBlue()*alpha + back.GetBlue()*(255 ^ alpha)) / 255;
	return ColourAlpha(red, green, blue, fore.GetAlpha()).RGBAValue();
}
#if NP2_USE_AVX2
uint32_t BitmapAlphaBlend_sse4(const uint32_t *fore, COLORREF back, BYTE alpha) noexcept {
	const __m128i origin = unpack_color_epi16_sse4_ptr32(fore);
	__m128i i16x4Back = rgba_to_bgra_epi16_sse4_si32(back);
	__m128i i16x4Alpha = mm_setlo_alpha_epi16(alpha);
	__m128i i16x4Fore = mm_alpha_blend_epi16(origin, i16x4Back, i16x4Alpha);
	i16x4Fore = _mm_blend_epi16(origin, i16x4Fore, 7);
	return pack_color_epi16_sse2_si32(i16x4Fore);
}

uint64_t BitmapAlphaBlend_sse4_x2(const uint64_t *fore, COLORREF back, BYTE alpha) noexcept {
	const __m128i origin = unpack_color_epi16_sse4_ptr64(fore);
	__m128i i16x8Back = rgba_to_bgra_epi16x8_sse4_si32(back);
	//__m128i i16x8Alpha = _mm_shuffle_epi32(mm_setlo_alpha_epi16(alpha), 0x44);
	__m128i i16x8Alpha = _mm_broadcastw_epi16(mm_setlo_epi32(alpha));
	__m128i i16x8Fore = mm_alpha_blend_epi16(origin, i16x8Back, i16x8Alpha);
	i16x8Fore = _mm_blend_epi16(origin, i16x8Fore, 0x77);
	return pack_color_epi16_sse2_si64(i16x8Fore);
}
#endif
uint32_t BitmapAlphaBlend_sse2(uint32_t fore, COLORREF back, BYTE alpha) noexcept {
	__m128i i16x4Fore = unpack_color_epi16_sse2_si32(fore);
	__m128i i16x4Back = rgba_to_bgra_epi16_sse2_si32(back);
	__m128i i16x4Alpha = mm_setlo_alpha_epi16(alpha);
	i16x4Fore = mm_alpha_blend_epi16(i16x4Fore, i16x4Back, i16x4Alpha);
	const uint32_t color = bgr_from_bgra_epi16_sse2_si32(i16x4Fore);
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

		const __m128i i16x4Alpha = mm_setlo_alpha_epi16(alpha);
		__m128i i16x4Back = rgba_to_bgra_epi16_sse2_si32(crDest);
		i16x4Back = _mm_mullo_epi16(i16x4Back, mm_xor_alpha_epi16(i16x4Alpha));
		for (size_t x = 0; x < pixelCount; x++, prgba++) {
			const uint32_t origin = *prgba;
			__m128i i16x4Fore = unpack_color_epi16_sse2_si32(origin);
			i16x4Fore = _mm_mullo_epi16(i16x4Fore, i16x4Alpha);
			i16x4Fore = _mm_add_epi32(i16x4Fore, i16x4Back);
			i16x4Fore = mm_div_epu16_by_255(i16x4Fore);
			uint32_t color = bgr_from_bgra_epi16_sse2_si32(i16x4Fore);
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

		const __m128i i16x8Alpha = _mm_shuffle_epi32(mm_setlo_alpha_epi16(alpha), 0x44);
		__m128i i16x8Back = _mm_shuffle_epi32(rgba_to_bgra_epi16_sse2_si32(crDest), 0x44);
		i16x8Back = _mm_mullo_epi16(i16x8Back, mm_xor_alpha_epi16(i16x8Alpha));
		for (ULONG x = offset; x < pixelCount; x += 4, prgba++, dest++) {
			__m128i origin = _mm_loadu_si128(prgba);
			__m128i color42 = _mm_shuffle_epi32(origin, 0x31);

			__m128i i16x8Fore = unpacklo_color_epi16_sse2_si32(origin);
			i16x8Fore = _mm_unpacklo_epi64(i16x8Fore, unpacklo_color_epi16_sse2_si32(color42));
			i16x8Fore = _mm_mullo_epi16(i16x8Fore, i16x8Alpha);
			i16x8Fore = _mm_add_epi16(i16x8Fore, i16x8Back);
			i16x8Fore = mm_div_epu16_by_255(i16x8Fore);
			__m128i i32x4Fore = pack_color_epi16_sse2_si128(i16x8Fore);

			i16x8Fore = unpackhi_color_epi16_sse2_si128(origin);
			i16x8Fore = _mm_unpacklo_epi64(i16x8Fore, unpackhi_color_epi16_sse2_si128(color42));
			i16x8Fore = _mm_mullo_epi16(i16x8Fore, i16x8Alpha);
			i16x8Fore = _mm_add_epi16(i16x8Fore, i16x8Back);
			i16x8Fore = mm_div_epu16_by_255(i16x8Fore);
			color42 = pack_color_epi16_sse2_si128(i16x8Fore);

			i32x4Fore = _mm_unpacklo_epi64(i32x4Fore, color42);
			i32x4Fore = _mm_and_si128(_mm_set1_epi32(0x00ffffff), i32x4Fore);
			origin = _mm_andnot_si128(_mm_set1_epi32(0x00ffffff), origin);
			i32x4Fore = _mm_or_si128(origin, i32x4Fore);
			_mm_storeu_si128(dest, i32x4Fore);
		}
		for (size_t x = offset; x < pixelCount; x++) {
			if (scale[x] != xmm[x]) {
				printf("sse2 1x4 fail at %4zu %08x, %08x %08x\n", x, data[x], scale[x], xmm[x]);
				break;
			}
		}
	}
#if NP2_USE_AVX2
	{ // sse4 1x1
		std::vector<uint32_t> xmm;
		xmm.resize(pixelCount);
		const uint32_t *prgba = data.data();
		uint32_t *dest = xmm.data();

		const __m128i i16x4Alpha = mm_setlo_alpha_epi16(alpha);
		__m128i i16x4Back = rgba_to_bgra_epi16_sse4_si32(crDest);
		i16x4Back = _mm_mullo_epi16(i16x4Back, mm_xor_alpha_epi16(i16x4Alpha));
		for (size_t x = 0; x < pixelCount; x++, prgba++, dest++) {
			const __m128i origin = unpack_color_epi16_sse4_ptr32(prgba);
			__m128i i32x4Fore = _mm_mullo_epi16(origin, i16x4Alpha);
			i32x4Fore = _mm_add_epi16(i32x4Fore, i16x4Back);
			i32x4Fore = mm_div_epu16_by_255(i32x4Fore);
			i32x4Fore = _mm_blend_epi16(origin, i32x4Fore, 7);
			i32x4Fore = pack_color_epi16_sse2_si128(i32x4Fore);
			mm_storeu_si32(dest, i32x4Fore);
		}
		for (size_t x = 0; x < pixelCount; x++) {
			if (scale[x] != xmm[x]) {
				printf("sse4 1x1 fail %4zu %08x, %08x %08x\n", x, data[x], scale[x], xmm[x]);
				break;
			}
		}
	}
	{ // sse4 2x1
		std::vector<uint32_t> xmm;
		xmm.resize(pixelCount);
		const uint64_t *prgba = (uint64_t *)data.data();
		uint64_t *dest = (uint64_t *)xmm.data();

		//__m128i i16x8Alpha = _mm_shuffle_epi32(mm_setlo_alpha_epi16(alpha), 0x44);
		__m128i i16x8Alpha = _mm_broadcastw_epi16(mm_setlo_epi32(alpha));
		__m128i i16x8Back = rgba_to_bgra_epi16x8_sse4_si32(crDest);
		i16x8Back = _mm_mullo_epi16(i16x8Back, mm_xor_alpha_epi16(i16x8Alpha));
		for (size_t x = 0; x < pixelCount; x += 2, prgba++, dest++) {
			const __m128i origin = unpack_color_epi16_sse4_ptr64(prgba);
			__m128i i16x8Fore = _mm_mullo_epi16(origin, i16x8Alpha);
			i16x8Fore = _mm_add_epi16(i16x8Fore, i16x8Back);
			i16x8Fore = mm_div_epu16_by_255(i16x8Fore);
			i16x8Fore = _mm_blend_epi16(origin, i16x8Fore, 0x77);
			i16x8Fore = pack_color_epi16_sse2_si128(i16x8Fore);
			_mm_storel_epi64((__m128i *)dest, i16x8Fore);
		}
		for (size_t x = 0; x < pixelCount; x++) {
			if (scale[x] != xmm[x]) {
				printf("sse4 2x1 fail %4zu %08x, %08x %08x\n", x, data[x], scale[x], xmm[x]);
				break;
			}
		}
	}
	{ // avx2 4x1
		std::vector<uint32_t> xmm;
		xmm.resize(pixelCount);
		const __m128i *prgba = (__m128i *)data.data();
		__m128i *dest = (__m128i *)xmm.data();

		const __m256i i16x16Alpha = _mm256_broadcastw_epi16(mm_setlo_epi32(alpha));
		const __m256i i16x16Back = _mm256_broadcastq_epi64(_mm_mullo_epi16(rgba_to_bgra_epi16_sse4_si32(crDest), mm_xor_alpha_epi16(_mm256_castsi256_si128(i16x16Alpha))));
		const __m256i i16x16_0x8081 = _mm256_broadcastsi128_si256(_mm_set1_epi16(-0x8000 | 0x81));
		for (size_t x = 0; x < pixelCount; x += 4, prgba++, dest++) {
			const __m256i origin = _mm256_cvtepu8_epi16(*prgba);
			__m256i i16x16Fore = _mm256_mullo_epi16(origin, i16x16Alpha);
			i16x16Fore = _mm256_add_epi16(i16x16Fore, i16x16Back);
			i16x16Fore = _mm256_srli_epi16(_mm256_mulhi_epu16(i16x16Fore, i16x16_0x8081), 7);
			i16x16Fore = _mm256_blend_epi16(origin, i16x16Fore, 0x77);
			i16x16Fore = _mm256_packus_epi16(i16x16Fore, i16x16Fore);
			i16x16Fore = _mm256_permute4x64_epi64(i16x16Fore, 8);
			_mm_storeu_si128(dest, _mm256_castsi256_si128(i16x16Fore));
		}
		for (size_t x = 0; x < pixelCount; x++) {
			if (scale[x] != xmm[x]) {
				printf("avx2 4x1 fail %4zu %08x, %08x %08x\n", x, data[x], scale[x], xmm[x]);
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
	return (fore.RGBAValue() & 0xff000000U) | (gray * 0x010101);
}
#if NP2_USE_AVX2
uint32_t BitmapGrayScale_sse4(const uint32_t fore) noexcept {
	__m128i i32x4Color = unpack_color_epi32_sse4_si32(fore);
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
Contrast VerifyContrast_sse4(COLORREF cr1, COLORREF cr2) noexcept {
	__m128i i32x4Fore = unpack_color_epi32_sse4_si32(cr1);
	__m128i i32x4Back = unpack_color_epi32_sse4_si32(cr2);

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
	constexpr uint8_t alpha = 0x60;
	constexpr uint32_t back = 0x05f3f2f1;
	constexpr uint32_t quad = 0x9b5170a3;
	constexpr uint32_t quad2 = 0x8c8172b2;
	const uint32_t fore = ColorFromRGBQuad(quad);
	const uint32_t fore2 = ColorFromRGBQuad(quad2);
	constexpr DualPixel dualPixels = { { quad, quad2 } };

#if 0 // MixedWith
	printf("MixedWith(%08x, %08x):\n", fore, back);
	printf("    scale %08x\n", MixedWith(fore, back).RGBAValue());
#if NP2_USE_AVX2
	printf("     sse4 %08x\n", MixedWith_sse4(fore, back).RGBAValue());
#endif
	printf("     sse2 %08x\n", MixedWith_sse2(fore, back).RGBAValue());
#endif // MixedWith

#if 0 // AlphaBlend
	printf("AlphaBlend(%08x, %08x, %02x):\n", fore, back, alpha);
	printf("    scale %08x\n", AlphaBlend(fore, back, alpha).RGBAValue());
#if NP2_USE_AVX2
	printf("     sse4 %08x\n", AlphaBlend_sse4(fore, back, alpha).RGBAValue());
#endif
	printf("     sse2 %08x\n", AlphaBlend_sse2(fore, back, alpha).RGBAValue());
#endif // AlphaBlend

#if 0 // RGBQuadMultiplied
	printf("RGBQuadMultiplied(%08x):\n", fore);
	printf("    scale %08x\n", RGBQuadMultiplied(fore));
#if NP2_USE_AVX2
	printf("sse4 16 b %08x\n", RGBQuadMultiplied_sse4_blend16(fore));
	printf("sse4 32 a %08x\n", RGBQuadMultiplied_sse4_align32(fore));
	printf("sse4 32 b %08x\n", RGBQuadMultiplied_sse4_blend32(fore));
#endif
	printf("     sse2 %08x\n", RGBQuadMultiplied_sse2(fore));
#endif // RGBQuadMultiplied

#if 0 // BGRAFromRGBA
	const uint32_t rgba[2] = { fore, fore2 };
	uint32_t bgra[2];
	const void * const pixelsRGBA = rgba;
	void *pixelsBGRA = bgra;
	constexpr size_t pixelCount = sizeof(rgba);
	printf("BGRAFromRGBA(%08x, %08x)\n", rgba[0], rgba[1]);
	BGRAFromRGBA(static_cast<uint8_t *>(pixelsBGRA), static_cast<const uint8_t *>(pixelsRGBA), pixelCount);
	printf("   single %08x %08x\n", RGBQuadMultiplied(rgba[0]), RGBQuadMultiplied(rgba[1]));
	printf("    scale %08x %08x\n", bgra[0], bgra[1]);
#if NP2_USE_AVX2
	memset(bgra, 0, sizeof(bgra));
	BGRAFromRGBA_sse4_align32(pixelsBGRA, pixelsRGBA, pixelCount);
	printf("sse4 1x1a %08x %08x\n", bgra[0], bgra[1]);
	memset(bgra, 0, sizeof(bgra));
	BGRAFromRGBA_sse4_2x1(pixelsBGRA, pixelsRGBA, pixelCount);
	printf("sse4 2x1b %08x %08x\n", bgra[0], bgra[1]);
#endif
	memset(bgra, 0, sizeof(bgra));
	BGRAFromRGBA_sse2_1x1(pixelsBGRA, pixelsRGBA, pixelCount);
	printf(" sse2 1x1 %08x %08x\n", bgra[0], bgra[1]);
	memset(bgra, 0, sizeof(bgra));
	BGRAFromRGBA_sse2_1x2(pixelsBGRA, pixelsRGBA, pixelCount);
	printf(" sse2 1x2 %08x %08x\n", bgra[0], bgra[1]);
#endif // BGRAFromRGBA

#if 0 // Proportional
	constexpr double proportion = alpha/255.0f;
	printf("Proportional(%08x, %08x, %f):\n", fore, back, proportion);
	printf("    scale %08x\n", Proportional(ColourAlpha(fore), ColourAlpha(back), proportion));
#if NP2_USE_AVX2
	printf("   sse4 a %08x\n", Proportional_sse4_align(fore, back, proportion));
	printf("   sse4 b %08x\n", Proportional_sse4_blend(fore, back, proportion));
#endif
	printf("     sse2 %08x\n", Proportional_sse2(fore, back, proportion));
#endif // Proportional

#if 0 // BitmapMergeAlpha
	printf("BitmapMergeAlpha (%08x / %08x), (%08x / %08x), %08x:\n", quad, fore, quad2, fore2, back);
	printf("    scale %08x %08x\n", RGBQuadFromColor(BitmapMergeAlpha(fore, back)), RGBQuadFromColor(BitmapMergeAlpha(fore2, back)));
#if NP2_USE_AVX2
	printf("     sse4 %08x %08x\n", BitmapMergeAlpha_sse4(&quad, back), BitmapMergeAlpha_sse4(&quad2, back));
	{
		DualPixel two;
		two.pixel64 = BitmapMergeAlpha_sse4_x2(&dualPixels.pixel64, back);
		printf("  sse4 x2 %08x %08x\n", two.pixel32[0], two.pixel32[1]);
	}
#endif
	printf("     sse2 %08x %08x\n", BitmapMergeAlpha_sse2(quad, back), BitmapMergeAlpha_sse2(quad2, back));
#endif // BitmapMergeAlpha

#if 0 // BitmapAlphaBlend
	printf("BitmapAlphaBlend (%08x / %08x), (%08x, %08x), %08x, %02x:\n", quad, fore, quad2, fore2, back, alpha);
	printf("    scale %08x %08x\n", RGBQuadFromColor(BitmapAlphaBlend(fore, back, alpha)), RGBQuadFromColor(BitmapAlphaBlend(fore2, back, alpha)));
#if NP2_USE_AVX2
	printf("     sse4 %08x %08x\n", BitmapAlphaBlend_sse4(&quad, back, alpha), BitmapAlphaBlend_sse4(&quad2, back, alpha));
	{
		DualPixel two;
		two.pixel64 = BitmapAlphaBlend_sse4_x2(&dualPixels.pixel64, back, alpha);
		printf("  sse4 x2 %08x %08x\n", two.pixel32[0], two.pixel32[1]);
	}
#endif
	printf("     sse2 %08x %08x\n", BitmapAlphaBlend_sse2(quad, back, alpha), BitmapAlphaBlend_sse2(quad2, back, alpha));
	//TestBitmapAlphaBlend("bitmap.dat", back, alpha);
#endif // BitmapAlphaBlend

#if 0 // BitmapGrayScale
	printf("BitmapGrayScale(%08x / %08x):\n", quad, fore);
	printf("    scale %08x\n", BitmapGrayScale(fore));
#if NP2_USE_AVX2
	printf("     sse4 %08x\n", BitmapGrayScale_sse4(quad));
#endif
#endif // BitmapGrayScale

#if 0 // VerifyContrast
	const Contrast cr1 = VerifyContrast(fore & 0xffffff, back & 0xffffff);
	printf("VerifyContrast(%08x, %08x):\n", fore, back);
	printf("    scale %d, %d\n", cr1.diff, cr1.scale);
#if NP2_USE_AVX2
	const Contrast cr2 = VerifyContrast_sse4(fore & 0xffffff, back & 0xffffff);
	printf("     sse4 %d, %d\n", cr2.diff, cr2.scale);
#endif
#endif // VerifyContrast

	return 0;
}
