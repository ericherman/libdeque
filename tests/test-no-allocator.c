/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-custom-allocator.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-deque.h"
#include <string.h>		/* memcpy */

#define BYTES_LEN 1000

void *deque_no_alloc(void *context, size_t size);

int test_deque_new_no_allocator(void)
{
	int failures = 0;
	deque_s *deque;
	unsigned char bytes[BYTES_LEN];
	size_t bytes_len = BYTES_LEN;
	void *foo;

	foo = deque_no_alloc(NULL, 1);
	failures += check_ptr(NULL, foo);

	deque = deque_new_no_allocator(NULL, 0);
	failures += check_ptr(deque, NULL);

	deque = deque_new_no_allocator(bytes, 2);
	failures += check_ptr(deque, NULL);

	deque = deque_new_no_allocator(bytes, bytes_len);

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

	failures += check_size_t_m(deque->size(deque), 0, "size C");

	return failures;
}

TEST_DEQUE_MAIN(test_deque_new_no_allocator())
