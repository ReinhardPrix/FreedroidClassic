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
 * This file contains all functions for the heart of the level editor.
 * ---------------------------------------------------------------------- */

#define _leveleditor_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

void Show_Waypoints( int PrintConnectionList );
void Level_Editor(void);

EXTERN char Previous_Mission_Name[1000];

/* ----------------------------------------------------------------------
 * This function is used by the Level Editor integrated into 
 * freedroid.  It highlights the map position that is currently 
 * edited or would be edited, if the user pressed something.  I.e. 
 * it provides a "cursor" for the Level Editor.
 * ---------------------------------------------------------------------- */
void 
Highlight_Current_Block(void)
{
  int i;
  char PanelText[5000]="";
  int Codepanel_Index;

#define HIGHLIGHTCOLOR 255

  //--------------------
  // At first we draw all the four lines that make up the 
  // cursor in the level editor
  //
  SDL_LockSurface( Screen );
  for (i=0; i<Block_Width; i++)
    {
      // This draws a (double) line at the upper border of the current block
      putpixel( Screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height , HIGHLIGHTCOLOR );
      putpixel( Screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height + 1, HIGHLIGHTCOLOR );

      // This draws a line at the lower border of the current block
      putpixel( Screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y + 0.5 ) * Block_Height -1, HIGHLIGHTCOLOR );
      putpixel( Screen , i + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y + 0.5 ) * Block_Height -2, HIGHLIGHTCOLOR );

      // This draws a line at the left border of the current block
      putpixel( Screen , 0 + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );
      putpixel( Screen , 1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x - 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );

      // This draws a line at the right border of the current block
      putpixel( Screen , -1 + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x + 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );
      putpixel( Screen , -2 + User_Rect.x + (User_Rect.w/2) + (rintf(Me[0].pos.x)-Me[0].pos.x + 0.5) * Block_Width , User_Rect.y + User_Rect.h/2 + (rintf(Me[0].pos.y)-Me[0].pos.y - 0.5 ) * Block_Height + i , HIGHLIGHTCOLOR );

    }
  SDL_UnlockSurface( Screen );

  //--------------------
  // Now we print out the codepanel information about this tile
  // just in case it is really a codepanel.
  //
  switch ( CurLevel->map [ (int)rintf( Me[0].pos.y) ] [ (int)rintf( Me[0].pos.x ) ] )
    {
    case CODEPANEL_L:
    case CODEPANEL_R:
    case CODEPANEL_U:
    case CODEPANEL_D:

      for ( Codepanel_Index = 0 ; Codepanel_Index < MAX_CODEPANELS_PER_LEVEL ; Codepanel_Index ++ )
	{
	  if ( ( ( (int) rintf( Me[0].pos.x ) ) == CurLevel->CodepanelList[ Codepanel_Index ].x ) && 
	       ( ( (int) rintf( Me[0].pos.y ) ) == CurLevel->CodepanelList[ Codepanel_Index ].y ) )
	    break;
	}

      if ( Codepanel_Index >= MAX_CODEPANELS_PER_LEVEL )
	{
	  sprintf( PanelText , "\nWARNING!  Either no codepanel code present or last entry used.\n" );
	}
      else
	{
	  sprintf( PanelText , "\nCode Panel Information: \n Codeword=\"%s\"." , 
		   CurLevel->CodepanelList[ Codepanel_Index ].Secret_Code );
	}

      DisplayText ( PanelText , User_Rect.x , User_Rect.y , &User_Rect );
      break;
    default:
      break;
    }
} // void Highlight_Current_Block(void)

/* ----------------------------------------------------------------------
 * This function is used by the Level Editor integrated into 
 * freedroid.  It marks all waypoints with a cross.
 * ---------------------------------------------------------------------- */
void 
Show_Waypoints( int PrintConnectionList )
{
  int wp;
  int i;
  int x;
  int y;
  int BlockX, BlockY;
  int color;
  char ConnectionText[5000];
  char TextAddition[1000];

#define ACTIVE_WP_COLOR 0x0FFFFFFFF

  BlockX=rintf(Me[0].pos.x);
  BlockY=rintf(Me[0].pos.y);
	  
  SDL_LockSurface( Screen );

  for (wp=0; wp<MAXWAYPOINTS; wp++)
    {

      if ( CurLevel->AllWaypoints[wp].x == 0) continue;

      //--------------------
      // Draw the cross in the middle of the middle of the tile
      //
      for (i= Block_Width/4; i<3 * Block_Width / 4; i++)
	{
	  // This draws a (double) line at the upper border of the current block
	  x = i + User_Rect.x+(User_Rect.w/2)- (( Me[0].pos.x)-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = i + User_Rect.y+User_Rect.h/2 - (( Me[0].pos.y)-CurLevel->AllWaypoints[wp].y + 0.5) * Block_Height;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );

		    
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x )-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = i + User_Rect.y+User_Rect.h/2- (( Me[0].pos.y)-CurLevel->AllWaypoints[wp].y + 0.5) * Block_Height + 1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
	  
	  // This draws a line at the lower border of the current block
	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x)-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = -i + User_Rect.y + User_Rect.h/2 - (( Me[0].pos.y )-CurLevel->AllWaypoints[wp].y - 0.5 ) * Block_Height -1;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );

	  x = i + User_Rect.x + (User_Rect.w/2) - (( Me[0].pos.x)-CurLevel->AllWaypoints[wp].x + 0.5) * Block_Width;
	  y = -i + User_Rect.y + User_Rect.h/2 - ((Me[0].pos.y)-CurLevel->AllWaypoints[wp].y - 0.5 ) * Block_Height -2;
	  if ( ( x < User_Rect.x ) || ( x > User_Rect.x + User_Rect.w ) || ( y < User_Rect. y) || ( y > User_Rect.y + User_Rect.h ) ) continue;
	  putpixel( Screen , x , y , HIGHLIGHTCOLOR );
	  
	}

      
      //--------------------
      // Draw the connections to other waypoints, BUT ONLY FOR THE WAYPOINT CURRENTLY TARGETED
      //
      if ( PrintConnectionList )
	{
	  strcpy( ConnectionText , "List of connection for this wp:\n" );
	}

      for ( i=0; i<MAX_WP_CONNECTIONS; i++ )
	{
	  if ( CurLevel->AllWaypoints[wp].connections[i] != (-1) )
	    {
	      if ( ( BlockX == CurLevel->AllWaypoints[wp].x ) && ( BlockY == CurLevel->AllWaypoints[wp].y ) )
		{
		  // color = ACTIVE_WP_COLOR ;
		  // else color = HIGHLIGHTCOLOR ; 
		  // printf(" Found a connection!! ");
		  // printf_SDL ( Screen  , 100 , 100 , "Waypoint connection to: " );

		  
		  //--------------------
		  // If this is desired, we also print a list of connections from
		  // this waypoint to other waypoints in text form...
		  //
		  if ( PrintConnectionList )
		    {
		      SDL_UnlockSurface( Screen );
		      sprintf ( TextAddition , "To: X=%d Y=%d    " , 
				CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].x , 
				CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].y 
				);
		      strcat ( ConnectionText , TextAddition );
		      DisplayText ( ConnectionText , User_Rect.x , User_Rect.y , &User_Rect );
		      SDL_LockSurface( Screen );
		    }
		      
		  DrawLineBetweenTiles( CurLevel->AllWaypoints[wp].x , CurLevel->AllWaypoints[wp].y , 
					CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].x , 
					CurLevel->AllWaypoints[CurLevel->AllWaypoints[wp].connections[i]].y ,
					color );
		}
	    }
	}
	      
    }
  SDL_UnlockSurface( Screen );

} // void Show_Waypoints(void);

/* ----------------------------------------------------------------------
 * This function is provides the Level Editor integrated into 
 * freedroid.  Actually this function is a submenu of the big
 * Escape Menu.  In here you can edit the level and, upon pressing
 * escape, you can enter a new submenu where you can save the level,
 * change level name and quit from level editing.
 * ---------------------------------------------------------------------- */
void 
Level_Editor(void)
{
  int BlockX=rintf(Me[0].pos.x);
  int BlockY=rintf(Me[0].pos.y);
  int Done=FALSE;
  int Weiter=FALSE;
  int MenuPosition=1;
  int i,j,k;
  int SpecialMapValue;
  int NewItemCode;
  int OriginWaypoint = (-1);
  char* NumericInputString;
  char* NewCommentOnThisSquare;
  char* OldMapPointer;
  char linebuf[10000];
  int MapInsertNr;
  char* MenuTexts[ 10 ];
  char Options0[1000];
  char Options1[1000];
  char Options2[1000];
  char Options3[1000];
  char Options4[1000];
  char Options5[1000];

  char VanishingMessage[10000]="Hello";
  float VanishingMessageDisplayTime = 0;
  long OldTicks;

  SDL_Rect Editor_Window;
  enum
    { SAVE_LEVEL_POSITION=1, CHANGE_LEVEL_POSITION, CHANGE_TILE_SET_POSITION, CHANGE_SIZE_X, CHANGE_SIZE_Y, SET_LEVEL_NAME , SET_BACKGROUND_SONG_NAME , SET_LEVEL_COMMENT, QUIT_LEVEL_EDITOR_POSITION };
  
  Editor_Window.x=User_Rect.x;
  Editor_Window.y=User_Rect.y;  
  Editor_Window.w=User_Rect.w;
  Editor_Window.h=User_Rect.h;
  
  while ( !Done )
    {
      Weiter=FALSE;
      OldTicks = SDL_GetTicks ( ) ;
      while (!EscapePressed())
	{
	  BlockX=rintf(Me[0].pos.x);
	  BlockY=rintf(Me[0].pos.y);
	  
	  VanishingMessageDisplayTime += ( SDL_GetTicks ( ) - OldTicks ) / 1000.0 ;
	  OldTicks = SDL_GetTicks ( ) ;

	  ClearUserFenster();
	  Assemble_Combat_Picture ( ONLY_SHOW_MAP_AND_TEXT );
	  Highlight_Current_Block();
	  Show_Waypoints( FALSE );
	  
	  SetCurrentFont ( FPS_Display_BFont ) ;

	  //--------------------
	  // Now we print out the current status directly onto the window:
	  //
	  CenteredPutString ( Screen ,  0*FontHeight( GetCurrentFont () ),    "LEVEL EDITOR");

	  if ( OriginWaypoint == ( -1 ) )
	    {
	      sprintf ( linebuf , " Source Waypoint selected : NONE" );
	    }
	  else
	    {
	      sprintf ( linebuf , " Source Waypoint selected : X=%d Y=%d. " , 
			CurLevel -> AllWaypoints [ OriginWaypoint ] . x , 
			CurLevel -> AllWaypoints [ OriginWaypoint ] . y );
	    }
	  LeftPutString ( Screen , 4 * FontHeight( GetCurrentFont() ), linebuf );

	  //--------------------
	  // Now we print out the latest connection operation success or failure...
	  //
	  if ( VanishingMessageDisplayTime < 7 )
	    {
	      DisplayText ( VanishingMessage ,  1 , 5 * FontHeight ( GetCurrentFont () ) , NULL );
	    }


	  //--------------------
	  // Now that everything is blitted and printed, we may update the screen again...
	  //
	  SDL_Flip( Screen );

	  //--------------------
	  // If the user of the Level editor pressed some cursor keys, move the
	  // highlited filed (that is Me[0].pos) accordingly. This is done here:
	  if (LeftPressed()) 
	    {
	      if ( rintf(Me[0].pos.x) > 0 ) Me[0].pos.x-=1;
	      while (LeftPressed());
	    }
	  if (RightPressed()) 
	    {
	      if ( rintf(Me[0].pos.x) < CurLevel->xlen-1 ) Me[0].pos.x+=1;
	      while (RightPressed());
	    }
	  if (UpPressed()) 
	    {
	      if ( rintf(Me[0].pos.y) > 0 ) Me[0].pos.y-=1;
	      while (UpPressed());
	    }
	  if (DownPressed()) 
	    {
	      if ( rintf(Me[0].pos.y) < CurLevel->ylen-1 ) Me[0].pos.y+=1;
	      while (DownPressed());
	    }

	  //--------------------
	  // With the 'S' key, you can attach a statement for the influencer to 
	  // say to a given location, i.e. the location the map editor cursor
	  // currently is on.
	  //
	  if ( SPressed () )
	    {
	      while (SPressed());
	      SetCurrentFont( FPS_Display_BFont );
	      // CenteredPutString   ( Screen ,  6*FontHeight(Menu_BFont), "Please enter new value (blindly):");
	      DisplayText ( "\n Please enter comment below: \n" , -1 , -1 , &User_Rect );
	      SDL_Flip( Screen );
	      NewCommentOnThisSquare = GetString( 1000, FALSE );  // TRUE currently not implemented
	      for ( i = 0 ; i < MAX_STATEMENTS_PER_LEVEL ; i ++ )
		{
		  if ( CurLevel->StatementList[ i ].x == (-1) ) break;
		}
	      if ( i == MAX_STATEMENTS_PER_LEVEL ) 
		{
		  DisplayText ( "\nNo more free comment position.  Using first. " , -1 , -1 , &User_Rect );
		  i=0;
		  SDL_Flip ( Screen );
		  getchar_raw();
		  // Terminate( ERR );
		}

	      CurLevel->StatementList[ i ].Statement_Text = NewCommentOnThisSquare;
	      CurLevel->StatementList[ i ].x = rintf( Me[0].pos.x );
	      CurLevel->StatementList[ i ].y = rintf( Me[0].pos.y );
	    }

	  //--------------------
	  // With the 'P' key, you can edit the codepanel codeword attached to any 
	  // codepanel.  Of course the cursor must be positioned at this codepanel
	  // so the feature can work.
	  //
	  if ( PPressed () )
	    {
	      while (PPressed());
	      SetCurrentFont( FPS_Display_BFont );

	      // First we check if we really are directly on a codepanel:
	      switch ( CurLevel->map [ (int)rintf( Me[0].pos.y) ] [ (int)rintf( Me[0].pos.x ) ] )
		{
		case CODEPANEL_L:
		case CODEPANEL_R:
		case CODEPANEL_U:
		case CODEPANEL_D:

		  // If yes, we ask for the new codepanel keyword
		  DisplayText ( "\n Please enter new codepanel codeword: \n" , -1 , -1 , &User_Rect );
		  SDL_Flip( Screen );
		  NewCommentOnThisSquare = GetString( 1000, FALSE );  // TRUE currently not implemented

		  // Now we see if a codepanel entry is existing already for this square
		  for ( i = 0 ; i < MAX_CODEPANELS_PER_LEVEL ; i ++ )
		    {
		      if ( ( CurLevel->CodepanelList[ i ].x == (int)rintf( Me[0].pos.x) ) &&
			   ( CurLevel->CodepanelList[ i ].y == (int)rintf( Me[0].pos.y) ) ) break;
		    }
		  if ( i >= MAX_CODEPANELS_PER_LEVEL ) 
		    {
		      DisplayText ( "\nNo existing codepanel entry found...\n" , -1 , -1 , &User_Rect );
		      i=0;
		      for ( i = 0 ; i < MAX_CODEPANELS_PER_LEVEL ; i ++ )
			{
			  if ( CurLevel->CodepanelList[ i ].x == (-1) )
			    break;
			}
		      if ( i >= MAX_CODEPANELS_PER_LEVEL )
			{
			  DisplayText ( "\nNo more free codepanel entry found... using first\n" , -1 , -1 , &User_Rect );
			  i = 0;
			}
		      else
			{
			  DisplayText ( "\nUsing new codepanel list entry...\n" , -1 , -1 , &User_Rect );
			}
		      // Terminate( ERR );
		    }
		  else
		    {
		      DisplayText ( "\nOverwriting existing codepanel list entry...\n" , -1 , -1 , &User_Rect );

		    }
		  CurLevel->CodepanelList[ i ].Secret_Code = NewCommentOnThisSquare;
		  CurLevel->CodepanelList[ i ].x = rintf( Me[0].pos.x );
		  CurLevel->CodepanelList[ i ].y = rintf( Me[0].pos.y );


		  SDL_Flip ( Screen );
		  getchar_raw();
		  
		  break;
		default:
		  DisplayText ( "\nBut you are not on a codepanel!!\n" , -1 , -1 , &User_Rect );
		  SDL_Flip( Screen );
		  getchar_raw();
		  break;
		}
	      
	    }

	  //--------------------
	  // Since the level editor will not always be able to
	  // immediately feature all the the map tiles that might
	  // have been added recently, we should offer a feature, so that you can
	  // specify the value of a map piece just numerically.  This will be
	  // done upon pressing the 'e' key.
	  //
	  if ( EPressed () )
	    {
	      while (EPressed());
	      CenteredPutString   ( Screen ,  6*FontHeight(Menu_BFont), "Please enter new value (blindly):");
	      SDL_Flip( Screen );
	      NumericInputString=GetString( 10, FALSE );  // TRUE currently not implemented
	      sscanf( NumericInputString , "%d" , &SpecialMapValue );
	      if ( SpecialMapValue >= NUM_MAP_BLOCKS ) SpecialMapValue=0;
	      CurLevel->map[BlockY][BlockX]=SpecialMapValue;
	    }

	  //--------------------
	  // From the level editor, it should also be possible to drop new goods
	  // at some location via the 'G' key. (G like in Goods.)
	  //
	  if ( GPressed () )
	    {
	      while ( GPressed() );
	      CenteredPutString   ( Screen ,  6*FontHeight(Menu_BFont), "Please enter code of new item:");
	      SDL_Flip( Screen );
	      NumericInputString=GetString( 10, FALSE );  // TRUE currently not implemented
	      sscanf( NumericInputString , "%d" , &NewItemCode );
	      if ( NewItemCode >= Number_Of_Item_Types ) 
		{
		  NewItemCode=0;
		  
		}

	      // DropSpecificItemAtPosition( rintf( Me[0].pos.x ) , rintf( Me[0].pos.y ) , NewItemCode );
	      DropItemAt( NewItemCode , rintf( Me[0].pos.x ) , rintf( Me[0].pos.y ) , -1 , -1 , 0 );
	    }

	  //--------------------
	  //If the person using the level editor decides he/she wants a different
	  //scale for the editing process, he/she may say so by using the O/I keys.
	  //
	  if ( OPressed () )
	    {
	      if (CurrentCombatScaleFactor > 0.25 )
		CurrentCombatScaleFactor -= 0.25;
	      SetCombatScaleTo (CurrentCombatScaleFactor);
	      while (OPressed());
	    }
	  if ( IPressed () )
	    {
	      CurrentCombatScaleFactor += 0.25;
	      SetCombatScaleTo (CurrentCombatScaleFactor);
	      while (IPressed());
	    }
  
	  // If the person using the level editor pressed w, the waypoint is
	  // toggled on the current square.  That means either removed or added.
	  // And in case of removal, also the connections must be removed.
	  if (WPressed())
	    {
	      // find out if there is a waypoint on the current square
	      for (i=0 ; i < MAXWAYPOINTS ; i++)
		{
		  if ( ( CurLevel->AllWaypoints[i].x == BlockX ) &&
		       ( CurLevel->AllWaypoints[i].y == BlockY ) ) break;
		}
	      
	      // if its waypoint already, this waypoint must be deleted.
	      if ( i != MAXWAYPOINTS )
		{
		  // Eliminate the waypoint itself
		  CurLevel->AllWaypoints[i].x = 0;
		  CurLevel->AllWaypoints[i].y = 0;
		  for ( k = 0; k < MAX_WP_CONNECTIONS ; k++) 
		    CurLevel->AllWaypoints[i].connections[k] = (-1) ;

		  
		  // Eliminate all connections pointing to this waypoint
		  for ( j = 0; j < MAXWAYPOINTS ; j++ )
		    {
		      for ( k = 0; k < MAX_WP_CONNECTIONS ; k++) 
			if ( CurLevel->AllWaypoints[j].connections[k] == i )
			  CurLevel->AllWaypoints[j].connections[k] = (-1) ;
		    }
		}
	      else // if its not a waypoint already, it must be made into one
		{
		  // seek a free position
		  for ( i = 0 ; i < MAXWAYPOINTS ; i++ )
		    {
		      if ( CurLevel->AllWaypoints[i].x == 0 ) break;
		    }
		  if ( i == MAXWAYPOINTS )
		    {
		      printf("\n\nSorry, no free waypoint available.  Using the first one.");
		      i = 0;
		    }

		  // Now make the new entry into the waypoint list
		  CurLevel->AllWaypoints[i].x = BlockX;
		  CurLevel->AllWaypoints[i].y = BlockY;

		  // delete all old connection information from the new waypoint
		  for ( k = 0; k < MAX_WP_CONNECTIONS ; k++ ) 
		    CurLevel->AllWaypoints[i].connections[k] = (-1) ;

		}

	      printf("\n\n  i is now: %d ", i ); fflush(stdout);

	      while ( WPressed() );
	    }

	  // If the person using the level editor presses C that indicated he/she wants
	  // a connection between waypoints.  If this is the first selected waypoint, its
	  // an origin and the second "C"-pressed waypoint will be used a target.
	  // If origin and destination are the same, the operation is cancelled.
	  if (CPressed())
	    {
	      // Determine which waypoint is currently targeted
	      for (i=0 ; i < MAXWAYPOINTS ; i++)
		{
		  if ( ( CurLevel->AllWaypoints[i].x == BlockX ) &&
		       ( CurLevel->AllWaypoints[i].y == BlockY ) ) break;
		}

	      if ( i == MAXWAYPOINTS )
		{
		  sprintf( VanishingMessage , "\n\nSorry, don't know which waypoint you mean." );
		  VanishingMessageDisplayTime = 0;
		}
	      else
		{
		  sprintf( VanishingMessage , "\n\nYou specified waypoint nr. %d." , i );
		  VanishingMessageDisplayTime = 0;
		  if ( OriginWaypoint== (-1) )
		    {
		      strcat ( VanishingMessage , "\nIt has been marked as the origin of the next connection." );
		      OriginWaypoint = i;
		    }
		  else
		    {
		      if ( OriginWaypoint == i )
			{
			  strcat ( VanishingMessage , "\n\nOrigin==Target --> Connection Operation cancelled.");
			  OriginWaypoint = (-1);
			}
		      else
			{
			  sprintf( VanishingMessage , "\n\nOrigin: %d Target: %d. Operation makes sense.", OriginWaypoint , i );
			  for ( k = 0; k < MAX_WP_CONNECTIONS ; k++ ) 
			    {
			      if (CurLevel->AllWaypoints[ OriginWaypoint ].connections[k] == (-1) ) break;
			    }
			  if ( k == MAX_WP_CONNECTIONS ) 
			    {
			      strcat ( VanishingMessage , "\nSORRY. NO MORE CONNECTIONS AVAILABLE FROM THERE." );
			    }
			  else
			    {
			      CurLevel->AllWaypoints[ OriginWaypoint ].connections[k] = i;
			      strcat ( VanishingMessage , "\nOPERATION DONE!! CONNECTION SHOULD BE THERE." );
			    }
			  OriginWaypoint = (-1);
			}
		    }
		}

	      while (CPressed());
	      fflush(stdout);
	    }

	  //----------------------------------------------------------------------
	  // If the person using the level editor pressed some editing keys, insert the
	  // corresponding map tile.  This is done in the following large chunk of code:
	  //----------------------------------------------------------------------

	  //--------------------
	  // Pressing the 'T' key will insert a teleporter field...
	  //
	  if ( TPressed()) 
	    {
	      CurLevel -> map [ BlockY ] [ BlockX ] = TELE_1 ;
	    }

	  //--------------------
	  // Pressing the 'B' key will toggle the big graphics insert by one...
	  //
	  if ( BPressed()) 
	    {
	      //--------------------
	      // First we wait till the b key is released again...
	      //
	      while ( BPressed() );

	      //--------------------
	      // Now we go and take a look if there is some grapics insert
	      // at this location present already...
	      //
	      for ( MapInsertNr = 0 ; MapInsertNr < MAX_MAP_INSERTS_PER_LEVEL ; MapInsertNr ++ )
		{
		  if ( CurLevel->MapInsertList [ MapInsertNr ] . type == ( -1 ) ) continue; 
		  if ( CurLevel->MapInsertList [ MapInsertNr ] . pos.x != BlockX ) continue; 
		  if ( CurLevel->MapInsertList [ MapInsertNr ] . pos.y != BlockY ) continue; 
		  break;
		}

	      if ( MapInsertNr < MAX_MAP_INSERTS_PER_LEVEL )
		{
		  sprintf( VanishingMessage , " Old map insert on this position found... " );
		  VanishingMessageDisplayTime = 0 ;
		}
	      else
		{
		  sprintf( VanishingMessage , " No Old map insert on this position found. Opening new index...." );
		  VanishingMessageDisplayTime = 0 ;
		  for ( MapInsertNr = 0 ; MapInsertNr < MAX_MAP_INSERTS_PER_LEVEL ; MapInsertNr ++ )
		    {
		      if ( CurLevel->MapInsertList [ MapInsertNr ] . type == ( -1 ) ) break;
		    }
		  if ( MapInsertNr >= MAX_MAP_INSERTS_PER_LEVEL )
		    {
		      sprintf( VanishingMessage , " No new map insert index available.\n Overwriting first one..." );
		      VanishingMessageDisplayTime = 0 ;
		      MapInsertNr = 0;
		    }
		  
		  //--------------------
		  // Now we enter the right coordinates for our new map index...
		  //
		  CurLevel->MapInsertList [ MapInsertNr ] . pos.x = BlockX ;
		  CurLevel->MapInsertList [ MapInsertNr ] . pos.y = BlockY ;
		  
		}

	      //--------------------
	      // At this point we know, that we have a good map insert index at our
	      // hands.  Therefore we increase the number of the type and possible
	      // reset it to -1 if the last index was exceeded...
	      //
	      CurLevel->MapInsertList [ MapInsertNr ] . type ++ ;

	      if ( CurLevel->MapInsertList [ MapInsertNr ] . type >= MAX_MAP_INSERTS )
		CurLevel->MapInsertList [ MapInsertNr ] . type = -1;

	    }

	  //--------------------
	  // Pressing the '1' to '5' keys will insert either classic 'block'
	  // fixed map tiles or decructible 'box' map blocks.
	  //
	  if (Number1Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=BOX_1;
	      else
		CurLevel->map[BlockY][BlockX]=BLOCK1;
	    }
	  if (Number2Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=BOX_2;
	      else
		CurLevel->map[BlockY][BlockX]=BLOCK2;
	    }
	  if (Number3Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=BOX_3;
	      else
		CurLevel->map[BlockY][BlockX]=BLOCK3;
	    }
	  if (Number4Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=BOX_4;
	      else
		CurLevel->map[BlockY][BlockX]=BLOCK4;
	    }
	  if (Number5Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=BLOCK5;
	    }
	  if (LPressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=LIFT;
	    }
	  if (KP_PLUS_Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=V_WALL;
	    }
	  if (KP0Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=H_WALL;
	    }
	  if (KP1Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_LU;
	    }
	  if (KP2Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=KONSOLE_U;
	      else if ( Ctrl_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CODEPANEL_D;
	      else if ( Alt_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CONVEY_D;
	      else CurLevel->map[BlockY][BlockX]=T_U;
	    }
	  if (KP3Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_RU;
	    }
	  if (KP4Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=KONSOLE_L;
	      else if ( Ctrl_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CODEPANEL_L;
	      else if ( Alt_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CONVEY_R;
	      else CurLevel->map[BlockY][BlockX]=T_L;
	    }
	  if (KP5Pressed()) 
	    {
	      if (!Shift_Was_Pressed())
		CurLevel->map[BlockY][BlockX]=KREUZ;
	      else CurLevel->map[BlockY][BlockX]=VOID;
	    }
	  if (KP6Pressed()) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=KONSOLE_R;
	      else if ( Ctrl_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CODEPANEL_R;
	      else if ( Alt_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CONVEY_L;
	      else CurLevel->map[BlockY][BlockX]=T_R;
	    }
	  if (KP7Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_LO;
	    }
	  if ( KP8Pressed() ) 
	    {
	      if ( Shift_Was_Pressed() )
		CurLevel->map[BlockY][BlockX]=KONSOLE_O;
	      else if ( Ctrl_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CODEPANEL_U;
	      else if ( Alt_Was_Pressed() ) 
		CurLevel->map[BlockY][BlockX]=CONVEY_U;
	      else CurLevel->map[BlockY][BlockX]=T_O;
	    }
	  if (KP9Pressed()) 
	    {
	      CurLevel->map[BlockY][BlockX]=ECK_RO;
	    }
	  if (APressed())
	    {
	      CurLevel->map[BlockY][BlockX]=ALERT;	      
	    }
	  if (RPressed())
	    {
	      CurLevel->map[BlockY][BlockX]=REFRESH1;	            
	    }
	  if (DPressed())
	    {
	      if ( !Ctrl_Was_Pressed())
		{
		  if (Shift_Was_Pressed())
		    CurLevel->map[BlockY][BlockX]=V_ZUTUERE;	            	      
		  else CurLevel->map[BlockY][BlockX]=H_ZUTUERE;	            	      
		}
	      else
		{
		  if (Shift_Was_Pressed())
		    CurLevel->map[BlockY][BlockX]=LOCKED_V_ZUTUERE;	            	      
		  else CurLevel->map[BlockY][BlockX]=LOCKED_H_ZUTUERE;	            	      
		}
	    }
	  if (SpacePressed())
	    CurLevel->map[BlockY][BlockX]=FLOOR;	            	      	    
	  if (QPressed())
	    {
	      Terminate(0);
	    }

	} // while (!EscapePressed())
      while( EscapePressed() );

      // After Level editing is done and escape has been pressed, 
      // display the Menu with level save options and all that.

      while (!Weiter)
	{

	  InitiateMenu( NULL );

	  MenuTexts[ 0 ] = "Save whole ship to 'Testship.shp'" ;
	  sprintf( Options0 , "Current: %d.  Level Up/Down" , CurLevel->levelnum );
	  MenuTexts[ 1 ] = Options0;
	  MenuTexts[ 2 ] = "Change tile set" ;
	  sprintf( Options1 , "Levelsize in X: %d.  Shrink/Enlarge" , CurLevel->xlen );
	  MenuTexts[ 3 ] = Options1;
	  sprintf( Options2 , "Levelsize in Y: %d.  Shrink/Enlarge" , CurLevel->ylen  );
	  MenuTexts[ 4 ] = Options2;
	  sprintf( Options3 , "Level name: %s" , CurLevel->Levelname );
	  MenuTexts[ 5 ] = Options3;
	  sprintf( Options4 , "Background music file name: %s" , CurLevel->Background_Song_Name );
	  MenuTexts[ 6 ] = Options4;
	  sprintf( Options5 , "Set Level Comment: %s" , CurLevel->Level_Enter_Comment );
	  MenuTexts[ 7 ] = Options5;
	  MenuTexts[ 8 ] = "Quit Level Editor" ;
	  MenuTexts[ 9 ] = "" ; 

	  MenuPosition = DoMenuSelection( "" , MenuTexts , -1 , NULL , FPS_Display_BFont );

	  while (EnterPressed() || SpacePressed() );

	  switch (MenuPosition) 
	    {
	      
	    case (-1):
	      while ( EscapePressed() );
	      Weiter=!Weiter;
	      if ( CurrentCombatScaleFactor != 1 ) SetCombatScaleTo( 1 );
	      break;
	    case SAVE_LEVEL_POSITION:
	      while (EnterPressed() || SpacePressed() ) ;
	      SaveShip("Testship.shp");
	      CenteredPutString ( Screen ,  11*FontHeight(Menu_BFont),    "Your ship was saved...");
	      SDL_Flip ( Screen );
	      while (!EnterPressed() && !SpacePressed() ) ;
	      while (EnterPressed() || SpacePressed() ) ;
	      // Weiter=!Weiter;
	      break;
	    case CHANGE_LEVEL_POSITION: 
	      // if ( CurLevel->levelnum ) Teleport ( CurLevel->levelnum-1 , Me[0].pos.x , Me[0].pos.y ); 
	      while (EnterPressed() || SpacePressed() ) ;
	      break;
	    case CHANGE_TILE_SET_POSITION: 
	      while (EnterPressed() || SpacePressed() ) ;
	      break;
		case SET_LEVEL_NAME:
		  while (EnterPressed() || SpacePressed() ) ;
		  CenteredPutString ( Screen ,  12*FontHeight(Menu_BFont), "Please enter new level name:");
		  SDL_Flip( Screen );
		  CurLevel->Levelname=GetString( 100 , FALSE );
		  Weiter=!Weiter;
		  break;
		case SET_BACKGROUND_SONG_NAME:
		  while (EnterPressed() || SpacePressed() ) ;
		  CenteredPutString ( Screen ,  12*FontHeight(Menu_BFont), "Please enter new music file name:");
		  SDL_Flip( Screen );
		  CurLevel->Background_Song_Name=GetString( 100 , FALSE );
		  Weiter=!Weiter;
		  break;
		case SET_LEVEL_COMMENT:
		  while (EnterPressed() || SpacePressed() ) ;
		  CenteredPutString ( Screen ,  12*FontHeight(Menu_BFont), "Please enter new level comment:\n");
		  SDL_Flip( Screen );
		  SetTextCursor( 15 , 440 );
		  CurLevel->Level_Enter_Comment=GetString( 100 , FALSE );
		  Weiter=!Weiter;
		  break;
		case QUIT_LEVEL_EDITOR_POSITION:
		  while (EnterPressed() || SpacePressed() ) ;
		  Weiter=!Weiter;
		  Done=TRUE;
		  SetCombatScaleTo( 1 );
		  break;
		default: 
		  break;

		} // switch

	  // If the user of the level editor pressed left or right, that should have
	  // an effect IF he/she is a the change level menu point

	  if (LeftPressed() || RightPressed() ) 
	    {
	      switch (MenuPosition)
		{

		case CHANGE_LEVEL_POSITION:
		  if ( LeftPressed() )
		    {
		      if ( CurLevel->levelnum > 0 )
			Teleport ( CurLevel->levelnum -1 , 3 , 3 , 0 );
		      while (LeftPressed());
		    }
		  if ( RightPressed() )
		    {
		      if ( CurLevel->levelnum < curShip.num_levels -1 )
			Teleport ( CurLevel->levelnum +1 , 3 , 3 , 0 );
		      while (RightPressed());
		    }
		  if ( CurrentCombatScaleFactor != 1 ) SetCombatScaleTo ( CurrentCombatScaleFactor );
		  break;
		  
		case CHANGE_TILE_SET_POSITION:
		  if ( RightPressed() && (CurLevel->color  < 6 ) )
		    {
		      CurLevel->color++;
		      while (RightPressed());
		    }
		  if ( LeftPressed() && (CurLevel->color > 0) )
		    {
		      CurLevel->color--;
		      while (LeftPressed());
		    }
		  Teleport ( CurLevel->levelnum , Me[0].pos.x , Me[0].pos.y , 0 ); 
		  break;
		case CHANGE_SIZE_X:
		  if ( RightPressed() )
		    {
		      CurLevel->xlen++;
		      // In case of enlargement, we need to do more:
		      for ( i = 0 ; i < CurLevel->ylen ; i++ )
			{
			  OldMapPointer=CurLevel->map[i];
			  CurLevel->map[i] = MyMalloc( CurLevel->xlen +1) ;
			  memcpy( CurLevel->map[i] , OldMapPointer , CurLevel->xlen-1 );
			  // We don't want to fill the new area with junk, do we? So we set it VOID
			  CurLevel->map[ i ] [ CurLevel->xlen-1 ] = VOID;  
			}
		      while (RightPressed());
		    }
		  if ( LeftPressed() )
		    {
		      CurLevel->xlen--; // making it smaller is always easy:  just modify the value for size
		                        // allocation of new memory or things like that are not nescessary.
		      while (LeftPressed());
		    }
		  break;
		  
		case CHANGE_SIZE_Y:
		  if ( RightPressed() )
		    {

		      //--------------------
		      // The enlargement of levels in y direction is limited by a constant
		      // defined in defs.h.  This is carefully checked or no operation at
		      // all will be performed.
		      //
		      if ( (CurLevel->ylen)+1 < MAX_MAP_LINES )
			{
			  CurLevel->ylen++;
			  // In case of enlargement, we need to do more:
			  CurLevel->map[ CurLevel->ylen-1 ] = MyMalloc( CurLevel->xlen +1) ;
			  // We don't want to fill the new area with junk, do we? So we set it VOID
			  memset( CurLevel->map[ CurLevel->ylen-1 ] , VOID , CurLevel->xlen );
			}
		      while (RightPressed());
		    }

		  if ( LeftPressed() )
		    {
		      CurLevel->ylen--; // making it smaller is always easy:  just modify the value for size
		                        // allocation of new memory or things like that are not nescessary.
		      while (LeftPressed());
		    }
		  break;
		  
		}
	    } // if LeftPressed || RightPressed

	  // If the user pressed up or down, the cursor within
	  // the level editor menu has to be moved, which is done here:
	  /*
	  if (UpPressed()) 
	    {
	      if (MenuPosition > 1) MenuPosition--;
	      MoveMenuPositionSound();
	      while (UpPressed());
	    }
	  if (DownPressed()) 
	    {
	      if ( MenuPosition < QUIT_LEVEL_EDITOR_POSITION ) MenuPosition++;
	      MoveMenuPositionSound();
	      while (DownPressed());
	    }
	  */


	}
      
    } // while (!Done)

} // void Level_Editor(void)








#undef _leveleditor_c
