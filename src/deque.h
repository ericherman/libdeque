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

Deque_begin_C_functions
#undef Deque_begin_C_functions
/*
 * freesting headers are safe to include
 */
#include <stddef.h>		/* size_t */
/* forward declaration of the deque type */
    struct deque;
typedef struct deque deque_s;

/* passed parameter functions */
typedef int (*deque_iterator_func)(deque_s *d, void *each, void *context);

struct deque {
	void *opaque_data;

	/* add items to the end of queue (or top of stack): */
	deque_s *(*push)(deque_s *d, void *data);

	/* remove items from end of queue (or top of stack): */
	void *(*pop)(deque_s *d);

	/* prepend items to queue (or bottom of stack): */
	deque_s *(*unshift)(deque_s *d, void *data);

	/* remove item from front of queue (or bottom of stack): */
	void *(*shift)(deque_s *d);

	/* reset the deque to an empty state */
	void (*clear)(deque_s *d);

	/* pointer to data from the end of the queue, or top of the stack */
	void *(*peek_top)(deque_s *d, size_t index);

	/* pointer to data from the front of the queue, or bottom of the stack */
	void *(*peek_bottom)(deque_s *d, size_t index);

	/* return the number of items in the deque */
	size_t (*size)(deque_s *d);

	/* internal iterator */
	int (*for_each)(deque_s *d, deque_iterator_func func, void *context);
};

struct eembed_allocator;	/* eembed.h */

/* constructors */
/* uses libc malloc and free */
deque_s *deque_new(void);

deque_s *deque_new_custom_allocator(struct eembed_allocator *ea);

/* this is a size-bounded deque, it is recommended that at least 256 bytes
 * extra is provided for the deque struct and opaque data */
deque_s *deque_new_no_allocator(unsigned char *bytes, size_t bytes_len);

void deque_free(deque_s *d);

Deque_end_C_functions
#undef Deque_end_C_functions
#endif /* DEQUE_H */
