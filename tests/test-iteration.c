/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-iteration.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

int func_1(struct deque *d, void *each, void *context)
{
	char *buf = (char *)context;
	char *val = (char *)each;
	(void)d;
	eembed_strcat(buf, val);
	return 0;
}

int func_2(struct deque *d, void *each, void *context)
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
	struct deque *d;
	char buf[80];
	int rv;

	d = deque_new();
	if (!d) {
		check_int(d != NULL ? 1 : 0, 1);
		return 1;
	}

	deque_push(d, "uno");
	deque_push(d, "due");
	deque_push(d, "tri");

	buf[0] = '\0';
	rv = deque_for_each(d, func_1, buf);
	check_int(rv, 0);
	check_str(buf, "unoduetri");

	buf[0] = '\0';
	rv = deque_for_each(d, func_2, buf);
	check_int(rv, 1);
	check_str(buf, "unodue");

	deque_free(d);
	return failures;
}

ECHECK_TEST_MAIN(test_iteration)
