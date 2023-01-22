#pragma once

#include <cstdlib>
#include <cassert>

#include "ad_buffer.h"

struct ad_curve
{
	size_t cardinality;
	size_t num_keys;

	ad_buffer times;
	ad_buffer values;

	ad_curve(size_t in_cardinality);

	bool init(size_t initial_capacity);
    void set(float time, float* value);
	void shift(float from_time, float to_time, float delta_time);

	int32_t find_nearest_lte(float at_time) const;
	int32_t find_inclusive_range(float from_time, float to_time, int32_t& out_n) const;
};
