/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-out-of-memory.c */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#include "test-deque.h"

#include <string.h>

int test_out_of_memory_push(unsigned long malloc_fail_bitmask)
{
	int failures = 0;
	int err = 0;
	size_t i;
	context_malloc_func ctx_alloc = oom_injecting_malloc;
	context_free_func ctx_free = oom_injecting_free;
	oom_injecting_context_s mctx;
	deque_s *deque;
	deque_s *rv;

	oom_injecting_context_init(&mctx);

	mctx.attempts_to_fail_bitmask = malloc_fail_bitmask;

	deque = deque_new_custom_allocator(ctx_alloc, ctx_free, &mctx);
	if (!deque) {
		++err;
		if (!malloc_fail_bitmask) {
			fprintf(stderr, "%s:%d deque_new_custom_allocator\n",
				__FILE__, __LINE__);
			++failures;
		}
		goto end_test_out_of_memory;
	}

	for (i = 0; i < 5; ++i) {
		rv = deque->push(deque, NULL);
		if (!rv) {
			++err;
			if (!malloc_fail_bitmask) {
				fprintf(stderr, "%s:%d push\n", __FILE__,
					__LINE__);
				++failures;
			}
		}
		rv = deque->unshift(deque, NULL);
		if (!rv) {
			++err;
			if (!malloc_fail_bitmask) {
				fprintf(stderr, "%s:%d unshift\n", __FILE__,
					__LINE__);
				++failures;
			}
		}
	}

	if (!err && malloc_fail_bitmask) {
		if (mctx.allocs < 3) {
			fprintf(stderr, "%s:%d only %lu mallocs?\n", __FILE__,
				__LINE__, (unsigned long)mctx.allocs);
			++failures;
		}
	}

	if (err && !malloc_fail_bitmask) {
		++failures;
	}

end_test_out_of_memory:
	deque_free(deque);

	failures += check_unsigned_int_m(mctx.frees, mctx.allocs, "alloc/free");
	failures +=
	    check_unsigned_int_m(mctx.free_bytes, mctx.alloc_bytes, "bytes");

	return failures;
}

int test_out_of_memory(void)
{
	int failures = 0;

	failures += test_out_of_memory_push(0);
	failures += test_out_of_memory_push(1UL << 0);
	failures += test_out_of_memory_push(1UL << 1);
	failures += test_out_of_memory_push(1UL << 2);
	failures += test_out_of_memory_push(1UL << 3);
	failures += test_out_of_memory_push(1UL << 4);
	failures += test_out_of_memory_push(1UL << 5);
	failures += test_out_of_memory_push(1UL << 6);
	failures += test_out_of_memory_push(1UL << 7);
	failures += test_out_of_memory_push(1UL << 8);
	failures += test_out_of_memory_push(1UL << 9);
	failures += test_out_of_memory_push(1UL << 10);

	return failures;
}

TEST_DEQUE_MAIN_IF(1, test_out_of_memory())
