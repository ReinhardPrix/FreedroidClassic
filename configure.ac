# Process this file with autoconf to produce a configure script.
AC_INIT(FreeDroid, 0.6.0, r.prix@maths.soton.ac.uk, freedroid)

#once Johannes has updated his autoconf, we'll enforce version 2.53
# this is necessary to get the PACKAGE_STRING #defines included in config.h.in
# AC_PREREQ(2.53)

# we want to use automake:
AM_INIT_AUTOMAKE(@PACKAGE_TARNAME@, @PACKAGE_VERSION@)

AC_CONFIG_SRCDIR([src/para.c])
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([Y2], [YOpenConnection])
AC_CHECK_LIB([m], [sin])
AC_CHECK_LIB([vga], [vga_init])
AC_CHECK_LIB([vgagl], [gl_clearscreen])
AC_CHECK_LIB([SDLmain], [main])
AC_CHECK_LIB([SDL], [main])
AC_CHECK_LIB([SDL_image], [main])

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

CPPFLAGS="$CPPFLAGS -I/usr/include/SDL"
CFLAGS="$CFLAGS -Wall" 

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
