#include "test-deque.h"

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

TEST_DEQUE_MAIN(test_deque_new())
