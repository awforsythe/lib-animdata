#pragma once

#include "testing.h"
#include "ad_curve.h"

const char* test_curve_init()
{
	ad_curve curve(2);
	t_assert(curve.cardinality == 2);
	const bool init_ok = curve.init(8);
	t_assert(init_ok);
	t_assert(curve.num_keys == 0);
	t_assert(curve.times.capacity == 8);
	t_assert(curve.times.size == 0);
	t_assert(curve.values.capacity == 16);
	t_assert(curve.values.size == 0);

	return nullptr;
}

const char* test_curve_find_nearest_lte()
{
	// Initialize a curve so we can give it some time keys
	ad_curve curve(1);
	const bool init_ok = curve.init(8);
	t_assert(init_ok);

	// Result should always be -1 with no keys
	t_assert(curve.find_nearest_lte(0.f) == -1);
	t_assert(curve.find_nearest_lte(5.f) == -1);
	t_assert(curve.find_nearest_lte(99.f) == -1);

	// Populate the time curve with an ordered series of keys
	float* ptr = curve.times.resize_for_edit(0, 8);
	for (size_t i = 0; i < curve.times.size; i++) {
		*ptr++ = static_cast<float>(i);
	}
	t_assert_floats(curve.times.data, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f);

	// Found index should always be the position of the key that's closest without going over
	t_assert(curve.find_nearest_lte(0.f) == 0);
	t_assert(curve.find_nearest_lte(0.0001f) == 0);
	t_assert(curve.find_nearest_lte(0.9999f) == 0);
	t_assert(curve.find_nearest_lte(1.f) == 1);
	t_assert(curve.find_nearest_lte(1.9999f) == 1);
	t_assert(curve.find_nearest_lte(3.3333f) == 3);
	t_assert(curve.find_nearest_lte(6.6666f) == 6);

	// OOB on the left should be -1 (no key is <= the search time)
	t_assert(curve.find_nearest_lte(-50.f) == -1);

	// OOB on the right should be index of last key
	t_assert(curve.find_nearest_lte(50.f) == curve.times.size - 1);

	return nullptr;
}

const char* test_curve_find_inclusive_range()
{
	// Initialize a curve so we can give it some time keys
	ad_curve curve(1);
	const bool init_ok = curve.init(8);
	t_assert(init_ok);
	int32_t n = -1;

	// Result should always be -1 with no keys
	t_assert(curve.find_inclusive_range(-50.f, 50.f, n) == -1 && n == 0);
	t_assert(curve.find_inclusive_range(0.f, 0.f, n) == -1 && n == 0);

	// Populate the time curve with an ordered series of keys
	float* ptr = curve.times.resize_for_edit(0, 8);
	for (size_t i = 0; i < curve.times.size; i++) {
		*ptr++ = static_cast<float>(i);
	}
	t_assert_floats(curve.times.data, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f);

	t_assert(curve.find_inclusive_range(0.f, 7.f, n) == 0 && n == 8);
	t_assert(curve.find_inclusive_range(2.5f, 6.1f, n) == 3 && n == 4);
	t_assert(curve.find_inclusive_range(-50.f, 2.f, n) == 0 && n == 3);
	t_assert(curve.find_inclusive_range(5.f, 50.f, n) == 5 && n == 3);
	t_assert(curve.find_inclusive_range(2.f, 2.f, n) == 2 && n == 1);
	t_assert(curve.find_inclusive_range(2.5f, 2.6f, n) == -1 && n == 0);
	t_assert(curve.find_inclusive_range(2.f, 2.5f, n) == 2 && n == 1);
	t_assert(curve.find_inclusive_range(2.5f, 3.f, n) == 3 && n == 1);
	t_assert(curve.find_inclusive_range(-50.f, -40.f, n) == -1 && n == 0);
	t_assert(curve.find_inclusive_range(-50.f, 0.f, n) == 0 && n == 1);
	t_assert(curve.find_inclusive_range(40.f, 50.f, n) == -1 && n == 0);
	t_assert(curve.find_inclusive_range(7.f, 50.f, n) == 7 && n == 1);
	t_assert(curve.find_inclusive_range(0.f, 0.f, n) == 0 && n == 1);
	t_assert(curve.find_inclusive_range(7.f, 7.f, n) == 7 && n == 1);

	return nullptr;
}

const char* test_curve_set()
{
	ad_curve curve(1);
	const bool init_ok = curve.init(8);
	t_assert(init_ok);
	float v;

	v = 0.0f; curve.set(0.0f, &v);
	t_assert(curve.num_keys == 1);
	t_assert_floats(curve.times.data, 0.0f);
	t_assert_floats(curve.values.data, 0.0f);

	v = 1.0f; curve.set(1.0f, &v);
	t_assert(curve.num_keys == 2);
	t_assert_floats(curve.times.data, 0.0f, 1.0f);
	t_assert_floats(curve.values.data, 0.0f, 1.0f);

	v = 10.0f; curve.set(2.0f, &v);
	t_assert(curve.num_keys == 3);
	t_assert_floats(curve.times.data, 0.0f, 1.0f, 2.0f);
	t_assert_floats(curve.values.data, 0.0f, 1.0f, 10.0f);

	v = 5.0f; curve.set(1.5f, &v);
	t_assert(curve.num_keys == 4);
	t_assert_floats(curve.times.data, 0.0f, 1.0f, 1.5f, 2.0f);
	t_assert_floats(curve.values.data, 0.0f, 1.0f, 5.0f, 10.0f);

	v = 999.9f; curve.set(1.0f, &v);
	t_assert(curve.num_keys == 4);
	t_assert_floats(curve.times.data, 0.0f, 1.0f, 1.5f, 2.0f);
	t_assert_floats(curve.values.data, 0.0f, 999.9f, 5.0f, 10.0f);

	v = 42.0f; curve.set(-50.0f, &v);
	t_assert(curve.num_keys == 5);
	t_assert_floats(curve.times.data, -50.0f, 0.0f, 1.0f, 1.5f, 2.0f);
	t_assert_floats(curve.values.data, 42.0f, 0.0f, 999.9f, 5.0f, 10.0f);

	return nullptr;
}
