/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
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
 * This file contains miscellaeous helpful functions for Freedroid.
 * ---------------------------------------------------------------------- */
/*
 * This file has been checked for remains of german comments in the code
 * I you still find some, please just kill it mercilessly.
 */
#define _misc_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

//--------------------
// The definition of the message structure can stay here,
// because its only needed in this module.
//
typedef struct
{
  void *NextMessage;
  int MessageCreated;
  char *MessageText;
}
message, Message;

mouse_press_button AllMousePressButtons[ MAX_MOUSE_PRESS_BUTTONS ] =
  {
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 560 , 434 ,  38 ,  45 } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 600 , 420 ,  38 ,  40 } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 590 , 376 ,  38 ,  47 } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 560 , 434 ,  38 ,  45 } } ,
    { NULL , "mouse_buttons/UPButton.png"                     , { 600 ,  94 ,  40 ,  40 } } ,
    { NULL , "mouse_buttons/DOWNButton.png"                   , { 600 , 316 ,  40 ,  40 } } ,
    { NULL , "mouse_buttons/LEFTButton.png"                   , { 300 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/RIGHTButton.png"                  , { 450 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapExitButton.png"                , {  50 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapUnlockDoorButton_gray.png"     , { 200 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapUnlockDoorButton_yellow.png"   , { 200 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapUnlockDoorButton_red.png"      , { 200 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapGunOnOffButton_gray.png"       , { 350 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapGunOnOffButton_yellow.png"     , { 350 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapGunOnOffButton_red.png"        , { 350 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapSecurityButtonMiddle.png"      , {  40 , 425 , 120 ,  50 } } ,
    { NULL , "mouse_buttons/MapSecurityButtonLeft.png"        , {   5 , 425 ,  30 ,  50 } } ,
    { NULL , "mouse_buttons/MapSecurityButtonRight.png"       , { 170 , 425 ,  30 ,  50 } } ,
    { NULL , "mouse_buttons/MapPasswordButtonMiddle.png"      , { 310 , 425 , 120 ,  50 } } ,
    { NULL , "mouse_buttons/MapSecurityButtonLeft.png"        , { 280 , 425 ,  30 ,  50 } } ,
    { NULL , "mouse_buttons/MapSecurityButtonRight.png"       , { 440 , 425 ,  30 ,  50 } } ,
    { NULL , "mouse_buttons/MapRequestEnergyRation_green.png" , { 200 ,  60 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapRequestEnergyRation_red.png"   , { 200 ,  60 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapReadEmail_green.png"           , { 350 ,  60 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapReadEmail_red.png"             , { 350 ,  60 , 100 ,  50 } } ,

    { NULL , "mouse_buttons/MapGunTypeButton1_red.png"        , { 570 ,  64 ,  64 ,  64 } } ,
    { NULL , "mouse_buttons/MapGunTypeButton2_red.png"        , { 570 , 128 ,  64 ,  64 } } ,
    { NULL , "mouse_buttons/MapGunTypeButton3_red.png"        , { 570 , 192 ,  64 ,  64 } } ,
    { NULL , "mouse_buttons/MapGunTypeButton4_red.png"        , { 570 , 256 ,  64 ,  64 } } ,
    { NULL , "mouse_buttons/MapGunOnButton_gray.png"          , { 500 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapGunOnButton_yellow.png"        , { 500 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapGunOnButton_red.png"           , { 500 ,   5 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/MapGunTypeButton1_yellow.png"     , { 570 ,  64 ,  64 ,  64 } } ,
    { NULL , "mouse_buttons/MapGunTypeButton2_yellow.png"     , { 570 , 128 ,  64 ,  64 } } ,
    { NULL , "mouse_buttons/MapGunTypeButton3_yellow.png"     , { 570 , 192 ,  64 ,  64 } } ,
    { NULL , "mouse_buttons/MapGunTypeButton4_yellow.png"     , { 570 , 256 ,  64 ,  64 } } ,

    { NULL , "mouse_buttons/ConsoleIdentifyButton_green.png"  , {  50 ,  60 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/ConsoleIdentifyButton_red.png"    , {  50 ,  60 , 100 ,  50 } } ,
    { NULL , "mouse_buttons/ConsoleIdentifyButton_yellow.png" , {  50 ,  60 , 100 ,  50 } } ,

    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 280 ,  44 ,  37 ,  37 } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 536 ,  44 ,  37 ,  37 } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 201 , 340 ,  47 ,  47 } } ,

    { NULL , "mouse_buttons/LeftShopButton.png"               , {  22 , 447 ,  26 ,  26 } } ,
    { NULL , "mouse_buttons/RightShopButton.png"              , { 576 , 447 ,  26 ,  26 } } ,
    { NULL , "mouse_buttons/LeftShopButton.png"               , {   5 ,  16 ,  26 ,  26 } } ,
    { NULL , "mouse_buttons/RightShopButton.png"              , { 580 ,  13 ,  26 ,  26 } } ,
    { NULL , "mouse_buttons/LeftShopButton.png"               , {  10 ,  20 ,  40 ,  40 } } ,
    { NULL , "mouse_buttons/RightShopButton.png"              , { 600 ,  20 ,  40 ,  40 } } ,

    { NULL , "mouse_buttons/number_selector_ok_button.png"    , { 300 , 288 ,  71 ,  31 } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 148 , 244 ,  35 ,  35 } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 404 , 244 ,  35 ,  35 } } ,

    { NULL , "mouse_buttons/buy_button.png"                   , { 199 ,  98 ,  47 ,  47 } } ,
    { NULL , "mouse_buttons/sell_button.png"                  , { 199 , 153 ,  47 ,  47 } } ,
    { NULL , "mouse_buttons/get_button.png"                   , { 193 , 102 ,  60 ,  60 } } ,
    { NULL , "mouse_buttons/put_button.png"                   , { 193 , 185 ,  60 ,  60 } } ,
    { NULL , "mouse_buttons/repair_button.png"                , { 199 , 225 ,  47 ,  47 } } ,
    { NULL , "mouse_buttons/identify_button.png"              , { 199 , 275 ,  47 ,  47 } } ,

    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 320 + 11 , 449 , 297 , 25 } } ,

    { NULL , "mouse_buttons/GoLevelNorthButton.png"           , { 640-50-6 , 480-50-8 , 25 ,  25 } } ,
    { NULL , "mouse_buttons/GoLevelSouthButton.png"           , { 640-50-6 , 480-4-25 , 25 ,  25 } } ,
    { NULL , "mouse_buttons/GoLevelEastButton.png"            , { 640-25-4 , 437 , 25 , 25 } } ,
    { NULL , "mouse_buttons/GoLevelWestButton.png"            , { 640-75-8 , 437 ,  0 , 0 } } ,
    { NULL , "mouse_buttons/ExportThisLevelButton.png"        , { 640-60 , 90 , 0,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorSaveShipButton.png"    , { 640-90 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorZoomInButton.png"      , { 30 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorZoomOutButton.png"     , { 30 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorRecursiveFillButton.png" , { 60 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorNewObstacleLabelButton.png" , { 90 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorNewMapLabelButton.png" , { 120 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorNewItemButton.png"     , { 150 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorMODEButton.png"        , { 0 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorESCButton.png"         , { 430 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorResizeLevelButton.png" , { 460 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorKeymapButton.png"      , { 640-120 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorQuitButton.png"        , { 640-30 , 90 , 0 ,  0 } } ,

    { NULL , "mouse_buttons/LevelEditorToggleTuxButton.png"        , { 210 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorToggleTuxButtonOff.png"        , { 210 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorToggleEnemiesButton.png"    , { 240 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorToggleEnemiesButtonOff.png"    , { 240 , 90 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorToggleObstaclesButton.png"  , { 270 , 90 , 0 ,  0 } } , 
    { NULL , "mouse_buttons/LevelEditorToggleObstaclesButtonOff.png"  , { 270 , 90 , 0 ,  0 } } , 
    { NULL , "mouse_buttons/LevelEditorToggleTooltipsButton.png"  , { 300 , 90 , 0 ,  0 } } , 
    { NULL , "mouse_buttons/LevelEditorToggleTooltipsButtonOff.png"  , { 300 , 90 , 0 ,  0 } } , 

    { NULL , "mouse_buttons/LevelEditorNextItemGroup.png"     , { 55 + 64 * 8 , 32+5*66 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorPrevItemGroup.png"     , { 55          , 32+5*66 , 0 ,  0 } } ,
    { NULL , "mouse_buttons/LevelEditorCancelItemDrop.png"    , { 55 + 130    , 32+5*66 , 0 ,  0 } } ,

    { NULL , "backgrounds/SaveGameBanner.png"                 , { (640-200)/2 , (480-50)/2 , 200 , 50 } } ,
    { NULL , "backgrounds/LoadGameBanner.png"                 , { (640-200)/2 , (480-50)/2 , 200 , 50 } } ,

    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { WEAPON_RECT_X , WEAPON_RECT_Y , WEAPON_RECT_WIDTH , WEAPON_RECT_HEIGHT } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { DRIVE_RECT_X  , DRIVE_RECT_Y  , DRIVE_RECT_WIDTH  , DRIVE_RECT_HEIGHT } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { SHIELD_POS_X  , SHIELD_POS_Y  , SHIELD_RECT_WIDTH , SHIELD_RECT_HEIGHT } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { AUX1_POS_X    , AUX1_POS_Y    , AUX1_RECT_WIDTH , AUX1_RECT_HEIGHT } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { AUX2_POS_X    , AUX2_POS_Y    , AUX2_RECT_WIDTH , AUX2_RECT_HEIGHT } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { HELMET_RECT_POS_X , HELMET_RECT_POS_Y , HELMET_RECT_WIDTH , HELMET_RECT_HEIGHT } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { ARMOUR_POS_X  , ARMOUR_POS_Y  , ARMOUR_RECT_WIDTH , ARMOUR_RECT_HEIGHT } } ,

    { NULL , "mouse_buttons/ScrollDialogMenuUp.png"           , { 233 , 480-20-130-20 , 160 ,  20 } } ,
    { NULL , "mouse_buttons/ScrollDialogMenuDown.png"         , { 233 , 480-20 , 160 ,  20 } } ,
    { NULL , "mouse_buttons/PlusButton.png"                   , { CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X  , STR_Y  , 38 , 22 } } ,
    { NULL , "mouse_buttons/PlusButton.png"                   , { CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X  , MAG_Y  , 38 , 22 } } ,
    { NULL , "mouse_buttons/PlusButton.png"                   , { CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X  , DEX_Y  , 38 , 22 } } ,
    { NULL , "mouse_buttons/PlusButton.png"                   , { CHARACTERRECT_X + BUTTON_MOD_X + STR_NOW_X  , VIT_Y  , 38 , 22 } } ,

    { NULL , "mouse_buttons/arrow_up_for_scroll_text.png"     , { 575 , 10 , 73 , 98 } } ,
    { NULL , "mouse_buttons/arrow_down_for_scroll_text.png"   , { 575 , 480-10-98 , 73 , 98 } } ,

    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 607 , 99 , 26 , 26 } } ,
    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 607 , 347 , 26 , 26 } } ,

    { NULL , "THIS_DOESNT_NEED_BLITTING"                      , { 202 , 311 , 47 , 47 } } ,

    { NULL , "mouse_buttons/ScrollDialogMenuUp.png"           , { 335 , 6   , 160 ,  20 } } ,
    { NULL , "mouse_buttons/ScrollDialogMenuDown.png"         , { 335 , 270 , 160 ,  20 } } ,

  }; // AllMousePressButtons[ MAX_MOUSE_PRESS_BUTTONS ] 

/* ----------------------------------------------------------------------
 * This function checks if a given screen position lies within the 
 * inventory screen toggle button or not.
 * ---------------------------------------------------------------------- */
int
CursorIsOnButton( int ButtonIndex , int x , int y )
{
  //--------------------
  // First a sanity check if the button index given does make
  // some sense.
  //
  if ( ( ButtonIndex >= MAX_MOUSE_PRESS_BUTTONS ) || ( ButtonIndex < 0 ) )
    {
      GiveStandardErrorMessage ( "CursorIsOnButton(...)" , "\
A Button that should be checked for mouse contact was requested, but the\n\
button index given exceeds the number of buttons defined in freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }

  //--------------------
  // Now that we know we have been given a valid button index,
  // we can start to check if the mouse cursor really is on that
  // rectangle or not.
  //
  if ( x > AllMousePressButtons[ ButtonIndex ] . button_rect . x + 
       AllMousePressButtons[ ButtonIndex ] . button_rect . w  ) return ( FALSE );
  if ( x < AllMousePressButtons[ ButtonIndex ] . button_rect . x ) return ( FALSE );
  if ( y > AllMousePressButtons[ ButtonIndex ] . button_rect . y + 
       AllMousePressButtons[ ButtonIndex ] . button_rect . h ) return ( FALSE );
  if ( y < AllMousePressButtons[ ButtonIndex ] . button_rect . y ) return ( FALSE );

  //--------------------
  // So since the cursor is not outside of this rectangle, it must
  // we inside, and so we'll return this answer.
  //
  return ( TRUE );

}; // int CursorIsOnButton( int ButtonIndex , int x , int y )

/* ----------------------------------------------------------------------
 * This function blits a button to the screen.  The button must have been
 * defined prior to this in the above button list.
 * ---------------------------------------------------------------------- */
void 
UpdateScreenOverButtonFromList ( int ButtonIndex )
{ 
  our_SDL_update_rect_wrapper ( Screen , 
		   AllMousePressButtons[ ButtonIndex ] . button_rect . x ,
		   AllMousePressButtons[ ButtonIndex ] . button_rect . y ,
		   AllMousePressButtons[ ButtonIndex ] . button_rect . w ,
		   AllMousePressButtons[ ButtonIndex ] . button_rect . h 
		   );
}; // void UpdateScreenOverButtonFromList ( int ButtonIndex )

/* ----------------------------------------------------------------------
 * This function blits a button to the screen.  The button must have been
 * defined prior to this in the above button list.
 * ---------------------------------------------------------------------- */
void 
ShowGenericButtonFromList ( int ButtonIndex )
{ 
  SDL_Surface *tmp;
  char *fpath;
  SDL_Rect Temp_Blitting_Rect;

  //--------------------
  // First a sanity check if the button index given does make
  // some sense.
  //
  //
  if ( ( ButtonIndex >= MAX_MOUSE_PRESS_BUTTONS ) || ( ButtonIndex < 0 ) )
    {
      GiveStandardErrorMessage ( "ShowGenericButtonFromList(...)" , 
"A Button that should be displayed on the screen was requested, but the\n\
button index given exceeds the number of buttons defined in freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }

  //--------------------
  // Now we check if we have to load the button image still
  // or if it is perhaps already loaded into memory.
  //
  if ( AllMousePressButtons[ ButtonIndex ] . button_surface == NULL )
    {
      fpath = find_file ( AllMousePressButtons[ ButtonIndex ] . button_image_file_name , GRAPHICS_DIR, FALSE);
      tmp = our_IMG_load_wrapper( fpath );
      if ( tmp == NULL )
	{
	  fprintf ( stderr , "\nfpath: %s.\nButton Index: %d.\n" , fpath , ButtonIndex ) ;
	  GiveStandardErrorMessage ( "ShowGenericButtonFromList(...)" , "\
An image file for a button that should be displayed on the screen couldn't\n\
be successfully loaded into memory.\n\
This is an indication of a severe bug/installation problem of freedroid.",
				     PLEASE_INFORM, IS_FATAL );
	}
      AllMousePressButtons[ ButtonIndex ] . button_surface = our_SDL_display_format_wrapperAlpha ( tmp );
      SDL_FreeSurface ( tmp );
    }

  //--------------------
  // Maybe we had '0' entries for the height or width of this button in the list.
  // This means that we will take the real width and the real height from the image
  // and overwrite the 0 entries with this.
  //
  if ( AllMousePressButtons[ ButtonIndex ] . button_rect . w == ( 0 ) )
    {
      AllMousePressButtons[ ButtonIndex ] . button_rect . w =
	AllMousePressButtons[ ButtonIndex ] . button_surface -> w ;
    }
  if ( AllMousePressButtons[ ButtonIndex ] . button_rect . h == ( 0 ) )
    {
      AllMousePressButtons[ ButtonIndex ] . button_rect . h =
	AllMousePressButtons[ ButtonIndex ] . button_surface -> h ;
    }

  //--------------------
  // Now that we know we have the button image loaded, we can start
  // to blit the button image to the screen.
  //
  // But in order not to damage the original rect data, we use the
  // temp value as parameter for the SDL_Blit thing..
  //
  Copy_Rect ( AllMousePressButtons[ ButtonIndex ] . button_rect , Temp_Blitting_Rect );
  our_SDL_blit_surface_wrapper( AllMousePressButtons[ ButtonIndex ] . button_surface , NULL , Screen , &Temp_Blitting_Rect );

}; // void ShowGenericButtonFromList ( int ButtonIndex )


#define SETTINGS_STRUCTURE_RAW_DATA_STRING "\nSettings Raw Data:\n"
#define END_OF_SETTINGS_DATA_STRING "\nEnd of Settings File.\n"

#define MESPOSX 0
#define MESPOSY 64
#define MESHOEHE 8
#define MESBARBREITE 320
#define MAX_MESSAGE_LEN 100
#define MESBAR_MEM MESBARBREITE*MESHOEHE+1000

void CreateMessageBar (char *MText);
void AdvanceQueue (void);

unsigned char *MessageBar;
message *Queue = NULL;
// int ThisMessageTime=0;               /* Counter fuer Message-Timing */

long oneframedelay = 0;
long tenframedelay = 0;
long onehundredframedelay = 0;
float FPSover1 = 10;
float FPSover10 = 10;
float FPSover100 = 10;
Uint32 Now_SDL_Ticks;
Uint32 One_Frame_SDL_Ticks;
Uint32 Ten_Frame_SDL_Ticks;
Uint32 Onehundred_Frame_SDL_Ticks;
int framenr = 0;

char *homedir = NULL;
char *ConfigDir = NULL;

/* -----------------------------------------------------------------
 * find a given filename in subdir relative to FD_DATADIR, 
 * using theme subdir if use_theme==TRUE
 *
 * if you pass NULL as subdir, it will be ignored
 *
 * returns pointer to _static_ string array File_Path, which 
 * contains the full pathname of the file.
 *
 * !! do never try to free the returned string !!
 *
 * ----------------------------------------------------------------- */
char *
find_file (char *fname, char *subdir, int use_theme)
{

  //--------------------
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  // 
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  //
  // This hack will only work if there is NEVER ANY NESTED use of find_file!!
  // Otherwise one file is found, a new search is started and overwrites the
  // previous file name.  If that one is then used again, unpredictable results
  // may occur.
  //
  // But the benefit of it is, of course, that memory leaking will be reduced,
  // even if the returned string is never freed, as it must be to prevent
  // segmentation faults this way anyway!
  //
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  // 
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  //
  // 
  // Because of the things mentioned above, FIND_FILE MUST NOT EVER BE USED
  // FROM A CALLBACK FUNCTION, cause then it migth be A NESTED FUNCTION CALL!
  //
  // IN Case of a Callback function, best use a separate version of this function,
  // best called find_file_for_callbacks.
  //
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  // 
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  //
  int i;
  static char File_Path[5000];   /* hope this will be enough */
  FILE *fp;  // this is the file we want to find?

  if (!fname)
    {
      GiveStandardErrorMessage ( "find_file(...)" , "\
A find_file call has been issued to generate the full path name of a\n\
certain file, but the file name given is an empty string!\n\
This is indicates a severe bug in Freedroid.",  PLEASE_INFORM, IS_FATAL );
    }
  if (!subdir)
    subdir = "";

  for ( i = 0 ; i < 2 ; i++ )
    {
      if (i==0)
	strcpy (File_Path, "..");   /* first try local subdirs */
      if (i==1)
	strcpy (File_Path, FD_DATADIR); /* then the DATADIR */

      strcat (File_Path, "/");
      strcat (File_Path, subdir);
      strcat (File_Path, "/");

      if (use_theme)
	strcat (File_Path, GameConfig.Theme_SubPath);

      strcat (File_Path, fname);
      
      if ( (fp = fopen (File_Path, "r")) != NULL)  /* found it? */
	{
	  fclose (fp);
	  break;
	}
      else
	{
	  if ( i == 0 )
	    DebugPrintf( 1 , "\nfind_file could not succeed with LOCAL path: %s." , File_Path );
	  else
	    {
	      DebugPrintf ( -4 , "The file name was: %s.\n" , fname );
	      GiveStandardErrorMessage ( "find_file(...)" , "File not found even in data dir (2nd attempt).", 
					 NO_NEED_TO_INFORM , IS_WARNING_ONLY );
	    }
	}
    } // for i 

  // DebugPrintf( 0 , "\nfind_file determined file path: %s." , File_Path );

  return (File_Path);
	
}; // char * find_file ( ... )

/* -----------------------------------------------------------------
 * find a given filename in subdir relative to FD_DATADIR, 
 * using theme subdir if use_theme==TRUE
 *
 * if you pass NULL as subdir, it will be ignored
 *
 * returns pointer to _static_ string array File_Path, which 
 * contains the full pathname of the file.
 *
 * !! do never try to free the returned string !!
 *
 * ----------------------------------------------------------------- */
char *
find_file_for_callbacks (char *fname, char *subdir, int use_theme)
{

  //--------------------
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  // 
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  //
  // This hack will only work if there is NEVER ANY NESTED use of find_file!!
  // Otherwise one file is found, a new search is started and overwrites the
  // previous file name.  If that one is then used again, unpredictable results
  // may occur.
  //
  // But the benefit of it is, of course, that memory leaking will be reduced,
  // even if the returned string is never freed, as it must be to prevent
  // segmentation faults this way anyway!
  //
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  // 
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  //
  // 
  // Because of the things mentioned above, FIND_FILE MUST NOT EVER BE USED
  // FROM A CALLBACK FUNCTION, cause then it migth be A NESTED FUNCTION CALL!
  //
  // IN Case of a Callback function, best use a separate version of this function,
  // best called find_file_for_callbacks.
  //
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  // 
  // WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! WARNING! 
  //
  int i;
  static char File_Path[5000];   /* hope this will be enough */
  FILE *fp;  // this is the file we want to find?

  if (!fname)
    {
      GiveStandardErrorMessage ( "find_file(...)" , "\
A find_file call has been issued to generate the full path name of a\n\
certain file, but the file name given is an empty string!\n\
This is indicates a severe bug in Freedroid.",
				 PLEASE_INFORM, IS_FATAL );
    }
  if (!subdir)
    subdir = "";

  for ( i = 0 ; i < 2 ; i++ )
    {
      if (i==0)
	strcpy (File_Path, "..");   /* first try local subdirs */
      if (i==1)
	strcpy (File_Path, FD_DATADIR); /* then the FD_DATADIR */

      strcat (File_Path, "/");
      strcat (File_Path, subdir);
      strcat (File_Path, "/");

      if (use_theme)
	strcat (File_Path, GameConfig.Theme_SubPath);

      strcat (File_Path, fname);
      
      if ( (fp = fopen (File_Path, "r")) != NULL)  /* found it? */
	{
	  fclose (fp);
	  break;
	}
      else
	{
	  if ( i == 0 )
	    DebugPrintf( 1 , "\nfind_file could not succeed with LOCAL path: %s." , File_Path );
	  else
	    GiveStandardErrorMessage ( "find_file(...)" , "File not found even in data dir (2nd attempt).", NO_NEED_TO_INFORM , IS_WARNING_ONLY );
	}
    } // for i 

  // DebugPrintf( 0 , "\nfind_file determined file path: %s." , File_Path );

  return (File_Path);
	
}; // char * find_file_for_callbacks ( ... )

/* ----------------------------------------------------------------------
 * This function realises the Pause-Mode: the game process is halted,
 * while the graphics and animations are not.  This mode 
 * can further be toggled from PAUSE to CHEESE, which is
 * a feature from the original program that should probably
 * allow for better screenshots.
 * ---------------------------------------------------------------------- */
void
Pause (void)
{
  int Pause = TRUE;

  Activate_Conservative_Frame_Computation();

  Me[0].status = PAUSE;
  AssembleCombatPicture ( DO_SCREEN_UPDATE );

  while ( Pause )
    {
      SetNewBigScreenMessage( " Pause " );
      AnimateInfluence ( 0 );
      AnimateCyclingMapTiles ();
      DisplayBanner ( );
      AssembleCombatPicture ( 0 );
      CenteredPutStringFont ( Screen , Menu_Filled_BFont , 200 , "G A M E    P A U S E D" ) ;
      our_SDL_flip_wrapper ( Screen );
      
      if (CPressed ())
	{
	  Me [ 0 ] . status = CHEESE;
	  DisplayBanner ( ) ;
	  AssembleCombatPicture ( DO_SCREEN_UPDATE );

	  while (!SpacePressed ()); /* stay CHEESE until Space pressed */
	  while ( SpacePressed() ); /* then wait for Space released */
	  
	  Me[0].status = PAUSE;       /* return to normal PAUSE */
	} /* if (CPressed) */

      if ( SpacePressed() )
	{
	  Pause = FALSE;
	  while ( SpacePressed() );  /* wait for release */
	}

      //--------------------
      // During the Pause mode, there is again no need to hog the CPU and to 
      // go at full force.  We introduce some rest for the CPU here...
      //
      SDL_Delay (1);

    } /* while (Pause) */
  SetNewBigScreenMessage( "" );
  return;
}; // Pause () 

/* ----------------------------------------------------------------------
 * This function starts the time-taking process.  Later the results
 * of this function will be used to calculate the current framerate
 * 
 * Two methods of time-taking are available.  One uses the SDL 
 * ticks.  This seems LESS ACCURATE.  The other one uses the
 * standard ansi c gettimeofday functions and are MORE ACCURATE
 * but less convenient to use.
 * ---------------------------------------------------------------------- */
void 
StartTakingTimeForFPSCalculation(void)
{
  /* This ensures, that 0 is never an encountered framenr,
   * therefore count to 100 here
   * Take the time now for calculating the frame rate
   * (DO NOT MOVE THIS COMMAND PLEASE!) */
  framenr++;
  
#ifdef USE_SDL_FRAMERATE
  One_Frame_SDL_Ticks=SDL_GetTicks();
  if (framenr % 10 == 1)
    Ten_Frame_SDL_Ticks=SDL_GetTicks();
  if (framenr % 100 == 1)
    {
      Onehundred_Frame_SDL_Ticks=SDL_GetTicks();
      // printf("\n%f",1/Frame_Time());
      // printf("Me[0].pos.x: %g Me[0].pos.y: %g Me[0].speed.x: %g Me[0].speed.y: %g \n",
      //Me[0].pos.x, Me[0].pos.y, Me[0].speed.x, Me[0].speed.y );
      //printf("Me[0].maxspeed.x: %g \n",
      //	     Druidmap[Me[0].type].maxspeed );
    }
#else
  gettimeofday (&oneframetimestamp, NULL);
  if (framenr % 10 == 1)
    gettimeofday (&tenframetimestamp, NULL);
  if (framenr % 100 == 1)
    {
      gettimeofday (&onehundredframetimestamp, NULL);
      printf("\n%f",1/Frame_Time());
    }
#endif
  
}; // void StartTakingTimeForFPSCalculation(void)

/* ----------------------------------------------------------------------
 * This function computes the framerate that has been experienced
 * in this frame.  It will be used to correctly calibrate all 
 * movements of game objects.
 * 
 * NOTE:  To query the actual framerate a DIFFERENT function must
 *        be used, namely Frame_Time().
 *
 *        Two methods of time-taking are available.  One uses the SDL 
 *        ticks.  This seems LESS ACCURATE.  The other one uses the
 *        standard ansi c gettimeofday functions and are MORE ACCURATE
 *        but less convenient to use.
 * ---------------------------------------------------------------------- */
void 
ComputeFPSForThisFrame(void)
{
  // static int time;
  // static int FPS_Displayed;

  // In the following paragraph the framerate calculation is done.
  // There are basically two ways to do this:
  // The first way is to use SDL_GetTicks(), a function measuring milliseconds
  // since the initialisation of the SDL.
  // The second way is to use gettimeofday, a standard ANSI C function I guess,
  // defined in time.h or so.
  // 
  // I have arranged for a definition set in defs.h to switch between the two
  // methods of ramerate calculation.  THIS MIGHT INDEED MAKE SENSE, SINCE THERE
  // ARE SOME UNEXPLAINED FRAMERATE PHENOMENA WHICH HAVE TO TO WITH KEYBOARD
  // SPACE KEY, SO PLEASE DO NOT ERASE EITHER METHOD.  PLEASE ASK JP FIRST.
  //

#ifdef USE_SDL_FRAMERATE

  Now_SDL_Ticks = SDL_GetTicks();
  oneframedelay = Now_SDL_Ticks-One_Frame_SDL_Ticks;
  tenframedelay = Now_SDL_Ticks-Ten_Frame_SDL_Ticks;
  onehundredframedelay = Now_SDL_Ticks-Onehundred_Frame_SDL_Ticks;
  
  if ( !oneframedelay ) FPSover1 = 1000 * 1 / 0.5 ;
  else FPSover1 = 1000 * 1 / (float) oneframedelay;

  if ( !tenframedelay ) FPSover10 = 1000 * 10 / 0.5 ;
  else FPSover10 = 1000 * 10 / (float) tenframedelay;

  if ( !tenframedelay ) FPSover100 = 1000 * 100 / 0.5 ;
  FPSover100 = 1000 * 100 / (float) onehundredframedelay;
  
#else
  
  gettimeofday (&now, NULL);
  
  oneframedelay =
    (now.tv_usec - oneframetimestamp.tv_usec) + (now.tv_sec -
						 oneframetimestamp.
						 tv_sec) * 1000000;
  if (framenr % 10 == 0)
    tenframedelay =
      ((now.tv_usec - tenframetimestamp.tv_usec)) + (now.tv_sec -
						     tenframetimestamp.
						     tv_sec) *
      1000000;
  if ((framenr % 100) == 0)
    {
      onehundredframedelay =
	(now.tv_sec - onehundredframetimestamp.tv_sec) * 1000000 +
	(now.tv_usec - onehundredframetimestamp.tv_usec);
      framenr = 0;
    }
  
  FPSover1 = 1000000 * 1 / (float) oneframedelay;
  FPSover10 = 1000000 * 10 / (float) tenframedelay;
  FPSover100 = 1000000 * 100 / (float) onehundredframedelay;
  
#endif

  /*
  time++;
  if ( time > 100 )
    {
      time = 0 ;
      if ( Frame_Time() > 0 )
	FPS_Displayed=(int)(1.0/Frame_Time());
      else
	FPS_Displayed=(int)9999;
      // TimeSinceLastFPSUpdate=0;
      
      DebugPrintf ( -2 , "\nFPS_Displayed: %d. " , FPS_Displayed );
    }
  */

}; // void ComputeFPSForThisFrame(void)

/* ----------------------------------------------------------------------
 *
 * This function is the key to independence of the framerate for various game elements.
 * It returns the average time needed to draw one frame.
 * Other functions use this to calculate new positions of moving objects, etc..
 *
 * Also there is of course a serious problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.  This might cause HUGE framerates, that could
 * box the influencer out of the ship if used to calculate the new position.
 *
 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.  But on the other hand, this seems to
 * work REALLY well this way.
 *
 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.
 *
 * ---------------------------------------------------------------------- */
float
Frame_Time (void)
{
  float Rate_To_Be_Returned;
  
  if ( SkipAFewFrames ) 
    {
      Rate_To_Be_Returned = Overall_Average;
      return Rate_To_Be_Returned;
    }

  Rate_To_Be_Returned = (1.0 / FPSover1);

  return Rate_To_Be_Returned;

}; // float Frame_Time ( void )

int
Get_Average_FPS ( void )
{
  return ( (int) ( 1.0 / Overall_Average ) );
}; // int Get_Average_FPS( void )

/* ----------------------------------------------------------------------
 * 
 * With framerate computation, there is a problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.  This might cause HUGE framerates, that could
 * box the influencer out of the ship if used to calculate the new position.
 *
 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.
 *
 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.
 *
 * ---------------------------------------------------------------------- */
void 
Activate_Conservative_Frame_Computation(void)
{
  // SkipAFewFrames=212;
  // SkipAFewFrames=22;
  SkipAFewFrames=3;

  DebugPrintf ( 1 , "\nConservative_Frame_Computation activated!" );

}; // void Activate_Conservative_Frame_Computation(void)

/* ----------------------------------------------------------------------
 * This function is used to generate an integer in range of all
 * numbers from 0 to UpperBound.
 * ---------------------------------------------------------------------- */
int
MyRandom (int UpperBound)
{
  float tmp;
  int PureRandom;
  int dice_val;    /* the result in [0, Obergrenze] */

  PureRandom = rand ();
  tmp = 1.0*PureRandom/RAND_MAX; /* random number in [0;1] */

  /* 
   * we always round OFF for the resulting int, therefore
   * we first add 0.99999 to make sure that Obergrenze has
   * roughly the same probablity as the other numbers 
   */
  dice_val = (int)( tmp * (1.0 * UpperBound + 0.99999) );

  return (dice_val);

}; // int MyRandom ( int UpperBound ) 


/* ----------------------------------------------------------------------
 * This function is kills all enemy robots on the whole ship.
 * It querys the user once for safety.
 * ---------------------------------------------------------------------- */
void
Armageddon (void)
{
  char key =' ';
  int i;

  printf ("\nKill all droids on ship (y/n) ? \n");
  while ((key != 'y') && (key != 'n'))
    key = getchar_raw ();
  if (key == 'n')
    return;
  else
    for (i = 0; i < MAX_ENEMYS_ON_SHIP; i++)
      {
	AllEnemys[i].energy = -10;
	// AllEnemys[i].Status = OUT;
      }
}; // void Armageddon(void)

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
move_all_items_in_area ( Level source_level ,
			 float source_start_x , float source_start_y , 
			 float source_area_width , float source_area_height ,
			 Level target_level ,
			 float target_start_x , float target_start_y )
{
    int i;
    int j;

    for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
      {
	if ( source_level -> ItemList [ i ] . type <= (-1) ) continue;
	if ( source_level -> ItemList [ i ] . pos . x < source_start_x ) continue;
	if ( source_level -> ItemList [ i ] . pos . y < source_start_y ) continue;
	if ( source_level -> ItemList [ i ] . pos . x > source_start_x + source_area_width ) continue;
	if ( source_level -> ItemList [ i ] . pos . y > source_start_y + source_area_height ) continue;

	//--------------------
	// So now we know that we must copy this item.  So we go and find a new free
	// item entry on the target level.
	//
	for ( j = 0 ; j < MAX_ITEMS_PER_LEVEL ; j ++ )
	  {
	    if ( target_level -> ItemList [ j ] . type == (-1) ) break;
	  }
	if ( j >= MAX_ITEMS_PER_LEVEL )
	  {
	    GiveStandardErrorMessage ( "move_all_items_in_area(...)" , 
				       "No more free item entries in target level!" ,
				       PLEASE_INFORM, IS_FATAL );
	  }

	memcpy ( & ( target_level -> ItemList [ j ] ) , & ( source_level -> ItemList [ i ] ) , sizeof ( item ) );
	target_level -> ItemList [ j ] . pos . x = target_start_x  + source_level -> ItemList [ i ] . pos . x - source_start_x ;
	target_level -> ItemList [ j ] . pos . y = target_start_y  + source_level -> ItemList [ i ] . pos . y - source_start_y ;

	DeleteItem ( & ( source_level -> ItemList [ i ] ) );

    }
  

}; // void move_all_items_in_area ( ... )

/* ----------------------------------------------------------------------
 * It might be the case that some items come to lie on the floor inside
 * the interface area of two levels.  That might cause strange problems
 * when trying to get close in order to pick up the items.  In face we've
 * had a bug report concerning exactly this case.  So we'll fix this with
 * the following solution:  Whenever the Tux changes level, all items in
 * interface areas will be moved to the level where the Tux is.  This is
 * not compatible with multi-player game.  But multiplayer game is so far
 * away that we need not worry about this possibility right now...
 * ---------------------------------------------------------------------- */
void
move_all_items_to_level ( int target_level )
{
  int source_level_num ;
  int AreaWidth;
  int AreaHeight;

  //--------------------
  // We must check all four interface areas and maybe grab the items from
  // there...
  //

  //--------------------
  // Start to check north interface area
  //
  source_level_num = curShip . AllLevels [ target_level ] -> jump_target_north ;
  if ( source_level_num != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ source_level_num ] -> xlen < curShip . AllLevels [ target_level ] -> xlen )
	AreaWidth = curShip . AllLevels [ source_level_num ] -> xlen;
      else 
	AreaWidth = curShip . AllLevels [ target_level ] -> xlen ;
      
      AreaHeight = curShip . AllLevels [ target_level ] -> jump_threshold_north;
      
      if ( AreaHeight <= 0 ) return;
      
      move_all_items_in_area ( curShip . AllLevels [ source_level_num ] , 0 , curShip . AllLevels [ source_level_num ] -> ylen-AreaHeight , 
			       AreaWidth , AreaHeight ,
			       curShip . AllLevels [ target_level ] , 0 , 0 ) ;

    }

  //--------------------
  // Start to check south interface area
  //
  source_level_num = curShip . AllLevels [ target_level ] -> jump_target_south ;
  if ( source_level_num != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ source_level_num ] -> xlen < curShip . AllLevels [ target_level ] -> xlen )
	AreaWidth = curShip . AllLevels [ source_level_num ] -> xlen;
      else 
	AreaWidth = curShip . AllLevels [ target_level ] -> xlen ;
      
      AreaHeight = curShip . AllLevels [ target_level ] -> jump_threshold_south;
      
      if ( AreaHeight <= 0 ) return;
      
      move_all_items_in_area ( curShip . AllLevels [ source_level_num ] , 0 , 0 , 
			       AreaWidth , AreaHeight ,
			       curShip . AllLevels [ target_level ] , 0 , 
			       curShip . AllLevels [ target_level ] -> ylen-AreaHeight ) ;

    }

  //--------------------
  // Start to check east interface area
  //
  source_level_num = curShip . AllLevels [ target_level ] -> jump_target_east ;
  if ( source_level_num != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ source_level_num ] -> ylen < curShip . AllLevels [ target_level ] -> ylen )
	AreaHeight = curShip . AllLevels [ source_level_num ] -> ylen;
      else 
	AreaHeight = curShip . AllLevels [ target_level ] -> ylen ;
      
      AreaWidth = curShip . AllLevels [ target_level ] -> jump_threshold_east;
      
      if ( AreaWidth <= 0 ) return;
      
      move_all_items_in_area ( curShip . AllLevels [ source_level_num ] , 0 , 0 , 
			       AreaWidth , AreaHeight ,
			       curShip . AllLevels [ target_level ] ,
			       curShip . AllLevels [ target_level ] -> xlen - AreaWidth , 0 ) ;

    }

  //--------------------
  // Start to check west interface area
  //
  source_level_num = curShip . AllLevels [ target_level ] -> jump_target_west ;
  if ( source_level_num != (-1) ) 
    {
      //--------------------
      // First we find out the dimensions of the area we want to copy
      //
      if ( curShip . AllLevels [ source_level_num ] -> ylen < curShip . AllLevels [ target_level ] -> ylen )
	AreaHeight = curShip . AllLevels [ source_level_num ] -> ylen;
      else 
	AreaHeight = curShip . AllLevels [ target_level ] -> ylen ;
      
      AreaWidth = curShip . AllLevels [ target_level ] -> jump_threshold_west;
      
      if ( AreaWidth <= 0 ) return;
      
      move_all_items_in_area ( curShip . AllLevels [ source_level_num ] , 
			       curShip . AllLevels [ source_level_num ] -> xlen - AreaWidth , 0 , 
			       AreaWidth , AreaHeight ,
			       curShip . AllLevels [ target_level ] ,
			       0 , 0 ) ;

    }

}; // void move_all_items_to_level ( int levelnum )

/* ----------------------------------------------------------------------
 * This function teleports the influencer to a new position on the
 * ship.  THIS CAN BE A POSITION ON A DIFFERENT LEVEL.
 * ---------------------------------------------------------------------- */
void
Teleport (int LNum, float X, float Y, int PlayerNum , int Shuffling , int WithSound )
{
  int curLevel = LNum;
  int array_num = 0;
  Level tmp;
  int i;
  static char entering_message[1000];

  if ( curLevel != Me [ PlayerNum ] . pos . z )
    {	

      //--------------------
      // In case a real level change has happend,
      // we need to do a lot of work.  Therefore we start by activating
      // the conservative frame time computation to avoid a 'jump'.
      //
      Activate_Conservative_Frame_Computation();

      //--------------------
      // I think this is for the unlikely case of misordered levels in 
      // the ship file used for this game?!
      //
      while ((tmp = curShip.AllLevels[array_num]) != NULL)
	{
	  if (tmp->levelnum == curLevel)
	    break;
	  else
	    array_num++;
	}

      //--------------------
      // We set a new CurLevel.  This is old and depreciated code,
      // that should sooner or later be completely deactivated.
      //
      CurLevel = curShip.AllLevels[array_num];

      ClearDetectedItemList( PlayerNum );

      Me [ PlayerNum ] . pos . x = X;
      Me [ PlayerNum ] . pos . y = Y;
      Me [ PlayerNum ] . pos . z = array_num; 

      silently_unhold_all_items ();
      move_all_items_to_level ( Me [ 0 ] . pos . z );
      silently_unhold_all_items ();

      //--------------------
      // We add some sanity check against teleporting to non-allowed
      // locations (like outside of map that is)
      //
      if ( ( LNum < 0 ) || ( Me [ PlayerNum ] . pos . x < 0 ) || ( Me [ PlayerNum ] . pos . y < 0 ) ||
	   ( LNum >= curShip.num_levels ) || 
	   ( Me [ PlayerNum ] . pos . x >= curShip.AllLevels[ array_num ] -> xlen ) ||
	   ( Me [ PlayerNum ] . pos . y >= curShip.AllLevels[ array_num ] -> ylen ) )
	{
	  fprintf( stderr, "\n\ntarget location was: lev=%d x=%f y=%f.\n" , LNum , X , Y );
	  GiveStandardErrorMessage ( "Teleport(...)" , "\
A Teleport was requested, but the location to teleport to lies outside\n\
the bounds of this 'ship' which means the current collection of levels.\n\
This indicates an error in the map system of Freedroid.",
				 PLEASE_INFORM, IS_FATAL );
	}

      //--------------------
      // Turn off all blasts and bullets from the old level
      //
      for (i = 0; i < MAXBLASTS; i++)
	{
	  AllBlasts[i].type = OUT;
	}
      for (i = 0; i < MAXBULLETS; i++)
	{
	  //--------------------
	  // Don't ever delete bullets any other way!!! SEGFAULTS might result!!!
	  // in this case, we need no bullet-explosions
	  //
	  DeleteBullet ( i , FALSE ); 
	}

      //--------------------
      // Since we've moved to a new level, we might also say so, a message like
      // "Entering ThisAndThat..." should appear in bold font on the screen.
      //
      strcpy ( entering_message , "Entering " );
      strcat ( entering_message , curShip . AllLevels [ Me [ 0 ] . pos . z ] -> Levelname );
      SetNewBigScreenMessage ( entering_message );

    }
  else
    {
      //--------------------
      // If no real level change has occured, everything
      // is simple and we just need to set the new coordinates, haha
      //
      Me [ PlayerNum ] . pos . x = X ;
      Me [ PlayerNum ] . pos . y = Y ;
    }

  //--------------------
  // We reset the mouse move target, cause the old target
  // still referred to the old level...
  //
  Me [ PlayerNum ] . mouse_move_target . x = ( -1 ) ;
  Me [ PlayerNum ] . mouse_move_target . y = ( -1 ) ;
  Me [ PlayerNum ] . mouse_move_target . z = ( -1 ) ;

  if ( WithSound ) teleport_arrival_sound ();

  //--------------------
  // Perhaps the player is visiting this level for the first time.  Then, the
  // tux should make it's initial statement about the location, if there is one.
  //
  if ( ! Me [ PlayerNum ] . HaveBeenToLevel [ CurLevel->levelnum ] )
    {
      PlayLevelCommentSound ( CurLevel->levelnum );
      Me [ PlayerNum ] . HaveBeenToLevel [ CurLevel->levelnum ] = TRUE;
      // if ( array_num != 0 ) ShuffleEnemys ( array_num );
      // if ( ( LNum != 0 ) && ( Shuffling ) ) ShuffleEnemys ( array_num );
      ShuffleEnemys ( array_num );
    }

  /*
  //--------------------
  // Maybe the Tux hasn't *ever* been to this level before.  Then it's time
  // to enable the respawning time countdown for this level.
  //
  if ( Me [ PlayerNum ] . time_since_last_visit_or_respawn [ CurLevel -> levelnum ] < 0 )
    Me [ PlayerNum ] . time_since_last_visit_or_respawn [ CurLevel -> levelnum ] = 0 ;
  */

  if ( Shuffling ) ShuffleEnemys ( array_num );

  // UnfadeLevel ();

  SwitchBackgroundMusicTo( CurLevel->Background_Song_Name );

  //--------------------
  // Since we've mightily changed position now, we should clear the
  // position history, so that noone get's confused...
  //
  InitInfluPositionHistory ( PlayerNum );

}; // void Teleport( ... ) 

/* ----------------------------------------------------------------------
 * I'd like to use the function strnlen in LoadSettings, but this would
 * be a gnu extension. so I have to write this function myselt in order
 * to keep the code completely portable.
 * ---------------------------------------------------------------------- */
int
mystrnlen ( char* MyString , int MyMaxlen )
{
  int i;

  for ( i = 0 ; i < MyMaxlen ; i ++ )
    {
      if ( MyString [ i ] == 0 ) return i;
    }
  return MyMaxlen;
}; // int mystrnlen ( char* MyString , int MyMaxlen )

/*----------------------------------------------------------------------
 * LoadGameConfig(): load saved options from config-file
 *
 * this should be the first of all load/save functions called
 * as here we read the $HOME-dir and create the config-subdir if neccessary
 *
 *----------------------------------------------------------------------*/
int
LoadGameConfig (void)
{
  char fname[5000];
  FILE *config;

  if (!ConfigDir)
    {
      DebugPrintf (1, "No useable config-dir. No config-loading possible\n");
      return (OK);
    }

  sprintf (fname, "%s/config", ConfigDir);
  if( (config = fopen (fname, "r")) == NULL)
    {
      DebugPrintf (0, "WARNING: failed to open config-file: %s\n");
      return (ERR);
    }
  
  //--------------------
  // Now read the actual data
  // ok, this is neither very portable nor very flexible, we just want that working...
  fread ( &(GameConfig), sizeof (configuration_for_freedroid), sizeof(char), config);
  fclose (config);

  //--------------------
  // Now we do some extra security check:  Maybe the old settings file
  // was generated by a different version of freedroid.  Then extra care
  // must be taken:  The misinterpretation of values could case segmentation
  // faults and things like that.
  //
  // For this purpose I'd like to use the function 'strnlen' but this would be 
  // a gnu extension and not standard C, so I have to use the 'mystrlen' function
  // taken from the gnu c manual.
  //
  if ( ( mystrnlen ( GameConfig.freedroid_version_string , 100 ) != strlen ( VERSION ) ) ||
       ( memcmp ( GameConfig.freedroid_version_string , VERSION , strlen ( VERSION ) ) ) )
    {
      GiveStandardErrorMessage ( "LoadGameConfig(...)" , "\
Settings file found in your ~/.freedroid_rpg dir does not\n\
seem to be from the same version a this installation of freedroid.\n\
This is perfectly normal if you have just upgraded your version of\n\
freedroid.  But the loading of your settings will be cancelled now,\n\
cause the format of the settings file is no longer supported.  \n\
No need to panic.  The default settings will be used instead and a new\n\
settings file will be generated.",
				 NO_NEED_TO_INFORM, IS_WARNING_ONLY );
      ResetGameConfigToDefaultValues (  );
      return (ERR);
    };

  //--------------------
  // We may print out that config was loaded successfully...
  // This might stay in here.
  //
  DebugPrintf ( 0 , "\nSuccessfully loaded and enforced your personal configuration file %s.\n\n" , fname );

  //--------------------
  // Now we will turn off the skills and inventory screen and that, cause
  // this should be off when the game starts...
  //
  GameConfig . CharacterScreen_Visible = FALSE ;
  GameConfig . Inventory_Visible = FALSE ;
  GameConfig . Mission_Log_Visible = FALSE ;
  GameConfig . SkillScreen_Visible = FALSE ;
  GameConfig . Automap_Visible = FALSE ;

  return (OK);

}; // int LoadGameConfig ( void )
    
/*----------------------------------------------------------------------
 * SaveGameConfig: do just that
 *
 *----------------------------------------------------------------------*/
int
SaveGameConfig (void)
{
  char fname[5000];
  FILE *config;
  
  if ( ConfigDir[0] == '\0')
    return (ERR);
  
  sprintf (fname, "%s/config", ConfigDir);
  if( (config = fopen (fname, "w")) == NULL)
    {
      DebugPrintf (0, "WARNING: failed to create config-file: %s\n");
      return (ERR);
    }
  
  //--------------------
  // We put the current version number of freedroid into the 
  // version number string.  This will be usefull so that later
  // versions of freedroid can identify old config files and decide
  // not to use them in some cases.
  //
  strcpy ( GameConfig.freedroid_version_string , VERSION );
  
  //--------------------
  // Now write the actual data
  fwrite ( &(GameConfig), sizeof (configuration_for_freedroid), sizeof(char), config);
  fclose (config);

  return (OK);
  
}; // int SaveGameConfig ( void )


/* ----------------------------------------------------------------------
 * This function is used for terminating freedroid.  It will close
 * the SDL submodules and exit.
 * ---------------------------------------------------------------------- */
void
Terminate (int ExitCode)
{
  DebugPrintf (2, "\nvoid Terminate(int ExitStatus) was called....");
  printf("\n----------------------------------------------------------------------");
  printf("\nTermination of Freedroid initiated...");

  // SaveSettings();
  SaveGameConfig();

  if ( ServerMode )
    {
      DebugPrintf ( 0 , "\n\
--------------------\n\
Closing all players connections to this server...\n\
--------------------\n");
      DisconnectAllRemoteClinets ( ) ;
    }

  // printf("\nUnallocation all resouces...");

  // free the allocated surfaces...
  // SDL_FreeSurface( static_blocks );

  // free the mixer channels...
  // Mix_CloseAudio();

  // printf("\nAnd now the final step...\n\n");
  printf("Thank you for playing Freedroid.\n\n");
  SDL_Quit();

  // raise ( SIGSEGV );

  exit (ExitCode);

  return;
}; // void Terminate ( int ExitCode )

/* ----------------------------------------------------------------------
 * Since numbers are not so very telling and can easily get confusing
 * we do not use numbers to reference the action from a trigger but 
 * rather we use labels already in the mission file.  However internally
 * the game needs numbers as a pointer or index in a list and therefore
 * this functions was added to go from a label to the corresponding 
 * number entry.
 * ---------------------------------------------------------------------- */
int
GiveNumberToThisActionLabel ( char* ActionLabel )
{
  int i;

  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : real function call confirmed. ");
  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : executing event labeld : %s" , ActionLabel );

  // In case of 'none' as action label, we don't do anything and return;
  if ( strcmp ( ActionLabel , "none" ) == 0 ) return ( -1 );

  //--------------------
  // Now we find out which index the desired action has
  //
  for ( i = 0 ; i < MAX_TRIGGERED_ACTIONS_IN_GAME ; i++ )
    {
      if ( strcmp ( AllTriggeredActions[ i ].ActionLabel , ActionLabel ) == 0 ) break;
    }

  if ( i >= MAX_TRIGGERED_ACTIONS_IN_GAME )
    {
      fprintf( stderr, "\n\nActionLabel: '%s'\n" , ActionLabel );
      GiveStandardErrorMessage ( "GiveNumberToThisActionLabel(...)" , "\
The label that should reference an action for later execution could not\n\
be identified as valid reference to an existing action.",
				 PLEASE_INFORM, IS_FATAL );
    }

  return ( i );
}; // int GiveNumberToThisActionLabel ( char* ActionLabel )

/* ----------------------------------------------------------------------
 * This function executes an action with a label.
 * ---------------------------------------------------------------------- */
void 
ExecuteActionWithLabel ( char* ActionLabel , int PlayerNum )
{
  ExecuteEvent( GiveNumberToThisActionLabel ( ActionLabel ) , PlayerNum );
}; // void ExecuteActionWithLabel ( char* ActionLabel )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
obstacle* 
give_pointer_to_obstacle_with_label ( char* obstacle_label ) 
{
  int i, j , k;

  //--------------------
  // Now we try to resolve the obstacle given in the labels of all the decks
  // of the current ship.
  //
  for ( i = 0 ; i < curShip . num_levels ; i ++ )
    {
      for ( j = 0 ; j < MAX_OBSTACLE_NAMES_PER_LEVEL ; j ++ )
	{
	  if ( curShip . AllLevels [ i ] -> obstacle_name_list [ j ] != NULL )
	    {
	      if ( strcmp ( curShip . AllLevels [ i ] -> obstacle_name_list [ j ] , obstacle_label ) == 0 )
		{
		  //--------------------
		  // Now we need to find out which obstacle is pointing to this label
		  //
		  for ( k = 0 ; k < MAX_OBSTACLES_ON_MAP ; k ++ )
		    {
		      if ( curShip . AllLevels [ i ] -> obstacle_list [ k ] . name_index == j )
			{
			  DebugPrintf ( 0 , "\nSUCCESSFULLY RESOLVED OBSTACLE NAME %s." , 
					obstacle_label );
			  return ( & ( curShip . AllLevels [ i ] -> obstacle_list [ k ] ) );
			}
		    }
		  //--------------------
		  // So here we've encountered an error!  There seems to be no obstacle
		  // pointing to this obstacle label (any more).
		  //
		  GiveStandardErrorMessage ( "give_pointer_to_obstacle_with_label(...)" , "\
The obstacle label was found in the label list,\n but no obstacle seems to point to this label." ,
					     PLEASE_INFORM, IS_FATAL );
		}
	    }
	}
    }

  //--------------------
  // So here we've encountered an error!  There seems to be no obstacle
  // pointing to this obstacle label (any more).
  //
  GiveStandardErrorMessage ( "give_pointer_to_obstacle_with_label(...)" , "\
The obstacle label given was NOT found in any levels obstacle label list." ,
			     PLEASE_INFORM, IS_FATAL );
  return ( NULL ) ;

}; // obstacle* give_pointer_to_obstacle_with_label ( char* obstacle_label ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
give_level_of_obstacle_with_label ( char* obstacle_label ) 
{
  int i, j , k;

  //--------------------
  // Now we try to resolve the obstacle given in the labels of all the decks
  // of the current ship.
  //
  for ( i = 0 ; i < curShip . num_levels ; i ++ )
    {
      for ( j = 0 ; j < MAX_OBSTACLE_NAMES_PER_LEVEL ; j ++ )
	{
	  if ( curShip . AllLevels [ i ] -> obstacle_name_list [ j ] != NULL )
	    {
	      if ( strcmp ( curShip . AllLevels [ i ] -> obstacle_name_list [ j ] , obstacle_label ) == 0 )
		{
		  //--------------------
		  // Now we need to find out which obstacle is pointing to this label
		  //
		  for ( k = 0 ; k < MAX_OBSTACLES_ON_MAP ; k ++ )
		    {
		      if ( curShip . AllLevels [ i ] -> obstacle_list [ k ] . name_index == j )
			{
			  DebugPrintf ( 0 , "\nSUCCESSFULLY RESOLVED OBSTACLE NAME %s." , 
					obstacle_label );
			  return ( i ) ;
			}
		    }
		  //--------------------
		  // So here we've encountered an error!  There seems to be no obstacle
		  // pointing to this obstacle label (any more).
		  //
		  GiveStandardErrorMessage ( "give_level_of_obstacle_with_label(...)" , "\
The obstacle label was found in the label list,\n but no obstacle seems to point to this label." ,
					     PLEASE_INFORM, IS_FATAL );
		}
	    }
	}
    }

  //--------------------
  // So here we've encountered an error!  There seems to be no obstacle
  // pointing to this obstacle label (any more).
  //
  GiveStandardErrorMessage ( "give_level_of_obstacle_with_label(...)" , "\
The obstacle label given was NOT found in any levels obstacle label list." ,
			     PLEASE_INFORM, IS_FATAL );
  return ( -1 ) ;

}; // int give_level_of_obstacle_with_label ( char* obstacle_label ) 

/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void 
ExecuteEvent ( int EventNumber , int PlayerNum )
{
  obstacle* our_obstacle;
  int obstacle_level_num ;
  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : real function call confirmed. ");
  // DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : executing event Nr.: %d." , EventNumber );

  // Do nothing in case of the empty action (-1) given.
  if ( EventNumber == (-1) ) return;

  // Does the action include a change of a map tile?
  if ( AllTriggeredActions[ EventNumber ].ChangeMapTo != -1 )
    {
      //YES.  So we need to check, if the location has been supplied fully first.
      if ( ( AllTriggeredActions[ EventNumber ].ChangeMapLocation.x == (-1) ) ||
	   ( AllTriggeredActions[ EventNumber ].ChangeMapLocation.y == (-1) ) ||
	   ( AllTriggeredActions[ EventNumber ].ChangeMapLevel == (-1) ) )
	{
	  DebugPrintf( 0 , "\n\nSorry! There has been a corrupt event specification!\n\nTerminating...\n\n");
	  Terminate(ERR);
	}
      else
	{
	  DebugPrintf( 1 , "\nvoid ExecuteEvent ( int EventNumber ) : Change map Event correctly specified. confirmed.");
	  curShip.AllLevels[ AllTriggeredActions[ EventNumber ].ChangeMapLevel ]->map [ AllTriggeredActions[ EventNumber ].ChangeMapLocation.y ] [ AllTriggeredActions[ EventNumber ].ChangeMapLocation.x ]  . floor_value  = AllTriggeredActions[ EventNumber ].ChangeMapTo ;
	  
	  GetAllAnimatedMapTiles ( curShip.AllLevels[ AllTriggeredActions[ EventNumber ].ChangeMapLevel ]  );
	}
    }

  //--------------------
  // Maybe the action will cause a map obstacle to change type.  If that is so, we
  // do it here...
  //
  if ( strlen ( AllTriggeredActions [ EventNumber ] . modify_obstacle_with_label ) > 0 )
    {
      
      our_obstacle = give_pointer_to_obstacle_with_label ( AllTriggeredActions [ EventNumber ] . modify_obstacle_with_label ) ;
      obstacle_level_num = give_level_of_obstacle_with_label ( AllTriggeredActions [ EventNumber ] . modify_obstacle_with_label ) ;
      our_obstacle -> type = AllTriggeredActions [ EventNumber ] . modify_obstacle_to_type ;
      //--------------------
      // Now we make sure the door lists and that are all updated...
      //
      GetAllAnimatedMapTiles ( curShip.AllLevels[ obstacle_level_num ] );
    }

  // Does the action include a teleport of the influencer to some other location?
  if ( AllTriggeredActions[ EventNumber ].TeleportTarget.x != (-1) )
    {
      Teleport ( AllTriggeredActions[ EventNumber ].TeleportTargetLevel ,
		 AllTriggeredActions[ EventNumber ].TeleportTarget.x + 0.5 ,
		 AllTriggeredActions[ EventNumber ].TeleportTarget.y + 0.5 ,
		 PlayerNum , FALSE , TRUE );
    }

  // Does the defined action assign the influencer a mission?
  if ( AllTriggeredActions[ EventNumber ].AssignWhichMission != (-1) )
    {
      AssignMission( AllTriggeredActions[ EventNumber ].AssignWhichMission );
    }

  // Does the defined action make the influencer say something?
  if ( strlen ( AllTriggeredActions[ EventNumber ].InfluencerSayText ) > 0 )
    {
      //YES. So we need to output his sentence as usual
      Me[0].TextVisibleTime=0;
      Me[0].TextToBeDisplayed=AllTriggeredActions[ EventNumber ].InfluencerSayText;
    }

  

}; // void ExecuteEvent ( int EventNumber )

/* ----------------------------------------------------------------------
 *
 * This function checks for triggered events & statements.  Those events are
 * usually entered via the mission file and read into the apropriate
 * structures via the InitNewMission function.  Here we check, whether
 * the nescessary conditions for an event are satisfied, and in case that
 * they are, we order the apropriate event to be executed.
 *
 * In addition, statements are started, if the influencer is at the 
 * right location for them.
 *
 * ---------------------------------------------------------------------- */
void 
CheckForTriggeredEventsAndStatements ( int PlayerNum )
{
  int i;
  int map_x, map_y;

  Level StatementLevel = curShip.AllLevels[ Me [ PlayerNum ] . pos . z ] ;

  //--------------------
  // Now we check if some statment location is reached
  //
  map_x = (int) rintf( (float) Me [ PlayerNum ] . pos . x ); map_y = (int) rintf( (float)Me [ PlayerNum ] . pos . y ) ;
  for ( i = 0 ; i < MAX_STATEMENTS_PER_LEVEL ; i++ )
    {
      if ( ( map_x == StatementLevel -> StatementList [ i ] . x ) &&
	   ( map_y == StatementLevel -> StatementList [ i ] . y ) )
	{
	  Me [ PlayerNum ] . TextVisibleTime = 0 ;
	  Me [ PlayerNum ] . TextToBeDisplayed = CurLevel -> StatementList [ i ] . Statement_Text ;
	}
    }

  //--------------------
  // Now we check if some event trigger is fullfilled.
  //
  for ( i = 0 ; i < MAX_EVENT_TRIGGERS ; i++ )
    {
      // if ( AllEventTriggers[i].EventNumber == (-1) ) continue;  // thats a sure sign this event doesn't need attention
      if ( strcmp (AllEventTriggers[i].TargetActionLabel , "none" ) == 0 ) continue;  // thats a sure sign this event doesn't need attention

      // --------------------
      // So at this point we know, that the event trigger is somehow meaningful. 
      // Fine, so lets check the details, if the event is triggered now
      //

      if ( AllEventTriggers[i].Influ_Must_Be_At_Point.x != (-1) )
	{
	  if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Point.x ) != (int) ( Me [ PlayerNum ] . pos.x ) ) continue;
	}

      if ( AllEventTriggers[i].Influ_Must_Be_At_Point.y != (-1) )
	{
	  if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Point.y ) != (int) ( Me [ PlayerNum ] . pos.y ) ) continue;
	}

      if ( AllEventTriggers[i].Influ_Must_Be_At_Level != (-1) )
	{
	  if ( rintf( AllEventTriggers[i].Influ_Must_Be_At_Level ) != StatementLevel->levelnum ) continue;
	}

      // printf("\nWARNING!! INFLU NOW IS AT SOME TRIGGER POINT OF SOME LOCATION-TRIGGERED EVENT!!!");
      // ExecuteEvent( AllEventTriggers[i].EventNumber );
      ExecuteActionWithLabel ( AllEventTriggers [ i ] . TargetActionLabel , PlayerNum ) ;

      if ( AllEventTriggers[i].DeleteTriggerAfterExecution == 1 )
	{
	  // AllEventTriggers[i].EventNumber = (-1); // That should prevent the event from being executed again.
	  AllEventTriggers[i].TargetActionLabel = "none"; // That should prevent the event from being executed again.
	}
    }

}; // CheckForTriggeredEventsAndStatements (void )


/*----------------------------------------------------------------------
 * Copyright (C) 1997-2001 Id Software, Inc., under GPL
 *
 * va(fmt, ...)
 * 
 * does a varargs printf into a temp buffer, so I don't need to have
 * varargs versions of all text functions.
 *----------------------------------------------------------------------*/
char
*va (char *format, ...)
{
  va_list	argptr;
  static char	string[1024];
  
  va_start (argptr, format);
  vsprintf (string, format,argptr);
  va_end (argptr);
  
  return string;	
}



#undef _misc_c
