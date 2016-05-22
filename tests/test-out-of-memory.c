#include <echeck.h>
#include <stdio.h>		/* fprintf */

#include "deque.h"

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

int main(int argc, char **argv)
{
	int failures = 0;
	int test_oom = (argc > 1) ? atoi(argv[1]) : 0;

	if (test_oom) {
		failures += test_out_of_memory();
	} else {
		fprintf(stderr, "skipping test_out_of_memory()\n");
	}

	if (failures) {
		fprintf(stderr, "%d failures in total\n", failures);
	}
	return failures;
}
