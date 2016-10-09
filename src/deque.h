/* deque.h Double-Ended QUEue interface
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
#ifndef DEQUE_H
#define DEQUE_H

/*
   Inspired by: Donald Knuth.
   The Art of Computer Programming, Volume 1: Fundamental Algorithms
   Section 2.2.1: Stacks, Queues, and Deques, pp. 238-243.
   ( third edition, Addison-Wesley, 1997. ISBN 0-201-89683-4 )
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>		/* size_t */

struct deque_s {
	void *data;

	struct deque_s *(*push) (struct deque_s *d, void *data);
	void *(*pop) (struct deque_s *d);

	struct deque_s *(*unshift) (struct deque_s *d, void *data);
	void *(*shift) (struct deque_s *d);

	void *(*top) (struct deque_s *d);
	void *(*bottom) (struct deque_s *d);

	size_t (*size) (struct deque_s *d);
};

typedef void *(*deque_malloc_func) (size_t size, void *context);
typedef void (*deque_free_func) (void *ptr, size_t size, void *context);

struct deque_s *deque_new(deque_malloc_func mfunc, deque_free_func mfree,
			  void *mcontext);

void deque_free(struct deque_s *d);

#ifdef __cplusplus
}
#endif

#endif /* DEQUE_H */
