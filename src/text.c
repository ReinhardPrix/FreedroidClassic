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
 * This file contains all functions dealing with texts on the screen,
 * that have to be blitted somehow, using bitmaps or OpenGL texturers,
 * but at least strongly rely on graphics concepts, not pure internal
 * text-processing.
 * ---------------------------------------------------------------------- */

#define _text_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "SDL_rotozoom.h"

int DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* ScrollBackground , float text_stretch );

int CharsPerLine;		// line length in chars:  obsolete

// curent text insertion position
int MyCursorX;
int MyCursorY;

// buffer for text environment
int StoreCursorX;
int StoreCursorY;

unsigned int StoreTextBG;
unsigned int StoreTextFG;

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
show_backgrounded_label_at_pixel_position ( char* LabelText , float fill_status , int pos_x , int pos_y )
{
    SDL_Rect background_rect;
    
    background_rect . x = pos_x ;
    background_rect . y = pos_y ;
    background_rect . w = 200 ;
    background_rect . h =  20 ;
    
    our_SDL_fill_rect_wrapper ( Screen , & ( background_rect ) , 0 );
    
    PutString ( Screen , pos_x , pos_y , LabelText );
    
}; // void show_backgrounded_label_at_pixel_position ( char* LabelText , float fill_status , int pos_x , int pos_y )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
show_backgrounded_label_at_map_position ( char* LabelText , float fill_status , float pos_x , float pos_y , int zoom_is_on )
{
    int pixel_x, pixel_y;
    
    if ( zoom_is_on )
    {
	pixel_x = translate_map_point_to_zoomed_screen_pixel ( pos_x , pos_y , TRUE );
	pixel_y = translate_map_point_to_zoomed_screen_pixel ( pos_x , pos_y , FALSE );
    }
    else
    {
	pixel_x = translate_map_point_to_screen_pixel ( pos_x , pos_y , TRUE );
	pixel_y = translate_map_point_to_screen_pixel ( pos_x , pos_y , FALSE );
    }
    show_backgrounded_label_at_pixel_position ( LabelText , fill_status , pixel_x , pixel_y );
    
}; // void show_backgrounded_label_at_map_position ( char* LabelText , float fill_status , float pos_x , float pos_y )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
show_backgrounded_text_rectangle ( char* text , int x , int y , int w , int h )
{
    SDL_Rect TargetRect;

    TargetRect . w = w ; 
    TargetRect . h = h ; 
    TargetRect . x = x ; 
    TargetRect . y = y ; 
    our_SDL_fill_rect_wrapper ( Screen , &TargetRect , 
				SDL_MapRGB ( Screen->format, 0 , 0 , 0 ) ) ;
    
#define IN_WINDOW_TEXT_OFFSET 15
    TargetRect . w -= IN_WINDOW_TEXT_OFFSET;
    TargetRect . h -= IN_WINDOW_TEXT_OFFSET;
    TargetRect . x += IN_WINDOW_TEXT_OFFSET;
    TargetRect . y += IN_WINDOW_TEXT_OFFSET;
    
    SetCurrentFont ( FPS_Display_BFont );
    DisplayText ( text , TargetRect . x, TargetRect . y , &TargetRect , 1.0 )  ;
    
}; // void show_backgrounded_text_rectangle ( char* text , int x , int y , int w , int h )

/* ----------------------------------------------------------------------
 * In some cases it will be nescessary to inform the user of something in
 * a big important style.  Then a popup window is suitable, with a mouse
 * button to confirm and make it go away again.
 * ---------------------------------------------------------------------- */
void
GiveMouseAlertWindow( char* WindowText )
{

    Activate_Conservative_Frame_Computation();
  
    show_backgrounded_text_rectangle ( WindowText , 
				       ( 640 - 440 ) / 2 , 
				       ( 480 - 340 ) / 2 , 
				       440 , 340 );

    our_SDL_flip_wrapper ( Screen );
    
    while ( !EnterPressed()  && !SpacePressed() ) ;
    while (  EnterPressed()  ||  SpacePressed() ) ;
    
}; // void GiveMouseAlertWindow( char* WindowText )

/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void 
CutDownStringToMaximalSize ( char* StringToCut , int LengthInPixels )
{
    int StringIndex=0;
    int LengthByNow=0;
    int i;
    
    if ( TextWidth ( StringToCut ) <= LengthInPixels ) return;
    
    while ( StringToCut[ StringIndex ] != 0 )
    {
	LengthByNow += CharWidth ( GetCurrentFont() , StringToCut [ StringIndex ] ) ;
	if ( LengthByNow >= LengthInPixels )
	{
	    for ( i = 0 ; i < 3 ; i ++ )
	    {
		if ( StringToCut [ StringIndex + i ] != 0 )
		{
		    StringToCut [ StringIndex + i ] = '.';
		}
		else
		    return;
	    }
	    StringToCut [ StringIndex + 3 ] = 0 ;
	    return;
	}
	StringIndex ++;
    }
    
}; // void CutDownStringToMaximalSize ( char* StringToCut , int LengthInPixels )

/* ----------------------------------------------------------------------
 * This function assigns a text comment to say for an enemy right after
 * is has been hit.  This can be turned off via a switch in GameConfig.
 * ---------------------------------------------------------------------- */
void 
EnemyHitByBulletText( int Enum )
{
    Enemy ThisRobot=&AllEnemys[ Enum ];

    if ( !GameConfig.Enemy_Hit_Text ) return;
    
    ThisRobot->TextVisibleTime=0;
    if ( !ThisRobot->is_friendly )
	switch (MyRandom(4))
	{
	    case 0:
		ThisRobot->TextToBeDisplayed="Unhandled exception fault.  Press ok to reboot.";
		break;
	    case 1:
		ThisRobot->TextToBeDisplayed="System fault. Please buy a newer version.";
		break;
	    case 2:
		ThisRobot->TextToBeDisplayed="System error. Might be a virus.";
		break;
	    case 3:
		ThisRobot->TextToBeDisplayed="System error. Pleae buy an upgrade from MS.";
		break;
	    case 4:
		ThisRobot->TextToBeDisplayed="System error. Press any key to reboot.";
		break;
	}
    else
	ThisRobot->TextToBeDisplayed="Aargh, I got hit.  Ugh, I got a bad feeling...";
}; // void EnemyHitByBullet( int Enum );

/* ----------------------------------------------------------------------
 * This function assigns a text comment to say for an enemy right after
 * it has bumped into the player.  This can be turned off via a switch in GameConfig.
 * ---------------------------------------------------------------------- */
void 
EnemyInfluCollisionText ( int Enum )
{
    Enemy ThisRobot=&AllEnemys[ Enum ];
    
    if ( !GameConfig.Enemy_Bump_Text ) return;
    
    ThisRobot->TextVisibleTime=0;
    
    if ( ThisRobot->is_friendly )
    {
	ThisRobot->TextToBeDisplayed="Ah, good, that we have an open collision avoiding standard, isn't it.";
    }
    else
    {
	switch ( MyRandom ( 1 ) )
	{
	    case 0:
		ThisRobot->TextToBeDisplayed="Hey, I'm from MS! Walk outa my way!";
		break;
	    case 1:
		ThisRobot->TextToBeDisplayed="Hey, I know the big MS boss! You better go.";
		break;
	}
    }
    
}; // void EnemyInfluCollisionText ( int Enum )

/* ----------------------------------------------------------------------
 * This function assigns a text comment to say for an enemy while it is
 * standing and aiming for player.  This can be turned off via a switch in GameConfig.
 * ---------------------------------------------------------------------- */
void 
AddStandingAndAimingText ( int Enum )
{
    Enemy ThisRobot=&AllEnemys[ Enum ];
    
    if ( !GameConfig.Enemy_Aim_Text ) return;
    
    ThisRobot->TextVisibleTime=0;
    
    if ( ( fabsf ( Me [ 0 ] . speed . x ) < 1 ) && ( fabsf ( Me [ 0 ] . speed . y ) < 1 ) )
    {
	ThisRobot->TextToBeDisplayed="Yeah, stay like that, haha.";
    }
    else
    {
	ThisRobot->TextToBeDisplayed="Stand still while I aim at you.";
    }
    
}; // void AddStandingAndAimingText( int Enum )

/* ----------------------------------------------------------------------
 * This function assigns a text comment to say for the influ right after
 * it has ran into an explosion.  This can be turned off via a switch in GameConfig.
 * ---------------------------------------------------------------------- */
void
AddInfluBurntText( void )
{
    int FinalTextNr;
    
    if ( !GameConfig.Influencer_Blast_Text ) return;
    
    Me[0].TextVisibleTime=0;
    
    FinalTextNr=MyRandom ( 6 );
    switch ( FinalTextNr )
    {
	case 0:
	    Me[0].TextToBeDisplayed="Aaarrgh, aah, that burnt me!";
	    break;
	case 1:
	    Me[0].TextToBeDisplayed="Hell, that blast was hot!";
	    break;
	case 2:
	    Me[0].TextToBeDisplayed="Ghaart, I hate to stain my chassis like that.";
	    break;
	case 3:
	    Me[0].TextToBeDisplayed="Oh no!  I think I've burnt a cable!";
	    break;
	case 4:
	    Me[0].TextToBeDisplayed="Oh no, my poor transfer connectors smolder!";
	    break;
	case 5:
	    Me[0].TextToBeDisplayed="I hope that didn't melt any circuits!";
	    break;
	case 6:
	    Me[0].TextToBeDisplayed="So that gives some more black scars on me ol' dented chassis!";
	    break;
	default:
	    printf("\nError in AddInfluBurntText! That shouldn't be happening.");
	    Terminate(ERR);
	    break;
    }
}; // void AddInfluBurntText( void )

/* ----------------------------------------------------------------------
 * This function sets the text cursor used in DisplayText.
 * ---------------------------------------------------------------------- */
void
SetTextCursor ( int x , int y )
{
    MyCursorX = x; 
    MyCursorY = y; 
    
    return;
}; // void SetTextCursor ( int x , int y )

/* -----------------------------------------------------------------
 *
 * This function scrolls a given text down inside the User-window, 
 * defined by the global SDL_Rect User_Rect
 *
 * startx/y give the Start-position, 
 * EndLine is the last line (?)
 *
 * ----------------------------------------------------------------- */
int
ScrollText (char *Text, int startx, int starty, int EndLine , int background_code )
{
    int Number_Of_Line_Feeds = 0;	// number of lines used for the text
    char *textpt;			// mobile pointer to the text
    int InsertLine = starty;
    int speed = +4;
    int maxspeed = 8;
    
    Activate_Conservative_Frame_Computation( );
    
    if ( background_code != ( -1 ) )
	blit_special_background ( background_code );
    
    SetCurrentFont( Para_BFont );
    
    // count the number of lines in the text
    textpt = Text;
    while (*textpt++)
	if (*textpt == '\n')
	    Number_Of_Line_Feeds++;
    
    while ( !SpacePressed () 
	    || ( MouseCursorIsOnButton ( SCROLL_TEXT_UP_BUTTON , GetMousePos_x()  , 
					 GetMousePos_y()  ) )
	    || ( MouseCursorIsOnButton ( SCROLL_TEXT_DOWN_BUTTON , GetMousePos_x()  , 
					 GetMousePos_y()  ) ) )
    {
	if ( UpPressed () 
	     || ( SpacePressed () 
		  && ( MouseCursorIsOnButton ( SCROLL_TEXT_UP_BUTTON , GetMousePos_x()  , 
					       GetMousePos_y()  ) ) ) )
	{
	    speed--;
	    if (speed < -maxspeed)
		speed = -maxspeed;
	}
	if ( DownPressed ()
	     || ( SpacePressed () 
		  && ( MouseCursorIsOnButton ( SCROLL_TEXT_DOWN_BUTTON , GetMousePos_x()  , 
					       GetMousePos_y()  ) ) ) )
	{
	    speed++;
	    if (speed > maxspeed)
		speed = maxspeed;
	}
	
	if ( MouseWheelDownPressed() )
	{
	    speed++;
	    if (speed > maxspeed)
		speed = maxspeed;
	}
	if ( MouseWheelUpPressed() )
	{
	    speed--;
	    if (speed < -maxspeed)
		speed = -maxspeed;
	}
	
	SDL_Delay (30);
	
	if ( background_code != ( -1 ) )
	    blit_special_background ( background_code );
	
	if ( ! DisplayText ( Text , startx , InsertLine , &User_Rect , TEXT_STRETCH ) )
	{
	    // JP: I've disabled this, since with this enabled we won't even
	    // see a single line of the first section of the briefing.
	    // But this leads to that we currently NEVER can see the second 
	    // or third part of the briefing text, cause it will not start
	    // the new text part when the lower end of the first text part
	    // is reached.  I don't consider this bug release-critical.
	    //
	    // break;  /* Text has been scrolled outside User_Rect */
	}
	
	InsertLine -= speed;
	
	//--------------------
	// We might add some buttons to be displayed here, so that, if you don't have
	// a mouse wheel and don't know about cursor keys, you can still click on these
	// buttons to control the scrolling speed of the text.
	//
	ShowGenericButtonFromList ( SCROLL_TEXT_UP_BUTTON );
	ShowGenericButtonFromList ( SCROLL_TEXT_DOWN_BUTTON );
	
	our_SDL_flip_wrapper (Screen);
	
	//--------------------
	// impose some limit on the amount to scroll away downwards
	//
	if (InsertLine > GameConfig . screen_height - 10 && (speed < 0))
	{
	    InsertLine = GameConfig . screen_height - 10;
	    speed = 0;
	}
	
    } // while !Space_Pressed 
    
    while ( SpacePressed() ); // so that we don't touch again immediately.
    
    return OK;
    
}; // int ScrollText ( ... )

/*-----------------------------------------------------------------
 * This function is much like DisplayText, but with the main difference,
 * that in case of the whole clipping window filled, the function will
 * display a ---more--- line, wait for a key and then scroll the text
 * further up.
 *
 * @Desc: prints *Text beginning at positions startx/starty, 
 * 
 *	and respecting the text-borders set by clip_rect
 *      -> this includes clipping but also automatic line-breaks
 *      when end-of-line is reached
 * 
 *      if clip_rect==NULL, no clipping is performed
 *      
 *      NOTE: the previous clip-rectange is restored before
 *            the function returns!
 *
 *      NOTE2: this function _does not_ update the screen
 *
 * @Ret: TRUE if some characters where written inside the clip rectangle
 *       FALSE if not (used by ScrollText to know if Text has been scrolled
 *             out of clip-rect completely)
 *-----------------------------------------------------------------*/
int
DisplayTextWithScrolling ( char *Text , int startx , int starty , const SDL_Rect *clip , SDL_Surface* Background , float text_stretch )
{
    char *tmp;	// mobile pointer to the current position as the text is drawn
    // SDL_Rect Temp_Clipping_Rect; // adding this to prevent segfault in case of NULL as parameter
    
    SDL_Rect store_clip;
    
    if ( startx != -1 ) MyCursorX = startx;		
    if ( starty != -1 ) MyCursorY = starty;
    
    SDL_GetClipRect (Screen, &store_clip);  // store previous clip-rect 
    if (clip)
	SDL_SetClipRect (Screen, clip);
    else
    {
	clip = & User_Rect;
    }
    
    
    tmp = Text;			// running text-pointer 
    
    while ( *tmp && (MyCursorY < clip->y + clip->h) )
    {
	if ( *tmp == '\n' )
	{
	    MyCursorX = clip->x;
	    MyCursorY += FontHeight ( GetCurrentFont() ) * TEXT_STRETCH;
	}
	else
	    DisplayChar (*tmp);
	tmp++;
	
	//--------------------
	// Here we plant in the question for ---more--- and a key to be pressed,
	// before we clean the screen and restart displaying text from the top
	// of the given Clipping rectangle
	//
	// if ( ( clip->h + clip->y - MyCursorY ) <= 2 * FontHeight ( GetCurrentFont() ) * TEXT_STRETCH )
	if ( ( clip->h + clip->y - MyCursorY ) <= 1 * FontHeight ( GetCurrentFont() ) * TEXT_STRETCH )
	{
	    DisplayText( "--- more --- more --- \n" , MyCursorX , MyCursorY , clip , 1.0 );
	    our_SDL_flip_wrapper( Screen );
	    while ( !SpacePressed() );
	    while (  SpacePressed() );
	    our_SDL_blit_surface_wrapper( Background , NULL , Screen , NULL );
	    MyCursorY = clip->y;
	    our_SDL_flip_wrapper( Screen );
	};
	
	if ( clip )
	    ImprovedCheckLineBreak ( tmp , clip , text_stretch );   // dont write over right border 
	
    } 
    
    SDL_SetClipRect (Screen, &store_clip); // restore previous clip-rect 
    
    //--------------------
    // ScrollText() wants to know if we still wrote something inside the
    // clip-rectangle, of if the Text has been scrolled out
    // 
    if ( clip && ((MyCursorY < clip->y) || (starty > clip->y + clip->h) ))
	return FALSE;  // no text was written inside clip 
    else
	return TRUE; 
    
};  // DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* Background )

/* ----------------------------------------------------------------------
 * This function sets a new text, that will be displayed in huge font 
 * directly over the combat window for a fixed duration of time, where
 * only the time in midst of combat and with no other windows opened
 * is counted.
 * ---------------------------------------------------------------------- */
void
SetNewBigScreenMessage( char* ScreenMessageText )
{
    int i;

    //--------------------
    // Instead of inserting the next big screen message at some fixed cursor position
    // we will now just move the other big screen messages down and insert the new
    // big screen message at the very top position of the row.
    //
    for ( i = 1 ; i < MAX_BIG_SCREEN_MESSAGES ; i ++ )
    {
	strcpy ( Me [ 0 ] . BigScreenMessage [ MAX_BIG_SCREEN_MESSAGES - i ] ,
		 Me [ 0 ] . BigScreenMessage [ MAX_BIG_SCREEN_MESSAGES - i - 1 ] );
	Me [ 0 ] . BigScreenMessageDuration [ MAX_BIG_SCREEN_MESSAGES - i ] = 
	    Me [ 0 ] . BigScreenMessageDuration [ MAX_BIG_SCREEN_MESSAGES - i - 1 ] ;
    }
    strcpy ( Me [ 0 ] . BigScreenMessage [ 0 ] , ScreenMessageText );
    Me [ 0 ] . BigScreenMessageDuration [ 0 ] = 0 ;

}; // void SetNewBigScreenMessage( char* ScreenMessageText )

/* ----------------------------------------------------------------------
 * This function displays the currently defined Bigscreenmessage on the
 * screen.  It will be called by AssembleCombatWindow.
 * ---------------------------------------------------------------------- */
void
DisplayBigScreenMessage( void )
{
    int i;
    int next_screen_message_position = 30 ;
    
    for ( i = 0 ; i < GameConfig . number_of_big_screen_messages ; i ++ )
    {
	if ( Me [ 0 ] . BigScreenMessageDuration [ i ] < GameConfig . delay_for_big_screen_messages )
	{
	    SDL_SetClipRect ( Screen , NULL );
	    CenteredPutStringFont ( Screen , Menu_BFont , next_screen_message_position , 
				    Me [ 0 ] . BigScreenMessage [ i ]  );
	    if ( !GameConfig.Inventory_Visible &&
		 !GameConfig.SkillScreen_Visible &&
		 !GameConfig.CharacterScreen_Visible )
		Me [ 0 ] . BigScreenMessageDuration [ i ]  += Frame_Time();

	    next_screen_message_position += FontHeight ( Menu_BFont ) ;
	}
    }
    
}; // void DisplayBigScreenMessage( void )

/*-----------------------------------------------------------------
 * This function prints *Text beginning at positions startx/starty,
 * respecting the text-borders set by clip_rect.  This includes 
 * clipping but also automatic line-breaks when end-of-line is 
 * reached.  If clip_rect==NULL, no clipping is performed.
 *      
 *      NOTE: the previous clip-rectange is restored before
 *            the function returns!
 *
 *     NOTE2: this function _does not_ update the screen
 *
 * @Ret: TRUE  if some characters where written inside the clip rectangle
 *       FALSE if not (used by ScrollText to know if Text has been scrolled
 *             out of clip-rect completely)
 *-----------------------------------------------------------------*/
int
DisplayText ( const char *Text, int startx, int starty, const SDL_Rect *clip , float text_stretch )
{
    char *tmp;	// mobile pointer to the current position in the string to be printed
    SDL_Rect Temp_Clipping_Rect; // adding this to prevent segfault in case of NULL as parameter
    SDL_Rect store_clip;
    
    //--------------------
    // We position the internal text cursor on the right spot for
    // the first character to be printed.
    //
    if ( startx != -1 ) MyCursorX = startx;		
    if ( starty != -1 ) MyCursorY = starty;
    
    //--------------------
    // We make a backup of the current clipping rect, so we can respore
    // it later.
    //
    SDL_GetClipRect (Screen, &store_clip);
    
    //--------------------
    // If we did receive some clipping rect in the parameter list (like e.g. it's
    // always the case with dialog output) we enforce this new clipping rect, otherwise
    // we just set the clipping rect to contain the whole screen.
    //
    if ( clip != NULL )
    {
	SDL_SetClipRect ( Screen , clip );
    }
    else
    {
	clip = & Temp_Clipping_Rect;
	Temp_Clipping_Rect.x=0;
	Temp_Clipping_Rect.y=0;
	Temp_Clipping_Rect.w=GameConfig . screen_width;
	Temp_Clipping_Rect.h=GameConfig . screen_height;
    }
    
    //--------------------
    // Now we can start to print the actual text to the screen.
    //
    // The running text pointer must be initialized.
    //
    tmp = (char*) Text;  // this is no longer a 'const' char*, but only a char*
    
    while ( *tmp && ( MyCursorY < clip -> y + clip -> h ) )
    {
	if ( *tmp == '\n' )
	{
	    MyCursorX = clip->x;
	    MyCursorY += FontHeight ( GetCurrentFont() ) * text_stretch ;
	}
	else
	    DisplayChar (*tmp);
	
	tmp++;
	
	if (clip)
	    ImprovedCheckLineBreak( tmp , clip , text_stretch );   // dont write over right border 
	
    }
    
    SDL_SetClipRect (Screen, &store_clip); // restore previous clip-rect 
    
    //--------------------
    // ScrollText() wants to know if we still wrote something inside the
    // clip-rectangle, of if the Text has been scrolled out
    //
    if ( clip && 
	 ( ( MyCursorY < clip -> y ) || ( starty > clip -> y + clip -> h ) ) )
	return FALSE;  // no text was written inside clip 
    else
	return TRUE; 
    
}; // int DisplayText(...)

/* -----------------------------------------------------------------
 * This function displays a char.  It uses Menu_BFont now
 * to do this.  MyCursorX is  updated to new position.
 * ----------------------------------------------------------------- */
void
DisplayChar (unsigned char c)
{

    if ( c == 1 ) 
    {
	SetCurrentFont ( Red_BFont );
	return;
    }
    else if ( c == 2 ) 
    {
	SetCurrentFont ( Blue_BFont );
	return;
    }
    else if ( c == 3 ) 
    {
	SetCurrentFont ( FPS_Display_BFont );
	return;
    }
    else if ( !isprint(c) ) // don't accept non-printable characters
    {
	fprintf ( stderr , "\nIllegal char passed to DisplayChar(): %d \n", c);
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was an illegal character passed to DisplayChar for printing.\n\
This indicates some error within Freedroid or within one of the dialog\n\
files of Freedroid.",
				   PLEASE_INFORM, IS_FATAL );
	return;
    }
    
    if ( ! display_char_disabled ) 
	PutChar ( Screen, MyCursorX, MyCursorY, c );
    
    // DebugPrintf( 0 , "%c" , c );
    
    // After the char has been displayed, we must move the cursor to its
    // new position.  That depends of course on the char displayed.
    //
    MyCursorX += CharWidth ( GetCurrentFont() , c);
    
}; // void DisplayChar(...)

/* ----------------------------------------------------------------------
 * This function checks if the next word still fits in this line
 * of text and initiates a carriage return/line feed if not.
 * Very handy and convenient, for that means it is no longer nescessary
 * to enter \n in the text every time its time for a newline. cool.
 *  
 * The function could perhaps still need a little improvement.  But for
 * now its good enough and improvement enough in comparison to the old
 * CheckLineBreak function.
 *
 * rp: added argument clip, which contains the text-window we're writing in
 *     (formerly known as "TextBorder")
 *
 * ---------------------------------------------------------------------- */
void
ImprovedCheckLineBreak (char* Resttext, const SDL_Rect *clip, float text_stretch )
{
    int i;
    int NeededSpace=0;
#define MAX_WORD_LENGTH 100
    
  // In case of a space, see if the next word will still fit on the line
  // and do a carriage return/line feed if not
    if ( *Resttext == ' ' ) {
	for ( i = 1 ; i < MAX_WORD_LENGTH ; i ++ ) 
	{
	    if ( ( Resttext [ i ] != ' ') && ( Resttext [ i ] != '\n') && ( Resttext [ i ] != 0 ) )
	    { 
		NeededSpace += CharWidth ( GetCurrentFont() , Resttext [ i ] );
		if ( MyCursorX + NeededSpace > clip->x + clip->w - 10 )
		{
		    MyCursorX = clip->x;
		    MyCursorY += FontHeight ( GetCurrentFont() ) * text_stretch ;
		    return;
		}
	    }
	    else 
		return;
	}
    }
}; // void ImprovedCheckLineBreak(void)

/* -----------------------------------------------------------------
 * This function reads a string of "MaxLen" from User-input, and 
 * echos it either to stdout or using graphics-text, depending on 
 * the parameter "echo":
 *                      echo=0    no echo
 *                      echo=1    print using printf
 *                      echo=2    print using graphics-text
 *
 * values of echo > 2 are ignored and treated like echo=0
 *
 * The function does have some extra complicating elelments coming 
 * from the potential slowness of machines with pure SDL and no
 * OpenGL.  These machines might not re-blit the whole background
 * fast enough for swiftly typed keystrokes, resulting in some
 * keypresses not being detected by the game.  To avoid that when
 * using SDL, not the whole screen will be re-blitted but only some
 * relevant parts, so that the CPU can save some time.  With OpenGL,
 * a complete re-blit is done in every cycle.
 *
 * NOTE: MaxLen is the maximal _strlen_ of the string (excl. \0 !)
 * 
 * @Ret: char *: String is allocated _here_!!!
 *       (but since it isn't such a huge amount of memory and since 
 *        it requires manual user input and therefore can't be done
 *        very often anyway, you can as well forget to free it...)
 * 
 * ----------------------------------------------------------------- */
char *
GetString ( int MaxLen, int echo , int background_code , const char* text_for_overhead_promt )
{
    char *input;		// pointer to the string entered by the user
    int key;          // last 'character' entered 
    int curpos;		// counts the characters entered so far
    int finished;
    int x0, y0, height;
    SDL_Rect store_rect, tmp_rect;
    SDL_Surface *store = NULL;
    
    if (echo == 1)		/* echo to stdout */
    {
	printf ("\nGetString(): sorry, echo=1 currently not implemented!\n");
	return NULL;
    }
    
    height = FontHeight (GetCurrentFont());
    
    DisplayText ( text_for_overhead_promt , 50 , 50 , NULL , TEXT_STRETCH );
    x0 = MyCursorX;
    y0 = MyCursorY;
    
    if ( use_open_gl )
	StoreMenuBackground ( 0 );
    else
    {
	store = SDL_CreateRGBSurface(0, GameConfig . screen_width, height, vid_bpp, 0, 0, 0, 0);
	Set_Rect ( store_rect , x0 , y0 , GameConfig . screen_width , height );
	our_SDL_blit_surface_wrapper (Screen, &store_rect, store, NULL);
    }
    
    // allocate memory for the users input
    input     = MyMalloc (MaxLen + 5);
    
    memset (input, '.', MaxLen);
    input[MaxLen] = 0;
    
    finished = FALSE;
    curpos = 0;
    
    while ( !finished  )
    {
	if ( use_open_gl )
	{
	    blit_special_background ( background_code );
	    DisplayText ( text_for_overhead_promt , 50 , 50 , NULL , TEXT_STRETCH );
	}
	else
	{
	    Copy_Rect( store_rect, tmp_rect);
	    our_SDL_blit_surface_wrapper (store, NULL, Screen, &tmp_rect);
	}
	
	x0 = MyCursorX;
	y0 = MyCursorY;
	
	PutString ( Screen, x0, y0, input);
	// PutString ( Screen, -1 , -1 , input);
	our_SDL_flip_wrapper (Screen);
	
	key = getchar_raw ();  
	
	if ( key == SDLK_RETURN ) 
	{
#ifdef __WIN32__
	    //--------------------
	    // On win32 machines, we need to perform an extra screen update,
	    // so that both buffers are in sync for future OpenGL operations
	    //
	    if ( use_open_gl )
	    {
		blit_special_background ( background_code );
		DisplayText ( text_for_overhead_promt , 50 , 50 , NULL , TEXT_STRETCH );
		x0 = MyCursorX;
		y0 = MyCursorY;
		PutString ( Screen, x0, y0, input);
		our_SDL_flip_wrapper (Screen);
	    }
#endif
	    input[curpos] = 0;
	    finished = TRUE;
	}
	else if ((key < SDLK_DELETE) && isprint (key) && (curpos < MaxLen) )  
	{
	    DebugPrintf (3, "isprint() true for keycode: %d ('%c')\n", key, (char)key);
	    /* printable characters are entered in string */
	    input[curpos] = (char) key;   
	    curpos ++;
	}
	else if (key == SDLK_BACKSPACE)
	{
	    if ( curpos > 0 ) curpos --;
	    input[curpos] = '.';
	}
	else if ( key == SDLK_ESCAPE )
	{
	    return ( NULL );
	}
	
    } // while(!finished) 
    
    DebugPrintf (2, "\n\nchar *GetString(..):  The final string is:\n");
    DebugPrintf (2,  input );
    DebugPrintf (2, "\n\n");
    
    return (input);
    
}; // char* GetString( ... ) 

/* -----------------------------------------------------------------
 * This function reads a string of "MaxLen" from User-input.
 *
 * NOTE: MaxLen is the maximal _strlen_ of the string (excl. \0 !)
 * 
 * ----------------------------------------------------------------- */
char *
GetEditableStringInPopupWindow ( int MaxLen , char* PopupWindowTitle , char* DefaultString )
{
    char *input;      // pointer to the string entered by the user
    int key;          // last 'character' entered 
    int curpos;	      // counts the characters entered so far
    int finished;
    int x0, y0;
    SDL_Rect TargetRect;
    SDL_Rect CursorRect;
    char tmp_char;
    int i;
    
#define EDIT_WINDOW_TEXT_OFFSET 15
    
    // if ( MaxLen > 60 ) MaxLen = 60;
    
    if ( ( ( int ) strlen ( DefaultString ) ) >= MaxLen -1 ) DefaultString [ MaxLen - 1 ] = 0 ;
    
    //--------------------
    // We prepare a new string, mostly empty...
    //
    input = MyMalloc (MaxLen + 5);
    // memset (input, '.', MaxLen);
    // input[MaxLen] = 0;
    // input [ 0 ] = 0 ;
    strcpy ( input , DefaultString );
    curpos = strlen ( input ) ;
    
    //--------------------
    // Now we prepare a rectangle for our popup window...
    //
    TargetRect . w = 440 ; 
    TargetRect . h = 340 ; 
    TargetRect . x = ( 640 - TargetRect . w ) / 2 ; 
    TargetRect . y = ( 480 - TargetRect . h ) / 2 ; 
    TargetRect . w -= EDIT_WINDOW_TEXT_OFFSET;
    TargetRect . h -= EDIT_WINDOW_TEXT_OFFSET;
    TargetRect . x += EDIT_WINDOW_TEXT_OFFSET;
    TargetRect . y += EDIT_WINDOW_TEXT_OFFSET;
    
    //--------------------
    // Now we find the right position for the new string to start by writing
    // out the title text once, just to get the cursor positioned right...
    //
    DisplayText ( PopupWindowTitle, TargetRect . x, TargetRect . y , &TargetRect , TEXT_STRETCH )  ;
    x0 = MyCursorX;
    y0 = MyCursorY;
    
    //--------------------
    // Now we can start the enter-string cycle...
    //
    finished = FALSE;
    while ( !finished  )
    {
	
	TargetRect . w = 440 ; 
	TargetRect . h = 340 ; 
	TargetRect . x = ( 640 - TargetRect . w ) / 2 ; 
	TargetRect . y = ( 480 - TargetRect . h ) / 2 ; 
	our_SDL_fill_rect_wrapper ( Screen , &TargetRect , 
				    SDL_MapRGB ( Screen->format, 0 , 0 , 0 ) ) ;
	
	TargetRect . w -= EDIT_WINDOW_TEXT_OFFSET;
	TargetRect . h -= EDIT_WINDOW_TEXT_OFFSET;
	TargetRect . x += EDIT_WINDOW_TEXT_OFFSET;
	TargetRect . y += EDIT_WINDOW_TEXT_OFFSET;
	
	SetCurrentFont ( FPS_Display_BFont );
	
	DisplayText ( PopupWindowTitle, TargetRect . x, TargetRect . y , &TargetRect , TEXT_STRETCH )  ;
	

	// PutString (Screen, x0, y0, input);
	TargetRect . y = x0 ;
	TargetRect . y = y0 ;
	DisplayText ( input , TargetRect . x, TargetRect . y , &TargetRect , TEXT_STRETCH )  ; 
	
	//--------------------
	// We position the cursor right on its real location
	//
	tmp_char = input [ curpos ] ;
	input [ curpos ] = 0 ;
	CursorRect . x = x0 + TextWidth( input ) ;
	input [ curpos ] = tmp_char ;
	CursorRect . y = y0 ;
	CursorRect . h = FontHeight ( GetCurrentFont() );
	CursorRect . w = 8 ;
	HighlightRectangle ( Screen , CursorRect );
	
	our_SDL_flip_wrapper (Screen);
	
	key = getchar_raw ();  
	
	if (key == SDLK_RETURN) 
	{
	    // input[curpos] = 0;
	    finished = TRUE;
	}
	else if (isprint (key) && (curpos < MaxLen) )  
	{
	    //--------------------
	    // If a printable character has been entered, it is either appended to
	    // the end of the current input string or the rest of the string is being
	    // moved and the new character inserted at the end.
	    //
	    if ( curpos == ( ( int ) strlen ( input ) ) )
	    {
		input[curpos] = (char) key;   
		curpos ++;
		input[curpos] = 0 ;
	    }
	    else
	    {
		if ( ( ( int ) strlen ( input ) ) == MaxLen - 1 ) input [ MaxLen - 2 ] = 0 ;
		for ( i = strlen ( input ) ; i >= curpos ; i -- )
		{
		    input [ i+1 ] = input [ i ] ;
		}
		input[curpos] = (char) key;   
		curpos ++;
	    }
	}
	else if ( key == SDLK_LEFT )
	{
	    if ( curpos > 0 ) curpos --;
	    // input[curpos] = '.';
	}
	else if ( key == SDLK_RIGHT )
	{
	    if ( curpos < ( ( int ) strlen ( input ) ) ) curpos ++;
	    // input[curpos] = '.';
	}
	else if (key == SDLK_BACKSPACE)
	{
	    if ( curpos > 0 ) 
	    {
		i = curpos ; 
		while ( input [ i - 1 ] != 0 ) 
		{
		    input [ i-1 ] = input [ i ] ;
		    i++;
		}
		curpos --;
	    }
	}
	
    } // while ( ! finished ) 
    
    DebugPrintf ( 1, "\n%s():  The final string is: '%s'." , __FUNCTION__ , input );
    
    return ( input );
    
}; // char* GetEditableStringInPopupWindow ( int MaxLen , char* PopupWindowTitle )

/* -----------------------------------------------------------------
 * This function is similar to putchar(), but uses the SDL via the 
 * BFont-fct PutChar() instead.
 *
 * Is sets MyCursor[XY], and allows passing (-1,-1) as coords to indicate
 * using the current cursor position.
 * ----------------------------------------------------------------- */
int
putchar_SDL (SDL_Surface *Surface, int x, int y, int c)
{
    int ret;
    if (x == -1) x = MyCursorX;
    if (y == -1) y = MyCursorY;
    
    MyCursorX = x + CharWidth (GetCurrentFont(), c);
    MyCursorY = y;
    
    ret = PutChar (Surface, x, y, c);
    
    our_SDL_flip_wrapper (Surface);
    
    return (ret);
}; // int putchar_SDL (SDL_Surface *Surface, int x, int y, int c)


/* -----------------------------------------------------------------
 * behaves similarly as gl_printf() of svgalib, using the BFont
 * print function PrintString().
 *  
 *  sets current position of MyCursor[XY],  
 *     if last char is '\n': to same x, next line y
 *     to end of string otherwise
 *
 * Added functionality to PrintString() is: 
 *  o) passing -1 as coord uses previous x and next-line y for printing
 *  o) Screen is updated immediatly after print, using SDL_flip()                       
 *
 * ----------------------------------------------------------------- */
void
printf_SDL (SDL_Surface *screen, int x, int y, const char *fmt, ...)
{
    va_list args;
    int i;
    
    char *tmp;
    va_start (args, fmt);
    
    if (x == -1) x = MyCursorX;
    else MyCursorX = x;
    
    if (y == -1) y = MyCursorY;
    else MyCursorY = y;
    
    tmp = (char *) MyMalloc (10000 + 1);
    vsprintf ( tmp , fmt , args );
    PutString (screen, x, y, tmp);
    
    // our_SDL_flip_wrapper (screen);
    
    if (tmp[strlen(tmp)-1] == '\n')
    {
	MyCursorX = x;
	MyCursorY = y+ 1.1* (GetCurrentFont()->h);
    }
    else
    {
	for ( i = 0 ; i < ( ( int ) strlen ( tmp ) ) ; i ++ )
	    MyCursorX += CharWidth ( GetCurrentFont ( ) , tmp [ i ] );
	MyCursorY = y;
    }

    free (tmp);
    va_end (args);

}; // void printf_SDL (SDL_Surface *screen, int x, int y, char *fmt, ...)


#undef _text_c

