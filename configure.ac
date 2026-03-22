# Process this file with autoconf to produce a configure script.
AC_INIT(freedroid, 1.9.0)

dnl Detect the canonical host and target build environment

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE
##AM_MAINTAINER_MODE

AC_CONFIG_HEADERS([config.h])

# Checks for programs.
#AC_PROG_AWK
AC_PROG_CC
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

AC_ARG_WITH([sdl-prefix],
	AS_HELP_STRING([--with-sdl-prefix=DIR], [Prefix containing SDL3 pkgconfig files (adds DIR/lib/pkgconfig to PKG_CONFIG_PATH)]),
	[fd_sdl_prefix="$withval"],
	[fd_sdl_prefix=""])

AC_ARG_ENABLE([sdl3-mixer],
	AS_HELP_STRING([--enable-sdl3-mixer], [Build with SDL3_mixer sound support]),
	[fd_enable_sdl3_mixer="$enableval"],
	[fd_enable_sdl3_mixer="no"])

if test "x$fd_sdl_prefix" != "x"; then
	PKG_CONFIG_PATH="$fd_sdl_prefix/lib/pkgconfig:$PKG_CONFIG_PATH"
	export PKG_CONFIG_PATH
	AC_MSG_NOTICE([Using SDL pkg-config prefix: $fd_sdl_prefix])
fi

AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_LIB([m], [sin],,
	AC_MSG_ERROR([libm not found!!
No maths library?? What kinda crazy system is that??]))

##AC_CHECK_LIB([c], [printf],, AC_MSG_ERROR([libc not found!! ]))

AC_PATH_X
AC_PATH_XTRA
if test x$have_x = xyes; then
	CFLAGS="$CFLAGS $X_CFLAGS"
fi

## Check for SDL3
AC_MSG_NOTICE([[Checking for compulsory SDL3 libraries:]])

PKG_CHECK_MODULES([SDL3], [sdl3],,
	AC_MSG_ERROR([*** SDL3 not found!]))
PKG_CHECK_MODULES([SDL3_IMAGE], [sdl3-image],,
	AC_MSG_ERROR([*** SDL3_image not found!]))

CFLAGS="$CFLAGS $SDL3_CFLAGS $SDL3_IMAGE_CFLAGS"
LIBS="$LIBS $SDL3_LIBS $SDL3_IMAGE_LIBS"

AC_CHECK_LIB([jpeg], [jpeg_start_compress],,AC_MSG_ERROR([
--------------------------------------------------
libjpeg needed to run Freedroid!
see http://www.ijg.org/
--------------------------------------------------]))

AC_CHECK_LIB([z], [compress],, AC_MSG_ERROR([
--------------------------------------------------
zlib is needed to run Freedroid!
see http://www.gzip.org/zlib/
--------------------------------------------------]))


AC_CHECK_LIB([png], [png_read_png],, AC_MSG_ERROR([
--------------------------------------------------
libpng needed to run Freedroid
see http://www.libpng.org/pub/png/libpng.html
--------------------------------------------------]))

AC_MSG_NOTICE([[Checking for optional SDL libraries:]])
if test "x$fd_enable_sdl3_mixer" = "xyes"; then
	PKG_CHECK_MODULES([SDL3_MIXER], [sdl3-mixer], [
		CFLAGS="$CFLAGS $SDL3_MIXER_CFLAGS"
		LIBS="$LIBS $SDL3_MIXER_LIBS"
		AC_DEFINE([HAVE_LIBSDL_MIXER], [1], [Define if SDL3_mixer is available])
	], [
		AC_MSG_WARN([
--------------------------------------------------
SDL3_mixer not found!
you need SDL3_mixer if you want sound!
--> compiling without sound support
--------------------------------------------------])
	])
else
	AC_MSG_NOTICE([SDL3_mixer disabled for this migration checkpoint])
fi

AC_CHECK_LIB([vorbis], [main],, AC_MSG_WARN([
--------------------------------------------------
libvorbis not found!
You need the Vorbis libs installed if you want
Freedroid to be able to play Ogg files (e.g. the Intro theme)
--------------------------------------------------]))


# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h sys/time.h time.h unistd.h dirent.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset sqrt strstr strtok strcspn strspn rand])

dnl Don't Check for getopt_long
dnl just use bundled version like hello does, and gnuchess


case "$target" in
     *-*-cygwin* | *-*-mingw32*)
     CFLAGS="$CFLAGS  -DFD_DATADIR='\".\"'"
     win32=true
     ;;

     *)
     CFLAGS="$CFLAGS  -DFD_DATADIR='\"\$(pkgdatadir)\"'"   #avoid expansion of $pkgdatadir !
     win32=false
     ;;
esac

dnl send a signal to automake if we're compiling for win32
AM_CONDITIONAL(WIN32, test x$win32 = xtrue)


AC_CONFIG_FILES([Makefile src/Makefile map/Makefile sound/Makefile graphics/Makefile
graphics/classic_theme/Makefile graphics/lanzz_theme/Makefile graphics/para90_theme/Makefile])

AC_OUTPUT
