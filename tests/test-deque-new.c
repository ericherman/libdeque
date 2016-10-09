/* test-deque-new.c
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

int test_deque_new()
{
	int failures = 0;
	struct deque_s *deque;

	deque = deque_new();

	if (deque == NULL) {
		++failures;
	}

	failures += check_unsigned_int_m(deque->size(deque), 0, "deque_size");

	deque_free(deque);
	return failures;
}

TEST_DEQUE_MAIN(test_deque_new())
