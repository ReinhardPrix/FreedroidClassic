/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of FreeParadroid+
 *
 *  FreeParadroid+ is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeParadroid+ is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeParadroid+; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*----------------------------------------------------------------------
 *
 * Desc:  all sound functions as dummies: for paranosound 
 *
 *----------------------------------------------------------------------*/
#include <config.h>

#ifndef _sound_c
#define _sound_c
#endif

#include "defs.h"

void YIFF_Server_Check_Events(void){}
int Init_YIFF_Sound_Server(void){return(OK);}
void YIFF_Server_Close_Connections(void){}
void Play_YIFF_BackgroundMusic(int dummy){}
void Play_YIFF_Server_Sound(int dummy){}
void StartSound(int dummy ){}
void GotHitSound(void){}
void GotIntoBlastSound(void){}
void CrySound(void){}
void RefreshSound(void){}
void MoveElevatorSound(void){}
void EnterElevatorSound(void){}
void LeaveElevatorSound(void){}
void FireBulletSound(void){}
void BounceSound(void){}
int InitModPlayer(void){return(OK);}
void StopModPlayer(void){}
void PlayMod(char *modfile){}

#undef _sound_c
