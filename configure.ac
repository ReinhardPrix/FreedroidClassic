# Process this file with autoconf to produce a configure script.
AC_INIT([src/main.c])

dnl Setup for automake
SDL_VERSION=1.2.3

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET


AM_INIT_AUTOMAKE(freedroid, 1.0.2)

AM_CONFIG_HEADER(config.h)

# Checks for programs.
#AC_PROG_AWK
AC_PROG_CC
AC_PROG_MAKE_SET

# we don't want optimisation for the moment, as that makes 
# debugging a bit more confusing
CFLAGS="-g -Wall"

# Checks for libraries.
AC_CHECK_LIB([m], [sin],, 
	AC_MSG_ERROR([libm not found!! 
No maths library?? What kinda crazy system is that??]))

AC_PATH_X	
AC_PATH_XTRA
if test x$have_x = xyes; then
	CFLAGS="$CFLAGS $X_CFLAGS"
fi

dnl Check for SDL
AC_MSG_NOTICE([[Checking for compulsory SDL libraries:]])

AM_PATH_SDL($SDL_VERSION,
            :,
	    AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"


AC_CHECK_LIB([SDL], [SDL_Init],, 
	AC_MSG_ERROR([SDL library needed for Freedroid!
see http://www.libsdl.org/]))


AC_CHECK_LIB([jpeg], [jpeg_start_compress],,
	AC_MSG_ERROR([libjpeg needed to run Freedroid!
see http://www.ijg.org/]))

AC_CHECK_LIB([z], [compress],,
	AC_MSG_ERROR([zlib is needed to run Freedroid!
see http://www.gzip.org/zlib/]))

AC_CHECK_LIB([png], [png_read_png],,
	AC_MSG_ERROR([libpng needed to run Freedroid
see http://www.libpng.org/pub/png/libpng.html]))

AC_CHECK_LIB([SDL_image], [IMG_LoadJPG_RW],,
	AC_MSG_ERROR([SDL_image library needed for Freedroid!
see http://www.libsdl.org/]))


AC_MSG_NOTICE([[Checking for optional SDL libraries:]])
AC_CHECK_LIB([SDL_mixer], [Mix_OpenAudio],,
	AC_MSG_WARN([libSDL_mixer not found!
you need the SDL_mixer library if you want sound!
(see see http://www.libsdl.org/)
--> compiling without sound support]))


 Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h sys/time.h time.h \
			  unistd.h dirent.h getopt.h sys/soundcard.h])

 Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

 Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([alarm getcwd gettimeofday memset sqrt strstr strtok])



CFLAGS="$CFLAGS  -DDATADIR='\"\$(pkgdatadir)\"'"   #avoid expansion of $pkgdatadir !


AC_CONFIG_FILES([Makefile src/Makefile map/Makefile sound/Makefile 
		graphics/Makefile graphics/classic_theme/Makefile
		graphics/lanzz_theme/Makefile])

AC_OUTPUT





