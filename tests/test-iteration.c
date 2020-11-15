/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-iteration.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "test-deque.h"

char *deque_test_diy_strcat(char *dest, const char *src)
{
	char *d;

	for (d = dest; *d; ++d) ;

	while (*src) {
		*d++ = *src++;
	}
	*d = '\0';

	return dest;
}

int deque_test_diy_strcmp(const char *s1, const char *s2)
{
	size_t i;

	i = 0;
	if (s1 == s2) {
		return 0;
	}
	if (!s1 || !s2) {
		return s1 ? 1 : -1;
	}
	for (i = 0; s1[i] && s2[i] && s1[i] == s2[i]; ++i) ;
	return s1[i] - s2[i];
}

int func_1(deque_s *d, void *each, void *context)
{
	char *buf = (char *)context;
	char *val = (char *)each;
	(void)d;
	deque_test_diy_strcat(buf, val);
	return 0;
}

int func_2(deque_s *d, void *each, void *context)
{
	char *buf = (char *)context;
	char *val = (char *)each;
	(void)d;
	if (deque_test_diy_strcmp(val, "tri") == 0) {
		return 1;
	}
	deque_test_diy_strcat(buf, (char *)each);
	return 0;
}

int test_iteration(void)
{
	int failures = 0;
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

TEST_DEQUE_MAIN(test_iteration())
