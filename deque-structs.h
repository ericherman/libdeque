#ifndef DEQUE_STRUCTS_H
#define DEQUE_STRUCTS_H

typedef void *(*deque_malloc_func) (size_t size, void *context);
typedef void (*deque_free_func) (void *ptr, size_t size, void *context);

struct deque_element_s {
	struct deque_element_s *above;
	struct deque_element_s *below;
	void *data;
};

struct deque_data_s {
	struct deque_element_s *top;
	struct deque_element_s *bottom;
	size_t size;

	deque_malloc_func mem_alloc;
	deque_free_func mem_free;
	void *mem_context;
};

struct deque_s *deque_init(struct deque_s *deque, deque_malloc_func a,
			   deque_free_func f, void *context);

#endif /* DEQUE_STRUCTS_H */
