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

#ifndef __ARGCC_H__
#define __ARGCC_H__

#include <iostream>
#include <map>
#include <exception>
#include <vector>
#include <memory>
#include <sstream>
#include <iomanip>
#include <any>
#include "typedefs.h"

namespace liblc {
    // valid types for args
    typedef int ArgNumber;
    typedef float ArgReal;
    typedef bool ArgBool;
    typedef std::string ArgString;

    class ArgparseCommonException: public std::exception {
        public:
            ~ArgparseCommonException() {}
            virtual const char* what() const throw() = 0;
    };

    class ArgparseTypeException: public ArgparseCommonException {
        public:
            virtual const char* what() const throw() {
                return "Unexpected type";
            }
    };

    class ArgparseInsufficientArguments: public ArgparseCommonException {
        public:
            virtual const char* what() const throw() {
                return "Unexpected type";
            }
    };

    class ArgparseInvalidArgument: public ArgparseCommonException {
        public:
            ArgparseInvalidArgument(std::string name): text("Unknown argument " + name) {}
            virtual const char* what() const throw() {
                return text.c_str();
            }
        private:
            std::string text;
    };

    class ArgparseMissingArgument: public ArgparseCommonException {
        public:
            ArgparseMissingArgument(std::string name): text("Required argument is missing " + name) {}
            virtual const char* what() const throw() {
                return text.c_str();
            }
        private:
            std::string text;
    };

    class Args {
        public:
            Args() {
            }

            ~Args() {
            }

            /**
             * Adds input value to set.
             */
            void addString(std::string name, ArgString value) {
                addGeneric<ArgString>(name, value);
            }

            void addNumber(std::string name, ArgNumber value) {
                addGeneric<ArgNumber>(name, value);
            }

            void addReal(std::string name, ArgReal value) {
                addGeneric<ArgReal>(name, value);
            }

            void addBool(std::string name, ArgBool value) {
                addGeneric<ArgBool>(name, value);
            }

            int getSize(std::string name) {
                auto iter = values.find(name);
                if (iter != values.end()) {
                    return iter->second.size();
                }

                return 0;
            }

            std::vector<std::any>::iterator getValueBegin(std::string name) {
                return values.find(name)->second.begin();
            }

            std::vector<std::any>::iterator getValueEnd(std::string name) {
                return values.find(name)->second.end();
            }

            /**
             * Attempts to find and cast value at index in set
             */
            ArgString toString(std::string name, unsigned long index=0, ArgString defaultValue="") {
                return toGeneric<ArgString>(name, defaultValue, index);
            }

            ArgBool toBool(std::string name, unsigned long index=0, ArgBool defaultValue=false) {
                return toGeneric<ArgBool>(name, defaultValue, index);
            }

            ArgNumber toNumber(std::string name, unsigned long index=0, ArgNumber defaultValue=0) {
                return toGeneric<ArgNumber>(name, defaultValue, index);
            }

            ArgReal toReal(std::string name, unsigned long index=0, ArgReal defaultValue=0.0f) {
                return toGeneric<ArgReal>(name, defaultValue, index);
            }

            /**
             * Returns:
             *  true if any set contains a given name
             */
            bool containsAny(std::string name) {
                return this->values.find(name) != this->values.end();
            }

        private:
            template<typename T>
            void addGeneric(std::string name, T value) {
                auto iter = values.find(name);
                if (iter != values.end()) {
                    iter->second.push_back(value);
                } else {
                    // add new item
                    std::vector<std::any> newSet;
                    newSet.push_back(value);
                    values.insert(std::make_pair(name, newSet));
                }
            }

            template<typename T>
            T toGeneric(std::string name, T defaultValue, unsigned long index=0) {
                auto iter = values.find(name);
                if (iter == values.end()) {
                    throw ArgparseInvalidArgument(name);
                }
                try {
                    if (iter->second.size() <= index) {
                        return defaultValue;
                    }
                    return std::any_cast<T>(iter->second.at(index));
                } catch (std::bad_any_cast &e) {
                    throw ArgparseTypeException();
                }
            }

            std::map<std::string, std::vector<std::any>> values;
    };

    /**
     * Virtual base parser
     */
    class Parser {
        public:
            Parser(int nargs, std::string help, bool unique, bool required):
                nargs(nargs), help(help), unique(unique), required(required) { }

            virtual ~Parser() {};
            /**
             * Takes an input string, a name and an args object
             * parsers the string and adds it to args under the provided name
             * Throws:
             *  ArgparseTypeException if type parsing fails
             */
            virtual void parse(std::string input, std::string name, Args *args) {};

            int getNargs() {
                return nargs;
            }

            std::string getHelp() {
                return help;
            }

            bool isUnique() {
                return unique;
            }

            bool isRequired() {
                return required;
            }
        private:
            int nargs;
            std::string help;
            bool unique;
            bool required;
    };

    class NumberParser: public Parser {
        public:
            NumberParser(int nargs, std::string help, bool unique, bool required):
                Parser::Parser(nargs, help, unique, required) { }

            virtual void parse(std::string input, std::string name, Args *args) {
                try {
                    ArgNumber i = std::stoi(input);
                    args->addNumber(name, i);
                } catch (std::invalid_argument &e) {
                    throw ArgparseTypeException();
                }
            }

    };

    class BoolParser: public Parser {
        public:
            BoolParser(int nargs, std::string help, bool unique, bool required):
                Parser::Parser(nargs, help, unique, required) { }

            virtual void parse(std::string input, std::string name, Args *args) {
                if (input != "true" && input != "false") {
                    throw ArgparseTypeException();
                }

                args->addBool(name, input == "true");
            }

    };

    class RealParser: public Parser {
        public:
            RealParser(int nargs, std::string help, bool unique, bool required):
                Parser::Parser(nargs, help, unique, required) { }

            virtual void parse(std::string input, std::string name, Args *args) {
                try {
                    ArgReal f = std::stof(input);
                    args->addReal(name, f);
                } catch (std::invalid_argument &e) {
                    throw ArgparseTypeException();
                }
            }
    };

    class StringParser: public Parser {
        public:
            StringParser(int nargs, std::string help, bool unique, bool required):
                Parser::Parser(nargs, help, unique, required) { }

            virtual void parse(std::string input, std::string name, Args *args) {
                args->addString(name, input);
            }
    };

    class Argparse {
        public:
            Argparse(std::string description, std::ostream &out=std::cout):
                out(out), consumer(std::shared_ptr<Parser>(nullptr)) {
                this->description = description;
            }

            ~Argparse() {}

            void addArgument(std::string name, ArgparseType type,
                    int nargs=1, std::string help="", std::string shortName="", bool unique=false, bool required=false) {
                args[name] = makeParser(name, type, nargs, help, unique, required);

                if (shortName != "") {
                    shortNames[shortName] = name;
                }
            }

            void addConsumer(std::string name, ArgparseType type, std::string help, bool required=false) {
                consumer = makeParser(name, type, -1, help, true, required);
                consumerName = name;
            }

            Args parse(int argc, char **argv) {
                Args resultArgs;
                this->argc = argc;
                this->argv = argv;
                this->index = 0;
                bool consumedDefault = false;

                if (argc > 0) {
                    progName = next();
                }

                // iterate over all argvs and attempt to parse them
                while (!isAtEnd()) {
                    std::string name = next();
                    if (!parseArgument(name, &resultArgs)) {
                        if (consumer.get() == nullptr) {
                            throw ArgparseInvalidArgument(name);
                        } else {
                            consumer->parse(name, consumerName, &resultArgs);
                            consumedDefault = true;
                        }
                    } else if (consumedDefault) {
                        // error cannot consume default and then parse again!
                        throw ArgparseInvalidArgument(name);
                    }
                }

                // find any required arguments that did not receive an input
                // if so throw!
                ensureRequiredArgs(resultArgs);


                return resultArgs;
            }

            char* next() {
                return argv[index++];
            }

            bool isAtEnd() {
                return index >= (unsigned long)argc;
            }

            std::string getHelpText() {
                std::stringstream strstream;

                strstream << description << std::endl << std::endl;

                strstream << "Usage:" << std::endl << progName;
                if (consumer.get()) {
                    strstream << " [" << consumer->getHelp() << "...] ";
                }

                strstream << std::endl << std::endl;

                auto it = args.begin();
                while (it != args.end()) {
                    std::string shortName = "";
                    // search for shortname
                    // map has no search so we need to be slow... ugh
                    for (auto sit = shortNames.begin(); sit != shortNames.end(); ++sit) {
                        if (sit->second == it->first) {
                            shortName = sit->first;
                        }
                    }
                    strstream << std::setw(10) << std::left << it->first << " " << shortName << "\t\t" << it->second->getHelp() << std::endl;
                    it++;
                }
                strstream << std::endl;

                return strstream.str();
            }

            std::string getProgName() {
                return progName;
            }
        private:
            std::shared_ptr<Parser> makeParser(std::string name, ArgparseType type,
                    int nargs, std::string help, bool unique, bool required) {
                switch (type) {
                    case STRING:
                        return std::shared_ptr<Parser>(new StringParser(nargs, help, unique, required));
                    case REAL:
                        return std::shared_ptr<Parser>(new RealParser(nargs, help, unique, required));
                    case BOOLEAN:
                        return std::shared_ptr<Parser>(new BoolParser(nargs, help, unique, required));
                    case NUMBER:
                        return std::shared_ptr<Parser>(new NumberParser(nargs, help, unique, required));
                    case IGNORE:
                        return std::shared_ptr<Parser>(new Parser(nargs, help, unique, required));
                    default:
                        break;
                }
                // this should never be reached
                return std::shared_ptr<Parser>(nullptr);
            }

            bool parseArgument(std::string name, Args *resultArgs) {
                auto shortNameIt = shortNames.find(name);
                if (shortNameIt != shortNames.end()) {
                    name = shortNameIt->second;
                }

                if (name == "--help" || name == "-h") {
                    // special case
                    out << getHelpText();
                    return true;
                }


                auto it = args.find(name);
                if (it != args.end()) {
                    std::shared_ptr<Parser> parser = it->second;

                    // if is unique and result args already exist throw error
                    if (parser->isUnique() && resultArgs->containsAny(name)) {
                        throw ArgparseInvalidArgument(name);
                    }

                    // parse amount of args we want
                    if (parser->getNargs() == 0) {
                        // set boolean
                        resultArgs->addBool(name, true);
                    } else {
                        // parse until end of stream or nargs
                        for (int i = 0; i < parser->getNargs(); i++) {
                            if (isAtEnd()) {
                                throw ArgparseInsufficientArguments();
                            }
                            // parse and add
                            parser->parse(next(), name, resultArgs);
                        }
                    }
                    return true;
                }
                return false;
            }

            void ensureRequiredArgs(Args resultArgs) {
                for (auto it = args.begin(); it != args.end(); it++) {
                    if (it->second->isRequired() && !resultArgs.containsAny(it->first)) {
                        throw ArgparseMissingArgument(it->first);
                    }
                }
            }

            std::string progName;

            // those are set when parse is called
            char **argv;
            int argc;
            unsigned long index;
            std::ostream &out;

            std::string description;
            std::map<std::string, std::shared_ptr<Parser>> args;
            std::shared_ptr<Parser> consumer;
            std::string consumerName;
            std::map<std::string, std::string> shortNames;
    };
};

#endif
