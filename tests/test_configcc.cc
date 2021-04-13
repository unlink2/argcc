#include "configcc.h"
#include "test_configcc.h"
#include <any>

void test_unescape(void **state) {
    std::string unescaped = liblc::unescape("Hello \\\"World\\\"\\nTHis.\\tIs\\nAn\\vEscaped\\rString!\\\\");
    assert_cc_string_equal(unescaped, std::string("Hello \"World\"\nTHis.\tIs\nAn\vEscaped\rString!\\"));
}

void test_object(void **state) {
    liblc::ConfigObject num(liblc::NUMBER, int(1234));

    assert_true(num.isNumber());
    assert_false(num.isNil());
    assert_false(num.isReal());
    assert_false(num.isString());
    assert_false(num.isBool());

    assert_int_equal(num.castTo<int>(), 1234);

    // typecasts

    assert_int_equal(num.toNumber(), 1234);
    assert_float_equal(num.toReal(), 1234.0, 0.001);

    liblc::ConfigObject str(liblc::STRING, std::string("Test"));
    assert_cc_string_equal(str.toString(), std::string("Test"));

    // scalar
    assert_false(str.isScalar());
    assert_true(num.isScalar());

    // float
    liblc::ConfigObject f(liblc::REAL, float(3.1415));
    assert_true(f.isScalar());

    // get section, we just test the parser for easy data setup
    {
        liblc::ConfigParser parser("{a=1 b=2 c=3}");
        auto root = parser.parse();

        auto a = root->get("a");
        assert_int_equal(a->toNumber(), 1);
        assert_throws(liblc::ConfigccOutOfBounds, {root->get("d");});
    }
    {
        liblc::ConfigParser parser("[1 2 3]");
        auto root = parser.parse();

        assert_int_equal(root->get(0)->toNumber(), 1);
        assert_int_equal(root->get(1)->toNumber(), 2);
        assert_int_equal(root->get(2)->toNumber(), 3);

        assert_throws(liblc::ConfigccOutOfBounds, {root->get(3);});
    }
}

void test_configcc_scanner_isAlphaNumeric(void **state) {
    liblc::ConfigScanner scanner("");
    for (char c = 'a'; c <= 'z'; c++) {
        assert_true(scanner.isAlpha(c));
        assert_false(scanner.isDigit(c));
        assert_true(scanner.isAlphaNumeric(c));
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        assert_true(scanner.isAlpha(c));
        assert_false(scanner.isDigit(c));
        assert_true(scanner.isAlphaNumeric(c));
    }
    for (char c = '0'; c <= '9'; c++) {
        assert_false(scanner.isAlpha(c));
        assert_true(scanner.isDigit(c));
        assert_true(scanner.isAlphaNumeric(c));
    }

    // hex
    for (char c = '0'; c <= '9'; c++) {
        assert_true(scanner.isHexDigit(c));
    }
    for (char c = 'a'; c <= 'f'; c++) {
        assert_true(scanner.isHexDigit(c));
    }
    for (char c = 'A'; c <= 'F'; c++) {
        assert_true(scanner.isHexDigit(c));
    }
    for (char c = 'G'; c <= 'Z'; c++) {
        assert_false(scanner.isHexDigit(c));
    }
    for (char c = 'g'; c <= 'z'; c++) {
        assert_false(scanner.isHexDigit(c));
    }

    // bin
    assert_true(scanner.isBinDigit('1'));
    assert_true(scanner.isBinDigit('0'));
    assert_false(scanner.isBinDigit('2'));
}

void test_configcc_scanner(void **state) {
    // scan some tokens
    liblc::ConfigScanner scanner("{}[],\"Hello\"'World'\n\r\t0x123 0b101 200 3.1415 true false nil =");
    auto scanned = scanner.scanTokens();

    auto it = scanned.begin();
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 1);
        assert_cc_string_equal(t->getLexeme(), std::string("{"));
        assert_int_equal(t->getType(), liblc::LEFT_BRACE);
        assert_int_equal(t->getLiteral().getType(), liblc::NIL);
        assert_null(t->getLiteral().castTo<std::nullptr_t>());
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 1);
        assert_cc_string_equal(t->getLexeme(), std::string("}"));
        assert_int_equal(t->getType(), liblc::RIGHT_BRACE);
        assert_int_equal(t->getLiteral().getType(), liblc::NIL);
        assert_null(t->getLiteral().castTo<std::nullptr_t>());
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 1);
        assert_cc_string_equal(t->getLexeme(), std::string("["));
        assert_int_equal(t->getType(), liblc::LEFT_BRACKET);
        assert_int_equal(t->getLiteral().getType(), liblc::NIL);
        assert_null(t->getLiteral().castTo<std::nullptr_t>());
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 1);
        assert_cc_string_equal(t->getLexeme(), std::string("]"));
        assert_int_equal(t->getType(), liblc::RIGHT_BRACKET);
        assert_int_equal(t->getLiteral().getType(), liblc::NIL);
        assert_null(t->getLiteral().castTo<std::nullptr_t>());
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 1);
        assert_cc_string_equal(t->getLexeme(), std::string(","));
        assert_int_equal(t->getType(), liblc::COMMA);
        assert_int_equal(t->getLiteral().getType(), liblc::NIL);
        assert_null(t->getLiteral().castTo<std::nullptr_t>());
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 1);
        assert_cc_string_equal(t->getLexeme(), std::string("\"Hello\""));
        assert_int_equal(t->getType(), liblc::STRING_TOKEN);
        assert_int_equal(t->getLiteral().getType(), liblc::STRING);
        assert_cc_string_equal(t->getLiteral().toString(), std::string("Hello"));
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 1);
        assert_cc_string_equal(t->getLexeme(), std::string("'World'"));
        assert_int_equal(t->getType(), liblc::STRING_TOKEN);
        assert_int_equal(t->getLiteral().getType(), liblc::STRING);
        assert_cc_string_equal(t->getLiteral().toString(), std::string("World"));
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 2);
        assert_cc_string_equal(t->getLexeme(), std::string("0x123"));
        assert_int_equal(t->getType(), liblc::NUMBER_TOKEN);
        assert_int_equal(t->getLiteral().getType(), liblc::NUMBER);
        assert_int_equal(t->getLiteral().toNumber(), 0x123);
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 2);
        assert_cc_string_equal(t->getLexeme(), std::string("0b101"));
        assert_int_equal(t->getType(), liblc::NUMBER_TOKEN);
        assert_int_equal(t->getLiteral().getType(), liblc::NUMBER);
        assert_int_equal(t->getLiteral().toNumber(), 0b101);
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 2);
        assert_cc_string_equal(t->getLexeme(), std::string("200"));
        assert_int_equal(t->getType(), liblc::NUMBER_TOKEN);
        assert_int_equal(t->getLiteral().getType(), liblc::NUMBER);
        assert_int_equal(t->getLiteral().toNumber(), 200);
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 2);
        assert_cc_string_equal(t->getLexeme(), std::string("3.1415"));
        assert_int_equal(t->getType(), liblc::REAL_TOKEN);
        assert_int_equal(t->getLiteral().getType(), liblc::REAL);
        assert_float_equal(t->getLiteral().toReal(), 3.1415, 0.001);
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 2);
        assert_cc_string_equal(t->getLexeme(), std::string("true"));
        assert_int_equal(t->getType(), liblc::TRUE);
        assert_int_equal(t->getLiteral().getType(), liblc::NIL);
        assert_null(t->getLiteral().castTo<std::nullptr_t>());
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 2);
        assert_cc_string_equal(t->getLexeme(), std::string("false"));
        assert_int_equal(t->getType(), liblc::FALSE);
        assert_int_equal(t->getLiteral().getType(), liblc::NIL);
        assert_null(t->getLiteral().castTo<std::nullptr_t>());
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 2);
        assert_cc_string_equal(t->getLexeme(), std::string("nil"));
        assert_int_equal(t->getType(), liblc::NIL_TOKEN);
        assert_int_equal(t->getLiteral().getType(), liblc::NIL);
        assert_null(t->getLiteral().castTo<std::nullptr_t>());
    }

    it++;
    {
        auto t = *it;
        assert_int_equal(t->getLine(), 2);
        assert_cc_string_equal(t->getLexeme(), std::string("="));
        assert_int_equal(t->getType(), liblc::EQUAL);
        assert_int_equal(t->getLiteral().getType(), liblc::NIL);
        assert_null(t->getLiteral().castTo<std::nullptr_t>());
    }
}

void test_configcc_scanner_failure(void **state) {
    {
        liblc::ConfigScanner scanner("\"String");
        assert_throws(liblc::ConfigccScannerError, {
                scanner.scanTokens();
            });
    }

    {
        liblc::ConfigScanner scanner("\'String");
        assert_throws(liblc::ConfigccScannerError, {
                scanner.scanTokens();
            });
    }

    {
        liblc::ConfigScanner scanner("/");
        assert_throws(liblc::ConfigccScannerError, {
                scanner.scanTokens();
            });
    }

    {
        liblc::ConfigScanner scanner("@");
        assert_throws(liblc::ConfigccScannerError, {
                scanner.scanTokens();
            });
    }
}

#define test_parser_full(input, expectedStringify) {\
    liblc::ConfigStringify stringify;\
    liblc::ConfigParser parser(input);\
    auto root = parser.parse();\
    assert_cc_string_equal(stringify.stringify(root), std::string(expectedStringify));\
}

void test_configcc(void **state) {
    // parse from string back to string
    test_parser_full("{x=10 pi=3.1415}", "{\"pi\"=3.1415 \"x\"=10}");

    test_parser_full("10", "10");

    test_parser_full("[10 20 30 40]", "[10 20 30 40]");

    test_parser_full("{\n"
            "hi='Hello'\n"
            "w=\"World\"\n"
            "array = [1 -2 +3.1 -4 -3.1415]\n"
            "section = {\n"
                    "a = 3.14\n"
                    "b = true\n"
                    "c = nil\n"
                    "d = false\n"
                    "\"key str\"=\"test\""
                "}\n"
            "}"
            , "{\"array\"=[1 -2 3.1 -4 -3.1415] \"hi\"=\"Hello\" "
            "\"section\"={\"a\"=3.14 \"b\"=true \"c\"=nil \"d\"=false \"key str\"=\"test\"} \"w\"=\"World\"}");
}

#define test_parser_error(input) {\
    liblc::ConfigParser parser(input);\
    assert_throws(liblc::ConfigccParserError, {parser.parse();});\
}


void test_configcc_failure(void **state) {
    test_parser_error("{a=1");
    test_parser_error("[a=1");
    test_parser_error("a=1]");
    test_parser_error("a=1}");
    test_parser_error("a=1");
    test_parser_error("{a=1, b=2}");
    test_parser_error("{a=null}");
    test_parser_error("1 2");
}
