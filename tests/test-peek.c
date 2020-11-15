/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-peek.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "test-deque.h"

int test_peek(void)
{
	int failures = 0;
	deque_s *deque;

	deque = deque_new();
	if (!deque) {
		check_int(deque != NULL ? 1 : 0, 1);
		++failures;
		return failures;
	}

	failures += check_ptr_m(deque->peek_top(deque, 0), NULL, "top 0 0");
	failures += check_ptr_m(deque->peek_bottom(deque, 0), NULL, "bot 0 0");

	deque->push(deque, "uno");
	deque->push(deque, "due");
	deque->push(deque, "tri");

	failures += check_size_t(deque->size(deque), 3);

	failures += check_str_m(deque->peek_top(deque, 0), "tri", "top 0 1");
	failures += check_str_m(deque->peek_top(deque, 1), "due", "top 1 1");
	failures += check_str_m(deque->peek_top(deque, 2), "uno", "top 2 1");
	failures += check_str_m(deque->peek_top(deque, 3), NULL, "top 3 1");

	failures += check_str_m(deque->peek_bottom(deque, 0), "uno", "bot 0 1");
	failures += check_str_m(deque->peek_bottom(deque, 1), "due", "bot 1 1");
	failures += check_str_m(deque->peek_bottom(deque, 2), "tri", "bot 2 1");
	failures += check_str_m(deque->peek_bottom(deque, 3), NULL, "bot 3 1");

	failures += check_str_m(deque->peek_top(deque, 0), "tri", "top 0 2");
	failures += check_str_m(deque->peek_bottom(deque, 0), "uno", "bot 0 2");

	deque->pop(deque);
	failures += check_size_t(deque->size(deque), 2);

	failures += check_str_m(deque->peek_top(deque, 0), "due", "top 0 3");
	failures += check_str_m(deque->peek_top(deque, 1), "uno", "top 1 3");
	failures += check_str_m(deque->peek_top(deque, 2), NULL, "top 2 3");

	failures += check_str_m(deque->peek_bottom(deque, 0), "uno", "bot 0 3");
	failures += check_str_m(deque->peek_bottom(deque, 1), "due", "bot 1 3");
	failures += check_str_m(deque->peek_bottom(deque, 2), NULL, "bot 2 3");

	deque->pop(deque);

	failures += check_size_t(deque->size(deque), 1);

	failures += check_str_m(deque->peek_top(deque, 0), "uno", "top 0 4");
	failures += check_str_m(deque->peek_top(deque, 1), NULL, "top 1 4");
	failures += check_str_m(deque->peek_bottom(deque, 0), "uno", "bot 0 4");
	failures += check_str_m(deque->peek_bottom(deque, 1), NULL, "bot 1 4");

	deque->pop(deque);

	failures += check_size_t(deque->size(deque), 0);

	failures += check_ptr_m(deque->peek_top(deque, 0), NULL, "top 0 5");
	failures += check_ptr_m(deque->peek_bottom(deque, 0), NULL, "bot 0 5");

	failures += check_ptr_m(deque->peek_top(deque, 0), NULL, "top 0 6");
	failures += check_ptr_m(deque->peek_bottom(deque, 0), NULL, "bot 0 6");

	deque_free(deque);
	return failures;
}

TEST_DEQUE_MAIN(test_peek())
