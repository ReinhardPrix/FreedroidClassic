/* 
 *
 *   Copyright (c) 2003 2004 Johannes Prix
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

/* ----------------------------------------------------------------------
 * This file contains almost all the code of the FreedroidRPG items
 * editor, a GTK+ based program to make setting up items for characters
 * in freedroidRPG an easier task.
 * ---------------------------------------------------------------------- */

#define DIALOG_EDITOR

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <getopt.h>

#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <dirent.h>

#include <SDL/SDL.h>
#include "../src/defs.h"
#include "../src/struct.h"
#include "../src/proto.h"

#define MAX_TEXT_ENTRIES_VISIBLE 3
#define WINDOW_WARNINGS_DEBUG 2
#define PAGE_SETUP_DEBUG 2
int Number_Of_Item_Types = 0 ;
itemspec* ItemMap;

char *raw_item_data_from_file;

GtkWidget *wnd;
GtkWidget *hbox;
GtkWidget *vbox;
GtkWidget *status_mode;
GtkWidget *status_help;
GtkWidget *menu;
GtkWidget *submenu;
GtkWidget *menuitem;

//--------------------
// this is for the edit dialog option popup window
//
GtkWidget *window; 
GtkWidget *npc_selection;

GtkWidget *do_something_extra_selection [ MAX_EXTRAS_PER_OPTION ] ;
GtkWidget *do_something_extra_selection_menu [ MAX_EXTRAS_PER_OPTION ] ;

GtkWidget *slot_selection ;
GtkWidget *slot_selection_menu ;
GtkWidget *ammo_type_selection ;
GtkWidget *ammo_type_selection_menu ;

GtkWidget *strength_requirement_entry ;
GtkWidget *dexterity_requirement_entry ;
GtkWidget *magic_requirement_entry ;

GtkWidget *base_duration_entry ;
GtkWidget *duration_modifier_entry ;
GtkWidget *sound_number_entry ;
GtkWidget *picture_number_entry ;
GtkWidget *base_list_price_entry ;


GtkWidget *base_item_gun_damage_entry;
GtkObject *base_item_gun_damage_adjustment;
GtkWidget *item_gun_damage_modifier_entry;
GtkObject *item_gun_damage_modifier_adjustment;
GtkWidget *item_gun_recharging_time_entry;
GtkObject *item_gun_recharging_time_adjustment;
GtkWidget *item_gun_speed_entry;
GtkObject *item_gun_speed_adjustment;

GtkWidget *item_drive_maxspeed_entry;
GtkObject *item_drive_maxspeed_adjustment;
GtkWidget *item_drive_accel_entry;
GtkObject *item_drive_accel_adjustment;

GtkWidget *base_ac_bonus_entry;
GtkObject *base_ac_bonus_adjustment;
GtkWidget *ac_bonus_modifier_entry;
GtkObject *ac_bonus_modifier_adjustment;

GtkWidget *extra_parameter_entry [ MAX_EXTRAS_PER_OPTION ] ;

GtkWidget *on_goto_selection ;
GtkWidget *on_goto_selection_menu ;
GtkWidget *on_goto_parameter_entry ;
GtkObject *adj1, *adj2;

GtkObject *strength_adjustment;
GtkObject *dexterity_adjustment;
GtkObject *magic_adjustment;
GtkObject *base_duration_adjustment;
GtkObject *duration_modifier_adjustment;
GtkObject *picture_number_adjustment;
GtkObject *sound_number_adjustment;
GtkObject *base_list_price_adjustment;
// GtkAdjustment *adj1, *adj2;

GtkWidget *item_name_entry;
GtkWidget *item_description_entry;
GtkWidget *item_3d_model_path_entry;
GtkWidget *item_sound_file_name_entry;
GtkWidget *item_inventory_image_file_name_entry;
GtkWidget *authors_notes_entry;
GtkWidget *disable_button;

GtkWidget *loop;
GtkWidget *always_prior_to_startup;
GtkWidget *can_be_used_like_potion_or_grenade;
GtkWidget *group_together_in_inventory;
GtkWidget *cond_edit;

GtkWidget *item_gun_bullet_pass_through_hit_bodies;
GtkWidget *item_gun_bullet_pass_through_explosions;
GtkWidget *item_gun_bullet_ignore_wall_collisions;
GtkWidget *item_gun_oneshotonly;

//--------------------
// This is for marking options so that new connections between
// to options can be established.
//
static int currently_marked_item_index = (-1);

//--------------------
// This is for the 'meta-tooltips' that pop up if the mouse ever gets
// over one of the dialog option boxes...
//
static int item_covered = (-1);
static GtkWidget *popup_meta_tool_tip = NULL ;

// GtkWidget *hpaned;
GtkWidget *vpaned;
GdkPixmap *icon;
GdkBitmap *mask;
GtkAccelGroup *accel_group;

//--------------------
// Here come some variables from the 'gui_graph' module...
//
GtkWidget *graph;       // Drawing Area
GdkPixmap *surface;     // Drawing surface

//--------------------
// This we need for the file open/load/save dialogs...
//
GtkWidget *filew;

int currently_mouse_grabbed_option = (-1) ;
char LastUsedFileName[10000] = "UNDEFINED_FILENAME.dialog" ;

typedef struct
{
  char* extra_identifier;
  int do_extra_menu_index;
  char parameter_termination_char;
} 
known_do_extra, *Known_do_extra;

#define ALL_KNOWN_EXTRAS 25

known_do_extra extra_map [ ALL_KNOWN_EXTRAS ] =
  {
    { "UNUSED EXTRA", 0 , '\0' } , 
    { "BreakOffAndBecomeHostile",  1 , '\0' } , 
    { "IncreaseMeleeWeaponSkill", 2 , '\0' } , 
    { "IncreaseRangedWeaponSkill", 3 , '\0' } , 
    { "IncreaseSpellcastingSkill", 4 , '\0' } , 
    { "IncreaseExtractPlasmaTransistorSkill", 5 , '\0' } , 
    { "IncreaseExtractConverterSkill", 6 , '\0' } , 
    { "IncreaseExtractInverterSkill", 7 , '\0' } , 
    { "IncreaseExtractCondensatorSkill", 8 , '\0' } , 
    { "GiveItem:", 9 , '\0' } , 
    { "ExecuteActionWithLabel:", 10 , '\0' } , 
    { "PlantCookie:", 11 , ':' } , 
    { "InitTradeWithCharacter:", 12 , '\0' } , 
    { "AssignMission:", 13 , '\0' } , 
    { "MarkMissionComplete:" , 14 , '\0' } ,
    { "AddExperienceBonus:" , 15 , '\0' } , 
    { "AddBigScreenMessageBUT_WITH_TERMINATION_CHARACTER_PLEASE:" , 16 , ':' } , 
    { "AddBaseMagic:" , 17 , '\0' } , 
    { "AddBaseDexterity:" , 18 , '\0' } , 
    { "SubtractPointsToDistribute:" , 19 , '\0' } , 
    { "SubtractGold:" , 20 , '\0' } , 
    { "AddGold:" , 21 , '\0' } , 
    { "DeleteAllInventoryItemsWithCode:" , 22 , '\0' } , 
    { "DeleteAllInventoryItemsOfType:" , 23 , '\0' } ,
    { "ExecuteSubdialog:", 24 , '\0' } 
  };

typedef struct
{
  char* on_goto_identifier;
  int on_goto_menu_index;
  char parameter_termination_char;
} 
known_on_goto_condition, *Known_on_goto_condition;

typedef struct
{
  char* slot_identifier;
  int slot_index;
} 
known_slot, *Known_slot;

typedef struct
{
  char* ammo_type_identifier;
  int ammo_type_index;
} 
known_ammo_type, *Known_ammo_type;

#define ALL_KNOWN_ON_GOTO_CONDITIONS 8

known_on_goto_condition on_goto_map [ ALL_KNOWN_ON_GOTO_CONDITIONS ] =
  {
    { "UNUSED CONDITION" , 0 , '\0' } , 
    { "MissionComplete:" , 1 , '\0' } , 
    { "PointsToDistributeAtLeast:" , 2 , '\0' } ,
    { "GoldIsLessThan:" , 3 , '\0' } , 
    { "MeleeSkillLesserThan:" , 4 , '\0' } , 
    { "CookieIsPlanted:" , 5 , ':' } ,
    { "MissionAssigned:" , 6 , '\0' } , 
    { "HaveItemWithCode:" , 7 , '\0' } , 
  };

#define ALL_KNOWN_SLOTS 7

known_slot slot_map [ ALL_KNOWN_ON_GOTO_CONDITIONS ] =
  {
    { "none"    , 0   } , 
    { "weapon"  , 1   } , 
    { "drive"   , 2   } , 
    { "armour"  , 3   } ,
    { "shield"  , 4   } , 
    { "special" , 5   } ,
    { "aux"     , 6   } 
  };

#define ALL_KNOWN_AMMO_TYPES 4

known_ammo_type ammo_type_map [ ALL_KNOWN_AMMO_TYPES ] =
  {
    { "none"    , 0   } , 
    { "laser"  , 1   } , 
    { "exterminator"   , 2   } , 
    { "plasma"  , 3   } ,
  };


#include "ItemEditor.h"


#define BOX_RADIUS_X 14.0
#define BOX_RADIUS_Y 14.0

enum
{
    GC_RED          = 0,
    GC_DARK_RED     = 1,
    GC_DARK_BLUE    = 2,
    GC_GREEN        = 3,
    GC_DARK_GREEN   = 4,
    GC_YELLOW       = 5,
    GC_GREY         = 6,
    GC_BLACK        = 7,
    GC_WHITE        = 8,
    MAX_GC          = 9
};

GdkGC *Gui_Color[MAX_GC];

void clear_item_archetype_array ( void );

//--------------------
// We add this dummy, so that dialog editor and item editor
// will properly compile...
//
void
load_item_surfaces_for_item_type ( int item_type )
{

};
void
print_trace ( int signum )
{
};

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
enforce_authors_notes ( void )
{
  char* tmp_string;

  //--------------------
  // At first we write the options text back into the option variable
  //
  tmp_string = 
    gtk_editable_get_chars( GTK_EDITABLE ( authors_notes_entry ) , 0, -1 );
  strcpy ( authors_notes , tmp_string );
  
}; // void enforce_authors_notes ( void) 

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
char* 
remove_termination_character_if_nescessary ( char* our_string , char term_char )
{
  int our_strlen;

  our_strlen = strlen ( our_string );
  if ( our_strlen == 0 ) return ( our_string ) ;

  if ( term_char != '\0' )
    {
      
      DebugPrintf ( 1 , "\nchar* remove_termination_character_if_nescessary (...): function call confirmed." );
      DebugPrintf ( 1 , "\nString given: '%s'." , our_string );
      DebugPrintf ( 1 , "\nLast char : '%c' as number: %d." , our_string[ our_strlen - 1 ] , (int) our_string[ our_strlen-1 ] );
      DebugPrintf ( 1 , "\nTermination character target: '%c'." , term_char );

      if ( our_string [ our_strlen - 1 ] == term_char ) 
	{
	  DebugPrintf ( 1 , "\nTERMINATION CHARACTER '%c' HAS BEEN REMOVED FROM STRING." , term_char );
	  our_string [ our_strlen - 1 ] = 0 ;
	}

    }

  return ( our_string );

}; // char* remove_termination_character_if_nescessary ( char* our_string , char term_char )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
char* 
add_termination_character_if_nescessary ( char* our_string , char term_char )
{
  int our_strlen;
  char* temp_string;

  our_strlen = strlen ( our_string );
  if ( our_strlen == 0 ) return ( our_string ) ;

  if ( term_char != '\0' )
    {
      
      DebugPrintf ( 1 , "\nchar* add_termination_character_if_nescessary (...): function call confirmed." );
      DebugPrintf ( 1 , "\nString given: '%s'." , our_string );
      DebugPrintf ( 1 , "\nLast char : '%c' as number: %d." , our_string[ our_strlen - 1 ] , (int) our_string[ our_strlen-1 ] );
      DebugPrintf ( 1 , "\nTermination character target: '%c'." , term_char );

      if ( our_string [ our_strlen - 1 ] != term_char ) 
	{
	  temp_string = malloc ( our_strlen + 10 );
	  strcpy ( temp_string , our_string );
	  temp_string [ our_strlen ] = term_char ;
	  temp_string [ our_strlen + 1 ] = 0 ;
	  DebugPrintf ( 1 , "\nTERMINATION CHARACTER '%c' HAS BEEN ADDED TO STRING." , term_char );
	  return ( temp_string );
	}
      
    }

  return ( our_string );
  
}; // char* add_termination_character_if_nescessary ( char* our_string , char term_char )

/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void
gui_start_pure_text_popup_window ( char* text_to_be_displayed , char* window_title )
{
  GtkWidget *vbox1;
  GtkWidget *ok_button;
  GtkWidget *scrolledwindow;
  GtkWidget *hbuttonbox1;
  GtkWidget *frame1;
  // GtkTooltips *tooltips = gtk_tooltips_new ();
  GtkWidget *used_text;

  //--------------------
  // At first we say, that we are here...
  //
  DebugPrintf ( 1 , "\ngui_start_pure_text_popup_window ( ... ): function call confirmed. " );
  
  //--------------------
  // Now we destroy that awful little window, that might still
  // be cluttering part of the screen... how annoying...
  //
  gui_destroy_existing_tooltips (  );
  
  // the "Edit Item Node" window
  window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (window), "window", window);
  gtk_widget_set_usize (window, 560, 360);
  gtk_window_set_title (GTK_WINDOW (window), window_title );
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
  gtk_window_set_modal (GTK_WINDOW (window), TRUE);
  gtk_window_set_policy (GTK_WINDOW (window), FALSE, FALSE, FALSE);

  gtk_container_set_border_width (GTK_CONTAINER ( window ), 24);  

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox1", vbox1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window), vbox1);
  
  // the entry for annotations
  frame1 = gtk_frame_new ("Text message:");
  gtk_widget_ref (frame1);
  gtk_object_set_data_full (GTK_OBJECT (window), "frame1", frame1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);
  
  //--------------------
  // Now at this point it would make sense to create a new text entry
  // and then pack this text entry into the hbox1 as well, right?
  //
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow);
  gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow4", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow);
  // gtk_container_add (GTK_CONTAINER ( vbox1 ), scrolledwindow);
  gtk_container_add (GTK_CONTAINER ( frame1 ), scrolledwindow);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 4);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  used_text = gtk_text_new ( NULL , NULL );
  gtk_widget_ref (used_text);
  gtk_object_set_data_full (GTK_OBJECT (window), "option_text_entered_here_entry", used_text, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (used_text);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), used_text);
  // gtk_tooltips_set_tip (tooltips, used_text, "This text field contains the additional parameter to this 'DoSomethingExtra' command string.", NULL);
  gtk_text_set_editable (GTK_TEXT (used_text), TRUE);

  gtk_text_insert ( GTK_TEXT ( used_text ), used_text->style->font,
		    &used_text->style->black, &used_text->style->white,
		    text_to_be_displayed , -1);


  //--------------------
  // And now the final button row, that will contain the ok and cancel
  // buttons, that are always visible, no matter what page is selected.
  //
  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbuttonbox1", hbuttonbox1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox1), 0);
  
  // ok button
  ok_button = gtk_button_new_with_label ("OK");
  gtk_widget_ref (ok_button);
  gtk_object_set_data_full (GTK_OBJECT (window), "ok_button", ok_button, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), ok_button);
  GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);
  
  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked", GTK_SIGNAL_FUNC ( edit_interface_cancel_button_pressed ), window   );
  gtk_signal_connect (GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC ( edit_interface_cancel_button_pressed ), window );
  
  // give focus to the text entry
  gtk_widget_grab_focus ( item_description_entry );
  gtk_widget_grab_default (item_description_entry );
  // gtk_object_set_data (GTK_OBJECT (window), "tooltips", tooltips);
  
  //--------------------
  // Clear?
  //
  gtk_widget_show ( window );
  gtk_main();
  
}; // void gui_start_pure_text_popup_window ( char* text_to_be_displayed )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
int
item_index_is_looping ( int item_index )
{
  int i ;

  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      if ( ( ChatRoster [ item_index ] . change_option_nr [ i ] == item_index ) &&
	   ( ChatRoster [ item_index ] . change_option_to_value [ i ] != 0 ) )
	{
	  return ( TRUE ) ;
	}
    }

  return ( FALSE );

}; // int item_index_is_looping ( int item_index )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
item_index_set_loop_status ( int item_index , int we_shall_loop )
{
  int i ;
  int we_re_done = FALSE ;

  //--------------------
  // At first we see if maybe we can set the loop status by just 
  // modifying an existing loop entry (or several of them)
  //
  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      if ( ChatRoster [ item_index ] . change_option_nr [ i ] == item_index )
	{
	  ChatRoster [ item_index ] . change_option_to_value [ i ] = we_shall_loop ; 
	  we_re_done = TRUE ;
	}
    }
  //--------------------
  // Maybe that already did the trick.  Then of course we can return
  // now.
  //
  if ( we_re_done ) return ;
  
  //--------------------
  // So now we know, that we must create a new self-connection.  Shouldn't be
  // hard to do...
  //
  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      if ( ChatRoster [ item_index ] . change_option_nr [ i ] == (-1) )
	{
	  ChatRoster [ item_index ] . change_option_to_value [ i ] = we_shall_loop ; 
	  return ;
	}
    }

}; // int item_index_is_looping ( int item_index )


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
make_index_from_real_extra ( char* real_extra_text )
{
  int i;

  //--------------------
  // Maybe the given extra isn't realy in use.  Then we'll just
  // return the 'UNUSED EXTRA' index, which is 0, right now at least I hope.
  //
  if ( strlen ( real_extra_text ) == 0 )
    return ( 0 ) ;

  //--------------------
  // So if a real string was given, we see if maybe one of the
  // extra identifiers is completely contained within the real extra text,
  // that may also contain a parameter behind that of course.
  //
  for ( i = 0 ; i < ALL_KNOWN_EXTRAS ; i ++ )
    {
      if ( strncmp ( real_extra_text , extra_map [ i ] . extra_identifier , 
		     strlen ( extra_map [ i ] . extra_identifier ) ) == 0 )
	{
	  DebugPrintf ( 1 , "\nExtra text :'%s', was identified with '%s', resulting in index %d." ,
			real_extra_text , extra_map [ i ] . extra_identifier , i );
	  return ( i );
	} 
    }

  DebugPrintf ( 0 , "\nExtra text :'%s', COUND NOT BE IDENTIFIED CORRECTLY!!  Terminating...." , real_extra_text );
  Terminate ( ERR );
  return ( -1 );

}; // int make_index_from_real_extra ( char* real_extra_text )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int
make_index_from_real_on_goto ( char* real_on_goto_text )
{
  int i;

  //--------------------
  // Maybe the given on_goto isn't realy in use.  Then we'll just
  // return the 'UNUSED ON_GOTO' index, which is 0, right now at least I hope.
  //
  if ( strlen ( real_on_goto_text ) == 0 )
    return ( 0 ) ;

  //--------------------
  // So if a real string was given, we see if maybe one of the
  // on_goto identifiers is completely contained within the real on_goto text,
  // that may also contain a parameter behind that of course.
  //
  for ( i = 0 ; i < ALL_KNOWN_ON_GOTO_CONDITIONS ; i ++ )
    {
      if ( strncmp ( real_on_goto_text , on_goto_map [ i ] . on_goto_identifier , 
		     strlen ( on_goto_map [ i ] . on_goto_identifier ) ) == 0 )
	{
	  DebugPrintf ( 1 , "\nOn_Goto text :'%s', was identified with '%s', resulting in index %d." ,
			real_on_goto_text , on_goto_map [ i ] . on_goto_identifier , i );
	  return ( i );
	} 
    }

  DebugPrintf ( 0 , "\nOn_Goto text :'%s', COUND NOT BE IDENTIFIED CORRECTLY!!  Terminating...." , real_on_goto_text );
  Terminate ( ERR );
  return ( -1 );

}; // int make_index_from_real_on_goto ( char* real_on_goto_text )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
char*
make_parameter_from_real_extra ( char* real_extra_text )
{
  int identifier_index;
  char* return_string;

  //--------------------
  // Maybe the given extra isn't realy in use.  Then we'll just
  // return the empty string as parameter.
  //
  if ( strlen ( real_extra_text ) == 0 )
    return ( "" ) ;

  //--------------------
  // First we get the type of extra we have been given...
  //
  identifier_index = make_index_from_real_extra ( real_extra_text );

  //--------------------
  // If the extra consists only of it's identifier, then the thing is
  // easy, cause that means that there are no parameters.
  //
  if ( strlen ( real_extra_text ) <= strlen ( extra_map [ identifier_index ] . extra_identifier ) )
    return ( "" );

  //--------------------
  // So now we know, that the given string does indeed have a parameter.
  // We'll make a new string (allocated!!) out of that and return it, not
  // caring about any potential memory leak here..
  //
  return_string = malloc ( strlen ( real_extra_text ) + 2 );

  strcpy ( return_string , 
	   real_extra_text + strlen ( extra_map [ identifier_index ] . extra_identifier ) );

  return ( return_string );
	   
}; // int make_parameter_from_real_extra ( char* real_extra_text )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
char*
make_parameter_from_real_on_goto ( char* real_on_goto_text )
{
  int identifier_index;
  char* return_string;

  //--------------------
  // Maybe the given on_goto isn't realy in use.  Then we'll just
  // return the empty string as parameter.
  //
  if ( strlen ( real_on_goto_text ) == 0 )
    return ( "" ) ;

  //--------------------
  // First we get the type of on_goto we have been given...
  //
  identifier_index = make_index_from_real_on_goto ( real_on_goto_text );

  //--------------------
  // If the on_goto consists only of it's identifier, then the thing is
  // easy, cause that means that there are no parameters.
  //
  if ( strlen ( real_on_goto_text ) <= strlen ( on_goto_map [ identifier_index ] . on_goto_identifier ) )
    return ( "" );

  //--------------------
  // So now we know, that the given string does indeed have a parameter.
  // We'll make a new string (allocated!!) out of that and return it, not
  // caring about any potential memory leak here..
  //
  return_string = malloc ( strlen ( real_on_goto_text ) + 2 );

  strcpy ( return_string , 
	   real_on_goto_text + strlen ( on_goto_map [ identifier_index ] . on_goto_identifier ) );

  return ( return_string );
	   
}; // int make_parameter_from_real_on_goto ( char* real_on_goto_text )

/* ----------------------------------------------------------------------
 * This function is used to handle the 'ok' button in the popup window
 * that appears when middle-mouse clicking onto one of the dialog options.
 * This is nescessary, since it seems not to be enough to just close the
 * window, but we must also write the window content back into the 
 * appropriate variables to the changes really take effect...
 * And then we must make the popup window go away.
 * ---------------------------------------------------------------------- */
void 
edit_interface_ok_button_pressed ( GtkWidget *w, int *item_index_pointer )
{
  int item_index;
  int extra_index;
  //--------------------
  // These things we'll need to squeeze the data out of the
  // selection menu for extras...
  //
  GtkMenu *m;
  GtkMenuItem *extra_item;

  //--------------------
  // At first we say, that we're here...
  //
  DebugPrintf ( 1 , "\nedit_interface_ok_button_pressed: Function call confirmed. " );

  item_index = * item_index_pointer ;
  DebugPrintf ( 1 , "\nWriting data back into option Nr. %d." , item_index );

  //--------------------
  // At first we write the options text back into the option variable
  //
  ItemMap [ item_index ] . item_name = 
    gtk_editable_get_chars( GTK_EDITABLE ( item_name_entry ) , 0, -1 );

  //--------------------
  // Now we write the options sample file name back into the option variable
  //
  ItemMap [ item_index ] . item_rotation_series_prefix = 
    gtk_editable_get_chars( GTK_EDITABLE ( item_3d_model_path_entry ) , 0, -1 );

  //--------------------
  // Now we write the item description back into the option variable
  //
  ItemMap [ item_index ] . item_description = 
    gtk_editable_get_chars( GTK_EDITABLE ( item_description_entry ) , 0, -1 );

  //--------------------
  // Now we write the item drop sound sample file name back into the option variable
  //
  ItemMap [ item_index ] . item_drop_sound_file_name = 
    gtk_editable_get_chars( GTK_EDITABLE ( item_sound_file_name_entry ) , 0, -1 );

  //--------------------
  // Now we write the item inventory image file name back into the option variable
  //
  ItemMap [ item_index ] . item_inv_file_name = 
    gtk_editable_get_chars( GTK_EDITABLE ( item_inventory_image_file_name_entry ) , 0, -1 );

  //--------------------
  // Now we extract the equipment slot from the widgets
  //
  m = (GtkMenu *) gtk_option_menu_get_menu ( GTK_OPTION_MENU ( slot_selection ) );
  extra_item = (GtkMenuItem *) gtk_menu_get_active ( m ) ;
  extra_index = * ( (int*) gtk_object_get_user_data ( GTK_OBJECT ( extra_item ) ) ) ; 
  DebugPrintf ( 0 , "\n Final index, that was extracted from the currently selected equipment slot: %d.",
		extra_index );

  ItemMap [ item_index ] . item_can_be_installed_in_weapon_slot  = FALSE ;
  ItemMap [ item_index ] . item_can_be_installed_in_drive_slot   = FALSE ;
  ItemMap [ item_index ] . item_can_be_installed_in_armour_slot  = FALSE ;
  ItemMap [ item_index ] . item_can_be_installed_in_shield_slot  = FALSE ;
  ItemMap [ item_index ] . item_can_be_installed_in_special_slot = FALSE ;
  ItemMap [ item_index ] . item_can_be_installed_in_aux_slot     = FALSE ;

  switch ( extra_index )
    {
    case 1:
      ItemMap [ item_index ] . item_can_be_installed_in_weapon_slot = TRUE ;
      break;
    case 2:
      ItemMap [ item_index ] . item_can_be_installed_in_drive_slot = TRUE ;
      break;
    case 3:
      ItemMap [ item_index ] . item_can_be_installed_in_armour_slot = TRUE ;
      break;
    case 4:
      ItemMap [ item_index ] . item_can_be_installed_in_shield_slot = TRUE ;
      break;
    case 5:
      ItemMap [ item_index ] . item_can_be_installed_in_special_slot = TRUE ;
      break;
    case 6:
      ItemMap [ item_index ] . item_can_be_installed_in_aux_slot = TRUE ;
      break;
    default:
      break;
    };

  //--------------------
  // Now after that, it's time to read all the scales from the first page of
  // the item edit window...
  //
  ItemMap [ item_index ] . item_require_strength = (int) GTK_ADJUSTMENT ( strength_adjustment ) -> value ;
  ItemMap [ item_index ] . item_require_dexterity = (int) GTK_ADJUSTMENT ( dexterity_adjustment ) -> value ;
  ItemMap [ item_index ] . item_require_magic = (int) GTK_ADJUSTMENT ( magic_adjustment ) -> value ;
  ItemMap [ item_index ] . base_item_duration = (int) GTK_ADJUSTMENT ( base_duration_adjustment ) -> value ;
  ItemMap [ item_index ] . item_duration_modifier = (int) GTK_ADJUSTMENT ( duration_modifier_adjustment ) -> value ;
  // ItemMap [ item_index ] . sound_number = (int) GTK_ADJUSTMENT ( sound_number_adjustment ) -> value ;
  ItemMap [ item_index ] . base_list_price = (int) GTK_ADJUSTMENT ( base_list_price_adjustment ) -> value ;

  //--------------------
  // Now we read out the click buttons from the first page of the item
  // edit window...
  //
  ItemMap [ item_index ] . item_group_together_in_inventory =
    gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( group_together_in_inventory ) ) ;
  ItemMap [ item_index ] . item_can_be_applied_in_combat =
    gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( can_be_used_like_potion_or_grenade ) ) ;


  //--------------------
  // Now we read out the many scales from the second page of the edit
  // item popup window.
  //
  ItemMap [ item_index ] . base_item_gun_damage = (int) GTK_ADJUSTMENT ( base_item_gun_damage_adjustment ) -> value ;
  ItemMap [ item_index ] . item_gun_damage_modifier = (int) GTK_ADJUSTMENT ( item_gun_damage_modifier_adjustment ) -> value ;
  ItemMap [ item_index ] . item_gun_recharging_time = (double) GTK_ADJUSTMENT ( item_gun_recharging_time_adjustment ) -> value ;
  ItemMap [ item_index ] . item_gun_speed = (double) GTK_ADJUSTMENT ( item_gun_speed_adjustment ) -> value ;

  //--------------------
  // Now we read out the click buttons from the second page of the item
  // edit window...
  //
  ItemMap [ item_index ] . item_gun_bullet_pass_through_hit_bodies =
    gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( item_gun_bullet_pass_through_hit_bodies ) ) ;
  ItemMap [ item_index ] . item_gun_bullet_pass_through_explosions =
    gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( item_gun_bullet_pass_through_explosions ) ) ;
  ItemMap [ item_index ] . item_gun_bullet_ignore_wall_collisions =
    gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( item_gun_bullet_ignore_wall_collisions ) ) ;
  ItemMap [ item_index ] . item_gun_oneshotonly =
    gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( item_gun_oneshotonly ) ) ;

  //--------------------
  // Now we extract the ammunition type information from the widgets
  //
  m = (GtkMenu *) gtk_option_menu_get_menu ( GTK_OPTION_MENU ( ammo_type_selection ) );
  extra_item = (GtkMenuItem *) gtk_menu_get_active ( m ) ;
  extra_index = * ( (int*) gtk_object_get_user_data ( GTK_OBJECT ( extra_item ) ) ) ; 
  DebugPrintf ( 0 , "\n Final index, that was extracted from the currently selected equipment ammo_type: %d.",
		extra_index );

  switch ( extra_index )
    {
    case 0:
      ItemMap [ item_index ] . item_gun_use_ammunition = 0 ; 
      break;
    case 1:
      ItemMap [ item_index ] . item_gun_use_ammunition = ITEM_LASER_AMMUNITION ; 
      break;
    case 2:
      ItemMap [ item_index ] . item_gun_use_ammunition = ITEM_EXTERMINATOR_AMMUNITION ; 
      break;
    case 3:
      ItemMap [ item_index ] . item_gun_use_ammunition = ITEM_PLASMA_AMMUNITION ; 
      break;
    default:
      DebugPrintf ( 0 , "\n\nILLEGAL AMMUNITION TYPE FOUND!!!" );
      Terminate ( ERR ) ;
      break;
    };


  //--------------------
  // Now we read out the many scales from the third page of the edit
  // item popup window.
  //
  // ItemMap [ item_index ] . item_drive_maxspeed = (double) GTK_ADJUSTMENT ( item_drive_maxspeed_adjustment ) -> value ;
  // ItemMap [ item_index ] . item_drive_accel = (double) GTK_ADJUSTMENT ( item_drive_accel_adjustment ) -> value ;
  ItemMap [ item_index ] . base_ac_bonus = (int) GTK_ADJUSTMENT ( base_ac_bonus_adjustment ) -> value ;
  ItemMap [ item_index ] . ac_bonus_modifier = (int) GTK_ADJUSTMENT ( ac_bonus_modifier_adjustment ) -> value ;

  //--------------------
  // Now we can destroy the window again and close all this stuff...
  //
  gtk_widget_destroy ( window );

  gtk_main_quit();
  
}; // void edit_interface_ok_button_pressed ( GtkWidget *w, GtkFileSelection *fs )

/* ----------------------------------------------------------------------
 * This function is used to handle the 'ok' button in the popup window
 * that appears when middle-mouse clicking onto one of the dialog options.
 * This is nescessary, since it seems not to be enough to just close the
 * window, but we must also write the window content back into the 
 * appropriate variables to the changes really take effect...
 * And then we must make the popup window go away.
 * ---------------------------------------------------------------------- */
void 
edit_interface_disable_button_pressed ( GtkWidget *w, int *item_index_pointer )
{
  int item_index;

  //--------------------
  // At first we say, that we're here...
  //
  DebugPrintf ( 1 , "\nedit_interface_disable_button_pressed: Function call confirmed. " );

  item_index = * item_index_pointer ;
  DebugPrintf ( 1 , "\nDisabling data in option Nr. %d." , item_index );

  //--------------------
  // Now we set the 'disable' text in the option sample file name entry...
  //
  gtk_text_set_point ( GTK_TEXT ( item_3d_model_path_entry ) , 0 );
  gtk_text_forward_delete ( GTK_TEXT ( item_3d_model_path_entry ) , gtk_text_get_length ( GTK_TEXT ( item_3d_model_path_entry ) ) );
  gtk_text_insert ( GTK_TEXT ( item_3d_model_path_entry ) , item_3d_model_path_entry->style->font,
		    &item_3d_model_path_entry->style->black,
		    &item_3d_model_path_entry->style->white,
		    "NONE_AVAILABLE_YET" ,
		    -1 );

}; // void edit_interface_disable_button_pressed ( GtkWidget *w, GtkFileSelection *fs )

/* ----------------------------------------------------------------------
 * This function is used to handle the 'cancel' button in the popup window
 * that appears when middle-mouse clicking onto one of the dialog options.
 * This is nescessary, since it seems not to be enough to just call the
 * gtk quit function or something, cause this will not make the big 
 * popup window go away.
 * ---------------------------------------------------------------------- */
void 
edit_interface_cancel_button_pressed ( GtkWidget *w, GtkFileSelection *fs )
{

  DebugPrintf ( 1 , "\nedit_interface_cancel_button_pressed: Function call confirmed. " );

  gtk_widget_destroy ( window );

  gtk_main_quit();

}; // edit_interface_cancel_button_pressed ( GtkWidget *w, GtkFileSelection *fs )

void 
on_switch_page ( GtkWidget *w, GtkFileSelection *fs )
{

}; // 

void 
on_radio_button_pressed ( GtkWidget *w, GtkFileSelection *fs )
{

}; // 

/* ----------------------------------------------------------------------
 * When the user presses the right mouse button in the graph window, this
 * click is interpreted as marking or unmarking dialog options or drawing
 * new connections or different connection types between the options in
 * question.  This is performed here, with the given parameter being the
 * option that the user right-clicked upon.
 * ---------------------------------------------------------------------- */
void
gui_react_to_right_mousebutton ( int item_index )
{
  int i;

  DebugPrintf ( 1 , "\ngui_react_to_right_mousebutton (...):  real function call confirmed." );

  //--------------------
  // The click was right in the void?  --  no reaction, no comment.
  //
  if ( item_index < 0 ) 
    {
      DebugPrintf ( 1 , "\nNothing needs to be done, cause click was into the void...." );
      return ;
    }

  //--------------------
  // The click was on an option and no other option currently marked? -- 
  // then we interpret this as marking an option.
  //
  if ( currently_marked_item_index == (-1) )
    {
      currently_marked_item_index = item_index ;
      DebugPrintf ( 1 , "\nMarked dialog option is now: %d." , currently_marked_item_index );
      gui_redraw_graph_completely (  );
      return;
    }

  //--------------------
  // The click occured on the very same option that is already marked? --
  // then we unmark this option again.
  //
  if ( currently_marked_item_index == item_index )
    {
      currently_marked_item_index = (-1) ;
      DebugPrintf ( 1 , "\nMarked dialog option is now: NONE ANY MORE." );
      gui_redraw_graph_completely (  );
      return;
    }

  //--------------------
  // The click occured on another option? -- then we interpret this as
  // changing the nature of the connection between those two options:
  // from ACTIVATE to DEACTIVATE to NONE to ACTIVATE to DEACTIVATE to NONE to ...
  //
  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      if ( ChatRoster [ currently_marked_item_index ] . change_option_nr [ i ] == item_index )
	{

	  if ( ChatRoster [ currently_marked_item_index ] . change_option_to_value [ i ] == 0 )
	    {
	      ChatRoster [ currently_marked_item_index ] . change_option_to_value [ i ] = 1 ;
	      gui_redraw_graph_completely (  );
	      DebugPrintf ( 1 , "\nChanging target value to : ACTIVATE." );
	      return;
	    }
	  else
	    {
	      ChatRoster [ currently_marked_item_index ] . change_option_nr [ i ] = (-1) ;
	      ChatRoster [ currently_marked_item_index ] . change_option_to_value [ i ] = (-1) ;
	      DebugPrintf ( 1 , "\nChanging: DELETING CONNECTION." );
	      gui_redraw_graph_completely (  );
	      return;
	    }

	}
    }

  //--------------------
  // If there was no connection up until now, we'll introduce one now...
  //
  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      if ( ChatRoster [ currently_marked_item_index ] . change_option_nr [ i ]  == (-1) )
	{
	  ChatRoster [ currently_marked_item_index ] . change_option_nr [ i ] = item_index ;
	  ChatRoster [ currently_marked_item_index ] . change_option_to_value [ i ] = 0 ;
	  gui_redraw_graph_completely (  );
	  DebugPrintf ( 1 , "\nChanging: INTRODUCING NEW CONNECTION." );
	  return;
	}
    }
  
}; // void gui_react_to_right_mousebutton ( int item_index )


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
show_str_dex_mag_requirements_of_item ( GtkWidget* vbox2 , int item_index )
{
  GtkWidget *strength_hbox;
  GtkWidget *dexterity_hbox;
  GtkWidget *magic_hbox;
  GtkWidget *label;
  GtkWidget *hscale;
  GtkWidget *str_dex_mag_vbox;
  GtkWidget *str_dex_mag_frame;

  /*
  //--------------------
  // First we make a new box with a new label.  Into this box we can then
  // add all the strength, dexterity and magic requirement for this item.
  //
  str_dex_mag_vbox = gtk_vbox_new ( FALSE , 0 );
  gtk_widget_ref ( str_dex_mag_vbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ), "str_dex_mag_vbox", str_dex_mag_vbox , 
			     (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show ( str_dex_mag_vbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), str_dex_mag_vbox );
  gtk_container_set_border_width ( GTK_CONTAINER ( str_dex_mag_vbox ), 4 );

  //--------------------
  // Now we add the options text entry box and the option sample text entry
  // box, and we put all of that within a frame box and a hbox into that.
  // 
  str_dex_mag_frame = gtk_frame_new ("For equipping/using this item, the character will require values of:");
  gtk_widget_ref ( str_dex_mag_frame );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "str_dex_mag_frame" , str_dex_mag_frame , 
			     ( GtkDestroyNotify ) gtk_widget_unref );
  gtk_widget_show ( str_dex_mag_frame );
  gtk_box_pack_start ( GTK_BOX ( str_dex_mag_vbox ) , str_dex_mag_frame , TRUE, TRUE , 0 );

  */

  //--------------------
  // Now we add the options text entry box and the option sample text entry
  // box, and we put all of that within a frame box and a hbox into that.
  // 
  str_dex_mag_frame = gtk_frame_new ("For equipping/using this item, the character will require values of:");
  gtk_widget_ref ( str_dex_mag_frame );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "str_dex_mag_frame" , str_dex_mag_frame , 
			     ( GtkDestroyNotify ) gtk_widget_unref );
  gtk_widget_show ( str_dex_mag_frame );
  gtk_box_pack_start ( GTK_BOX ( vbox2 ) , str_dex_mag_frame , TRUE, TRUE , 0 );

  //--------------------
  // First we make a new box with a new label.  Into this box we can then
  // add all the strength, dexterity and magic requirement for this item.
  //
  str_dex_mag_vbox = gtk_vbox_new ( FALSE , 0 );
  gtk_widget_ref ( str_dex_mag_vbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ), "str_dex_mag_vbox", str_dex_mag_vbox , 
			     (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show ( str_dex_mag_vbox );
  gtk_container_add ( GTK_CONTAINER ( str_dex_mag_frame ), str_dex_mag_vbox );
  gtk_container_set_border_width ( GTK_CONTAINER ( str_dex_mag_vbox ), 4 );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the strength requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  strength_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( strength_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "strength_hbox" , strength_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( strength_hbox );
  gtk_container_add ( GTK_CONTAINER ( str_dex_mag_vbox ), strength_hbox );

  //--------------------
  // We add the text to the left of the STRENGTH REQUIREMENT scale...
  //
  label = gtk_label_new ("STRENGTH (-1 for no requirement):");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( strength_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  strength_adjustment = gtk_adjustment_new ( 0.0, -1.0, 101.0, 0.1, 1.0, 1.0);
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (strength_adjustment) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( strength_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);
  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( strength_adjustment ) , (gfloat) ItemMap [ item_index ] . item_require_strength );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the dexterity requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  dexterity_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( dexterity_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "dexterity_hbox" , dexterity_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( dexterity_hbox );
  gtk_container_add ( GTK_CONTAINER ( str_dex_mag_vbox ), dexterity_hbox );

  //--------------------
  // We add the text to the left of the DEXTERITY REQUIREMENT scale...
  //
  label = gtk_label_new ("DEXTERITY (-1 for no requirement):");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( dexterity_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  dexterity_adjustment = gtk_adjustment_new ( 0.0, -1.0, 101.0, 0.1, 1.0, 1.0);
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (dexterity_adjustment) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( dexterity_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( dexterity_adjustment ) , (gfloat) ItemMap [ item_index ] . item_require_dexterity );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the magic requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  magic_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( magic_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "magic_hbox" , magic_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( magic_hbox );
  gtk_container_add ( GTK_CONTAINER ( str_dex_mag_vbox ), magic_hbox );

  //--------------------
  // We add the text to the left of the magic requirement scale...
  //
  label = gtk_label_new ("MAGIC (-1 for no requirement):");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( magic_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  magic_adjustment = gtk_adjustment_new ( 0.0, -1.0, 101.0, 0.1, 1.0, 1.0);
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (magic_adjustment) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( magic_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( magic_adjustment ) , (gfloat) ItemMap [ item_index ] . item_require_magic );

}; // void 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
gui_edit_dialog_set_up_first_page ( GtkWidget* notebook1 , int item_index )
{
  GtkWidget *vbox2;
  GtkWidget *option_hbox;
  GtkWidget *base_duration_hbox;
  GtkWidget *duration_modifier_hbox;
  GtkWidget *base_list_price_hbox;
  GtkWidget *scrolledwindow;
  GtkWidget *label;
  GtkWidget *frame1;
  GtkWidget *ebox;
  GtkWidget *hbox1;
  GtkTooltips *tooltips = gtk_tooltips_new ();
  GtkWidget *glade_menuitem;
  int i;
  GtkWidget *hscale;

  //--------------------
  // At first we set up the overall v-box, that should contain
  // all the fields within this page...
  //
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox2", vbox2, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);
  
  //--------------------
  // Now we add the options text entry box and the option sample text entry
  // box, and we put all of that within a frame box and a hbox into that.
  // 
  frame1 = gtk_frame_new ("LEFT: Item name                         RIGHT: 3d model directory name (click 'DISABLE' for none)");
  gtk_widget_ref (frame1);
  gtk_object_set_data_full (GTK_OBJECT (window), "frame1", frame1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox2), frame1, TRUE, TRUE, 0);

  
  option_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( option_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "option_hbox" , option_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( option_hbox );
  gtk_container_add ( GTK_CONTAINER ( frame1 ), option_hbox );
  // gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);

  //--------------------
  // Now we add the item name box...
  //
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow);
  gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow4", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER ( option_hbox ), scrolledwindow);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 4);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  
  item_name_entry = gtk_text_new ( NULL , NULL );
  gtk_widget_ref (item_name_entry);
  gtk_object_set_data_full (GTK_OBJECT (window), "item_name_text_entered_here_entry", item_name_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (item_name_entry);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), item_name_entry);
  gtk_tooltips_set_tip (tooltips, item_name_entry, "This text field contains the name of the item.", NULL);
  gtk_text_set_editable (GTK_TEXT (item_name_entry), TRUE);
  
  gtk_text_insert ( GTK_TEXT ( item_name_entry ), item_name_entry->style->font,
		    &item_name_entry->style->black, &item_name_entry->style->white,
		    ItemMap [ item_index ] . item_name , -1);
  
  //--------------------
  // Now we add the 3d model directory box...
  //
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow);
  gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow4", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER ( option_hbox ), scrolledwindow);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 4);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  
  item_3d_model_path_entry = gtk_text_new ( NULL , NULL );
  gtk_widget_ref (item_3d_model_path_entry);
  gtk_object_set_data_full (GTK_OBJECT (window), "option_sample_entered_here_entry", item_3d_model_path_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (item_3d_model_path_entry);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), item_3d_model_path_entry);
  gtk_tooltips_set_tip (tooltips, item_3d_model_path_entry, "This text field contains the name of the subdirectory with the 3d model information for this item.", NULL);
  gtk_text_set_editable (GTK_TEXT (item_3d_model_path_entry), TRUE);
  
  gtk_text_insert ( GTK_TEXT ( item_3d_model_path_entry ), item_3d_model_path_entry->style->font,
		    &item_3d_model_path_entry->style->black, &item_3d_model_path_entry->style->white,
		    ItemMap [ item_index ] . item_rotation_series_prefix , -1);

  //--------------------
  // Now we add the 'disable' button on the very right...
  //
  disable_button = gtk_button_new_with_label ("Disable");
  gtk_widget_ref (disable_button);
  gtk_object_set_data_full (GTK_OBJECT (window), "disable_button", disable_button, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (disable_button);
  gtk_container_add ( GTK_CONTAINER ( option_hbox ), disable_button);
  GTK_WIDGET_SET_FLAGS (disable_button, GTK_CAN_DEFAULT);
  gtk_tooltips_set_tip (tooltips, disable_button , "Use this button to enter the keywords, that will disable processing of the OPTIONS entries by the game engine.  The reply entries will still be processed, if they are not empty.", NULL);
  gtk_signal_connect (GTK_OBJECT (disable_button), "clicked", GTK_SIGNAL_FUNC ( edit_interface_disable_button_pressed ), &(item_index ) );


  //----------------------------------------------------------------------
  //----------------------------------------------------------------------


  //--------------------
  // Now we add the item sound sample file name box and the item inventory
  // file name box, and we put all of that within a frame box and a hbox into that.
  // 
  frame1 = gtk_frame_new ("LEFT: Item Sound Sample file name                         RIGHT: Inventory image file name");
  gtk_widget_ref (frame1);
  gtk_object_set_data_full (GTK_OBJECT (window), "frame1", frame1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox2), frame1, TRUE, TRUE, 0);

  
  option_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( option_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "option_hbox" , option_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( option_hbox );
  gtk_container_add ( GTK_CONTAINER ( frame1 ), option_hbox );
  // gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);

  //--------------------
  // Now we add the item sound file name box...
  //
  scrolledwindow = gtk_scrolled_window_new ( NULL , NULL );
  gtk_widget_ref ( scrolledwindow );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "scrolledwindow5" , scrolledwindow, 
			     ( GtkDestroyNotify ) gtk_widget_unref );
  gtk_widget_show ( scrolledwindow );
  gtk_container_add ( GTK_CONTAINER ( option_hbox ) , scrolledwindow );
  gtk_container_set_border_width ( GTK_CONTAINER ( scrolledwindow ) , 4 );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolledwindow ) , GTK_POLICY_NEVER , GTK_POLICY_ALWAYS );
  
  item_sound_file_name_entry = gtk_text_new ( NULL , NULL );
  gtk_widget_ref ( item_sound_file_name_entry );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "item_sound_sample_file_name_text_entered_here_entry" , 
			     item_sound_file_name_entry, ( GtkDestroyNotify ) gtk_widget_unref );
  gtk_widget_show ( item_sound_file_name_entry );
  gtk_container_add ( GTK_CONTAINER ( scrolledwindow ) , item_sound_file_name_entry );
  gtk_tooltips_set_tip ( tooltips , item_sound_file_name_entry, 
			 "This text field contains the sound file name for this item.", NULL);
  gtk_text_set_editable ( GTK_TEXT ( item_sound_file_name_entry ) , TRUE );
  
  gtk_text_insert ( GTK_TEXT ( item_sound_file_name_entry ), item_sound_file_name_entry -> style -> font ,
		    & item_sound_file_name_entry -> style -> black, & item_sound_file_name_entry -> style -> white,
		    ItemMap [ item_index ] . item_drop_sound_file_name , -1);
  
  //--------------------
  // Now we add the item inventory picture file name box...
  //
  scrolledwindow = gtk_scrolled_window_new ( NULL , NULL );
  gtk_widget_ref ( scrolledwindow );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "scrolledwindow6", scrolledwindow, 
			     ( GtkDestroyNotify ) gtk_widget_unref );
  gtk_widget_show ( scrolledwindow );
  gtk_container_add ( GTK_CONTAINER ( option_hbox ), scrolledwindow );
  gtk_container_set_border_width ( GTK_CONTAINER ( scrolledwindow ) , 4 );
  gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW ( scrolledwindow ) , GTK_POLICY_NEVER , GTK_POLICY_ALWAYS );
  
  item_inventory_image_file_name_entry = gtk_text_new ( NULL , NULL );
  gtk_widget_ref ( item_inventory_image_file_name_entry );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "option_sample_entered_here_entry" , 
			     item_inventory_image_file_name_entry, ( GtkDestroyNotify ) gtk_widget_unref );
  gtk_widget_show ( item_inventory_image_file_name_entry );
  gtk_container_add ( GTK_CONTAINER ( scrolledwindow ) , item_inventory_image_file_name_entry );
  gtk_tooltips_set_tip ( tooltips , item_inventory_image_file_name_entry, 
			 "This text field contains the name of the subdirectory with the 3d model information for this item." , NULL );
  gtk_text_set_editable ( GTK_TEXT ( item_inventory_image_file_name_entry), TRUE);
  
  gtk_text_insert ( GTK_TEXT ( item_inventory_image_file_name_entry ) , 
		    item_inventory_image_file_name_entry -> style -> font,
		    & item_inventory_image_file_name_entry -> style -> black, 
		    & item_inventory_image_file_name_entry -> style -> white,
		    ItemMap [ item_index ] . item_inv_file_name , -1);


  //----------------------------------------------------------------------
  //----------------------------------------------------------------------

  //--------------------
  // Now it's time to set up everything for the item description text...
  // 
  frame1 = gtk_frame_new ("Item description:");
  gtk_widget_ref (frame1);
  gtk_object_set_data_full (GTK_OBJECT (window), "frame_for_description_text", frame1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox2), frame1, TRUE, TRUE, 0);
  
  //--------------------
  // Now we add the item description box...
  //
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow);
  gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow4", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER ( frame1 ), scrolledwindow);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 4);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  
  item_description_entry = gtk_text_new ( NULL , NULL );
  gtk_widget_ref (item_description_entry);
  gtk_object_set_data_full (GTK_OBJECT (window), "item_description_entered_here_entry", item_description_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (item_description_entry);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), item_description_entry);
  gtk_tooltips_set_tip (tooltips, item_description_entry, "This text field contains the description of the item.", NULL);
  gtk_text_set_editable (GTK_TEXT (item_description_entry), TRUE);
  
  gtk_text_insert ( GTK_TEXT ( item_description_entry ), 
		    item_description_entry->style->font,
		    &item_description_entry->style->black, 
		    &item_description_entry->style->white,
		    ItemMap [ item_index ] . item_description , -1);
  

  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_first_page(...) : starting setup of slot selection." );
  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now it's time to set up everything for the apply slot selection menu...
  // 
  // frame1 = gtk_frame_new ("Item can be equipped in slot:");
  // gtk_widget_ref (frame1);
  // gtk_object_set_data_full (GTK_OBJECT (window), "frame_for_slot_selection", frame1, (GtkDestroyNotify) gtk_widget_unref);
  // gtk_widget_show (frame1);
  // gtk_box_pack_start (GTK_BOX (vbox2), frame1, TRUE, TRUE, 0);
  
  //--------------------
  // First we make a new h-box here, cause a selection menu requires that?
  //
  hbox1 = gtk_hbox_new (FALSE, 10);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (window), "slot_selection_hbox", hbox1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start ( GTK_BOX ( vbox2 ) , hbox1 , FALSE , TRUE , 0 );

  //--------------------
  // We add the text in front of the slot selection...
  //
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_first_page(...) : now we add a label for the slot selection." );
  label = gtk_label_new ("This item can be equipped in slot:");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( hbox1 ), label , TRUE, TRUE, 0);

  //--------------------
  // Now we add the slot selection menu....
  //
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_first_page(...) : now we add the slot selection menu." );
  slot_selection = gtk_option_menu_new ();
  gtk_widget_ref ( slot_selection );
  gtk_object_set_data_full ( GTK_OBJECT ( window ), "slot_selection", slot_selection, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show ( slot_selection );

  //--------------------
  // Now we insert one whole selector into the window
  //
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_first_page(...) : now we insert it into a box." );
  gtk_box_pack_start ( GTK_BOX ( hbox1 ), slot_selection , TRUE, TRUE, 0);
  // gtk_tooltips_set_tip (tooltips, slot_selection , "Select the speaker", NULL);
  slot_selection_menu = gtk_menu_new ();

  //--------------------
  // Now we must fill in all known extras, so that the user of the dialog editor
  // can select really any chat extra command, that he/she feels like adding...
  //
  for ( i = 0 ; i < ALL_KNOWN_SLOTS ; i ++ )
    {
      DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_first_page(...) : we add another menu item..." );
      glade_menuitem = gtk_menu_item_new_with_label ( slot_map [ i ] . slot_identifier );
      gtk_object_set_user_data ( GTK_OBJECT ( glade_menuitem ) , &( slot_map [ i ] . slot_index ) );
      gtk_widget_show (glade_menuitem);
      gtk_menu_append ( GTK_MENU ( slot_selection_menu ), glade_menuitem);
    }
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_first_page(...) : we set the menu..." );
  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( slot_selection ) , slot_selection_menu );
  
  //--------------------
  // Now we set the correct old value, so that the user can really modify
  // the existing characters and is not forced to re-enter all the old values
  // again and again...
  //
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_first_page(...) : we set some values..." );
  if ( ItemMap [ item_index ] . item_can_be_installed_in_weapon_slot )
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( slot_selection ) , 1 );
  else if ( ItemMap [ item_index ] . item_can_be_installed_in_drive_slot )
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( slot_selection ) , 2 );
  else if ( ItemMap [ item_index ] . item_can_be_installed_in_armour_slot )
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( slot_selection ) , 3 );
  else if ( ItemMap [ item_index ] . item_can_be_installed_in_shield_slot )
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( slot_selection ) , 4 );
  else if ( ItemMap [ item_index ] . item_can_be_installed_in_special_slot )
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( slot_selection ) , 5 );
  else if ( ItemMap [ item_index ] . item_can_be_installed_in_aux_slot )
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( slot_selection ) , 6 );
  else 
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( slot_selection ) , 0 );

  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_first_page(...) : end setup of slot selection." );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------

  //--------------------
  // Now we set up the clickbox for whether the item can be applied in
  // combat (i.e. 'eaten up' or not...
  //
  ebox = gtk_event_box_new ();
  gtk_widget_ref (ebox);
  gtk_widget_show (ebox);
  gtk_object_set_data_full (GTK_OBJECT (window), "ebox2", ebox, (GtkDestroyNotify) gtk_widget_unref);
  gtk_box_pack_end (GTK_BOX (vbox2), ebox, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, ebox, "Check this to make the item usable like a potion or grenade as a 'one-shot' device.", NULL);

  can_be_used_like_potion_or_grenade = gtk_check_button_new_with_label ("Can be spent like a potion or grenade");
  gtk_widget_ref ( can_be_used_like_potion_or_grenade );
  gtk_object_set_data_full (GTK_OBJECT (window), "can_be_used_like_potion_or_grenade",   can_be_used_like_potion_or_grenade , (GtkDestroyNotify) gtk_widget_unref);
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( can_be_used_like_potion_or_grenade ), ItemMap [ item_index ] . item_can_be_applied_in_combat ) ;
  gtk_widget_show ( can_be_used_like_potion_or_grenade );
  gtk_container_add ( GTK_CONTAINER ( ebox ) , can_be_used_like_potion_or_grenade );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we set up the clickbox for whether the item can be applied in
  // combat (i.e. 'eaten up' or not...
  //
  ebox = gtk_event_box_new ();
  gtk_widget_ref (ebox);
  gtk_widget_show (ebox);
  gtk_object_set_data_full (GTK_OBJECT (window), "ebox2", ebox, (GtkDestroyNotify) gtk_widget_unref);
  gtk_box_pack_end (GTK_BOX (vbox2), ebox, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, ebox, "Check this to have these items made into one with greater multiplicity as soon as picked up by the Tux.", NULL);

  group_together_in_inventory = gtk_check_button_new_with_label ("Multiple of these group together in inventory");
  gtk_widget_ref ( group_together_in_inventory );
  gtk_object_set_data_full (GTK_OBJECT (window), "group_together_in_inventory",   group_together_in_inventory , (GtkDestroyNotify) gtk_widget_unref);
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( group_together_in_inventory ), ItemMap [ item_index ] . item_group_together_in_inventory ) ;
  gtk_widget_show ( group_together_in_inventory );
  gtk_container_add ( GTK_CONTAINER ( ebox ) , group_together_in_inventory );

  //--------------------
  // Now we add a separate box with the strength, dexterity and magic requirements
  // for equipping/using this item...
  //
  show_str_dex_mag_requirements_of_item( vbox2 , item_index );

  //----------------------------------------------------------------------
  // Now we print out the base_duration requirement...
  //--------------------
  // Now the h-box for the base_duration...
  //
  base_duration_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( base_duration_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "base_duration_hbox" , base_duration_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( base_duration_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), base_duration_hbox );

  //--------------------
  // We add the text to the left of the base_duration requirement scale...
  //
  label = gtk_label_new ("Base duration of this item (-1 for indestructable):");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( base_duration_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  base_duration_adjustment = gtk_adjustment_new ( 0.0, -1.0, 101.0, 0.1, 1.0, 1.0);
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (base_duration_adjustment) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( base_duration_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( base_duration_adjustment ) , (gfloat) ItemMap [ item_index ] . base_item_duration );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the duration_modifier requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  duration_modifier_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( duration_modifier_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "duration_modifier_hbox" , duration_modifier_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( duration_modifier_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), duration_modifier_hbox );

  //--------------------
  // We add the text to the left of the duration_modifier requirement scale...
  //
  label = gtk_label_new ("Duration modifier for this item:");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( duration_modifier_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  duration_modifier_adjustment = gtk_adjustment_new ( 0.0, -1.0, 101.0, 0.1, 1.0, 1.0);
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (duration_modifier_adjustment) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( duration_modifier_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( duration_modifier_adjustment ) , (gfloat) ItemMap [ item_index ] . item_duration_modifier );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the base_list_price requirement...
  //
  base_list_price_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( base_list_price_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "base_list_price_hbox" , base_list_price_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( base_list_price_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), base_list_price_hbox );

  //--------------------
  // We add the text to the left of the base_list_price requirement scale...
  //
  label = gtk_label_new ("Base list price:");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( base_list_price_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  base_list_price_adjustment = gtk_adjustment_new ( 0.0, -1.0, 1001.0, 0.1, 1.0, 1.0);
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (base_list_price_adjustment) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( base_list_price_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( base_list_price_adjustment ) , (gfloat) ItemMap [ item_index ] . base_list_price );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  
  // The heading of this page
  label = gtk_label_new ("General");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label);
  

}; // void gui_edit_dialog_set_up_first_page ( GtkWidget* notebook1 , int item_index )

/* ----------------------------------------------------------------------
 * This second page of the edit item popup window should contain all the
 * weapon statistics of this item.
 * ---------------------------------------------------------------------- */
void
gui_edit_dialog_set_up_second_page ( GtkWidget *notebook1 , int item_index )
{
  GtkWidget *vbox2;
  GtkWidget *label;
  GtkWidget *base_item_gun_damage_hbox;
  GtkWidget *item_gun_damage_modifier_hbox;
  GtkWidget *item_gun_recharging_time_hbox;
  GtkWidget *item_gun_speed_hbox;
  GtkWidget *ammo_type_hbox;
  GtkWidget *hscale;
  GtkWidget *ebox;
  GtkTooltips *tooltips = gtk_tooltips_new ();
  int i;
  GtkWidget *glade_menuitem;

  //--------------------
  // We prepare the big big v-box that should contain this whole
  // edit page...
  //
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox2", vbox2, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);
  
  // The heading of this page
  label = gtk_label_new ("Weapon Stats");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label);

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the base_item_gun_damage requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  base_item_gun_damage_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( base_item_gun_damage_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "base_item_gun_damage_hbox" , base_item_gun_damage_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( base_item_gun_damage_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), base_item_gun_damage_hbox );

  //--------------------
  // We add the text to the left of the base_item_gun_damage requirement scale...
  //
  label = gtk_label_new ("Base damage value for this item:");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( base_item_gun_damage_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  base_item_gun_damage_adjustment = gtk_adjustment_new ( 0.0, -1.0, 101.0, 0.1, 1.0, 1.0);
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (base_item_gun_damage_adjustment) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( base_item_gun_damage_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( base_item_gun_damage_adjustment ) , (gfloat) ItemMap [ item_index ] . base_item_gun_damage );


  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the item_gun_damage_modifier requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  item_gun_damage_modifier_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( item_gun_damage_modifier_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "item_gun_damage_modifier_hbox" , item_gun_damage_modifier_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( item_gun_damage_modifier_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), item_gun_damage_modifier_hbox );

  //--------------------
  // We add the text to the left of the item_gun_damage_modifier requirement scale...
  //
  label = gtk_label_new ("Damage modifier for this item:");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( item_gun_damage_modifier_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  item_gun_damage_modifier_adjustment = gtk_adjustment_new ( 0.0, -1.0, 101.0, 0.1, 1.0, 1.0);
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (item_gun_damage_modifier_adjustment) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( item_gun_damage_modifier_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( item_gun_damage_modifier_adjustment ) , (gfloat) ItemMap [ item_index ] . item_gun_damage_modifier );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the item_gun_recharging_time requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  item_gun_recharging_time_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( item_gun_recharging_time_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "item_gun_recharging_time_hbox" , item_gun_recharging_time_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( item_gun_recharging_time_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), item_gun_recharging_time_hbox );

  //--------------------
  // We add the text to the left of the item_gun_recharging_time requirement scale...
  //
  label = gtk_label_new ("Recharging time (also for melee weapons):");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( item_gun_recharging_time_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  item_gun_recharging_time_adjustment = gtk_adjustment_new ( 0.1, -1.0, 5.0, 0.001, 0.001 , 0.003 );
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT ( item_gun_recharging_time_adjustment ) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 3 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( item_gun_recharging_time_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( item_gun_recharging_time_adjustment ) , (gfloat) ItemMap [ item_index ] . item_gun_recharging_time );


  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the item_gun_speed requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  item_gun_speed_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( item_gun_speed_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "item_gun_speed_hbox" , item_gun_speed_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( item_gun_speed_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), item_gun_speed_hbox );

  //--------------------
  // We add the text to the left of the item_gun_speed requirement scale...
  //
  label = gtk_label_new ("Speed of bullets from this item:");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( item_gun_speed_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  item_gun_speed_adjustment = gtk_adjustment_new ( 0.0, -1.0, 101.0, 0.1, 1.0, 1.0);
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (item_gun_speed_adjustment) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 2 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( item_gun_speed_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( item_gun_speed_adjustment ) , (gfloat) ItemMap [ item_index ] . item_gun_speed );


  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we set up the clickbox for whether the bullets can pass through bodies...
  //
  ebox = gtk_event_box_new ();
  gtk_widget_ref (ebox);
  gtk_widget_show (ebox);
  gtk_object_set_data_full (GTK_OBJECT (window), "ebox2", ebox, (GtkDestroyNotify) gtk_widget_unref);
  gtk_box_pack_end (GTK_BOX (vbox2), ebox, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, ebox, "Check this to make bullets pass though (hit) bodies.", NULL);

  item_gun_bullet_pass_through_hit_bodies = gtk_check_button_new_with_label ("Bullets of this type pass through (hit) bodies");
  gtk_widget_ref ( item_gun_bullet_pass_through_hit_bodies );
  gtk_object_set_data_full (GTK_OBJECT (window), "item_gun_bullet_pass_through_hit_bodies",   item_gun_bullet_pass_through_hit_bodies , (GtkDestroyNotify) gtk_widget_unref);
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( item_gun_bullet_pass_through_hit_bodies ), ItemMap [ item_index ] . item_gun_bullet_pass_through_hit_bodies ) ;
  gtk_widget_show ( item_gun_bullet_pass_through_hit_bodies );
  gtk_container_add ( GTK_CONTAINER ( ebox ) , item_gun_bullet_pass_through_hit_bodies );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we set up the clickbox for whether the bullets can pass through explosions...
  //
  ebox = gtk_event_box_new ();
  gtk_widget_ref (ebox);
  gtk_widget_show (ebox);
  gtk_object_set_data_full (GTK_OBJECT (window), "ebox2", ebox, (GtkDestroyNotify) gtk_widget_unref);
  gtk_box_pack_end (GTK_BOX (vbox2), ebox, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, ebox, "Check this to make bullets pass though explisions.", NULL);

  item_gun_bullet_pass_through_explosions = gtk_check_button_new_with_label ("Bullets of this type pass through explosions.");
  gtk_widget_ref ( item_gun_bullet_pass_through_explosions );
  gtk_object_set_data_full (GTK_OBJECT (window), "item_gun_bullet_pass_through_explosions",   item_gun_bullet_pass_through_explosions , (GtkDestroyNotify) gtk_widget_unref);
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( item_gun_bullet_pass_through_explosions ), ItemMap [ item_index ] . item_gun_bullet_pass_through_explosions ) ;
  gtk_widget_show ( item_gun_bullet_pass_through_explosions );
  gtk_container_add ( GTK_CONTAINER ( ebox ) , item_gun_bullet_pass_through_explosions );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we set up the clickbox for whether the bullets ignore wall collisions
  //
  ebox = gtk_event_box_new ();
  gtk_widget_ref (ebox);
  gtk_widget_show (ebox);
  gtk_object_set_data_full (GTK_OBJECT (window), "ebox2", ebox, (GtkDestroyNotify) gtk_widget_unref);
  gtk_box_pack_end (GTK_BOX (vbox2), ebox, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, ebox, "Check this to make bullets survive wall collisions.", NULL);

  item_gun_bullet_ignore_wall_collisions = gtk_check_button_new_with_label ("Bullets of this survive collisions with walls (e.g. swordblade).");
  gtk_widget_ref ( item_gun_bullet_ignore_wall_collisions );
  gtk_object_set_data_full (GTK_OBJECT (window), "item_gun_bullet_ignore_wall_collisions",   item_gun_bullet_ignore_wall_collisions , (GtkDestroyNotify) gtk_widget_unref);
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( item_gun_bullet_ignore_wall_collisions ), ItemMap [ item_index ] . item_gun_bullet_ignore_wall_collisions ) ;
  gtk_widget_show ( item_gun_bullet_ignore_wall_collisions );
  gtk_container_add ( GTK_CONTAINER ( ebox ) , item_gun_bullet_ignore_wall_collisions );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we set up the clickbox for whether only one of those bullets can exist at a time...
  //
  ebox = gtk_event_box_new ();
  gtk_widget_ref (ebox);
  gtk_widget_show (ebox);
  gtk_object_set_data_full (GTK_OBJECT (window), "ebox2", ebox, (GtkDestroyNotify) gtk_widget_unref);
  gtk_box_pack_end (GTK_BOX (vbox2), ebox, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, ebox, "Check this to allow no than one such bullet at any given time.", NULL);

  item_gun_oneshotonly = gtk_check_button_new_with_label ("No more than one of these bullets can exist at any time.");
  gtk_widget_ref ( item_gun_oneshotonly );
  gtk_object_set_data_full (GTK_OBJECT (window), "item_gun_oneshotonly",   item_gun_oneshotonly , (GtkDestroyNotify) gtk_widget_unref);
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( item_gun_oneshotonly ), ItemMap [ item_index ] . item_gun_oneshotonly ) ;
  gtk_widget_show ( item_gun_oneshotonly );
  gtk_container_add ( GTK_CONTAINER ( ebox ) , item_gun_oneshotonly );








  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_second_page(...): starting setup of ammo type selection." );
  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now it's time to set up everything for the apply ammo_type selection menu...
  // 
  // frame1 = gtk_frame_new ("Item can be equipped in ammo_type:");
  // gtk_widget_ref (frame1);
  // gtk_object_set_data_full (GTK_OBJECT (window), "frame_for_ammo_type_selection", frame1, (GtkDestroyNotify) gtk_widget_unref);
  // gtk_widget_show (frame1);
  // gtk_box_pack_start (GTK_BOX (vbox2), frame1, TRUE, TRUE, 0);
  
  //--------------------
  // First we make a new h-box here, cause a selection menu requires that?
  //
  ammo_type_hbox = gtk_hbox_new (FALSE, 10);
  gtk_widget_ref (ammo_type_hbox);
  gtk_object_set_data_full (GTK_OBJECT (window), "ammo_type_selection_hbox", ammo_type_hbox, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ammo_type_hbox);
  gtk_box_pack_start ( GTK_BOX ( vbox2 ) , ammo_type_hbox , FALSE , TRUE , 0 );

  //--------------------
  // We add the text in front of the ammo_type selection...
  //
  label = gtk_label_new ("This requires ammunition of type:");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( ammo_type_hbox ), label , FALSE, FALSE, 0);

  //--------------------
  // Now we add the ammo_type selection menu....
  //
  ammo_type_selection = gtk_option_menu_new ();
  gtk_widget_ref ( ammo_type_selection );
  gtk_object_set_data_full ( GTK_OBJECT ( window ), "ammo_type_selection", ammo_type_selection, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show ( ammo_type_selection );

  //--------------------
  // Now we insert one whole selector into the window
  //
  gtk_box_pack_start ( GTK_BOX ( ammo_type_hbox ), ammo_type_selection , TRUE, TRUE, 0);
  // gtk_tooltips_set_tip (tooltips, ammo_type_selection , "Select the speaker", NULL);
  ammo_type_selection_menu = gtk_menu_new ();

  //--------------------
  // Now we must fill in all known extras, so that the user of the dialog editor
  // can select really any chat extra command, that he/she feels like adding...
  //
  for ( i = 0 ; i < ALL_KNOWN_AMMO_TYPES ; i ++ )
    {
      glade_menuitem = gtk_menu_item_new_with_label ( ammo_type_map [ i ] . ammo_type_identifier );
      gtk_object_set_user_data ( GTK_OBJECT ( glade_menuitem ) , &( ammo_type_map [ i ] . ammo_type_index ) );
      gtk_widget_show (glade_menuitem);
      gtk_menu_append ( GTK_MENU ( ammo_type_selection_menu ), glade_menuitem);
    }
  gtk_option_menu_set_menu ( GTK_OPTION_MENU ( ammo_type_selection ) , ammo_type_selection_menu );
  
  //--------------------
  // Now we set the correct old value, so that the user can really modify
  // the existing characters and is not forced to re-enter all the old values
  // again and again...
  //
  if ( ItemMap [ item_index ] . item_gun_use_ammunition == ITEM_PLASMA_AMMUNITION )
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( ammo_type_selection ) , 3 ) ;
  else if ( ItemMap [ item_index ] . item_gun_use_ammunition == ITEM_EXTERMINATOR_AMMUNITION )
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( ammo_type_selection ) , 2 ) ;
  else if ( ItemMap [ item_index ] . item_gun_use_ammunition == ITEM_LASER_AMMUNITION )
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( ammo_type_selection ) , 1 ) ;
  else
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( ammo_type_selection ) , 0 );

  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_second_page(...):  end setup of ammo_type selection." );

}; // void gui_edit_dialog_set_up_second_page ( GtkWidget *notebook1 , int item_index )


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
gui_edit_dialog_set_up_third_page ( GtkWidget *notebook1 , int item_index )
{
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *label;
  GtkWidget *hscale;
  GtkWidget *item_drive_maxspeed_hbox;
  GtkWidget *item_drive_accel_hbox;
  GtkWidget *base_ac_bonus_hbox;
  GtkWidget *ac_bonus_modifier_hbox;
  GtkWidget *frame1;

  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_third_page(...) : starting..." );

  //--------------------
  // Now we create the MAIN vbox and put it into the notebook...
  //
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox4", vbox1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox1);
  gtk_container_set_border_width (GTK_CONTAINER (vbox1), 4);

  //--------------------
  // First we create a heading for this page of the edit item
  // popup window.
  //
  // NOTE THAT THIS MUST BE DONE *AFTER* THE NOTEBOOK HAS BEEN
  // FILLED WITH THE MAIN V-BOX!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //
  label = gtk_label_new ("Drive and defence stats");
  gtk_widget_ref (label);
  gtk_object_set_data_full ( GTK_OBJECT ( window ), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_notebook_set_tab_label ( GTK_NOTEBOOK ( notebook1 ), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label);

  //--------------------
  // Now we create a frame to put into the main v-box
  //
  frame1 = gtk_frame_new ("Drive settings:");
  gtk_widget_ref (frame1);
  gtk_object_set_data_full (GTK_OBJECT (window), "frame1", frame1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);
  
  //--------------------
  // Now we create the inner v-box for the frame and add it into
  // the frame
  //
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox3", vbox2, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER ( frame1 ), vbox2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);
  

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the item_drive_maxspeed requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  item_drive_maxspeed_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( item_drive_maxspeed_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "item_drive_maxspeed_hbox" , item_drive_maxspeed_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( item_drive_maxspeed_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), item_drive_maxspeed_hbox );

  //--------------------
  // We add the text to the left of the item_drive_maxspeed requirement scale...
  //
  label = gtk_label_new ("Item as drive:  maxspeed: ");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( item_drive_maxspeed_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  item_drive_maxspeed_adjustment = gtk_adjustment_new ( 0.1, -1.0, 12.0, 0.001, 0.001 , 0.003 );
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT ( item_drive_maxspeed_adjustment ) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 3 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( item_drive_maxspeed_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  // gtk_adjustment_set_value ( GTK_ADJUSTMENT ( item_drive_maxspeed_adjustment ) , (gfloat) ItemMap [ item_index ] . item_drive_maxspeed );
  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( item_drive_maxspeed_adjustment ) , (gfloat) 0 );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the item_drive_accel requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  item_drive_accel_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( item_drive_accel_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "item_drive_accel_hbox" , item_drive_accel_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( item_drive_accel_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), item_drive_accel_hbox );

  //--------------------
  // We add the text to the left of the item_drive_accel requirement scale...
  //
  label = gtk_label_new ("Item as drive:  acceleration: ");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( item_drive_accel_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  item_drive_accel_adjustment = gtk_adjustment_new ( 0.1, -1.0, 12.0, 0.001, 0.001 , 0.003 );
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT ( item_drive_accel_adjustment ) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 3 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( item_drive_accel_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  // gtk_adjustment_set_value ( GTK_ADJUSTMENT ( item_drive_accel_adjustment ) , (gfloat) ItemMap [ item_index ] . item_drive_accel );
  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( item_drive_accel_adjustment ) , (gfloat) 0 );

  //--------------------
  // Now we create a frame to put into the main v-box
  //
  frame1 = gtk_frame_new ("Defence settings:");
  gtk_widget_ref (frame1);
  gtk_object_set_data_full (GTK_OBJECT (window), "frame1", frame1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);
  
  //--------------------
  // Now we create the inner v-box for the frame and add it into
  // the frame
  //
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox3", vbox2, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER ( frame1 ), vbox2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);
  
  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the base_ac_bonus requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  base_ac_bonus_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( base_ac_bonus_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "base_ac_bonus_hbox" , base_ac_bonus_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( base_ac_bonus_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), base_ac_bonus_hbox );

  //--------------------
  // We add the text to the left of the base_ac_bonus requirement scale...
  //
  label = gtk_label_new ("Base AC bonus: ");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( base_ac_bonus_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  base_ac_bonus_adjustment = gtk_adjustment_new ( 0.1, -5.0, 120.0, 1.0, 1.0 , 1.0 );
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT ( base_ac_bonus_adjustment ) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( base_ac_bonus_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( base_ac_bonus_adjustment ) , (gfloat) ItemMap [ item_index ] . base_ac_bonus );

  //----------------------------------------------------------------------
  //----------------------------------------------------------------------
  //--------------------
  // Now we print out the ac_bonus_modifier requirement...
  //
  //--------------------
  // Now the h-box for each reply-sample reply-subtitle combination...
  //
  ac_bonus_modifier_hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref ( ac_bonus_modifier_hbox );
  gtk_object_set_data_full ( GTK_OBJECT ( window ) , "ac_bonus_modifier_hbox" , ac_bonus_modifier_hbox , (GtkDestroyNotify) gtk_widget_unref );
  gtk_widget_show ( ac_bonus_modifier_hbox );
  gtk_container_add ( GTK_CONTAINER ( vbox2 ), ac_bonus_modifier_hbox );

  //--------------------
  // We add the text to the left of the ac_bonus_modifier requirement scale...
  //
  label = gtk_label_new ("AC Bonus modifier: ");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start ( GTK_BOX ( ac_bonus_modifier_hbox ), label , FALSE, FALSE, 0);

  // Reuse the same adjustment 
  ac_bonus_modifier_adjustment = gtk_adjustment_new ( 0.1, -5.0, 120.0, 1.0, 1.0 , 1.0 );
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT ( ac_bonus_modifier_adjustment ) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( ac_bonus_modifier_hbox ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  gtk_adjustment_set_value ( GTK_ADJUSTMENT ( ac_bonus_modifier_adjustment ) , (gfloat) ItemMap [ item_index ] . ac_bonus_modifier );

  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_dialog_set_up_third_page(...) : done." );

}; // void gui_edit_dialog_set_up_third_page ( GtkWidget *notebook1 , int item_index )

/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void
gui_edit_item_index_interface ( int item_index )
{
  GtkWidget *vbox1;
  GtkWidget *notebook1;
  GtkWidget *ok_button;
  GtkWidget *cancel_button;
  GtkWidget *hbuttonbox1;
  GtkTooltips *tooltips = gtk_tooltips_new ();

  //--------------------
  // At first we say, that we are here...
  //
  DebugPrintf ( 1 , "\ngui_edit_item_index_interface ( int item_index ): function call confirmed. " );
  
  //--------------------
  // Maybe the click was into the void.  Then of course we do
  // not do anything here.
  //
  if ( item_index < 0 ) return;

  //--------------------
  // Now we destroy that awful little window, that might still
  // be cluttering part of the screen... how annoying...
  //
  gui_destroy_existing_tooltips (  );
  
  // the "Edit Item Node" window
  window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (window), "window", window);
  gtk_widget_set_usize (window, 600, 520);
  gtk_window_set_title (GTK_WINDOW (window), "Edit Item Node");
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
  gtk_window_set_modal (GTK_WINDOW (window), TRUE);
  gtk_window_set_policy (GTK_WINDOW (window), FALSE, FALSE, FALSE);
  
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox1", vbox1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window), vbox1);
  
  // the GtkNotebook with the different 'pages' of the window
  notebook1 = gtk_notebook_new ();
  gtk_widget_ref (notebook1);
  gtk_object_set_data_full (GTK_OBJECT (window), "notebook1", notebook1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (notebook1);
  gtk_box_pack_start (GTK_BOX (vbox1), notebook1, TRUE, TRUE, 0);
  gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook1), FALSE);
  gtk_notebook_set_tab_hborder (GTK_NOTEBOOK (notebook1), 6);
  GTK_WIDGET_UNSET_FLAGS (notebook1, GTK_CAN_FOCUS);
  

  gui_edit_dialog_set_up_first_page ( notebook1 , item_index );

  gui_edit_dialog_set_up_second_page ( notebook1 , item_index );

  gui_edit_dialog_set_up_third_page ( notebook1 , item_index );

  //--------------------
  // And now the final button row, that will contain the ok and cancel
  // buttons, that are always visible, no matter what page is selected.
  //
  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbuttonbox1", hbuttonbox1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox1), 0);
  
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_item_index_interface(...): now adding 'ok' button..." );

  // ok button
  ok_button = gtk_button_new_with_label ("OK");
  gtk_widget_ref (ok_button);
  gtk_object_set_data_full (GTK_OBJECT (window), "ok_button", ok_button, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ok_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), ok_button);
  GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);
  
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_item_index_interface(...): now adding 'cancel' button..." );

  // cancel button
  cancel_button = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (window), "cancel_button", cancel_button, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);
  
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_item_index_interface(...): now adding input handlers..." );

  // the various input handlers
  gtk_signal_connect (GTK_OBJECT (notebook1), "switch_page", GTK_SIGNAL_FUNC (on_switch_page), window);
  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked", GTK_SIGNAL_FUNC ( edit_interface_ok_button_pressed ), &(item_index ) );
  gtk_signal_connect (GTK_OBJECT (cancel_button), "clicked", GTK_SIGNAL_FUNC ( edit_interface_cancel_button_pressed ), window );
  gtk_signal_connect (GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC ( edit_interface_cancel_button_pressed ), window );
  
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_item_index_interface(...): now giving focus..." );
  // give focus to the text entry
  gtk_widget_grab_focus ( item_description_entry );
  // gtk_widget_grab_default ( item_description_entry );
  gtk_object_set_data (GTK_OBJECT (window), "tooltips", tooltips);
  
  //--------------------
  // Clear?
  //
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_item_index_interface(...): showing window..." );
  gtk_widget_show ( window );
  DebugPrintf ( PAGE_SETUP_DEBUG , "\ngui_edit_item_index_interface(...): starting up user control..." );
  gtk_main();
  
}; // void gui_edit_item_index_interface ( void )

/* ----------------------------------------------------------------------
 * In order to have some other colors than white and black available in 
 * the dialog editors graph window, we need to define these colors first.
 * This function should do the trick, PROVIDED THAT THE WIDGET GIVEN HAS
 * ALREADY BEEN REALIZED FROM THE GTK!  THIS MUST BE ENSURED!!!
 * ---------------------------------------------------------------------- */
void
gui_set_up_color_arrangements ( GtkWidget *widget ) 
{
    GdkColor c;

    DebugPrintf( 1 , "\nNOW SETTING UP COLORS FOR LATER USE..." );

    // Yellow
    Gui_Color[GC_YELLOW] = gdk_gc_new (gtk_widget_get_parent_window (widget));
    c.red = 65535;
    c.green = 32700;
    c.blue = 0;
    gdk_colormap_alloc_color (gtk_widget_get_colormap (widget), &c, TRUE, TRUE);
    gdk_gc_set_foreground (Gui_Color[GC_YELLOW], &c);

    // Orange
    Gui_Color[GC_RED] = gdk_gc_new (gtk_widget_get_parent_window (widget));
    c.red = 65535;
    c.green = 16350;
    c.blue = 0;
    gdk_colormap_alloc_color (gtk_widget_get_colormap (widget), &c, TRUE, TRUE);
    gdk_gc_set_foreground (Gui_Color[GC_RED], &c);

    // Dark Red
    Gui_Color[GC_DARK_RED] = gdk_gc_new (gtk_widget_get_parent_window (widget));
    c.red = 65535;
    c.green = 0;
    c.blue = 0;
    gdk_colormap_alloc_color (gtk_widget_get_colormap (widget), &c, TRUE, TRUE);
    gdk_gc_set_foreground (Gui_Color[GC_DARK_RED], &c);

    // Dark Blue
    Gui_Color[GC_DARK_BLUE] = gdk_gc_new (gtk_widget_get_parent_window (widget));
    c.red = 0;
    c.green = 0;
    c.blue = 35000;
    gdk_colormap_alloc_color (gtk_widget_get_colormap (widget), &c, TRUE, TRUE);
    gdk_gc_set_foreground (Gui_Color[GC_DARK_BLUE], &c);

    // Green
    Gui_Color[GC_GREEN] = gdk_gc_new (gtk_widget_get_parent_window (widget));
    c.red = 16000;
    c.green = 50000;
    c.blue = 5000;
    gdk_colormap_alloc_color (gtk_widget_get_colormap (widget), &c, TRUE, TRUE);
    gdk_gc_set_foreground (Gui_Color[GC_GREEN], &c);

    // Dark Green
    Gui_Color[GC_DARK_GREEN] = gdk_gc_new (gtk_widget_get_parent_window (widget));
    c.red = 0;
    c.green = 27300;
    c.blue = 15600;
    gdk_colormap_alloc_color (gtk_widget_get_colormap (widget), &c, TRUE, TRUE);
    gdk_gc_set_foreground (Gui_Color[GC_DARK_GREEN], &c);

    // Grey
    Gui_Color[GC_GREY] = gdk_gc_new (gtk_widget_get_parent_window (widget));
    c.red = 40000;
    c.green = 40000;
    c.blue = 40000;
    gdk_colormap_alloc_color (gtk_widget_get_colormap (widget), &c, TRUE, TRUE);
    gdk_gc_set_foreground (Gui_Color[GC_GREY], &c);

    // Black
    Gui_Color[GC_BLACK] = gdk_gc_new (gtk_widget_get_parent_window (widget));
    c.red = 0;
    c.green = 0;
    c.blue = 0;
    gdk_colormap_alloc_color (gtk_widget_get_colormap (widget), &c, TRUE, TRUE);
    gdk_gc_set_foreground (Gui_Color[GC_BLACK], &c);
    
    // White
    Gui_Color[GC_WHITE] = gdk_gc_new (gtk_widget_get_parent_window (widget));
    c.red = 65535;
    c.green = 65535;
    c.blue = 65535;
    gdk_colormap_alloc_color (gtk_widget_get_colormap (widget), &c, TRUE, TRUE);
    gdk_gc_set_foreground (Gui_Color[GC_WHITE], &c);

}; // void gui_set_up_color_arrangements ( GtkWidget *widget ) 

GdkGC *gui_get_color ( int color )
{
    if (color < 0 || color > MAX_GC) 
      {
	// return (GdkGC *) NULL;
	DebugPrintf( 0 , "\n*getColor:  SEVERE ERROR!  ILLEGAL COLOR REQUESTED..." );
	Terminate ( ERR );
	return (GdkGC *) NULL;
      }
    else return Gui_Color[color];
}; // GdkGC *gui_get_color ( int color )

/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void
gui_destroy_existing_tooltips ( void )
{
  if ( popup_meta_tool_tip != NULL ) gtk_widget_destroy (popup_meta_tool_tip);
  popup_meta_tool_tip = NULL ;
}; // void gui_destroy_existing_tooltips ( void )

/* ----------------------------------------------------------------------
 * Whenever the mouse cursor hovers over a box of a dialog option, there
 * should be a small window with the main content of this dialog box 
 * poping up and staying for as long as the mouse cursor also stays there.
 * This function pops up and maintains this small, nifty popup window.
 * ---------------------------------------------------------------------- */
void
gui_show_tooltip ( int x , int y )
{
  static GtkWidget *frame;
  static GtkWidget *tip;
  static char popup_meta_tool_tip_text[20000];
  // char temp_text[20000];

  //--------------------
  // At first we need to find out which dialog option really
  // is below the mouse cursor, if one at all...
  //
  // item_covered = item_under_position ( x , y ) ;

  //--------------------
  // Maybe none at all, then we can destroy any existing
  // popup_meta_tool_tip and then just quit...
  //
  if ( item_under_position ( x , y ) == (-1) )
    {
      gui_destroy_existing_tooltips ();
      item_covered = item_under_position ( x , y ) ;
      return ;
    }

  if ( item_under_position ( x , y ) != item_covered )
    {
      gui_destroy_existing_tooltips ( );
      item_covered = item_under_position ( x , y ) ;
    }

  //--------------------
  // So now we know that we must display the text of this dialog
  // option, or at least the first (few) line(s) of this dialog
  // option.
  //

  //--------------------
  // At first we need to create a new popup_meta_tool_tip...
  //
  if ( popup_meta_tool_tip == NULL )
    {
      //--------------------
      // At first we assemble a new popup window
      //
      DebugPrintf ( 1 , "\n NEW POPUP_META_TOOL_TIP IS BEING ASSEMBLED!!" );
      popup_meta_tool_tip = gtk_window_new (GTK_WINDOW_POPUP);
      gtk_object_set_data (GTK_OBJECT (popup_meta_tool_tip), "tip_window", popup_meta_tool_tip);
      gtk_window_set_policy (GTK_WINDOW (popup_meta_tool_tip), FALSE, FALSE, FALSE);

      //--------------------
      // Now stuff some border around the label and everything...
      //
      frame = gtk_frame_new (NULL);
      gtk_widget_ref (frame);
      gtk_object_set_data_full (GTK_OBJECT (popup_meta_tool_tip), "frame", frame, (GtkDestroyNotify) gtk_widget_unref);
      gtk_widget_show (frame);
      gtk_container_add (GTK_CONTAINER (popup_meta_tool_tip), frame);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);

      // label with the text
      sprintf( popup_meta_tool_tip_text , "Item Nr: %d\n" , item_covered );
      strcat( popup_meta_tool_tip_text , ItemMap [ item_covered ] . item_name );
      tip = gtk_label_new ( popup_meta_tool_tip_text );

      gtk_widget_ref (tip);
      gtk_object_set_data_full (GTK_OBJECT (popup_meta_tool_tip), "tip", tip, (GtkDestroyNotify) gtk_widget_unref);
      gtk_widget_show (tip);

      gtk_container_add (GTK_CONTAINER (frame), tip);
      gtk_label_set_justify (GTK_LABEL (tip), GTK_JUSTIFY_LEFT);
      gtk_label_set_line_wrap (GTK_LABEL (tip), TRUE);
      gtk_misc_set_padding (GTK_MISC (tip), 4, 1);

      //--------------------
      // Now we can really draw out popup_meta_tool_tip to the screen
      //
      gtk_widget_realize (popup_meta_tool_tip);
    
      // calculate the position of the popup_meta_tool_tip
      x = ItemMap [ item_covered ] . position_x ;
      y = ItemMap [ item_covered ] . position_y ;
      
      // position and display the popup_meta_tool_tip
      gtk_widget_set_uposition (popup_meta_tool_tip, x , y );
      gtk_widget_show (popup_meta_tool_tip);
    }
}; // void gui_show_tooltip ( int x , int y )

/* ----------------------------------------------------------------------
 * If there was a mouse click or something, we would like to know if that
 * mouse click occured directly over some dialog option in the graph
 * window or not.  So this function shall find out which option was below
 * the given position, or, if none at all was below, then (-1) will be
 * returned.
 * ---------------------------------------------------------------------- */
int 
item_under_position ( int x , int y )
{
  int i;

  for ( i = 0 ; i < Number_Of_Item_Types ; i ++ )
    {
      if ( ItemMap [ i ] . position_x == (-1) ) continue;
      if ( ItemMap [ i ] . position_y == (-1) ) continue;

      if ( ( abs ( ItemMap [ i ] . position_x - x ) < 1 * BOX_RADIUS_X ) &&
	   ( abs ( ItemMap [ i ] . position_y - y ) < 1 * BOX_RADIUS_Y ) )
	return ( i ) ;

    }

  return ( -1 );
}; // int item_under_position ( int x , int y) 

// Mouse-button released on Drawing Area
gint button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{

  switch ( event -> button )
    {
    case 1:
      currently_mouse_grabbed_option = (-1) ;
      DebugPrintf ( 1 , "\n Now the 'Drop' in 'Drag and Drop' has occured..." );
      break;
    default:
      DebugPrintf ( 1 , "\n Unhandled mouse button release event encountered..." );
      break;
    }

    return TRUE;

}

// key pressed in Drawing Area
gint key_press_event ( GtkWidget *widget , GdkEventButton *event , gpointer data )
{

  DebugPrintf ( 1 , "\ngint key_press_event ( ... ) : real function call confirmed..." );

  switch ( ( ( GdkEventKey* ) event) -> keyval )
    {
    case GDK_BackSpace:
    case GDK_Delete:
      item_index_delete_marked ( wnd , NULL , (gpointer) NULL ) ;
      // currently_mouse_grabbed_option = (-1) ;
      // DebugPrintf ( 1 , "\n Now the 'Drop' in 'Drag and Drop' has occured..." );
      break;
    case GDK_Insert:

      break;

    default:
      DebugPrintf ( 1 , "\nKey press event encountered..." );
      break;
    }

    return TRUE;
}

/* ----------------------------------------------------------------------
 * This function should just draw the boxes of the currently loaded
 * dialog.  Nothing more than that is done.  No update of any form is
 * performed.
 * ---------------------------------------------------------------------- */
void
gui_draw_all_item_boxes( void )
{
  int i;
  GdkGC *box_color;

  for ( i = 0 ; i < Number_Of_Item_Types ; i ++ )
    {
      //--------------------
      // Of course we draw only those dialog boxes, that are really in use...
      //
      if ( ItemMap [ i ] . position_x == (-1) ) continue;
      if ( ItemMap [ i ] . position_y == (-1) ) continue;

      //--------------------
      // If that box is the one currently marked, then we'll draw it in red color,
      // otherwise it will be in black color.
      //
      if ( currently_marked_item_index == i )
	box_color = gui_get_color ( GC_RED );
      else
	box_color = gui_get_color ( GC_BLACK );

      //--------------------
      // Here we draw the diamond...
      //
      gdk_draw_line ( surface , box_color , 
		      ItemMap [ i ] . position_x - BOX_RADIUS_X , ItemMap [ i ] . position_y , 
		      ItemMap [ i ] . position_x , ItemMap [ i ] . position_y + BOX_RADIUS_Y );
      gdk_draw_line ( surface , box_color, 
		      ItemMap [ i ] . position_x , ItemMap [ i ] . position_y + BOX_RADIUS_Y , 
		      ItemMap [ i ] . position_x + BOX_RADIUS_X , ItemMap [ i ] . position_y );
      gdk_draw_line ( surface , box_color , 
		      ItemMap [ i ] . position_x + BOX_RADIUS_X , ItemMap [ i ] . position_y , 
		      ItemMap [ i ] . position_x , ItemMap [ i ] . position_y - BOX_RADIUS_Y );
      gdk_draw_line ( surface , box_color , 
		      ItemMap [ i ] . position_x , ItemMap [ i ] . position_y - BOX_RADIUS_Y , 
		      ItemMap [ i ] . position_x - BOX_RADIUS_X , ItemMap [ i ] . position_y );

    }
}; // void gui_draw_all_item_boxes( void )

/* ----------------------------------------------------------------------
 *
 * This function should redraw the current graph completely.  Whenever
 * something has changed, this function should be called, so that the
 * current state is again reflected on the screen.
 *
 * ---------------------------------------------------------------------- */
void 
gui_redraw_graph_completely ( void )
{
  GdkRectangle update_rect;

  gui_clear_graph_window ();

  gui_draw_all_item_boxes();

  //--------------------
  // Now that we have drawn a lot, we need to update the graph window, which
  // we'll do here...
  //
  update_rect.x = 0 ;
  update_rect.y = 0 ; 
  update_rect.width = graph -> allocation.width ;
  update_rect.height = graph -> allocation.height ;
  gtk_widget_draw ( wnd , &update_rect);

}; // void gui_redraw_graph_completely ( void )

/* ----------------------------------------------------------------------
 * The following function provides a standard GTK-based interaction with
 * the user, where the user can select a file and browse the directory
 * structure of the file system via the mouse or keyboard as usual.  All
 * of the functionality is provided by the GTK, while this function only
 * needs to take the full path and file name of the final user selected
 * file.
 * ---------------------------------------------------------------------- */
void 
load_dialog_file_selector( GtkWidget *w, GtkFileSelection *fs )
{
  DebugPrintf ( 1 ,"\n ATTENTION!  A file name has been selected (for loading)!" );
  DebugPrintf ( 1 ,"\n The file name is : %s" , gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( fs ) ) ) ;
  DebugPrintf ( 1 ,"\n ...now attempting to item archetypes file..." );
  DebugPrintf ( 1 ,"\n----------------------------------------------------------------------" );

  strcpy ( LastUsedFileName , gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( fs ) ) ) ;
  // LoadChatRosterWithChatSequence ( gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( fs ) ) );

  raw_item_data_from_file = ReadAndMallocAndTerminateFile( gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( fs ) ) ,  "*** End of this Freedroid data File ***" ) ;

  Get_Item_Data ( raw_item_data_from_file );

  //--------------------
  // Now that we have loaded something, the main window should show the current file name
  //
  gtk_window_set_title ( GTK_WINDOW ( wnd ), LastUsedFileName );

  //--------------------
  // Now we set the 'authors notes' text in the lower part of the split window...
  //
  gtk_text_set_point ( GTK_TEXT ( authors_notes_entry ) , 0 );
  gtk_text_forward_delete ( GTK_TEXT ( authors_notes_entry ) , gtk_text_get_length ( GTK_TEXT ( authors_notes_entry ) ) ) ;
  gtk_text_insert ( GTK_TEXT ( authors_notes_entry ) , authors_notes_entry->style->font,
		    &authors_notes_entry->style->black,
		    &authors_notes_entry->style->white,
		    authors_notes ,
		    -1 );
  


  DebugPrintf ( 1 ,"\n----------------------------------------------------------------------" );
  DebugPrintf ( 1 ,"\n ...Item archetype file should be loaded now by now." );

  gtk_widget_destroy ( filew ) ;

  //--------------------
  // Now that we have loaded a new dialog roster, we must redraw the current
  // dialog graph as well...
  //
  gui_redraw_graph_completely (  );

}; // void load_dialog_file_selector( GtkWidget *w, GtkFileSelection *fs )

/* ----------------------------------------------------------------------
 * The following function provides a standard GTK-based interaction with
 * the user, where the user can select a file and browse the directory
 * structure of the file system via the mouse or keyboard as usual.  All
 * of the functionality is provided by the GTK, while this function only
 * needs to take the full path and file name of the final user selected
 * file.
 * ---------------------------------------------------------------------- */
void 
save_as_dialog_file_selector( GtkWidget *w, GtkFileSelection *fs )
{
  DebugPrintf ( 1 ,"\n ATTENTION!  A file name has been selected (for saving)!" );
  DebugPrintf ( 1 ,"\n The file name is : %s" , gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( fs ) ) ) ;  
  DebugPrintf ( 1 ,"\n ...now attempting to save dialog file..." );

  enforce_authors_notes (  );

  strcpy ( LastUsedFileName , gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( fs ) ) ) ;
  save_item_roster_to_file ( gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( fs ) ) );

  //--------------------
  // Now that we have given a new name to the file, the main window should show the current file name
  //
  gtk_window_set_title ( GTK_WINDOW ( wnd ), LastUsedFileName );

  DebugPrintf ( 1 ,"\n ...Item archetype file should be saved by now." );
 
  gtk_widget_destroy ( filew ) ;

}; // void save_as_dialog_file_selector( GtkWidget *w, GtkFileSelection *fs )

/* ----------------------------------------------------------------------
 * This is the 
 *
 * ---------------------------------------------------------------------- */
void
Terminate ( int exit_status ) 
// Terminate ( GtkWidget *widget, GdkEvent  *event, gpointer data ) 
{
  printf ( "\n----------------------------------------------------------------------\n" );
  printf ( "Termination of the FreedroidRPG Item Editor invoked.  \n\
Thank you for contibuting to FreedroidRPG.  \n\
Good bye.\n" );

  gtk_main_quit();
  gtk_exit ( exit_status );
  exit ( exit_status );

};

/* ----------------------------------------------------------------------
 * This is the callback function used to exit the current GTK main window.
 * important that this function terminates the 
 * ---------------------------------------------------------------------- */
gint delete_event( GtkWidget *widget, GdkEvent  *event, gpointer data )
{
  printf ( "\n----------------------------------------------------------------------" );
  printf ( "\nTermination of the current GTK main window invoked." );
  printf ( "\n----------------------------------------------------------------------\n" );

  gtk_main_quit();
  return(FALSE);

}; // gint delete_event( GtkWidget *widget, GdkEvent  *event, gpointer data )

/* another callback */
gint on_display_help( GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   data )
{
  // gtk_main_quit();
  DebugPrintf ( 1 , "\nGot 'on_display_help': " );
  DebugPrintf ( 1 , "%s" , (char*) data );
  return(FALSE);
}

/* another callback */
gint on_clear_help( GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   data )
{
  // gtk_main_quit();
  DebugPrintf ( 1 , "\nGot 'on_clear_help': " );
  DebugPrintf ( 1 , "%s" , (char*) data );
  return(FALSE);
}

gint 
on_file_new_activate ( GtkWidget *widget, GdkEvent  *event ) // ,                    gpointer   data )
{
  clear_item_archetype_array (   );

  //--------------------
  // We can assume, that once the 'File->New...' button is pressed,
  // the window is already there and drawn once, so we may safely
  // access the window for clearing the graph now, right?
  //
  gui_redraw_graph_completely ( ) ;
  strcpy ( LastUsedFileName , "UNDEFINED_FILENAME.dialog" ) ;
  gtk_window_set_title ( GTK_WINDOW ( wnd ) , "Freedroid Item Editor: No file loaded!" );

  return(FALSE);
}

/* ----------------------------------------------------------------------
 *
 * This is the callback function that should do a dialog where the user
 * can specify the dialog file he wishes to load.
 *
 * Do not change the parameters of this function, since it has to be of
 * this specific form in order to be of proper callback function format.
 *
 * ---------------------------------------------------------------------- */
gint 
file_load_event_callback_function ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
  DebugPrintf ( 1 , "\nfile_load_event_callback_function(...): function call confirmed." );
    
  /* Create a new file selection widget */
  filew = gtk_file_selection_new ("File selection");
  
  gtk_signal_connect (GTK_OBJECT (filew), "destroy",
		      (GtkSignalFunc) delete_event, &filew);
  /* Connect the ok_button to load_dialog_file_selector function */
  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
		      "clicked", (GtkSignalFunc) load_dialog_file_selector , filew );
  
  /* Connect the cancel_button to destroy the widget */
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION
					 (filew)->cancel_button),
			     "clicked", (GtkSignalFunc) gtk_widget_destroy,
			     GTK_OBJECT (filew));
  
  /* Lets set the filename, as if this were a save dialog, and we are giving
     a default filename */
  gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew), 
				   "../map/freedroid.item_archetypes");
  
  gtk_widget_show(filew);
  gtk_main ();
  
  return(FALSE);
}; // gint file_load_event_callback_function ( GtkWidget *widget, GdkEvent *event, gpointer data )

/* ----------------------------------------------------------------------
 *
 * This is the callback function that should do a dialog where the user
 * can specify the dialog file he wishes to load.
 *
 * Do not change the parameters of this function, since it has to be of
 * this specific form in order to be of proper callback function format.
 *
 * ---------------------------------------------------------------------- */
gint 
file_save_event_callback_function ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{

  DebugPrintf ( 1 , "\nfile_save_event_callback_function(...): function call confirmed." );

  //--------------------
  // Now we only need to save the file to the current file name,
  // which must be the same as used to load the file...
  //
  DebugPrintf ( 1 ,"\n ...now attempting to save item archetype file..." );

  enforce_authors_notes (  );
  save_item_roster_to_file ( LastUsedFileName );

  DebugPrintf ( 1 ,"\n ...Item archetype file should be saved by now." );

  return(FALSE);

}; // gint file_save_event_callback_function ( GtkWidget *widget, GdkEvent *event, gpointer data )

/* ----------------------------------------------------------------------
 *
 * This is the callback function that should do a dialog where the user
 * can specify the dialog file he wishes to load.
 *
 * Do not change the parameters of this function, since it has to be of
 * this specific form in order to be of proper callback function format.
 *
 * ---------------------------------------------------------------------- */
gint 
file_save_as_event_callback_function ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{

  DebugPrintf ( 1 , "\nfile_save_as_event_callback_function(...): function call confirmed." );
    
  /* Create a new file selection widget */
  filew = gtk_file_selection_new ("File selection");
  
  gtk_signal_connect (GTK_OBJECT (filew), "destroy",
		      (GtkSignalFunc) delete_event, &filew);
  /* Connect the ok_button to load_dialog_file_selector function */
  gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),
		      "clicked", (GtkSignalFunc) save_as_dialog_file_selector , filew );
  
  /* Connect the cancel_button to destroy the widget */
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION
					 (filew)->cancel_button),
			     "clicked", (GtkSignalFunc) gtk_widget_destroy,
			     GTK_OBJECT (filew));
  
  /* Lets set the filename, as if this were a save dialog, and we are giving
     a default filename */
  gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew), 
				   "../map/test.item_archetypes");
  
  gtk_widget_show(filew);
  gtk_main ();
  
  return(FALSE);

}; // gint file_save_as_event_callback_function ( GtkWidget *widget, GdkEvent *event, gpointer data )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
gint 
help_about_freedroidRPG_dialog_editor ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
  char* license_string;

  //--------------------
  // We say, that we're here...
  //
  DebugPrintf ( 1 , "\nhelp_about_freedroidRPG_dialog_editor (...): real function call confirmed...");

#include "assign_license_string.h"

  gui_start_pure_text_popup_window ( license_string  , "About FreedroidRPG Item Editor" );

  return ( FALSE );

}; // gint help_about_freedroidRPG_dialog_editor ( GtkWidget *widget, GdkEvent  *event, gpointer   data )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
gint 
help_about_freedroid ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
  //--------------------
  // We say, that we're here...
  //
  DebugPrintf ( 1 , "\nhelp_about_freedroid (...): real function call confirmed...");

  gui_start_pure_text_popup_window ( "\n\
\n\
    FreedroidRPG is a graphical single player role playing game.\n\
\n\
This project is done entirely by volunteers in their free time,\n\
not receiving any income except experience and the fun of doing\n\
it for their work.\n\
\n\
If you feel like contributing to this project, you're more than welcome.\n\
Please contact in this case our main mailing list, i.e. please send e-mail\n\
to\n\
\n\
    freedroid-discussion@lists.sourceforge.net\n\
\n\
We'd appreciate any help or also just some feedback on what you think of\n\
the project and what things you liked or disliked and what could be improved\n\
in future versions of FreedroidRPG.\n\
\n\
Thank you and see ya, the FreedroidRPG dev team,\n\
\n\
\n" , "About FreedroidRPG"  );

  return ( FALSE );

}; // gint help_about_freedroid ( GtkWidget *widget, GdkEvent  *event, gpointer   data )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
gint 
help_reporting_bugs ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
  //--------------------
  // We say, that we're here...
  //
  DebugPrintf ( 1 , "\nhelp_reporting_bugs (...): real function call confirmed...");

  gui_start_pure_text_popup_window ( "\n\
\n\
This item editor has been tested, but that does by no means\n\
mean that it is completely bug-free.\n\
\n\
If you think you have found some bug, please contact the FreedroidRPG\n\
developers, best by sending e-mail to the following address:\n\
\n\
freedroid-discussion@lists.sourceforge.net\n\
\n\
Thanks a lot for submitting any bug or feedback or, of course and even\n\
more appreciated, new or patched items that we can\n\
include in FreedroidRPG.\n\
\n\
Have fun and see ya, the FreedroidRPG dev team.\n\
\n\
\n" , "Reporting Bugs" );

  return ( FALSE );

}; // gint help_reporting_bugs ( GtkWidget *widget, GdkEvent  *event, gpointer   data )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
gint 
help_dialog_editor_usage ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
  //--------------------
  // We say, that we're here...
  //
  DebugPrintf ( 1 , "\nhelp_dialog_editor_usage (...): real function call confirmed...");

  gui_start_pure_text_popup_window ( "\n\
\n\
     ITEM EDITOR USAGE\n\
\n\
Use the left mouse button to drag and drop items to their desired position.\n\
\n\
Use right-mouse button directly on the option.  That should open up a popup\n\
window and prompt you for new input.\n\
\n\
If there is no 3d model yet for the item in question, you can use the 'diable'\n\
button to supply a default path to the 'questionmark' 3d rendering, indicating\n\
that this is an end where FreedroidRPG has not yet been developed further.\n\
This will cause the keyword NONE_AVAILABLE_YET to appear.\n\
\n\
\n\
\n" , "FreedroidRPG Item Editor Usage" );

  return ( FALSE );

}; // gint help_dialog_editor_usage ( GtkWidget *widget, GdkEvent  *event, gpointer   data )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
gint 
item_index_insert_new ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
  itemspec* new_item_map;

  //--------------------
  // We say, that we're here...
  //
  DebugPrintf ( 1 , "\nitem_index_insert_new (...): real function call confirmed...");

  //--------------------
  // Now we allocate some new (bigger) memory chunk, copy over the old
  // information (and maybe even free the old information after that? --> better not, it might be empty?)
  // and have our fine new structures, maybe with some special coordinates
  // and startup values...
  //
  new_item_map = MyMalloc ( ( Number_Of_Item_Types + 2 ) * sizeof ( itemspec ) );
  memcpy ( new_item_map , ItemMap , sizeof ( itemspec ) * Number_Of_Item_Types );
  ItemMap = new_item_map;

  ItemMap [ Number_Of_Item_Types ] . position_x = 20 ;
  ItemMap [ Number_Of_Item_Types ] . position_y = 20 ; 

  ItemMap [ Number_Of_Item_Types ] . item_name = MyMalloc ( strlen ( "New item just created" ) + 2 ) ;
  strcpy ( ItemMap [ Number_Of_Item_Types ] . item_name , "New item just created" ) ;

  ItemMap [ Number_Of_Item_Types ] . item_rotation_series_prefix = MyMalloc ( strlen ( "NONE_AVAILABLE_YET" ) + 2 ) ;
  strcpy ( ItemMap [ Number_Of_Item_Types ] . item_rotation_series_prefix , "NONE_AVAILABLE_YET" ) ;

  ItemMap [ Number_Of_Item_Types ] . item_description = MyMalloc ( strlen ( "no description for this one yet" ) + 2 ) ;
  strcpy ( ItemMap [ Number_Of_Item_Types ] . item_description , "no description for this one yet" ) ;

  ItemMap [ Number_Of_Item_Types ] . item_drop_sound_file_name = MyMalloc ( strlen ( "Item_Drop_Sound_0.wav" ) + 2 ) ;
  strcpy ( ItemMap [ Number_Of_Item_Types ] . item_drop_sound_file_name , ( "Item_Drop_Sound_0.wav" ) ) ;

  ItemMap [ Number_Of_Item_Types ] . item_can_be_applied_in_combat = FALSE ;
  ItemMap [ Number_Of_Item_Types ] . item_can_be_installed_in_influ = FALSE ;
  ItemMap [ Number_Of_Item_Types ] . item_can_be_installed_in_weapon_slot = FALSE ;
  ItemMap [ Number_Of_Item_Types ] . item_can_be_installed_in_drive_slot = FALSE ;
  ItemMap [ Number_Of_Item_Types ] . item_can_be_installed_in_armour_slot = FALSE ;
  ItemMap [ Number_Of_Item_Types ] . item_can_be_installed_in_shield_slot = FALSE ;
  ItemMap [ Number_Of_Item_Types ] . item_can_be_installed_in_special_slot = FALSE ;
  ItemMap [ Number_Of_Item_Types ] . item_can_be_installed_in_aux_slot = FALSE ;

  ItemMap [ Number_Of_Item_Types ] . item_group_together_in_inventory = FALSE ;

  //--------------------
  // How good is the item as weapon???
  //

  // if this is set, then you can't use a shield together with the weapon
  ItemMap [ Number_Of_Item_Types ] . item_gun_is_motion_class = FALSE ;	 

  // time until the next shot can be made, measures in seconds
  ItemMap [ Number_Of_Item_Types ] . item_gun_recharging_time = 0.5 ;       

  // which type of image to use for displaying this bullet
  ItemMap [ Number_Of_Item_Types ] . item_gun_bullet_image_type = 0 ;       

  // if this is set, there is only 1 shot 
  ItemMap [ Number_Of_Item_Types ] . item_gun_oneshotonly = FALSE ;

  // how fast should a bullet move straightforward?
  ItemMap [ Number_Of_Item_Types ] . item_gun_speed = 1.0 ;

  // damage done by this bullettype 
  ItemMap [ Number_Of_Item_Types ] . base_item_gun_damage = 10.0 ; 

  // modifier to the damage done by this bullettype 
  ItemMap [ Number_Of_Item_Types ] . item_gun_damage_modifier = 2.0 ; 

  // how long does a 'bullet' from this gun type live?
  ItemMap [ Number_Of_Item_Types ] . item_gun_bullet_lifetime = 5.0 ;

  // can this 'bullet' reflect other bullets
  ItemMap [ Number_Of_Item_Types ] . item_gun_bullet_reflect_other_bullets = FALSE ; 

  // can this 'bullet' reflect other bullets
  ItemMap [ Number_Of_Item_Types ] . item_gun_bullet_pass_through_explosions = FALSE ; 

  // does this bullet go through hit bodies (e.g. like a laser sword)
  ItemMap [ Number_Of_Item_Types ] . item_gun_bullet_pass_through_hit_bodies = FALSE ; 

  // can this bullet pass through walls and map barriers?
  ItemMap [ Number_Of_Item_Types ] . item_gun_bullet_ignore_wall_collisions = FALSE ; 

  //--------------------
  // the following values have only relevance in case of a melee weapon
  //

  // how fast to do a melee weapon swing
  ItemMap [ Number_Of_Item_Types ] . item_gun_angle_change = 0 ;

  // where to start with a melee weapon swing
  ItemMap [ Number_Of_Item_Types ] . item_gun_start_angle_modifier = 0 ;

  // how far away from the swinger should a melee weapon swing occur?
  ItemMap [ Number_Of_Item_Types ] . item_gun_fixed_offset = 1.3 ;


  // which ammunition does this gun use?
  ItemMap [ Number_Of_Item_Types ] . item_gun_use_ammunition = -1 ; 

  // how good is the item as armour or shield or other protection???
  ItemMap [ Number_Of_Item_Types ] . base_ac_bonus = 0 ;
  ItemMap [ Number_Of_Item_Types ] . ac_bonus_modifier = 0 ;

  // which requirement for strength, dexterity and magic (force) does the item have?
  ItemMap [ Number_Of_Item_Types ] . item_require_strength = 0 ;
  ItemMap [ Number_Of_Item_Types ] . item_require_dexterity = 0 ;
  ItemMap [ Number_Of_Item_Types ] . item_require_magic = 0 ;
  
  // what duration does the item have?
  ItemMap [ Number_Of_Item_Types ] . base_item_duration = 40 ;
  ItemMap [ Number_Of_Item_Types ] . item_duration_modifier = 20 ;

  // the base price of this item at the shop
  ItemMap [ Number_Of_Item_Types ] . base_list_price = 1000 ;

  //--------------------
  // Now that the new item is there, we must increase the total number
  // of items counter, so that this last item will be found too.
  //
  Number_Of_Item_Types ++ ;

  //--------------------
  // So an option was inserted.  This means we have to redraw the screen, so that
  // the change made can become visible as well...
  //
  gui_redraw_graph_completely ( ); 

  return ( FALSE );
}; // gint item_index_insert_new ( GtkWidget *widget, GdkEvent  *event, gpointer   data )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
gint 
item_index_delete_marked ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
  int i;
  int j;

  //--------------------
  // We say, that we're here...
  //
  DebugPrintf ( 1 , "\nitem_index_delete_marked (...): real function call confirmed...");

  //--------------------
  // Now if there is no dialog option marked, we don't have to
  // do a thing and can return immediately...
  //
  if ( currently_marked_item_index == (-1) )
    return ( FALSE );

  //--------------------
  // We clear out this dialog option, not taking into account the 
  // neglectable memory leak here...
  //
  // delete_one_item_index ( currently_marked_item_index , TRUE ); 

  //--------------------
  // But all other connections, that connected into this dialog option
  // now don't make any sense any more and must be removed as well...
  //
  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      for ( j = 0 ; j < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; j ++ )
	{
	  if ( ChatRoster [ i ] . change_option_nr [ j ] == currently_marked_item_index )
	    {
	      ChatRoster [ i ] . change_option_nr [ j ] = (-1) ;
	      ChatRoster [ i ] . change_option_to_value [ j ] = (-1) ;
	    }
	}
    }

  //--------------------
  // And since the marked dialog option doesn't exist any more now,
  // we must of course also remove the mark that is still on it.
  //
  currently_marked_item_index = (-1) ;

  //--------------------
  // So an option was removed, i.e. we have to redraw the screen, so that
  // the change made can become visible as well...
  //
  gui_redraw_graph_completely ( ); 

  return ( FALSE );
}; // gint item_index_delete_marked ( GtkWidget *widget, GdkEvent  *event, gpointer   data )

/* ----------------------------------------------------------------------
 * This function creates the main window of the FreedroidRPG Item Editor.
 * It also sets the proper window title.  This is NOT a callback function.
 * ---------------------------------------------------------------------- */
void
gui_create_main_window ( void )
{
  //--------------------
  // Now we create the main window
  wnd = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_usize (GTK_WIDGET (wnd), 700, 740);
  gtk_signal_connect (GTK_OBJECT (wnd), "delete_event", GTK_SIGNAL_FUNC ( delete_event ), NULL);

  //--------------------
  // This window should also have a proper window title...
  //
  gtk_window_set_title (GTK_WINDOW ( wnd ), "Freedroid Item Editor: No file loaded!");

}; // void gui_create_main_window ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
gui_create_bottom_line( void )
{
  // Statusbar for displaying help and error messages
  status_help = gtk_statusbar_new ();

  // Status bars
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox);
  gtk_object_set_data_full (GTK_OBJECT (wnd), "hbox", hbox, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
  gtk_widget_set_usize (hbox, -2, 20);
  
  // help message
  gtk_widget_ref (status_help);
  gtk_object_set_data_full (GTK_OBJECT (wnd), "status_help", status_help, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (status_help);
  gtk_box_pack_start (GTK_BOX (hbox), status_help, TRUE, TRUE, 0);
  gtk_widget_set_usize (status_help, -2, 20);
  
  // program mode
  status_mode = gtk_statusbar_new ();
  gtk_widget_ref (status_mode);
  gtk_object_set_data_full (GTK_OBJECT (wnd), "status_mode", status_mode, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (status_mode);
  gtk_box_pack_start (GTK_BOX (hbox), status_mode, FALSE, TRUE, 0);
  gtk_widget_set_usize (status_mode, 150, -2);
  
}; // 

/* ----------------------------------------------------------------------
 *
 * This function should initialize the menu bar of the main window.  
 *
 * This is NOT a callback function.
 * 
 * ---------------------------------------------------------------------- */
void
gui_create_top_menu_line ( void )
{
  // Menu Accelerators
  // accel_group = gtk_accel_group_get_default ();
  
  // Main Windows Menu
  menu = gtk_menu_bar_new ();
  
  // Attach Menubar
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (wnd), vbox);
  gtk_widget_show (vbox);
  
  gtk_box_pack_start (GTK_BOX (vbox), menu, FALSE, FALSE, 2);
  
  // File Menu
  submenu = gtk_menu_new ();
  
  // New
  menuitem = gtk_menu_item_new_with_label ("New");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (1));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 1" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 2" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_new_activate), "NEW FILE??SURE??" );
  gtk_widget_show (menuitem);
  
  // Open
  menuitem = gtk_menu_item_new_with_label ("Open ...");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 3" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 4" );
  // gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_activate), "OPEN FILE?" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (file_load_event_callback_function), "OPEN FILE?" );
  gtk_widget_show (menuitem);
  
  // Save
  menuitem = gtk_menu_item_new_with_label ("Save");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 5" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 6" );
  // gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_activate), "OPEN FILE?" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( file_save_event_callback_function ), "SAVE FILE?" );
  gtk_widget_show (menuitem);
  
  // Save as...
  menuitem = gtk_menu_item_new_with_label ("Save as...");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 7" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 8" );
  // gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_activate), "OPEN FILE?" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( file_save_as_event_callback_function), "SAVE AS ... FILE?" );
  gtk_widget_show (menuitem);
  
  
  // Quit
  menuitem = gtk_menu_item_new_with_label ("Quit");
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (6));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "QUIT??" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "QUIT!!" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( delete_event ), (gpointer) NULL);
  gtk_widget_show (menuitem);
  
  // Attach File Menu
  menuitem = gtk_menu_item_new_with_label ("File");
  gtk_widget_show (menuitem);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
  gtk_menu_bar_append (GTK_MENU_BAR (menu), menuitem);

  //--------------------
  // Now that the file menu is done, we can start to attach another
  // line of menu options....  This time it will not be about 'File'
  // but rather about 'Item Node'
  //

  // File Menu
  submenu = gtk_menu_new ();
  
  // New
  menuitem = gtk_menu_item_new_with_label ("Insert New...");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (1));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 9" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 10" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( item_index_insert_new ), "NEW DIALOG OPTION?" );
  gtk_widget_show (menuitem);
  
  // Open
  menuitem = gtk_menu_item_new_with_label ("Delete Marked...");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 11" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 12" );
  // gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_activate), "OPEN FILE?" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( item_index_delete_marked ), "DELETE MARKED DIALOG OPTION?" );
  gtk_widget_show (menuitem);
  
  // Attach 'Item Node' Menu
  menuitem = gtk_menu_item_new_with_label ("Item Node");
  gtk_widget_show (menuitem);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
  gtk_menu_bar_append (GTK_MENU_BAR (menu), menuitem);
  
  //--------------------
  // Now that the dialog option menu is done, we can start to attach another
  // line of menu options....  This time it will not be about 'Item Node'
  // but rather about 'Help/About'
  //

  // File Menu
  submenu = gtk_menu_new ();
  
  // New
  menuitem = gtk_menu_item_new_with_label ("About FreedroidRPG Item Editor");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (1));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 9" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 10" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( help_about_freedroidRPG_dialog_editor ), "NEW DIALOG OPTION?" );
  gtk_widget_show (menuitem);
  
  // Open
  menuitem = gtk_menu_item_new_with_label ("About FreedroidRPG");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 11" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 12" );
  // gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_activate), "OPEN FILE?" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( help_about_freedroid ), "DELETE MARKED DIALOG OPTION?" );
  gtk_widget_show (menuitem);
  
  // Reporting bugs...
  menuitem = gtk_menu_item_new_with_label ("Reporting bugs...");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 11" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 12" );
  // gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_activate), "OPEN FILE?" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( help_reporting_bugs ), "DELETE MARKED DIALOG OPTION?" );
  gtk_widget_show (menuitem);
  
  // Using dialog editor...
  menuitem = gtk_menu_item_new_with_label ("Item Editor Usage");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 11" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 12" );
  // gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_activate), "OPEN FILE?" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( help_dialog_editor_usage ), "DELETE MARKED DIALOG OPTION?" );
  gtk_widget_show (menuitem);
  
  // Attach 'Item Node' Menu
  menuitem = gtk_menu_item_new_with_label ("Help");
  gtk_widget_show (menuitem);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
  gtk_menu_bar_append (GTK_MENU_BAR (menu), menuitem);
  
}; // void gui_create_top_menu_line ( void )

/* ----------------------------------------------------------------------
 *
 * This event is called whenever something changes in our graph area, i.e.
 * in case something else obscure the drawing area or in case ANYTHING
 * changes in that window.  
 *
 * This thing is now that with a Gtk drawing area we are completely 
 * responsible for the content of the drawing window, i.e. we must redraw
 * the window after 'anything' has happend to the window.  And this 
 * is the event handler that is supposed to do this.
 *
 * So here we draw our in-memory pixmap back to the actually visible
 * graphics screen, much like 'SDL Flip' in the SDL context, just that
 * we'd have full control there anyway.
 *
 * ---------------------------------------------------------------------- */
static gint
expose_event (GtkWidget *widget, GdkEventExpose *event)
{
  gdk_draw_pixmap(widget->window,
                  widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                  surface,
                  event->area.x, event->area.y,
                  event->area.x, event->area.y,
                  event->area.width, event->area.height);

  return FALSE;
}; // static gint expose_event (GtkWidget *widget, GdkEventExpose *event)

/* ----------------------------------------------------------------------
 * This event handler is called, whenever the 'configuration' of the 
 * drawing area for the 'graph' changes, i.e. when the window is resized
 * or things like that.
 *
 * When this happens, we have to organize a new (differently sized)
 * background in-memory graphics area, so it fits to the new window 
 * configuration again.  So we 'free' the old area and allocate a new
 * one.
 * And then of course, we draw the white background color into it, to
 * make if fresh and clean and good as new again.
 * ---------------------------------------------------------------------- */
static gint
configure_event (GtkWidget *widget, GdkEventConfigure *event)
{
  if (surface)
    gdk_pixmap_unref(surface);

  surface = gdk_pixmap_new(widget->window,
                          widget->allocation.width,
                          widget->allocation.height,
                          -1);
  gdk_draw_rectangle (surface,
                      widget->style->white_gc,
                      TRUE,
                      0, 0,
                      widget->allocation.width,
                      widget->allocation.height);

  gui_redraw_graph_completely ( );

  return TRUE;
}; // static gint configure_event (GtkWidget *widget, GdkEventConfigure *event)

/* ----------------------------------------------------------------------
 * This very low-level event handler is invoked whenever a mouse button
 * press occurs within the drawing area for our 'graph'.
 *
 * Currently it does not do much except for drawing some rectangle at
 * the current cursor location.  This will later be exchanged for some
 * more suitable dialog editing action.
 * ---------------------------------------------------------------------- */
// Mouse-button pressed on Drawing Area
static gint 
button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  int Pos_x, Pos_y;

#define MOUSE_BUTTON_PRESS_DEBUG 1

  //--------------------
  // We get the mouse position where the mouse click occured...
  //
  Pos_x = event -> x ;
  Pos_y = event -> y ;

  //--------------------
  // Now let's see which button it was:  1 is left, 2 is middle and 3 is right 
  // mouse button...
  //
  switch ( event->button )
    {
    case 1:  
      DebugPrintf ( MOUSE_BUTTON_PRESS_DEBUG , "\nButton 1 WAS PRESSED!!" );
      
      currently_mouse_grabbed_option = item_under_position ( Pos_x , Pos_y ) ;

      gui_redraw_graph_completely (  );

      break;
    case 3:  
      DebugPrintf ( MOUSE_BUTTON_PRESS_DEBUG , "\nButton 3 WAS PRESSED!!" );

      gui_edit_item_index_interface ( item_under_position ( Pos_x , Pos_y ) ) ;

      gui_redraw_graph_completely (  );

      break;
    case 2:  
      DebugPrintf ( MOUSE_BUTTON_PRESS_DEBUG , "\nButton 2 WAS PRESSED!!" );

      gui_react_to_right_mousebutton ( item_under_position ( Pos_x , Pos_y ) ) ;

      break;
    default:
      DebugPrintf ( MOUSE_BUTTON_PRESS_DEBUG , "\nERROR!  UNRECOGNIZED MOUSE BUTTON WAS PRESSED!!" );
      break;
    }
  return TRUE;
}; // static gint button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)

/* ----------------------------------------------------------------------
 * This event handler is called whenever the mouse moves (withing the
 * drawing area we use for our 'graph').  But according to the docu, NOT
 * EVERY TIME the mouse moves, this event is generated.  I'm not sure I've
 * understood this whole mechansism by now already.
 * ---------------------------------------------------------------------- */
static gint
motion_notify_event (GtkWidget *widget, GdkEventMotion *event)
{
  int x, y;
  GdkModifierType state;

  if (event->is_hint)
    gdk_window_get_pointer (event->window, &x, &y, &state);
  else
    {
      x = event->x;
      y = event->y;
      state = event->state;
    }
    
  if (state & GDK_BUTTON1_MASK && surface != NULL)
    {

      DebugPrintf ( 1 , "\nSOME 'DRAGGING' MOTION DETECTED!" );

      if ( currently_mouse_grabbed_option != (-1) )
	{
	  ItemMap [ currently_mouse_grabbed_option ] . position_x = event -> x ;
	  ItemMap [ currently_mouse_grabbed_option ] . position_y = event -> y ;

	  gui_redraw_graph_completely (  );

	}
    }
  else
    {
      gui_show_tooltip ( (int) event->x , (int) event->y );
    }
  
  return TRUE;
}

/* ----------------------------------------------------------------------
 * This function should add empty (white) space in the drawing area.
 * ---------------------------------------------------------------------- */
void 
gui_clear_graph_window ( void )
{
  GdkRectangle t;

  // delete the old surface
  if (surface) gdk_pixmap_unref (surface);

  // surface = gdk_pixmap_new( widget ->window, widget ->allocation.width, widget ->allocation.height, -1 );
  surface = gdk_pixmap_new( wnd ->window, wnd ->allocation.width, wnd ->allocation.height, -1);
  gdk_draw_rectangle ( surface, wnd->style->white_gc, 
		       TRUE, 0, 0, graph->allocation.width, graph->allocation.height);
  
  t.x = 0;
  t.y = 0;
  t.width = graph->allocation.width;
  t.height = graph->allocation.height;
  gtk_widget_draw (graph, &t);      
  
}; // void gui_clear_graph_window ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
gui_create_authors_notes_part ( GtkWidget* paned )
{
  GtkWidget *scrolledwindow;
  GtkTooltips *tooltips = gtk_tooltips_new ();  

  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\ngui_create_authors_notes_part(...):  now creating scrollwindow....." );

  //--------------------
  // Now we create the right scrollwindow
  //
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow);
  gtk_object_set_data_full ( GTK_OBJECT ( scrolledwindow ) , "scrolledwindow[i]1", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow);
      
  // gtk_box_pack_start ( GTK_BOX ( option_hbox ), scrolledwindow, TRUE, TRUE, 0);

  gtk_paned_add2 ( GTK_PANED ( paned ) , scrolledwindow );

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\ngui_create_authors_notes_part(...):  now creating new 'text' for authors notes....." );

  //--------------------
  // Now we create the right box text for each reply sample file name entry
  //
  authors_notes_entry = gtk_text_new (NULL, NULL);
  gtk_widget_ref (authors_notes_entry);
  gtk_object_set_data_full ( GTK_OBJECT ( authors_notes_entry ), "authors_notes_entry", authors_notes_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (authors_notes_entry);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), authors_notes_entry);
  GTK_WIDGET_SET_FLAGS (authors_notes_entry, GTK_CAN_DEFAULT);
  gtk_tooltips_set_tip (tooltips, authors_notes_entry, "This text field contains the file name of a voice sample file, that will be issued by the game engine in sync with the reply subtitle text on the in the left text box.", NULL);
  gtk_text_set_editable (GTK_TEXT (authors_notes_entry), TRUE);
  gtk_text_set_word_wrap (GTK_TEXT (authors_notes_entry), TRUE);
  
  gtk_text_insert (GTK_TEXT (authors_notes_entry), authors_notes_entry->style->font,
		   &authors_notes_entry->style->black, &authors_notes_entry->style->white,
		   authors_notes , -1);

}; // void gui_create_authors_notes_part ( GtkWidget* paned )

/* ----------------------------------------------------------------------
 * After the top status bar has been initialized, it's time to add the
 * large window below it.  This will be done here.
 * ---------------------------------------------------------------------- */
void
gui_create_graph_window ( GtkWidget* paned )
{
  // initialize members to sane values
  surface = NULL;
    
  //--------------------
  // We set a flag to show, that the graph has been initialized
  // now and therefore drawing may be done...
  //
  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\ngui_create_graph_window:  now starting 'draw_area_new()'." );

  // create drawing area for the graph
  graph = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (graph), 200, 450);

  gtk_paned_add1 (GTK_PANED (paned), graph);

  gtk_widget_show (graph);
  gtk_widget_grab_focus (graph);
    
  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\ngui_create_graph_window:  now setting events..." );

  //--------------------
  // This function call should make GTK raise events when some low-level
  // events occur.  Otherwise these events whould not cause any implication.
  // But we want to handle those events, and we do it via the event handlers
  // set up above.
  //
  gtk_widget_set_events ( graph , GDK_EXPOSURE_MASK
			  | GDK_LEAVE_NOTIFY_MASK
			  | GDK_BUTTON_PRESS_MASK
			  | GDK_BUTTON_RELEASE_MASK 
			  | GDK_POINTER_MOTION_MASK
			  | GDK_KEY_PRESS
			  | GDK_POINTER_MOTION_HINT_MASK);

  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\ngui_create_graph_window:  now 'realizing' graph widget..." );

  //--------------------
  // WARNING!  
  // The graph widget must be realized before any 
  // color arrangement can succeed!!
  //
  gtk_widget_realize ( graph );

  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\ngui_create_graph_window:  now going into the clear graph thing..." );

  gui_clear_graph_window ( );

  //--------------------
  // Now that the main graph window has been drawn, we can as well glue our
  // event callbacks to it.
  // 
  gtk_signal_connect (GTK_OBJECT ( graph ), "expose_event", (GtkSignalFunc) expose_event, NULL);
  gtk_signal_connect (GTK_OBJECT( graph ),"configure_event", (GtkSignalFunc) configure_event, NULL);
  gtk_signal_connect (GTK_OBJECT ( graph ), "motion_notify_event", (GtkSignalFunc) motion_notify_event, NULL);
  gtk_signal_connect (GTK_OBJECT ( graph ), "button_press_event", (GtkSignalFunc) button_press_event, NULL);
  gtk_signal_connect (GTK_OBJECT (graph), "button_release_event", (GtkSignalFunc) button_release_event, NULL );
  gtk_signal_connect (GTK_OBJECT ( wnd ), "key_press_event", (GtkSignalFunc) key_press_event, NULL );
  // gtk_signal_connect (GTK_OBJECT (GuiDlgedit::window->getWindow ()), "key_press_event", (GtkSignalFunc) key_press_notify_event, this);

  // register our event callbacks
  // gtk_signal_connect (GTK_OBJECT (graph), "expose_event", (GtkSignalFunc) expose_event, this);
  // gtk_signal_connect (GTK_OBJECT (graph), "configure_event", (GtkSignalFunc) configure_event, this);
  // gtk_signal_connect (GTK_OBJECT (graph), "button_press_event", (GtkSignalFunc) button_press_event, this);
  // gtk_signal_connect (GTK_OBJECT (graph), "button_release_event", (GtkSignalFunc) button_release_event, this);
  // gtk_signal_connect (GTK_OBJECT (graph), "motion_notify_event", (GtkSignalFunc) motion_notify_event, this);
  // gtk_signal_connect (GTK_OBJECT (GuiDlgedit::window->getWindow ()), "key_press_event", (GtkSignalFunc) key_press_notify_event, this);
    
  // gtk_widget_set_events (graph, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
  // GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_KEY_PRESS_MASK);

  // gtk_widget_set_events (graph, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
  // GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_KEY_PRESS_MASK);

  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\ngui_create_graph_window:  starting color arrangements..." );

  //--------------------
  // Now that the graph widget has been realized, we
  // can define some colors for later use in the arrows...
  //
  gui_set_up_color_arrangements ( graph ) ;

  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\ngui_create_graph_window:  creation of graph now finished." );

}; // void gui_create_graph_window ( GtkWidget* paned )


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
clear_item_archetype_array ( void )
{
  //--------------------
  // We insert some 'blank' memory here, for now.
  //
  // ItemMap = (itemspec*) MyMalloc ( sizeof ( itemspec ) * ( 301 ) );

  //--------------------
  // This means some memory leak, but in these small amount, who cares?
  //
  ItemMap = NULL ;
  Number_Of_Item_Types = 0 ;

}; // void clear_item_archetype_array ( void )

/* ----------------------------------------------------------------------
 * This is the main function of our dialog editor.  But this time it does
 * not contain some 'main game loop' like in FreedroidRPG or Freedroid,
 * cause this 'main loop' is done within the GTK main function anyway.
 * ---------------------------------------------------------------------- */
int
main( int argc, char *argv[] )
{
  
  clear_item_archetype_array();

  //--------------------
  // This function call should filter out all the given parameters
  // to the dialog editor, that aren't intended for the dialog editor
  // anyway, but rather for the GTK library.  After this, we can process
  // the rest of the given arguments, without having to fear that some
  // GTK parameters are still in there.
  //
  gtk_init (&argc, &argv);

  //--------------------
  // At first we need to create the main window of our dialog editor.
  // Within this window, almost everything of the editor will take place...
  //
  gui_create_main_window ();

  //--------------------
  // Now that we have our main window, we can add the top menu line
  // with all the most important menu entries...
  //
  gui_create_top_menu_line ();

  // Window Menu
  // windowMenu = gtk_menu_new ();

  // Attach Window Menu
  // menuitem = gtk_menu_item_new_with_label ("Windows");
  // gtk_widget_show (menuitem);
  // gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), windowMenu);
  // gtk_menu_bar_append (GTK_MENU_BAR (menu), menuitem);
  // gtk_widget_show (menu);

  vpaned = gtk_vpaned_new ();
  gtk_box_pack_start (GTK_BOX (vbox), vpaned, TRUE, TRUE, 2);
  gtk_paned_set_handle_size (GTK_PANED (vpaned), 5);
  gtk_paned_set_gutter_size (GTK_PANED (vpaned), 7);
  gtk_widget_show (vpaned);

  // Accelerators
  // gtk_window_add_accel_group (GTK_WINDOW (wnd), accel_group);

  //--------------------
  // Now 
  gui_create_graph_window ( vpaned );

  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\nmain:  now starting to create 'authors notes' part...." );
  
  gui_create_authors_notes_part ( vpaned );

  //--------------------
  // Not it might be a good time to add the status bar on the very bottom of the
  // window.
  //
  gui_create_bottom_line( );

  //--------------------
  // Now that the main window has been assembled completely, we can start to
  // show it and all it's content to the user.
  //
  gtk_widget_show_all (wnd);

  //--------------------
  // At this point all should be set up.  The editing machine is ready to be
  // used.  All we need to do is start the gtk main loop and let everything
  // else spring forth from there.
  //
  gtk_main ();

  return ( 0 );

}; // int main (...)


#undef DIALOG_EDITOR
