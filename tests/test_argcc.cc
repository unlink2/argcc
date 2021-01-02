#include "../src/argcc.h"
#include "test_argcc.h"
#include <any>

void test_argcc(void **state) {
    std::stringstream testOut;
    argcc::Argparse parser("Unit test", testOut);

    parser.addArgument("string", argcc::ARGPARSE_STRING, 1, "String help", "-s");
    parser.addArgument("int", argcc::ARGPARSE_INT, 1, "int help", "-i");
    parser.addArgument("bool", argcc::ARGPARSE_BOOL, 1, "bool help", "-b");
    parser.addArgument("float", argcc::ARGPARSE_FLOAT, 1, "float help", "-f");

    parser.addArgument("string_lst", argcc::ARGPARSE_STRING, 2, "String list help", "-sl");
    parser.addArgument("int_lst", argcc::ARGPARSE_INT, 2, "int list help", "-il");
    parser.addArgument("bool_lst", argcc::ARGPARSE_BOOL, 2, "bool list help", "-bl");
    parser.addArgument("float_lst", argcc::ARGPARSE_FLOAT, 2, "float list help", "-fl");

    parser.addArgument("ignore_me", argcc::ARGPARSE_IGNORE, 0, "ignored", "-ig");

    parser.addArgument("flag", argcc::ARGPARSE_BOOL, 0, "flag", "-flag");
    parser.addArgument("unique", argcc::ARGPARSE_STRING, 1, "unqiue help", "-un", true);

    parser.addConsumer("consumer", argcc::ARGPARSE_STRING, "consumer...");
    {
        int argc = 27;
        const char *argv[] = {
            "test",
            "string", "Test",
            "int", "123",
            "bool", "true",
            "float", "3.1415",
            "-s", "Short Test",
            "-i", "456",
            "-f", "6.1415",
            "-b", "false",
            "string_lst", "Hello", "World",
            "-flag",
            "ignore_me",
            "unique", "test unique",
            "consume", "the", "rest",
        };
        argcc::Args a = parser.parse(argc, (char**)argv);

        assert_cc_string_equal(parser.getProgName(), std::string("test"));

        // assert arg content
        auto boolSet = a.getSize("bool");
        assert_int_equal(boolSet, 2);
        assert_true(a.toBool("bool"));
        assert_false(a.toBool("bool", 1));

        auto flagSet = a.getSize("flag");
        assert_int_equal(flagSet, 1);
        assert_true(a.toBool("flag"));

        auto intSet = a.getSize("int");
        assert_int_equal(intSet, 2);
        assert_int_equal(a.toInt("int"), 123);
        assert_int_equal(a.toInt("int", 1), 456);
        assert_int_equal(a.toInt("int", 2, 64), 64);

        auto floatSet = a.getSize("float");
        assert_int_equal(floatSet, 2);
        assert_float_equal(a.toFloat("float"), 3.1415, 0.001);
        assert_float_equal(a.toFloat("float", 1), 6.1415, 0.001);

        auto stringSet = a.getSize("string");
        assert_int_equal(stringSet, 2);
        assert_cc_string_equal(a.toString("string"), std::string("Test"));
        assert_cc_string_equal(a.toString("string", 1), std::string("Short Test"));

        auto stringListSet = a.getSize("string_lst");
        assert_int_equal(stringListSet, 2);
        assert_cc_string_equal(a.toString("string_lst"), std::string("Hello"));
        assert_cc_string_equal(a.toString("string_lst", 1), std::string("World"));

        auto uniqueSet = a.getSize("unique");
        assert_int_equal(uniqueSet, 1);
        assert_cc_string_equal(a.toString("unique"), std::string("test unique"));

        auto consumerSet = a.getSize("consumer");
        assert_int_equal(consumerSet, 3);
        assert_cc_string_equal(a.toString("consumer"), std::string("consume"));
        assert_cc_string_equal(a.toString("consumer", 1), std::string("the"));
        assert_cc_string_equal(a.toString("consumer", 2), std::string("rest"));
    }

    // test help command
    {
        int argc = 2;
        const char *argv[] = {
            "test",
            "--help"
        };
        argcc::Args a = parser.parse(argc, (char**)argv);
        assert_cc_string_equal(testOut.str(), parser.getHelpText());

    }
}

void test_argcc_failure(void **state) {
    argcc::Argparse parser("Unit test");

    parser.addArgument("string", argcc::ARGPARSE_STRING, 1, "String help", "-s");
    parser.addArgument("int", argcc::ARGPARSE_INT, 1, "int help", "-i");
    parser.addArgument("bool", argcc::ARGPARSE_BOOL, 1, "bool help", "-b");
    parser.addArgument("float", argcc::ARGPARSE_FLOAT, 1, "float help", "-f");

    parser.addArgument("string_lst", argcc::ARGPARSE_STRING, 2, "String list help", "-sl");
    parser.addArgument("int_lst", argcc::ARGPARSE_INT, 2, "int list help", "-il");
    parser.addArgument("bool_lst", argcc::ARGPARSE_BOOL, 2, "bool list help", "-bl");
    parser.addArgument("float_lst", argcc::ARGPARSE_FLOAT, 2, "float list help", "-fl");

    parser.addConsumer("consumer", argcc::ARGPARSE_STRING, "consumer help");
    parser.addArgument("unique", argcc::ARGPARSE_STRING, 1, "unqiue help", "-un", true);
    // bool failure
    {
        int argc = 3;
        const char *argv[] = {
            "test",
            "bool", "something_else",
        };

        assert_throws(argcc::ArgparseTypeException, {
            argcc::Args a = parser.parse(argc, (char**)argv);
        });
    }
    // int failure
    {
        int argc = 3;
        const char *argv[] = {
            "test",
            "int", "F123",
        };

        assert_throws(argcc::ArgparseTypeException, {
            argcc::Args a = parser.parse(argc, (char**)argv);
        });
    }

    // float failure
    {
        int argc = 3;
        const char *argv[] = {
            "test",
            "float", "f123.3",
        };

        assert_throws(argcc::ArgparseTypeException, {
            argcc::Args a = parser.parse(argc, (char**)argv);
        });
    }

    // list failure
    {
        int argc = 3;
        const char *argv[] = {
            "test",
            "string_lst", "f123.3",
        };

        assert_throws(argcc::ArgparseInsufficientArguments, {
            argcc::Args a = parser.parse(argc, (char**)argv);
        });
    }
    // consumer failure
    {
        int argc = 4;
        const char *argv[] = {
            "test",
            "consume_me", "string", "test",
        };

        assert_throws(argcc::ArgparseInvalidArgument, {
            argcc::Args a = parser.parse(argc, (char**)argv);
        });
    }

    // bad typecast
    {
        int argc = 3;
        const char *argv[] = {
            "test",
            "int", "1234"
        };
        argcc::Args a = parser.parse(argc, (char**)argv);
        assert_throws(argcc::ArgparseTypeException, {
                auto i = a.toString("int");
        });
    }

    // bad unique
    {
        int argc = 5;
        const char *argv[] = {
            "test",
            "unique", "Test string",
            "unique", "Second string "
        };
        assert_throws(argcc::ArgparseInvalidArgument, {
            argcc::Args a = parser.parse(argc, (char**)argv);
        });
    }
}
