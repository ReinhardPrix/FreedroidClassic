# Process this file with autoconf to produce a configure script.
AC_INIT([src/main.c])

dnl Setup for automake
SDL_VERSION=1.2.3

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET


AM_INIT_AUTOMAKE(freedroidRPG, 0.9.5)

AM_CONFIG_HEADER(config.h)

# Checks for programs.
#AC_PROG_AWK
AC_PROG_CC
AC_PROG_MAKE_SET

# we don't want optimisation for the moment, as that makes 
# debugging a bit more confusing
#CFLAGS="-g -Wall"

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


AC_CHECK_LIB([vorbis], [ov_open],, AC_MSG_WARN([
--------------------------------------------------
libvorbis not found!
You need the Vorbis libs installed if you want
Freedroid to be able to play Ogg files (e.g. the Intro theme)
--------------------------------------------------]))


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h sys/time.h time.h \
			  unistd.h dirent.h getopt.h sys/soundcard.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([alarm getcwd gettimeofday memset sqrt strstr strtok])

## Check gtk libs
AM_PATH_GTK(1.2.0,,AC_MSG_WARN([
--------------------------------------------------
WARNING: You need libgtk-dev >= 1.20 in order to 
be able to build the DialogEditor!

If you only want to play Freedroid you can ignore this!
--------------------------------------------------]))dnl
CFLAGS="$CFLAGS  -DDATADIR='\"\$(pkgdatadir)\"'"   #avoid expansion of $pkgdatadir !


AC_CONFIG_FILES([Makefile 
                src/Makefile 
                map/Makefile 
                dialogs/Makefile 
		dialog_editor/Makefile
                sound/Makefile 
		sound/speeches/Makefile
		sound/effects/Makefile 
		sound/effects/bot_sounds/Makefile 
		sound/music/Makefile
		graphics/Makefile
		graphics/lanzz_theme/Makefile 
		graphics/lanzz_theme/single_map_tiles/Makefile 
                graphics/backgrounds/Makefile
		graphics/font/Makefile 
                graphics/map_inserts/Makefile 
		graphics/mouse_buttons/Makefile
		graphics/skill_icons/Makefile
		graphics/droids/Makefile 
		graphics/droids/123/Makefile
		graphics/droids/139/Makefile
		graphics/droids/247/Makefile
		graphics/droids/249/Makefile
		graphics/droids/296/Makefile
		graphics/droids/302/Makefile
		graphics/droids/329/Makefile
		graphics/droids/420/Makefile
		graphics/droids/476/Makefile
		graphics/droids/493/Makefile
		graphics/droids/516/Makefile
		graphics/droids/571/Makefile
		graphics/droids/598/Makefile
		graphics/droids/614/Makefile
		graphics/droids/615/Makefile
		graphics/droids/629/Makefile
		graphics/droids/711/Makefile
		graphics/droids/742/Makefile
		graphics/droids/751/Makefile
		graphics/droids/821/Makefile
		graphics/droids/834/Makefile
		graphics/droids/883/Makefile
		graphics/droids/999/Makefile
		graphics/droids/brown_worker/Makefile
		graphics/droids/red_guard/Makefile
		graphics/droids/green_guard/Makefile
		graphics/droids/blue_guard/Makefile
		graphics/droids/professor/Makefile
		graphics/items/Makefile
		graphics/items/ammo_exterminator_sphere/Makefile
		graphics/items/ammo_laser_crystal/Makefile 
		graphics/items/ammo_plasma_bullet/Makefile 
		graphics/items/antigrav_unit/Makefile 
		graphics/items/antimatter_matter_converter/Makefile 
		graphics/items/basic_armour/Makefile 
		graphics/items/buckler/Makefile 
		graphics/items/brain_enlargement_antidode/Makefile 
		graphics/items/desk_lamp/Makefile 
		graphics/items/dixons_toolbox/Makefile 
		graphics/items/drinking_mug/Makefile 
		graphics/items/emp_grenade/Makefile 
		graphics/items/energy_capsule/Makefile 
		graphics/items/eating_plate/Makefile 
		graphics/items/eating_fork/Makefile 
		graphics/items/entropy_inverter/Makefile 
		graphics/items/green_armour/Makefile 
		graphics/items/hacker_wristband/Makefile 
		graphics/items/large_shield/Makefile 
		graphics/items/laser_sword_green/Makefile 
		graphics/items/laser_sword_red/Makefile 
		graphics/items/laser_sword_yellow/Makefile 
		graphics/items/monkey_wrench/Makefile 
		graphics/items/map_maker/Makefile 
		graphics/items/pill_dexterity/Makefile 
		graphics/items/pill_strength/Makefile 
		graphics/items/pill_magic/Makefile 
		graphics/items/plasma_double/Makefile 
		graphics/items/plasma_grenade/Makefile 
		graphics/items/plasma_single/Makefile 
		graphics/items/plasma_transistor/Makefile 
		graphics/items/plasma_triple/Makefile 
		graphics/items/red_dilitium_crystal/Makefile 
		graphics/items/small_shield/Makefile 
		graphics/items/sniper_wristband/Makefile 
		graphics/items/spellbook/Makefile 
		graphics/items/standard_shield/Makefile 
		graphics/items/superconducting_relay_unit/Makefile 
		graphics/items/tachyon_converter/Makefile 
		graphics/items/track_drive/Makefile 
		graphics/items/vmx_grenade/Makefile 
		graphics/items/warrior_wristband/Makefile 
		graphics/items/NONE_AVAILABLE_YET/Makefile 
                graphics/tux_motion_parts/Makefile])

AC_OUTPUT





