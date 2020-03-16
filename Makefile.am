# SPDX-License-Identifier: LGPL-3.0-or-later
# Copyright (C) 2016, 2019, 2020 Eric Herman <eric@freesa.org>

if DEBUG
BUILD_TYPE_CFLAGS=-ggdb -O0 \
	-fsanitize=address,leak,undefined \
	-fno-inline-small-functions \
	-fkeep-inline-functions \
	-fkeep-static-functions \
	--coverage
BUILD_TYPE_LDFLAGS=--coverage
else
BUILD_TYPE_CFLAGS=-ggdb -O2 -DNDEBUG -Wno-unused-parameter -fomit-frame-pointer
BUILD_TYPE_LDFLAGS=
endif

AM_CFLAGS=-std=c89 \
	-Wall -Wextra -Wpedantic -Werror \
	$(BUILD_TYPE_CFLAGS) \
	-I./submodules/context-alloc/src -I../../submodules/context-alloc/src \
	-pipe

AM_LDFLAGS=$(BUILD_TYPE_LDFLAGS)

lib_LTLIBRARIES=libdeque.la
libdeque_la_SOURCES=src/deque.c \
		submodules/context-alloc/src/context-alloc.c

include_HEADERS=src/deque.h \
		submodules/context-alloc/src/context-alloc.h

TESTS=$(check_PROGRAMS)
check_PROGRAMS=\
 test-deque-new \
 test-push-pop \
 test-peek \
 test-custom-allocator \
 test-out-of-memory

T_LDADD=libdeque.la

TEST_COMMON_SOURCES=\
 src/deque.h \
 tests/test-deque.h \
 submodules/context-alloc/util/oom-injecting-malloc.h \
 submodules/context-alloc/util/oom-injecting-malloc.c \
 submodules/libecheck/src/echeck.h \
 submodules/libecheck/src/echeck.c

test_deque_new_SOURCES=$(TEST_COMMON_SOURCES) tests/test-deque-new.c
test_deque_new_LDADD=$(T_LDADD)

test_push_pop_SOURCES=$(TEST_COMMON_SOURCES) tests/test-push-pop.c
test_push_pop_LDADD=$(T_LDADD)

test_peek_SOURCES=$(TEST_COMMON_SOURCES) tests/test-peek.c
test_peek_LDADD=$(T_LDADD)

test_custom_allocator_SOURCES=$(TEST_COMMON_SOURCES) \
 tests/test-custom-allocator.c
test_custom_allocator_LDADD=$(T_LDADD)

test_out_of_memory_SOURCES=$(TEST_COMMON_SOURCES) tests/test-out-of-memory.c
test_out_of_memory_LDADD=$(T_LDADD)

ACLOCAL_AMFLAGS=-I m4 --install

EXTRA_DIST=COPYING.LESSER \
 submodules/context-alloc/COPYING \
 submodules/context-alloc/COPYING.LESSER \
 submodules/context-alloc/src/context-alloc.h \
 submodules/context-alloc/src/context-alloc.c \
 submodules/context-alloc/util/oom-injecting-malloc.h \
 submodules/context-alloc/util/oom-injecting-malloc.c \
 submodules/libecheck/COPYING \
 submodules/libecheck/COPYING.LESSER \
 submodules/libecheck/src/echeck.h \
 submodules/libecheck/src/echeck.c

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

tidy:
	$(LINDENT) \
		-T FILE \
		-T size_t \
		-T deque_s \
		`find src tests -name '*.h' -o -name '*.c'`

spotless:
	rm -rf `cat .gitignore | sed -e 's/#.*//'`
	pushd src && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd
	pushd tests && rm -rf `cat ../.gitignore | sed -e 's/#.*//'` && popd
	pushd submodules/context-alloc && git clean -dxf && popd

vg-test-deque-new: test-deque-new
	./libtool --mode=execute valgrind -q ./test-deque-new

vg-test-push-pop: test-push-pop
	./libtool --mode=execute valgrind -q ./test-push-pop

vg-test-peek: test-peek
	./libtool --mode=execute valgrind -q ./test-peek

vg-test-custom-allocator: test-custom-allocator
	./libtool --mode=execute valgrind -q ./test-custom-allocator

vg-test-out-of-memory: test-out-of-memory
	./libtool --mode=execute valgrind -q ./test-out-of-memory


valgrind: \
	vg-test-custom-allocator \
	vg-test-deque-new \
	vg-test-out-of-memory \
	vg-test-peek \
	vg-test-push-pop
