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
//--------------------
// Definitions for the menu inside the in-game console
//
#define CONS_MENU_HEIGHT 		256
#define CONS_MENU_LENGTH 		100

SDL_Rect Cons_Menu_Rect = {32, 180, 100, 256};
SDL_Rect Cons_Rect = {16, 162, 595, 315};

SDL_Rect Cons_Text_Rect = {175, 180, SCREENLEN-175, 305}; 

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

  //--------------------
  // It might happen, that the influencer enters the elevator, but then decides to
  // come out on the same level where he has been before.  In this case of course there
  // is no need to reshuffle enemys or to reset influencers position.  Therefore, only
  // when a real level change has occured, we need to do real changes as below, where
  // we set the new level and set new position and initiate timers and all that...
  //
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

      // redistribute the enemys around the level
      ShuffleEnemys ();

      // set the position of the influencer to the correct locatiohn
      Me.pos.x =
	curShip.AllLifts[curLift].x; 
      Me.pos.y =
	curShip.AllLifts[curLift].y; 

      // We reset the time on this level and the position history
      Me.FramesOnThisLevel=0;
      // for ( i = 0 ; i < MAX_INFLU_POSITION_HISTORY ; i++ ) 
      // {
      // Me.Position_History[ i ].x = Me.pos.x;
      // Me.Position_History[ i ].y = Me.pos.y;
      // }

      // delete all bullets and blasts
      for (i = 0; i < MAXBLASTS; i++)
	// AllBlasts[i].type = OUT;
	DeleteBlast( i );
      for (i = 0; i < MAXBULLETS; i++)
	{
	  // AllBullets[i].type = OUT;
	  // AllBullets[i].mine = FALSE;
	  // Never remove bullets any other way than via DeleteBullet or you will
	  // get SEGFAULTS!!!!!!!!!!
	  DeleteBullet ( i ) ;
	}
    } // if real level change has occured

  LeaveLiftSound ( );
  ClearGraphMem ( );
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  // UnfadeLevel ();

  // if the level is empty, let's color it in gray
  if (CurLevel->empty)
    LevelGrauFaerben ();

  InitBars = TRUE;

  while (SpacePressed ()) ;

  Me.status = MOBILE;
  Me.TextVisibleTime=0;
  Me.TextToBeDisplayed=CurLevel->Level_Enter_Comment;

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
  SDL_Color lift_bg_color = {0,0,0};  /* black... */
  int xoffs = (User_Rect.w - 578)/2;
  int yoffs = (User_Rect.h - 211)/2;

  ship_off_pic= IMG_Load (find_file (ship_off_filename, GRAPHICS_DIR, TRUE));
  ship_on_pic = IMG_Load (find_file (ship_on_filename, GRAPHICS_DIR, TRUE));

  // clear the whole screen
  //ClearGraphMem();
  // fill the user fenster with some color
  Fill_Rect (User_Rect, lift_bg_color);
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );      

  /* First blit ship "lights off" */
  Copy_Rect (User_Rect, dst);
  SDL_SetClipRect (ne_screen, &dst);
  Copy_Rect (User_Rect, dst);
  dst.x += xoffs;
  dst.y += yoffs;
  SDL_BlitSurface (ship_off_pic, NULL, ne_screen, &dst);
  
  if (level >= 0)
    for (i=0; i<curShip.num_level_rects[level]; i++)
      {
	Copy_Rect (curShip.Level_Rects[level][i], src);
	Copy_Rect (src, dst);
	dst.x += User_Rect.x + xoffs;   /* offset respective to User-Rectangle */
	dst.y += User_Rect.y + yoffs; 
	SDL_BlitSurface (ship_on_pic, &src, ne_screen, &dst);
      }

  if (liftrow >=0)
    {
      Copy_Rect (curShip.LiftRow_Rect[liftrow], src);
      Copy_Rect (src, dst);
      dst.x += User_Rect.x + xoffs;   /* offset respective to User-Rectangle */
      dst.y += User_Rect.y + yoffs; 
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
	  SetCombatScaleTo(1.0);
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

  // Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );
  Switch_Background_Music_To ( CurLevel->Background_Song_Name );

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
  DisplayImage ( find_file( NE_CONSOLE_BG_PIC1_FILE , GRAPHICS_DIR, FALSE) );

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

  strcpy (MenuText, "\nAccess granted.\nArea : ");
  strcat (MenuText, curShip.AreaName ); // Shipnames[ThisShip]);
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
  SDL_Rect tmp;
  
  Copy_Rect (User_Rect, tmp)

  SDL_FillRect( ne_screen , &tmp, 0 );
  return;

} // void ClearUserFenster(void)


/* ----------------------------------------------------------------------
 * This function does the robot show when the user has selected robot
 * show from the console menu.
 * ---------------------------------------------------------------------- */
void
GreatDruidShow (void)
{
  int droidtype;
  int key;
  int finished;
  int page;

  droidtype = Me.type;
  page = 0;

  finished = FALSE;
  while (!finished)
    {
      show_droid_info (droidtype, page);

      key = getchar_raw();
      switch (key)
	{
	case SDLK_RETURN:
	case SDLK_SPACE:
	case SDLK_ESCAPE:
	  finished = TRUE;
	  break;
	case SDLK_UP:
	  if (droidtype < Me.type) droidtype ++;
	  break;
	case SDLK_DOWN:
	  if (droidtype > 0) droidtype --;
	  break;
	case SDLK_RIGHT:
	  if (page < 2) page ++;
	  break;
	case SDLK_LEFT:
	  if (page > 0) page --;
	  break;
	default:
	  break;
	} /* switch (key) */

    } /* while !finished */

  return;
}; // void GreatDroidShow( void ) 

/*------------------------------------------------------------
 * display infopage page of droidtype
 *
 *  does update the screen, no SDL_Flip() necesary !
 *
 *------------------------------------------------------------*/
void 
show_droid_info (int droidtype, int page)
{
  char InfoText[1000];
  SDL_SetClipRect ( ne_screen , NULL );
  DisplayImage ( find_file( NE_CONSOLE_BG_PIC2_FILE , GRAPHICS_DIR, FALSE) );
  DisplayBanner (NULL, NULL,  BANNER_NO_SDL_UPDATE | BANNER_FORCE_UPDATE );

  ShowRobotPicture (Cons_Menu_Rect.x, Cons_Menu_Rect.y,  droidtype);

  switch (page)
    {
    case -3: // Title screen: intro unit
      sprintf (InfoText, "Unit type %s - %s\n \
This is the unit that you currently control. Prepare to board Robo-frighter \
Paradroid to eliminate all rogue robots.", 
	       Druidmap[droidtype].druidname, 
	       Classname[Druidmap[droidtype].class]);
      break;
    case -2: // Takeover: unit that you wish to control
      sprintf (InfoText, "Unit type %s - %s\n \
This is the unit that you wish to control. Prepare to takeover.", 
	       Druidmap[droidtype].druidname, 
	       Classname[Druidmap[droidtype].class]);
      break;
    case -1: // Takeover: unit that you control
      sprintf (InfoText, "Unit type %s - %s\n \
This is the unit that you currently control.", 
	       Druidmap[droidtype].druidname, 
	       Classname[Druidmap[droidtype].class]);
      break;
    case 0:
      sprintf (InfoText, "\
Unit type %s - %s\n\
Entry : %d\n\
Class : %s\n\
Height : %f\n\
Weight: %f \n\
Drive : %s \n\
Brain : %s", Druidmap[droidtype].druidname, Classname[Druidmap[droidtype].class],
	       droidtype+1, Classes[Druidmap[droidtype].class],
	       Druidmap[droidtype].height, Druidmap[droidtype].weight,
	       Drivenames [ Druidmap[ droidtype].drive], 
	       Brainnames[ Druidmap[droidtype].brain ]);
      break;
    case 1:
      sprintf( InfoText , "\
Unit type %s - %s\n\
Armamant : %s\n\
Sensors  1: %s\n          2: %s\n          3: %s", Druidmap[droidtype].druidname,
	       Classname[Druidmap[droidtype].class],
	       Weaponnames [ Druidmap[droidtype].armament],
	       Sensornames[ Druidmap[droidtype].sensor1 ],
	       Sensornames[ Druidmap[droidtype].sensor2 ],
	       Sensornames[ Druidmap[droidtype].sensor3 ]);
      break;
    case 2:
      sprintf (InfoText, "\
Unit type %s - %s\n\
Notes: %s", Druidmap[droidtype].druidname , Classname[Druidmap[droidtype].class],
	       Druidmap[droidtype].notes);
      break;
    default:
      sprintf (InfoText, "ERROR: Page not implemented!! \nPlease report bug!");
      break;
    } /* switch (page) */

  SetCurrentFont( Para_BFont );
  DisplayText (InfoText, Cons_Text_Rect.x, Cons_Text_Rect.y, &Cons_Text_Rect);
  SDL_Flip (ne_screen);

} /* show_droid_info */




#undef _ship_c
