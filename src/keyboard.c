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
 * Desc: functions to make keyboard access via svgalib somewhat easyer.
 *
 *----------------------------------------------------------------------*/
#include <config.h>

#define _keyboard_c

#undef DIAGONAL_KEYS_AUS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

SDL_Event event;
int CurrentlyEnterPressed=0;
int CurrentlySpacePressed=0;
int CurrentlyLeftPressed=0;
int CurrentlyRightPressed=0;
int CurrentlyUpPressed=0;
int CurrentlyDownPressed=0;
int CurrentlyAPressed=0;
int CurrentlyBPressed=0;
int CurrentlyCPressed=0;
int CurrentlyDPressed=0;
int CurrentlyEPressed=0;
int CurrentlyFPressed=0;
int CurrentlyGPressed=0;
int CurrentlyHPressed=0;
int CurrentlyIPressed=0;
int CurrentlyJPressed=0;
int CurrentlyKPressed=0;
int CurrentlyLPressed=0;
int CurrentlyMPressed=0;
int CurrentlyNPressed=0;
int CurrentlyOPressed=0;
int CurrentlyPPressed=0;
int CurrentlyQPressed=0;
int CurrentlyRPressed=0;
int CurrentlySPressed=0;
int CurrentlyTPressed=0;
int CurrentlyUPressed=0;
int CurrentlyVPressed=0;
int CurrentlyWPressed=0;
int CurrentlyXPressed=0;
int CurrentlyYPressed=0;
int CurrentlyZPressed=0;
int CurrentlyEscapePressed=0;
int CurrentlyBackspacePressed=0;

void 
Init_SDL_Keyboard(void)
{
  
}

int 
keyboard_update(void)
{
  static int Number_Of_Screenshot=0;
  char *Screenshoot_Filename;

  // Screenshoot_Filename=malloc(100);
  //="This is a dummy string, that should be sufficiently long ";

  while( SDL_PollEvent( &event ) )
    {
      switch( event.type )
	{
	case SDL_QUIT:
	  printf("\n\nUser requestet Termination...\n\nTerminating...");
	  Terminate(0);
	  break;
	  /* Look for a keypress */
	case SDL_KEYDOWN:
	  /* Check the SDLKey values */
	  switch( event.key.keysym.sym )
	    {
	    case SDLK_BACKSPACE:
	      CurrentlyBackspacePressed=TRUE;
	      break;
	    case SDLK_LEFT:
	      CurrentlyLeftPressed=TRUE;
	      break;
	    case SDLK_RIGHT:
	      CurrentlyRightPressed=TRUE;
	      break;
	    case SDLK_UP:
	      CurrentlyUpPressed=TRUE;
	      break;
	    case SDLK_DOWN:
	      CurrentlyDownPressed=TRUE;
	      break;
	    case SDLK_SPACE:
	      CurrentlySpacePressed=TRUE;
	      break;
	    case SDLK_RETURN:
	      CurrentlyEnterPressed=TRUE;
	      break;
	    case SDLK_a:
	      CurrentlyAPressed=TRUE;
	      break;
	    case SDLK_b:
	      CurrentlyBPressed=TRUE;
	      break;
	    case SDLK_c:
	      CurrentlyCPressed=TRUE;
	      break;
	    case SDLK_d:
	      CurrentlyDPressed=TRUE;
	      break;
	    case SDLK_e:
	      CurrentlyEPressed=TRUE;
	      break;
	    case SDLK_f:
	      CurrentlyFPressed=TRUE;
	      break;
	    case SDLK_g:
	      CurrentlyGPressed=TRUE;
	      break;
	    case SDLK_h:
	      CurrentlyHPressed=TRUE;
	      break;
	    case SDLK_i:
	      CurrentlyIPressed=TRUE;
	      break;
	    case SDLK_j:
	      CurrentlyJPressed=TRUE;
	      break;
	    case SDLK_k:
	      CurrentlyKPressed=TRUE;
	      break;
	    case SDLK_l:
	      CurrentlyLPressed=TRUE;
	      break;
	    case SDLK_m:
	      CurrentlyMPressed=TRUE;
	      break;
	    case SDLK_n:
	      CurrentlyNPressed=TRUE;
	      break;
	    case SDLK_o:
	      CurrentlyOPressed=TRUE;
	      break;
	    case SDLK_p:
	      CurrentlyPPressed=TRUE;

	      Screenshoot_Filename=malloc(100);
	      printf("\n\nScreenshoot function called.\n\n");
	      sprintf( Screenshoot_Filename , "Screenshot_%d.bmp", Number_Of_Screenshot );
	      printf("\n\nScreenshoot function: The Filename is: %s.\n\n" , Screenshoot_Filename );
	      SDL_SaveBMP( ScaledSurface , Screenshoot_Filename );
	      Number_Of_Screenshot++;
	      free(Screenshoot_Filename);

	      break;
	    case SDLK_q:
	      CurrentlyQPressed=TRUE;
	      break;
	    case SDLK_r:
	      CurrentlyRPressed=TRUE;
	      break;
	    case SDLK_s:
	      CurrentlySPressed=TRUE;
	      break;
	    case SDLK_t:
	      CurrentlyTPressed=TRUE;
	      break;
	    case SDLK_u:
	      CurrentlyUPressed=TRUE;
	      break;
	    case SDLK_v:
	      CurrentlyVPressed=TRUE;
	      break;
	    case SDLK_w:
	      CurrentlyWPressed=TRUE;
	      break;
	    case SDLK_x:
	      CurrentlyXPressed=TRUE;
	      break;
	    case SDLK_y:
	      CurrentlyYPressed=TRUE;
	      break;
	    case SDLK_z:
	      CurrentlyZPressed=TRUE;
	      break;
	    case SDLK_ESCAPE:
	      CurrentlyEscapePressed=TRUE;
	      break;
	    default:
	      /*
		printf("\n\nUnhandled keystroke!! Terminating...\n\n");
		Terminate(ERR);
	      */
	      break;
	    }
	  break;
	  /* We must also use the SDL_KEYUP events to zero the x */
	  /* and y velocity variables. But we must also be       */
	  /* careful not to zero the velocities when we shouldn't*/
	case SDL_KEYUP:
	  switch( event.key.keysym.sym )
	    {
	    case SDLK_BACKSPACE:
	      CurrentlyBackspacePressed=FALSE;
	      break;
	    case SDLK_LEFT:
	      CurrentlyLeftPressed=FALSE;
	      break;
	    case SDLK_RIGHT:
	      CurrentlyRightPressed=FALSE;
	      break;
	    case SDLK_UP:
	      CurrentlyUpPressed=FALSE;
	      break;
	    case SDLK_DOWN:
	      CurrentlyDownPressed=FALSE;
	      break;
	    case SDLK_SPACE:
	      CurrentlySpacePressed=FALSE;
	      break;
	    case SDLK_RETURN:
	      CurrentlyEnterPressed=FALSE;
	      break;
	    case SDLK_a:
	      CurrentlyAPressed=FALSE;
	      break;
	    case SDLK_b:
	      CurrentlyBPressed=FALSE;
	      break;
	    case SDLK_c:
	      CurrentlyCPressed=FALSE;
	      break;
	    case SDLK_d:
	      CurrentlyDPressed=FALSE;
	      break;
	    case SDLK_e:
	      CurrentlyEPressed=FALSE;
	      break;
	    case SDLK_f:
	      CurrentlyFPressed=FALSE;
	      break;
	    case SDLK_g:
	      CurrentlyGPressed=FALSE;
	      break;
	    case SDLK_h:
	      CurrentlyHPressed=FALSE;
	      break;
	    case SDLK_i:
	      CurrentlyIPressed=FALSE;
	      break;
	    case SDLK_j:
	      CurrentlyJPressed=FALSE;
	      break;
	    case SDLK_k:
	      CurrentlyKPressed=FALSE;
	      break;
	    case SDLK_l:
	      CurrentlyLPressed=FALSE;
	      break;
	    case SDLK_m:
	      CurrentlyMPressed=FALSE;
	      break;
	    case SDLK_n:
	      CurrentlyNPressed=FALSE;
	      break;
	    case SDLK_o:
	      CurrentlyOPressed=FALSE;
	      break;
	    case SDLK_p:
	      CurrentlyPPressed=FALSE;
	      break;
	    case SDLK_q:
	      CurrentlyQPressed=FALSE;
	      break;
	    case SDLK_r:
	      CurrentlyRPressed=FALSE;
	      break;
	    case SDLK_s:
	      CurrentlySPressed=FALSE;
	      break;
	    case SDLK_t:
	      CurrentlyTPressed=FALSE;
	      break;
	    case SDLK_u:
	      CurrentlyUPressed=FALSE;
	      break;
	    case SDLK_v:
	      CurrentlyVPressed=FALSE;
	      break;
	    case SDLK_w:
	      CurrentlyWPressed=FALSE;
	      break;
	    case SDLK_x:
	      CurrentlyXPressed=FALSE;
	      break;
	    case SDLK_y:
	      CurrentlyYPressed=FALSE;
	      break;
	    case SDLK_z:
	      CurrentlyZPressed=FALSE;
	      break;
	    case SDLK_ESCAPE:
	      CurrentlyEscapePressed=FALSE;
	      break;
	    default:
	      break;
	    }
	  break;
	  
	default:
	  break;
	}
    }
  return 0;
}

void
ClearKbState (void)
{
  keyboard_clearstate ();	// This resets the state of all keys when keyboard in raw mode
}				// void ClearKbState(void)

int
LeftPressed (void)
{
  keyboard_update ();
  return CurrentlyLeftPressed;
}				// int LeftPressed(void)

int
RightPressed (void)
{
  keyboard_update ();
  return CurrentlyRightPressed;
}				// int RightPressed(void)

int
UpPressed (void)
{
  keyboard_update ();
  return CurrentlyUpPressed;
}				// int UpPressed(void)

int
DownPressed (void)
{
  keyboard_update ();
  return CurrentlyDownPressed;
}				// int DownPressed(void)

int
SpacePressed (void)
{
  keyboard_update ();
  return CurrentlySpacePressed;
}				// int SpacePressed(void)

int
EnterPressed (void)
{
  keyboard_update ();
  return CurrentlyEnterPressed;
}				// int SpacePressed(void)

int
BackspacePressed (void)
{
  keyboard_update ();
  return CurrentlyBackspacePressed;
}				// int SpacePressed(void)

int
APressed (void)
{
  keyboard_update ();
  return CurrentlyAPressed;
}				// int PPressed(void)

int
BPressed (void)
{
  keyboard_update ();
  return CurrentlyBPressed;
}				// int PPressed(void)

int
EPressed (void)
{
  keyboard_update ();
  return CurrentlyEPressed;
}				// int PPressed(void)

int
FPressed (void)
{
  keyboard_update ();
  return CurrentlyFPressed;
}				// int PPressed(void)

int
GPressed (void)
{
  keyboard_update ();
  return CurrentlyGPressed;
}				// int PPressed(void)

int
HPressed (void)
{
  keyboard_update ();
  return CurrentlyHPressed;
}				// int PPressed(void)

int
JPressed (void)
{
  keyboard_update ();
  return CurrentlyJPressed;
}				// int PPressed(void)

int
KPressed (void)
{
  keyboard_update ();
  return CurrentlyKPressed;
}				// int PPressed(void)

int
MPressed (void)
{
  keyboard_update ();
  return CurrentlyMPressed;
}				// int PPressed(void)

int
NPressed (void)
{
  keyboard_update ();
  return CurrentlyNPressed;
}				// int PPressed(void)

int
OPressed (void)
{
  keyboard_update ();
  return CurrentlyOPressed;
}				// int PPressed(void)

int
PPressed (void)
{
  keyboard_update ();
  return CurrentlyPPressed;
}				// int PPressed(void)

int
RPressed (void)
{
  keyboard_update ();
  return CurrentlyRPressed;
}				// int PPressed(void)

int
SPressed (void)
{
  keyboard_update ();
  return CurrentlySPressed;
}				// int PPressed(void)

int
TPressed (void)
{
  keyboard_update ();
  return CurrentlyTPressed;
}				// int PPressed(void)

int
QPressed (void)
{
  keyboard_update ();
  return CurrentlyQPressed;
}				// int QPressed(void)

int
WPressed (void)
{
  keyboard_update ();
  return CurrentlyWPressed;
}				// int WPressed(void)

int
DPressed (void)
{
  keyboard_update ();
  return CurrentlyDPressed;
}				// int WPressed(void)

int
LPressed (void)
{
  keyboard_update ();
  return CurrentlyLPressed;
}				// int WPressed(void)

int
IPressed (void)
{
  keyboard_update ();
  return CurrentlyIPressed;
}				// int WPressed(void)

int
VPressed (void)
{
  keyboard_update ();
  return CurrentlyVPressed;
}				// int WPressed(void)

int
CPressed (void)
{
  keyboard_update ();
  return CurrentlyCPressed;
}				// int WPressed(void)

int
UPressed (void)
{
  keyboard_update ();
  return CurrentlyUPressed;
}				// int WPressed(void)

int
XPressed (void)
{
  keyboard_update ();
  return CurrentlyXPressed;
}				// int WPressed(void)

int
YPressed (void)
{
  keyboard_update ();
  return CurrentlyYPressed;
}				// int WPressed(void)

int
ZPressed (void)
{
  keyboard_update ();
  return CurrentlyZPressed;
}				// int WPressed(void)

int
EscapePressed (void)
{
  keyboard_update ();
  return CurrentlyEscapePressed;
}				// int WPressed(void)

/*@Function============================================================
  @Desc: Diese Funktion ermittelt, ob irgend eine Richtungstaste gedrueckt ist
  
  @Ret: wenn eine Richtungstaste gedrueckt ist FALSE
  ansonsten TRUE 
* $Function----------------------------------------------------------*/

int
NoDirectionPressed (void)
{
  if (DownPressed ())
    return (0);
  if (UpPressed ())
    return (0);
  if (LeftPressed ())
    return (0);
  if (RightPressed ())
    return (0);
  return (1);
}				// int NoDirectionPressed(void)

/* ************************************************************** *
 * * Diese Funktion setzt die Zeichenwiederholrate der Tastatur * *
 * * Es wird zuerst das Befehlswort 0x0F3 gesendet und dann die * *
 * * neue Rate (0,1 delay 2,3,4,5,6 typematic) nach 0x60 gesandt. *
 * ************************************************************** */

int
SetTypematicRate (unsigned char Rate)
{
  return 0;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
JoystickControl (void)
{
  //      SpacePressed=JoyB;
  //      if (JoyX < 50) LeftPressed=TRUE; else LeftPressed=FALSE;
  //      if (JoyX >200) RightPressed=TRUE; else RightPressed=FALSE;
  //      if (JoyY < 50) UpPressed=TRUE; else UpPressed=FALSE;
  //      if (JoyY >200) DownPressed=TRUE; else DownPressed=FALSE;
  //
}				// void JoystickControl(void)

/*@Function============================================================
@Desc:  This function drains the keyboard buffer of characters to
			prevent nasty beeps when it is overloaded

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
KillTastaturPuffer (void)
{
  //PORT: nix tun hier!
}


#undef _keyboard_c
