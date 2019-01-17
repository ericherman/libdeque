/* deque.c Double-Ended QUEue
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

/*
   Inspired by: Donald Knuth.
   The Art of Computer Programming, Volume 1: Fundamental Algorithms
   Section 2.2.1: Stacks, Queues, and Deques, pp. 238-243.
   ( third edition, Addison-Wesley, 1997. ISBN 0-201-89683-4 )
*/
#include "deque.h"
#include <stdlib.h>
#include <assert.h>

struct deque_element_s {
	struct deque_element_s *above;
	struct deque_element_s *below;
	void *user_data;
};

struct deque_data_s {
	struct deque_element_s *top_de;
	struct deque_element_s *bottom_de;
	size_t size;

	deque_malloc_func mem_alloc;
	deque_free_func mem_free;
	void *mem_context;
};

static void *deque_memalloc(size_t size, void *context)
{
	assert(context == NULL);
	return malloc(size);
}

static void deque_memfree(void *ptr, void *context)
{
	assert(size != 0);
	assert(context == NULL);
	free(ptr);
}

static struct deque_data_s *deque_get_internal_data(struct deque_s *deque)
{
	assert(deque != NULL);
	assert(deque->opaque_data != NULL);
	return (struct deque_data_s *)deque->opaque_data;
}

static void deque_set_internal_data(struct deque_s *deque,
				    struct deque_data_s *d)
{
	assert(deque != NULL);
	assert(d != NULL);
	assert(deque->opaque_data == NULL);
	deque->opaque_data = (void *)d;
}

static void *deque_peek_top(struct deque_s *deque)
{
	struct deque_data_s *d;

	d = deque_get_internal_data(deque);

	return d->top_de ? d->top_de->user_data : NULL;
}

static void *deque_peek_bottom(struct deque_s *deque)
{
	struct deque_data_s *d;

	d = deque_get_internal_data(deque);
	return d->bottom_de ? d->bottom_de->user_data : NULL;
}

static size_t deque_size(struct deque_s *deque)
{
	struct deque_data_s *d;

	d = deque_get_internal_data(deque);
	return d->size;
}

static struct deque_s *deque_push(struct deque_s *deque, void *user_data)
{
	struct deque_data_s *d;
	struct deque_element_s *e;

	d = deque_get_internal_data(deque);

	e = (struct deque_element_s *)
	    d->mem_alloc(sizeof(struct deque_element_s), d->mem_context);
	if (!e) {
		return NULL;
	}
	e->user_data = user_data;
	e->above = NULL;

	if (d->size == 0) {
		d->bottom_de = e;
		e->below = NULL;
	} else {
		e->below = d->top_de;
		d->top_de->above = e;
	}
	d->top_de = e;
	++d->size;
	return deque;
}

static void *deque_pop(struct deque_s *deque)
{
	struct deque_data_s *d;
	struct deque_element_s *freeme;
	void *user_data;

	d = deque_get_internal_data(deque);

	if (d->size == 0) {
		return NULL;
	}

	freeme = d->top_de;
	user_data = freeme->user_data;
	if (d->bottom_de == freeme) {
		d->top_de = NULL;
		d->bottom_de = NULL;
	} else {
		d->top_de = freeme->below;
	}
	d->mem_free(freeme, d->mem_context);
	--d->size;
	return user_data;
}

static struct deque_s *deque_unshift(struct deque_s *deque, void *user_data)
{
	struct deque_data_s *d;
	struct deque_element_s *e;

	d = deque_get_internal_data(deque);
	e = (struct deque_element_s *)
	    d->mem_alloc(sizeof(struct deque_element_s), d->mem_context);
	if (!e) {
		return NULL;
	}
	e->user_data = user_data;
	e->below = NULL;

	if (d->size == 0) {
		d->top_de = e;
		e->above = NULL;
	} else {
		e->above = d->bottom_de;
		d->bottom_de->below = e;
	}
	d->bottom_de = e;
	++d->size;
	return deque;
}

static void *deque_shift(struct deque_s *deque)
{
	struct deque_data_s *d;
	void *user_data;
	struct deque_element_s *freeme;

	d = deque_get_internal_data(deque);
	freeme = d->bottom_de;
	user_data = freeme->user_data;
	if (d->top_de == freeme) {
		d->top_de = NULL;
		d->bottom_de = NULL;
	} else {
		d->bottom_de = freeme->above;
	}
	d->mem_free(freeme, d->mem_context);
	--d->size;
	return user_data;
}

struct deque_s *deque_new()
{
	return deque_new_custom_allocator(NULL, NULL, NULL);
}

struct deque_s *deque_new_custom_allocator(deque_malloc_func a,
					   deque_free_func f, void *mem_context)
{
	struct deque_s *deque;
	struct deque_data_s *d;

	if (!a) {
		a = deque_memalloc;
	}
	deque = (struct deque_s *)a(sizeof(struct deque_s), mem_context);
	if (!deque) {
		return NULL;
	}

	deque->push = deque_push;
	deque->pop = deque_pop;
	deque->unshift = deque_unshift;
	deque->shift = deque_shift;
	deque->peek_top = deque_peek_top;
	deque->peek_bottom = deque_peek_bottom;
	deque->size = deque_size;

	d = (struct deque_data_s *)a(sizeof(struct deque_data_s), mem_context);
	if (!d) {
		deque_memfree(deque, mem_context);
		return NULL;
	}

	d->top_de = NULL;
	d->bottom_de = NULL;
	d->size = 0;

	d->mem_alloc = a;
	d->mem_free = f ? f : deque_memfree;
	d->mem_context = mem_context;

	deque_set_internal_data(deque, d);

	return deque;
}

void deque_free(struct deque_s *deque)
{
	struct deque_data_s *d;

	d = deque_get_internal_data(deque);
	while (d->top_de) {
		deque_pop(deque);
	}
	d->mem_free(deque, d->mem_context);
	d->mem_free(d, d->mem_context);
}
