# Process this file with autoconf to produce a configure script.
AC_INIT([src/main.c])

AM_INIT_AUTOMAKE(freedroid, 0.7.1)

AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC

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
AC_CHECK_LIB([SDL_mixer], [Mix_OpenAudio])

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

# we don't want optimisation for the moment, as that makes 
# debugging a bit more confusing
CFLAGS="-g -Wall" 


#allow --enable-NE to choose the NEW_ENGINE :
AC_ARG_ENABLE([NE],
	AC_HELP_STRING( [--enable-NE],
			[use new experimental graphics engine]),
	[echo "--- New experimental graphics engine will be built!"; 
		CFLAGS="$CFLAGS -DNEW_ENGINE"],
	[echo "--- We'll build the old but working graphics engine"])



AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

