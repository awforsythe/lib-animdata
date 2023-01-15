#pragma once

#include <cstdio>

#include "testing.h"
#include "ad_buffer.h"

const char* _init_buffer(ad_buffer& buf)
{
	const bool init_ok = buf.init(8);
	t_assert(init_ok);
	t_assert(buf.data != nullptr);
	t_assert(buf.capacity == 8);
	t_assert(buf.size == 0);

	float* ptr = buf.resize_for_edit(0, 6);
	t_assert(ptr != nullptr);
	t_assert(buf.capacity == 8);
	t_assert(buf.size == 6);

	for (int i = 0; i < 6; i++) {
		*ptr++ = static_cast<float>(i);
	}
	t_assert_floats(buf.data, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f);

	return nullptr;
}

#define init_buffer(buf) \
	const char* init_err = _init_buffer(buf); \
	if (init_err) { \
		return init_err; \
	}

const char* test_buffer_init()
{
	ad_buffer buf;
	init_buffer(buf);
	return nullptr;
}

const char* test_buffer_remove_one()
{
	ad_buffer buf;
	init_buffer(buf);
	float* const initial_data = buf.data;

	float* ptr = buf.resize_for_edit(2, -1);
	t_assert(ptr == initial_data + 2);
	t_assert(buf.capacity == 8);
	t_assert(buf.size == 5);
	t_assert_floats(buf.data, 0.f, 1.f, 3.f, 4.f, 5.f, 5.f);

	return nullptr;
}

const char* test_buffer_remove_many_left()
{
	ad_buffer buf;
	init_buffer(buf);
	float* const initial_data = buf.data;

	float* ptr = buf.resize_for_edit(0, -4);
	t_assert(ptr == initial_data);
	t_assert(buf.capacity == 8);
	t_assert(buf.size == 2);
	t_assert_floats(buf.data, 4.f, 5.f, 2.f, 3.f, 4.f, 5.f);

	return nullptr;
}

const char* test_buffer_remove_many_right()
{
	ad_buffer buf;
	init_buffer(buf);
	float* const initial_data = buf.data;

	float* ptr = buf.resize_for_edit(2, -4);
	t_assert(ptr == initial_data + 2);
	t_assert(buf.capacity == 8);
	t_assert(buf.size == 2);
	t_assert_floats(buf.data, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f);

	return nullptr;
}

const char* test_buffer_remove_all()
{
	ad_buffer buf;
	init_buffer(buf);
	float* const initial_data = buf.data;

	float* ptr = buf.resize_for_edit(0, -6);
	t_assert(ptr == initial_data);
	t_assert(buf.capacity == 8);
	t_assert(buf.size == 0);
	t_assert_floats(buf.data, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f);

	return nullptr;
}

const char* test_buffer_add_one()
{
	ad_buffer buf;
	init_buffer(buf);
	float* const initial_data = buf.data;

	float* ptr = buf.resize_for_edit(3, 1);
	t_assert(ptr == initial_data + 3);
	t_assert(buf.capacity == 8);
	t_assert(buf.size == 7);
	*ptr = 100.f;
	t_assert_floats(buf.data, 0.f, 1.f, 2.f, 100.f, 3.f, 4.f, 5.f);
	
	return nullptr;
}

const char* test_buffer_add_many_left()
{
	ad_buffer buf;
	init_buffer(buf);
	float* const initial_data = buf.data;

	float* ptr = buf.resize_for_edit(0, 2);
	t_assert(ptr == initial_data);
	t_assert(buf.capacity == 8);
	t_assert(buf.size == 8);
	*ptr++ = 100.f;
	*ptr = 200.f;
	t_assert_floats(buf.data, 100.f, 200.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f);

	return nullptr;
}

const char* test_buffer_add_many_right()
{
	ad_buffer buf;
	init_buffer(buf);
	float* const initial_data = buf.data;

	float* ptr = buf.resize_for_edit(6, 2);
	t_assert(ptr == initial_data + 6);
	t_assert(buf.capacity == 8);
	t_assert(buf.size == 8);
	*ptr++ = 100.f;
	*ptr = 200.f;
	t_assert_floats(buf.data, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 100.f, 200.f);

	return nullptr;
}

const char* test_buffer_resize()
{
	ad_buffer buf;
	init_buffer(buf);

	float* ptr = buf.resize_for_edit(3, 4);
	t_assert(buf.capacity == 16);
	t_assert(buf.size == 10);
	*ptr++ = 100.f;
	*ptr++ = 200.f;
	*ptr++ = 300.f;
	*ptr = 400.f;
	t_assert_floats(buf.data, 0.f, 1.f, 2.f, 100.f, 200.f, 300.f, 400.f, 3.f, 4.f, 5.f);

	return nullptr;
}

const char* test_buffer_resize_noshrink()
{
	ad_buffer buf;
	init_buffer(buf);

	float* ptr = buf.resize_for_edit(3, 4);
	float* const expanded_data = buf.data;
	t_assert(buf.capacity == 16);
	t_assert(buf.size == 10);
	*ptr++ = 100.f;
	*ptr++ = 200.f;
	*ptr++ = 300.f;
	*ptr = 400.f;

	float* ptr2 = buf.resize_for_edit(1, -5);
	t_assert(ptr2 == buf.data + 1);
	t_assert(buf.data == expanded_data);
	t_assert(buf.capacity == 16);
	t_assert(buf.size == 5);
	t_assert_floats(buf.data, 0.f, 400.f, 3.f, 4.f, 5.f);

	return nullptr;
}
