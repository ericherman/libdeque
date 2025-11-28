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

#define deque_assert(d) do { \
	eembed_assert(d != NULL); \
	eembed_assert(d->data_space != NULL); \
	eembed_assert(d->ea != NULL); \
	eembed_assert(d->first_pos <= d->end_pos); \
	eembed_assert(d->end_pos <= d->data_space_len); \
} while (0)

void *deque_peek_top(struct deque *d, size_t index)
{
	size_t i = 0;

	deque_assert(d);

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

void *deque_peek_bottom(struct deque *d, size_t index)
{
	size_t i = 0;

	deque_assert(d);

	if (d->first_pos == d->end_pos) {
		return NULL;
	}
	i = index + d->first_pos;
	if (i >= d->end_pos) {
		return NULL;
	}
	return d->data_space[i];
}

size_t deque_size(struct deque *d)
{
	deque_assert(d);

	return d->end_pos - d->first_pos;
}

struct deque *deque_push(struct deque *d, void *user_data)
{
	deque_assert(d);

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
			size_t pos_shift =
			    Deque_default_unshift_space(new_space_len);
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
	return d;
}

void *deque_pop(struct deque *d)
{
	void *user_data = NULL;

	deque_assert(d);

	if (d->end_pos == d->first_pos) {
		return NULL;
	}

	eembed_assert(d->end_pos > 0);

	user_data = d->data_space[--d->end_pos];
	d->data_space[d->end_pos] = NULL;

	eembed_assert(d->first_pos <= d->end_pos);

	return user_data;
}

struct deque *deque_unshift(struct deque *d, void *user_data)
{
	deque_assert(d);

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

	return d;
}

void *deque_shift(struct deque *d)
{
	void *user_data = NULL;

	deque_assert(d);

	if (d->first_pos == d->end_pos) {
		return NULL;
	}

	eembed_assert(d->first_pos < d->data_space_len);

	user_data = d->data_space[d->first_pos];
	d->data_space[d->first_pos] = NULL;
	++d->first_pos;

	if (d->first_pos == d->end_pos) {
		size_t pos = Deque_default_unshift_space(d->data_space_len);
		d->first_pos = pos;
		d->end_pos = pos;
	}

	eembed_assert(d->first_pos <= d->end_pos);

	return user_data;
}

void deque_clear(struct deque *d)
{
	deque_assert(d);
	d->first_pos = Deque_default_unshift_space(d->data_space_len);
	d->end_pos = d->first_pos;
}

int deque_for_each(struct deque *d, deque_iterator_func pfunc, void *context)
{
	size_t i, end;

	deque_assert(d);

	end = 0;
	for (i = d->first_pos; i < d->end_pos && !end; ++i) {
		end = pfunc(d, d->data_space[i], context);
	}
	return end;
}

struct deque *deque_init(struct deque *d, void **data_space,
			 size_t data_space_len, struct eembed_allocator *ea)
{

	if (!ea) {
		ea = eembed_global_allocator;
	}

	if (d) {
		memset(d, 0x00, sizeof(struct deque));
	} else {
		d = (struct deque *)ea->calloc(ea, 1, sizeof(struct deque));
		if (!d) {
			return NULL;
		}
		d->flags.deque_needs_free = 1;
	}

	if (!data_space) {
		if (!data_space_len) {
			data_space_len = Deque_default_len;
		}
		data_space =
		    (void **)ea->calloc(ea, data_space_len, sizeof(void *));
		if (!data_space) {
			if (d->flags.deque_needs_free) {
				ea->free(ea, d);
			}
			return NULL;
		}
		d->flags.data_space_needs_free = 1;
	}

	d->data_space = data_space;
	d->data_space_len = data_space_len;
	d->first_pos = Deque_default_unshift_space(d->data_space_len);
	d->end_pos = d->first_pos;
	d->ea = ea;

	deque_assert(d);

	eembed_memset(d->data_space, 0x00, data_space_len * sizeof(void *));

	return d;
}

struct deque *deque_new(void)
{
	return deque_init(NULL, NULL, 0, NULL);
}

struct deque *deque_new_custom_allocator(struct eembed_allocator *ea)
{
	return deque_init(NULL, NULL, 0, ea);
}

struct deque *deque_new_no_allocator(unsigned char *bytes, size_t bytes_len)
{
	struct eembed_allocator *ea = eembed_null_allocator;
	struct deque *d = NULL;
	void **data_space = NULL;
	size_t data_space_len = 0;

	size_t min_size = 0;
	size_t used = 0;

	if (!bytes) {
		return NULL;
	}

	/* we need at least room for the structs and the default length */
	min_size = eembed_align(sizeof(struct deque)) + (4 * sizeof(void *));

	/* if we grow more than 88 bytes, we should bump the version
	 * and update deque.h and docs */
	eembed_assert(min_size <= 88);

	if (bytes_len < min_size) {
		return NULL;
	}

	eembed_memset(bytes, 0x00, bytes_len);
	d = (struct deque *)bytes;
	used = eembed_align(sizeof(struct deque));
	data_space = (void **)(bytes + used);
	data_space_len = (bytes_len - used) / sizeof(void *);

	return deque_init(d, data_space, data_space_len, ea);
}

void deque_free(struct deque *d)
{
	struct eembed_allocator *ea = NULL;

	if (!d) {
		return;
	}

	eembed_assert(d->ea);

	ea = d->ea;

	if (d->flags.data_space_needs_free) {
		ea->free(ea, d->data_space);
		d->data_space = NULL;
		d->data_space_len = 0;
		d->flags.data_space_needs_free = 0;
		d->first_pos = 0;
		d->end_pos = 0;
	} else {
		size_t size = d->data_space_len * sizeof(void *);
		eembed_memset(d->data_space, 0x00, size);
	}
	if (d->flags.deque_needs_free) {
		ea->free(ea, d);
	} else {
		d->end_pos = d->first_pos;
	}
}
