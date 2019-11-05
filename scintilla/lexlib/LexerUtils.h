#ifndef LEXERUTILS_H
#define LEXERUTILS_H

namespace Scintilla {

// TODO: change packed line state to SparseState

template<int bitCount, int maxStateCount, int PackState(int state) noexcept>
int PackLineState(const std::vector<int>& states) {
	int lineState = 0;
	int count = 0;
	size_t index = states.size();
	while (count < maxStateCount && index != 0) {
		++count;
		--index;
		lineState = (lineState << bitCount) | PackState(states[index]);
	}
	return lineState;
}

template<int bitCount, int maxStateCount, int UnpackState(int state) noexcept>
void UnpackLineState(int lineState, int count, std::vector<int>& states) {
	constexpr int mask = (1 << bitCount) - 1;
	count = (count > maxStateCount)? maxStateCount : count;
	while (count > 0) {
		states.push_back(UnpackState(lineState & mask));
		lineState >>= bitCount;
		--count;
	}
}

}

#endif // LEXERUTILS_H
