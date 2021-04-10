/*
Copyright 2021 Lukas Krickl (lukas@krickl.dev)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"),
to deal in the Software without restriction,
including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS",
WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __CONFIGCC_H__
#define __CONFIGCC_H__

#include <iostream>
#include <map>
#include <exception>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <any>

#include "lstr.h"

namespace configcc {
    enum ObjectType {
        NIL,
        BOOLEAN,
        NUMBER,
        REAL,
        STRING,
        LIST,
        SECTION
    };

    enum TokenType {
        EQUAL,
        SECTION_NAME,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        COMMA,
        QUOTE,
        OBJECT,
        LEFT_BRACE,
        RIGHT_BRACE,
        EOF_T,
        TRUE,
        FALSE,
        NIL_TOKEN,
        NUMBER_TOKEN,
        REAL_TOKEN,
        STRING_TOKEN
    };

    enum ErrorType {
        NO_ERROR,
        UNEXPECTED_TOKEN,
        NUMBER_PARSE_ERROR,
        UNTERMINATED_STRING,
        ROOT_SHOULD_BE_MAP_OR_LIST,
        MISSING_RIGHT_BRACKET,
        EXPECTED_EQUAL,
        EXPECTED_COMMA,
        EXPECTED_SECTION_NAME,
        MISSING_RIGHT_BRACE,
        EXPECTED_EOF,
        OBJECT_CANNOT_BE_INDEXED
    };


    class Token;

    class ConfigparseCommonException: public std::exception {
        public:
            ConfigparseCommonException(std::shared_ptr<Token> token, ErrorType error):
                token(token), error(error) {}
            ~ConfigparseCommonException() {}
            virtual const char* what() const throw() {
                switch (error) {
                    case NO_ERROR:
                        return "No error";
                    case UNEXPECTED_TOKEN:
                        return "Unexpected token";
                    case NUMBER_PARSE_ERROR:
                        return "Number parse error";
                    case UNTERMINATED_STRING:
                        return "Unterminated string";
                    case ROOT_SHOULD_BE_MAP_OR_LIST:
                        return "Root should be map or list";
                    case MISSING_RIGHT_BRACKET:
                        return "Expected }";
                    case EXPECTED_SECTION_NAME:
                        return "Expected section name";
                    case EXPECTED_COMMA:
                        return "Expected ,";
                    case EXPECTED_EQUAL:
                        return "Expected =";
                    case MISSING_RIGHT_BRACE:
                        return "Expected ]";
                    case EXPECTED_EOF:
                        return "Expected End of File";
                    case OBJECT_CANNOT_BE_INDEXED:
                        return "Object cannot be indexed";
                }
                return "";
            }

            std::shared_ptr<Token> token;
            ErrorType error;
    };

    class ConfigccScannerError: public ConfigparseCommonException {
        public:
            ConfigccScannerError(std::shared_ptr<Token> token, ErrorType error):
                ConfigparseCommonException::ConfigparseCommonException(token, error) {}
    };

    class ConfigccParserError: public ConfigparseCommonException {
        public:
            ConfigccParserError(std::shared_ptr<Token> token, ErrorType error):
                ConfigparseCommonException::ConfigparseCommonException(token, error) {}
    };

    class ConfigObject;

    class ConfigObjectVisitor {
        public:
            ConfigObjectVisitor() {}
            ~ConfigObjectVisitor() {}

            virtual std::any visitBoolean(ConfigObject *obj) {
                return std::any();
            }

            virtual std::any visitNumber(ConfigObject *obj) {
                return std::any();
            }

            virtual std::any visitReal(ConfigObject *obj) {
                return std::any();
            }

            virtual std::any visitString(ConfigObject *obj) {
                return std::any();
            }

            virtual std::any visitList(ConfigObject *obj) {
                return std::any();
            }

            virtual std::any visitSection(ConfigObject *obj) {
                return std::any();
            }

            virtual std::any visitNil(ConfigObject *obj) {
                return std::any();
            }
    };

    /**
     * Generic object using std::any
     */
    class ConfigObject {
        public:
            ConfigObject(ObjectType type, std::any value):
                type(type), value(value) {}

            // copy constructor
            ConfigObject(ConfigObject *original) {
                type = original->type;
                value = original->value;
            }

            template<typename T>
            T castTo() {
                return std::any_cast<T>(value);
            }

            float toReal() {
                if (isNumber()) {
                    return castTo<int>();
                }
                return castTo<float>();
            }

            int toNumber() {
                if (isReal()) {
                    return castTo<float>();
                }
                return castTo<int>();
            }

            std::string& toString() {
                return castTo<std::string&>();
            }

            bool toBool()  {
                return castTo<bool>();
            }

            std::nullptr_t toNil() {
                return castTo<std::nullptr_t>();
            }

            std::shared_ptr<std::vector<std::shared_ptr<ConfigObject>>> toList() {
                return castTo<std::shared_ptr<std::vector<std::shared_ptr<ConfigObject>>>>();
            }

            std::shared_ptr<std::map<std::string, std::shared_ptr<ConfigObject>>> toSection() {
                return castTo<std::shared_ptr<std::map<std::string, std::shared_ptr<ConfigObject>>>>();
            }

            ObjectType getType() {
                return type;
            }

            bool isNumber() {
                return type == NUMBER;
            }

            bool isReal() {
                return type == REAL;
            }

            bool isBool() {
                return type == BOOLEAN;
            }

            bool isNil() {
                return type == NIL;
            }

            bool isString() {
                return type == STRING;
            }

            bool isList() {
                return type == LIST;
            }

            bool isScalar() {
                return type == NUMBER || type == REAL;
            }

            bool isSection() {
                return type == SECTION;
            }


            std::any accept(ConfigObjectVisitor *visitor) {
                std::any result;
                switch (getType()) {
                    case BOOLEAN:
                        result = visitor->visitBoolean(this);
                        break;
                    case NUMBER:
                        result = visitor->visitNumber(this);
                        break;
                    case REAL:
                        result = visitor->visitReal(this);
                        break;
                    case STRING:
                        result = visitor->visitString(this);
                        break;
                    case LIST:
                        result = visitor->visitList(this);
                        break;
                    case SECTION:
                        result = visitor->visitSection(this);
                        break;
                    case NIL:
                        result = visitor->visitNil(this);
                        break;
                }

                return result;
            }
        private:
            ObjectType type;
            std::any value;
    };

    class Token {
        public:
            Token(TokenType type, std::string lexeme, ConfigObject literal, unsigned int line,
                    std::string path, unsigned int tokenStart, std::shared_ptr<std::string> source):
                lexeme(lexeme), line(line), type(type), path(path), literal(literal), tokenStart(tokenStart),
                source(source) {}

            const std::string getLexeme() {
                return lexeme;
            }

            const unsigned int getLine() {
                return line;
            }

            const std::string getPath() {
                return path;
            }

            ConfigObject getLiteral() {
                return literal;
            }

            const unsigned int getTokenStart() {
                return tokenStart;
            }

            const std::shared_ptr<std::string> getSource() {
                return source;
            }

            const TokenType getType() {
                return type;
            }

        private:
            const std::string lexeme;
            const unsigned int line;
            const TokenType type;
            const std::string path;
            ConfigObject literal;
            const unsigned int tokenStart;
            const std::shared_ptr<std::string> source;
    };

    class ConfigScanner {
        public:
            ConfigScanner(std::string source, std::string path=""):
                source(std::make_shared<std::string>(source)), path(path) { }

            std::vector<std::shared_ptr<Token>> scanTokens() {
                while (!isAtEnd()) {
                    start = current;
                    scanToken();
                }
                addToken(EOF_T);
                return tokens;
            }

            bool isDigit(char c) {
                return c >= '0' && c <= '9';
            }

            bool isHexDigit(char c) {
                return (c >= 'a' && c <= 'f') || (c >= 'A' && c<= 'F') || isDigit(c);
            }

            bool isBinDigit(char c) {
                return c == '0' || c == '1';
            }

            bool isAlpha(char c) {
                return (c >= 'a' && c <= 'z') ||
                    (c >= 'A' && c <= 'Z') ||
                    c == '_' || c == ':';
            }

            bool isAlphaNumeric(char c) {
                return isAlpha(c) || isDigit(c);
            }

        private:
            void scanToken() {
                char c = advance();
                switch (c) {
                    case '{':
                        addToken(LEFT_BRACE);
                        break;
                    case '}':
                        addToken(RIGHT_BRACE);
                        break;
                    case '[':
                        addToken(LEFT_BRACKET);
                        break;
                    case ']':
                        addToken(RIGHT_BRACKET);
                        break;
                    case ',':
                        addToken(COMMA);
                        break;
                    case '=':
                        addToken(EQUAL);
                        break;
                    case '/':
                        // ignore comments
                        if (match('/')) {
                            // until end of line
                            while (peek() != '\n' && !isAtEnd()) {
                                advance();
                            }
                        } else {
                            addToken(EOF_T);
                            throw ConfigccScannerError(tokens[tokens.size()-1], UNEXPECTED_TOKEN);
                        }
                    // ignore space \t and \r
                    case ' ':
                    case '\t':
                    case '\r':
                        break;
                    case '\n':
                        line++;
                        break;
                    case '\'':
                    case '"':
                        // string
                        scanString(c);
                        break;
                    default:
                        // otherwise either speical word or error
                        if (isDigit(c)) {
                            scanNumber(c);
                        } else if (isAlpha(c)) {
                            scanIdentifier();
                        } else {
                            addToken(EOF_T);
                            throw ConfigccScannerError(tokens[tokens.size()-1], UNEXPECTED_TOKEN);
                        }
                }
            }

            char advance() {
                current++;
                return source->at(current-1);
            }

            bool isAtEnd() {
                return current >= source->size();
            }

            char peek() {
                if (isAtEnd()) {
                    return '\0';
                }
                return source->at(current);
            }

            char peekNext() {
                if (current+1 >= source->size()) {
                    return '\0';
                }
                return source->at(current+1);
            }

            void addToken(TokenType type) {
                addToken(type, ConfigObject(NIL, nullptr));
            }

            void addToken(TokenType type, ConfigObject literal) {
                std::string text = source->substr(start, current-start);
                tokens.push_back(std::make_shared<Token>(Token(type, text, literal, line, path, start, source)));
            }

            bool match(char expected) {
                if (isAtEnd()
                        || source->at(current) != expected) {
                    return false;
                }
                current++;
                return true;
            }

            void scanNumber(char c) {
                bool isFloat = false;
                bool isHex = c == '0' && peek() == 'x';
                bool isBin = c == '0' && peek() == 'b';
                TokenType type = NUMBER_TOKEN;
                ObjectType objType = NUMBER;

                if (isHex) {
                    advance();
                    while (isHexDigit(peek())) {
                        advance();
                    }
                } else if (isBin) {
                    advance();
                    while (isBinDigit(peek())) {
                        advance();
                    }
                } else {
                    // decimal
                    while (isDigit(peek())) {
                        advance();
                    }

                    // is float?
                    if (peek() == '.' && isDigit(peekNext())) {
                        advance();
                        isFloat = true;
                        type = REAL_TOKEN;
                        objType = REAL;
                        while (isDigit(peek())) {
                            advance();
                        }
                    }
                }

                try {
                    std::any value;
                    if (isFloat) {
                        auto number = source->substr(start, current-start);
                        value = std::any(stringToReal(number));
                    } else if (isBin) {
                        auto number = source->substr(start+2, current-start);
                        value = std::any(stringToNumber(number, 2));
                    } else if (isHex) {
                        auto number = source->substr(start, current-start);
                        value = std::any(stringToNumber(number, 16));
                    } else {
                        auto number = source->substr(start, current-start);
                        value = std::any(stringToNumber(number));
                    }
                    addToken(type, ConfigObject(objType, value));
                } catch (...) {
                    addToken(EOF_T);
                    throw ConfigccScannerError(tokens[tokens.size()-1], NUMBER_PARSE_ERROR);
                }
            }

            void scanIdentifier() {
                while (isAlphaNumeric(peek())) {
                    advance();
                }

                std::string text = source->substr(start, current-start);

                TokenType type = SECTION_NAME;
                // speicial identifiers
                if (text == "true") {
                    type = TRUE;
                } else if (text == "false") {
                    type = FALSE;
                } else if (text == "nil") {
                    type = NIL_TOKEN;
                }

                addToken(type);
            }

            void scanString(char quote) {
                while (peek() != quote && !isAtEnd()) {
                    if (peek() == '\n') {
                        line++;
                    }

                    // escape character
                    if (peek() == '\\') {
                        advance();
                    }
                    advance();
                }

                if (isAtEnd()) {
                    addToken(EOF_T);
                    throw ConfigccScannerError(tokens[tokens.size()-1], UNTERMINATED_STRING);
                }

                // closing "
                advance();

                std::string value = lstr::unescape(source->substr(start+1, current-start-2));
                addToken(STRING_TOKEN, ConfigObject(STRING, value));
            }

            float stringToReal(const std::string& number) {
                return std::stod(number);
            }

            int stringToNumber(const std::string& number, int base=10) {
                return std::stol(number, nullptr, base);
            }

            const std::shared_ptr<std::string> source;
            const std::string path;
            unsigned int line = 1;
            unsigned int start = 0;
            unsigned int current = 0;

            std::vector<std::shared_ptr<Token>> tokens;
    };

    // interface for generic stringify operation on config objects
    // this is the minimal implementation
    class ConfigStringify: public ConfigObjectVisitor {
        public:
            ConfigStringify() {}
            ~ConfigStringify() {}

            virtual std::string stringify(std::shared_ptr<ConfigObject> root) {
                std::stringstream stream;
                stream << std::any_cast<std::string>(root->accept(this));
                return stream.str();
            }
        private:
            virtual std::any visitNumber(ConfigObject *obj) {
                std::stringstream stream;
                stream << obj->toNumber();
                return stream.str();
            }

            virtual std::any visitReal(ConfigObject *obj) {
                std::stringstream stream;
                stream << obj->toReal();
                return stream.str();
            }

            virtual std::any visitBoolean(ConfigObject *obj) {
                if (obj->toBool()) {
                    return std::string("true");
                } else {
                    return std::string("false");
                }
            }

            virtual std::any visitString(ConfigObject *obj) {
                std::stringstream stream;
                stream << '"' << obj->toString() << '"';
                return stream.str();
            }

            virtual std::any visitNil(ConfigObject *obj) {
                return std::string("nil");
            }

            virtual std::any visitList(ConfigObject *obj) {
                std::stringstream stream;
                stream << '[';
                auto objList = obj->toList();
                for (auto it = objList->begin(); it != objList->end(); it++) {
                    if (it != objList->begin()) {
                        stream << ' ';
                    }
                    stream << stringify(*it);
                }
                stream << ']';
                return stream.str();
            }

            virtual std::any visitSection(ConfigObject *obj) {
                std::stringstream stream;
                stream << '{';
                auto objMap = obj->toSection();
                for (auto it = objMap->begin(); it != objMap->end(); it++) {
                    if (it != objMap->begin()) {
                        stream << ' ';
                    }
                    stream << it->first << '=';
                    stream << stringify(it->second);
                }
                stream << '}';
                return stream.str();
            }

    };

    class ConfigParser {
        public:
            ConfigParser(std::vector<std::shared_ptr<Token>> tokens):
                tokens(tokens) {}

            ConfigParser(std::string data) {
                ConfigScanner scanner(data);
                tokens = scanner.scanTokens();
            }

            std::shared_ptr<ConfigObject> parse() {
                // root-level object
                auto root = object();
                if (!isAtEnd()) {
                    throw handleError(EXPECTED_EOF);
                }

                return root;
            }

        private:
            std::shared_ptr<ConfigObject> object() {
                if (check(LEFT_BRACE)) {
                    return section();
                } else if (check(LEFT_BRACKET)) {
                    return list();
                } else if (match(std::vector<TokenType> { TRUE, FALSE })) {
                    return boolean();
                } else if (check(NIL_TOKEN)) {
                    return nil();
                }
                return literal();
            }

            std::shared_ptr<ConfigObject> section() {
                advance(); // {
                auto root = std::make_shared<ConfigObject>(ConfigObject(SECTION,
                            std::make_shared<std::map<std::string, std::shared_ptr<ConfigObject>>>(
                                std::map<std::string, std::shared_ptr<ConfigObject>>())));
                // name = value until end of section
                while (!check(RIGHT_BRACE) && !isAtEnd()) {
                    auto name = consume(SECTION_NAME, EXPECTED_SECTION_NAME);
                    consume(EQUAL, EXPECTED_EQUAL);
                    auto value = object();

                    addObjectToSection(value, name->getLexeme(), root);
                }

                consume(RIGHT_BRACE, MISSING_RIGHT_BRACE);

                return root;
            }

            std::shared_ptr<ConfigObject> list() {
                advance(); // [
                auto root = std::make_shared<ConfigObject>(ConfigObject(LIST,
                            std::make_shared<std::vector<std::shared_ptr<ConfigObject>>>(
                                std::vector<std::shared_ptr<ConfigObject>>())));
                // [value value value... ]
                while (!check(RIGHT_BRACKET) && !isAtEnd()) {
                    auto value = object();
                    addObjectToList(value, root);
                }

                consume(RIGHT_BRACKET, MISSING_RIGHT_BRACKET);

                return root;
            }

            std::shared_ptr<ConfigObject> boolean() {
                return std::make_shared<ConfigObject>(ConfigObject(BOOLEAN, previous()->getType() == TRUE));
            }

            std::shared_ptr<ConfigObject> nil() {
                advance();
                return std::make_shared<ConfigObject>(ConfigObject(NIL, nullptr));
            }

            std::shared_ptr<ConfigObject> literal() {
                if (match(std::vector<TokenType> {NUMBER_TOKEN, REAL_TOKEN, STRING_TOKEN})) {
                    auto token = previous();
                    auto literal = token->getLiteral();
                    return std::make_shared<ConfigObject>(ConfigObject(&literal));
                }
                throw handleError(UNEXPECTED_TOKEN);
            }

            void addObjectToList(std::shared_ptr<ConfigObject> toAdd, std::shared_ptr<ConfigObject> list) {
                auto objVector = list->toList();
                objVector->push_back(toAdd);
            }

            void addObjectToSection(std::shared_ptr<ConfigObject> toAdd, std::string name,
                    std::shared_ptr<ConfigObject> list) {
                auto objMap = list->toSection();
                objMap->insert(std::pair<std::string, std::shared_ptr<ConfigObject>>(name, toAdd));
            }


            std::shared_ptr<Token> consume(TokenType token, ErrorType error, bool optional=false) {
                if (check(token)) {
                    return advance();
                }

                if (optional) {
                    return std::shared_ptr<Token>(nullptr);
                }

                throw handleError(error);
            }

            bool match(std::vector<TokenType> types) {
                for (auto it = types.begin(); it != types.end(); it++) {
                    if (check(*it)) {
                        advance();
                        return true;
                    }
                }
                return false;
            }

            bool check(TokenType type) {
                return !isAtEnd() && peek()->getType() == type;
            }

            bool isAtEnd() {
                return peek()->getType() == EOF_T;
            }

            std::shared_ptr<Token> advance() {
                if (!isAtEnd()) {
                    current++;
                }
                return previous();
            }

            std::shared_ptr<Token> peek() {
                return tokens.at(current);
            }

            std::shared_ptr<Token> previous() {
                return tokens.at(current-1);
            }

            ConfigccParserError handleError(ErrorType error, std::shared_ptr<Token> token=std::shared_ptr<Token>(nullptr)) {
                if (!token.get()) {
                    token = peek();
                }
                return ConfigccParserError(token, error);
            }

            unsigned long current = 0;
            std::vector<std::shared_ptr<Token>> tokens;
    };
}

#endif
