/*----------------------------------------------------------------------
 *
 * Desc: the konsole- and lift functions
 *
 *----------------------------------------------------------------------*/

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
#define _ship_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "text.h"
#include "ship.h"
#include "SDL_rotozoom.h"

int NoKeyPressed (void);

int ConsoleMenuPos=0;


/*-----------------------------------------------------------------
 * @Desc: does all the work when we enter a lift
 * 
 *-----------------------------------------------------------------*/
void
EnterLift (void)
{
  int i;
  int curLevel;
  int curLift, upLift, downLift, liftrow;

  DebugPrintf (2, "\nvoid EnterLiftator(void): Function call confirmed.");

  /* Prevent distortion of framerate by the delay coming from 
   * the time spend in the menu. */
  Activate_Conservative_Frame_Computation();

  /* Prevent the influ from coming out of the lift in transfer mode
   * by turning off transfer mode as soon as the influ enters the lift */
  Me.status= ELEVATOR;

  curLevel = CurLevel->levelnum;

  if ((curLift = GetCurrentLift ()) == -1)
    {
      printf ("Lift out of order, I'm so sorry !");
      return;
    }

  EnterLiftSound ();

  upLift = curShip.AllLifts[curLift].up;
  downLift = curShip.AllLifts[curLift].down;

  liftrow = curShip.AllLifts[curLift].lift_row;

  ShowLifts (curLevel, liftrow);

  /* Warten, bis User Feuer auslaesst */
  while (SpacePressed ()) ;


  while (!SpacePressed ())
    {
      if (UpPressed () && !DownPressed ())
	if (upLift != -1)
	  {			/* gibt es noch einen Lift hoeher ? */
	    if (curShip.AllLifts[upLift].x == 99)
	      {
		printf ("Out of order, so sorry ..");
	      }
	    else
	      {
		downLift = curLift;
		curLift = upLift;
		curLevel = curShip.AllLifts[curLift].level;
		upLift = curShip.AllLifts[curLift].up;

		ShowLifts (curLevel, liftrow);

		/* Warten, bis user Taste auslaesst */
		MoveLiftSound ();
		while (UpPressed ()) ;
	      }
	  }			/* if uplevel */


      if (DownPressed () && !UpPressed ())
	if (downLift != -1)
	  {			/* gibt es noch einen Lift tiefer ? */
	    if (curShip.AllLifts[downLift].x == 99)
	      {
		printf ("Out of order, so sorry ..");
	      }
	    else
	      {
		upLift = curLift;
		curLift = downLift;
		curLevel = curShip.AllLifts[curLift].level;
		downLift = curShip.AllLifts[curLift].down;

		ShowLifts (curLevel, liftrow);

		/* Warten, bis User Taste auslaesst */
		MoveLiftSound ();
		while (DownPressed ()) ;
	      }
	  }			/* if downlevel */
    }				/* while !SpaceReleased */

  /* Neuen Level und Position setzen */
  if (curLevel != CurLevel->levelnum)
    {				/* wirklich neu ??? */
      int array_num = 0;
      Level tmp;

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
      Me.pos.x =
	curShip.AllLifts[curLift].x; //NORMALISATION * Block_Width + Block_Width / 2;
      Me.pos.y =
	curShip.AllLifts[curLift].y; //NORMALISATION* Block_Height + Block_Height / 2;

      /* Alle Blasts und Bullets loeschen */
      for (i = 0; i < MAXBLASTS; i++)
	AllBlasts[i].type = OUT;
      for (i = 0; i < MAXBULLETS; i++)
	{
	  // AllBullets[i].type = OUT;
	  // AllBullets[i].mine = FALSE;
	  // Never remove bullets any other way than via DeleteBullet or you will
	  // get SEGFAULTS!!!!!!!!!!
	  DeleteBullet ( i ) ;
	  
	}

    }				/* if neuer Level */

  LeaveLiftSound ();
  ClearGraphMem ( );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  // UnfadeLevel ();

  /* Wenn Level leer: grau faerben */
  if (CurLevel->empty)
    LevelGrauFaerben ();

  InitBars = TRUE;

  while (SpacePressed ()) ;

  Me.status = MOBILE;

  DebugPrintf (2, "\nvoid EnterLift(void): Usual end of function reached.");
}	/* EnterLift */

/*-----------------------------------------------------------------
 * @Desc: show side-view of the ship, and hightlight the current 
 *        level + lift
 *
 *  if level==-1: don't highlight any level
 *  if liftrow==-1: dont' highlight any liftrows
 *
 *-----------------------------------------------------------------*/
void
ShowLifts (int level, int liftrow)
{
  SDL_Rect src, dst;
  int i;

  ship_off_pic= IMG_Load (find_file (ship_off_filename, GRAPHICS_DIR, TRUE));
  ship_on_pic = IMG_Load (find_file (ship_on_filename, GRAPHICS_DIR, TRUE));

  // clear the whole screen
  //ClearGraphMem();
  // fill the user fenster with some color
  SetUserfenster ( EL_BG_COLOR );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );      

  /* First blit ship "lights off" */
  Copy_Rect (User_Rect, dst);
  SDL_SetClipRect (ne_screen, &dst);
  Copy_Rect (User_Rect, dst);
  SDL_BlitSurface (ship_off_pic, NULL, ne_screen, &dst);
  
  /* Now superpose current level & lift "lights on"  */
  //  SDL_BlitSurface (ship_on_pic, NULL, ne_screen, &dst);

  if (level >= 0)
    for (i=0; i<curShip.num_level_rects[level]; i++)
      {
	Copy_Rect (curShip.Level_Rects[level][i], src);
	Copy_Rect (src, dst);
	dst.x += User_Rect.x;   /* offset respective to User-Rectangle */
	dst.y += User_Rect.y; 
	SDL_BlitSurface (ship_on_pic, &src, ne_screen, &dst);
      }

  if (liftrow >=0)
    {
      Copy_Rect (curShip.LiftRow_Rect[liftrow], src);
      Copy_Rect (src, dst);
      dst.x += User_Rect.x;   /* offset respective to User-Rectangle */
      dst.y += User_Rect.y; 
      SDL_BlitSurface (ship_on_pic, &src, ne_screen, &dst);
    }

  SDL_Flip (ne_screen);

  SDL_FreeSurface( ship_off_pic );
  SDL_FreeSurface( ship_on_pic );

  return;

} /* ShowLifts() */

/*@Function============================================================
@Desc: EnterKonsole(): does all konsole- duties
This function runs the consoles. This means the following duties:
	2	* Show a small-scale plan of the current deck
	3	* Show a side-elevation on the ship
	1	* Give all available data on lower druid types
	0	* Reenter the game without squashing the colortable
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
EnterKonsole (void)
{
  int ReenterGame = 0;
  int TasteOK;

  // Prevent distortion of framerate by the delay coming from 
  // the time spend in the menu.
  Activate_Conservative_Frame_Computation();

  Me.status = CONSOLE;

  SetCurrentFont( Para_BFont );

  Switch_Background_Music_To (CONSOLE_BACKGROUND_MUSIC_SOUND);

  while (SpacePressed ());  /* wait for user to release Space */

  ConsoleMenuPos=0;

  /* Gesamtkonsolenschleife */

  while (!ReenterGame)
    {

      PaintConsoleMenu ();
      SDL_Flip (ne_screen);

      /* Nichts tun bis eine vern"unftige Taste gedr"uckt wurde */
      TasteOK = 0;
      while (!TasteOK)
	{
	  if (UpPressed ())
	    {
	      ConsoleMenuPos--;
	      TasteOK = 1;
	    }
	  if (DownPressed ())
	    {
	      ConsoleMenuPos++;
	      TasteOK = 1;
	    }
	  if (SpacePressed ())
	    TasteOK = 1;
	}

      /* Verhindern, da"s der Menucursor das Menu verl"a"st */
      if (ConsoleMenuPos < 0)
	ConsoleMenuPos = 0;
      if (ConsoleMenuPos > 3)
	ConsoleMenuPos = 3;

      /* gew"ahlte Menupunkte betreten */
      if ((ConsoleMenuPos == 0) & (SpacePressed ()))
	ReenterGame = TRUE;
      if ((ConsoleMenuPos == 1) & (SpacePressed ()))
	GreatDruidShow ();
      if ((ConsoleMenuPos == 2) & (SpacePressed ()))
	{
	  ShowDeckMap (CurLevel);
	  /* this is not very elegant at the moment, but it works ok.. */
	  while ( SpacePressed() );  /* wait for space-release */
	  while (!SpacePressed () ); /* and wait for another space before leaving */
	  // Now that we leave, we restore the combat screen scaling factor..
	  // if ( CurrentCombatScaleFactor == 1 ) ReInitPictures();
	  // else {
	  // if ( CurrentCombatScaleFactor != 0.5 ) SetCombatScaleTo( CurrentCombatScaleFactor );
	  // }
	  SetCombatScaleTo( 1 );
	  while ( SpacePressed() ); /* but also wait for the release before going on..*/
	}
      if ((ConsoleMenuPos == 3) & (SpacePressed ()))
	{
	  while (SpacePressed ());
	  ClearGraphMem();
	  ShowLifts (CurLevel->levelnum, -1);
	  while (!SpacePressed ());
	  while (SpacePressed ());
	}

      while (DownPressed ());
      while (UpPressed ());
    }				/* (while !ReenterGane) */

  Me.status = MOBILE;
  /* Die Textfarben wieder setzen wie sie vorher waren */
  SetTextColor (FONT_WHITE, FONT_RED);	/* BG: Bannerwei"s FG: FONT_RED */

  ClearGraphMem ( );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );
  SDL_Flip( ne_screen );

  while (SpacePressed ());

  Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );

  return;

} // void EnterKonsole(void)

/*-----------------------------------------------------------------
 * @Desc: diese Funktion zeigt die m"oglichen Auswahlpunkte des Menus
 *    Sie soll die Schriftfarben nicht ver"andern
 *
 *  NOTE: this function does not actually _display_ anything yet,
 *        it just prepares the display, so you need
 *        to call SDL_Flip() to display the result!
 *
 *
 *-----------------------------------------------------------------*/
void
PaintConsoleMenu (void)
{
  char MenuText[200];

  SDL_Rect SourceRectangle;
  SDL_Rect TargetRectangle;

  ClearGraphMem ();

  SDL_SetClipRect ( ne_screen , NULL );
  DisplayImage ( find_file( NE_CONSOLE_BG_PIC_FILE , GRAPHICS_DIR, FALSE) );

  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  /* Userfenster faerben */
  // SetUserfenster (KON_BG_COLOR, Outline320x200);
  // SetUserfenster ( 208 );

  /* Konsolen-Menue Farbe setzen */
  // SetTextColor (KON_BG_COLOR, KON_TEXT_COLOR);
  SetTextColor (208, BANNER_VIOLETT );	// RED // YELLOW

  strcpy (MenuText, "Unit type ");
  strcat (MenuText, Druidmap[Me.type].druidname);
  strcat (MenuText, " - ");
  strcat (MenuText, Classname[Druidmap[Me.type].class]);
  DisplayText (MenuText, USERFENSTERPOSX, USERFENSTERPOSY, &User_Rect);

  strcpy (MenuText, "\nAccess granted.\nShip : ");
  strcat (MenuText, Shipnames[ThisShip]);
  strcat (MenuText, "\nDeck : ");
  strcat (MenuText, CurLevel->Levelname );
  strcat (MenuText, "\n\nAlert: ");
  strcat (MenuText, Alertcolor[Alert]);

  DisplayText (MenuText, MENUTEXT_X, USERFENSTERPOSY + 15, &Menu_Rect);

  /*
   * Hier werden die Icons des Menus ausgegeben
   *
   */

  SourceRectangle.x=(MENUITEMLENGTH+2)*ConsoleMenuPos;
  SourceRectangle.y=0;
  SourceRectangle.w=MENUITEMLENGTH;
  SourceRectangle.h=USERFENSTERHOEHE;
  TargetRectangle.x=MENUITEMPOSX;
  TargetRectangle.y=MENUITEMPOSY;
  SDL_BlitSurface( ne_console_surface , &SourceRectangle , ne_screen , &TargetRectangle );

  return;
}	// PaintConsoleMenu ()

/*@Function============================================================
@Desc: Zeigt alle erlaubten Roboter.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
GreatDruidShow (void)
{
  char InfoText[10000];
  int Infodroid;
  char PassOn = 0;
  SDL_Rect Droid_Text_Rect;

  DebugPrintf (2, "\nvoid GreadDruidShow(void): Function call confirmed.");

  /* Warte, bis User Space auslaesst */
  while (SpacePressed ()) ;

  SetUserfenster ( KON_BG_COLOR );

  for (Infodroid = Me.type; Infodroid > -1;)
    {

      PassOn = 0;
      while (!PassOn)
	{

	  ClearUserFenster( );
	  // SetTextColor (208, BANNER_VIOLETT );	// RED // YELLOW

	  SDL_SetClipRect ( ne_screen , NULL );
	  DisplayImage ( find_file( NE_CONSOLE_BG_PIC_FILE , GRAPHICS_DIR, FALSE) );
	  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );



	  strcpy (InfoText, "Unit type ");
	  strcat (InfoText, Druidmap[Infodroid].druidname);
	  strcat (InfoText, " - ");
	  strcat (InfoText, Classname[Druidmap[Infodroid].class]);

	  Droid_Text_Rect.x=User_Rect.x;
	  Droid_Text_Rect.y=User_Rect.y;
	  Droid_Text_Rect.w=User_Rect.w-20;  // keep away from the right border
	  Droid_Text_Rect.h=User_Rect.h;

	  /*	  SetTextBorder (USERFENSTERPOSX , USERFENSTERPOSY , USERFENSTERPOSX + 
			 USERFENSTERBREITE-2*FONTBREITE ,
			 USERFENSTERPOSY + USERFENSTERHOEHE , 36 );
	  */
	  DisplayText (InfoText, USERFENSTERPOSX, USERFENSTERPOSY, &Droid_Text_Rect);

	  ShowRobotPicture (USERFENSTERPOSX, USERFENSTERPOSY + 2 * FONTHOEHE,
			    Infodroid);

	  SDL_Flip (ne_screen);

	  while (!LeftPressed () && !UpPressed () && !DownPressed ()
		 && !RightPressed () && !SpacePressed ()) ;

	  if (UpPressed ())
	    {
	      Infodroid--;
	      // Einem zu schnellen Weiterbl"attern vorbeugen
	      while (UpPressed () || DownPressed ()) ;
	    }

	  if (DownPressed ())
	    {
	      Infodroid++;
	      // Einem zu schnellen Weiterbl"attern vorbeugen
	      while (UpPressed () || DownPressed ()) ;
	    }

	  if ((RightPressed ()) || (LeftPressed ()))
	    PassOn = 1;

	  if (Infodroid > Me.type)
	    Infodroid = DRUID001;
	  if (Infodroid < DRUID001)
	    Infodroid = Me.type;
	  if (SpacePressed ())
	    {
	      while (SpacePressed ()) ;
	      return;
	    }

	}
      while (LeftPressed () || UpPressed () || DownPressed ()
	     || RightPressed () || SpacePressed ()) ;

      /*
       * Ausgabe der Liste von Werten dieses Druids
       *
       */

      //      ClearGraphMem( Outline320x200 );
      //      DisplayBanner( Outline320x200 );
      // ClearUserFenster( );
      SetTextColor (208, BANNER_VIOLETT );	// RED // YELLOW
      // ClearUserFenster ();

      SDL_SetClipRect ( ne_screen , NULL );
      DisplayImage ( find_file( NE_CONSOLE_BG_PIC_FILE , GRAPHICS_DIR, FALSE) );
      DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );



      sprintf( InfoText , "Unit type %s - %s" , Druidmap[Infodroid].druidname , Classname[Druidmap[Infodroid].class] );

      /*      SetTextBorder (USERFENSTERPOSX, USERFENSTERPOSY, USERFENSTERPOSX +
		     USERFENSTERBREITE, USERFENSTERPOSY + USERFENSTERHOEHE,
		     36);
      */
      DisplayText (InfoText, USERFENSTERPOSX, USERFENSTERPOSY, &User_Rect);

      
      ShowRobotPicture (USERFENSTERPOSX, USERFENSTERPOSY + 2 * FONTHOEHE, Infodroid );

      sprintf( InfoText, "Entry : %d\nClass : %s\nHeight : %f\nWeight: %f \nDrive : %s \nBrain : %s " , 
	       Infodroid+1 , 
	       Classes[Druidmap[Infodroid].class] ,
	       Druidmap[Infodroid].height ,
	       Druidmap[Infodroid].weight ,
	       Drivenames[ Druidmap[Infodroid].drive ] ,
	       Brainnames[ Druidmap[Infodroid].brain ] );

      DisplayText (InfoText, MENUTEXT_X, USERFENSTERPOSY + FontHeight (Menu_BFont),
		   &Menu_Rect);


      SDL_Flip (ne_screen);

      PassOn = 0;
      while (!PassOn)
	{
	  if ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
	      || (DownPressed ()))
	    {
	      PassOn = 1;
	      while ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
		     || (DownPressed ())) ;
	    }
	  if (SpacePressed ())
	    {
	      while (SpacePressed ()) ;
	      DebugPrintf (2, "\nvoid GreadDruidShow(void): Alternate end of function reached via Space1.");
	      return;
	    }
	}


      /*
       * Ausgabe der Liste von Ausr"ustung dieses Druids
       *
       */

      // ClearUserFenster ();
      // ClearGraphMem( Outline320x200 );
      // DisplayBanner( Outline320x200 );
      ClearUserFenster( );
      // SetTextColor (208, BANNER_VIOLETT );	// BLACK and VIOLETT

      SDL_SetClipRect ( ne_screen , NULL );
      DisplayImage ( find_file( NE_CONSOLE_BG_PIC_FILE , GRAPHICS_DIR, FALSE) );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );


      sprintf( InfoText , "Unit type %s - %s" , Druidmap[Infodroid].druidname , Classname[Druidmap[Infodroid].class] );

      DisplayText (InfoText, USERFENSTERPOSX, USERFENSTERPOSY, &User_Rect);

      ShowRobotPicture (USERFENSTERPOSX, USERFENSTERPOSY + 2 * FONTHOEHE, Infodroid);

      strcpy (InfoText, "Armamant : ");
      strcat (InfoText, Weaponnames[ Druidmap[Infodroid].armament ]);
      strcat (InfoText, "\nSensors  1: ");
      strcat (InfoText, Sensornames[ Druidmap[Infodroid].sensor1 ]);
      strcat (InfoText, "\n          2: ");
      strcat (InfoText, Sensornames[ Druidmap[Infodroid].sensor2 ]);
      strcat (InfoText, "\n          3: ");
      strcat (InfoText, Sensornames[ Druidmap[Infodroid].sensor3 ]);

      DisplayText (InfoText, MENUTEXT_X, USERFENSTERPOSY + FontHeight (Menu_BFont),
		   &Menu_Rect);

      SDL_Flip (ne_screen);

      PassOn = 0;
      while (!PassOn)
	{
	  if ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
	      || (DownPressed ()))
	    {
	      PassOn = 1;
	      while ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
		     || (DownPressed ())) ;
	    }
	  if (SpacePressed ())
	    {
	      while (SpacePressed ()) ;
	      return;
	    }
	}


      /*
       * Ausgabe der Informationen bezuglich des Druidhintergrundes
       *
       */

      //      ClearGraphMem( Outline320x200 );
      //      DisplayBanner( Outline320x200 );

      // ClearUserFenster( );
      SetTextColor (208, BANNER_VIOLETT );	// RED // YELLOW
      // ClearUserFenster ();

      SDL_SetClipRect ( ne_screen , NULL );
      DisplayImage ( find_file( NE_CONSOLE_BG_PIC_FILE , GRAPHICS_DIR, FALSE) );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );


      sprintf( InfoText , "Unit type %s - %s" , Druidmap[Infodroid].druidname , Classname[Druidmap[Infodroid].class] );

      DisplayText (InfoText, USERFENSTERPOSX, USERFENSTERPOSY, &User_Rect);

      ShowRobotPicture (USERFENSTERPOSX, USERFENSTERPOSY + 2 * FONTHOEHE, Infodroid );

      strcpy (InfoText, "Notes: ");
      strcat (InfoText, Druidmap[Infodroid].notes);

      DisplayText (InfoText, MENUTEXT_X, USERFENSTERPOSY + FontHeight (Menu_BFont),
		   &Menu_Rect);

      SDL_Flip (ne_screen);

      PassOn = 0;
      while (!PassOn)
	{
	  if ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
	      || (DownPressed ()))
	    {
	      PassOn = 1;
	      while ((RightPressed ()) || (LeftPressed ()) || (UpPressed ())
		     || (DownPressed ())) ;
	    }
	  if (SpacePressed ())
	    {
	      while (SpacePressed ()) ;
	      return;
	    }
	}
    }				/* for */

  DebugPrintf (2, "\nvoid GreadDruidShow(void): End of function reached.");
}				/* GreatDruidShow() */


/*-----------------------------------------------------------------
 * @Desc: Displays the concept view of Level "deck" in Userfenster
 * 	  
 *	Note: we no longer wait here for a key-press, but return
 *            immediately 
 *-----------------------------------------------------------------*/
void
ShowDeckMap (Level deck)
{
  finepoint tmp;
  tmp.x=Me.pos.x;
  tmp.y=Me.pos.y;

  ClearUserFenster ();
  Me.pos.x = CurLevel->xlen/2;
  Me.pos.y = CurLevel->ylen/2;

  SetCombatScaleTo( 0.25 );

  Assemble_Combat_Picture( ONLY_SHOW_MAP );

  SDL_Flip (ne_screen);

  Me.pos.x=tmp.x;
  Me.pos.y=tmp.y;

  // ne_blocks=zwisch;
} /* ShowDeckMap() */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
LevelEmpty (void)
{
  int i;
  int levelnum = CurLevel->levelnum;

  if (CurLevel->empty)
    return TRUE;

  for (i = 0; i < NumEnemys; i++)
    {
      if ((AllEnemys[i].Status != OUT)
	  && (AllEnemys[i].levelnum == levelnum))
	return FALSE;
    }

  CurLevel->empty = TRUE;
  RealScore += DECKCOMPLETEBONUS;
  ShowScore += DECKCOMPLETEBONUS;

  if (ShipEmpty ())
    ThouArtVictorious ();

  return TRUE;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/

int
ShipEmpty (void)
{
  int i;

  for (i = 0; i < curShip.num_levels; i++)
    {
      if (curShip.AllLevels[i] == NULL)
	continue;

      if (!((curShip.AllLevels[i])->empty))
	return (FALSE);
    }
  return (TRUE);
}


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
NoKeyPressed (void)
{
  if (SpacePressed ())
    return (FALSE);
  if (LeftPressed ())
    return (FALSE);
  if (RightPressed ())
    return (FALSE);
  if (UpPressed ())
    return (FALSE);
  if (DownPressed ())
    return (FALSE);
  return (TRUE);
}				// int NoKeyPressed(void)


/*@Function============================================================
@Desc: l"oscht das Userfenster

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ClearUserFenster (void)
{
  SDL_Rect ClearingRectangle;
  
  ClearingRectangle.x=USERFENSTERPOSX;
  ClearingRectangle.y=USERFENSTERPOSY;
  ClearingRectangle.w=USERFENSTERBREITE;
  ClearingRectangle.h=USERFENSTERHOEHE;

  SDL_FillRect( ne_screen , &ClearingRectangle, 0 );
  return;

} // void ClearUserFenster(void)

#undef _ship_c
