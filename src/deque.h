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

/* see: bug #65165: extern "C" mishandled in header files */
/* https://savannah.gnu.org/bugs/?65165 */
#ifdef __cplusplus

#define Deque_begin_C_declarations \
extern "C" { \
struct deque_allow_semicolon

#define Deque_end_C_declarations \
} \
struct deque_cpp_allow_semicolon

#endif /* ifdef __cplusplus */

#ifndef Deque_begin_C_declarations
#define Deque_begin_C_declarations \
struct deque_allow_semicolon
#endif

#ifndef Deque_end_C_declarations
#define Deque_end_C_declarations \
struct deque_allow_semicolon
#endif

Deque_begin_C_declarations;
#undef Deque_begin_C_declarations

/*
 * freestanding headers are safe to include
 */
#include <stddef.h>
#include <limits.h>
#include <stdint.h>

#ifndef Deque_default_len
#ifdef __WORDSIZE
#define Deque_default_len ( 2 * __WORDSIZE )
#else
#define Deque_default_len 32
#endif
#endif

#ifndef Deque_default_unshift_space
#define Deque_default_unshift_space(data_space_len) (data_space_len/4)
#endif

struct deque {
	size_t first_pos;
	size_t end_pos;
	struct eembed_allocator *ea;
	union {
		struct {
			uint8_t deque_needs_free:1;
			uint8_t data_space_needs_free:1;
			uintptr_t reserved:((sizeof(uintptr_t) * CHAR_BIT) - 2);
		}
		flags;
		uintptr_t all_flags;
	};
	void *reseverd_for_future_use;
	size_t data_space_len;
	void **data_space;
};

/* passed parameter functions */
typedef int (*deque_iterator_func)(struct deque *d, void *each, void *context);

/* initialize the deque data_space using the custom allocator */
struct deque *deque_init(struct deque *d,
			 void **data_space,
			 size_t data_space_len, struct eembed_allocator *ea);

/* add items to the end of queue (or top of stack): */
struct deque *deque_push(struct deque *d, void *data);

/* remove items from end of queue (or top of stack): */
void *deque_pop(struct deque *d);

/* prepend items to queue (or bottom of stack): */
struct deque *deque_unshift(struct deque *d, void *data);

/* remove item from front of queue (or bottom of stack): */
void *deque_shift(struct deque *d);

/* reset the deque to an empty state */
void deque_clear(struct deque *d);

/* pointer to data from the end of the queue, or top of the stack */
void *deque_peek_top(struct deque *d, size_t index);

/* pointer to data from the front of the queue, or bottom of the stack */
void *deque_peek_bottom(struct deque *d, size_t index);

/* return the number of items in the deque */
size_t deque_size(struct deque *d);

/* internal iterator */
int deque_for_each(struct deque *d, deque_iterator_func func, void *context);

struct deque *deque_new(void);
struct deque *deque_new_custom_allocator(struct eembed_allocator *ea);
struct deque *deque_new_no_allocator(unsigned char *bytes, size_t bytes_len);

void deque_free(struct deque *d);

Deque_end_C_declarations;
#undef Deque_end_C_declarations
#endif /* DEQUE_H */
