// Scintilla source code edit control
/** @file XPM.cxx
 ** Define a class that holds data in the X Pixmap (XPM) format.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <climits>

#include <stdexcept>
#include <string_view>
#include <vector>
#include <map>
#include <optional>
#include <algorithm>
#include <iterator>
#include <memory>

#include "ScintillaTypes.h"

#include "Debugging.h"
#include "Geometry.h"
#include "Platform.h"
#include "VectorISA.h"
#include "GraphicUtils.h"

#include "XPM.h"

using namespace Scintilla;
using namespace Scintilla::Internal;

namespace {

const char *NextField(const char *s) noexcept {
	// In case there are leading spaces in the string
	while (*s == ' ') {
		s++;
	}
	while (*s && *s != ' ') {
		s++;
	}
	while (*s == ' ') {
		s++;
	}
	return s;
}

// Data lines in XPM can be terminated either with NUL or "
size_t MeasureLength(const char *s) noexcept {
	size_t i = 0;
	while (s[i] && (s[i] != '\"')) {
		i++;
	}
	return i;
}

inline ColourRGBA ColourFromHex(const char *val) noexcept {
	unsigned color = strtoul(val, nullptr, 16);
	color = ColorFromRGBHex(color);
	return ColourRGBA(color);
}

constexpr bool IsPixelIndex(unsigned index, unsigned dimension) noexcept {
	return index < dimension;
}

#if !(NP2_USE_AVX2 || NP2_USE_SSE2)
constexpr unsigned char AlphaMultiplied(unsigned char value, unsigned char alpha) noexcept {
	return (value * alpha) / UCHAR_MAX;
}
#endif

}


ColourRGBA XPM::ColourFromCode(int ch) const noexcept {
	return colourCodeTable[ch];
}

void XPM::FillRun(Surface *surface, int code, int startX, int y, int x) const {
	if ((code != codeTransparent) && (startX != x)) {
		const PRectangle rc = PRectangle::FromInts(startX, y, x, y + 1);
		surface->FillRectangle(rc, ColourFromCode(code));
	}
}

XPM::XPM(const char *textForm) {
	Init(textForm);
}

XPM::XPM(const char *const *linesForm) {
	Init(linesForm);
}

void XPM::Init(const char *textForm) {
	// Test done is two parts to avoid possibility of overstepping the memory
	// if memcmp implemented strangely. Must be 4 bytes at least at destination.
	if ((0 == memcmp(textForm, "/* X", 4)) && (0 == memcmp(textForm, "/* XPM */", 9))) {
		// Build the lines form out of the text form
		std::vector<const char *> linesForm = LinesFormFromTextForm(textForm);
		if (!linesForm.empty()) {
			Init(linesForm.data());
		}
	} else {
		// It is really in line form
		Init(reinterpret_cast<const char * const *>(textForm));
	}
}

void XPM::Init(const char *const *linesForm) {
	height = 1;
	width = 1;
	nColours = 1;
	pixels.clear();
	codeTransparent = ' ';
	if (!linesForm)
		return;

	std::fill(colourCodeTable, std::end(colourCodeTable), black);
	const char *line0 = linesForm[0];
	width = atoi(line0);
	line0 = NextField(line0);
	height = atoi(line0);
	pixels.resize(width*height);
	line0 = NextField(line0);
	nColours = atoi(line0);
	line0 = NextField(line0);
	if (atoi(line0) != 1) {
		// Only one char per pixel is supported
		return;
	}

	for (int c = 0; c < nColours; c++) {
		const char *colourDef = linesForm[c + 1];
		const unsigned char code = colourDef[0];
		colourDef += 4;
		ColourRGBA colour(0, 0, 0);
		if (*colourDef == '#') {
			colour = ColourFromHex(colourDef + 1);
		} else {
			codeTransparent = code;
		}
		colourCodeTable[code] = colour;
	}

	for (int y = 0; y < height; y++) {
		const char *lform = linesForm[y + nColours + 1];
		const size_t len = MeasureLength(lform);
		for (size_t x = 0; x < len; x++) {
			pixels[y * width + x] = lform[x];
		}
	}
}

void XPM::Draw(Surface *surface, PRectangle rc) {
	if (pixels.empty()) {
		return;
	}
	// Centre the pixmap
	const int startY = static_cast<int>(rc.top + (rc.Height() - height) / 2);
	const int startX = static_cast<int>(rc.left + (rc.Width() - width) / 2);
	for (int y = 0; y < height; y++) {
		int prevCode = 0;
		int xStartRun = 0;
		for (int x = 0; x < width; x++) {
			const int code = pixels[y * width + x];
			if (code != prevCode) {
				FillRun(surface, prevCode, startX + xStartRun, startY + y, startX + x);
				xStartRun = x;
				prevCode = code;
			}
		}
		FillRun(surface, prevCode, startX + xStartRun, startY + y, startX + width);
	}
}

ColourRGBA XPM::PixelAt(int x, int y) const noexcept {
	if (pixels.empty() || !IsPixelIndex(x, width) || !IsPixelIndex(y, height)) {
		// Out of bounds -> transparent black
		return ColourRGBA(0, 0, 0, 0);
	}
	const int code = pixels[y * width + x];
	return ColourFromCode(code);
}

std::vector<const char *> XPM::LinesFormFromTextForm(const char *textForm) {
	// Build the lines form out of the text form
	std::vector<const char *> linesForm;
	int countQuotes = 0;
	int strings = 1;
	int j = 0;
	for (; countQuotes < (2 * strings) && textForm[j] != '\0'; j++) {
		if (textForm[j] == '\"') {
			if (countQuotes == 0) {
				// First field: width, height, number of colours, chars per pixel
				const char *line0 = textForm + j + 1;
				// Skip width
				line0 = NextField(line0);
				// Add 1 line for each pixel of height
				strings += atoi(line0);
				line0 = NextField(line0);
				// Add 1 line for each colour
				strings += atoi(line0);
			}
			if (countQuotes / 2 >= strings) {
				break;	// Bad height or number of colours!
			}
			if ((countQuotes & 1) == 0) {
				linesForm.push_back(textForm + j + 1);
			}
			countQuotes++;
		}
	}
	if (textForm[j] == '\0' || countQuotes / 2 > strings) {
		// Malformed XPM! Height + number of colours too high or too low
		linesForm.clear();
	}
	return linesForm;
}

RGBAImage::RGBAImage(int width_, int height_, float scale_, const unsigned char *pixels_) :
	height(height_), width(width_), scale(scale_) {
	if (pixels_) {
		pixelBytes.assign(pixels_, pixels_ + CountBytes());
	} else {
		pixelBytes.resize(CountBytes());
	}
}

RGBAImage::RGBAImage(const XPM &xpm) {
	height = xpm.GetHeight();
	width = xpm.GetWidth();
	scale = 1;
	pixelBytes.resize(CountBytes());
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			SetPixel(x, y, xpm.PixelAt(x, y));
		}
	}
}

int RGBAImage::CountBytes() const noexcept {
	return width * height * 4;
}

const unsigned char *RGBAImage::Pixels() const noexcept {
	return pixelBytes.data();
}

void RGBAImage::SetPixel(int x, int y, ColourRGBA colour) noexcept {
	unsigned char *pixel = pixelBytes.data() + (y*width + x) * 4;
	// RGBA
	pixel[0] = colour.GetRed();
	pixel[1] = colour.GetGreen();
	pixel[2] = colour.GetBlue();
	pixel[3] = colour.GetAlpha();
}

// Transform a block of pixels from RGBA to BGRA with premultiplied alpha.
// Used for DrawRGBAImage on some platforms.
void RGBAImage::BGRAFromRGBA(unsigned char *pixelsBGRA, const unsigned char *pixelsRGBA, size_t count) noexcept {
	static_assert(UCHAR_MAX == 255);
	// reduce code size for clang auto vectorization, we no longer use XPM for bookmark symbol.
#if 0//NP2_USE_AVX512
	count /= (sizeof(__m256i) / bytesPerPixel);
	__m256i *pbgra = reinterpret_cast<__m256i *>(pixelsBGRA);
	const __m256i *prgba = reinterpret_cast<const __m256i *>(pixelsRGBA);
	const __m512i i16x32_0x8081 = _mm512_broadcast_i32x4(_mm_set1_epi16(-0x8000 | 0x81));
	for (size_t i = 0; i < count; i++, pbgra++) {
		__m512i i16x32Color = unpack_color_epi16_avx512_ptr256(prgba++);
		i16x32Color = _mm512_shufflehi_epi16(_mm512_shufflelo_epi16(i16x32Color, _MM_SHUFFLE(3, 0, 1, 2)), _MM_SHUFFLE(3, 0, 1, 2));
		const __m512i i16x32Alpha = _mm512_shufflehi_epi16(_mm512_shufflelo_epi16(i16x32Color, 0xff), 0xff);

		i16x32Color = _mm512_mullo_epi16(i16x32Color, i16x32Alpha);
		i16x32Color = mm512_div_epu16_by_255(i16x32Color, i16x32_0x8081);
		i16x32Color = _mm512_mask_blend_epi16(0x77777777, i16x32Alpha, i16x32Color);

		i16x32Color = pack_color_epi16_avx512_si512(i16x32Color);
		_mm256_storeu_si256(pbgra, _mm512_castsi512_si256(i16x32Color));
	}

#elif NP2_USE_AVX2
#if 1
	count /= (sizeof(__m128i) / bytesPerPixel);
	__m128i *pbgra = reinterpret_cast<__m128i *>(pixelsBGRA);
	const __m128i *prgba = reinterpret_cast<const __m128i *>(pixelsRGBA);
	const __m256i i16x16_0x8081 = _mm256_broadcastsi128_si256(_mm_set1_epi16(-0x8000 | 0x81));
	for (size_t i = 0; i < count; i++, pbgra++) {
		__m256i i16x16Color = unpack_color_epi16_avx2_ptr128(prgba++);
		i16x16Color = _mm256_shufflehi_epi16(_mm256_shufflelo_epi16(i16x16Color, _MM_SHUFFLE(3, 0, 1, 2)), _MM_SHUFFLE(3, 0, 1, 2));
		const __m256i i16x16Alpha = _mm256_shufflehi_epi16(_mm256_shufflelo_epi16(i16x16Color, 0xff), 0xff);

		i16x16Color = _mm256_mullo_epi16(i16x16Color, i16x16Alpha);
		i16x16Color = mm256_div_epu16_by_255(i16x16Color, i16x16_0x8081);
		i16x16Color = _mm256_blend_epi16(i16x16Alpha, i16x16Color, 0x77);

		i16x16Color = pack_color_epi16_avx2_si256(i16x16Color);
		_mm_storeu_si128(pbgra, _mm256_castsi256_si128(i16x16Color));
	}

#else
	count /= (sizeof(uint64_t) / bytesPerPixel);
	uint64_t *pbgra = reinterpret_cast<uint64_t *>(pixelsBGRA);
	const uint64_t *prgba = reinterpret_cast<const uint64_t *>(pixelsRGBA);
	for (size_t i = 0; i < count; i++, pbgra++) {
		__m128i i16x8Color = unpack_color_epi16_sse4_ptr64(prgba++);
		i16x8Color = _mm_shufflehi_epi16(_mm_shufflelo_epi16(i16x8Color, _MM_SHUFFLE(3, 0, 1, 2)), _MM_SHUFFLE(3, 0, 1, 2));
		const __m128i i16x8Alpha = _mm_shufflehi_epi16(_mm_shufflelo_epi16(i16x8Color, 0xff), 0xff);

		i16x8Color = _mm_mullo_epi16(i16x8Color, i16x8Alpha);
		i16x8Color = mm_div_epu16_by_255(i16x8Color);
		i16x8Color = _mm_blend_epi16(i16x8Alpha, i16x8Color, 0x77);

		i16x8Color = pack_color_epi16_sse2_si128(i16x8Color);
		_mm_storel_epi64(reinterpret_cast<__m128i *>(pbgra), i16x8Color);
	}
#endif
	// end of NP2_USE_AVX2
#elif NP2_USE_SSE2
	count /= (sizeof(uint32_t) / bytesPerPixel);
	uint32_t *pbgra = reinterpret_cast<uint32_t *>(pixelsBGRA);
	const uint32_t *prgba = reinterpret_cast<const uint32_t *>(pixelsRGBA);
	for (size_t i = 0; i < count; i++, pbgra++) {
		const uint32_t rgba = bswap32(*prgba++);
		__m128i i16x4Color = unpack_color_epi16_sse2_si32(rgba);
		const __m128i i16x4Alpha = _mm_shufflelo_epi16(i16x4Color, 0);
		i16x4Color = _mm_mullo_epi16(i16x4Color, i16x4Alpha);
		i16x4Color = mm_div_epu16_by_255(i16x4Color);

		const uint32_t color = bgr_from_abgr_epi16_sse2_si32(i16x4Color);
		*pbgra = (color | (rgba << 24));
	}

#else
	for (size_t i = 0; i < count; i++) {
		const unsigned char alpha = pixelsRGBA[3];
		// Input is RGBA, output is BGRA with premultiplied alpha
		pixelsBGRA[2] = AlphaMultiplied(pixelsRGBA[0], alpha);
		pixelsBGRA[1] = AlphaMultiplied(pixelsRGBA[1], alpha);
		pixelsBGRA[0] = AlphaMultiplied(pixelsRGBA[2], alpha);
		pixelsBGRA[3] = alpha;
		pixelsRGBA += bytesPerPixel;
		pixelsBGRA += bytesPerPixel;
	}
#endif
}

RGBAImageSet::RGBAImageSet() noexcept : height(-1), width(-1) {
}

/// Remove all images.
void RGBAImageSet::Clear() noexcept {
	images.clear();
	height = -1;
	width = -1;
}

/// Add an image.
void RGBAImageSet::AddImage(int ident, std::unique_ptr<RGBAImage> image) {
	images[ident] = std::move(image);
	height = -1;
	width = -1;
}

/// Get image by id.
RGBAImage *RGBAImageSet::Get(int ident) const {
	const auto it = images.find(ident);
	if (it != images.end()) {
		return it->second.get();
	}
	return nullptr;
}

/// Give the largest height of the set.
int RGBAImageSet::GetHeight() const noexcept {
	int h = height;
	if (h < 0) {
		for (const auto &image : images) {
			h = std::max(h, image.second->GetHeight());
		}
		height = h;
	}
	return std::max(h, 0);
}

/// Give the largest width of the set.
int RGBAImageSet::GetWidth() const noexcept {
	int w = width;
	if (w < 0) {
		for (const auto &image : images) {
			w = std::max(w, image.second->GetWidth());
		}
		width = w;
	}
	return std::max(w, 0);
}
