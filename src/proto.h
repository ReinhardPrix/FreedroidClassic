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
#ifndef _proto_h
#define _proto_h

/* main.c */
#undef EXTERN
#ifdef _main_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN float Frame_Time (void);
EXTERN void Activate_Conservative_Frame_Computation(void);
EXTERN int ShipEmptyCounter;
EXTERN int WaitElevatorCounter;
EXTERN int TimerFlag;
EXTERN void PutEnergy (void);
EXTERN void ThouArtDefeated (void);
EXTERN void ThouArtVictorious (void);
EXTERN void StandardEnergieLieferungen (void);

/* main.c */
#undef EXTERN
#ifdef _init_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void parse_command_line (int argc, char *const argv[]);
EXTERN void Title (void);
EXTERN void EndTitle (void);
EXTERN void InitParaplus (void);
EXTERN void InitNewGame (void);
EXTERN void Init_Druidmap (void);
/* influ.c */
#undef EXTERN
#ifdef _influ_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int isignf (float);
EXTERN void MoveInfluence (void);
EXTERN void AdjustSpeed (void);
EXTERN void BounceInfluencer (void);
EXTERN void Reibung (void);
EXTERN void CheckEnergieLevel (void);
EXTERN void AnimateInfluence (void);
EXTERN void InfluenceEnemyCollision (void);
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
EXTERN void StartBlast (int x, int y, int type);
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
EXTERN unsigned char *FeindZusammenstellen (const char *, int);
// EXTERN void SetColors(int,int,char*);
EXTERN void RotateBulletColor (void);
EXTERN void GetView (void);
EXTERN void ShowPosition (void);
EXTERN void DisplayView (void);
EXTERN void GetInternFenster (int mask);
EXTERN void PutInfluence (void);
EXTERN void PutBullet (int);
EXTERN void PutBlast (int);
EXTERN void PutEnemy (int);
EXTERN void PutInternFenster (int);
EXTERN int PutObject (int x, int y, unsigned char *pic, int check);
EXTERN void DrawDigit (unsigned char *, unsigned char *);
EXTERN void RedrawInfluenceNumber (void);
EXTERN void SetUserfenster (int color, unsigned char *screen);
EXTERN void ShowRobotPicture (int PosX, int PosY, int Number,
			      unsigned char *Screen);


/* blocks.c */
#undef EXTERN
#ifdef _blocks_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void SmallBlock (int, int, int, unsigned char *, int);
EXTERN void SmallEnemy (int, int, int, unsigned char *, int);
EXTERN void SmallBlast (int, int, int, int, unsigned char *, int);
EXTERN void SmallBullet (int, int, int, int, unsigned char *, int);

EXTERN void GetMapBlocks (void);
EXTERN void GetShieldBlocks (void);
EXTERN void IsolateBlock (unsigned char *screen,
			  unsigned char *target,
			  int BlockEckLinks,
			  int BlockEckOben, int Blockbreite, int Blockhoehe);

EXTERN unsigned char *GetBlocks (char *picfile, int line, int num);
EXTERN int MergeBlockToWindow (register unsigned char *source,
			       register unsigned char *target,
			       int WinLineLen, int check);

EXTERN void GetDigits (void);

EXTERN void DisplayBlock (int x, int y,
			  unsigned char *block,
			  int len, int height, unsigned char *screen);


EXTERN void DisplayMergeBlock (int x, int y,
			       unsigned char *block,
			       int len, int height, unsigned char *screen);

EXTERN void CopyMergeBlock (unsigned char *target,
			    unsigned char *source, int mem);

EXTERN int MergeBlockToWindow (unsigned char *,
			       unsigned char *, int WinLineLen, int check);

/* graphics.c */
#undef EXTERN
#ifdef _graphics_c
#define EXTERN
#else
#define EXTERN extern
#endif

// EXTERN void Lock_SDL_Screen(void);
// EXTERN void Unlock_SDL_Screen(void);
// EXTERN void Update_SDL_Screen(void);
// EXTERN Uint32 getpixel(SDL_Surface *surface, int x, int y);
// EXTERN void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
EXTERN void display_bmp(char *file_name);
EXTERN void MakeGridOnScreen(unsigned char*);
EXTERN int InitPictures (void);
EXTERN void ClearVGAScreen (void);
EXTERN void SetColors (int FirstCol, int PalAnz, char *PalPtr);
EXTERN void SetPalCol (unsigned int palpos, unsigned char rot,
		       unsigned char gruen, unsigned char blau);
EXTERN void SetPalCol2 (unsigned int palpos, color Farbwert);
EXTERN int InitLevelColorTable (void);
EXTERN int InitPalette (void);
EXTERN void SetLevelColor (int);
EXTERN void Load_PCX_Image (char *, unsigned char *, int);
EXTERN void LadeLBMBild (char *LBMDateiname, unsigned char *Screen,
			 int LoadPal);
EXTERN void TransparentLadeLBMBild (char *LBMDateiname, unsigned char *Screen,
				    int LoadPal);
EXTERN void Init_Video (void);
EXTERN void UnfadeLevel (void);
EXTERN void LadeZeichensatz (char *Zeichensatzname);
EXTERN void RotateColors (int, int);
EXTERN void LevelGrauFaerben (void);
EXTERN void ClearGraphMem (unsigned char *screen);
EXTERN void Flimmern (int type);

/* map.c */
#undef EXTERN
#ifdef _map_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN unsigned char GetMapBrick (Level deck, float x, float y);
EXTERN int GetCurrentElevator (void);
EXTERN void ActSpecialField (float, float);

EXTERN int LoadShip (char *shipname);
EXTERN int SaveShip(char *shipname);
EXTERN Level LevelToStruct (char *data);
EXTERN int GetDoors (Level Lev);
EXTERN int GetWaypoints (Level Lev);
EXTERN int GetRefreshes (Level Lev);
EXTERN int GetElevatorConnections (char *shipname);
EXTERN int GetCrew (char *shipname);

EXTERN void AnimateRefresh (void);
EXTERN void MoveLevelDoors (void);
EXTERN int IsPassable (int x, int y, int Checkpos);
EXTERN int DruidPassable (int x, int y);
EXTERN int IsVisible (Finepoint objpos);
EXTERN int TranslateMap (Level Lev);

/* sound.c  OR nosound.c */
#undef EXTERN
#ifdef _sound_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void Set_BG_Music_Volume(float);
EXTERN void Set_Sound_FX_Volume(float);
EXTERN void YIFF_Server_Check_Events (void);
EXTERN int Init_YIFF_Sound_Server (void);
EXTERN void YIFF_Server_Close_Connections (void);
EXTERN void Switch_Background_Music_To (int);
EXTERN void Play_YIFF_Server_Sound (int);
EXTERN void StartSound (int);
EXTERN void GotHitSound (void);
EXTERN void GotIntoBlastSound (void);
EXTERN void CrySound (void);
EXTERN void RefreshSound (void);
EXTERN void MoveElevatorSound (void);
EXTERN void MenuItemSelectedSound (void);
EXTERN void MoveMenuPositionSound (void);
EXTERN void EnterElevatorSound (void);
EXTERN void LeaveElevatorSound (void);
// EXTERN void FireBulletSound (void);
EXTERN void Fire_Bullet_Sound (int);
EXTERN void BounceSound (void);
EXTERN void DruidBlastSound (void);
EXTERN void ThouArtDefeatedSound (void);
EXTERN void Takeover_Set_Capsule_Sound (void);
EXTERN void Takeover_Game_Won_Sound (void);
EXTERN void Takeover_Game_Deadlock_Sound (void);
EXTERN void Takeover_Game_Lost_Sound (void);

/* keyboard.c */
#undef EXTERN
#ifdef _keyboard_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int Shift_Was_Pressed(void);
EXTERN int Ctrl_Was_Pressed(void);
EXTERN int Alt_Was_Pressed(void);
EXTERN void Init_SDL_Keyboard(void);
EXTERN int getchar_raw (void);
EXTERN int keyboard_update(void);
EXTERN void ClearKbState (void);
EXTERN int LeftPressed (void);
EXTERN int RightPressed (void);
EXTERN int UpPressed (void);
EXTERN int DownPressed (void);
EXTERN int SpacePressed (void);
EXTERN int EnterPressed (void);
EXTERN int EscapePressed (void);
EXTERN int BackspacePressed (void);
EXTERN int KP0Pressed (void);
EXTERN int KP1Pressed (void);
EXTERN int KP2Pressed (void);
EXTERN int KP3Pressed (void);
EXTERN int KP4Pressed (void);
EXTERN int KP5Pressed (void);
EXTERN int KP6Pressed (void);
EXTERN int KP7Pressed (void);
EXTERN int KP8Pressed (void);
EXTERN int KP9Pressed (void);
EXTERN int APressed (void);
EXTERN int BPressed (void);
EXTERN int CPressed (void);
EXTERN int DPressed (void);
EXTERN int EPressed (void);
EXTERN int FPressed (void);
EXTERN int GPressed (void);
EXTERN int HPressed (void);
EXTERN int IPressed (void);
EXTERN int JPressed (void);
EXTERN int KPressed (void);
EXTERN int LPressed (void);
EXTERN int MPressed (void);
EXTERN int NPressed (void);
EXTERN int OPressed (void);
EXTERN int PPressed (void);
EXTERN int QPressed (void);
EXTERN int RPressed (void);
EXTERN int SPressed (void);
EXTERN int TPressed (void);
EXTERN int UPressed (void);
EXTERN int VPressed (void);
EXTERN int WPressed (void);
EXTERN int XPressed (void);
EXTERN int YPressed (void);
EXTERN int ZPressed (void);
EXTERN int NoDirectionPressed (void);

/* misc.c */
#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void DebugPrintf (char *);
EXTERN void DebugPrintfInt (int);
EXTERN void DebugPrintfFloat (float);
EXTERN void gotoxy (int, int);
EXTERN int MyRandom (int);
EXTERN void reverse (char *);
EXTERN char *itoa (int, char *, int);
EXTERN char *ltoa (long, char *, int);
EXTERN void Armageddon (void);
EXTERN void Teleport (int LNum, int X, int Y);
EXTERN void Cheatmenu (void);
EXTERN void EscapeMenu (void);
EXTERN void InsertNewMessage (void);
EXTERN void Terminate (int);
EXTERN void KillQueue (void);
EXTERN void PutMessages (void);
EXTERN void InsertMessage (char *MText);
EXTERN void *MyMalloc (long);
EXTERN void DirToVect (int dir, Vect vector);

EXTERN long my_sqrt (long);
EXTERN int my_abs (int);

EXTERN void ShowDebugInfos (void);
EXTERN void ShowHighscoreList (void);

/* enemy.c */
#undef EXTERN
#ifdef _enemy_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void InitEnemys (void);
EXTERN void ShuffleEnemys (void);
EXTERN int EnemyEnemyCollision (int enemynum);
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
EXTERN void EnterElevator (void);
EXTERN void EnterKonsole (void);
EXTERN void AlleLevelsGleichFaerben (void);
EXTERN void HilightLevel (int);
EXTERN void HilightElevator (int);
EXTERN int LevelEmpty (void);
EXTERN int ShipEmpty (void);
EXTERN void ClearUserFenster (void);
EXTERN void ClearAllButRahmen (void);


/* text.c */
#undef EXTERN
#ifdef _text_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void StoreTextEnvironment (void);
EXTERN void RestoreTextEnvironment (void);

EXTERN int InitParaplusFont (void);

EXTERN void SetTextColor (unsigned char bg, unsigned char fg);
EXTERN void GetTextColor (unsigned int *bg, unsigned int *fg);

EXTERN void SetTextBorder (int left, int upper, int right, int lower,
			   int chars);
EXTERN void ClearTextBorder (unsigned char *screen, int color);
EXTERN void SetTextCursor (int x, int y);
EXTERN void SetLineLength (int);

EXTERN void DisplayText (char *text, int startx, int starty,
			 unsigned char *screen, int EnterCursor);
EXTERN void DisplayChar (unsigned char Zeichen, unsigned char *screen);
EXTERN int ScrollText (char *text, int startx, int starty, int EndLine);

EXTERN void CheckUmbruch (void);
EXTERN void ImprovedCheckUmbruch(char *);
EXTERN void MakeUmbruch (void);
EXTERN int FensterVoll (void);
EXTERN char *PreviousLine (char *textstart, char *text);
EXTERN char *NextLine (char *text);
EXTERN char *GetString (int MaxLen, int echo);

/* rahmen.c */
#undef EXTERN
#ifdef _rahmen_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void DisplayRahmen (unsigned char *screen);
EXTERN void SetInfoline (const char *left, const char *right);


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

EXTERN int GetTakeoverGraphics (void);
EXTERN void ShowPlayground (void);
EXTERN void InventPlayground (void);

EXTERN void ProcessPlayground (void);
EXTERN void ProcessDisplayColumn (void);
EXTERN void ProcessCapsules (void);

EXTERN void ClearPlayground (void);
EXTERN int IsActive (int color, int row);
EXTERN void InitTakeover (void);

#undef EXTERN
#ifdef _svgaemu_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void PrepareScaledSurface(int);
EXTERN void Lock_SDL_Screen(void);
EXTERN void Unlock_SDL_Screen(void);
EXTERN void Update_SDL_Screen(void);
EXTERN Uint32 getpixel(SDL_Surface *surface, int x, int y);
EXTERN void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

EXTERN int vga_setflipchar(int c);
EXTERN int vga_flip(void);
EXTERN int vga_getxdim(void);
EXTERN int vga_getydim(void);
EXTERN int vga_getcolors(void);
EXTERN int vga_setpalette(int index, int red, int green, int blue);
EXTERN int vga_getpalette(int index, int *red, int *green, int *blue);
EXTERN int vga_setpalvec(int start, int num, int *pal);
EXTERN int vga_getpalvec(int start, int num, int *pal);
EXTERN int vga_screenoff(void);
EXTERN int vga_screenon(void);
EXTERN int vga_setcolor(int color);
EXTERN int vga_drawpixel(int x, int y);
EXTERN int vga_drawline(int x1, int y1, int x2, int y2);
EXTERN int vga_drawscanline(int line, unsigned char *colors);
EXTERN int vga_drawscansegment(unsigned char *colors, int x, int y, int length);
EXTERN int vga_getpixel(int x, int y);      /* Added. */
EXTERN int vga_getscansegment(unsigned char *colors, int x, int y, int length);
EXTERN int vga_getch(void);
EXTERN int vga_dumpregs(void);
EXTERN int vga_white(void);
EXTERN void vga_waitretrace(void);
EXTERN int vga_getdefaultmode(void);


#endif
