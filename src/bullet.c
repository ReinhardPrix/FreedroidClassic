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
 */

/* ----------------------------------------------------------------------
 * Desc: all Bullet AND Blast - related functions.
 * ---------------------------------------------------------------------- */

#define _bullet_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

/* Distances for hitting a druid */
#define MORE		(4/64.0)
#define DRUIDHITDIST2		(0.3+MORE)*(Druid_Radius_Y+MORE)

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
move_this_bullet_and_check_its_collisions ( int num )
{
  Bullet CurBullet = & ( AllBullets [ num ] ) ; 
  moderately_finepoint dist_vector;
  moderately_finepoint bullet_step_vector;
  float whole_step_size;
  int i;
  float number_of_steps;

  //--------------------
  // In case of a bullet, which is not a melee weapon, we just move
  // the bullets as specified in it's speed vector.  But of course we
  // must make several stops and check for collisions in case the 
  // planned step would be too big to crash into walls...
  //
  whole_step_size = max ( fabsf ( CurBullet->speed.x * Frame_Time () ) , 
			  fabsf ( CurBullet->speed.y * Frame_Time () ) );

  //--------------------
  // NOTE:  The number 0.25 here is the value of thickness of the collision
  // rectangle of a standard wall.  Since we might not have loaded all wall tiles
  // at every time of the game, also during game, guessing the minimum thickness
  // of walls at runtime is a bit hard and would be unconvenient and complicated,
  // so I leave this with the hard-coded constant for now...
  //
  number_of_steps = rintf ( whole_step_size / 0.25 ) + 1 ;

  bullet_step_vector . x = 0.5 * CurBullet -> speed . x * Frame_Time () / number_of_steps ;
  bullet_step_vector . y = 0.5 * CurBullet -> speed . y * Frame_Time () / number_of_steps ;

  for ( i = 0 ; i < number_of_steps ; i ++ )
    {
      CurBullet -> pos . x += bullet_step_vector . x ;
      CurBullet -> pos . y += bullet_step_vector . y ;
  
      CheckBulletCollisions ( num ) ;
    }

  //--------------------
  // Now we move the angle-changing melee weapons.  These should leave the
  // code anyway as soon as bastian can start doing the enemy movement cycles
  // so we're a bit careless here for now...
  //
  if ( CurBullet->angle_change_rate > 0 )
    {
      //--------------------
      // We change the angle of the bullet itself.  That's the easier part.
      // Rotation by selecting angle.  Very easy indead.
      //
      DebugPrintf( 1 , "\n Angle change rate : %f " , CurBullet->angle_change_rate );
      CurBullet->angle += CurBullet->angle_change_rate * Frame_Time();
      
      //--------------------
      // Now we must rotate the bullet around the influence device or other
      // owner as specified in the bullets owner pointer
      //
      dist_vector.x = 0;
      dist_vector.y = - CurBullet->fixed_offset;
      
      DebugPrintf( 1 , "\n distance vector : (%f/%f) " , dist_vector.x , dist_vector.y );
      
      RotateVectorByAngle ( &dist_vector , CurBullet->angle );
      
      CurBullet->pos.x = CurBullet->owner_pos->x + dist_vector.x;
      CurBullet->pos.y = CurBullet->owner_pos->y + dist_vector.y;
      
      // We tell the graphics functions, that they shall generate new pictures...
      if ( CurBullet->Surfaces_were_generated )
	{
	  SDL_FreeSurface( CurBullet->SurfacePointer[0] );
	  CurBullet->Surfaces_were_generated = FALSE ;
	}
    }
}; // void move_this_bullet_and_check_its_collisions ( CurBullet )

/* ----------------------------------------------------------------------
 * This function moves all the bullets according to their speeds and the
 * current frame rate of course.
 * ---------------------------------------------------------------------- */
void
MoveBullets (void)
{
  int i;
  int map_x;
  int map_y;
  Bullet CurBullet;
  Level BulletLevel;

  //--------------------
  // We move all the bullets
  //
  for ( CurBullet = AllBullets, i = 0; i < MAXBULLETS; CurBullet++, i++)
    {
      //--------------------
      // We need not move any bullets, that are OUT already...
      //
      if ( CurBullet -> type == OUT )
	continue;
      if ( CurBullet -> time_to_hide_still > 0 )
	continue;
      if ( ! IsActiveLevel ( CurBullet -> pos . z ) )
	{
	  if ( Me [ 0 ] . energy > 0 )
	    {
	      GiveStandardErrorMessage ( __FUNCTION__  , "\
Non-OUT bullet found 'stale' outside the active levels.\n\
This is quite normal, if the Tux has just died and the 'present'\n\
level is therefore not 'active' any more, but in this case it happend\n\
while the Tux still had some energy in him.  Very suspicios!!",
					 NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	      continue;
	    }
	}

      move_this_bullet_and_check_its_collisions ( i );

      //--------------------
      // WARNING!  The bullet collision check might have deleted the bullet, so 
      //           maybe there's nothing sensible at the end of that 'CurBullet'
      //           pointer any more at this point.  So we check AGAIN for 'OUT'
      //           bullets, before we proceed with the safety out-of-map checks...
      //
      if ( CurBullet -> type == OUT )
	continue;

      //--------------------
      // Maybe the bullet has a limited lifetime.  In that case we check if the
      // bullet has expired yet or not.
      //
      if ( ( CurBullet->bullet_lifetime != (-1) ) && 
	   ( CurBullet->time_in_seconds > CurBullet->bullet_lifetime ) )
	{
	  DeleteBullet( i , FALSE );
	  continue;
	}
      CurBullet->time_in_frames++;
      CurBullet->time_in_seconds += Frame_Time();

      //--------------------
      // Maybe the bullet is currently on a converyor belt.
      // In this case, be must move on the bullet accordinly
      //
      map_x= (int) rintf( CurBullet->pos.x );
      map_y= (int) rintf( CurBullet->pos.y );

      //--------------------
      // But maybe the bullet is also outside the map already, which would
      // cause a SEGFAULT directly afterwards, when the map is queried.
      // Therefore we introduce some extra security here...
      //
      BulletLevel = curShip.AllLevels [ CurBullet -> pos.z ];
      if ( ( map_x < 0 ) || ( map_x >= BulletLevel->xlen ) ||
	   ( map_y < 0 ) || ( map_y >= BulletLevel->ylen ) )
	{
	  GiveStandardErrorMessage ( __FUNCTION__  , "\
A BULLET WAS FOUND TO EXIST OUTSIDE THE BOUNDS OF THE MAP.\n\
This is an idication for an error of some form, but might also be due\n\
to short occasions of very low frame rates sometimes or it might be due\n\
to a shot right through a gate between two levels.\n\
However:  This problem is not severe, so this is a warning message only.",
				     NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	  DebugPrintf ( -1000 , "\nBullet ouside of map: pos.x=%f, pos.y=%f, pos.z=%d, type=%d." ,
			CurBullet -> pos . x , CurBullet -> pos . y , CurBullet -> pos . z , CurBullet -> type );
	  DeleteBullet ( i , FALSE );
	  return;
	  Terminate(ERR);
	}
      
    }				/* for */
}; // void MoveBullets(void)

/* ----------------------------------------------------------------------
 * This function eliminates the bullet with the given number.  As an 
 * additional parameter you can specify if there should be a blast 
 * generated at the location where the bullet died (=TRUE) or not (=FALSE).
 * ---------------------------------------------------------------------- */
void 
DeleteBullet ( int Bulletnumber , int ShallWeStartABlast )
{
  Bullet CurBullet = & ( AllBullets [ Bulletnumber ] ) ;
  int i;

  //--------------------
  // At first we generate the blast at the collision spot of the bullet,
  // cause later, after the bullet is deleted, it will be hard to know
  // the correct location ;)
  //
  if ( ShallWeStartABlast ) StartBlast ( CurBullet->pos.x, CurBullet->pos.y, CurBullet->pos.z , BULLETBLAST );

  //--------------------
  // maybe, the bullet had several SDL_Surfaces attached to it.  Then we need to 
  // free the SDL_Surfaces again as well...
  //
  if ( CurBullet->Surfaces_were_generated ) 
    {
      DebugPrintf( 1 , "\nvoid DeleteBullet(...): freeing this bullets attached surfaces...");
      for ( i = 0 ; i < Bulletmap [ CurBullet->type ] . phases ; i++ )
	{
	  SDL_FreeSurface( CurBullet -> SurfacePointer [ i ] );
	  CurBullet -> SurfacePointer [ i ] = NULL;
	}
      CurBullet -> Surfaces_were_generated = FALSE;
    }

  //--------------------
  // Now that the memory has been freed again, we can finally delete this bullet entry.
  // Hope, that this does not give us a SEGFAULT, but it should not do so.
  //
  CurBullet->type = OUT;
  CurBullet->time_in_seconds = 0;
  CurBullet->time_in_frames = 0;
  CurBullet->mine = FALSE;
  CurBullet->phase = 0;
  CurBullet->pos.x = 0;
  CurBullet->pos.y = 0;
  CurBullet->pos.z = -1;
  CurBullet->angle = 0;

}; // void DeleteBullet( int Bulletnumber , int StartBlast )

/* ----------------------------------------------------------------------
 * This function starts a blast (i.e. an explosion) at the given location
 * in the usual map coordinates of course.  The type of blast must also
 * be specified, where possible values are defined in defs.h as follows:
 *
 * BULLETBLAST = 0 , (explosion of a small bullet hitting the wall)
 * DRUIDBLAST,       (explosion of a dying droid)
 * OWNBLAST          (not implemented)
 *
 * ---------------------------------------------------------------------- */
void
StartBlast ( float x, float y, int level , int type)
{
    int i;
    Blast NewBlast;
    
    //--------------------
    // Maybe there is a box under the blast.  In this case, the box will
    // get smashed and perhaps an item will drop.
    // 
    smash_obstacle ( x , y , -1 );
    
    // find out the position of the next free blast
    for (i = 0; i < MAXBLASTS; i++)
	if (AllBlasts[i].type == OUT)
	    break;
    
    // didn't fine any --> then take the first one
    if (i >= MAXBLASTS)
	i = 0;
    
    // get pointer to it: more comfortable 
    NewBlast = &(AllBlasts[i]);
    
    // create a blast at the specified x/y coordinates
    NewBlast->pos.x = x;
    NewBlast->pos.y = y;
    NewBlast->pos.z = level;
    
    NewBlast->type = type;
    NewBlast->phase = 0;
    
    NewBlast->MessageWasDone = 0;
    
    if (type == DRUIDBLAST)
    {
	DruidBlastSound ();
    }
    
}; // void StartBlast( ... )

/* ----------------------------------------------------------------------
 * This function advances the different phases of an explosion according
 * to the current lifetime of each explosion (=blast).
 * ---------------------------------------------------------------------- */
void
animate_blasts (void)
{
  int i, map_x, map_y;
  Blast CurBlast = AllBlasts;
  Level BlastLevel;

  for (i = 0; i < MAXBLASTS; i++, CurBlast++)
    if (CurBlast->type != OUT)
      {
	
	//--------------------
	// But maybe the bullet is also outside the map already, which would
	// cause a SEGFAULT directly afterwards, when the map is queried.
	// Therefore we introduce some extra security here...
	//
	map_x= (int) rintf( CurBlast->pos.x );
	map_y= (int) rintf( CurBlast->pos.y );
	BlastLevel = curShip.AllLevels[ CurBlast->pos.z ];
	if ( ( map_x < 0 ) || ( map_x >= BlastLevel->xlen ) ||
	     ( map_y < 0 ) || ( map_y >= BlastLevel->ylen ) )
	  {
	    GiveStandardErrorMessage ( __FUNCTION__  , "\
A BLAST WAS FOUND TO EXIST OUTSIDE THE BOUNDS OF THE MAP.\n\
This is an idication for an inconsistency in Freedroid.\n\
\n\
However, the error is not fatal and will be silently compensated for now.\n\
When reporting a problem to the Freedroid developers, please note if this\n\
warning message was created prior to the error in your report.\n\
However, it should NOT cause any serious trouble for Freedroid.",
				       NO_NEED_TO_INFORM, IS_WARNING_ONLY );
	    CurBlast->pos.x = 0 ;
	    CurBlast->pos.y = 0 ;
	    CurBlast->pos.z = 0 ;
	    DeleteBlast( i );
	    continue;
	  }

	//--------------------
	// Druid blasts are dangerous, so we check if someone gets
	// hurt by this particular droid explosion
	//
	if (CurBlast->type == DRUIDBLAST) CheckBlastCollisions (i);

	//--------------------
	// And now we advance the phase of the blast according to the
	// time that has passed since the last frame (approximately)
	//
	// CurBlast->phase += Frame_Time () * Blastmap[ CurBlast->type ].phases / Blastmap[ CurBlast->type ].total_animation_time;
	CurBlast->phase += Frame_Time () * PHASES_OF_EACH_BLAST / Blastmap[ CurBlast->type ].total_animation_time;

	//--------------------
	// Maybe the blast has lived over his normal lifetime already.
	// Then of course it's time to delete the blast, which is done
	// here.
	//
	if ( ( (int) floorf (CurBlast->phase)) >= PHASES_OF_EACH_BLAST )
	  DeleteBlast (i);
      }				/* if */
}; // void animate_blasts( ... )

/* ----------------------------------------------------------------------
 * This function deletes a single blast entry from the list of all blasts
 * ---------------------------------------------------------------------- */
void
DeleteBlast (int BlastNum)
{
  AllBlasts[ BlastNum ].phase = OUT;
  AllBlasts[ BlastNum ].type = OUT;
}; // void DeleteBlast( int BlastNum )

/* ----------------------------------------------------------------------
 * This function advances the currently active spells.
 * ---------------------------------------------------------------------- */
void
MoveActiveSpells (void)
{
  int i , j ;
  float PassedTime;
  float DistanceFromCenter;
  PassedTime = Frame_Time ();

  for ( i = 0; i < MAX_ACTIVE_SPELLS; i++ )
    {
      //--------------------
      // We can ignore all unused entries...
      //
      if ( AllActiveSpells [ i ] . type == (-1) ) continue;

      //--------------------
      // All spells should count their lifetime...
      //
      AllActiveSpells [ i ] . spell_age += PassedTime;

      //--------------------
      // Now we handle the emp waves...
      //
      if ( AllActiveSpells [ i ] . type == SPELL_RADIAL_EMP_WAVE )
	{
	  AllActiveSpells [ i ] . spell_radius += 5.0 * PassedTime;

	  //--------------------
	  // Here we also do the spell-collision checking in this case
	  //
	  for ( j = 0 ; j < MAX_ENEMYS_ON_SHIP ; j ++ )
	    {
	      if ( AllEnemys [ j ] . Status == OUT ) continue;
	      if ( AllEnemys [ j ] . pos . z != Me [ 0 ] . pos . z ) continue;

	      DistanceFromCenter = sqrt ( ( AllActiveSpells [ i ] . spell_center . x - AllEnemys [ j ] . pos . x ) *
					  ( AllActiveSpells [ i ] . spell_center . x - AllEnemys [ j ] . pos . x ) +
					  ( AllActiveSpells [ i ] . spell_center . y - AllEnemys [ j ] . pos . y ) *
					  ( AllActiveSpells [ i ] . spell_center . y - AllEnemys [ j ] . pos . y ) );
	      
	      if ( fabsf ( DistanceFromCenter - AllActiveSpells [ i ] . spell_radius ) < 0.4 )
		{
		  AllEnemys [ j ] . energy -= 80.0 * Frame_Time();
		  AllEnemys [ j ] . firewait = Druidmap [ AllEnemys [ j ] . type ] . recover_time_after_getting_hit ;
		}
	    }

	  //--------------------
	  // Such a spell can not live for longer than 1.0 seconds, say
	  //
	  if ( AllActiveSpells [ i ] . spell_age >= 1.0 ) DeleteSpell ( i ) ;
	}

      //--------------------
      // Now we handle the vmx waves...
      //
      if ( AllActiveSpells [ i ] . type == SPELL_RADIAL_VMX_WAVE )
	{
	  AllActiveSpells [ i ] . spell_radius += 6.0 * PassedTime;

	  //--------------------
	  // Here we also do the spell-collision checking in this case
	  //
	  for ( j = 0 ; j < MAX_ENEMYS_ON_SHIP ; j ++ )
	    {
	      if ( AllEnemys [ j ] . Status == OUT ) continue;
	      if ( AllEnemys [ j ] . pos . z != Me [ 0 ] . pos . z ) continue;

	      DistanceFromCenter = sqrt ( ( AllActiveSpells [ i ] . spell_center . x - AllEnemys [ j ] . pos . x ) *
					  ( AllActiveSpells [ i ] . spell_center . x - AllEnemys [ j ] . pos . x ) +
					  ( AllActiveSpells [ i ] . spell_center . y - AllEnemys [ j ] . pos . y ) *
					  ( AllActiveSpells [ i ] . spell_center . y - AllEnemys [ j ] . pos . y ) );
	      
	      if ( fabsf ( DistanceFromCenter - AllActiveSpells [ i ] . spell_radius ) < 0.4 )
		{
		  AllEnemys [ j ] . energy -= 180.0 * Frame_Time();
		  AllEnemys [ j ] . firewait = Druidmap [ AllEnemys [ j ] . type ] . recover_time_after_getting_hit ;
		}
	    }

	  //--------------------
	  // Such a spell can not live for longer than 1.0 seconds, say
	  //
	  if ( AllActiveSpells [ i ] . spell_age >= 1.0 ) DeleteSpell ( i ) ;
	}

      //--------------------
      // Now we handle the fire waves...
      //
      if ( AllActiveSpells [ i ] . type == SPELL_RADIAL_FIRE_WAVE )
	{
	  AllActiveSpells [ i ] . spell_radius += 6.0 * PassedTime;

	  //--------------------
	  // Here we also do the spell-collision checking in this case
	  //
	  for ( j = 0 ; j < MAX_ENEMYS_ON_SHIP ; j ++ )
	    {
	      if ( AllEnemys [ j ] . Status == OUT ) continue;
	      if ( AllEnemys [ j ] . pos . z != Me [ 0 ] . pos . z ) continue;

	      DistanceFromCenter = sqrt ( ( AllActiveSpells [ i ] . spell_center . x - AllEnemys [ j ] . pos . x ) *
					  ( AllActiveSpells [ i ] . spell_center . x - AllEnemys [ j ] . pos . x ) +
					  ( AllActiveSpells [ i ] . spell_center . y - AllEnemys [ j ] . pos . y ) *
					  ( AllActiveSpells [ i ] . spell_center . y - AllEnemys [ j ] . pos . y ) );
	      
	      if ( fabsf ( DistanceFromCenter - AllActiveSpells [ i ] . spell_radius ) < 0.4 )
		{
		  AllEnemys [ j ] . energy -= 300.0 * Frame_Time();
		  AllEnemys [ j ] . firewait = Druidmap [ AllEnemys [ j ] . type ] . recover_time_after_getting_hit ;
		}
	    }

	  //--------------------
	  // Such a spell can not live for longer than 1.0 seconds, say
	  //
	  if ( AllActiveSpells [ i ] . spell_age >= 1.0 ) DeleteSpell ( i ) ;
	}

    }

}; // void MoveActiveSpells( ... )

/* ----------------------------------------------------------------------
 * This function deletes a single blast entry from the list of all blasts
 * ---------------------------------------------------------------------- */
void
DeleteSpell (int SpellNum)
{
  AllActiveSpells [ SpellNum ] . type = ( -1 );
  AllActiveSpells [ SpellNum ] . spell_age = 0 ;
}; // void DeleteSpell( int SpellNum )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
clear_active_spells ( void )
{
  int i ;

  for ( i = 0; i < MAX_ACTIVE_SPELLS; i++ )
    {
      DeleteSpell ( i ) ;
    }

}; // void clear_active_spells ( void )

/* ----------------------------------------------------------------------
 * When an enemy is his, this causes some blood to be sprayed on the floor.
 * The blood is just an obstacle (several types of blood exist) with 
 * preput flag set, so that the Tux and everyone can really step *on* the
 * blood.
 *
 * Blood will always be sprayed, but there is a toggle available for making
 * the blood visible/invisible for more a children-friendly version of the
 * game.
 *
 * This function does the blood spraying (adding of these obstacles).
 * ---------------------------------------------------------------------- */
void
enemy_spray_blood ( Enemy CurEnemy ) 
{
  moderately_finepoint target_pos = { 1.0 , 0 } ;

  DebugPrintf ( 1 , "\nBlood has been sprayed..." );

  RotateVectorByAngle ( & target_pos , MyRandom ( 360 ) );

  target_pos . x += CurEnemy -> virt_pos . x ;
  target_pos . y += CurEnemy -> virt_pos . y ;

  create_new_obstacle_on_level ( curShip . AllLevels [ CurEnemy -> pos . z ] , ISO_BLOOD_1 + MyRandom ( 7 ) , target_pos . x , target_pos . y );
  
}; // void enemy_spray_blood ( Enemy CurEnemy ) 

/* ----------------------------------------------------------------------
 * Bullet collision checks and effect handling for 'flash' bullets is 
 * done here...  Classic bullet collision and effect handling is done
 * somewhere else...
 * ---------------------------------------------------------------------- */
void
handle_flash_effects ( bullet* CurBullet )
{
    int i;

    //--------------------
    // if the flash is over, just delete it and return
    if ( CurBullet->time_in_seconds > FLASH_DURATION_IN_SECONDS )
    {
	CurBullet->time_in_frames = 0;
	CurBullet->time_in_seconds = 0;
	CurBullet->type = OUT;
	CurBullet->mine = FALSE;
	return;
    }
    
    //--------------------
    // if the flash is not yet over, do some checking for who gets
    // hurt by it.  
    //
    // Two different methode for doing this are available:
    // The first but less elegant Method is just to check for
    // flash immunity, for distance and visiblity.
    //
    // The second and more elegant method is to recursively fill
    // out the room where the flash-maker is in and to hurt all
    // robots in there except of course for those immune.
    //
    if ( CurBullet->time_in_frames != 1 ) return; // we only do the damage once and thats at frame nr. 1 of the flash
    
    //--------------------
    // We make sure that the first and last index numbers for each
    // level are halfway correct...
    //
    occasionally_update_first_and_last_bot_indices ( );

    for ( i  = first_index_of_bot_on_level [ CurBullet -> pos . z ] ; 
	  i <=  last_index_of_bot_on_level [ CurBullet -> pos . z ] ; i++ )
    {
	//--------------------
	// Bots on other levels need not be affected by the flash
	// weapon on this level...
	//
	if ( AllEnemys [ i ] . pos . z != CurBullet -> pos . z ) continue ;
	if ( AllEnemys [ i ] . type == (-1) ) continue ;
	
	//--------------------
	// The flash weapon will only take effect, if there is a direct
	// line of sigh between the bullet (the firing bot that is...)
	// and the target bot.
	//
	if ( IsVisible ( & AllEnemys [ i ] . pos , 0 ) &&
	     ( ! Druidmap [ AllEnemys [ i ] . type ] . flashimmune ) ) 
	{
	    AllEnemys [ i ] . energy -= CurBullet -> damage ;

	    //--------------------
	    // Since the enemy just got hit, it might as well say so 
	    // and also spray some blood all around... :)
	    //
	    EnemyHitByBulletText( i );
	    enemy_spray_blood ( & ( AllEnemys [ i ] ) ) ;
	    
	}
    }
    
    //--------------------
    // We do some damage to the Tux, depending on the current
    // disruptor resistance that the Tux might have...
    //
    Me [ 0 ] . energy -= CurBullet -> damage * ( 100 - Me [ 0 ] . resist_disruptor ) / 100 ;
    DebugPrintf ( -4 , "\n%s(): Tux took damage from flash: %f." , __FUNCTION__ , (float)(CurBullet -> damage * ( 100 - Me [ 0 ] . resist_disruptor ) / 100 ) );
    
}; // handle_flash_effects ( bullet* CurBullet )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */  
void
check_bullet_background_collisions ( bullet* CurBullet , int num )
{
  // Check for collision with background
  if ( ! IsPassable ( CurBullet -> pos . x , CurBullet -> pos . y , CurBullet -> pos . z ) )
    {
      if ( CurBullet->ignore_wall_collisions )
	{
	  StartBlast ( CurBullet->pos.x , CurBullet->pos.y , CurBullet->pos.z , BULLETBLAST );
	}
      else
	{
	  DeleteBullet ( num , TRUE ); // we want a bullet-explosion
	  return;
	}
    }
}; // void check_bullet_background_collisions ( CurBullet , num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
apply_bullet_damage_to_player ( int player_num , int damage ) 
{
    float real_damage = damage;
    
    UpdateAllCharacterStats( player_num );
    
    real_damage -= Me [ player_num ] . AC ;
    if ( real_damage < 1.0 ) real_damage = 1.0 ;
    
    //--------------------
    // NEW RULE:  Even when the bullet hits, there's still a chance that
    // the armour will compensate the shot
    //
    if ( MyRandom( 100 ) < Me [ player_num ] . AC )
    {
	Me [ player_num ] . TextVisibleTime = 0 ;
	Me [ player_num ] . TextToBeDisplayed = "That one went into the armour." ;
	BulletReflectedSound ( ) ;
    }
    else
    {
	
	Me [ player_num ] . TextVisibleTime = 0 ;
	Me [ player_num ] . TextToBeDisplayed = "Ouch!" ;
	Me [ player_num ] . energy -= real_damage ;	// loose some energy
	DebugPrintf ( 1 , "\n%s(): Tux took damage from bullet: %f." , __FUNCTION__ , real_damage );
	//--------------------
	// A hit of what form so ever should make the Tux stop
	// dead in his tracks.
	//
	// Me [ player_num ] . speed . x = 0;
	// Me [ player_num ] . speed . y = 0; 
	
	//--------------------
	// As the new rule, the influencer after getting hit, must completely
	// start anew to recover his weapon from the previous shot
	//
	// Me [ player_num ] . firewait = ItemMap[ Me [ player_num ] . weapon_item . type ] . item_gun_recharging_time;
	// Me [ player_num ] . got_hit_time = 0;
	
	// GotHitSound ();
	tux_scream_sound ( );
    }
    //--------------------
    // NEW RULE:  All items equipped suffer damage when the influencer gets hit
    //
    DamageAllEquipment ( player_num ) ;
}; // void apply_bullet_damage_to_player ( int player_num , int damage ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
check_bullet_player_collisions ( bullet* CurBullet , int num )
{
  int player_num;
  double xdist, ydist;

  //--------------------
  // Now we check for collisions with one of the players.
  //
  for ( player_num = 0 ; player_num < MAX_PLAYERS ; player_num ++ )
  {
      //--------------------
      // Of course only active players and players on the same level
      // may be checked!
      //
      if ( Me [ player_num ] . status == OUT ) continue;
      if ( Me [ player_num ] . pos . z != CurBullet -> pos . z ) continue;
      
      //--------------------
      // A player is supposed not to hit himself with his bullets, so we may
      // check for that case as well....
      //
      if ( CurBullet -> mine ) return;
      
      //--------------------
      // Now we see if the distance to the bullet is as low as hitting
      // distance or not.
      //
      xdist = Me [ player_num ] . pos . x - CurBullet -> pos . x ;
      ydist = Me [ player_num ] . pos . y - CurBullet -> pos . y ;
      if ((xdist * xdist + ydist * ydist) < DRUIDHITDIST2)
	{
#ifdef USE_MISS_HIT_ARRAYS
	    if ( CurBullet->miss_hit_influencer == UNCHECKED ) 
	    {
		if ( MyRandom ( 100 ) < CurBullet->to_hit )
		{
		    CurBullet->miss_hit_influencer = HIT ;
#endif			  
		    
		    apply_bullet_damage_to_player ( player_num , CurBullet-> damage ) ;
		    
		    DeleteBullet ( num , TRUE ) ; // we want a bullet-explosion
		    return;  // This bullet was deleted and does not need to be processed any further...
#ifdef USE_MISS_HIT_ARRAYS
		}
		else
		{
		    CurBullet->miss_hit_influencer = MISS ;
		}
	    }
#endif
	}
    }
}; // check_bullet_player_collisions ( CurBullet , num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
check_bullet_enemy_collisions ( bullet* CurBullet , int num )
{
  int i;
  double xdist, ydist;
  int level = CurBullet -> pos.z ;
  static int FBTZaehler = 0;
  enemy* ThisRobot;

  //--------------------
  // Check for collision with enemys
  //
  ThisRobot = & ( AllEnemys [ 0 ] ) ;
  ThisRobot -- ;
  for (i = 0; i < Number_Of_Droids_On_Ship; i++)
    {
      ThisRobot ++ ;
      if ( ThisRobot -> Status == OUT || ThisRobot -> pos . z != level)
	continue;
      
      xdist = CurBullet->pos.x - ThisRobot -> pos . x;
      ydist = CurBullet->pos.y - ThisRobot -> pos . y;
      
      if ( (xdist * xdist + ydist * ydist) < DRUIDHITDIST2 )
	{
#ifdef USE_MISS_HIT_ARRAYS
	  if ( CurBullet->total_miss_hit[ i ] == UNCHECKED )
	    {
	      if ( MyRandom ( 100 ) < CurBullet->to_hit + Druidmap [ ThisRobot -> type ] . getting_hit_modifier )
		{
		  CurBullet->total_miss_hit[ i ] = HIT;
#endif
		  //--------------------
		  // The enemy who was hit, loses some energy, depending on the bullet, and 
		  // also gets stunned from the hit, which only means that the enemy can't
		  // fire immediately now but takes (double?) normal time for the next shot.
		  //
		  ThisRobot -> energy -= CurBullet->damage;
		  
		  enemy_spray_blood ( ThisRobot ) ;

		  //--------------------
		  // If it was a friend, and the bullet came from Tux, the friend
		  // might now become very angry...
		  //
		  if ( CurBullet -> mine ) 
		    {
		      robot_group_turn_hostile ( i );
		    }
		  
		  ThisRobot -> frozen += CurBullet -> freezing_level;
		  
		  ThisRobot -> poison_duration_left += CurBullet->poison_duration;
		  ThisRobot -> poison_damage_per_sec += CurBullet->poison_damage_per_sec;
		  
		  ThisRobot -> paralysation_duration_left += CurBullet->paralysation_duration;
		  
		  // ThisRobot -> firewait =
		  // 1 * ItemMap [ Druidmap [ ThisRobot -> type ].weapon_item.type ].item_gun_recharging_time ;
		  
		  ThisRobot -> firewait = Druidmap [ ThisRobot -> type ] . recover_time_after_getting_hit ;

		  start_gethit_animation_if_applicable ( ThisRobot ) ;

		  // Maybe he will also stop doing his fixed routine and return to normal
		  // operation as well
		  ThisRobot -> AdvancedCommand = 0;
		  
		  // We might also start a little bullet-blast even after the
		  // collision of the bullet with an enemy (not in Paradroid)
		  
		  //--------------------
		  // If the blade can pass through dead and not dead bodies, it will so
		  // so and create a small explosion passing by.  But if it can't, it should
		  // be completely deleted of course, with the same small explosion as well
		  //
		  if ( CurBullet -> pass_through_hit_bodies )
		    StartBlast ( CurBullet -> pos.x , CurBullet -> pos.y , CurBullet -> pos.z , BULLETBLAST );
		  else DeleteBullet( num , TRUE ); // we want a bullet-explosion
		  
		  Enemy_Post_Bullethit_Behaviour( i );
		  
		  if (!CurBullet->mine)
		    {
		      FBTZaehler++;
		    }
		  return;
#ifdef USE_MISS_HIT_ARRAYS
		}
	      
	      else
		{
		  CurBullet -> total_miss_hit[ i ] = MISS;
		  ThisRobot -> TextVisibleTime = 0;
		  ThisRobot -> TextToBeDisplayed = "Haha, you missed me!";
		}
	    }
#endif
	} // if distance low enough to possibly be at hit
    }  /* for AllEnemys */
}; // void check_bullet_enemy_collisions ( CurBullet , num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
check_bullet_bullet_collisions ( bullet* CurBullet , int num )
{
  int i;

  // check for collisions with other bullets
  for (i = 0; i < MAXBULLETS; i++)
    {
      if (i == num) continue;  // never check for collision with youself.. ;)
      if (AllBullets[i].type == OUT) continue; // never check for collisions with dead bullets.. 
      if (AllBullets[i].type == FLASH) continue; // never check for collisions with flashes bullets.. 
      
      if ( fabsf(AllBullets[i].pos.x-CurBullet->pos.x) > BULLET_BULLET_COLLISION_DIST ) continue;
      if ( fabsf(AllBullets[i].pos.y-CurBullet->pos.y) > BULLET_BULLET_COLLISION_DIST ) continue;
      // it seems like we have a collision of two bullets!
      // both will be deleted and replaced by blasts..
      DebugPrintf ( 1 , "\nBullet-Bullet-Collision detected..." );
      
      //CurBullet->type=OUT;
      //AllBullets[num].type=OUT;
      
      if ( CurBullet->reflect_other_bullets )
	{
	  if ( AllBullets[ i ].was_reflected )
	    {
	      // well, if it has been reflected once, we don't do any more
	      // reflections after that...
	    }
	  else
	    {
	      AllBullets[i].speed.x = - AllBullets[i].speed.x;
	      AllBullets[i].speed.y = - AllBullets[i].speed.y;
	      AllBullets[i].was_reflected = TRUE;
	    }
	}
      
      if ( AllBullets[ i ].reflect_other_bullets )
	{
	  if ( CurBullet->was_reflected )
	    {
	      // well, if it has been reflected once, we don't do any more
	      // reflections after that...
	    }
	  else
	    {
	      CurBullet->speed.x = - CurBullet->speed.x;
	      CurBullet->speed.y = - CurBullet->speed.y;
	      CurBullet->was_reflected = TRUE;
	    }
	}
      
    }
}; // void check_bullet_bullet_collisions ( CurBullet , num )

/* ----------------------------------------------------------------------
 * This function checks if there are some collisions of the one bullet
 * with number num with anything else in the game, like blasts, walls,
 * droids, the tux and other bullets.
 * ---------------------------------------------------------------------- */
void
CheckBulletCollisions (int num)
{
    Bullet CurBullet = &AllBullets[num];

    switch ( CurBullet -> type )
    {
	case OUT:
	    // --------------------
	    // Never do any collision checking if the bullet is OUT already...
	    return;
	    break;
	    
	case FLASH:
	    // --------------------
	    // Next we handle the case that the bullet is of type FLASH
	    handle_flash_effects ( CurBullet );
	    return;
	    break;
	    
	default:
	    // --------------------
	    // If its a "normal" Bullet, several checks have to be
	    // done, one for collisions with background, 
	    // one for collision with influencer
	    // some for collisions with enemys
	    // and some for collisions with other bullets
	    // and some for collisions with blast
	    //
	    check_bullet_background_collisions ( CurBullet , num );
	    check_bullet_player_collisions ( CurBullet , num );
	    check_bullet_enemy_collisions ( CurBullet , num );
	    check_bullet_bullet_collisions ( CurBullet , num );
	    
	    break;
    } // switch ( Bullet-Type )
}; // CheckBulletCollisions( ... )

/* ----------------------------------------------------------------------
 * This function checks for collisions of blasts with bullets and druids
 * and delivers damage to the hit objects according to how long they have
 * been in the blast.
 * 
 * Maybe even some text like 'Ouch, this was hot' might be generated.
 *
 * ---------------------------------------------------------------------- */
void
CheckBlastCollisions (int num)
{
    int i;
    Blast CurBlast = &(AllBlasts[num]);
    // int level = CurLevel->levelnum;
    int level = CurBlast->pos.z;
    
    //--------------------
    // At first, we check for collisions of this blast with all bullets 
    //
    for (i = 0; i < MAXBULLETS; i++)
    {
	if (AllBullets[i].type == OUT)
	    continue;
	if (CurBlast->phase > 4)
	    break;
	
	if (abs (AllBullets[i].pos.x - CurBlast->pos.x ) < Blast_Radius)
	{
	    if (abs (AllBullets[i].pos.y - CurBlast->pos.y ) < Blast_Radius)
	    {
		if ( AllBullets[i].pos.z == CurBlast->pos.z )
		{
		    if ( ! AllBullets[i].pass_through_explosions )
		    {
			DeleteBullet( i , TRUE ); // we want a bullet-explosion
		    }
		}
	    }
	}
    }	
    
    //--------------------
    // Now we check for enemys, that might have stepped into this
    // one blasts area of effect...
    //
    for ( i = 0 ; i < Number_Of_Droids_On_Ship ; i ++ )
    {
	if ((AllEnemys[i].Status == OUT)
	    || (AllEnemys[i].pos.z != level))
	    continue;
	
	if ( ( fabsf (AllEnemys[i].pos.x - CurBlast->pos.x ) < Blast_Radius ) &&
	     ( fabsf (AllEnemys[i].pos.y - CurBlast->pos.y ) < Blast_Radius ) )
	{
	    //--------------------
	    // drag energy of enemy 
	    //
	    AllEnemys[i].energy -= Blast_Damage_Per_Second * Frame_Time ();
	}
    }
    
    //--------------------
    // Now we check, if perhaps the influencer has stepped into the area
    // of effect of this one blast.  Then he'll get burnt ;)
    // 
    if ( (Me[0].status != OUT) && 
	 ( fabsf (Me[0].pos.x - CurBlast->pos.x ) < Blast_Radius ) &&
	 ( fabsf (Me[0].pos.y - CurBlast->pos.y ) < Blast_Radius ) )
    {
	Me[0].energy -= Blast_Damage_Per_Second * Frame_Time ();
	
	// So the influencer got some damage from the hot blast
	// Now most likely, he then will also say so :)
	if ( !CurBlast->MessageWasDone )
	{
	    AddInfluBurntText();
	    CurBlast->MessageWasDone=TRUE;
	}
    }
    
}; // CheckBlastCollisions( ... )

#undef _bullet_c
