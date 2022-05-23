#ifndef IMAGE_SUITES_H
#define IMAGE_SUITES_H

#include "test/lily-test.h"

void suite_image();
void suite_processing();

#define RUN_TESTS() \
	lily_run_suite(suite_image); \
	lily_run_suite(suite_processing);

#endif
