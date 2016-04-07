/*
   Inspired by: Donald Knuth.
   The Art of Computer Programming, Volume 1: Fundamental Algorithms
   Section 2.2.1: Stacks, Queues, and Deques, pp. 238-243.
   ( third edition, Addison-Wesley, 1997. ISBN 0-201-89683-4 )
*/
#include "deque.h"
#include "deque-structs.h"
#include <stdlib.h>
#include <assert.h>

static void *deque_memalloc(size_t size, void *context)
{
	assert(context == NULL);
	return malloc(size);
}

static void deque_memfree(void *ptr, size_t size, void *context)
{
	assert(size != 0);
	assert(context == NULL);
	free(ptr);
}

static void *deque_top(struct deque_s *deque)
{
	struct deque_data_s *d;

	d = deque->data;

	return d->top ? d->top->data : NULL;
}

static void *deque_bottom(struct deque_s *deque)
{
	struct deque_data_s *d;

	d = deque->data;
	return d->bottom ? d->bottom->data : NULL;
}

static size_t deque_size(struct deque_s *deque)
{
	struct deque_data_s *d;

	d = deque->data;
	return d->size;
}

static struct deque_s *deque_push(struct deque_s *deque, void *data)
{
	struct deque_data_s *d;
	struct deque_element_s *e;

	d = deque->data;

	e = d->mem_alloc(sizeof(struct deque_element_s), d->mem_context);
	if (!e) {
		return NULL;
	}
	e->data = data;
	e->above = NULL;

	if (d->size == 0) {
		d->bottom = e;
		e->below = NULL;
	} else {
		e->below = d->top;
		d->top->above = e;
	}
	d->top = e;
	++d->size;
	return deque;
}

static void *deque_pop(struct deque_s *deque)
{
	struct deque_data_s *d;
	struct deque_element_s *freeme;
	void *data;

	d = deque->data;

	if (d->size == 0) {
		return NULL;
	}

	freeme = d->top;
	data = freeme->data;
	if (d->bottom == freeme) {
		d->top = NULL;
		d->bottom = NULL;
	} else {
		d->top = freeme->below;
	}
	d->mem_free(freeme, sizeof(struct deque_element_s), d->mem_context);
	--d->size;
	return data;
}

static struct deque_s *deque_unshift(struct deque_s *deque, void *data)
{
	struct deque_data_s *d;
	struct deque_element_s *e;

	d = deque->data;
	e = d->mem_alloc(sizeof(struct deque_element_s), d->mem_context);
	if (!e) {
		return NULL;
	}
	e->data = data;
	e->below = NULL;

	if (d->size == 0) {
		d->top = e;
		e->above = NULL;
	} else {
		e->above = d->bottom;
		d->bottom->below = e;
	}
	d->bottom = e;
	++d->size;
	return deque;
}

static void *deque_shift(struct deque_s *deque)
{
	struct deque_data_s *d;
	void *data;
	struct deque_element_s *freeme;

	d = deque->data;
	freeme = d->bottom;
	data = freeme->data;
	if (d->top == freeme) {
		d->top = NULL;
		d->bottom = NULL;
	} else {
		d->bottom = freeme->above;
	}
	d->mem_free(freeme, sizeof(struct deque_element_s), d->mem_context);
	--d->size;
	return data;
}

struct deque_s *deque_init(struct deque_s *deque, deque_malloc_func a,
			   deque_free_func f, void *mem_context)
{
	struct deque_data_s *d;

	deque->push = deque_push;
	deque->pop = deque_pop;
	deque->unshift = deque_unshift;
	deque->shift = deque_shift;
	deque->top = deque_top;
	deque->bottom = deque_bottom;
	deque->size = deque_size;

	if (!a) {
		a = deque_memalloc;
	}
	d = a(sizeof(struct deque_data_s), mem_context);
	if (!d) {
		deque_memfree(deque, sizeof(struct deque_s), mem_context);
		return NULL;
	}

	d->top = NULL;
	d->bottom = NULL;
	d->size = 0;

	d->mem_alloc = a;
	d->mem_free = f ? f : deque_memfree;
	d->mem_context = mem_context;

	deque->data = d;

	return deque;
}

struct deque_s *deque_new()
{
	struct deque_s *deque;
	void *mem_context;

	mem_context = NULL;

	deque = deque_memalloc(sizeof(struct deque_s), mem_context);
	if (!deque) {
		return NULL;
	}

	return deque_init(deque, deque_memalloc, deque_memfree, mem_context);
}

void deque_free(struct deque_s *deque)
{
	struct deque_data_s *d;

	d = deque->data;
	while (d->top) {
		deque_pop(deque);
	}
	d->mem_free(deque, sizeof(struct deque_s), d->mem_context);
	d->mem_free(d, sizeof(struct deque_data_s), d->mem_context);
}
