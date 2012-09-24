#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([libpaxos-cpp], [0.1.0], http://solatis.github.com/libpaxos-cpp)
AC_CONFIG_SRCDIR([paxos++/paxos.hpp])
AC_CONFIG_HEADER([config.h])


LT_INIT([shared static])

# Makefile initialization.
AM_INIT_AUTOMAKE([-Wall -Werror])

# Checks for programs.
if test -z $CXXFLAGS; then
    CXXFLAGS='-Wall -Werror -g -ggdb'
fi
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile paxos++/Makefile test/Makefile])
AC_OUTPUT
