CFLAGS= -g -Wall
INC =-I/usr/src/linux/include/ 
STDLIBS = -lvgagl -lvga -lm 
SOUNDLIBS = -lasound -lY2

CC = gcc
HFILES = Makefile defs.h proto.h global.h struct.h paravars.h
OBJ = para.o graphics.o view.o blocks.o  map.o misc.o enemy.o influ.o bullet.o ship.o takeover.o rahmen.o paratext.o keyboard.o parainit.o

SND_OBJ =  sound.o
NOSND_OBJ= nosound.o

para: $(OBJ) $(SND_OBJ)
	$(CC) -o $@ $(CFLAGS) $(STDLIBS) $(SOUNDLIBS) $(OBJ) $(SND_OBJ)

paranosound: $(OBJ) $(NOSND_OBJ)
	$(CC) -o $@ $(CFLAGS)  $(STDLIBS) $(OBJ) $(NOSND_OBJ)

%.o : %.c $(HFILES)
	$(CC) -c $(CFLAGS) $(INC) $<


clean: 
	rm *.o

soundtest: soundtest.o
	$(CC) -o soundtest $(CFLAGS) $(STDLIBS) $(SOUNDLIBS) soundtest.o

vplay: vplay.c
	$(CC) -o vplay $(CFLAGS) $(STDLIBS) $(SOUNDLIBS) vplay.c

osstest: osstest.o
	$(CC) -o osstest $(CFLAGS) $(STDLIBS) $(SOUNDLIBS) osstest.o






