/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
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

// main.c 
#undef EXTERN
#ifdef _main_c
#define EXTERN
#else
#define EXTERN extern
#endif

// init.c
#undef EXTERN
#ifdef _init_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ResetGameConfigToDefaultValues ( void );
EXTERN void ShowStartupPercentage ( int Percentage ) ;
EXTERN void AssignMission( int MissNum );
EXTERN void ParseCommandLine (int argc, char *const argv[]);
EXTERN void ClearAutomapData( void );
EXTERN void InitFreedroid (void);
EXTERN void PrepareStartOfNewCharacter ( void ) ;
EXTERN void ThouArtDefeated (void);
EXTERN void CheckIfMissionIsComplete (void);
EXTERN void GetEventsAndEventTriggers ( char* EventsAndEventTriggersFilename );
EXTERN void PlayATitleFile ( char* Filename );

// influ.c 
#undef EXTERN
#ifdef _influ_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN float vect_len ( moderately_finepoint our_vector );
EXTERN int GetLivingDroidBelowMouseCursor ( int player_num );
EXTERN int find_free_floor_items_index ( int levelnum ) ;
EXTERN int closed_chest_below_mouse_cursor ( int player_num ) ;
EXTERN int smashable_barred_below_mouse_cursor ( int player_num ) ;
EXTERN void tux_wants_to_attack_now ( int player_num ) ;
EXTERN void PerformTuxAttackRaw ( int player_num ) ;
EXTERN void CheckForJumpThresholds ( int player_num );
EXTERN int isignf (float);
EXTERN void InitInfluPositionHistory( int player_num );
EXTERN float GetInfluPositionHistoryX( int Index );
EXTERN float GetInfluPositionHistoryY( int Index );
EXTERN float GetInfluPositionHistoryZ( int Index );
EXTERN void FireTuxRangedWeaponRaw ( int player_num , int weapon_item_type , int bullet_image_type , int ForceMouseUse , int FreezeSeconds , float PoisonDuration , float PoisonDamagePerSec , float ParalysationDuration , int HitPercentage ) ;
EXTERN void MoveInfluence ( int player_num ) ;
EXTERN void InfluenceFrictionWithAir ( int player_num ) ;
EXTERN void CheckEnergieLevel (void);
EXTERN void AnimateInfluence ( int player_num ) ;
EXTERN void CheckInfluenceEnemyCollision (void);
EXTERN void ExplodeInfluencer (void);
EXTERN int translate_map_point_to_screen_pixel ( float x_map_pos , float y_map_pos , int give_x );
EXTERN int translate_map_point_to_zoomed_screen_pixel ( float x_map_pos , float y_map_pos , int give_x );
EXTERN float translate_pixel_to_map_location ( int player_num , float axis_x , float axis_y , int give_x ) ;
EXTERN float translate_pixel_to_zoomed_map_location ( int player_num , float axis_x , float axis_y , int give_x );
EXTERN void blit_zoomed_iso_image_to_map_position ( iso_image* our_iso_image , float pos_x , float pos_y );
EXTERN int tux_can_walk_this_line ( int player_num , float x1, float y1 , float x2 , float y2 );
EXTERN void clear_out_intermediate_points ( int player_num );
EXTERN void set_up_intermediate_course_for_tux ( int player_num );

// bullet.c 
#undef EXTERN
#ifdef _bullet_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void RotateVectorByAngle ( moderately_finepoint* vector , float rot_angle );
EXTERN void AnalyzePlayersMouseClick ( int player_num ) ;
EXTERN void MoveBullets (void);
EXTERN void DeleteBullet (int num , int StartBlast );
EXTERN void StartBlast ( float x , float y , int level , int type );
EXTERN void animate_blasts (void);
EXTERN void DeleteBlast (int num);
EXTERN void MoveActiveSpells (void);
EXTERN void DeleteSpell (int num);
EXTERN void clear_active_spells ( void );
EXTERN void CheckBulletCollisions (int num);
EXTERN void CheckBlastCollisions (int num);
EXTERN void enemy_spray_blood ( Enemy CurEnemy ) ;

// view.c 
#undef EXTERN
#ifdef _view_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void FillRect (SDL_Rect rect, SDL_Color color);
EXTERN void ShowPosition (void);
EXTERN void DisplayItemImageAtMouseCursor( int ItemImageCode );
EXTERN void AssembleCombatPicture (int );
EXTERN void blit_tux (int , int , int );
EXTERN void PutBullet ( int Bullet_number , int mask );
EXTERN void PutItem ( int ItemNumber , int mask , int put_thrown_items_flag , int highlight_item );
EXTERN void PutBlast (int);
EXTERN void PutEnemy (int Enum, int x , int y , int mask , int highlight );
EXTERN void PutMouseMoveCursor ( void ) ;
EXTERN void ShowRobotPicture (int PosX, int PosY, int Number);
EXTERN void ShowInventoryScreen ( void );
EXTERN int get_light_strength ( moderately_finepoint target_pos );
EXTERN void clear_all_loaded_tux_images ( int with_free );

// open_gl.c 
#undef EXTERN
#ifdef _open_gl_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int our_SDL_flip_wrapper ( SDL_Surface *screen ) ;
EXTERN int our_SDL_blit_surface_wrapper(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
EXTERN void our_SDL_update_rect_wrapper ( SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h ) ;
EXTERN int our_SDL_fill_rect_wrapper (SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);
EXTERN SDL_Surface* our_SDL_display_format_wrapper ( SDL_Surface *surface );
EXTERN SDL_Surface* our_SDL_display_format_wrapperAlpha ( SDL_Surface *surface );
EXTERN SDL_Surface* our_IMG_load_wrapper( const char *file );
EXTERN void flip_image_horizontally ( SDL_Surface* tmp1 ) ;
EXTERN void make_texture_out_of_surface ( iso_image* our_image ) ;
EXTERN SDL_Surface* pad_image_for_texture ( SDL_Surface* our_surface ) ;
EXTERN void blit_open_gl_light_radius ( void );
EXTERN void blit_open_gl_cheap_light_radius ( void ) ;
EXTERN void PutPixel_open_gl ( int x, int y, Uint32 pixel);
EXTERN void blit_rotated_open_gl_texture_with_center ( iso_image our_floor_iso_image , int x , int y , float angle_in_degree ) ;
EXTERN void remove_open_gl_blending_mode_again ( void );
EXTERN void prepare_open_gl_for_blending_textures( void );
EXTERN void GL_HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area , unsigned char r , unsigned char g , unsigned char b , unsigned char alpha );
EXTERN void ShowInventoryScreenBackground ( void );
EXTERN void show_character_screen_background ( void );
EXTERN int initialzize_our_default_open_gl_parameters ( void ) ;
EXTERN void blit_open_gl_texture_to_map_position ( iso_image our_floor_iso_image , float our_col , float our_line , float r , float g , float b , int highlight_texture ) ;
EXTERN void blit_zoomed_open_gl_texture_to_map_position ( iso_image our_floor_iso_image , float our_col , float our_line , float r , float g , float b , int highlight_texture ) ;
EXTERN void blit_open_gl_texture_to_screen_position ( iso_image our_floor_iso_image , int x , int y , int set_gl_parameters ) ;
EXTERN void blit_zoomed_open_gl_texture_to_screen_position ( iso_image* our_floor_iso_image , int x , int y , int set_gl_parameters , float zoom_factor ) ;
EXTERN void blit_special_background ( int background_code );
EXTERN void flush_background_image_cache ( void );
EXTERN void open_gl_check_error_status ( void );

// blocks.c 
#undef EXTERN
#ifdef _blocks_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void try_to_load_ingame_item_surface ( int item_type );
EXTERN void iso_load_bullet_surfaces ( void );
EXTERN void get_iso_image_from_file_and_path ( char* fpath , iso_image* our_iso_image , int use_offset_file ) ;
EXTERN void make_sure_zoomed_surface_is_there ( iso_image* our_iso_image );
EXTERN void load_item_surfaces_for_item_type ( int item_type );
EXTERN void Load_Mouse_Move_Cursor_Surfaces(void);
EXTERN void Load_Skill_Level_Button_Surfaces( void );
EXTERN void LoadOneSkillSurfaceIfNotYetLoaded ( int SkillSpellNr );
EXTERN void LoadAndPrepareEnemyRotationModelNr ( int RotationModel );
EXTERN void LoadAndPrepareRedEnemyRotationModelNr ( int RotationModel );
EXTERN void LoadAndPrepareGreenEnemyRotationModelNr ( int RotationModel );
EXTERN void LoadAndPrepareBlueEnemyRotationModelNr ( int RotationModel );
EXTERN void Load_Enemy_Surfaces (void);
EXTERN void Load_Influencer_Surfaces (void);
EXTERN void InitTuxWorkingCopy( void );
EXTERN void Load_Tux_Surfaces( void );
EXTERN void LoadOneMapInsertSurfaceIfNotYetLoaded ( int i );
EXTERN void Load_Bullet_Surfaces (void);
EXTERN void Load_Blast_Surfaces (void);
EXTERN void load_all_isometric_floor_tiles ( void );
EXTERN void load_all_obstacles ( void );
EXTERN void blit_iso_image_to_map_position ( iso_image our_iso_image , float pos_x , float pos_y );
EXTERN void blit_iso_image_to_map_position_in_buffer ( SDL_Surface *current_buffer , 
						       iso_image our_iso_image , float pos_x , float pos_y );
EXTERN int iso_image_positioned_inside_copy_rectangle ( iso_image our_iso_image , float pos_x , float pos_y , 
							float shift_x , float shift_y );
EXTERN void blit_iso_image_to_screen_position ( iso_image our_iso_image , float pos_x , float pos_y );
EXTERN void blit_outline_of_iso_image_to_map_position ( iso_image our_iso_image , float pos_x , float pos_y );

// graphics.c 
#undef EXTERN
#ifdef _graphics_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN SDL_Surface* rip_rectangle_from_alpha_image ( SDL_Surface* our_surface , SDL_Rect our_rect ) ;
EXTERN SDL_Surface* CreateAlphaCombinedSurface ( SDL_Surface* FirstSurface , SDL_Surface* SecondSurface );
EXTERN SDL_Surface* CreateColorFilteredSurface ( SDL_Surface* FirstSurface , int FilterType );
EXTERN void fade_out_using_gamma_ramp ( void );
EXTERN void fade_in_using_gamma_ramp ( void );
EXTERN void TakeScreenshot( void );
EXTERN void DisplayImage(char *file_name);
EXTERN void MakeGridOnScreen( SDL_Rect* Grid_Rectangle );
EXTERN void SetCombatScaleTo(float);
EXTERN int ReInitPictures (void);
EXTERN int InitPictures (void);
EXTERN void InitTimer (void);
EXTERN void InitVideo (void);
EXTERN void ClearGraphMem ( void );
EXTERN void SDL_HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area );
EXTERN void HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area );
EXTERN int do_graphical_number_selection_in_range ( int lower_range , int upper_range );
EXTERN Uint8 GetAlphaComponent ( SDL_Surface* surface , int x , int y );
EXTERN void swap_red_and_blue_for_open_gl ( SDL_Surface* FullView );

// saveloadgame.c 
#undef EXTERN
#ifdef _saveloadgame_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ShowSaveLoadGameProgressMeter( int Percentage , int IsSavegame ) ;
EXTERN void LoadAndShowThumbnail ( char* CoreFilename );
EXTERN int SaveGame( void );
EXTERN int LoadGame( void );
EXTERN int DeleteGame( void );
EXTERN void LoadAndShowStats ( char* CoreFilename );

// map.c 
#undef EXTERN
#ifdef _map_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void respawn_level ( int level_num );
EXTERN int position_collides_with_this_obstacle ( float x , float y , obstacle* our_obstacle );
EXTERN int position_collides_with_obstacles_on_square ( float x, float y , int x_tile , int y_tile , Level PassLevel );
EXTERN void glue_obstacles_to_floor_tiles_for_level ( int level_num );
EXTERN void ResolveMapLabelOnShip ( char* MapLabel , location* PositionPointer );
EXTERN void CollectAutomapData ( void ) ;
EXTERN void SmashBox ( float x , float y );
EXTERN int smash_obstacle ( float x , float y );
EXTERN void AnimateTeleports (void);
EXTERN Uint16 GetMapBrick (Level deck, float x, float y);
EXTERN void ActSpecialField ( int player_num ) ;

EXTERN void CountNumberOfDroidsOnShip ( void );
EXTERN int LoadShip (char *filename);
EXTERN int SaveShip(char *filename);
EXTERN void GetAllAnimatedMapTiles (Level Lev);
EXTERN int GetCrew (char *shipname);

EXTERN void AnimateCyclingMapTiles (void);
EXTERN void MoveLevelDoors ( int player_num ) ;
EXTERN void WorkLevelGuns ( int player_num ) ;
EXTERN int IsPassable ( float x , float y , int z ) ;
EXTERN int DruidPassable ( float x , float y , int z );
EXTERN int IsVisible ( GPS objpos , int player_num ) ;
EXTERN int TranslateMap (Level Lev);
EXTERN void PurifyWaypointList (level* Lev);
EXTERN void DeleteWaypoint (level *Lev, int num);
EXTERN void CreateWaypoint (level *Lev, int x, int y);

// sound.c  OR nosound.c 
#undef EXTERN
#ifdef _sound_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void LoadAllStaticModFiles( void );
EXTERN void PlayOnceNeededSoundSample( char* SoundSampleFileName , int With_Waiting , int no_double_catching );
EXTERN void InitAudio(void);
EXTERN void SetBGMusicVolume(float);
EXTERN void SetSoundFXVolume(float);
EXTERN void SwitchBackgroundMusicTo ( char * filename_raw );
EXTERN void GotHitSound (void);
EXTERN void tux_scream_sound (void);
EXTERN void No_Ammo_Sound ( void );
EXTERN void Not_Enough_Power_Sound( void );
EXTERN void Not_Enough_Dist_Sound( void );
EXTERN void Not_Enough_Mana_Sound( void );
EXTERN void CrySound (void);
EXTERN void CantCarrySound (void);
EXTERN void TransferSound (void);
EXTERN void MenuItemSelectedSound (void);
EXTERN void MenuItemDeselectedSound (void);
EXTERN void MoveMenuPositionSound (void);
EXTERN void teleport_arrival_sound (void);
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
EXTERN void play_enter_attack_run_state_sound ( int SoundCode );
EXTERN void play_death_sound_for_bot ( enemy* ThisRobot );
EXTERN void play_item_sound ( int item_type ) ;
EXTERN void PlayLevelCommentSound ( int levelnum );
EXTERN void PlayEnemyGotHitSound ( int enemytype );
EXTERN void BulletReflectedSound (void);
EXTERN void Play_Spell_ForceToEnergy_Sound( void );
EXTERN void Play_Spell_DetectItems_Sound( void );
EXTERN void play_melee_weapon_hit_something_sound ( void );
EXTERN void play_melee_weapon_missed_sound ( void );
EXTERN void play_open_chest_sound ( void );
EXTERN void play_sample_using_WAV_cache( char* SoundSampleFileName , int With_Waiting , int no_double_catching ) ;

// items.c
#undef EXTERN
#ifdef _items_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void MoveItem( item* SourceItem , item* DestItem );
EXTERN void silently_unhold_all_items ( void );
EXTERN int CountItemtypeInInventory( int Itemtype , int player_num );
EXTERN void DeleteAllInventoryItemsOfType( int Itemtype , int player_num );
EXTERN void DeleteOneInventoryItemsOfType( int Itemtype , int player_num );
EXTERN void DamageItem( item* CurItem );
EXTERN int GetFreeInventoryIndex( void );
EXTERN int ItemCanBeDroppedInInv ( int ItemType , int InvPos_x , int InvPos_y );
EXTERN long calculate_item_buy_price ( item* BuyItem );
EXTERN long calculate_item_repair_price ( item* repair_item );
EXTERN long calculate_item_sell_price ( item* BuyItem );
EXTERN void FillInItemProperties( item* ThisItem , int FullDuration , int TreasureChestRange );
EXTERN void DamageAllEquipment( int player_num ) ;
EXTERN void DropChestItemAt( int ItemType , float x , float y , int prefix , int suffix , int TreasureChestRange );
EXTERN void DropItemAt( int ItemType , float x , float y , int prefix , int suffix , int TreasureChestRange , int multiplicity );
EXTERN void Quick_ApplyItem( int ItemKey );
EXTERN void ApplyItem( item* CurItem );
EXTERN int Inv_Pos_Is_Free( int x , int y );
EXTERN int GetInventoryItemAt ( int x , int y );
EXTERN item* GetHeldItemPointer( void );
EXTERN Item FindPointerToPositionCode ( int PositionCode , int player_num ) ;
EXTERN int ItemUsageRequirementsMet( item* UseItem , int MakeSound );
EXTERN int CursorIsInInventoryGrid( int x , int y );
EXTERN int CursorIsInUserRect( int x , int y );
EXTERN int CursorIsInInvRect( int x , int y );
EXTERN int GetHeldItemCode ( void );
EXTERN int GetInventorySquare_x( int x );
EXTERN int GetInventorySquare_y( int x );
EXTERN void DropHeldItemToInventory( void );
EXTERN void DropHeldItemToTheFloor ( void );
EXTERN void DropItemToTheFloor ( Item DropItemPointer , float x , float y , int levelnum ) ;
EXTERN void ShowQuickInventory ( void );
EXTERN void ManageInventoryScreen ( void );
EXTERN void AddFloorItemDirectlyToInventory( item* ItemPointer );
EXTERN void CopyItem( item* SourceItem , item* DestItem , int MakeSound );
EXTERN void DeleteItem( item* Item );
EXTERN void DropRandomItem( float x , float y , int TreasureChestRange , int ForceMagical , int ForceDrop , int ChestItem );
EXTERN int get_floor_item_index_under_mouse_cursor ( int player_num );


// character.c
#undef EXTERN
#ifdef _character_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void InitiateNewCharacter ( int player_num , int CharacterClass );
EXTERN void DisplayButtons( void );
EXTERN void UpdateAllCharacterStats ( int player_num );
EXTERN void ShowCharacterScreen ( void );

// character.c
#undef EXTERN
#ifdef _leveleditor_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void create_new_obstacle_on_level ( Level EditLevel , int our_obstacle_type , float pos_x , float pos_y );
EXTERN void show_button_tooltip ( char* tooltip_text );
EXTERN void delete_obstacle ( level* EditLevel , obstacle* our_obstacle );
EXTERN void ExportLevelInterface ( int level_num ) ;
EXTERN void give_new_name_to_obstacle ( Level EditLevel , obstacle* our_obstacle , char* predefined_name );

// skills.c
#undef EXTERN
#ifdef _skills_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void RadialVMXWave ( gps ExpCenter , int SpellCostsMana );
EXTERN void RadialEMPWave ( gps ExpCenter , int SpellCostsMana );
EXTERN void RadialFireWave ( gps ExpCenter , int SpellCostsMana );
EXTERN void ClearDetectedItemList( int player_num );
EXTERN void ShowSkillsScreen ( void );
EXTERN void HandleCurrentlyActivatedSkill( int player_num );
EXTERN void activate_nth_aquired_skill ( int skill_num );

// input.c 
#undef EXTERN
#ifdef _input_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int MouseWheelUpPressed(void);
EXTERN int MouseWheelDownPressed(void);
EXTERN int MouseRightPressed(void);
EXTERN int MouseLeftPressed(void);
EXTERN int GetMousePos_x(void);
EXTERN int GetMousePos_y(void);
EXTERN void Init_Joy(void);
EXTERN void ReactToSpecialKeys(void);
EXTERN int Shift_Was_Pressed(void);
EXTERN int LeftShiftWasPressed(void);
EXTERN int RightShiftWasPressed(void);
EXTERN int CtrlWasPressed(void);
EXTERN int RightCtrlWasPressed(void);
EXTERN int LeftCtrlWasPressed(void);
EXTERN int Alt_Was_Pressed(void);
EXTERN int LeftAltWasPressed(void);
EXTERN int RightAltWasPressed(void);
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
EXTERN int Shift_Is_Pressed(void);
EXTERN int BackspacePressed (void);
EXTERN int LeftCtrlPressed (void);
EXTERN int KP_PLUS_Pressed (void);
EXTERN int KP_MINUS_Pressed (void);
EXTERN int KP_MULTIPLY_Pressed (void);
EXTERN int KP_DIVIDE_Pressed (void);
EXTERN int KP_ENTER_Pressed (void);
EXTERN int Number0Pressed (void); EXTERN int Number1Pressed (void); EXTERN int Number2Pressed (void);
EXTERN int Number3Pressed (void); EXTERN int Number4Pressed (void); EXTERN int Number5Pressed (void);
EXTERN int Number6Pressed (void); EXTERN int Number7Pressed (void); EXTERN int Number8Pressed (void);
EXTERN int Number9Pressed (void); EXTERN int KP0Pressed (void); EXTERN int KP1Pressed (void);
EXTERN int KP2Pressed (void); EXTERN int KP3Pressed (void); EXTERN int KP4Pressed (void);
EXTERN int KP5Pressed (void); EXTERN int KP6Pressed (void); EXTERN int KP7Pressed (void);
EXTERN int KP8Pressed (void); EXTERN int KP9Pressed (void); EXTERN int F1Pressed (void); 
EXTERN int F2Pressed (void); EXTERN int F3Pressed (void); EXTERN int F4Pressed (void);
EXTERN int F5Pressed (void); EXTERN int F6Pressed (void); EXTERN int F7Pressed (void);
EXTERN int F8Pressed (void); EXTERN int F9Pressed (void); EXTERN int F10Pressed (void);
EXTERN int F11Pressed (void); EXTERN int F12Pressed (void); EXTERN int APressed (void);
EXTERN int BPressed (void); EXTERN int CPressed (void); EXTERN int DPressed (void);
EXTERN int EPressed (void); EXTERN int FPressed (void); EXTERN int GPressed (void);
EXTERN int HPressed (void); EXTERN int IPressed (void); EXTERN int JPressed (void);
EXTERN int KPressed (void); EXTERN int LPressed (void); EXTERN int MPressed (void);
EXTERN int NPressed (void); EXTERN int OPressed (void); EXTERN int PPressed (void);
EXTERN int QPressed (void); EXTERN int RPressed (void); EXTERN int SPressed (void);
EXTERN int TPressed (void); EXTERN int UPressed (void); EXTERN int VPressed (void);
EXTERN int WPressed (void); EXTERN int XPressed (void); EXTERN int YPressed (void);
EXTERN int ZPressed (void);
EXTERN int NoDirectionPressed (void);

// menu.c 
#undef EXTERN
#ifdef _menu_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void StoreMenuBackground ( int backup_slot );
EXTERN void RestoreMenuBackground ( int backup_slot );
EXTERN int DoMenuSelection( char* InitialText , char* MenuTexts[10] , int FirstItem , int background_code , void* MenuFont );
EXTERN int ChatDoMenuSelectionFlagged( char* InitialText , char* MenuTexts[ MAX_ANSWERS_PER_PERSON] , 
				       unsigned char Chat_Flags[ MAX_ANSWERS_PER_PERSON ] , int FirstItem , 
				       int background_code , void* MenuFont , enemy* ChatDroid );
EXTERN int ChatDoMenuSelection( char* InitialText , char* MenuTexts[10] , int FirstItem , int background_code , void* MenuFont , enemy* ChatDroid );
EXTERN void StartupMenu (void);
EXTERN void BuySellMenu ( void );
EXTERN void InitiateMenu( int background_code );
EXTERN void Cheatmenu (void);
EXTERN void EscapeMenu (void);
EXTERN void Credits_Menu (void);
EXTERN int GetNumberOfTextLinesNeeded ( char* GivenText, SDL_Rect GivenRectangle , float text_stretch );

// misc.c 
#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void GiveStandardErrorMessage ( char* FunctionName , char* ProblemDescription, int InformDevelopers , int IsFatal );
EXTERN void UpdateScreenOverButtonFromList ( int ButtonIndex );
EXTERN void ShowGenericButtonFromList ( int ButtonIndex );
EXTERN int CursorIsOnButton( int ButtonIndex , int x , int y );
EXTERN void *MyMemmem ( unsigned char *haystack, size_t haystacklen, unsigned char *needle, size_t needlelen);
EXTERN char* ReadAndMallocStringFromData ( char* SearchString , char* StartIndicationString , char* EndIndicationString );
EXTERN int CountStringOccurences ( char* SearchString , char* TargetString ) ;
EXTERN void ReadValueFromString( char* SearchBeginPointer , char* ValuePreceedText , char* FormatString , void* TargetValue , char* EndOfSearchSectionPointer );
EXTERN char* ReadAndMallocAndTerminateFile( char* filename , char* File_End_String ) ;
EXTERN char* LocateStringInData ( char* SearchBeginPointer, char* SearchTextPointer ) ;
EXTERN char * find_file (char *fname, char *datadir, int use_theme);
EXTERN char * find_file_for_callbacks (char *fname, char *datadir, int use_theme);
EXTERN void CheckForTriggeredEventsAndStatements ( int player_num );
EXTERN void Pause (void);
EXTERN void ComputeFPSForThisFrame(void);
EXTERN void StartTakingTimeForFPSCalculation(void);
EXTERN int Get_Average_FPS ( void );
EXTERN float Frame_Time (void);
EXTERN void Activate_Conservative_Frame_Computation(void);
EXTERN void gotoxy (int, int);
EXTERN int MyRandom (int);
EXTERN void Armageddon (void);
EXTERN void Teleport ( int LNum , float X , float Y , int player_num , int Shuffling , int WithSound ) ;
EXTERN int SaveGameConfig (void);
EXTERN int LoadGameConfig (void);
EXTERN void InsertNewMessage (void);
EXTERN void Terminate (int);
EXTERN void ShowDebugInfos (void);
EXTERN int GiveNumberToThisActionLabel ( char* ActionLabel );
EXTERN void ExecuteActionWithLabel ( char* ActionLabel , int player_num ) ;
EXTERN void ExecuteEvent ( int EventNumber , int player_num );
EXTERN char *va (char *format, ...);

// enemy.c 
#undef EXTERN
#ifdef _enemy_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void robot_group_turn_hostile ( int enemy_num );
EXTERN void SetRestOfGroupToState ( Enemy ThisRobot , int NewState );
EXTERN int MakeSureEnemyIsInsideThisLevel ( Enemy ThisRobot );
EXTERN void Enemy_Post_Bullethit_Behaviour( int EnemyNum );
EXTERN void ShuffleEnemys ( int LevelNum );
EXTERN int CheckEnemyEnemyCollision (int enemynum);
EXTERN void MoveEnemys (void);
EXTERN void AttackInfluence (int enemynum);
EXTERN void ClearEnemys (void);
EXTERN int DirectLineWalkable( float x1 , float y1 , float x2 , float y2 , int z );
EXTERN int CheckIfWayIsFreeOfDroidsWithTuxchecking ( float x1 , float y1 , float x2 , float y2 , int OurLevel , Enemy ExceptedRobot ) ;
EXTERN int CheckIfWayIsFreeOfDroidsWithoutTuxchecking ( float x1 , float y1 , float x2 , float y2 , int OurLevel , Enemy ExceptedRobot ) ;
EXTERN void start_gethit_animation_if_applicable ( enemy* ThisRobot ) ;
EXTERN int find_free_bullet_index ( void ) ;
EXTERN int IsActiveLevel ( int levelnum ) ;
EXTERN void AnimateEnemys ( void ) ;
EXTERN void occasionally_update_first_and_last_bot_indices ( void );

// ship.c 
#undef EXTERN
#ifdef _ship_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int GreatItemShow ( int NumberOfItems , item* ShowPointerList[ MAX_ITEMS_IN_INVENTORY ] );
EXTERN void ShowDeckMap (Level deck);
EXTERN void ShowItemInfo ( item* ShowItem , int page , char ShowArrows , int background_code , int title_text_flag );
EXTERN void EnterLift (void);
EXTERN void EnterItemIdentificationBooth( void );
EXTERN void EnterCodepanel (void);
EXTERN void EnterConsole (void);
EXTERN void AlleLevelsGleichFaerben (void);
EXTERN void ClearUserFenster (void);
void GreatDruidShow (void);
void ShowDroidInfo (int droidtype, int page , char ShowArrows );
void PaintConsoleMenu (int menu_pos);
EXTERN void write_full_item_name_into_string ( item* ShowItem , char* full_item_name ) ;

// text.c 
#undef EXTERN
#ifdef _text_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void show_backgrounded_label_at_map_position ( char* LabelText , float fill_status , float pos_x , float pos_y , int zoom_is_on );
EXTERN char * GetEditableStringInPopupWindow ( int MaxLen , char* PopupWindowTitle , char* DefaultString );
EXTERN void GiveMouseAlertWindow ( char* WindowText ) ;
EXTERN void CutDownStringToMaximalSize ( char* StringToCut , int LengthInPixels );
EXTERN void SetNewBigScreenMessage( char* ScreenMessageText );
EXTERN void DisplayBigScreenMessage( void );
EXTERN char* GetChatWindowInput( SDL_Surface* Background , SDL_Rect* Chat_Window_Pointer );
EXTERN void ChatWithFriendlyDroid( Enemy ChatDroid );
EXTERN void EnemyHitByBulletText( int Enum );
EXTERN void EnemyInfluCollisionText ( int Enum );
EXTERN void AddInfluBurntText( void );
EXTERN void AddStandingAndAimingText ( int Enum );

EXTERN void SetTextCursor (int x, int y);
EXTERN void SetLineLength (int);

EXTERN int DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* Background );
EXTERN int DisplayText (char *text, int startx, int starty, const SDL_Rect *clip);
EXTERN void display_current_chat_protocol ( int background_picture_code , enemy* ChatDroid , int with_update );

EXTERN void DisplayChar (unsigned char c);
EXTERN int ScrollText (char *text, int startx, int starty, int EndLine , int background_code );

EXTERN void ImprovedCheckLineBreak(char *text, const SDL_Rect *clip);
EXTERN char *PreviousLine (char *textstart, char *text);
EXTERN char *NextLine (char *text);
EXTERN char *GetString ( int max_len , int echo , int background_code , char* text_for_overhead_promt ) ;
EXTERN void printf_SDL (SDL_Surface *screen, int x, int y, char *fmt, ...);
EXTERN int putchar_SDL (SDL_Surface *Surface, int x, int y, int c);

// text.c 
#undef EXTERN
#ifdef _text_public_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void LoadChatRosterWithChatSequence ( char* FullPathAndFullFilename );
EXTERN void delete_one_dialog_option ( int i , int FirstInitialisation );
EXTERN void InitChatRosterForNewDialogue( void );
EXTERN void Get_Item_Data ( char* DataPointer );
EXTERN void DebugPrintf (int db_level, char *fmt, ...);
EXTERN void *MyMalloc (long);
EXTERN void PrepareMultipleChoiceDialog ( Enemy ChatDroid , int with_flip );
EXTERN int FS_filelength (FILE *f);

// rahmen.c 
#undef EXTERN
#ifdef _rahmen_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void GiveItemDescription ( char* ItemDescText , item* CurItem , int ForShop );
EXTERN void DisplayBanner ( void );

// shop.c 
#undef EXTERN
#ifdef _shop_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ShowRescaledItem ( int position , int TuxItemRow , item* ShowItem );
EXTERN int TryToIntegrateItemIntoInventory ( item* BuyItem , int AmountToBuyAtMost );
EXTERN int AssemblePointerListForChestShow ( item** ItemPointerListPointer , int player_num );
EXTERN int AssemblePointerListForItemShow ( item** ItemPointerListPointer , int IncludeWornItems, int player_num );
EXTERN void InitTradeWithCharacter( int CharacterCode ) ;
EXTERN void Sell_Items( int ForHealer );
EXTERN void Repair_Items( void );
EXTERN void Identify_Items ( void );
EXTERN void EnterChest ( moderately_finepoint pos );

// network.c 
#undef EXTERN
#ifdef _network_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void Init_Network ( void ) ;
EXTERN int ServerThinksInputAxisX ( int player_num ) ;
EXTERN int ServerThinksInputAxisY ( int player_num ) ;
EXTERN int ServerThinksAxisIsActive ( int player_num ) ;
EXTERN int ServerThinksShiftWasPressed ( int player_num ) ;
EXTERN int ServerThinksRightPressed ( int player_num ) ;
EXTERN int ServerThinksLeftPressed ( int player_num ) ;
EXTERN int ServerThinksUpPressed ( int player_num ) ;
EXTERN int ServerThinksDownPressed ( int player_num ) ;
EXTERN int ServerThinksSpacePressed ( int player_num ) ;
EXTERN int ServerThinksNoDirectionPressed ( int player_num ) ;
EXTERN void OpenTheServerSocket ( void ) ;
EXTERN void AcceptConnectionsFromClients ( void ) ;
EXTERN void ConnectToFreedroidServer ( void );
EXTERN void Send1024MessageToServer ( char message[1024] );
EXTERN void ListenToAllRemoteClients ( void );
EXTERN void DisconnectAllRemoteClinets ( void );
EXTERN void ServerSendMessageToAllClients ( char ServerMessage[1024] );
EXTERN void ListenForServerMessages ( void ) ;
EXTERN void SendTextMessageToServer ( char* message );
EXTERN void SendPlayerKeyboardEventToServer ( SDL_Event event ) ;
EXTERN void SendPlayerMouseButtonEventToServer ( SDL_Event event ) ;
EXTERN void SendPlayerItemDropToServer ( int PositionCode , float x , float y ) ;
EXTERN void SendPlayerItemMoveToServer ( int SourcePositionCode , int DestPositionCode , int inv_x , int inv_y ) ;
EXTERN void PrintServerStatusInformation ( void ) ;
EXTERN void SendPeriodicServerMessagesToAllClients ( void );

// takeover.c 
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


// BFont.c
#undef EXTERN
#ifdef _bfont_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN Uint32 GetPixel (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN void PutPixel (SDL_Surface * surface, int x, int y, Uint32 pixel);

//--------------------
// Leave this final endif in here!  It's the wrapper of the whole
// file to prevent double-definitions.
//
#endif
