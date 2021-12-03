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
#include "eembed.h"

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

struct deque_data {
	void **data_space;
	size_t data_space_len;
	size_t first_pos;
	size_t end_pos;

	struct eembed_allocator *ea;
};

static struct deque_data *deque_get_internal_data(deque_s *deque)
{
	eembed_assert(deque != NULL);
	eembed_assert(deque->opaque_data != NULL);
	return (struct deque_data *)deque->opaque_data;
}

static void deque_set_internal_data(deque_s *deque, struct deque_data *d)
{
	eembed_assert(deque != NULL);
	eembed_assert(d != NULL);
	eembed_assert(deque->opaque_data == NULL);
	deque->opaque_data = (void *)d;
}

static void *deque_peek_top(deque_s *deque, size_t index)
{
	size_t i = 0;
	struct deque_data *d = deque_get_internal_data(deque);

	if (d->first_pos == d->end_pos) {
		return NULL;
	}
	if (index >= d->end_pos) {
		return NULL;
	}
	i = d->end_pos - (index + 1);
	if (i < d->first_pos) {
		return NULL;
	}

	return d->data_space[i];
}

static void *deque_peek_bottom(deque_s *deque, size_t index)
{
	size_t i = 0;
	struct deque_data *d = deque_get_internal_data(deque);

	if (d->first_pos == d->end_pos) {
		return NULL;
	}
	i = index + d->first_pos;
	if (i >= d->end_pos) {
		return NULL;
	}
	return d->data_space[i];
}

static size_t deque_size(deque_s *deque)
{
	struct deque_data *d = deque_get_internal_data(deque);

	d = deque_get_internal_data(deque);
	return d->end_pos - d->first_pos;
}

static deque_s *deque_push(deque_s *deque, void *user_data)
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
			eembed_assert(pos_shift > 0);
			eembed_memmove(&d->data_space[new_first_pos],
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
			struct eembed_allocator *ea = d->ea;
			size_t old_size = sizeof(void *) * old_space_len;
			void **new_space = (void **)ea->malloc(ea, size);
			if (!new_space) {
				return NULL;
			}
			eembed_memmove(&new_space[pos_shift], old_space,
				       old_size);
			ea->free(ea, old_space);
			d->data_space = new_space;
			d->data_space_len = new_space_len;
			d->first_pos += pos_shift;
			d->end_pos += pos_shift;
		}
	}
	eembed_assert(d->end_pos < d->data_space_len);
	d->data_space[d->end_pos++] = user_data;
	return deque;
}

static void *deque_pop(deque_s *deque)
{
	void *user_data = NULL;
	struct deque_data *d = deque_get_internal_data(deque);

	if (d->end_pos == d->first_pos) {
		return NULL;
	}

	eembed_assert(d->end_pos > 0);

	user_data = d->data_space[--d->end_pos];

	eembed_assert(d->first_pos <= d->end_pos);

	return user_data;
}

static deque_s *deque_unshift(deque_s *deque, void *user_data)
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
			eembed_memmove(&d->data_space[pos_shift], d->data_space,
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
			struct eembed_allocator *ea = d->ea;
			void **new_space = (void **)ea->malloc(ea, size);
			if (!new_space) {
				return NULL;
			}
			eembed_memmove(&new_space[pos_shift], old_space,
				       old_size);

			ea->free(ea, old_space);
			d->data_space = new_space;
			d->data_space_len = new_space_len;
			d->first_pos += pos_shift;
			d->end_pos += pos_shift;
		}
	}
	eembed_assert(d->first_pos > 0);

	d->data_space[--d->first_pos] = user_data;

	eembed_assert(d->first_pos <= d->end_pos);

	return deque;
}

static void *deque_shift(deque_s *deque)
{
	void *user_data = NULL;
	struct deque_data *d = deque_get_internal_data(deque);

	d = deque_get_internal_data(deque);

	if (d->first_pos == d->end_pos) {
		return NULL;
	}

	eembed_assert(d->first_pos < d->data_space_len);

	user_data = d->data_space[d->first_pos++];

	if (d->first_pos == d->end_pos) {
		size_t pos = Deque_default_unshift_space;
		d->first_pos = pos;
		d->end_pos = pos;
	}

	eembed_assert(d->first_pos <= d->end_pos);

	return user_data;
}

static void deque_clear(deque_s *deque)
{
	struct deque_data *d = deque_get_internal_data(deque);
	d->first_pos = Deque_default_unshift_space;
	d->end_pos = d->first_pos;
}

static int deque_for_each(deque_s *deque, deque_iterator_func pfunc,
			  void *context)
{
	size_t i, end;
	struct deque_data *d = deque_get_internal_data(deque);

	end = 0;
	for (i = d->first_pos; i < d->end_pos && !end; ++i) {
		end = (*pfunc) (deque, d->data_space[i], context);
	}
	return end;
}

static void deque_init(struct eembed_allocator *ea,
		       deque_s *deque,
		       struct deque_data *d,
		       void **data_space, size_t data_space_len)
{
	d->data_space = data_space;
	d->data_space_len = data_space_len;
	d->first_pos = Deque_default_unshift_space;
	d->end_pos = Deque_default_unshift_space;

	d->ea = ea;

	deque->opaque_data = NULL;
	deque->push = deque_push;
	deque->pop = deque_pop;
	deque->unshift = deque_unshift;
	deque->shift = deque_shift;
	deque->clear = deque_clear;
	deque->peek_top = deque_peek_top;
	deque->peek_bottom = deque_peek_bottom;
	deque->size = deque_size;
	deque->for_each = deque_for_each;

	deque_set_internal_data(deque, d);
}

deque_s *deque_new(void)
{
	return deque_new_custom_allocator(NULL);
}

deque_s *deque_new_custom_allocator(struct eembed_allocator *ea)
{
	deque_s *deque = NULL;
	struct deque_data *d = NULL;
	size_t size = 0;
	void **data_space = NULL;
	size_t data_space_len = Deque_default_len;

	if (!ea) {
		ea = eembed_global_allocator;
	}

	size = sizeof(deque_s);
	deque = (deque_s *)ea->malloc(ea, size);
	if (!deque) {
		return NULL;
	}

	size = sizeof(struct deque_data);
	d = (struct deque_data *)ea->malloc(ea, size);
	if (!d) {
		ea->free(ea, deque);
		return NULL;
	}

	size = sizeof(void *) * data_space_len;
	data_space = (void **)ea->malloc(ea, size);
	if (!data_space) {
		ea->free(ea, d);
		ea->free(ea, deque);
		return NULL;
	}

	deque_init(ea, deque, d, data_space, data_space_len);

	return deque;
}

deque_s *deque_new_no_allocator(unsigned char *bytes, size_t bytes_len)
{
	struct eembed_allocator *ea = eembed_null_allocator;
	deque_s *deque = NULL;
	struct deque_data *d = NULL;
	void **data_space = NULL;
	size_t data_space_len = 0;

	size_t min_size = 0;
	size_t used = 0;

	if (!bytes) {
		return NULL;
	}

	/* we need at least room for the structs and the default length */
	min_size = eembed_align(sizeof(deque_s))
	    + eembed_align(sizeof(struct deque_data))
	    + (4 * sizeof(void *));

	/* if we grow more than 256 bytes, we should bump the version
	 * and update deque.h and docs */
	eembed_assert(min_size <= 256);

	if (bytes_len < min_size) {
		return NULL;
	}

	deque = (deque_s *)bytes;
	used = eembed_align(sizeof(deque_s));
	d = (struct deque_data *)(bytes + used);
	used += eembed_align(sizeof(struct deque_data));
	data_space = (void **)(bytes + used);
	data_space_len = (bytes_len - used) / sizeof(void *);
	deque_init(ea, deque, d, data_space, data_space_len);
	return deque;
}

void deque_free(deque_s *deque)
{
	struct deque_data *d = NULL;
	struct eembed_allocator *ea = NULL;

	if (!deque) {
		return;
	}

	d = deque_get_internal_data(deque);

	ea = d->ea;

	ea->free(ea, d->data_space);
	ea->free(ea, d);
	ea->free(ea, deque);
}
