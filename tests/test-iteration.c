/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-iteration.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

int func_1(deque_s *d, void *each, void *context)
{
	char *buf = (char *)context;
	char *val = (char *)each;
	(void)d;
	eembed_strcat(buf, val);
	return 0;
}

int func_2(deque_s *d, void *each, void *context)
{
	char *buf = (char *)context;
	char *val = (char *)each;
	(void)d;
	if (eembed_strcmp(val, "tri") == 0) {
		return 1;
	}
	eembed_strcat(buf, (char *)each);
	return 0;
}

unsigned test_iteration(void)
{
	unsigned failures = 0;
	deque_s *deque;
	char buf[80];
	int rv;

	deque = deque_new();
	if (!deque) {
		check_int(deque != NULL ? 1 : 0, 1);
		return 1;
	}

	deque->push(deque, "uno");
	deque->push(deque, "due");
	deque->push(deque, "tri");

	buf[0] = '\0';
	rv = deque->for_each(deque, func_1, buf);
	check_int(rv, 0);
	check_str(buf, "unoduetri");

	buf[0] = '\0';
	rv = deque->for_each(deque, func_2, buf);
	check_int(rv, 1);
	check_str(buf, "unodue");

	deque_free(deque);
	return failures;
}

ECHECK_TEST_MAIN(test_iteration)
