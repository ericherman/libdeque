/* deque.h */
#ifndef DEQUE_H
#define DEQUE_H

/*
   Inspired by: Donald Knuth.
   The Art of Computer Programming, Volume 1: Fundamental Algorithms
   Section 2.2.1: Stacks, Queues, and Deques, pp. 238-243.
   ( third edition, Addison-Wesley, 1997. ISBN 0-201-89683-4 )
*/

#include <stddef.h>		/* size_t */

typedef void *(*deque_malloc_func) (size_t size, void *context);
typedef void (*deque_free_func) (void *ptr, size_t size, void *context);

struct deque_s {
	void *data;

	struct deque_s *(*push) (struct deque_s * d, void *data);
	void *(*pop) (struct deque_s * d);

	struct deque_s *(*unshift) (struct deque_s * d, void *data);
	void *(*shift) (struct deque_s * d);

	void *(*top) (struct deque_s * d);
	void *(*bottom) (struct deque_s * d);

	size_t (*size) (struct deque_s * d);
};

struct deque_s *deque_new(deque_malloc_func mfunc, deque_free_func mfree,
			  void *mcontext);
void deque_free(struct deque_s *d);

#endif /* DEQUE_H */
