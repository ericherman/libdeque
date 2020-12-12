/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-push-pop.c */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

unsigned test_deque_push_pop(void)
{
	size_t i;
	unsigned failures = 0;
	deque_s *deque;

	deque = deque_new();
	if (!deque) {
		check_int(deque != NULL ? 1 : 0, 1);
		++failures;
		return failures;
	}

	failures += check_size_t_m(deque->size(deque), 0, "initial size");

	failures += check_ptr_m(deque->pop(deque), NULL, "pop size 0");

	deque->push(deque, "one");
	deque->push(deque, "two");
	deque->push(deque, "three");

	failures += check_size_t_m(deque->size(deque), 3, "size A");

	deque->unshift(deque, "zero");

	failures += check_size_t_m(deque->size(deque), 4, "size B");

	failures += check_str_m((char *)deque->pop(deque), "three", "pop1");

	deque->push(deque, "four");

	failures += check_str_m((char *)deque->shift(deque), "zero", "shift1");
	failures += check_str_m((char *)deque->shift(deque), "one", "shift2");

	failures += check_str_m((char *)deque->pop(deque), "four", "pop2");
	failures += check_str_m((char *)deque->pop(deque), "two", "pop3");

	for (i = 0; i < 100; ++i) {
		deque->push(deque, "foo");
	}
	for (i = 0; i < 100; ++i) {
		deque->pop(deque);
	}
	for (i = 0; i < 1000; ++i) {
		deque->unshift(deque, "foo");
	}
	deque->clear(deque);

	failures += check_size_t_m(deque->size(deque), 0, "size C");

	failures += check_str_m((char *)deque->shift(deque), NULL, "shift3");
	failures += check_ptr_m(deque->unshift(deque, "foo"), deque, "unshift");
	failures += check_str_m((char *)deque->shift(deque), "foo", "shift4");

	deque_free(deque);
	return failures;
}

ECHECK_TEST_MAIN(test_deque_push_pop)
