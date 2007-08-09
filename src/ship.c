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
 * This file contains all the console and lift functions (mostly)
 * ---------------------------------------------------------------------- */

/*
 * This file has been checked for remains of german in the documentation.
 * They should be all out by now, and if you still find any, please do not
 * hesitate to remove them.
 */

#define _ship_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "SDL_rotozoom.h"

enum 
  {
    NO_FUNCTION,
    UNLOCK_FUNCTION,
    GUNON_FUNCTION,
    GUNOFF_FUNCTION
  };

int NoKeyPressed (void);
int GreatItemShow ( int NumberOfItems , item* ShowPointerList[ MAX_ITEMS_IN_INVENTORY ] );

void ShowDroidPicture (int PosX, int PosY, int Number );

char *EmailText1 = N_("Hello Tux!\n\nHello Tux!\nI'm glad to see you alive and moving again.\nYour awakening has stirred considerable amounts of dicsussion within our group, but it's a bit too early to disclose anything yet.\nAnyway, we're glad you're back.");

char *EmailText2 = N_("The FreedroidRPG maps are currently full of bugs.\n\nBut that's only the start of our problems.\nWe also need more characters, maps and story and quests.\nPlease take a look at he 'contribute' section of the main menu to see on what ways you could aid the FreedroidRPG development team.\n\nThanks a lot and see ya, the FreedroidRPG dev team.");

/* ----------------------------------------------------------------------
 * This function does the item show when the user has selected item
 * show from the console menu.
 * ---------------------------------------------------------------------- */
int
GreatItemShow ( int NumberOfItems , item* ShowPointerList[ MAX_ITEMS_IN_INVENTORY ] )
{
    int ItemType;
    int Displacement=0;
    bool finished = FALSE;
    static int WasPressed = FALSE ;
    // item* ShowPointerList[ MAX_ITEMS_IN_INVENTORY ];
    // int NumberOfItems;
    int ItemIndex=0;
    int PasswordIndex = (-1) ;
    // int ClearanceIndex = (-1) ;
    int IdentifyAllowed = FALSE ;
    char* MenuTexts[ 10 ];
    MenuTexts[0]=_("Yes");
    MenuTexts[1]=_("No");
    MenuTexts[2]="";
    
    //--------------------
    // We initialize the text rectangle
    //
    Cons_Text_Rect . x = 258 ;
    Cons_Text_Rect . y = 89 ;
    Cons_Text_Rect . w = 346 ;
    Cons_Text_Rect . h = 282 ;
    
    // NumberOfItems = AssemblePointerListForItemShow ( &(ShowPointerList[0]), 0 );
    
    if ( ShowPointerList[0] == NULL )
    {
	MenuTexts[0]=_(" BACK ");
	MenuTexts[1]="";
	DoMenuSelection ( _(" YOU DONT HAVE ANYTHING IN INVENTORY, THAT COULD BE VIEWED. "), 
			  MenuTexts, 1 , -1 , NULL );
	return (-1) ;
    }
    
    ItemType = ShowPointerList [ ItemIndex ] -> type ;
    
    Displacement = 0;
    
    while (!finished)
    {
	SDL_Delay (1);
	
	//--------------------
	// We show all the info and the buttons that should be in this
	// interface...
	//
	ShowItemInfo ( ShowPointerList [ ItemIndex ] , Displacement , TRUE , ITEM_BROWSER_BG_PIC_BACKGROUND_CODE , TRUE );
	
	// PutPasswordButtonsAndPassword ( PasswordIndex );
	// PutSecurityButtonsAndClearance ( ClearanceIndex );
	blit_our_own_mouse_cursor ( );
	our_SDL_flip_wrapper( Screen );
	
	ItemType = ShowPointerList [ ItemIndex ] -> type ;
	
	if (SpacePressed() || EscapePressed() || MouseLeftPressed() )
	{
	    if ( MouseCursorIsOnButton( ITEM_BROWSER_RIGHT_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) && MouseLeftPressed() && !WasPressed )
	    {
		if ( ItemIndex < NumberOfItems -1 ) 
		{
		    ItemIndex ++;	    
		    MoveMenuPositionSound();
		    Displacement = 0;
		}
	    }
	    else if ( MouseCursorIsOnButton( ITEM_BROWSER_LEFT_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) && MouseLeftPressed() && !WasPressed )
	    {
		if ( ItemIndex > 0) 
		{
		    ItemIndex --;	      
		    MoveMenuPositionSound();
		    Displacement = 0;
		}
	    }
	    else if ( MouseCursorIsOnButton( UP_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) && MouseLeftPressed() && !WasPressed )
	    {
		MoveMenuPositionSound();
		Displacement += FontHeight ( GetCurrentFont () );
	    }
	    else if ( MouseCursorIsOnButton( DOWN_BUTTON , GetMousePos_x()  , GetMousePos_y()  ) && MouseLeftPressed() && !WasPressed )
	    {
		MoveMenuPositionSound();
		// if (page > 0) page --;
		Displacement -= FontHeight ( GetCurrentFont () );
	    }
	    else if ( MouseCursorIsOnButton( ITEM_BROWSER_EXIT_BUTTON , GetMousePos_x ( )  , GetMousePos_y ( )  ) && MouseLeftPressed() && !WasPressed )
	    {
		finished = TRUE;
		while (SpacePressed() ||EscapePressed());
	    }
	    
	}
	
	WasPressed = MouseLeftPressed();
	
	if (UpPressed() || MouseWheelUpPressed())
	{
	    MoveMenuPositionSound();
	    while (UpPressed());
	    Displacement += FontHeight ( GetCurrentFont () );
	}
	if (DownPressed() || MouseWheelDownPressed())
	{
	    MoveMenuPositionSound();
	    while (DownPressed());
	    Displacement -= FontHeight ( GetCurrentFont () );
	}
	if (RightPressed() )
	{
	    MoveMenuPositionSound();
	    while (RightPressed());
	    if ( ItemType < Me[0].type) ItemType ++;
	}
	if (LeftPressed() )
	{
	    MoveMenuPositionSound();
	    while (LeftPressed());
	    if (ItemType > 0) ItemType --;
	}
	
	if ( EscapePressed() )
	{
	    while ( EscapePressed() );
	    return (-1);
	}
	
    } // while !finished 
    
    return ( ItemIndex ) ;  // Currently equippment selection is not yet possible...
    
}; // int GreatItemShow ( int NumberOfItems , item* ShowPointerList[ MAX_ITEMS_IN_INVENTORY ] )

/* ------------------------------------------------------------
 * display infopage page of droidtype
 * does update the screen, no our_SDL_flip_wrapper() necesary !
 * ------------------------------------------------------------ */
void 
ShowDroidInfo ( int droidtype, int Displacement , char ShowArrows )
{
    char *item_name;
    int type;
    char InfoText[10000];
    char TextChunk[2000];
    
    //--------------------
    // We initialize the text rectangle
    //
    Cons_Text_Rect . x = 258 * GameConfig . screen_width / 640 ; 
    Cons_Text_Rect . y = 89 * GameConfig . screen_height / 480 ; 
    Cons_Text_Rect . w = 346 * GameConfig . screen_width / 640 ; 
    Cons_Text_Rect . h = 282 * GameConfig . screen_height / 480 ;
    
    SDL_SetClipRect ( Screen , NULL );
    
    blit_special_background ( ITEM_BROWSER_BG_PIC_BACKGROUND_CODE ) ;
    
    ShowDroidPicture ( 45 * GameConfig . screen_width / 640 , 190 * GameConfig . screen_height / 480 , droidtype );
    
    //--------------------
    // We fill out the header area of the items browser.
    //
    SetCurrentFont ( Menu_BFont );
    strcpy ( TextChunk , Druidmap [ droidtype ] . druidname );
    CutDownStringToMaximalSize ( TextChunk , 225 );
    PutString ( Screen , 330 * GameConfig . screen_width / 640 , 38 * GameConfig . screen_height / 480 , TextChunk );
    
    sprintf( InfoText, _("\
Unit type %s - %s\n\
Entry : %d\n\
Class : %s\n\
Drive : %s \n\
Brain : %s"), Druidmap[droidtype].druidname, _(Classname[Druidmap[ droidtype ].class]),
	     droidtype+1, _(Classes[Druidmap[droidtype].class]),
	     ItemMap [ Druidmap[ droidtype ].drive_item.type ].item_name,
	     _(Brainnames[ Druidmap[droidtype].brain ]));
    
    if ( (type = Druidmap[droidtype].weapon_item.type) >= 0) // make sure item=-1 
	item_name = ItemMap[type].item_name;                 // does not segfault 
    else 
	item_name = "none";
    
    sprintf( TextChunk , _("\nArmamant : %s\n\
Sensors  1: %s\n          2: %s\n          3: %s"),
	     item_name,
	     _(Sensornames[ Druidmap[droidtype].sensor1 ]),
	     _(Sensornames[ Druidmap[droidtype].sensor2 ]),
	     _(Sensornames[ Druidmap[droidtype].sensor3 ]));
    strcat ( InfoText , TextChunk );
    
    sprintf ( TextChunk , _("\nNotes: %s\n"), Druidmap[droidtype].notes);
    strcat ( InfoText , TextChunk );
    
    SetCurrentFont( FPS_Display_BFont );
    DisplayText (InfoText, Cons_Text_Rect.x, Cons_Text_Rect.y + Displacement , &Cons_Text_Rect , TEXT_STRETCH );
    
    if ( ShowArrows ) 
    {
	ShowGenericButtonFromList ( UP_BUTTON );
	ShowGenericButtonFromList ( DOWN_BUTTON );
    }
    
}; // void ShowDroidInfo ( ... )

/* ----------------------------------------------------------------------
 * This function displays an item picture. 
 * ---------------------------------------------------------------------- */
void
ShowItemPicture (int PosX, int PosY, int Number )
{
    SDL_Surface *tmp;
    SDL_Rect target;
    char ConstructedFileName[2048];
    char fpath[2048];
    static char LastImageSeriesPrefix[1000] = "NONE_AT_ALL";
    static int NumberOfImagesInPreviousRotation = 0 ;
    static int NumberOfImagesInThisRotation = 0 ;
#define NUMBER_OF_IMAGES_IN_ITEM_ROTATION 16
#define MAX_NUMBER_OF_IMAGES_IN_ITEM_ROTATION 100
    static SDL_Surface *ItemRotationSurfaces[ MAX_NUMBER_OF_IMAGES_IN_ITEM_ROTATION ] = { NULL } ;
    SDL_Surface *Whole_Image;
    int i;
    int RotationIndex;
    
    // DebugPrintf (2, "\nvoid ShowItemPicture(...): Function call confirmed.");
    
    // if ( !strcmp ( ItemMap[ Number ] . item_rotation_series_prefix , "NONE_AVAILABLE_YET" ) )
    // return; // later this should be a default-correction instead
    
    //--------------------
    // Maybe we have to reload the whole image series
    //
    if ( strcmp ( LastImageSeriesPrefix , ItemMap [ Number ] . item_rotation_series_prefix ) )
    {
	//--------------------
	// Maybe we have to free the series from an old item display first
	//
	if ( ItemRotationSurfaces[ 0 ] != NULL )
	{
	    for ( i = 0 ; i < NumberOfImagesInPreviousRotation ; i ++ )
	    {
		SDL_FreeSurface ( ItemRotationSurfaces[ i ] ) ;
	    }
	}
	
	//--------------------
	// Now we can start to load the whole series into memory
	//
	for ( i=0 ; i < MAX_NUMBER_OF_IMAGES_IN_ITEM_ROTATION ; i++ )
	{
	    //--------------------
	    // At first we will try to find some item rotation models in the
	    // new directory structure.
	    //
	    sprintf ( ConstructedFileName , "items/%s/portrait_%04d.jpg" , ItemMap[ Number ] . item_rotation_series_prefix , i+1 );
	    if ( find_file ( ConstructedFileName , GRAPHICS_DIR, fpath, 1 ) )
		Whole_Image = NULL ;
	    else
		Whole_Image = our_IMG_load_wrapper( fpath ); // This is a surface with alpha channel, since the picture is one of this type
	    
	    //--------------------
	    // If that didn't work, then it's time to try the same directory with 'png' ending...
	    // Maybe there's still some (old) rotation image of this kind.
	    //
	    if ( Whole_Image == NULL )
	    {
		DebugPrintf ( 1 , "\nNo luck trying to load .jpg item image series from the 'bastian' dir... trying png..." );
		sprintf ( ConstructedFileName , "items/%s/portrait_%04d.png" , ItemMap[ Number ] . item_rotation_series_prefix , i+1 );
		if ( find_file ( ConstructedFileName , GRAPHICS_DIR, fpath, 1 ) )
		    Whole_Image = NULL ;
		else
		    Whole_Image = our_IMG_load_wrapper( fpath ); // This is a surface with alpha channel, since the picture is one of this type
	    }
	    
	    //--------------------
	    // If that didn't work, then it's time to try out the 'classic' rotation models directory.
	    // Maybe there's still some rotation image there.
	    //
	    if ( Whole_Image == NULL )
	    {
		DebugPrintf ( 1 , "\nNo luck trying to load .png item image series from the 'bastian' dir... trying 'classic' dir..." );
		sprintf ( ConstructedFileName , "rotation_models/items/%s_%04d.jpg" , ItemMap[ Number ] . item_rotation_series_prefix , i+1 );
		if ( find_file ( ConstructedFileName , GRAPHICS_DIR, fpath, 1) )
		    Whole_Image = NULL ;
		else
		    Whole_Image = our_IMG_load_wrapper( fpath ); // This is a surface with alpha channel, since the picture is one of this type
	    }
	    
	    if ( Whole_Image == NULL )
	    {
		DebugPrintf ( 1 , "\nNo luck trying to load .jpg item image series from 'classic' dir... trying png..." );
		sprintf ( ConstructedFileName , "rotation_models/items/%s_%04d.png" , ItemMap[ Number ] . item_rotation_series_prefix , i+1 );
		if ( find_file ( ConstructedFileName , GRAPHICS_DIR, fpath, 1 ) )
		    Whole_Image = NULL ;
		else
		    Whole_Image = our_IMG_load_wrapper( fpath ); // This is a surface with alpha channel, since the picture is one of this type
	    }
	    // }
	    
	    //--------------------
	    // But at this point, we should have found the image!!
	    // or if not, this maybe indicates that we have reached the
	    // last image in the image series...
	    //
	    if ( Whole_Image == NULL )
	    {
		NumberOfImagesInThisRotation = i ;
		NumberOfImagesInPreviousRotation = NumberOfImagesInThisRotation ;
		DebugPrintf ( 1 , "\nDONE LOADING ITEM IMAGE SERIES.  Loaded %d images into memory." , 
			      NumberOfImagesInThisRotation );
		
		//--------------------
		// Maybe we've received the nothing loaded case even on the first attempt
		// to load something.  This of course would mean a severe error in Freedroid!
		//
		if ( NumberOfImagesInThisRotation <= 0 )
		{
		    fprintf( stderr, "\n\nfpath: %s. \n" , fpath );
		    GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid was unable to load even one image of a rotated item image series into memory.\n\
This error indicates some installation problem with freedroid.",
					       PLEASE_INFORM, IS_FATAL );
		}
		
		break;
	    }
	    
	    //--------------------
	    // Also we must check for our upper bound of the list of 
	    // item images.  This will most likely never be exceeded, but it
	    // can hurt to just be on the safe side.
	    //
	    if ( i >= MAX_NUMBER_OF_IMAGES_IN_ITEM_ROTATION -2 )
	    {
		fprintf( stderr, "\n\nfpath: %s. \n" , fpath );
		GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid was encountered more item images in an item rotation image series\n\
than it is able to handle.  This is a very strange error.  Someone has been\n\
trying to make the ultra-fine item rotation series.  Strange.",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    
	    SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
	    ItemRotationSurfaces[i] = our_SDL_display_format_wrapperAlpha( Whole_Image ); // now we have an alpha-surf of right size
	    SDL_SetColorKey( ItemRotationSurfaces[i] , 0 , 0 ); // this should clear any color key in the dest surface
	    SDL_FreeSurface( Whole_Image );
	    
	    // We must remember, that his is already loaded of course
	    strcpy ( LastImageSeriesPrefix , ItemMap [ Number ] . item_rotation_series_prefix );
	    
	}
	
    }
    
    RotationIndex = ( SDL_GetTicks() / 70 ) ;
    
    RotationIndex = RotationIndex - ( RotationIndex / NumberOfImagesInThisRotation ) * NumberOfImagesInThisRotation ;
    
    tmp = ItemRotationSurfaces[ RotationIndex ] ;
    
    SDL_SetClipRect( Screen , NULL );
    Set_Rect ( target, PosX, PosY, GameConfig . screen_width, GameConfig . screen_height);
    our_SDL_blit_surface_wrapper( tmp , NULL, Screen , &target);
    
    DebugPrintf ( 2 , "\n%s(): Usual end of function reached." , __FUNCTION__ );
    
}; // void ShowItemPicture ( ... )


/* ----------------------------------------------------------------------
 * This function displays an item picture. 
 * ---------------------------------------------------------------------- */
void
ShowDroidPicture (int PosX, int PosY, int Number )
{
    SDL_Surface *tmp;
    SDL_Rect target;
    char ConstructedFileName[5000];
char fpath[2048];
    static char LastImageSeriesPrefix[1000] = "NONE_AT_ALL";
#define NUMBER_OF_IMAGES_IN_DROID_PORTRAIT_ROTATION 32
    static SDL_Surface *DroidRotationSurfaces[ NUMBER_OF_IMAGES_IN_DROID_PORTRAIT_ROTATION ] = { NULL } ;
    SDL_Surface *Whole_Image;
    int i;
    int RotationIndex;
    
    DebugPrintf ( 2 , "\nvoid ShowDroidPicture(...): Function call confirmed.");
    
    if ( !strcmp ( Druidmap[ Number ] . droid_portrait_rotation_series_prefix , "NONE_AVAILABLE_YET" ) )
	return; // later this should be a default-correction instead
    
    //--------------------
    // Maybe we have to reload the whole image series
    //
    if ( strcmp ( LastImageSeriesPrefix , Druidmap [ Number ] . droid_portrait_rotation_series_prefix ) )
    {
	//--------------------
	// Maybe we have to free the series from an old item display first
	//
	if ( DroidRotationSurfaces[ 0 ] != NULL )
	{
	    for ( i = 1 ; i < NUMBER_OF_IMAGES_IN_DROID_PORTRAIT_ROTATION ; i ++ )
	    {
		SDL_FreeSurface ( DroidRotationSurfaces[ i ] ) ;
	    }
	}
	
	//--------------------
	// Now we can start to load the whole series into memory
	//
	for ( i=0 ; i < NUMBER_OF_IMAGES_IN_DROID_PORTRAIT_ROTATION ; i++ )
	{
	    if ( !strcmp ( Druidmap[ Number ] . droid_portrait_rotation_series_prefix , "NONE_AVAILABLE_YET" ) )
	    {
		Terminate ( ERR );
	    }
	    else
	    {
		sprintf ( ConstructedFileName , "droids/%s/portrait_%04d.jpg" , Druidmap[ Number ] . droid_portrait_rotation_series_prefix , i+1 );
		DebugPrintf ( 1 , "\nConstructedFileName = %s " , ConstructedFileName );
	    }
	    
	    // We must remember, that his is already loaded of course
	    strcpy ( LastImageSeriesPrefix , Druidmap [ Number ] . droid_portrait_rotation_series_prefix );
	    
	    find_file (ConstructedFileName , GRAPHICS_DIR, fpath, 0 );
	    
	    Whole_Image = our_IMG_load_wrapper( fpath ); // This is a surface with alpha channel, since the picture is one of this type
	    if ( Whole_Image == NULL )
	    {
		fprintf( stderr, "\n\nfpath: %s. \n" , fpath );
		GiveStandardErrorMessage ( __FUNCTION__  , "\
Freedroid was unable to load an image of a rotated droid into memory.\n\
This error indicates some installation problem with freedroid.",
					   PLEASE_INFORM, IS_FATAL );
	    }
	    
	    SDL_SetAlpha( Whole_Image , 0 , SDL_ALPHA_OPAQUE );
	    
	    DroidRotationSurfaces[i] = our_SDL_display_format_wrapperAlpha( Whole_Image ); // now we have an alpha-surf of right size
	    SDL_SetColorKey( DroidRotationSurfaces[i] , 0 , 0 ); // this should clear any color key in the dest surface
	    
	    SDL_FreeSurface( Whole_Image );
	}
    }

    RotationIndex = ( SDL_GetTicks() / 50 ) ;
    
    RotationIndex = RotationIndex - ( RotationIndex / NUMBER_OF_IMAGES_IN_DROID_PORTRAIT_ROTATION ) * NUMBER_OF_IMAGES_IN_DROID_PORTRAIT_ROTATION ;
    
    tmp = DroidRotationSurfaces[ RotationIndex ] ;
    
    SDL_SetClipRect( Screen , NULL );
    Set_Rect ( target, PosX, PosY, GameConfig . screen_width, GameConfig . screen_height);
    our_SDL_blit_surface_wrapper( tmp , NULL, Screen , &target);
    
    DebugPrintf ( 2 , "\nvoid ShowDroidPicture(...): Usual end of function reached.");
    
}; // void ShowDroidPicture ( ... )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
write_full_item_name_into_string ( item* ShowItem , char* full_item_name ) 
{
  strcpy ( full_item_name , "" );

  // --------------------
  // First clear the string and the we print out the item name.  That's simple.
  // we also add the extension of the name, the 'suffix' to it.
  //
  if ( ( ShowItem->suffix_code != (-1) ) || ( ShowItem->prefix_code != (-1) ) )
    {
      strcat ( full_item_name , font_switchto_blue );
    }
  else
    {
      strcat ( full_item_name , font_switchto_neon );
    }

  if ( ShowItem->type == ITEM_MONEY ) sprintf( full_item_name , "%d " , ShowItem->gold_amount );

  //--------------------
  // If the item is is magical, we give the prefix name of course.
  // In any case we'll give the suffix name and then, if the item
  // is identified we'll also append any suffix to the description
  // string.
  //
  if ( ( ShowItem->prefix_code != (-1) ) && ( ShowItem->is_identified ) )
    strcat( full_item_name , PrefixList[ ShowItem->prefix_code ].bonus_name );
  strcat( full_item_name , ItemMap[ ShowItem->type ].item_name );
  if ( ( ShowItem->suffix_code != (-1) ) && ( ShowItem->is_identified ) )
    strcat( full_item_name , SuffixList[ ShowItem->suffix_code ].bonus_name );

  //--------------------
  // If the item is magical but not identified, we might add the word
  // in parentheses and red font afterwards...
  //
  if ( ( ( ShowItem -> suffix_code != (-1) ) || ( ShowItem -> prefix_code != (-1) ) ) && ( ! ShowItem -> is_identified ) )
    {
      strcat ( full_item_name , font_switchto_red );
      strcat ( full_item_name , _(" (Unidentified)"));
    }

  //--------------------
  // Now that the item name is out, we can switch back to the standard font color...
  //
  strcat ( full_item_name , font_switchto_neon );

}; // void write_full_item_name_into_string ( item* ShowItem , char* full_item_name ) 

/* ------------------------------------------------------------
 * This function displays information about one item on a
 * Paradroid-console like display.
 * ------------------------------------------------------------ */
void 
ShowItemInfo ( item* ShowItem , int Displacement , char ShowArrows , int background_code , int title_text_flag )
{
    char InfoText[10000];
    char TextChunk[2000];
    char* ClassString;
    long int repairPrice = 0;
    
    SDL_SetClipRect ( Screen , NULL );
    
    blit_special_background ( background_code );
    ShowItemPicture ( 40 * GameConfig . screen_width / 1024 + ((250 * GameConfig . screen_width / 1024) - 132) / 2 , 
		      185 * GameConfig . screen_height / 768 + ((322 * GameConfig . screen_height / 768) - 180) / 2, ShowItem->type );
    
    //--------------------
    // If that is wanted, we fill out the title header line, announcing the
    // currently browsed items name in full glory.
    //
    if ( title_text_flag )
    {
	SetCurrentFont ( Menu_BFont );
	strcpy ( TextChunk , ItemMap [ ShowItem->type ] . item_name );
	CutDownStringToMaximalSize ( TextChunk , 225 );
	PutString ( Screen , 330, 38, TextChunk );
    }
    
    //--------------------
    // Now we can display the rest of the smaller-font item description.
    //
    if ( ItemMap [ ShowItem->type ] . item_can_be_installed_in_weapon_slot )
	ClassString = _("Weapon");
    else if ( ItemMap [ ShowItem->type ] . item_can_be_installed_in_drive_slot )
	ClassString = _("Drive"); 
    else if ( ItemMap [ ShowItem->type ] . item_can_be_installed_in_armour_slot )
	ClassString = _("Armour"); 
    else if ( ItemMap [ ShowItem->type ] . item_can_be_installed_in_shield_slot )
	ClassString = _("Shield"); 
    else if ( ItemMap [ ShowItem->type ] . item_can_be_installed_in_special_slot )
	ClassString = _("Helm"); 
    else if ( ItemMap [ ShowItem->type ] . item_can_be_installed_in_aux_slot )
	ClassString = _("Wristband/Collar"); 
    else ClassString = _("Miscellaneous"); 
    
    write_full_item_name_into_string ( ShowItem , TextChunk ) ;
    sprintf( InfoText, _("Item: %s \nClass: %s\n"), TextChunk , ClassString );
    
    if ( ( ShowItem->suffix_code != (-1) ) || ( ShowItem->prefix_code != (-1) ) )
    	{
        if ( ShowItem->is_identified == TRUE )
            {
	    GiveItemDescription( TextChunk, ShowItem, TRUE );
            // Now clean up TextChunk to get only information about the special abilities of the object
	    char * tmp = strstr(TextChunk, "             +");
	    if(tmp) //In case something would go wrong...
		{
		tmp = strstr(tmp, " +");
		strcpy(TextChunk, tmp);
		}
	    strcat( InfoText, _("Specials:"));
	    strcat( InfoText, font_switchto_red);
	    strcat( InfoText, TextChunk);
	    strcat( InfoText, "\n");
	    strcat( InfoText, font_switchto_neon);
	    }
        }
    
    if ( ItemMap [ ShowItem->type ] . item_group_together_in_inventory )
    {
	strcat ( InfoText , _("Multiplicity: "));
	sprintf( TextChunk, "%d \n" , 
		 (int)ShowItem->multiplicity );
	strcat ( InfoText , TextChunk );
    }
    
    strcat ( InfoText , _("Duration: "));
    if ( ShowItem->max_duration >= 0 )
	sprintf( TextChunk, "%d / %d\n" , 
		 (int)ShowItem->current_duration, 
		 ShowItem->max_duration );
    else
	sprintf( TextChunk, _("Indestructible\n"));
    strcat ( InfoText , TextChunk );
    
    if ( ! ItemMap [ ShowItem->type ] . item_can_be_applied_in_combat )
	{
	strcat ( InfoText , _("Attributes required: "));
    
	if ( ( ItemMap [ ShowItem->type ] . item_require_strength == (-1) ) &&
	 ( ItemMap [ ShowItem->type ] . item_require_dexterity == (-1) ) &&
	 ( ItemMap [ ShowItem->type ] . item_require_magic == (-1) ) )
	    {
	    strcat ( InfoText , _("NONE\n"));
	    }
	else
	    {
	    if ( ItemMap [ ShowItem->type ] . item_require_strength > 0 )
		{
	        sprintf( TextChunk, _("Str: %d "), ItemMap [ ShowItem->type ] . item_require_strength ) ;
	        strcat ( InfoText , TextChunk );
		}
	    if ( ItemMap [ ShowItem->type ] . item_require_dexterity > 0 )
		{
	        sprintf( TextChunk, _("Dex: %d "), ItemMap [ ShowItem->type ] . item_require_dexterity ) ;
	        strcat ( InfoText , TextChunk );
		}
            if ( ItemMap [ ShowItem->type ] . item_require_magic > 0 )
		{
	        sprintf( TextChunk, _("Mag: %d "), ItemMap [ ShowItem->type ] . item_require_magic ) ;
                strcat ( InfoText , TextChunk );
         	}
	    strcat ( InfoText , "\n" );
	    }
	}
    else {
	/*    switch ( ShowItem -> type )
	        {
	            case ITEM_SPELLBOOK_OF_HEALING:
	            case ITEM_SPELLBOOK_OF_EXPLOSION_CIRCLE:
	            case ITEM_SPELLBOOK_OF_EXPLOSION_RAY:
	            case ITEM_SPELLBOOK_OF_TELEPORT_HOME:
        	    case ITEM_SPELLBOOK_OF_IDENTIFY:
	            case ITEM_SPELLBOOK_OF_PLASMA_BOLT:
	            case ITEM_SPELLBOOK_OF_ICE_BOLT:
	            case ITEM_SPELLBOOK_OF_POISON_BOLT:
	            case ITEM_SPELLBOOK_OF_PETRIFICATION:
	            case ITEM_SPELLBOOK_OF_RADIAL_EMP_WAVE:
	            case ITEM_SPELLBOOK_OF_RADIAL_VMX_WAVE:
	            case ITEM_SPELLBOOK_OF_RADIAL_PLASMA_WAVE:

	                sprintf( TextChunk , "Spellcasting skill: %s\n " ,
	                         _(AllSkillTexts [ required_spellcasting_skill_for_item ( ShowItem -> type ) ]));
	                strcat( InfoText , TextChunk );
	                sprintf( TextChunk , "Magic: %d\n " ,
	                         required_magic_stat_for_next_level_and_item ( ShowItem -> type ) );
	                strcat( InfoText , TextChunk );
	                break;
                default:
                	break;
        	}*/
	}
    //--------------------
    // Now we give some pricing information, the base list price for the item,
    // the repair price and the sell value
    if ( calculate_item_buy_price ( ShowItem ) )
	{
	sprintf( TextChunk, _("Base list price: %ld\n"), 
	     calculate_item_buy_price ( ShowItem ) ) ;
        strcat ( InfoText , TextChunk );
        sprintf( TextChunk, _("Sell value: %ld\n"), 
	     calculate_item_sell_price ( ShowItem ) ) ;
        strcat ( InfoText , TextChunk );
        if ( ShowItem->current_duration == ShowItem->max_duration ||
	     ShowItem->max_duration == ( -1 ) )
        	repairPrice = 0;
	else
	        repairPrice = calculate_item_repair_price ( ShowItem );
        sprintf( TextChunk, _("Repair cost: %ld\n"), repairPrice );
        strcat ( InfoText , TextChunk );
	}
    else
	{
	sprintf( TextChunk, _("Unsellable\n"));
        strcat ( InfoText , TextChunk );
	}
  
    //--------------------
    // If the item is a weapon, then we print out some weapon stats...
    //
    if ( ItemMap [ ShowItem->type ] . base_item_gun_damage + ItemMap [ ShowItem->type ] . item_gun_damage_modifier > 0 )
    {
	sprintf( TextChunk, _("Damage: %d - %d\n"), 
		 ItemMap [ ShowItem->type ] . base_item_gun_damage,
		 ItemMap [ ShowItem->type ] . base_item_gun_damage + 
		 ItemMap [ ShowItem->type ] . item_gun_damage_modifier );
	strcat ( InfoText , TextChunk );
    }
    
    if ( ItemMap [ ShowItem->type ] . item_gun_recharging_time > 0 )
    {
	sprintf( TextChunk, _("Recharge time: %3.2f\n"), 
		 ItemMap [ ShowItem->type ] . item_gun_recharging_time );
	strcat ( InfoText , TextChunk );
    }

    if ( ItemMap [ ShowItem->type ] . item_gun_reloading_time > 0 )
    {
	sprintf( TextChunk, _("Time to reload ammo clip: %3.2f\n"), 
		 ItemMap [ ShowItem->type ] . item_gun_reloading_time );
	strcat ( InfoText , TextChunk );
    }
    
    if ( ShowItem->ac_bonus > 0 )
    {
	sprintf ( TextChunk, _("Defense bonus: %d\n"), ShowItem->ac_bonus ) ;
	strcat ( InfoText , TextChunk );
    }
    
    sprintf ( TextChunk, _("Notes: %s"), 
	      ItemMap [ ShowItem->type ] . item_description );
    strcat ( InfoText, TextChunk );
    
    switch ( ItemMap [ ShowItem->type ] . item_gun_use_ammunition )
    {
	case ITEM_PLASMA_AMMUNITION:
	    strcat ( InfoText, _(" This weapon requires standard plasma ammunition."));
	    break;
	case ITEM_LASER_AMMUNITION:
	    strcat ( InfoText, _(" This weapon requires standard laser crystal ammunition."));
	    break;
	case ITEM_EXTERMINATOR_AMMUNITION:
	    strcat ( InfoText, _(" This weapon requires standard exterminator ammunition spheres."));
	    break;
	default:
	    break;
    }
    
    // SetCurrentFont( Para_BFont );
    // SetCurrentFont( Menu_BFont );
    SetCurrentFont( FPS_Display_BFont );
    DisplayText (InfoText, Cons_Text_Rect.x, Cons_Text_Rect.y + Displacement , &Cons_Text_Rect , TEXT_STRETCH );
    
    if ( ShowArrows ) 
    {
	ShowGenericButtonFromList ( UP_BUTTON );
	ShowGenericButtonFromList ( DOWN_BUTTON );
    }
    
}; // void ShowItemInfo ( ... )

/* ----------------------------------------------------------------------
 * This function fills the whole User_Rect with color 0 = black.
 * ---------------------------------------------------------------------- */
void
ClearUserFenster (void)
{
  SDL_Rect tmp;
  
  Copy_Rect (User_Rect, tmp)

  our_SDL_fill_rect_wrapper( Screen , &tmp, 0 );

}; // void ClearUserFenster( void )

#undef _ship_c
