#include "colodefs.h"

char Text1[]="Clear the freighter of Robots by\ndestroying them with twin lasers\nor by transferring control to them.\n\nControl is by Joystick and also\nby Keyboard as follows:\n\nAny time Joystick moves and holding\nfirebutton down will allow use of\nlifts and consoles. Diese Nachricht\nist noch nicht lang genug, und daher\nschreibe ich jetzt ein Par Extrazeilen, die, wenn \nsie nicht mehr am Bildschirm\nzu sehen sind von der Textausgaberoutine\nnicht mehr angezeigt werden.";

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
                 accel.  lose_health          aggr.  fl.imm.
name maxspeed cl.      energy     gun      vneut.  firewait score notes
---------------------------------------------------------------------- */
{"001", 30*5,  0, 1*30, 165, 2,  PULSE,       1, 00,  0,  0,   0, NULL },
{"123", 30*2,  1, 1*30,  20, 2,  PULSE,       1, 00,  0,  0,  25, NULL },
{"139", 30*2,  1, 1*30,  20, 2,  PULSE,       1, 00,  0,  0,  25, NULL },
{"247", 30*4,  2, 1*30,  20, 2,  PULSE,       1, 00,  0,  0,  50, NULL },
{"249", 30*4,  2, 1*30,  20, 2,  PULSE,       1, 00,  0,  0,  50, NULL },
{"296", 30*2,  2, 1*30,  40, 2,  PULSE,       1, 00,  0,  0,  50, NULL },
{"302", 30*7,  3, 2*30,  40, 2,  PULSE,       1, 00,  0,  0,  75, NULL },
{"329", 30*1,  3, 1*30,  40, 2,  PULSE,       1, 00,  0,  0,  75, NULL },
{"420",	30*2,  4, 1*30,  60, 2,  PULSE,       1, 00,  0,  1, 100, NULL },
{"476", 30*4,  4, 1*30,  70, 2, SINGLE_PULSE, 2, 65,  6,  0, 100, NULL },
{"493", 30*4,  4, 1*30,  80, 3,  PULSE,       2, 00,  0,  0, 100, NULL },
{"516",	30*3,  5, 1*30,  80, 3,  PULSE,       2, 00,  0,  0, 125, NULL },
{"571",	30*4,  5, 1*30,  80, 3,  PULSE,       2, 00,  0,  0, 125, NULL },
{"598",	30*3,  5, 1*30,  80, 3,  PULSE,       2, 00,  0,  0, 125, NULL },
{"614",	30*3,  6, 1*30, 100, 3, SINGLE_PULSE, 2, 50,  6,  0, 150, NULL },
{"615",	30*4,  6, 1*30, 100, 3, SINGLE_PULSE, 2, 50,  6,  0, 150, NULL },
{"629",	30*2,  6, 1*30, 100, 3, MILITARY,     2, 60,  6,  0, 150, NULL },
{"711",	30*5,  7, 1*30, 110, 3,  FLASH,       2, 30,  5,  1, 175, NULL },
{"742",	30*4,  7, 1*30, 130, 3,  FLASH,       2, 30,  5,  1, 175, NULL },
{"751",	30*5,  7, 1*30, 150, 3, SINGLE_PULSE, 2, 60,  5,  0, 175, NULL },
{"821", 30*6,  8, 2*30, 160, 3,  MILITARY,    2, 70,  3,  1, 200, NULL },
{"834", 30*7,  8, 1*30, 140, 3, SINGLE_PULSE, 2, 70,  5,  0, 200, NULL },
{"883", 30*2,  8, 1*30, 180, 3, SINGLE_PULSE, 2, 80,  3,  0, 200, NULL },
{"999", 30*6,  9, 2*30, 300, 5,  MILITARY,    2, 60,  3,  1, 225, NULL }
};


bulletspec Bulletmap[ALLBULLETTYPES]={
	
/*  
speed     salve         blast                 oneshotonly
     damage 	phases	                pic.	
------------------------------------------------------------*/
{ 10*35,  25,    1,  4, 	BULLETBLAST,	NULL ,	FALSE, 34},	/* 01: PULSE */
{ 10*35,  40,    1,  4,	BULLETBLAST,	NULL ,	FALSE, 34},	/* single_pulse */
{ 10*35,  55,    1,  4,	BULLETBLAST,	NULL ,	FALSE, 34},     /* military */
{ 10*35,  30,    1,  3,	BULLETBLAST,	NULL ,	FALSE, 44}      /* flash */  
};

blastspec Blastmap[ALLBLASTTYPES]={
/*  phases		picpointer */
	{ 6, 			NULL},			/* BULLETBLAST */
	{ 9,			NULL}				/* DRUIDBLAST */
};

