/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-push-pop-grow.c */
/* Copyright (C) 2021 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

unsigned test_deque_push_pop_grow(void)
{
	size_t i, limit;
	unsigned failures = 0;
	deque_s *deque;
	char msg[40];
	char msg2[40];
	const char *s;

	deque = deque_new();
	if (!deque) {
		check_int(deque != NULL ? 1 : 0, 1);
		++failures;
		return failures;
	}

	failures += check_size_t_m(deque->size(deque), 0, "initial size");

	failures += check_ptr_m(deque->pop(deque), NULL, "pop size 0");

	limit = 256;
	for (i = 0; i < limit; ++i) {
		eembed_ulong_to_str(msg, 40, i);
		deque->push(deque, "foo");
		s = (const char *)deque->peek_bottom(deque, i);
		failures += check_str_m(s, "foo", msg);

		if (i > 10) {
			eembed_strcat(msg, "_");
			eembed_ulong_to_str(msg2, 40, 10);
			eembed_strcat(msg, msg2);
			s = (const char *)deque->peek_bottom(deque, 10);
			failures += check_str_m(s, "foo", msg);
		}
	}
	for (i = 0; i < limit; ++i) {
		eembed_ulong_to_str(msg, 40, i);
		s = (const char *)deque->shift(deque);
		failures += check_str_m(s, "foo", msg);
	}
	failures += check_size_t_m(deque->size(deque), 0, "end size");

	deque_free(deque);
	return failures;
}

ECHECK_TEST_MAIN(test_deque_push_pop_grow)
