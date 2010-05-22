/*
 *
 *   Copyright (c) 1994, 2002, 2003  Johannes Prix
 *   Copyright (c) 1994, 2002, 2003  Reinhard Prix
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


/*
 *  _Definitions_ of global variables
 * This file should only be included in main.c, and
 * the variable _declarations_ should be made in global.h under _main_c
 *
 */
SDL_Rect OrigBlock_Rect = {0, 0, 64, 64};      // not to be rescaled ever!!
SDL_Rect Block_Rect = {0, 0, 64, 64};
SDL_Rect Screen_Rect = {0, 0, 640, 480};
SDL_Rect User_Rect = {0, 0, 0, 0};
SDL_Rect Classic_User_Rect = {32, 150, 9*64, 4*64};
SDL_Rect Full_User_Rect = {0, 64, 640, 480 - 64};
SDL_Rect Banner_Rect = {0, 0, 640, 64 };
SDL_Rect Portrait_Rect = {0, 0, 132, 180};  // for droid-pic display in console
SDL_Rect Cons_Droid_Rect = {30, 190, 132, 180};

SDL_Rect Menu_Rect = {2*64, 150, 640 - 3*64, 480 - 64};
SDL_Rect OptionsMenu_Rect = {232, 0, 0, 0};

SDL_Rect OrigDigit_Rect = {0, 0, 16, 18};  	 // not to be rescaled!
SDL_Rect Digit_Rect = {0, 0, 16, 18};

SDL_Rect Cons_Header_Rect = {75, 64+40, 640 - 80, 135 - 64};
SDL_Rect Cons_Menu_Rect = {60, 180, 100, 256};
SDL_Rect Cons_Text_Rect = {180, 180, 640-185, 480 - 185};
SDL_Rect Cons_Menu_Rects[4] = {
  {60, 180 + 0*64, 100, 62},
  {60, 181 + 1*64, 100, 62},
  {60, 181 + 2*64, 100, 62},
  {60, 181 + 3*64, 100, 62} };

// Startpos + dimensions of Banner-Texts
SDL_Rect LeftInfo_Rect = { 26, 44, 0, 0 };
SDL_Rect RightInfo_Rect ={484, 44, 0, 0 };

SDL_Rect ProgressMeter_Rect = {0, 0, 640, 480};
SDL_Rect ProgressBar_Rect = {446, 155, 22, 111};
SDL_Rect ProgressText_Rect = {213, 390, 157, 30};

int ShipEmptyCounter = 0;	/* counter to Message: you have won(this ship */

influence_t Me = {
  DRUID001, TRANSFERMODE, {0, 0}, {120, 48}, 100, 100, 0, 0, 0, 0, 0, 0, NULL };

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
  "Victory",
  "Activate",
  "-- OUT --",
  NULL
};


char *Classname[] = {
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

char *Alertcolor[AL_LAST] = {
  "green",
  "yellow",
  "amber",
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

char *Weaponnames[] = {      // Bullet-names:
  "none",                    // pulse
  "lasers",                  // single
  "lasers",                  // Military
  "disruptor",               // flash
  "exterminator",            // exterminator
  "laser rifle",             // laser-rifle
  "error"
};


Druidspec Druidmap;

Bulletspec Bulletmap;

blastspec Blastmap[ALLBLASTTYPES];



