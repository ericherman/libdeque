/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* eba_test_arduino.ino */
/* Copyright (C) 2020 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <Arduino.h>
#include <HardwareSerial.h>

#include "deque.h"
#include "eembed-arduino.h"

const unsigned long Serial_baud = 9600;

/* the deque test functions */
unsigned test_deque_custom_allocator(void);
unsigned test_deque_new_no_allocator(void);
unsigned test_deque_new(void);
unsigned test_deque_push_pop(void);
unsigned test_iteration(void);
unsigned test_out_of_memory(void);
unsigned test_peek(void);

/* Keep Byte_allocator_buflen under 256 bytes in order to keep the
   whole SRAM usage (including stack needs) down to under 2.5k (or
   ideally under 2k), so that it will be able to run on even the smaller
   (if not the very smallest) Arduinos.

   This output was copy-pasted from a Mega build, which shows that
   program storages space is plentiful, but RAM remains scarce:

   Sketch uses 20870 bytes (8%) of program storage space.
     Maximum is 253952 bytes.
   Global variables use 2200 bytes (26%) of dynamic memory,
     leaving 5992 bytes for local variables.
     Maximum is 8192 bytes.
*/
const size_t Byte_allocator_buflen = 256;
unsigned char byte_allocator_buf[Byte_allocator_buflen];

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
	eembed_arduino_serial_log_init();

	Serial.println("Begin");
	delay(1000);

	loop_count = 0;
}

unsigned test_func(const char *name, int (*func)(void))
{
	Serial.print(name);
	Serial.print(" ...");
	byte_allocator_init();
	unsigned fail = func();
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

void byte_allocator_init(void)
{
	struct eembed_allocator *ba;

	ba = eembed_bytes_allocator(byte_allocator_buf, Byte_allocator_buflen);
	if (ba) {
		eembed_global_allocator = ba;
	} else {
		Serial.println();
		Serial.println();
		Serial.println("eembed_bytes_allocator returned NULL?");
		Serial.println();
		Serial.println();
	}

}
