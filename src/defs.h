/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of FreeDroid
 *
 *  FreeDroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeDroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeDroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */

/*----------------------------------------------------------------------
 *
 * Desc: all the define-constants and macroes
 *
 *----------------------------------------------------------------------*/

#ifndef _defs_h
#define _defs_h

#ifdef WARNALL
#include "warnall.h"
#endif
#include "colodefs.h"

// **********************************************************************
//

#define MEMDEBUG
#define SOUND

enum
{ SHOW_ALL = 0, SHOW_MAP = 1 };


// For development purposes, the sound will not be activated unless the following definition is made
// #undef PARADROID_SOUND_SUPPORT_ON
#define PARADROID_SOUND_SUPPORT_ON

// The following is the definition of the sound file names used in freedroid
// DO NOT EVER CHANGE THE ORDER OF APPEARENCE IN THIS LIST PLEASE!!!!!
// The order of appearance here should match the order of appearance 
// in the SoundSampleFilenames definition located in sound.c!

#define SILENCE (-1)
enum _sounds
{
  ERRORSOUND = 0,
  COMBAT_BACKGROUND_MUSIC_SOUND,
  TAKEOVER_BACKGROUND_MUSIC_SOUND,
  CONSOLE_BACKGROUND_MUSIC_SOUND,
  CLASSICAL_BEEP_BEEP_BACKGROUND_MUSIC,
  BLASTSOUND,
  COLLISIONSOUND,
  FIRESOUND,
  GOT_INTO_BLAST_SOUND,
  MOVE_ELEVATOR_SOUND,
  REFRESH_SOUND,
  LEAVE_ELEVATOR_SOUND,
  ENTER_ELEVATOR_SOUND,
  THOU_ART_DEFEATED_SOUND,
  GOT_HIT_SOUND,
  TAKEOVER_SET_CAPSULE_SOUND
};

// **********************************************************************

#define PARA64 						TRUE

#define TRUE (1==1)
#define FALSE (1==0)

#define ERR 	-1
#define OK		0

/* Konstanten fuer die Dateinamen */
/*
#define SHIPNAME "./map/ship1" 
#define PALBILD	"./graphics/palbild.lbm"
#define COLORFILE "./map/levels.col"
#define BLOCKBILD1 "./graphics/block.lbm"
#define BLOCKBILD2 ""
// #define TITELBILD1 "./graphics/title.lbm"
#define TITELBILD1PCX "./graphics/title.pcx"
#define RAHMENBILD1 "./graphics/rahmen.lbm"
#define PARAPLUSRAHMENBILD "./graphics/plusrah1.lbm"
#define BLASTBILD "./graphics/blast.lbm"
#define BULLETBILD "./graphics/bullet.lbm"
#define INFLUENCEBILD "./graphics/influ.lbm"
#define DATA70ZEICHENSATZ "./graphics/para8x8.fnt"
#define DIGITBILD "./graphics/digits.lbm"
#define ENEMYBILD "./graphics/enemy.lbm"
#define SEITENANSICHTBILD "./graphics/ship.lbm"
#define EL_BLOCKS_FILE 		"./graphics/ship2.lbm"
#define FONTBILD "./graphics/parafont.lbm"
#define CONSOLENBILD "./graphics/console.lbm"
#define ROBOTPICTUREBILD "./graphics/robots.lbm"
#define SHIELDPICTUREBILD "./graphics/shield.lbm"
#define TAKEOVERBACKGROUNDBILD "./graphics/overtake.lbm"
#define PLAYGROUND_FILE	"./graphics/to_ground.lbm"	// graphics - files 
#define ELEMENTS_FILE	"./graphics/to_elem.lbm"
*/

#define PALBILD_PCX                "../graphics/palbild.pcx"
#define BLOCKBILD1_PCX             "../graphics/block.pcx"
#define BLOCKBILD2_PCX             ""
#define TITELBILD1_PCX             "../graphics/newtitle.pcx"
#define RAHMENBILD1_PCX            "../graphics/rahmen.pcx"
#define PARAPLUSRAHMENBILD_PCX     "../graphics/plusrah1.pcx"
#define BLASTBILD_PCX              "../graphics/blast.pcx"
#define BULLETBILD_PCX             "../graphics/bullet.pcx"
#define INFLUENCEBILD_PCX          "../graphics/influ.pcx"
#define DIGITBILD_PCX              "../graphics/digits.pcx"
#define ENEMYBILD_PCX              "../graphics/enemy.pcx"
#define SEITENANSICHTBILD_PCX      "../graphics/ship.pcx"
#define EL_BLOCKS_FILE_PCX         "../graphics/ship2.pcx"
#define FONTBILD_PCX               "../graphics/parafont.pcx"
#define CONSOLENBILD_PCX           "../graphics/console.pcx"
#define ROBOTPICTUREBILD_PCX       "../graphics/robots.pcx"
#define SHIELDPICTUREBILD_PCX      "../graphics/shield.pcx"
/* Takeover-game graphics elements: */
#define ELEMENTS_FILE_PCX	   "../graphics/to_elem.pcx"

#define DATA70ZEICHENSATZ          "../graphics/para8x8.fnt"
#define SHIPNAME                   "../map/ship1"
#define COLORFILE                  "../map/levels.col"

/* Konstanten die unmittelbar die Hardware betreffen */
#define SCREENADDRESS		0xa000	/* screen - data */
#define RETRACEWARTEPERIODE 	5
#define SCREENBREITE 		320
#define SCREENHOEHE 		200
#define SCREENLEN		320
#define SCREENHEIGHT		200

#define DRUIDIMAGE_LENGTH       66
#define DRUIDIMAGE_HEIGHT       90

/* Dimensionen von Bloecken und Bildern */

/* Rahmen */
#define RAHMENBREITE		SCREENBREITE
#define RAHMENHOEHE		32

/* Menu in Konsole */
#define MENUITEMHEIGHT 		77
#define MENUITEMLENGTH 		50
#define MENUITEMMEM 		(MENUITEMLENGTH * MENUITEMHEIGHT)

/* Ship-Elevator Picture */
#define NUM_EL_BLOCKS		17

#define EL_BLOCK_LEN		8
#define EL_BLOCK_HEIGHT		8
#define EL_BLOCK_MEM 		EL_BLOCK_LEN * EL_BLOCK_HEIGHT

#define ELEVATOR_LEN		38
#define ELEVATOR_HEIGHT		12

/* Konstanten die die Ausmasse von maps und Arrays beeinflussen */
#define BLOCKBREITE 		32
#define BLOCKHOEHE 		32
#define BLOCKMEM  		BLOCKBREITE*BLOCKHOEHE


#define VIEWBREITE 		9
#define VIEWHOEHE 		4

#define INTERNBREITE 		13
#define INTERNHOEHE 		7

/* Diese Konstanten geben die Groesse des unsichtbaren Bereiches links,rechts
	und oberhalb des Userfensters an. */
// #define USERFENSTERLINKS ((INTERNBREITE-VIEWBREITE)*BLOCKBREITE/2) // (BLOCKBREITE/2+10)  /* 32 */  /* 32+16 */
// #define USERFENSTEROBEN BLOCKHOEHE/2  /* 25 */
//#define USERFENSTERUNTEN USERFENSTEROBEN+USERFENSTERHOEHE
//#define USERFENSTERRECHTS (BLOCKBREITE/2+10) /* 32 */  /* 32+16 */
//#define USERFENSTERBREITE ((INTERNBREITE-1)*BLOCKBREITE-20)  /* (288-32) */
//#define USERFENSTERHOEHE ((INTERNHOEHE-1)*BLOCKHOEHE)     /* 112 */

#define USERFENSTERHOEHE 	VIEWHOEHE*BLOCKHOEHE
#define USERFENSTERBREITE 	VIEWBREITE*BLOCKBREITE

#define USERFENSTERPOSX 	( (SCREENBREITE-USERFENSTERBREITE) / 2)
#define USERFENSTERPOSY 	( (SCREENHOEHE-USERFENSTERHOEHE) )

//#define USERFENSTEROBEN       BLOCKHOEHE+BLOCKHOEHE/2
//#define USERFENSTERLINKS      1*BLOCKBREITE

#define BULLETSPEEDINFLUENCE 	2

#define DIRECTIONS 		8

#define ALLCLASSNAMES 		11
#define ALLSHIPS 		4

#define NUMBEROFS 		5+12*BLOCKBREITE
#define ENEMYPHASES 		8

#define WAIT_LEVELEMPTY		18	/* warte bevor Graufaerben */
#define WAIT_AFTER_KILLED	45	/* warte, bevor Spiel aus */
#define WAIT_SHIPEMPTY		20
#define WAIT_TRANSFERMODE	5	/* warte, bevor in Transfermode schalten */

/* Soundblasterkonstanten */
#define SBCHANNELS 		7

/* Nummern der Schilder */
enum _shields
{
  SHIELD0 = 0,
  SHIELD1,
  SHIELD2,
  SHIELD3
};

/* Richtungsdefinitionen (fuer die Einstellung der Schussrichtung) */
enum _directions
{
  OBEN = 0,
  RECHTSOBEN,
  RECHTS,
  RECHTSUNTEN,
  UNTEN,
  LINKSUNTEN,
  LINKS,
  LINKSOBEN,
  CENTER,
  LIGHT				/* special: checking passability for light, not for a checkpos */
};

/* Koordinaten der Bloecke die isoliert werden sollen */
#define INFLUENCEBLOCKPOSITIONX 0
#define INFLUENCEBLOCKPOSITIONY 0	/* BLOCKHOEHE */
#define BULLETBLOCKPOSITIONY 0	/* (BLOCKHOEHE*4) */
#define BLASTBLOCKPOSITIONY 0	/* (BLOCKHOEHE*3) */

// #define STARTENERGIE 100
#define ENERGIEBALKENBREITE 15
#define MAXENERGIEBALKENLAENGE 160
#define ENERGIEPOSY 180

#define CORNERHOEHE 3
#define CORNERBREITE 3
#define CORNERRECHTS 22
#define CORNERLINKS 10
#define CORNEROBEN 8
#define CORNERUNTEN 18

/* Maximale Anzahl von ... */

#define BLOCKANZAHL 43

#define ALLDRUIDTYPES		24	/* number of druid-models that exist */
#define ALLBULLETTYPES		4	/* number of bullet-types */
#define ALLBLASTTYPES		2	/* number of different exposions */

#define MAXBULLETS		10	/* maximum possible Bullets in the air */
#define MAXBLASTS			5	/* max. possible Blasts visible */
#define AGGRESSIONMAX  100

/* Map-related defines:
	WARNING leave them here, they are required in struct.h
*/
#define MAX_WP_CONNECTIONS 4
#define MAXMAPLINES 50
#define ALLLEVELS			18
#define MAX_LEVELS_ON_SHIP		20

#define MAX_ENEMYS_ON_SHIP		1000

#define ALLELEVATORS		30
#define MAXWAYPOINTS 30
#define MAX_DOORS_ON_LEVEL 20
#define MAX_REFRESHES_ON_LEVEL	10

#define WAIT_COLLISION		9	/* enemy bleibt nach coll. stehen */

/* Macros */
#define GrobX (Me.pos.x / BLOCKBREITE)
#define GrobY (Me.pos.y / BLOCKHOEHE)
#define FeinX (Me.pos.x%BLOCKBREITE)
#define FeinY (Me.pos.y%BLOCKHOEHE)

#define CLFeinY ((Me.pos.y+BLOCKHOEHE/2) % BLOCKHOEHE)
#define CLGrobY ((Me.pos.y+BLOCKHOEHE/2) / BLOCKHOEHE)
#define CLFeinX ((Me.pos.x+BLOCKBREITE/2) % BLOCKHOEHE)
#define CLGrobX ((Me.pos.x+BLOCKBREITE/2) / BLOCKHOEHE)

#define BREMSDREHUNG 3		/* warte 3*, bevor Influencer weitergedreht wird */

/* Wegstossgeschw. von Tueren u.ae. */
#define PUSHSPEED 2

/* Schusstypen */
enum _bullets
{
  PULSE,
  SINGLE_PULSE,
  MILITARY,
  FLASH
};


/* Explosionstypen */
enum _explosions
{
  BULLETBLAST,
  DRUIDBLAST,
  OWNBLAST
};

#define BLINKENERGY 25

/* Druidtypen */
enum _druids
{
  DRUID001 = 0,			/* You will know why are the numbers there, when you */
  DRUID123 = 1,			/* enter the crew of a level !! */
  DRUID139 = 2,
  DRUID247 = 3,
  DRUID249 = 4,
  DRUID296 = 5,
  DRUID302 = 6,
  DRUID329 = 7,
  DRUID420 = 8,
  DRUID476 = 9,
  DRUID493 = 10,
  DRUID516 = 11,
  DRUID571 = 12,
  DRUID598 = 13,
  DRUID614 = 14,
  DRUID615 = 15,
  DRUID629 = 16,
  DRUID711 = 17,
  DRUID742 = 18,
  DRUID751 = 19,
  DRUID821 = 20,
  DRUID834 = 21,
  DRUID883 = 22,
  DRUID999 = 23,
  DEBUG_ENEMY = 24
};

/* Status- Werte der Druids */
enum _status
{
  MOBILE,
  TRANSFERMODE,
  WEAPON,
  CAPTURED,
  COMPLETE,
  REJECTED,
  CONSOLE,
  DEBRIEFING,
  TERMINATED,
  PAUSE,
  CHEESE
};


/* Dimensionen der Druids (fuer NotPassable() and CheckEnemyCollsion() ) */
#define DRUIDRADIUSX		10
#define DRUIDRADIUSY		10
#define DRUIDRADIUSXY	7

/* Dimension eines Blasts */
#define BLASTRADIUS		BLOCKBREITE/3
#define BLASTDAMAGE		5
#define BLASTPHASES_PER_SECOND  15

#define DECKCOMPLETEBONUS 500

/* Tastaturgeschwindigkeiten */
#define TYPEMATIC_SLOW 127
#define TYPEMATIC_FAST 0
#define TYPEMATIC_TAKEOVER 92	/* For the takeover-game */

#define OUT TERMINATED		/* Type fuer inaktive Dinge */

#endif
