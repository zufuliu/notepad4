// Scintilla source code edit control
/** @file SplitVector.h
 ** Main data structure for holding arrays that handle insertions
 ** and deletions efficiently.
 **/
// Copyright 1998-2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.
#pragma once

#ifdef NDEBUG
#define ENABLE_SHOW_DEBUG_INFO	0
#else
#define ENABLE_SHOW_DEBUG_INFO	0
#endif
#if ENABLE_SHOW_DEBUG_INFO
#include <cstdio>
#endif

namespace Scintilla::Internal {

constexpr bool InRangeInclusive(size_t index, size_t length) noexcept {
	return index <= length;
}

constexpr bool InRangeExclusive(size_t index, size_t length) noexcept {
	return index < length;
}

constexpr bool IsValidIndex(size_t index, size_t length) noexcept {
	return index < length;
}

template <typename T>
class SplitVector {
protected:
	std::vector<T> body;
	ptrdiff_t lengthBody = 0;
	ptrdiff_t part1Length = 0;
	ptrdiff_t gapLength = 0;	/// invariant: gapLength == body.size() - lengthBody
	size_t growSize;

	/// Move the gap to a particular position so that insertion and
	/// deletion at that point will not require much copying and
	/// hence be fast.
	void GapTo(ptrdiff_t position) noexcept {
		if (position != part1Length) {
			try {
				if (gapLength > 0) { // If gap to move
					// This can never fail but std::move and std::move_backward are not noexcept.
					if (position < part1Length) {
						// Moving the gap towards start so moving elements towards end
						std::move_backward(
							body.data() + position,
							body.data() + part1Length,
							body.data() + gapLength + part1Length);
					} else {	// position > part1Length
						// Moving the gap towards end so moving elements towards start
						std::move(
							body.data() + part1Length + gapLength,
							body.data() + gapLength + position,
							body.data() + part1Length);
					}
				}
				part1Length = position;
			} catch (...) {
				// Ignore any exception
			}
		}
	}

	/// Check that there is room in the buffer for an insertion,
	/// reallocating if more space needed.
	void RoomFor(ptrdiff_t insertionLength) {
		if (gapLength < insertionLength) {
			const size_t size = body.size();
			const size_t upper = size / 6;
			while (growSize < upper) {
				growSize *= 2;
			}
			ReAllocate(size + insertionLength + growSize);
		}
	}

	void Init() {
		body.clear();
		body.shrink_to_fit();
		lengthBody = 0;
		part1Length = 0;
		gapLength = 0;
		growSize = 8;
	}

public:
	/// Construct a split buffer.
	SplitVector(size_t growSize_ = 8) noexcept : growSize{growSize_} {}

	size_t GetGrowSize() const noexcept {
		return growSize;
	}

	void SetGrowSize(size_t growSize_) noexcept {
		growSize = growSize_;
	}

	/// Reallocate the storage for the buffer to be newSize and
	/// copy existing contents to the new buffer.
	/// Must not be used to decrease the size of the buffer.
	void ReAllocate(size_t newSize) {
		const size_t size = body.size();
		if (newSize > size) {
#if ENABLE_SHOW_DEBUG_INFO
			printf("before %s(%td, %zu) part1Length=%td, gapLength=%td, lengthBody=%td, growSize=%zu\n",
				__func__, newSize, size, part1Length, gapLength, lengthBody, growSize);
#endif
			// Move the gap to the end
			GapTo(lengthBody);
			gapLength += newSize - size;
			// RoomFor implements a growth strategy but so does vector::resize so
			// ensure vector::resize allocates exactly the amount wanted by
			// calling reserve first.
			body.reserve(newSize);
			body.resize(newSize);
#if ENABLE_SHOW_DEBUG_INFO
			printf("after %s(%td, %zu) part1Length=%td, gapLength=%td, lengthBody=%td, growSize=%zu\n",
				__func__, newSize, size, part1Length, gapLength, lengthBody, growSize);
#endif
		}
	}

	/// Retrieve the element at a particular position.
	/// Retrieving positions outside the range of the buffer returns empty or 0.
	T ValueAt(ptrdiff_t position) const noexcept {
		if (IsValidIndex(position, part1Length)) {
			return body[position];
		}
		if (IsValidIndex(position, lengthBody)) {
			return body[gapLength + position];
		}
		return {};
	}

	const T& ValueOr(ptrdiff_t position, const T& empty) const noexcept {
		if (IsValidIndex(position, part1Length)) {
			return body[position];
		}
		if (IsValidIndex(position, lengthBody)) {
			return body[gapLength + position];
		}
		return empty;
	}

	/// Set the element at a particular position.
	/// Setting positions outside the range of the buffer performs no assignment
	/// but asserts in debug builds.
	template <typename ParamType>
	void SetValueAt(ptrdiff_t position, ParamType&& v) noexcept {
		PLATFORM_ASSERT(position >= 0 && position < lengthBody);
		if (IsValidIndex(position, part1Length)) {
			body[position] = std::forward<ParamType>(v);
		} else if (IsValidIndex(position, lengthBody)) {
			body[gapLength + position] = std::forward<ParamType>(v);
		}
	}

	template <typename ParamType>
	bool UpdateValueAt(ptrdiff_t position, ParamType&& v) noexcept {
		PLATFORM_ASSERT(position >= 0 && position < lengthBody);
		if (IsValidIndex(position, lengthBody)) {
			T * const data = ElementPointer(position);
			const T current = std::forward<ParamType>(v);
			if (current != *data) {
				*data = current;
				return true;
			}
		}
		return false;
	}

	template <typename ParamType>
	T ReplaceValueAt(ptrdiff_t position, ParamType&& v) noexcept {
		PLATFORM_ASSERT(position >= 0 && position < lengthBody);
		T * const data = ElementPointer(position);
		const T previous = *data;
		*data = std::forward<ParamType>(v);
		return previous;
	}

	/// Retrieve the element at a particular position.
	/// The position must be within bounds or an assertion is triggered.
	const T &operator[](ptrdiff_t position) const noexcept {
		PLATFORM_ASSERT(position >= 0 && position < lengthBody);
		if (position < part1Length) {
			return body[position];
		} else {
			return body[gapLength + position];
		}
	}

	/// Retrieve reference to the element at a particular position.
	/// This, instead of the const variant, can be used to mutate in-place.
	/// The position must be within bounds or an assertion is triggered.
	T &operator[](ptrdiff_t position) noexcept {
		PLATFORM_ASSERT(position >= 0 && position < lengthBody);
		if (position < part1Length) {
			return body[position];
		} else {
			return body[gapLength + position];
		}
	}

	/// Retrieve the length of the buffer.
	ptrdiff_t Length() const noexcept {
		return lengthBody;
	}

	/// Insert a single value into the buffer.
	/// Inserting at positions outside the current range fails.
	void Insert(ptrdiff_t position, T v) {
		PLATFORM_ASSERT((position >= 0) && (position <= lengthBody));
		if (!InRangeInclusive(position, lengthBody)) {
			return;
		}
		RoomFor(1);
		GapTo(position);
		body[part1Length] = std::move(v);
		lengthBody++;
		part1Length++;
		gapLength--;
	}

	/// Insert a number of elements into the buffer setting their value.
	/// Inserting at positions outside the current range fails.
	void InsertValue(ptrdiff_t position, ptrdiff_t insertLength, T v) {
		PLATFORM_ASSERT((position >= 0) && (position <= lengthBody));
		if (insertLength > 0) {
			if (!InRangeInclusive(position, lengthBody)) {
				return;
			}
			RoomFor(insertLength);
			GapTo(position);
			std::fill_n(body.data() + part1Length, insertLength, v);
			lengthBody += insertLength;
			part1Length += insertLength;
			gapLength -= insertLength;
		}
	}

	/// Add some new empty elements.
	/// InsertValue is good for value objects but not for unique_ptr objects
	/// since they can only be moved from once.
	/// Callers can write to the returned pointer to transform inputs without copies.
	T *InsertEmpty(ptrdiff_t position, ptrdiff_t insertLength) {
		PLATFORM_ASSERT((position >= 0) && (position <= lengthBody));
		if (insertLength > 0) {
			if (!InRangeInclusive(position, lengthBody)) {
				return nullptr;
			}
			RoomFor(insertLength);
			GapTo(position);
			T *ptr = body.data() + part1Length;
			//std::uninitialized_value_construct_n(ptr, insertLength);
			if constexpr (std::is_scalar_v<T>) {
				memset(ptr, 0, insertLength*sizeof(T));
			} else {
				static_assert(std::is_nothrow_default_constructible_v<T>);
				for (ptrdiff_t elem = 0; elem < insertLength; elem++, ptr++) {
					::new (ptr)T();
				}
			}
			lengthBody += insertLength;
			part1Length += insertLength;
			gapLength -= insertLength;
		}
		return body.data() + position;
	}

	/// Ensure at least length elements allocated,
	/// appending zero valued elements if needed.
	void EnsureLength(ptrdiff_t wantedLength) {
		if (Length() < wantedLength) {
			InsertEmpty(Length(), wantedLength - Length());
		}
	}

	/// Insert text into the buffer from an array.
	void InsertFromArray(ptrdiff_t positionToInsert, const T s[], ptrdiff_t positionFrom, ptrdiff_t insertLength) {
		PLATFORM_ASSERT((positionToInsert >= 0) && (positionToInsert <= lengthBody));
		if (insertLength > 0) {
			if (!InRangeInclusive(positionToInsert, lengthBody)) {
				return;
			}
			RoomFor(insertLength);
			GapTo(positionToInsert);
			if constexpr (__is_standard_layout(T)) {
				memcpy(body.data() + part1Length, s + positionFrom, insertLength*sizeof(T));
			} else {
				std::copy_n(s + positionFrom, insertLength, body.data() + part1Length);
			}
			lengthBody += insertLength;
			part1Length += insertLength;
			gapLength -= insertLength;
		}
	}

	/// Delete one element from the buffer.
	void Delete(ptrdiff_t position) {
		PLATFORM_ASSERT((position >= 0) && (position < lengthBody));
		DeleteRange(position, 1);
	}

	/// Delete a range from the buffer.
	/// Deleting positions outside the current range fails.
	/// Cannot be noexcept as vector::shrink_to_fit may be called and it may throw.
	void DeleteRange(ptrdiff_t position, ptrdiff_t deleteLength) {
		PLATFORM_ASSERT((position >= 0) && (position + deleteLength <= lengthBody));
		if ((position == 0) && (deleteLength == lengthBody)) {
			// Full deallocation returns storage and is faster
			Init();
		} else if (position >= 0 && deleteLength > 0 && (position + deleteLength) <= lengthBody) {
			GapTo(position);
			lengthBody -= deleteLength;
			gapLength += deleteLength;
		}
	}

	/// Delete all the buffer contents.
	void DeleteAll() {
		DeleteRange(0, lengthBody);
	}

	/// Retrieve a range of elements into an array
	void GetRange(T *buffer, ptrdiff_t position, ptrdiff_t retrieveLength) const noexcept {
		// Split into up to 2 ranges, before and after the split then use memcpy on each.
		ptrdiff_t range1Length = 0;
		const T* data = body.data() + position;
		if (position < part1Length) {
			range1Length = std::min(retrieveLength, part1Length - position);
			memcpy(buffer, data, range1Length*sizeof(T));
		}
		if (range1Length < retrieveLength) {
			data += range1Length + gapLength;
			const ptrdiff_t range2Length = retrieveLength - range1Length;
			memcpy(buffer + range1Length, data, range2Length*sizeof(T));
		}
	}

	int CheckRange(const T *buffer, ptrdiff_t position, ptrdiff_t rangeLength) const noexcept {
		// Split into up to 2 ranges, before and after the split then use memcmp on each.
		ptrdiff_t range1Length = 0;
		int result = 0;
		const T* data = body.data() + position;
		if (position < part1Length) {
			range1Length = std::min(rangeLength, part1Length - position);
			result = memcmp(buffer, data, range1Length*sizeof(T));
		}
		if (range1Length < rangeLength) {
			data += range1Length + gapLength;
			const ptrdiff_t range2Length = rangeLength - range1Length;
			// NOLINTNEXTLINE(bugprone-suspicious-string-compare)
			result |= memcmp(buffer + range1Length, data, range2Length*sizeof(T));
		}
		return result;
	}

	/// Compact the buffer and return a pointer to the first element.
	/// Also ensures there is an empty element beyond logical end in case its
	/// passed to a function expecting a NUL terminated string.
	const T *BufferPointer() {
		RoomFor(1);
		GapTo(lengthBody);
		T emptyOne = {};
		body[lengthBody] = std::move(emptyOne);
		return body.data();
	}

	/// Return a pointer to a range of elements, first rearranging the buffer if
	/// needed to make that range contiguous.
	const T *RangePointer(ptrdiff_t position, ptrdiff_t rangeLength) noexcept {
		const T *data = body.data() + position;
		if (position < part1Length) {
			if ((position + rangeLength) > part1Length) {
				// Range overlaps gap, so move gap to start of range.
				GapTo(position);
				data += gapLength;
			}
		} else {
			data += gapLength;
		}
		return data;
	}

	T *ElementPointer(ptrdiff_t position) noexcept {
		T *data = body.data() + position;
		if (position >= part1Length) {
			data += gapLength;
		}
		return data;
	}

	/// Return a pointer to a single element.
	/// Does not rearrange the buffer.
	const T *ElementPointer(ptrdiff_t position) const noexcept {
		const T *data = body.data() + position;
		if (position >= part1Length) {
			data += gapLength;
		}
		return data;
	}

	/// Return the position of the gap within the buffer.
	ptrdiff_t GapPosition() const noexcept {
		return part1Length;
	}
};

}
