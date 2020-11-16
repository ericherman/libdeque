/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* eba_test_arduino.ino */
/* Copyright (C) 2020 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <Arduino.h>
#include <HardwareSerial.h>
#include "deque.h"
#include "oom-injecting-malloc.h"
#include "echeck-arduino.h"

const unsigned long Serial_baud = 115200;

/* the deque test functions */
int test_deque_custom_allocator(void);
int test_deque_new_no_allocator(void);
int test_deque_new(void);
int test_deque_push_pop(void);
int test_iteration(void);
int test_out_of_memory(void);
int test_peek(void);

/* Keep Bogus_allocator_buflen under 256 bytes in order to keep the
   whole SRAM usage (including stack needs) down to under 2.5k (or
   ideally under 2k), so that it will be able to run on even the smaller
   (if not the very smallest) Arduinos.

   This output was copy-pasted from a Mega build, which shows that
   program storages space is plentiful, but RAM remains scarce:

   Sketch uses 16116 bytes (6%) of program storage space.
     Maximum is 253952 bytes.
   Global variables use 2022 bytes (24%) of dynamic memory,
     leaving 6170 bytes for local variables. Maximum is 8192 bytes.
*/
/* no individual allocation needs to be larger than this: */
const unsigned Max_alloc_size = 64 + sizeof(size_t) + 1;
/* we want to have at least this many objects allocated simultaneously: */
const unsigned Min_concurrent_allocs = 3;
const unsigned Desired_len = Max_alloc_size * Min_concurrent_allocs;
const unsigned Bogus_allocator_buflen = Desired_len < 256 ? Desired_len : 256;
const unsigned Max_allocs = Bogus_allocator_buflen / Max_alloc_size;
/* 0 is silent, 1 is normal, 2 is noisy */
const int Bogus_malloc_verbosity = 1;

/* globals */
/* Set aside some memory for our *totally* *bogus* memory allocator. */
char bogus_allocator_buf[Bogus_allocator_buflen];
bool bogus_allocator_chunks_in_use[Max_allocs];

uint32_t loop_count;

void setup(void)
{
	Serial.begin(Serial_baud);
	delay(50);
	Serial.println();
	for (int i = 0; i < 20; ++i) {
		Serial.print("*");
		delay(1000 / 20);
	}
	Serial.println();

	Serial.println("echeck_arduino_serial_log_init");
	delay(50);
	echeck_arduino_serial_log_init();

	Serial.println("bogus_allocator_init");
	delay(50);
	bogus_allocator_init();

	Serial.println("oom_injecting_init");
	delay(50);
	oom_injecting_init();

	Serial.println("Begin");
	delay(1000);

	loop_count = 0;
}

int test_func(const char *name, int (*func)(void))
{
	Serial.print(name);
	Serial.print(" ...");
	reset_bogus_allocator();
	int fail = func();
	Serial.println(fail ? " FAIL!" : " ok.");
	return fail;
}

#define Test_func(func) test_func(#func, func);

void loop(void)
{
	++loop_count;

	Serial.println();
	Serial.println("=================================================");
	Serial.print("Starting run #");
	Serial.println(loop_count);
	Serial.println("=================================================");

	int failures = 0;
	failures += Test_func(test_deque_custom_allocator);
	failures += Test_func(test_deque_new);
	failures += Test_func(test_deque_new_no_allocator);
	failures += Test_func(test_deque_push_pop);
	failures += Test_func(test_iteration);
	failures += Test_func(test_out_of_memory);
	failures += Test_func(test_peek);

	Serial.println("=================================================");
	if (failures) {
		Serial.print("failures = ");
		Serial.println(failures);
		Serial.println("FAIL");
	} else {
		Serial.println("SUCCESS");
	}
	Serial.println("=================================================");

	Serial.println();
	for (int i = 0; i < 20; ++i) {
		Serial.print(".");
		delay(250);
	}
	Serial.println();
}

void errlog_s(void *context, const char *str)
{
	(void)context;
	Serial.print(str);
}

void errlog_z(void *context, size_t z)
{
	(void)context;
	Serial.print(z);
}

void errlog_eol(void *context)
{
	(void)context;
	Serial.println();
}

void *diy_memset(void *s, int c, size_t n)
{
	unsigned char *d;

	if (!s) {
		return NULL;
	}
	d = (unsigned char *)s;
	while (n--) {
		d[n] = c;
	}
	return d;
}

void *bogus_malloc(size_t size)
{
	if (Bogus_malloc_verbosity > 2) {
		Serial.print("bogus_malloc: ");
		Serial.println(size);
	}
	if (size > Max_alloc_size) {
		if (Bogus_malloc_verbosity > 1) {
			Serial.print("Could not allocate size ");
			Serial.println(size);
		}
		return NULL;
	}
	for (size_t i = 0; i < Max_allocs; ++i) {
		if (!bogus_allocator_chunks_in_use[i]) {
			bogus_allocator_chunks_in_use[i] = true;
			return bogus_allocator_buf + (i * Max_alloc_size);
		}
	}
	if (Bogus_malloc_verbosity > 0) {
		Serial.print("Could not allocate size ");
		Serial.print(size);
		Serial.print(" (no free buffers)");
	}
	return NULL;
}

void bogus_free(void *ptr)
{
	if (Bogus_malloc_verbosity > 2) {
		Serial.print("bogus_free: &");
		Serial.println((unsigned long)ptr);
	}
	for (size_t i = 0; i < Max_allocs; ++i) {
		char *bufi = bogus_allocator_buf + (i * Max_alloc_size);
		if (ptr == bufi) {
			bogus_allocator_chunks_in_use[i] = false;
			return;
		}
	}

	Serial.println();
	Serial.println("Programmer Error! Asked to free pointer at location:");
	Serial.print("\t");
	Serial.println((unsigned long)ptr);
	Serial.println("Valid locations are: ");
	for (size_t i = 0; i < Max_allocs; ++i) {
		char *bufi = bogus_allocator_buf + (i * Max_alloc_size);
		Serial.print("\t");
		Serial.println((unsigned long)bufi);
	}
	delay(3 * 1000);
	Serial.println("Continuing.");
}

void bogus_allocator_init(void)
{
	Serial.print("\tBogus_allocator_buflen: ");
	Serial.println(Bogus_allocator_buflen);
	Serial.print("\tMax_allocs: ");
	Serial.println(Max_allocs);
	Serial.print("\tmax alloc size: ");
	Serial.println(Max_alloc_size);
	for (size_t i = 0; i < Max_allocs; ++i) {
		bogus_allocator_chunks_in_use[i] = false;
	}
}

void reset_bogus_allocator(void)
{
	for (size_t i = 0; i < Max_allocs; ++i) {
		if (bogus_allocator_chunks_in_use[i]) {
			if (Bogus_malloc_verbosity > 0) {
				Serial.print("buffer ");
				Serial.print(i);
				Serial.println(" not freed?");
			}
			bogus_allocator_chunks_in_use[i] = false;
		}
	}
}

void oom_injecting_init(void)
{
	stdlib_malloc = bogus_malloc;
	stdlib_free = bogus_free;

	oom_injecting_memset = diy_memset;
	oom_injecting_memcpy = deque_memmove;

	oom_injecting_errlog_s = errlog_s;
	oom_injecting_errlog_z = errlog_z;
	oom_injecting_errlog_eol = errlog_eol;
}
