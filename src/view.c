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
 * Desc: all the functions managing the things one gets to see.
 *	That includes assembling of enemys, assembling the currently
 *	relevant porting of the map (the bricks I mean), drawing all visible
 *	elements like bullets, blasts, enemys or influencer in a nonvisible
 *	place in memory at first, and finally drawing them to the visible
 *	screen for the user.
 *
 *----------------------------------------------------------------------*/

#define _view_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "map.h"
#include "proto.h"

#include "SDL_rotozoom.h"

SDL_Color Black = {0, 0, 0};
SDL_Color Flash_Light = {11, 11, 11};
SDL_Color Flash_Dark  = {230, 230, 230};

#define BLINK_LEN 1.0   // length of one blink cycle at low energy (in s)

/*----------------------------------------------------------------------
 * Map2ScreenXY (): translate a map-pos (x,y) to screen-coords (X,Y)
 *                  using influ-position Me.pos
 *
 *----------------------------------------------------------------------*/
void
Map2ScreenXY (finepoint mappos, point *screenpos)
{
  


}// Map2ScreenXY()

/*
-----------------------------------------------------------------
@Desc: This function assembles the contents of the combat window 
       in ne_screen.

       Several FLAGS can be used to control its behaviour:

       (*) ONLY_SHOW_MAP = 0x01:  This flag indicates not do draw any
           game elements but the map blocks

       (*) DO_SCREEgN_UPDATE = 0x02: This flag indicates for the function
           to also cause an SDL_Update of the portion of the screen
           that has been modified

       (*) SHOW_FULL_MAP = 0x04: show complete map, disregard visibility

 @Ret: none
-----------------------------------------------------------------
*/

void
Assemble_Combat_Picture (int mask)
{
  int MapBrick;
  int line, col;
  int i;
  static float TimeSinceLastFPSUpdate=10;
  static int FPS_Displayed=1;
  SDL_Rect TargetRectangle;
  SDL_Rect TxtRect;
  finepoint pos, vect;
  float len;
  bool vis = TRUE;
  grob_point upleft, downright;


#define UPDATE_FPS_HOW_OFTEN 0.75

  DebugPrintf (2, "\nvoid Assemble_Combat_Picture(...): Real function call confirmed.");
  
  

  SDL_SetClipRect (ne_screen , &User_Rect);

  if ( !GameConfig.AllMapVisible )
    Fill_Rect (User_Rect, Black);

  if ( (mask & SHOW_FULL_MAP) != 0 )
    {
      upleft.x = -5; upleft.y = -5;
      downright.x = CurLevel->xlen + 5; downright.y = CurLevel->ylen + 5;
    }
  else
    {
      upleft.x = Me.pos.x - 6; upleft.y = Me.pos.y - 5;
      downright.x = Me.pos.x + 7; downright.y = Me.pos.y + 5;
    }
    

  for (line = (int)upleft.y; line < (int)downright.y; line++)
    {
      for (col = (int)upleft.x; col < (int)downright.x; col++)
	{
	  vis = TRUE;
	  if ( !GameConfig.AllMapVisible && ( (mask & SHOW_FULL_MAP) == 0x0) )
	    {
	      pos.x = col;
	      pos.y = line;
	      vect.x = Me.pos.x - pos.x;
	      vect.y = Me.pos.y - pos.y;
	      len = sqrt( vect.x * vect.x + vect.y * vect.y) + 0.01;
	      vect.x /= len;
	      vect.y /= len;
	      if (len > 0.5)
		{
		  pos.x += vect.x;
		  pos.y += vect.y;
		}
	      if ( !IsVisible (&pos) )
		continue;
	    }
	  
	  MapBrick = GetMapBrick( CurLevel, col , line );
	  TargetRectangle.x = UserCenter_x + (int)rint( (-Me.pos.x+1.0*col-0.5 )*Block_Rect.w);
	  TargetRectangle.y = UserCenter_y + (int)rint( (-Me.pos.y+1.0*line-0.5 )*Block_Rect.h);
	  SDL_BlitSurface( MapBlockSurfacePointer[CurLevel->color][MapBrick], NULL, ne_screen, &TargetRectangle);
	}			// for(col) 
    }				// for(line) 


  // if we don't use Fullscreen mode, we have to clear the text-background manually
  // for the info-line text:

  TxtRect.x = Full_User_Rect.x;
  TxtRect.y = Full_User_Rect.y+Full_User_Rect.h - FontHeight (Font0_BFont);
  TxtRect.h = FontHeight (Font0_BFont);
  TxtRect.w = Full_User_Rect.w;
  SDL_SetClipRect (ne_screen, &TxtRect);
  if (!GameConfig.FullUserRect)
    SDL_FillRect(ne_screen, &TxtRect, 0);


  if ( GameConfig.Draw_Position )
    {
      PrintStringFont( ne_screen , Font0_BFont , Full_User_Rect.x+Full_User_Rect.w/6 , 
		       Full_User_Rect.y+Full_User_Rect.h - FontHeight( Font0_BFont ), 
		       "GPS: X=%d Y=%d Lev=%d" , (int) rintf(Me.pos.x) , (int) rintf(Me.pos.y) , 
		       CurLevel->levelnum );
    }


  if (!(mask & ONLY_SHOW_MAP) )
    {
      if ( GameConfig.Draw_Framerate )
	{
	  TimeSinceLastFPSUpdate += Frame_Time();
	  if ( TimeSinceLastFPSUpdate > UPDATE_FPS_HOW_OFTEN )
	    {
	      FPS_Displayed=(int)(1.0/Frame_Time());
	      TimeSinceLastFPSUpdate=0;
	    }
	  
	  PrintStringFont( ne_screen , Font0_BFont , Full_User_Rect.x , 
			   Full_User_Rect.y+Full_User_Rect.h - FontHeight( Font0_BFont ), 
			   "FPS: %d " , FPS_Displayed );
	}

      if ( GameConfig.Draw_Energy )
	{
	  PrintStringFont( ne_screen , Font0_BFont , Full_User_Rect.x+Full_User_Rect.w/2 , 
			   Full_User_Rect.y+Full_User_Rect.h - FontHeight( Font0_BFont ), 
			   "Energy: %d" , (int)Me.energy);
	}
      if (GameConfig.Draw_DeathCount)
	{
	  PrintStringFont( ne_screen , Font0_BFont , Full_User_Rect.x+2*Full_User_Rect.w/3 , 
			   Full_User_Rect.y+Full_User_Rect.h - FontHeight( Font0_BFont ), 
			   "Deathcount: %d", (int)DeathCount );
	}


      SDL_SetClipRect (ne_screen, &User_Rect);


      // make sure Ashes are displayed _before_ droids, so that they are _under_ them!
      for (i = 0; i < NumEnemys ; i++)
	if ( (AllEnemys[i].status == TERMINATED) && (AllEnemys[i].levelnum == CurLevel->levelnum) )
	  {
	    if (IsVisible (&(AllEnemys[i].pos) ) )
	      PutAshes (AllEnemys[i].pos.x, AllEnemys[i].pos.y);
	  }

      for (i = 0; i < NumEnemys ; i++)
	if ( (AllEnemys[i].levelnum != CurLevel->levelnum) || (AllEnemys[i].status == OUT) ||
	     (AllEnemys[i].status == TERMINATED) )
	  continue;
	else
	  PutEnemy (i , -1 , -1 );

      if (Me.energy > 0)
	PutInfluence ( -1 , -1 );

      for (i = 0; i < (MAXBULLETS); i++)
	if (AllBullets[i].type != OUT)
	  PutBullet (i);

      for (i = 0; i < (MAXBLASTS); i++)
	if (AllBlasts[i].type != OUT)
	  PutBlast (i);

    }

  // At this point we are done with the drawing procedure
  // and all that remains to be done is updating the screen.

  if ( mask & DO_SCREEN_UPDATE )
    {
      SDL_UpdateRect (ne_screen, User_Rect.x, User_Rect.y, User_Rect.w, User_Rect.h);
      SDL_UpdateRect (ne_screen, TxtRect.x, TxtRect.y, TxtRect.w, TxtRect.h);
    }

  SDL_SetClipRect (ne_screen, NULL);
  
  return;

} // void Assemble_Combat_Picture(...)

/*
-----------------------------------------------------------------
@Desc: This function draws the influencer to the screen, either
to the center of the combat window if (-1,-1) was specified, or
to the specified coordinates anywhere on the screen, useful e.g.
for using the influencer as a cursor in the menus.

@Ret: none
-----------------------------------------------------------------*/
void
PutInfluence ( int x, int y)
{
  SDL_Rect dst;
  SDL_Rect Text_Rect;
  float rest, filt;

  Text_Rect.x=User_Rect.x+(User_Rect.w/2) + Block_Rect.w/3;
  Text_Rect.y=User_Rect.y+(User_Rect.h/2) - Block_Rect.h/2;
  Text_Rect.w=User_Rect.w/2 - Block_Rect.w/3;
  Text_Rect.h=User_Rect.h/2;

  DebugPrintf (2, "\nvoid PutInfluence(void): real function call confirmed.");

  // Now we draw the hat and shoes of the influencer
  SDL_BlitSurface( InfluencerSurfacePointer[ (int) floorf (Me.phase) ], NULL , BuildBlock, NULL);


  // Now we draw the first digit of the influencers current number.
  dst.x = FirstDigit_Rect.x;
  dst.y = FirstDigit_Rect.y;
  SDL_BlitSurface (InfluDigitSurfacePointer[ Druidmap[Me.type].druidname[0]-'1'+1 ], NULL, BuildBlock, &dst);

  // Now we draw the second digit of the influencers current number.
  dst.x = SecondDigit_Rect.x;
  dst.y = SecondDigit_Rect.y;
  SDL_BlitSurface( InfluDigitSurfacePointer[ Druidmap[Me.type].druidname[1]-'1'+1 ], NULL, BuildBlock, &dst);

  // Now we draw the third digit of the influencers current number.
  dst.x = ThirdDigit_Rect.x ;
  dst.y = ThirdDigit_Rect.y;
  SDL_BlitSurface( InfluDigitSurfacePointer[ Druidmap[Me.type].druidname[2]-'1'+1 ] , NULL, BuildBlock, &dst);

  if ( ( (Me.energy*100/Druidmap[Me.type].maxenergy) <= BLINKENERGY) && ( x == -1 ) ) 
    {
      // In case of low energy, do the fading effect...
      rest = fmod(Me.timer, BLINK_LEN);  // period of fading is given by BLINK_LEN
      if (rest  < BLINK_LEN/2 )
	filt = 0.40 + (1.0 - 2.0*rest/BLINK_LEN)*0.60;   // decrease white->grey
      else
	filt = 0.40 + (2.0*rest/BLINK_LEN - 1.0)*0.60;  // increase back to white

      ApplyFilter (BuildBlock, filt, filt, filt);

      // ... and also maybe start a new cry-sound

      if ( Me.LastCrysoundTime > CRY_SOUND_INTERVAL )
	{
	  Me.LastCrysoundTime = 0;
	  CrySound();
	}
    }

  //--------------------
  // In case of transfer mode, we produce the transfer mode sound
  // but of course only in some periodic intervall...

  if ( Me.status == TRANSFERMODE  && (x == -1))
    {
      ApplyFilter (BuildBlock, 1.0, 0.0, 0.0);

      if ( Me.LastTransferSoundTime > TRANSFER_SOUND_INTERVAL )
	{
	  Me.LastTransferSoundTime = 0;
	  TransferSound();
	}
    }


  if ( x == -1 ) 
    {
      dst.x=UserCenter_x - Block_Rect.w/2;
      dst.y=UserCenter_y - Block_Rect.h/2;
    }
  else
    {
      dst.x=x ;
      dst.y=y ;
    }


  SDL_BlitSurface (BuildBlock, NULL, ne_screen, &dst);



  //--------------------
  // Maybe the influencer has something to say :)
  // so let him say it..
  //
  if ( ( x == (-1) ) && ( Me.TextVisibleTime < GameConfig.WantedTextVisibleTime ) && GameConfig.Droid_Talk )
    {
      //      PutStringFont ( ne_screen , Font0_BFont , 
      //		      User_Rect.x+(User_Rect.w/2) + Block_Rect.w/3 , 
      //		      User_Rect.y+(User_Rect.h/2) - Block_Rect.h/2 ,  
      //		      Me.TextToBeDisplayed );
      SetCurrentFont( Font0_BFont );
      DisplayText( Me.TextToBeDisplayed, User_Rect.x+(User_Rect.w/2) + Block_Rect.w/3,
		   User_Rect.y+(User_Rect.h/2) - Block_Rect.h/2 , &Text_Rect );
    }

  DebugPrintf (2, "\nvoid PutInfluence(void): enf of function reached.");

} /* PutInfluence() */


/*@Function============================================================
@Desc: PutEnemy: This function draws an enemy into the combat window.
       The only parameter given is the number of the enemy within the
       AllEnemys array. Everything else is computed in here.

@Ret: void
* $Function----------------------------------------------------------*/
void
PutEnemy (int Enum , int x , int y)
{
  SDL_Rect dst;
  enemy *droid  = &AllEnemys[Enum];
  int type = droid->type;
  int phase = droid->phase;
  char *name = Druidmap[type].druidname;

  if ( (droid->status == TERMINATED) || (droid->status == OUT) || 
       (droid->levelnum != CurLevel->levelnum) )
    return;

  // if the enemy is out of sight, we need not do anything more here
  if ((!show_all_droids) && (!IsVisible (&AllEnemys[Enum].pos)) )
    {
      DebugPrintf (3, "\nvoid PutEnemy(int Enum): ONSCREEN=FALSE --> usual end of function reached.\n");
      return;
    }

  // We check for incorrect droid types, which sometimes might occor, especially after
  // heavy editing of the crew initialisation functions ;)
  if ( droid->type >= Number_Of_Droid_Types )
    {
      DebugPrintf (0, "ERROR: nonexistant droid-type encountered: %d\n", droid->type);
      Terminate(ERR);
    }

  //--------------------
  // First blit just the enemy hat and shoes.
  SDL_BlitSurface (EnemySurfacePointer[phase], NULL, BuildBlock, NULL);

  //--------------------
  // Now the numbers should be blittet.
  dst.x = FirstDigit_Rect.x;
  dst.y = FirstDigit_Rect.y;
  SDL_BlitSurface( EnemyDigitSurfacePointer[name[0]-'1'+1 ], NULL, BuildBlock, &dst);
  
  dst.x = SecondDigit_Rect.x;
  dst.y = SecondDigit_Rect.y;
  SDL_BlitSurface( EnemyDigitSurfacePointer[name[1]-'1'+1 ], NULL, BuildBlock, &dst);

  dst.x = ThirdDigit_Rect.x;
  dst.y = ThirdDigit_Rect.y;
  SDL_BlitSurface( EnemyDigitSurfacePointer[name[2]-'1'+1], NULL,  BuildBlock, &dst);
  
  // now blit the whole construction to screen:
  if ( x == -1 ) 
    {
      dst.x=UserCenter_x + (droid->pos.x - Me.pos.x) * Block_Rect.w - Block_Rect.w/2;
      dst.y=UserCenter_y + (droid->pos.y - Me.pos.y) * Block_Rect.h - Block_Rect.h/2;
    }
  else
    {
      dst.x=x ;
      dst.y=y ;
    }
  SDL_BlitSurface (BuildBlock, NULL, ne_screen, &dst);

  //--------------------
  // At this point we can assume, that the enemys has been blittet to the
  // screen, whether it's a friendly enemy or not.
  // 
  // So now we can add some text the enemys says.  That might be fun.
  //
  if ( (x == -1)
       && ( AllEnemys[Enum].TextVisibleTime < GameConfig.WantedTextVisibleTime )
       && GameConfig.Droid_Talk )
    {
      PutStringFont ( ne_screen , Font0_BFont , 
		      User_Rect.x+(User_Rect.w/2) + Block_Rect.w/3 + (AllEnemys[Enum].pos.x - Me.pos.x) * Block_Rect.w , 
		      User_Rect.y+(User_Rect.h/2) - Block_Rect.h/2 + (AllEnemys[Enum].pos.y - Me.pos.y) * Block_Rect.h ,  
		      AllEnemys[Enum].TextToBeDisplayed );
    }

  

  DebugPrintf (2, "\nvoid PutEnemy(int Enum): ENEMY HAS BEEN PUT --> usual end of function reached.\n");

}	// void PutEnemy(int Enum , int x , int y) 


// ----------------------------------------------------------------------
// put some ashes at (x,y)
//----------------------------------------------------------------------
void
PutAshes (float x, float y)
{
  SDL_Rect dst;

  if (!GameConfig.ShowDecals)
    return;

  dst.x = UserCenter_x + ( -Me.pos.x + x ) * Block_Rect.w  -Block_Rect.w/2;
  dst.y = UserCenter_y + ( -Me.pos.y + y ) * Block_Rect.h -Block_Rect.h/2;
  SDL_BlitSurface( Decal_pics[0], NULL , ne_screen, &dst);

  return;

} // PutAshes

/*@Function============================================================
@Desc: PutBullet: draws a Bullet into the combat window.  The only 
       parameter given is the number of the bullet in the AllBullets 
       array. Everything else is computed in here.

@Ret: void
* $Function----------------------------------------------------------*/
void
PutBullet (int BulletNummer)
{
  Bullet CurBullet = &AllBullets[BulletNummer];
  SDL_Rect dst;
  // SDL_Surface *tmp;
  int PhaseOfBullet;
  int i;

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
      if ( CurBullet->time_in_seconds <= FLASH_DURATION/4)
	Fill_Rect (User_Rect, Flash_Light);
      else if (CurBullet->time_in_seconds <= FLASH_DURATION/2)
	Fill_Rect (User_Rect, Flash_Dark);
      else if (CurBullet->time_in_seconds <= 3*FLASH_DURATION/4)
	Fill_Rect (User_Rect, Flash_Light);
      else if (CurBullet->time_in_seconds <= FLASH_DURATION)
	Fill_Rect (User_Rect, Flash_Dark);

      return;
    } // if type == FLASH



  PhaseOfBullet = (CurBullet->time_in_seconds * Bulletmap[ CurBullet->type ].phase_changes_per_second );

  PhaseOfBullet = PhaseOfBullet % Bulletmap[CurBullet->type].phases ;

  // DebugPrintf( 0 , "\nPhaseOfBullet: %d.", PhaseOfBullet );

#define ONE_ROTATION_ONLY
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
  // LONGER of dimension Block_Rect.w times Block_Rect.h, but of the dimesions of the smallest
  // rectangle containing the full rotated Block_Rect.h x Block_Rect.w rectangle!!!
  // This has to be taken into account when calculating the target position for the 
  // blit of these surfaces!!!!
  dst.x = UserCenter_x
    - (Me.pos.x-CurBullet->pos.x)*Block_Rect.w-CurBullet->SurfacePointer[ PhaseOfBullet ]->w/2;
  dst.y = UserCenter_y
    - (Me.pos.y-CurBullet->pos.y)*Block_Rect.w-CurBullet->SurfacePointer[ PhaseOfBullet ]->h/2;

  SDL_BlitSurface( CurBullet->SurfacePointer[ PhaseOfBullet ] , NULL, ne_screen , &dst );
#else
  tmp = rotozoomSurface( Bulletmap[CurBullet->type].SurfacePointer[ PhaseOfBullet ] , CurBullet->angle , 1.0 , FALSE );

  // WARNING!!! PAY ATTENTION HERE!! After the rotozoom was applied to the image, it is NO
  // LONGER of dimension Block_Rect.w times Block_Rect.h, but of the dimesions of the smallest
  // rectangle containing the full rotated Block_Rect.h x Block_Rect.w rectangle!!!
  // This has to be taken into account when calculating the target position for the 
  // blit of these surfaces!!!!
  dst.x = UserCenter_x
    - (Me.pos.x-CurBullet->pos.x)*Block_Rect.w-CurBullet->SurfacePointer[ PhaseOfBullet ]->w/2;
  dst.y = UserCenter_y
    - (Me.pos.y-CurBullet->pos.y)*Block_Rect.w-CurBullet->SurfacePointer[ PhaseOfBullet ]->h/2;

  SDL_BlitSurface( tmp , NULL, ne_screen , &dst );
  SDL_FreeSurface( tmp );
#endif

  DebugPrintf ( 1 , "\nvoid PutBullet(int BulletNummer): end of function reched.\n");

}; // void PutBullet (int Bulletnumber )

/*@Function============================================================
@Desc:  PutBlast: This function draws a blast into the combat window.
        The only given parameter is the number of the blast within
	the AllBlasts array.

@Ret: void
* $Function----------------------------------------------------------*/
void
PutBlast (int BlastNummer)
{
  Blast CurBlast = &AllBlasts[BlastNummer];
  SDL_Rect dst;

  // If the blast is already long deat, we need not do anything else here
  if (CurBlast->type == OUT)
    return;

  
  dst.x=UserCenter_x - (Me.pos.x - CurBlast->PX)*Block_Rect.w  -Block_Rect.w/2;
  dst.y=UserCenter_y - (Me.pos.y - CurBlast->PY)*Block_Rect.h -Block_Rect.h/2;
  // SDL_BlitSurface( ne_blocks, 
  // Blastmap[CurBlast->type].block + ((int) floorf(CurBlast->phase)), ne_screen , &dst);
  SDL_BlitSurface( Blastmap[CurBlast->type].SurfacePointer[ (int)floorf(CurBlast->phase) ] , NULL , ne_screen , &dst);

}  // void PutBlast(int BlastNummer)

/*@Function============================================================
@Desc: This function fills the whole combat window with the one color
       given as the only parameter to the function.  For this purpose
       a fast SDL basic function is used.

@Ret: none
* $Function----------------------------------------------------------*/
void
SetUserfenster (int color)
{
  SDL_Rect tmp;

  Set_Rect (tmp, User_Rect.x, User_Rect.y, User_Rect.w, User_Rect.h);

  SDL_FillRect( ne_screen , &tmp, color );

  return;
}				/* SetUserFenster() */

/*-----------------------------------------------------------------
 * Fill given rectangle with given RBG color
 *
 *-----------------------------------------------------------------*/
void
Fill_Rect (SDL_Rect rect, SDL_Color color)
{
  Uint32 pixcolor;
  SDL_Rect tmp;

  Copy_Rect (rect, tmp);

  pixcolor = SDL_MapRGB (ne_screen->format, color.r, color.g, color.b);

  SDL_FillRect (ne_screen, &tmp, pixcolor);
  
  return;
}

/*-----------------------------------------------------------------
 * @Desc: This function updates the top status bar.
 * To save framerate on slow machines however it will only work
 * if it thinks that work needs to be done. 
 * You can however force update if you say so with a flag.
 *
 * BANNER_FORCE_UPDATE=1: Forces the redrawing of the title bar
 *
 * BANNER_DONT_TOUCH_TEXT=2: Prevents DisplayBanner from touching the
 * text.
 * 
 * BANNER_NO_SDL_UPDATE=4: Prevents any SDL_Update calls.
 * 
 * -----------------------------------------------------------------*/
void
DisplayBanner (const char* left, const char* right,  int flags )
{
  SDL_Rect dst;
  char dummy[80];
  char left_box [LEFT_TEXT_LEN + 10];
  char right_box[RIGHT_TEXT_LEN + 10];
  static char previous_left_box [LEFT_TEXT_LEN + 10]="NOUGHT";
  static char previous_right_box[RIGHT_TEXT_LEN + 10]="NOUGHT";
  int left_len, right_len;   /* the actualy string-lens */

  // --------------------
  // At first the text is prepared.  This can't hurt.
  // we will decide whether to dispaly it or not later...
  //

  if (left == NULL)       /* Left-DEFAULT: Mode */
    left = InfluenceModeNames[Me.status];

  if ( right == NULL )  /* Right-DEFAULT: Score */
    {
      sprintf ( dummy , "%ld" , ShowScore );
      right = dummy;
    }

  // Now fill in the text
  left_len = strlen (left);
  if( left_len > LEFT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Left Infoline!!",left);
      left_len = LEFT_TEXT_LEN;  /* too long, so we cut it! */
      Terminate(ERR);
    }
  right_len = strlen (right);
  if( right_len > RIGHT_TEXT_LEN )
    {
      printf ("\nWarning: String %s too long for Right Infoline!!", right);
      right_len = RIGHT_TEXT_LEN;  /* too long, so we cut it! */
      Terminate(ERR);
    }
  
  /* Now prepare the left/right text-boxes */
  memset (left_box,  ' ', LEFT_TEXT_LEN);  /* pad with spaces */
  memset (right_box, ' ', RIGHT_TEXT_LEN);  
  
  strncpy (left_box,  left, left_len);  /* this drops terminating \0 ! */
  strncpy (right_box, right, left_len);  /* this drops terminating \0 ! */
  
  left_box [LEFT_TEXT_LEN]  = '\0';     /* that's right, we want padding! */
  right_box[RIGHT_TEXT_LEN] = '\0';
  
  // --------------------
  // No we see if the screen need an update...

  if ( BannerIsDestroyed || 
       (flags & BANNER_FORCE_UPDATE ) || 
       (strcmp( left_box , previous_left_box )) || 
       (strcmp( right_box , previous_right_box )) )
    {
      // Redraw the whole background of the top status bar
      dst.x=0;
      dst.y=0;
      SDL_SetClipRect( ne_screen , NULL );  // this unsets the clipping rectangle
      SDL_BlitSurface( banner_pic, NULL, ne_screen , &dst );

      // Now the text should be ready and its
      // time to display it...
      if ( (strcmp( left_box , previous_left_box )) || 
	   (strcmp( right_box , previous_right_box )) ||
	   ( flags & BANNER_FORCE_UPDATE ) )
	{
	  dst.x = LeftInfo_Rect.x;
	  dst.y = LeftInfo_Rect.y - FontHeight(Para_BFont);
	  PrintStringFont (ne_screen, Para_BFont, dst.x, dst.y, left_box );
	  strcpy( previous_left_box , left_box );
	  dst.x = RightInfo_Rect.x;
	  dst.y = RightInfo_Rect.y - FontHeight(Para_BFont);
	  PrintStringFont (ne_screen, Para_BFont, dst.x, dst.y, right_box );
	  strcpy( previous_right_box , right_box );
	}

      // finally update the whole top status box
      if ( !(flags & BANNER_NO_SDL_UPDATE ) )
	SDL_UpdateRect( ne_screen, 0, 0, Banner_Rect.w , Banner_Rect.h );
      BannerIsDestroyed=FALSE;
      return;
    } /* if */

} /* DisplayBanner() */


#undef _view_c
