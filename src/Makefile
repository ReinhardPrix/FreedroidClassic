CFLAGS= -g
INC =-I/usr/src/linux/include/ 
LIBS = -lvgagl -lvga -lm -lasound -lY2

CC = gcc
HFILES = Makefile defs.h proto.h global.h struct.h
OBJ = para.o graphics.o view.o blocks.o sound.o map.o misc.o enemy.o influ.o bullet.o ship.o fm_hrd.o takeover.o rahmen.o paratext.o

para: $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(LIBS) $(OBJ)

%.o : %.c $(HFILES)
	$(CC) -c $(CFLAGS) $(INC) $<


clean: 
	rm *.o

soundtest: soundtest.o
	$(CC) -o soundtest $(CFLAGS) $(LIBS) soundtest.o

vplay: vplay.c
	$(CC) -o vplay $(CFLAGS) $(LIBS) vplay.c

osstest: osstest.o
	$(CC) -o osstest $(CFLAGS) $(LIBS) osstest.o






