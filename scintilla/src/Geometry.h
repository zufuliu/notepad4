// Scintilla source code edit control
/** @file Geometry.h
 ** Classes and functions for geometric and colour calculations.
 **/
// Copyright 2020 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

// use __vectorcall to pass float/double arguments such as Point and PRectangle.
#if defined(_WIN64) && defined(NDEBUG)
	#if defined(_MSC_BUILD)
		#define SCICALL __vectorcall
	#elif defined(__INTEL_COMPILER_BUILD_DATE)
		//#define SCICALL __regcall
		#define SCICALL
	#else
		#define SCICALL
	#endif
#else
	#define SCICALL
#endif

namespace Scintilla::Internal {

typedef double XYPOSITION;
typedef double XYACCUMULATOR;

// https://secret.club/2021/04/09/std-clamp.html
// https://bugs.llvm.org/show_bug.cgi?id=49909
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96733
template <typename T>
constexpr T Clamp(T x, T lower, T upper) noexcept {
	x = (x < lower) ? lower : x;
	x = (x > upper) ? upper : x;
	return x;
}

/**
 * A geometric point class.
 * Point is similar to the Win32 POINT and GTK+ GdkPoint types.
 */
class Point final {
public:
	XYPOSITION x;
	XYPOSITION y;

	constexpr explicit Point(XYPOSITION x_ = 0, XYPOSITION y_ = 0) noexcept : x(x_), y(y_) {}

	static constexpr Point FromInts(int x_, int y_) noexcept {
		return Point(static_cast<XYPOSITION>(x_), static_cast<XYPOSITION>(y_));
	}

	constexpr bool operator==(Point other) const noexcept {
		return (x == other.x) && (y == other.y);
	}

	constexpr bool operator!=(Point other) const noexcept {
		return (x != other.x) || (y != other.y);
	}

	constexpr Point operator+(Point other) const noexcept {
		return Point(x + other.x, y + other.y);
	}

	constexpr Point operator-(Point other) const noexcept {
		return Point(x - other.x, y - other.y);
	}

	// Other automatically defined methods (assignment, copy constructor, destructor) are fine
};


/**
 * A geometric interval class.
 */
class Interval final {
public:
	XYPOSITION left;
	XYPOSITION right;
	constexpr bool operator==(const Interval &other) const noexcept {
		return (left == other.left) && (right == other.right);
	}
	constexpr XYPOSITION Width() const noexcept {
		return right - left;
	}
	constexpr bool Empty() const noexcept {
		return Width() <= 0;
	}
	constexpr bool Intersects(Interval other) const noexcept {
		return (right > other.left) && (left < other.right);
	}
};

/**
 * A geometric rectangle class.
 * PRectangle is similar to Win32 RECT.
 * PRectangles contain their top and left sides, but not their right and bottom sides.
 */
class PRectangle final {
public:
	XYPOSITION left;
	XYPOSITION top;
	XYPOSITION right;
	XYPOSITION bottom;

	constexpr explicit PRectangle(XYPOSITION left_ = 0, XYPOSITION top_ = 0, XYPOSITION right_ = 0, XYPOSITION bottom_ = 0) noexcept :
		left(left_), top(top_), right(right_), bottom(bottom_) {}

	static constexpr PRectangle FromInts(int left_, int top_, int right_, int bottom_) noexcept {
		return PRectangle(static_cast<XYPOSITION>(left_), static_cast<XYPOSITION>(top_),
			static_cast<XYPOSITION>(right_), static_cast<XYPOSITION>(bottom_));
	}

	// Other automatically defined methods (assignment, copy constructor, destructor) are fine

	constexpr bool operator==(const PRectangle &rc) const noexcept {
		return (rc.left == left) && (rc.right == right) &&
			(rc.top == top) && (rc.bottom == bottom);
	}
	constexpr bool Contains(Point pt) const noexcept {
		return (pt.x >= left) && (pt.x <= right) &&
			(pt.y >= top) && (pt.y <= bottom);
	}
	constexpr bool ContainsWholePixel(Point pt) const noexcept {
		// Does the rectangle contain all of the pixel to left/below the point
		return (pt.x >= left) && ((pt.x + 1) <= right) &&
			(pt.y >= top) && ((pt.y + 1) <= bottom);
	}
	constexpr bool Contains(PRectangle rc) const noexcept {
		return (rc.left >= left) && (rc.right <= right) &&
			(rc.top >= top) && (rc.bottom <= bottom);
	}
	constexpr bool Intersects(PRectangle other) const noexcept {
		return (right > other.left) && (left < other.right) &&
			(bottom > other.top) && (top < other.bottom);
	}
	void Move(XYPOSITION xDelta, XYPOSITION yDelta) noexcept {
		left += xDelta;
		top += yDelta;
		right += xDelta;
		bottom += yDelta;
	}

	constexpr PRectangle Inflate(XYPOSITION xDelta, XYPOSITION yDelta) const noexcept {
		return PRectangle(left - xDelta, top - yDelta, right + xDelta, bottom + yDelta);
	}
	constexpr PRectangle Inflate(int xDelta, int yDelta) const noexcept {
		return PRectangle(left - xDelta, top - yDelta, right + xDelta, bottom + yDelta);
	}
	constexpr PRectangle Deflate(XYPOSITION xDelta, XYPOSITION yDelta) const noexcept {
		return Inflate(-xDelta, -yDelta);
	}
	constexpr PRectangle Deflate(int xDelta, int yDelta) const noexcept {
		return Inflate(-xDelta, -yDelta);
	}

	constexpr PRectangle Inset(XYPOSITION delta) const noexcept {
		return PRectangle(left + delta, top + delta, right - delta, bottom - delta);
	}

	constexpr PRectangle Inset(Point delta) const noexcept {
		return PRectangle(left + delta.x, top + delta.y, right - delta.x, bottom - delta.y);
	}

	constexpr Point Centre() const noexcept {
		return Point((left + right) / 2, (top + bottom) / 2);
	}

	constexpr XYPOSITION Width() const noexcept {
		return right - left;
	}
	constexpr XYPOSITION Height() const noexcept {
		return bottom - top;
	}
	constexpr bool Empty() const noexcept {
		return (Height() <= 0) || (Width() <= 0);
	}
};

enum class Edge { left, top, bottom, right };

PRectangle Clamp(PRectangle rc, Edge edge, XYPOSITION position) noexcept;
PRectangle Side(PRectangle rc, Edge edge, XYPOSITION size) noexcept;

Interval Intersection(Interval a, Interval b) noexcept;
PRectangle Intersection(PRectangle rc, Interval horizontalBounds) noexcept;
Interval HorizontalBounds(PRectangle rc) noexcept;

XYPOSITION PixelAlign(XYPOSITION xy, int pixelDivisions) noexcept;
XYPOSITION PixelAlignFloor(XYPOSITION xy, int pixelDivisions) noexcept;
XYPOSITION PixelAlignCeil(XYPOSITION xy, int pixelDivisions) noexcept;

Point PixelAlign(Point pt, int pixelDivisions) noexcept;

PRectangle PixelAlign(PRectangle rc, int pixelDivisions) noexcept;
PRectangle PixelAlignOutside(PRectangle rc, int pixelDivisions) noexcept;

/**
* Holds an RGBA colour with 8 bits for each component.
*/
constexpr float componentMaximum = 255.0f;
class ColourRGBA final {
	unsigned int co;
public:
	constexpr explicit ColourRGBA(unsigned int co_ = 0) noexcept : co(co_) {}

	constexpr ColourRGBA(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha = 0xff) noexcept :
		ColourRGBA(red | (green << 8) | (blue << 16) | (alpha << 24)) {
	}

	constexpr ColourRGBA(ColourRGBA cd, unsigned int alpha) noexcept :
		ColourRGBA(cd.OpaqueRGB() | (alpha << 24)) {
	}

	static constexpr ColourRGBA FromRGB(unsigned int co_) noexcept {
		return ColourRGBA(co_ | (0xffu << 24));
	}

	static constexpr ColourRGBA FromIpRGB(intptr_t co_) noexcept {
		return ColourRGBA(static_cast<unsigned int>(co_) | (0xffu << 24));
	}

	constexpr ColourRGBA WithoutAlpha() const noexcept {
		return ColourRGBA(co & 0xffffff);
	}

	constexpr ColourRGBA Opaque() const noexcept {
		return ColourRGBA(co | (0xffu << 24));
	}

	constexpr unsigned int AsInteger() const noexcept {
		return co;
	}

	constexpr unsigned int OpaqueRGB() const noexcept {
		return co & 0xffffff;
	}

	// Red, green, blue and alpha values as bytes 0..255
	constexpr unsigned char GetRed() const noexcept {
		return co & 0xff;
	}
	constexpr unsigned char GetGreen() const noexcept {
		return (co >> 8) & 0xff;
	}
	constexpr unsigned char GetBlue() const noexcept {
		return (co >> 16) & 0xff;
	}
	constexpr unsigned char GetAlpha() const noexcept {
		return co >> 24;
	}

	// Red, green, blue, and alpha values as float 0..1.0
	constexpr float GetRedComponent() const noexcept {
		return GetRed() / componentMaximum;
	}
	constexpr float GetGreenComponent() const noexcept {
		return GetGreen() / componentMaximum;
	}
	constexpr float GetBlueComponent() const noexcept {
		return GetBlue() / componentMaximum;
	}
	constexpr float GetAlphaComponent() const noexcept {
		return GetAlpha() / componentMaximum;
	}

	constexpr bool operator==(const ColourRGBA &other) const noexcept {
		return co == other.co;
	}

	constexpr bool IsOpaque() const noexcept {
		return co >= 0xff000000U;
	}

	constexpr ColourRGBA MixedWith(ColourRGBA other) const noexcept {
		const unsigned int red = (GetRed() + other.GetRed()) / 2;
		const unsigned int green = (GetGreen() + other.GetGreen()) / 2;
		const unsigned int blue = (GetBlue() + other.GetBlue()) / 2;
		const unsigned int alpha = (GetAlpha() + other.GetAlpha()) / 2;
		return ColourRGBA(red, green, blue, alpha);
	}

#if 0
	static constexpr unsigned int Mixed(unsigned char a, unsigned char b, double proportion) noexcept {
		return static_cast<unsigned int>(a + proportion * (b - a));
	}

	constexpr ColourRGBA MixedWith(ColourRGBA other, double proportion) const noexcept {
		return ColourRGBA(
			Mixed(GetRed(), other.GetRed(), proportion),
			Mixed(GetGreen(), other.GetGreen(), proportion),
			Mixed(GetBlue(), other.GetBlue(), proportion),
			Mixed(GetAlpha(), other.GetAlpha(), proportion));
	}
#endif
	static constexpr ColourRGBA MixAlpha(ColourRGBA colour, ColourRGBA other) noexcept {
		unsigned int alpha = other.GetAlpha();
		const unsigned int red = (other.GetRed()*alpha + colour.GetRed()*(255 ^ alpha)) >> 8;
		const unsigned int green = (other.GetGreen()*alpha + colour.GetGreen()*(255 ^ alpha)) >> 8;
		const unsigned int blue = (other.GetBlue()*alpha + colour.GetBlue()*(255 ^ alpha)) >> 8;
		alpha = (alpha*alpha + colour.GetAlpha()*(255 ^ alpha)) >> 8;
		return ColourRGBA(red, green, blue, alpha);
	}

	// Manual alpha blending
	static constexpr ColourRGBA AlphaBlend(ColourRGBA fore, ColourRGBA back, unsigned int alpha) noexcept {
		const unsigned int red = (fore.GetRed()*alpha + back.GetRed()*(255 ^ alpha)) >> 8;
		const unsigned int green = (fore.GetGreen()*alpha + back.GetGreen()*(255 ^ alpha)) >> 8;
		const unsigned int blue = (fore.GetBlue()*alpha + back.GetBlue()*(255 ^ alpha)) >> 8;
		return ColourRGBA(red, green, blue);
	}
};

/**
* Holds an RGBA colour and stroke width to stroke a shape.
*/
class Stroke final {
public:
	ColourRGBA colour;
	XYPOSITION width;
	constexpr explicit Stroke(ColourRGBA colour_, XYPOSITION width_ = 1.0) noexcept :
		colour(colour_), width(width_) {}
	constexpr float WidthF() const noexcept {
		return static_cast<float>(width);
	}
};

/**
* Holds an RGBA colour to fill a shape.
*/
class Fill final {
public:
	ColourRGBA colour;
	constexpr Fill(ColourRGBA colour_) noexcept :
		colour(colour_) {}
};

/**
* Holds a pair of RGBA colours and stroke width to fill and stroke a shape.
*/
class FillStroke final {
public:
	Fill fill;
	Stroke stroke;
	constexpr FillStroke(ColourRGBA colourFill_, ColourRGBA colourStroke_, XYPOSITION widthStroke_ = 1.0) noexcept :
		fill(colourFill_), stroke(colourStroke_, widthStroke_) {}
	constexpr explicit FillStroke(ColourRGBA colourBoth, XYPOSITION widthStroke_ = 1.0) noexcept :
		fill(colourBoth), stroke(colourBoth, widthStroke_) {}
};

/**
* Holds an element of a gradient with an RGBA colour and a relative position.
*/
class ColourStop final {
public:
	XYPOSITION position;
	ColourRGBA colour;
	constexpr ColourStop(XYPOSITION position_, ColourRGBA colour_) noexcept :
		position(position_), colour(colour_) {}
};

}
