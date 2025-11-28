/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-out-of-memory.c */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#include "deque.h"
#include "echeck.h"

unsigned test_out_of_memory_push(unsigned long malloc_fail_bitmask)
{
	struct eembed_log *elog = eembed_err_log;
	unsigned failures = 0;
	int err = 0;
	size_t i;
	struct eembed_allocator *real = eembed_global_allocator;
	struct eembed_allocator wrap;
	struct echeck_err_injecting_context mctx;
	struct deque *d;
	struct deque *rv;

	echeck_err_injecting_allocator_init(&wrap, real, &mctx, elog);

	mctx.attempts_to_fail_bitmask = malloc_fail_bitmask;

	d = deque_new_custom_allocator(&wrap);
	if (!d) {
		++err;
		if (!malloc_fail_bitmask) {
			elog->append_s(elog, __FILE__);
			elog->append_s(elog, ":");
			elog->append_l(elog, __LINE__);
			elog->append_s(elog, " deque_new_custom_allocator");
			elog->append_eol(elog);
			++failures;
		}
		goto end_test_out_of_memory;
	}

	for (i = 0; i < 5; ++i) {
		rv = deque_push(d, NULL);
		if (!rv) {
			++err;
			if (!malloc_fail_bitmask) {
				elog->append_s(elog, __FILE__);
				elog->append_s(elog, ":");
				elog->append_l(elog, __LINE__);
				elog->append_s(elog, " push");
				elog->append_eol(elog);
				++failures;
			}
		}
		rv = deque_unshift(d, NULL);
		if (!rv) {
			++err;
			if (!malloc_fail_bitmask) {
				elog->append_s(elog, __FILE__);
				elog->append_s(elog, ":");
				elog->append_l(elog, __LINE__);
				elog->append_s(elog, " unshift");
				elog->append_eol(elog);
				++failures;
			}
		}
	}

	if (!err && malloc_fail_bitmask) {
		if (mctx.allocs < 2) {
			elog->append_s(elog, __FILE__);
			elog->append_s(elog, ":");
			elog->append_l(elog, __LINE__);
			elog->append_s(elog, " only ");
			elog->append_ul(elog, (unsigned long)mctx.allocs);
			elog->append_s(elog, " mallocs?");
			elog->append_eol(elog);
			++failures;
		}
	}

	if (err && !malloc_fail_bitmask) {
		++failures;
	}

end_test_out_of_memory:
	deque_free(d);

	failures += check_unsigned_int_m(mctx.frees, mctx.allocs, "alloc/free");
	failures +=
	    check_unsigned_int_m(mctx.free_bytes, mctx.alloc_bytes, "bytes");

	return failures;
}

unsigned test_out_of_memory(void)
{
	unsigned failures = 0;

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

ECHECK_TEST_MAIN(test_out_of_memory)
