/*=@Header==============================================================
 * $Source$
 *
 * @Desc:  all sound functions as dummies: for paranosound
 *
 *-@Header------------------------------------------------------------*/

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
