#pragma once

namespace Scintilla {

// TODO: change packed line state to NestedStateStack (convert lexer to class).

template<int valueBit, int maxStateCount, int countBit, int PackState(int state) noexcept>
inline int PackLineState(const std::vector<int>& states) noexcept {
	constexpr uint32_t countMask = (1 << countBit) - 1;
	size_t index = states.size();
	int count = static_cast<int>((index > countMask) ? countMask : index);
	int lineState = count;
	lineState <<= countBit;
	while (count < maxStateCount && index != 0) {
		++count;
		--index;
		lineState = (lineState << valueBit) | PackState(states[index]);
	}
	return lineState;
}

template<int valueBit, int maxStateCount, int countBit, int UnpackState(int state) noexcept>
inline void UnpackLineState(int lineState, std::vector<int>& states) {
	constexpr int mask = (1 << valueBit) - 1;
	constexpr int countMask = (1 << countBit) - 1;
	int count = lineState & countMask;
	lineState >>= countBit;
	count = (count > maxStateCount)? maxStateCount : count;
	while (count != 0) {
		states.push_back(UnpackState(lineState & mask));
		lineState >>= valueBit;
		--count;
	}
}

inline int TakeAndPop(std::vector<int>& states) {
	const int value = states.back();
	states.pop_back();
	return value;
}

inline int TryPopAndPeek(std::vector<int>& states, int value = 0) {
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
using NestedStateStack = std::map<Sci_Position, std::vector<int>>;

inline void GetNestedState(const NestedStateStack& stateStack, Sci_Position line, std::vector<int>& states) {
	const auto it = stateStack.find(line);
	if (it != stateStack.end()) {
		states = it->second;
	}
}

inline void SaveNestedState(NestedStateStack& stateStack, Sci_Position line, const std::vector<int>& states) {
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
