/* 
 * Joystick/gamepad related code
 */

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
#define _joy_c

#include "system.h"
#include "defs.h"
#include "proto.h"

void Init_Joy (void)
{
  SDL_Joystick *joy;
  int num_joy;

  if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) == -1)
    {
      fprintf(stderr, "Couldn't initialize SDL-Joystick: %s\n",SDL_GetError());
      Terminate(ERR);
    } else
      printf("\nSDL Joystick initialisation successful.\n");


  printf (" %d Joysticks found!\n", num_joy = SDL_NumJoysticks ());

  if (num_joy > 0)
    {
      printf ("Identifier: %s\n", SDL_JoystickName (0));
      joy = SDL_JoystickOpen (0);
    }


  return;
}

#undef _joy_c

