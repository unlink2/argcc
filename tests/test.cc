#include "test_argcc.h"
#include "test_configcc.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
extern "C" {
#include <cmocka.h>
}
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern "C" {
    int main(int argc, char **argv) {
        const struct CMUnitTest tests[] = {
            // str utility
            cmocka_unit_test(test_unescape),
            // argparse
            cmocka_unit_test(test_argcc),
            cmocka_unit_test(test_argcc_failure),
            // configparse
            cmocka_unit_test(test_object),
            cmocka_unit_test(test_configcc_scanner_isAlphaNumeric),
            cmocka_unit_test(test_configcc_scanner),
            cmocka_unit_test(test_configcc_scanner_failure),
            cmocka_unit_test(test_configcc),
            cmocka_unit_test(test_configcc_failure)
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
    }
}
