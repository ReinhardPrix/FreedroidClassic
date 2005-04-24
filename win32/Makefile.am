if WIN32
else
bin_PROGRAMS = pngtoico
pngtoico_SOURCES = pngtoico.c
endif

%.ico : %.png
	pngtoico $< > $@

WINDRES = $(target)-windres
freedroidRPG.coff: freedroidRPG.rc w32icon_64x64.ico
	$(WINDRES) -i freedroidRPG.rc -o freedroidRPG.coff -O COFF

