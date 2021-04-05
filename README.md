# ARGCC - A C++ header-only command line parser

This library is a very basic argument parser.

Why?

I found myself writing argument parser over and over again.
They always ended up looking about the same and working well enough.
From now on for all my projects I will be using this unified helper library instead
of coming up with a new implementation for every project.

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

Unit tests can be run with `make test`.
To contribute make sure to write tests and keep the actual librarie's code in the header file.

## Usage

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

