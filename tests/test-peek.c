/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-peek.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

#ifndef Check_v_str
#define Check_v_str(ptr, str, msg) \
	check_str_m((const char *)ptr, str, msg)
#endif

unsigned test_peek(void)
{
	unsigned failures = 0;
	struct deque *d;

	d = deque_new();
	if (!d) {
		check_int(d != NULL ? 1 : 0, 1);
		++failures;
		return failures;
	}

	failures += check_ptr_m(deque_peek_top(d, 0), NULL, "top 0 0");
	failures += check_ptr_m(deque_peek_bottom(d, 0), NULL, "bot 0 0");

	deque_push(d, "uno");
	deque_push(d, "due");
	deque_push(d, "tri");

	failures += check_size_t(deque_size(d), 3);

	failures += Check_v_str(deque_peek_top(d, 0), "tri", "top 0 1");
	failures += Check_v_str(deque_peek_top(d, 1), "due", "top 1 1");
	failures += Check_v_str(deque_peek_top(d, 2), "uno", "top 2 1");
	failures += Check_v_str(deque_peek_top(d, 3), NULL, "top 3 1");

	failures += Check_v_str(deque_peek_bottom(d, 0), "uno", "bot 0 1");
	failures += Check_v_str(deque_peek_bottom(d, 1), "due", "bot 1 1");
	failures += Check_v_str(deque_peek_bottom(d, 2), "tri", "bot 2 1");
	failures += Check_v_str(deque_peek_bottom(d, 3), NULL, "bot 3 1");

	failures += Check_v_str(deque_peek_top(d, 0), "tri", "top 0 2");
	failures += Check_v_str(deque_peek_bottom(d, 0), "uno", "bot 0 2");

	deque_pop(d);
	failures += check_size_t(deque_size(d), 2);

	failures += Check_v_str(deque_peek_top(d, 0), "due", "top 0 3");
	failures += Check_v_str(deque_peek_top(d, 1), "uno", "top 1 3");
	failures += Check_v_str(deque_peek_top(d, 2), NULL, "top 2 3");

	failures += Check_v_str(deque_peek_bottom(d, 0), "uno", "bot 0 3");
	failures += Check_v_str(deque_peek_bottom(d, 1), "due", "bot 1 3");
	failures += Check_v_str(deque_peek_bottom(d, 2), NULL, "bot 2 3");

	deque_pop(d);

	failures += check_size_t(deque_size(d), 1);

	failures += Check_v_str(deque_peek_top(d, 0), "uno", "top 0 4");
	failures += Check_v_str(deque_peek_top(d, 1), NULL, "top 1 4");
	failures += Check_v_str(deque_peek_bottom(d, 0), "uno", "bot 0 4");
	failures += Check_v_str(deque_peek_bottom(d, 1), NULL, "bot 1 4");

	deque_pop(d);

	failures += check_size_t(deque_size(d), 0);

	failures += check_ptr_m(deque_peek_top(d, 0), NULL, "top 0 5");
	failures += check_ptr_m(deque_peek_bottom(d, 0), NULL, "bot 0 5");

	failures += check_ptr_m(deque_peek_top(d, 0), NULL, "top 0 6");
	failures += check_ptr_m(deque_peek_bottom(d, 0), NULL, "bot 0 6");

	deque_free(d);
	return failures;
}

ECHECK_TEST_MAIN(test_peek)
