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

/* ----------------------------------------------------------------------
 * This file contains all the functions managing the things one gets to see.
 * That includes assembling of enemys, assembling the currently
 * relevant porting of the map (the bricks I mean), drawing all visible
 * elements like bullets, blasts, enemys or influencer in a nonvisible
 * place in memory at first, and finally drawing them to the visible
 * screen for the user.
 * ---------------------------------------------------------------------- */

/*
 * This file has been checked for remnants of german comments.  If you still find
 * any, please let me know.
 */

#define _view_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "map.h"
#include "proto.h"
#include "colodefs.h"
#include "items.h"

#include "SDL_rotozoom.h"

void FlashWindow (SDL_Color Flashcolor);
void RecFlashFill (int LX, int LY, int Color, unsigned char *Parameter_Screen,
		   int SBreite);
int Cent (int);

char *Affected;
EXTERN int MyCursorX;
EXTERN int MyCursorY;

SDL_Color flashcolor1 = {100, 100, 100};
SDL_Color flashcolor2 = {0, 0, 0};

//
// POSSIBLY OUTDATED AND UNUSED FUNCTION
// PLEASE CHECK FOR REMOVAL POSSIBLE
//
int
Cent (int Val)
{
  Val = Val - (Val % Block_Width) + Block_Width / 2;
  return Val;
}

/* ----------------------------------------------------------------------
 * This function should display the automap data, that was collected so
 * far, by the tux.
 * ---------------------------------------------------------------------- */
void
ShowAutomapData( void )
{
  int x , y ;
#define AUTOMAP_SQUARE_SIZE 3
#define AUTOMAP_COLOR 0x0FFFF
  int i;
  int TuxColor = SDL_MapRGB( Screen->format, 0 , 0 , 255 ); 
  int FriendColor = SDL_MapRGB( Screen->format, 0 , 255 , 0 ); 
  int BoogyColor = SDL_MapRGB( Screen->format, 255 , 0 , 0 ); 
  // int ItemColor = SDL_MapRGB( Screen->format, 255 , 0 , 255 );  pink
  int ItemColor = SDL_MapRGB( Screen->format, 255 , 255 , 0 ); 
  Level AutomapLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
  int level = Me [ 0 ] . pos . z ;

  //--------------------
  // Of course we only display the automap on demand of the user...
  //
  if ( GameConfig.Automap_Visible == FALSE ) return;

  //--------------------
  // At first, we only blit the known data about the pure wall-type
  // obstacles on this level
  //
  for ( y = 0 ; y < AutomapLevel->ylen ; y ++ )
    {
      for ( x = 0 ; x < AutomapLevel->xlen ; x ++ )
	{
	  if ( Me [ 0 ] . Automap [ level ] [ y ] [ x ] & RIGHT_WALL_BIT )
	    {
	      putpixel ( Screen , 3*x+2 , 3*y+0 , AUTOMAP_COLOR );
	      putpixel ( Screen , 3*x+2 , 3*y+1 , AUTOMAP_COLOR );
	      putpixel ( Screen , 3*x+2 , 3*y+2 , AUTOMAP_COLOR );
	    }
	  if ( Me [ 0 ] . Automap [ level ] [ y ] [ x ] & LEFT_WALL_BIT )
	    {
	      putpixel ( Screen , 3*x , 3*y+0 , AUTOMAP_COLOR );
	      putpixel ( Screen , 3*x , 3*y+1 , AUTOMAP_COLOR );
	      putpixel ( Screen , 3*x , 3*y+2 , AUTOMAP_COLOR );
	    }
	  if ( Me [ 0 ] . Automap [ level ] [ y ] [ x ] & UP_WALL_BIT )
	    {
	      putpixel ( Screen , 3*x+0 , 3*y , AUTOMAP_COLOR );
	      putpixel ( Screen , 3*x+1 , 3*y , AUTOMAP_COLOR );
	      putpixel ( Screen , 3*x+2 , 3*y , AUTOMAP_COLOR );
	    }
	  if ( Me [ 0 ] . Automap [ level ] [ y ] [ x ] & DOWN_WALL_BIT )
	    {
	      putpixel ( Screen , 3*x+0 , 3*y+2 , AUTOMAP_COLOR );
	      putpixel ( Screen , 3*x+1 , 3*y+2 , AUTOMAP_COLOR );
	      putpixel ( Screen , 3*x+2 , 3*y+2 , AUTOMAP_COLOR );
	    }
	}
    }

  //--------------------
  // Now that the pure map data has been drawn, we add yellow dots for 
  // the items, that have been detected on this level.
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      //--------------------
      // This would be the most accurate information, but this is not granted
      // without casting a spell and then only a copy is made and only updated
      // once, so the player must really get on with much less information.
      //
      // if ( AutomapLevel -> ItemList [ i ] . type == (-1) ) continue;
      if ( Me [ 0 ] . DetectedItemList [ i ] . x == 0 ) continue;

      for ( x = 0 ; x < AUTOMAP_SQUARE_SIZE ; x ++ )
	{
	  for ( y = 0 ; y < AUTOMAP_SQUARE_SIZE ; y ++ )
	    {
	      putpixel ( Screen , AUTOMAP_SQUARE_SIZE * AutomapLevel -> ItemList [ i ].pos.x + x , 
			 AUTOMAP_SQUARE_SIZE * AutomapLevel -> ItemList [ i ].pos.y + y , ItemColor );
	    }
	}
    }

  //--------------------
  // Now that the pure map data has been drawn, we add red dots for 
  // the ememys around.
  //
  for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i ++ )
    {
      if ( AllEnemys [ i ] . Status  == OUT ) continue;
      if ( AllEnemys [ i ] . type == (-1) ) continue;
      if ( AllEnemys [ i ] . pos . z != AutomapLevel -> levelnum ) continue;

      for ( x = 0 ; x < AUTOMAP_SQUARE_SIZE ; x ++ )
	{
	  for ( y = 0 ; y < AUTOMAP_SQUARE_SIZE ; y ++ )
	    {
	      putpixel ( Screen , AUTOMAP_SQUARE_SIZE * AllEnemys[i].pos.x + x , 
			 AUTOMAP_SQUARE_SIZE * AllEnemys[i].pos.y + y , BoogyColor );
	    }
	}
    }

  //--------------------
  // Now that the automap is drawn so far, we add a blue dot for the
  // tux himself and also for colleagues, that are on this level and alive.
  //
  for ( x = 0 ; x < AUTOMAP_SQUARE_SIZE ; x ++ )
    {
      for ( y = 0 ; y < AUTOMAP_SQUARE_SIZE ; y ++ )
	{
	  putpixel ( Screen , AUTOMAP_SQUARE_SIZE * Me [ 0 ] . pos . x + x , AUTOMAP_SQUARE_SIZE * Me [ 0 ] . pos . y + y , TuxColor );
	  
	  for ( i = 1 ; i < MAX_PLAYERS ; i ++ )
	    {
	      //--------------------
	      // We don't blit other players, that are either dead or not
	      // on this level...
	      //
	      if ( Me [ i ] . pos . z != Me [ 0 ] . pos . z ) continue;
	      if ( Me [ i ] . status == OUT  ) continue;

	      putpixel ( Screen , AUTOMAP_SQUARE_SIZE * Me [ i ] . pos . x + x , AUTOMAP_SQUARE_SIZE * Me [ i ] . pos . y + y , FriendColor );
	    }
	}
    }

}; // void ShowAutomapData( void )

/* ----------------------------------------------------------------------
 * There is more than one approach to the problem of disruptor flashes.
 * (*) One solution is to just completely fill the visible screen white and
 *     black altenatingly.
 * (*) The other solution is to start in the center and then recursively
 *     proceed through the passable tiles and using this method fill 
 *     exactly the whole room where you're currently in.  That is perhaps
 *     the more sophisticated method.  Right now however, it's disabled.
 * 
 * ---------------------------------------------------------------------- */

/*
void
RecFlashFill (int LX, int LY, int Color, unsigned char *Parameter_Screen, int SBreite)
{
  int i;
  static int num;

  LY = LY;
  LX = LX;
  num++;

//      gotoxy(1,1);
//      printf(" RFF: X=%d Y=%d.\n",LX,LY);
//      getchar();

  // mark this square as within the area of effect
  Affected[LY / Block_Height * CurLevel->xlen + LX / Block_Width] = TRUE;

  // fill this square up
  for (i = LY / 4 - ((LY / 4) % 8); i < (LY / 4 - ((LY / 4) % 8) + 8); i++)
    {
      memset (Parameter_Screen + i * SBreite + LX / 4 - ((LX / 4) % 8), Color, 8);
    }
  i -= 4;

  // fill the square to the right also up
  if ((*(Parameter_Screen + i * SBreite + LX / 4 + 8) != Color) &&
      (IsPassable (Cent (LX + Block_Width), Cent (LY), CENTER) == CENTER))
    RecFlashFill (LX + Block_Width, LY, Color, Parameter_Screen, SBreite);

  // fill the square to the left
  if (LX > Block_Width)
    {
      if ((*(Parameter_Screen + i * SBreite + LX / 4 - 8) != Color) &&
	  (IsPassable (Cent (LX - Block_Width), Cent (LY), CENTER) == CENTER))
	RecFlashFill (LX - Block_Width, LY, Color, Parameter_Screen, SBreite);
    }

  // fill the square above
  if ((i > 8) && (LY > Block_Height))
    {
      if ((*(Parameter_Screen + (i - 8) * SBreite + LX / 4) != Color) &&
	  (IsPassable (Cent (LX), Cent (LY - Block_Height), CENTER) == CENTER))
	RecFlashFill (LX, LY - Block_Height, Color, Parameter_Screen, SBreite);
    }

  // fill the square below
  if ((*(Parameter_Screen + (i + 8) * SBreite + LX / 4) != Color) &&
      (IsPassable (Cent (LX), Cent (LY + Block_Height), CENTER) == CENTER))
    RecFlashFill (LX, LY + Block_Height, Color, Parameter_Screen, SBreite);
}
*/

/* ----------------------------------------------------------------------
 * This function should display the currently assigned/unassigned mission
 * and all that directly over the combat screen without interrupting the
 * game in any other way.
 * ---------------------------------------------------------------------- */
void 
ShowMissionCompletitionMessages( void )
{
  int MissNum;

  //--------------------
  // If the log is not set to visible right now, we do not need to 
  // do anything more
  //
  if ( GameConfig.Mission_Log_Visible == FALSE ) return;
  if ( GameConfig.Mission_Log_Visible_Time >= GameConfig.Mission_Log_Visible_Max_Time ) return;

  //--------------------
  // At this point we know, that the quest log is desired and
  // therefore we display it in-game:
  //
  SDL_SetClipRect( Screen , NULL );
  DisplayText( "See quest log: \n" , User_Rect.x , User_Rect.y , &User_Rect );

  for ( MissNum = 0 ; MissNum < MAX_MISSIONS_IN_GAME; MissNum ++ )
    {
      // In case the mission does not exist at all, we need not do anything more...
      if ( Me[0].AllMissions[ MissNum ].MissionExistsAtAll != TRUE ) continue;

      // In case the mission was not yet assigned, we need not do anything more...
      // if ( Me[0].AllMissions[ MissNum ].MissionWasAssigned != TRUE ) continue;

      // In case the message is rather old, we need not do anything more...
      // if ( Me[0].AllMissions[ MissNum ].MissionLastStatusChangeTime > 1000 ) continue;

      // At this point we know, that the mission has recently been completed or failed

      if ( Me[0].AllMissions[ MissNum ].MissionIsComplete == TRUE )
	{
	  DisplayText( "\n* Mission completed: " , -1 , -1 , &User_Rect );
	}
      else if ( Me[0].AllMissions[ MissNum ].MissionWasFailed == TRUE )
	{
	  DisplayText( "\n* Mission failed: " , -1 , -1 , &User_Rect );
	}
      else if ( ! Me[0].AllMissions[ MissNum ].MissionWasAssigned == TRUE )
	{
	  DisplayText( "\n* Mission not yet assigned: " , -1 , -1 , &User_Rect );
	}
      else 
	DisplayText( "\n* Mission assigned: " , -1 , -1 , &User_Rect );

      DisplayText( Me[0].AllMissions[ MissNum ].MissionName , -1 , -1 , &User_Rect );

    }
};

/* ----------------------------------------------------------------------
 * This function displays an item at the current mouse cursor position.
 * The typical crosshair cursor is assumed.  The item is centered around
 * this crosshair cursor, depending on item size.
 * ---------------------------------------------------------------------- */
void
DisplayItemImageAtMouseCursor( int ItemImageCode )
{
  SDL_Rect TargetRect;

  if ( ItemImageCode == (-1) )
    {
      DebugPrintf( 2 , "\nCurrently no (-1 code) item held in hand.");
      return;
    }

  //--------------------
  // We define the target location for the item.  This will be the current
  // mouse cursor position of course, but -16 for the crosshair center, 
  // which is somewhat (16) to the lower right of the cursor top left 
  // corner.
  //
  // And then of course we also have to take into account the size of the
  // item, wich is also not always the same.
  //
  TargetRect.x = GetMousePos_x() + 16 - ItemImageList[ ItemImageCode ].inv_size.x * 16;
  TargetRect.y = GetMousePos_y() + 16 - ItemImageList[ ItemImageCode ].inv_size.y * 16;

  SDL_BlitSurface( ItemImageList[ ItemImageCode ].Surface , NULL , Screen , &TargetRect );
}; // void DisplayItemImageAtMouseCursor( int ItemImageCode )

/* ----------------------------------------------------------------------
 * This function displays (several) blinking warning signs as soon as item
 * durations reach critical (<5) duration level.
 * ---------------------------------------------------------------------- */
void
ShowOneItemAlarm( item* AlarmItem , int Position )
{
  SDL_Rect TargetRect;
  int ItemImageCode;

  if ( AlarmItem->type == ( -1 ) ) return;

  ItemImageCode = ItemMap [ AlarmItem->type ].picture_number ;

  TargetRect.x = 60 * Position ;
  TargetRect.y = 400;

  if ( AlarmItem->current_duration < 5 )
    {
      SDL_BlitSurface( ItemImageList[ ItemImageCode ].Surface , NULL , Screen , &TargetRect );
    }
}; // void ShowOneItemAlarm( item* AlarmItem )

/* ----------------------------------------------------------------------
 * This function displays (several) blinking warning signs as soon as item
 * durations reach critical (<5) duration level.
 * ---------------------------------------------------------------------- */
void
ShowItemAlarm( void )
{

  if ( ( ( int ) ( Me[0].MissionTimeElapsed * 2 ) ) % 2 == 1 ) return;

  ShowOneItemAlarm( & Me[0].weapon_item , 1 );
  ShowOneItemAlarm( & Me[0].drive_item , 2 );
  ShowOneItemAlarm( & Me[0].shield_item , 3 );
  ShowOneItemAlarm( & Me[0].armour_item , 4 );

}; // void ShowItemAlarm( void )

/* -----------------------------------------------------------------
 * This function assembles the contents of the combat window 
 * in Screen.
 *
 * Several FLAGS can be used to control its behaviour:
 *
 * (*) ONLY_SHOW_MAP = 1:  This flag indicates not do draw any
 *     game elements but the map blocks
 *
 * (*) DO_SCREEN_UPDATE = 2: This flag indicates for the function
 *     to also cause an SDL_Update of the portion of the screen
 *     that has been modified
 *
 * (*) ONLY_SHOW_MAP_AND_TEXT = 4: This flag indicates, that only
 *     the map and also info like the current coordinate position
 *     should be entered into the Screen.  This flag is mainly
 *     used for the level editor.
 *
 * ----------------------------------------------------------------- */
void
Assemble_Combat_Picture (int mask)
{
  int MapBrick;
  int line, col;
  int i;
  int PlayerNum;
  int minutes;
  int seconds;
  int MapInsertNr;
  float ResizeFactor;
  static float TimeSinceLastFPSUpdate=10;
  static int FPS_Displayed=1;
  SDL_Rect TargetRectangle;
  SDL_Surface *TempRescaledInsert = NULL ;
  Level DisplayLevel = curShip.AllLevels [ Me [ 0 ] . pos . z ] ;

#define UPDATE_FPS_HOW_OFTEN 0.75

  DebugPrintf (2, "\nvoid Assemble_Combat_Picture(...): Real function call confirmed.");
  
  // Why not blit the WHOLE map?  Lets try it!
  // THAT IS A VERY POWERFUL AND VERY ABSTRACT PROCEDURE:
  // * THE COMBATSCREENSIZE COULD *EASYLY* BE CHANGED WITHOUT HAVING TO CHANGE THE CODE!!!
  // 
  // Recently there were complaints about garbage outside the ship.  This was because
  // outside the ship, nothing was blittet.  Now the blitting starts at -5 tiles outside
  // the ship and ends +5 tiles outside the other end of the ship.  That should do it.
  //

  SDL_SetColorKey (Screen, 0, 0);
  // SDL_SetAlpha( Screen , 0 , SDL_ALPHA_OPAQUE ); 

                         
  SDL_SetClipRect (Screen , &User_Rect);

  for (line = -5; line < DisplayLevel->ylen + 5; line++)
    {
      for (col = -5; col < DisplayLevel->xlen + 5; col++)
	{
	  if ((MapBrick = GetMapBrick( DisplayLevel, col , line )) != INVISIBLE_BRICK)
	    {
	      TargetRectangle.x = UserCenter_x 
		+ ( -Me[0].pos.x+col-0.5 )*Block_Width;
	      TargetRectangle.y = UserCenter_y
		+ ( -Me[0].pos.y+line-0.5 )*Block_Height;
	      SDL_BlitSurface( MapBlockSurfacePointer[ DisplayLevel->color ][MapBrick] , NULL ,
 			       Screen, &TargetRectangle);
	    }			// if !INVISIBLE_BRICK 
	}			// for(col) 
    }				// for(line) 

  //--------------------
  // Now we draw the list of big graphics inserts for this level
  //
  for ( MapInsertNr = 0 ; MapInsertNr < MAX_MAP_INSERTS_PER_LEVEL ; MapInsertNr ++ )
    {
      if ( DisplayLevel->MapInsertList [ MapInsertNr ] . type == ( -1 ) ) continue;
      TargetRectangle.x = UserCenter_x 
	+ ( - Me [ 0 ] . pos . x + DisplayLevel->MapInsertList [ MapInsertNr ] . pos . x - 0.5 ) * Block_Width;
      TargetRectangle.y = UserCenter_y
	+ ( - Me [ 0 ] . pos . y + DisplayLevel->MapInsertList [ MapInsertNr ] . pos . y - 0.5 ) * Block_Height;

      if ( Block_Width != INITIAL_BLOCK_WIDTH )
	{
	  ResizeFactor = (float)Block_Width / INITIAL_BLOCK_WIDTH  ;
	  TempRescaledInsert =	      
	    zoomSurface ( AllMapInserts [ DisplayLevel->MapInsertList [ MapInsertNr ] . type ] . insert_surface , 
			  ResizeFactor , ResizeFactor , 0 );
	  SDL_BlitSurface( TempRescaledInsert , NULL , Screen , &TargetRectangle );
	  SDL_FreeSurface( TempRescaledInsert );
	}
      else
	{
	  SDL_BlitSurface( AllMapInserts [ DisplayLevel->MapInsertList [ MapInsertNr ] . type ] . insert_surface , NULL ,
			   Screen, &TargetRectangle);
	}
    }


  if (mask & ONLY_SHOW_MAP) 
    {
      // in case we only draw the map, we are done here.  But
      // of course we must check if we should update the screen too.
      if ( mask & DO_SCREEN_UPDATE ) 
	SDL_UpdateRect( Screen , User_Rect.x , User_Rect.y , User_Rect.w , User_Rect.h );
      return;
    }

  if ( ! ( mask & ONLY_SHOW_MAP_AND_TEXT ) )
    {
      //--------------------
      // At this point we know that now only the map is to be drawn.
      // so we start drawing the rest of the INTERIOR of the combat window:
      
      for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
	{
	  PutItem( i );
	}
      
      for (i = 0; i < MAX_ENEMYS_ON_SHIP ; i++)
	PutEnemy (i , -1 , -1 );
      
      //--------------------
      // Now we blit all the player tuxes...
      //
      for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
	{
	  if (Me [ PlayerNum ].energy > 0)
	    PutInfluence ( -1 , -1 , PlayerNum ); // this blits player 0 
	}
      
      for (i = 0; i < (MAXBULLETS); i++)
	if (AllBullets[i].type != OUT)
	  PutBullet (i);
      
      for (i = 0; i < (MAXBLASTS); i++)
	if (AllBlasts[i].type != OUT)
	  PutBlast (i);

      PutMouseMoveCursor ( );

      DisplayBigScreenMessage();

    } // ! ONLY_SHOW_MAP_AND_TEXT
      

  ShowAutomapData();

  if ( GameConfig.Draw_Framerate )
    {
      TimeSinceLastFPSUpdate += Frame_Time();
      if ( TimeSinceLastFPSUpdate > UPDATE_FPS_HOW_OFTEN )
	{
	  FPS_Displayed=(int)(1.0/Frame_Time());
	  TimeSinceLastFPSUpdate=0;
	}
      
      PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x , 
		       User_Rect.y+User_Rect.h - FontHeight( FPS_Display_BFont ), 
		       "FPS: %d " , FPS_Displayed );

      PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x + 100, 
		       User_Rect.y+User_Rect.h - FontHeight( FPS_Display_BFont ), 
		       "Axis: %d %d" , input_axis.x, input_axis.y);
    }

  if ( GameConfig.Draw_Energy )
    {
      PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x+User_Rect.w/2 , 
		       User_Rect.y+User_Rect.h - FontHeight( FPS_Display_BFont ), 
		       "Energy: %d " , (int) (Me[0].energy) );
      PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x+User_Rect.w/2 , 
		       User_Rect.y+User_Rect.h - 2 * FontHeight( FPS_Display_BFont ), 
		       "Resistance: %f " , (Me[0].Current_Victim_Resistance_Factor) );
    }

  if ( GameConfig.Draw_Position || ( mask & ONLY_SHOW_MAP_AND_TEXT ) )
    {
      PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x+2*User_Rect.w/3 , 
		       User_Rect.y+User_Rect.h - FontHeight( FPS_Display_BFont ), 
		       "GPS: X=%d Y=%d Lev=%d" , (int) rintf(Me[0].pos.x) , (int) rintf(Me[0].pos.y) , DisplayLevel->levelnum );
    }

  if ( Me[0].AllMissions[0].MustLiveTime != (-1) )
    {
      minutes=floor( ( Me[0].AllMissions[0].MustLiveTime - Me[0].MissionTimeElapsed ) / 60 );
      seconds= rintf( Me[0].AllMissions[0].MustLiveTime - Me[0].MissionTimeElapsed ) - 60 * minutes;
      if ( minutes < 0 ) 
	{
	  minutes = 0;
	  seconds = 0;
	}
      PrintStringFont( Screen , FPS_Display_BFont , User_Rect.x , 
		       User_Rect.y + 0*FontHeight( FPS_Display_BFont ), 
		       "Time to hold out still: %2d:%2d " , minutes , seconds );
    }

  ShowMissionCompletitionMessages();

  //--------------------
  // Here are some more things, that are not needed in the level editor
  // view...
  //
  if ( ! ( mask & ONLY_SHOW_MAP_AND_TEXT ) )
    {
      ShowItemAlarm();
      ShowCharacterScreen ( );
      ShowSkillsScreen ( );
      ManageInventoryScreen ( );
      ShowQuickInventory ();
      DisplayButtons( );
    }

  if ( ServerMode )
    CenteredPrintStringFont ( Screen , Menu_BFont , SCREEN_HEIGHT/2 , " S E R V E R ! ! ! " );

  if ( GameConfig.Inventory_Visible ) 
    {
      User_Rect.x = SCREEN_WIDTH/2;
      User_Rect.w = SCREEN_WIDTH/2;
    }
  else if ( GameConfig.CharacterScreen_Visible || GameConfig.SkillScreen_Visible ) 
    {
      User_Rect.x = 0; // SCREEN_WIDTH/2;
      User_Rect.w = SCREEN_WIDTH/2;
    }
  else
    {
      User_Rect.x = 0;
      User_Rect.w = SCREEN_WIDTH;
    }


  //--------------------
  // At this point we are done with the drawing procedure
  // and all that remains to be done is updating the screen.
  // Depending on where we did our modifications, we update
  // an according portion of the screen.
  if ( mask & DO_SCREEN_UPDATE )
    {
      SDL_UpdateRect( Screen , User_Rect.x , User_Rect.y , User_Rect.w , User_Rect.h );
    }


  DebugPrintf (2, "\nvoid Assemble_Combat_Picture(...): end of function reached.");
}; // void Assemble_Combat_Picture(...)

/* ----------------------------------------------------------------------
 * This function blits robot digits into the robot.  It can do so for 
 * friendly droids as well as for hostile ones.
 * ---------------------------------------------------------------------- */
void
BlitRobotDigits( point UpperLeftBlitCorner , char* druidname , int is_friendly )
{
  SDL_Rect TargetRectangle;
  int HumanModifier;
  int i;

  if ( druidname[ 0 ] >= 'A' ) HumanModifier = 'A' - '1' - 13 - 10 * (!is_friendly) ;
  else HumanModifier = 0 ;

  for ( i = 0 ; i < 3 ; i ++ )
    {
      TargetRectangle.x = UpperLeftBlitCorner.x + Digit_Pos[i].x ;
      TargetRectangle.y = UpperLeftBlitCorner.y + Digit_Pos[i].y ;
      if ( is_friendly == 0 )
	{
	  SDL_BlitSurface( EnemyDigitSurfacePointer[ druidname[i]-'1'+1+HumanModifier ] , 
			   NULL, Screen, &TargetRectangle );
	}
      else
	{
	  SDL_BlitSurface( InfluDigitSurfacePointer[ druidname[i]-'1'+1+HumanModifier ] , 
			   NULL, Screen, &TargetRectangle );
	}
    }
}; // void 

/* -----------------------------------------------------------------
 * This function draws the mouse move cursor.
 * ----------------------------------------------------------------- */
void
PutMouseMoveCursor ( void )
{
  SDL_Rect TargetRectangle;

  if ( ( Me [ 0 ] . mouse_move_target . x == (-1) ) &&
       ( Me [ 0 ] . mouse_move_target_is_enemy == (-1) ) )
    {
      // DebugPrintf ( 0 , "\nMouse Move Target: x == (-1) ! and NO ENEMY TARGETED!! " );
      return;
    }

  if ( Me [ 0 ] . mouse_move_target_is_enemy == (-1) )
    {
      TargetRectangle . x = UserCenter_x - 
	( Me[0].pos.x - Me [ 0 ] . mouse_move_target . x ) * Block_Width  - Block_Width  / 2  ;
      TargetRectangle . y = UserCenter_y - 
	( Me[0].pos.y - Me [ 0 ] . mouse_move_target . y ) * Block_Height - Block_Height / 2 ;
    }
  else
    {
      TargetRectangle . x = UserCenter_x - 
	( Me[0].pos.x - AllEnemys [ Me [ 0 ] . mouse_move_target_is_enemy ] . pos . x ) * Block_Width  - Block_Width  / 2  ;
      TargetRectangle . y = UserCenter_y - 
	( Me[0].pos.y - AllEnemys [ Me [ 0 ] . mouse_move_target_is_enemy ] . pos . y ) * Block_Height - Block_Height / 2 ;
    }

  if ( Me [ 0 ] . mouse_move_target_is_enemy == (-1) )
    SDL_BlitSurface ( MouseCursorImageList[ 0 ] , NULL , Screen , &TargetRectangle);
  else
    SDL_BlitSurface ( MouseCursorImageList[ 1 ] , NULL , Screen , &TargetRectangle);

}; // void PutMouseMoveCursor ( void )

/* -----------------------------------------------------------------
 * This function draws the influencer to the screen, either
 * to the center of the combat window if (-1,-1) was specified, or
 * to the specified coordinates anywhere on the screen, useful e.g.
 * for using the influencer as a cursor in the menus.
 *
 * The given coordinates then indicate the UPPER LEFT CORNER for
 * the blit.
 * ----------------------------------------------------------------- */
void
PutInfluence ( int x , int y , int PlayerNum )
{
  SDL_Rect TargetRectangle;
  SDL_Rect Text_Rect;
  int alpha_value;
  int i;
  int use_tux = TRUE;
  point UpperLeftBlitCorner;
  float angle;
  static float Previous_angle [ MAX_PLAYERS ]  = { -1000 , -1000 , -1000 , -1000 , -1000 } ; // a completely unrealistic value
  static SDL_Surface* tmp_influencer [ MAX_PLAYERS ]  = { NULL , NULL , NULL , NULL , NULL };
  static int Previous_phase [ MAX_PLAYERS ] = { -100 , -100 , -100 , -100 , -100 } ; // a completely unrealistic value
  moderately_finepoint in_tile_shift;

  Text_Rect.x=UserCenter_x + Block_Width/3;
  Text_Rect.y=UserCenter_y  - Block_Height/2;
  Text_Rect.w=User_Rect.w/2 - Block_Width/3;
  Text_Rect.h=User_Rect.h/2;

  DebugPrintf ( 2 , "\nvoid PutInfluence(void): real function call confirmed." ) ;

  if ( x == -1 ) 
    {
      //--------------------
      // The (-1) parameter indicates, that the tux should be drawn right 
      // into the game field at it's apropriate location.
      //
      // Well, for game purposes, we do not need to blit anything if the
      // tux is out, so we'll query for that first, as well as for the case
      // of other players that are not on this level.
      //
      if ( Me [ PlayerNum ] . status == OUT ) return;
      if ( Me [ PlayerNum ] . pos . z != Me [ 0 ] . pos . z ) return;
      

      UpperLeftBlitCorner.x = UserCenter_x - Block_Width  / 2 ;
      UpperLeftBlitCorner.y = UserCenter_y - Block_Height / 2 ;

    }
  else
    {
      //--------------------
      // The not (-1) parameter indicates, that the tux should be drawn 
      // for cursor purposes.  This will be done anyway, regardless of
      // whether the tux is currently out or not.
      //
      UpperLeftBlitCorner.x=x ;
      UpperLeftBlitCorner.y=y ;
    }

  TargetRectangle.x = UpperLeftBlitCorner.x ;
  TargetRectangle.y = UpperLeftBlitCorner.y ;


  //--------------------
  // Maybe the influencer is fading due to low energy?
  // to achive this, is might be nescessary to add some 
  // alpha to the surface, that will later be
  // removed again.  We do this here:
  //
  
#define alpha_offset 80
  if ( ( ( Me [ PlayerNum ].energy * 100 / Me [ PlayerNum ].maxenergy ) <= BLINKENERGY ) && ( x == (-1) ) ) 
    {

      // In case of low energy, do the fading effect...
      alpha_value = (int) ( ( 256 - alpha_offset ) * 
			    fabsf( 0.5 * Me [ PlayerNum ].MissionTimeElapsed - floor( 0.5 * Me [ PlayerNum ].MissionTimeElapsed ) - 0.5 ) + 
			    ( alpha_offset ) );

      for ( i = 0 ; i < DIGITNUMBER ; i++ )
	SDL_SetAlpha( InfluDigitSurfacePointer[i] , SDL_SRCALPHA , alpha_value );

      // ... and also maybe start a new cry-sound

      if ( Me [ PlayerNum ].LastCrysoundTime > CRY_SOUND_INTERVAL )
	{
	  Me [ PlayerNum ].LastCrysoundTime = 0;
	  CrySound();
	}
    }
  else
    {
      for ( i = 0 ; i < DIGITNUMBER ; i++ )
	SDL_SetAlpha( InfluDigitSurfacePointer[i] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
    }

  //--------------------
  // In case of transfer mode, we produce the transfer mode sound
  // but of course only in some periodic intervall...

  if ( Me [ PlayerNum ].status == TRANSFERMODE )
    {
      if ( Me [ PlayerNum ].LastTransferSoundTime > TRANSFER_SOUND_INTERVAL )
	{
	  Me [ PlayerNum ].LastTransferSoundTime = 0;
	  TransferSound();
	}
    }

  //--------------------
  // Either we draw the classical influencer or we draw the more modern
  // tux, a descendant of the influencer :)
  //
  if ( use_tux )
    {
      //--------------------
      // If we make the angle dependent upon direction of movement we use
      // angle = - ( atan2 (Me [ PlayerNum ].speed.y,  Me [ PlayerNum ].speed.x) * 180 / M_PI + 90 );
      //
      // But currently, we use as the angle the current location of the mouse on the local
      // client for the first player,
      // but for other players, we use the last known mouse possision as reported by the server
      //
      if ( PlayerNum == 0 ) 
	{
	  angle = - ( atan2 ( input_axis.y,  input_axis.x ) * 180 / M_PI + 90 );
	}
      else
	{
	  angle = - ( atan2 ( Me [ PlayerNum ] . LastMouse_Y ,  Me [ PlayerNum ] . LastMouse_X ) * 180 / M_PI + 90 );
	}

      //--------------------
      // Now we see if we must re-rotate the tux for this player...
      //
      if ( ( angle != Previous_angle [ PlayerNum ] ) || ( tmp_influencer [ PlayerNum ] == NULL ) || ( ( (int) Me [ PlayerNum ].phase) != Previous_phase [ PlayerNum ] ) )
	{
	  if ( tmp_influencer [ PlayerNum ] != NULL ) SDL_FreeSurface( tmp_influencer[ PlayerNum ] );
	  tmp_influencer [ PlayerNum ] = 
	    rotozoomSurface( TuxWorkingCopy [ PlayerNum ] [ ((int) Me [ PlayerNum ].phase) ] , angle , 1.0 , FALSE );
	  Previous_angle [ PlayerNum ] = angle;
	  Previous_phase [ PlayerNum ] = (int) Me [ PlayerNum ].phase;
	}
      // SDL_SetColorKey ( tmp_influencer [ PlayerNum ], SDL_SRCCOLORKEY, SDL_MapRGB ( tmp_influencer[ PlayerNum ]->format , 255 , 0 , 255 ) ); 
      SDL_SetColorKey ( tmp_influencer [ PlayerNum ] , 0 , SDL_MapRGB ( tmp_influencer [ PlayerNum ]->format , 255 , 0 , 255 ) ); // turn off colorkey
      SDL_SetAlpha( TuxMotionArchetypes[5][i] , SDL_SRCALPHA , 0 );


      //--------------------
      // The rotation may of course have changed the dimensions of the
      // block to be blitted, so we must adapt the blit target coordinates
      // accoridngly
      //
      in_tile_shift.x = 0 ;
      in_tile_shift.y = - Block_Height/2 ; // tux is half a tile lower the tux_tile center
      RotateVectorByAngle ( & in_tile_shift , angle );
	  
      if ( x == -1 ) 
	{
	  // TargetRectangle.x = UserCenter_x - tmp_influencer [ PlayerNum ]->w / 2 + in_tile_shift.x ;
	  // TargetRectangle.y = UserCenter_y - tmp_influencer [ PlayerNum ]->h / 2 + in_tile_shift.y ;
	  TargetRectangle.x = UserCenter_x - tmp_influencer[ PlayerNum ]->w / 2 + in_tile_shift.x +
	    ( ( - Me[0].pos.x + Me[ PlayerNum ].pos.x ) ) * Block_Width;
	  TargetRectangle.y = UserCenter_y - tmp_influencer[ PlayerNum ]->h / 2 + in_tile_shift.y +
	    ( ( - Me[0].pos.y + Me[ PlayerNum ].pos.y ) ) * Block_Width;


	}
      else
	{
	  TargetRectangle.x = x - tmp_influencer[ PlayerNum ]->w / 2 + in_tile_shift.x;
	  TargetRectangle.y = y - tmp_influencer[ PlayerNum ]->h / 2 + in_tile_shift.y;
	}
      
      SDL_BlitSurface( tmp_influencer[ PlayerNum ] , NULL , Screen, &TargetRectangle );
      // SDL_FreeSurface( tmp_influencer[ PlayerNum ] );
    }
  else
    {
      //--------------------
      // Now we draw the hat and shoes of the influencer
      // and the digits of the influencers current number.
      //
      SDL_BlitSurface( InfluencerSurfacePointer[ (int) floorf (Me [ PlayerNum ].phase) ], NULL , Screen, &TargetRectangle );
      BlitRobotDigits( UpperLeftBlitCorner , Druidmap[ Me [ PlayerNum ].type ].druidname , TRUE );
    }


  //--------------------
  // Now that all fading effects are done, we can restore the blocks surface to OPAQUE,
  // which is the oposite of TRANSPARENT :)
  //

  //--------------------
  // Maybe the influencer has something to say :)
  // so let him say it..
  //
  if ( ( x == (-1) ) && ( Me [ PlayerNum ].TextVisibleTime < GameConfig.WantedTextVisibleTime ) && GameConfig.All_Texts_Switch )
    {
      //      PutStringFont ( Screen , FPS_Display_BFont , 
      //		      User_Rect.x+(User_Rect.w/2) + Block_Width/3 , 
      //		      User_Rect.y+(User_Rect.h/2) - Block_Height/2 ,  
      //		      Me [ PlayerNum ].TextToBeDisplayed );
      SetCurrentFont( FPS_Display_BFont );
      DisplayText( Me [ PlayerNum ].TextToBeDisplayed , UserCenter_x + Block_Width/3,
		   UserCenter_y - Block_Height/2 , &Text_Rect );
    }

  DebugPrintf (2, "\nvoid PutInfluence(void): enf of function reached.");

}; // void PutInfluence( int x , int y )

/* ----------------------------------------------------------------------
 * If the corresponding configuration flag is enabled, enemies might 'say'
 * some text comment on the screen, like 'ouch' or 'i'll getch' or 
 * something else more sensible.  This function is here to blit these
 * comments, that must have been set before, to the screen.
 * ---------------------------------------------------------------------- */
void
PrintCommentOfThisEnemy ( int Enum , int x , int y )
{
  //--------------------
  // At this point we can assume, that the enemys has been blittet to the
  // screen, whether it's a friendly enemy or not.
  // 
  // So now we can add some text the enemys says.  That might be fun.
  //
  if ( (x == -1)
       && ( AllEnemys[Enum].TextVisibleTime < GameConfig.WantedTextVisibleTime )
       && GameConfig.All_Texts_Switch )
    {
      PutStringFont ( Screen , FPS_Display_BFont , 
		      UserCenter_x + Block_Width/3
		      + (AllEnemys[Enum].pos.x - Me[0].pos.x) * Block_Width ,  
		      UserCenter_y - Block_Height/2
		      + (AllEnemys[Enum].pos.y - Me[0].pos.y) * Block_Height ,  
		      AllEnemys[Enum].TextToBeDisplayed );
    }

}; // void PrintCommentOfThisEnemy ( int Enum, int x, int y )

/* ----------------------------------------------------------------------
 * Not every enemy has to be blitted onto the combat screen every time.
 * This function is here to find out whether this enemy has to be blitted
 * or whether we can skip it.
 * ---------------------------------------------------------------------- */
int
ThisEnemyNeedsToBeBlitted ( int Enum , int x , int y )
{

  /*
  // if enemy is on other level, return 
  if ( AllEnemys[Enum].pos.z != CurLevel->levelnum )
    {
      // DebugPrintf (3, "\nvoid PutEnemy(int Enum): DIFFERENT LEVEL-->usual end of function reached.\n");
      return;
    }
  */

  // if enemy is on other level, return 
  if ( AllEnemys[Enum].pos.z != Me [ 0 ] . pos . z )
    {
      // DebugPrintf (3, "\nvoid PutEnemy(int Enum): DIFFERENT LEVEL-->usual end of function reached.\n");
      return FALSE;
    }

  // if enemy is of type (-1), return 
  if ( AllEnemys[Enum].type == ( -1 ) )
    {
      // DebugPrintf (3, "\nvoid PutEnemy(int Enum): DIFFERENT LEVEL-->usual end of function reached.\n");
      return FALSE ;
    }

  // if the enemy is outside of the current map, that's an error and needs to be correted.
  if ( ( AllEnemys[Enum]. pos . x <= 0 ) || 
       ( AllEnemys[Enum]. pos . x >= curShip.AllLevels[ AllEnemys[Enum].pos.z ]->xlen ) ||
       ( AllEnemys[Enum]. pos . y <= 0 ) || 
       ( AllEnemys[Enum]. pos . y >= curShip.AllLevels[ AllEnemys[Enum].pos.z ]->ylen ) )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
There was a droid found outside the bounds of this level.\n\
\n\
This is an error and should not occur, but most likely it does since\n\
the bots are allowed some motion without respect to existing waypoints\n\
in Freedroid RPG.  \n\
\n\
But for now we'll choose not to worry and delete the bots that have\n\
somehow gotten outside of the map.\n\
----------------------------------------------------------------------\n\
\n" );
      AllEnemys[Enum].type = (-1) ;
      AllEnemys[Enum].Status = (OUT) ;
      // Terminate(ERR);
    }

  // if the enemy is out of sight, we need not do anything more here
  if ( ( ! show_all_droids ) && ( ! IsVisible ( & AllEnemys [ Enum ] . pos , 0 ) ) )
    {
      DebugPrintf (3, "\nvoid PutEnemy(int Enum): ONSCREEN=FALSE --> usual end of function reached.\n");
      return FALSE ;
    }

  return TRUE;
}; // int ThisEnemyNeedsToBeBlitted ( int Enum , int x , int y )

/* ----------------------------------------------------------------------
 * This function is here to blit the 'body' of a droid to the screen, 
 * but the 'body' in the classical paradroid-like form, i.e. some ball-
 * shaped form where the digits can later be filled in.
 * ---------------------------------------------------------------------- */
void
PutBallShapedDroidBody ( int Enum , SDL_Rect TargetRectangle )
{
  int phase = AllEnemys[Enum].phase;
  int alpha_value;
  int i;

  if ( ! AllEnemys[Enum].is_friendly ) 
    {
      if ( AllEnemys[Enum].paralysation_duration_left != 0 ) 
	{
	  SDL_BlitSurface( RedEnemySurfacePointer[ phase ] , NULL , Screen, &TargetRectangle);
	}
      else if ( AllEnemys[Enum].poison_duration_left != 0 ) 
	{
	  SDL_BlitSurface( GreenEnemySurfacePointer[ phase ] , NULL , Screen, &TargetRectangle);
	}
      else if ( AllEnemys[Enum].frozen != 0 ) 
	{
	  SDL_BlitSurface( BlueEnemySurfacePointer[ phase ] , NULL , Screen, &TargetRectangle);
	}
      else
	{
	  SDL_BlitSurface( EnemySurfacePointer[ phase ] , NULL , Screen, &TargetRectangle);
	}

    }
  else
    {
      SDL_BlitSurface( InfluencerSurfacePointer[ phase ] , NULL , Screen, &TargetRectangle);

      if ( ( ( AllEnemys[Enum].energy*100/Druidmap[AllEnemys[Enum].type].maxenergy) <= BLINKENERGY) ) 
	{
	  // In case of low energy, do the fading effect...
	  alpha_value = (int) ( ( 256 - alpha_offset ) * 
				fabsf( 0.5 * Me[0].MissionTimeElapsed - floor( 0.5 * Me[0].MissionTimeElapsed ) - 0.5 ) + 
				( alpha_offset ) );
	  for ( i = 0 ; i < DIGITNUMBER ; i++ )
	    SDL_SetAlpha( InfluDigitSurfacePointer[i] , SDL_SRCALPHA , alpha_value );
	}
      else
	{
	  for ( i = 0 ; i < DIGITNUMBER ; i++ )
	    SDL_SetAlpha( InfluDigitSurfacePointer[i] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
	}
    }

}; // void PutBallShapedDroidBody ( int Enum , SDL_Rect TargetRectangle );

/* ----------------------------------------------------------------------
 * This function is here to blit the 'body' of a droid to the screen, 
 * but the 'body' in the new and more modern sense with the 3d models
 * in various rotated forms as they are provided by Bastian.
 * This shape now depends upon the behaviour of the droid, which makes
 * everthing a little bit more complicated.
 * ---------------------------------------------------------------------- */
void
PutIndividuallyShapedDroidBody ( int Enum , SDL_Rect TargetRectangle )
{
  int phase = AllEnemys[Enum].phase;
  float angle;
  int alpha_value;
  int i;
  int RotationModel;
  int RotationIndex;

  if ( ! AllEnemys[Enum].is_friendly ) 
    {
      //--------------------
      // The phase now depends upon the direction this robot
      // is heading.
      //
      // We calsulate the angle of the vector
      //
      if ( ( fabsf ( AllEnemys[Enum].speed.y ) > 1 ) || ( fabsf ( AllEnemys[Enum].speed.x ) > 1 ) )
	{
	  angle = - ( atan2 ( AllEnemys[Enum].speed.y,  AllEnemys[Enum].speed.x) * 180 / M_PI + 90 );
	  AllEnemys[Enum].previous_angle = angle ;
	}
      else
	{
	  angle = AllEnemys[Enum].previous_angle ;
	}
      //
      // 3. We make a phase out of the current angle
      //
      RotationIndex = ( angle * ROTATION_ANGLES_PER_ROTATION_MODEL / 360 ) ;
      while ( RotationIndex < 0  ) RotationIndex += ROTATION_ANGLES_PER_ROTATION_MODEL ; // just to make sure... a modulo ROTATION_ANGLES_PER_ROTATION_MODEL operation can't hurt
      while ( RotationIndex >= ROTATION_ANGLES_PER_ROTATION_MODEL ) RotationIndex -= ROTATION_ANGLES_PER_ROTATION_MODEL ; // just to make sure... a modulo ROTATION_ANGLES_PER_ROTATION_MODEL operation can't hurt
      // DebugPrintf ( 0 , "\nCurrent angle: %f Current RotationIndex: %d. " , angle, RotationIndex );
      RotationModel = Druidmap [ AllEnemys [ Enum ] . type ] . individual_shape_nr ;

      //--------------------
      // First we check if the robot is still alive.  If it isn't, 
      // then we can use the explosion dust from the classic ball-shaped
      // version.
      //
      if ( phase != DROID_PHASES )
	{
	  if ( AllEnemys[Enum].paralysation_duration_left != 0 ) 
	    {
	      SDL_BlitSurface( RedEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] , NULL , Screen, &TargetRectangle);
	    }
	  else if ( AllEnemys[Enum].poison_duration_left != 0 ) 
	    {
	      SDL_BlitSurface( GreenEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] , NULL , Screen, &TargetRectangle);
	    }
	  else if ( AllEnemys[Enum].frozen != 0 ) 
	    {
	      SDL_BlitSurface( BlueEnemyRotationSurfacePointer [ RotationModel ] [ RotationIndex ] , NULL , Screen, &TargetRectangle);
	    }
	  else
	    {
	      SDL_BlitSurface( EnemyRotationSurfacePointer[ RotationModel ] [ RotationIndex ] , NULL , Screen, &TargetRectangle);
	    }
	}
      else
	{
	  SDL_BlitSurface( InfluencerSurfacePointer[ phase ] , NULL , Screen, &TargetRectangle);
	  
	  if ( ( ( AllEnemys[Enum].energy*100/Druidmap[AllEnemys[Enum].type].maxenergy) <= BLINKENERGY) ) 
	    {
	      // In case of low energy, do the fading effect...
	      alpha_value = (int) ( ( 256 - alpha_offset ) * 
				    fabsf( 0.5 * Me[0].MissionTimeElapsed - floor( 0.5 * Me[0].MissionTimeElapsed ) - 0.5 ) + 
				    ( alpha_offset ) );
	      for ( i = 0 ; i < DIGITNUMBER ; i++ )
		SDL_SetAlpha( InfluDigitSurfacePointer[i] , SDL_SRCALPHA , alpha_value );
	}
	  else
	    {
	      for ( i = 0 ; i < DIGITNUMBER ; i++ )
		SDL_SetAlpha( InfluDigitSurfacePointer[i] , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
	    }
	}
      
    }
  else
    {
      //--------------------
      // Only if the robot is dead already, we can print
      // out the explosion dust like in the classic ball shaped version.
      //
      SDL_BlitSurface( EnemySurfacePointer[ phase ] , NULL , Screen, &TargetRectangle);
    }

}; // void PutIndividuallyShapedDroidBody ( int Enum , SDL_Rect TargetRectangle );

/* ----------------------------------------------------------------------
 * This function draws an enemy into the combat window.
 * The only parameter given is the number of the enemy within the
 * AllEnemys array. Everything else is computed in here.
 * ---------------------------------------------------------------------- */
void
PutEnemy (int Enum , int x , int y)
{
  char *druidname;	/* the number-name of the Enemy */
  SDL_Rect TargetRectangle;
  point UpperLeftBlitCorner;

  DebugPrintf (3, "\nvoid PutEnemy(int Enum): real function call confirmed...\n");

  if ( ! ThisEnemyNeedsToBeBlitted ( Enum , x , y ) ) return;

  DebugPrintf ( 3 , "\nvoid PutEnemy(int Enum): it seems that we must draw this one on the screen....\n" );

  //--------------------
  // We check for incorrect droid types, which sometimes might occor, especially after
  // heavy editing of the crew initialisation functions ;)
  //
  if ( AllEnemys[Enum].type >= Number_Of_Droid_Types )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
There was a droid type on this level, that does not really exist.\n\
\n\
We might use a fallback to shortly work around this problem.  That would\n\
not be difficult.  But for now Freedroid will terminate to draw attention \n\
to the sound problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
      AllEnemys[Enum].type = 0;
      Terminate(ERR);
    }

  //--------------------
  // Since we will need that several times in the sequel, we find out the correct
  // target location on the screen for our surface blit once and remember it for
  // later.  ( THE TARGET RECTANGLE GETS MODIFIED IN THE SDL BLIT!!! )
  //
  if ( x == (-1) ) 
    {
      UpperLeftBlitCorner.x = UserCenter_x + 
	( ( - Me[0].pos.x + AllEnemys[Enum].pos.x ) ) * Block_Width  - Block_Width / 2 ;
      UpperLeftBlitCorner.y = UserCenter_y + 
	( ( - Me[0].pos.y + AllEnemys[Enum].pos.y ) ) * Block_Height - Block_Height / 2 ;
    }
  else
    {
      UpperLeftBlitCorner.x = x ;
      UpperLeftBlitCorner.y = y ;
    }

  //--------------------
  // First blit just the enemy hat and shoes.
  // The number will be blittet later
  //
  druidname = Druidmap[AllEnemys[Enum].type].druidname;

  TargetRectangle.x = UpperLeftBlitCorner.x ;
  TargetRectangle.y = UpperLeftBlitCorner.y ;
  // DebugPrintf( 0 , "X: %d." , TargetRectangle.x ); fflush(stdout);

  // PutBallShapedDroidBody ( Enum , TargetRectangle );
  PutIndividuallyShapedDroidBody ( Enum , TargetRectangle );


  // if this enemy is dead, we need not do anything more here
  if (AllEnemys[Enum].Status == OUT)
    {
      // DebugPrintf (3, "\nvoid PutEnemy(int Enum): STATUS==OUT --> usual end of function reached.\n");
      return;
    }

  //--------------------
  // Now the numbers should be blittet.
  //
  BlitRobotDigits( UpperLeftBlitCorner , druidname , AllEnemys[Enum].is_friendly );

  PrintCommentOfThisEnemy ( Enum , x , y );

  DebugPrintf (2, "\nvoid PutEnemy(int Enum): ENEMY HAS BEEN PUT --> usual end of function reached.\n");
}; // void PutEnemy(int Enum , int x , int y) 

/* ----------------------------------------------------------------------
 * This function draws a Bullet into the combat window.  The only 
 * parameter given is the number of the bullet in the AllBullets 
 * array. Everything else is computed in here.
 * ---------------------------------------------------------------------- */
void
PutBullet (int BulletNummer)
{
  Bullet CurBullet = &AllBullets[BulletNummer];
  SDL_Rect TargetRectangle;
  int PhaseOfBullet;
  // int i;
  SDL_Surface* tmp;

  DebugPrintf (2, "\nvoid PutBullet(int BulletNummer): real function call confirmed.\n");

  //--------------------
  // in case our bullet is of the type "FLASH", we only
  // draw a big white or black rectangle right over the 
  // combat window, white for even frames and black for 
  // odd frames.
  if (CurBullet->type == FLASH)
    {
      // Now the whole window will be filled with either white
      // or black each frame until the flash is over.  (Flash 
      // deletion after some time is done in CheckBulletCollisions.)
      if ( (CurBullet->time_in_frames % 2) == 1)
	{
	  FlashWindow (flashcolor1);
	  return;
	}
      if ( (CurBullet->time_in_frames % 2) == 0)
	{
	  FlashWindow (flashcolor2);
	  return;
	}
    } // if type == FLASH


  // DebugPrintf( 0 , "\nBulletType before calculating phase : %d." , CurBullet->type );
  if ( CurBullet->type >= Number_Of_Bullet_Types ) 
    {
      fprintf (stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The PutBullet function should blit a bullet of a type that does not\n\
exist at all.  THIS IS A SEVERE INTERNAL BUG IN FREEDROID.  IF YOU EVER\n\
ENCOUNTER THIS MESSAGE, PLEASE CONTACT THE DEVELOPERS AND TELL THEM\n\
ABOUT THIS ERROR MESSAGE!! THANKS A LOT.
\n\
Freedroid will terminate now to point at the error.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
      Terminate( ERR );
    };

  PhaseOfBullet = (CurBullet->time_in_seconds * Bulletmap[ CurBullet->type ].phase_changes_per_second );

  PhaseOfBullet = PhaseOfBullet % Bulletmap[CurBullet->type].phases ;

  // DebugPrintf( 0 , "\nPhaseOfBullet: %d.", PhaseOfBullet );

  // #define ONE_ROTATION_ONLY
#undef ONE_ROTATION_ONLY
#ifdef ONE_ROTATION_ONLY
  //--------------------
  // Maybe it's the first time this bullet is displayed.  But then, the images
  // of the rotated bullet in all phases are not yet attached to the bullet.
  // Then, we'll have to generate these
  //
  //if ( CurBullet->time_in_frames == 1 )
  if ( !CurBullet->Surfaces_were_generated )
    {
      for ( i=0; i<Bulletmap[ CurBullet->type ].phases ; i++ )
	{
	  CurBullet->SurfacePointer[i] = 
	    rotozoomSurface( Bulletmap[CurBullet->type].SurfacePointer[ i ] , CurBullet->angle , 1.0 , FALSE );
	}
      DebugPrintf( 1 , "\nvoid PutBullet(i): This was the first time for this bullet, so images were generated... angle=%f" , CurBullet->angle);
      CurBullet->Surfaces_were_generated=TRUE;
    }

  // WARNING!!! PAY ATTENTION HERE!! After the rotozoom was applied to the image, it is NO
  // LONGER of dimension Block_Width times Block_Height, but of the dimesions of the smallest
  // rectangle containing the full rotated Block_Height x Block_Width rectangle!!!
  // This has to be taken into account when calculating the target position for the 
  // blit of these surfaces!!!!
  TargetRectangle.x = UserCenter_x
    - (Me[0].pos.x-CurBullet->pos.x)*Block_Width-CurBullet->SurfacePointer[ PhaseOfBullet ]->w/2;
  TargetRectangle.y = UserCenter_y
    - (Me[0].pos.y-CurBullet->pos.y)*Block_Width-CurBullet->SurfacePointer[ PhaseOfBullet ]->h/2;

  SDL_BlitSurface( CurBullet->SurfacePointer[ PhaseOfBullet ] , NULL, Screen , &TargetRectangle );
#else
  tmp = rotozoomSurface( Bulletmap[CurBullet->type].SurfacePointer[ PhaseOfBullet ] , CurBullet->angle , 1.0 , FALSE );

  // WARNING!!! PAY ATTENTION HERE!! After the rotozoom was applied to the image, it is NO
  // LONGER of dimension Block_Width times Block_Height, but of the dimesions of the smallest
  // rectangle containing the full rotated Block_Height x Block_Width rectangle!!!
  // This has to be taken into account when calculating the target position for the 
  // blit of these surfaces!!!!
  TargetRectangle.x = UserCenter_x
    - (Me[0].pos.x-CurBullet->pos.x)*Block_Width - tmp -> w / 2 ;
  TargetRectangle.y = UserCenter_y
    - (Me[0].pos.y-CurBullet->pos.y)*Block_Width - tmp -> h / 2 ;

  SDL_BlitSurface( tmp , NULL, Screen , &TargetRectangle );
  SDL_FreeSurface( tmp );
  CurBullet->Surfaces_were_generated = FALSE ;

#endif

  DebugPrintf ( 1 , "\nvoid PutBullet(int BulletNummer): end of function reched.\n");

}; // void PutBullet (int Bulletnumber )

/* ----------------------------------------------------------------------
 * This function draws an item into the combat window.
 * The only given parameter is the number of the item within
 * the AllItems array.
 * ---------------------------------------------------------------------- */
void
PutItem( int ItemNumber )
{
  Level ItemLevel = curShip . AllLevels [ Me [ 0 ] . pos . z ] ;
  Item CurItem = &ItemLevel -> ItemList [ ItemNumber ] ;
  SDL_Rect TargetRectangle;
  
  if ( CurItem->type == ( -1 ) ) return;

  // We don't blit any item, that we're currently holding in our hand, do we?
  if ( CurItem->currently_held_in_hand == TRUE ) return;

  TargetRectangle . x = UserCenter_x - ( Me [ 0 ] . pos . x - CurItem -> pos . x ) * Block_Width  - 
    ( 16 * ItemImageList [ ItemMap [ CurItem -> type ] . picture_number ] . inv_size . x ) ;
  TargetRectangle . y = UserCenter_y - ( Me [ 0 ] . pos . y - CurItem -> pos . y ) * Block_Height - 
    ( 16 * ItemImageList [ ItemMap [ CurItem -> type ] . picture_number ] . inv_size . y ) ;

  SDL_BlitSurface( ItemImageList[ ItemMap[ CurItem->type ].picture_number ].Surface , NULL , Screen , &TargetRectangle);

}; // void PutItem( int ItemNumber );

/* ----------------------------------------------------------------------
 * This function draws a blast into the combat window.
 * The only given parameter is the number of the blast within
 * the AllBlasts array.
 * ---------------------------------------------------------------------- */
void
PutBlast (int BlastNummer)
{
  Blast CurBlast = &AllBlasts[BlastNummer];
  SDL_Rect TargetRectangle;

  // If the blast is already long dead, we need not do anything else here
  if (CurBlast->type == OUT)
    return;

  // DebugPrintf( 0 , "\nBulletType before calculating phase : %d." , CurBullet->type );
  if ( CurBlast->type >= ALLBLASTTYPES ) 
    {
      fprintf (stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The PutBlast function should blit a blast of a type that does not\n\
exist at all.  THIS IS A SEVERE INTERNAL BUG IN FREEDROID.  IF YOU EVER\n\
ENCOUNTER THIS MESSAGE, PLEASE CONTACT THE DEVELOPERS AND TELL THEM\n\
ABOUT THIS ERROR MESSAGE!! THANKS A LOT.
\n\
Freedroid will terminate now to point at the error.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
      Terminate( ERR );
    };

  
  TargetRectangle.x=UserCenter_x - (Me[0].pos.x - CurBlast->pos.x )*Block_Width  -Block_Width/2;
  TargetRectangle.y=UserCenter_y - (Me[0].pos.y - CurBlast->pos.y )*Block_Height -Block_Height/2;
  // Blastmap[CurBlast->type].block + ((int) floorf(CurBlast->phase)), Screen , &TargetRectangle);
  SDL_BlitSurface( Blastmap[CurBlast->type].SurfacePointer[ (int)floorf(CurBlast->phase) ] , NULL , Screen , &TargetRectangle);

}  // void PutBlast(int BlastNummer)

/* ----------------------------------------------------------------------
 * This function fills the combat window with one single color, given as
 * the only parameter to the function.
 * ---------------------------------------------------------------------- */
void
FlashWindow (SDL_Color Flashcolor)
{
  Fill_Rect( User_Rect, Flashcolor);
}; // void FlashWindow(int Flashcolor)


/* -----------------------------------------------------------------
 * Fill given rectangle with given RBG color
 * ----------------------------------------------------------------- */
void
Fill_Rect (SDL_Rect rect, SDL_Color color)
{
  Uint32 pixcolor;
  SDL_Rect tmp;

  Set_Rect (tmp, rect.x, rect.y, rect.w, rect.h);

  pixcolor = SDL_MapRGB (Screen->format, color.r, color.g, color.b);

  SDL_FillRect (Screen, &tmp, pixcolor);
  
  return;
}; // void Fill_Rect (SDL_Rect rect, SDL_Color color)

/* ----------------------------------------------------------------------
 * This function displays a robot picture.  This does NOT mean a
 * robot picture like in combat but this means a finely renderd
 * artwork by bastian, that is displayed in the console if info
 * about a robot is requested.  The only parameters to this 
 * function are the position on the screen where to blit the 
 * picture and the number of the robot in the Druidmap *NOT*
 * in AllEnemys!!
 *
 * ---------------------------------------------------------------------- */
void
ShowRobotPicture (int PosX, int PosY, int Number )
{
  SDL_Surface *tmp;
  SDL_Rect target;
  char *fpath;
  char fname[500];

  DebugPrintf (2, "\nvoid ShowRobotPicture(...): Function call confirmed.");

  // strcpy( fname, Druidmap[Number].druidname );
  strcpy( fname, "droids/" );
  strcat( fname, Druidmap[Number].portrait_filename_without_ext );
  strcat( fname , ".png" );

  fpath = find_file (fname, GRAPHICS_DIR, FALSE);

  if ( (tmp=IMG_Load (fpath)) == NULL )
    {
      fprintf (stderr,
	     "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The image file named %s could not be read by SDL.\n\
\n\
The error reason as specified by SDL is : %s.\n\
\n\
Please check that the file is present and not corrupted\n\
in your distribution of Freedroid.\n\
\n\
Freedroid will terminate now to point at the error.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , fpath , SDL_GetError() );
      Terminate (ERR);
    }
  

  SDL_SetClipRect( Screen , NULL );
  Set_Rect (target, PosX, PosY, SCREEN_WIDTH, SCREEN_HEIGHT);
  SDL_BlitSurface( tmp , NULL, Screen , &target);

  SDL_FreeSurface(tmp);

  DebugPrintf (2, "\nvoid ShowRobotPicture(...): Usual end of function reached.");
}; // void ShowRobotPicture ( ... )


/* ----------------------------------------------------------------------
 * This function displays the inventory screen and also fills in all the
 * items the influencer is carrying in his inventory and also all the 
 * items the influencer is fitted with.
 * ---------------------------------------------------------------------- */
void
ShowInventoryScreen( void )
{
  static SDL_Surface *InventoryImage = NULL;
  static SDL_Surface *TransparentPlateImage = NULL;
  SDL_Surface *tmp;
  char *fpath;
  char fname[]=INVENTORY_SCREEN_BACKGROUND_FILE;
  char fname2[]=INVENTORY_SQUARE_OCCUPIED_FILE;
  SDL_Rect TargetRect;
  int SlotNum;
  int i , j ;

  // --------------------
  // Some things like the loading of the inventory and initialisation of the
  // inventory rectangle need to be done only once at the first call of this
  // function. 
  //
  if ( InventoryImage == NULL )
    {
      // SDL_FillRect( Screen, & InventoryRect , 0x0FFFFFF );
      fpath = find_file ( fname , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      InventoryImage = SDL_DisplayFormat ( tmp );
      SDL_FreeSurface ( tmp );

      fpath = find_file ( fname2 , GRAPHICS_DIR, FALSE);
      tmp = IMG_Load( fpath );
      TransparentPlateImage = SDL_DisplayFormatAlpha ( tmp );
      SDL_FreeSurface ( tmp );

      //--------------------
      // We define the right side of the user screen as the rectangle
      // for our inventory screen.
      //
      InventoryRect.x = 0;
      // InventoryRect.y = SCREEN_HEIGHT - InventoryImage->h;
      InventoryRect.y = User_Rect.y;
      InventoryRect.w = SCREEN_WIDTH/2;
      InventoryRect.h = User_Rect.h;
    }

  //--------------------
  // At this point we know, that the inventory screen is desired and must be
  // displayed in-game:
  //
  // Into this inventory rectangle we draw the inventory mask
  //
  SDL_SetClipRect( Screen, NULL );
  SDL_BlitSurface ( InventoryImage , NULL , Screen , &InventoryRect );

  //--------------------
  // Now we display the item in the influencer drive slot
  //
  TargetRect.x = InventoryRect.x + DRIVE_RECT_X;
  TargetRect.y = InventoryRect.y + DRIVE_RECT_Y;
  if ( ( ! Me[0].drive_item.currently_held_in_hand ) && ( Me[0].drive_item.type != (-1) ) )
    {
      SDL_BlitSurface( ItemImageList[ ItemMap[ Me[0].drive_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display the item in the influencer weapon slot
  // At this point we have to pay extra care, cause the weapons in Freedroid
  // really come in many different sizes.
  //
  TargetRect.x = InventoryRect.x + WEAPON_RECT_X;
  TargetRect.y = InventoryRect.y + WEAPON_RECT_Y;
  if ( ( ! Me[0].weapon_item.currently_held_in_hand ) && ( Me[0].weapon_item.type != (-1) ) )
    {
      TargetRect.x += 32 * 0.5 * ( 2 - ItemImageList [ ItemMap[ Me[0].weapon_item.type ] . picture_number ] . inv_size . x ) ;
      TargetRect.y += 32 * 0.5 * ( 3 - ItemImageList [ ItemMap[ Me[0].weapon_item.type ] . picture_number ] . inv_size . y ) ;
      SDL_BlitSurface( ItemImageList[ ItemMap[ Me[0].weapon_item.type ].picture_number ] . Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display the item in the influencer armour slot
  //
  TargetRect.x = InventoryRect.x + ARMOUR_POS_X ;
  TargetRect.y = InventoryRect.y + ARMOUR_POS_Y ;
  if ( ( ! Me[0].armour_item.currently_held_in_hand ) && ( Me[0].armour_item.type != (-1) ) )
    {
      SDL_BlitSurface( ItemImageList[ ItemMap[ Me[0].armour_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display the item in the influencer shield slot
  //
  TargetRect.x = InventoryRect.x + SHIELD_POS_X ;
  TargetRect.y = InventoryRect.y + SHIELD_POS_Y ;
  if ( ( ! Me[0].shield_item.currently_held_in_hand ) && ( Me[0].shield_item.type != (-1) ) )
    {
      SDL_BlitSurface( ItemImageList[ ItemMap[ Me[0].shield_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }
  
  //--------------------
  // Now we display the item in the influencer special slot
  //
  TargetRect.x = InventoryRect.x + SPECIAL_POS_X ;
  TargetRect.y = InventoryRect.y + SPECIAL_POS_Y ;
  if ( ( ! Me[0].special_item.currently_held_in_hand ) && ( Me[0].special_item.type != (-1) ) )
    {
      SDL_BlitSurface( ItemImageList[ ItemMap[ Me[0].special_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display the item in the influencers aux1 slot
  //
  TargetRect.x = InventoryRect.x + AUX1_POS_X ;
  TargetRect.y = InventoryRect.y + AUX1_POS_Y ;
  if ( ( ! Me[0].aux1_item.currently_held_in_hand ) && ( Me[0].aux1_item.type != (-1) ) )
    {
      SDL_BlitSurface( ItemImageList[ ItemMap[ Me[0].aux1_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display the item in the influencers aux2 slot
  //
  TargetRect.x = InventoryRect.x + AUX2_POS_X ;
  TargetRect.y = InventoryRect.y + AUX2_POS_Y ;
  if ( ( ! Me[0].aux2_item.currently_held_in_hand ) && ( Me[0].aux2_item.type != (-1) ) )
    {
      SDL_BlitSurface( ItemImageList[ ItemMap[ Me[0].aux2_item.type ].picture_number ].Surface , NULL , Screen , &TargetRect );
    }

  //--------------------
  // Now we display all the items the influencer is carrying with him
  //
  for ( SlotNum = 0 ; SlotNum < MAX_ITEMS_IN_INVENTORY -1 ; SlotNum ++ )
    {
      // In case the item does not exist at all, we need not do anything more...
      if ( Me[0].Inventory[ SlotNum ].type == ( -1 ) ) 
	{
	  // DisplayText( "\n--- Slot empty ---" , -1 , -1 , &InventoryRect );
	  continue;
	}

      // In case the item is currently held in hand, we need not do anything more HERE ...
      if ( Me[0].Inventory[ SlotNum ].currently_held_in_hand == TRUE )
	{
	  continue;
	}

      for ( i = 0 ; i < ItemImageList[ ItemMap[ Me[0].Inventory[ SlotNum ].type ].picture_number ].inv_size.y ; i++ )
	{
	  for ( j = 0 ; j < ItemImageList[ ItemMap[ Me[0].Inventory[ SlotNum ].type ].picture_number ].inv_size.x ; j++ )
	    {
	      TargetRect.x = INVENTORY_RECT_X + 32 * ( Me[0].Inventory[ SlotNum ].inventory_position.x + j );
	      TargetRect.y = User_Rect.y + INVENTORY_RECT_Y + 32 * ( Me[0].Inventory[ SlotNum ].inventory_position.y + i );
	    
	      SDL_BlitSurface( TransparentPlateImage , NULL , Screen , &TargetRect );
	    }
	}

      TargetRect.x = INVENTORY_RECT_X + 32 * Me[0].Inventory[ SlotNum ].inventory_position.x;
      TargetRect.y = User_Rect.y +INVENTORY_RECT_Y + 32 * Me[0].Inventory[ SlotNum ].inventory_position.y;
      
      SDL_BlitSurface( ItemImageList[ ItemMap[ Me[0].Inventory[ SlotNum ].type ].picture_number ].Surface , NULL , Screen , &TargetRect );

    }
}; // void ShowInventoryScreen( void )


#undef _view_c
