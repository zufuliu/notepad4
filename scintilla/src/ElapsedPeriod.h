// Scintilla source code edit control
/** @file ElapsedPeriod.h
 ** Encapsulate C++ <chrono> to simplify use.
 **/
// Copyright 2018 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla {

// Simplified access to high precision timing.
class ElapsedPeriod {
#ifndef _LIBCPP_HAS_NO_MONOTONIC_CLOCK
	using ElapsedClock = std::chrono::steady_clock;
#else
	using ElapsedClock = std::chrono::high_resolution_clock;
#endif
	ElapsedClock::time_point tp;
public:
	/// Capture the moment
	ElapsedPeriod() noexcept : tp(ElapsedClock::now()) {}
	/// Return duration as floating point seconds
	double Duration(bool reset=false) noexcept {
		const auto tpNow = ElapsedClock::now();
		const auto stylingDuration =
			std::chrono::duration_cast<std::chrono::duration<double>>(tpNow - tp);
		if (reset) {
			tp = tpNow;
		}
		return stylingDuration.count();
	}
};

}
