#include "colodefs.h"

char Text1[]="\
Clear the freighter of Robots by\n\
destroying them with twin lasers\n\
or by transferring control to them.\n\
\n\
Control is by Joystick and also\n\
by Keyboard as follows:\n\
\n\
Any time Joystick moves and holding\n\
firebutton down will allow use of\n\
lifts and consoles. Diese Nachricht\n\
ist noch nicht lang genug, und daher\n\
schreibe ich jetzt ein Par Extrazeilen, die, wenn \n\
sie nicht mehr am Bildschirm\n\
zu sehen sind von der Textausgaberoutine\n\
nicht mehr angezeigt werden.";

char TitleText1[]="\
 A fleet of Robo-freighters on\n\
its way to the Beta Ceti system\n\
reported entering an uncharted field\n\
of asteroids. Each ship carries a\n\
cargo of battle droids to reinforce\n\
the outworld defences.\n\
 Two distress beacons have been\n\
discovered. Similar Messages were\n\
stored on each. The ships had been\n\
bombarded by a powerful radionic\n\
beam from one of the asteroids.\n\
All of the robots on the ships,\n\
including those in storage, became\n\
hyper-active. The crews report an\n\
attack by droids, isolating them on\n\
the bridge. They cannot reach the\n\
shuttle and can hold out for only a\n\
couple more hours.\n\
 Since these beacons were located\n\
two days ago, we can only fear the\n\
worst.\n\
 Some of the fleet was last seen\n\
heading for enemy space. In enemy\n\
hands the droids can be used against\n\
our forces.\n\
Docking would be impossible but\n\
we can beam aboard a prototype\n\
Influence Device.";


char TitleText2[]="\
The 001 Influence Device consists\n\
of a helmet, which, when placed\n\
over a robots control unit can halt\n\
the normal activities of that robot\n\
for a short time. The helmet has\n\
its own energy supply and powers\n\
the robot itself, at an upgraded\n\
capability. The helmet also uses\n\
an energy cloak for protection of\n\
the host.\n\
The helmet is fitted with twin\n\
lasers mounted in a turret. These\n\
can be focussed on any target inside\n\
a range of eight metres.\n\
Most of the device's resources are\n\
channelled towards holding control\n\
of the host robot, as it attempts\n\
to resume 'normal' operation.\n\
It is therefore necessary to change\n\
the host robot often to prevent the\n\
device from burning out. Transfer\n\
to a new robot requires the device\n\
to drain its host of energy in order\n\
to take ist over. Failure to achieve\n\
transfer results in the device being\n\
a free agent once more.\n\
\n\
        Press fire to play";

char TitleText3[]="\
An Influence Device can transmitt\n\
\
\
console. A small-scale plan of the\n\
whole deck is available, as well\n\
as a side elevation of the ship.\n\
Robots are represented on-screen\n\
as a symbol showing a three-digit\n\
number. The first digit shown is\n\
the important one, the class of the\n\
robot. It denotes the strength also.\n\
To find out more about any given\n\
robot, use the robot enquiry system\n\
at a console. Only data about units\n\
of a lower class than your current\n\
host is available, since it is the\n\
host's security clearance which is\n\
used to acces the console. \n\
  \n\
  \n\
  \n\
        Press fire to play";
      
int Shieldcolors[BLOCKBREITE];

int ShipEmptyCounter = 0;		/* counter to Message: you have won(this ship*/
int TimerFlag = FALSE;		/* the internal timer */
	
influence_t Me = {
DRUID001, TRANSFERMODE, {0, 0}, {120, 48}, STARTENERGIE, STARTENERGIE, 0, 0
};

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
	NULL
};


char *Classname[ALLCLASSNAMES]={
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

char* Classes[]={
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

char *Shipnames[ALLSHIPS]={
	"Paradroid",
	"Metahawk",
	"Graftgold",
	NULL
};

char *Decknames[]={
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

char *Alertcolor[ALLALERTCOLORS]={
	"green",
	"yellow",
	"red"
};

char* Drivenames[]={
	"none",
	"tracks",
	"anti-grav",
	"tripedal",
	"wheels",
	"bipedal",
	"error"
};

char* Sensornames[]={
	" - ",
	"spectral",
	"infra-red",
	"subsonic",
	"ultra-sonic",
	"radar",
	"error"
};

char* Brainnames[]={
	"none",
	"neutronic",
	"primode",
	"error"
};

char* Weaponnames[]={
	"none",
	"lasers",
	"laser rifle",
	"disruptor",
	"exterminator",
	"error"
};

// Robotnummern                 001     123     139     247     249     296     302     329     420     476     493     516     571     598     614     615     629     711     742     751     821     834     883     999
char* Entry[ALLDRUIDTYPES]= {   "01",   "02",   "03",   "04",   "05",   "06",   "07",   "08",   "09",   "10",   "11",   "12",   "13",   "14",   "15",   "16",   "17",   "18",   "19",   "20",   "21",   "22",   "23",   "24" };
char* Height[ALLDRUIDTYPES]={ "1.00", "1.37", "1.22", "1.56", "1.63", "1.20", "1.07", "1.07", "1.41", "1.32", "1.48", "1,57", "1.76", "1.72", "1.93", "1.20", "1.09", "1.93", "1.87", "1.93", "1.00", "1.10", "1.62", "1.87" };
char* Weight[ALLDRUIDTYPES]={  "027",  "085",  "061",  "078",  "083",  "047",  "023",  "031",  "057",  "042",  "051",  "074",  "062",  "093",  "121",  "029",  "059",  "102",  "140",  "227",  "028",  "034",  "079",  "162" };
int Drive[ALLDRUIDTYPES] =  {     0,      1,      2,      2,      3,      1,      2,      4,      1,      2,      2,      5,      5,      5,      5,      2,      1,      5,      5,      5,      2,      2,      4,      2  };
int Brain[ALLDRUIDTYPES] =  {     0,      0,      0,      1,      1,      1,      0,      0,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      2  };
int Armament[ALLDRUIDTYPES]={     0,      0,      0,      0,      0,      0,      0,      0,      0,      1,      0,      0,      0,      0,      2,      1,      1,      3,      3,      1,      1,      1,      4,      1  };
int Sensor1[ALLDRUIDTYPES] ={     0,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      1,      4,      1,      1,      1,      1,      1,      2  };
int Sensor2[ALLDRUIDTYPES] ={     0,      2,      0,      0,      0,      0,      0,      0,      0,      2,      0,      0,      0,      0,      3,      2,      3,      5,      5,      0,      5,      5,      5,      5  };
int Sensor3[ALLDRUIDTYPES] ={     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      2,      0,      0,      3  };


FCU AllFCUs[]={
	{"none"},
	{"SimpleA"},
	{"Battle1234AD"}
};

shieldspec AllShields[]={
	{"none"},
	{"SimpleF"},
	{"SimpleR"},
	{"RefF"},
	{"RefR"},
	{"Full"}
};


druidspec Druidmap[ALLDRUIDTYPES]={
/*
name maxspeed cl. accel. energy lose_health gun vneut. aggr.  firewait fl.imm. score notes
---------------------------------------------------------------------- */
{"001", 	5,0,	 	1, 	165, 		2, 	PULSE,1,			00,	0,	0,   0,NULL },
{"123",  2,1,   	1,		 20,		2,		PULSE,1,			00,	0,	0,  25,NULL },
{"139",  2,1,   	1,		 20,		2,		PULSE,1,			00,	0,	0,  25,NULL },
{"247",  4,2,   	1,		 20,		2,		PULSE,1,			00,	0,	0,	 50,NULL },
{"249",  4,2,   	1,		 20,		2,		PULSE,1,			00,	0,	0,	 50,NULL },
{"296",  2,2,   	1,		 40,		2,		PULSE,1,			00,	0,	0,  50,NULL },
{"302", 	7,3, 		2, 	 40, 		2, 	PULSE,1,			00,	0,	0,	 75,NULL },
{"329",  1,3,		1,		 40,		2,		PULSE,1,			00,	0,	0,	 75,NULL },
{"420",	2,4,		1,		 60,		2,		PULSE,1,			00,	0,	1,	100,NULL },
{"476", 	4,4, 		1, 	 70, 		2, 	SINGLE_PULSE,2,65,	6,	0,	100,NULL },
{"493",  4,4,		1,		 80,		3,		PULSE,2,			00,	0,	0,	100,NULL },
{"516",	3,5,		1,		 80,		3,		PULSE,2,			00,	0,	0,	125,NULL },
{"571",	4,5,		1,		 80,		3,		PULSE,2,			00,	0,	0,	125,NULL },
{"598",	3,5,		1,		 80,		3,		PULSE,2,			00,	0,	0,	125,NULL },
{"614",	3,6,		1,		100,		3,		SINGLE_PULSE,2,50,	6,	0,	150,NULL },
{"615",	4,6,		1,		100,		3,		SINGLE_PULSE,2,50,	6,	0,	150,NULL },
{"629",	2,6,		1,		100,		3,		MILITARY,2,		60,	6,	0,	150,NULL },
{"711",	5,7,		1,		110,		3,		FLASH,2,			30,	5,	1,	175,NULL },
{"742",	4,7,		1,		130,		3,		FLASH,2,			30,	5,	1,	175,NULL },
{"751",	5,7,		1,		150,		3,		SINGLE_PULSE,2,60,	5,	0,	175,NULL },
{"821", 	6,8, 		2, 	160, 		3, 	MILITARY,2,		70,	3,	1,	200,NULL },
{"834", 	7,8, 		1, 	140, 		3, 	SINGLE_PULSE,2,70,	5,	0,	200,NULL },
{"883", 	2,8, 		1, 	180, 		3, 	SINGLE_PULSE,2,80,	3,	0,	200,NULL },
{"999", 	6,9, 		2, 	300, 		5, 	MILITARY,2,		60,	3,	1,	225,NULL }
};


bulletspec Bulletmap[ALLBULLETTYPES]={
	
/*  speed damage	salve	phases	blast			pic.	oneshotonly
------------------------------------------------------------ */
	{ 10,	 	25, 		1,  4, 	BULLETBLAST,	NULL ,	FALSE, 34},	/* 01: PULSE */
	{ 10,		40,		1,	 4,	BULLETBLAST,	NULL ,	FALSE, 34},	/* single_pulse */
	{ 10,		55,		1,	 4,	BULLETBLAST,	NULL ,	FALSE, 34},  /* military */
	{ 10,		30,		1,	 3,	BULLETBLAST,	NULL ,	FALSE, 44}   /* flash */  
};

blastspec Blastmap[ALLBLASTTYPES]={
/*  phases		picpointer */
	{ 6, 			NULL},			/* BULLETBLAST */
	{ 9,			NULL}				/* DRUIDBLAST */
};

