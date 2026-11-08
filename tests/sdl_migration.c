#include "system.h"
#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include <assert.h>

SDL_Rect User_Rect;

void DebugPrintf (int level, const char *fmt, ...)
{
  (void)level;
  (void)fmt;
}

void Terminate (int code) { exit (code); }

static void check_joystick (void)
{
  SDL_VirtualJoystickDesc desc;
  SDL_JoystickID virtual_id;
  SDL_JoystickID *sticks;
  int count;

  assert (SDL_Init (SDL_INIT_JOYSTICK));
  SDL_INIT_INTERFACE (&desc);
  desc.name = "Freedroid migration test";
  desc.naxes = 2;
  desc.nbuttons = 4;
  virtual_id = SDL_AttachVirtualJoystick (&desc);
  assert (virtual_id != 0);
  sticks = SDL_GetJoysticks (&count);
  assert (sticks && count > 0);
  Init_Joy ();
  assert (joy && SDL_GetJoystickID (joy) == sticks[0]);
  assert (SDL_GetJoystickName (joy));
  assert (joy_num_axes == SDL_GetNumJoystickAxes (joy));
  SDL_free (sticks);
  SDL_CloseJoystick (joy);
  joy = NULL;
  assert (SDL_DetachVirtualJoystick (virtual_id));

  sticks = SDL_GetJoysticks (&count);
  assert (sticks);
  if (count == 0)
    {
      Init_Joy ();
      assert (joy == NULL);
    }
  SDL_free (sticks);
  SDL_Quit ();
}

int main (void)
{
  check_joystick ();
  return 0;
}
