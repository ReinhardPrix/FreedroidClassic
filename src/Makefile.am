## Process this file with automake to produce Makefile.in

bin_PROGRAMS = freedroidRPG DialogEditor

freedroidRPG_SOURCES = menu.c shop.c network.c items.c character.c skills.c saveloadgame.c \
	leveleditor.c blocks.c  graphics.c  map.c   init.c  ship.c \
	takeover.c bullet.c  influ.c misc.c text.c text_public.c sound.c \
	enemy.c   input.c  main.c  rahmen.c view.c BFont.c \
	colodefs.h itemdefs.h defs.h map.h vars.h ship.h takeover.h global.h \
	proto.h struct.h system.h BFont.h DialogEditor.h \
	SDL_rotozoom.c SDL_rotozoom.h highscore.c

DialogEditor_SOURCES = DialogEditor.c text_public.c DialogEditor.h

INCLUDES        = @GTK_CFLAGS@
LDADD           = @GTK_LIBS@

man_MANS = freedroidRPG.6

EXTRA_DIST = $(man_MANS)
