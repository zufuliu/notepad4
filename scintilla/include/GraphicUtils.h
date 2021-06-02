// This file is part of Notepad2.
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

static inline uint32_t ColorFromRGBQuad(uint32_t quad) NP2_noexcept {
	return rotr8(bswap32(quad));
}

static inline uint32_t ColorFromRGBHex(uint32_t hex) NP2_noexcept {
	return bswap32(hex) >> 8;
}

static inline uint32_t ColorFromRGBAHex(uint32_t hex) NP2_noexcept {
	return bswap32(hex);
}

static inline uint32_t ColorFromARGBHex(uint32_t hex) NP2_noexcept {
	return rotr8(bswap32(hex));
}

static inline uint32_t ColorToRGBQuad(uint32_t color) NP2_noexcept {
	return rotr8(bswap32(color));
}

static inline uint32_t ColorToRGBHex(uint32_t color) NP2_noexcept {
	return bswap32(color) >> 8;
}

static inline uint32_t ColorToRGBAHex(uint32_t color) NP2_noexcept {
	return bswap32(color);
}

static inline uint32_t ColorToARGBHex(uint32_t color) NP2_noexcept {
	return rotr8(bswap32(color));
}

// see https://docs.microsoft.com/en-us/windows/win32/uxguide/vis-icons#size-requirements
// we can process 4 pixels at a time for all our bitmap (even after DPI scaling).

#if NP2_USE_SSE2
static inline __m128i mm_setlo_epi32(uint32_t value) NP2_noexcept {
#if defined(__GNUC__) || defined(__clang__)
	// Visual C++ 2017 compiles this into pinsrd instead of movq for AVX2 target.
	// _mm_loadu_si32() requires GCC 11, https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99754
	return _mm_setr_epi32(value, 0, 0, 0);
#else
	return _mm_loadu_si32(&value);
#endif
}

static inline __m128i mm_loadu_si32(const uint32_t *value) NP2_noexcept {
#if defined(__GNUC__)
	return _mm_setr_epi32(*value, 0, 0, 0);
#else
	return _mm_loadu_si32(value);
#endif
}

static inline void mm_storeu_si32(uint32_t *mem_addr, __m128i a) NP2_noexcept {
#if defined(__GNUC__)
	*mem_addr = _mm_cvtsi128_si32(a);
#else
	_mm_storeu_si32(mem_addr, a);
#endif
}

static inline __m128i mm_setlo_epi64(uint64_t value) NP2_noexcept {
#if defined(__GNUC__) || defined(__clang__)
	// Visual C++ 2017 compiles this into pinsrd instead of movq for AVX2 target.
	// _mm_loadu_si64() requires GCC 9, https://gcc.gnu.org/bugzilla/show_bug.cgi?id=78782
	return _mm_set_epi64x(0, value);
#else
	return _mm_loadl_epi64((__m128i *)(&value));
#endif
}


static inline __m128i mm_div_epu16_by_255(__m128i i16x8) NP2_noexcept {
	// uint16_t value / 255  => (value * 0x8081) >> (16 + 7)
	i16x8 = _mm_mulhi_epu16(i16x8, _mm_set1_epi16(-0x8000 | 0x81));
	return _mm_srli_epi16(i16x8, 7);
}

static inline __m128i mm_divlo_epu16_by_255(__m128i i32x4) NP2_noexcept {
	// uint16_t value / 255  => (value * 0x8081) >> (16 + 7)
	i32x4 = _mm_mulhi_epu16(i32x4, _mm_set1_epi32(0x8081));
	return _mm_srli_epi32(i32x4, 7);
}

static inline __m128i mm_setlo_alpha_epi16(uint32_t alpha) NP2_noexcept {
	return _mm_shufflelo_epi16(mm_setlo_epi32(alpha), 0);
}

static inline __m128i mm_xor_alpha_epi16(__m128i alpha) NP2_noexcept {
	// 255 - alpha => 255 ^ alpha
	return _mm_xor_si128(_mm_set1_epi16(255), alpha);
}

static inline __m128i mm_xor_alpha_epi32(__m128i alpha) NP2_noexcept {
	// 255 - alpha => 255 ^ alpha
	return _mm_xor_si128(_mm_set1_epi32(255), alpha);
}

static inline __m128i mm_alpha_blend_epi16(__m128i fore, __m128i back, __m128i alpha) NP2_noexcept {
	// (fore*alpha + back*(255 - alpha)) / 255
	fore = _mm_mullo_epi16(fore, alpha);
	back = _mm_mullo_epi16(back, mm_xor_alpha_epi16(alpha));
	fore = _mm_add_epi16(fore, back);
	return mm_div_epu16_by_255(fore);
}

static inline __m128i mm_alpha_blend_epi32(__m128i fore, __m128i back, __m128i alpha) NP2_noexcept {
	// (fore*alpha + back*(255 - alpha)) / 255
	fore = _mm_mullo_epi16(fore, alpha);
	back = _mm_mullo_epi16(back, mm_xor_alpha_epi32(alpha));
	fore = _mm_add_epi32(fore, back);
	return mm_divlo_epu16_by_255(fore);
}


static inline __m128i unpacklo_color_epi16_sse2_si32(__m128i i32x4) NP2_noexcept {
	return _mm_unpacklo_epi8(i32x4, _mm_setzero_si128());
}

static inline __m128i unpackhi_color_epi16_sse2_si128(__m128i i32x4) NP2_noexcept {
	return _mm_unpackhi_epi8(i32x4, _mm_setzero_si128());
}

static inline __m128i unpack_color_epi16_sse2_si32(uint32_t color) NP2_noexcept {
	return _mm_unpacklo_epi8(mm_setlo_epi32(color), _mm_setzero_si128());
}

static inline __m128i unpack_color_epi16_sse2_ptr32(const uint32_t *color) NP2_noexcept {
	return _mm_unpacklo_epi8(mm_loadu_si32(color), _mm_setzero_si128());
}

static inline __m128i unpack_color_epi32_sse2_si32(uint32_t color) NP2_noexcept {
	__m128i i32x4 = mm_setlo_epi32(color);
	i32x4 = _mm_unpacklo_epi8(i32x4, _mm_setzero_si128());
	i32x4 = _mm_unpacklo_epi8(i32x4, _mm_setzero_si128());
	return i32x4;
}

static inline __m128i pack_color_epi16_sse2_si128(__m128i i16x8) NP2_noexcept {
	return _mm_packus_epi16(i16x8, i16x8);
}

static inline uint32_t pack_color_epi16_sse2_si32(__m128i i16x4) NP2_noexcept {
	return _mm_cvtsi128_si32(_mm_packus_epi16(i16x4, i16x4));
}

#if defined(_WIN64)
static inline uint64_t pack_color_epi16_sse2_si64(__m128i i16x8) NP2_noexcept {
	return _mm_cvtsi128_si64(_mm_packus_epi16(i16x8, i16x8));
}
#endif

static inline __m128i pack_color_epi32_sse2_si128(__m128i i32x4) NP2_noexcept {
	i32x4 = _mm_packus_epi16(i32x4, i32x4);
	i32x4 = _mm_packus_epi16(i32x4, i32x4);
	return i32x4;
}

static inline uint32_t pack_color_epi32_sse2_si32(__m128i i32x4) NP2_noexcept {
	return _mm_cvtsi128_si32(pack_color_epi32_sse2_si128(i32x4));
}


static inline __m128i rgba_to_abgr_epi16_sse2_si32(uint32_t color) NP2_noexcept {
	return unpack_color_epi16_sse2_si32(bswap32(color));
}

static inline __m128i rgba_to_abgr_epi32_sse2_si32(uint32_t color) NP2_noexcept {
	return unpack_color_epi32_sse2_si32(bswap32(color));
}

static inline uint32_t bgr_from_abgr_epi16_sse2_si32(__m128i i16x4) NP2_noexcept {
	return pack_color_epi16_sse2_si32(i16x4) >> 8;
}

static inline uint32_t bgr_from_abgr_epi32_sse2_si32(__m128i i32x4) NP2_noexcept {
	return pack_color_epi32_sse2_si32(i32x4) >> 8;
}

static inline __m128i rgba_to_bgra_epi16_sse2_si32(uint32_t color) NP2_noexcept {
	__m128i i32x4 = unpack_color_epi16_sse2_si32(color);
	return _mm_shufflelo_epi16(i32x4, _MM_SHUFFLE(3, 0, 1, 2));
}

static inline __m128i rgba_to_bgra_epi32_sse2_si32(uint32_t color) NP2_noexcept {
	__m128i i32x4 = unpack_color_epi32_sse2_si32(color);
	return _mm_shuffle_epi32(i32x4, _MM_SHUFFLE(3, 0, 1, 2));
}

static inline uint32_t bgr_from_bgra_epi16_sse2_si32(__m128i i16x4) NP2_noexcept {
	return pack_color_epi16_sse2_si32(i16x4) & 0x00ffffff;
}

static inline uint32_t bgr_from_bgra_epi32_sse2_si32(__m128i i32x4) NP2_noexcept {
	return pack_color_epi32_sse2_si32(i32x4) & 0x00ffffff;
}
#endif // NP2_USE_SSE2

#if NP2_USE_AVX2
#define pshufb_1to2(n)	((-0x8000) | (n))
#define pshufb_1to4(n)	(0x80808000 | (n))

#define mm_alignr_ps(a, b, imm8) \
	_mm_castsi128_ps(_mm_alignr_epi8(_mm_castps_si128(a), _mm_castps_si128(b), (imm8)*4))

static inline int mm_hadd_epi32_si32(__m128i i32x4) NP2_noexcept {
	i32x4 = _mm_hadd_epi32(i32x4, i32x4);
	i32x4 = _mm_hadd_epi32(i32x4, i32x4);
	return _mm_cvtsi128_si32(i32x4);
}


static inline __m128i unpack_color_epi16_sse4_si32(uint32_t color) NP2_noexcept {
	return _mm_cvtepu8_epi16(mm_setlo_epi32(color));
}

static inline __m128i unpack_color_epi16_sse4_ptr32(const uint32_t *color) NP2_noexcept {
	return _mm_cvtepu8_epi16(*((__m128i *)color));
}

static inline __m128i unpack_color_epi16_sse4_ptr64(const uint64_t *color) NP2_noexcept {
	return _mm_cvtepu8_epi16(*((__m128i *)color));
}

static inline __m128i unpack_color_epi32_sse4_si32(uint32_t color) NP2_noexcept {
	return _mm_cvtepu8_epi32(mm_setlo_epi32(color));
}


static inline __m128i rgba_to_abgr_epi16_sse4_si32(uint32_t color) NP2_noexcept {
	return unpack_color_epi16_sse4_si32(bswap32(color));
}

static inline __m128i rgba_to_abgr_epi32_sse4_si32(uint32_t color) NP2_noexcept {
	return unpack_color_epi32_sse4_si32(bswap32(color));
}

static inline __m128i rgba_to_bgra_epi16_sse4_si32(uint32_t color) NP2_noexcept {
#if 1
	__m128i i16x4 = unpack_color_epi16_sse4_si32(color);
	return _mm_shufflelo_epi16(i16x4, _MM_SHUFFLE(3, 0, 1, 2));
#else
	__m128i i16x4 = mm_setlo_epi32(color);
	return _mm_shuffle_epi8(i16x4, _mm_setr_epi16(pshufb_1to2(2), pshufb_1to2(1), pshufb_1to2(0), pshufb_1to2(3), pshufb_1to2(2), pshufb_1to2(1), pshufb_1to2(0), pshufb_1to2(3)));
#endif
}

static inline __m128i rgba_to_bgra_epi16x8_sse4_si32(uint32_t color) NP2_noexcept {
#if 1
	__m128i i16x4 = unpack_color_epi16_sse4_si32(color);
	//return _mm_shuffle_epi32(_mm_shufflelo_epi16(i16x4, _MM_SHUFFLE(3, 0, 1, 2)), 0x44);
	return _mm_broadcastq_epi64(_mm_shufflelo_epi16(i16x4, _MM_SHUFFLE(3, 0, 1, 2)));
#else
	__m128i i16x4 = mm_setlo_epi32(color);
	return _mm_shuffle_epi8(i16x4, _mm_setr_epi16(pshufb_1to2(2), pshufb_1to2(1), pshufb_1to2(0), pshufb_1to2(3), pshufb_1to2(2), pshufb_1to2(1), pshufb_1to2(0), pshufb_1to2(3)));
#endif
}

static inline __m128i rgba_to_bgra_epi32_sse4_si32(uint32_t color) NP2_noexcept {
#if 1
	__m128i i32x4 = unpack_color_epi32_sse4_si32(color);
	return _mm_shuffle_epi32(i32x4, _MM_SHUFFLE(3, 0, 1, 2));
#else
	__m128i i32x4 = mm_setlo_epi32(color);
	return _mm_shuffle_epi8(i32x4, _mm_setr_epi32(pshufb_1to4(2), pshufb_1to4(1), pshufb_1to4(0), pshufb_1to4(3)));
#endif
}
#endif // NP2_USE_AVX2
