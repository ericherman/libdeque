/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-deque-new.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

unsigned test_deque_new()
{
	unsigned failures = 0;
	deque_s *deque;

	deque = deque_new();

	if (!deque) {
		check_int(deque != NULL ? 1 : 0, 1);
		return 1;
	}

	failures += check_unsigned_int_m(deque->size(deque), 0, "deque_size");

	deque_free(deque);
	return failures;
}

ECHECK_TEST_MAIN(test_deque_new)
