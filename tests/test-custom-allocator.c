/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-custom-allocator.c */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "test-deque.h"

struct mem_context {
	unsigned allocs;
	unsigned alloc_bytes;
	unsigned frees;
	unsigned free_bytes;
	unsigned fails;
};

void *test_malloc(void *context, size_t size)
{
	struct mem_context *ctx;
	unsigned char *sneaky_stash;
	void *ptr;

	ctx = (struct mem_context *)context;
	ptr = NULL;
	sneaky_stash = (unsigned char *)malloc(sizeof(size_t) + size);
	if (!sneaky_stash) {
		++ctx->fails;
	} else {
		++ctx->allocs;
		ctx->alloc_bytes += size;
		/* stash the size in the bytes allocated */
		deque_memmove(sneaky_stash, &size, sizeof(size_t));
		/* set the application pointer */
		ptr = (void *)(sneaky_stash + sizeof(size_t));
	}
	return ptr;
}

void test_free(void *context, void *ptr)
{
	struct mem_context *ctx;
	unsigned char *sneaky_stash;
	size_t size;

	ctx = (struct mem_context *)context;
	if (ptr == NULL) {
		++ctx->fails;
	} else {
		/* retreive the allocated sneaky stash pointer */
		sneaky_stash = ((unsigned char *)ptr) - sizeof(size_t);
		/* fetch the size from the stash */
		deque_memmove(&size, sneaky_stash, sizeof(size_t));

		++ctx->frees;
		ctx->free_bytes += size;
		free(sneaky_stash);
	}
}

int test_deque_custom_allocator()
{
	int failures = 0;
	deque_s *deque;
	struct mem_context ctx = { 0, 0, 0, 0, 0 };

	deque = deque_new_custom_allocator(test_malloc, test_free, &ctx);
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

TEST_DEQUE_MAIN(test_deque_custom_allocator())
