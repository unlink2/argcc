#include "argcc.h"
#include "test_argcc.h"
#include <any>

void test_argcc(void **state) {
    std::stringstream testOut;
    liblc::Argparse parser("Unit test", testOut);

    parser.addArgument("string", liblc::STRING, 1, "String help", "-s");
    parser.addArgument("int", liblc::NUMBER, 1, "int help", "-i");
    parser.addArgument("bool", liblc::BOOLEAN, 1, "bool help", "-b");
    parser.addArgument("float", liblc::REAL, 1, "float help", "-f");

    parser.addArgument("string_lst", liblc::STRING, 2, "String list help", "-sl");
    parser.addArgument("int_lst", liblc::NUMBER, 2, "int list help", "-il");
    parser.addArgument("bool_lst", liblc::BOOLEAN, 2, "bool list help", "-bl");
    parser.addArgument("float_lst", liblc::REAL, 2, "float list help", "-fl");

    parser.addArgument("ignore_me", liblc::IGNORE, 0, "ignored", "-ig");

    parser.addArgument("flag", liblc::BOOLEAN, 0, "flag", "-flag");
    parser.addArgument("unique", liblc::STRING, 1, "unqiue help", "-un", true);
    parser.addArgument("required", liblc::STRING, 1, "required help", "-req", true);

    parser.addConsumer("consumer", liblc::STRING, "consumer...");
    {
        int argc = 29;
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
            "required", "test required",
            "consume", "the", "rest"
        };
        liblc::Args a = parser.parse(argc, (char**)argv);

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
        assert_int_equal(a.toNumber("int"), 123);
        assert_int_equal(a.toNumber("int", 1), 456);
        assert_int_equal(a.toNumber("int", 2, 64), 64);

        auto floatSet = a.getSize("float");
        assert_int_equal(floatSet, 2);
        assert_float_equal(a.toReal("float"), 3.1415, 0.001);
        assert_float_equal(a.toReal("float", 1), 6.1415, 0.001);

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

        auto reqSet = a.getSize("required");
        assert_int_equal(reqSet, 1);
        assert_cc_string_equal(a.toString("required"), std::string("test required"));

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
        liblc::Args a = parser.parse(argc, (char**)argv);
        assert_cc_string_equal(testOut.str(), parser.getHelpText());

    }
}

void test_argcc_failure(void **state) {
    liblc::Argparse parser("Unit test");

    parser.addArgument("string", liblc::STRING, 1, "String help", "-s");
    parser.addArgument("int", liblc::NUMBER, 1, "int help", "-i");
    parser.addArgument("bool", liblc::BOOLEAN, 1, "bool help", "-b");
    parser.addArgument("float", liblc::REAL, 1, "float help", "-f");

    parser.addArgument("string_lst", liblc::STRING, 2, "String list help", "-sl");
    parser.addArgument("int_lst", liblc::NUMBER, 2, "int list help", "-il");
    parser.addArgument("bool_lst", liblc::BOOLEAN, 2, "bool list help", "-bl");
    parser.addArgument("float_lst", liblc::REAL, 2, "float list help", "-fl");

    parser.addConsumer("consumer", liblc::STRING, "consumer help");
    parser.addArgument("unique", liblc::STRING, 1, "unqiue help", "-un", true);
    // bool failure
    {
        int argc = 3;
        const char *argv[] = {
            "test",
            "bool", "something_else",
        };

        assert_throws(liblc::ArgparseTypeException, {
            liblc::Args a = parser.parse(argc, (char**)argv);
        });
    }
    // int failure
    {
        int argc = 3;
        const char *argv[] = {
            "test",
            "int", "F123",
        };

        assert_throws(liblc::ArgparseTypeException, {
            liblc::Args a = parser.parse(argc, (char**)argv);
        });
    }

    // float failure
    {
        int argc = 3;
        const char *argv[] = {
            "test",
            "float", "f123.3",
        };

        assert_throws(liblc::ArgparseTypeException, {
            liblc::Args a = parser.parse(argc, (char**)argv);
        });
    }

    // list failure
    {
        int argc = 3;
        const char *argv[] = {
            "test",
            "string_lst", "f123.3",
        };

        assert_throws(liblc::ArgparseInsufficientArguments, {
            liblc::Args a = parser.parse(argc, (char**)argv);
        });
    }
    // consumer failure
    {
        int argc = 4;
        const char *argv[] = {
            "test",
            "consume_me", "string", "test",
        };

        assert_throws(liblc::ArgparseInvalidArgument, {
            liblc::Args a = parser.parse(argc, (char**)argv);
        });
    }

    // bad typecast
    {
        int argc = 3;
        const char *argv[] = {
            "test",
            "int", "1234"
        };
        liblc::Args a = parser.parse(argc, (char**)argv);
        assert_throws(liblc::ArgparseTypeException, {
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
        assert_throws(liblc::ArgparseInvalidArgument, {
            liblc::Args a = parser.parse(argc, (char**)argv);
        });
    }

    // required test
    liblc::Argparse parserRequired("Unit test");

    parser.addArgument("string", liblc::STRING, 1, "String help", "-s", false, true);
    {
        int argc = 1;
        const char *argv[] = {
            "test"
        };
        assert_throws(liblc::ArgparseMissingArgument, {
            liblc::Args a = parser.parse(argc, (char**)argv);
        });
    }
}
