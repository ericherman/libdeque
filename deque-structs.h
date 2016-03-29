#ifndef DEQUE_STRUCTS_H
#define DEQUE_STRUCTS_H

typedef void *(*deque_malloc) (size_t size, void *context);
typedef void (*deque_mfree) (void *ptr, size_t size, void *context);

struct deque_element_s {
	struct deque_element_s *above;
	struct deque_element_s *below;
	void *data;
};

struct deque_s {
	struct deque_element_s *top;
	struct deque_element_s *bottom;
	size_t size;

	deque_malloc mem_alloc;
	deque_mfree mem_free;
	void *mem_context;
};

void deque_init(struct deque_s *d, deque_malloc a, deque_mfree f,
		void *context);

#endif /* DEQUE_STRUCTS_H */
