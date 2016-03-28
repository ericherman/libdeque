#ifndef DEQUE_STRUCTS_H
#define DEQUE_STRUCTS_H

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

#endif /* DEQUE_STRUCTS_H */
