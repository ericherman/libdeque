/* test-custom-allocator.c
   Copyright (C) 2016 Eric Herman <eric@freesa.org>

   This work is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This work is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License and the GNU General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public
   License (COPYING) and the GNU General Public License (COPYING.GPL3).
   If not, see <http://www.gnu.org/licenses/>.
*/

#include "test-deque.h"
#include <string.h>		/* memcpy */

struct mem_context {
	unsigned allocs;
	unsigned alloc_bytes;
	unsigned frees;
	unsigned free_bytes;
	unsigned fails;
};

void *test_malloc(size_t size, void *context)
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
		memcpy(sneaky_stash, &size, sizeof(size_t));
		/* set the application pointer */
		ptr = (void *)(sneaky_stash + sizeof(size_t));
	}
	return ptr;
}

void test_free(void *ptr, void *context)
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
		memcpy(&size, sneaky_stash, sizeof(size_t));

		++ctx->frees;
		ctx->free_bytes += size;
		free(sneaky_stash);
	}
}

int test_deque_custom_allocator()
{
	int failures = 0;
	struct deque_s *deque;
	struct mem_context ctx = { 0, 0, 0, 0, 0 };
	unsigned allocs = 0;

	deque = deque_new_custom_allocator(test_malloc, test_free, &ctx);
	++allocs;
	++allocs;

	failures += check_size_t_m(deque->size(deque), 0, "initial size");

	failures += check_ptr_m(deque->pop(deque), NULL, "pop size 0");

	deque->push(deque, "one");
	++allocs;
	deque->push(deque, "two");
	++allocs;
	deque->push(deque, "three");
	++allocs;

	failures += check_size_t_m(deque->size(deque), 3, "size A");

	deque->unshift(deque, "zero");
	++allocs;

	failures += check_size_t_m(deque->size(deque), 4, "size B");

	failures += check_str_m((char *)deque->pop(deque), "three", "pop1");

	deque->push(deque, "four");
	++allocs;

	failures += check_str_m((char *)deque->shift(deque), "zero", "shift1");
	failures += check_str_m((char *)deque->shift(deque), "one", "shift2");

	failures += check_str_m((char *)deque->pop(deque), "four", "pop2");
	failures += check_str_m((char *)deque->pop(deque), "two", "pop3");

	failures += check_size_t_m(deque->size(deque), 0, "size C");

	deque_free(deque);

	failures += check_unsigned_int_m(ctx.allocs, allocs, "ctx.allocs");
	failures += check_unsigned_int_m(ctx.frees, allocs, "ctx.freess");
	failures += check_unsigned_int_m(ctx.alloc_bytes > 0, 1, "bytes > 0");
	failures +=
	    check_unsigned_int_m(ctx.free_bytes, ctx.alloc_bytes, "bytes");
	failures += check_unsigned_int_m(ctx.fails, 0, "free NULL pointers");

	return failures;
}

TEST_DEQUE_MAIN(test_deque_custom_allocator())
