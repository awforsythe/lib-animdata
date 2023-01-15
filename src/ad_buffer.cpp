#include "ad_buffer.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

ad_buffer::ad_buffer()
	: capacity(0)
	, size(0)
	, data(nullptr)
{
}

ad_buffer::~ad_buffer()
{
	free(data);
}

bool ad_buffer::init(size_t initial_capacity)
{
	assert(initial_capacity > 0);

	capacity = initial_capacity;
	data = reinterpret_cast<float*>(malloc(capacity * sizeof(float)));
	return data != nullptr;
}

float* ad_buffer::resize_for_edit(size_t i, int32_t delta_size)
{
	// We should've called init before attempting to make edits
	assert(data && capacity > 0);

	// Input arguments must fit within the bounds of the existing data
	assert(i <= size);
	assert(delta_size != 0);
	assert(size + delta_size >= 0);

	// If this is a cut, chop out the desired length after the edit point
	if (delta_size < 0)
	{
		// Just shift the tail left to the edit point and reduce size
		float* tail_start = data + i - delta_size;
		float* tail_end = data + size;
		const size_t num_tail_bytes = (tail_end - tail_start) * sizeof(float);
		memmove(data + i, tail_start, num_tail_bytes);
		size += delta_size;
		return data + i;
	}

	// Otherwise, we want to insert new space in the middle, shifting the tail right
	float* head_start = data;
	float* head_end = data + i;
	float* tail_start = head_end;
	float* tail_end = data + size;
	const size_t num_head_bytes = (head_end - head_start) * sizeof(float);
	const size_t num_tail_bytes = (tail_end - tail_start) * sizeof(float);

	// If we've exceeded our capacity, allocate a new buffer, copy to it, and free
	size += delta_size;
	if (size > capacity)
	{
		// Increase capacity and allocate a new buffer
		capacity += capacity;
		const size_t capacity_bytes = capacity * sizeof(float);
		float* new_data = reinterpret_cast<float*>(malloc(capacity_bytes));
		if (new_data == nullptr)
		{
			return nullptr;
		}

		// Copy both the head and the tail (shifted right) to the new buffer
		memcpy(new_data, head_start, num_head_bytes);
		memcpy(new_data + (tail_start - data) + delta_size, tail_start, num_tail_bytes);

		// Free the old buffer and return the location of the edit point in our new buffer
		free(data);
		data = new_data;
		return data + i;
	}

	// We can fit the new data without reallocating, so just shift the tail rightward
	memmove(data + i + delta_size, tail_start, num_tail_bytes);
	return data + i;
}
