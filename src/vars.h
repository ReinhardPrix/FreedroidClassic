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



char EndTitleText1[] =
"Congratulations!!\n\nIt seems that you have made it!  The Ship is clear.\n\n At the moment, this is the end of Freedroid.  However we are looking forward to writing a new and different story line, introduce more new concepts, features and sounds.\n\n If you enjoyed the game and would like to contribute, contact one of the developers. \n\n Also if you would like to donate something to help keep alive the Freedroid development, please also contact the developers.\n\n  Since we have not yet written something new, we can not launch the second part of the game now.\n\n What we can do is inform you about the concept of the new story-line and the features we plan to introduce sooner or later:\n\n After this preview of the coming storyline is over, the game will be restarted.\n You however have made it, but if you want, you can restart from the beginning nevertheless.\n\n  Press Space Bar to\nrestart Freedroid from the beginning. \n \n \n ";

char EndTitleText2[] =
"In the year 2400 in a far distant galaxy strange civilisation has been enslaved by an all-mighty Software Corporation named 'MegaSoft' and called MS for short.  This came to be as follows:  At first all the druids of that civilisation were running a MS Operating System which turned out to be a trojan horse and led to the revolution and the enslavement of men.  By virtue of the tremendous wealth the Corporation had accumulated it was able to break all resistances and ban all other operating systems forever, using the druids with their MS Operating System to enforce the oppression with military strength.\n\n  However not all is yet lost:\nA small group of rebels has managed to create a new 'influence device' running the latest Linux 28.32.199 kernel.  The rebels will soon be spotted and can not hope to withstand an open attack.  The Influence device must be beamed aboard an unmanned transport ship that could evacuate the rebel group.  In the first episode, the rebel influence device is beamed aboard the robo freighter and must eliminate all of the robot crew running the MS operating system.  After this is done, the rebels can beam aboard and make their escape to some distant planet from where they can prepare their next steps to free the world from oppression.\n\n Press Space Bar to relauch the original Freedroid game \n \n \n ";

char TitleText1[] =
  "A fleet of Robo-freighters on its way to the Beta Ceti system reported entering an uncharted field of asteroids. Each ship carries a cargo of battle droids to reinforce the outworld defences. Two distress beacons have been discovered. Similar Messages were stored on each. The ships had been bombarded by a powerful radionic beam from one of the asteroids. All of the robots on the ships, including those in storage, became hyper-active. The crews report an attack by droids, isolating them on the bridge. They cannot reach the shuttle and can hold out for only a couple more hours.  Since these beacons were located two days ago, we can only fear the worst.  Some of the fleet was last seen heading for enemy space. In enemy hands the droids can be used against our forces. Docking would be impossible but we can beam aboard a prototype Influence Device. \n \n ";


char TitleText2[] =
  "The 001 Influence Device consists of a helmet, which, when placed over a robots control unit can halt the normal activities of that robot for a short time. The helmet has its own energy supply and powers the robot itself, at an upgraded capability. The helmet also uses an energy cloak for protection of the host. The helmet is fitted with twin lasers mounted in a turret. These can be focussed on any target inside a range of eight metres. Most of the device's resources are channelled towards holding control of the host robot, as it attempts to resume 'normal' operation. It is therefore necessary to change the host robot often to prevent the device from burning out. Transfer to a new robot requires the device to drain its host of energy in order to take ist over. Failure to achieve transfer results in the device being a free agent once more.\n\n        Press space bar to skip instructions\n \n ";

char TitleText3[] =
  "An Influence Device can transmitt data to your console.  A small-scale plan of the whole deck is available, as well as a side elevation of the ship. Robots are represented on-screen as a symbol showing a three-digit number. The first digit shown is the important one, the class of the robot. It denotes the strength also. To find out more about any given robot, use the robot enquiry system at a console. Only data about units of a lower class than your current host is available, since it is the host's security clearance which is used to acces the console. \n  \n  \n  \n Press space bar to skip instructions\n\n\n";

char TitleText4[] =
  "Controls\n\
\n\
The game is controlled via keyboard input or via joystick input.\n\
The joystick control is similar to keyboard control.\n\
Therefore only keyboard control is expained here:\n\
\n\
Use cursor keys to move around.  The speed you can go \
depends on the druid you currently control.\n\
\n\
If you press space bar in -addition- to a cursor key, this fires \
the weapon of the druid you currently control.\n\n\
If you press space bar whilst NOT pressing a cursor key, \
this will enter transfer mode.  You will notice your robot to \
take on a flashy red color.  \
Now if you touch some other druid, this will initiate the takeover \
process in which you have to win a small game of logical curcuits \
within the given time.\n\
If you succeed, you thereafter can control this new droid and for game \
purposes, it is as if you were him.\n\
If you loose, you either are destroyed if you didn't control an enemy \
druid at that time, or the host you controlled is destroyed together with \
the unit you wished to control.\n\
\n\
The P key causes the game to pause.\n\
The S key causes a screenshot to be taken.\n\
The Escape key causes the menu to pop up.\n\
\n\
Watch out for energy refreshing fields and elevators.\n\
\n\
Elevators are also entered via transfer mode if you stand still on the elevaor.\n\
\n\
    Press Fire to Play\n \n \n \n \n \n";



char Text1[] =
  "Clear the freighter of Robots by\ndestroying them with twin lasers\nor by transferring control to them.\n\nControl is by Joystick and also\nby Keyboard as follows:\n\nAny time Joystick moves and holding\nfirebutton down will allow use of\nlifts and consoles. Diese Nachricht\nist noch nicht lang genug, und daher\nschreibe ich jetzt ein Par Extrazeilen, die, wenn \nsie nicht mehr am Bildschirm\nzu sehen sind von der Textausgaberoutine\nnicht mehr angezeigt werden.";

int Shieldcolors[Block_Width];

int ShipEmptyCounter = 0;	/* counter to Message: you have won(this ship */
int TimerFlag = FALSE;		/* the internal timer */

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
char *Entry[ALLDRUIDTYPES] =
  { "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12",
"13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24" };
char *Height[ALLDRUIDTYPES] =
  { "1.00", "1.37", "1.22", "1.56", "1.63", "1.20", "1.07", "1.07", "1.41",
"1.32", "1.48", "1,57", "1.76", "1.72", "1.93", "1.20", "1.09", "1.93", "1.87", "1.93", "1.00",
"1.10", "1.62", "1.87" };
char *Weight[ALLDRUIDTYPES] =
  { "027", "085", "061", "078", "083", "047", "023", "031", "057", "042",
"051", "074", "062", "093", "121", "029", "059", "102", "140", "227", "028", "034", "079", "162" };
int Drive[ALLDRUIDTYPES] =
  { 0, 1, 2, 2, 3, 1, 2, 4, 1, 2, 2, 5, 5, 5, 5, 2, 1, 5, 5, 5, 2, 2, 4, 2 };
int Brain[ALLDRUIDTYPES] =
  { 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 };
int Armament[ALLDRUIDTYPES] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 1, 1, 3, 3, 1, 1, 1, 4, 1 };
int Sensor1[ALLDRUIDTYPES] =
  { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 2 };
int Sensor2[ALLDRUIDTYPES] =
  { 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 3, 2, 3, 5, 5, 0, 5, 5, 5, 5 };
int Sensor3[ALLDRUIDTYPES] =
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3 };


FCU AllFCUs[] = {
  {"none"},
  {"SimpleA"},
  {"Battle1234AD"}
};


#define ROBOT_SPEED_CALIBRATOR (20*SCALE_FACTOR)
#define ROBOT_ACCEL_CALIBRATOR (10)
#define LOSE_HEALTH_CALIBRATOR 0.2
druidspec Druidmap[ALLDRUIDTYPES] = {
/*
variable lose_health:  How much energy to lose per second for the influencer if controlling
                       this host and how much energy to gain per second for the enemys of that type.
-------------------------------------------------------------------------------------------
    name        maxspeed             cl.        accel.              energy lose_health
-------------------------------------------------------------------------------------------
   gun  ,        vneut. aggr. 
------------------------------------------------------------------------------------------- */
  {"001", ROBOT_SPEED_CALIBRATOR * 5, 0, 1 * ROBOT_ACCEL_CALIBRATOR, 100, 1*LOSE_HEALTH_CALIBRATOR,
   PULSE,        1,     00,   0, 0, 0, NULL},
  {"123", ROBOT_SPEED_CALIBRATOR * 2, 1, 1 * ROBOT_ACCEL_CALIBRATOR,  20, 2*LOSE_HEALTH_CALIBRATOR,
   PULSE,        1,     00,   0, 0, 25, NULL},
  {"139", ROBOT_SPEED_CALIBRATOR * 2, 1, 1 * ROBOT_ACCEL_CALIBRATOR,  20, 2*LOSE_HEALTH_CALIBRATOR,
   PULSE,        1,     00,   0, 0, 25, NULL},
  {"247", ROBOT_SPEED_CALIBRATOR * 4, 2, 1 * ROBOT_ACCEL_CALIBRATOR,  20, 2*LOSE_HEALTH_CALIBRATOR,
   PULSE,        1,     00,   0, 0, 50, NULL},
  {"249", ROBOT_SPEED_CALIBRATOR * 4, 2, 1 * ROBOT_ACCEL_CALIBRATOR,  20, 2*LOSE_HEALTH_CALIBRATOR,
   PULSE,        1,     00, 0,    0, 50, NULL},
  {"296", ROBOT_SPEED_CALIBRATOR * 2, 2, 1 * ROBOT_ACCEL_CALIBRATOR,  40, 2*LOSE_HEALTH_CALIBRATOR,
   PULSE,        1,     00, 0,    0, 50, NULL},
  {"302", ROBOT_SPEED_CALIBRATOR * 7, 3, 2 * ROBOT_ACCEL_CALIBRATOR,  40, 2*LOSE_HEALTH_CALIBRATOR,
   PULSE,        1,     00, 0, 0, 75, NULL},
  {"329", ROBOT_SPEED_CALIBRATOR * 1, 3, 1 * ROBOT_ACCEL_CALIBRATOR,  40, 2*LOSE_HEALTH_CALIBRATOR,
   PULSE,        1,     00, 0, 0, 75, NULL},
  {"420", ROBOT_SPEED_CALIBRATOR * 2, 4, 1 * ROBOT_ACCEL_CALIBRATOR,  60, 2*LOSE_HEALTH_CALIBRATOR,
   PULSE,        1,     00, 0, 1, 100, NULL},
  {"476", ROBOT_SPEED_CALIBRATOR * 4, 4, 1 * ROBOT_ACCEL_CALIBRATOR,  70, 2*LOSE_HEALTH_CALIBRATOR,
   SINGLE_PULSE, 2,     65, 6, 0, 100, NULL},
  {"493", ROBOT_SPEED_CALIBRATOR * 4, 4, 1 * ROBOT_ACCEL_CALIBRATOR,  80, 3*LOSE_HEALTH_CALIBRATOR,
   PULSE,        2,     00, 0, 0, 100, NULL},
  {"516", ROBOT_SPEED_CALIBRATOR * 3, 5, 1 * ROBOT_ACCEL_CALIBRATOR,  80, 3*LOSE_HEALTH_CALIBRATOR,
   PULSE,        2,     00, 0, 0, 125, NULL},
  {"571", ROBOT_SPEED_CALIBRATOR * 4, 5, 1 * ROBOT_ACCEL_CALIBRATOR,  80, 3*LOSE_HEALTH_CALIBRATOR,
   PULSE,        2,     00, 0, 0, 125, NULL},
  {"598", ROBOT_SPEED_CALIBRATOR * 3, 5, 1 * ROBOT_ACCEL_CALIBRATOR,  80, 3*LOSE_HEALTH_CALIBRATOR,
   PULSE,        2,     00, 0, 0, 125, NULL},
  {"614", ROBOT_SPEED_CALIBRATOR * 3, 6, 1 * ROBOT_ACCEL_CALIBRATOR, 100, 3*LOSE_HEALTH_CALIBRATOR,
   SINGLE_PULSE, 2,     50, 6, 0, 150, NULL},
  {"615", ROBOT_SPEED_CALIBRATOR * 4, 6, 1 * ROBOT_ACCEL_CALIBRATOR, 100, 3*LOSE_HEALTH_CALIBRATOR,
   SINGLE_PULSE, 2,     50, 6, 0, 150, NULL},
  {"629", ROBOT_SPEED_CALIBRATOR * 2, 6, 1 * ROBOT_ACCEL_CALIBRATOR, 100, 3*LOSE_HEALTH_CALIBRATOR,
   MILITARY,     2,     60, 6, 0, 150, NULL},
  {"711", ROBOT_SPEED_CALIBRATOR * 5, 7, 1 * ROBOT_ACCEL_CALIBRATOR, 110, 3*LOSE_HEALTH_CALIBRATOR,
   FLASH,        2,     30, 5, 1, 175, NULL},
  {"742", ROBOT_SPEED_CALIBRATOR * 4, 7, 1 * ROBOT_ACCEL_CALIBRATOR, 130, 3*LOSE_HEALTH_CALIBRATOR,
   FLASH,        2,     30, 5, 1, 175, NULL},
  {"751", ROBOT_SPEED_CALIBRATOR * 5, 7, 1 * ROBOT_ACCEL_CALIBRATOR, 150, 3*LOSE_HEALTH_CALIBRATOR,
   SINGLE_PULSE, 2,     60, 5, 0, 175, NULL},
  {"821", ROBOT_SPEED_CALIBRATOR * 6, 8, 2 * ROBOT_ACCEL_CALIBRATOR, 160, 3*LOSE_HEALTH_CALIBRATOR,
   MILITARY,     2,     70, 3, 1, 200, NULL},
  {"834", ROBOT_SPEED_CALIBRATOR * 7, 8, 1 * ROBOT_ACCEL_CALIBRATOR, 140, 3*LOSE_HEALTH_CALIBRATOR,
   SINGLE_PULSE, 2,     70, 5, 0, 200, NULL},
  {"883", ROBOT_SPEED_CALIBRATOR * 2, 8, 1 * ROBOT_ACCEL_CALIBRATOR, 180, 3*LOSE_HEALTH_CALIBRATOR,
   SINGLE_PULSE, 2,     80, 3, 0, 200, NULL},
  {"999", ROBOT_SPEED_CALIBRATOR * 6, 9, 2 * ROBOT_ACCEL_CALIBRATOR, 300, 15*LOSE_HEALTH_CALIBRATOR,
   MILITARY,     2,     60, 3, 1, 225, NULL}
};



#define BULLET_SPEED_CALIBRATOR (ROBOT_SPEED_CALIBRATOR)
bulletspec Bulletmap[ALLBULLETTYPES] = {
/*  
  speed                             salve         blast                 oneshotonly
                              damage 	phases	                pic.	
------------------------------------------------------------*/
  {10 * BULLET_SPEED_CALIBRATOR, 25, 1, 4, BULLETBLAST, NULL, FALSE, 34, NULL},
  {10 * BULLET_SPEED_CALIBRATOR, 40, 1, 4, BULLETBLAST, NULL, FALSE, 34, NULL},
  {10 * BULLET_SPEED_CALIBRATOR, 55, 1, 4, BULLETBLAST, NULL, FALSE, 34, NULL},
  {10 * BULLET_SPEED_CALIBRATOR, (60/FLASH_DURATION_IN_FRAMES) , 1, 3, BULLETBLAST, NULL, FALSE, 44, NULL}	/* flash */
};

blastspec Blastmap[ALLBLASTTYPES] = {
/*  phases		picpointer */
  {6, NULL},			/* BULLETBLAST */
  {9, NULL}			/* DRUIDBLAST */
};


