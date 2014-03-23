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
 * Desc: functions for keyboard and joystick handling
 *
 *----------------------------------------------------------------------*/

#define _input_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

// earlier SDL versions didn't define these...
#ifndef SDL_BUTTON_WHEELUP
#define SDL_BUTTON_WHEELUP 4
#endif
#ifndef SDL_BUTTON_WHEELDOWN
#define SDL_BUTTON_WHEELDOWN 5
#endif

bool show_cursor;    // show mouse-cursor or not?
#define CURSOR_KEEP_VISIBLE  3000   // ticks to keep mouse-cursor visible without mouse-input

int WheelUpEvents=0;    // count number of not read-out wheel events
int WheelDownEvents=0;
Uint32 last_mouse_event = 0;  // record when last mouse event took place (SDL ticks)

SDLMod current_modifiers;

SDL_Event event;

int input_state[INPUT_LAST];	// array of states (pressed/released) of all keys

int key_cmds[CMD_LAST][3] =  // array of mappings {key1,key2,key3 -> cmd}
  {
#ifdef GCW0
    {SDLK_UP, 	  JOY_UP, 	0 },		// CMD_UP
    {SDLK_DOWN,	  JOY_DOWN, 	0 },		// CMD_DOWN
    {SDLK_LEFT,   JOY_LEFT, 	0 },		// CMD_LEFT
    {SDLK_RIGHT,  JOY_RIGHT, 	0 },		// CMD_RIGHT
    {SDLK_SPACE,  SDLK_LCTRL,  0 },// CMD_FIRE
    {SDLK_LALT, SDLK_LSHIFT, 	0 }, 		// CMD_ACTIVATE
    {SDLK_BACKSPACE,  SDLK_TAB,  0 },// CMD_TAKEOVER
    {0, 	  0, 		0  }, 	// CMD_QUIT,
    {SDLK_RETURN, 0, 0  }, 			// CMD_PAUSE,
    {0, 0, 0 }  		// CMD_SCREENSHOT
#else
    {SDLK_UP, 	  JOY_UP, 	'w' },		// CMD_UP
    {SDLK_DOWN,	  JOY_DOWN, 	's' },		// CMD_DOWN
    {SDLK_LEFT,   JOY_LEFT, 	'a' },		// CMD_LEFT
    {SDLK_RIGHT,  JOY_RIGHT, 	'd' },		// CMD_RIGHT
    {SDLK_SPACE,  JOY_BUTTON1,   MOUSE_BUTTON1 },// CMD_FIRE
    {SDLK_RETURN, SDLK_RSHIFT, 	'e' }, 		// CMD_ACTIVATE
    {SDLK_SPACE,  JOY_BUTTON2,   MOUSE_BUTTON2 },// CMD_TAKEOVER
    {'q', 	  'q', 		 'q'  }, 	// CMD_QUIT,
    {SDLK_PAUSE, 'p', 'p'  }, 			// CMD_PAUSE,
    {SDLK_F12, SDLK_F12, SDLK_F12 }  		// CMD_SCREENSHOT
#endif
  };

char *keystr[INPUT_LAST];

char *cmd_strings[CMD_LAST] =
  {
    "UP",
    "DOWN",
    "LEFT",
    "RIGHT",
    "FIRE",
    "ACTIVATE",
    "TAKEOVER",
    "QUIT",
    "PAUSE",
    "SCREENSHOT"
  };

#define FRESH_BIT   	(0x01<<8)
#define OLD_BIT		(0x01<<9)
#define LONG_PRESSED	(TRUE|OLD_BIT)
#define PRESSED		(TRUE|FRESH_BIT)
#define RELEASED	(FALSE|FRESH_BIT)

#define is_down(x) ((x) & (~FRESH_BIT) )
#define just_pressed(x) ( (x) & PRESSED == PRESSED)

#define clear_fresh(x) do { (x) &= ~FRESH_BIT; } while(0)


void
init_keystr (void)
{
  keystr[0]             = "NONE"; // Empty bind will otherwise crash on some platforms - also, we choose "NONE" as a placeholder...
#ifdef GCW0 // The GCW0 may change to joystick input altogether in the future - which will make these ifdefs unnecessary, I hope...
  keystr[SDLK_BACKSPACE] = "RSldr";
  keystr[SDLK_TAB]	= "LSldr";
  keystr[SDLK_RETURN]	= "Start";
  keystr[SDLK_SPACE]	= "Y";
  keystr[SDLK_ESCAPE]	= "Select";
#else
  keystr[SDLK_BACKSPACE] = "BS";
  keystr[SDLK_TAB]	= "Tab";
  keystr[SDLK_RETURN]	= "Return";
  keystr[SDLK_SPACE]	= "Space";
  keystr[SDLK_ESCAPE]	= "Esc";
#endif
  keystr[SDLK_CLEAR]	= "Clear";
  keystr[SDLK_PAUSE]	= "Pause";
  keystr[SDLK_EXCLAIM]	= "!";
  keystr[SDLK_QUOTEDBL]	= "\"";
  keystr[SDLK_HASH]	= "#";
  keystr[SDLK_DOLLAR]	= "$";
  keystr[SDLK_AMPERSAND]= "&";
  keystr[SDLK_QUOTE]	= "'";
  keystr[SDLK_LEFTPAREN]= "(";
  keystr[SDLK_RIGHTPAREN]=")";
  keystr[SDLK_ASTERISK]	= "*";
  keystr[SDLK_PLUS]	= "+";
  keystr[SDLK_COMMA]	= ",";
  keystr[SDLK_MINUS]	= "-";
  keystr[SDLK_PERIOD]	= ".";
  keystr[SDLK_SLASH]	= "/";
  keystr[SDLK_0]	= "0";
  keystr[SDLK_1]	= "1";
  keystr[SDLK_2]	= "2";
  keystr[SDLK_3]	= "3";
  keystr[SDLK_4]	= "4";
  keystr[SDLK_5]	= "5";
  keystr[SDLK_6]	= "6";
  keystr[SDLK_7]	= "7";
  keystr[SDLK_8]	= "8";
  keystr[SDLK_9]	= "9";
  keystr[SDLK_COLON]	= ":";
  keystr[SDLK_SEMICOLON]= ";";
  keystr[SDLK_LESS]	= "<";
  keystr[SDLK_EQUALS]	= "=";
  keystr[SDLK_GREATER]	= ">";
  keystr[SDLK_QUESTION]	= "?";
  keystr[SDLK_AT]	= "@";
  keystr[SDLK_LEFTBRACKET]	= "[";
  keystr[SDLK_BACKSLASH]	= "\\";
  keystr[SDLK_RIGHTBRACKET]	= "]";
  keystr[SDLK_CARET]	= "^";
  keystr[SDLK_UNDERSCORE]	= "_";
  keystr[SDLK_BACKQUOTE]	= "`";
  keystr[SDLK_a]	= "a";
  keystr[SDLK_b]	= "b";
  keystr[SDLK_c]	= "c";
  keystr[SDLK_d]	= "d";
  keystr[SDLK_e]	= "e";
  keystr[SDLK_f]	= "f";
  keystr[SDLK_g]	= "g";
  keystr[SDLK_h]	= "h";
  keystr[SDLK_i]	= "i";
  keystr[SDLK_j]	= "j";
  keystr[SDLK_k]	= "k";
  keystr[SDLK_l]	= "l";
  keystr[SDLK_m]	= "m";
  keystr[SDLK_n]	= "n";
  keystr[SDLK_o]	= "o";
  keystr[SDLK_p]	= "p";
  keystr[SDLK_q]	= "q";
  keystr[SDLK_r]	= "r";
  keystr[SDLK_s]	= "s";
  keystr[SDLK_t]	= "t";
  keystr[SDLK_u]	= "u";
  keystr[SDLK_v]	= "v";
  keystr[SDLK_w]	= "w";
  keystr[SDLK_x]	= "x";
  keystr[SDLK_y]	= "y";
  keystr[SDLK_z]	= "z";
  keystr[SDLK_DELETE]	= "Del";


  /* Numeric keypad */
  keystr[SDLK_KP0]	= "Num[0]";
  keystr[SDLK_KP1]	= "Num[1]";
  keystr[SDLK_KP2]	= "Num[2]";
  keystr[SDLK_KP3]	= "Num[3]";
  keystr[SDLK_KP4]	= "Num[4]";
  keystr[SDLK_KP5]	= "Num[5]";
  keystr[SDLK_KP6]	= "Num[6]";
  keystr[SDLK_KP7]	= "Num[7]";
  keystr[SDLK_KP8]	= "Num[8]";
  keystr[SDLK_KP9]	= "Num[9]";
  keystr[SDLK_KP_PERIOD]= "Num[.]";
  keystr[SDLK_KP_DIVIDE]= "Num[/]";
  keystr[SDLK_KP_MULTIPLY]= "Num[*]";
  keystr[SDLK_KP_MINUS]	= "Num[-]";
  keystr[SDLK_KP_PLUS]	= "Num[+]";
  keystr[SDLK_KP_ENTER]	= "Num[Enter]";
  keystr[SDLK_KP_EQUALS]= "Num[=]";

  /* Arrows + Home/End pad */
  keystr[SDLK_UP]	= "Up";
  keystr[SDLK_DOWN]	= "Down";
  keystr[SDLK_RIGHT]	= "Right";
  keystr[SDLK_LEFT]	= "Left";
  keystr[SDLK_INSERT]	= "Insert";
  keystr[SDLK_HOME]	= "Home";
  keystr[SDLK_END]	= "End";
  keystr[SDLK_PAGEUP]	= "PageUp";
  keystr[SDLK_PAGEDOWN]	= "PageDown";

  /* Function keys */
  keystr[SDLK_F1]	= "F1";
  keystr[SDLK_F2]	= "F2";
  keystr[SDLK_F3]	= "F3";
  keystr[SDLK_F4]	= "F4";
  keystr[SDLK_F5]	= "F5";
  keystr[SDLK_F6]	= "F6";
  keystr[SDLK_F7]	= "F7";
  keystr[SDLK_F8]	= "F8";
  keystr[SDLK_F9]	= "F9";
  keystr[SDLK_F10]	= "F10";
  keystr[SDLK_F11]	= "F11";
  keystr[SDLK_F12]	= "F12";
  keystr[SDLK_F13]	= "F13";
  keystr[SDLK_F14]	= "F14";
  keystr[SDLK_F15]	= "F15";

  /* Key state modifier keys */
  keystr[SDLK_NUMLOCK]	= "NumLock";
  keystr[SDLK_CAPSLOCK]	= "CapsLock";
  keystr[SDLK_SCROLLOCK]= "ScrlLock";
#ifdef GCW0
  keystr[SDLK_LSHIFT]	= "X";
  keystr[SDLK_LCTRL]	= "A";
  keystr[SDLK_LALT]	= "B";
#else
  keystr[SDLK_LSHIFT]	= "LShift";
  keystr[SDLK_LCTRL]	= "LCtrl";
  keystr[SDLK_LALT]	= "LAlt";
#endif
  keystr[SDLK_RSHIFT]	= "RShift";
  keystr[SDLK_RCTRL]	= "RCtrl";
  keystr[SDLK_RALT]	= "RAlt";
  keystr[SDLK_RMETA]	= "RMeta";
  keystr[SDLK_LMETA]	= "LMeta";
  keystr[SDLK_LSUPER]	= "LSuper";
  keystr[SDLK_RSUPER]	= "RSuper";
  keystr[SDLK_MODE]	= "Mode";
  keystr[SDLK_COMPOSE]	= "Compose";

  /* Miscellaneous function keys */
  keystr[SDLK_HELP]	= "Help";
  keystr[SDLK_PRINT]	= "Print";
  keystr[SDLK_SYSREQ]	= "SysReq";
  keystr[SDLK_BREAK]	= "Break";
  keystr[SDLK_MENU]	= "Menu";
  keystr[SDLK_POWER]	= "Power";
  keystr[SDLK_EURO]	= "Euro";
  keystr[SDLK_UNDO]	= "Undo";

  /* Mouse und Joy buttons */
  keystr[MOUSE_BUTTON1]	= "Mouse1";
  keystr[MOUSE_BUTTON2] = "Mouse2";
  keystr[MOUSE_BUTTON3] = "Mouse3";
  keystr[MOUSE_WHEELUP] = "WheelUp";
  keystr[MOUSE_WHEELDOWN]="WheelDown";

  keystr[JOY_UP]	= "JoyUp";
  keystr[JOY_DOWN]	= "JoyDown";
  keystr[JOY_LEFT]	= "JoyLeft";
  keystr[JOY_RIGHT]	= "JoyRight";
  keystr[JOY_BUTTON1] 	= "Joy1";
  keystr[JOY_BUTTON2] 	= "Joy2";
  keystr[JOY_BUTTON3] 	= "Joy3";

  return;
} // init_keystr()


int sgn (int x)
{
  return (x ? ((x)/abs(x)) : 0);
}

void Init_Joy (void)
{
  int num_joy;

  if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) == -1)
    {
      fprintf(stderr, "Couldn't initialize SDL-Joystick: %s\n", SDL_GetError());
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

// FIXME: remove that obsolete stuff...
void
ReactToSpecialKeys(void)
{

  if ( cmd_is_activeR(CMD_QUIT) )
    QuitGameMenu();

  if ( cmd_is_activeR(CMD_PAUSE) )
    Pause ();

  if ( cmd_is_active (CMD_SCREENSHOT) )
    TakeScreenshot();

  // this stuff remains hardcoded to keys
  if ( KeyIsPressedR('c') && AltPressed() && CtrlPressed() && ShiftPressed() )
    Cheatmenu ();

  if ( EscapePressedR() )
    EscapeMenu ();


} // void ReactToSpecialKeys(void)

//----------------------------------------------------------------------
// main input-reading routine
//----------------------------------------------------------------------
int
update_input (void)
{
  Uint8 axis;

  // switch mouse-cursor visibility as a function of time of last activity
  if (SDL_GetTicks () - last_mouse_event > CURSOR_KEEP_VISIBLE)
    show_cursor = FALSE;
  else
    show_cursor = TRUE;

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
	  current_modifiers = event.key.keysym.mod;
	  input_state[event.key.keysym.sym] = PRESSED;
#ifdef GCW0
	  if ( input_axis.x || input_axis.y ) axis_is_active = TRUE; // 4 GCW-0 ; breaks cursor keys after axis has been active...
#endif
	  break;
	case SDL_KEYUP:
	  current_modifiers = event.key.keysym.mod;
	  input_state[event.key.keysym.sym] = RELEASED;
#ifdef GCW0
	  axis_is_active = FALSE;
#endif
	  break;

	case SDL_JOYAXISMOTION:
	  axis = event.jaxis.axis;
	  if (axis == 0 || ((joy_num_axes >= 5) && (axis == 3)) ) /* x-axis */
	    {
	      input_axis.x = event.jaxis.value;

	      // this is a bit tricky, because we want to allow direction keys
	      // to be soft-released. When mapping the joystick->keyboard, we
	      // therefore have to make sure that this mapping only occurs when
	      // and actual _change_ of the joystick-direction ('digital') occurs
	      // so that it behaves like "set"/"release"
	      if (joy_sensitivity*event.jaxis.value > 10000)   /* about half tilted */
		{
		  input_state[JOY_RIGHT] = PRESSED;
		  input_state[JOY_LEFT] = FALSE;
		}
	      else if (joy_sensitivity*event.jaxis.value < -10000)
		{
		  input_state[JOY_LEFT] = PRESSED;
		  input_state[JOY_RIGHT] = FALSE;
		}
	      else
		{
		  input_state[JOY_LEFT] = FALSE;
		  input_state[JOY_RIGHT] = FALSE;
		}
	    }
	  else if ((axis == 1) || ((joy_num_axes >=5) && (axis == 4))) /* y-axis */
	    {
	      input_axis.y = event.jaxis.value;

	      if (joy_sensitivity*event.jaxis.value > 10000)
		{
		  input_state[JOY_DOWN] = PRESSED;
		  input_state[JOY_UP] =  FALSE;
		}
	      else if (joy_sensitivity*event.jaxis.value < -10000)
		{
		  input_state[JOY_UP] = PRESSED;
		  input_state[JOY_DOWN]= FALSE;
		}
	      else
		{
		  input_state[JOY_UP] = FALSE;
		  input_state[JOY_DOWN] = FALSE;
		}
	    }

	  break;

	case SDL_JOYBUTTONDOWN:
	  // first button
	  if (event.jbutton.button == 0)
	    input_state[JOY_BUTTON1] = PRESSED;

	  // second button
	  else if (event.jbutton.button == 1)
	    input_state[JOY_BUTTON2] = PRESSED;

	  // and third button
	  else if (event.jbutton.button == 2)
	    input_state[JOY_BUTTON3] = PRESSED;

	  axis_is_active = TRUE;
	  break;

	case SDL_JOYBUTTONUP:
	  // first button
	  if (event.jbutton.button == 0)
	    input_state[JOY_BUTTON1] = FALSE;

	  // second button
	  else if (event.jbutton.button == 1)
	    input_state[JOY_BUTTON2] = FALSE;

	  // and third button
	  else if (event.jbutton.button == 2)
	    input_state[JOY_BUTTON3] = FALSE;

	  axis_is_active = FALSE;
	  break;

	case SDL_MOUSEMOTION:
	  input_axis.x = event.button.x - UserCenter_x + 16;
	  input_axis.y = event.button.y - UserCenter_y + 16;

	  last_mouse_event = SDL_GetTicks ();

	  break;

	  /* Mouse control */
	case SDL_MOUSEBUTTONDOWN:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    {
	      input_state[MOUSE_BUTTON1] = PRESSED;
	      axis_is_active = TRUE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    input_state[MOUSE_BUTTON2] = PRESSED;

	  if (event.button.button == SDL_BUTTON_MIDDLE)
	    input_state[MOUSE_BUTTON3] = PRESSED;

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
	      input_state[MOUSE_BUTTON1] = FALSE;
	      axis_is_active = FALSE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    input_state[MOUSE_BUTTON2] = FALSE;

	  if (event.button.button == SDL_BUTTON_MIDDLE)
	    input_state[MOUSE_BUTTON3] = FALSE;

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
  int Returnkey = 0;

  //  keyboard_update ();   /* treat all pending keyboard-events */

  while ( !Returnkey )
    {
      SDL_WaitEvent (&event);    /* wait for next event */

      switch (event.type)
	{
	case SDL_KEYDOWN:
	  /*
	   * here we use the fact that, I cite from SDL_keyboard.h:
	   * "The keyboard syms have been cleverly chosen to map to ASCII"
	   * ... I hope that this design feature is portable, and durable ;)
	   */
	  Returnkey = (int) event.key.keysym.sym;
	  if ( event.key.keysym.mod & KMOD_SHIFT )
	    Returnkey = toupper( (int)event.key.keysym.sym );
	  break;

	case SDL_JOYBUTTONDOWN:
	  if (event.jbutton.button == 0)
	    Returnkey = JOY_BUTTON1;
	  else if (event.jbutton.button == 1)
	    Returnkey = JOY_BUTTON2;
	  else if (event.jbutton.button == 2)
	    Returnkey = JOY_BUTTON3;
	  break;

	case SDL_MOUSEBUTTONDOWN:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    Returnkey = MOUSE_BUTTON1;
	  else if (event.button.button == SDL_BUTTON_RIGHT)
	    Returnkey = MOUSE_BUTTON2;
	  else if (event.button.button == SDL_BUTTON_MIDDLE)
	    Returnkey = MOUSE_BUTTON3;
	  else if (event.button.button == SDL_BUTTON_WHEELUP)
	    Returnkey = MOUSE_WHEELUP;
	  else if (event.button.button == SDL_BUTTON_WHEELDOWN)
	    Returnkey = MOUSE_WHEELDOWN;
	  break;

	default:
	  SDL_PushEvent (&event);  /* put this event back into the queue */
	  update_input ();  /* and treat it the usual way */
	  continue;
	}

    } /* while(1) */

  return ( Returnkey );

} /* getchar_raw() */

// forget the wheel-counters
void
ResetMouseWheel (void)
{
  WheelUpEvents = WheelDownEvents = 0;
  return;
}

bool
WheelUpPressed (void)
{
  update_input();
  if (WheelUpEvents)
    return (WheelUpEvents--);
  else
    return (FALSE);
}

bool
WheelDownPressed (void)
{
  update_input();
  if (WheelDownEvents)
    return (WheelDownEvents--);
  else
    return (FALSE);
}

bool
KeyIsPressed (SDLKey key)
{
  update_input();

  return( (input_state[key] & PRESSED) == PRESSED );
}


// does the same as KeyIsPressed, but automatically releases the key as well..
bool
KeyIsPressedR (SDLKey key)
{
  bool ret;

  ret = KeyIsPressed (key);

  ReleaseKey (key);
  return (ret);
}

void
ReleaseKey (SDLKey key)
{
  input_state[key] = FALSE;
  return;
}

bool
ModIsPressed (SDLMod mod)
{
  bool ret;
  update_input();
  ret = ( (current_modifiers & mod) != 0) ;
  return (ret);
}

bool
NoDirectionPressed (void)
{
  if ( (axis_is_active && (input_axis.x || input_axis.y)) ||
      DownPressed () || UpPressed() || LeftPressed() || RightPressed() )
    return ( FALSE );
  else
    return ( TRUE );
} // int NoDirectionPressed(void)


//----------------------------------------------------------------------
// check if a particular key has been pressed



// check if any keys or buttons1 are pressed
bool
any_key_pressed (void)
{
  int i;
  bool ret = FALSE;

  update_input();

  for (i=0; i<SDLK_LAST; i++)
    if ( just_pressed(input_state[i]) )
      {
	clear_fresh(input_state[i]);
	ret = TRUE;
	break;
      }
  if ( just_pressed(input_state[JOY_BUTTON1]) )
    {
      clear_fresh (input_state[JOY_BUTTON1]);
      ret = TRUE;
    }

  if ( just_pressed(input_state[MOUSE_BUTTON1]) )
    {
      ret = TRUE;
      clear_fresh (input_state[MOUSE_BUTTON1]);
    }

  return (ret);

}  // any_key_pressed()

bool
cmd_is_active (enum _cmds cmd)
{
  if (cmd >= CMD_LAST)
    {
      DebugPrintf (0, "ERROR: Illegal command '%d'\n", cmd);
      Terminate (ERR);
    }

  if ( KeyIsPressed( key_cmds[cmd][0] ) ||
       KeyIsPressed( key_cmds[cmd][1] ) ||
       KeyIsPressed( key_cmds[cmd][2] ))
    return (TRUE);
  else
    return (FALSE);

} // cmd_is_active()

// --------------------------------------------------
// the same but release the keys: use only for menus!
// --------------------------------------------------
bool
cmd_is_activeR (enum _cmds cmd)
{
  if (cmd >= CMD_LAST)
    {
      DebugPrintf (0, "ERROR: Illegal command '%d'\n", cmd);
      Terminate (ERR);
    }

  if ( KeyIsPressedR( key_cmds[cmd][0] ) ||
       KeyIsPressedR( key_cmds[cmd][1] ) ||
       KeyIsPressedR( key_cmds[cmd][2] ))
    return (TRUE);
  else
    return (FALSE);

} // cmd_is_active()

#undef _intput_c
