# Process this file with autoconf to produce a configure script.
AC_INIT([src/main.c])

AM_INIT_AUTOMAKE(freedroid, 0.7.3)

AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([m], [sin],, 
AC_MSG_ERROR([libm not found!! 
No maths library?? What kinda crazy system is that??]))

AC_MSG_NOTICE([[Checking for compulsory SDL libraries:]])
AC_CHECK_LIB([SDL], [SDL_Init],, 
	AC_MSG_ERROR([libSDL not found!
*******************************************************
You need the SDL library to compile Freedroid!
*******************************************************]))

AC_CHECK_LIB([SDL_image], [IMG_Load],,
	AC_MSG_ERROR([libSDL_image not found!
*******************************************************
You need the SDL_image library to compile Freedroid!
*******************************************************]))

AC_MSG_NOTICE([[Checking for optional SDL libraries:]])
AC_CHECK_LIB([SDL_mixer], [Mix_OpenAudio],,
	AC_MSG_WARN([libSDL_mixer not found!
*******************************************************
you need the SDL_mixer library if you want sound!
--> compiling without sound support
*******************************************************]))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h sys/time.h unistd.h])

AC_CHECK_HEADERS([SDL/SDL.h SDL/SDL_image.h],,
	AC_MSG_ERROR([missing <SDL/SDL.h> or <SDL/SDL_image.h> headers!
*******************************************************
You need the SDL header files to compile Freedroid!
*******************************************************]))

AC_CHECK_HEADERS([SDL/SDL_mixer.h],,
	AC_MSG_WARN([Missing <SDL/SDL_mixer.h> header !
*******************************************************
You need the SDL_mixer header if you want sound!
--> compiling without sound support
*******************************************************]))

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
	[echo "--- We'll build the old (but stable) graphics engine"])



AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

