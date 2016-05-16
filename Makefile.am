lib_LTLIBRARIES=libdeque.la
libdeque_la_SOURCES=src/deque.c

include_HEADERS=src/deque.h

TESTS=$(check_PROGRAMS)
check_PROGRAMS=test-deque
test_deque_SOURCES=tests/test-deque.c src/deque.h
test_deque_LDADD=libdeque.la -lecheck

ACLOCAL_AMFLAGS=-I m4 --install

AM_CFLAGS=-std=c89 -O3 -ggdb -Wall -Wextra -Wpedantic -Werror

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

tidy:
	$(LINDENT) \
		-T FILE \
		-T size_t \
		-T deque_s \
		`find src tests -name '*.h' -o -name '*.c'`

spotless:
	rm -rf `cat .gitignore`
	( pushd src; rm -rf `cat ../.gitignore`; popd )
	( pushd tests; rm -rf `cat ../.gitignore`; popd )

valgrind: test-deque
	 valgrind ./test-deque 0
