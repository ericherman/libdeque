/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-push-pop.c */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

unsigned test_deque_push_pop(void)
{
	size_t i;
	unsigned failures = 0;
	struct deque *d;

	d = deque_new();
	if (!d) {
		check_int(d != NULL ? 1 : 0, 1);
		++failures;
		return failures;
	}

	failures += check_size_t_m(deque_size(d), 0, "initial size");

	failures += check_ptr_m(deque_pop(d), NULL, "pop size 0");

	deque_push(d, "one");
	deque_push(d, "two");
	deque_push(d, "three");

	failures += check_size_t_m(deque_size(d), 3, "size A");

	deque_unshift(d, "zero");

	failures += check_size_t_m(deque_size(d), 4, "size B");

	failures += check_str_m((char *)deque_pop(d), "three", "pop1");

	deque_push(d, "four");

	failures += check_str_m((char *)deque_shift(d), "zero", "shift1");
	failures += check_str_m((char *)deque_shift(d), "one", "shift2");

	failures += check_str_m((char *)deque_pop(d), "four", "pop2");
	failures += check_str_m((char *)deque_pop(d), "two", "pop3");

	for (i = 0; i < 100; ++i) {
		deque_push(d, "foo");
	}
	for (i = 0; i < 100; ++i) {
		deque_pop(d);
	}
	for (i = 0; i < 1000; ++i) {
		deque_unshift(d, "foo");
	}
	deque_clear(d);

	failures += check_size_t_m(deque_size(d), 0, "size C");

	failures += check_str_m((char *)deque_shift(d), NULL, "shift3");
	failures += check_ptr_m(deque_unshift(d, "foo"), d, "unshift");
	failures += check_str_m((char *)deque_shift(d), "foo", "shift4");

	deque_free(d);
	return failures;
}

ECHECK_TEST_MAIN(test_deque_push_pop)
