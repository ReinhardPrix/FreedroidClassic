## Process this file with automake to produce Makefile.in

if WIN32
freedroidRPG_LDADD = ../win32/freedroidRPG.coff
endif

## compile Dialog-editor if gtk-libs were found
if BUILD_EDITORS
extra_prg = DialogEditor ItemEditor
else
extra_prg = 
endif


bin_PROGRAMS = freedroidRPG $(extra_prg)

EXTRA_PROGRAMS = DialogEditor ItemEditor

freedroidRPG_SOURCES = menu.c shop.c network.c items.c character.c skills.c saveloadgame.c \
	leveleditor.c blocks.c  graphics.c  map.c   init.c  ship.c \
	takeover.c bullet.c  influ.c misc.c text.c text_public.c sound.c \
	enemy.c   input.c  main.c  hud.c view.c automap.c BFont.c \
	colodefs.h itemdefs.h defs.h map.h vars.h ship.h takeover.h global.h \
	proto.h struct.h system.h BFont.h assign_license_string.h \
	SDL_rotozoom.c SDL_rotozoom.h open_gl.c mission.c chat.c light.c \
	getopt.c getopt1.c getopt.h scandir.c scandir.h 

DialogEditor_SOURCES = DialogEditor.c text_public.c DialogEditor.h
ItemEditor_SOURCES = ItemEditor.c text_public.c ItemEditor.h DialogEditor.h


if WIN32
INCLUDES	= -I/usr/i586-mingw32msvc -I/usr/include/glib-1.2 -I/usr/lib/glib/include
LDADD           = -lgtk -lgdk
else
LDADD           = @GTK_LIBS@
INCLUDES        = @GTK_CFLAGS@
endif
