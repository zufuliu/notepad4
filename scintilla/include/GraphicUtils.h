// This file is part of Notepad4.
// See License.txt for details about distribution and modification.
#pragma once

// color byte order from high to lower:
// [a][b][g][r]		COLORREF/RGB(), ColourRGBA, D2D_COLOR_F
// bswap:		[r][g][b][a]
//      rotr8:	[a][r][g][b]
// rotl8:		[b][g][r][a]
//
// [a][r][g][b]		BITMAP RGBQUAD, RGBTRIPLE
// rotl8:		[r][g][b][a]
// bswap:		[b][g][r][a]
//      rotr8:	[a][b][g][r]

// color RGB hex: #RRGGBB => 0x00RRGGBB
// RGBA hex: #RRGGBBAA
// ARGB hex: #AARRGGBB
// BGRA hex: #BBGGRRAA
// ABGR hex: #AABBGGRR

inline uint32_t ColorFromRGBQuad(uint32_t quad) noexcept {
	return rotr8(bswap32(quad));
}

inline uint32_t ColorFromRGBHex(uint32_t hex) noexcept {
	return bswap32(hex) >> 8;
}

inline uint32_t ColorFromRGBAHex(uint32_t hex) noexcept {
	return bswap32(hex);
}

inline uint32_t ColorFromARGBHex(uint32_t hex) noexcept {
	return rotr8(bswap32(hex));
}

inline uint32_t ColorFromBGRAHex(uint32_t hex) noexcept {
	return rotr8(hex);
}

inline uint32_t ColorToRGBQuad(uint32_t color) noexcept {
	return rotr8(bswap32(color));
}

inline uint32_t ColorToRGBHex(uint32_t color) noexcept {
	return bswap32(color) >> 8;
}

inline uint32_t ColorToRGBAHex(uint32_t color) noexcept {
	return bswap32(color);
}

inline uint32_t ColorToARGBHex(uint32_t color) noexcept {
	return rotr8(bswap32(color));
}

inline uint32_t ColorToBGRAHex(uint32_t color) noexcept {
	return rotl8(color);
}

// see https://docs.microsoft.com/en-us/windows/win32/uxguide/vis-icons#size-requirements
// we can process 4 pixels at a time for all our bitmap (even after DPI scaling).

#if NP2_USE_SSE2
inline __m128i mm_div_epu16_by_255(__m128i i16x8) noexcept {
	// uint16_t value / 255  => (value * 0x8081) >> (16 + 7)
	i16x8 = _mm_mulhi_epu16(i16x8, _mm_set1_epi16(-0x8000 | 0x81));
	return _mm_srli_epi16(i16x8, 7);
}

inline __m128i mm_divlo_epu16_by_255(__m128i i32x4) noexcept {
	// uint16_t value / 255  => (value * 0x8081) >> (16 + 7)
	i32x4 = _mm_mulhi_epu16(i32x4, _mm_set1_epi32(0x8081));
	return _mm_srli_epi32(i32x4, 7);
}

inline __m128i mm_setlo_alpha_epi16(uint32_t alpha) noexcept {
	return _mm_shufflelo_epi16(_mm_cvtsi32_si128(alpha), 0);
}

inline __m128i mm_xor_alpha_epi16(__m128i alpha) noexcept {
	// 255 - alpha => 255 ^ alpha
	return _mm_xor_si128(_mm_set1_epi16(255), alpha);
}

inline __m128i mm_xor_alpha_epi32(__m128i alpha) noexcept {
	// 255 - alpha => 255 ^ alpha
	return _mm_xor_si128(_mm_set1_epi32(255), alpha);
}

inline __m128i mm_alpha_blend_epi16(__m128i fore, __m128i back, __m128i alpha) noexcept {
	// (fore*alpha + back*(255 - alpha)) / 255
	fore = _mm_mullo_epi16(fore, alpha);
	back = _mm_mullo_epi16(back, mm_xor_alpha_epi16(alpha));
	fore = _mm_add_epi16(fore, back);
	return mm_div_epu16_by_255(fore);
}

inline __m128i mm_alpha_blend_epi32(__m128i fore, __m128i back, __m128i alpha) noexcept {
	// (fore*alpha + back*(255 - alpha)) / 255
	fore = _mm_mullo_epi16(fore, alpha);
	back = _mm_mullo_epi16(back, mm_xor_alpha_epi32(alpha));
	fore = _mm_add_epi32(fore, back);
	return mm_divlo_epu16_by_255(fore);
}


inline __m128i unpacklo_color_epi16_sse2_si32(__m128i i32x4) noexcept {
	return _mm_unpacklo_epi8(i32x4, _mm_setzero_si128());
}

inline __m128i unpackhi_color_epi16_sse2_si128(__m128i i32x4) noexcept {
	return _mm_unpackhi_epi8(i32x4, _mm_setzero_si128());
}

inline __m128i unpack_color_epi16_sse2_si32(uint32_t color) noexcept {
	return _mm_unpacklo_epi8(_mm_cvtsi32_si128(color), _mm_setzero_si128());
}

inline __m128i unpack_color_epi16_sse2_ptr32(const uint32_t *color) noexcept {
	return _mm_unpacklo_epi8(_mm_cvtsi32_si128(*color), _mm_setzero_si128());
}

inline __m128i unpack_color_epi32_sse2_si32(uint32_t color) noexcept {
	__m128i i32x4 = _mm_cvtsi32_si128(color);
	i32x4 = _mm_unpacklo_epi8(i32x4, _mm_setzero_si128());
	i32x4 = _mm_unpacklo_epi8(i32x4, _mm_setzero_si128());
	return i32x4;
}

inline __m128i pack_color_epi16_sse2_si128(__m128i i16x8) noexcept {
	return _mm_packus_epi16(i16x8, i16x8);
}

inline uint32_t pack_color_epi16_sse2_si32(__m128i i16x4) noexcept {
	return _mm_cvtsi128_si32(_mm_packus_epi16(i16x4, i16x4));
}

#if defined(_WIN64)
inline uint64_t pack_color_epi16_sse2_si64(__m128i i16x8) noexcept {
	return _mm_cvtsi128_si64(_mm_packus_epi16(i16x8, i16x8));
}
#endif

inline __m128i pack_color_epi32_sse2_si128(__m128i i32x4) noexcept {
	i32x4 = _mm_packus_epi16(i32x4, i32x4);
	i32x4 = _mm_packus_epi16(i32x4, i32x4);
	return i32x4;
}

inline uint32_t pack_color_epi32_sse2_si32(__m128i i32x4) noexcept {
	return _mm_cvtsi128_si32(pack_color_epi32_sse2_si128(i32x4));
}


inline __m128i rgba_to_abgr_epi16_sse2_si32(uint32_t color) noexcept {
	return unpack_color_epi16_sse2_si32(bswap32(color));
}

inline __m128i rgba_to_abgr_epi32_sse2_si32(uint32_t color) noexcept {
	return unpack_color_epi32_sse2_si32(bswap32(color));
}

inline uint32_t bgr_from_abgr_epi16_sse2_si32(__m128i i16x4) noexcept {
	return pack_color_epi16_sse2_si32(i16x4) >> 8;
}

inline uint32_t bgr_from_abgr_epi32_sse2_si32(__m128i i32x4) noexcept {
	return pack_color_epi32_sse2_si32(i32x4) >> 8;
}

inline __m128i rgba_to_bgra_epi16_sse2_si32(uint32_t color) noexcept {
	const __m128i i32x4 = unpack_color_epi16_sse2_si32(color);
	return _mm_shufflelo_epi16(i32x4, _MM_SHUFFLE(3, 0, 1, 2));
}

inline __m128i rgba_to_bgra_epi32_sse2_si32(uint32_t color) noexcept {
	const __m128i i32x4 = unpack_color_epi32_sse2_si32(color);
	return _mm_shuffle_epi32(i32x4, _MM_SHUFFLE(3, 0, 1, 2));
}

inline uint32_t bgr_from_bgra_epi16_sse2_si32(__m128i i16x4) noexcept {
	return pack_color_epi16_sse2_si32(i16x4) & 0x00ffffff;
}

inline uint32_t bgr_from_bgra_epi32_sse2_si32(__m128i i32x4) noexcept {
	return pack_color_epi32_sse2_si32(i32x4) & 0x00ffffff;
}
#endif // NP2_USE_SSE2

#if NP2_USE_AVX2
#define pshufb_1to2(n)	((-0x8000) | (n))
#define pshufb_1to4(n)	(0x80808000 | (n))

#define mm_alignr_ps(a, b, imm8) \
	_mm_castsi128_ps(_mm_alignr_epi8(_mm_castps_si128(a), _mm_castps_si128(b), (imm8)*4))

inline int mm_hadd_epi32_si32(__m128i i32x4) noexcept {
	i32x4 = _mm_hadd_epi32(i32x4, i32x4);
	i32x4 = _mm_hadd_epi32(i32x4, i32x4);
	return _mm_cvtsi128_si32(i32x4);
}

inline __m256i mm256_div_epu16_by_255(__m256i i16x16, __m256i i16x16_0x8081) noexcept {
	return _mm256_srli_epi16(_mm256_mulhi_epu16(i16x16, i16x16_0x8081), 7);
}


inline __m128i unpack_color_epi16_sse4_si32(uint32_t color) noexcept {
	return _mm_cvtepu8_epi16(_mm_cvtsi32_si128(color));
}

inline __m128i unpack_color_epi16_sse4_ptr32(const uint32_t *color) noexcept {
	return _mm_cvtepu8_epi16(*(reinterpret_cast<const __m128i *>(color)));
}

inline __m128i unpack_color_epi16_sse4_ptr64(const uint64_t *color) noexcept {
	return _mm_cvtepu8_epi16(*(reinterpret_cast<const __m128i *>(color)));
}

inline __m256i unpack_color_epi16_avx2_ptr128(const __m128i *color) noexcept {
	return _mm256_cvtepu8_epi16(*color);
}

inline __m128i unpack_color_epi32_sse4_si32(uint32_t color) noexcept {
	return _mm_cvtepu8_epi32(_mm_cvtsi32_si128(color));
}

inline __m256i pack_color_epi16_avx2_si256(__m256i i16x16) noexcept {
	return _mm256_permute4x64_epi64(_mm256_packus_epi16(i16x16, i16x16), 8);
}


inline __m128i rgba_to_abgr_epi16_sse4_si32(uint32_t color) noexcept {
	return unpack_color_epi16_sse4_si32(bswap32(color));
}

inline __m128i rgba_to_abgr_epi32_sse4_si32(uint32_t color) noexcept {
	return unpack_color_epi32_sse4_si32(bswap32(color));
}

inline __m128i rgba_to_bgra_epi16_sse4_si32(uint32_t color) noexcept {
#if 1
	const __m128i i16x4 = unpack_color_epi16_sse4_si32(color);
	return _mm_shufflelo_epi16(i16x4, _MM_SHUFFLE(3, 0, 1, 2));
#else
	const __m128i i16x4 = _mm_cvtsi32_si128(color);
	return _mm_shuffle_epi8(i16x4, _mm_setr_epi16(pshufb_1to2(2), pshufb_1to2(1), pshufb_1to2(0), pshufb_1to2(3), pshufb_1to2(2), pshufb_1to2(1), pshufb_1to2(0), pshufb_1to2(3)));
#endif
}

inline __m128i rgba_to_bgra_epi16x8_sse4_si32(uint32_t color) noexcept {
#if 1
	const __m128i i16x4 = unpack_color_epi16_sse4_si32(color);
	//return _mm_shuffle_epi32(_mm_shufflelo_epi16(i16x4, _MM_SHUFFLE(3, 0, 1, 2)), 0x44);
	return _mm_broadcastq_epi64(_mm_shufflelo_epi16(i16x4, _MM_SHUFFLE(3, 0, 1, 2)));
#else
	const __m128i i16x4 = _mm_cvtsi32_si128(color);
	return _mm_shuffle_epi8(i16x4, _mm_setr_epi16(pshufb_1to2(2), pshufb_1to2(1), pshufb_1to2(0), pshufb_1to2(3), pshufb_1to2(2), pshufb_1to2(1), pshufb_1to2(0), pshufb_1to2(3)));
#endif
}

inline __m128i rgba_to_bgra_epi32_sse4_si32(uint32_t color) noexcept {
#if 1
	const __m128i i32x4 = unpack_color_epi32_sse4_si32(color);
	return _mm_shuffle_epi32(i32x4, _MM_SHUFFLE(3, 0, 1, 2));
#else
	const __m128i i32x4 = _mm_cvtsi32_si128(color);
	return _mm_shuffle_epi8(i32x4, _mm_setr_epi32(pshufb_1to4(2), pshufb_1to4(1), pshufb_1to4(0), pshufb_1to4(3)));
#endif
}
#endif // NP2_USE_AVX2

#if NP2_USE_AVX512
inline __m512i mm512_div_epu16_by_255(__m512i i16x32, __m512i i16x32_0x8081) noexcept {
	return _mm512_srli_epi16(_mm512_mulhi_epu16(i16x32, i16x32_0x8081), 7);
}

inline __m512i unpack_color_epi16_avx512_ptr256(const __m256i *color) noexcept {
	return _mm512_cvtepu8_epi16(*color);
}

inline __m512i pack_color_epi16_avx512_si512(__m512i i16x32) noexcept {
	i16x32 = _mm512_packus_epi16(i16x32, i16x32);
	__m256i i16x16 = _mm256_unpacklo_epi64(_mm512_castsi512_si256(i16x32), _mm512_extracti64x4_epi64(i16x32, 1));
	i16x16 = _mm256_permute4x64_epi64(i16x16, 0b11011000);
	return _mm512_castsi256_si512(i16x16);
}
#endif // NP2_USE_AVX512
