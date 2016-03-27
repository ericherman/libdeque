/*
   Inspired by: Donald Knuth.
   The Art of Computer Programming, Volume 1: Fundamental Algorithms
   Section 2.2.1: Stacks, Queues, and Deques, pp. 238-243.
   ( third edition, Addison-Wesley, 1997. ISBN 0-201-89683-4 )
*/
#include "deque.h"
#include <stdlib.h>

struct deque_element_s {
	struct deque_element_s *above;
	struct deque_element_s *below;
	void *data;
};

struct deque_s {
	struct deque_element_s *top;
	struct deque_element_s *bottom;
	size_t size;
};

struct deque_s *deque_new()
{
	struct deque_s *d;

	d = malloc(sizeof(struct deque_s));
	d->top = NULL;
	d->bottom = NULL;
	d->size = 0;
	return d;
}

void deque_free(struct deque_s *d)
{
	while (d->top) {
		deque_pop(d);
	}
	free(d);
}

void *deque_top(struct deque_s *d)
{
	return d->top ? d->top->data : NULL;
}

void *deque_bottom(struct deque_s *d)
{
	return d->bottom ? d->bottom->data : NULL;
}

size_t deque_size(struct deque_s *d)
{
	return d->size;
}

struct deque_s *deque_push(struct deque_s *d, void *data)
{
	struct deque_element_s *e;

	e = malloc(sizeof(struct deque_element_s));
	if (!e) {
		return NULL;
	}
	e->data = data;
	e->above = NULL;

	if (d->size == 0) {
		d->top = e;
		d->bottom = e;
		e->below = NULL;
	} else {
		e->below = d->top;
		d->top->above = e;
		d->top = e;
	}
	++d->size;
	return d;
}

void *deque_pop(struct deque_s *d)
{
	struct deque_element_s *freeme;
	void *data;

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
	free(freeme);
	--d->size;
	return data;
}

struct deque_s *deque_unshift(struct deque_s *d, void *data)
{
	struct deque_element_s *e;

	e = malloc(sizeof(struct deque_element_s));
	if (!e) {
		return NULL;
	}
	e->data = data;
	e->below = NULL;

	if (d->size == 0) {
		d->top = e;
		d->bottom = e;
		e->above = NULL;
	} else {
		e->above = d->bottom;
		d->bottom->below = e;
		d->bottom = e;
	}
	++d->size;
	return d;
}

void *deque_shift(struct deque_s *d)
{
	void *data;
	struct deque_element_s *freeme;

	freeme = d->bottom;
	data = freeme->data;
	if (d->top == freeme) {
		d->top = NULL;
		d->bottom = NULL;
	} else {
		d->bottom = freeme->above;
	}
	free(freeme);
	--d->size;
	return data;
}
