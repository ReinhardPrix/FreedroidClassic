if WIN32
else
bin_PROGRAMS = pngtoico
pngtoico_SOURCES = pngtoico.c
endif

%.ico : %.png
	pngtoico $< > $@

WINDRES = i586-mingw32msvc-windres
freedroidRPG.coff: freedroidRPG.rc w32icon_64x64.ico
	$(WINDRES) -i freedroidRPG.rc -o freedroidRPG.coff -O COFF

