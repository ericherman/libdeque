LIB_NAME=deque

AUX_INCLUDES=-I ../libecheck
AUX_A_FILES=../libecheck/libecheck.a
AUX_LDFLAGS=-L../libecheck
AUX_LDADD=-lecheck
AUX_LD_LIBRARY_PATHS=:../libecheck


UNAME := $(shell uname)

ifeq ($(UNAME), Darwin)
SHAREDFLAGS = -dynamiclib
SHAREDEXT = dylib
else
SHAREDFLAGS = -shared
SHAREDEXT = so
endif

LIB_SRC=$(LIB_NAME).c
LIB_HDR=$(LIB_NAME).h
LIB_OBJ=$(LIB_SRC:.c=.o)
SO_OBJS=$(LIB_NAME).o
SO_NAME=lib$(LIB_NAME).$(SHAREDEXT)
ifneq ($(UNAME), Darwin)
    SHAREDFLAGS += -Wl,-soname,$(SO_NAME)
endif

A_NAME=lib$(LIB_NAME).a

INCLUDES=-I.
TEST_SRC=test-$(LIB_NAME).c
TEST_OBJ=test-$(LIB_NAME).o
TEST=test-$(LIB_NAME)

CSTD_CFLAGS=-std=c89
#DEBUG_CFLAGS=-ggdb -O0
DEBUG_CFLAGS=-ggdb -O3 -DNDEBUG -Wno-unused-parameter
NOISY_CFLAGS=-Wall -Wextra -pedantic -Werror

CFLAGS += $(CSTD_CFLAGS) $(DEBUG_CFLAGS) $(NOISY_CFLAGS)
LDFLAGS += -L.
LDADD += -l$(LIB_NAME)
CC=gcc

ifeq ("$(PREFIX)", "")
PREFIX=/usr/local
endif

ifeq ("$(LIBDIR)", "")
LIBDIR=$(PREFIX)/lib
endif

ifeq ("$(INCDIR)", "")
INCDIR=$(PREFIX)/include
endif

LD_LIBRARY_PATH=.$(AUX_LD_LIBRARY_PATHS)

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

default: library

.c.o:
	$(CC) -c -fPIC $(CFLAGS) $< -o $@

$(SO_NAME): $(LIB_OBJ)
	$(CC) $(SHAREDFLAGS) -o $(SO_NAME).1.0 $(SO_OBJS)
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME).1
	ln -sf ./$(SO_NAME).1.0 ./$(SO_NAME)

$(A_NAME): $(LIB_OBJ)
	ar -r $(A_NAME) $(SO_OBJS)

library: $(SO_NAME) $(A_NAME)

$(TEST): library
	$(CC) -c $(INCLUDES) $(AUX_INCLUDES) $(CFLAGS) \
		$(TEST_SRC) -o $(TEST_OBJ)
	$(CC) $(TEST_OBJ) $(A_NAME) $(AUX_A_FILES) -o $(TEST)-static
	$(CC) $(LDFLAGS) $(TEST_OBJ) $(AUX_LDFLAGS) -o $(TEST)-dynamic \
		$(LDADD) $(AUX_LDADD)

check: $(TEST)
	./$(TEST)-static ${TEST_OOM}
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) ./$(TEST)-dynamic

valgrind: $(TEST)
	valgrind ./$(TEST)-static ${TEST_OOM}

tidy:
	$(LINDENT) \
		-T FILE \
		-T size_t \
		-T deque_s \
		*.h *.c

clean:
	rm -f *.o *.a *.$(SHAREDEXT) \
		$(SO_NAME).* \
		$(TEST)-static $(TEST)-dynamic demo-deque

install: library
	@echo "Installing libraries in $(LIBDIR)"
	mkdir -pv $(LIBDIR)
	cp -pv $(A_NAME) $(LIBDIR)/
	cp -Rv $(SO_NAME)* $(LIBDIR)/
	@echo "Installing headers in $(INCDIR)"
	mkdir -pv $(INCDIR)
	cp -pv $(LIB_HDR) $(INCDIR)/
