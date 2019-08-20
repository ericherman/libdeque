/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-deque.h */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include <stdio.h>
#include "echeck.h"
#include "../src/deque.h"

#define TEST_DEQUE_MAIN_IF(val, func) \
int main(int argc, char **argv) \
{ \
	int failures = 0; \
	if (val) { \
		failures += func; \
	} \
	if (failures) { \
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__); \
	} \
	return failures ? 1 : 0; \
}

#define TEST_DEQUE_MAIN(func) TEST_DEQUE_MAIN_IF(1, func)
