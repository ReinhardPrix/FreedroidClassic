## Process this file with automake to produce Makefile.in
if WIN32
win32_res = freedroidRPG.rc
freedroidRPG_LDADD = freedroidRPG.coff
## dont' try to build dialog-editor when compiling for Win32!
extra_prg = 
else
win32_res = 
## compile Dialog-editor if gtk-libs were found
extra_prg = @DIALOG_EDITOR@
endif


bin_PROGRAMS = freedroidRPG $(extra_prg)

EXTRA_PROGRAMS = DialogEditor

freedroidRPG_SOURCES = menu.c shop.c network.c items.c character.c skills.c saveloadgame.c \
	leveleditor.c blocks.c  graphics.c  map.c   init.c  ship.c \
	takeover.c bullet.c  influ.c misc.c text.c text_public.c sound.c \
	enemy.c   input.c  main.c  rahmen.c view.c BFont.c \
	colodefs.h itemdefs.h defs.h map.h vars.h ship.h takeover.h global.h \
	proto.h struct.h system.h BFont.h DialogEditor.h \
	SDL_rotozoom.c SDL_rotozoom.h highscore.c open_gl.c \
	getopt.c getopt1.c getopt.h scandir.c scandir.h  \
	$(win32_res)

DialogEditor_SOURCES = DialogEditor.c text_public.c DialogEditor.h

INCLUDES        = @GTK_CFLAGS@
LDADD           = @GTK_LIBS@

WINDRES = i586-mingw32msvc-windres
freedroidRPG.coff: freedroidRPG.rc ../graphics/freedroidRPG.ico
	$(WINDRES) -i freedroidRPG.rc -o freedroidRPG.coff -O COFF

