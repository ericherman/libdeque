/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-custom-allocator.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

unsigned test_deque_custom_allocator()
{
	unsigned failures = 0;
	deque_s *deque = NULL;
	struct echeck_err_injecting_context ctx;
	struct eembed_allocator wrap;
	struct eembed_allocator *real = eembed_global_allocator;
	struct eembed_log *elog = eembed_err_log;

	echeck_err_injecting_allocator_init(&wrap, real, &ctx, elog);

	deque = deque_new_custom_allocator(&wrap);
	if (!deque) {
		check_int(deque != NULL ? 1 : 0, 1);
		return 1;
	}

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

	deque_free(deque);

	failures += check_unsigned_int_m(ctx.allocs, ctx.frees, "frees,allocs");
	failures += check_unsigned_int_m(ctx.alloc_bytes > 0, 1, "bytes > 0");
	failures +=
	    check_unsigned_int_m(ctx.free_bytes, ctx.alloc_bytes, "bytes");
	failures += check_unsigned_int_m(ctx.fails, 0, "free NULL pointers");

	return failures;
}

ECHECK_TEST_MAIN(test_deque_custom_allocator)
