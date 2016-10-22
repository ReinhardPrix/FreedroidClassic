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

/*----------------------------------------------------------------------
 *
 * Desc: level-editor functions
 *
 *----------------------------------------------------------------------*/

// ----- includes --------------------
#include "system.h"

#include "defs.h"
#include "global.h"
#include "proto.h"
#include "map.h"

// ----- defines --------------------
#define HIGHLIGHTCOLOR 255
#define HIGHLIGHTCOLOR2 100
#define ACTIVE_WP_COLOR 0x0FFFFFFFF

// ----- prototypes --------------------
void Show_Waypoints(void);
void DeleteWaypoint (level *level, int num);
void CreateWaypoint (level *level, int BlockX, int BlockY);


// ----- function definitions --------------------
/*@Function============================================================
@Desc: This function is used by the Level Editor integrated into
       freedroid.  It highlights the map position that is currently
       edited or would be edited, if the user pressed something.  I.e.
       it provides a "cursor" for the Level Editor.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Highlight_Current_Block(void)
{
  int i;

  SDL_LockSurface( ne_screen );

  for (i=0; i<Block_Rect.w; i++)
    {
      // This draws a (double) line at the upper border of the current block
      putpixel( ne_screen ,
		i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w ,
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h ,
		HIGHLIGHTCOLOR );
      putpixel( ne_screen ,
		i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w ,
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + 1 ,
		HIGHLIGHTCOLOR );

      // This draws a line at the lower border of the current block
      putpixel( ne_screen ,
		i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w ,
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y + 0.5 ) * Block_Rect.h - 1,
		HIGHLIGHTCOLOR );
      putpixel( ne_screen ,
		i + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w ,
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y + 0.5 ) * Block_Rect.h - 2,
		HIGHLIGHTCOLOR );

      // This draws a line at the left border of the current block
      putpixel( ne_screen ,
		0 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w ,
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i ,
		// User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i ,
		HIGHLIGHTCOLOR );
      putpixel( ne_screen ,
		1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x - 0.5) * Block_Rect.w ,
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i ,
		// User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i ,
		HIGHLIGHTCOLOR );

      // This draws a line at the right border of the current block
      putpixel( ne_screen ,
		-1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x + 0.5) * Block_Rect.w ,
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i ,
		// User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i ,
		HIGHLIGHTCOLOR );
      putpixel( ne_screen ,
		-2 + User_Rect.x + (User_Rect.w/2) + (rintf(Me.pos.x)-Me.pos.x + 0.5) * Block_Rect.w ,
		UserCenter_y + ( rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i ,
		// User_Rect.y + User_Rect.h/2 + (rintf(Me.pos.y)-Me.pos.y - 0.5 ) * Block_Rect.h + i ,
		HIGHLIGHTCOLOR );

      /*
	      TargetRectangle.x = UserCenter_x
		+ ( -Me.pos.x+col-0.5 )*Block_Rect.w;
	      TargetRectangle.y = UserCenter_y
		+ ( -Me.pos.y+line-0.5 )*Block_Rect.h;
	      SDL_BlitSurface( MapBlockSurfacePointer[ CurLevel->color ][MapBrick] , NULL ,
 			       ne_screen, &TargetRectangle);
      */


    }

  SDL_UnlockSurface( ne_screen );
} // void Highlight_Current_Block(void)

/*@Function============================================================
@Desc: This function is used by the Level Editor integrated into
       freedroid.  It marks all waypoints with a cross.

@Ret:  none
* $Function----------------------------------------------------------*/
void
Show_Waypoints(void)
{
  int wp;
  int i;
  int x;
  int y;
  int BlockX, BlockY;
  waypoint *this_wp;


  BlockX=rintf(Me.pos.x);
  BlockY=rintf(Me.pos.y);

  SDL_LockSurface( ne_screen );

  for (wp=0; wp<CurLevel->num_waypoints; wp++)
    {
      this_wp = &CurLevel->AllWaypoints[wp];
      //--------------------
      // Draw the cross in the middle of the middle of the tile
      //
      for (i= Block_Rect.w/4; i<3 * Block_Rect.w / 4; i++)
	{
	  // This draws a (double) line at the upper border of the current block
	  x = i + User_Rect.x+(User_Rect.w/2)- (( Me.pos.x)-this_wp->x + 0.5) * Block_Rect.w;
	  y = i + UserCenter_y - (( Me.pos.y)-this_wp->y + 0.5) * Block_Rect.h;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );


	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me.pos.x )-this_wp->x + 0.5) * Block_Rect.w;
	  y = i + UserCenter_y - (( Me.pos.y)-this_wp->y + 0.5) * Block_Rect.h + 1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );

	  // This draws a line at the lower border of the current block
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me.pos.x)-this_wp->x + 0.5) * Block_Rect.w;
	  y = -i + UserCenter_y - (( Me.pos.y )-this_wp->y - 0.5 ) * Block_Rect.h -1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );

	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me.pos.x)-this_wp->x + 0.5) * Block_Rect.w;
	  y = -i + UserCenter_y - ((Me.pos.y)-this_wp->y - 0.5 ) * Block_Rect.h -2;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( ne_screen , x , y , HIGHLIGHTCOLOR );

	}

      //--------------------
      // Draw the connections to other waypoints, BUT ONLY FOR THE WAYPOINT CURRENTLY TARGETED
      //
      if ( (BlockX == this_wp->x) && (BlockY == this_wp->y) )
	for ( i=0; i<this_wp->num_connections; i++ )
	  {
	    DrawLineBetweenTiles( this_wp->x , this_wp->y ,
				  CurLevel->AllWaypoints[this_wp->connections[i]].x ,
				  CurLevel->AllWaypoints[this_wp->connections[i]].y ,
				  HIGHLIGHTCOLOR );
	  }
    }

  SDL_UnlockSurface( ne_screen );

  return;

} // void Show_Waypoints(void);

/*@Function============================================================
@Desc: This function is provides the Level Editor integrated into
       freedroid.  Actually this function is a submenu of the big
       Escape Menu.  In here you can edit the level and upon pressing
       escape enter a further submenu where you can save the level,
       change level name and quit from level editing.

@Ret:  none
* $Function----------------------------------------------------------*/
void
LevelEditor(void)
{
  int BlockX=rintf(Me.pos.x);
  int BlockY=rintf(Me.pos.y);
  int Done=FALSE;
  int i,k;
  int SpecialMapValue;
  int OriginWaypoint = (-1);
  char* NumericInputString;
  SDL_Rect rect;
  waypoint *SrcWp;

  int KeymapOffset = 15;

  Copy_Rect (User_Rect, rect);
  Copy_Rect (Screen_Rect, User_Rect);  /// level editor can use the full screen!

  while ( !Done )
    {
      SDL_Delay(1);

      BlockX=rintf(Me.pos.x);
      BlockY=rintf(Me.pos.y);

      Fill_Rect (User_Rect, Black);
      Assemble_Combat_Picture ( ONLY_SHOW_MAP );
      Highlight_Current_Block();
      Show_Waypoints();

      // show line between a selected connection-origin and the current block
      if (OriginWaypoint != (-1) )
	DrawLineBetweenTiles( BlockX, BlockY,
			      CurLevel->AllWaypoints[OriginWaypoint].x,
			      CurLevel->AllWaypoints[OriginWaypoint].y,
			      HIGHLIGHTCOLOR2 );


      PrintStringFont (ne_screen, Font0_BFont, Full_User_Rect.x+Full_User_Rect.w/3 ,
		       Full_User_Rect.y+Full_User_Rect.h - FontHeight(Font0_BFont),
		       "Press F1 for keymap");

      SDL_Flip( ne_screen );

      //--------------------
      // If the user of the Level editor pressed some cursor keys, move the
      // highlited filed (that is Me.pos) accordingly. This is done here:
      //
      if (LeftPressedR())
	if ( rintf(Me.pos.x) > 0 ) Me.pos.x-=1;

      if (RightPressedR())
	if ( rintf(Me.pos.x) < CurLevel->xlen-1 ) Me.pos.x+=1;

      if (UpPressedR())
	if ( rintf(Me.pos.y) > 0 ) Me.pos.y-=1;

      if (DownPressedR())
	if ( rintf(Me.pos.y) < CurLevel->ylen-1 ) Me.pos.y+=1;


      if ( KeyIsPressedR (SDLK_F1) )
	{
	  k=3;
	  //	  SDL_BlitSurface ( console_bg_pic2 , NULL, ne_screen, NULL);
	  MakeGridOnScreen (NULL);
	  CenteredPutString   ( ne_screen ,  (k)*FontHeight(Menu_BFont), "Level Editor Keymap"); k+=2;
	  // DisplayText ("Use cursor keys to move around.", 1, 2 *FontHeight(Menu_BFont), NULL );
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Use cursor keys to move around." ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Use number pad to plant walls." ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Use shift and number pad to plant extras." ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "R...Refresh, 1-5...Blocktype 1-5, L...Lift" ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "F...Fine grid, T/SHIFT + T...Doors" ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "M...Alert, E...Enter tile by number" ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "Space/Enter...Floor" ); k+=2;

	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "I/O...zoom INTO/OUT OF the map" ); k+=2;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "P...toggle wayPOINT on/off" ); k++;
	  PutString ( ne_screen , KeymapOffset , (k) * FontHeight(Menu_BFont)  , "C...start/end waypoint CONNECTION" ); k++;

	  SDL_Flip ( ne_screen );
	  while (!FirePressedR() && !EscapePressedR() && !ReturnPressedR() ) SDL_Delay(1);
	}

      //--------------------
      // Since the level editor will not always be able to
      // immediately feature all the the map tiles that might
      // have been added recently, we should offer a feature, so that you can
      // specify the value of a map piece just numerically.  This will be
      // done upon pressing the 'e' key.
	  //
      if ( KeyIsPressedR ('e') )
	{
	  CenteredPutString   ( ne_screen ,  6*FontHeight(Menu_BFont), "Please enter new value: ");
	  SDL_Flip( ne_screen );
	  NumericInputString = GetString (10, 2);
	  sscanf( NumericInputString , "%d" , &SpecialMapValue );
	  if ( SpecialMapValue >= NUM_MAP_BLOCKS ) SpecialMapValue=0;
	  CurLevel->map[BlockY][BlockX]=SpecialMapValue;
	}

      //--------------------
      //If the person using the level editor decides he/she wants a different
      //scale for the editing process, he/she may say so by using the O/I keys.
      //
      if ( KeyIsPressedR ('o') )
	{
	  if (CurrentCombatScaleFactor > 0.25 )
	    CurrentCombatScaleFactor -= 0.25;
	  SetCombatScaleTo (CurrentCombatScaleFactor);
	}
      if ( KeyIsPressedR ('i') )
	{
	  CurrentCombatScaleFactor += 0.25;
	  SetCombatScaleTo (CurrentCombatScaleFactor);
	}

      // toggle waypoint on current square.  That means either removed or added.
      // And in case of removal, also the connections must be removed.
      if (KeyIsPressedR('p'))
	{
	  // find out if there is a waypoint on the current square
	  for (i=0 ; i < CurLevel->num_waypoints; i++)
	    {
	      if ( ( CurLevel->AllWaypoints[i].x == BlockX ) &&
		   ( CurLevel->AllWaypoints[i].y == BlockY ) ) break;
	    }

	  // if its waypoint already, this waypoint must be deleted.
	  if (i < CurLevel->num_waypoints)
	    DeleteWaypoint (CurLevel, i);
	  else // if its not a waypoint already, it must be made into one
	    CreateWaypoint (CurLevel, BlockX, BlockY);

	} // if 'p' pressed (toggle waypoint)

      // create a connection between waypoints.  If this is the first selected waypoint, its
      // an origin and the second "C"-pressed waypoint will be used a target.
      // If origin and destination are the same, the operation is cancelled.
      if (KeyIsPressedR ('c'))
	{
	  // Determine which waypoint is currently targeted
	  for (i=0 ; i < CurLevel->num_waypoints ; i++)
	    {
	      if ( ( CurLevel->AllWaypoints[i].x == BlockX ) &&
		   ( CurLevel->AllWaypoints[i].y == BlockY ) ) break;
	    }

	  if ( i == CurLevel->num_waypoints )
	    DebugPrintf(0, "\nSorry, no waypoint here to connect...\n");
	  else
	    {

	      if ( OriginWaypoint == (-1) )
		{
		  OriginWaypoint = i;
		  SrcWp = &(CurLevel->AllWaypoints[OriginWaypoint]);
		  if (SrcWp->num_connections < MAX_WP_CONNECTIONS)
		    DebugPrintf (0, "\nWaypoint nr. %d. selected as origin\n", i);
		  else
		    {
		      DebugPrintf (0, "\nSorry, maximal number of waypoint-connections (%d) reached!\n",
				   MAX_WP_CONNECTIONS);
		      DebugPrintf (0, "Operation not possible\n");
		      OriginWaypoint = (-1);
		      SrcWp = NULL;
		    }
		}
	      else
		{
		  if ( OriginWaypoint == i )
		    {
		      DebugPrintf(0, "\nOrigin==Target --> Connection Operation cancelled.\n");
		      OriginWaypoint = (-1);
		      SrcWp = NULL;
		    }
		  else
		    {
		      DebugPrintf(0, "\nTarget-waypoint %d selected\n Connection established!\n", i );
		      SrcWp->connections[SrcWp->num_connections] = i;
		      SrcWp->num_connections ++;
		      OriginWaypoint = (-1);
		      SrcWp = NULL;
		    }
		}
	    }

	}

      // If the person using the level editor pressed some editing keys, insert the
      // corresponding map tile.  This is done here:
      if (KeyIsPressedR ('f'))
	CurLevel->map[BlockY][BlockX]=FINE_GRID;
      if (KeyIsPressedR ('1'))
	CurLevel->map[BlockY][BlockX]=BLOCK1;
      if (KeyIsPressedR ('2'))
	CurLevel->map[BlockY][BlockX]=BLOCK2;
      if (KeyIsPressedR ('3'))
	CurLevel->map[BlockY][BlockX]=BLOCK3;
      if (KeyIsPressedR ('4'))
	CurLevel->map[BlockY][BlockX]=BLOCK4;
      if (KeyIsPressedR ('5'))
	CurLevel->map[BlockY][BlockX]=BLOCK5;
      if (KeyIsPressedR ('l'))
	CurLevel->map[BlockY][BlockX]=LIFT;
      if (KeyIsPressedR (SDLK_KP_PLUS))
	CurLevel->map[BlockY][BlockX]=V_WALL;
      if (KeyIsPressedR (SDLK_KP0))
	CurLevel->map[BlockY][BlockX]=H_WALL;
      if (KeyIsPressedR(SDLK_KP1))
	CurLevel->map[BlockY][BlockX]=ECK_LU;
      if (KeyIsPressedR (SDLK_KP2))
	{
	  if (!ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=T_U;
	  else CurLevel->map[BlockY][BlockX]=KONSOLE_U;
	    }
      if (KeyIsPressedR (SDLK_KP3))
	CurLevel->map[BlockY][BlockX]=ECK_RU;
      if (KeyIsPressedR (SDLK_KP4))
	{
	  if (!ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=T_L;
	  else CurLevel->map[BlockY][BlockX]=KONSOLE_L;
	}
      if (KeyIsPressedR (SDLK_KP5))
	{
	  if (!ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=KREUZ;
	  else CurLevel->map[BlockY][BlockX]=VOID;
	}
      if (KeyIsPressedR (SDLK_KP6))
	{
	  if (!ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=T_R;
	  else CurLevel->map[BlockY][BlockX]=KONSOLE_R;
	}
      if (KeyIsPressedR (SDLK_KP7))
	CurLevel->map[BlockY][BlockX]=ECK_LO;
      if (KeyIsPressedR (SDLK_KP8))
	{
	  if (!ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=T_O;
	  else CurLevel->map[BlockY][BlockX]=KONSOLE_O;
	}
      if (KeyIsPressedR (SDLK_KP9))
	CurLevel->map[BlockY][BlockX]=ECK_RO;
      if ( KeyIsPressedR ('m'))
	CurLevel->map[BlockY][BlockX]=ALERT_GREEN;
      if (KeyIsPressedR ('r'))
	CurLevel->map[BlockY][BlockX]=REFRESH1;
      if (KeyIsPressedR('t'))
	{
	  if (ShiftPressed())
	    CurLevel->map[BlockY][BlockX]=V_ZUTUERE;
	  else CurLevel->map[BlockY][BlockX]=H_ZUTUERE;
	}
      if ((SpacePressed() || MouseLeftPressed()))
	CurLevel->map[BlockY][BlockX]=FLOOR;

      // After Level editing is done and escape has been pressed,
      // display the Menu with level save options and all that.

      if (EscapePressedR())
	Done = LevelEditMenu();

    } // while (!Done)

  ShuffleEnemys ();  // now make sure droids get redestributed correctly!

  Copy_Rect (rect, User_Rect);

  ClearGraphMem();
  return;

} // void LevelEditor(void)



// delete given waypoint num (and all its connections) on level Lev
void
DeleteWaypoint (level *Lev, int num)
{
  int i, j;
  waypoint *WpList, *ThisWp;
  int wpmax;

  WpList = Lev->AllWaypoints;
  wpmax = Lev->num_waypoints - 1;

  // is this the last one? then just delete
  if (num == wpmax)
    WpList[num].num_connections = 0;
  else // otherwise shift down all higher waypoints
    memcpy (&WpList[num], &WpList[num+1], (wpmax - num) * sizeof(waypoint) );

  // now there's one less:
  Lev->num_waypoints --;
  wpmax --;

  // now adjust the remaining wp-list to the changes:
  ThisWp = WpList;
  for (i=0; i < Lev->num_waypoints; i++, ThisWp++)
    for (j=0; j < ThisWp->num_connections; j++)
      {
	// eliminate all references to this waypoint
	if (ThisWp->connections[j] == num)
	  {
	    // move all connections after this one down
	    memcpy (&(ThisWp->connections[j]), &(ThisWp->connections[j+1]),
		    (ThisWp->num_connections-1 - j)*sizeof(int));
	    ThisWp->num_connections --;
	    j --;  // just to be sure... check the next connection as well...(they have been shifted!)
	    continue;
	  }
	// adjust all connections to the shifted waypoint-numbers
	else if (ThisWp->connections[j] > num)
	  ThisWp->connections[j] --;

      } // for j < num_connections

} // DeleteWaypoint()

/*----------------------------------------------------------------------
 * create a new empty waypoint on position x/y
 *----------------------------------------------------------------------*/
void
CreateWaypoint (level *Lev, int x, int y)
{
  int num;

  if (Lev->num_waypoints == MAXWAYPOINTS)
    {
      DebugPrintf (0, "WARNING: maximal number of waypoints (%d) reached on this level!!\n",
		   MAXWAYPOINTS);
      DebugPrintf (0, "... cannot insert any more, sorry!\n");
      return;
    }

  num = Lev->num_waypoints;
  Lev->num_waypoints ++;

  Lev->AllWaypoints[num].x = x;
  Lev->AllWaypoints[num].y = y;
  Lev->AllWaypoints[num].num_connections = 0;

  return;
} // CreateWaypoint()

