## Process this file with automake to produce Makefile.in
##AM_CFLAGS=@CFLAGS@

bin_PROGRAMS = freedroid

EXTRA_PROGRAMS = freedroidDebug

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

## clang santizer flags: turn on clang's memory-leak sanitizer, see https://clang.llvm.org/docs/AddressSanitizer.html
freedroidDebug_CFLAGS = $(AM_CFLAGS) -g3 -Og -fsanitize=address,undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-common
freedroidDebug_SOURCES = $(freedroid_SOURCES)

freedroid_CPPFLAGS = -DLOCAL_DATADIR=\"$(top_srcdir)\"
freedroidDebug_CPPFLAGS = $(freedroid_CPPFLAGS)

WINDRES = i586-mingw32msvc-windres
freedroid.coff: freedroid.rc ../graphics/paraicon.ico
	$(WINDRES) -i freedroid.rc -o freedroid.coff -O COFF
