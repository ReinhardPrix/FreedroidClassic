/* 
 * system.h: here we include all system-wide includes
 *           and take into account the AC-defined conditionals
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


/* 
 * well, for the time being we actually don't care too much about the 
 * AC-defined conditionals and pretend it's standard linux
 */

#include "config.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <getopt.h>

#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#if HAVE_LIBY2
#include <Y2/Y.h>		//  Basic Y types and constants.
#include <Y2/Ylib.h>		//  YLib functions and structs.
/* Change this to the address and port of the Y server you want
 * to connect to. Note that 127.0.0.1 is a symbolic address
 * meaning `localhost'.
 */
#define CON_ARG             "127.0.0.1:9433"
#endif

