// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
#pragma once

namespace Lexilla {

template<int valueBit, int maxStateCount, int countBit, int baseStyle>
int PackLineState(const std::vector<int>& states) noexcept {
	size_t index = states.size();
	const int backCount = sci::min(static_cast<int>(index), maxStateCount);
	int lineState = 0;
	int count = backCount;
	while (count != 0) {
		--count;
		--index;
		int state = states[index];
		if (state) {
			state -= baseStyle;
		}
		lineState = (lineState << valueBit) | state;
	}
	lineState = (lineState << countBit) | backCount;
	return lineState;
}

template<int valueBit, int maxStateCount, int countBit, int baseStyle>
void UnpackLineState(int lineState, std::vector<int>& states) {
	constexpr int valueMask = (1 << valueBit) - 1;
	constexpr int countMask = (1 << countBit) - 1;
	int count = lineState & countMask;
	lineState >>= countBit;
	count = sci::min(count, maxStateCount);
	while (count != 0) {
		int state = lineState & valueMask;
		if (state) {
			state += baseStyle;
		}
		states.push_back(state);
		lineState >>= valueBit;
		--count;
	}
}

enum {
	DefaultNestedStateValueBit = 3,
	DefaultMaxNestedStateCount = 4,
	DefaultNestedStateCountBit = 3,
	DefaultNestedStateBaseStyle = 10,
};

int PackLineState(const std::vector<int>& states) noexcept;
void UnpackLineState(int lineState, std::vector<int>& states);

template <typename T>
inline T TryGetFront(std::vector<T>& states, T value = {}) noexcept {
	return states.empty() ? value : states.front();
}

template <typename T>
inline T TryGetBack(std::vector<T>& states, T value = {}) noexcept {
	return states.empty() ? value : states.back();
}

template <typename T>
inline T TryTakeAndPop(std::vector<T>& states, T value = {}) {
	if (!states.empty()) {
		value = states.back();
		states.pop_back();
	}
	return value;
}

template <typename T>
inline void TryPopBack(std::vector<T>& states) {
	if (!states.empty()) {
		states.pop_back();
	}
}

template <typename T>
inline T TakeAndPop(std::vector<T>& states) {
	const T value = states.back();
	states.pop_back();
	return value;
}

template <typename T>
inline T TryPopAndPeek(std::vector<T>& states, T value = {}) {
	if (!states.empty()) {
		states.pop_back();
		if (!states.empty()) {
			value = states.back();
		}
	}
	return value;
}

}
