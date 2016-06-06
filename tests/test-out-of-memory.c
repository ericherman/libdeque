#include "test-deque.h"

int test_out_of_memory(void)
{
	int failures = 0;
	struct deque_s *deque;
	struct deque_s *rv;

	deque = deque_new(NULL, NULL, NULL);

	/* push in a loop until OOM */
	while ((rv = deque->push(deque, NULL)) != NULL) ;

	rv = deque->push(deque, NULL);
	failures += check_ptr(rv, NULL);

	deque_free(deque);
	return failures;
}

TEST_DEQUE_MAIN_IF((argc > 1) ? atoi(argv[1]) : 0, test_out_of_memory())
