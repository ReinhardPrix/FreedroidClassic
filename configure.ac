# Process this file with autoconf to produce a configure script.
AC_INIT(FreeDroid, 0.6.0, r.prix@maths.soton.ac.uk, freedroid)

# we want to use automake:
AM_INIT_AUTOMAKE(@PACKAGE_TARNAME@, @PACKAGE_VERSION@)

AC_CONFIG_SRCDIR([para.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([Y2], [YOpenConnection])
AC_CHECK_LIB([m], [sin])
AC_CHECK_LIB([vga], [vga_init])
AC_CHECK_LIB([vgagl], [gl_clearscreen])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([alarm getcwd gettimeofday memset sqrt strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
