/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
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
 * This file contains all functions for the networking client-server
 * interaction, preparation, initialisation and interpretation back into
 * the classical data structures.
 * ---------------------------------------------------------------------- */
/*
 * This file has been checked for remains of german comments in the code
 * I you still find some, please just kill it mercilessly.
 */
#define _network_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

#include "SDL_net.h"

#define FREEDROID_NET_PORT (5674)

#define PERIODIC_MESSAGE_DEBUG 2 
#define SERVER_SEND_DEBUG 2 
#define PLAYER_RECEIVE_COMMAND_DEBUG 2
#define ENFORCE_DEBUG 2

Uint16 port = FREEDROID_NET_PORT;
Uint32 ipaddr;
IPaddress ip,*remoteip; // this is for the server (and for the client?)
// TCPsocket client; // this is for the server
TCPsocket TheServerSocket; // this is for the server only
TCPsocket sock; // this is for the client 
int len;

//--------------------
// Now we define a shortened data type, that is designed to
// be sent from the server to the client, best periodically,
// no matter whether whether there was some change or not.
//
typedef struct
{
  int ItemIndex;

  finepoint pos;
  int type;
  int is_identified;  // is the item identified already?
  // int currently_held_in_hand; // is the item currently held 'in hand' with the mouse cursor?

  int prefix_code;
  int suffix_code;

  int bonus_to_dex;
  int bonus_to_str;
  int bonus_to_vit;
  int bonus_to_mag;
  int bonus_to_life;
  int bonus_to_force;
  int bonus_to_tohit;
  int bonus_to_all_attributes;
  int bonus_to_ac_or_damage; // this is a percentage
  int bonus_to_resist_fire;  // this is a percentage
  int bonus_to_resist_electricity; // this is a percentage
  int bonus_to_resist_force; // this is a percentage

  int ac_bonus;    // how much is ac increased by this item worn
  int damage; // how much damage does this item
  int damage_modifier; // how much additional damage can add to the base damage
  int max_duration;     // the maximum item durability reachable for this item
  int gold_amount; // how much cyberbucks are there, IN CASE OF CYBERBUCKS
  float current_duration; // the currently remaining durability for this item
  grob_point inventory_position;
} item_engram , *Item_Engram ;

item_engram ItemEngram [ MAX_ITEMS_PER_LEVEL ] ;

//--------------------
// Now we define a short message, that is designed to
// be sent from the server to the client, best periodically,
// no matter whether whether there was some change or not.
//
typedef struct
{
  int8_t status;			/* attacking, defense, dead, ... */
  int8_t phase;                 // the current phase of motion
  finepoint speed;		/* the current speed of the druid */
  gps pos;		        /* current position in the whole ship */
  double health;		/* the max. possible energy in the moment */
  double energy;		/* current energy level */
  double mana;                  // current mana level 
  int16_t LastMouse_X;          // mostly for other players:  Where was the last mouseclick...
  int16_t LastMouse_Y;          // mostly for other players:  Where was the last mouseclick...
}
player_engram, *Player_Engram;

player_engram PlayerEngram [ MAX_PLAYERS ] ;

//--------------------
// Now we define a short message, that is designed to
// be sent from the server to the client, to inform the server
// about occuring item drops.
//
typedef struct
{
  int item_slot_code;		// indead of an item pointer, we have to supply a code over the net....
  moderately_finepoint pos;	// current position where to drop the item....
}
item_drop_engram, *Item_Drop_Engram;

item_drop_engram ItemDropEngram;

//--------------------
// Now we define a short message, that is designed to
// be sent from the server to the client, to inform the server
// about occuring item moves.
//
typedef struct
{
  int source_item_slot_code; // which slot to put the item to 
  int dest_item_slot_code; // which slot to place the item 
  grob_point dest_inv_pos; // where in inventory shall this item now appear
}
item_move_engram, *Item_Move_Engram;

item_move_engram ItemMoveEngram;

//--------------------
// Now we define a short message, that is designed to
// be sent from the server to the client, to inform the client
// that it should swap two enemys.
//
typedef struct
{
  int FirstBotIndex;
  int SecondBotIndex;
}
enemy_swap_signal, *Enemy_Swap_Signal;

enemy_swap_signal EnemySwapSignal;

//--------------------
// Now we define a short message, that is designed to
// be sent from the server to the client, best periodically,
// no matter whether whether there was some change or not.
//
typedef struct
{
  int16_t EnemyIndex; // this is an information not included in the original data!!!
                      // but it's required, so that the client knows what to do with
                      // this enemy.

  int8_t type;			// the number of the droid specifications in Druidmap 
  int8_t Status;		// current status like OUT=TERMINATED or not OUT
  gps pos;		        // coordinates of the current position in the level
  finepoint speed;		// current speed  
  int16_t energy;		// current energy of this droid
  int8_t phase;		// current phase of rotation of this droid
  int8_t friendly;

}
enemy_engram, *Enemy_Engram;

enemy_engram EnemyEngram [ MAX_ENEMYS_ON_SHIP ] ;

typedef struct
{
  int16_t BlastIndex;

  gps pos;
  int16_t type;
  double phase;
  // int MessageWasDone; // we won't need to send this over the line...
}
blast_engram, *Blast_Engram ;

blast_engram BlastEngram [ MAXBLASTS ] ;

//--------------------
// Now we define a short message, that is designed to
// be sent from the server to the client, best periodically,
// no matter whether whether there was some change or not.
//
typedef struct
{
  int16_t BulletIndex; // this is an information not included in the original data!!!
                      // but it's required, so that the client knows what to do with
                      // this enemy.

  int8_t type;			// the number of the droid specifications in Druidmap 
  gps pos;		        // coordinates of the current position in the level
  finepoint speed;		// current speed  
  byte phase;
  double angle;

}
bullet_engram, *Bullet_Engram;

bullet_engram BulletEngram [ MAXBULLETS ] ;

//--------------------
// Now we define a command buffer.
// This buffer is intended to contain a full command, meaning a
// command indication area and a command data area.
// Command indication area always comes first.
//
#define COMMAND_BUFFER_MAXLEN 500000
typedef struct
{
  int command_code ;
  int data_chunk_length ;
  char command_data_buffer [ COMMAND_BUFFER_MAXLEN ] ;
}
network_command, *Network_Command;

network_command CommandFromPlayer [ MAX_PLAYERS ];
network_command CommandFromServer [ 1 ];

//--------------------
// Now we define all the known server and client commands
//
enum
  { 

    PLAYER_SERVER_ERROR = 0,

    PLAYER_TELL_ME_YOUR_NAME ,
    SERVER_THIS_IS_MY_NAME ,

    PLAYER_SERVER_TAKE_THIS_TEXT_MESSAGE ,

    PLAYER_ACCEPT_ALL_ME_UPDATES ,
    PLAYER_ACCEPT_PLAYER_ENGRAM ,
    PLAYER_ACCEPT_BULLET_ENGRAM ,
    PLAYER_ACCEPT_BLAST_ENGRAM ,
    PLAYER_ACCEPT_ITEM_ENGRAM ,

    PLAYER_ACCEPT_FULL_ENEMY_ENGRAM ,
    PLAYER_ACCEPT_UPDATE_ENEMY_ENGRAM ,

    PLAYER_DELETE_ALL_YOUR_ENEMYS,
    PLAYER_ACCEPT_THIS_ENEMY_SWAP_SIGNAL ,

    SERVER_ACCEPT_THIS_KEYBOARD_EVENT ,
    SERVER_ACCEPT_THIS_MOUSE_BUTTON_EVENT ,
    SERVER_ACCEPT_THIS_ITEM_DROP ,
    SERVER_ACCEPT_THIS_ITEM_MOVE ,

    NUMBER_OF_KNOWN_COMMANDS

  };

#define MAX_KEYCODES (10000)

typedef struct
{
  TCPsocket ThisPlayersSocketAtTheServer; // this is for the server too
  int command_buffer_fill_status;
  int server_mouse_button;
  int server_mouse_x;
  int server_mouse_y;
  int network_status;
  unsigned char server_keyboard [ MAX_KEYCODES ] ;
}
remote_player, *Remote_Player;

remote_player AllPlayers[ MAX_PLAYERS_AT_MOST ] = {
  { NULL, 0 } ,
  { NULL, 0 } ,
  { NULL, 0 } ,
  { NULL, 0 } ,
  { NULL, 0 } 
};

int server_buffer_fill_status = 0 ;

//--------------------
// This is the set of at most MAX_PLAYERS socket, that the server has.
// Any client socket at the server is added to this set.
// That way we can easily query if there was some data sent from one
// of the clients or not.
//
SDLNet_SocketSet The_Set_Of_All_Client_Sockets;

//--------------------
// This is the one socket, that the client has and that connets him to
// the server.  In order to correctly query if there war activity in this
// socket or not, we put it into it's own set.  This set then has only
// one element, never more, but that's not a problem, is it?
//
SDLNet_SocketSet The_Set_Of_The_One_Server_Socket;

void SendPlayerNameToServer ( void );

/* ----------------------------------------------------------------------
 * This function initializes the networking libraty SDL_net.  Without or
 * before this initialisation, no networking operations should be 
 * attempted.
 * ---------------------------------------------------------------------- */
void 
Init_Network(void)
{
  int PlayerNum; // and index for the clinet sockets.
  int i;

  //--------------------
  // This code was taken from the SDL_Net manual and then
  // slightly modified.
  //
  if ( SDLNet_Init() == (-1) ) 
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET SUBSYSTEM COULD NOT BE INITIALIZED.\n\
\n\
The reason for this as reportet by the SDL Net is as follows:\n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the network code problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) );
      Terminate( ERR );
    }
  else
    {
      DebugPrintf( 0 , "\nSDL Net initialisation.\n" );
    }

  //--------------------
  // Next we make sure the clinet sockets at the server are empty.  This is
  // for the server, so he knows, that there are no active sockets
  // from some clients or something.
  //
  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
      AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer = NULL ; 
      AllPlayers [ PlayerNum ] . network_status = UNCONNECTED ; 
      for ( i = 0 ; i < MAX_KEYCODES ; i ++ )
	AllPlayers [ PlayerNum ] . server_keyboard [ i ] = FALSE ;
    }

  //--------------------
  // We will later want to read out only those client sockets, that really contain
  // some data.  Therefore we must create a socket set, which will allow
  // to query if there is some data there or not.
  //
  The_Set_Of_All_Client_Sockets = SDLNet_AllocSocketSet ( MAX_PLAYERS ) ;

  if( ! The_Set_Of_All_Client_Sockets ) 
    {
      printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
      Terminate ( ERR );
    }
  else
    {
      DebugPrintf ( 0 , "\nSet of client sockets established successfully.\n" );
    }

  //--------------------
  // We will later want to read out the server information from a socket,
  // but only if there is some data of course.  Therefore we must create 
  // a socket set, which will allow us to query if the one socket to the
  // server contains some data or not.
  //
  The_Set_Of_The_One_Server_Socket = SDLNet_AllocSocketSet ( 1 ) ;

  if( ! The_Set_Of_The_One_Server_Socket ) 
    {
      printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
      Terminate ( ERR );
    }
  else
    {
      DebugPrintf ( 0 , "\nSet of the one server sockets established successfully.\n" );
    }


}; // void Init_Network(void)

/* ----------------------------------------------------------------------
 * This function reports what the server thinks that the mouse cursor
 * location was at the last button press/release.
 * If not in server mode, this function returns just the local mouse
 * cursor location.
 * ---------------------------------------------------------------------- */
int
ServerThinksInputAxisX ( int PlayerNum )
{

  if ( ServerMode ) 
    {
      return ( AllPlayers [ PlayerNum ] . server_mouse_x );
    }
  else 
    {
      if ( PlayerNum == 0 )
	return ( input_axis . x ) ;
      else
	return ( FALSE );
    }

}; // int ServerThinksInputAxisX ( int Playernum )

/* ----------------------------------------------------------------------
 * This function reports what the server thinks that the mouse cursor
 * location was at the last button press/release.
 * If not in server mode, this function returns just the local mouse
 * cursor location.
 * ---------------------------------------------------------------------- */
int
ServerThinksInputAxisY ( int PlayerNum )
{

  if ( ServerMode ) 
    {
      return ( AllPlayers [ PlayerNum ] . server_mouse_y );
    }
  else 
    {
      if ( PlayerNum == 0 )
	return ( input_axis . y ) ;
      else
	return ( FALSE );
    }

}; // int ServerThinksInputAxisY ( int Playernum )

/* ----------------------------------------------------------------------
 * This function reports if the server thinks that the cursor right key
 * was pressed or not.  If not in server mode, this function returns just
 * the local keyboard status.
 * ---------------------------------------------------------------------- */
int
ServerThinksAxisIsActive ( int PlayerNum )
{

  if ( ServerMode ) 
    {
      return ( AllPlayers [ PlayerNum ] . server_mouse_button );
    }
  else 
    {
      if ( PlayerNum == 0 )
	return ( axis_is_active ) ;
      else
	return ( FALSE );
    }

}; // int ServerThinksRightPressed ( int Playernum )


/* ----------------------------------------------------------------------
 * This function reports if the server thinks that the cursor left key
 * was pressed or not.  If not in server mode, this function returns just
 * the local keyboard status.
 * ---------------------------------------------------------------------- */
int
ServerThinksShiftWasPressed ( int PlayerNum )
{

  if ( ServerMode ) 
    {
      return ( AllPlayers [ PlayerNum ] . server_keyboard [ SDLK_RSHIFT ] || 
	       AllPlayers [ PlayerNum ] . server_keyboard [ SDLK_LSHIFT ] );
    }
  else 
    {
      if ( PlayerNum == 0 )
	return ( Shift_Is_Pressed ( ) ) ;
      else
	return ( FALSE );
    }

  return FALSE;
}; // int ServerThinksLeftPressed ( int Playernum )


/* ----------------------------------------------------------------------
 * This function reports if the server thinks that the cursor right key
 * was pressed or not.  If not in server mode, this function returns just
 * the local keyboard status.
 * ---------------------------------------------------------------------- */
int
ServerThinksRightPressed ( int PlayerNum )
{

  if ( ServerMode ) 
    {
      return ( AllPlayers [ PlayerNum ] . server_keyboard [ SDLK_RIGHT ] );
    }
  else 
    {
      if ( PlayerNum == 0 )
	return ( RightPressed ( ) ) ;
      else
	return ( FALSE );
    }

}; // int ServerThinksRightPressed ( int Playernum )


/* ----------------------------------------------------------------------
 * This function reports if the server thinks that the cursor left key
 * was pressed or not.  If not in server mode, this function returns just
 * the local keyboard status.
 * ---------------------------------------------------------------------- */
int
ServerThinksLeftPressed ( int PlayerNum )
{

  if ( ServerMode ) 
    {
      return ( AllPlayers [ PlayerNum ] . server_keyboard [ SDLK_LEFT ] );
    }
  else 
    {
      if ( PlayerNum == 0 )
	return ( LeftPressed ( ) ) ;
      else
	return ( FALSE );
    }

}; // int ServerThinksLeftPressed ( int Playernum )


/* ----------------------------------------------------------------------
 * This function reports if the server thinks that the cursor up key
 * was pressed or not.  If not in server mode, this function returns just
 * the local keyboard status.
 * ---------------------------------------------------------------------- */
int
ServerThinksUpPressed ( int PlayerNum )
{

  if ( ServerMode ) 
    {
      return ( AllPlayers [ PlayerNum ] . server_keyboard [ SDLK_UP ] );
    }
  else 
    {
      if ( PlayerNum == 0 )
	return ( UpPressed ( ) ) ;
      else
	return ( FALSE );
    }

}; // int ServerThinksUpPressed ( int Playernum )


/* ----------------------------------------------------------------------
 * This function reports if the server thinks that the cursor down key
 * was pressed or not.  If not in server mode, this function returns just
 * the local keyboard status.
 * ---------------------------------------------------------------------- */
int
ServerThinksDownPressed ( int PlayerNum )
{

  if ( ServerMode ) 
    {
      return ( AllPlayers [ PlayerNum ] . server_keyboard [ SDLK_DOWN ] );
    }
  else 
    {
      if ( PlayerNum == 0 )
	return ( DownPressed ( ) ) ;
      else
	return ( FALSE );
    }

}; // int ServerThinksDownPressed ( int Playernum )

/* ----------------------------------------------------------------------
 * This function reports if the server thinks that the cursor down key
 * was pressed or not.  If not in server mode, this function returns just
 * the local keyboard status.
 * ---------------------------------------------------------------------- */
int
ServerThinksSpacePressed ( int PlayerNum )
{

  if ( ServerMode ) 
    {
      return ( AllPlayers [ PlayerNum ] . server_keyboard [ SDLK_SPACE ] );
    }
  else 
    {
      if ( PlayerNum == 0 )
	return ( SpacePressed ( ) ) ;
      else
	return ( FALSE );
    }

}; // int ServerThinksDownPressed ( int Playernum )

/* ----------------------------------------------------------------------
 * This function checks whether the server thinks some direction key was 
 * pressed or not. 
 * In case of a direction key pressed, FALSE will be returned, otherwise
 * TRUE of course.
 * ---------------------------------------------------------------------- */
int
ServerThinksNoDirectionPressed ( int PlayerNum )
{
  if ( ServerMode ) 
    {
      if ( ( ServerThinksAxisIsActive ( PlayerNum ) && 
	     ( ServerThinksInputAxisX ( PlayerNum ) || 
	       ServerThinksInputAxisY ( PlayerNum ) ) ) ||
	   ServerThinksDownPressed ( PlayerNum ) || 
	   ServerThinksUpPressed ( PlayerNum ) || 
	   ServerThinksLeftPressed ( PlayerNum ) || 
	   ServerThinksRightPressed ( PlayerNum ) )
	return ( FALSE );
      else
	return ( TRUE );
    }
  else 
    {
      if ( PlayerNum == 0 )
	return ( NoDirectionPressed ( ) ) ;
      else
	return ( FALSE );
    }

}; // int NoDirectionPressed( void )

/* ----------------------------------------------------------------------
 * This function creates a copy of the Me structure, that is reduced to
 * those parts, that the client must know.
 * ---------------------------------------------------------------------- */
void 
CopyMeToNetworkMe( int PlayerNum )
{
  int i;
  int WriteIndex=0;

  //--------------------
  // At first we copy this particular player to position 0 within
  // the networking me.
  //
  memcpy ( & NetworkMe[ WriteIndex ] , & Me [ PlayerNum ] , sizeof ( NetworkMe [ WriteIndex ] ) );
  WriteIndex ++;

  //--------------------
  // Now we copy the other existing or nonexisting players into the
  // networking me, but only in file after the zero-entry.
  //
  for ( i = 0 ; i < MAX_PLAYERS ; i ++ )
    {
      if ( PlayerNum != i )
	{
	  memcpy ( & NetworkMe[ WriteIndex ] , & Me [ i ] , sizeof ( NetworkMe [ WriteIndex ] ) );
	  WriteIndex ++;
	}
    }
}; // void CopyMeToNetworkMe( void )

/* ----------------------------------------------------------------------
 * This function prepares a player engram for player number PlayerNum.
 * ---------------------------------------------------------------------- */
void
PreparePlayerEngramForPlayer ( PlayerNum ) 
{
  int WriteIndex = 0 ;
  int i;

  PlayerEngram [ 0 ] . status    = Me [ PlayerNum ] . status ;
  PlayerEngram [ 0 ] . phase     = Me [ PlayerNum ] . phase ;
  PlayerEngram [ 0 ] . speed . x = Me [ PlayerNum ] . speed . x ;
  PlayerEngram [ 0 ] . speed . y = Me [ PlayerNum ] . speed . y ;
  PlayerEngram [ 0 ] . pos . x   = Me [ PlayerNum ] . pos . x ;
  PlayerEngram [ 0 ] . pos . y   = Me [ PlayerNum ] . pos . y ;
  PlayerEngram [ 0 ] . pos . z   = Me [ PlayerNum ] . pos . z ;
  PlayerEngram [ 0 ] . health    = Me [ PlayerNum ] . health ;
  PlayerEngram [ 0 ] . energy    = Me [ PlayerNum ] . energy ;
  PlayerEngram [ 0 ] . mana      = Me [ PlayerNum ] . mana ;

  WriteIndex ++;

  //--------------------
  // Now we copy the other existing or nonexisting players into the
  // networking me, but only in file after the zero-entry.
  //
  for ( i = 0 ; i < MAX_PLAYERS ; i ++ )
    {
      if ( PlayerNum != i )
	{
	  
	  PlayerEngram [ WriteIndex ] . status    = Me [ i ] . status ;
	  PlayerEngram [ WriteIndex ] . phase     = Me [ i ] . phase ;
	  PlayerEngram [ WriteIndex ] . speed . x = Me [ i ] . speed . x ;
	  PlayerEngram [ WriteIndex ] . speed . y = Me [ i ] . speed . y ;
	  PlayerEngram [ WriteIndex ] . pos . x   = Me [ i ] . pos . x ;
	  PlayerEngram [ WriteIndex ] . pos . y   = Me [ i ] . pos . y ;
	  PlayerEngram [ WriteIndex ] . pos . z   = Me [ i ] . pos . z ;
	  PlayerEngram [ WriteIndex ] . health    = Me [ i ] . health ;
	  PlayerEngram [ WriteIndex ] . energy    = Me [ i ] . energy ;
	  PlayerEngram [ WriteIndex ] . mana      = Me [ i ] . mana ;

	  WriteIndex ++;

	}
    }
  
}; // void PreparePlayerEngramForPlayer ( PlayerNum ) 

/* ----------------------------------------------------------------------
 * This function fills data from AllEnemys into the enemy engram.
 * ---------------------------------------------------------------------- */
void
FillDataIntoEnemyEngram ( int WriteIndex , int EnemyIndex )
{

  EnemyEngram [ WriteIndex ] . EnemyIndex = EnemyIndex ;
  EnemyEngram [ WriteIndex ] . type       = AllEnemys [ EnemyIndex ] . type ;
  EnemyEngram [ WriteIndex ] . Status     = AllEnemys [ EnemyIndex ] . Status ;
  EnemyEngram [ WriteIndex ] . speed . x  = AllEnemys [ EnemyIndex ] . speed . x ;
  EnemyEngram [ WriteIndex ] . speed . y  = AllEnemys [ EnemyIndex ] . speed . y ;
  EnemyEngram [ WriteIndex ] . pos . x    = AllEnemys [ EnemyIndex ] . pos . x ;
  EnemyEngram [ WriteIndex ] . pos . y    = AllEnemys [ EnemyIndex ] . pos . y ;
  EnemyEngram [ WriteIndex ] . pos . z    = AllEnemys [ EnemyIndex ] . pos . z ;
  EnemyEngram [ WriteIndex ] . energy     = AllEnemys [ EnemyIndex ] . energy ;
  EnemyEngram [ WriteIndex ] . phase = AllEnemys [ EnemyIndex ] . phase ;
  EnemyEngram [ WriteIndex ] . friendly   = AllEnemys [ EnemyIndex ] . is_friendly ;

}; // void FillDataIntoEnemyEngram ( int WriteIndex , int EnemyIndex )

/* ----------------------------------------------------------------------
 * This function fills data from AllEnemys into the enemy engram.
 * ---------------------------------------------------------------------- */
void
FillDataIntoBulletEngram ( int WriteIndex , int BulletIndex )
{

  BulletEngram [ WriteIndex ] . BulletIndex = BulletIndex ;
  BulletEngram [ WriteIndex ] . type       = AllBullets [ BulletIndex ] . type ;
  BulletEngram [ WriteIndex ] . speed . x  = AllBullets [ BulletIndex ] . speed . x ;
  BulletEngram [ WriteIndex ] . speed . y  = AllBullets [ BulletIndex ] . speed . y ;
  BulletEngram [ WriteIndex ] . pos . x    = AllBullets [ BulletIndex ] . pos . x ;
  BulletEngram [ WriteIndex ] . pos . y    = AllBullets [ BulletIndex ] . pos . y ;
  BulletEngram [ WriteIndex ] . pos . z    = AllBullets [ BulletIndex ] . pos . z ;
  BulletEngram [ WriteIndex ] . phase = AllBullets [ BulletIndex ] . phase ;
  BulletEngram [ WriteIndex ] . angle = AllBullets [ BulletIndex ] . angle ;
  
}; // void FillDataIntoEnemyEngram ( int WriteIndex , int EnemyIndex )

/* ----------------------------------------------------------------------
 * This function fills data from AllBlasts into the blasts engram.
 * ---------------------------------------------------------------------- */
void
FillDataIntoBlastEngram ( int WriteIndex , int BlastIndex )
{

  BlastEngram [ WriteIndex ] . BlastIndex = BlastIndex ;
  BlastEngram [ WriteIndex ] . type       = AllBlasts [ BlastIndex ] . type ;
  BlastEngram [ WriteIndex ] . pos . x    = AllBlasts [ BlastIndex ] . pos . x ;
  BlastEngram [ WriteIndex ] . pos . y    = AllBlasts [ BlastIndex ] . pos . y ;
  BlastEngram [ WriteIndex ] . pos . z    = AllBlasts [ BlastIndex ] . pos . z ;
  BlastEngram [ WriteIndex ] . phase      = AllBlasts [ BlastIndex ] . phase ;
  // BulletEngram [ WriteIndex ] . angle = AllBlasts [ BlastIndex ] . angle ;
  
}; // void FillDataIntoEnemyEngram ( int WriteIndex , int EnemyIndex )

/* ----------------------------------------------------------------------
 * This function fills data from AllBlasts into the blasts engram.
 * ---------------------------------------------------------------------- */
void
FillDataIntoItemEngram ( int WriteIndex , int ItemIndex , int MapLevel )
{

  ItemEngram [ WriteIndex ] . ItemIndex = ItemIndex ;
  ItemEngram [ WriteIndex ] . type       = curShip . AllLevels [ MapLevel ]->ItemList [ ItemIndex ] . type ;
  ItemEngram [ WriteIndex ] . pos . x    = curShip . AllLevels [ MapLevel ]->ItemList [ ItemIndex ] . pos . x ;
  ItemEngram [ WriteIndex ] . pos . y    = curShip . AllLevels [ MapLevel ]->ItemList [ ItemIndex ] . pos . y ;

  DebugPrintf ( 0 , "\nFilling in type %d from item nr. %d." , ItemEngram [ WriteIndex ] . type , ItemIndex );
  

  // DON't SEND THIS INFORMATION OVER THE NET!!
  // ItemEngram [ WriteIndex ] . currently_held_in_hand = curShip . AllLevels [ MapLevel ]->ItemList [ ItemIndex ] . currently_held_in_hand ;


  // More should follow later...
  
}; // void FillDataIntoEnemyEngram ( int WriteIndex , int EnemyIndex )

/* ----------------------------------------------------------------------
 * This function prepares a player engram for player number PlayerNum.
 * ---------------------------------------------------------------------- */
void
PrepareFullEnemysEngram ( void ) 
{
  int i;

  //--------------------
  // Now we copy the existing information about the enemys
  // into the short engram.
  //
  for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i ++ )
    {
      FillDataIntoEnemyEngram ( i , i ) ;
    }
  
}; // void PrepareFullEnemysEngram ( void ) 

/* ----------------------------------------------------------------------
 * This function prepares a the bullets engram for player number PlayerNum.
 * ---------------------------------------------------------------------- */
void
PrepareFullBulletEngramForPlayer ( int PlayerNum ) 
{
  int i;

  //--------------------
  // Now we copy the existing information about the enemys
  // into the short engram.
  //
  for ( i = 0 ; i < MAXBULLETS ; i ++ )
    {
      FillDataIntoBulletEngram ( i , i ) ;
    }
  
}; // void PrepareFullBulletEngramForPlayer ( int PlayerNum ) 

/* ----------------------------------------------------------------------
 * This function prepares a full blasts engram for player number PlayerNum.
 * ---------------------------------------------------------------------- */
void
PrepareFullBlastEngramForPlayer ( int PlayerNum ) 
{
  int i;

  //--------------------
  // Now we copy the existing information about the enemys
  // into the short engram.
  //
  for ( i = 0 ; i < MAXBLASTS ; i ++ )
    {
      FillDataIntoBlastEngram ( i , i ) ;
    }
  
}; // void PrepareFullBlastEngramForPlayer ( int PlayerNum ) 

/* ----------------------------------------------------------------------
 * This function prepares a full blasts engram for player number PlayerNum.
 * ---------------------------------------------------------------------- */
void
PrepareFullItemEngramForPlayer ( int PlayerNum ) 
{
  int i;

  //--------------------
  // Now we copy the existing information about the enemys
  // into the short engram.
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      FillDataIntoItemEngram ( i , i , Me [ PlayerNum ] . pos . z ) ;
    }
  
}; // void PrepareFullBlastEngramForPlayer ( int PlayerNum ) 

/* ----------------------------------------------------------------------
 * This function prepares a player engram for player number PlayerNum.
 * It assumes, that the ItemEngram has been loaded with the nescessary
 * item drop information already.
 * ---------------------------------------------------------------------- */
void
EnforcePlayersItemDrop ( int PlayerNum  ) 
{

  Item SourceItemPointer;

  DebugPrintf ( 0 , "\nvoid EnforcePlayersItemDrop ( int PlayerNum ) : real function call confirmed. " );

  //--------------------
  // Now we find out the item pointer on the server, that the item code
  // in the engram is indicating.
  //
  SourceItemPointer = FindPointerToPositionCode ( ItemDropEngram . item_slot_code , PlayerNum ) ;

  DebugPrintf ( 0 , "\nItem type of item to drop at the server : %d . " , SourceItemPointer -> type ) ;

  DropItemToTheFloor ( SourceItemPointer , ItemDropEngram . pos . x , 
		       ItemDropEngram . pos . y , Me [ PlayerNum ] . pos . z ) ;

  DebugPrintf ( 0 , "\nvoid EnforcePlayersItemDrop ( int PlayerNum ) : end of function reached. " );
  
}; // void EnforcePlayersItemDrop ( int PlayerNum  ) 

/* ----------------------------------------------------------------------
 * This function enforces the item move request from a player.
 * It assumes, that the ItemMoveEngram has been loaded with the nescessary
 * item move information already.
 * ---------------------------------------------------------------------- */
void
EnforcePlayersItemMove ( int PlayerNum  ) 
{
  Item SourceItemPointer;
  Item DestItemPointer;

  DebugPrintf ( 0 , "\nvoid EnforcePlayersItemMove ( int PlayerNum ) : real function call confirmed. " );

  //--------------------
  // Now we find out the item pointer on the server, that the item code
  // in the engram is indicating.
  //
  SourceItemPointer = FindPointerToPositionCode ( ItemMoveEngram . source_item_slot_code , PlayerNum ) ;
  DestItemPointer = FindPointerToPositionCode ( ItemMoveEngram . dest_item_slot_code , PlayerNum ) ;

  DebugPrintf ( 0 , "\nItem type of item to move at the server : %d . " , SourceItemPointer -> type ) ;

  CopyItem ( SourceItemPointer , DestItemPointer , FALSE ) ;
  DeleteItem ( SourceItemPointer ) ;

  if ( ItemMoveEngram . dest_inv_pos . x != ( -1 ) )
    DestItemPointer -> inventory_position . x = ItemMoveEngram . dest_inv_pos . x ;

  if ( ItemMoveEngram . dest_inv_pos . y != ( -1 ) )
    DestItemPointer -> inventory_position . y = ItemMoveEngram . dest_inv_pos . y ;

  // DropItemToTheFloor ( SourceItemPointer , ItemDropEngram . pos . x , ItemDropEngram . pos . y ) ;

  DebugPrintf ( 0 , "\nvoid EnforcePlayersItemDrop ( int PlayerNum ) : end of function reached. " );
  
}; // void EnforcePlayersItemDrop ( int PlayerNum  ) 

/* ----------------------------------------------------------------------
 * This function prepares a player engram for player number PlayerNum.
 * ---------------------------------------------------------------------- */
void
EnforceServersPlayerEngram ( void ) 
{
  int PlayerNum;

  DebugPrintf ( ENFORCE_DEBUG , "\nvoid EnforceServersAllMeUpdate ( void ) : real function call confirmed. " );

  memcpy ( PlayerEngram , CommandFromServer [ 0 ] . command_data_buffer , sizeof ( PlayerEngram ) );

  DebugPrintf ( ENFORCE_DEBUG , "\nPlayerEngram[0].energy : %f." , PlayerEngram [ 0 ] . energy );
  DebugPrintf ( ENFORCE_DEBUG , "\nPlayerEngram[0].status : %d." , PlayerEngram [ 0 ] . status );

  //--------------------
  // Now we copy the players information of the engram to the
  // Me structures.
  //
  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
	  
      Me [ PlayerNum ] . status    = PlayerEngram [ PlayerNum ] . status    ;
      Me [ PlayerNum ] . phase     = PlayerEngram [ PlayerNum ] . phase     ;
      Me [ PlayerNum ] . speed . x = PlayerEngram [ PlayerNum ] . speed . x ;
      Me [ PlayerNum ] . speed . y = PlayerEngram [ PlayerNum ] . speed . y ;
      Me [ PlayerNum ] . pos . x   = PlayerEngram [ PlayerNum ] . pos . x   ;
      Me [ PlayerNum ] . pos . y   = PlayerEngram [ PlayerNum ] . pos . y   ;
      Me [ PlayerNum ] . pos . z   = PlayerEngram [ PlayerNum ] . pos . z   ;
      Me [ PlayerNum ] . health    = PlayerEngram [ PlayerNum ] . health    ;
      Me [ PlayerNum ] . energy    = PlayerEngram [ PlayerNum ] . energy    ;
      Me [ PlayerNum ] . mana      = PlayerEngram [ PlayerNum ] . mana      ;


    }

  DebugPrintf ( ENFORCE_DEBUG , "\nvoid EnforceServersPlayerEngram ( void ) : end of function reached. " );
  
}; // void EnforceServersPlayerEngram ( void ) 

/* ----------------------------------------------------------------------
 * This function prepares a player engram for player number PlayerNum.
 * ---------------------------------------------------------------------- */
void
EnforceServersFullEnemysEngram ( void ) 
{
  int i ;

  DebugPrintf ( ENFORCE_DEBUG , "\nvoid EnforceServersFullEnemysEngram ( void ) : real function call confirmed. " );

  memcpy ( EnemyEngram , CommandFromServer [ 0 ] . command_data_buffer , sizeof ( EnemyEngram ) );

  //--------------------
  // Now we copy the existing information about the enemys
  // into the short engram.
  //
  for ( i = 0 ; i < MAX_ENEMYS_ON_SHIP ; i ++ )
    {
	  
      // AllEnemys [ i ] . EnemyIndex = i ;
      AllEnemys [ i ] . type       = EnemyEngram [ i ] . type ;
      AllEnemys [ i ] . Status     = EnemyEngram [ i ] . Status ;
      AllEnemys [ i ] . speed . x  = EnemyEngram [ i ] . speed . x ;
      AllEnemys [ i ] . speed . y  = EnemyEngram [ i ] . speed . y ;
      AllEnemys [ i ] . pos . x    = EnemyEngram [ i ] . pos . x ;
      AllEnemys [ i ] . pos . y    = EnemyEngram [ i ] . pos . y ;
      AllEnemys [ i ] . pos . z    = EnemyEngram [ i ] . pos . z ;
      AllEnemys [ i ] . energy     = EnemyEngram [ i ] . energy ;
      AllEnemys [ i ] . phase = EnemyEngram [ i ] . phase ;
      AllEnemys [ i ] . is_friendly   = EnemyEngram [ i ] . friendly ;

    }

  DebugPrintf ( ENFORCE_DEBUG , "\nvoid EnforceServersFullEnemysEngram ( void ) : end of function reached. " );
  
}; // void EnforceServersFullEnemysEngram ( void ) 

/* ----------------------------------------------------------------------
 * This function prepares a player engram for player number PlayerNum.
 * ---------------------------------------------------------------------- */
void
EnforceServersUpdateEnemysEngram ( int NumberOfTargets ) 
{
  int i ;
  int WriteIndex;

  DebugPrintf ( ENFORCE_DEBUG , 
		"\nvoid EnforceServersUpdateEnemysEngram ( void ) : real function call confirmed. " );
  DebugPrintf ( ENFORCE_DEBUG , 
		"\nvoid EnforceServersUpdateEnemysEngram ( void ) : %d enemys to update. " , NumberOfTargets );

  memcpy ( EnemyEngram , CommandFromServer [ 0 ] . command_data_buffer , sizeof ( EnemyEngram ) );

  //--------------------
  // Now we copy the existing information about the enemys
  // into the short engram.
  //
  for ( i = 0 ; i < NumberOfTargets ; i ++ )
    {
	  
      WriteIndex = EnemyEngram [ i ] . EnemyIndex ;
	
      AllEnemys [ WriteIndex ] . type       = EnemyEngram [ i ] . type ;
      AllEnemys [ WriteIndex ] . Status     = EnemyEngram [ i ] . Status ;
      AllEnemys [ WriteIndex ] . speed . x  = EnemyEngram [ i ] . speed . x ;
      AllEnemys [ WriteIndex ] . speed . y  = EnemyEngram [ i ] . speed . y ;
      AllEnemys [ WriteIndex ] . pos . x    = EnemyEngram [ i ] . pos . x ;
      AllEnemys [ WriteIndex ] . pos . y    = EnemyEngram [ i ] . pos . y ;
      AllEnemys [ WriteIndex ] . pos . z    = EnemyEngram [ i ] . pos . z ;
      AllEnemys [ WriteIndex ] . energy     = EnemyEngram [ i ] . energy ;
      AllEnemys [ WriteIndex ] . phase = EnemyEngram [ i ] . phase ;
      AllEnemys [ WriteIndex ] . is_friendly   = EnemyEngram [ i ] . friendly ;

    }

  DebugPrintf ( ENFORCE_DEBUG , "\nvoid EnforceServersUpdateEnemysEngram ( void ) : end of function reached. " );
  
}; // void EnforceServersUpdateEnemysEngram ( void ) 

/* ----------------------------------------------------------------------
 * This function prepares a player engram for player number PlayerNum.
 * ---------------------------------------------------------------------- */
void
EnforceServersBulletEngram ( int NumberOfTargets ) 
{
  int i ;
  int WriteIndex;

  DebugPrintf ( ENFORCE_DEBUG , 
		"\nvoid EnforceServersBulletEngram ( void ) : real function call confirmed. " );
  DebugPrintf ( ENFORCE_DEBUG , 
		"\nvoid EnforceServersBulletEngram ( void ) : %d enemys to update. " , NumberOfTargets );

  memcpy ( BulletEngram , CommandFromServer [ 0 ] . command_data_buffer , sizeof ( BulletEngram ) );

  //--------------------
  // Now we copy the existing information about the enemys
  // into the short engram.
  //
  for ( i = 0 ; i < NumberOfTargets ; i ++ )
    {
	  
      WriteIndex = BulletEngram [ i ] . BulletIndex ;
	
      AllBullets [ WriteIndex ] . type       = BulletEngram [ i ] . type ;
      AllBullets [ WriteIndex ] . speed . x  = BulletEngram [ i ] . speed . x ;
      AllBullets [ WriteIndex ] . speed . y  = BulletEngram [ i ] . speed . y ;
      AllBullets [ WriteIndex ] . pos . x    = BulletEngram [ i ] . pos . x ;
      AllBullets [ WriteIndex ] . pos . y    = BulletEngram [ i ] . pos . y ;
      AllBullets [ WriteIndex ] . pos . z    = BulletEngram [ i ] . pos . z ;
      AllBullets [ WriteIndex ] . phase      = BulletEngram [ i ] . phase ;
      AllBullets [ WriteIndex ] . angle      = BulletEngram [ i ] . angle ;

    }

  DebugPrintf ( ENFORCE_DEBUG , 
		"\nvoid EnforceServersUpdateEnemysEngram ( void ) : end of function reached. " );
  
}; // void EnforceServersBulletEngram ( void ) 

/* ----------------------------------------------------------------------
 * This function enforces a blast engram with NumberOfTargets blasts.
 * ---------------------------------------------------------------------- */
void
EnforceServersBlastEngram ( int NumberOfTargets ) 
{
  int i ;
  int WriteIndex;

  DebugPrintf ( ENFORCE_DEBUG , "\nvoid EnforceServersBlastEngram ( void ) : real function call confirmed. " );
  DebugPrintf ( ENFORCE_DEBUG , "\nvoid EnforceServersBlastEngram ( void ) : %d blasts to update. " , NumberOfTargets );

  memcpy ( BlastEngram , CommandFromServer [ 0 ] . command_data_buffer , sizeof ( BlastEngram ) );

  //--------------------
  // Now we copy the existing information about the enemys
  // into the short engram.
  //
  for ( i = 0 ; i < NumberOfTargets ; i ++ )
    {
	  
      WriteIndex = BlastEngram [ i ] . BlastIndex ;
	
      AllBlasts [ WriteIndex ] . type       = BlastEngram [ i ] . type ;
      AllBlasts [ WriteIndex ] . pos . x    = BlastEngram [ i ] . pos . x ;
      AllBlasts [ WriteIndex ] . pos . y    = BlastEngram [ i ] . pos . y ;
      AllBlasts [ WriteIndex ] . pos . z    = BlastEngram [ i ] . pos . z ;
      AllBlasts [ WriteIndex ] . phase      = BlastEngram [ i ] . phase ;

    }

  DebugPrintf ( ENFORCE_DEBUG , 
		"\nvoid EnforceServersUpdateEnemysEngram ( void ) : end of function reached. " );
  
}; // void EnforceServersBlastEngram ( void ) 

/* ----------------------------------------------------------------------
 * This function enforces a item engram with NumberOfTargets items.
 * ---------------------------------------------------------------------- */
void
EnforceServersItemEngram ( int NumberOfTargets ) 
{
  int i ;
  int WriteIndex;

  DebugPrintf ( ENFORCE_DEBUG , "\nvoid EnforceServersItemEngram ( void ) : real function call confirmed. " );
  DebugPrintf ( ENFORCE_DEBUG , "\nvoid EnforceServersItemEngram ( void ) : %d targets to update. " , NumberOfTargets );

  memcpy ( ItemEngram , CommandFromServer [ 0 ] . command_data_buffer , sizeof ( ItemEngram ) );

  //--------------------
  // Now we copy the existing information about the enemys
  // into the short engram.
  //
  for ( i = 0 ; i < NumberOfTargets ; i ++ )
    {
	  
      WriteIndex = ItemEngram [ i ] . ItemIndex ;
	
      curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ItemList [ WriteIndex ] . type    = ItemEngram [ i ] . type ;
      curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ItemList [ WriteIndex ] . pos . x = ItemEngram [ i ] . pos . x ;
      curShip . AllLevels [ Me [ 0 ] . pos . z ] -> ItemList [ WriteIndex ] . pos . y = ItemEngram [ i ] . pos . y ;

      DebugPrintf ( 0 , "\nUpdating Item Nr. %d to new type %d." , WriteIndex , ItemEngram [ i ] . type ) ;
      
    }

  DebugPrintf ( ENFORCE_DEBUG , "\nvoid EnforceServersUpdateEnemysEngram ( void ) : end of function reached. " );
  
}; // void EnforceServersItemsEngram ( void ) 

/* ----------------------------------------------------------------------
 * This function creates a copy of the Me structure, that is reduced to
 * those parts, that the client must know.
 * ---------------------------------------------------------------------- */
void 
CopyNetworkMeToMe( void )
{
  int PlayerNum;

  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
      memcpy ( & Me[ PlayerNum ] , & NetworkMe [ PlayerNum ] , sizeof ( NetworkMe [ PlayerNum ] ) );
    }

}; // void CopyMeToNetworkMe( void )

/* ----------------------------------------------------------------------
 * This function creates a copy of the AllEnemys array, that is reduced to
 * those parts, that the client must know.
 * ---------------------------------------------------------------------- */
void 
CopyAllEnemysToNetworkAllEnemys ( void )
{
  int EnemyNum;

  for ( EnemyNum = 0 ; EnemyNum < MAX_ENEMYS_ON_SHIP ; EnemyNum ++ )
    {
      memcpy ( & NetworkAllEnemys[ EnemyNum ] , & AllEnemys [ EnemyNum ] , sizeof ( NetworkAllEnemys [ EnemyNum ] ) );
    }

}; // void CopyAllEnemysToNetworkAllEnemys ( void )

/* ----------------------------------------------------------------------
 * This function creates a copy of the Me structure, that is reduced to
 * those parts, that the client must know.
 * ---------------------------------------------------------------------- */
void 
CopyNetworkAllEnemysToAllEnemys ( void )
{
  int EnemyNum;

  for ( EnemyNum = 0 ; EnemyNum <  MAX_ENEMYS_ON_SHIP ; EnemyNum ++ )
    {
      memcpy ( & AllEnemys[ EnemyNum ] , & NetworkAllEnemys [ EnemyNum ] , sizeof ( NetworkAllEnemys [ EnemyNum ] ) );
    }
}; // void CopyNetworkAllEnemysToAllEnemys ( void )

/* ----------------------------------------------------------------------
 * This function sends a text message to a client in command form.
 * ---------------------------------------------------------------------- */
void
SendTextMessageToClient ( int PlayerNum , char* message )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( 0 , "\nSending text message to client in command form.  \nMessage is : %s\n" , message ) ;
  len = strlen ( message ) + 1 ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long message to server... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_SERVER_TAKE_THIS_TEXT_MESSAGE ;
  LocalCommandBuffer . data_chunk_length = len ;
  strcpy ( LocalCommandBuffer . command_data_buffer , message );

  

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( 0 , "\nSending TCP message returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A TEXT MESSAGE TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendTextMessageToClient ( int PlayerNum , char* message )


/* ----------------------------------------------------------------------
 * This function sends a text message to a client in command form.
 * ---------------------------------------------------------------------- */
void
SendFullMeUpdateToClient ( int PlayerNum )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending full me update to client in command form.\n" ) ;
  len = sizeof ( NetworkMe ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long full NetworkMe update to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // We prepare the NetworkMe we want to send...
  //
  CopyMeToNetworkMe ( PlayerNum ) ;

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_ACCEPT_ALL_ME_UPDATES ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , NetworkMe , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( 0 , "\nSending Full Me Update returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A FULL ME UPDATE TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendFullMeUpdateToClient ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function sends a text message to a client in command form.
 * ---------------------------------------------------------------------- */
void
SendFullPlayerEngramToClient ( int PlayerNum )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending full player engram to client in command form.\n" ) ;
  len = sizeof ( PlayerEngram ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long full NetworkMe update to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // We prepare the NetworkMe we want to send...
  //
  PreparePlayerEngramForPlayer ( PlayerNum ) ;

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_ACCEPT_PLAYER_ENGRAM ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , PlayerEngram , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending Full PlayerEngram returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A FULL PLAYER ENGRAM TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendFullPlayerEngramToClient ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function sends a signal to swap two enemys to the client
 * ---------------------------------------------------------------------- */
void
SendEnemySwapSignalToClient ( int PlayerNum , int First , int Second )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending swap enemy signal to client in command form.\n" ) ;
  len = sizeof ( EnemySwapSignal ) ; // the amount of bytes in the data buffer


  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long full EnemySwapSignal to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // We prepare the signal we want to send...
  //
  EnemySwapSignal.FirstBotIndex = First;
  EnemySwapSignal.SecondBotIndex = Second;

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_ACCEPT_THIS_ENEMY_SWAP_SIGNAL ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , & ( EnemySwapSignal ) , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending Enemy swap signal returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND AN ENEMY SWAP SIGNAL TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendEnemySwapSignalToClient ( int PlayerNum , int First , int Second )

/* ----------------------------------------------------------------------
 * This function sends a text message to a client in command form.
 * ---------------------------------------------------------------------- */
void
SendFullBulletEngramToClient ( int PlayerNum )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending full bullet engram to client in command form.\n" ) ;
  len = sizeof ( BulletEngram ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long full bullet engram update to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // We prepare the network information we want to send...
  //
  PrepareFullBulletEngramForPlayer ( PlayerNum ) ;

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_ACCEPT_BULLET_ENGRAM ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , BulletEngram , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending Full BulletEngram returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A FULL BULLET ENGRAM TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendFullBulletEngramToClient ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function sends a full blast engram to a client in command form.
 * ---------------------------------------------------------------------- */
void
SendFullBlastEngramToClient ( int PlayerNum )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending full blast engram to client in command form.\n" ) ;
  len = sizeof ( BlastEngram ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long full blast engram to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // We prepare the network information we want to send...
  //
  PrepareFullBlastEngramForPlayer ( PlayerNum ) ;

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_ACCEPT_BLAST_ENGRAM ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , BlastEngram , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending full blast engram returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A FULL BLAST ENGRAM TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendFullBlastEngramToClient ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function sends a full item engram to a client in command form.
 * ---------------------------------------------------------------------- */
void
SendFullItemEngramToClient ( int PlayerNum )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending full item engram to client in command form.\n" ) ;
  len = sizeof ( ItemEngram ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long item engram to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // We prepare the network information we want to send...
  //
  PrepareFullItemEngramForPlayer ( PlayerNum ) ;

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_ACCEPT_ITEM_ENGRAM ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , ItemEngram , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending full item engram returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A FULL ITEM ENGRAM TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendFullItemEngramToClient ( int PlayerNum )

/* ----------------------------------------------------------------------
 * Since memcmp may not to work correctly with structs such as items,
 * I write my own comparison function here as recommended in the GNU C
 * lib manual.
 * ---------------------------------------------------------------------- */
int 
ItemHasChanged ( Item OneItem , Item SecondItem )
{
  if ( ( OneItem->type != SecondItem->type ) ||
       ( OneItem->pos.x != SecondItem->pos.x ) ||
       ( OneItem->pos.y != SecondItem->pos.y ) ||
       ( OneItem->is_identified != SecondItem->is_identified ) ||
       ( OneItem->max_duration != SecondItem->max_duration ) ||
       ( OneItem->current_duration != SecondItem->current_duration ) ||
       ( OneItem->prefix_code != SecondItem->prefix_code ) ||
       ( OneItem->suffix_code != SecondItem->suffix_code ) )

    return TRUE;
  return FALSE;

}; // int ItemHasChanged ( Item OneItem , Item SecondItem )

/* ----------------------------------------------------------------------
 *
 * This function detects what changes have occured in the item list of
 * this current players level and assembles appropriate item update
 * information in the engram.
 *
 * It will also return the number of items that need updating, cause this
 * information will be needed in the calling function.
 *
 * ---------------------------------------------------------------------- */
int
AssembleItemUpdateInformation ( int PlayerNum )
{
  int UpdateCounter = 0 ;
  Level UpdateLevel = curShip . AllLevels [ Me [ PlayerNum ] . pos . z ] ;
  int i;
  
  //--------------------
  // We proceed through all the items of this level and compare each item
  // entry with the entry from the previous frame.
  //
  for ( i = 0 ; i < MAX_ITEMS_PER_LEVEL ; i ++ )
    {
      //--------------------
      // If we find a match, we add this item to the item engram, that
      // will later be sent over the network and of course we count the
      // number of added entries for later use.
      //
      // if ( memcmp ( & ( UpdateLevel -> ItemList [ i ] ) ,
      // & ( UpdateLevel -> OldItemList [ i ] ) ,
      // sizeof ( UpdateLevel -> ItemList [ i ] ) ) ) ;
      if ( ItemHasChanged ( & ( UpdateLevel -> ItemList [ i ] ) ,
			    & ( UpdateLevel -> OldItemList [ i ] ) ) )
	{

	  FillDataIntoItemEngram ( UpdateCounter , i , Me [ PlayerNum ] . pos . z ) ;
	  UpdateCounter ++ ;

	}
    }

  return UpdateCounter ;

}; // void AssembleItemUpdateInformation ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function sends a full item engram to a client in command form.
 * ---------------------------------------------------------------------- */
void
SendItemUpdateToClient ( int PlayerNum )
{
  int CommunicationResult;
  int len;
  int ChangedItems;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending item update engram to client in command form.\n" ) ;

  ChangedItems = AssembleItemUpdateInformation ( PlayerNum );

  if ( ChangedItems == 0 )
    {
      DebugPrintf ( SERVER_SEND_DEBUG , "\nComparison of the items on the Level of Player %d showed no changes.  Sending suppressed." ,
		    PlayerNum );
      return;
    }
  else
    {
      DebugPrintf ( 0 , "\nComparison of the items on the Level of Player %d showed %d changes." ,
		    PlayerNum , ChangedItems ) ;
    }

  len = ChangedItems * sizeof ( ItemEngram [ 0 ] ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long item update engram to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // We prepare the network information we want to send...
  //
  // PrepareFullItemEngramForPlayer ( PlayerNum ) ;

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_ACCEPT_ITEM_ENGRAM ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , ItemEngram , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending item update engram returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND AN ITEM UPDATE TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendFullItemEngramToClient ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function sends a text message to a client in command form.
 * ---------------------------------------------------------------------- */
void
SendFullEnemyEngramToClient ( int PlayerNum )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending full player engram to client in command form.\n" ) ;
  len = sizeof ( EnemyEngram ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long full NetworkMe update to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // We prepare the network information we want to send...
  //
  PrepareFullEnemysEngram (  ) ;

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_ACCEPT_FULL_ENEMY_ENGRAM ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , EnemyEngram , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending Full PlayerEngram returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A FULL PLAYER ENGRAM TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendFullPlayerEngramToClient ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function sends an update engram about the enemys to a client in command form.
 * ---------------------------------------------------------------------- */
void
SendBulletUpdateEngramToClient ( int PlayerNum )
{
  int CommunicationResult;
  int len;
  int EnemyIndex;
  int WriteIndex = 0 ;
  network_command LocalCommandBuffer;

  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending bullet update engram to client in command form.\n" ) ;

  return;

  for ( EnemyIndex = 0 ; EnemyIndex < MAX_ENEMYS_ON_SHIP ; EnemyIndex ++ )
    {
      if ( AllEnemys [ EnemyIndex ] . pos . z == Me [ PlayerNum ] . pos . z )
	{
	  //--------------------
	  // We also ignore the deactivated enemys on level 0
	  //
	  if ( ( AllEnemys [ EnemyIndex ] . pos . z == 0 ) && ( AllEnemys [ EnemyIndex ] . Status == OUT ) )
	    continue;

	  FillDataIntoEnemyEngram ( WriteIndex , EnemyIndex );
	  WriteIndex ++ ;
	}
    };


  len = WriteIndex * sizeof ( EnemyEngram[0] ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long full NetworkMe update to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_ACCEPT_UPDATE_ENEMY_ENGRAM ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , EnemyEngram , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending enemy update engram returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A FULL PLAYER ENGRAM TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendBulletUpdateEngramToClient ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function sends an update engram about the enemys to a client in command form.
 * ---------------------------------------------------------------------- */
void
SendEnemyUpdateEngramToClient ( int PlayerNum )
{
  int CommunicationResult;
  int len;
  int EnemyIndex;
  int WriteIndex = 0 ;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending enemy update engram to client in command form.\n" ) ;

  for ( EnemyIndex = 0 ; EnemyIndex < MAX_ENEMYS_ON_SHIP ; EnemyIndex ++ )
    {
      if ( AllEnemys [ EnemyIndex ] . pos . z == Me [ PlayerNum ] . pos . z )
	{
	  //--------------------
	  // We also ignore the deactivated enemys on level 0
	  //
	  if ( ( AllEnemys [ EnemyIndex ] . pos . z == 0 ) && ( AllEnemys [ EnemyIndex ] . Status == OUT ) )
	    continue;

	  FillDataIntoEnemyEngram ( WriteIndex , EnemyIndex );
	  WriteIndex ++ ;
	}
    };


  len = WriteIndex * sizeof ( EnemyEngram[0] ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long full NetworkMe update to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_ACCEPT_UPDATE_ENEMY_ENGRAM ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , EnemyEngram , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( SERVER_SEND_DEBUG , "\nSending enemy update engram returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A FULL PLAYER ENGRAM TO THE CLIENT SUCCESSFULLY\n\
in the function void SendTextMessageToClient ( int PlayerNum , char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendEnemyUpdateEngramToClient ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function sends a request for the client to erase all enemys in
 * the enemy array.
 * ---------------------------------------------------------------------- */
void
SendEnemyDeletionRequestToClient ( int PlayerNum )
{
  int CommunicationResult;
  int len;
  char dummy_byte;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( 0 , "\nSending full player engram to client in command form.\n" ) ;
  len = sizeof ( dummy_byte ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long detetion request to client... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_DELETE_ALL_YOUR_ENEMYS ;
  LocalCommandBuffer . data_chunk_length = len ;
  // memcpy ( LocalCommandBuffer . command_data_buffer , PlayerEngram , len );

  CommunicationResult = SDLNet_TCP_Send ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( 0 , "\nSending Delete All Enemys Request to Client returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A REQUEST FOR DELETION OF ALL ENEMYS TO THE CLIENT SUCCESSFULLY\n\
in the function void SendEnemyDeletionRequestToClient ( int PlayerNum ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendEnemyDeletionRequestToClient ( int PlayerNum )

/* ----------------------------------------------------------------------
 * When the server sends us update about the complete (well, with the
 * networking reductions of course) Me array, this new information must
 * be enforced, i.e. written into the really used Me structures.
 * This is what this function should do.  It assumes, that the data
 * has been completely received and the command correctly interpeted
 * by the calling function before.
 * ---------------------------------------------------------------------- */
void 
EnforceServersAllMeUpdate ( void )
{

  DebugPrintf ( 0 , "\nvoid EnforceServersAllMeUpdate ( void ) : real function call confirmed. " );

  memcpy ( NetworkMe , CommandFromServer [ 0 ] . command_data_buffer , MAX_PLAYERS * sizeof ( NetworkMe[0] ) );

  DebugPrintf ( 0 , "\nNetworkMe[0].energy : %f." , NetworkMe[0].energy );
  DebugPrintf ( 0 , "\nNetworkMe[0].status : %d." , NetworkMe[0].status );

  CopyNetworkMeToMe ( ) ;

  DebugPrintf ( 0 , "\nvoid EnforceServersAllMeUpdate ( void ) : end of function reached. " );

}; // void EnforceServersAllMeUpdate ( void )


/* ----------------------------------------------------------------------
 * This function should execute the command the server has sent us.  This
 * command is assumed to be stored in the appropriate command structure
 * already.
 * ---------------------------------------------------------------------- */
void
ExecuteServerCommand ( void )
{
  int TransmittedEnemys;
  int TransmittedBullets;
  int TransmittedBlasts;
  int TransmittedItems;

  DebugPrintf ( 2 , "\nExecuteServerCommand ( void ): real function call confirmed." ) ;

  switch ( CommandFromServer [ 0 ] . command_code )
    {
    case PLAYER_SERVER_ERROR:
      DebugPrintf ( 0 , "\nPLAYER_SERVER_ERROR command received...Terminting..." );
      Terminate ( ERR );
      break;
      
    case SERVER_THIS_IS_MY_NAME:
      //--------------------
      // This command is not allowed to be given to a Player
      //
      DebugPrintf ( 0 , "\nSERVER_THIS_IS_MY_NAME command received AS PLAYER!!!... Terminating..." );
      Terminate ( ERR );
      break;

    case PLAYER_SERVER_TAKE_THIS_TEXT_MESSAGE:
      DebugPrintf ( PLAYER_RECEIVE_COMMAND_DEBUG , 
		    "\nPLAYER_SERVER_TAKE_THIS_IS_TEXT_MESSAGE command received... AS CLIENT." );
      DebugPrintf ( 0 , "\nMessage transmitted : %s." , & CommandFromServer [ 0 ] . command_data_buffer );
      // Terminate ( ERR );
      break;
  
    case PLAYER_ACCEPT_ALL_ME_UPDATES:
      DebugPrintf ( PLAYER_RECEIVE_COMMAND_DEBUG , "\nPLAYER_ACCEPT_ALL_ME_UPDATES command received... " );
      EnforceServersAllMeUpdate ( );
      // Terminate ( ERR );
      break;

    case PLAYER_ACCEPT_PLAYER_ENGRAM:
      DebugPrintf ( PLAYER_RECEIVE_COMMAND_DEBUG , "\nPLAYER_ACCEPT_PLAYER_ENGRAM command received... " );
      EnforceServersPlayerEngram ( ) ;
      // Terminate ( ERR );
      break;

    case PLAYER_ACCEPT_FULL_ENEMY_ENGRAM:
      DebugPrintf ( PLAYER_RECEIVE_COMMAND_DEBUG , "\nPLAYER_ACCEPT_FULL_ENEMY_ENGRAM command received... " );
      EnforceServersFullEnemysEngram ( ) ;
      // Terminate ( ERR );
      break;

    case PLAYER_ACCEPT_UPDATE_ENEMY_ENGRAM:
      DebugPrintf ( PLAYER_RECEIVE_COMMAND_DEBUG , "\nPLAYER_ACCEPT_UPDATE_ENEMY_ENGRAM command received... " );
      TransmittedEnemys = CommandFromServer [ 0 ] . data_chunk_length / sizeof ( EnemyEngram [ 0 ] ) ;
      EnforceServersUpdateEnemysEngram ( TransmittedEnemys ) ;
      // Terminate ( ERR );
      break;

    case PLAYER_ACCEPT_BULLET_ENGRAM:
      DebugPrintf ( PLAYER_RECEIVE_COMMAND_DEBUG , "\nPLAYER_ACCEPT_BULLET_ENGRAM command received... " );
      TransmittedBullets = CommandFromServer [ 0 ] . data_chunk_length / sizeof ( BulletEngram [ 0 ] ) ;
      EnforceServersBulletEngram ( TransmittedBullets ) ;
      // Terminate ( ERR );
      break;

    case PLAYER_ACCEPT_BLAST_ENGRAM:
      DebugPrintf ( PLAYER_RECEIVE_COMMAND_DEBUG , "\nPLAYER_ACCEPT_BLAST_ENGRAM command received... " );
      TransmittedBlasts = CommandFromServer [ 0 ] . data_chunk_length / sizeof ( BlastEngram [ 0 ] ) ;
      EnforceServersBlastEngram ( TransmittedBlasts ) ;
      // Terminate ( ERR );
      break;

    case PLAYER_ACCEPT_ITEM_ENGRAM:
      DebugPrintf ( PLAYER_RECEIVE_COMMAND_DEBUG , "\nPLAYER_ACCEPT_ITEM_ENGRAM command received... " );
      TransmittedItems = CommandFromServer [ 0 ] . data_chunk_length / sizeof ( ItemEngram [ 0 ] ) ;
      EnforceServersItemEngram ( TransmittedItems ) ;
      // Terminate ( ERR );
      break;

    case PLAYER_DELETE_ALL_YOUR_ENEMYS:
      DebugPrintf ( PLAYER_RECEIVE_COMMAND_DEBUG , "\nPLAYER_DELETE_ALL_YOUR_ENEMYS command received... " );
      ClearEnemys ( ) ;
      break;

    case PLAYER_ACCEPT_THIS_ENEMY_SWAP_SIGNAL:
      DebugPrintf ( PLAYER_RECEIVE_COMMAND_DEBUG , "\nPLAYER_ACCEPT_THIS_ENEMY_SWAP_SIGNAL command received... " );
      memcpy ( & ( EnemySwapSignal ) , CommandFromServer [ 0 ] . command_data_buffer , sizeof ( EnemySwapSignal ) );
      SwapEnemys ( EnemySwapSignal.FirstBotIndex , EnemySwapSignal.SecondBotIndex );
      break;

    default:
      DebugPrintf ( 0 , "\nUNKNOWN COMMAND CODE ERROR!!!! Received as PLAYER!!!  Terminating..." );
      Terminate ( ERR );
      break;

    };

}; // void ExecuteServerCommand ( void )


/* ----------------------------------------------------------------------
 * This function reads the signals from the client socket of player
 * PlayerNum at the server.  Depending on the current fill and 
 * command completition status of this players command buffer, be data
 * will be read into the buffer at the right location and also the right
 * amount of data will be read.
 * ---------------------------------------------------------------------- */
void
Read_Command_Buffer_From_Server ( void )
{
  int len;

  //--------------------
  // Depending on the current fill status of the command buffer, we read in the 
  // command code and command buffer length or not.
  //
  if ( ! server_buffer_fill_status )
    {
      // --------------------
      // Here we clearly expect a completely new command sequence to begin.
      //
      DebugPrintf ( 2 , "\nvoid Read_Command_Buffer_From_Server ( void ): New Command expected." );

      //--------------------
      // At first we read in the command code.
      //
      len = SDLNet_TCP_Recv ( sock , 
			      & ( CommandFromServer [ 0 ] . command_code ) , 
			      sizeof ( CommandFromServer [ 0 ] . command_code ) ) ;
      if ( len == 0 ) 
	{
	  DebugPrintf ( 0 , "\n\nReading command code failed.... Terminating... " );
	  Terminate ( ERR );
	}
      if ( len < sizeof ( CommandFromServer [ 0 ] . command_code ) ) 
	{
	  DebugPrintf ( 0 , "\n\nReading command:  command code did not completely come over... " );
	  Terminate ( ERR );
	}

      //--------------------
      // Now we read in the command data chunk length to expect.
      //
      len = SDLNet_TCP_Recv ( sock , 
			      & ( CommandFromServer [ 0 ] . data_chunk_length ) , 
			      sizeof ( CommandFromServer [ 0 ] . data_chunk_length ) ) ;
      
      if ( len == 0 ) 
	{
	  DebugPrintf ( 0 , "\n\nReading command data buffer length failed.... Terminating... " );
	  Terminate ( ERR );
	}
      if ( len < sizeof ( CommandFromServer [ 0 ] . data_chunk_length ) ) 
	{
	  DebugPrintf ( 0 , "\n\nReading command:  data buffer length info did not completely come over... " );
	  Terminate ( ERR );
	}
    }

  //--------------------
  // At this point we know, that either an old command code and chunk exists
  // and must be appended or a new command chunk is being read
  //
  len = SDLNet_TCP_Recv ( sock , 
			  & ( CommandFromServer [ 0 ] . command_data_buffer [ server_buffer_fill_status ] ) , 
			  CommandFromServer [ 0 ] . data_chunk_length - server_buffer_fill_status ) ;

  if ( len == 0 ) 
    {
      DebugPrintf ( 0 , "\n\nReading command data buffer itself failed.... Terminating... " );
      Terminate ( ERR );
    }

  if ( ( len + server_buffer_fill_status ) < ( CommandFromServer [ 0 ] . data_chunk_length ) ) 
    {
      DebugPrintf ( 0 , "\n\nWARNING!! Reading command:  data (still) did not come completely over... " );
      DebugPrintf ( 0 , "\nAppending to existing data and continuing..." );

      //--------------------
      // So we advance our fill pointer and wait for the next function call...
      //
      server_buffer_fill_status += len;

      // Terminate ( ERR );

    }
  else
    {
      DebugPrintf ( 2 , "\nReading command:  (Finally) Data did come completely over... " );

      //--------------------
      // Now at this point we know, that we have received a complete and full
      // command from the client.  That means we can really execute the command
      // that has just been filled into the command buffer.
      //
      ExecuteServerCommand (  );
      
      server_buffer_fill_status = 0 ;

    }

      
}; // void Read_Command_Buffer_From_Server ( void )

/* ----------------------------------------------------------------------
 * This function should establish a connection to a known server, so that
 * the game can run as a client now.
 * ---------------------------------------------------------------------- */
void
ConnectToFreedroidServer ( void )
{
  int UsedSockets;
  Uint32 ServerIpAddress;

  //--------------------
  // If nothing at all was typed in for the server name, then we will try
  // to use the local host.
  //
  DebugPrintf ( 0 , "\n Servername received : %s." , ServerName ) ;
  if ( strlen ( ServerName ) == 0 ) strcpy ( ServerName , "localhost" ) ;

  //--------------------
  // Now we try to resolve the given server name into an IPaddress type
  //
  if( SDLNet_ResolveHost( &ip , ServerName , port ) == ( -1 ) )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET SUBSYSTEM COULD NOT RESOLVE THE HOSTNAME %s.\n\
\n\
The reason for this as reportet by the SDL Net is as follows:\n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the network code problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , ServerName , SDLNet_GetError ( ) );

      Terminate ( ERR );
    }
  else
    {
      DebugPrintf( 0 , "\n--------------------\nSDLNet_ResolveHost was successful...\n--------------------\n " );
    }
  
  // open the server socket
  ServerIpAddress = SDL_SwapBE32 ( ip.host ) ;
  sock = SDLNet_TCP_Open( &ip ) ;
  if( ! sock )
    {
      fprintf( stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET SUBSYSTEM COULD NOT OPEN A TCP CONNECTION TO THE SERVER\n\
WITH THE RESOLVED IP ADDRESS : %d.%d.%d.%d port %hu\n\
The reason for this as reportet by the SDL Net is as follows:\n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the network code problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , 
	       ServerIpAddress >> 24 ,
	       ( ServerIpAddress >> 16) &0xff ,
	       ( ServerIpAddress >> 8 ) &0xff ,
	       ServerIpAddress &0xff ,
	       ip.port,
	       SDLNet_GetError ( ) );
      // printf("Accepted a connection from %d.%d.%d.%d port %hu\n",
      // DebugPrintf( 0 , "\n--------------------\nERROR!!! SDLNet_TCP_Open: %s\n--------------------\n" , 
      // SDLNet_GetError ( ) ) ;
      Terminate ( ERR );
    }
  else
    {
      DebugPrintf( 0 , "\n--------------------\nSDLNet_TCP_Open was successful...\n" );
      DebugPrintf( 0 , "Connected to Server %d.%d.%d.%d at port %hu\n--------------------\n"  , 
		   ServerIpAddress >> 24 ,
		   ( ServerIpAddress >> 16) &0xff ,
		   ( ServerIpAddress >> 8 ) &0xff ,
		   ServerIpAddress &0xff ,
		   ip.port,
		   SDLNet_GetError ( ) );
    }

  //--------------------
  // Now that we have successfully opened a socket for information
  // exchange with the server, we add this socket to a socket set of
  // one socket, so that we can later easily query if there was some
  // transmission and data ready for reading from the socket or not.
  //
  if ( ( UsedSockets = SDLNet_TCP_AddSocket( The_Set_Of_The_One_Server_Socket , sock ) ) == ( -1 ) )
    {
      fprintf( stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET SUBSYSTEM WAS UNABLE TO ADD A NEW SOCKET TO THE SOCKET SET.\n\
\n\
The reason for this as reported by the SDL is : \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError( ) ) ;
      Terminate(ERR);
    }
  else
    {
      DebugPrintf ( 0 , "\nSOCKETS IN SOCKET SET OF NO MORE THAN ONE SOCKET IS NOW : %d.\n" , UsedSockets );
    }

  //--------------------
  // Now we tell the server our name
  //
  SendPlayerNameToServer ( ) ;

}; // void ConnectToFreedroidServer ( void )

/* ----------------------------------------------------------------------
 * This function will go out soon in favour of the command format based
 * client-server-communication.
 * ---------------------------------------------------------------------- */
void
Send1024MessageToServer ( char message[1024] )
{
  int CommunicationResult;
  int len;

  // print out the message
  DebugPrintf ( 0 , "\nSending message : %s\n" , message ) ;
  len = strlen ( message );

  CommunicationResult = SDLNet_TCP_Send ( sock , message , len ); // add 1 for the NULL

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( 0 , "\nSending TCP message returned : %d . " , CommunicationResult );

  /*
  if(result<len)
    printf("SDLNet_TCP_Send: %s\n",SDLNet_GetError());
  */
  
};
  
/* ----------------------------------------------------------------------
 * This opens the TCP/IP sockets, so that some clients can connect
 * to this server...
 * ---------------------------------------------------------------------- */
void
OpenTheServerSocket ( void )
{

  // Resolve the argument into an IPaddress type
  if ( SDLNet_ResolveHost ( &ip , NULL , port ) == (-1) )
    {
      DebugPrintf ( 0 , "SDLNet_ResolveHost: %s\n" , SDLNet_GetError ( ) ) ;
      Terminate ( ERR );
    }
  else
    {
      DebugPrintf ( 0 , "\n--------------------\nSDLNet_ResolveHost was successful...\n--------------------\n" );
    }
  
  // open the server socket
  TheServerSocket = SDLNet_TCP_Open ( & ip ) ;

  if ( ! TheServerSocket )
    {
      DebugPrintf ( 0 , "\n--------------------\nERROR!!! SDLNet_TCP_Open: %s\n--------------------\n" , 
		    SDLNet_GetError ( ) ) ;
      Terminate ( ERR );
    }
  else
    {
      DebugPrintf ( 0 , "\n--------------------\nSDLNet_TCP_Open was successful...\n--------------------\n" );
    }

}; // void OpenTheServerSocket ( void )
  
/* ----------------------------------------------------------------------
 * This function does what?? 
 *
 * ---------------------------------------------------------------------- */
void
AcceptConnectionsFromClients ( void )
{
  int PlayerNum;
  int UsedSockets;

  //--------------------
  // We plan to offer MAX_PLAYERS connections to remote machines.
  // Therefore we check for new connections in this loop.
  //
  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
      //--------------------
      // Of course we MUST NOT use an existing and already used connections for
      // connecting a new player!!  So we avoid this...
      //
      if ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer ) continue;

      //--------------------
      // So having reached an unused position for the client now,
      // we can see if someone wants to join at this socket position
      // into our running server game.
      // 
      AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer = SDLNet_TCP_Accept ( TheServerSocket );

      if ( ! AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer )
	{ 
	  // no connection accepted
	  // printf("SDLNet_TCP_Accept: %s\n",SDLNet_GetError());
	  // SDL_Delay(100); //sleep 1/10th of a second
	  return;
	  // continue;
	}
      else
	{
	  DebugPrintf( 0 , "\nvoid AcceptConnectionsFromClinets ( void ) : NEW PLAYER HAS JUST CONNECTED !!!! \n" );
	  if ( ( UsedSockets = SDLNet_TCP_AddSocket( The_Set_Of_All_Client_Sockets , 
						     AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer ) 
		 ) == ( -1 ) )
	    {
	      fprintf( stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET SUBSYSTEM WAS UNABLE TO ADD A NEW SOCKET TO THE SOCKET SET.\n\
\n\
The reason for this as reported by the SDL is : \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError( ) ) ;
	      Terminate(ERR);
	    }
	  else
	    {
	      DebugPrintf ( 0 , "\nSOCKETS IN SET OF ALL CLIENT SOCKETS NOW : %d.\n" , UsedSockets );
	      AllPlayers [ PlayerNum ] . network_status = CONNECTION_FRESHLY_OPENED;
	    }
	}
  
      //--------------------
      // So now that we know that a new player has connected this instant,
      // we go and try to get the clients IP and port number
      //
      remoteip=SDLNet_TCP_GetPeerAddress( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer );
      if ( ! remoteip )
	{
	  DebugPrintf ( 0 , "\n--------------------\nERROR!!! SDLNet_TCP_GetPeerAddress: %s\n--------------------\n" , 
			SDLNet_GetError ( ) ) ;
	  Terminate( ERR );
	}
      else
	{
	  DebugPrintf ( 0 , "\n--------------------\nSDLNet_TCP_GetPeerAddress was successful... \n" ) ;

	  //--------------------
	  // print out the clients IP and port number
	  //
	  ipaddr=SDL_SwapBE32(remoteip->host);
	  printf("Accepted a connection from %d.%d.%d.%d port %hu\n",
		 ipaddr>>24,
		 (ipaddr>>16)&0xff,
		 (ipaddr>>8)&0xff,
		 ipaddr&0xff,
		 remoteip->port);
	}

      //--------------------
      // Now that the new client has been accepted, we send a welcome 
      // message to the client program.
      //
      SendTextMessageToClient ( PlayerNum , "\nMesg. f.t. server: WELCOME to the game!!!\n" );

    } // for .. test all MAX_PLAYER sockets for new clients wanting to connect.
  
}; // void AcceptConnectionsFromClients ( void )


/* ----------------------------------------------------------------------
 * When the server experiences problems and can't help but shut down, e.g.
 * due to a segfault or something or if the running multiplayer game 
 * should be terminated, then all existing clients should (perhaps be
 * informed? and then) disconnected.
 * ---------------------------------------------------------------------- */
void 
DisconnectAllRemoteClinets ( void )
{
  int PlayerNum;

  //--------------------
  // We close all the players sockets at the server...
  //
  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
      if ( AllPlayers[ PlayerNum ].ThisPlayersSocketAtTheServer )
	SDLNet_TCP_Close ( AllPlayers[ PlayerNum ].ThisPlayersSocketAtTheServer );
    }

}; // void DisconnectAllRemoteClinets ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
ExecutePlayerCommand ( int PlayerNum )
{
  SDL_Event *KeyEventPointer;

  DebugPrintf ( 0 , "\nExecutePlayerCommand ( int PlayerNum ): real function call confirmed." ) ;

  switch ( CommandFromPlayer [ PlayerNum ] . command_code )
    {
    case PLAYER_SERVER_ERROR:
      DebugPrintf ( 0 , "\nPLAYER_SERVER_ERROR command received...Terminting..." );
      Terminate ( ERR );
      break;
      
    case SERVER_THIS_IS_MY_NAME:
      DebugPrintf ( 0 , "\nSERVER_THIS_IS_MY_NAME command received... " );
      DebugPrintf ( 0 , "\nNew name string has length : %d. " , 
		    strlen ( CommandFromPlayer [ PlayerNum ] . command_data_buffer ) );
      strcpy ( Me [ PlayerNum ] . character_name , CommandFromPlayer [ PlayerNum ] . command_data_buffer ) ;
      DebugPrintf ( 0 , "\nNew name has been set for Player %d. " , PlayerNum ) ;

      //--------------------
      // At this point, for a test, we also initiate a new character at the server
      // Hope this doesn't crash everything.
      //
      InitiateNewCharacter ( PlayerNum , WAR_BOT );

      //--------------------
      // Since this is a completely new client or new character connecting, the first thing
      // it will need is a full update about the current me structure at
      // the server.
      //
      // I hope this will at least not overwrite the first Me entry as well
      // or come too early or something...
      //
      SendFullMeUpdateToClient ( PlayerNum );

      AllPlayers [ PlayerNum ] . network_status = GAME_ON ;

      SendEnemyDeletionRequestToClient ( PlayerNum ) ;

      SendFullEnemyEngramToClient ( PlayerNum ) ;

      SendFullItemEngramToClient ( PlayerNum ) ;

      break;

    case SERVER_ACCEPT_THIS_KEYBOARD_EVENT:
      DebugPrintf ( 0 , "\nSERVER_ACCEPT_THIS_KEYBOARD_EVENT command received..." );

      KeyEventPointer = ( SDL_Event* ) CommandFromPlayer [ PlayerNum ] . command_data_buffer ;
      
      switch ( KeyEventPointer -> type )
	{

	case SDL_KEYDOWN:
	  //--------------------
	  // In case the client just reportet a key being pressed, we enter
	  // this change into the server keyboard pressing table for this
	  // client.
	  //
	  DebugPrintf ( 0 , "\nKEYBOARD EVENT: Key down received... " );
	  AllPlayers [ PlayerNum ] . server_keyboard [ KeyEventPointer -> key.keysym.sym ] = TRUE ;

	  //--------------------
	  // Now we report the shift key status to the server as well...
	  //
	  // if ( event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT) )
	  // ShiftWasPressedInAddition=TRUE;
	  // else ShiftWasPressedInAddition=FALSE;


	  break;
	case SDL_KEYUP:
	  //--------------------
	  // In case the client just reportet a key being released, we enter
	  // this change into the server keyboard pressing table for this
	  // client.
	  //
	  DebugPrintf ( 0 , "\nKEYBOARD EVENT: Key up received... " );
	  AllPlayers [ PlayerNum ] . server_keyboard [ KeyEventPointer -> key.keysym.sym ] = FALSE ;
	  break;

	default:
	  //--------------------
	  // There should no be any other keyboard events than keys being pressed
	  // or keys being released.  It some other event is sent, this is assumed to
	  // be a major error and terminates the server.
	  //
	  DebugPrintf ( 0 , "\nKEYBOARD EVENT DATA RECEIVED IS BULLSHIT!! TERMINATING... " );
	  Terminate ( ERR ) ;
	  break;
	}
      break;

    case SERVER_ACCEPT_THIS_MOUSE_BUTTON_EVENT:
      DebugPrintf ( 0 , "\nSERVER_ACCEPT_THIS_MOUSE_BUTTON_EVENT command received..." );

      KeyEventPointer = ( SDL_Event* ) CommandFromPlayer [ PlayerNum ] . command_data_buffer ;
      
      switch ( KeyEventPointer -> type )
	{
	case SDL_MOUSEBUTTONDOWN:
	  //--------------------
	  // In case the client just reportet a mouse button being pressed, we enter
	  // this change into the server mouse button pressing table for this
	  // client.
	  //
	  DebugPrintf ( 0 , "\nMOUSE BUTTON EVENT: Mouse button down received... " );
	  AllPlayers [ PlayerNum ] . server_mouse_button = TRUE ;
	  AllPlayers [ PlayerNum ] . server_mouse_x = KeyEventPointer -> button.x - UserCenter_x + 16 ;
	  AllPlayers [ PlayerNum ] . server_mouse_y = KeyEventPointer -> button.y - UserCenter_y + 16 ;

	  Me [ PlayerNum ] . LastMouse_X = AllPlayers [ PlayerNum ] . server_mouse_x ;
	  Me [ PlayerNum ] . LastMouse_Y = AllPlayers [ PlayerNum ] . server_mouse_y ;

	  break;
	case SDL_MOUSEBUTTONUP:
	  //--------------------
	  // In case the client just reportet a mouse button being released, we enter
	  // this change into the server mouse button pressing table for this
	  // client.
	  //
	  DebugPrintf ( 0 , "\nMOUSE BUTTON EVENT: Mouse button up received... " );
	  AllPlayers [ PlayerNum ] . server_mouse_button = FALSE ;
	  AllPlayers [ PlayerNum ] . server_mouse_x = KeyEventPointer -> button.x ;
	  AllPlayers [ PlayerNum ] . server_mouse_y = KeyEventPointer -> button.y ;
	  break;

	default:
	  //--------------------
	  // There should no be any other mouse button events than buttons being pressed
	  // or buttons being released.  It some other event is sent, this is assumed to
	  // be a major error and terminates the server.
	  //
	  DebugPrintf ( 0 , "\nMOUSE BUTTON EVENT DATA RECEIVED IS BULLSHIT!! TERMINATING... " );
	  Terminate ( ERR ) ;
	  break;
	}
      break;

    case SERVER_ACCEPT_THIS_ITEM_DROP:
      DebugPrintf ( 0 , "\nSERVER_ACCEPT_THIS_ITEM_DROP command received from player %d." , PlayerNum ) ;
      memcpy ( & ( ItemDropEngram) , CommandFromPlayer [ PlayerNum ] . command_data_buffer , sizeof ( ItemDropEngram ) );
      EnforcePlayersItemDrop ( PlayerNum ) ;
      break;

    case SERVER_ACCEPT_THIS_ITEM_MOVE:
      DebugPrintf ( 0 , "\nSERVER_ACCEPT_THIS_ITEM_MOVE command received from player %d." , PlayerNum ) ;
      memcpy ( & ( ItemMoveEngram) , CommandFromPlayer [ PlayerNum ] . command_data_buffer , sizeof ( ItemMoveEngram ) );
      EnforcePlayersItemMove ( PlayerNum ) ;
      break;

    case PLAYER_TELL_ME_YOUR_NAME:
      DebugPrintf ( 0 , "\nPLAYER_TELL_ME_YOUR_NAME command received...Terminting..." );
      Terminate ( ERR );
      break;
      
    case PLAYER_SERVER_TAKE_THIS_TEXT_MESSAGE:
      DebugPrintf ( 0 , "\nPLAYER_SERVER_TAKE_THIS_IS_TEXT_MESSAGE command received... AS SERVER." );
      DebugPrintf ( 0 , "\nMessage transmitted : %s." , & CommandFromPlayer [ PlayerNum ] . command_data_buffer );
      // Terminate ( ERR );
      break;

      
    case PLAYER_ACCEPT_ALL_ME_UPDATES:
      DebugPrintf ( 0 , "\nPLAYER_ACCEPT_ALL_ME_UPDATES command received AS SERVER!!!! Terminating... " );
      Terminate ( ERR );
      break;

    default:
      DebugPrintf ( 0 , "\nUNKNOWN COMMAND CODE ERROR!!!! Terminating..." );
      Terminate ( ERR );
      break;

    };

}; // void ExecutePlayerCommand ( PlayerNum )

/* ----------------------------------------------------------------------
 * This function reads the signals from the client socket of player
 * PlayerNum at the server.  Depending on the current fill and 
 * command completition status of this players command buffer, be data
 * will be read into the buffer at the right location and also the right
 * amount of data will be read.
 * ---------------------------------------------------------------------- */
void
Read_Command_Buffer_From_Player_No ( int PlayerNum )
{
  int len;

  //--------------------
  // Depending on the current fill status of the command buffer, we read in the 
  // command code and command buffer length or not.
  //
  if ( ! AllPlayers [ PlayerNum ] . command_buffer_fill_status )
    {

      DebugPrintf ( 0 , "\nvoid Read_Command_Buffer_From_Player_No ( int PlayerNum ): New Command expected." );

      len = SDLNet_TCP_Recv ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
			      & ( CommandFromPlayer [ PlayerNum ] . command_code ) , 
			      sizeof ( CommandFromPlayer [ PlayerNum ] . command_code ) ) ;

      if ( len == 0 ) 
	{
	  DebugPrintf ( 0 , "\n\nReading command code failed.... Terminating... " );
	  Terminate ( ERR );
	}
      if ( len < sizeof ( CommandFromPlayer [ PlayerNum ] . command_code ) ) 
	{
	  DebugPrintf ( 0 , "\n\nReading command:  command code did not completely come over... " );
	  Terminate ( ERR );
	}

      len = SDLNet_TCP_Recv ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
			      & ( CommandFromPlayer [ PlayerNum ] . data_chunk_length ) , 
			      sizeof ( CommandFromPlayer [ PlayerNum ] . data_chunk_length ) ) ;
      
      if ( len == 0 ) 
	{
	  DebugPrintf ( 0 , "\n\nReading command data buffer length failed.... Terminating... " );
	  Terminate ( ERR );
	}
      if ( len < sizeof ( CommandFromPlayer [ PlayerNum ] . data_chunk_length ) ) 
	{
	  DebugPrintf ( 0 , "\n\nReading command:  data buffer length info did not completely come over... " );
	  Terminate ( ERR );
	}

      len = SDLNet_TCP_Recv ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer , 
			      & ( CommandFromPlayer [ PlayerNum ] . command_data_buffer ) , 
			      CommandFromPlayer [ PlayerNum ] . data_chunk_length ) ;

      if ( len == 0 ) 
	{
	  DebugPrintf ( 0 , "\n\nReading command data buffer itself failed.... Terminating... " );
	  Terminate ( ERR );
	}
      if ( len < CommandFromPlayer [ PlayerNum ] . data_chunk_length ) 
	{
	  DebugPrintf ( 0 , "\n\nReading command:  data did not come completely over... " );
	  Terminate ( ERR );
	}

      //--------------------
      // Now at this point we know, that we have received a complete and full
      // command from the client.  That means we can really execute the command
      // that has just been filled into the command buffer.
      //
      ExecutePlayerCommand ( PlayerNum );

    }
  else
    {

      DebugPrintf ( 0 , "\nvoid Read_Command_Buffer_From_Player_No ( int PlayerNum ): APPENDING OLD COMMAND." );

    }

}; // void Read_Command_Buffer_From_Player_No ( int PlayerNum )

/* ----------------------------------------------------------------------
 * This function listens if some signal just came from one of the clinets
 * ---------------------------------------------------------------------- */
void
ListenToAllRemoteClients ( void )
{
  int PlayerNum;
  int ActivityInTheSet;

  DebugPrintf( 2 , "\nvoid ListenToAllRemoteClients ( void ) : real function call confirmed ..." );

  //--------------------
  // At first we check if something is going on in the set of all
  // the clints sockets at the server.  If not, we're done here and
  // need not do anything more...
  //
  // ActivityInTheSet = SDLNet_SocketReady ( The_Set_Of_All_Client_Sockets ) ;
  ActivityInTheSet = SDLNet_CheckSockets ( The_Set_Of_All_Client_Sockets , 0 ) ;
  if ( ActivityInTheSet == 0 )
    {
      DebugPrintf ( 2 , "\nNo Activity in the whole set detected.... Returning.... " );
      return;
    }
  else
    {
      DebugPrintf ( 2 , "\nvoid ListenToAllRemoteClients ( void ) : Activity in the set detected." );
    }

  //--------------------
  // Now we check in which one of the sockets the acitivty has
  // taken place.
  //
  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
      //--------------------
      // Of course we should not try to read some info from a
      // client socket, that has not yet been occupied.  Therefore
      // we introduce this protection.
      //
      if ( ! AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer ) continue;

      //--------------------
      // Since we do not know which of the sockets in the set has caused the 
      // activity, we check if it was perhaps this one, before we start a blocking
      // read receive command, which would halt the whole server, cause it's blocking
      // if this socket didn't really have something to say...
      //
      if ( ! SDLNet_SocketReady( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer ) )
	{
	  DebugPrintf ( 0 , "\nSocked Nr. %d didn't cause the activity..." , PlayerNum );
	  continue;
	}

      //--------------------
      // Now that we know, that there is at least a socket, where 
      // perhaps some information might be ready for us to read, we
      // try to get this piece of information.
      //
      Read_Command_Buffer_From_Player_No ( PlayerNum );

    }

  DebugPrintf( 2 , "\nvoid ListenToAllRemoteClients ( void ) : end of function reached..." );

}; // void ListenToAllRemoteClients ( void )

/* ---------------------------------------------------------------------- 
 * This function sends something from the server to all of the clients
 * currently connected to this server.
 * ---------------------------------------------------------------------- */
void
ServerSendMessageToAllClients ( char ServerMessage[1024] )
{
  int PlayerNum;
  int BytesSent;
  int MessageLength;

  MessageLength = strlen ( ServerMessage );

  DebugPrintf( SERVER_SEND_DEBUG , "\n--------------------" ) ;
  DebugPrintf( SERVER_SEND_DEBUG , "\nSending message to all clinets: %s." , ServerMessage );
  DebugPrintf( SERVER_SEND_DEBUG , "\nMessage length is found to be : %d." , MessageLength );

  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
      if ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer )
	{
	  DebugPrintf( SERVER_SEND_DEBUG , "\nNow sending to Player %d." , PlayerNum );
	  
	  BytesSent = SDLNet_TCP_Send( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer, 
				       ServerMessage , MessageLength ) ;
	  DebugPrintf( SERVER_SEND_DEBUG , "\nNumber of Bytes sent to Player %d : %d." , PlayerNum , BytesSent );

	}
    }

}; // void ServerSendMessageToAllClients ( char ServerMessage[1024] );

/* ----------------------------------------------------------------------
 * This function listens, if the server has perhaps said something to
 * us.  In this case, the received data is printed out.
 * ---------------------------------------------------------------------- */
void
ListenForServerMessages ( void ) 
{
  int ActivityInTheSet;

  //--------------------
  // Here we check if the set of the one and only server socket
  // perhaps contains some data in its only socket.
  //
  // if ( ActivityInTheSet == 0 )
  // {
  // DebugPrintf ( 2 , "\nNo Activity in the whole set of the one server socket at the client detected.... Returning.... " );
  // return;
  // }

  //--------------------
  // Now we read out the detected activity AND we keep doing so, until
  // no more activity is detected in the set of the one server socket.
  //
  while ( ( ActivityInTheSet = SDLNet_CheckSockets ( The_Set_Of_The_One_Server_Socket , 0 ) ) )
    {
      // else
      // {
      // DebugPrintf ( 2 , "\nvoid ListenForServerMessages ( void ) : Something was sent from the server!!!" );
      // }
      
      //--------------------
      // This is somewhat redundant, cause since there is never more than one socket
      // in this socket set, it must be this one socket, where the activity came from.
      // But nevertheless, being a networking newbie, I'll check again if the activity
      // has really been observed in the socket itself.
      //
      if ( ! SDLNet_SocketReady( sock ) )
	{
	  DebugPrintf ( 0 , "\n\
--------------------\n\
WARNING!! SEVERE ERROR ENCOUNTERED! ACTIVITY THERE, BUT NOT DETECTABLE!! ERROR!!\n\
Termination....\n\
--------------------\n" );
	  Terminate ( ERR ) ;
	}
      
      //--------------------
      // Now that we know, that we really got some message from the server,
      // we try to read that information.
      //
      Read_Command_Buffer_From_Server (  ) ;
      
    }
}; // void ListenForServerMessages ( void ) 

/* ----------------------------------------------------------------------
 * This function sends a text message to the server in command form.
 * ---------------------------------------------------------------------- */
void
SendTextMessageToServer ( char* message )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( 0 , "\nSending to server in command form.  Message is : %s\n" , message ) ;
  len = strlen ( message ) + 1 ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long message to server... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = PLAYER_SERVER_TAKE_THIS_TEXT_MESSAGE ;
  LocalCommandBuffer . data_chunk_length = len ;
  strcpy ( LocalCommandBuffer . command_data_buffer , message );

  

  CommunicationResult = SDLNet_TCP_Send ( sock , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( 0 , "\nSending TCP message returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A TEXT MESSAGE TO THE SERVER SUCCESSFULLY\n\
in the function void SendTextMessageToServer ( char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendTextMessageToServer ( char* message )

/* ----------------------------------------------------------------------
 * This function sends a text message to the server in command form.
 * ---------------------------------------------------------------------- */
void
SendPlayerNameToServer ( void )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( 0 , "\nSending player name to server in command form. " ) ;
  len = strlen ( Me [ 0 ] . character_name ) + 1 ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= MAX_CHARACTER_NAME_LENGTH )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long character name to server... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = SERVER_THIS_IS_MY_NAME ;
  LocalCommandBuffer . data_chunk_length = len ;
  strcpy ( LocalCommandBuffer . command_data_buffer , Me [ 0 ] . character_name );

  CommunicationResult = SDLNet_TCP_Send ( sock , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( 0 , "\nSending character name to server returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A TEXT MESSAGE TO THE SERVER SUCCESSFULLY\n\
in the function void SendTextMessageToServer ( char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendPlayerNameToServer ( void )

/* ----------------------------------------------------------------------
 * This function sends a keyboard event to the server in command form.
 * ---------------------------------------------------------------------- */
void
SendPlayerKeyboardEventToServer ( SDL_Event event )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( 0 , "\nSending keyboard event to server in command form. " ) ;
  len = sizeof ( event ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long keyboard event to server... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = SERVER_ACCEPT_THIS_KEYBOARD_EVENT ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , & ( event ) , sizeof ( event ) );

  CommunicationResult = SDLNet_TCP_Send ( sock , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( 0 , "\nSending keyboard event to server returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A KEYBOARD EVENT TO THE SERVER SUCCESSFULLY\n\
in the function void SendTextMessageToServer ( char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendPlayerKeyboardEventToServer ( void )

/* ----------------------------------------------------------------------
 * This function sends a mouse button event to the server in command form.
 * ---------------------------------------------------------------------- */
void
SendPlayerMouseButtonEventToServer ( SDL_Event event )
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( 0 , "\nSending keyboard event to server in command form. " ) ;
  len = sizeof ( event ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long keyboard event to server... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // Now we prepare our command buffer.
  //
  LocalCommandBuffer . command_code = SERVER_ACCEPT_THIS_MOUSE_BUTTON_EVENT ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , & ( event ) , sizeof ( event ) );

  CommunicationResult = SDLNet_TCP_Send ( sock , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( 0 , "\nSending mouse button event to server returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND A MOUSE BUTTON EVENT TO THE SERVER SUCCESSFULLY\n\
in the function void SendTextMessageToServer ( char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendPlayerMouseButtonEventToServer ( void )

/* ----------------------------------------------------------------------
 * This function informs the server, that an item drop has occured from
 * the players equipment to the floor.
 * ---------------------------------------------------------------------- */
void
SendPlayerItemDropToServer ( int PositionCode , float x , float y ) 
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( 0 , "\nSending item drop to server in command form. " ) ;
  len = sizeof ( ItemDropEngram ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long item drop engram to server... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // Now we prepare our command buffer.
  //
  ItemDropEngram . item_slot_code = PositionCode ;
  ItemDropEngram . pos . x = x ;
  ItemDropEngram . pos . y = y ;

  LocalCommandBuffer . command_code = SERVER_ACCEPT_THIS_ITEM_DROP ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , & ( ItemDropEngram ) , sizeof ( ItemDropEngram ) );

  CommunicationResult = SDLNet_TCP_Send ( sock , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( 0 , "\nSending item drop engram to server returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND AN ITEM DROP ENGRAM TO THE SERVER SUCCESSFULLY\n\
in the function void SendTextMessageToServer ( char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }

}; // void SendPlayerItemDropToServer ( int PositionCode , float x , float y ) 

/* ----------------------------------------------------------------------
 * This function sends an internal item move on the client to the server.
 * ---------------------------------------------------------------------- */
void
SendPlayerItemMoveToServer ( int SourcePositionCode , int DestPositionCode , int inv_x , int inv_y ) 
{
  int CommunicationResult;
  int len;
  network_command LocalCommandBuffer;

  // print out the message
  DebugPrintf ( 0 , "\nSending item move to server in command form. " ) ;
  len = sizeof ( ItemMoveEngram ) ; // the amount of bytes in the data buffer

  //--------------------
  // We check against sending too long messages to the server.
  //
  if ( len >= COMMAND_BUFFER_MAXLEN )
    {
      DebugPrintf ( 0 , "\nAttempted to send too long item move engram to server... Terminating..." );
      Terminate ( ERR ) ;
    }

  //--------------------
  // Now we prepare our command buffer.
  //
  ItemMoveEngram . source_item_slot_code = SourcePositionCode ;
  ItemMoveEngram . dest_item_slot_code = DestPositionCode ;

  if ( inv_x != ( -1 ) ) ItemMoveEngram . dest_inv_pos . x = inv_x ;
  if ( inv_y != ( -1 ) ) ItemMoveEngram . dest_inv_pos . y = inv_y ;

  LocalCommandBuffer . command_code = SERVER_ACCEPT_THIS_ITEM_MOVE ;
  LocalCommandBuffer . data_chunk_length = len ;
  memcpy ( LocalCommandBuffer . command_data_buffer , & ( ItemMoveEngram ) , sizeof ( ItemMoveEngram ) );

  CommunicationResult = SDLNet_TCP_Send ( sock , 
					  & ( LocalCommandBuffer ) , 
					  2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length ); 

  //--------------------
  // Now we print out the success or return value of the sending operation
  //
  DebugPrintf ( 0 , "\nSending item move engram to server returned : %d . " , CommunicationResult );
  if ( CommunicationResult < 2 * sizeof ( int ) + LocalCommandBuffer . data_chunk_length )
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL NET COULD NOT SEND AN ITEM MOVE ENGRAM TO THE SERVER SUCCESSFULLY\n\
in the function void SendTextMessageToServer ( char* message ).\n\
\n\
The cause of this problem as reportet by the SDL_net was: \n\
%s\n\
\n\
Freedroid will terminate now to draw attention \n\
to the networking problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , SDLNet_GetError ( ) ) ;
      Terminate(ERR);
    }
}; // void SendPlayerItemMoveToServer ( int SourcePositionCode , int DestPositionCode ) 

/* ----------------------------------------------------------------------
 * This function assembles copys of the item information on all levels
 * of the map, so that later this information can be used to tell what
 * changes have happened since the last server-client talk.
 * ---------------------------------------------------------------------- */
void
StoreAllOldItemInformation ( void )
{
  int LevelNum;

  // DebugPrintf ( 0 , "\nvoid StoreAllOldItemInformation ( void ) : doing it for %d levels." , curShip.num_levels ) ;

  for ( LevelNum = 0 ; LevelNum < curShip.num_levels ; LevelNum ++ )
    {
      
      memcpy ( & ( curShip . AllLevels [ LevelNum ] -> OldItemList [ 0 ] ) ,
	       & ( curShip . AllLevels [ LevelNum ] ->    ItemList [ 0 ] ) ,
	       MAX_ITEMS_PER_LEVEL * sizeof ( item ) ) ;
    }

}; // void StoreAllOldItemInformation ( void )

/* ----------------------------------------------------------------------
 * This prints out the server status.  Well, it will print anyway, but
 * only the server will know the full and correct information.
 * ---------------------------------------------------------------------- */
void
PrintServerStatusInformation ( void )
{
  int PlayerNum;

  DebugPrintf ( 0 , "\n----------------------------------------" );
  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
      DebugPrintf ( 0 , "\nPlayer Nr : %d Character Name : %s. Status : %d NW-Status: %s." ,
		    PlayerNum , Me [ PlayerNum ] . character_name , Me [ PlayerNum ] . status , 
		    NetworkClientStatusNames[ AllPlayers [ PlayerNum ] . network_status ] );
      DebugPrintf ( 0 , "\nPos:X=%d Y=%d Z=%d Energy=%d." ,
		    ( int ) Me [ PlayerNum ] . pos.x , 
		    ( int ) Me [ PlayerNum ] . pos.y ,
		    ( int ) Me [ PlayerNum ] . pos.z ,
		    ( int ) Me [ PlayerNum ] . energy );
    }
  DebugPrintf ( 0 , "\n----------------------------------------" );

}; // void PrintServerStatusInformation ( void )

/* ----------------------------------------------------------------------
 * This function sends the periodic updates to all connected clients.
 * ---------------------------------------------------------------------- */
void
SendPeriodicServerMessagesToAllClients ( void )
{
  int PlayerNum;
  static int DelayCounter = 0;

#define SEND_PACKET_ON_EVERY_FRAME_CONG_MOD ( 3 )

  DelayCounter ++ ;
  if ( DelayCounter < SEND_PACKET_ON_EVERY_FRAME_CONG_MOD ) return;

  DelayCounter = 0;


  DebugPrintf( PERIODIC_MESSAGE_DEBUG , "\n--------------------" ) ;
  DebugPrintf( PERIODIC_MESSAGE_DEBUG , "\nSending periodic server messages to all clinets...." );

  for ( PlayerNum = 0 ; PlayerNum < MAX_PLAYERS ; PlayerNum ++ )
    {
      if ( AllPlayers [ PlayerNum ] . ThisPlayersSocketAtTheServer )
	{
	  DebugPrintf( SERVER_SEND_DEBUG , "\nNow sending periodic server message to Player %d." , PlayerNum );
	  
	  SendFullPlayerEngramToClient ( PlayerNum ) ;

	  SendEnemyUpdateEngramToClient ( PlayerNum ) ;

	  SendFullBulletEngramToClient ( PlayerNum ) ;

	  SendFullBlastEngramToClient ( PlayerNum ) ;

	  // SendFullItemEngramToClient ( PlayerNum ) ;
	  SendItemUpdateToClient ( PlayerNum ) ;

	}
    }

  StoreAllOldItemInformation ( ) ;

}; // void SendPeriodicServerMessagesToAllClients ( void )

#undef _network_c
