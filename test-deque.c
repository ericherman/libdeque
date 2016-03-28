#include <echeck.h>
#include <stdio.h>		/* fprintf */
#include <string.h>		/* strlen */

#include "deque.h"

int test_deque_new()
{
	int failures = 0;
	struct deque_s *deque;

	deque = deque_new();

	if (deque == NULL) {
		++failures;
	}

	failures += check_unsigned_int_m(deque_size(deque), 0, "deque_size");

	deque_free(deque);
	return failures;
}

int test_deque_push_pop()
{
	int failures = 0;
	struct deque_s *deque;

	deque = deque_new();

	failures += check_size_t_m(deque_size(deque), 0, "initial size");

	failures += check_ptr_m(deque_pop(deque), NULL, "pop size 0");

	deque_push(deque, "one");
	deque_push(deque, "two");
	deque_push(deque, "three");

	failures += check_size_t_m(deque_size(deque), 3, "size A");

	deque_unshift(deque, "zero");

	failures += check_size_t_m(deque_size(deque), 4, "size B");

	failures += check_str_m((char *)deque_pop(deque), "three", "pop1");

	deque_push(deque, "four");

	failures += check_str_m((char *)deque_shift(deque), "zero", "shift1");
	failures += check_str_m((char *)deque_shift(deque), "one", "shift2");

	failures += check_str_m((char *)deque_pop(deque), "four", "pop2");
	failures += check_str_m((char *)deque_pop(deque), "two", "pop3");

	failures += check_size_t_m(deque_size(deque), 0, "size C");

	deque_free(deque);
	return failures;
}

int test_out_of_memory()
{
	int failures = 0;
	struct deque_s *deque;
	struct deque_s *rv;

	deque = deque_new();

	while ((rv = deque_push(deque, NULL)) != NULL) ;

	rv = deque_push(deque, NULL);
	failures += check_ptr(rv, NULL);

	deque_free(deque);
	return failures;
}

int main(int argc, char **argv)
{
	int failures = 0;
	int test_oom = (argc > 1) ? atoi(argv[1]) : 0;

	failures += test_deque_new();
	failures += test_deque_push_pop();

	if (test_oom) {
		failures += test_out_of_memory();
	}

	if (failures) {
		fprintf(stderr, "%d failures in total\n", failures);
	}
	return failures;
}
