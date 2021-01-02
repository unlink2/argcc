#ifndef __MACROS_H__
#define __MACROS_H__

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
extern "C" {
#include <cmocka.h>
}

#define assert_cc_string_equal(s1, s2) assert_string_equal(s1.c_str(), s2.c_str())

#define assert_throws(exception, ...) try { __VA_ARGS__; assert_true(false);} catch (exception &e) { assert_true(true); }

#endif
