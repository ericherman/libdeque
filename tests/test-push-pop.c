#include <echeck.h>
#include <stdio.h>		/* fprintf */

#include "deque.h"

int test_deque_push_pop(void)
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

int main(void)
{
	int failures = 0;

	failures += test_deque_push_pop();

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}
	return failures;
}
