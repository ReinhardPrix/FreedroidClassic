/* 
 *  _Definitions_ of global variables
 * This file should only be included in main.c, and 
 * the variable _declarations_ should be made in global.h under _main_c
 *
 */

/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */

#include "colodefs.h"
#include "ship.h"
// this defines our "Userfenster"
const SDL_Rect User_Rect = { USERFENSTERPOSX, USERFENSTERPOSY,
		       USERFENSTERBREITE, USERFENSTERHOEHE };

/*
const SDL_Rect Menu_Rect = {MENUTEXT_X, USERFENSTERPOSY,
			USERFENSTERPOSX + USERFENSTERBREITE,
			USERFENSTERPOSY + USERFENSTERHOEHE};
*/

const SDL_Rect Menu_Rect = {MENUTEXT_X, USERFENSTERPOSY,
			USERFENSTERBREITE - MENUTEXT_X + USERFENSTERPOSX ,
			USERFENSTERPOSY + USERFENSTERHOEHE};

int Shieldcolors[INITIAL_BLOCK_WIDTH];

int ShipEmptyCounter = 0;	/* counter to Message: you have won(this ship */

influence_t Me = {
  DRUID001, TRANSFERMODE, {0, 0}, {120, 48}, 100, 100, 0, 0
};

/*
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
  CHEESE,
  ELEVATOR,
  BRIEFING
};
*/

char *InfluenceModeNames[] = {
  "Mobile",
  "Transfer",
  "Weapon",
  "Captured",
  "Complete",
  "Rejected",
  "Logged In",
  "Debriefing",
  "Terminated",
  "Pause",
  "Cheese",
  "Elevator",
  "Briefing",
  "Menu",
  NULL
};


char *Classname[ALLCLASSNAMES] = {
  "Influence device",
  "Disposal robot",
  "Servant robot",
  "Messenger robot",
  "Maintenance robot",
  "Crew droid",
  "Sentinel droid",
  "Battle droid",
  "Security droid",
  "Command Cyborg",
  NULL
};

char *Classes[] = {
  "influence",
  "disposal",
  "servant",
  "messenger",
  "maintenance",
  "crew",
  "sentinel",
  "battle",
  "security",
  "command",
  "error"
};

char *Shipnames[ALLSHIPS] = {
  "Paradroid",
  "Metahawk",
  "Graftgold",
  NULL
};

char *Decknames[] = {
  "maintenance",
  "engeneering",
  "robostores",
  "quarterd",
  "repairs",
  "staterooms",
  "stores",
  "research",
  "bridge",
  "observation",
  "airlock",
  "reactor",
  "upper cargo",
  "mid carga",
  "vehicle hold",
  "shuttle bay",
  NULL
};

char *Alertcolor[ALLALERTCOLORS] = {
  "green",
  "yellow",
  "red"
};

char *Drivenames[] = {
  "none",
  "tracks",
  "anti-grav",
  "tripedal",
  "wheels",
  "bipedal",
  "error"
};

char *Sensornames[] = {
  " - ",
  "spectral",
  "infra-red",
  "subsonic",
  "ultra-sonic",
  "radar",
  "error"
};

char *Brainnames[] = {
  "none",
  "neutronic",
  "primode",
  "error"
};

char *Weaponnames[] = {
  "none",
  "lasers",
  "laser rifle",
  "disruptor",
  "exterminator",
  "error"
};

// Robotnummern                 001     123     139     247     249     296     302     329     420     476     493     516     571     598     614     615     629     711     742     751     821     834     883     999

FCU AllFCUs[] = {
  {"none"},
  {"SimpleA"},
  {"Battle1234AD"}
};

Druidspec Druidmap;

#define BULLET_SPEED_CALIBRATOR (1)
bulletspec Bulletmap[ALLBULLETTYPES] = {
/*  
  speed                                      blast                 oneshotonly
                              damage phases	                pic.	
------------------------------------------------------------*/
  { 1.0, 10 * BULLET_SPEED_CALIBRATOR, 25                            , 4, BULLETBLAST, NULL, FALSE, 34, NULL},
  { 1.0, 10 * BULLET_SPEED_CALIBRATOR, 40                            , 4, BULLETBLAST, NULL, FALSE, 34, NULL},
  { 1.0, 10 * BULLET_SPEED_CALIBRATOR, 55                            , 4, BULLETBLAST, NULL, FALSE, 34, NULL},
  { 1.0, 10 * BULLET_SPEED_CALIBRATOR, (60/FLASH_DURATION_IN_FRAMES) , 3, BULLETBLAST, NULL, FALSE, 44, NULL}	/* flash */
};

blastspec Blastmap[ALLBLASTTYPES] = {
/*  phases		picpointer */
  {6, NULL},			/* BULLETBLAST */
  {9, NULL}			/* DRUIDBLAST */
};


