/* 
 *
 *   Copyright (c) 1994, 2002, 2003 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
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

// automap.c
#undef EXTERN
#ifdef _automap_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void set_up_texture_for_automap ( void );
EXTERN void show_automap_data_ogl ( float scale_factor );
EXTERN void show_automap_data_sdl ( void );
EXTERN void insert_old_map_info_into_texture ( void );
EXTERN void full_update_of_automap_texture ( void );

// init.c
#undef EXTERN
#ifdef _init_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ResetGameConfigToDefaultValues ( void );
EXTERN void ShowStartupPercentage ( int Percentage ) ;
EXTERN void ParseCommandLine ( int argc , char *const argv[] );
EXTERN void ClearAutomapData( void );
EXTERN void InitFreedroid ( void );
EXTERN void PrepareStartOfNewCharacter ( void ) ;
EXTERN void ThouArtDefeated ( void );
EXTERN void ThouHastWon ( void );
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
EXTERN int GetObstacleBelowMouseCursor ( int player_num );
EXTERN int find_free_floor_items_index ( int levelnum ) ;
EXTERN int closed_chest_below_mouse_cursor ( int player_num ) ;
EXTERN int smashable_barrel_below_mouse_cursor ( int player_num ) ;
EXTERN void tux_wants_to_attack_now ( int player_num , int use_mouse_cursor_for_targeting ) ;
EXTERN void PerformTuxAttackRaw ( int player_num , int use_mouse_cursor_for_targeting ) ;
EXTERN void TuxReloadWeapon ( void ) ;
EXTERN void correct_tux_position_according_to_jump_thresholds ( int player_num );
EXTERN int isignf (float);
EXTERN void InitInfluPositionHistory( int player_num );
EXTERN float GetInfluPositionHistoryX( int Index );
EXTERN float GetInfluPositionHistoryY( int Index );
EXTERN float GetInfluPositionHistoryZ( int Index );
EXTERN void FillInDefaultBulletStruct ( int player_num, bullet * CurBullet, int bullet_image_type, int weapon_item_type );
EXTERN void FireTuxRangedWeaponRaw ( int player_num , int weapon_item_type , int bullet_image_type , bullet *, moderately_finepoint target_location ) ;
EXTERN void move_tux ( int player_num ) ;
EXTERN void animate_tux ( int player_num ) ;
EXTERN void check_tux_enemy_collision (void);
EXTERN void start_tux_death_explosions (void);
#define translate_map_point_to_screen_pixel translate_map_point_to_screen_pixel_macro
#define translate_map_point_to_screen_pixel_macro(X,Y,x_res,y_res,zoom_factor) {\
  if((x_res)!=NULL)\
    (*(x_res)) = rintf ((UserCenter_x) + \
      ( (X) + Me[0].pos.y - Me[0].pos.x - (Y) ) * iso_floor_tile_width*0.5*(zoom_factor)) ;\
  if((y_res)!=NULL)\
    (*(y_res)) = rintf ((UserCenter_y) + \
      ( (X) + (Y) - Me[0].pos.x - Me[0].pos.y ) * iso_floor_tile_height*0.5*(zoom_factor)) ;\
}
#define translate_map_point_to_screen_pixel_x(X,Y)  ( UserCenter_x + rintf(( (X) + Me [ 0 ] . pos . y - Me [ 0 ] . pos . x - (Y) ) * (iso_floor_tile_width_over_two) ) )
#define translate_map_point_to_screen_pixel_y(X,Y)  ( UserCenter_y + rintf(( (X) + (Y) - Me [ 0 ] . pos . x - Me [ 0 ] . pos . y ) * (iso_floor_tile_height_over_two))) 
EXTERN void translate_map_point_to_screen_pixel_func( float x_map_pos, float y_map_pos, int * x_res, int * y_res, float zoom_factor);
EXTERN int translate_map_point_to_screen_pixel_deviation_tracking ( float x_map_pos , float y_map_pos , int give_x );
//EXTERN void translate_map_point_to_zoomed_screen_pixel ( float x_map_pos , float y_map_pos , int* x_res, int* y_res);
EXTERN inline float translate_pixel_to_map_location ( int player_num , float axis_x , float axis_y , int give_x ) ;
EXTERN void skew_and_blit_rect( float x1, float y1, float x2, float y2, Uint32 color);
EXTERN float translate_pixel_to_zoomed_map_location ( int player_num , float axis_x , float axis_y , int give_x );
EXTERN void blit_zoomed_iso_image_to_map_position ( iso_image* our_iso_image , float pos_x , float pos_y );
EXTERN int tux_can_walk_this_line ( int player_num , float x1, float y1 , float x2 , float y2 );
EXTERN void clear_out_intermediate_points ( int player_num );
EXTERN int set_up_intermediate_course_for_tux ( int player_num );
EXTERN void adapt_position_for_jump_thresholds ( gps* old_position, gps* new_position );

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
EXTERN void StartBlast ( float x , float y , int level , int type, int dmg );
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
EXTERN void update_virtual_position ( gps* target_pos , gps* source_pos , int level_num );
EXTERN void FdFillRect (SDL_Rect rect, SDL_Color color);
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
EXTERN void clear_all_loaded_tux_images ( int with_free );
EXTERN int set_rotation_index_for_this_robot ( enemy* ThisRobot );
EXTERN int set_rotation_model_for_this_robot ( enemy* ThisRobot );
EXTERN void grab_enemy_images_from_archive ( int enemy_model_nr );
EXTERN int level_is_partly_visible ( int level_num );

// light.c 
#undef EXTERN
#ifdef _light_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int get_light_strength ( moderately_finepoint target_pos );
EXTERN void update_light_list ( int player_num );
EXTERN void blit_light_radius ( void );

// open_gl.c 
#undef EXTERN
#ifdef _open_gl_c
#define EXTERN
#else
#define EXTERN extern
#endif

typedef struct {
		Uint8 r;
		Uint8 g;
		Uint8 b;
		Uint8 a;
} myColor ;

enum {
		Z_DIR,
		X_DIR,
		Y_DIR
};

EXTERN int our_SDL_flip_wrapper ( SDL_Surface *screen ) ;
EXTERN int our_SDL_blit_surface_wrapper(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
EXTERN void our_SDL_update_rect_wrapper ( SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h ) ;
EXTERN int our_SDL_fill_rect_wrapper (SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);
EXTERN int blit_quad ( int x1 , int y1 , int x2, int y2, int x3, int y3, int x4 , int y4 , Uint32 color );
EXTERN void drawIsoEnergyBar(int dir, int x, int y, int z, int w, int d, int length, float fill, myColor *c1, myColor *c2  ) ;

EXTERN SDL_Surface* our_SDL_display_format_wrapper ( SDL_Surface *surface );
EXTERN SDL_Surface* our_SDL_display_format_wrapperAlpha ( SDL_Surface *surface );
EXTERN SDL_Surface* our_IMG_load_wrapper( const char *file );
EXTERN void flip_image_horizontally ( SDL_Surface* tmp1 ) ;
EXTERN void make_texture_out_of_surface ( iso_image* our_image ) ;
EXTERN void make_texture_out_of_prepadded_image ( iso_image* our_image ) ;
EXTERN SDL_Surface* pad_image_for_texture ( SDL_Surface* our_surface ) ;
EXTERN void clear_screen (void) ;
EXTERN void blit_open_gl_light_radius ( void );
EXTERN void blit_open_gl_stretched_texture_light_radius ( void ) ;
EXTERN void PutPixel_open_gl ( int x, int y, Uint32 pixel);
EXTERN void blit_rotated_open_gl_texture_with_center ( iso_image * our_floor_iso_image , int x , int y , float angle_in_degree ) ;
EXTERN void remove_open_gl_blending_mode_again ( void );
EXTERN void prepare_open_gl_for_blending_textures( void );
EXTERN void GL_HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area , unsigned char r , unsigned char g , unsigned char b , unsigned char alpha );
EXTERN void ShowInventoryScreenBackground ( void );
EXTERN void show_character_screen_background ( void );
EXTERN int safely_initialize_our_default_open_gl_parameters ( void ) ;
EXTERN void blit_open_gl_texture_to_map_position ( iso_image * our_floor_iso_image , float our_col , float our_line , double r , double g , double b , int highlight_texture, int blend ) ;
EXTERN void blit_zoomed_open_gl_texture_to_map_position ( iso_image * our_floor_iso_image , float our_col , float our_line , float r , float g , float b , int highlight_texture, int blend ) ;
EXTERN void blit_open_gl_texture_to_screen_position ( iso_image * our_floor_iso_image , int x , int y , int set_gl_parameters ) ;
EXTERN void blit_semitransparent_open_gl_texture_to_screen_position ( iso_image * our_floor_iso_image , int x , int y , float scale_factor ) ;
EXTERN void blit_zoomed_open_gl_texture_to_screen_position ( iso_image * our_floor_iso_image , int x , int y , int set_gl_parameters , float zoom_factor ) ;
EXTERN void blit_special_background ( int background_code );
EXTERN void flush_background_image_cache ( void );
EXTERN void open_gl_check_error_status ( const char* name_of_calling_function );
EXTERN void blit_open_gl_texture_to_full_screen ( iso_image * our_floor_iso_image , int x , int y , int set_gl_parameters ) ;

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
EXTERN void make_sure_automap_surface_is_there ( obstacle_spec* our_obstacle_spec );
EXTERN void load_item_surfaces_for_item_type ( int item_type );
EXTERN void Load_Mouse_Move_Cursor_Surfaces(void);
EXTERN void Load_Skill_Level_Button_Surfaces( void );
EXTERN void LoadOneSkillSurfaceIfNotYetLoaded ( int SkillSpellNr );
EXTERN void LoadAndPrepareEnemyRotationModelNr ( int RotationModel );
EXTERN void LoadAndPrepareRedEnemyRotationModelNr ( int RotationModel );
EXTERN void LoadAndPrepareGreenEnemyRotationModelNr ( int RotationModel );
EXTERN void LoadAndPrepareBlueEnemyRotationModelNr ( int RotationModel );
EXTERN void Load_Enemy_Surfaces (void);
EXTERN void InitTuxWorkingCopy( void );
EXTERN void Load_Tux_Surfaces( void );
EXTERN void LoadOneMapInsertSurfaceIfNotYetLoaded ( int i );
EXTERN void Load_Bullet_Surfaces (void);
EXTERN void Load_Blast_Surfaces (void);
EXTERN void load_all_isometric_floor_tiles ( void );
EXTERN void load_all_obstacles ( void );
EXTERN void blit_iso_image_to_map_position ( iso_image * our_iso_image , float pos_x , float pos_y );
EXTERN void blit_iso_image_to_map_position_in_buffer ( SDL_Surface *current_buffer , 
						       iso_image * our_iso_image , float pos_x , float pos_y );
EXTERN int iso_image_positioned_inside_copy_rectangle ( iso_image * our_iso_image , float pos_x , float pos_y , 
							float shift_x , float shift_y );
EXTERN void blit_iso_image_to_screen_position ( iso_image * our_iso_image , float pos_x , float pos_y );
EXTERN void blit_outline_of_iso_image_to_map_position ( iso_image * our_iso_image , float pos_x , float pos_y );

// graphics.c 
#undef EXTERN
#ifdef _graphics_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void set_mouse_cursor_to_shape ( int given_shape );
EXTERN void make_sure_system_mouse_cursor_is_turned_off ( void );
EXTERN void make_sure_system_mouse_cursor_is_turned_on ( void );
EXTERN void blit_our_own_mouse_cursor ( void );
EXTERN void blit_mouse_cursor_corona ( void );
EXTERN SDL_Surface* rip_rectangle_from_alpha_image ( SDL_Surface* our_surface , SDL_Rect our_rect ) ;
EXTERN SDL_Surface* CreateAlphaCombinedSurface ( SDL_Surface* FirstSurface , SDL_Surface* SecondSurface );
EXTERN SDL_Surface* CreateColorFilteredSurface ( SDL_Surface* FirstSurface , int FilterType );
EXTERN void fade_out_using_gamma_ramp ( void );
EXTERN void fade_in_using_gamma_ramp ( void );
EXTERN void TakeScreenshot( void );
EXTERN void DisplayImage(char *file_name);
EXTERN void MakeGridOnScreen( SDL_Rect* Grid_Rectangle );
EXTERN void SetCombatScaleTo(float);
EXTERN void InitPictures (void);
EXTERN void InitTimer (void);
EXTERN void InitVideo (void);
EXTERN void InitOurBFonts (void);
EXTERN void FreeOurBFonts (void);
EXTERN void ClearGraphMem ( void );
EXTERN void SDL_HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area );
EXTERN void HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area );
EXTERN void ShadowingRectangle ( SDL_Surface* Surface , SDL_Rect Area );
EXTERN int do_graphical_number_selection_in_range ( int lower_range , int upper_range, int default_value );
EXTERN Uint8 GetAlphaComponent ( SDL_Surface* surface , int x , int y );
EXTERN void swap_red_and_blue_for_open_gl ( SDL_Surface* FullView );
EXTERN void clear_automap_texture_completely ( void ) ;

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

// mission.c 
#undef EXTERN
#ifdef _mission_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void classic_show_mission_list ( void );
EXTERN void quest_browser_interface ( void );
EXTERN void AssignMission( int MissNum );
EXTERN void GetQuestList ( char* QuestListFilename ) ;
EXTERN void clear_tux_mission_info ( int player_num ) ;
EXTERN void CheckIfMissionIsComplete ( void );
EXTERN void quest_browser_enable_new_diary_entry ( int mis_num , int mis_diary_entry_num , int player_num );

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
EXTERN int smash_obstacle ( float x , float y , int player_num );
EXTERN void AnimateTeleports (void);
EXTERN Uint16 GetMapBrick (Level deck, float x, float y);
EXTERN void ActSpecialField ( int player_num ) ;

EXTERN void CountNumberOfDroidsOnShip ( void );
EXTERN int LoadShip (char *filename);
EXTERN int SaveShip(const char *filename);
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
EXTERN void healing_spell_sound ( void );
EXTERN void application_requirements_not_met_sound ( void );
EXTERN void Fire_Bullet_Sound (int);
EXTERN void Mission_Status_Change_Sound (void);
EXTERN void BounceSound (void);
EXTERN void DruidBlastSound (void);
EXTERN void ExterminatorBlastSound (void);
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
EXTERN void play_sample_using_WAV_cache_v( char* SoundSampleFileName , int With_Waiting , int no_double_catching ,double volume) ;
// items.c
#undef EXTERN
#ifdef _items_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void handle_player_identification_command ( int player_num );
EXTERN void MoveItem( item* SourceItem , item* DestItem );
EXTERN void silently_unhold_all_items ( void );
EXTERN int required_spellcasting_skill_for_item ( int item_type );
EXTERN int required_magic_stat_for_next_level_and_item ( int item_type );
EXTERN int CountItemtypeInInventory( int Itemtype , int player_num );
EXTERN void DeleteInventoryItemsOfType( int Itemtype , int amount, int player_num );
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
EXTERN void DropItemAt( int ItemType , int level_num , float x , float y , int prefix , int suffix , int TreasureChestRange , int multiplicity );
EXTERN void Quick_ApplyItem( int ItemKey );
EXTERN void ApplyItem( item* CurItem );
EXTERN int Inv_Pos_Is_Free( int x , int y );
EXTERN int GetInventoryItemAt ( int x , int y );
EXTERN item* GetHeldItemPointer( void );
EXTERN Item FindPointerToPositionCode ( int PositionCode , int player_num ) ;
EXTERN int ItemUsageRequirementsMet( item* UseItem , int MakeSound );
EXTERN int MouseCursorIsInInventoryGrid( int x , int y );
EXTERN int MouseCursorIsInUserRect( int x , int y );
EXTERN int MouseCursorIsInInvRect( int x , int y );
EXTERN int MouseCursorIsInChaRect( int x , int y );
EXTERN int MouseCursorIsInSkiRect( int x , int y );
EXTERN int GetHeldItemCode ( void );
EXTERN int GetInventorySquare_x( int x );
EXTERN int GetInventorySquare_y( int x );
EXTERN void DropHeldItemToInventory( void );
EXTERN int DropHeldItemToTheFloor ( void );
EXTERN void DropItemToTheFloor ( Item DropItemPointer , float x , float y , int levelnum ) ;
EXTERN void ShowQuickInventory ( void );
EXTERN void HandleInventoryScreen ( void );
EXTERN int AddFloorItemDirectlyToInventory( item* ItemPointer );
EXTERN void CopyItem( item* SourceItem , item* DestItem , int MakeSound );
EXTERN void DeleteItem( item* Item );
EXTERN void DropRandomItem( int level_num , float x , float y , int TreasureChestRange , int ForceMagical , int ForceDrop , int ChestItem );
EXTERN int get_floor_item_index_under_mouse_cursor ( int player_num );
EXTERN int item_is_currently_equipped( item* Item );
EXTERN int Get_Prefixes_Data ( char * DataPointer );

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
EXTERN void ShowCharacterScreen ( int player_num );
EXTERN void HandleCharacterScreen ( int player_num );

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
EXTERN void ShowSkillsScreen ( void );
EXTERN void HandleCurrentlyActivatedSkill();
EXTERN int DoSkill( int skill_index, int SpellCost);
EXTERN void activate_nth_aquired_skill ( int skill_num );
EXTERN void ImproveSkill(int *skill);
EXTERN int calculate_program_heat_cost(int program_id);
EXTERN int calculate_program_hit_damage(int program_id);
EXTERN int get_program_index_with_name(const char *);
// input.c 
#undef EXTERN
#ifdef _input_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN Uint8 * key_pressed_real_frame;
EXTERN Uint8 * key_pressed_last_frame;
EXTERN Uint8 mouse_state_last_frame;
EXTERN int keyboard_update();
EXTERN void track_last_frame_input_status();
EXTERN void init_keyboard_input_array();

EXTERN int MouseWheelUpPressed(void);
EXTERN int MouseWheelDownPressed(void);
EXTERN int GetMousePos_x(void);
EXTERN int GetMousePos_y(void);
EXTERN int Shift_Was_Pressed(void);
EXTERN int LeftShiftWasPressed(void);
EXTERN int RightShiftWasPressed(void);
EXTERN int CtrlWasPressed(void);
EXTERN int RightCtrlWasPressed(void);
EXTERN int LeftCtrlWasPressed(void);
EXTERN int Alt_Was_Pressed(void);
EXTERN int LeftAltWasPressed(void);
EXTERN int RightAltWasPressed(void);
EXTERN void Init_Joy(void);
EXTERN void ReactToSpecialKeys(void);
EXTERN void Init_SDL_Keyboard(void);
EXTERN int getchar_raw (void);
EXTERN void ClearKbState (void);
EXTERN int MouseRightPressed(); 
EXTERN int MouseLeftPressed();
EXTERN int MouseLeftClicked();
EXTERN int MouseRightClicked();
EXTERN int LeftPressed () ;
EXTERN int RightPressed () ;
EXTERN int LeftWasPressed () ;
EXTERN int RightWasPressed () ;
EXTERN int UpPressed () ;
EXTERN int DownPressed () ;
EXTERN int SpacePressed () ;
EXTERN int EnterPressed () ;
EXTERN int EscapePressed () ;
EXTERN int TabPressed () ;
EXTERN int BackspacePressed () ;
EXTERN int LeftCtrlPressed () ;
EXTERN int CtrlWasPressed () ;
EXTERN int CtrlPressed () ;
EXTERN int ShiftPressed () ;
EXTERN int ShiftWasPressed () ;
EXTERN int AltWasPressed () ;
EXTERN int AltPressed () ;
EXTERN int KP_PLUS_Pressed () ;
EXTERN int KP_MINUS_Pressed () ;
EXTERN int KP_MULTIPLY_Pressed () ;
EXTERN int KP_DIVIDE_Pressed () ;
EXTERN int Number0Pressed () ;
EXTERN int Number0Hit () ;
EXTERN int Number1Pressed () ;
EXTERN int Number1Hit () ;
EXTERN int Number2Pressed () ;
EXTERN int Number2Hit () ;
EXTERN int Number3Pressed () ;
EXTERN int Number3Hit () ;
EXTERN int Number4Pressed () ;
EXTERN int Number4Hit () ;
EXTERN int Number5Pressed () ;
EXTERN int Number5Hit () ;
EXTERN int Number6Pressed () ;
EXTERN int Number6Hit () ;
EXTERN int Number7Pressed () ;
EXTERN int Number7Hit () ;
EXTERN int Number8Pressed () ;
EXTERN int Number8Hit () ;
EXTERN int Number9Pressed () ;
EXTERN int Number9Hit () ;
EXTERN int KP0Pressed () ;
EXTERN int KP1Pressed () ;
EXTERN int KP2Pressed () ;
EXTERN int KP3Pressed () ;
EXTERN int KP4Pressed () ;
EXTERN int KP5Pressed () ;
EXTERN int KP6Pressed () ;
EXTERN int KP7Pressed () ;
EXTERN int KP8Pressed () ;
EXTERN int KP9Pressed () ;
EXTERN int F1Pressed () ;
EXTERN int F2Pressed () ;
EXTERN int F3Pressed () ;
EXTERN int F4Pressed () ;
EXTERN int F5Pressed () ;
EXTERN int F6Pressed () ;
EXTERN int F7Pressed () ;
EXTERN int F8Pressed () ;
EXTERN int F9Pressed () ;
EXTERN int F10Pressed () ;
EXTERN int F11Pressed () ;
EXTERN int F12Pressed () ;
EXTERN int APressed () ;
EXTERN int BPressed () ;
EXTERN int CPressed () ;
EXTERN int DPressed () ;
EXTERN int EPressed () ;
EXTERN int FPressed () ;
EXTERN int GPressed () ;
EXTERN int HPressed () ;
EXTERN int IPressed () ;
EXTERN int JPressed () ;
EXTERN int KPressed () ;
EXTERN int LPressed () ;
EXTERN int LHit () ;
EXTERN int MPressed () ;
EXTERN int NPressed () ;
EXTERN int OPressed () ;
EXTERN int PPressed () ;
EXTERN int QPressed () ;
EXTERN int RPressed () ;
EXTERN int SPressed () ;
EXTERN int TPressed () ;
EXTERN int THit () ;
EXTERN int UPressed () ;
EXTERN int VPressed () ;
EXTERN int WPressed () ;
EXTERN int XPressed () ;
EXTERN int YPressed () ;
EXTERN int ZWasPressed () ;
EXTERN int ZHit () ;
EXTERN void toggle_game_config_screen_visibility ( int screen_visible );

// menu.c 
#undef EXTERN
#ifdef _menu_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int ResolveDialogSectionToChatFlagsIndex ( char* SectionName );
EXTERN void clear_player_inventory_and_stats ( void );
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
EXTERN void print_trace ( int signum );
EXTERN void implant_backtrace_into_signal_handlers ( void ) ;
EXTERN void adapt_button_positions_to_screen_resolution( void );
EXTERN void GiveStandardErrorMessage ( const char* FunctionName , const char* ProblemDescription, int InformDevelopers , int IsFatal );
EXTERN void UpdateScreenOverButtonFromList ( int ButtonIndex );
EXTERN void ShowGenericButtonFromList ( int ButtonIndex );
EXTERN int MouseCursorIsInRect ( SDL_Rect* our_rect , int x , int y );
EXTERN int MouseCursorIsOnButton( int ButtonIndex , int x , int y );
EXTERN void *MyMemmem ( unsigned char *haystack, size_t haystacklen, unsigned char *needle, size_t needlelen);
EXTERN char* ReadAndMallocStringFromData ( char* SearchString , const char* StartIndicationString , const char* EndIndicationString );
EXTERN int CountStringOccurences ( char* SearchString , const char* TargetString ) ;
EXTERN void ReadValueFromStringWithDefault( char* SearchBeginPointer , const char* ValuePreceedText , const char* FormatString , const char * DefaultValueString, void* TargetValue , char* EndOfSearchSectionPointer );
EXTERN void ReadValueFromString( char* SearchBeginPointer , const char* ValuePreceedText , const char* FormatString , void* TargetValue , char* EndOfSearchSectionPointer );
EXTERN char* ReadAndMallocAndTerminateFile( char* filename , const char* File_End_String ) ;
EXTERN char* LocateStringInData ( char* SearchBeginPointer, const char* SearchTextPointer ) ;
EXTERN int find_file (char *fname, char *datadir, char * File_Path, int silent);
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
EXTERN Sint16 ReadSint16 (void * memory);
EXTERN void endian_swap(char * pdata, size_t dsize, size_t nelements);

// enemy.c 
#undef EXTERN
#ifdef _enemy_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void robot_group_turn_hostile ( int enemy_num );
EXTERN void SetRestOfGroupToState ( Enemy ThisRobot , int NewState );
EXTERN int MakeSureEnemyIsInsideHisLevel ( Enemy ThisRobot );
EXTERN void Enemy_Post_Bullethit_Behaviour( int EnemyNum );
EXTERN void ShuffleEnemys ( int LevelNum );
EXTERN int CheckEnemyEnemyCollision (int enemynum);
EXTERN void MoveEnemys (void);
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
EXTERN void show_backgrounded_text_rectangle ( char* text , int x , int y , int w , int h );
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

EXTERN int DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* Background , float text_stretch );
EXTERN int DisplayText ( const char *text, int startx, int starty, const SDL_Rect *clip , float text_stretch );
EXTERN void display_current_chat_protocol ( int background_picture_code , enemy* ChatDroid , int with_update );

EXTERN void DisplayChar (unsigned char c);
EXTERN int ScrollText (char *text, int startx, int starty, int EndLine , int background_code );

EXTERN int ImprovedCheckLineBreak(unsigned char *text, const SDL_Rect *clip , float text_stretch );
EXTERN char *PreviousLine (char *textstart, char *text);
EXTERN char *NextLine (char *text);
EXTERN char *GetString ( int max_len , int echo , int background_code , const char* text_for_overhead_promt ) ;
EXTERN void printf_SDL (SDL_Surface *screen, int x, int y, const char *fmt, ...);
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
EXTERN void DebugPrintf (int db_level, const char *fmt, ...);
EXTERN void *MyMalloc (long);
EXTERN void PrepareMultipleChoiceDialog ( Enemy ChatDroid , int with_flip );
EXTERN int FS_filelength (FILE *f);

// hud.c 
#undef EXTERN
#ifdef _hud_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void GiveItemDescription ( char* ItemDescText , item* CurItem , int ForShop );
EXTERN void DisplayBanner ( void );
EXTERN int get_days_of_game_duration ( float current_game_date );
EXTERN int get_hours_of_game_duration ( float current_game_date );
EXTERN int get_minutes_of_game_duration ( float current_game_date );
EXTERN void append_new_game_message ( char* game_message_text );
EXTERN void display_current_game_message_window ( void );

// shop.c 
#undef EXTERN
#ifdef _shop_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ShowRescaledItem ( int position , int TuxItemRow , item* ShowItem );
EXTERN int TryToIntegrateItemIntoInventory ( item* BuyItem , int AmountToBuyAtMost );
EXTERN int AssemblePointerListForChestShow ( item** ItemPointerListPointer , int player_num , moderately_finepoint chest_pos );
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
EXTERN inline Uint32 FdGetPixel32 (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN inline Uint32 FdGetPixel24 (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN inline Uint16 FdGetPixel16 (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN inline Uint8 FdGetPixel8 (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN inline void PutPixel32 (SDL_Surface * surface, int x, int y, Uint32 pixel);
EXTERN inline void PutPixel24 (SDL_Surface * surface, int x, int y, Uint32 pixel);
EXTERN inline void PutPixel16 (SDL_Surface * surface, int x, int y, Uint32 pixel);
EXTERN inline void PutPixel8 (SDL_Surface * surface, int x, int y, Uint32 pixel);
EXTERN Uint32 FdGetPixel (SDL_Surface * Surface, Sint32 X, Sint32 Y);
EXTERN void PutPixel (SDL_Surface * surface, int x, int y, Uint32 pixel);

//--------------------
// Leave this final endif in here!  It's the wrapper of the whole
// file to prevent double-definitions.
//
#endif
