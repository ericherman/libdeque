/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* deque.h Double-Ended QUEue interface */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

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
#include <context-alloc.h>
/* constructors */
/* uses libc malloc and free */
struct deque_s *deque_new(void);

struct deque_s *deque_new_custom_allocator(context_malloc_func mfunc,
					   context_free_func mfree,
					   void *mcontext);

/* this is a size-bounded deque, it is recommended that at least 256 bytes
 * extra is provided for the deque struct and opaque data */
struct deque_s *deque_new_no_allocator(unsigned char *bytes, size_t bytes_len);

void deque_free(struct deque_s *d);

Deque_end_C_functions
#undef Deque_end_C_functions
#endif /* DEQUE_H */
