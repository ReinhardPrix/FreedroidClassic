# Process this file with autoconf to produce a configure script.
AC_INIT(freedroidRPG, 0.9.12cvs)

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Setup for automake
AM_INIT_AUTOMAKE

AM_CONFIG_HEADER(config.h)

# Checks for programs.
#AC_PROG_AWK
AC_PROG_CC
AC_PROG_MAKE_SET

# we don't want optimisation for the moment, as that makes 
# debugging a bit more confusing
#CFLAGS="-g -Wall"
CFLAGS="$CFLAGS -Wall"
LDFLAGS="$LDFLAGS -rdynamic"

# Checks for libraries.
AC_CHECK_LIB([m], [sin],, 
	AC_MSG_ERROR([libm not found!! 
No maths library?? What kinda crazy system is that??]))

AC_PATH_X	
AC_PATH_XTRA
if test x$have_x = xyes; then
	CFLAGS="$CFLAGS $X_CFLAGS"
fi

case "$target" in
	*-*-cygwin* | *-*-mingw32*)
	CFLAGS="$CFLAGS  -DFD_DATADIR='\".\"'"
	SYS_GL_LIB=opengl32
	win32=true
	;;

	*)
	CFLAGS="$CFLAGS  -DFD_DATADIR='\"\$(pkgdatadir)\"'"   #avoid expansion of $pkgdatadir !
	SYS_GL_LIB=GL
	win32=false
	;;
esac

dnl Check for SDL
AC_MSG_NOTICE([[Checking for compulsory SDL libraries:]])

SDL_VERSION=1.2.3
AM_PATH_SDL($SDL_VERSION,
            :,
	    AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CFLAGS="$CFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"


AC_CHECK_LIB([SDL], [SDL_Init],, 
	AC_MSG_ERROR([SDL library needed for FreedroidRPG!
see http://www.libsdl.org/]))


AC_CHECK_LIB([jpeg], [jpeg_start_compress],,
	AC_MSG_ERROR([libjpeg needed to run FreedroidRPG!
see http://www.ijg.org/]))

AC_CHECK_LIB([z], [compress],,
	AC_MSG_ERROR([zlib is needed to run FreedroidRPG!
see http://www.gzip.org/zlib/]))

AC_CHECK_LIB([png], [png_read_png],,
	AC_MSG_ERROR([libpng needed to run FreedroidRPG
see http://www.libpng.org/pub/png/libpng.html]))

AC_CHECK_LIB([SDL_image], [IMG_LoadJPG_RW],,
	AC_MSG_ERROR([SDL_image library needed for FreedroidRPG!
see http://www.libsdl.org/]))

AC_CHECK_LIB([SDL_net], [SDLNet_Init],,
	AC_MSG_ERROR([SDL_net library needed for FreedroidRPG!
see http://www.libsdl.org/]))


AC_MSG_NOTICE([[Checking for optional SDL libraries:]])
AC_CHECK_LIB([SDL_mixer], [Mix_ChannelFinished],,
	AC_MSG_WARN([libSDL_mixer not found!
you need the SDL_mixer library (version >= 1.2.1) if you want sound!
(see see http://www.libsdl.org/)
--> compiling without sound support]))


AC_CHECK_LIB([vorbis], [vorbis_block_init],, AC_MSG_WARN([
--------------------------------------------------
libvorbis not found!
You need the Vorbis libs installed if you want
Freedroid to be able to play Ogg files (e.g. the Intro theme)
--------------------------------------------------]))

dnl , [ -logg -lvorbisenc -lvorbisfile  ]

dnl Don't Check for getopt_long 
dnl just use bundled version like hello does, and gnuchess

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h sys/time.h time.h \
			  unistd.h dirent.h sys/soundcard.h GL/gl.h])


if test x$win32 = xtrue; then
dnl use openGL by default for win32 
LIBS="$LIBS -l$SYS_GL_LIB"
AC_DEFINE(HAVE_LIBGL, 1)
else
AC_CHECK_LIB([GL], [glEnable],, AC_MSG_WARN([
--------------------------------------------------
OpenGL development libs/header not found!
Building FreedroidRPG without OpenGL support.
--------------------------------------------------]))
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
##AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([alarm getcwd gettimeofday memset sqrt strstr strtok ])

## Check gtk libs
if test x$win32 = xfalse; then
AM_PATH_GTK(1.2.0, [ found_gtk=true ], AC_MSG_WARN([
--------------------------------------------------
WARNING: You need libgtk-dev >= 1.20 in order to 
be able to build the DialogEditor and ItemEditor!

If you only want to play Freedroid you can ignore this!
--------------------------------------------------]))
fi

dnl send a signal to automake if we're compiling for win32
AM_CONDITIONAL(WIN32, test x$win32 = xtrue)

dnl send a signal to automake wether gtk was found or not
AM_CONDITIONAL(FOUND_GTK, test x$found_gtk = xtrue)

AC_CONFIG_FILES([Makefile src/Makefile croppy/Makefile gluem/Makefile win32/Makefile])

AC_OUTPUT
