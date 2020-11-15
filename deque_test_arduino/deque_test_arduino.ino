/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* eba_test_arduino.ino */
/* Copyright (C) 2020 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <Arduino.h>
#include <HardwareSerial.h>
#include "deque.h"
#include "oom-injecting-malloc.h"
#include "echeck-arduino.h"

int test_deque_custom_allocator(void);
int test_deque_new(void);
int test_iteration(void);
int test_deque_new_no_allocator(void);
int test_out_of_memory(void);
int test_peek(void);
int test_deque_push_pop(void);

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

const int buf_size = 600;
char buf0[buf_size];
char buf1[buf_size];
char buf2[buf_size];
char buf3[buf_size];
char buf4[buf_size];
bool used[5] = { false, false, false, false, false };
char *bufs[5] = { buf0, buf1, buf2, buf3, buf4 };

void *bogus_malloc(size_t size)
{
	if (size > buf_size) {
		/* Serial.print("Could not allocate size "); */
		/* Serial.println(size); */
		return NULL;
	}
	for (int i = 0; i < 5; ++i) {
		if (!used[i]) {
			used[i] = true;
			return bufs[i];
		}
	}
	Serial.print("Could not allocate size ");
	Serial.print(size);
	Serial.print(" (no free buffers)");
	return NULL;
}

void bogus_free(void *ptr)
{
	for (int i = 0; i < 5; ++i) {
		if (ptr == bufs[i]) {
			used[i] = false;
			return;
		}
	}
	Serial.print("Error, could not free pointer at location ");
	Serial.println((unsigned long)ptr);
}

/* globals */
uint32_t loop_count;

void setup(void)
{
	Serial.begin(115200);
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

	stdlib_malloc = bogus_malloc;
	stdlib_free = bogus_free;
	oom_injecting_memset = diy_memset;
	oom_injecting_memcpy = deque_memmove;

	Serial.println("Begin");
	delay(1000);

	loop_count = 0;
}

#define Test(func) do { \
	Serial.print(#func " ..."); \
	int fail = func(); \
	Serial.println(fail ? " FAIL!" : " ok."); \
	failures += fail; \
	for (int i = 0; i < 5; ++i) { \
		if (used[i]) { \
			Serial.print("buffer "); \
			Serial.print(i); \
			Serial.println(" not freed?"); \
			used[i] = false; \
		} \
	} \
} while(0)

void loop(void)
{
	int failures = 0;
	Serial.println("=================================================");
	++loop_count;
	Serial.print("Starting run #");
	Serial.println(loop_count);
	Serial.println("=================================================");

	Test(test_deque_custom_allocator);
	Test(test_deque_new);
	Test(test_deque_new_no_allocator);
	Test(test_deque_push_pop);
	Test(test_iteration);
	Test(test_out_of_memory);
	Test(test_peek);

	Serial.println("=================================================");
	if (failures) {
		Serial.print("failures = ");
		Serial.println(failures);
		Serial.println("FAIL");
	} else {
		Serial.println("SUCCESS");
	}
	Serial.println("=================================================");

	for (int i = 0; i < 20; ++i) {
		Serial.print(".");
		delay(failures ? 1000 : 100);
	}
	Serial.println();
}
