lib_LTLIBRARIES=libdeque.la
libdeque_la_SOURCES=src/deque.c

include_HEADERS=src/deque.h

TESTS=$(check_PROGRAMS)
check_PROGRAMS=\
 test-deque-new \
 test-push-pop \
 test-custom-allocator \
 test-out-of-memory

T_LDADD=libdeque.la -lecheck

test_deque_new_SOURCES=tests/test-deque-new.c src/deque.h tests/test-deque.h
test_deque_new_LDADD=$(T_LDADD)

test_push_pop_SOURCES=tests/test-push-pop.c src/deque.h tests/test-deque.h
test_push_pop_LDADD=$(T_LDADD)

test_custom_allocator_SOURCES=tests/test-custom-allocator.c src/deque.h \
			      tests/test-deque.h
test_custom_allocator_LDADD=$(T_LDADD)

test_out_of_memory_SOURCES=tests/test-out-of-memory.c src/deque.h \
			   tests/test-deque.h
test_out_of_memory_LDADD=$(T_LDADD)

ACLOCAL_AMFLAGS=-I m4 --install

#OPTIMIZE_FLAGS=-ggdb -O0
OPTIMIZE_FLAGS=-ggdb -O3 -DNDEBUG -Wno-unused-parameter
AM_CFLAGS=-std=c89 -Wall -Wextra -Wpedantic -Werror $(OPTIMIZE_FLAGS)

EXTRA_DIST=misc

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

tidy:
	patch -Np1 -i misc/pre-tidy.patch
	$(LINDENT) \
		-T FILE \
		-T size_t \
		-T deque_s \
		`find src tests -name '*.h' -o -name '*.c'`
	patch -Rp1 -i misc/pre-tidy.patch

spotless:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd
	pushd tests && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd

valgrind: $(check_PROGRAMS)
	libtool --mode=execute valgrind -q ./test-deque-new
	libtool --mode=execute valgrind -q ./test-push-pop
	libtool --mode=execute valgrind -q ./test-custom-allocator
