/* 
 *
 *   Copyright (c) 2003  Johannes Prix
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
#ifdef DIALOG_EDITOR 

dialogue_option ChatRoster[MAX_DIALOGUE_OPTIONS_IN_ROSTER];
int debug_level = 0;

char* remove_termination_character_if_nescessary ( char* our_string , char term_char );
char* add_termination_character_if_nescessary ( char* our_string , char term_char );
void gui_clear_graph_window ( void );
int option_under_position ( int x , int y );
void Terminate ( int exit_status ) ;
void gui_destroy_existing_tooltips ( void );
void gui_redraw_graph_completely ( void );
void edit_interface_cancel_button_pressed ( GtkWidget *w, GtkFileSelection *fs ); 
gint dialog_option_delete_marked ( GtkWidget *widget, GdkEvent  *event, gpointer   data );

void save_dialog_roster_to_file ( char* filename );
void RotateVectorByAngle ( moderately_finepoint* vector , float rot_angle );
void InitChatRosterForNewDialogue( void );
void LoadChatRosterWithChatSequence ( char* FullPathAndFullFilename );
void delete_one_dialog_option ( int i , int FirstInitialisation );
extern char authors_notes[100000];

extern void DebugPrintf (int db_level, char *fmt, ...);

#else

extern dialogue_option ChatRoster[MAX_DIALOGUE_OPTIONS_IN_ROSTER];
extern int debug_level ;
extern void Terminate ( int exit_status ) ;
char authors_notes[100000]="\nYou can use this text field to make some notes on anything.  These notes will get saved with the dialog.\nThis is intended purely for the author's reference when using the dialog editor, while the engine of the game itself will ignore this entry.\n";

#endif

