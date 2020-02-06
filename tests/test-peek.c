/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-peek.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "test-deque.h"

int test_peek(void)
{
	int failures = 0;
	struct deque_s *deque;

	deque = deque_new();

	failures += check_ptr_m(deque->peek_top(deque), NULL, "top 0");
	failures += check_ptr_m(deque->peek_bottom(deque), NULL, "bottom 0");

	deque->push(deque, "one");
	deque->push(deque, "two");
	deque->push(deque, "three");

	failures += check_str_m(deque->peek_top(deque), "three", "top 1");
	failures += check_str_m(deque->peek_bottom(deque), "one", "bottom 1");

	failures += check_str_m(deque->peek_top(deque), "three", "top 2");
	failures += check_str_m(deque->peek_bottom(deque), "one", "bottom 2");

	deque->pop(deque);

	failures += check_str_m(deque->peek_top(deque), "two", "top 3");
	failures += check_str_m(deque->peek_bottom(deque), "one", "bottom 3");

	deque->pop(deque);

	failures += check_str_m(deque->peek_top(deque), "one", "top 4");
	failures += check_str_m(deque->peek_bottom(deque), "one", "bottom 4");

	deque->pop(deque);

	failures += check_ptr_m(deque->peek_top(deque), NULL, "top 5");
	failures += check_ptr_m(deque->peek_bottom(deque), NULL, "bottom 5");

	failures += check_ptr_m(deque->peek_top(deque), NULL, "top 6");
	failures += check_ptr_m(deque->peek_bottom(deque), NULL, "bottom 6");

	deque_free(deque);
	return failures;
}

TEST_DEQUE_MAIN(test_peek())
