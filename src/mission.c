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

/* ----------------------------------------------------------------------
 * This file contains all the functions managing the things one gets to see.
 * That includes assembling of enemys, assembling the currently
 * relevant porting of the map (the bricks I mean), drawing all visible
 * elements like bullets, blasts, enemys or influencer in a nonvisible
 * place in memory at first, and finally drawing them to the visible
 * screen for the user.
 * ---------------------------------------------------------------------- */

#define _mission_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "map.h"
#include "proto.h"
#include "colodefs.h"
#include "SDL_rotozoom.h"

int currently_selected_mission = (-1) ;
SDL_Rect mission_list_rect = { 20 , 280 , 280 , 180 } ; 
SDL_Rect mission_description_rect = { 300 , 30 , 320 , 420 } ; 

/* ----------------------------------------------------------------------
 * This function should display the currently assigned/unassigned mission
 * and all that directly over the combat screen without interrupting the
 * game in any other way.
 * ---------------------------------------------------------------------- */
void 
classic_show_mission_list ( void )
{
    int mis_num;
    
    //--------------------
    // If the log is not set to visible right now, we do not need to 
    // do anything more
    //
    if ( GameConfig.Mission_Log_Visible == FALSE ) return;
    if ( GameConfig.Mission_Log_Visible_Time >= GameConfig.Mission_Log_Visible_Max_Time ) return;
    
    SetCurrentFont ( FPS_Display_BFont );

    //--------------------
    // At this point we know, that the quest log is desired and
    // therefore we display it in-game:
    //
    SDL_SetClipRect( Screen , NULL );
    DisplayText( "\n   See quest log: \n" , User_Rect.x , User_Rect.y , &User_Rect );
    
    for ( mis_num = 0 ; mis_num < MAX_MISSIONS_IN_GAME; mis_num ++ )
    {
	// In case the mission does not exist at all, we need not do anything more...
	if ( Me[0].AllMissions[ mis_num ].MissionExistsAtAll != TRUE ) continue;
	
	// In case the mission was not yet assigned, we need not do anything more...
	// if ( Me[0].AllMissions[ mis_num ].MissionWasAssigned != TRUE ) continue;
	
	// In case the message is rather old, we need not do anything more...
	// if ( Me[0].AllMissions[ mis_num ].MissionLastStatusChangeTime > 1000 ) continue;
	
	// At this point we know, that the mission has recently been completed or failed
	
	if ( Me[0].AllMissions[ mis_num ].MissionIsComplete == TRUE )
	{
	    DisplayText( "\n* Mission completed: " , -1 , -1 , &User_Rect );
	}
	else if ( Me[0].AllMissions[ mis_num ].MissionWasFailed == TRUE )
	{
	    DisplayText( "\n* Mission failed: " , -1 , -1 , &User_Rect );
	}
	else if ( ! Me[0].AllMissions[ mis_num ].MissionWasAssigned == TRUE )
	{
	    DisplayText( "\n* Mission not yet assigned: " , -1 , -1 , &User_Rect );
	}
	else 
	    DisplayText( "\n* Mission assigned: " , -1 , -1 , &User_Rect );
	
	DisplayText( Me[0].AllMissions[ mis_num ].MissionName , -1 , -1 , &User_Rect );
	
    }
}; // void classic_show_mission_list ( void )

/* ----------------------------------------------------------------------
 * Inside the quest browser, when the user clicks on a mission from the
 * mission list, this mission should then become the currently selected
 * mission.  For this it is important, that a mouse click position can 
 * be resolved to a mission index.  This function is responsible for
 * exactly this.
 * ---------------------------------------------------------------------- */
int
resolve_mouse_click_to_mission_index ( void )
{
    int mis_num ;
    SDL_Rect current_rectangle;

    current_rectangle . x = mission_list_rect . x ;
    current_rectangle . y = mission_list_rect . y ;
    current_rectangle . w = mission_list_rect . w ;
    current_rectangle . h = FontHeight ( FPS_Display_BFont ) ;

    if ( GetMousePos_x() < mission_list_rect . x ) return ( -1 ) ;
    if ( GetMousePos_x() > mission_list_rect . x + mission_list_rect . w ) return ( -1 ) ;
    if ( GetMousePos_y() < mission_list_rect . y ) return ( -1 ) ;
    if ( GetMousePos_x() > mission_list_rect . y + mission_list_rect . h ) return ( -1 ) ;

    for ( mis_num = 0 ; mis_num < MAX_MISSIONS_IN_GAME; mis_num ++ )
    {
	// In case the mission does not exist at all, we need not do anything more...
	if ( Me [ 0 ] . AllMissions [ mis_num ] . MissionExistsAtAll != TRUE ) continue;
	
	//  In case the mission was not yet assigned, we need not do anything more...
	if ( Me [ 0 ] . AllMissions[ mis_num ].MissionWasAssigned != TRUE ) continue;

	if ( MouseCursorIsInRect ( & current_rectangle , 
				   GetMousePos_x() , GetMousePos_y() ) )
	{
	    return ( mis_num );
	}

	current_rectangle . y += FontHeight ( FPS_Display_BFont ) ;

    }

    return (-1) ;

}; // int resolve_mouse_click_to_mission_index ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
quest_browser_show_mission_info ( int mis_num )
{
    char temp_text[10000];
    int quest_text_index;
    int mission_diary_index;

    if ( ( mis_num < 0 ) || ( mis_num >= MAX_MISSIONS_IN_GAME ) )
    {
	fprintf ( stderr , "\nmission number received: %d." , mis_num );
	GiveStandardErrorMessage ( __FUNCTION__  , "\
There was an illegal mission number received.",
				   PLEASE_INFORM, IS_FATAL );
    }

    SetTextCursor ( mission_description_rect . x , 
		    mission_description_rect . y );

    DisplayText( "Mission: " , -1 , -1 , &mission_description_rect );
    DisplayText( Me [ 0 ] . AllMissions [ mis_num ] . MissionName , -1 , -1 , &mission_description_rect );
    DisplayText( "\nStatus: " , -1 , -1 , &mission_description_rect );
    if ( Me [ 0 ] . AllMissions [ mis_num ] . MissionIsComplete )
	DisplayText( "COMPLETE" , -1 , -1 , &mission_description_rect );
    else if ( Me [ 0 ] . AllMissions [ mis_num ] . MissionWasFailed )
	DisplayText( "FAILED" , -1 , -1 , &mission_description_rect );
    else
	DisplayText( "STILL OPEN" , -1 , -1 , &mission_description_rect );
    DisplayText( "\nDetails: " , -1 , -1 , &mission_description_rect );
    
    for ( mission_diary_index = 0 ; mission_diary_index < MAX_MISSION_DESCRIPTION_TEXTS ;
	  mission_diary_index ++ )
    {
	if ( Me [ 0 ] . AllMissions [ mis_num ] . mission_description_visible [ mission_diary_index ] )
	{
	    DisplayText( mission_diary_texts [ mis_num ] [ mission_diary_index ] , -1 , -1 , &mission_description_rect );	    
	    DisplayText( "\n" , -1 , -1 , &mission_description_rect );	    
	}
    }

}; // void quest_browser_show_mission_info ( int mis_num )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
quest_browser_fill_out_mission_list ( SDL_Rect list_rectangle )
{
    int mis_num ;
    point current_text_cursor = { list_rectangle . x , list_rectangle . y } ;

    //--------------------
    // We enter the current list of *known* missions (i.e. those assigned) into
    // the appropriate field.
    //
    for ( mis_num = 0 ; mis_num < MAX_MISSIONS_IN_GAME; mis_num ++ )
    {
	// In case the mission does not exist at all, we need not do anything more...
	if ( Me [ 0 ] . AllMissions [ mis_num ] . MissionExistsAtAll != TRUE ) continue;
	
	//  In case the mission was not yet assigned, we need not do anything more...
	if ( Me [ 0 ] . AllMissions[ mis_num ] . MissionWasAssigned != TRUE ) continue;

	if ( mis_num == currently_selected_mission )
	{
	    PutStringFont ( Screen , Blue_BFont , 
			    current_text_cursor . x , current_text_cursor . y ,
			    Me [ 0 ] . AllMissions [ mis_num ] . MissionName );
	}
	else
	{
	    if ( Me [ 0 ] . AllMissions [ mis_num ] . MissionIsComplete == TRUE )
	    {
		PutStringFont ( Screen , FPS_Display_BFont , 
				current_text_cursor . x , current_text_cursor . y ,
				Me [ 0 ] . AllMissions [ mis_num ] . MissionName );
	    }
	    else
	    {
		PutStringFont ( Screen , Red_BFont , 
				current_text_cursor . x , current_text_cursor . y ,
				Me [ 0 ] . AllMissions [ mis_num ] . MissionName );
	    }
	}
	current_text_cursor . y += FontHeight ( FPS_Display_BFont );
    }
    
}; // void quest_browser_fill_out_mission_list ( void )
        
/* ----------------------------------------------------------------------
 * This function manages the quest browser.
 * ---------------------------------------------------------------------- */
void
quest_browser_interface ( void )
{
    int back_to_game = FALSE;
    int mis_num ;

    //--------------------
    // This might take some time, so we need to be careful here,
    // so as not to generate a massive frame time, that would
    // throw every moving thing from the map.
    //
    Activate_Conservative_Frame_Computation ();
    make_sure_system_mouse_cursor_is_turned_on ( );

    while ( EscapePressed() );
    while ( SpacePressed() );
    while ( QPressed() );

    while ( ! back_to_game )
    {
	blit_special_background ( QUEST_BROWSER_BACKGROUND_CODE );
	quest_browser_fill_out_mission_list( mission_list_rect );
	if ( currently_selected_mission != (-1) )
	    quest_browser_show_mission_info ( currently_selected_mission );
	our_SDL_flip_wrapper ( Screen );
	    
	if ( QPressed() )
	{
	    while ( QPressed() );
	    back_to_game = TRUE ;
	}

	if ( SpacePressed() )
	{
	    currently_selected_mission = resolve_mouse_click_to_mission_index ( );
	    while ( SpacePressed() );
	}
	    
    }

}; // void quest_browser_interface ( void )


/*----------------------------------------------------------------------
 * This function checks, if the influencer has succeeded in his given 
 * mission.  If not it returns, if yes the EndTitle/Debriefing is
 * started.
 ----------------------------------------------------------------------*/
void 
CheckIfMissionIsComplete (void)
{
    int Robot_Counter;
    int ItemCounter;
    int mis_num;
    int ActionNum;
    static int CheckMissionGrid; 
    int this_mission_seems_completed = TRUE ;
#define MIS_COMPLETE_DEBUG 1
    
    //--------------------
    // We do not need to check for mission completed EVERY frame
    // It will be enough to do it now and then..., e.g. every 50th frame
    //
    CheckMissionGrid++;
    if ( ( CheckMissionGrid % 50 ) != 0 ) return;
    
    for ( mis_num = 0 ; mis_num < MAX_MISSIONS_IN_GAME ; mis_num ++ )
    {
	
	//--------------------
	// We need not do anything, if the mission has already failed or if
	// the mission is already completed or if the mission does not exist
	// at all or if the mission was not assigned yet
	//
	if ( Me [ 0 ] . AllMissions [ mis_num ] . MissionIsComplete == TRUE ) continue;
	if ( Me [ 0 ] . AllMissions [ mis_num ] . MissionWasFailed == TRUE ) continue;
	if ( Me [ 0 ] . AllMissions [ mis_num ] . MissionExistsAtAll != TRUE ) continue;
	if ( Me [ 0 ] . AllMissions [ mis_num ] . MissionWasAssigned != TRUE ) continue;
	
	DebugPrintf ( MIS_COMPLETE_DEBUG , "\nSomething was assigned at all..... mis_num = %d " , mis_num );
	
	this_mission_seems_completed = TRUE ;
	
	//--------------------
	// Continue if the Mission target KillOne is given but not fullfilled
	//
	if ( Me [ 0 ] . AllMissions [ mis_num ].KillOne != (-1) )
	{
	    for ( Robot_Counter=0 ; Robot_Counter < Number_Of_Droids_On_Ship ; Robot_Counter++ )
	    {
		if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
		     ( AllEnemys[Robot_Counter].Status != OUT ) && 
		     ( AllEnemys[Robot_Counter] . marker == Me [ 0 ] . AllMissions [ mis_num ] . KillOne ) )
		{
		    DebugPrintf ( MIS_COMPLETE_DEBUG , "\nOne of the marked droids is still alive...");
		    this_mission_seems_completed = FALSE ;
		    break;
		}
	    }
	}
	
	//--------------------
	// Continue if the Mission target fetch_item is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ].fetch_item != (-1) )
	{
	    
	    for ( ItemCounter = 0 ; ItemCounter < MAX_ITEMS_IN_INVENTORY ; ItemCounter++ )
	    {
		if ( Me [ 0 ] . Inventory [ ItemCounter ] . type == Me[0].AllMissions[ mis_num ].fetch_item )
		{
		    DebugPrintf ( MIS_COMPLETE_DEBUG , "\nDesired item IS PRESENT!!");
		    break;
		}
	    }
	    if ( ItemCounter >= MAX_ITEMS_IN_INVENTORY ) 
	    {
		// goto CheckNextMission;
		this_mission_seems_completed = FALSE ;
	    }
	}
	
	//--------------------
	// Continue if the Mission target KillAll is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ].KillAll != (-1) )
	{
	    //	  for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	    for ( Robot_Counter=0 ; Robot_Counter < Number_Of_Droids_On_Ship ; Robot_Counter++ )
	    {
		if ( ( AllEnemys[Robot_Counter].energy > 0 ) && ( AllEnemys[Robot_Counter].is_friendly == FALSE ) )
		{
		    DebugPrintf ( MIS_COMPLETE_DEBUG , "\nThere are some robots still alive, and you should kill them all...");
		    fflush(stdout);
		    
		    this_mission_seems_completed = FALSE ;
		    break;
		}
	    }
	}
	
	//--------------------
	// Continue if the Mission target KillClass is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ].KillClass != (-1) )
	{
	    // for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	    for ( Robot_Counter=0 ; Robot_Counter < Number_Of_Droids_On_Ship ; Robot_Counter++ )
	    {
		if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
		     ( AllEnemys[Robot_Counter].Status != OUT ) && 
		     ( Druidmap[AllEnemys[Robot_Counter].type].class == Me[0].AllMissions[ mis_num ].KillClass ) ) 
		{
		    DebugPrintf ( MIS_COMPLETE_DEBUG , "\nOne of that class is still alive: Nr=%d Lev=%d X=%f Y=%f." , 
				  Robot_Counter , AllEnemys[Robot_Counter].pos.z , 
				  AllEnemys[Robot_Counter].pos.x , AllEnemys[Robot_Counter].pos.y );
		    this_mission_seems_completed = FALSE ;
		    break;
		}
	    }
	}
	
	//--------------------
	// Continue if the Mission target must_clear_first_level is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ]. must_clear_first_level != (-1) )
	{
	    // for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	    for ( Robot_Counter=0 ; Robot_Counter < Number_Of_Droids_On_Ship ; Robot_Counter++ )
	    {
		if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
		     ( AllEnemys[Robot_Counter].Status != OUT ) && 
		     ( ! AllEnemys[Robot_Counter]. is_friendly ) && 
		     ( AllEnemys[Robot_Counter] . pos . z == Me[0].AllMissions[ mis_num ].must_clear_first_level ) ) 
		{
		    DebugPrintf ( MIS_COMPLETE_DEBUG , "\nOne bot on that first level is still alive: Nr=%d Lev=%d X=%f Y=%f." , 
				  Robot_Counter , AllEnemys[Robot_Counter].pos.z , 
				  AllEnemys[Robot_Counter].pos.x , AllEnemys[Robot_Counter].pos.y );
		    this_mission_seems_completed = FALSE ;
		    break;
		}
	    }
	}
	
	//--------------------
	// Continue if the Mission target must_clear_second_level is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ]. must_clear_second_level != (-1) )
	{
	    // for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	    for ( Robot_Counter=0 ; Robot_Counter < Number_Of_Droids_On_Ship ; Robot_Counter++ )
	    {
		if ( ( AllEnemys[Robot_Counter].energy > 0 ) && 
		     ( AllEnemys[Robot_Counter].Status != OUT ) && 
		     ( ! AllEnemys[Robot_Counter]. is_friendly ) && 
		     ( AllEnemys[Robot_Counter] . pos . z == Me[0].AllMissions[ mis_num ].must_clear_second_level ) ) 
		{
		    DebugPrintf ( MIS_COMPLETE_DEBUG , "\nOne bot on that second level is still alive: Nr=%d Lev=%d X=%f Y=%f." , 
				  Robot_Counter , AllEnemys[Robot_Counter].pos.z , 
				  AllEnemys[Robot_Counter].pos.x , AllEnemys[Robot_Counter].pos.y );
		    this_mission_seems_completed = FALSE ;
		    break;
		}
	    }
	}
	
	
	//--------------------
	// Continue if the Mission target MustBeClass is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ].MustBeClass != (-1) )
	{
	    DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe[0].type is now: %d.", Me[0].type );
	    if ( Druidmap[Me[0].type].class != Me[0].AllMissions[ mis_num ].MustBeClass ) 
	    {
		DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe[0].class does not match...");
		continue;
	    }
	}
	
	
	//--------------------
	// Continue if the Mission target MustBeType is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ].MustBeType != (-1) )
	{
	    DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe[0].type is now: %d.", Me[0].type );
	    if ( Me[0].type != Me[0].AllMissions[ mis_num ].MustBeType ) 
	    {
		DebugPrintf ( MIS_COMPLETE_DEBUG , "\nMe[0].type does not match...");
		continue;
	    }
	}
	
	
	//--------------------
	// Continue if the Mission target MustReachLevel is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ].MustReachLevel != (-1) )
	{
	    if ( CurLevel->levelnum != Me[0].AllMissions[ mis_num ].MustReachLevel ) 
	    {
		DebugPrintf ( MIS_COMPLETE_DEBUG , "\nLevel number does not match...");
		continue;
	    }
	}
	
	//--------------------
	// Continue if the Mission target MustReachPoint.x is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ].MustReachPoint.x != (-1) )
	{
	    if ( Me[0].pos.x != Me[0].AllMissions[ mis_num ].MustReachPoint.x ) 
	    {
		DebugPrintf ( MIS_COMPLETE_DEBUG , "\nX coordinate does not match...");
		continue;
	    }
	}
	
	//--------------------
	// Continue if the Mission target MustReachPoint.y is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ].MustReachPoint.y != (-1) )
	{
	    if ( Me[0].pos.y != Me[0].AllMissions[ mis_num ].MustReachPoint.y ) 
	    {
		DebugPrintf ( MIS_COMPLETE_DEBUG , "\nY coordinate does not match..."); 
		continue;
	    }
	}
	
	//--------------------
	// Continue if the Mission target MustLiveTime is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ].MustLiveTime != (-1) )
	{
	    if ( Me[0].MissionTimeElapsed < Me[0].AllMissions[ mis_num ].MustLiveTime ) 
	    {
		DebugPrintf ( MIS_COMPLETE_DEBUG , "\nTime Limit not yet reached...");
		continue;
	    }
	}
	
	//--------------------
	// Continue if the Mission target MustBeOne is given but not fullfilled
	//
	if ( Me[0].AllMissions[ mis_num ].MustBeOne != (-1) )
	{
	    if ( Me[0].marker != Me[0].AllMissions[ mis_num ].MustBeOne ) 
	    {
		DebugPrintf ( MIS_COMPLETE_DEBUG , "\nYou're not yet one of the marked ones...");
		continue;
	    }
	}
	
	if ( this_mission_seems_completed )
	{
	    //--------------------
	    // AT THIS POINT WE KNOW THAT ALL OF THE GIVEN TARGETS FOR THIS MISSION ARE FULLFILLED
	    // We therefore mark the mission as completed
	    //
	    GameConfig.Mission_Log_Visible_Time = 0;
	    GameConfig.Mission_Log_Visible = TRUE;
	    Me[0].AllMissions[ mis_num ].MissionIsComplete = TRUE;
	    Mission_Status_Change_Sound ( );
	    for ( ActionNum = 0 ; ActionNum < MAX_MISSION_TRIGGERED_ACTIONS ; ActionNum ++ )
	    {
		ExecuteEvent( Me[0].AllMissions[ mis_num ].ListOfActionsToBeTriggeredAtCompletition[ ActionNum ] , 0 );
	    }
	}
	
    } // for AllMissions
    
}; // void CheckIfMissionIsComplete

/* ----------------------------------------------------------------------
 * This function assigns a new mission to the influencer, which means 
 * that the status of the mission in the mission array is changed and
 * perhaps the mission log activated.
 * ---------------------------------------------------------------------- */
void 
AssignMission( int MissNum )
{
    int j;
    
    Mission_Status_Change_Sound ( );
    GameConfig.Mission_Log_Visible = TRUE;
    GameConfig.Mission_Log_Visible_Time = 0;
    Me [ 0 ] . AllMissions [ MissNum ] . MissionWasAssigned = TRUE;
    
    for ( j = 0 ; j < MAX_MISSION_TRIGGERED_ACTIONS ; j ++ )
    {
	ExecuteEvent( Me [ 0 ] . AllMissions [ MissNum ] . ListOfActionsToBeTriggeredAtAssignment [ j ] , 0 );
    }

    //--------------------
    // We also make visible the very first of the mission diary enties. 
    // That should be sane as upon mission assignment, there should always
    // be some first diary entry, and usually there's only one way a mission
    // can be assigned, so it's safe to do that automatically.
    //
    Me [ 0 ] . AllMissions [ MissNum ] . mission_description_visible [ 0 ] = TRUE;

}; // void AssignMission( int MissNum );

/* ----------------------------------------------------------------------
 * This function reads the mission specifications from the mission file
 * which is assumed to be loaded into memory already.
 * ---------------------------------------------------------------------- */
void 
GetQuestList ( char* QuestListFilename )
{
    char *EndOfMissionTargetPointer;
    char *NextEventPointer;
    int MissionTargetIndex = 0;
    int NumberOfEventsToTriggerAtThisAssignment;
    int NumberOfEventsToTriggerAtThisCompletition;
    int ActionNr;
    char* ActionLabel;
    char* MissionTargetPointer;
    char* fpath;
    char InnerPreservedLetter=0;
    int diary_entry_nr;
    char* next_diary_entry_pointer;
    int number_of_diary_entries;

    
#define MISSION_TARGET_SUBSECTION_START_STRING "** Start of this mission target subsection **"
#define MISSION_TARGET_SUBSECTION_END_STRING "** End of this mission target subsection **"

#define MISSION_TARGET_NAME_INITIALIZER "Mission Name=\""

#define MISSION_AUTOMATICALLY_ASSIGN_STRING "Assign this mission to influencer automatically at start : "
#define MISSION_TARGET_FETCH_ITEM_STRING "Mission target is to fetch item : "
#define MISSION_TARGET_KILL_ALL_STRING "Mission target is to kill all droids : "
#define MISSION_TARGET_KILL_CLASS_STRING "Mission target is to kill class of droids : "
#define MISSION_TARGET_KILL_ONE_STRING "Mission target is to kill droids with marker : "
#define MISSION_TARGET_MUST_CLEAR_FIRST_LEVEL "Mission target is to kill all hostile droids this first level : "
#define MISSION_TARGET_MUST_CLEAR_SECOND_LEVEL "Mission target is to also kill all hostile droids on second level : "
#define MISSION_TARGET_MUST_REACH_LEVEL_STRING "Mission target is to reach level : "
#define MISSION_TARGET_MUST_REACH_POINT_X_STRING "Mission target is to reach X-Pos : "
#define MISSION_TARGET_MUST_REACH_POINT_Y_STRING "Mission target is to reach Y-Pos : "
#define MISSION_TARGET_MUST_LIVE_TIME_STRING "Mission target is to live for how many seconds : "
#define MISSION_TARGET_MUST_BE_CLASS_STRING "Mission target is to become class : "
#define MISSION_TARGET_MUST_BE_TYPE_STRING "Mission target is to become type : "
#define MISSION_TARGET_MUST_BE_ONE_STRING "Mission target is to overtake a droid with marker : "

#define MISSION_ASSIGNMENT_TRIGGERED_ACTION_STRING "On mission assignment immediately trigger action Nr. : "
#define MISSION_COMPLETITION_TRIGGERED_ACTION_STRING "On mission completition immediately trigger action labeled=\""
#define MISSION_DIARY_ENTRY_STRING "Mission diary entry=\""

    //--------------------
    // At first we must load the quest list file given...
    //
    fpath = find_file ( QuestListFilename , MAP_DIR , FALSE );
    MissionTargetPointer = 
	ReadAndMallocAndTerminateFile( fpath , "*** END OF QUEST LIST *** LEAVE THIS TERMINATOR IN HERE ***" ) ;
    
    //--------------------
    // At first we clear out all existing mission entries, so that no 'zombies' remain
    // when the game is restarted and (perhaps less) new missions are loaded.
    //
    for ( MissionTargetIndex = 0 ; MissionTargetIndex < MAX_MISSIONS_IN_GAME ; MissionTargetIndex ++ )
    {
	Me[0].AllMissions[ MissionTargetIndex ].MissionExistsAtAll = FALSE;
	Me[0].AllMissions[ MissionTargetIndex ].MissionIsComplete = FALSE;
	Me[0].AllMissions[ MissionTargetIndex ].MissionWasFailed = FALSE;
	Me[0].AllMissions[ MissionTargetIndex ].MissionWasAssigned = FALSE;
    }
    
    MissionTargetIndex = 0;
    while ( ( MissionTargetPointer = strstr( MissionTargetPointer , MISSION_TARGET_SUBSECTION_START_STRING ) ) != NULL )
    {
	EndOfMissionTargetPointer = LocateStringInData ( MissionTargetPointer , MISSION_TARGET_SUBSECTION_END_STRING ) ;
	
	//--------------------
	// We need to add an inner terminator here, so that the strstr operation
	// below will know where to stop within this subsection.
	//
	InnerPreservedLetter = * EndOfMissionTargetPointer;
	* EndOfMissionTargetPointer = 0 ;
	
	Me[0].AllMissions[ MissionTargetIndex ].MissionExistsAtAll = TRUE;
	Me[0].AllMissions[ MissionTargetIndex ].MissionIsComplete = FALSE;
	Me[0].AllMissions[ MissionTargetIndex ].MissionWasFailed = FALSE;
	Me[0].AllMissions[ MissionTargetIndex ].MissionWasAssigned = FALSE;
	
	// Me[0].AllMissions[ MissionTargetIndex ].MissionName = 
	// ReadAndMallocStringFromData ( MissionTargetPointer , MISSION_TARGET_NAME_INITIALIZER , "\"" ) ;
	strcpy ( Me[0].AllMissions[ MissionTargetIndex ].MissionName , ReadAndMallocStringFromData ( MissionTargetPointer , MISSION_TARGET_NAME_INITIALIZER , "\"" ) ) ;
	
	//--------------------
	// No we read in if this mission should be assigned to the influencer
	// automatically at the game start and without the influencer having to apply
	// for the mission first.
	//
	// The assignment however will take at the end of this mission list initialisation function,
	// cause we need the rest of the mission target data and the events to properly 'assign' the mission.
	// 
	ReadValueFromString( MissionTargetPointer , MISSION_AUTOMATICALLY_ASSIGN_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].AutomaticallyAssignThisMissionAtGameStart , 
			     EndOfMissionTargetPointer );
	
	//--------------------
	// From here on we read the details of the mission target, i.e. what the
	// influencer has to do, so that the mission can be thought of as completed
	//
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_FETCH_ITEM_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].fetch_item , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_KILL_ALL_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].KillAll , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_KILL_CLASS_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].KillClass , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_KILL_ONE_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].KillOne , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_CLEAR_FIRST_LEVEL , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ]. must_clear_first_level , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_CLEAR_SECOND_LEVEL , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ]. must_clear_second_level , EndOfMissionTargetPointer );
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_BE_CLASS_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].MustBeClass , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_BE_TYPE_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].MustBeType , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_BE_ONE_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].MustBeOne , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_REACH_POINT_X_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].MustReachPoint.x , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_REACH_POINT_Y_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].MustReachPoint.y , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_REACH_LEVEL_STRING , "%d" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].MustReachLevel , EndOfMissionTargetPointer );
	
	ReadValueFromString( MissionTargetPointer , MISSION_TARGET_MUST_LIVE_TIME_STRING , "%lf" , 
			     &Me[0].AllMissions[ MissionTargetIndex ].MustLiveTime , EndOfMissionTargetPointer );
	
	//--------------------
	// At this point we have read in the target values.  Now it is time to
	// read in the events, that need to be triggered immediately after the mission has been
	// assigned.
	//
	// But first we initialize all the actions to be triggered with -1
	//
	for ( ActionNr = 0 ; ActionNr < MAX_MISSION_TRIGGERED_ACTIONS; ActionNr ++ )
	{
	    Me[0].AllMissions[ MissionTargetIndex ].ListOfActionsToBeTriggeredAtAssignment[ ActionNr ] = (-1) ;
	}
	
	NextEventPointer = MissionTargetPointer;
	NumberOfEventsToTriggerAtThisAssignment = 0;
	while ( ( NextEventPointer = strstr( NextEventPointer , MISSION_ASSIGNMENT_TRIGGERED_ACTION_STRING ) ) != NULL )
	{
	    
	    ReadValueFromString( NextEventPointer , MISSION_ASSIGNMENT_TRIGGERED_ACTION_STRING , "%d" ,
				 &Me[0].AllMissions[ MissionTargetIndex ].ListOfActionsToBeTriggeredAtAssignment[ NumberOfEventsToTriggerAtThisAssignment ] ,
				 EndOfMissionTargetPointer );
	    
	    
	    NumberOfEventsToTriggerAtThisAssignment ++;
	    NextEventPointer ++;
	}
	DebugPrintf ( 1 , "\nDetected %d events to be triggered at this assignment." , 
		      NumberOfEventsToTriggerAtThisAssignment ) ;
	
	//--------------------
	// Now it is time to read in the events, that need to be triggered immediately after the
	// mission has been completed.
	//
	// But first we initialize all the actions to be triggered with -1
	//
	for ( ActionNr = 0 ; ActionNr < MAX_MISSION_TRIGGERED_ACTIONS; ActionNr ++ )
	{
	    Me[0].AllMissions[ MissionTargetIndex ].ListOfActionsToBeTriggeredAtCompletition[ ActionNr ] = (-1) ;
	}
	NextEventPointer = MissionTargetPointer;
	NumberOfEventsToTriggerAtThisCompletition = 0;
	while ( ( NextEventPointer = strstr( NextEventPointer , MISSION_COMPLETITION_TRIGGERED_ACTION_STRING ) ) != NULL )
	{
	    
	    ActionLabel=
		ReadAndMallocStringFromData ( NextEventPointer , MISSION_COMPLETITION_TRIGGERED_ACTION_STRING , "\"" ) ;
	    
	    Me [ 0 ] . AllMissions [ MissionTargetIndex ] . ListOfActionsToBeTriggeredAtCompletition [ NumberOfEventsToTriggerAtThisCompletition ] = GiveNumberToThisActionLabel ( ActionLabel );
	    
	    NumberOfEventsToTriggerAtThisCompletition ++;
	    NextEventPointer ++;
	}
	DebugPrintf ( 1 , "\nDetected %d events to be triggered at this mission completition." , 
		      NumberOfEventsToTriggerAtThisCompletition );
	
	//--------------------
	// Now it is time to read in the mission diary entries, that might
	// be displayed in the quest browser later.
	//
	for ( diary_entry_nr = 0 ; diary_entry_nr < MAX_MISSION_DESCRIPTION_TEXTS ; diary_entry_nr ++ )
	{
	    mission_diary_texts [ MissionTargetIndex ] [ diary_entry_nr ] = "" ;
	    Me [ 0 ] . AllMissions [ MissionTargetIndex ] . mission_description_visible [ diary_entry_nr ] = FALSE ;
	}
	next_diary_entry_pointer = MissionTargetPointer;
	number_of_diary_entries = 0;
	while ( ( next_diary_entry_pointer = strstr( next_diary_entry_pointer , MISSION_DIARY_ENTRY_STRING ) ) != NULL )
	{    
	    mission_diary_texts [ MissionTargetIndex ] [ number_of_diary_entries ] = 
		ReadAndMallocStringFromData ( next_diary_entry_pointer , MISSION_DIARY_ENTRY_STRING , "\"" ) ;
	    number_of_diary_entries ++;
	    next_diary_entry_pointer ++;
	}
	DebugPrintf ( -4 , "\n%s(): Detected %d mission description entries." , 
		      __FUNCTION__ , number_of_diary_entries );
	
	//--------------------
	// Now we are done with reading in THIS one mission target
	// We need to advance the MissionTargetPointer, so that we avoid doubly
	// reading in this mission OR ONE OF THIS MISSIONS VALUES!!!!
	// 
	// And we need of course to advance the array index for mission targets too...
	//
	MissionTargetPointer = EndOfMissionTargetPointer; // to avoid double entering the same target
	MissionTargetIndex++; // to avoid overwriting the same entry again
	
	//--------------------
	// We restore the termination character we added before, even if that
	// is maybe not really nescessary...
	//
	* EndOfMissionTargetPointer = InnerPreservedLetter ;
	
	
    } // while mission target found...
    

    //--------------------
    // Finally we record the number of mission targets scanned and are done with this function
    DebugPrintf ( 1 , "\nNUMBER OF MISSION TARGETS FOUND: %d.\n" , MissionTargetIndex );
    fflush( stdout );
    
}; // void Get_Mission_Targets( ... )


#undef _mission_c
