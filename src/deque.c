/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* deque.c Double-Ended QUEue */
/* Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org> */

/*
   Inspired by: Donald Knuth.
   The Art of Computer Programming, Volume 1: Fundamental Algorithms
   Section 2.2.1: Stacks, Queues, and Deques, pp. 238-243.
   ( third edition, Addison-Wesley, 1997. ISBN 0-201-89683-4 )
*/
#include "deque.h"

#ifndef assert
#include <assert.h>
#endif

#ifndef Deque_memmove
#include <string.h>
#define Deque_memmove(dest, src, n) memmove(dest, src, n)
#endif

#ifndef Deque_memcpy
#include <string.h>
#define Deque_memcpy(dest, src, n) memcpy(dest, src, n)
#endif

#ifndef Deque_default_len
#define Deque_default_len 32
#endif
#if ((Deque_default_len) < 4)
#error "((Deque_default_len) < 4)"
#endif

#ifndef Deque_default_unshift_space
#define Deque_default_unshift_space ((Deque_default_len)/4)
#endif
#if ((Deque_default_unshift_space) >= (Deque_default_len))
#error "((Deque_default_unshift_space) >= (Deque_default_len))"
#endif

#ifndef Deque_wordsize
#ifdef __WORDSIZE
#define Deque_wordsize __WORDSIZE
#else
#define Deque_wordsize 4
#endif /* __WORDSIZE */
#endif /* Deque_wordsize */

#define Deque_align(x) \
	(((x) + ((Deque_wordsize) - 1)) \
	     & ~((Deque_wordsize) - 1))

struct deque_data {
	void **data_space;
	size_t data_space_len;
	size_t first_pos;
	size_t end_pos;

	context_malloc_func mem_alloc;
	context_free_func mem_free;
	void *mem_context;
};

static struct deque_data *deque_get_internal_data(struct deque_s *deque)
{
	assert(deque != NULL);
	assert(deque->opaque_data != NULL);
	return (struct deque_data *)deque->opaque_data;
}

static void deque_set_internal_data(struct deque_s *deque, struct deque_data *d)
{
	assert(deque != NULL);
	assert(d != NULL);
	assert(deque->opaque_data == NULL);
	deque->opaque_data = (void *)d;
}

static void *deque_peek_top(struct deque_s *deque)
{
	struct deque_data *d = deque_get_internal_data(deque);

	if (d->first_pos == d->end_pos) {
		return NULL;
	}

	assert(d->end_pos > 0);

	return d->data_space[d->end_pos - 1];
}

static void *deque_peek_bottom(struct deque_s *deque)
{
	struct deque_data *d = deque_get_internal_data(deque);

	if (d->first_pos == d->end_pos) {
		return NULL;
	}
	return d->data_space[d->first_pos];
}

static size_t deque_size(struct deque_s *deque)
{
	struct deque_data *d = deque_get_internal_data(deque);

	d = deque_get_internal_data(deque);
	return d->end_pos - d->first_pos;
}

static struct deque_s *deque_push(struct deque_s *deque, void *user_data)
{
	struct deque_data *d = deque_get_internal_data(deque);

	if (d->end_pos == d->data_space_len) {
		/* no space to append at end */
		if (d->first_pos > 1) {
			/* free space at beginning, shift content that way */
			/* use half of the free space */
			size_t new_first_pos = d->first_pos / 2;
			size_t pos_shift = d->first_pos - new_first_pos;
			size_t used = d->end_pos - d->first_pos;
			assert(pos_shift > 0);
			Deque_memmove(&d->data_space[new_first_pos],
				      &d->data_space[d->first_pos],
				      sizeof(void *) * used);
			d->first_pos -= pos_shift;
			d->end_pos -= pos_shift;
		} else {
			/* no free space, double the amount */
			size_t old_space_len = d->data_space_len;
			size_t new_space_len = old_space_len * 2;
			size_t size = sizeof(void *) * new_space_len;
			/* allow some free space for unshifting */
			size_t pos_shift = Deque_default_unshift_space;
			void **old_space = d->data_space;
			void **new_space = d->mem_alloc(d->mem_context, size);
			if (!new_space) {
				return NULL;
			}
			Deque_memcpy(new_space + pos_shift, old_space,
				     old_space_len);
			d->mem_free(d->mem_context, old_space);
			d->data_space = new_space;
			d->data_space_len = new_space_len;
			d->first_pos += pos_shift;
			d->end_pos += pos_shift;
		}
	}
	assert(d->end_pos < d->data_space_len);
	d->data_space[d->end_pos++] = user_data;
	return deque;
}

static void *deque_pop(struct deque_s *deque)
{
	void *user_data = NULL;
	struct deque_data *d = deque_get_internal_data(deque);

	if (d->end_pos == d->first_pos) {
		return NULL;
	}

	assert(d->end_pos > 0);

	user_data = d->data_space[--d->end_pos];

	assert(d->first_pos <= d->end_pos);

	return user_data;
}

static struct deque_s *deque_unshift(struct deque_s *deque, void *user_data)
{
	struct deque_data *d = deque_get_internal_data(deque);

	if (d->first_pos == d->end_pos) {
		/* unshifting onto an empty deque */
		/* best to make extra room, put first item in the middle */
		size_t pos = 1 + (d->data_space_len / 2);
		d->first_pos = pos;
		d->end_pos = pos;
	} else if (d->first_pos == 0) {
		/* no room at the front */
		if (d->end_pos < d->data_space_len) {
			/* but room at the end, use half of that */
			size_t avail = d->data_space_len - d->end_pos;
			size_t pos_shift = 1 + (avail / 2);
			size_t size = sizeof(void *) * d->end_pos;
			Deque_memmove(&d->data_space[pos_shift], d->data_space,
				      size);
			d->first_pos += pos_shift;
			d->end_pos += pos_shift;
		} else {
			/* no room at all, double space */
			size_t old_space_len = d->data_space_len;
			size_t new_space_len = old_space_len * 2;
			size_t size = sizeof(void *) * new_space_len;
			size_t old_size = sizeof(void *) * old_space_len;
			/* give half the new space to front for unshifting */
			size_t pos_shift = new_space_len / 2;
			void **old_space = d->data_space;
			void **new_space = d->mem_alloc(d->mem_context, size);
			if (!new_space) {
				return NULL;
			}
			Deque_memcpy(new_space + pos_shift, old_space,
				     old_size);
			d->mem_free(d->mem_context, old_space);
			d->data_space = new_space;
			d->data_space_len = new_space_len;
			d->first_pos += pos_shift;
			d->end_pos += pos_shift;
		}
	}
	assert(d->first_pos > 0);

	d->data_space[--d->first_pos] = user_data;

	assert(d->first_pos <= d->end_pos);

	return deque;
}

static void *deque_shift(struct deque_s *deque)
{
	void *user_data = NULL;
	struct deque_data *d = deque_get_internal_data(deque);

	d = deque_get_internal_data(deque);

	if (d->first_pos == d->end_pos) {
		return NULL;
	}

	assert(d->first_pos < d->data_space_len);

	user_data = d->data_space[d->first_pos++];

	if (d->first_pos == d->end_pos) {
		size_t pos = Deque_default_unshift_space;
		d->first_pos = pos;
		d->end_pos = pos;
	}

	assert(d->first_pos <= d->end_pos);

	return user_data;
}

static void deque_init(context_malloc_func mem_alloc,
		       context_free_func mem_free,
		       void *mem_context,
		       struct deque_s *deque,
		       struct deque_data *d,
		       void **data_space, size_t data_space_len)
{
	d->data_space = data_space;
	d->data_space_len = data_space_len;
	d->first_pos = Deque_default_unshift_space;
	d->end_pos = Deque_default_unshift_space;

	d->mem_alloc = mem_alloc;
	d->mem_free = mem_free;
	d->mem_context = mem_context;

	deque->opaque_data = NULL;
	deque->push = deque_push;
	deque->pop = deque_pop;
	deque->unshift = deque_unshift;
	deque->shift = deque_shift;
	deque->peek_top = deque_peek_top;
	deque->peek_bottom = deque_peek_bottom;
	deque->size = deque_size;

	deque_set_internal_data(deque, d);
}

struct deque_s *deque_new(void)
{
	return deque_new_custom_allocator(NULL, NULL, NULL);
}

struct deque_s *deque_new_custom_allocator(context_malloc_func mem_alloc,
					   context_free_func mem_free,
					   void *mem_context)
{
	struct deque_s *deque = NULL;
	struct deque_data *d = NULL;
	size_t size = 0;
	void **data_space = NULL;
	size_t data_space_len = Deque_default_len;

	if (!mem_alloc || !mem_free) {
		mem_alloc = context_stdlib_malloc;
		mem_free = context_stdlib_free;
		mem_context = NULL;
	}

	size = sizeof(struct deque_s);
	deque = (struct deque_s *)mem_alloc(mem_context, size);
	if (!deque) {
		return NULL;
	}

	size = sizeof(struct deque_data);
	d = (struct deque_data *)mem_alloc(mem_context, size);
	if (!d) {
		mem_free(mem_context, deque);
		return NULL;
	}

	size = sizeof(void *) * data_space_len;
	data_space = (void **)mem_alloc(mem_context, size);
	if (!data_space) {
		mem_free(mem_context, d);
		mem_free(mem_context, deque);
		return NULL;
	}

	deque_init(mem_alloc, mem_free, mem_context,
		   deque, d, data_space, data_space_len);

	return deque;
}

void *deque_no_alloc(void *context, size_t size)
{
	(void)context;
	(void)size;
	return NULL;
}

struct deque_s *deque_new_no_allocator(unsigned char *bytes, size_t bytes_len)
{
	context_malloc_func mem_alloc = deque_no_alloc;
	context_free_func mem_free = NULL;
	void *mem_context = NULL;

	struct deque_s *deque = NULL;
	struct deque_data *d = NULL;
	void **data_space = NULL;
	size_t data_space_len = 0;

	size_t min_size = 0;
	size_t used = 0;

	if (!bytes) {
		return NULL;
	}

	/* we need at least room for the structs and the default length */
	min_size = Deque_align(sizeof(struct deque_s))
	    + Deque_align(sizeof(struct deque_data))
	    + (4 * sizeof(void *));

	/* if we grow more than 256 bytes, we should bump the version
	 * and update deque.h and docs */
	assert(min_size <= 256);

	if (bytes_len < min_size) {
		return NULL;
	}

	deque = (struct deque_s *)bytes;
	used = Deque_align(sizeof(struct deque_s));
	d = (struct deque_data *)(bytes + used);
	used += Deque_align(sizeof(struct deque_data));
	data_space = (void **)(bytes + used);
	data_space_len = (bytes_len - used) / sizeof(void *);
	deque_init(mem_alloc, mem_free, mem_context,
		   deque, d, data_space, data_space_len);
	return deque;
}

void deque_free(struct deque_s *deque)
{
	struct deque_data *d = NULL;
	context_free_func mem_free = NULL;
	void *mem_context = NULL;

	if (!deque) {
		return;
	}

	d = deque_get_internal_data(deque);

	mem_free = d->mem_free;
	mem_context = d->mem_context;

	mem_free(mem_context, d->data_space);
	mem_free(mem_context, d);
	mem_free(mem_context, deque);
}
