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
EXTERN void AssignMission( int MissNum );
EXTERN void parse_command_line (int argc, char *const argv[]);
EXTERN void Title ( char *MissionBriefingPointer );
EXTERN void EndTitle (void);
EXTERN void ClearAutomapData( void );
EXTERN void InitFreedroid (void);
EXTERN void InitNewMissionList (char *MissionName);
EXTERN void ThouArtDefeated (void);
EXTERN void ThouArtVictorious (void);
EXTERN void CheckIfMissionIsComplete (void);

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
// EXTERN void Move_Influencers_Friends ( void );
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
EXTERN void RotateVectorByAngle ( moderately_finepoint* vector , float rot_angle );
EXTERN void FireBullet (void);
EXTERN void MoveBullets (void);
EXTERN void DeleteBullet (int num , int StartBlast );
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
EXTERN void DisplayItemImageAtMouseCursor( int ItemImageCode );
EXTERN void Assemble_Combat_Picture (int );
EXTERN void PutInfluence (int , int );
EXTERN void PutBullet (int);
EXTERN void PutItem (int);
EXTERN void PutBlast (int);
EXTERN void PutEnemy (int Enum, int x , int y );
EXTERN void ShowRobotPicture (int PosX, int PosY, int Number);
EXTERN void ShowInventoryScreen ( void );


/* blocks.c */
#undef EXTERN
#ifdef _blocks_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void Load_Item_Surfaces (void);
EXTERN void Load_SkillIcon_Surfaces (void);
EXTERN void Load_MapBlock_Surfaces (void);
EXTERN void Load_Enemy_Surfaces (void);
EXTERN void Load_Influencer_Surfaces (void);
EXTERN void Update_Tux_Working_Copy ( void );
EXTERN void Load_Tux_Surfaces( void );
EXTERN void Load_Digit_Surfaces (void);
EXTERN void Load_Bullet_Surfaces (void);
EXTERN void Load_Blast_Surfaces (void);

/* graphics.c */
#undef EXTERN
#ifdef _graphics_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void DrawLineBetweenTiles( float x1 , float y1 , float x2 , float y2 , int Color );
EXTERN void TakeScreenshot( void );
EXTERN void replace_color (SDL_Surface *surf, SDL_Color src, SDL_Color dst);
EXTERN void DisplayImage(char *file_name);
EXTERN void MakeGridOnScreen( SDL_Rect* Grid_Rectangle );
EXTERN void SetCombatScaleTo(float);
EXTERN int ReInitPictures (void);
EXTERN int InitPictures (void);
EXTERN void SetColors (int FirstCol, int PalAnz, char *PalPtr);
EXTERN void SetPalCol (unsigned int palpos, unsigned char rot,
		       unsigned char gruen, unsigned char blau);
EXTERN void Init_Video (void);
EXTERN void LadeZeichensatz (char *Zeichensatzname);
EXTERN void LevelGrauFaerben (void);
EXTERN void ClearGraphMem ( void );
EXTERN Uint32 getpixel(SDL_Surface *surface, int x, int y);
EXTERN void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

// saveloadgame.c 
#undef EXTERN
#ifdef _saveloadgame_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int SaveGame( void );
EXTERN int LoadGame( void );


/* map.c */
#undef EXTERN
#ifdef _map_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void CollectAutomapData ( void ) ;
EXTERN void Smash_Box ( float x , float y );
EXTERN void AnimateTeleports (void);
EXTERN unsigned char GetMapBrick (Level deck, float x, float y);
EXTERN int GetCurrentLift (void);
EXTERN void ActSpecialField (float, float);

EXTERN int LoadShip (char *filename);
EXTERN int SaveShip(char *filename);
EXTERN int GetDoors (Level Lev);
EXTERN int GetRefreshes (Level Lev);
EXTERN int GetLiftConnections (char *shipname);
EXTERN int GetCrew (char *shipname);

EXTERN void AnimateRefresh (void);
EXTERN void MoveLevelDoors (void);
EXTERN int IsPassable (float x, float y, int Checkpos);
EXTERN int DruidPassable (float x, float y);
EXTERN int IsVisible (Finepoint objpos);
EXTERN int TranslateMap (Level Lev);

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
// EXTERN void Switch_Background_Music_To (int);
EXTERN void Switch_Background_Music_To ( char * filename_raw );
EXTERN void Play_Sound (int);
EXTERN void StartSound (int);
EXTERN void GotHitSound (void);
EXTERN void Influencer_Scream_Sound (void);
EXTERN void GotIntoBlastSound (void);
EXTERN void Not_Enough_Power_Sound( void );
EXTERN void Not_Enough_Dist_Sound( void );
EXTERN void CrySound (void);
EXTERN void CantCarrySound (void);
EXTERN void ItemTakenSound (void);
EXTERN void TransferSound (void);
EXTERN void RefreshSound (void);
EXTERN void MoveLiftSound (void);
EXTERN void MenuItemSelectedSound (void);
EXTERN void MenuItemDeselectedSound (void);
EXTERN void MoveMenuPositionSound (void);
EXTERN void EnterLiftSound (void);
EXTERN void LeaveLiftSound (void);
EXTERN void Fire_Bullet_Sound (int);
EXTERN void Mission_Status_Change_Sound (void);
EXTERN void BounceSound (void);
EXTERN void DruidBlastSound (void);
EXTERN void ThouArtDefeatedSound (void);
EXTERN void Takeover_Set_Capsule_Sound (void);
EXTERN void Takeover_Game_Won_Sound (void);
EXTERN void Takeover_Game_Deadlock_Sound (void);
EXTERN void Takeover_Game_Lost_Sound (void);
EXTERN void PlayGreetingSound ( int SoundCode );
EXTERN void PlayItemSound ( int SoundCode );
EXTERN void PlayLevelCommentSound ( int levelnum );


// items.c
#undef EXTERN
#ifdef _items_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int GetFreeInventoryIndex( void );
EXTERN int ItemCanBeDroppedInInv ( int ItemType , int InvPos_x , int InvPos_y );
EXTERN long CalculateItemPrice ( item* BuyItem , int ForRepair );
EXTERN void FillInItemProperties( item* ThisItem , int FullDuration , int TreasureChestRange );
EXTERN void DamageAllEquipment( void );
EXTERN void DropItemAt( int ItemType , float x , float y , int prefix , int suffix , int TreasureChestRange );
EXTERN void Quick_ApplyItem( int ItemKey );
EXTERN void ApplyItem( item* CurItem );
EXTERN int Inv_Pos_Is_Free( int x , int y );
EXTERN int GetInventoryItemAt ( int x , int y );
EXTERN item* GetHeldItemPointer( void );
EXTERN int ItemUsageRequirementsMet( item* UseItem , int MakeSound );
EXTERN int CursorIsInInventoryGrid( int x , int y );
EXTERN int CursorIsInUserRect( int x , int y );
EXTERN int CursorIsInWeaponRect( int x , int y );
EXTERN int CursorIsInDriveRect( int x , int y );
EXTERN int CursorIsInShieldRect( int x , int y );
EXTERN int CursorIsInAux1Rect( int x , int y );
EXTERN int CursorIsInAux2Rect( int x , int y );
EXTERN int CursorIsInSpecialRect( int x , int y );
EXTERN int CursorIsInArmourRect( int x , int y );
EXTERN int GetHeldItemCode ( void );
EXTERN int GetInventorySquare_x( int x );
EXTERN int GetInventorySquare_y( int x );
EXTERN void DropHeldItemToWeaponSlot ( void );
EXTERN void DropHeldItemToDriveSlot ( void );
EXTERN void DropHeldItemToInventory( void );
EXTERN void DropHeldItemToTheFloor ( void );
EXTERN void ShowQuickInventory ( void );
EXTERN void ManageInventoryScreen ( void );
EXTERN void AddFloorItemDirectlyToInventory( item* ItemPointer );
EXTERN void CopyItem( item* SourceItem , item* DestItem , int MakeSound );
EXTERN void DeleteItem( item* Item );
EXTERN void DropRandomItem( float x , float y , int TreasureChestRange , int ForceMagical , int ForceDrop );

// character.c
#undef EXTERN
#ifdef _character_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void DisplayButtons( void );
EXTERN void UpdateAllCharacterStats ( void );
EXTERN void ShowCharacterScreen ( void );

// skills.c
#undef EXTERN
#ifdef _skills_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ShowSkillsScreen ( void );
EXTERN void HandleCurrentlyActivatedSkill( void );

/* input.c */
#undef EXTERN
#ifdef _input_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int MouseRightPressed(void);
EXTERN int GetMousePos_x(void);
EXTERN int GetMousePos_y(void);
EXTERN void Init_Joy(void);
EXTERN void ReactToSpecialKeys(void);
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
EXTERN int TabPressed (void);
EXTERN int BackspacePressed (void);
EXTERN int KP_PLUS_Pressed (void);
EXTERN int KP_MINUS_Pressed (void);
EXTERN int KP_MULTIPLY_Pressed (void);
EXTERN int KP_DIVIDE_Pressed (void);
EXTERN int KP_ENTER_Pressed (void);
EXTERN int Number0Pressed (void);
EXTERN int Number1Pressed (void);
EXTERN int Number2Pressed (void);
EXTERN int Number3Pressed (void);
EXTERN int Number4Pressed (void);
EXTERN int Number5Pressed (void);
EXTERN int Number6Pressed (void);
EXTERN int Number7Pressed (void);
EXTERN int Number8Pressed (void);
EXTERN int Number9Pressed (void);
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
EXTERN int F1Pressed (void);
EXTERN int F2Pressed (void);
EXTERN int F3Pressed (void);
EXTERN int F4Pressed (void);
EXTERN int F5Pressed (void);
EXTERN int F6Pressed (void);
EXTERN int F7Pressed (void);
EXTERN int F8Pressed (void);
EXTERN int F9Pressed (void);
EXTERN int F10Pressed (void);
EXTERN int F11Pressed (void);
EXTERN int F12Pressed (void);
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

/* menu.c */
#undef EXTERN
#ifdef _menu_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int DoMenuSelection( char* InitialText , char* MenuTexts[10] , int FirstItem , char* BackgroundToUse );
EXTERN void StartupMenu (void);
EXTERN void BuySellMenu ( void );
EXTERN void HealerMenu ( void );
EXTERN void InitiateMenu( char* BackgroundToUse );
EXTERN void MissionSelectMenu (void);
EXTERN void Cheatmenu (void);
EXTERN void EscapeMenu (void);

/* misc.c */
#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void *MyMemmem ( unsigned char *haystack, size_t haystacklen, unsigned char *needle, size_t needlelen);
EXTERN char* ReadAndMallocStringFromData ( char* SearchString , char* StartIndicationString , char* EndIndicationString );
EXTERN int CountStringOccurences ( char* SearchString , char* TargetString ) ;
EXTERN void ReadValueFromString( char* SearchBeginPointer , char* ValuePreceedText , char* FormatString , void* TargetValue , char* EndOfSearchSectionPointer );
EXTERN char* ReadAndMallocAndTerminateFile( char* filename , char* File_End_String ) ;
EXTERN char* LocateStringInData ( char* SearchBeginPointer, char* SearchTextPointer ) ;
EXTERN char * find_file (char *fname, char *datadir, int use_theme);
EXTERN void CheckForTriggeredEventsAndStatements ( void );
EXTERN void Pause (void);
EXTERN void ComputeFPSForThisFrame(void);
EXTERN void StartTakingTimeForFPSCalculation(void);
EXTERN int Get_Average_FPS ( void );
EXTERN float Frame_Time (void);
EXTERN void Activate_Conservative_Frame_Computation(void);
EXTERN void DebugPrintf (int db_level, char *fmt, ...);
EXTERN void gotoxy (int, int);
EXTERN int MyRandom (int);
EXTERN void Armageddon (void);
EXTERN void Teleport (int LNum, int X, int Y);
EXTERN void SaveSettings (void);
EXTERN void LoadSettings (void);
EXTERN void InsertNewMessage (void);
EXTERN void Terminate (int);
EXTERN void *MyMalloc (long);
EXTERN void ShowDebugInfos (void);
EXTERN int GiveNumberToThisActionLabel ( char* ActionLabel );
EXTERN void ExecuteActionWithLabel ( char* ActionLabel );
EXTERN void ExecuteEvent ( int EventNumber );

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
EXTERN void EnterItemIdentificationBooth( void );
EXTERN void EnterCodepanel (void);
EXTERN void EnterKonsole (void);
EXTERN void AlleLevelsGleichFaerben (void);
EXTERN int LevelEmpty (void);
EXTERN int ShipEmpty (void);
EXTERN void ClearUserFenster (void);
void GreatDruidShow (void);
void show_droid_info (int droidtype, int page);
void ShowLifts (int level, int liftrow);
void PaintConsoleMenu (int menu_pos);

/* text.c */
#undef EXTERN
#ifdef _text_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN char* GetChatWindowInput( SDL_Surface* Background , SDL_Rect* Chat_Window_Pointer );
EXTERN void ChatWithFriendlyDroid( int Enum );
EXTERN void EnemyHitByBulletText( int Enum );
EXTERN void EnemyInfluCollisionText ( int Enum );
EXTERN void AddInfluBurntText( void );
EXTERN void AddStandingAndAimingText ( int Enum );

EXTERN void SetTextCursor (int x, int y);
EXTERN void SetLineLength (int);

EXTERN int DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* Background );
EXTERN int DisplayText (char *text, int startx, int starty, const SDL_Rect *clip);

EXTERN void DisplayChar (unsigned char c);
EXTERN int ScrollText (char *text, int startx, int starty, int EndLine , char* TitlePictureName );

EXTERN void ImprovedCheckUmbruch(char *text, const SDL_Rect *clip);
EXTERN char *PreviousLine (char *textstart, char *text);
EXTERN char *NextLine (char *text);
EXTERN char *GetString (int MaxLen, int echo);
EXTERN void printf_SDL (SDL_Surface *screen, int x, int y, char *fmt, ...);
EXTERN int putchar_SDL (SDL_Surface *Surface, int x, int y, int c);


/* rahmen.c */
#undef EXTERN
#ifdef _rahmen_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void GiveItemDescription ( char* ItemDescText , item* CurItem , int ForShop );
EXTERN void DisplayBanner (const char* left, const char* right, int flags );

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
EXTERN void Init_Highscores (void);
EXTERN void update_highscores (void);
EXTERN void Show_Highscores (void);
#endif
