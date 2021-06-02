// Scintilla source code edit control
/** @file ElapsedPeriod.h
 ** Encapsulate C++ <chrono> to simplify use.
 **/
// Copyright 2018 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

namespace Scintilla::Internal {

// Simplified access to high precision timing.
class ElapsedPeriod {
	using ElapsedClock = std::chrono::steady_clock;
	ElapsedClock::time_point tp;
public:
	/// Capture the moment
	ElapsedPeriod() noexcept : tp(ElapsedClock::now()) {}
	/// Return duration as floating point seconds
	double Duration(bool reset) noexcept {
		const auto tpNow = ElapsedClock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(tpNow - tp);
		if (reset) {
			tp = tpNow;
		}
		return duration.count();
	}

	double Duration() const noexcept {
		const auto tpNow = ElapsedClock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(tpNow - tp);
		return duration.count();
	}
};

}
