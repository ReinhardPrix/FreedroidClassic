/* 
 *   Copyright (c) 2003 2004 Johannes Prix
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
 * This file contains almost all the code of the FreedroidRPG dialog 
 * editor, a GTK+ based program to make setting up dialogs for characters
 * in freedroidRPG an easier task.
 * ---------------------------------------------------------------------- */

#define DIALOG_EDITOR

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include "system.h"
#include "defs.h"
#include "struct.h"

#define MAX_TEXT_ENTRIES_VISIBLE 7
#define WINDOW_WARNINGS_DEBUG 2

int Number_Of_Item_Types = 0 ;
itemspec* ItemMap;

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
GtkWidget *extra_parameter_entry [ MAX_EXTRAS_PER_OPTION ] ;

GtkWidget *on_goto_selection ;
GtkWidget *on_goto_selection_menu ;
GtkWidget *on_goto_parameter_entry ;
GtkObject *adj1, *adj2;
// GtkAdjustment *adj1, *adj2;

GtkWidget *option_text_entry;
GtkWidget *option_sample_entry;
GtkWidget *reply_subtitle_entry[ MAX_TEXT_ENTRIES_VISIBLE ]; 
GtkWidget *reply_sample_file_name_entry[ MAX_TEXT_ENTRIES_VISIBLE ]; 
GtkWidget *authors_notes_entry;
GtkWidget *disable_button;

GtkWidget *loop;
GtkWidget *always_prior_to_startup;
GtkWidget *cond_edit;

//--------------------
// This is for marking options so that new connections between
// to options can be established.
//
static int currently_marked_dialog_option = (-1);

//--------------------
// This is for the 'meta-tooltips' that pop up if the mouse ever gets
// over one of the dialog option boxes...
//
static int DialogOptionCovered = (-1);
static GtkWidget *popup_meta_tool_tip = NULL ;

// GtkWidget *hpaned;
GtkWidget *vpaned;
GdkPixmap *icon;
GdkBitmap *mask;
GtkAccelGroup *accel_group;

//--------------------
// Here come some variables from the 'gui_graph' module...
//
GtkWidget *graph = NULL + 1 ;       // Drawing Area
GdkPixmap *surface = NULL + 1 ;     // Drawing surface

//--------------------
// This we need for the file open/load/save dialogs...
//
GtkWidget *filew;

int currently_mouse_grabbed_option = (-1) ;
char LastUsedFileName[10000] = "UNDEFINED_FILENAME.dialog" ;

static int graph_has_been_created = FALSE ;

typedef struct
{
  char* extra_identifier;
  int do_extra_menu_index;
  char parameter_termination_char;
} 
known_do_extra, *Known_do_extra;

#define ALL_KNOWN_EXTRAS 31

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
    { "ExecuteSubdialog:", 24 , '\0' } ,
    { "EverybodyBecomesHostile",  25 , '\0' } ,
    { "MakeTuxTownGuardMember",  26 , '\0' } ,
    { "IncreaseHackingSkill", 27 , '\0' } ,
    { "ForceBotRespawnOnLevel:" , 28 , '\0' } , 
    { "CompletelyHealTux",  29 , '\0' } , 
    { "OpenQuestDiaryEntry:" , 30 , ':' } , 
  };

typedef struct
{
  char* on_goto_identifier;
  int on_goto_menu_index;
  char parameter_termination_char;
} 
known_on_goto_condition, *Known_on_goto_condition;

#define ALL_KNOWN_ON_GOTO_CONDITIONS 9

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
    { "OldTownMissionScoreAtLeast:" , 8 , '\0' } , 
  };

#include "DialogEditor.h"

// #include "../src/file_utilities.c"

// #include "../src/text_public.c"


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
  
  // the "Edit Dialogue Node" window
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
  gtk_widget_grab_focus (reply_subtitle_entry[0]);
  gtk_widget_grab_default (reply_subtitle_entry[0]);
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
dialog_option_is_looping ( int dialog_option_index )
{
  int i ;

  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      if ( ( ChatRoster [ dialog_option_index ] . change_option_nr [ i ] == dialog_option_index ) &&
	   ( ChatRoster [ dialog_option_index ] . change_option_to_value [ i ] != 0 ) )
	{
	  return ( TRUE ) ;
	}
    }

  return ( FALSE );

}; // int dialog_option_is_looping ( int dialog_option_index )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
void
dialog_option_set_loop_status ( int dialog_option_index , int we_shall_loop )
{
  int i ;
  int we_re_done = FALSE ;

  //--------------------
  // At first we see if maybe we can set the loop status by just 
  // modifying an existing loop entry (or several of them)
  //
  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      if ( ChatRoster [ dialog_option_index ] . change_option_nr [ i ] == dialog_option_index )
	{
	  if ( ! we_re_done )
	    {
	      ChatRoster [ dialog_option_index ] . change_option_to_value [ i ] = we_shall_loop ; 
	      we_re_done = TRUE ;
	    }
	  else
	    {
	      //--------------------
	      // We do this to prevent double self-references...
	      //
	      ChatRoster [ dialog_option_index ] . change_option_to_value [ i ] = -1 ; 
	      ChatRoster [ dialog_option_index ] . change_option_nr [ i ] = -1 ; 
	    }
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
      if ( ChatRoster [ dialog_option_index ] . change_option_nr [ i ] == (-1) )
	{
	  ChatRoster [ dialog_option_index ] . change_option_to_value [ i ] = we_shall_loop ; 
	  ChatRoster [ dialog_option_index ] . change_option_nr [ i ] = dialog_option_index ;
	  return ;
	}
    }

}; // int dialog_option_is_looping ( int dialog_option_index )


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
edit_interface_ok_button_pressed ( GtkWidget *w, int *dialog_option_pointer )
{
  int dialog_option;
  int i;
  int extra_index;
  int on_goto_index;
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

  dialog_option = * dialog_option_pointer ;
  DebugPrintf ( 1 , "\nWriting data back into option Nr. %d." , dialog_option );

  //--------------------
  // At first we write the options text back into the option variable
  //
  ChatRoster [ dialog_option ] . option_text = 
    gtk_editable_get_chars( GTK_EDITABLE ( option_text_entry ) , 0, -1 );

  //--------------------
  // At first we write the options sample file name back into the option variable
  //
  ChatRoster [ dialog_option ] . option_sample_file_name = 
    gtk_editable_get_chars( GTK_EDITABLE ( option_sample_entry ) , 0, -1 );

  //--------------------
  // Now it's time to read the reply subtitles back into their proper
  // array structures...
  //
  for ( i = 0 ; i < MAX_TEXT_ENTRIES_VISIBLE ; i ++ )
    {
      ChatRoster [ dialog_option ] . reply_subtitle_list [ i ] = 
	gtk_editable_get_chars( GTK_EDITABLE ( reply_subtitle_entry [ i ]  ) , 0, -1 );
    }

  //--------------------
  // Now it's time to read the reply sample file names back into their proper
  // array structures...
  //
  for ( i = 0 ; i < MAX_TEXT_ENTRIES_VISIBLE ; i ++ )
    {
      ChatRoster [ dialog_option ] . reply_sample_list [ i ] = 
	gtk_editable_get_chars( GTK_EDITABLE ( reply_sample_file_name_entry [ i ]  ) , 0, -1 );
    }

  //--------------------
  // Now we can start to write the current extra configuration back into
  // the proper data structures.
  //
  for ( i = 0 ; i < MAX_EXTRAS_PER_OPTION ; i ++ ) 
    {
      //--------------------
      // First we find out the menu option, then we extract the user data
      // pointer, that was built into it upon menu assembling.  This should
      // give us a clue on what extra option index has been selected.
      //
      m = (GtkMenu *) gtk_option_menu_get_menu ( GTK_OPTION_MENU ( do_something_extra_selection [ i ] ) );
      extra_item = (GtkMenuItem *) gtk_menu_get_active ( m ) ;
      extra_index = * ( (int*) gtk_object_get_user_data (GTK_OBJECT ( extra_item )) ) ; 
      DebugPrintf ( 1 , "\n Final index, that was extracted from the currently selected extra menu option: %d.",
		    extra_index );

      //--------------------
      // So now we can enter the apropriate identifier into a new string, that has
      // to be generously allocated, again not paying attention to eventual memory
      // leak....
      //
      // But the text may only be transfered in case it's not the 'UNUSED EXTRA' text,
      // that has no place in the real FreedroidRPG, where empty string is used to 
      // indicate unused extra entries.
      //
      ChatRoster [ dialog_option ] . extra_list [ i ] = malloc ( 10000 );
      if ( extra_index != 0 )
	{
	  strcpy ( ChatRoster [ dialog_option ] . extra_list [ i ] , extra_map [ extra_index ] . extra_identifier ) ;
	  //--------------------
	  // To this string, we can now concatenate the given parameter text.
	  //
	  strcat ( ChatRoster [ dialog_option ] . extra_list [ i ] , 
		   gtk_editable_get_chars( GTK_EDITABLE ( extra_parameter_entry [ i ]  ) , 0, -1 ) ) ;
	  //--------------------
	  // Maybe the parameter is supposed to be terminated with a special character.
	  // In this case, the character should be appended now...
	  //
	  ChatRoster [ dialog_option ] . extra_list [ i ] =
	    add_termination_character_if_nescessary ( ChatRoster [ dialog_option ] . extra_list [ i ] , 
						      extra_map [ extra_index ] . parameter_termination_char );
	}
      else
	strcpy ( ChatRoster [ dialog_option ] . extra_list [ i ] , "" ) ;
    }

  //--------------------
  // After this we can now start to enforce the new 'loop' status.
  //
  dialog_option_set_loop_status ( dialog_option , gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( loop ) ) );

  //--------------------
  // Now we enforce the always on startup flag..
  //
  ChatRoster [ dialog_option ] . always_execute_this_option_prior_to_dialog_start =
    gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( always_prior_to_startup ) ) ;

  //--------------------
  // Now it's time to squeeze out the information we have in the on-goto-condition
  // and enter the currently set values into the proper structures...
  //
  // First we find out the menu option, then we extract the user data
  // pointer, that was built into it upon menu assembling.  This should
  // give us a clue on what on-goto condition index has been selected.
  //
  m = (GtkMenu *) gtk_option_menu_get_menu ( GTK_OPTION_MENU ( on_goto_selection ) ) ;
  extra_item = (GtkMenuItem *) gtk_menu_get_active ( m ) ;
  on_goto_index = * ( (int*) gtk_object_get_user_data ( GTK_OBJECT ( extra_item )) ) ; 
  DebugPrintf ( 1 , "\n Final index, that was extracted from the currently selected on-goto condition: %d.",
		on_goto_index );

  //--------------------
  // So now we can enter the apropriate identifier into a new string, that has
  // to be generously allocated, again not paying attention to eventual memory
  // leak....
  //
  // But the text may only be transfered in case it's not the 'UNUSED EXTRA' text,
  // that has no place in the real FreedroidRPG, where empty string is used to 
  // indicate unused extra entries.
  //
  ChatRoster [ dialog_option ] . on_goto_condition = malloc ( 10000 );
  if ( on_goto_index != 0 )
    {
      strcpy ( ChatRoster [ dialog_option ] . on_goto_condition , on_goto_map [ on_goto_index ] . on_goto_identifier ) ;
      //--------------------
      // To this string, we can now concatenate the given parameter text.
      //
      strcat ( ChatRoster [ dialog_option ] . on_goto_condition , 
	       gtk_editable_get_chars( GTK_EDITABLE ( on_goto_parameter_entry ) , 0, -1 ) ) ;
      //--------------------
      // Maybe the parameter is supposed to be terminated with a special character.
      // In this case, the character should be appended now...
      //
      ChatRoster [ dialog_option ] . on_goto_condition = 
	add_termination_character_if_nescessary ( ChatRoster [ dialog_option ] . on_goto_condition ,
						  on_goto_map [ on_goto_index ] . parameter_termination_char );
    }
  else
    {
      strcpy ( ChatRoster [ dialog_option ] . on_goto_condition , "" ) ;
    }

  //--------------------
  // Now after that, it's time to extract the on-goto-jump targets from the
  // scales on the third page of the dialog option editor popup window...
  //
  ChatRoster [ dialog_option ] . on_goto_first_target = (int) GTK_ADJUSTMENT(adj1)->value ;
  ChatRoster [ dialog_option ] . on_goto_second_target = (int) GTK_ADJUSTMENT(adj2)->value ;
  
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
edit_interface_disable_button_pressed ( GtkWidget *w, int *dialog_option_pointer )
{
  int dialog_option;

  //--------------------
  // At first we say, that we're here...
  //
  DebugPrintf ( 1 , "\nedit_interface_disable_button_pressed: Function call confirmed. " );

  dialog_option = * dialog_option_pointer ;
  DebugPrintf ( 1 , "\nDisabling data in option Nr. %d." , dialog_option );

  //--------------------
  // Now we set the 'disable' text in the option subtitle entry...
  //
  gtk_text_set_point ( GTK_TEXT ( option_text_entry ) , 0 );
  gtk_text_forward_delete ( GTK_TEXT ( option_text_entry ) , gtk_text_get_length ( GTK_TEXT ( option_text_entry ) ) ) ;
  gtk_text_insert ( GTK_TEXT ( option_text_entry ) , option_text_entry->style->font,
		    &option_text_entry->style->black,
		    &option_text_entry->style->white,
		    "THIS WILL NOT EVER BE VISIBLE" ,
		    -1 );
  
  //--------------------
  // Now we set the 'disable' text in the option sample file name entry...
  //
  gtk_text_set_point ( GTK_TEXT ( option_sample_entry ) , 0 );
  gtk_text_forward_delete ( GTK_TEXT ( option_sample_entry ) , gtk_text_get_length ( GTK_TEXT ( option_sample_entry ) ) );
  gtk_text_insert ( GTK_TEXT ( option_sample_entry ) , option_sample_entry->style->font,
		    &option_sample_entry->style->black,
		    &option_sample_entry->style->white,
		    "NO_SAMPLE_HERE_AND_DONT_WAIT_EITHER" ,
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
gui_react_to_right_mousebutton ( int dialog_option_index )
{
  int i;

  DebugPrintf ( 1 , "\ngui_react_to_right_mousebutton (...):  real function call confirmed." );

  //--------------------
  // The click was right in the void?  --  no reaction, no comment.
  //
  if ( dialog_option_index < 0 ) 
    {
      DebugPrintf ( 1 , "\nNothing needs to be done, cause click was into the void...." );
      return ;
    }

  //--------------------
  // The click was on an option and no other option currently marked? -- 
  // then we interpret this as marking an option.
  //
  if ( currently_marked_dialog_option == (-1) )
    {
      currently_marked_dialog_option = dialog_option_index ;
      DebugPrintf ( 1 , "\nMarked dialog option is now: %d." , currently_marked_dialog_option );
      gui_redraw_graph_completely (  );
      return;
    }

  //--------------------
  // The click occured on the very same option that is already marked? --
  // then we unmark this option again.
  //
  if ( currently_marked_dialog_option == dialog_option_index )
    {
      currently_marked_dialog_option = (-1) ;
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
      if ( ChatRoster [ currently_marked_dialog_option ] . change_option_nr [ i ] == dialog_option_index )
	{

	  if ( ChatRoster [ currently_marked_dialog_option ] . change_option_to_value [ i ] == 0 )
	    {
	      ChatRoster [ currently_marked_dialog_option ] . change_option_to_value [ i ] = 1 ;
	      gui_redraw_graph_completely (  );
	      DebugPrintf ( 1 , "\nChanging target value to : ACTIVATE." );
	      return;
	    }
	  else
	    {
	      ChatRoster [ currently_marked_dialog_option ] . change_option_nr [ i ] = (-1) ;
	      ChatRoster [ currently_marked_dialog_option ] . change_option_to_value [ i ] = (-1) ;
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
      if ( ChatRoster [ currently_marked_dialog_option ] . change_option_nr [ i ]  == (-1) )
	{
	  ChatRoster [ currently_marked_dialog_option ] . change_option_nr [ i ] = dialog_option_index ;
	  ChatRoster [ currently_marked_dialog_option ] . change_option_to_value [ i ] = 0 ;
	  gui_redraw_graph_completely (  );
	  DebugPrintf ( 1 , "\nChanging: INTRODUCING NEW CONNECTION." );
	  return;
	}
    }
  
}; // void gui_react_to_right_mousebutton ( int dialog_option_index )


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
gui_edit_dialog_set_up_first_page ( GtkWidget* notebook1 , int dialog_option_index )
{
  GtkWidget *vbox2;
  GtkWidget *option_hbox;
  GtkWidget *scrolledwindow;
  GtkWidget *label;
  GtkWidget *frame1;
  GtkWidget *ebox;
  GtkTooltips *tooltips = gtk_tooltips_new ();
  int i;

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
  frame1 = gtk_frame_new ("Option Text and voice sample file name:");
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
  // Now we add the left of the two option boxes...
  //
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow);
  gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow4", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER ( option_hbox ), scrolledwindow);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 4);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  
  option_text_entry = gtk_text_new ( NULL , NULL );
  gtk_widget_ref (option_text_entry);
  gtk_object_set_data_full (GTK_OBJECT (window), "option_text_entered_here_entry", option_text_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (option_text_entry);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), option_text_entry);
  gtk_tooltips_set_tip (tooltips, option_text_entry, "This text field contains the text, that the Tux will have as his dialog option.", NULL);
  gtk_text_set_editable (GTK_TEXT (option_text_entry), TRUE);
  
  gtk_text_insert ( GTK_TEXT ( option_text_entry ), option_text_entry->style->font,
		    &option_text_entry->style->black, &option_text_entry->style->white,
		    ChatRoster [ dialog_option_index ] . option_text , -1);
  
  //--------------------
  // Now we add the right of the two option boxes...
  //
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow);
  gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow4", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow);
  gtk_container_add (GTK_CONTAINER ( option_hbox ), scrolledwindow);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 4);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  
  option_sample_entry = gtk_text_new ( NULL , NULL );
  gtk_widget_ref (option_sample_entry);
  gtk_object_set_data_full (GTK_OBJECT (window), "option_sample_entered_here_entry", option_sample_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (option_sample_entry);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), option_sample_entry);
  gtk_tooltips_set_tip (tooltips, option_sample_entry, "This text field contains the file name of a voice file, that will be issued by the game engine together with the subtitle on the left.", NULL);
  gtk_text_set_editable (GTK_TEXT (option_sample_entry), TRUE);
  
  gtk_text_insert ( GTK_TEXT ( option_sample_entry ), option_sample_entry->style->font,
		    &option_sample_entry->style->black, &option_sample_entry->style->white,
		    ChatRoster [ dialog_option_index ] . option_sample_file_name , -1);

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
  gtk_signal_connect (GTK_OBJECT (disable_button), "clicked", GTK_SIGNAL_FUNC ( edit_interface_disable_button_pressed ), &(dialog_option_index ) );

  //--------------------
  // Now we add all the dialog reply boxes...
  //
  for ( i = 0 ; i < MAX_TEXT_ENTRIES_VISIBLE ; i ++ )
    {

      //--------------------
      // Now the h-box for each reply-sample reply-subtitle combination...
      //
      option_hbox = gtk_hbox_new (FALSE, 0);
      gtk_widget_ref ( option_hbox );
      gtk_object_set_data_full ( GTK_OBJECT ( window ) , "option_hbox" , option_hbox , (GtkDestroyNotify) gtk_widget_unref );
      gtk_widget_show ( option_hbox );
      gtk_container_add ( GTK_CONTAINER ( vbox2 ), option_hbox );

      //--------------------
      // Now we crate the scrollbar for the left box
      //
      scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
      gtk_widget_ref (scrolledwindow);
      gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow[i]1", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
      gtk_widget_show (scrolledwindow);

      gtk_box_pack_start (GTK_BOX ( option_hbox ), scrolledwindow, TRUE, TRUE, 0);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

      //--------------------
      // At first we create the left box for each reply subtitle entry
      //
      reply_subtitle_entry[i] = gtk_text_new (NULL, NULL);
      gtk_widget_ref (reply_subtitle_entry[i]);
      gtk_object_set_data_full (GTK_OBJECT (window), "reply_subtitle_entry[i]", reply_subtitle_entry[i], (GtkDestroyNotify) gtk_widget_unref);
      gtk_widget_show (reply_subtitle_entry[i]);
      gtk_container_add (GTK_CONTAINER (scrolledwindow), reply_subtitle_entry[i]);
      GTK_WIDGET_SET_FLAGS (reply_subtitle_entry[i], GTK_CAN_DEFAULT);
      gtk_tooltips_set_tip (tooltips, reply_subtitle_entry[i], "This text field contains the response text, that the dialog partner will say to the Tux as a response to the option text in the very top text field.", NULL);
      gtk_text_set_editable (GTK_TEXT (reply_subtitle_entry[i]), TRUE);
      gtk_text_set_word_wrap (GTK_TEXT (reply_subtitle_entry[i]), TRUE);
      
      gtk_text_insert (GTK_TEXT (reply_subtitle_entry[i]), reply_subtitle_entry[i]->style->font,
		       &reply_subtitle_entry[i]->style->black, &reply_subtitle_entry[i]->style->white,
		       ChatRoster [ dialog_option_index ] . reply_subtitle_list [ i ] , -1);

      //--------------------
      // Now we create the right scrollwindow
      //
      scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
      gtk_widget_ref (scrolledwindow);
      gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow[i]1", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
      gtk_widget_show (scrolledwindow);

      gtk_box_pack_start (GTK_BOX ( option_hbox ), scrolledwindow, TRUE, TRUE, 0);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

      //--------------------
      // Now we create the right box text for each reply sample file name entry
      //
      reply_sample_file_name_entry[i] = gtk_text_new (NULL, NULL);
      gtk_widget_ref (reply_sample_file_name_entry[i]);
      gtk_object_set_data_full (GTK_OBJECT (window), "reply_sample_file_name_entry[i]", reply_sample_file_name_entry[i], (GtkDestroyNotify) gtk_widget_unref);
      gtk_widget_show (reply_sample_file_name_entry[i]);
      gtk_container_add (GTK_CONTAINER (scrolledwindow), reply_sample_file_name_entry[i]);
      GTK_WIDGET_SET_FLAGS (reply_sample_file_name_entry[i], GTK_CAN_DEFAULT);
      gtk_tooltips_set_tip (tooltips, reply_sample_file_name_entry[i], "This text field contains the file name of a voice sample file, that will be issued by the game engine in sync with the reply subtitle text on the in the left text box.", NULL);
      gtk_text_set_editable (GTK_TEXT (reply_sample_file_name_entry[i]), TRUE);
      gtk_text_set_word_wrap (GTK_TEXT (reply_sample_file_name_entry[i]), TRUE);
      
      gtk_text_insert (GTK_TEXT (reply_sample_file_name_entry[i]), reply_sample_file_name_entry[i]->style->font,
		       &reply_sample_file_name_entry[i]->style->black, &reply_sample_file_name_entry[i]->style->white,
		       ChatRoster [ dialog_option_index ] . reply_sample_list [ i ] , -1);
    }
  
  // The heading of this page
  label = gtk_label_new ("Text");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label);
  
  //--------------------
  // Now we add the loop checkbox to this first page of the edit window...
  // 
  ebox = gtk_event_box_new ();
  gtk_widget_ref (ebox);
  gtk_widget_show (ebox);
  gtk_object_set_data_full (GTK_OBJECT (window), "ebox", ebox, (GtkDestroyNotify) gtk_widget_unref);
  gtk_box_pack_end (GTK_BOX (vbox2), ebox, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, ebox, "Unless this is checked, the dialog option will becomes unavailable after being selected once by the player in the dialog.", NULL);
  
  loop = gtk_check_button_new_with_label ("Loop");
  gtk_widget_ref (loop);
  gtk_object_set_data_full (GTK_OBJECT (window), "loop", loop, (GtkDestroyNotify) gtk_widget_unref);
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( loop ), dialog_option_is_looping ( dialog_option_index ) );
  gtk_widget_show (loop);
  gtk_container_add (GTK_CONTAINER (ebox), loop);

  //--------------------
  // Now we add the always_on_start checkbox to this first page of the edit window...
  // 
  ebox = gtk_event_box_new ();
  gtk_widget_ref (ebox);
  gtk_widget_show (ebox);
  gtk_object_set_data_full (GTK_OBJECT (window), "ebox2", ebox, (GtkDestroyNotify) gtk_widget_unref);
  gtk_box_pack_end (GTK_BOX (vbox2), ebox, FALSE, FALSE, 0);
  gtk_tooltips_set_tip (tooltips, ebox, "Check this to cause this dialog option to automatically be processed whenever the Tux enters this dialog in the game.", NULL);
  
  always_prior_to_startup = gtk_check_button_new_with_label ("Always on dialog start");
  gtk_widget_ref (always_prior_to_startup);
  gtk_object_set_data_full (GTK_OBJECT (window), "always_prior_to_startup", always_prior_to_startup, (GtkDestroyNotify) gtk_widget_unref);
  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON ( always_prior_to_startup ), ChatRoster [ dialog_option_index ] . always_execute_this_option_prior_to_dialog_start ) ;
  gtk_widget_show ( always_prior_to_startup );
  gtk_container_add ( GTK_CONTAINER (ebox), always_prior_to_startup );



}; // void gui_edit_dialog_set_up_first_page ( GtkWidget* notebook1 , int dialog_option_index )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
gui_edit_dialog_set_up_second_page ( GtkWidget *notebook1 , int dialog_option_index )
{
  int j;
  int i;
  GtkWidget *vbox2;
  GtkWidget *glade_menuitem;
  GtkWidget *label;
  GtkWidget *hbox1;
  GtkTooltips *tooltips = gtk_tooltips_new ();
  GtkWidget *scrolledwindow;
  char our_term_char;

  // Page 2: Edit Extras
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox2", vbox2, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);
  
  // The heading of this page
  label = gtk_label_new ("Extra Actions");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label);
  
  for ( j = 0 ; j < MAX_EXTRAS_PER_OPTION ; j ++ )
    {

      //--------------------
      // First we make a new h-box here, cause we want to put one
      // extra selector next to it's parameter text entry, right?
      //
      hbox1 = gtk_hbox_new (FALSE, 10);
      gtk_widget_ref (hbox1);
      gtk_object_set_data_full (GTK_OBJECT (window), "hbox1", hbox1, (GtkDestroyNotify) gtk_widget_unref);
      gtk_widget_show (hbox1);
      gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, TRUE, 0);

      // the option menu with the available NPC's
      do_something_extra_selection[j] = gtk_option_menu_new ();
      gtk_widget_ref (do_something_extra_selection[j]);
      gtk_object_set_data_full (GTK_OBJECT (window), "do_something_extra_selection[j]", do_something_extra_selection[j], (GtkDestroyNotify) gtk_widget_unref);
      gtk_widget_show (do_something_extra_selection[j]);

      //--------------------
      // Now we insert one whole selector into the window
      //
      gtk_box_pack_start ( GTK_BOX ( hbox1 ), do_something_extra_selection[j], TRUE, TRUE, 0);
      // gtk_tooltips_set_tip (tooltips, do_something_extra_selection[j], "Select the speaker", NULL);
      do_something_extra_selection_menu [ j ] = gtk_menu_new ();

      //--------------------
      // Now we must fill in all known extras, so that the user of the dialog editor
      // can select really any chat extra command, that he/she feels like adding...
      //
      for ( i = 0 ; i < ALL_KNOWN_EXTRAS ; i ++ )
	{
	  glade_menuitem = gtk_menu_item_new_with_label ( extra_map [ i ] . extra_identifier );
	  gtk_object_set_user_data ( GTK_OBJECT ( glade_menuitem ) , &( extra_map [ i ] . do_extra_menu_index ) );
	  gtk_widget_show (glade_menuitem);
	  gtk_tooltips_set_tip (tooltips, glade_menuitem , extra_map [ i ] . extra_identifier , NULL);
	  gtk_menu_append (GTK_MENU ( do_something_extra_selection_menu [ j ] ), glade_menuitem);
	}
      gtk_option_menu_set_menu ( GTK_OPTION_MENU ( do_something_extra_selection[j] ), do_something_extra_selection_menu[j]);

      //--------------------
      // Now we set the correct old value, so that the user can really modify
      // the existing characters and is not forced to re-enter all the old values
      // again and again...
      //
      // This is also a good point to find out the right termination character
      // for the possible parameter to this extra...
      //
      gtk_option_menu_set_history ( GTK_OPTION_MENU ( do_something_extra_selection [ j ] ) , 
				    make_index_from_real_extra ( ChatRoster [ dialog_option_index ] . extra_list [ j ] ) );
      our_term_char = extra_map [ make_index_from_real_extra ( ChatRoster [ dialog_option_index ] . extra_list [ j ] ) ] . parameter_termination_char ;

      //--------------------
      // Now at this point it would make sense to create a new text entry
      // and then pack this text entry into the hbox1 as well, right?
      //
      scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
      gtk_widget_ref (scrolledwindow);
      gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow4", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
      gtk_widget_show (scrolledwindow);
      // gtk_container_add (GTK_CONTAINER (frame1), scrolledwindow);
      gtk_container_add (GTK_CONTAINER ( hbox1 ), scrolledwindow);
      gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 4);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

      extra_parameter_entry[j] = gtk_text_new ( NULL , NULL );
      gtk_widget_ref (extra_parameter_entry[j]);
      gtk_object_set_data_full (GTK_OBJECT (window), "option_text_entered_here_entry", extra_parameter_entry[j], (GtkDestroyNotify) gtk_widget_unref);
      gtk_widget_show (extra_parameter_entry[j]);
      gtk_container_add (GTK_CONTAINER (scrolledwindow), extra_parameter_entry[j]);
      gtk_tooltips_set_tip (tooltips, extra_parameter_entry[j], "This text field contains the additional parameter to this 'DoSomethingExtra' command string.", NULL);
      gtk_text_set_editable (GTK_TEXT (extra_parameter_entry[j]), TRUE);
      
      //--------------------
      // Now that we have put together and added our text fields, where the
      // parameter for the extras will be stored, we can start to fill in the
      // correct (old) value of these parameters.  And we do that with respect
      // to the possible termination character, that might be required in there.
      //

      gtk_text_insert ( GTK_TEXT ( extra_parameter_entry[j] ), 
			extra_parameter_entry[j]->style->font,
			&extra_parameter_entry[j]->style->black, 
			&extra_parameter_entry[j]->style->white,

			remove_termination_character_if_nescessary ( 
								    make_parameter_from_real_extra ( ChatRoster [ dialog_option_index ] . extra_list [ j ] ) , our_term_char )

			, -1);

    }

}; // void gui_edit_dialog_set_up_second_page ( GtkWidget *notebook1 , int dialog_option_index )


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
gui_edit_dialog_set_up_third_page ( GtkWidget *notebook1 , int dialog_option_index )
{
  int j;
  int i;
  GtkWidget *vbox2;
  GtkWidget *glade_menuitem;
  GtkWidget *label;
  GtkWidget *hbox1;
  GtkTooltips *tooltips = gtk_tooltips_new ();
  GtkWidget *scrolledwindow;
  GtkWidget *hscale;
  char our_term_char;

  // Page 2: Edit On_Gotos
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (window), "vbox3", vbox2, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (notebook1), vbox2);
  gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);
  
  // The heading of this page
  label = gtk_label_new ("On Goto Condition");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label);
  
  j=0;

  //--------------------
  // First we make a new h-box here, cause we want to put one
  // on_goto selector next to it's parameter text entry, right?
  //
  hbox1 = gtk_hbox_new (FALSE, 10);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbox1", hbox1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, TRUE, 0);
  
  // the option menu with the available NPC's
  on_goto_selection = gtk_option_menu_new ();
  gtk_widget_ref ( on_goto_selection );
  gtk_object_set_data_full (GTK_OBJECT (window), "on_goto_selection", on_goto_selection , (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (on_goto_selection );
  
  //--------------------
  // Now we insert one whole selector into the window
  //
  // gtk_box_pack_start ( GTK_BOX ( vbox2 ), on_goto_selection, TRUE, TRUE, 0);
  gtk_box_pack_start ( GTK_BOX ( hbox1 ), on_goto_selection, TRUE, TRUE, 0);
  
  // gtk_tooltips_set_tip (tooltips, on_goto_selection, "Select the speaker", NULL);
  on_goto_selection_menu = gtk_menu_new ();
  
  for ( i = 0 ; i < ALL_KNOWN_ON_GOTO_CONDITIONS ; i ++ )
    {
      glade_menuitem = gtk_menu_item_new_with_label ( on_goto_map [ i ] . on_goto_identifier );
      gtk_object_set_user_data ( GTK_OBJECT ( glade_menuitem ) , &( on_goto_map [ i ] . on_goto_menu_index ) );
      gtk_widget_show (glade_menuitem);
      gtk_tooltips_set_tip (tooltips, glade_menuitem , on_goto_map [ i ] . on_goto_identifier , NULL);
      gtk_menu_append (GTK_MENU ( on_goto_selection_menu ), glade_menuitem);
    }
  
  gtk_option_menu_set_menu (GTK_OPTION_MENU (on_goto_selection), on_goto_selection_menu );
  
  gtk_option_menu_set_history ( GTK_OPTION_MENU ( on_goto_selection ) , 
				make_index_from_real_on_goto ( ChatRoster [ dialog_option_index ] . on_goto_condition ) );
  
  our_term_char = on_goto_map [ make_index_from_real_on_goto ( ChatRoster [ dialog_option_index ] . on_goto_condition ) ] . parameter_termination_char ;

  //--------------------
  // Now at this point it would make sense to create a new text entry
  // and then pack this text entry into the hbox1 as well, right?
  //
  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow);
  gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow4", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow);
  // gtk_container_add (GTK_CONTAINER (frame1), scrolledwindow);
  gtk_container_add (GTK_CONTAINER ( hbox1 ), scrolledwindow);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 4);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  
  on_goto_parameter_entry = gtk_text_new ( NULL , NULL );
  gtk_widget_ref ( on_goto_parameter_entry );
  gtk_object_set_data_full (GTK_OBJECT (window), "option_text_entered_here_entry", on_goto_parameter_entry, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (on_goto_parameter_entry );
  gtk_container_add (GTK_CONTAINER ( scrolledwindow ), on_goto_parameter_entry );
  gtk_tooltips_set_tip (tooltips, on_goto_parameter_entry , "This text field contains the additional parameter to the condition string on the left.", NULL);
  gtk_text_set_editable (GTK_TEXT ( on_goto_parameter_entry ), TRUE);
  
  //--------------------
  // Now that the on-goto jump conditional text field has been established, it's time
  // to fill in the on-goto condition text there...
  //
  


  gtk_text_insert ( GTK_TEXT ( on_goto_parameter_entry ), on_goto_parameter_entry->style->font,
		    &on_goto_parameter_entry->style->black, &on_goto_parameter_entry->style->white,

		    remove_termination_character_if_nescessary ( 
								make_parameter_from_real_on_goto ( ChatRoster [ dialog_option_index ] . on_goto_condition ) ,
								our_term_char ) 

		    , -1);

  //--------------------
  // Now that the condition selector and on_goto_condition parameter thing
  // are done, we can start to add the entries for the real on-goto-condition,
  // i.e. where to go to in case of true value and where to go in case of false
  // value.
  //

  //--------------------
  // First we make a new h-box here, cause we want to put one
  // on_goto selector next to it's parameter text entry, right?
  //
  hbox1 = gtk_hbox_new (FALSE, 10);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbox Nr. waht", hbox1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, TRUE, 0);
  
  label = gtk_label_new ("On condition evaluates TRUE goto option Nr.:");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX ( hbox1 ), label , FALSE, TRUE, 0);

  /* value, lower, upper, step_increment, page_increment, page_size */
  /* Note that the page_size value only makes a difference for
   * scrollbar widgets, and the highest value you'll get is actually
   * (upper - page_size). */
  adj1 = gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0);
  
  /* Reuse the same adjustment */
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (adj1) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( hbox1 ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  //--------------------
  // Now the adjustment, i.e. the thing underlying the scale should be there.  
  // So it's about time to change the value of it to the proper value, if
  // appropriate...
  //
  if ( ChatRoster [ dialog_option_index ] . on_goto_first_target >= 0 ) 
    {
      gtk_adjustment_set_value ( GTK_ADJUSTMENT ( adj1 ) , (gfloat) ChatRoster [ dialog_option_index ] . on_goto_first_target );
    }

  //--------------------
  // Second we make a new h-box here, cause we want to put one
  // on_goto selector next to it's parameter text entry, right?
  //
  hbox1 = gtk_hbox_new (FALSE, 10);
  gtk_widget_ref (hbox1);
  gtk_object_set_data_full (GTK_OBJECT (window), "hbox Nr. waht", hbox1, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, TRUE, 0);
  
  label = gtk_label_new ("On condition evaluates FALSE goto option Nr.:");
  gtk_widget_ref (label);
  gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX ( hbox1 ), label , FALSE, TRUE, 0);

  /* value, lower, upper, step_increment, page_increment, page_size */
  /* Note that the page_size value only makes a difference for
   * scrollbar widgets, and the highest value you'll get is actually
   * (upper - page_size). */
  adj2 = gtk_adjustment_new (0.0, 0.0, 101.0, 0.1, 1.0, 1.0);
  
  /* Reuse the same adjustment */
  hscale = gtk_hscale_new ( GTK_ADJUSTMENT (adj2) );
  gtk_widget_set_usize ( GTK_WIDGET ( hscale ) , 200 , 30 );
  gtk_range_set_update_policy ( GTK_RANGE ( hscale ) , GTK_UPDATE_CONTINUOUS );
  gtk_scale_set_digits ( GTK_SCALE ( hscale ) , 0 );
  gtk_scale_set_value_pos ( GTK_SCALE ( hscale ) , GTK_POS_TOP);
  gtk_scale_set_draw_value ( GTK_SCALE ( hscale ) , TRUE);
  gtk_box_pack_start ( GTK_BOX ( hbox1 ) , hscale, TRUE, TRUE, 0);
  gtk_widget_show (hscale);

  //--------------------
  // Now the adjustment, i.e. the thing underlying the scale should be there.  
  // So it's about time to change the value of it to the proper value, if
  // appropriate...
  //
  if ( ChatRoster [ dialog_option_index ] . on_goto_second_target >= 0 ) 
    {
      gtk_adjustment_set_value ( GTK_ADJUSTMENT ( adj2 ) , (gfloat) ChatRoster [ dialog_option_index ] . on_goto_second_target );
    }

}; // void gui_edit_dialog_set_up_third_page ( GtkWidget *notebook1 , int dialog_option_index )

/* ----------------------------------------------------------------------
 * 
 * ---------------------------------------------------------------------- */
void
gui_edit_dialog_option_interface ( int dialog_option_index )
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
  DebugPrintf ( 1 , "\ngui_edit_dialog_option_interface ( int dialog_option_index ): function call confirmed. " );
  
  //--------------------
  // Maybe the click was into the void.  Then of course we do
  // not do anything here.
  //
  if ( dialog_option_index < 0 ) return;

  //--------------------
  // Now we destroy that awful little window, that might still
  // be cluttering part of the screen... how annoying...
  //
  gui_destroy_existing_tooltips (  );
  
  // the "Edit Dialogue Node" window
  window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (window), "window", window);
  gtk_widget_set_usize (window, 600, 520);
  gtk_window_set_title (GTK_WINDOW (window), "Edit Dialogue Node");
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
  

  gui_edit_dialog_set_up_first_page ( notebook1 , dialog_option_index );

  gui_edit_dialog_set_up_second_page ( notebook1 , dialog_option_index );

  gui_edit_dialog_set_up_third_page ( notebook1 , dialog_option_index );

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
  
  // cancel button
  cancel_button = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (cancel_button);
  gtk_object_set_data_full (GTK_OBJECT (window), "cancel_button", cancel_button, (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cancel_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), cancel_button);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);
  
  // the various input handlers
  gtk_signal_connect (GTK_OBJECT (notebook1), "switch_page", GTK_SIGNAL_FUNC (on_switch_page), window);
  // gtk_signal_connect (GTK_OBJECT (player_button), "clicked", GTK_SIGNAL_FUNC (on_radio_button_pressed), reply_subtitle_entry[0]);
  // gtk_signal_connect (GTK_OBJECT (npc_button), "clicked", GTK_SIGNAL_FUNC (on_radio_button_pressed), reply_subtitle_entry[0]);
  // gtk_signal_connect (GTK_OBJECT (narrator_button), "clicked", GTK_SIGNAL_FUNC (on_radio_button_pressed), reply_subtitle_entry[0]);
  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked", GTK_SIGNAL_FUNC ( edit_interface_ok_button_pressed ), &(dialog_option_index ) );
  gtk_signal_connect (GTK_OBJECT (cancel_button), "clicked", GTK_SIGNAL_FUNC ( edit_interface_cancel_button_pressed ), window );
  gtk_signal_connect (GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC ( edit_interface_cancel_button_pressed ), window );
  
  // give focus to the text entry
  gtk_widget_grab_focus (reply_subtitle_entry[0]);
  gtk_widget_grab_default (reply_subtitle_entry[0]);
  gtk_object_set_data (GTK_OBJECT (window), "tooltips", tooltips);
  
  //--------------------
  // Clear?
  //
  gtk_widget_show ( window );
  gtk_main();
  
}; // void gui_edit_dialog_option_interface ( void )

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
  gint x_offs = 0 ;
  gint y_offs = 0 ;

  //--------------------
  // At first we need to find out which dialog option really
  // is below the mouse cursor, if one at all...
  //
  // DialogOptionCovered = option_under_position ( x , y ) ;

  //--------------------
  // Maybe none at all, then we can destroy any existing
  // popup_meta_tool_tip and then just quit...
  //
  if ( option_under_position ( x , y ) == (-1) )
    {
      gui_destroy_existing_tooltips ();
      DialogOptionCovered = option_under_position ( x , y ) ;
      return ;
    }

  if ( option_under_position ( x , y ) != DialogOptionCovered )
    {
      gui_destroy_existing_tooltips ( );
      DialogOptionCovered = option_under_position ( x , y ) ;
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
      sprintf( popup_meta_tool_tip_text , "Option Nr: %d\n" , DialogOptionCovered );
      strcat( popup_meta_tool_tip_text , ChatRoster [ DialogOptionCovered ] . option_text );
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
    
      //--------------------
      // We need to respect the current position of the window when calculating
      // position for the tooltip placement...
      //
      // gdk_window_get_origin ( wnd , &x_offs , & y_offs );
      // gdk_window_get_origin ( graph , &x_offs , & y_offs );
      gdk_window_get_origin ( wnd->window, &x_offs , &y_offs );

      // calculate the position of the popup_meta_tool_tip
      x = ChatRoster [ DialogOptionCovered ] . position_x + x_offs ;
      y = ChatRoster [ DialogOptionCovered ] . position_y + y_offs ;
      
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
option_under_position ( int x , int y )
{
  int i;

  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      if ( ChatRoster [ i ] . position_x == (-1) ) continue;
      if ( ChatRoster [ i ] . position_y == (-1) ) continue;

      if ( ( abs ( ChatRoster [ i ] . position_x - x ) < 2 * BOX_RADIUS_X ) &&
	   ( abs ( ChatRoster [ i ] . position_y - y ) < 2 * BOX_RADIUS_Y ) )
	return ( i ) ;

    }

  return ( -1 );
}; // int option_under_position ( int x , int y) 

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

  /*
    GuiGraph *graph = (GuiGraph *) data;
    DlgPoint point ((int) event->x, (int) event->y);
    
    // Left button released
    if (event->button == 1)
    {
        switch (graph->mode ())
        {
            // nothing selected
            case IDLE:
            {
                // select the node under the cursor, if any
                if (!graph->selectNode (point))
                    // otherwise create a new circle at that position
                    if (GuiDlgedit::window->mode () != L10N_PREVIEW)
                        graph->newCircle (point);
                
                break;
            }

            // node selected
            case NODE_SELECTED:
            {
                // ignore edit command if in preview mode
                if (GuiDlgedit::window->mode () == L10N_PREVIEW)
                    break;

                // try to create a new link between two nodes
                graph->newArrow (point);
                break;
            }
                        
            // node dragged
            case NODE_DRAGGED:
            {
                // stop dragging
                graph->stopDragging (point);
                break;
            }
                    
            default: break;
        }
    }

    */

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
      dialog_option_delete_marked ( wnd , NULL , (gpointer) NULL ) ;
      // currently_mouse_grabbed_option = (-1) ;
      // DebugPrintf ( 1 , "\n Now the 'Drop' in 'Drag and Drop' has occured..." );
      break;
    case GDK_Insert:

      break;

    default:
      DebugPrintf ( 1 , "\nKey press event encountered..." );
      break;
    }

  /*
    GuiGraph *graph = (GuiGraph *) data;
    DlgPoint point ((int) event->x, (int) event->y);
    
    // Left button released
    if (event->button == 1)
    {
        switch (graph->mode ())
        {
            // nothing selected
            case IDLE:
            {
                // select the node under the cursor, if any
                if (!graph->selectNode (point))
                    // otherwise create a new circle at that position
                    if (GuiDlgedit::window->mode () != L10N_PREVIEW)
                        graph->newCircle (point);
                
                break;
            }

            // node selected
            case NODE_SELECTED:
            {
                // ignore edit command if in preview mode
                if (GuiDlgedit::window->mode () == L10N_PREVIEW)
                    break;

                // try to create a new link between two nodes
                graph->newArrow (point);
                break;
            }
                        
            // node dragged
            case NODE_DRAGGED:
            {
                // stop dragging
                graph->stopDragging (point);
                break;
            }
                    
            default: break;
        }
    }

    */

    return TRUE;
}

/* ----------------------------------------------------------------------
 * This function should just draw the boxes of the currently loaded
 * dialog.  Nothing more than that is done.  No update of any form is
 * performed.
 * ---------------------------------------------------------------------- */
void
gui_draw_all_dialog_boxes( void )
{
  int i;
  GdkRectangle update_rect;
  GdkGC *box_color;

  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      //--------------------
      // Of course we draw only those dialog boxes, that are really in use...
      //
      if ( ChatRoster [ i ] . position_x == (-1) ) continue;
      if ( ChatRoster [ i ] . position_y == (-1) ) continue;

      //--------------------
      // If that box is the one currently marked, then we'll draw it in red color,
      // otherwise it will be in black color.
      //
      if ( currently_marked_dialog_option == i )
	box_color = gui_get_color ( GC_RED );
      else
	box_color = gui_get_color ( GC_BLACK );

      //--------------------
      // Depending on whether it's a real dialog option or just a technical
      // option like in a flow chart, we draw either a box or a diamond...
      //
      if ( strcmp ( ChatRoster [ i ] . option_text , "THIS WILL NOT EVER BE VISIBLE" ) )
	{
	  //--------------------
	  // Here we draw the box...
	  //
	  update_rect.x = ChatRoster [ i ] . position_x - BOX_RADIUS_X ;
	  update_rect.y = ChatRoster [ i ] . position_y - BOX_RADIUS_Y ;
	  update_rect.width = 2 * BOX_RADIUS_X ;
	  update_rect.height = 2 * BOX_RADIUS_Y ;
	  gdk_draw_rectangle ( surface , box_color , FALSE, update_rect.x, update_rect.y,
			       update_rect.width, update_rect.height);
	}
      else
	{
	  //--------------------
	  // Here we draw the diamond...
	  //
	  gdk_draw_line ( surface , box_color , 
			  ChatRoster [ i ] . position_x - BOX_RADIUS_X , ChatRoster [ i ] . position_y , 
			  ChatRoster [ i ] . position_x , ChatRoster [ i ] . position_y + BOX_RADIUS_Y );
	  gdk_draw_line ( surface , box_color, 
			  ChatRoster [ i ] . position_x , ChatRoster [ i ] . position_y + BOX_RADIUS_Y , 
			  ChatRoster [ i ] . position_x + BOX_RADIUS_X , ChatRoster [ i ] . position_y );
	  gdk_draw_line ( surface , box_color , 
			  ChatRoster [ i ] . position_x + BOX_RADIUS_X , ChatRoster [ i ] . position_y , 
			  ChatRoster [ i ] . position_x , ChatRoster [ i ] . position_y - BOX_RADIUS_Y );
	  gdk_draw_line ( surface , box_color , 
			  ChatRoster [ i ] . position_x , ChatRoster [ i ] . position_y - BOX_RADIUS_Y , 
			  ChatRoster [ i ] . position_x - BOX_RADIUS_X , ChatRoster [ i ] . position_y );
	}

      //--------------------
      // In any case, we can make a rectangle into the box or diamond to indicate it
      // will be executed on startup...
      //
      if ( ChatRoster [ i ] . always_execute_this_option_prior_to_dialog_start )
	{
	  //--------------------
	  // Here we draw the box...
	  //
	  update_rect.x = ChatRoster [ i ] . position_x - BOX_RADIUS_X/2 ;
	  update_rect.y = ChatRoster [ i ] . position_y - BOX_RADIUS_Y/2 ;
	  update_rect.width = 2 * BOX_RADIUS_X/2 ;
	  update_rect.height = 2 * BOX_RADIUS_Y/2 ;
	  gdk_draw_rectangle ( surface , box_color , TRUE, update_rect.x, update_rect.y,
			       update_rect.width, update_rect.height);
	}

    }
}; // void gui_draw_all_dialog_boxes( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
gui_draw_arrow ( int source_x, int source_y , int dest_x, int dest_y , int color , int cut_off_tail )
{
  moderately_finepoint rotation_vector;
  float vect_len;

  //--------------------
  // Maybe we have to cut off the tail a bit.  Ok, that should not be too
  // difficult after all...
  //
  if ( cut_off_tail )
    {
      rotation_vector . x = dest_x - source_x ;
      rotation_vector . y = dest_y - source_y ;
      vect_len = sqrt ( rotation_vector.x * rotation_vector.x + rotation_vector.y * rotation_vector.y ) ;
      rotation_vector . x *= (vect_len - ( (float) BOX_RADIUS_X ) * sqrt(2) ) / vect_len ;
      rotation_vector . y *= (vect_len - ( (float) BOX_RADIUS_X ) * sqrt(2) ) / vect_len ;
      source_x = dest_x - rotation_vector . x ;
      source_y = dest_y - rotation_vector . y ;
    }

  //--------------------
  // First we draw the main line of the arrow, always in plain black...
  //
  gdk_draw_line ( surface , wnd ->style->black_gc , source_x , source_y , dest_x , dest_y );

  //--------------------
  // Maybe this 'line' is a point only.  Then of course we can and must
  // quit here...
  //
  if ( ( dest_x == source_x ) && ( dest_y == source_y ) ) return ;

  //--------------------
  // Now we can start to draw the arrowhead.  For this we will have to
  // rotate vectors a bit...  shouldn't be too difficult though...
  //
  // First we cut down the arrow-vector to appropriate length...
  //
  rotation_vector . x = dest_x - source_x ; 
  rotation_vector . y = dest_y - source_y ;
  vect_len = sqrt ( rotation_vector . x * rotation_vector . x + rotation_vector . y * rotation_vector . y ) ;
  rotation_vector . x *= ( (float) BOX_RADIUS_X ) / vect_len ;
  rotation_vector . y *= ( (float) BOX_RADIUS_Y ) / vect_len ;

  //--------------------
  // Now we can rotate it and use the result for our arrowhead lines...
  //
  RotateVectorByAngle ( & ( rotation_vector ) , 30 );
  gdk_draw_line ( surface , gui_get_color ( color ) , dest_x , dest_y , 
		  dest_x - rotation_vector.x , dest_y-rotation_vector.y );
  gdk_draw_line ( surface , gui_get_color ( color ) , dest_x + 1 , dest_y , 
		  dest_x - rotation_vector.x + 1 , dest_y-rotation_vector.y );
  gdk_draw_line ( surface , gui_get_color ( color ) , dest_x , dest_y + 1 , 
		  dest_x - rotation_vector.x , dest_y-rotation_vector.y + 1 );

  RotateVectorByAngle ( & ( rotation_vector ) , -60 );
  gdk_draw_line ( surface , gui_get_color ( color ) , dest_x , dest_y , 
		  dest_x - rotation_vector.x , dest_y-rotation_vector.y );
  gdk_draw_line ( surface , gui_get_color ( color ) , dest_x + 1 , dest_y , 
		  dest_x - rotation_vector.x + 1 , dest_y-rotation_vector.y );
  gdk_draw_line ( surface , gui_get_color ( color ) , dest_x , dest_y + 1 , 
		  dest_x - rotation_vector.x , dest_y-rotation_vector.y + 1 );

}; // void gui_draw_arrow ( int source_x, int source_y , int dest_x, int dest_y )

/* ----------------------------------------------------------------------
 * This function displays all the arrows going from some dialog option to
 * another dialog options.  
 * The drawing will be done, but no updating on the screen occurs within
 * this function.
 * ---------------------------------------------------------------------- */
void
gui_draw_all_dialog_arrows ( void )
{
  int i;
  int j;
  moderately_finepoint arrow_vector;
  float vector_len;
  int color ;

  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      //--------------------
      // Maybe this dialog option is not in use... then we can
      // continue with the next dialog option...
      //
      if ( ChatRoster [ i ] . position_x == (-1) ) continue;
      if ( ChatRoster [ i ] . position_y == (-1) ) continue;

      //--------------------
      // Now we can show each of the implications of this dialog
      // option being chosen by the player...
      //
      for ( j = 0 ; j < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; j ++ )
	{
	  if ( ChatRoster [ i ] . change_option_nr [ j ] < 0 ) continue;
	  if ( i == ChatRoster [ i ] . change_option_nr [ j ] ) continue;

	  arrow_vector . x = ChatRoster [ ChatRoster [ i ] . change_option_nr [ j ] ] . position_x - ChatRoster [ i ] . position_x ;
	  arrow_vector . y = ChatRoster [ ChatRoster [ i ] . change_option_nr [ j ] ] . position_y - ChatRoster [ i ] . position_y ;

	  vector_len = sqrt ( arrow_vector . x * arrow_vector . x + arrow_vector . y * arrow_vector . y );
	  arrow_vector . x *= (vector_len - ( (float) BOX_RADIUS_X ) * sqrt(2) ) / vector_len ;
	  arrow_vector . y *= (vector_len - ( (float) BOX_RADIUS_X ) * sqrt(2) ) / vector_len ;

	  if ( ChatRoster [ i ] . change_option_to_value [ j ] == 0 )
	    color = GC_DARK_BLUE ;
	  else 
	    color = GC_RED ;

	  gui_draw_arrow ( ChatRoster [ i ] . position_x , 
			   ChatRoster [ i ] . position_y , 
			   ChatRoster [ i ] . position_x + arrow_vector . x ,
			   ChatRoster [ i ] . position_y + arrow_vector . y , color , TRUE );

	}

      //--------------------
      // Maybe there is also an on-goto-condition here.  Then we will make
      // green and orange arrows for that...
      //
      if ( strlen ( ChatRoster [ i ] . on_goto_condition ) != 0 )
	{

	  arrow_vector . x = ChatRoster [ ChatRoster [ i ] . on_goto_first_target ] . position_x - ChatRoster [ i ] . position_x ;
	  arrow_vector . y = ChatRoster [ ChatRoster [ i ] . on_goto_first_target ] . position_y - ChatRoster [ i ] . position_y ;

	  vector_len = sqrt ( arrow_vector . x * arrow_vector . x + arrow_vector . y * arrow_vector . y );
	  arrow_vector . x *= (vector_len - ( (float) BOX_RADIUS_X ) * sqrt(2) ) / vector_len ;
	  arrow_vector . y *= (vector_len - ( (float) BOX_RADIUS_X ) * sqrt(2) ) / vector_len ;
	  gui_draw_arrow ( ChatRoster [ i ] . position_x , 
			   ChatRoster [ i ] . position_y , 
			   ChatRoster [ i ] . position_x + arrow_vector . x ,
			   ChatRoster [ i ] . position_y + arrow_vector . y , GC_GREEN , TRUE );




	  arrow_vector . x = ChatRoster [ ChatRoster [ i ] . on_goto_second_target ] . position_x - ChatRoster [ i ] . position_x ;
	  arrow_vector . y = ChatRoster [ ChatRoster [ i ] . on_goto_second_target ] . position_y - ChatRoster [ i ] . position_y ;

	  vector_len = sqrt ( arrow_vector . x * arrow_vector . x + arrow_vector . y * arrow_vector . y );
	  arrow_vector . x *= (vector_len - ( (float) BOX_RADIUS_X ) * sqrt(2) ) / vector_len ;
	  arrow_vector . y *= (vector_len - ( (float) BOX_RADIUS_X ) * sqrt(2) ) / vector_len ;
	  gui_draw_arrow ( ChatRoster [ i ] . position_x , 
			   ChatRoster [ i ] . position_y , 
			   ChatRoster [ i ] . position_x + arrow_vector . x ,
			   ChatRoster [ i ] . position_y + arrow_vector . y , GC_YELLOW , TRUE );


	}


    }
}; // void gui_draw_all_dialog_arrows ( void )

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

  gui_draw_all_dialog_boxes();

  gui_draw_all_dialog_arrows ();

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
  
  DebugPrintf ( 1 ,"\n ...cleaning out old dialog roster from possible old data..." );
  DebugPrintf ( 1 ,"\n----------------------------------------------------------------------" );

  InitChatRosterForNewDialogue(  );

  DebugPrintf ( 1 ,"\n ...now attempting to load dialog file..." );
  DebugPrintf ( 1 ,"\n----------------------------------------------------------------------" );

  strcpy ( LastUsedFileName , gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( fs ) ) ) ;
  LoadChatRosterWithChatSequence ( gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( fs ) ) );

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
  DebugPrintf ( 1 ,"\n ...Dialog file should be loaded now by now." );

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
  save_dialog_roster_to_file ( gtk_file_selection_get_filename ( GTK_FILE_SELECTION ( fs ) ) );

  //--------------------
  // Now that we have given a new name to the file, the main window should show the current file name
  //
  gtk_window_set_title ( GTK_WINDOW ( wnd ), LastUsedFileName );

  DebugPrintf ( 1 ,"\n ...Dialog file should be saved by now." );
 
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
  printf ( "Termination of the FreedroidRPG Dialog Editor invoked.  \n\
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

gint on_file_new_activate ( GtkWidget *widget,
			    GdkEvent  *event ) // ,                    gpointer   data )
{

  InitChatRosterForNewDialogue( );

  gui_redraw_graph_completely ( ) ;

  strcpy ( LastUsedFileName , "UNDEFINED_FILENAME.dialog" ) ;

  gtk_window_set_title ( GTK_WINDOW ( wnd ) , "Freedroid Dialog Editor: No file loaded!" );

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
				   "../dialogs/penguin.png");
  
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
  DebugPrintf ( 1 ,"\n ...now attempting to save dialog file..." );

  enforce_authors_notes (  );
  save_dialog_roster_to_file ( LastUsedFileName );

  DebugPrintf ( 1 ,"\n ...Dialog file should be saved by now." );

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
				   "../dialogs/penguin.png");
  
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
  //--------------------
  // We say, that we're here...
  //
  DebugPrintf ( 1 , "\nhelp_about_freedroidRPG_dialog_editor (...): real function call confirmed...");

  gui_start_pure_text_popup_window ( "\n\
\n\
   FreedroidRPG Dialog Editor Version 1.0\n\
\n\
   (c) 2003 by Johannes Prix\n\
\n\
   Feel free to copy and redistribute this file under the terms\n\
   specified in the GNU General Public License, Version 2, \n\
   see below for the details.\n\
\n\
\n\
This program is distributed under the terms of the GPL v2.\n\
\n\
		    GNU GENERAL PUBLIC LICENSE\n\
		       Version 2, June 1991\n\
\n\
 Copyright (C) 1989, 1991 Free Software Foundation, Inc.\n\
                          675 Mass Ave, Cambridge, MA 02139, USA\n\
 Everyone is permitted to copy and distribute verbatim copies\n\
 of this license document, but changing it is not allowed.\n\
\n\
			    Preamble\n\
\n\
  The licenses for most software are designed to take away your\n\
freedom to share and change it.  By contrast, the GNU General Public\n\
License is intended to guarantee your freedom to share and change free\n\
software--to make sure the software is free for all its users.  This\n\
General Public License applies to most of the Free Software\n\
Foundation's software and to any other program whose authors commit to\n\
using it.  (Some other Free Software Foundation software is covered by\n\
the GNU Library General Public License instead.)  You can apply it to\n\
your programs, too.\n\
\n\
  When we speak of free software, we are referring to freedom, not\n\
price.  Our General Public Licenses are designed to make sure that you\n\
have the freedom to distribute copies of free software (and charge for\n\
this service if you wish), that you receive source code or can get it\n\
if you want it, that you can change the software or use pieces of it\n\
in new free programs; and that you know you can do these things.\n\
\n\
  To protect your rights, we need to make restrictions that forbid\n\
anyone to deny you these rights or to ask you to surrender the rights.\n\
These restrictions translate to certain responsibilities for you if you\n\
distribute copies of the software, or if you modify it.\n\
\n\
  For example, if you distribute copies of such a program, whether\n\
gratis or for a fee, you must give the recipients all the rights that\n\
you have.  You must make sure that they, too, receive or can get the\n\
source code.  And you must show them these terms so they know their\n\
rights.\n\
\n\
  We protect your rights with two steps: (1) copyright the software, and\n\
(2) offer you this license which gives you legal permission to copy,\n\
distribute and/or modify the software.\n\
\n\
  Also, for each author's protection and ours, we want to make certain\n\
that everyone understands that there is no warranty for this free\n\
software.  If the software is modified by someone else and passed on, we\n\
want its recipients to know that what they have is not the original, so\n\
that any problems introduced by others will not reflect on the original\n\
authors' reputations.\n\
\n\
  Finally, any free program is threatened constantly by software\n\
patents.  We wish to avoid the danger that redistributors of a free\n\
program will individually obtain patent licenses, in effect making the\n\
program proprietary.  To prevent this, we have made it clear that any\n\
patent must be licensed for everyone's free use or not licensed at all.\n\
\n\
  The precise terms and conditions for copying, distribution and\n\
modification follow.\n\
		    GNU GENERAL PUBLIC LICENSE\n\
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION\n\
\n\
  0. This License applies to any program or other work which contains\n\
a notice placed by the copyright holder saying it may be distributed\n\
under the terms of this General Public License.  The \"Program\", below,\n\
refers to any such program or work, and a \"work based on the Program\"\n\
means either the Program or any derivative work under copyright law:\n\
that is to say, a work containing the Program or a portion of it,\n\
either verbatim or with modifications and/or translated into another\n\
language.  (Hereinafter, translation is included without limitation in\n\
the term \"modification\".)  Each licensee is addressed as \"you\".\n\
\n\
Activities other than copying, distribution and modification are not\n\
covered by this License; they are outside its scope.  The act of\n\
running the Program is not restricted, and the output from the Program\n\
is covered only if its contents constitute a work based on the\n\
Program (independent of having been made by running the Program).\n\
Whether that is true depends on what the Program does.\n\
\n\
  1. You may copy and distribute verbatim copies of the Program's\n\
source code as you receive it, in any medium, provided that you\n\
conspicuously and appropriately publish on each copy an appropriate\n\
copyright notice and disclaimer of warranty; keep intact all the\n\
notices that refer to this License and to the absence of any warranty;\n\
and give any other recipients of the Program a copy of this License\n\
along with the Program.\n\
\n\
You may charge a fee for the physical act of transferring a copy, and\n\
you may at your option offer warranty protection in exchange for a fee.\n\
\n\
  2. You may modify your copy or copies of the Program or any portion\n\
of it, thus forming a work based on the Program, and copy and\n\
distribute such modifications or work under the terms of Section 1\n\
above, provided that you also meet all of these conditions:\n\
\n\
    a) You must cause the modified files to carry prominent notices\n\
    stating that you changed the files and the date of any change.\n\
\n\
    b) You must cause any work that you distribute or publish, that in\n\
    whole or in part contains or is derived from the Program or any\n\
    part thereof, to be licensed as a whole at no charge to all third\n\
    parties under the terms of this License.\n\
\n\
    c) If the modified program normally reads commands interactively\n\
    when run, you must cause it, when started running for such\n\
    interactive use in the most ordinary way, to print or display an\n\
    announcement including an appropriate copyright notice and a\n\
    notice that there is no warranty (or else, saying that you provide\n\
    a warranty) and that users may redistribute the program under\n\
    these conditions, and telling the user how to view a copy of this\n\
    License.  (Exception: if the Program itself is interactive but\n\
    does not normally print such an announcement, your work based on\n\
    the Program is not required to print an announcement.)\n\
\n\
These requirements apply to the modified work as a whole.  If\n\
identifiable sections of that work are not derived from the Program,\n\
and can be reasonably considered independent and separate works in\n\
themselves, then this License, and its terms, do not apply to those\n\
sections when you distribute them as separate works.  But when you\n\
distribute the same sections as part of a whole which is a work based\n\
on the Program, the distribution of the whole must be on the terms of\n\
this License, whose permissions for other licensees extend to the\n\
entire whole, and thus to each and every part regardless of who wrote it.\n\
\n\
Thus, it is not the intent of this section to claim rights or contest\n\
your rights to work written entirely by you; rather, the intent is to\n\
exercise the right to control the distribution of derivative or\n\
collective works based on the Program.\n\
\n\
In addition, mere aggregation of another work not based on the Program\n\
with the Program (or with a work based on the Program) on a volume of\n\
a storage or distribution medium does not bring the other work under\n\
the scope of this License.\n\
\n\
  3. You may copy and distribute the Program (or a work based on it,\n\
under Section 2) in object code or executable form under the terms of\n\
Sections 1 and 2 above provided that you also do one of the following:\n\
\n\
    a) Accompany it with the complete corresponding machine-readable\n\
    source code, which must be distributed under the terms of Sections\n\
    1 and 2 above on a medium customarily used for software interchange; or,\n\
\n\
    b) Accompany it with a written offer, valid for at least three\n\
    years, to give any third party, for a charge no more than your\n\
    cost of physically performing source distribution, a complete\n\
    machine-readable copy of the corresponding source code, to be\n\
    distributed under the terms of Sections 1 and 2 above on a medium\n\
    customarily used for software interchange; or,\n\
\n\
    c) Accompany it with the information you received as to the offer\n\
    to distribute corresponding source code.  (This alternative is\n\
    allowed only for noncommercial distribution and only if you\n\
    received the program in object code or executable form with such\n\
    an offer, in accord with Subsection b above.)\n\
\n\
The source code for a work means the preferred form of the work for\n\
making modifications to it.  For an executable work, complete source\n\
code means all the source code for all modules it contains, plus any\n\
associated interface definition files, plus the scripts used to\n\
control compilation and installation of the executable.  However, as a\n\
special exception, the source code distributed need not include\n\
anything that is normally distributed (in either source or binary\n\
form) with the major components (compiler, kernel, and so on) of the\n\
operating system on which the executable runs, unless that component\n\
itself accompanies the executable.\n\
\n\
If distribution of executable or object code is made by offering\n\
access to copy from a designated place, then offering equivalent\n\
access to copy the source code from the same place counts as\n\
distribution of the source code, even though third parties are not\n\
compelled to copy the source along with the object code.\n\
\n\
  4. You may not copy, modify, sublicense, or distribute the Program\n\
except as expressly provided under this License.  Any attempt\n\
otherwise to copy, modify, sublicense or distribute the Program is\n\
void, and will automatically terminate your rights under this License.\n\
However, parties who have received copies, or rights, from you under\n\
this License will not have their licenses terminated so long as such\n\
parties remain in full compliance.\n\
\n\
  5. You are not required to accept this License, since you have not\n\
signed it.  However, nothing else grants you permission to modify or\n\
distribute the Program or its derivative works.  These actions are\n\
prohibited by law if you do not accept this License.  Therefore, by\n\
modifying or distributing the Program (or any work based on the\n\
Program), you indicate your acceptance of this License to do so, and\n\
all its terms and conditions for copying, distributing or modifying\n\
the Program or works based on it.\n\
\n\
  6. Each time you redistribute the Program (or any work based on the\n\
Program), the recipient automatically receives a license from the\n\
original licensor to copy, distribute or modify the Program subject to\n\
these terms and conditions.  You may not impose any further\n\
restrictions on the recipients' exercise of the rights granted herein.\n\
You are not responsible for enforcing compliance by third parties to\n\
this License.\n\
\n\
  7. If, as a consequence of a court judgment or allegation of patent\n\
infringement or for any other reason (not limited to patent issues),\n\
conditions are imposed on you (whether by court order, agreement or\n\
otherwise) that contradict the conditions of this License, they do not\n\
excuse you from the conditions of this License.  If you cannot\n\
distribute so as to satisfy simultaneously your obligations under this\n\
License and any other pertinent obligations, then as a consequence you\n\
may not distribute the Program at all.  For example, if a patent\n\
license would not permit royalty-free redistribution of the Program by\n\
all those who receive copies directly or indirectly through you, then\n\
the only way you could satisfy both it and this License would be to\n\
refrain entirely from distribution of the Program.\n\
\n\
If any portion of this section is held invalid or unenforceable under\n\
any particular circumstance, the balance of the section is intended to\n\
apply and the section as a whole is intended to apply in other\n\
circumstances.\n\
\n\
It is not the purpose of this section to induce you to infringe any\n\
patents or other property right claims or to contest validity of any\n\
such claims; this section has the sole purpose of protecting the\n\
integrity of the free software distribution system, which is\n\
implemented by public license practices.  Many people have made\n\
generous contributions to the wide range of software distributed\n\
through that system in reliance on consistent application of that\n\
system; it is up to the author/donor to decide if he or she is willing\n\
to distribute software through any other system and a licensee cannot\n\
impose that choice.\n\
\n\
This section is intended to make thoroughly clear what is believed to\n\
be a consequence of the rest of this License.\n\
\n\
  8. If the distribution and/or use of the Program is restricted in\n\
certain countries either by patents or by copyrighted interfaces, the\n\
original copyright holder who places the Program under this License\n\
may add an explicit geographical distribution limitation excluding\n\
those countries, so that distribution is permitted only in or among\n\
countries not thus excluded.  In such case, this License incorporates\n\
the limitation as if written in the body of this License.\n\
\n\
  9. The Free Software Foundation may publish revised and/or new versions\n\
of the General Public License from time to time.  Such new versions will\n\
be similar in spirit to the present version, but may differ in detail to\n\
address new problems or concerns.\n\
\n\
Each version is given a distinguishing version number.  If the Program\n\
specifies a version number of this License which applies to it and \"any\n\
later version\", you have the option of following the terms and conditions\n\
either of that version or of any later version published by the Free\n\
Software Foundation.  If the Program does not specify a version number of\n\
this License, you may choose any version ever published by the Free Software\n\
Foundation.\n\
\n\
  10. If you wish to incorporate parts of the Program into other free\n\
programs whose distribution conditions are different, write to the author\n\
to ask for permission.  For software which is copyrighted by the Free\n\
Software Foundation, write to the Free Software Foundation; we sometimes\n\
make exceptions for this.  Our decision will be guided by the two goals\n\
of preserving the free status of all derivatives of our free software and\n\
of promoting the sharing and reuse of software generally.\n\
\n\
			    NO WARRANTY\n\
\n\
  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY\n\
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN\n\
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n\
PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED\n\
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n\
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS\n\
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE\n\
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n\
REPAIR OR CORRECTION.\n\
\n\
  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n\
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR\n\
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,\n\
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING\n\
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n\
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY\n\
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER\n\
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n\
POSSIBILITY OF SUCH DAMAGES.\n\
\n\
		     END OF TERMS AND CONDITIONS\n\
\n\n" , "About FreedroidRPG Dialog Editor" );

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
This dialog editor has been tested, but that does by no means\n\
mean that it is completely bug-free.\n\
\n\
If you think you have found some bug, please contact the FreedroidRPG\n\
developers, best by sending e-mail to the following address:\n\
\n\
freedroid-discussion@lists.sourceforge.net\n\
\n\
Thanks a lot for submitting any bug or feedback or, of course and even\n\
more appreciated, new or patched dialogs for new characters that we can\n\
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
FILES AND DIALOG FILE NAMING CONVENTIONS\n\
\n\
The file menu usage is obious to any computer literate person.\n\
Use it to load and save your dialog files.  The file names you choose\n\
should end in '.dialog' though.  And any subdialogs you wish to make\n\
use of should start with 'subdlg_' and then anything.\n\
These conventions are currently required by the game engine, but can\n\
be adapted if you say you'd like to have it differently.\n\
\n\
DRAGGING AND DROPPING DIALOG OPTIONS\n\
\n\
Dialog options can be moved via the left mouse button in typical\n\
drag'n'drop style.  The position of the dialog options will be remembered,\n\
but it will be without meaning for the overall flow of the dialog editor.\n\
Repositioning of dialog options is only useful to get a better overview\n\
about what really happens and what really can happen within the flow \n\
of the dialog, not to modify the flow of the dialog itself.\n\
\n\
ADDING AND REMOVING CONNECTIONS BETWEEN DIALOG OPTIONS\n\
\n\
Any dialog option can MAKE AVAILABLE any other dialog option (red arrow)\n\
or it can also DISABLE other dialog options (blue arrow).\n\
It can also ignore the availability of any other dialog option (no arrow).\n\
\n\
In order to change current connection situation, first mark the SOURCE\n\
dialog option by clicking the middle mouse button on it.  (That might be\n\
both buttons simulaneously, if you're using a 2-button-mouse and have \n\
emulation of third button turned on.)\n\
Then click the middle mouse button again on the TARGET dialog option.\n\
The connection status should cycle from DISABLE to MAKE AVAILABE \n\
to NO CONNECTION to DISABLE to MAKE AVAILABLE to NO ... \n\
\n\
EDITING THE INNER WORKINGS OF A DIALOG OPTION\n\
\n\
Use right-mouse button directly on the option.  That should open up a popup\n\
window and prompt you for new input.\n\
\n\
For option text and option file names there are certain keywords that\n\
the game engine will understand.  These keywords are:\n\
\n\
THIS WILL NOT EVER BE VISIBLE\n\
\n\
and\n\
\n\
NO_SAMPLE_HERE_AND_DONT_WAIT_EITHER\n\
\n\
The first of these two keywords can go into the dialog option text field.\n\
It will cause the dialog option to take a diamond-shaped form.  It does not\n\
have other implications.\n\
The second of these keywords will take effect when put into the dialog\n\
option sample file name field.  There it will COMPLETELY DISABLE SAMPLE\n\
FILE PLAYING AND ALSO DISABLE WAITING so that the actual 'option text'\n\
will never really be displayed/voiced.  This is very useful when having\n\
some dialog options for inner workings like separating some cases and then\n\
finally arriving somewhere at an answer.  The options passed on the way\n\
before that final answer will best be disabled and only the final answer\n\
be displayed.  These keywords help to achieve this.\n\
NOTE:  Note however that REPLY SAMPLES AND REPLY TEXTS WILL\n\
STILL BE DISPLAYED (if present) EVEN IF THE ABOVE KEYWORDS\n\
WERE SUPPLIED AS OPTIONS.\n\
If you do not wish to have some reply uttered, just leave the corresponding\n\
reply entries blank.\n\
\n\
\n" , "FreedroidRPG Dialog Editor Usage" );

  return ( FALSE );

}; // gint help_dialog_editor_usage ( GtkWidget *widget, GdkEvent  *event, gpointer   data )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
gint 
dialog_option_insert_new ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
  int i;

  //--------------------
  // We say, that we're here...
  //
  DebugPrintf ( 1 , "\ndialog_option_insert_new (...): real function call confirmed...");

  //--------------------
  // We must find an unused option entry, so we can plant our
  // new option there.
  //
  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      if ( strlen ( ChatRoster [ i ] . option_text ) == 0 ) break;
    }

  //--------------------
  // If there was no more free entry within all of the roster, we do nothing
  // and just return...
  //
  if ( i >= MAX_DIALOGUE_OPTIONS_IN_ROSTER ) return ( FALSE );

  ChatRoster [ i ] . option_text = "NEWLY INSERTED DIALOG OPTION" ;
  ChatRoster [ i ] . option_sample_file_name = "Sorry_No_Voice_Sample_Yet_0.wav" ;
  ChatRoster [ i ] . position_x = 550 ;
  ChatRoster [ i ] . position_y = 35 ;

  //--------------------
  // So an option was inserted.  This means we have to redraw the screen, so that
  // the change made can become visible as well...
  //
  gui_redraw_graph_completely ( ); 

  return ( FALSE );
}; // gint dialog_option_insert_new ( GtkWidget *widget, GdkEvent  *event, gpointer   data )

/* ----------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------- */
gint 
dialog_option_delete_marked ( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
  int i;
  int j;

  //--------------------
  // We say, that we're here...
  //
  DebugPrintf ( 1 , "\ndialog_option_delete_marked (...): real function call confirmed...");

  //--------------------
  // Now if there is no dialog option marked, we don't have to
  // do a thing and can return immediately...
  //
  if ( currently_marked_dialog_option == (-1) )
    return ( FALSE );

  //--------------------
  // We clear out this dialog option, not taking into account the 
  // neglectable memory leak here...
  //
  delete_one_dialog_option ( currently_marked_dialog_option , TRUE ); 

  //--------------------
  // But all other connections, that connected into this dialog option
  // now don't make any sense any more and must be removed as well...
  //
  for ( i = 0 ; i < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; i ++ )
    {
      for ( j = 0 ; j < MAX_DIALOGUE_OPTIONS_IN_ROSTER ; j ++ )
	{
	  if ( ChatRoster [ i ] . change_option_nr [ j ] == currently_marked_dialog_option )
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
  currently_marked_dialog_option = (-1) ;

  //--------------------
  // So an option was removed, i.e. we have to redraw the screen, so that
  // the change made can become visible as well...
  //
  gui_redraw_graph_completely ( ); 

  return ( FALSE );
}; // gint dialog_option_delete_marked ( GtkWidget *widget, GdkEvent  *event, gpointer   data )

/* ----------------------------------------------------------------------
 * This function creates the main window of the FreedroidRPG Dialog Editor.
 * It also sets the proper window title.  This is NOT a callback function.
 * ---------------------------------------------------------------------- */
void
gui_create_main_window ( void )
{
  //--------------------
  // Now we create the main window
  wnd = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_usize (GTK_WIDGET (wnd), 980, 740);
  gtk_signal_connect (GTK_OBJECT (wnd), "delete_event", GTK_SIGNAL_FUNC ( delete_event ), NULL);

  //--------------------
  // This window should also have a proper window title...
  //
  gtk_window_set_title (GTK_WINDOW ( wnd ), "Freedroid Dialog Editor: No file loaded!");

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
  // but rather about 'Dialog Option'
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
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( dialog_option_insert_new ), "NEW DIALOG OPTION?" );
  gtk_widget_show (menuitem);
  
  // Open
  menuitem = gtk_menu_item_new_with_label ("Delete Marked...");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 11" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 12" );
  // gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_activate), "OPEN FILE?" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( dialog_option_delete_marked ), "DELETE MARKED DIALOG OPTION?" );
  gtk_widget_show (menuitem);
  
  // Attach 'Dialog Option' Menu
  menuitem = gtk_menu_item_new_with_label ("Dialog Option");
  gtk_widget_show (menuitem);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
  gtk_menu_bar_append (GTK_MENU_BAR (menu), menuitem);
  
  //--------------------
  // Now that the dialog option menu is done, we can start to attach another
  // line of menu options....  This time it will not be about 'Dialog option'
  // but rather about 'Help'
  //

  // File Menu
  submenu = gtk_menu_new ();
  
  // New
  menuitem = gtk_menu_item_new_with_label ("About FreedroidRPG Dialog Editor");
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
  menuitem = gtk_menu_item_new_with_label ("Dialog Editor Usage");
  gtk_container_add (GTK_CONTAINER (submenu), menuitem);
  // gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
  gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), "NOTIFY 11" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), "NOTIFY 12" );
  // gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_activate), "OPEN FILE?" );
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC ( help_dialog_editor_usage ), "DELETE MARKED DIALOG OPTION?" );
  gtk_widget_show (menuitem);
  
  // Attach 'Dialog Option' Menu
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
      
      currently_mouse_grabbed_option = option_under_position ( Pos_x , Pos_y ) ;

      gui_redraw_graph_completely (  );

      break;
    case 3:  
      DebugPrintf ( MOUSE_BUTTON_PRESS_DEBUG , "\nButton 3 WAS PRESSED!!" );

      gui_edit_dialog_option_interface ( option_under_position ( Pos_x , Pos_y ) ) ;

      gui_redraw_graph_completely (  );

      break;
    case 2:  
      DebugPrintf ( MOUSE_BUTTON_PRESS_DEBUG , "\nButton 2 WAS PRESSED!!" );

      gui_react_to_right_mousebutton ( option_under_position ( Pos_x , Pos_y ) ) ;

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
	  ChatRoster [ currently_mouse_grabbed_option ] . position_x = event -> x ;
	  ChatRoster [ currently_mouse_grabbed_option ] . position_y = event -> y ;

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
  graph_has_been_created = TRUE ;
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
 * This is the main function of our dialog editor.  But this time it does
 * not contain some 'main game loop' like in FreedroidRPG or Freedroid,
 * cause this 'main loop' is done within the GTK main function anyway.
 * ---------------------------------------------------------------------- */
int
main( int argc, char *argv[] )
{
  //--------------------
  // At first we initialize our dialog roster.  After all we don't want
  // complete random data in our structure being displayed at program
  // startup...
  //
  InitChatRosterForNewDialogue(  ) ;

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

  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\nmain:  now setting up the 'bottom line' part...." );

  //--------------------
  // Not it might be a good time to add the status bar on the very bottom of the
  // window.
  //
  gui_create_bottom_line( );

  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\nmain:  now getting ready for 'show_all'...." );

  //--------------------
  // Now that the main window has been assembled completely, we can start to
  // show it and all it's content to the user.
  //
  gtk_widget_show_all (wnd);

  DebugPrintf ( WINDOW_WARNINGS_DEBUG , "\nmain:  Ok.  Starting main loop and that's it from here..." );

  //--------------------
  // At this point all should be set up.  The editing machine is ready to be
  // used.  All we need to do is start the gtk main loop and let everything
  // else spring forth from there.
  //
  gtk_main ();

  return ( 0 );

}; // int main (...)


#undef DIALOG_EDITOR
