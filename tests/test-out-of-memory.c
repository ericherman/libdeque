/* test-out-of-memory.c
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

int test_out_of_memory(void)
{
	int failures = 0;
	struct deque_s *deque;
	struct deque_s *rv;

	deque = deque_new();

	/* push in a loop until OOM */
	while ((rv = deque->push(deque, NULL)) != NULL) ;

	rv = deque->push(deque, NULL);
	failures += check_ptr(rv, NULL);

	deque_free(deque);
	return failures;
}

TEST_DEQUE_MAIN_IF((argc > 1) ? atoi(argv[1]) : 0, test_out_of_memory())
