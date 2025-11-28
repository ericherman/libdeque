/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-custom-allocator.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

#define BYTES_LEN 1000

void *deque_no_alloc(void *context, size_t size);

unsigned test_deque_new_no_allocator(void)
{
	unsigned failures = 0;
	struct deque *d;
	unsigned char bytes[BYTES_LEN];
	size_t bytes_len = BYTES_LEN;
	struct eembed_allocator *nea = eembed_null_allocator;
	void *foo;

	foo = nea->malloc(nea, 1);
	failures += check_ptr(NULL, foo);

	d = deque_new_no_allocator(NULL, 0);
	failures += check_ptr(d, NULL);

	d = deque_new_no_allocator(bytes, 2);
	failures += check_ptr(d, NULL);

	d = deque_new_no_allocator(bytes, bytes_len);
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

	failures += check_size_t_m(deque_size(d), 0, "size C");

	deque_free(d);

	return failures;
}

ECHECK_TEST_MAIN(test_deque_new_no_allocator)
