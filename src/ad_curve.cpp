#include "ad_curve.h"

#include <cstdio>
#include <cassert>

ad_curve::ad_curve(size_t in_cardinality)
	: cardinality(in_cardinality)
	, num_keys(0)
	, times()
	, values()
{
	assert(cardinality > 0);
}

bool ad_curve::init(size_t initial_capacity)
{
	assert(initial_capacity > 0);
	return times.init(initial_capacity) && values.init(initial_capacity * cardinality);
}

void ad_curve::shift(float from_time, float to_time, float delta_time)
{
	int32_t n = 0;
	int32_t from_i = find_inclusive_range(from_time, to_time, n);
}

int32_t ad_curve::find_nearest_lte(float at_time) const
{
	// Use -1 as a sentinel if there are no keys <= the search time
	int32_t i = -1;

	// Start a binary search encompassing the entire times array
	int32_t lo = 0;
	int32_t hi = static_cast<int32_t>(times.size) - 1;
	while (lo <= hi)
	{
		// Examine the time value in the middle of the current search space
		const int32_t mid = lo + (hi - lo) / 2;
		const float time = times.data[mid];
		if (time <= at_time)
		{
			// If this time is <= the search time, update our result and keep searching to the right
			i = mid;
			lo = mid + 1;
		}
		else
		{
			// If this key is past the search time, keep searching to the left
			hi = mid - 1;
		}
	}
	return i;
}

int32_t ad_curve::find_inclusive_range(float from_time, float to_time, int32_t& out_n) const
{
	assert(to_time >= from_time);

	// Run a binary search to find the rightmost key < from_time
	int32_t i_lt_from = -1;
	int32_t lo = 0;
	int32_t hi = static_cast<int32_t>(times.size) - 1;
	while (lo <= hi)
	{
		const int32_t mid = lo + (hi - lo) / 2;
		const float time = times.data[mid];
		if (time < from_time)
		{
			i_lt_from = mid;
			lo = mid + 1;
		}
		else
		{
			hi = mid - 1;
		}
	}

	// If the next key is > to_time, our range sits between two keys
	if (i_lt_from >= 0 && static_cast<size_t>(i_lt_from) + 1 < times.size && times.data[i_lt_from + 1] > to_time)
	{
		out_n = 0;
		return -1;
	}

	// Run another search in the range to the right, to find the leftmost key > to_time
	int32_t i_gt_to = -1;
	lo = i_lt_from + 1;
	hi = static_cast<int32_t>(times.size) - 1;
	while (lo <= hi)
	{
		const int32_t mid = lo + (hi - lo) / 2;
		const float time = times.data[mid];
		if (time <= to_time)
		{
			i_gt_to = mid + 1;
			lo = mid + 1;
		}
		else
		{
			hi = mid - 1;
		}
	}

	if (i_gt_to == -1)
	{
		out_n = 0;
		return -1;
	}

	const int32_t i = i_lt_from + 1;
	out_n = i_gt_to - i;
	return i;
}
