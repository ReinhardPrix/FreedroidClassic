# Process this file with autoconf to produce a configure script.
AC_INIT(FreeDroid, 0.7.0, r.prix@maths.soton.ac.uk, freedroid)

#once Johannes has updated his autoconf, we'll enforce version 2.53
# this is necessary to get the PACKAGE_STRING #defines included in config.h.in
# AC_PREREQ(2.53)

# actually, this macro might just do the right thing, even with version <2.53
# so let's try:
AC_PACKAGE_STRING

# we want to use automake:
AM_INIT_AUTOMAKE(@PACKAGE_TARNAME@, @PACKAGE_VERSION@)

AC_CONFIG_SRCDIR([src/para.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

# Checks for libraries.
AC_MSG_NOTICE([[Checking for standard maths library]])
AC_MSG_NOTICE([[-----------------------------------]])
AC_CHECK_LIB([m], [sin])
AC_MSG_NOTICE([[Checking for Yiff sound server]])
AC_MSG_NOTICE([[-----------------------------------]])
AC_CHECK_LIB([Y2], [YOpenConnection])
AC_MSG_NOTICE([[Checking for SDL libraries]])
AC_MSG_NOTICE([[-----------------------------------]])
AC_CHECK_LIB([SDL], [SDL_Init])
AC_CHECK_LIB([SDL_image], [IMG_Load])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([alarm getcwd gettimeofday memset sqrt strstr])

CPPFLAGS="$CPPFLAGS -I.. -I/usr/include/SDL"
CFLAGS="$CFLAGS -Wall" 

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
