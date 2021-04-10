#ifndef __TEST_CC_CONFIGPARSE_H__
#define __TEST_CC_CONFIGPARSE_H__

#include "macros.h"
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void test_unescape(void **state);

void test_object(void **state);

void test_configcc_scanner_isAlphaNumeric(void **state);

void test_configcc_scanner(void **state);

void test_configcc_scanner_failure(void **state);

void test_configcc(void **state);

void test_configcc_failure(void **state);

#endif
