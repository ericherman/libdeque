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
	struct oom_injecting_context_s mctx;
	struct deque_s *deque;
	struct deque_s *rv;

	memset(&mctx, 0, sizeof(struct oom_injecting_context_s));
	mctx.attempts_to_fail_bitmask = malloc_fail_bitmask;

	deque =
	    deque_new_custom_allocator(oom_injecting_malloc, oom_injecting_free,
				       &mctx);
	if (!deque) {
		++err;
		if (!malloc_fail_bitmask) {
			++failures;
		}
		goto end_test_out_of_memory;
	}

	for (i = 0; i < 5; ++i) {
		rv = deque->push(deque, NULL);
		if (!rv) {
			++err;
			if (!malloc_fail_bitmask) {
				++failures;
			}
		}
		rv = deque->unshift(deque, NULL);
		if (!rv) {
			++err;
			if (!malloc_fail_bitmask) {
				++failures;
			}
		}
	}

	if (!err && malloc_fail_bitmask) {
		++failures;
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
