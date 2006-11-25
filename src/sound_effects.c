#include "sound.h"

/** ============================================ EFFECTS ============================================ */


void
CrySound (void)
{
  play_sample_using_WAV_cache ("../effects/Cry_Sound_0.wav", FALSE, FALSE);
}

void
play_open_chest_sound (void)
{
  PlayOnceNeededSoundSample ("../effects/open_chest_sound.wav", FALSE, TRUE);
}

void
Play_Spell_ForceToEnergy_Sound ()
{
  play_sample_using_WAV_cache ("../effects/Spell_ForceToEnergy_Sound_0.wav", FALSE, FALSE);
}

void
Play_Spell_DetectItems_Sound ()
{
  play_sample_using_WAV_cache ("../effects/Spell_DetectItems_Sound_0.wav", FALSE, FALSE);
}



/* ----------------------------------------------------------------------
 * This function plays a voice sample, stating that not enough power
 * (strength) is available to use a certain item.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 * ---------------------------------------------------------------------- */
void
No_Ammo_Sound (void)
{
  static Uint32 PreviousSound = (-1);
  Uint32 now;

  now = SDL_GetTicks ();
  if (SDL_GetTicks () - PreviousSound >= 0.25 * 1000)
    {
      play_sample_using_WAV_cache ("../effects/No_Ammo_Sound_0.wav", FALSE, FALSE);
      PreviousSound = now;
    }
}//void No_Ammo_Sound ( void )


/* ----------------------------------------------------------------------
 * This function plays a voice sample, stating that not enough power
 * (strength) is available to use a certain item.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 * ---------------------------------------------------------------------- */
void
Not_Enough_Power_Sound (void)
{
  static Uint32 PreviousSound = (-1);
  Uint32 now;

  now = SDL_GetTicks ();
  if (SDL_GetTicks () - PreviousSound >= 1.15 * 1000)
    {
      play_sample_using_WAV_cache
	("../effects/tux_ingame_comments/Not_Enough_Power_Sound_0.ogg", FALSE, FALSE);
      PreviousSound = now;
    }
} // void Not_Enough_Power_Sound ( void )



/* ----------------------------------------------------------------------
 * This function plays a voice sample, stating that not enough power
 * distribution (dexterity) is available to use a certain item.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 * ---------------------------------------------------------------------- */
void
Not_Enough_Dist_Sound (void)
{
  static Uint32 PreviousSound = (-1);
  Uint32 now;

  now = SDL_GetTicks ();
  if (SDL_GetTicks () - PreviousSound >= 1.15 * 1000)
    {
      play_sample_using_WAV_cache
	("../effects/tux_ingame_comments/Not_Enough_Dist_Sound_0.ogg", FALSE, FALSE);
      PreviousSound = now;
    }
}



/* ----------------------------------------------------------------------
 * This function plays a voice sample, stating that not enough magical
 * energy (force) is available to cast a certain spell.
 * The sample must of course only be played, if it hasn't been played just
 * milliseconds before, so a check is made to see that the file is played
 * with at least a certain interval in between to the last occasion of the
 * file being played.
 * ---------------------------------------------------------------------- */
void
Not_Enough_Mana_Sound (void)
{

  static Uint32 PreviousNotEnoughForceSound = (-1);
  Uint32 now;

  now = SDL_GetTicks ();
  if (SDL_GetTicks () - PreviousNotEnoughForceSound >= 1.15 * 1000)
    {
      play_sample_using_WAV_cache ("../effects/tux_ingame_comments/Not_Enough_Mana_0.ogg", FALSE, FALSE);
      PreviousNotEnoughForceSound = now;
    }

}

/* ----------------------------------------------------------------------
 * Whenever the Tux meets someone in the game for the very first time, 
 * this enemy or friend will issue the first-time greeting.
 * Depending on whether that is typically a hostile unit or not, we play
 * the corresponding sound from preloaded memory or, if there is no combat
 * to expect, by loading it freshly from disk and then forgeting about it
 * again.
 * ---------------------------------------------------------------------- */
void
PlayGreetingSound (int SoundCode)
{
  switch (SoundCode)
    {
    case -1:
      return;
      break;
    case 0:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_0.wav", FALSE, FALSE);
      break;
    case 1:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_1.wav", FALSE, FALSE);
      break;
    case 2:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_2.wav", FALSE, FALSE);
      break;
    case 3:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_3.wav", FALSE, FALSE);
      break;
    case 4:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_4.wav", FALSE, FALSE);
      break;
    case 5:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_5.wav", FALSE, FALSE);
      break;
    case 6:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_6.wav", FALSE, FALSE);
      break;
    case 7:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_7.wav", FALSE, FALSE);
      break;
    case 8:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_8.wav", FALSE, FALSE);
      break;
    case 9:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_9.wav", FALSE, FALSE);
      break;
    case 10:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_10.wav", FALSE, FALSE);
      break;
    case 11:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_11.wav", FALSE, FALSE);
      break;
    case 12:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_12.wav", FALSE, FALSE);
      break;
    case 13:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_13.wav", FALSE, FALSE);
      break;
    case 14:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_14.wav", FALSE, FALSE);
      break;
    case 15:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_15.wav", FALSE, FALSE);
      break;
    case 16:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_16.wav", FALSE, FALSE);
      break;
    case 17:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_17.wav", FALSE, FALSE);
      break;
    case 18:
      play_sample_using_WAV_cache ("../effects/bot_sounds/First_Contact_Sound_18.wav", FALSE, FALSE);
      break;
    default:
      DebugPrintf (0, "\nUnknown Greeting sound!!! Terminating...");
      Terminate (ERR);
      break;
    }
}


/* ----------------------------------------------------------------------
 * Whenever a bot dies, that should create a dying sound.  But so far,
 * this will be done only for fully animated bots, since the other bots
 * just explode and that has a sound of it's own.
 * ---------------------------------------------------------------------- */
void
play_death_sound_for_bot (enemy * ThisRobot)
{
  char filename[5000];

  //--------------------
  // If the keyword 'none' for the death sound file name is encountered,
  // nothing will be done...
  //.
  if (!strcmp (Druidmap[ThisRobot->type].droid_death_sound_file_name, "none"))
    return;

  //--------------------
  // Now we play the given death sound, looking for the file in the
  // appropriate sound folder.
  //
  strcpy (filename, "../effects/bot_sounds/");
  strcat (filename, Druidmap[ThisRobot->type].droid_death_sound_file_name);
  // PlayOnceNeededSoundSample ( filename , FALSE , FALSE );
  play_sample_using_WAV_cache (filename, FALSE, FALSE);

}

/* ----------------------------------------------------------------------
 * Whenever a bot dies, that should create a dying sound.  But so far,
 * this will be done only for fully animated bots, since the other bots
 * just explode and that has a sound of it's own.
 * ---------------------------------------------------------------------- */
void
play_attack_animation_sound_for_bot (enemy * ThisRobot)
{
  char filename[5000];

  //--------------------
  // If the keyword 'none' for the death sound file name is encountered,
  // nothing will be done...
  //
  if (!strcmp (Druidmap[ThisRobot->type].droid_attack_animation_sound_file_name, "none"))
    return;

  //--------------------
  // Now we play the given death sound, looking for the file in the
  // appropriate sound folder.
  //
  strcpy (filename, "../effects/bot_sounds/");
  strcat (filename, Druidmap[ThisRobot->type].droid_attack_animation_sound_file_name);
  PlayOnceNeededSoundSample (filename, FALSE, FALSE);

}


/* ----------------------------------------------------------------------
 * Whenever a bot starts to attack the Tux, he'll issue the attack cry.
 * This is done here, and no respect to loading time issues for now...
 * ---------------------------------------------------------------------- */
void
play_enter_attack_run_state_sound (int SoundCode)
{
  switch (SoundCode)
    {
    case -1:
      return;
      break;
    case 0:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_0.wav", FALSE, FALSE);
      break;
    case 1:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_1.wav", FALSE, FALSE);
      break;
    case 2:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_2.wav", FALSE, FALSE);
      break;
    case 9:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_9.wav", FALSE, FALSE);
      break;
    case 10:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_10.wav", FALSE, FALSE);
      break;
    case 11:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_11.wav", FALSE, FALSE);
      break;
    case 12:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_12.wav", FALSE, FALSE);
      break;
    case 13:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_13.wav", FALSE, FALSE);
      break;
    case 14:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_14.wav", FALSE, FALSE);
      break;
    case 15:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_15.wav", FALSE, FALSE);
      break;
    case 16:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_16.wav", FALSE, FALSE);
      break;
    case 17:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_17.wav", FALSE, FALSE);
      break;
    case 18:
      PlayOnceNeededSoundSample ("../effects/bot_sounds/Start_Attack_Sound_18.wav", FALSE, FALSE);
      break;
    default:
      DebugPrintf (0, "\nUnknown Start Attack sound!!! NOT TERMINATING CAUSE OF THIS...");
      // Terminate( ERR );
      break;
    }
}

/* ----------------------------------------------------------------------
 * Whenever an item is placed or taken, we'll issue a sound attached to
 * that item. 
 * ---------------------------------------------------------------------- */
void
play_item_sound (int item_type)
{
  char final_file_name[5000];

  //--------------------
  // First some sanity check...
  //
  if (item_type < 0)
    {
      fprintf (stderr, "\n\nitem_type %d \n", item_type);
      GiveStandardErrorMessage (__FUNCTION__, "\
		                           negative item type received!\n", PLEASE_INFORM, IS_FATAL);
    }

  //--------------------
  // Now we can proceed and just play the sound associated with that item...
  //
  strcpy (final_file_name, "../effects/item_sounds/");
  strcat (final_file_name, ItemMap[item_type].item_drop_sound_file_name);
  PlayOnceNeededSoundSample (final_file_name, FALSE, TRUE);

}

/* ----------------------------------------------------------------------
 * This function generates a voice output stating that the influencer 
 * can't carry any more right now.  Also this function will see to it,
 * that the sentence is not repeated until 4 seconds after the previous
 * cant-carry-sentence have passed.
 * ---------------------------------------------------------------------- */
void
CantCarrySound (void)
{
  static Uint32 CurrentTicks = 0;

  if ((SDL_GetTicks () - CurrentTicks) > 2 * 1000)
    {
      CurrentTicks = SDL_GetTicks ();
      switch (MyRandom (2))
	{
	case 0:
	  play_sample_using_WAV_cache
	    ("../effects/tux_ingame_comments/ICantCarryAnyMore_Sound_0.ogg", FALSE, FALSE);
	  break;
	case 1:
	  play_sample_using_WAV_cache
	    ("../effects/tux_ingame_comments/ICantCarryAnyMore_Sound_1.ogg", FALSE, FALSE);
	  break;
	case 2:
	  play_sample_using_WAV_cache
	    ("../effects/tux_ingame_comments/ICantCarryAnyMore_Sound_2.ogg", FALSE, FALSE);
	  break;
	default:
	  break;
	}
    }
}

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
TransferSound (void)
{
  play_sample_using_WAV_cache ("../effects/Takeover_Sound_0.wav", FALSE, FALSE);
}

;				// void TransferSound (void)

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
Mission_Status_Change_Sound (void)
{
  play_sample_using_WAV_cache ("../effects/Mission_Status_Change_Sound_0.wav", FALSE, FALSE);
}


/* ----------------------------------------------------------------------
 * When the Tux uses the 'teleport home' spell, this sound will be played.
 * ---------------------------------------------------------------------- */
void
teleport_arrival_sound (void)
{
  // play_sample_using_WAV_cache ( "../effects/LeaveElevator_Sound_0.wav", FALSE , FALSE );
  play_sample_using_WAV_cache ("../effects/new_teleporter_sound.wav", FALSE, FALSE);
}

/* ----------------------------------------------------------------------
 * When the Tux casts a healing spell, this sound will be played.
 * ---------------------------------------------------------------------- */
void
healing_spell_sound (void)
{
  play_sample_using_WAV_cache ("../effects/new_healing_sound.wav", FALSE, FALSE);
}

/* ----------------------------------------------------------------------
 * Some items, that can be applied inside the running game, like e.g. 
 * spellbooks, do have a certain character stat requirement.  If that stat
 * requirement isn't met, a sound will be played, which uses this function.
 * ---------------------------------------------------------------------- */
void
application_requirements_not_met_sound (void)
{
  play_sample_using_WAV_cache ("../effects/tux_ingame_comments/i_cannot_use_this_item_yet.ogg", FALSE, FALSE);
}



void
GotHitSound (void)
{
  play_sample_using_WAV_cache ("../effects/Got_Hit_Sound_0.wav", FALSE, FALSE);
}



/* ----------------------------------------------------------------------
 * Whenever the Tux gets hit, we *might* play a sound sample for the hit,
 * depending on random results.  Since we don't want to hear the same 
 * sound sample all the time, there is a selection of possible files 
 * one of which will be selected at random.
 * ---------------------------------------------------------------------- */
void
tux_scream_sound (void)
{
  if (MyRandom (7) <= 5)
    return;

  switch (MyRandom (4))
    {
    case 0:
      play_sample_using_WAV_cache ("../effects/Influencer_Scream_Sound_0.wav", FALSE, FALSE);
      break;
    case 1:
      play_sample_using_WAV_cache ("../effects/Influencer_Scream_Sound_1.wav", FALSE, FALSE);
      break;
    case 2:
      play_sample_using_WAV_cache ("../effects/Influencer_Scream_Sound_2.wav", FALSE, FALSE);
      break;
    case 3:
      play_sample_using_WAV_cache ("../effects/Influencer_Scream_Sound_3.wav", FALSE, FALSE);
      break;
    case 4:
      play_sample_using_WAV_cache ("../effects/Influencer_Scream_Sound_4.wav", FALSE, FALSE);
      break;
    default:
      break;
    }
}


/* ----------------------------------------------------------------------
 * We add a matching group of sounds here for the menu movements.  It's
 * a 'ping-ping' sound, well, not super, but where do we get a better one?
 * ---------------------------------------------------------------------- */
void
MenuItemSelectedSound (void)
{
  play_sample_using_WAV_cache ("../effects/Menu_Item_Selected_Sound_1.wav", FALSE, FALSE);
}

void
MenuItemDeselectedSound (void)
{
  play_sample_using_WAV_cache ("../effects/Menu_Item_Deselected_Sound_0.wav", FALSE, FALSE);
}

void
MoveMenuPositionSound (void)
{
  play_sample_using_WAV_cache ("../effects/Move_Menu_Position_Sound_0.wav", FALSE, FALSE);
}



/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ThouArtDefeatedSound (void)
{
  if (!sound_on)
    return;
  PlayOnceNeededSoundSample ("../effects/ThouArtDefeated_Sound_0.wav", FALSE, FALSE);
}


/* ----------------------------------------------------------------------
 * When the Tux makes a weapon swing, this will either cause a swinging
 * sound and then a 'hit' sound or it will just be a swinging sound.  The
 * following functions do this, also creating some variation in the choice
 * of sample used.
 * ---------------------------------------------------------------------- */
void
play_melee_weapon_hit_something_sound (void)
{
  switch (MyRandom (3))
    {
    case 0:
      play_sample_using_WAV_cache ("../effects/swing_then_hit_1.wav", FALSE, FALSE);
      break;
    case 1:
      play_sample_using_WAV_cache ("../effects/swing_then_hit_2.wav", FALSE, FALSE);
      break;
    case 2:
      play_sample_using_WAV_cache ("../effects/swing_then_hit_3.wav", FALSE, FALSE);
      break;
    case 3:
      play_sample_using_WAV_cache ("../effects/swing_then_hit_4.wav", FALSE, FALSE);
      break;
    default:
      break;
    }
}

void
play_melee_weapon_missed_sound (void)
{
  switch (MyRandom (3))
    {
    case 0:
      play_sample_using_WAV_cache ("../effects/swing_then_nohit_1.wav", FALSE, FALSE);
      break;
    case 1:
      play_sample_using_WAV_cache ("../effects/swing_then_nohit_2.wav", FALSE, FALSE);
      break;
    case 2:
      play_sample_using_WAV_cache ("../effects/swing_then_nohit_3.wav", FALSE, FALSE);
      break;
    case 3:
      play_sample_using_WAV_cache ("../effects/swing_then_nohit_4.wav", FALSE, FALSE);
      break;
    default:
      break;
    }
}


/* ----------------------------------------------------------------------
 * This function should generate the sound that belongs to a certain
 * (ranged) weapon.  This does not include the Tux swinging/swinging_and_hit
 * sounds, when Tux is using melee weapons, but it does include ranged
 * weapons and the non-animated bot weapons too.
 * ---------------------------------------------------------------------- */
void
Fire_Bullet_Sound (int BulletType)
{
  if (!sound_on)
    return;

  DebugPrintf (1, "\nFireBulletSound called...");

  switch (BulletType)
    {
    case PULSE:
      play_sample_using_WAV_cache ("../effects/Fire_Bullet_Pulse_Sound_0.wav", FALSE, FALSE);
      break;

    case SINGLE_PULSE:
      play_sample_using_WAV_cache ("../effects/Fire_Bullet_Single_Pulse_Sound_0.wav", FALSE, FALSE);
      break;

    case MILITARY:
      play_sample_using_WAV_cache ("../effects/Fire_Bullet_Military_Sound_0.wav", FALSE, FALSE);
      break;

    case FLASH:
      play_sample_using_WAV_cache ("../effects/Fire_Bullet_Flash_Sound_0.wav", FALSE, FALSE);
      break;

    case EXTERMINATOR:
      play_sample_using_WAV_cache ("../effects/Fire_Bullet_Exterminator_Sound_0.wav", FALSE, FALSE);
      break;

    case LASER_RIFLE:
      play_sample_using_WAV_cache ("../effects/phaser.wav", FALSE, FALSE);
      break;

    case SINGLE_LASER:
      play_sample_using_WAV_cache ("../effects/Fire_Bullet_Single_Laser_Sound_0.wav", FALSE, FALSE);
      break;

    case PLASMA_PISTOL:
      play_sample_using_WAV_cache ("../effects/Fire_Bullet_Plasma_Pistol_Sound_0.wav", FALSE, FALSE);
      break;

    case LASER_SWORD_1:
    case LASER_AXE:
    case LASER_SWORD_2:
    default:
      play_melee_weapon_missed_sound ();
      break;
    }
}


/* ----------------------------------------------------------------------
 * For the takeover game, there are 4 main sounds.  We handle them from
 * the cache, even if that might also be possible as 'once_needed' type
 * sound samples...
 * ---------------------------------------------------------------------- */
void
Takeover_Set_Capsule_Sound (void)
{
  play_sample_using_WAV_cache ("../effects/TakeoverSetCapsule_Sound_0.wav", FALSE, FALSE);
}

void
Takeover_Game_Won_Sound (void)
{
  play_sample_using_WAV_cache ("../effects/Takeover_Game_Won_Sound_0.wav", FALSE, FALSE);
}

void
Takeover_Game_Deadlock_Sound (void)
{
  play_sample_using_WAV_cache ("../effects/Takeover_Game_Deadlock_Sound_0.wav", FALSE, FALSE);
}

void
Takeover_Game_Lost_Sound (void)
{
  play_sample_using_WAV_cache ("../effects/Takeover_Game_Lost_Sound_0.wav", FALSE, FALSE);
}



/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void
BounceSound (void)
{
  play_sample_using_WAV_cache ("../effects/Collision_Sound_0.wav", FALSE, FALSE);
}


/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
DruidBlastSound (void)
{
  play_sample_using_WAV_cache ("../effects/Blast_Sound_0.wav", FALSE, FALSE);
}

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
ExterminatorBlastSound (void)
{
  play_sample_using_WAV_cache ("../effects/Blast_Sound_0.wav", FALSE, FALSE);
}

/* ----------------------------------------------------------------------
 * 
 *
 * ---------------------------------------------------------------------- */
void
PlayLevelCommentSound (int levelnum)
{
  switch (levelnum)
    {
    case 0:
      // I've been away for a far too long time it seems...
      break;
    case 3:
      play_sample_using_WAV_cache ("../effects/MSMachinesClose_0.wav", FALSE, FALSE);
      break;
    default:
      break;
    };
}


/* ----------------------------------------------------------------------
 * Whenever an enemy is hit by the tux with a melee weapon, then the
 * following sound is played...
 * ---------------------------------------------------------------------- */
void
PlayEnemyGotHitSound (int enemytype)
{
  switch (enemytype)
    {
    case -1:
      // Don't play anything at all...
      break;
    case 0:
      // Play a grunting enemy got hit sound...
      play_sample_using_WAV_cache ("../effects/Enemy_Got_Hit_Sound_0.wav", FALSE, FALSE);
      break;
    default:
      break;
    };
}


/* ----------------------------------------------------------------------
 * This function plays a sound of a bullet being reflected. It is only
 * used, when a bullets is compensated by the tux armour.
 * ---------------------------------------------------------------------- */
void
BulletReflectedSound (void)
{
  play_sample_using_WAV_cache ("../effects/Bullet_Reflected_Sound_0.wav", FALSE, FALSE);
}


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
