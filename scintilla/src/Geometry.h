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

namespace Scintilla {

typedef double XYPOSITION;
typedef double XYACCUMULATOR;

// Test if an enum class value has the bit flag(s) of test set.
template <typename T>
constexpr bool FlagSet(T value, T test) noexcept {
	return (static_cast<int>(value) & static_cast<int>(test)) == static_cast<int>(test);
}

// https://bugs.llvm.org/show_bug.cgi?id=49377
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96733
template <typename T>
constexpr T Clamp(T x, T lower, T upper) noexcept {
#if defined(__GNUC__) || defined(__clang__)
	return std::min(std::max(x, lower), upper);
#else
	return (x < lower) ? lower : (x > upper) ? upper : x;
#endif
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

Point PixelAlign(Point pt, int pixelDivisions) noexcept;

PRectangle PixelAlign(PRectangle rc, int pixelDivisions) noexcept;
PRectangle PixelAlignOutside(PRectangle rc, int pixelDivisions) noexcept;

/**
 * Holds an RGB colour with 8 bits for each component.
 */
constexpr const float componentMaximum = 255.0f;
class ColourDesired {
	unsigned int co;
public:
	constexpr explicit ColourDesired(unsigned int co_ = 0) noexcept : co(co_) {}

	constexpr ColourDesired(unsigned int red, unsigned int green, unsigned int blue) noexcept :
		co(red | (green << 8) | (blue << 16)) {}

	constexpr bool operator==(const ColourDesired &other) const noexcept {
		return co == other.co;
	}

	constexpr unsigned int AsInteger() const noexcept {
		return co;
	}

	// Red, green and blue values as bytes 0..255
	constexpr unsigned char GetRed() const noexcept {
		return co & 0xff;
	}
	constexpr unsigned char GetGreen() const noexcept {
		return (co >> 8) & 0xff;
	}
	constexpr unsigned char GetBlue() const noexcept {
		return (co >> 16) & 0xff;
	}

	// Red, green and blue values as float 0..1.0
	constexpr float GetRedComponent() const noexcept {
		return GetRed() / componentMaximum;
	}
	constexpr float GetGreenComponent() const noexcept {
		return GetGreen() / componentMaximum;
	}
	constexpr float GetBlueComponent() const noexcept {
		return GetBlue() / componentMaximum;
	}

	// Manual alpha blending
	constexpr ColourDesired AlphaBlendOn(unsigned int alpha, ColourDesired back) const noexcept {
		const unsigned int red = (GetRed()*alpha + back.GetRed()*(255 - alpha)) >> 8;
		const unsigned int green = (GetGreen()*alpha + back.GetGreen()*(255 - alpha)) >> 8;
		const unsigned int blue = (GetBlue()*alpha + back.GetBlue()*(255 - alpha)) >> 8;
		return ColourDesired(red, green, blue);
	}
};

/**
* Holds an RGBA colour.
*/
class ColourAlpha final : public ColourDesired {
public:
	constexpr explicit ColourAlpha(unsigned co_ = 0) noexcept : ColourDesired(co_) {}

	constexpr ColourAlpha(unsigned int red, unsigned int green, unsigned int blue, unsigned int alpha = 0xff) noexcept :
		ColourDesired(red | (green << 8) | (blue << 16) | (alpha << 24)) {
	}

	constexpr ColourAlpha(ColourDesired cd, unsigned int alpha) noexcept :
		ColourDesired(cd.AsInteger() | (alpha << 24)) {
	}

	constexpr ColourAlpha(ColourDesired cd) noexcept :
		ColourDesired(cd.AsInteger() | (0xffu << 24)) {
	}

	constexpr ColourDesired GetColour() const noexcept {
		return ColourDesired(AsInteger() & 0xffffff);
	}

	constexpr unsigned char GetAlpha() const noexcept {
		return (AsInteger() >> 24) & 0xff;
	}

	constexpr float GetAlphaComponent() const noexcept {
		return GetAlpha() / componentMaximum;
	}

	constexpr bool IsOpaque() const noexcept {
		return GetAlpha() == 0xff;
	}

	constexpr ColourAlpha MixedWith(ColourAlpha other) const noexcept {
		const unsigned int red = (GetRed() + other.GetRed()) / 2;
		const unsigned int green = (GetGreen() + other.GetGreen()) / 2;
		const unsigned int blue = (GetBlue() + other.GetBlue()) / 2;
		const unsigned int alpha = (GetAlpha() + other.GetAlpha()) / 2;
		return ColourAlpha(red, green, blue, alpha);
	}
};

/**
* Holds an RGBA colour and stroke width to stroke a shape.
*/
class Stroke final {
public:
	ColourAlpha colour;
	XYPOSITION width;
	constexpr explicit Stroke(ColourAlpha colour_, XYPOSITION width_ = 1.0f) noexcept :
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
	ColourAlpha colour;
	constexpr Fill(ColourAlpha colour_) noexcept :
		colour(colour_) {}
	constexpr Fill(ColourDesired colour_) noexcept :
		colour(colour_) {}
};

/**
* Holds a pair of RGBA colours and stroke width to fill and stroke a shape.
*/
class FillStroke final {
public:
	Fill fill;
	Stroke stroke;
	constexpr FillStroke(ColourAlpha colourFill_, ColourAlpha colourStroke_, XYPOSITION widthStroke_ = 1.0f) noexcept :
		fill(colourFill_), stroke(colourStroke_, widthStroke_) {}
	constexpr explicit FillStroke(ColourAlpha colourBoth, XYPOSITION widthStroke_ = 1.0f) noexcept :
		fill(colourBoth), stroke(colourBoth, widthStroke_) {}
};

/**
* Holds an element of a gradient with an RGBA colour and a relative position.
*/
class ColourStop final {
public:
	XYPOSITION position;
	ColourAlpha colour;
	constexpr ColourStop(XYPOSITION position_, ColourAlpha colour_) noexcept :
		position(position_), colour(colour_) {}
};

}
