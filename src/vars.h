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

const SDL_Rect Menu_Rect = {MENUTEXT_X, USERFENSTERPOSY,
			USERFENSTERPOSX + USERFENSTERBREITE,
			USERFENSTERPOSY + USERFENSTERHOEHE};


char EndTitleText1[] =
"Congratulations!!\n\nIt seems that you have made it!  The Ship is clear.\n\n At the moment, this is the end of Freedroid.  However we are looking forward to writing a new and different story line, introduce more new concepts, features and sounds.\n\n If you enjoyed the game and would like to contribute, contact one of the developers. \n\n Also if you would like to donate something to help keep alive the Freedroid development, please also contact the developers.\n\n  Since we have not yet written something new, we can not launch the second part of the game now.\n\n What we can do is inform you about the concept of the new story-line and the features we plan to introduce sooner or later:\n\n After this preview of the coming storyline is over, the game will be restarted.\n You however have made it, but if you want, you can restart from the beginning nevertheless.\n\n  Press Space Bar to\nrestart Freedroid from the beginning. \n \n \n ";

char EndTitleText2[] =
"In the year 2400 in a far distant galaxy strange civilisation has been enslaved by an all-mighty Software Corporation named 'MegaSoft' and called MS for short.  This came to be as follows:  At first all the druids of that civilisation were running a MS Operating System which turned out to be a trojan horse and led to the revolution and the enslavement of men.  By virtue of the tremendous wealth the Corporation had accumulated it was able to break all resistances and ban all other operating systems forever, using the druids with their MS Operating System to enforce the oppression with military strength.\n\n  However not all is yet lost:\nA small group of rebels has managed to create a new 'influence device' running the latest Linux 28.32.199 kernel.  The rebels will soon be spotted and can not hope to withstand an open attack.  The Influence device must be beamed aboard an unmanned transport ship that could evacuate the rebel group.  In the first episode, the rebel influence device is beamed aboard the robo freighter and must eliminate all of the robot crew running the MS operating system.  After this is done, the rebels can beam aboard and make their escape to some distant planet from where they can prepare their next steps to free the world from oppression.\n\n Press Space Bar to relauch the original Freedroid game \n \n \n ";

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


