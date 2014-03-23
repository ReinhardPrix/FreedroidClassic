     ## Process this file with automake to produce Makefile.in

##AM_CFLAGS=@CFLAGS@

bin_PROGRAMS = freedroid

if WIN32
win32_res = freedroid.rc
freedroid_LDADD = freedroid.coff
else
win32_res =
endif

freedroid_SOURCES = menu.c graphics.c  map.c   init.c  ship.c \
	takeover.c bullet.c  influ.c  misc.c  text.c  sound.c \
	enemy.c input.c  main.c  view.c BFont.c defs.h \
	map.h vars.h takeover.h global.h \
	text.h proto.h struct.h system.h BFont.h maped.h \
	highscore.c getopt.c getopt1.c getopt.h \
	$(win32_res)

freedroid_CPPFLAGS = -DLOCAL_DATADIR=\"$(top_srcdir)\"


WINDRES = i586-mingw32msvc-windres
freedroid.coff: freedroid.rc ../graphics/paraicon.ico
	$(WINDRES) -i freedroid.rc -o freedroid.coff -O COFF

