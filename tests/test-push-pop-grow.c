/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-push-pop-grow.c */
/* Copyright (C) 2021 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

unsigned test_deque_push_pop_grow(void)
{
	size_t i, limit;
	unsigned failures = 0;
	struct deque *d;
	char msg[40];
	char msg2[40];
	const char *s;

	d = deque_new();
	if (!d) {
		check_int(d != NULL ? 1 : 0, 1);
		++failures;
		return failures;
	}

	failures += check_size_t_m(deque_size(d), 0, "initial size");

	failures += check_ptr_m(deque_pop(d), NULL, "pop size 0");

	limit = 256;
	for (i = 0; i < limit; ++i) {
		eembed_ulong_to_str(msg, 40, i);
		deque_push(d, "foo");
		s = (const char *)deque_peek_bottom(d, i);
		failures += check_str_m(s, "foo", msg);

		if (i > 10) {
			eembed_strcat(msg, "_");
			eembed_ulong_to_str(msg2, 40, 10);
			eembed_strcat(msg, msg2);
			s = (const char *)deque_peek_bottom(d, 10);
			failures += check_str_m(s, "foo", msg);
		}
	}
	for (i = 0; i < limit; ++i) {
		eembed_ulong_to_str(msg, 40, i);
		s = (const char *)deque_shift(d);
		failures += check_str_m(s, "foo", msg);
	}
	failures += check_size_t_m(deque_size(d), 0, "end size");

	deque_free(d);
	return failures;
}

ECHECK_TEST_MAIN(test_deque_push_pop_grow)
