#include <cstdio>

#include "testing.h"
#include "ad_buffer_tests.h"
#include "ad_curve_tests.h"

int main(void)
{
	t_begin();

	t_run(test_buffer_init);
	t_run(test_buffer_remove_one);
	t_run(test_buffer_remove_many_left);
	t_run(test_buffer_remove_many_right);
	t_run(test_buffer_remove_all);
	t_run(test_buffer_add_one);
	t_run(test_buffer_add_many_left);
	t_run(test_buffer_add_many_right);
	t_run(test_buffer_resize);
	t_run(test_buffer_resize_noshrink);

	t_run(test_curve_init);
	t_run(test_curve_find_nearest_lte);
	t_run(test_curve_find_inclusive_range);
	t_run(test_curve_set);
	t_run(test_curve_remove_at);
	t_run(test_curve_evaluate);

	t_end();
}
