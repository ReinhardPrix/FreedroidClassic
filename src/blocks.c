/*=@Header==============================================================
 * $Source$
 *
 * @Desc: block operating functions
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 *-@Header------------------------------------------------------------*/

// static const char RCSid[]=\
// "$Id$";

#define _blocks_c

#include <stdio.h>
#include <math.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#define DIGITLENGTH 9
#define DIGITHEIGHT 9
#define DIGITNUMBER 20
#define DIGITMEM DIGITHEIGHT*DIGITLENGTH*DIGITNUMBER

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SmallBlock(int LX,int LY, int BlockN,unsigned char* Screen,int SBreite)
{
  int i,j;
  unsigned char *source=MapBlocks+BLOCKBREITE*BLOCKHOEHE*BlockN;
  unsigned char *target=Screen+LY*SBreite+LX;

  //printf("\nvoid SmallBlock(...): Function call confirmed.");
  if (LX>USERFENSTERPOSX+USERFENSTERBREITE) return;
  for(i=0;i<8;i++) {
    for(j=0;j<8;j++) {
      *target=*source;
      target++;
      if (Screen == RealScreen) {
	vga_setcolor(*source);
	vga_drawpixel(LX+i,LY+j);
      }
      source+=4;
      //Screen[LX+j+(LY+i)*SBreite]=
      //MapBlocks[BlockN*BLOCKBREITE*BLOCKHOEHE+j*4+i*BLOCKBREITE*4];
    }
    target+=SBreite-8;
    source+=4*BLOCKBREITE-4*8;
  }

  //printf("\nvoid SmallBlock(...): Usual end of function reached.");
} // void SmallBlock(...)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SmallBlast(int LX,int LY, int BlastT,int phase,unsigned char* Screen,int SBreite)
{
	int i,j;

	if (LX>USERFENSTERPOSX+USERFENSTERBREITE) return;
	for(i=0;i<8;i++) {
		for(j=0;j<8;j++)
			if (*(Blastmap[BlastT].picpointer+j*4+i*BLOCKBREITE*4+phase*BLOCKMEM) != TRANSPARENTCOLOR )
				Screen[LX-DIGITLENGTH/2+j+(LY+i-DIGITHEIGHT/2)*SBreite]=
					*(Blastmap[BlastT].picpointer+j*4+i*BLOCKBREITE*4+phase*BLOCKMEM);
	}
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SmallBullet(int LX,int LY, int BulletT,int phase,unsigned char* Screen,int SBreite)
{
 	Blast CurBlast = &(AllBlasts[0]);
	int i,j;

	if (LX>USERFENSTERPOSX+USERFENSTERBREITE) return;
	for(i=0;i<8;i++) {
		for(j=0;j<8;j++) {
			if (*(Bulletmap[BulletT].picpointer+j*4+i*BLOCKBREITE*4+phase*BLOCKMEM) != TRANSPARENTCOLOR) {
				if ((unsigned char)Screen[LX-DIGITLENGTH/2+j+(LY+i-DIGITHEIGHT/2)*SBreite] == BULLETCOLOR) StartBlast(LX*4+2,LY*4+2,DRUIDBLAST);
				Screen[LX-DIGITLENGTH/2+j+(LY+i-DIGITHEIGHT/2)*SBreite]=
					*(Bulletmap[BulletT].picpointer+j*4+i*BLOCKBREITE*4+phase*BLOCKMEM);
			}
		}
	}
	for(j=0;j<MAXBLASTS;j++){
	 	/* check Blast-Bullet Collisions and kill hit Bullets */
		for( i=0; i<MAXBULLETS; i++ ) {
			if( AllBullets[i].type == OUT ) continue;
			if( CurBlast->phase > 4) break;
		
			if( abs(AllBullets[i].PX - CurBlast->PX) < BLASTRADIUS ) 
				 if( abs(AllBullets[i].PY - CurBlast->PY) < BLASTRADIUS)
				 {
				 	/* KILL Bullet silently */
				 	AllBullets[i].type = OUT;
				 	AllBullets[i].mine = FALSE;
				 }
		} /* for */
	 	CurBlast++;
	} /* for */
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void SmallEnemy(int LX,int LY,int enemyclass,unsigned char* Screen,int SBreite)
{
	int i,j;
	enemyclass+=10;

	if (LX>USERFENSTERPOSX+USERFENSTERBREITE) return;
	for(i=0;i<DIGITHEIGHT;i++) {
		for(j=0;j<DIGITLENGTH-1;j++)
			if (Digitpointer[enemyclass*DIGITLENGTH*DIGITHEIGHT+i*DIGITLENGTH+j]!=TRANSPARENTCOLOR)
				Screen[LX-DIGITLENGTH/2+j+(LY+i-DIGITHEIGHT/2)*SBreite]=
					Digitpointer[enemyclass*DIGITLENGTH*DIGITHEIGHT+i*DIGITLENGTH+j];
//					Enemypointer[j*4+i*BLOCKBREITE*4];
	}
}

/* *********************************************************************** */

void GetDigits(){
int i;
unsigned char* Localpointer;

	Digitpointer=MyMalloc(DIGITMEM);
	LadeLBMBild(DIGITBILD,InternalScreen,FALSE);

	for (i=0;i<20;i++) {
		IsolateBlock(
			InternalScreen,
			Digitpointer+DIGITHEIGHT*DIGITLENGTH*i,
			i*DIGITLENGTH,
			0,DIGITLENGTH,DIGITHEIGHT);
	}
}

/* *********************************************************************** */

/*@Function============================================================
@Desc: 	Diese Prozedur isoliert einzelne Teile aus dem momentan angezeigten Bild
	und legt diese im Speicher ab

	Parameter:
		unsigned char *screen: Screen, auf dem Grafik steht
		char *target: Ziel, in das der Block kopiert wird
					==> muss schon reserviert sein !!
					
		Links/Oben : Koordinaten des linken oberen Eck des Blocks
	
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void IsolateBlock(
	unsigned char *screen,
	unsigned char *target,
	int BlockEckLinks,
	int BlockEckOben,
	int Blockbreite,
	int Blockhoehe)
{
	int row, col;
	unsigned char *source;
	unsigned char *tmp;

	source = screen + BlockEckOben*SCREENLEN + BlockEckLinks;
	tmp = target;
	
	for( row = 0; row < Blockhoehe; row ++ ) {
		memcpy(tmp, source, Blockbreite);
		tmp += Blockbreite;
		source += SCREENLEN;
	}
	
	
	
}

/* ***********************************************************************/
void GetMapBlocks(void)
{
/*
	Diese Prozedur isoliert ueber Aufrufe der Funktion GetBlock aus dem
	Anfangsbild alle Bloecke, die benoetigt werden und legt diese im Speicher
   ab

   Parameter: keine
*/

	int i;
	unsigned char *tmp;

	MapBlocks=(unsigned char*)MyMalloc(BLOCKANZAHL*BLOCKMEM+100);
	LadeLBMBild(BLOCKBILD1,InternalScreen,FALSE);

	tmp = MapBlocks;
	
	for(i=0;i<9;tmp += BLOCKMEM, i++)
		IsolateBlock(InternalScreen, tmp, i*(BLOCKBREITE+1),0, BLOCKBREITE, BLOCKHOEHE);

   for(i=0;i<9;tmp += BLOCKMEM, i++)
		IsolateBlock(InternalScreen, tmp, i*(BLOCKBREITE+1), BLOCKHOEHE+1,
			BLOCKBREITE, BLOCKHOEHE);

   for(i=0;i<9;tmp += BLOCKMEM, i++)
		IsolateBlock(InternalScreen, tmp, i*(BLOCKBREITE+1), BLOCKHOEHE*2+2,
			BLOCKBREITE, BLOCKHOEHE);

   for(i=0;i<7;tmp += BLOCKMEM, i++)
		IsolateBlock(InternalScreen, tmp, i*(BLOCKBREITE+1), BLOCKHOEHE*3+3,
			BLOCKBREITE, BLOCKHOEHE);

	for(i=0;i<8;tmp += BLOCKMEM, i++)
		IsolateBlock(InternalScreen, tmp, i*(BLOCKBREITE+1), BLOCKHOEHE*4+4,
			BLOCKBREITE, BLOCKHOEHE);

}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void GetShieldBlocks(void){
	int i;

	// Sicherheitsabfrage gegen doppeltes initialisieren
	if (ShieldBlocks) {
		printf(" Die Schildbloecke waren schon initialisiert !");
		getchar();
		Terminate(ERR);
	}

	if (!(ShieldBlocks=MyMalloc(4*BLOCKMEM+10))){
		printf(" Keine Memory fuer ShieldBlocks !.");
		getchar();
		Terminate(ERR);
	}
	
	// Laden des Bildes
	LadeLBMBild(SHIELDPICTUREBILD,InternalScreen,FALSE);

	// Isolieren der Schildbl"ocke
	for(i=0;i<4;i++){
		IsolateBlock(InternalScreen, ShieldBlocks+i*BLOCKBREITE*BLOCKHOEHE, i*(BLOCKBREITE+1), 0, BLOCKBREITE, BLOCKHOEHE);
	}
}



/*@Function============================================================
@Desc: GetBlocks
				gets the requested picture file and returns a pointer to
				the requested blocks (sequtially stored)
				
@Arguments:	char *picfile	: the picture-file to load, or
									NULL to use the old one
									
				int line: block-line to get blocks from
				int num:	number of blocks to get from line

@Ret: char * : pointer to 
@Int:
* $Function----------------------------------------------------------*/
unsigned char *GetBlocks(char *picfile, int line, int num)
{
	int i;
	unsigned char *tmp;
	unsigned char *blocktarget;
	
	if(picfile) {
		LadeLBMBild(picfile,InternalScreen,FALSE);
	}
	
	if(!num) return NULL; /* this was only an 'init'-call */
	
	blocktarget=MyMalloc(BLOCKMEM * num+1600);
	tmp = blocktarget;
	
	for (i=0;i<num;tmp += BLOCKMEM, i++)
		IsolateBlock(InternalScreen, tmp, i*(BLOCKBREITE+1), line*(BLOCKHOEHE+1),
			BLOCKBREITE, BLOCKHOEHE );

	return blocktarget;
}


/*@function============================================================
@Desc: void DisplayBlock(): gibt Block *block (len*height) an angegebener
Bildschirmposition x/y aus auf screen
						
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void DisplayBlock(
		  int x, int y,
		  unsigned char *block,
		  int len, int height,
		  unsigned char *screen )
{
  int row,i,j;
  unsigned char *screenpos;
  unsigned char *source = block;
  
  
  //
  // THIS IS NEW FROM AFTER THE PORT, BECAUSE 'REALSCREEN' IS NO LONGER DIRECTLY ACCESSIBLE
  //

    if( screen == RealScreen ) {
      // printf("\nvoid DisplayBlock(...): screen parameter is NULL... Outputing DIRECTLY!\n");
      for(i=0;i<height;i++) {
	for(j=0;j<len;j++) {
	  vga_setcolor(*source);
	  source++;
	  vga_drawpixel(j+x,i+y);
	}
      }
      return;
    } // if (screen==RealScreen)
  
  

  // 
  // HERE COMES THE OLD STUFF FROM BEFORE THE PORT
  //
  
	screenpos = screen + y*SCREENLEN + x;

	for( row = 0; row < height; row++ ) {
		memcpy(screenpos, source, len);
		screenpos += SCREENLEN;
		source += len;
	}

	return;
}


/*@function============================================================
@Desc: void DisplayMergeBlock(): setzt Block *block (len*height) an angegebener
								Bildschirmposition x/y in den angeg. Bildschirm
								-beachtet dabei TRANSPARENTCOLOR 

						
@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void DisplayMergeBlock(
int x, int y,
unsigned char *block,
int len, int height,
unsigned char *screen)
{
	int row, col;
	unsigned char *Screenpos;
	unsigned char *source = block;

	if( screen == NULL )
		return;
		
	Screenpos = screen + y*SCREENLEN + x;

	for( row = 0; row < height; row++ ) {
		for( col = 0; col < len; col ++ ) 
			if( *source != TRANSPARENTCOLOR ) *Screenpos++ = *source ++;
			else {
				Screenpos ++;
				source ++;
			}
		Screenpos += SCREENLEN-len;
	}

	return;
}

/*@Function============================================================
@Desc: CopyMergeBlock(): copies a block in memory, but doesn't copy
								Transparent-color 

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void CopyMergeBlock(unsigned char *target, unsigned char *source, int mem)
{
	register int i;

	for( i=0; i<mem; i++, source++, target++)
		if( *source != TRANSPARENTCOLOR ) *target = *source;

} /* Copy merge Block */
	

/*@Function============================================================
@Desc: MergeBlockToWindow(source, target, linelen)
			This copies a block from memory (sequentially stored)
			to a "window": All lines under each other, "invisible" points
			are left out (TRANSPARENTCOLOR).
			It must	know the length of a window-line.

			int check: TRUE/FALSE: Bullet-Collisionen checken
			
@Ret: TRUE/FALSE: BulletCollision
@Int:
* $Function----------------------------------------------------------*/
int MergeBlockToWindow(
	register unsigned char *source,
	register unsigned char *target,
	int WinLineLen,		/* in pixel ! */
	int check)
{
	register int i, j;
	int ret=FALSE;
	register int lineskip = WinLineLen - BLOCKBREITE;

	if (check) {
		for(i=0; i<BLOCKHOEHE; i++, target += lineskip)
			for(j=0; j<BLOCKBREITE; j++) {
				if( *source != TRANSPARENTCOLOR) {
					if (*target == BULLETCOLOR) ret=TRUE;
					*target++ = *source++;
				} else {
					target++;
					source++;
				}
			}
	} else {
		for(i=0; i<BLOCKHOEHE; i++, target += lineskip)
			for(j=0; j<BLOCKBREITE; j++) {
						
				if( *source != TRANSPARENTCOLOR) {
					*target++ = *source++;
				}
				else {
					target++;
					source++;
				}
			}
	} /* if else */
	
	return (ret);
	
} /* MergeBlockto.. */




#undef _blocks_c

