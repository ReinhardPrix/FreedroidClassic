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

#define JOY_MAX_VAL 32767     // maximal amplitude of joystick axis values
#ifndef ANDROID
#define JOY_DEAD_ZONE 10000   // joystick tilt ignored below this value
#else
#define JOY_DEAD_ZONE 0   // make android controls more reactive
#endif

bool show_cursor;    // show mouse-cursor or not?
#define CURSOR_KEEP_VISIBLE  3000   // ticks to keep mouse-cursor visible without mouse-input

int WheelUpEvents=0;    // count number of not read-out wheel events
int WheelDownEvents=0;
Uint32 last_mouse_event = 0;  // record when last mouse event took place (SDL ticks)

SDLMod current_modifiers;

#define MAX_TRACKED_INPUTS 1024

typedef struct
{
  int key;
  int state;
} tracked_input_t;

static tracked_input_t input_state[MAX_TRACKED_INPUTS];
static int input_state_count = 0;

typedef struct
{
  int key;
  const char *name;
} key_name_entry_t;

static key_name_entry_t key_names[128];
static int key_name_count = 0;

int key_cmds[CMD_LAST][3] =  // array of mappings {key1,key2,key3 -> cmd}
  {
#ifdef GCW0
    {SDLK_UP, 	  JOY_UP, 	0 },		// CMD_UP
    {SDLK_DOWN,	  JOY_DOWN, 	0 },		// CMD_DOWN
    {SDLK_LEFT,   JOY_LEFT, 	0 },		// CMD_LEFT
    {SDLK_RIGHT,  JOY_RIGHT, 	0 },		// CMD_RIGHT
    {SDLK_SPACE,  SDLK_LCTRL,  0 },             // CMD_FIRE
    {SDLK_LALT,   JOY_BUTTON2,	0 }, 		// CMD_ACTIVATE
    {SDLK_BACKSPACE,  SDLK_TAB,  0 },           // CMD_TAKEOVER
    {0, 	  0, 		0  }, 	        // CMD_QUIT,
    {SDLK_RETURN, 0, 0  }, 			// CMD_PAUSE,
    {0, 0, 0 },  		                // CMD_SCREENSHOT
    {0,           0,              0  },		// CMD_FULLSCREEN,
    {SDLK_ESCAPE, JOY_BUTTON4,    0 },	        // CMD_MENU,
    {SDLK_ESCAPE, JOY_BUTTON2,   MOUSE_BUTTON2 }// CMD_BACK
#else
    {SDLK_UP, 	  JOY_UP, 	'w' },		// CMD_UP
    {SDLK_DOWN,	  JOY_DOWN, 	's' },		// CMD_DOWN
    {SDLK_LEFT,   JOY_LEFT, 	'a' },		// CMD_LEFT
    {SDLK_RIGHT,  JOY_RIGHT, 	'd' },		// CMD_RIGHT
    {SDLK_SPACE,  JOY_BUTTON1,   MOUSE_BUTTON1 },// CMD_FIRE
    {SDLK_RETURN, SDLK_RSHIFT, 	'e' }, 		// CMD_ACTIVATE
    {SDLK_SPACE,  JOY_BUTTON2,   MOUSE_BUTTON2 },// CMD_TAKEOVER
    {'q', 	  0, 		 0  }, 	        // CMD_QUIT,
    {SDLK_PAUSE,  'p',           0  },	        // CMD_PAUSE,
    {SDLK_F12,    0,             0  }, 	        // CMD_SCREENSHOT
    {'f',         0,             0  },		// CMD_FULLSCREEN,
    {SDLK_ESCAPE, JOY_BUTTON4,   0  },		// CMD_MENU,
    {SDLK_ESCAPE, JOY_BUTTON2,   MOUSE_BUTTON2 }// CMD_BACK
#endif
  };

const char *cmd_strings[CMD_LAST] =
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
    "SCREENSHOT",
    "FULLSCREEN",
    "MENU",
    "BACK"
  };

#define FRESH_BIT   	(0x01<<8)
#define OLD_BIT		(0x01<<9)
#define LONG_PRESSED	(TRUE|OLD_BIT)
#define PRESSED		(TRUE|FRESH_BIT)
#define RELEASED	(FALSE|FRESH_BIT)

#define is_down(x) ((x) & (~FRESH_BIT) )
#define just_pressed(x) ( ((x) & PRESSED) == PRESSED)

#define clear_fresh(x) do { (x) &= ~FRESH_BIT; } while(0)

// local prototypes
int sgn (int x);
static int *get_input_state_ptr (int key, int create);
static void set_input_state (int key, int state);
static int get_input_state (int key);
static void set_key_name (int key, const char *name);

const char *
GetKeyString (int key)
{
  static char buf[32];
  int i;

  if (key == 0) return "NONE";

  for (i=0; i < key_name_count; i++)
    if (key_names[i].key == key)
      return key_names[i].name;

  if (key > 0)
    {
      const char *sdl_name = SDL_GetKeyName((SDL_Keycode)key);
      if (sdl_name && *sdl_name)
        return sdl_name;
    }

  snprintf(buf, sizeof(buf), "Key%d", key);
  return buf;
}

static int *
get_input_state_ptr (int key, int create)
{
  int i;

  for (i=0; i < input_state_count; i++)
    if (input_state[i].key == key)
      return &input_state[i].state;

  if (!create || input_state_count >= MAX_TRACKED_INPUTS)
    return NULL;

  input_state[input_state_count].key = key;
  input_state[input_state_count].state = 0;
  input_state_count++;

  return &input_state[input_state_count - 1].state;
}

static void
set_input_state (int key, int state)
{
  int *slot = get_input_state_ptr(key, 1);
  if (slot)
    *slot = state;
}

static int
get_input_state (int key)
{
  int *slot = get_input_state_ptr(key, 0);
  return slot ? *slot : 0;
}

static void
set_key_name (int key, const char *name)
{
  int i;

  for (i=0; i < key_name_count; i++)
    {
      if (key_names[i].key == key)
        {
          key_names[i].name = name;
          return;
        }
    }

  if (key_name_count < (int)NUM_ELEM(key_names))
    {
      key_names[key_name_count].key = key;
      key_names[key_name_count].name = name;
      key_name_count++;
    }
}

void
init_keystr (void)
{
  key_name_count = 0;

#ifdef GCW0
  set_key_name(SDLK_BACKSPACE, "RSldr");
  set_key_name(SDLK_TAB, "LSldr");
  set_key_name(SDLK_RETURN, "Start");
  set_key_name(SDLK_SPACE, "Y");
  set_key_name(SDLK_ESCAPE, "Select");
  set_key_name(SDLK_LSHIFT, "X");
  set_key_name(SDLK_LCTRL, "A");
  set_key_name(SDLK_LALT, "B");
#else
  set_key_name(SDLK_BACKSPACE, "BS");
  set_key_name(SDLK_TAB, "Tab");
  set_key_name(SDLK_RETURN, "Return");
  set_key_name(SDLK_SPACE, "Space");
  set_key_name(SDLK_ESCAPE, "Esc");
#endif

  set_key_name(MOUSE_BUTTON1, "Mouse1");
  set_key_name(MOUSE_BUTTON2, "Mouse2");
  set_key_name(MOUSE_BUTTON3, "Mouse3");
  set_key_name(MOUSE_WHEELUP, "WheelUp");
  set_key_name(MOUSE_WHEELDOWN, "WheelDown");

  set_key_name(JOY_UP, "JoyUp");
  set_key_name(JOY_DOWN, "JoyDown");
  set_key_name(JOY_LEFT, "JoyLeft");
  set_key_name(JOY_RIGHT, "JoyRight");
  set_key_name(JOY_BUTTON1, "Joy-A");
  set_key_name(JOY_BUTTON2, "Joy-B");
  set_key_name(JOY_BUTTON3, "Joy-X");
  set_key_name(JOY_BUTTON4, "Joy-Y");

  return;
} // init_keystr()


int sgn (int x)
{
  return (x ? ((x)/abs(x)) : 0);
}

void Init_Joy (void)
{
  int num_joy;

  if (!SDL_InitSubSystem (SDL_INIT_JOYSTICK))
    {
      fprintf(stderr, "Couldn't initialize SDL-Joystick: %s\n", SDL_GetError());
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL Joystick initialisation successful.\n");


  DebugPrintf (1, " %d Joysticks found!\n", num_joy = SDL_NumJoysticks ());

  if (num_joy > 0)
    joy = SDL_OpenJoystick (0);

  if (joy)
    {
      DebugPrintf (1, "Identifier: %s\n", SDL_GetJoystickName (0));
      DebugPrintf (1, "Number of Axes: %d\n", joy_num_axes = SDL_GetNumJoystickAxes(joy));
      DebugPrintf (1, "Number of Buttons: %d\n", SDL_GetNumJoystickButtons(joy));

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

  if ( cmd_is_activeR(CMD_QUIT) ) {
    handle_QuitGame( ACTION_CLICK );
  }

  if ( cmd_is_activeR(CMD_PAUSE) )
    Pause ();

  if ( cmd_is_active (CMD_SCREENSHOT) )
    TakeScreenshot();

  if ( cmd_is_activeR ( CMD_FULLSCREEN ) ) {
    toggle_fullscreen();
  }

  if ( cmd_is_activeR ( CMD_MENU ) ) {
    showMainMenu ();
  }

  // this stuff remains hardcoded to keys
  if ( KeyIsPressedR('c') && AltPressed() && CtrlPressed() && ShiftPressed() )
    Cheatmenu ();

} // void ReactToSpecialKeys(void)

//----------------------------------------------------------------------
// main input-reading routine
//----------------------------------------------------------------------
int
update_input (void)
{
  Uint8 axis;
  SDL_Event event;
  
  // switch mouse-cursor visibility as a function of time of last activity
  if (SDL_GetTicks () - last_mouse_event > CURSOR_KEEP_VISIBLE)
    show_cursor = FALSE;
  else
    show_cursor = TRUE;

  while( SDL_PollEvent( &event ) )
    {
      switch( event.type )
	{
	case SDL_EVENT_QUIT:
	  printf("\n\nUser requested termination...\n\nTerminating...");
	  Terminate(0);
	  break;
	  /* Look for a keypress */

	case SDL_EVENT_KEY_DOWN:
	  current_modifiers = event.key.mod;
	  set_input_state(event.key.key, PRESSED);
#ifdef GCW0
	  if ( input_axis.x || input_axis.y ) axis_is_active = TRUE; // 4 GCW-0 ; breaks cursor keys after axis has been active...
#endif
	  break;
	case SDL_EVENT_KEY_UP:
	  current_modifiers = event.key.mod;
	  set_input_state(event.key.key, RELEASED);
#ifdef GCW0
	  axis_is_active = FALSE;
#endif
	  break;

	case SDL_EVENT_JOYSTICK_AXIS_MOTION:
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
		  set_input_state(JOY_RIGHT, PRESSED);
		  set_input_state(JOY_LEFT, FALSE);
		}
	      else if (joy_sensitivity*event.jaxis.value < -10000)
		{
		  set_input_state(JOY_LEFT, PRESSED);
		  set_input_state(JOY_RIGHT, FALSE);
		}
	      else
		{
		  set_input_state(JOY_LEFT, FALSE);
		  set_input_state(JOY_RIGHT, FALSE);
		}
	    }
	  else if ((axis == 1) || ((joy_num_axes >=5) && (axis == 4))) /* y-axis */
	    {
	      input_axis.y = event.jaxis.value;

	      if (joy_sensitivity*event.jaxis.value > 10000)
		{
		  set_input_state(JOY_DOWN, PRESSED);
		  set_input_state(JOY_UP, FALSE);
		}
	      else if (joy_sensitivity*event.jaxis.value < -10000)
		{
		  set_input_state(JOY_UP, PRESSED);
		  set_input_state(JOY_DOWN, FALSE);
		}
	      else
		{
		  set_input_state(JOY_UP, FALSE);
		  set_input_state(JOY_DOWN, FALSE);
		}
	    }

	  break;

	case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
	  // first button
	  if (event.jbutton.button == 0)
	    set_input_state(JOY_BUTTON1, PRESSED);

	  // second button
	  else if (event.jbutton.button == 1)
	    set_input_state(JOY_BUTTON2, PRESSED);

	  // and third button
	  else if (event.jbutton.button == 2)
	    set_input_state(JOY_BUTTON3, PRESSED);

	  // and fourth button
          else if (event.jbutton.button == 3)
	    set_input_state(JOY_BUTTON4, PRESSED);

	  axis_is_active = TRUE;
	  break;

	case SDL_EVENT_JOYSTICK_BUTTON_UP:
	  // first button
	  if (event.jbutton.button == 0)
	    set_input_state(JOY_BUTTON1, FALSE);

	  // second button
	  else if (event.jbutton.button == 1)
	    set_input_state(JOY_BUTTON2, FALSE);

	  // and third button
	  else if (event.jbutton.button == 2)
	    set_input_state(JOY_BUTTON3, FALSE);

	  // and fourth button
          else if (event.jbutton.button == 3)
	    set_input_state(JOY_BUTTON4, FALSE);

	  axis_is_active = FALSE;
	  break;

	case SDL_EVENT_MOUSE_MOTION:
	  input_axis.x = event.motion.x - UserCenter_x + 16;
	  input_axis.y = event.motion.y - UserCenter_y + 16;

	  last_mouse_event = SDL_GetTicks ();

	  break;

	  /* Mouse control */
	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    {
	      input_axis.x = event.button.x - UserCenter_x + 16;
	      input_axis.y = event.button.y - UserCenter_y + 16;
	      set_input_state(MOUSE_BUTTON1, PRESSED);
	      axis_is_active = TRUE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    set_input_state(MOUSE_BUTTON2, PRESSED);

	  if (event.button.button == SDL_BUTTON_MIDDLE)
	    set_input_state(MOUSE_BUTTON3, PRESSED);

	  // wheel events are immediately released, so we rather
	  // count the number of not yet read-out events
	  if (event.button.button == SDL_BUTTON_WHEELUP)
	      WheelUpEvents ++;

	  if (event.button.button == SDL_BUTTON_WHEELDOWN)
	      WheelDownEvents ++;

	  last_mouse_event = SDL_GetTicks();
	  break;

	case SDL_EVENT_MOUSE_WHEEL:
	  if (event.wheel.y > 0)
	    WheelUpEvents ++;
	  else if (event.wheel.y < 0)
	    WheelDownEvents ++;

	  last_mouse_event = SDL_GetTicks();
	  break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    {
	      set_input_state(MOUSE_BUTTON1, FALSE);
	      axis_is_active = FALSE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    set_input_state(MOUSE_BUTTON2, FALSE);

	  if (event.button.button == SDL_BUTTON_MIDDLE)
	    set_input_state(MOUSE_BUTTON3, FALSE);

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
  Uint8 axis = 0;

  //  keyboard_update ();   /* treat all pending keyboard-events */

  while ( !Returnkey )
    {
      SDL_WaitEvent (&event);    /* wait for next event */

      switch (event.type)
	{
	case SDL_EVENT_KEY_DOWN:
	  /*
	   * here we use the fact that, I cite from SDL_keyboard.h:
	   * "The keyboard syms have been cleverly chosen to map to ASCII"
	   * ... I hope that this design feature is portable, and durable ;)
	   */
	  Returnkey = (int) event.key.key;
	  if ( event.key.mod & SDL_KMOD_SHIFT )
	    Returnkey = toupper( (int)event.key.key );
	  break;

	case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
	  if (event.jbutton.button == 0)
	    Returnkey = JOY_BUTTON1;
	  else if (event.jbutton.button == 1)
	    Returnkey = JOY_BUTTON2;
	  else if (event.jbutton.button == 2)
	    Returnkey = JOY_BUTTON3;
          else if (event.jbutton.button == 3)
	    Returnkey = JOY_BUTTON4;
	  break;

        case SDL_EVENT_JOYSTICK_AXIS_MOTION:
	  axis = event.jaxis.axis;
          if (axis == 0 || ((joy_num_axes >= 5) && (axis == 3)) ) /* x-axis */
	    {
	      if (joy_sensitivity*event.jaxis.value > JOY_DEAD_ZONE)   /* about half tilted */
		{
		  Returnkey = JOY_RIGHT;
		}
	      else if (joy_sensitivity*event.jaxis.value < -JOY_DEAD_ZONE)
		{
		  Returnkey = JOY_LEFT;
		}
            }
          else if ((axis == 1) || ((joy_num_axes >=5) && (axis == 4))) /* y-axis */
	    {
	      if (joy_sensitivity*event.jaxis.value > JOY_DEAD_ZONE)
		{
		  Returnkey = JOY_DOWN;
		}
	      else if (joy_sensitivity*event.jaxis.value < -JOY_DEAD_ZONE)
		{
                  Returnkey = JOY_UP;
		}
            }
          break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
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

	case SDL_EVENT_MOUSE_WHEEL:
	  if (event.wheel.y > 0)
	    Returnkey = MOUSE_WHEELUP;
	  else if (event.wheel.y < 0)
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
KeyIsPressed (int key)
{
  update_input();

  return( (get_input_state(key) & PRESSED) == PRESSED );
}


// does the same as KeyIsPressed, but automatically releases the key as well..
bool
KeyIsPressedR (int key)
{
  bool ret;

  ret = KeyIsPressed (key);

  ReleaseKey (key);
  return (ret);
}

void
ReleaseKey (int key)
{
  set_input_state(key, FALSE);
  return;
}

void
wait_for_all_keys_released (void)
{
  while ( any_key_is_pressedR() ) {
    SDL_Delay(1);
  }
  ResetMouseWheel();
  return;
}

int
wait_for_key_pressed ( void )
{
  int key;
  while ( (key = any_key_just_pressed()) == 0 ) {
    SDL_Delay(1);
  }
  return key;
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

int
JoyAxisMotion ( void )
{
  update_input();
  return ( get_input_state(JOY_UP) || get_input_state(JOY_DOWN) || get_input_state(JOY_LEFT) || get_input_state(JOY_RIGHT) );
}

// check if any keys have been 'freshly' pressed
// if yes, return key-code, otherwise 0
int
any_key_just_pressed (void)
{
#ifdef ANDROID
    SDL_UpdateWindowSurface(FD_GetWindow());	// make sure we keep updating screen to read out Android inputs
#endif
  update_input();

  int i;
  for (i=0; i < input_state_count; i++)
    {
      if ( just_pressed(input_state[i].state) )
        {
          clear_fresh(input_state[i].state);
          return input_state[i].key;
        }
    }

  return 0;

}  // any_key_just_pressed()

// check if any keys are in a current 'pressed' state, and soft-release them
bool
any_key_is_pressedR (void)
{
#ifdef ANDROID
  SDL_UpdateWindowSurface(FD_GetWindow());	// make sure we keep updating screen to read out Android inputs
#endif
  update_input();

  int i;
  for ( i=0; i < input_state_count; i ++)
    {
      if ( (input_state[i].state & PRESSED) ) {
        input_state[i].state = 0;
        return TRUE;
      }
    }

  return FALSE;

}  // any_key_is_pressed()

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

  bool c1 = KeyIsPressedR( key_cmds[cmd][0] );
  bool c2 = KeyIsPressedR( key_cmds[cmd][1] );
  bool c3 = KeyIsPressedR( key_cmds[cmd][2] );

  return ( (c1 || c2 || c3) );

} // cmd_is_active()

#undef _intput_c
