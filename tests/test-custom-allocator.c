#include <echeck.h>
#include <stdio.h>		/* fprintf */

#include "deque.h"

struct mem_context {
	unsigned allocs;
	unsigned alloc_bytes;
	unsigned frees;
	unsigned free_bytes;
	unsigned fails;
};

void *test_malloc(size_t size, void *context)
{
	struct mem_context *ctx = (struct mem_context *)context;
	++ctx->allocs;
	ctx->alloc_bytes += size;
	return malloc(size);
}

void test_free(void *ptr, size_t size, void *context)
{
	struct mem_context *ctx = (struct mem_context *)context;
	++ctx->frees;
	ctx->free_bytes += size;
	if (ptr == NULL) {
		++ctx->fails;
	}
	free(ptr);
}

int test_deque_custom_allocator()
{
	int failures = 0;
	struct deque_s *deque;
	struct mem_context ctx = { 0, 0, 0, 0, 0 };
	unsigned allocs = 0;

	deque = deque_new(test_malloc, test_free, &ctx);
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

int main()
{
	int failures = 0;

	failures += test_deque_custom_allocator();

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}
	return failures;
}
