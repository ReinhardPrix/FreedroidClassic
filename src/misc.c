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
 * Desc: miscellaeous helpful functions for paraplus
 *	 
 *----------------------------------------------------------------------*/
#include <config.h>

#define _misc_c

#undef DIAGONAL_KEYS_AUS
// #undef QUEUEDEBUG
#define QUEUEDEBUG

#define MESPOSX 0
#define MESPOSY 64
#define MESHOEHE 8
#define MESBARBREITE 320

#define MAX_MESSAGE_LEN 100
#define MESBAR_MEM	MESBARBREITE*MESHOEHE+1000

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>
#include <math.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

// Die Definition f"ur eine Message kann ruhig lokal hier stehen, da sie
// nur innerhalb des Moduls gebraucht wird.

typedef struct
{
  void *NextMessage;
  int MessageCreated;
  char *MessageText;
}
message, Message;

void CreateMessageBar (char *MText);
void CleanMessageLine (void);
void AdvanceQueue (void);

int VectsHaveBeenTurned = 0;
unsigned char *MessageBar;
message *Queue = NULL;

// int ThisMessageTime=0;               /* Counter fuer Message-Timing */

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

void
gotoxy (int x, int y)
{

};

void
DebugPrintf (char *Print_String)
{
  static int first_time = TRUE;
  FILE *debugfile;

  if (first_time)		/* make sure the first call deletes previous log-file */
    {
      debugfile = fopen ("debug.out", "w");
      first_time = FALSE;
    }
  else
    debugfile = fopen ("debug.out", "a");

  fprintf (debugfile, Print_String);
  fclose (debugfile);
};

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

int
MyRandom (int Obergrenze)
{
  float Zwisch;
  double ReinerZufall;
  int Endwert;

  DebugPrintf
    ("\nint MyRandom(int Obergrenze): real function call confirmed...");
  printf ("Obergrenze ist jetzt: %d.", Obergrenze);

  ReinerZufall = (double) rand ();
  printf ("\nint MyRandom(int Obergrenze): Reiner Zufall ist jetzt: %f.",
	  ReinerZufall);
  Zwisch = ((float) ReinerZufall) / ((float) RAND_MAX);
  printf ("\nint MyRandom(int Obergrenze): Zwisch ist jetzt: %f.", Zwisch);
  Zwisch = Zwisch * ((float) Obergrenze);
  Endwert = (unsigned int) rintf (Zwisch);


  printf ("\nint MyRandom(int Obergrenze): Endwert ist jetzt: %d.", Endwert);
  DebugPrintf ("\nint MyRandom(int Obergrenze): end of function reached...");

  return Endwert;
};

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

void
reverse (char s[])
{
  int c, i, j;
  for (i = 0, j = strlen (s) - 1; i < j; i++, j--)
    {
      c = s[i];
      s[i] = s[j];
      s[j] = c;
    }
}				// void reverse(char s[]) siehe Kernighan&Ritchie!

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

char *
itoa (int n, char s[], int Dummy)
{
  int i, sign;

  if ((sign = n) < 0)
    n = -n;
  i = 0;
  do
    {
      s[i++] = n % 10 + '0';
    }
  while ((n /= 10) > 0);
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse (s);
  return s;
}				// void itoa(int n, char s[]) siehe Kernighan&Ritchie!

// This Function is for the PORT!!!!
// Replacing all MyRandom-calls with MyMyRandom-calls

char *
ltoa (long n, char s[], int Dummy)
{
  int i, sign;

  if ((sign = n) < 0)
    n = -n;
  i = 0;
  do
    {
      s[i++] = n % 10 + '0';
    }
  while ((n /= 10) > 0);
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse (s);
  return s;
}				// void ltoa(long n, char s[]) angelehnt an itoa!

/* **********************************************************************
   Diese Funktion l"oscht alle Roboter auf dem momentanen Level
**********************************************************************/
void
Armageddon (void)
{
  int i;
  char ATaste = ' ';

  gl_printf (-1, -1, "\nKill all droids on ship (y/n) ?");
  while ((ATaste != 'y') && (ATaste != 'n'))
    ATaste = getchar ();
  if (ATaste == 'n')
    return;
  else
    for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
      {
	Feindesliste[i].energy = 0;
      }
}

/* **********************************************************************
	Diese Funktion teleportiert an einen beliebigen Ort im Schiff.
	Es werden Levelnummer und Grobkoordinaten "ubergeben.

	**********************************************************************/

void
Teleport (int LNum, int X, int Y)
{
  int curLevel = LNum;
  int array_num = 0;
  Level tmp;
  int i;

  /* Neuen Level und Position setzen */

  if (curLevel != CurLevel->levelnum)
    {				/* wirklich neu ??? */

      FadeLevel ();
      /* Aktuellen Level setzen */
      while ((tmp = curShip.AllLevels[array_num]) != NULL)
	{
	  if (tmp->levelnum == curLevel)
	    break;
	  else
	    array_num++;
	}

      CurLevel = curShip.AllLevels[array_num];

      /* Enemys gut verteilen: */
      ShuffleEnemys ();

      /* Position des Influencer richtig setzen */
      Me.pos.x = X * BLOCKBREITE + BLOCKBREITE / 2;
      Me.pos.y = Y * BLOCKHOEHE + BLOCKHOEHE / 2;

      /* Alle Blasts und Bullets loeschen */
      for (i = 0; i < MAXBLASTS; i++)
	AllBlasts[i].type = OUT;
      for (i = 0; i < MAXBULLETS; i++)
	{
	  AllBullets[i].type = OUT;
	  AllBullets[i].mine = FALSE;
	}
    }
  else
    {
      FadeLevel ();
      Me.pos.x = X * BLOCKBREITE + BLOCKBREITE / 2;
      Me.pos.y = Y * BLOCKHOEHE + BLOCKHOEHE / 2;
    }

  LeaveElevatorSound ();
  BeamLine = BLOCKBREITE / 2;
  UnfadeLevel ();
}				// void Teleport(...)

/* **********************************************************************
   Diese Funktion stellt ein praktisches Cheatmenu zur Verf"ugung
   Es sind geplant:
		* Robotvernichtung
		* Teleportationen nach Belieben
		* Belibige Typenauswahl
		* Ausgabe einer Gesamtrobotliste
   **********************************************************************/
void
Cheatmenu (void)
{
  char CTaste = ' ';
  // int vgamode;
  char NewRoboType[80];		// name of new influencer robot-type, i.e. "123"
  int Weiter = 0;
  int LNum, X, Y, i;
  int X0 = 20, Y0 = 5;		// startpos for gl_- text writing

  // return to normal keyboard operation
  keyboard_close ();

  //  gotoxy(1,1);
  //  vgamode = vga_getcurrentmode();
  //  vga_setmode(TEXT);

  // rp: try out the gl-textfunctions
  while (!Weiter)
    {
      vga_clear ();
      gl_setfont (8, 8, gl_font8x8);
      gl_setwritemode (FONT_COMPRESSED + WRITEMODE_OVERWRITE);
      gl_setfontcolors (0, vga_white ());

      gl_printf (X0, Y0, "*******************************\n");
      gl_printf (-1, -1, " ----  C H E A T M E N U  ----\n");
      gl_printf (-1, -1, "-------------------------------\n\n");
      gl_printf (-1, -1, " a. Armageddon (alle Robots sprengen)\n");
      gl_printf (-1, -1, " l. Robotliste von einem Deck\n");
      gl_printf (-1, -1, " g. Gesamtrobotliste\n");
      gl_printf (-1, -1, " r. Robotvernichtung dieses Deck\n");
      gl_printf (-1, -1, " t. Teleportation\n");
      gl_printf (-1, -1, " w. Wechsle Robo-typ\n\n");
      gl_printf (-1, -1, " i. Invinciblemode: %s\n",
		 InvincibleMode ? "ON" : "OFF");
      gl_printf (-1, -1, " v. Volle Energie\n");
      gl_printf (-1, -1, " b. Blinkenergie\n");
      gl_printf (-1, -1, " c. Conceptview: %s\n", Conceptview ? "ON" : "OFF");
      gl_printf (-1, -1, " m. Map von Deck xy\n");
      gl_printf (-1, -1, " s. Sound: %s\n", sound_on ? "ON" : "OFF");
      gl_printf (-1, -1, "\n q. RESUME game\n");

      CTaste = getchar ();
      switch (CTaste)
	{
	case 'a':
	  Weiter = 1;
	  Armageddon ();
	  break;
	case 'l':
	  vga_clear ();
	  gl_printf (X0, Y0, "NR.\tID\tX\tY\tENERGY.\n");
	  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
	    {
	      if (Feindesliste[i].levelnum == CurLevel->levelnum)
		gl_printf (-1, -1, "%d.\t%s\t%d\t%d\t%d.\n", i,
			   Druidmap[Feindesliste[i].type].druidname,
			   Feindesliste[i].pos.x, Feindesliste[i].pos.y,
			   Feindesliste[i].energy);
	    }
	  getchar ();
	  break;
	case 'r':
	  for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
	    {
	      if (Feindesliste[i].levelnum == CurLevel->levelnum)
		Feindesliste[i].energy = 0;
	    }
	  gl_printf (-1, -1, "All robots on this deck killed!");
	  getchar ();
	  break;

	case 'g':
	  vga_clear ();
	  gl_printf (X0, Y0, "Nr.\tLev.\tID\tEnergy\n");
	  for (i = 0; i < NumEnemys; i++)
	    {
	      gl_printf (-1, -1, "%d\t%d\t%s\t%d\n",
			 i, Feindesliste[i].levelnum,
			 Druidmap[Feindesliste[i].type].druidname,
			 Feindesliste[i].energy);
	      if ((i % 22) == 0 && i > 0)
		{
		  gl_printf (-1, -1, " --- MORE --- \n");
		  getchar ();
		  vga_clear ();
		  gl_printf (X0, Y0, "Nr.\tLev.\tID\tEnergy\n");
		}
	    }
	  break;

	case 't':
	  gl_printf (-1, -1, "\n Enter Levelnummer, X-Pos, Y-Pos: ");
	  scanf ("%d, %d, %d", &LNum, &X, &Y);
	  getchar ();		// remove the cr from input
	  vga_clear ();
	  Teleport (LNum, X, Y);
	  gl_printf (1, 1, "This is your position on level %d.\n", LNum);
	  gl_printf (-1, -1, "Press key to continue");
	  getchar ();
	  break;

	case 'w':
	  gl_printf (-1, -1, "\nTypennummer ihres neuen robos: ");
	  scanf ("%s", NewRoboType);
	  getchar ();		// remove cr from input
	  for (i = 0; i < ALLDRUIDTYPES; i++)
	    {
	      if (!strcmp (Druidmap[i].druidname, NewRoboType))
		break;
	    }
	  if (i == ALLDRUIDTYPES)
	    {
	      gl_printf (-1, -1, "\nUnrecognized robot-type: %s\n",
			 NewRoboType);
	      getchar ();
	    }
	  else
	    {
	      Me.type = i;
	      Me.energy = Druidmap[Me.type].maxenergy;
	      Me.health = Me.energy;
	      gl_printf (-1, -1, "\nYou are now a %s. Have fun!\n",
			 NewRoboType);
	      getchar ();
	      RedrawInfluenceNumber ();
	    }
	  break;

	case 'i':
	  InvincibleMode = !InvincibleMode;
	  break;
	case 'v':
	  Me.energy = Druidmap[Me.type].maxenergy;
	  Me.health = Me.energy;
	  gl_printf (-1, -1, "\nSie sind wieder gesund!");
	  getchar ();
	  break;
	case 'b':
	  Me.energy = 1;
	  gl_printf (-1, -1, "\nSie sind jetzt ziemlich schwach!");
	  getchar ();
	  break;
	case 'c':
	  Conceptview = !Conceptview;
	  break;
	case 's':
	  Weiter = 1;
	  sound_on = !sound_on;
	  break;
	case 'm':
	  gl_printf (-1, -1, "\nLevelnum:");
	  scanf ("%d", &LNum);
	  getchar ();
	  // this function works in raw-kb mode, so we switch again
	  keyboard_init ();
	  ShowDeckMap (curShip.AllLevels[LNum]);
	  keyboard_close ();
	  break;

	case ' ':
	case 'q':
	  Weiter = 1;
	  break;
	}
    }
  ClearGraphMem (RealScreen);
  DisplayRahmen (RealScreen);
  InitBars = TRUE;

  vga_clear ();
  keyboard_init (); /* return to raw keyboard mode */

  return;
}


/*@Function============================================================
@Desc: Testfunktion fuer InsertMessage()

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
InsertNewMessage (void)
{
  static int counter = 0;
  char testmessage[100];

  DebugPrintf
    ("\nvoid InsertNewMessage(void): real function call confirmed...");

  counter++;
  sprintf (testmessage, "Das ist die %d .te Message !!", counter);
  InsertMessage (testmessage);

  DebugPrintf ("\nvoid InsertNewMessage(void): end of function reached...");
  return;
}				// void InsertNewMessage(void)

/*@Function============================================================
@Desc: 	Diese Funktion beendet das Programm und setzt alle notwendigen Dinge
	auf ihre Ausgangswerte zur"uck.
	Wenn ein ExitCode != 0 angegeben wurde wartet er noch auf eine Taste.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Terminate (int ExitCode)
{

  DebugPrintf ("\nvoid Terminate(int ExitStatus) wurde aufgerufen....\n");
  printf ("GameOver : %i\n", GameOver);
  keyboard_close ();
  vga_setmode (TEXT);
  exit (ExitCode);
  return;

}				// void Terminate(int ExitCode)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
KillQueue (void)
{
  while (Queue)
    AdvanceQueue ();
}

/*@Function============================================================
@Desc: AdvanceQueue(): rueckt in der Queue eins vor und loescht Vorgaenger.

@Ret:
@Int:
* $Function----------------------------------------------------------*/
void
AdvanceQueue (void)
{
  message *tmp;

  DebugPrintf ("\nvoid AdvanceQueue(void): Funktion wurde echt aufgerufen.");

  if (Queue == NULL)
    return;

  if (Queue->MessageText)
    free (Queue->MessageText);
  tmp = Queue;

  Queue = Queue->NextMessage;

  free (tmp);

  DebugPrintf
    ("\nvoid AdvanceQueue(void): Funktion hat ihr natuerliches Ende erfolgreich erreicht....");
}				// void AdvanceQueue(void)


/*@Function============================================================
@Desc: Handles all Screenmessages

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PutMessages (void)
{
  static int MesPos = 0;	/* X-Position der Message-Bar */
  static int Working = FALSE;	/* wird gerade eine Message bearbeitet ?? */
  message *LQueue;		/* Bewegl. Queue-Pointer */
  int i;

  if (!PlusExtentionsOn)
    return;

  DebugPrintf ("\nvoid PutMessages(void): Funktion wurde echt aufgerufen.");

  if (!Queue)
    return;			/* nichts liegt an */
  if (!Working)
    ThisMessageTime = 0;	/* inaktiv, aber Queue->reset time */


#ifdef QUEUEDEBUG
  gotoxy (1, 10);
  printf ("Time: %d", ThisMessageTime);
#endif

// Ausgabe der momentanen Liste:
#ifdef QUEUEDEBUG
  LQueue = Queue;
  i = 0;
  gotoxy (1, 5);
  DebugPrintf ("\nvoid PutMessages(void): This is the Queue of Messages:\n");
  while (LQueue != NULL)
    {
      if ((LQueue->MessageText) == NULL)
	{
	  DebugPrintf
	    ("\nvoid PutMessages(void): ERROR: Textpointer is NULL !!!!!!\n");
	  getchar ();
	}
      printf ("%d. '%s' %d\n", i, LQueue->MessageText,
	      LQueue->MessageCreated);
      i++;
      LQueue = LQueue->NextMessage;
    }
  DebugPrintf (" NULL reached !\n");
#endif


  // Wenn die Nachricht schon sehr alt ist, wird sie gel"oscht. 
  if (Working && (ThisMessageTime > MaxMessageTime))
    {
      AdvanceQueue ();
      CleanMessageLine ();
      Working = FALSE;		// inaktiv
      ThisMessageTime = 0;	// Counter init.
      return;
    }


  /* Alte Mes. hat MinTime gelebt, neue wartet */
  if ((ThisMessageTime > MinMessageTime) && (Queue->NextMessage))
    {
      AdvanceQueue ();		/* Queue weiterbewegen */
      Working = FALSE;		/* inaktiv setzen */
      ThisMessageTime = 0;	/* counter neu init. */
      return;
    }

  // Modul inaktiv und neue Message da --> aktivieren
  if ((!Working) && Queue)
    {

      // Wenn die neue Nachricht noch nicht erzeugt ist, dann erzeuge sie
      if (!Queue->MessageCreated)
	{
	  CreateMessageBar (Queue->MessageText);
	  Queue->MessageCreated = TRUE;
	}

      ThisMessageTime = 0;	/* counter init. */
      CleanMessageLine ();	/* Zeile loeschen */
      Working = TRUE;		/* aktiviert */
    }


  // Modul ist gerade aktiv --> bewegen und anzeigen
  if (Working && Queue)
    {

      MesPos = 10 * ThisMessageTime;	/* zeitl. synchronisierte Bewegung */

      /* nicht ueber linken Rand fahren !! */
      if (MesPos > (MESBARBREITE - 2))
	MesPos = MESBARBREITE - 2;

      // 
      // SINCE 'REALSCREEN' NO LONGER RELEVANT, A NEW ROUTINE HAS TO BE FOUND FOR THE PORT
      //
      //    for(i=0;i<MESHOEHE;i++) {
      //      memcpy(
      //             RealScreen+MESPOSX+MESBARBREITE-MesPos+(MESPOSY+i)*SCREENBREITE,
      //             MessageBar+i*MESBARBREITE,
      //             MesPos
      //            );
      //    } /* for */

      //
      // THIS IS THE NEW ROUTINE.
      //
      for (i = 0; i < MESHOEHE; i++)
	{
	  vga_drawscansegment (MessageBar + i * MESBARBREITE,
			       MESPOSX - MesPos, MESPOSY + i, MesPos);
	}
    }				/* if aktiv + Message da */

}				/* Put Messages */

/*@Function============================================================
@Desc: CleanMessageLine: l"oscht die Nachrichtenzeile am Bildschrim

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
CleanMessageLine (void)
{
  memset (RealScreen + MESPOSX + MESPOSY * SCREENBREITE, 0,
	  SCREENBREITE * MESHOEHE);
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
CreateMessageBar (char *MText)
{
  char Worktext[42];
  int i, j;

  DebugPrintf
    ("\nvoid CreateMessageBar(char* MText): Funktion echt aufgerufen.");

  // "Uberl"angen checken
  if (strlen (MText) > 40)
    {
      DebugPrintf
	("\nvoid CreateMessageBar(char* MText): Message hat mehr als 40 Zeichen !.\n");
      getchar ();
      Terminate (ERR);
    }

  // Speicher anfordern, wenn noch keiner da
  if (MessageBar == NULL)
    if ((MessageBar = MyMalloc (MESBAR_MEM)) == NULL)
      {
	DebugPrintf
	  ("\nvoid CreateMessageBar(char* MText): Bekomme keinen Speicher fuer MessageBar !!\n");
	getchar ();
	Terminate (ERR);
      }

  // Message auf 40 Zeichen erg"anzen
  strcpy (Worktext, MText);
  while (strlen (Worktext) < 40)
    strcat (Worktext, " ");

  // Im internen Screen die Nachricht anzeigen und dann ausschneiden
  for (i = 0; i < 40; i++)
    {
      for (j = 0; j < 8; j++)
	{
	  memcpy ((MessageBar + i * 8 + j * SCREENBREITE),
		  (Data70Pointer + Worktext[i] * 8 * 8 + j * 8), 8);
	}
    }

// Am unteren Rand die Nachricht anzeigen und dann ausschneiden
//      gotoxy(1,4);
//      printf("%s",MText);
//      memcpy(MessageBar,RealScreen+3*8*SCREENBREITE,8*SCREENBREITE);

  DebugPrintf
    ("\nvoid CreateMessageBar(char* MText): Funktion hat ihr natuerliches Ende erreicht.");
}				// void CreateMessageBar(char* MText)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
InsertMessage (char *MText)
{
  message *LQueue = Queue;

  DebugPrintf
    ("\nvoid InsertMessage(char* MText): real function call confirmed...");

  if (LQueue)
    {
      // Bis vor die n"achste freie Position vorr"ucken
      while (LQueue->NextMessage != NULL)
	LQueue = LQueue->NextMessage;
      LQueue->NextMessage = MyMalloc (sizeof (message) + 1);
      LQueue = LQueue->NextMessage;
    }
  else
    {
      Queue = MyMalloc (sizeof (message) + 1);
      LQueue = Queue;
    }

  LQueue->MessageText = MyMalloc (MAX_MESSAGE_LEN + 1);
  strcpy (LQueue->MessageText, MText);
  LQueue->NextMessage = NULL;
  LQueue->MessageCreated = FALSE;
#ifdef QUEUEDEBUG
  gotoxy (1, 11);
  printf ("\nvoid InsertMessage(char* MText): A message has been added:%s",
	  MText);
#endif

  DebugPrintf ("\nvoid InsertMessage(char* MText): end of function reached.");
}				// void InsertMessage(char* MText)

/* **********************************************************************
	Diese Funktion erledigt ein normales Malloc, trifft zuerst aber ein
	Par Sicherheitsvorkehrungen.
	**********************************************************************/
void *
MyMalloc (long Mamount)
{
  void *Mptr = NULL;

  if ((Mptr = malloc ((size_t) Mamount)) == NULL)
    {
      printf (" MyMalloc(%ld) did not succeed!\n", Mamount);
      getchar ();
    }
  return Mptr;
}				// void* MyMalloc(long Mamount)

/*@Function============================================================
@Desc: DirToVect
			int direction: a number representing a direction
			Vect vector: 	a pointer to a vector for the result
								(must be already allocated !)

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
DirToVect (int dir, Vect vector)
{

  switch (dir)
    {
    case OBEN:
      vector->x = 0;
      vector->y = -1;
      break;

    case RECHTSOBEN:
      vector->x = 1;
      vector->y = -1;
      break;

    case RECHTS:
      vector->x = 1;
      vector->y = 0;
      break;

    case RECHTSUNTEN:
      vector->x = 1;
      vector->y = 1;
      break;

    case UNTEN:
      vector->x = 0;
      vector->y = 1;
      break;

    case LINKSUNTEN:
      vector->x = -1;
      vector->y = 1;
      break;

    case LINKS:
      vector->x = -1;
      vector->y = 0;
      break;

    case LINKSOBEN:
      vector->x = -1;
      vector->y = -1;
      break;


    default:
      DebugPrintf ("illegal direction in VectToDir() !");
      vector->x = vector->y = 0;
      return;
    }				/* switch */

}				/* DirToVect */

/*@Function============================================================
@Desc: my_sqrt:		Quadrat-Wurzel

@Ret: int
@Int:
* $Function----------------------------------------------------------*/
long
my_sqrt (long wert)
{
  long base = (long) wert - 1;
  long x;
  long res;
  long tmp = wert;
  int prec = 0;			/* Groessenordnung der Zahl */
  long verschiebung = 1;	/* Verschiebung zur Erhoehung der Genauigkeit */
  int counter;

  if (wert < 1)
    return (long) 1;

  while (tmp /= 10)
    prec++;
  prec = 4 - prec;		/* verschiebe auf mind. 4 stellen */
  if (prec < 0)
    prec = 0;

  counter = prec;
  while (counter--)
    verschiebung *= 10;

  x = base;
  x *= verschiebung;

  res = verschiebung + x / 2;
  x *= base;
  res -= x / 8;
  x *= base;
  res += x / 16;

  while (counter--)
    {
      if ((counter == 0) && ((res % 10) >= 5))
	res += 10;
      res /= 10;
    }


  return res;
}

/*@Function============================================================
@Desc: my_abs

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
my_abs (int wert)
{
  return (wert < 0) ? -wert : wert;

}

/*@Function============================================================
@Desc: ShowDebugInfos()

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ShowDebugInfos (void)
{
  vga_clear ();
  gl_printf (20, 5, "\nMe.energy: %d", Me.energy);
  gl_printf (-1, -1, "\nMe.pos: %d %d", Me.pos.x, Me.pos.y);
  keyboard_close ();
  getchar ();
  keyboard_init ();
  return;
}

#undef _misc_c
