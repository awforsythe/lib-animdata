#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cinttypes>

struct ad_buffer
{
	size_t capacity; // Number of floats that can be stored in the data buffer
	size_t size; // Number of floats actually stored in the data buffer
	float* data; // Contiguous buffer whose length == capacity

	ad_buffer();
	~ad_buffer();

	bool init(size_t initial_capacity);
	float* resize_for_edit(size_t i, int32_t delta_size);
};
