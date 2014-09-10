     ## Process this file with automake to produce Makefile.in

##AM_CFLAGS=@CFLAGS@

bin_PROGRAMS = freedroid

if WIN32
win32_res = freedroid.rc
freedroid_LDADD = freedroid.coff
else
win32_res =
endif

freedroid_SOURCES = \
		BFont.c \
		BFont.h \
		bullet.c \
		config.h \
		defs.h \
		enemy.c \
		getopt.c \
		getopt.h \
		getopt1.c \
		global.h \
		graphics.c \
		highscore.c \
		influ.c \
		init.c \
		input.c \
		level_editor.c \
		main.c \
		map.c \
		map.h \
		maped.h \
		menu.c \
		misc.c \
		proto.h \
		ship.c \
		sound.c \
		struct.h \
		system.h \
		takeover.c \
		takeover.h \
		text.c \
		text.h \
		vars.h \
		view.c \
		$(win32_res)

freedroid_CPPFLAGS = -DLOCAL_DATADIR=\"$(top_srcdir)\"


WINDRES = i586-mingw32msvc-windres
freedroid.coff: freedroid.rc ../graphics/paraicon.ico
	$(WINDRES) -i freedroid.rc -o freedroid.coff -O COFF

