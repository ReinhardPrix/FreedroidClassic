CFLAGS= -g
INC =-I/usr/src/linux/include/ 
LIBS = -lvgagl -lvga -lm 

CC = gcc
HFILES = Makefile defs.h proto.h global.h struct.h
OBJ = para.o graphics.o view.o blocks.o sound.o map.o misc.o enemy.o influ.o bullet.o ship.o fm_hrd.o takeover.o rahmen.o paratext.o

para: $(OBJ)
	$(CC) -o $@ $(CFLAGS) $(LIBS) $(OBJ)

%.o : %.c $(HFILES)
	$(CC) -c $(CFLAGS) $(INC) $<


clean: 
	rm *.o








