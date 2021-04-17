// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
#pragma once

// color byte order from high to lower:
// [a][b][g][r]		COLORREF/RGB(), ColourAlpha, D2D_COLOR_F
// swap:		[r][g][b][a]
//     rotr8:	[a][r][g][b]
// rol:			[b][g][r][a]
//
// [a][r][g][b]		BITMAP RGBQUAD, RGBTRIPLE
// rotl8:		[r][g][b][a]
// swap:		[b][g][r][a]
//     rotr8:	[a][b][g][r]


static inline uint32_t RGBQuadFromColor(uint32_t color) NP2_noexcept {
	return rotr8(bswap32(color));
}

static inline uint32_t ColorFromRGBQuad(uint32_t quad) NP2_noexcept {
	return rotr8(bswap32(quad));
}

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

static inline __m128i mm_loadl_si32(const uint32_t *value) NP2_noexcept {
#if defined(__GNUC__)
	return _mm_setr_epi32(*value, 0, 0, 0);
#else
	return _mm_loadu_si32(value);
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


static inline __m128i mm_divlo_epu16_by_255(__m128i i32x4) NP2_noexcept {
	// uint16_t value / 255  => (value * 0x8081) >> (16 + 7)
	//i32x4 = _mm_add_epi32(i32x4, _mm_set1_epi32(127)); // round the result
	i32x4 = _mm_mulhi_epu16(i32x4, _mm_set1_epi32(0x8081));
	return _mm_srli_epi32(i32x4, 7);
}

static inline __m128i mm_xor_alpha_epi32(__m128i alpha) NP2_noexcept {
	// 255 - alpha => 255 ^ alpha
	return _mm_xor_si128(_mm_set1_epi32(255), alpha);
}

static inline __m128i mm_alpha_blend_epi32(__m128i fore, __m128i back, __m128i alpha) NP2_noexcept {
	// (fore*alpha + back*(255 - alpha)) / 255
	fore = _mm_mullo_epi16(fore, alpha);
	back = _mm_mullo_epi16(back, mm_xor_alpha_epi32(alpha));
	fore = _mm_add_epi32(fore, back);
	fore = mm_divlo_epu16_by_255(fore);
	return fore;
}

static inline __m128i mm_unpack_color_sse2_si32(uint32_t color) NP2_noexcept {
	__m128i i32x4 = mm_setlo_epi32(color);
	i32x4 = _mm_unpacklo_epi8(i32x4, _mm_setzero_si128());
	i32x4 = _mm_unpacklo_epi8(i32x4, _mm_setzero_si128());
	return i32x4;
}

static inline __m128i mm_unpack_color_sse2_ptr32(const uint32_t *color) NP2_noexcept {
	__m128i i32x4 = mm_loadl_si32(color);
	i32x4 = _mm_unpacklo_epi8(i32x4, _mm_setzero_si128());
	i32x4 = _mm_unpacklo_epi8(i32x4, _mm_setzero_si128());
	return i32x4;
}

static inline __m128i mm_pack_color_si128(__m128i i32x4) NP2_noexcept {
	i32x4 = _mm_packus_epi16(i32x4, i32x4);
	i32x4 = _mm_packus_epi16(i32x4, i32x4);
	return i32x4;
}

static inline uint32_t mm_pack_color_si32(__m128i i32x4) NP2_noexcept {
	return _mm_cvtsi128_si32(mm_pack_color_si128(i32x4));
}


static inline __m128i rgba_to_bgra_sse2_si32(uint32_t color) NP2_noexcept {
	__m128i i32x4 = mm_unpack_color_sse2_si32(color);
	return _mm_shuffle_epi32(i32x4, 0xc6); // 0b11_00_01_10
}

static inline uint32_t bgr_from_bgra_s132(__m128i i32x4) NP2_noexcept {
	return mm_pack_color_si32(i32x4) & 0xffffff;
}

static inline __m128i rgba_to_abgr_sse2_si32(uint32_t color) NP2_noexcept {
	return mm_unpack_color_sse2_si32(bswap32(color));
}

static inline __m128i bgra_to_abgr_sse2_si32(uint32_t quad) NP2_noexcept {
	return mm_unpack_color_sse2_si32(rotl8(quad));
}

static inline uint32_t bgra_from_abgr_si32(__m128i i32x4) NP2_noexcept {
	return rotr8(mm_pack_color_si32(i32x4));
}

static inline uint32_t bgr_from_abgr_s132(__m128i i32x4) NP2_noexcept {
	return mm_pack_color_si32(i32x4) >> 8;
}
#endif // NP2_USE_SSE2

#if NP2_USE_AVX2
#define mm_alignr_ps(a, b, imm8) \
	_mm_castsi128_ps(_mm_alignr_epi8(_mm_castps_si128(a), _mm_castps_si128(b), (imm8)*4))

static inline __m128i mm_unpack_color_avx2_si32(uint32_t color) NP2_noexcept {
	__m128i i32x4 = mm_setlo_epi32(color);
	return _mm_cvtepu8_epi32(i32x4);
}

static inline __m128i mm_unpack_color_avx2_ptr32(const uint32_t *color) NP2_noexcept {
	return _mm_cvtepu8_epi32(*((__m128i *)(color)));
}

static inline __m128i rgba_to_bgra_avx2_si32(uint32_t color) NP2_noexcept {
#if 1
	__m128i i32x4 = mm_unpack_color_avx2_si32(color);
	return _mm_shuffle_epi32(i32x4, 0xc6); // 0b11_00_01_10
#else
	__m128i i32x4 = mm_setlo_epi32(color);
#define mask32(n)	(0x80808000 | (n))
	return _mm_shuffle_epi8(i32x4, _mm_setr_epi32(mask32(2), mask32(1), mask32(0), mask32(3)));
#undef mask32
#endif
}

static inline __m128i rgba_to_abgr_avx2_si32(uint32_t color) NP2_noexcept {
	return mm_unpack_color_avx2_si32(bswap32(color));
}

static inline int mm_hadd_epi32_si32(__m128i i32x4) NP2_noexcept {
	i32x4 = _mm_hadd_epi32(i32x4, i32x4);
	i32x4 = _mm_hadd_epi32(i32x4, i32x4);
	return _mm_cvtsi128_si32(i32x4);
}


// process 2 pixels at a time

static inline __m256i mm256_divlo_epu16_by_255(__m256i i32x8) NP2_noexcept {
	// uint16_t value / 255  => (value * 0x8081) >> (16 + 7)
	i32x8 = _mm256_mulhi_epu16(i32x8, _mm256_set1_epi32(0x8081));
	return _mm256_srli_epi32(i32x8, 7);
}

static inline __m256i mm256_xor_alpha_epi32(__m256i alpha) NP2_noexcept {
	// 255 - alpha => 255 ^ alpha
	return _mm256_xor_si256(_mm256_set1_epi32(255), alpha);
}

static inline __m256i mm256_alpha_blend_epi32(__m256i fore, __m256i back, __m256i alpha) NP2_noexcept {
	// (fore*alpha + back*(255 - alpha)) / 255
	fore = _mm256_mullo_epi16(fore, alpha);
	back = _mm256_mullo_epi16(back, mm256_xor_alpha_epi32(alpha));
	fore = _mm256_add_epi32(fore, back);
	fore = mm256_divlo_epu16_by_255(fore);
	return fore;
}


static inline __m256i mm256_unpack_color_ptr64(const uint64_t *color) NP2_noexcept {
	return _mm256_cvtepu8_epi32(*((__m128i *)color));
}

static inline __m256i mm256_unpack_color_si64(uint64_t color) NP2_noexcept {
	__m128i i32x4 = mm_setlo_epi64(color);
	return _mm256_cvtepu8_epi32(i32x4);
}

static inline __m256i mm256_unpack_color_si32(uint32_t color) NP2_noexcept {
	__m128i i32x4 = mm_setlo_epi32(color);
	return _mm256_cvtepu8_epi32(_mm_broadcastd_epi32(i32x4));
}

static inline __m128i mm256_pack_color_si128(__m256i i32x8) NP2_noexcept {
	i32x8 = _mm256_packus_epi16(i32x8, i32x8);
	i32x8 = _mm256_packus_epi16(i32x8, i32x8);
	return _mm_unpacklo_epi32(_mm256_castsi256_si128(i32x8), _mm256_extracti128_si256(i32x8, 1));
}

static inline uint64_t mm256_pack_color_si64(__m256i i32x8) NP2_noexcept {
	return _mm_cvtsi128_si64(mm256_pack_color_si128(i32x8));
}

static inline __m256i mm256_rgba_to_bgra_si32(uint32_t color) NP2_noexcept {
	__m128i i32x4 = rgba_to_bgra_avx2_si32(color);
	return _mm256_setr_m128i(i32x4, i32x4);
}
#endif
