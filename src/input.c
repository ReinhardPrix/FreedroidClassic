/*----------------------------------------------------------------------
 *
 * Desc: functions for keyboard and joystick handling
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
#define _input_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#ifndef SDL_BUTTON_WHEELUP 
#define SDL_BUTTON_WHEELUP 4
#endif
#ifndef SDL_BUTTON_WHEELDOWN
#define SDL_BUTTON_WHEELDOWN 5
#endif

int WheelUpEvents=0;    // count number of not read-out wheel events
int WheelDownEvents=0;
Uint32 last_mouse_event = 0;  // record when last mouse event took place (SDL ticks)
int CurrentlyMouseRightPressed=0;

SDL_Event event;
int ShiftWasPressedInAddition=FALSE;
int CtrlWasPressedInAddition=FALSE;
int AltWasPressedInAddition=FALSE;
int CurrentlyEnterPressed=0;
int CurrentlySpacePressed=0;
int CurrentlyLeftPressed=0;
int CurrentlyRightPressed=0;
int CurrentlyUpPressed=0;
int CurrentlyDownPressed=0;
int Currently0Pressed=0;
int Currently1Pressed=0;
int Currently2Pressed=0;
int Currently3Pressed=0;
int Currently4Pressed=0;
int Currently5Pressed=0;
int Currently6Pressed=0;
int Currently7Pressed=0;
int Currently8Pressed=0;
int Currently9Pressed=0;
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
int CurrentlyKP_PLUS_Pressed=0;
int CurrentlyKP_MINUS_Pressed=0;
int CurrentlyKP_MULTIPLY_Pressed=0;
int CurrentlyKP_DIVIDE_Pressed=0;
int CurrentlyKP_ENTER_Pressed=0;
int CurrentlyKP0Pressed=0;
int CurrentlyKP1Pressed=0;
int CurrentlyKP2Pressed=0;
int CurrentlyKP3Pressed=0;
int CurrentlyKP4Pressed=0;
int CurrentlyKP5Pressed=0;
int CurrentlyKP6Pressed=0;
int CurrentlyKP7Pressed=0;
int CurrentlyKP8Pressed=0;
int CurrentlyKP9Pressed=0;
int CurrentlyF1Pressed=0;
int CurrentlyF2Pressed=0;
int CurrentlyF3Pressed=0;
int CurrentlyF4Pressed=0;
int CurrentlyF5Pressed=0;
int CurrentlyF6Pressed=0;
int CurrentlyF7Pressed=0;
int CurrentlyF8Pressed=0;
int CurrentlyF9Pressed=0;
int CurrentlyF10Pressed=0;
int CurrentlyF11Pressed=0;
int CurrentlyF12Pressed=0;
int CurrentlyEscapePressed=0;
int CurrentlyBackspacePressed=0;

int sgn (int x)
{
  return (x ? ((x)/abs(x)) : 0);
}

void Init_Joy (void)
{
  int num_joy;

  if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) == -1)
    {
      fprintf(stderr, "Couldn't initialize SDL-Joystick: %s\n",SDL_GetError());
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL Joystick initialisation successful.\n");


  DebugPrintf (1, " %d Joysticks found!\n", num_joy = SDL_NumJoysticks ());

  if (num_joy > 0)
    joy = SDL_JoystickOpen (0);

  if (joy)
    {
      DebugPrintf (1, "Identifier: %s\n", SDL_JoystickName (0));
      DebugPrintf (1, "Number of Axes: %d\n", joy_num_axes = SDL_JoystickNumAxes(joy));
      DebugPrintf (1, "Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));

      /* aktivate Joystick event handling */
      SDL_JoystickEventState (SDL_ENABLE); 

    }
  else 
    joy = NULL;  /* signals that no yoystick is present */


  return;
}


void 
ReactToSpecialKeys(void)
{
  if ( QPressed() ) /* user asked for quit */
    QuitGameMenu();

/*   if ( DPressed() ) */
/*     Me.energy = 0; */

  
  // To debug the Debriefing() I added a function to add or subtract
  // a thousand points of score via numerical keyboard functions.
  // Activate this if you want to test that.  
  
/*   if ( KP0Pressed() ) */
/*     { */
/*       while (KP0Pressed()); */
/*       RealScore-=1000; */
/*     } */
/*   if ( KP1Pressed() ) */
/*     { */
/*       while (KP1Pressed()); */
/*       RealScore+=1000; */
/*     } */
  
  
  if ( CPressed() && Alt_Was_Pressed()
       && Ctrl_Was_Pressed() && Shift_Was_Pressed() ) 
    Cheatmenu ();
  if ( EscapePressed() )
    EscapeMenu ();

  if ( PPressed () )
    Pause ();
  
  
} // void ReactToSpecialKeys(void)

int
Shift_Was_Pressed(void)
{
  return (ShiftWasPressedInAddition);  
}

int
Ctrl_Was_Pressed(void)
{
  return (CtrlWasPressedInAddition);  
}

int
Alt_Was_Pressed(void)
{
  return (AltWasPressedInAddition);  
}

int 
keyboard_update(void)
{
  Uint8 axis; 

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
	  // printf("\nSLD_KEYDOWN event detected...");
	  // fflush(stdout);

	  // Check for some additional modifiers and set flags accordingly
	  if ( event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
	    ShiftWasPressedInAddition=TRUE;
	  else ShiftWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL) )
	    CtrlWasPressedInAddition=TRUE;
	  else CtrlWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LALT | KMOD_RALT) )
	    AltWasPressedInAddition=TRUE;
	  else AltWasPressedInAddition=FALSE;

	  /* Check the SDLKey values */
	  switch( event.key.keysym.sym )
	    {
	    case SDLK_KP_PLUS:
	      CurrentlyKP_PLUS_Pressed=TRUE;
	      break;
	    case SDLK_KP_MINUS:
	      CurrentlyKP_MINUS_Pressed=TRUE;
	      break;
	    case SDLK_KP_MULTIPLY:
	      CurrentlyKP_MULTIPLY_Pressed=TRUE;
	      break;
	    case SDLK_KP_DIVIDE:
	      CurrentlyKP_DIVIDE_Pressed=TRUE;
	      break;
	    case SDLK_KP_ENTER:
	      CurrentlyKP_ENTER_Pressed=TRUE;
	      break;
	    case SDLK_0:
	      Currently0Pressed=TRUE;
	      break;
	    case SDLK_1:
	      Currently1Pressed=TRUE;
	      break;
	    case SDLK_2:
	      Currently2Pressed=TRUE;
	      break;
	    case SDLK_3:
	      Currently3Pressed=TRUE;
	      break;
	    case SDLK_4:
	      Currently4Pressed=TRUE;
	      break;
	    case SDLK_5:
	      Currently5Pressed=TRUE;
	      break;
	    case SDLK_6:
	      Currently6Pressed=TRUE;
	      break;
	    case SDLK_7:
	      Currently7Pressed=TRUE;
	      break;
	    case SDLK_8:
	      Currently8Pressed=TRUE;
	      break;
	    case SDLK_9:
	      Currently9Pressed=TRUE;
	      break;
	    case SDLK_KP0:
	      CurrentlyKP0Pressed=TRUE;
	      break;
	    case SDLK_KP1:
	      CurrentlyKP1Pressed=TRUE;
	      break;
	    case SDLK_KP2:
	      CurrentlyKP2Pressed=TRUE;
	      break;
	    case SDLK_KP3:
	      CurrentlyKP3Pressed=TRUE;
	      break;
	    case SDLK_KP4:
	      CurrentlyKP4Pressed=TRUE;
	      break;
	    case SDLK_KP5:
	      CurrentlyKP5Pressed=TRUE;
	      break;
	    case SDLK_KP6:
	      CurrentlyKP6Pressed=TRUE;
	      break;
	    case SDLK_KP7:
	      CurrentlyKP7Pressed=TRUE;
	      break;
	    case SDLK_KP8:
	      CurrentlyKP8Pressed=TRUE;
	      break;
	    case SDLK_KP9:
	      CurrentlyKP9Pressed=TRUE;
	      break;
	    case SDLK_F1:
	      CurrentlyF1Pressed=TRUE;
	      break;
	    case SDLK_F2:
	      CurrentlyF2Pressed=TRUE;
	      TakeScreenshot();
	      break;
	    case SDLK_F3:
	      CurrentlyF3Pressed=TRUE;
	      break;
	    case SDLK_F4:
	      CurrentlyF4Pressed=TRUE;
	      break;
	    case SDLK_F5:
	      CurrentlyF5Pressed=TRUE;
	      break;
	    case SDLK_F6:
	      CurrentlyF6Pressed=TRUE;
	      break;
	    case SDLK_F7:
	      CurrentlyF7Pressed=TRUE;
	      break;
	    case SDLK_F8:
	      CurrentlyF8Pressed=TRUE;
	      break;
	    case SDLK_F9:
	      CurrentlyF9Pressed=TRUE;
	      break;
	    case SDLK_F10:
	      CurrentlyF10Pressed=TRUE;
	      break;
	    case SDLK_F11:
	      CurrentlyF11Pressed=TRUE;
	      break;
	    case SDLK_F12:
	      CurrentlyF12Pressed=TRUE;
	      break;
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

	  // printf("\nSLD_KEYUP event detected...");
	  // fflush(stdout);

	  // Check for some additional modifiers and set flags accordingly
	  if ( event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
	    ShiftWasPressedInAddition=TRUE;
	  else ShiftWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL) )
	    CtrlWasPressedInAddition=TRUE;
	  else CtrlWasPressedInAddition=FALSE;

	  if ( event.key.keysym.mod & (KMOD_LALT | KMOD_RALT) )
	    AltWasPressedInAddition=TRUE;
	  else AltWasPressedInAddition=FALSE;

	  switch( event.key.keysym.sym )
	    {
	    case SDLK_KP_PLUS:
	      CurrentlyKP_PLUS_Pressed=FALSE;
	      break;
	    case SDLK_KP_MINUS:
	      CurrentlyKP_MINUS_Pressed=FALSE;
	      break;
	    case SDLK_KP_MULTIPLY:
	      CurrentlyKP_MULTIPLY_Pressed=FALSE;
	      break;
	    case SDLK_KP_DIVIDE:
	      CurrentlyKP_DIVIDE_Pressed=FALSE;
	      break;
	    case SDLK_KP_ENTER:
	      CurrentlyKP_ENTER_Pressed=FALSE;
	      break;
	    case SDLK_0:
	      Currently0Pressed=FALSE;
	      break;
	    case SDLK_1:
	      Currently1Pressed=FALSE;
	      break;
	    case SDLK_2:
	      Currently2Pressed=FALSE;
	      break;
	    case SDLK_3:
	      Currently3Pressed=FALSE;
	      break;
	    case SDLK_4:
	      Currently4Pressed=FALSE;
	      break;
	    case SDLK_5:
	      Currently5Pressed=FALSE;
	      break;
	    case SDLK_6:
	      Currently6Pressed=FALSE;
	      break;
	    case SDLK_7:
	      Currently7Pressed=FALSE;
	      break;
	    case SDLK_8:
	      Currently8Pressed=FALSE;
	      break;
	    case SDLK_9:
	      Currently9Pressed=FALSE;
	      break;
	    case SDLK_KP0:
	      CurrentlyKP0Pressed=FALSE;
	      break;
	    case SDLK_KP1:
	      CurrentlyKP1Pressed=FALSE;
	      break;
	    case SDLK_KP2:
	      CurrentlyKP2Pressed=FALSE;
	      break;
	    case SDLK_KP3:
	      CurrentlyKP3Pressed=FALSE;
	      break;
	    case SDLK_KP4:
	      CurrentlyKP4Pressed=FALSE;
	      break;
	    case SDLK_KP5:
	      CurrentlyKP5Pressed=FALSE;
	      break;
	    case SDLK_KP6:
	      CurrentlyKP6Pressed=FALSE;
	      break;
	    case SDLK_KP7:
	      CurrentlyKP7Pressed=FALSE;
	      break;
	    case SDLK_KP8:
	      CurrentlyKP8Pressed=FALSE;
	      break;
	    case SDLK_KP9:
	      CurrentlyKP9Pressed=FALSE;
	      break;
	    case SDLK_F1:
	      CurrentlyF1Pressed=FALSE;
	      break;
	    case SDLK_F2:
	      CurrentlyF2Pressed=FALSE;
	      break;
	    case SDLK_F3:
	      CurrentlyF3Pressed=FALSE;
	      break;
	    case SDLK_F4:
	      CurrentlyF4Pressed=FALSE;
	      break;
	    case SDLK_F5:
	      CurrentlyF5Pressed=FALSE;
	      break;
	    case SDLK_F6:
	      CurrentlyF6Pressed=FALSE;
	      break;
	    case SDLK_F7:
	      CurrentlyF7Pressed=FALSE;
	      break;
	    case SDLK_F8:
	      CurrentlyF8Pressed=FALSE;
	      break;
	    case SDLK_F9:
	      CurrentlyF9Pressed=FALSE;
	      break;
	    case SDLK_F10:
	      CurrentlyF10Pressed=FALSE;
	      break;
	    case SDLK_F11:
	      CurrentlyF11Pressed=FALSE;
	      break;
	    case SDLK_F12:
	      CurrentlyF12Pressed=FALSE;
	      break;
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

	case SDL_JOYAXISMOTION:
	  axis = event.jaxis.axis;
	  if (axis == 0 || ((joy_num_axes >= 5) && (axis == 3)) ) /* x-axis */
	    {
	      input_axis.x = event.jaxis.value;

	      if (event.jaxis.value > joy_sensitivity*1000)   /* about half tilted */
		{
		  CurrentlyRightPressed = TRUE;
		  CurrentlyLeftPressed = FALSE;
		}
	      else if (event.jaxis.value <  -joy_sensitivity*1000)
		{
		  CurrentlyLeftPressed = TRUE;
		  CurrentlyRightPressed = FALSE;
		}
	      else
		{
		  CurrentlyLeftPressed = FALSE;
		  CurrentlyRightPressed= FALSE;
		}
	    }
	  else if ((axis == 1) || ((joy_num_axes >=5) && (axis == 4))) /* y-axis */
	    {
	      input_axis.y = event.jaxis.value;

	      if (event.jaxis.value > joy_sensitivity*1000)  
		{
		  CurrentlyDownPressed = TRUE;
		  CurrentlyUpPressed = FALSE;
		}
	      else if (event.jaxis.value < -joy_sensitivity*1000)
		{
		  CurrentlyUpPressed = TRUE;
		  CurrentlyDownPressed = FALSE;
		}
	      else
		{
		  CurrentlyUpPressed = FALSE;
		  CurrentlyDownPressed= FALSE;
		}
	    }
		
	  break;
	  
	case SDL_JOYBUTTONDOWN:
	  CurrentlySpacePressed = TRUE;
	  axis_is_active = TRUE;
	  break;

	case SDL_JOYBUTTONUP:
	  CurrentlySpacePressed = FALSE;
	  axis_is_active = FALSE;
	  break;

	case SDL_MOUSEMOTION:
	  if (mouse_control)
	    {
	      //
	      // Since the new mouse cursor does have it's tip at the top left corner
	      // of the mouse cursor, but rather in the center of the 32x32 pixel mouse
	      // cursor, we need to correct the given axis a little (16 pixels) bit.
	      //
	      input_axis.x = event.button.x - USER_FENSTER_CENTER_X + 16; 
	      input_axis.y = event.button.y - USER_FENSTER_CENTER_Y + 16; 	  
	      last_mouse_event = SDL_GetTicks ();
	    }
	  break;
	  
	  /* Mouse control */
	case SDL_MOUSEBUTTONDOWN:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    {
	      CurrentlySpacePressed = TRUE;
	      axis_is_active = TRUE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    CurrentlyMouseRightPressed = TRUE;

	  if (event.button.button == SDL_BUTTON_MIDDLE)  // we just map middle->Escape
	    CurrentlyEscapePressed = TRUE;

	  // wheel events are immediately released, so we rather
	  // count the number of not yet read-out events
	  if (event.button.button == SDL_BUTTON_WHEELUP)
	      WheelUpEvents ++;

	  if (event.button.button == SDL_BUTTON_WHEELDOWN)
	      WheelDownEvents ++;

	  last_mouse_event = SDL_GetTicks();
	  break;

        case SDL_MOUSEBUTTONUP:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    {
	      CurrentlySpacePressed = FALSE;
	      axis_is_active = FALSE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    CurrentlyMouseRightPressed = FALSE;

	  if (event.button.button == SDL_BUTTON_MIDDLE)  // we just map middle->Escape
	    CurrentlyEscapePressed = FALSE;

	  break;

 	default:
 	  break;
 	}

    }

  return 0;
}

/*-----------------------------------------------------------------
 * Desc: should do roughly what getchar() does, but in raw 
 * 	 (SLD) keyboard mode. 
 * 
 * Return: the (SDLKey) of the next key-pressed event cast to (int)
 *
 *-----------------------------------------------------------------*/
int
getchar_raw (void)
{
  SDL_Event event;
  int Returnkey;

  //  keyboard_update ();   /* treat all pending keyboard-events */

  while (1)
    {
      SDL_WaitEvent (&event);    /* wait for next event */
      
      if (event.type == SDL_KEYDOWN)
	{
	  /* 
	   * here we use the fact that, I cite from SDL_keyboard.h:
	   * "The keyboard syms have been cleverly chosen to map to ASCII"
	   * ... I hope that this design feature is portable, and durable ;)  
	   */
	  Returnkey = (int) event.key.keysym.sym;
	  if ( event.key.keysym.mod & KMOD_SHIFT ) Returnkey = toupper( (int)event.key.keysym.sym );
	  return ( Returnkey );
	}
      else
	{
	  SDL_PushEvent (&event);  /* put this event back into the queue */
	  keyboard_update ();  /* and treat it the usual way */
	  continue;
	}

    } /* while(1) */

} /* getchar_raw() */

// forget the wheel-counters
void
ResetMouseWheel (void)
{
  WheelUpEvents = WheelDownEvents = 0;
  return;
}

int
WheelUpPressed (void)
{
  keyboard_update();
  if (WheelUpEvents)
    return (WheelUpEvents--);
  else
    return (FALSE);
}

int
WheelDownPressed (void)
{
  keyboard_update();
  if (WheelDownEvents)
    return (WheelDownEvents--);
  else
    return (FALSE);
}

int 
KP_PLUS_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_PLUS_Pressed;
}

int 
KP_MULTIPLY_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_MULTIPLY_Pressed;
}

int 
KP_MINUS_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_MINUS_Pressed;
}

int 
KP_DIVIDE_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_DIVIDE_Pressed;
}

int 
KP_ENTER_Pressed (void)
{
  keyboard_update();
  return CurrentlyKP_ENTER_Pressed;
}

int
Number0Pressed (void)
{
  keyboard_update ();
  return Currently0Pressed;
}				// int KP0Pressed(void)

int
Number1Pressed (void)
{
  keyboard_update ();
  return Currently1Pressed;
}				// int KP1Pressed(void)

int
Number2Pressed (void)
{
  keyboard_update ();
  return Currently2Pressed;
}				// int KP2Pressed(void)

int
Number3Pressed (void)
{
  keyboard_update ();
  return Currently3Pressed;
}				// int KP3Pressed(void)

int
Number4Pressed (void)
{
  keyboard_update ();
  return Currently4Pressed;
}				// int KP4Pressed(void)

int
Number5Pressed (void)
{
  keyboard_update ();
  return Currently5Pressed;
}				// int KP5Pressed(void)

int
Number6Pressed (void)
{
  keyboard_update ();
  return Currently6Pressed;
}				// int KP6Pressed(void)

int
Number7Pressed (void)
{
  keyboard_update ();
  return Currently7Pressed;
}				// int KP7Pressed(void)

int
Number8Pressed (void)
{
  keyboard_update ();
  return Currently8Pressed;
}				// int KP8Pressed(void)

int
Number9Pressed (void)
{
  keyboard_update ();
  return Currently9Pressed;
}				// int KP9Pressed(void)

int
KP0Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP0Pressed;
}				// int KP0Pressed(void)

int
KP1Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP1Pressed;
}				// int KP1Pressed(void)

int
KP2Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP2Pressed;
}				// int KP2Pressed(void)

int
KP3Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP3Pressed;
}				// int KP3Pressed(void)

int
KP4Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP4Pressed;
}				// int KP4Pressed(void)

int
KP5Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP5Pressed;
}				// int KP5Pressed(void)

int
KP6Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP6Pressed;
}				// int KP6Pressed(void)

int
KP7Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP7Pressed;
}				// int KP7Pressed(void)

int
KP8Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP8Pressed;
}				// int KP8Pressed(void)

int
KP9Pressed (void)
{
  keyboard_update ();
  return CurrentlyKP9Pressed;
}				// int KP9Pressed(void)

int
F1Pressed (void)
{
  keyboard_update ();
  return CurrentlyF1Pressed;
}				// int F1Pressed(void)

int
F2Pressed (void)
{
  keyboard_update ();
  return CurrentlyF2Pressed;
}				// int F2Pressed(void)

int
F3Pressed (void)
{
  keyboard_update ();
  return CurrentlyF3Pressed;
}				// int F3Pressed(void)

int
F4Pressed (void)
{
  keyboard_update ();
  return CurrentlyF4Pressed;
}				// int F4Pressed(void)

int
F5Pressed (void)
{
  keyboard_update ();
  return CurrentlyF5Pressed;
}				// int F5Pressed(void)

int
F6Pressed (void)
{
  keyboard_update ();
  return CurrentlyF6Pressed;
}				// int F6Pressed(void)

int
F7Pressed (void)
{
  keyboard_update ();
  return CurrentlyF7Pressed;
}				// int F7Pressed(void)

int
F8Pressed (void)
{
  keyboard_update ();
  return CurrentlyF8Pressed;
}				// int F8Pressed(void)

int
F9Pressed (void)
{
  keyboard_update ();
  return CurrentlyF9Pressed;
}				// int F9Pressed(void)

int
F10Pressed (void)
{
  keyboard_update ();
  return CurrentlyF10Pressed;
}				// int F10Pressed(void)

int
F11Pressed (void)
{
  keyboard_update ();
  return CurrentlyF11Pressed;
}				// int F11Pressed(void)

int
F12Pressed (void)
{
  keyboard_update ();
  return CurrentlyF12Pressed;
}				// int F12Pressed(void)

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
  // hack hack: we map return-> space, so that it 
  // generally acts as "fire" in the game, should be convenient:
  return (CurrentlySpacePressed|CurrentlyEnterPressed);
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
  if ( (axis_is_active && (input_axis.x || input_axis.y)) ||
      DownPressed () || UpPressed() || LeftPressed() || RightPressed() )
    return ( FALSE );
  else
    return ( TRUE );
} // int NoDirectionPressed(void)


int
MouseRightPressed(void)
{
  keyboard_update();
  return CurrentlyMouseRightPressed;
}





#undef _intput_c
