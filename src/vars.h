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

/* 
 *  _Definitions_ of global variables
 * This file should only be included in main.c, and 
 * the variable _declarations_ should be made in global.h under _main_c
 *
 */


#include "colodefs.h"

char* floor_tile_filenames [ ALL_ISOMETRIC_FLOOR_TILES ] =
  {
    "iso_miscellaneous_floor_0000.png" , 
    "iso_miscellaneous_floor_0001.png" , 
    "iso_miscellaneous_floor_0002.png" , 
    "iso_miscellaneous_floor_0003.png" , 
    "iso_miscellaneous_floor_0004.png" , 
    "iso_miscellaneous_floor_0005.png" , 
    "iso_grass_floor_0000.png" , 
    "iso_grass_floor_0001.png" , 
    "iso_grass_floor_0002.png" , 
    "iso_grass_floor_0003.png" , 
    "iso_grass_floor_0004.png" , 
    "iso_grass_floor_0005.png" , 
    "iso_grass_floor_0006.png" , 
    "iso_grass_floor_0007.png" , 
    "iso_grass_floor_0008.png" , 
    "iso_grass_floor_0009.png" , 
    "iso_grass_floor_0010.png" , 
    "iso_grass_floor_0011.png" , 
    "iso_grass_floor_0012.png" , 
    "iso_grass_floor_0013.png" , 
    "iso_grass_floor_0014.png" , 
    "iso_grass_floor_0015.png" , 
    "iso_grass_floor_0016.png" , 
    "iso_grass_floor_0017.png" , 
    "iso_grass_floor_0018.png" , 
    "iso_grass_floor_0019.png" , 
    "iso_grass_floor_0020.png" , 
    "iso_grass_floor_0021.png" , 
    "iso_grass_floor_0022.png" , 
    "iso_grass_floor_0023.png" , 
    "iso_miscellaneous_floor_0006.png" , 
    "iso_miscellaneous_floor_0007.png" , 
    "iso_miscellaneous_floor_0008.png" , 
    "iso_grass_floor_0024.png" , 
    "iso_grass_floor_0025.png" ,
    "iso_grass_floor_0026.png" , 
    "iso_grass_floor_0027.png" , 
    "iso_grass_floor_0028.png" ,

    "iso_miscellaneous_floor_0009.png" ,
    "iso_miscellaneous_floor_0010.png" ,
    "iso_miscellaneous_floor_0011.png" ,
    "iso_miscellaneous_floor_0012.png" ,
    "iso_miscellaneous_floor_0013.png" ,
    "iso_miscellaneous_floor_0014.png" ,
    "iso_miscellaneous_floor_0015.png" ,
    "iso_miscellaneous_floor_0016.png" ,
    "iso_miscellaneous_floor_0017.png" ,
    "iso_miscellaneous_floor_0018.png" ,
    "iso_miscellaneous_floor_0019.png" ,
    "iso_miscellaneous_floor_0020.png" ,
    "iso_miscellaneous_floor_0021.png" ,
    "iso_miscellaneous_floor_0022.png" , 

    "iso_sidewalk_0001.png" ,
    "iso_sidewalk_0002.png" ,
    "iso_sidewalk_0003.png" ,
    "iso_sidewalk_0004.png" ,
    "iso_sidewalk_0005.png" ,
    "iso_sidewalk_0006.png" ,
    "iso_sidewalk_0007.png" ,
    "iso_sidewalk_0008.png" ,
    "iso_sidewalk_0009.png" ,
    "iso_sidewalk_0010.png" ,
    "iso_sidewalk_0011.png" ,
    "iso_sidewalk_0012.png" ,
    "iso_sidewalk_0013.png" ,
    "iso_sidewalk_0014.png" ,
    "iso_sidewalk_0015.png" ,
    "iso_sidewalk_0016.png" ,
    "iso_sidewalk_0017.png" ,
    "iso_sidewalk_0018.png" ,
    "iso_sidewalk_0019.png" ,
    "iso_sidewalk_0020.png" ,

    "iso_miscellaneous_floor_0023.png" ,
    
    "iso_sand_floor_0001.png" , 
    "iso_sand_floor_0002.png" ,
    "iso_sand_floor_0003.png" ,
    "iso_sand_floor_0004.png" ,
    "iso_sand_floor_0005.png" ,
    "iso_sand_floor_0006.png" ,

    // , "ERROR_UNUSED.png" , 
  };

tux_t Me[ MAX_PLAYERS ] ;

network_tux_t NetworkMe[ MAX_PLAYERS ];

char *InfluenceModeNames[] = {
  N_("Mobile"),
  N_("Transfer"),
  N_("Weapon"),
  N_("Captured"),
  N_("Complete"),
  N_("Rejected"),
  N_("Logged In"),
  N_("Debriefing"),
  N_("Terminated"),
  N_("Pause"),
  N_("Cheese"),
  N_("Elevator"),
  N_("Briefing"),
  N_("Menu"),
  N_("Fresh Net"),
  N_("Name Tran"),
  NULL
};

// Debug string, no need for translation
char *NetworkClientStatusNames[] = {
  "Network ERROR" ,
  "Unconnected" , 
  "Connection freshly opened" ,
  "Name has been transmitted" , 
  "Game on" ,
  NULL
};


char *Classname[] = {
  N_("Influence device"),
  N_("Disposal robot"),
  N_("Servant robot"),
  N_("Messenger robot"),
  N_("Maintenance robot"),
  N_("Crew droid"),
  N_("Sentinel droid"),
  N_("Battle droid"),
  N_("Security droid"),
  N_("Command Cyborg"),
  N_("Peaceful Human"),
  N_("Cold MS Force"),
  N_("Mrs. Stone, Shopowner"),
  N_("Resistance Founder"),
  N_("Town Healer"),
  N_("Sorenson, Master of Magic"), // 15
  N_("Pendragon, Rebel Fighter"),
  N_("Dixon, Teleporter Service Man"),
  N_("Chandra, Tourist Guide"),
  NULL
};

char *Classes[] = {
  N_("influence"),
  N_("disposal"),
  N_("servant"),
  N_("messenger"),
  N_("maintenance"),
  N_("crew"),
  N_("sentinel"),
  N_("battle"),
  N_("security"),
  N_("command"),
  N_("error")
};

char *Drivenames[] = {
  N_("none"),
  N_("tracks"),
  N_("anti-grav"),
  N_("tripedal"),
  N_("wheels"),
  N_("bipedal"),
  N_("error")
};

char *Sensornames[] = {
  N_(" - "),
  N_("spectral"),
  N_("infra-red"),
  N_("subsonic"),
  N_("ultra-sonic"),
  N_("radar"),
  N_("error")
};

char *Brainnames[] = {
  N_("none"),
  N_("neutronic"),
  N_("primode"),
  N_("error")
};

char *Weaponnames[] = {
  N_("none"),
  N_("lasers"),
  N_("laser rifle"),
  N_("disruptor"),
  N_("exterminator"),
  N_("error")
};

char* AllSkillTexts [ NUMBER_OF_SKILL_LEVELS ] =
  {
    N_("novice"),
    N_("average"),
    N_("experienced"),
    N_("skilled"),
    N_("adept"),
    N_("masterful"),
    N_("inhuman"),
    N_("god-like"),
    N_("super-god-like"),
    N_("wicked sick")
  };

//--------------------
// When a character aquires better melee_weapon_skill for melee
// weapons or better ranged_weapon_skill for ranged weapons, this
// will affect (his chance to hit and also) the damage the player
// does, by applying a multiplier to the normal computed damage.
// These multipliers are given by the table below.
//
float MeleeDamageMultiplierTable [  ] =
  {  1.0 , 1.5 , 2.0 , 2.5 , 3.0 , 3.5 , 4.0 , 4.5 , 5.0 } ;
float MeleeRechargeMultiplierTable [  ] =
  {  1.0 , 0.8 , 0.6 , 0.5 , 0.4 , 0.2 , 0.15 , 0.10 , 0.05 } ;

float RangedDamageMultiplierTable [  ] =
  {  1.0 , 1.5 , 2.0 , 2.5 , 3.0 , 3.5 , 4.0 , 4.5 , 5.0} ;
float RangedRechargeMultiplierTable [  ] =
  {  1.0 , 0.8 , 0.6 , 0.5 , 0.4 , 0.2 , 0.15 , 0.10 , 0.05 } ;

//--------------------
// In the game, many spells are still internally similar
// to bullets flying around.  But what hit chance shall this
// bullet have?  --  That will depend upon the skill level
// of the caster.  The details will be taken from the following
// table:
//
int SpellHitPercentageTable [  ] =
  {  50 , 70 , 90 , 110 , 130 , 150 , 190 , 230 , 270 } ;

spell_skill_spec *SpellSkillMap; 
int number_of_skills;
/*[ number_of_skills ] = {

  { 1 , "Force -> Energy "       , "FirstAidSpell.png"         , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 26 , 35 , 45 , 55 , 65 , 70 , 75 , 80 , 85 , 999 } ,
  "This is maybe the most vital spell/skill of them all.  With this spell, you can regain some of your health lost in combat for a comparatively little cost of mana." } ,

  { 1 , "Teleport Old Town" , "TeleportHomeSpell.png"     , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 26 , 35 , 45 , 55 , 65 , 70 , 75 , 80 , 85 , 999 } ,
  "This is spell allows the caster to immediately teleport to the central square of the old town.  To do this, select this skill and right click the mouse once.  It should take effect immediately." } ,

  { 1 , "Plasma Bolt"             , "PlasmaBoltSpell.png"       , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 30 , 45 , 55 , 65 , 70 , 75 , 80 , 85 , 90 , 999 } ,
  "This is a very simple combat spell.  It fires a plasma bolt into the direction the caster is facing.  This spell does not have as much effect as some ranged weapons, but the good thing about it is that it doesn't require any ammo as well, except perhaps for some mana." } ,

  { 1 , "Calculate PI"               , "Calculate_PI.png"        , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 30 , 45 , 55 , 65 , 70 , 75 , 80 , 85 , 90 , 999 } ,
  "This program allows you to execute a calculate PI script on the target, causing them to to slow down/stop their motions for an amount of time. Dumb MS low-level programming, they think they can actually compute it." } ,

  { 1 , "Explosion Circle"       , "ExplosionCircleSpell.png"  , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 28 , 30 , 35 , 45 , 55 , 65 , 70 , 75 , 80 , 999 } ,
  "This is a very simple skill, invented more for training purposes than for actual combat situations.  It will create a circle of explosions right around the caster, but they don't do very much damage.  It's also a bit dangerous to use this spell while moving fast, cause then the caster might get himself hurt in the explosions he creates." } ,

  { 1 , "Poison Bolt"            , "PoisonBoltSpell.png"       , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 30 , 45 , 55 , 65 , 70 , 75 , 80 , 85 , 90 , 999 } ,
  "This spell creates a poisonous and acidric bolt that affects biological and mechanic entities.  The poison/acid combination will do some damage over time.  While one of these bolts is not overly effective, the effect of multiple such bots fired in rapid succession is not to be underestimated, cause damage AND duration add up, therefore doing more damage with the square of bold fired." } ,

  { 1 , "Petrification"          , "StoningBoltSpell.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 30 , 45 , 55 , 65 , 70 , 75 , 80 , 85 , 90 , 999 } ,
  "This spell creates simple crystaline bolts, that can cause a machine or biological unit to completely freeze for a certain time." } ,

  { 1 , "Detect Stuff"           , "DetectItemsSpell.png"        , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 29 , 35 , 45 , 55 , 65 , 70 , 75 , 80 , 85 , 999 } ,
  "This is spell will allow the caster to immediately detect all items not contained in any chest or dead body.  They will appear in the automatic map as yellow dots." } ,

  { 1 , "Radial EMP Wave"        , "RadialEMPWaveSpell.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 45 , 55 , 65 , 70 , 75 , 80 , 85 , 90 , 100 , 999 } ,
  "This spell creates a rapidly spreading radial electro magnetic pulse around the caster.  It can be highly effective against large numbers of weak enemies." } ,

  { 1 , "Radial VMX Wave"        , "RadialVMXWaveSpell.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 45 , 55 , 65 , 70 , 75 , 80 , 85 , 90 , 100 , 999 } ,
  "This spell creates a rapidly spreading radial wave of VMX poison gas.  While very harmful to any biological life forms it can also do some damage to organic parts build into certain droids.  This spell can also be highly effective against large numbers of weak enemies." } ,

  { 1 , "Radial Plasma Wave"     , "RadialPlasmaWaveSpell.png"   , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 45 , 55 , 65 , 70 , 75 , 80 , 85 , 90 , 100 , 999 } ,
  "This spell creates a rapidly spreading radial wave of VMX poison gas.  While very harmful to any biological life forms it can also do some damage to organic parts build into certain droids.  This spell can also be highly effective against large numbers of weak enemies." } ,

  { 1 , "Ex. Plasma Transistor"  , "ExtractPlasmaTransistor.png" , UNLOADED_ISO_IMAGE , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This skill represents the knowledge of how to extract certain parts, such as plasma transistors from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any plasma transistors that can still be used will automatically appear in the dead bodys inventory." } ,
  
  { 1 , "Extract Superconductor" , "ExtractSuperconductor.png"      , UNLOADED_ISO_IMAGE , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This skill represents the knowledge of how to extract certain parts, such as superconducting relays from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any superconductor that can still be used will automatically appear in the dead bodys inventory." } ,

  { 1 , "Ex. Antim. Converter"   , "ExtractAntimatterConverter.png" , UNLOADED_ISO_IMAGE , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This skill represents the knowledge of how to extract certain parts, such as Antimatter-matter converters from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any converters that can still be used will automatically appear in the dead bodys inventory." } ,

  { 1 , "Ex. Entropy Inverter"   , "ExtractEntropyInverter.png"     , UNLOADED_ISO_IMAGE , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This skill represents the knowledge of how to extract certain parts, such as entropy inverters from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any inverters that can still be used will automatically appear in the dead bodys inventory.This is a skill" } ,

  { 1 , "Ex. Tach. Condensator"  , "ExtractTachyonCondensator.png"  , UNLOADED_ISO_IMAGE , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This skill represents the knowledge of how to extract certain parts, such as tachyon condensators from dead bodies of droids.  It is a passive skill, i.e. it is not nescessary to ready this skill in order to use it.  The effect automatically takes place when a dead body of a droid is examined.  Any condensators that can still be used will automatically appear in the dead bodys inventory." } ,

  { 0 , "Force Explosion Ray"    , "ExplosionRaySpell.png"     , UNLOADED_ISO_IMAGE , 
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
    { 28 , 30 , 35 , 45 , 55 , 65 , 70 , 75 , 80 , 999 } ,
  "This is a skill manly used for training purposes.  It is very simple and also of very limited use.  But every adept usually has to learn it sooner or later in the course of his training in making use of the force." } ,


  { 1 , "Unused spell Nr. 22"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 23"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 24"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 25"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 26"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 27"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 28"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 29"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 30"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 31"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 32"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 33"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 34"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,

  { 1 , "Unused spell Nr. 35"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 36"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 37"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 38"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } ,
  { 1 , "Unused spell Nr. 39"    , "NoSkillAvailable.png"      , UNLOADED_ISO_IMAGE , 
    { 6 , 5 , 4 , 3 , 2 , 1 , 1 , 1 , 1 , 1 } ,
    { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ,
  "This is a skill" } };*/

Druidspec Druidmap;

Bulletspec Bulletmap;

blastspec Blastmap[ALLBLASTTYPES];

char font_switchto_red [ 2 ] = { 1 , 0 };
char font_switchto_blue [ 2 ] = { 2 , 0 };
char font_switchto_neon [ 2 ] = { 3 , 0 };

int skip_initial_menus = FALSE ;

