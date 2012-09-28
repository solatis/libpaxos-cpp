#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([libpaxos-cpp], [0.1.0], http://solatis.github.com/libpaxos-cpp)
AC_CONFIG_SRCDIR([paxos++/server.hpp])
AC_CONFIG_HEADER([config.h])

LT_INIT([shared static])

# Makefile initialization.
AM_INIT_AUTOMAKE([-Wall -Werror])

# Checks for programs.
AC_ARG_ENABLE([debug],
        AS_HELP_STRING([--enable-debug], [Enable debugging output on stderr]))

AC_ARG_WITH([includes],
        [AS_HELP_STRING([--with-includes], [look for additional header files in DIRS])],
        [],
        [])

AC_ARG_WITH([libraries],
        [AS_HELP_STRING([--with-libraries], [look for additional libraries in DIRS])],
        [],
        [])
                       

for dir in $with_libraries
do
        LIBDIRS="$LIBDIRS -L$dir"
done

for dir in $with_includes
do
        INCLUDEDIRS="$INCLUDEDIRS -I$dir"
done

DEBUG=""
AS_IF([test "x$enable_debug" == "xyes"], [
        DEBUG="-Wall -Werror -g -ggdb -DDEBUG"
])

CXXFLAGS="$CXXFLAGS -std=c++11 $INCLUDEDIRS $DEBUG"
LDFLAGS="$LDFLAGS $LIBDIRS"

AC_PROG_CC
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile paxos++/Makefile paxos++/detail/Makefile paxos++/detail/protocol/Makefile test/Makefile])
AC_OUTPUT
