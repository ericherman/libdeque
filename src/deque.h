/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* deque.h Double-Ended QUEue interface */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#ifndef DEQUE_H
#define DEQUE_H

/*
   Inspired by: Donald Knuth.
   The Art of Computer Programming, Volume 1: Fundamental Algorithms
   Section 2.2.1: Stacks, Queues, and Deques, pp. 238-243.
   ( third edition, Addison-Wesley, 1997. ISBN 0-201-89683-4 )
*/

#ifdef __cplusplus
#define Deque_begin_C_functions extern "C" {
#define Deque_end_C_functions }
#else
#define Deque_begin_C_functions
#define Deque_end_C_functions
#endif

#include <stddef.h>		/* size_t */

struct deque_s {
	void *opaque_data;

	/* add items to the end of queue (or top of stack): */
	struct deque_s *(*push) (struct deque_s *d, void *data);

	/* remove items from end of queue (or top of stack): */
	void *(*pop)(struct deque_s *d);

	/* prepend items to queue (or bottom of stack): */
	struct deque_s *(*unshift) (struct deque_s *d, void *data);

	/* remove item from front of queue (or bottom of stack): */
	void *(*shift)(struct deque_s *d);

	/* pointer to data at the end of the queue, or top of the stack */
	void *(*peek_top)(struct deque_s *d);

	/* pointer to  data at the front of the queue, or bottom of the stack */
	void *(*peek_bottom)(struct deque_s *d);

	/* return the number of items in the deque */
	size_t (*size)(struct deque_s *d);
};

Deque_begin_C_functions
#undef Deque_begin_C_functions
/* function pointer typedefs for ease of use in full constructor */
typedef void *(*deque_malloc_func)(size_t size, void *context);
typedef void (*deque_free_func)(void *ptr, void *context);

/* constructors */

/* uses libc malloc and free */
struct deque_s *deque_new();

struct deque_s *deque_new_custom_allocator(deque_malloc_func mfunc,
					   deque_free_func mfree,
					   void *mcontext);

void deque_free(struct deque_s *d);

Deque_end_C_functions
#undef Deque_end_C_functions
#endif /* DEQUE_H */
