// Scintilla source code edit control
/** @file Geometry.cxx
 ** Helper functions for geometric calculations.
 **/
// Copyright 2020 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstdint>
#include <cmath>

#include <algorithm>

#include "Geometry.h"

namespace Scintilla::Internal {

PRectangle Clamp(PRectangle rc, Edge edge, XYPOSITION position) noexcept {
	switch (edge) {
	case Edge::left:
		rc.left = Clamp(position, rc.left, rc.right);
		break;
	case Edge::top:
		rc.top = Clamp(position, rc.top, rc.bottom);
		break;
	case Edge::right:
		rc.right = Clamp(position, rc.left, rc.right);
		break;
	case Edge::bottom:
	default:
		rc.bottom =Clamp(position, rc.top, rc.bottom);
		break;
	}
	return rc;
}

PRectangle Side(PRectangle rc, Edge edge, XYPOSITION size) noexcept {
	switch (edge) {
	case Edge::left:
		rc.right = std::min(rc.left + size, rc.right);
		break;
	case Edge::top:
		rc.bottom = std::min(rc.top + size, rc.bottom);
		break;
	case Edge::right:
		rc.left = std::max(rc.left, rc.right - size);
		break;
	case Edge::bottom:
	default:
		rc.top = std::max(rc.top, rc.bottom - size);
		break;
	}
	return rc;
}

Interval Intersection(Interval a, Interval b) noexcept {
	const XYPOSITION leftMax = std::max(a.left, b.left);
	const XYPOSITION rightMin = std::min(a.right, b.right);
	// If the result would have a negative width. make empty instead.
	const XYPOSITION rightResult = std::max(rightMin, leftMax);
	return { leftMax, rightResult };
}

PRectangle Intersection(PRectangle rc, Interval horizontalBounds) noexcept {
	const Interval intersection = Intersection(HorizontalBounds(rc), horizontalBounds);
	return PRectangle(intersection.left, rc.top, intersection.right, rc.bottom);
}

Interval HorizontalBounds(PRectangle rc) noexcept {
	return { rc.left, rc.right };
}

XYPOSITION PixelAlign(XYPOSITION xy, int pixelDivisions) noexcept {
	return std::round(xy * pixelDivisions) / pixelDivisions;
}

XYPOSITION PixelAlignFloor(XYPOSITION xy, int pixelDivisions) noexcept {
	return std::floor(xy * pixelDivisions) / pixelDivisions;
}

XYPOSITION PixelAlignCeil(XYPOSITION xy, int pixelDivisions) noexcept {
	return std::ceil(xy * pixelDivisions) / pixelDivisions;
}

Point PixelAlign(Point pt, int pixelDivisions) noexcept {
	return Point(
			PixelAlign(pt.x, pixelDivisions),
			PixelAlign(pt.y, pixelDivisions));
}

PRectangle PixelAlign(PRectangle rc, int pixelDivisions) noexcept {
	// Move left and right side to nearest pixel to avoid blurry visuals.
	// The top and bottom should be integers but floor them to make sure.
	// `pixelDivisions` is commonly 1 except for 'retina' displays where it is 2.
	// On retina displays, the positions should be moved to the nearest device
	// pixel which is the nearest half logical pixel.
	return PRectangle(
		PixelAlign(rc.left, pixelDivisions),
		PixelAlignFloor(rc.top, pixelDivisions),
		PixelAlign(rc.right, pixelDivisions),
		PixelAlignFloor(rc.bottom, pixelDivisions));
}

PRectangle PixelAlignOutside(PRectangle rc, int pixelDivisions) noexcept {
	// Move left and right side to extremes (floor(left) ceil(right)) to avoid blurry visuals.
	return PRectangle(
		PixelAlignFloor(rc.left, pixelDivisions),
		PixelAlignFloor(rc.top, pixelDivisions),
		PixelAlignCeil(rc.right, pixelDivisions),
		PixelAlignFloor(rc.bottom, pixelDivisions));
}

}
