// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
#pragma once

namespace Lexilla {

// TODO: change packed line state to NestedStateStack (convert lexer to class).

template<int valueBit, int maxStateCount, int countBit, int baseStyle>
int PackLineState(const std::vector<int>& states) noexcept {
	constexpr size_t countMask = (1 << countBit) - 1;
	size_t index = states.size();
	int count = static_cast<int>(sci::min(index, countMask));
	int lineState = count;
	lineState <<= countBit;
	count = sci::min(count, maxStateCount);
	while (count != 0) {
		--count;
		--index;
		int state = states[index];
		if (state) {
			state -= baseStyle;
		}
		lineState = (lineState << valueBit) | state;
	}
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

#if 0

// nested state stack on each line
using NestedStateStack = std::map<Sci_Line, std::vector<int>>;

inline void GetNestedState(const NestedStateStack& stateStack, Sci_Line line, std::vector<int>& states) {
	const auto it = stateStack.find(line);
	if (it != stateStack.end()) {
		states = it->second;
	}
}

inline void SaveNestedState(NestedStateStack& stateStack, Sci_Line line, const std::vector<int>& states) {
	if (states.empty()) {
		auto it = stateStack.find(line);
		if (it != stateStack.end()) {
			stateStack.erase(it);
		}
	} else {
		stateStack[line] = states;
	}
}

#endif

}
