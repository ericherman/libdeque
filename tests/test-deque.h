/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-deque.h */
/* Copyright (C) 2016, 2019 Eric Herman <eric@freesa.org> */

#include "echeck.h"
#include "deque.h"

#ifdef ARDUINO
#define DEQUE_HOSTED 0
#endif

#ifndef DEQUE_HOSTED
#ifdef __STDC_HOSTED__
#define DEQUE_HOSTED __STDC_HOSTED__
#else
#define DEQUE_HOSTED 1
#endif
#endif

#if DEQUE_HOSTED
#define TEST_DEQUE_MAIN_IF(val, func) \
int main(int argc, char **argv) \
{ \
	struct echeck_log *elog = echeck_default_log; \
	int failures = 0; \
	(void)(argc); \
	(void)(argv); \
	if (val) { \
		failures += func; \
	} \
	if (failures) { \
		elog->append_l(elog, failures); \
		elog->append_s(elog, " failures in "); \
		elog->append_s(elog, __FILE__); \
		elog->append_eol(elog); \
	} \
	return failures ? 1 : 0; \
}

#define TEST_DEQUE_MAIN(func) TEST_DEQUE_MAIN_IF(1, func)
#else
#define TEST_DEQUE_MAIN_IF(val, func)	/* nothing */
#define TEST_DEQUE_MAIN(func)	/* nothing */
#endif
