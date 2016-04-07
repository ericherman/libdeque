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

struct deque_s *deque_new();
void deque_free(struct deque_s *d);

#endif /* DEQUE_H */
