// Scintilla source code edit control
/** @file XPM.cxx
 ** Define a class that holds data in the X Pixmap (XPM) format.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdlib>
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

constexpr unsigned int GetHexDigit(unsigned char ch) noexcept {
	unsigned int diff = ch - '0';
	if (diff < 10) {
		return diff;
	}
	diff = (ch | 0x20) - 'a';
	if (diff < 6) {
		return diff + 10;
	}
	return 0;
}

constexpr unsigned int GetHexValue(unsigned char ch1, unsigned char ch2) noexcept {
	return (GetHexDigit(ch1) << 4) | GetHexDigit(ch2);
}

constexpr ColourRGBA ColourFromHex(const char *val) noexcept {
	const unsigned int r = GetHexValue(val[0], val[1]);
	const unsigned int g = GetHexValue(val[2], val[3]);
	const unsigned int b = GetHexValue(val[4], val[5]);
	return ColourRGBA(r, g, b);
}

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

	std::fill(colourCodeTable, std::end(colourCodeTable), ColourRGBA(0, 0, 0, 0));
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
		const char code = colourDef[0];
		colourDef += 4;
		ColourRGBA colour(0, 0, 0);
		if (*colourDef == '#') {
			colour = ColourFromHex(colourDef + 1);
		} else {
			codeTransparent = code;
		}
		colourCodeTable[static_cast<unsigned char>(code)] = colour;
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
	if (pixels.empty() || (x < 0) || (x >= width) || (y < 0) || (y >= height)) {
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
#if NP2_USE_AVX2
	count /= (bytesPerPixel * 2);
	uint64_t *pbgra = reinterpret_cast<uint64_t *>(pixelsBGRA);
	const uint64_t *prgba = reinterpret_cast<const uint64_t *>(pixelsRGBA);
	for (size_t i = 0; i < count; i++, pbgra++) {
		__m128i i16x8Color = unpack_color_epi16_sse4_ptr64(prgba++);
		i16x8Color = _mm_shufflehi_epi16(_mm_shufflelo_epi16(i16x8Color, _MM_SHUFFLE(3, 0, 1, 2)), _MM_SHUFFLE(3, 0, 1, 2));
		__m128i i16x8Alpha = _mm_shufflehi_epi16(_mm_shufflelo_epi16(i16x8Color, 0xff), 0xff);

		i16x8Color = _mm_mullo_epi16(i16x8Color, i16x8Alpha);
		i16x8Color = mm_div_epu16_by_255(i16x8Color);
		i16x8Color = _mm_blend_epi16(i16x8Alpha, i16x8Color, 0x77);

		i16x8Color = pack_color_epi16_sse2_si128(i16x8Color);
		_mm_storel_epi64((__m128i *)pbgra, i16x8Color);
	}

#elif NP2_USE_SSE2
	count /= bytesPerPixel;
	uint32_t *pbgra = reinterpret_cast<uint32_t *>(pixelsBGRA);
	const uint32_t *prgba = reinterpret_cast<const uint32_t *>(pixelsRGBA);
	for (size_t i = 0; i < count; i++, pbgra++) {
		const uint32_t rgba = bswap32(*prgba++);
		__m128i i16x4Color = unpack_color_epi16_sse2_si32(rgba);
		__m128i i16x4Alpha = _mm_shufflelo_epi16(i16x4Color, 0);
		i16x4Color = _mm_mullo_epi16(i16x4Color, i16x4Alpha);
		i16x4Color = mm_div_epu16_by_255(i16x4Color);

		const uint32_t color = bgr_from_abgr_epi16_sse2_si32(i16x4Color);
		*pbgra = (color | (rgba << 24));
	}

#else
	for (size_t i = 0; i < count; i++) {
		const unsigned char alpha = pixelsRGBA[3];
		// Input is RGBA, output is BGRA with premultiplied alpha
		pixelsBGRA[2] = pixelsRGBA[0] * alpha / UCHAR_MAX;
		pixelsBGRA[1] = pixelsRGBA[1] * alpha / UCHAR_MAX;
		pixelsBGRA[0] = pixelsRGBA[2] * alpha / UCHAR_MAX;
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
