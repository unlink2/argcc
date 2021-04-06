AC_INIT([argcc], [0.1], [lukas@krickl.dev])
AC_ARG_WITH(tests, [AS_HELP_STRING([--with-tests], [build makefile for tests])])
name="argcc"

installdir="/usr/local/bin"
AC_SUBST(installdir, ["$installdir"])

main="main"
testMain="test"

srcdir="./src"
AC_SUBST(srcdir, "$srcdir")

testsdir="./tests"
frontdir="./frontend"

ext=".cc"
AC_SUBST(ext, "$ext")

define(GET_OBJS,
[
 objs=$(ls $2/*$ext | xargs -I{} -n 1 basename {} $ext | sed "s/$3*$//" | tr '\n' ' ')
 AC_SUBST($1, "$objs")
])

# check programs
AC_CANONICAL_TARGET
AC_LANG(C++)
${CXXFLAGS=""}
AC_PROG_CXX

# get header only internal libraries

define(GET_LIBS,
[
    ./getlibs.sh
])

GET_LIBS([Updating external libraries])

if test $with_tests
then
    AC_SUBST(main, ["$testMain"])
    AC_SUBST(frontdir, "$testsdir")
    AC_SUBST(name, ["test"])

    # get obj files
    GET_OBJS([srcObj], [$srcdir], [])
    GET_OBJS([frontendObj], [$testsdir], $testMain)

    # test for libraries
    AC_CHECK_HEADERS(cmocka.h, [], [AC_MSG_ERROR([libcmocka is required])],
        [
         [#include <setjmp.h>],
         AC_INCLUDES_DEFAULT
        ])
    # search for a specific function
    # AC_SEARCH_LIBS(cmocka_unit_test, cmocka, [], [AC_MSG_ERROR([libcmocka is required])])

    # set libs
    LIBS='-lcmocka'
else
    AC_SUBST(main, ["$main"])
    AC_SUBST(frontdir, ["$frontdir"])
    AC_SUBST(name, ["$name"])

    # get obj files
    GET_OBJS([srcObj], [$srcdir], [])
    GET_OBJS([frontendObj], [$frontdir], $main)
fi

AC_CONFIG_FILES([makefile])
AC_OUTPUT
