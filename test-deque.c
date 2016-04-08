#include <echeck.h>
#include <stdio.h>		/* fprintf */
#include <string.h>		/* strlen */

#include "deque.h"

int test_deque_new()
{
	int failures = 0;
	struct deque_s *deque;

	deque = deque_new(NULL, NULL, NULL);

	if (deque == NULL) {
		++failures;
	}

	failures += check_unsigned_int_m(deque->size(deque), 0, "deque_size");

	deque_free(deque);
	return failures;
}

int test_deque_push_pop()
{
	int failures = 0;
	struct deque_s *deque;

	deque = deque_new(NULL, NULL, NULL);

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
	return failures;
}

int test_out_of_memory()
{
	int failures = 0;
	struct deque_s *deque;
	struct deque_s *rv;

	deque = deque_new(NULL, NULL, NULL);

	while ((rv = deque->push(deque, NULL)) != NULL) ;

	rv = deque->push(deque, NULL);
	failures += check_ptr(rv, NULL);

	deque_free(deque);
	return failures;
}

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

int main(int argc, char **argv)
{
	int failures = 0;
	int test_oom = (argc > 1) ? atoi(argv[1]) : 0;

	failures += test_deque_new();
	failures += test_deque_push_pop();
	failures += test_deque_custom_allocator();

	if (test_oom) {
		failures += test_out_of_memory();
	}

	if (failures) {
		fprintf(stderr, "%d failures in total\n", failures);
	}
	return failures;
}
