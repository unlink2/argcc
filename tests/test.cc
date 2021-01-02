
#include "test_argcc.h"

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
            // argparse
            cmocka_unit_test(test_argcc),
            cmocka_unit_test(test_argcc_failure),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
    }
}
