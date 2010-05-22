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
#ifndef _proto_h
#define _proto_h

#include "struct.h"

/* main.c */
#undef EXTERN
#ifdef _main_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int ShipEmptyCounter;

/* main.c */
#undef EXTERN
#ifdef _init_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void parse_command_line (int argc, char *const argv[]);
EXTERN void Title ( char *MissionBriefingPointer );
EXTERN void InitFreedroid (int argc, char *const argv[]);
EXTERN void InitNewMission (char *MissionName);
EXTERN void CheckIfMissionIsComplete (void);
EXTERN void ThouArtDefeated (void);
EXTERN void ThouArtVictorious (void);
EXTERN void Win32Disclaimer (void);

/* influ.c */
#undef EXTERN
#ifdef _influ_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int isignf (float);
EXTERN void InitInfluPositionHistory( void );
EXTERN float GetInfluPositionHistoryX( int Index );
EXTERN float GetInfluPositionHistoryY( int Index );
EXTERN float GetInfluPositionHistoryZ( int Index );
EXTERN void MoveInfluence (void);
EXTERN void AdjustSpeed (void);
EXTERN void CheckInfluenceWallCollisions (void);
EXTERN void InfluenceFrictionWithAir (void);
EXTERN void CheckEnergieLevel (void);
EXTERN void AnimateInfluence (void);
EXTERN void CheckInfluenceEnemyCollision (void);
EXTERN void RefreshInfluencer (void);
EXTERN void ExplodeInfluencer (void);

/* bullet.c */
#undef EXTERN
#ifdef _bullet_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void FireBullet (void);
EXTERN void MoveBullets (void);
EXTERN void DeleteBullet (int num);
EXTERN void StartBlast (float x, float y, int type);
EXTERN void ExplodeBlasts (void);
EXTERN void DeleteBlast (int num);
EXTERN void CheckBulletCollisions (int num);
EXTERN void CheckBlastCollisions (int num);

/* view.c */
#undef EXTERN
#ifdef _view_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void Fill_Rect (SDL_Rect rect, SDL_Color color);
EXTERN void ShowPosition (void);
EXTERN void Assemble_Combat_Picture (int );
EXTERN void PutInfluence (int , int );
EXTERN void PutBullet (int);
EXTERN void PutBlast (int);
EXTERN void PutEnemy (int Enum, int x , int y );
EXTERN void PutAshes (float x, float y);
EXTERN void SetUserfenster (int color );
EXTERN void ShowRobotPicture (int PosX, int PosY, int droid_num, int frame_num);
EXTERN void DisplayBanner (const char* left, const char* right, int flags );

/* graphics.c */
#undef EXTERN
#ifdef _graphics_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int ApplyFilter (SDL_Surface *surf, float fred, float fgreen, float fblue);
EXTERN void GetRGBA ( SDL_Surface* surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);

EXTERN void DrawLineBetweenTiles( float x1 , float y1 , float x2 , float y2 , int Color );
EXTERN void TakeScreenshot( void );
EXTERN void DisplayImage(char *file_name);
EXTERN void MakeGridOnScreen( SDL_Rect* Grid_Rectangle );
EXTERN void SetCombatScaleTo(float);
EXTERN int InitPictures (void);

EXTERN void Init_Video (void);
EXTERN void ClearGraphMem ( void );
EXTERN Uint32 getpixel(SDL_Surface *surface, int x, int y);
EXTERN void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
EXTERN void white_noise (SDL_Surface *bitmap, SDL_Rect *rect, int timeout);
EXTERN void ScaleGraphics (float scale);
EXTERN void ScalePic (SDL_Surface **pic, float scale);
EXTERN void ScaleStatRects (float scale);
EXTERN void toggle_fullscreen (void);
EXTERN SDL_Surface *Load_Block (char *fpath, int line, int col, SDL_Rect * block, int flags);

/* map.c */
#undef EXTERN
#ifdef _map_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN unsigned char GetMapBrick (Level deck, float x, float y);
EXTERN int GetCurrentLift (void);
EXTERN void ActSpecialField (float, float);

EXTERN int LoadShip (char *shipname);
EXTERN int SaveShip(char *shipname);
EXTERN Level LevelToStruct (char *data);
EXTERN int GetDoors (Level Lev);
EXTERN int GetRefreshes (Level Lev);
EXTERN int GetLiftConnections (char *shipname);
EXTERN int GetCrew (char *shipname);

EXTERN void AnimateRefresh (void);
EXTERN void MoveLevelDoors (void);
EXTERN int IsPassable (float x, float y, int Checkpos);
EXTERN int DruidPassable (float x, float y);
EXTERN int IsVisible (Finepoint objpos);
EXTERN int InterpretMap (Level Lev);

/* sound.c  OR nosound.c */
#undef EXTERN
#ifdef _sound_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void Init_Audio(void);
EXTERN void Set_BG_Music_Volume(float);
EXTERN void Set_Sound_FX_Volume(float);
EXTERN void Switch_Background_Music_To ( char * filename_raw );
EXTERN void Play_Sound (int);
EXTERN void GotHitSound (void);
EXTERN void GotIntoBlastSound (void);
EXTERN void CountdownSound (void);
EXTERN void EndCountdownSound (void);
EXTERN void CrySound (void);
EXTERN void TransferSound (void);
EXTERN void RefreshSound (void);
EXTERN void MoveLiftSound (void);
EXTERN void MenuItemSelectedSound (void);
EXTERN void MoveMenuPositionSound (void);
EXTERN void EnterLiftSound (void);
EXTERN void LeaveLiftSound (void);
EXTERN void Fire_Bullet_Sound (int);
EXTERN void BounceSound (void);
EXTERN void DruidBlastSound (void);
EXTERN void ThouArtDefeatedSound (void);
EXTERN void Takeover_Set_Capsule_Sound (void);
EXTERN void Takeover_Game_Won_Sound (void);
EXTERN void Takeover_Game_Deadlock_Sound (void);
EXTERN void Takeover_Game_Lost_Sound (void);

/* input.c */
#undef EXTERN
#ifdef _input_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ReleaseKey (SDLKey key);
EXTERN int update_input(void);
EXTERN int getchar_raw (void);
EXTERN void ResetMouseWheel (void);
EXTERN bool WheelUpPressed(void);
EXTERN bool WheelDownPressed(void);
EXTERN void Init_Joy(void);
EXTERN void ReactToSpecialKeys(void);

EXTERN bool any_key_pressed (void);

EXTERN bool KeyIsPressed (SDLKey key);
EXTERN bool KeyIsPressedR (SDLKey key);
EXTERN bool ModIsPressed (SDLMod mod);

EXTERN bool NoDirectionPressed (void);
EXTERN bool cmd_is_active(enum _cmds cmd);
EXTERN bool cmd_is_activeR(enum _cmds cmd);
EXTERN void init_keystr (void);

/* menu.c */
#undef EXTERN
#ifdef _menu_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void InitiateMenu (bool with_droids);
EXTERN void QuitGameMenu (void);
EXTERN void Cheatmenu (void);
EXTERN void EscapeMenu (void);
EXTERN bool LevelEditMenu (void);

/* misc.c */
#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int LoadGameConfig (void);
EXTERN int SaveGameConfig (void);
EXTERN int sign (float x);
EXTERN char* ReadAndMallocStringFromData ( char* SearchString , char* StartIndicationString , char* EndIndicationString );
EXTERN int CountStringOccurences ( char* SearchString , char* TargetString ) ;
EXTERN void ReadValueFromString(char* data, char* label, char* FormatString, void* dst);
EXTERN char* ReadAndMallocAndTerminateFile( char* filename , char* File_End_String ) ;
EXTERN char* LocateStringInData ( char* SearchBeginPointer, char* SearchTextPointer ) ;
EXTERN char* find_file (char *fname, char *subdir, int use_theme, int critical);
EXTERN void CheckForTriggeredEvents ( void );
EXTERN void Pause (void);
EXTERN void ComputeFPSForThisFrame(void);
EXTERN void StartTakingTimeForFPSCalculation(void);
EXTERN int Get_Average_FPS ( void );
EXTERN float Frame_Time (void);
EXTERN void Activate_Conservative_Frame_Computation(void);
EXTERN void DebugPrintf (int db_level, char *fmt, ...);
EXTERN int MyRandom (int);
EXTERN void Armageddon (void);
EXTERN void Teleport (int LNum, int X, int Y);
EXTERN void Terminate (int);
EXTERN void *MyMalloc (long);
EXTERN int FS_filelength (FILE *f);
EXTERN void init_progress (char *txt);
EXTERN void update_progress (int percent);

/* enemy.c */
#undef EXTERN
#ifdef _enemy_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void Enemy_Post_Bullethit_Behaviour( int EnemyNum );
EXTERN void ShuffleEnemys (void);
EXTERN int CheckEnemyEnemyCollision (int enemynum);
EXTERN void MoveEnemys (void);
EXTERN void AttackInfluence (int enemynum);
EXTERN void AnimateEnemys (void);
EXTERN void ClearEnemys (void);
EXTERN int ClassOfDruid (int druidtype);

/* ship.c */
#undef EXTERN
#ifdef _ship_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ShowDeckMap (Level deck);
EXTERN void EnterLift (void);
EXTERN void EnterKonsole (void);
EXTERN int LevelEmpty (void);
void GreatDruidShow (void);
EXTERN void show_droid_info (int droidtype, int page, int flags);
EXTERN void show_droid_portrait (SDL_Rect dst, int droid_type, float cycle_time, int flags);
void ShowLifts (int level, int liftrow);
void PaintConsoleMenu (int pos,int flag);
void AlertLevelWarning (void);

/* text.c */
#undef EXTERN
#ifdef _text_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void EnemyHitByBulletText( int Enum );
EXTERN void EnemyInfluCollisionText ( int Enum );
EXTERN void AddInfluBurntText( void );
EXTERN void AddStandingAndAimingText ( int Enum );
EXTERN int DisplayText (char *text, int startx, int starty, const SDL_Rect *clip);
EXTERN void DisplayChar (unsigned char c);
EXTERN int ScrollText (char *Text, SDL_Rect *rect , int SecondsMinimumDuration );
EXTERN bool linebreak_needed (char *textpos , const SDL_Rect *clip);
EXTERN char *GetString (int MaxLen, int echo);
EXTERN void printf_SDL (SDL_Surface *screen, int x, int y, char *fmt, ...);
EXTERN int putchar_SDL (SDL_Surface *Surface, int x, int y, int c);

/* takeover.c */
#undef EXTERN
#ifdef _takeover_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int Takeover (int enemynum);
EXTERN void ChooseColor (void);
EXTERN void PlayGame (void);
EXTERN void EnemyMovements (void);

EXTERN int set_takeover_rects (void);
EXTERN void ShowPlayground ();
EXTERN void InventPlayground (void);

EXTERN void ProcessPlayground (void);
EXTERN void ProcessDisplayColumn (void);
EXTERN void ProcessCapsules (void);
EXTERN void AnimateCurrents (void);

EXTERN void ClearPlayground (void);
EXTERN int IsActive (int color, int row);

#undef EXTERN
#ifdef _highscore_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void InitHighscores (void);
EXTERN int SaveHighscores (void);
EXTERN void UpdateHighscores (void);
EXTERN void ShowHighscores (void);
#endif
