# LibLC (Lib Lukas Common) - A C++ header-only command line parser and configuration file parser

This is a common shared library for command line argument
and configuration file parsing.

## Build

To use the library simplt include argcc.h in your project.
This library requires at least C++17.

## Contribute/Run tests

To compile run

```bash
autoconf -i
./configure
make
```

Unit tests can be built using:
```bash
autoconf -i
./configure --with-tests
make
make run
```

To contribute make sure to write tests and keep the actual librarie's code in the header file.

## Usage Command Line Parser

### Create parser

```c++
    argcc::Argparser parser("Program name");
```

### Add argument

```c++
    /**
      * Name, type, amount of inputs, helptext, alternative name, unique
      */
    parser.addArgument("-long-name", argcc::ARGPARSE_STRING, 1, "-short");
```

### Parse

```c++
    auto parsed = parser.parse(argc, argv);
```

### Access
```c++
    auto isPresent = parsed.containsAny("-long-name");

    /**
      * key, (optinal index, optional default value)
      */
    auto value = parser.toString("-long-name");

    // amount of items in argument's array
    auto amount = parser.getSize("-long-name");
```

## Code sample

The `frontend` folder contains a sample argument parser.

## Usage Configuratio File Parser

The configration files have a json-like format.

```
{
    string="Hello",
    string2='World',
    integer=1,
    real=3.1415,
    section = {
        a=1,
        b=2,
        c=3,
    },
    list=[1, 2, 3],
    undefined=nil
}
```

To parse an input string simply creata the parser object

```c++
#include "configcc.h"

int main(int argc, char **argv) {
    std::string input = readConfigFile();
    configcc::ConfigStringify stringify;
    configcc::ConfigParser parser(input);
    std::shared_ptr<configcc::ConfigObject> root = parser.parse();

    // turn back into string
    stringify.stringify(root);
}
```

The parsed file will return a hirachy of ConfigObjects

```c++
// check if the object is of a certain type and convert
root.isString();
root.toString();

root.isNumber();
root.toNumber();

root.isReal();
root.totReal();

root.isBool();
root.toBool();

root.isNil();
root.toNil();

// implemented using std::vector<std::shared_ptr<ConfigObject>>
root.isList();
root.toList();

// implemented using std::map<std::string, std::shared_ptr<ConfigObject>>
root.isSection();
root.toSection();
```
