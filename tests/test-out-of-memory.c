/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-out-of-memory.c */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

#include "test-deque.h"

#include <string.h>

struct tracking_mem_context_s {
	unsigned long allocs;
	unsigned long alloc_bytes;
	unsigned long frees;
	unsigned long free_bytes;
	unsigned long fails;
	unsigned long max_used;
	unsigned long attempts;
	unsigned long attempts_to_fail_bitmask;
};

void *test_malloc(size_t size, void *context)
{
	struct tracking_mem_context_s *ctx = NULL;
	unsigned char *tracking_buffer = NULL;
	void *ptr = NULL;
	size_t used = 0;

	ptr = NULL;
	ctx = (struct tracking_mem_context_s *)context;
	if (0x01 & (ctx->attempts_to_fail_bitmask >> ctx->attempts++)) {
		return NULL;
	}
	tracking_buffer = malloc(sizeof(size_t) + size);
	if (!tracking_buffer) {
		++ctx->fails;
		return NULL;
	}

	memcpy(tracking_buffer, &size, sizeof(size_t));
	++ctx->allocs;
	ctx->alloc_bytes += size;
	if (ctx->free_bytes > ctx->alloc_bytes) {
		fprintf(stderr,
			"%s: %d BAD MOJO: free_bytes > alloc_bytes?! (%lu > %lu)\n",
			__FILE__, __LINE__, (unsigned long)ctx->free_bytes,
			(unsigned long)ctx->alloc_bytes);
	} else {
		used = ctx->alloc_bytes - ctx->free_bytes;
		if (used > ctx->max_used) {
			ctx->max_used = used;
		}
	}
	ptr = (void *)(tracking_buffer + sizeof(size_t));
	return ptr;
}

void test_free(void *ptr, void *context)
{
	struct tracking_mem_context_s *ctx = NULL;
	unsigned char *tracking_buffer = NULL;
	size_t size = 0;

	ctx = (struct tracking_mem_context_s *)context;
	if (ptr == NULL) {
		++ctx->fails;
		return;
	}
	tracking_buffer = ((unsigned char *)ptr) - sizeof(size_t);
	memcpy(&size, tracking_buffer, sizeof(size_t));
	ctx->free_bytes += size;
	++ctx->frees;
	free(tracking_buffer);
	if (ctx->free_bytes > ctx->alloc_bytes) {
		fprintf(stderr,
			"%s: %d BAD MOJO: free_bytes > alloc_bytes?! (%lu > %lu) just freed %lu\n",
			__FILE__, __LINE__, (unsigned long)ctx->free_bytes,
			(unsigned long)ctx->alloc_bytes, (unsigned long)size);
	}

}

int test_out_of_memory_push(unsigned long malloc_fail_bitmask)
{
	int failures = 0;
	int err = 0;
	size_t i;
	struct tracking_mem_context_s mctx;
	struct deque_s *deque;
	struct deque_s *rv;

	memset(&mctx, 0, sizeof(struct tracking_mem_context_s));
	mctx.attempts_to_fail_bitmask = malloc_fail_bitmask;

	deque = deque_new_custom_allocator(test_malloc, test_free, &mctx);
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
