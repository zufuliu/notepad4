// Scintilla source code edit control
/** @file ElapsedPeriod.h
 ** Encapsulate C++ <chrono> to simplify use.
 **/
// Copyright 2018 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

//#include <chrono>

namespace Scintilla::Internal {

// Simplified access to high precision timing.
#if 0
class ElapsedPeriod {
	using ElapsedClock = std::chrono::steady_clock;
	ElapsedClock::time_point tp;
public:
	/// Capture the moment
	ElapsedPeriod() noexcept : tp(ElapsedClock::now()) {}
	/// Return duration as floating point seconds
	double Reset() noexcept {
		const auto tpNow = ElapsedClock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(tpNow - tp);
		tp = tpNow;
		return duration.count();
	}

	double Duration() const noexcept {
		const auto tpNow = ElapsedClock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(tpNow - tp);
		return duration.count();
	}
};

#else
extern int64_t QueryPerformanceFrequency() noexcept;
extern int64_t QueryPerformanceCounter() noexcept;

class ElapsedPeriod {
	int64_t freq;
	int64_t begin;
public:
	ElapsedPeriod() noexcept {
		freq = QueryPerformanceFrequency();
		begin = QueryPerformanceCounter();
	}
	double Reset() noexcept {
		const int64_t end = QueryPerformanceCounter();
		const int64_t diff = end - begin;
		const double duration = diff / static_cast<double>(freq);
		begin = end;
		return duration;
	}
	double Duration() const noexcept {
		const int64_t end = QueryPerformanceCounter();
		const int64_t diff = end - begin;
		const double duration = diff / static_cast<double>(freq);
		return duration;
	}
};

#endif

}
