
#ifdef DIALOG_EDITOR 

dialogue_option ChatRoster[MAX_DIALOGUE_OPTIONS_IN_ROSTER];
int debug_level = 0;

char* remove_termination_character_if_nescessary ( char* our_string , char term_char );
char* add_termination_character_if_nescessary ( char* our_string , char term_char );
void gui_clear_graph_window ( void );
int item_under_position ( int x , int y );
void Terminate ( int exit_status ) ;
void gui_destroy_existing_tooltips ( void );
void gui_redraw_graph_completely ( void );
void edit_interface_cancel_button_pressed ( GtkWidget *w, GtkFileSelection *fs ); 
gint item_index_delete_marked ( GtkWidget *widget, GdkEvent  *event, gpointer   data );

void save_item_roster_to_file ( char* filename );
void RotateVectorByAngle ( moderately_finepoint* vector , float rot_angle );
void InitChatRosterForNewDialogue( void );
void LoadChatRosterWithChatSequence ( char* FullPathAndFullFilename );
void delete_one_item_index ( int i , int FirstInitialisation );
extern char authors_notes[100000];

extern void DebugPrintf (int db_level, const char *fmt, ...);

#else

extern dialogue_option ChatRoster[MAX_DIALOGUE_OPTIONS_IN_ROSTER];
extern int debug_level ;
extern void Terminate ( int exit_status ) ;
char authors_notes[100000]="\nYou can use this text field to make some notes on anything.  These otes will get saved with the dialog.\nThis is intended purely for the author's reference when using the dialog editor, while the engine of the game itself will ignore this entry.\n";

#endif




