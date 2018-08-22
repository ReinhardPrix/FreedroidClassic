/*
 *
 *   Copyright (c) 1994, 2002, 2003  Johannes Prix
 *   Copyright (c) 1994, 2002, 2003  Reinhard Prix
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

/*----------------------------------------------------------------------
 *
 * Desc: Everything that has to do with the takeover game of Paradroid
 * 	is contained in this file.
 *
 *----------------------------------------------------------------------*/

#define _takeover_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "takeover.h"
#include "map.h"

Uint32 cur_time;  		/* current time in ms */

/* Class seperation of the blocks */
int BlockClass[TO_BLOCKS] = {
  CONNECTOR,			/* KABEL */
  NON_CONNECTOR,		/* KABELENDE */
  CONNECTOR,			/* VERSTAERKER */
  CONNECTOR,			/* FARBTAUSCHER */
  CONNECTOR,			/* VERZWEIGUNG_O */
  NON_CONNECTOR,		/* VERZWEIGUNG_M */
  CONNECTOR,			/* VERZWEIGUNG_U */
  NON_CONNECTOR,		/* GATTER_O */
  CONNECTOR,			/* GATTER_M */
  NON_CONNECTOR,		/* GATTER_U */
  NON_CONNECTOR			/* LEER */
};

/* Probability of the various elements */
#define MAX_PROB		100
int ElementProb[TO_ELEMENTS] = {
  100,				/* EL_KABEL */
  2,				/* EL_KABELENDE */
  5,				/* EL_VERSTAERKER */
  5,				/* EL_FARBTAUSCHER: only on last layer */
  5,				/* EL_VERZWEIGUNG */
  5				/* EL_GATTER */
};


int NumCapsules[TO_COLORS] = {
  0, 0
};

point LeftCapsulesStart[TO_COLORS] = {
  { 4, 2*27 },
  { 2*255 + 2*30 - 10, 2*27 }
};

point CurCapsuleStart[TO_COLORS] = {
  { 2*26, 2*19},
  { 2*255, 2*19}
};

point PlaygroundStart[TO_COLORS] = {
  { 2*33, 2*26},
  { 2*159, 2*26}
};

point DruidStart[TO_COLORS] = {
  { 2*40, -4},
  { 2*220, -4}
};

/* Offset of the left/right "grounds" and the "column" */
point TO_LeftGroundStart = 	{ 2*10, 2*15 };
point TO_ColumnStart = 		{ 2*136, 2*27};
point TO_LeaderBlockStart = 	{ 2*129, 2*8};
point TO_RightGroundStart = 	{ 2*255, 2*15};

SDL_Rect TO_LeaderLed = { 2*136, 2*11, 2*16, 2*19};
SDL_Rect TO_FillBlock = { 0, 0, 2*16, 2*7 };
SDL_Rect TO_ElementRect = { 0, 0, 2*32, 2*8 };
SDL_Rect TO_CapsuleRect = { 0, 0, 2*7, 2*8};
SDL_Rect TO_GroundRect = { 0, 0, 2*23, 2*8 };
SDL_Rect TO_ColumnRect = { 0, 0, 2*30, 2*8 };


int CapsuleCurRow[TO_COLORS] = { 0, 0 };


int LeaderColor = GELB;		/* momentary leading color */
int YourColor = GELB;
int OpponentColor = VIOLETT;
int OpponentType;		/* The druid-type of your opponent */
int DroidNum;

/* the display  column */
int DisplayColumn[NUM_LINES] = {
  GELB, VIOLETT, GELB, VIOLETT, GELB, VIOLETT, GELB, VIOLETT, GELB, VIOLETT,
  GELB, VIOLETT
};


SDL_Color to_bg_color = {130,130,130};

playground_t ToPlayground;
playground_t ActivationMap;
playground_t CapsuleCountdown;

/*-----------------------------------------------------------------
 * @Desc: play takeover-game against a druid
 *
 * @Ret: TRUE/FALSE:  user has won/lost
 *
 *-----------------------------------------------------------------*/
int
Takeover (int enemynum)
{
  int row;
  int FinishTakeover = FALSE;
  static int RejectEnergy = 0;	/* your energy if you're rejected */
  char *message;
  SDL_Rect buf;
  Uint32 now;

  /* Prevent distortion of framerate by the delay coming from
   * the time spend in the menu.
   */
  Activate_Conservative_Frame_Computation ();

  // release fire keys
  FirePressedR();
  ReleaseKey (CMD_FIRE);
  ReleaseKey (CMD_ACTIVATE);
  ReleaseKey (CMD_TAKEOVER);

  // Takeover game always uses Classic User_Rect:
  Copy_Rect (User_Rect, buf);
  Copy_Rect (Classic_User_Rect, User_Rect);

  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  Fill_Rect (User_Rect, to_bg_color);

  Me.status = MOBILE; /* the new status _after_ the takeover game */

  SDL_ShowCursor (SDL_DISABLE); // no mouse-cursor in takeover game!

  show_droid_info ( Me.type, -1 , 0);
  show_droid_portrait (Cons_Droid_Rect, Me.type, DROID_ROTATION_TIME, UPDATE);
  while (!FirePressedR())
    show_droid_portrait (Cons_Droid_Rect, Me.type, DROID_ROTATION_TIME, 0);

  show_droid_info ( AllEnemys[enemynum].type, -2 ,0);
  show_droid_portrait (Cons_Droid_Rect,  AllEnemys[enemynum].type, DROID_ROTATION_TIME, UPDATE);
  while (!FirePressedR())
    show_droid_portrait (Cons_Droid_Rect,  AllEnemys[enemynum].type, DROID_ROTATION_TIME, 0);

  SDL_BlitSurface (takeover_bg_pic, NULL, ne_screen, NULL);
  DisplayBanner (NULL, NULL,  BANNER_FORCE_UPDATE );

  while (!FinishTakeover)
    {
      /* Init Color-column and Capsule-Number for each opponenet and your color */
      for (row = 0; row < NUM_LINES; row++)
	{
	  DisplayColumn[row] = (row % 2);
	  CapsuleCountdown[GELB][0][row] = -1;
	  CapsuleCountdown[VIOLETT][0][row] = -1;
	}			/* for row */

      YourColor = GELB;
      OpponentColor = VIOLETT;

      CapsuleCurRow[GELB] = 0;
      CapsuleCurRow[VIOLETT] = 0;

      DroidNum = enemynum;
      OpponentType = AllEnemys[enemynum].type;
      NumCapsules[YOU] = 3 + ClassOfDruid (Me.type);
      NumCapsules[ENEMY] = 4 + ClassOfDruid (OpponentType);

      InventPlayground ();

      ShowPlayground ();

      ChooseColor ();

      PlayGame ();

      /* Ausgang beurteilen und returnen */
      if (InvincibleMode || (LeaderColor == YourColor))
	{
	  Takeover_Game_Won_Sound ();
	  if (Me.type == DRUID001)
	    {
	      RejectEnergy = Me.energy;
	      PreTakeEnergy = Me.energy;
	    }

	  // We provide some security agains too high energy/health values gained
	  // by very rapid successions of successful takeover attempts
	  if (Me.energy > Druidmap[DRUID001].maxenergy) Me.energy = Druidmap[DRUID001].maxenergy;
	  if (Me.health > Druidmap[DRUID001].maxenergy) Me.health = Druidmap[DRUID001].maxenergy;

	  // We allow to gain the current energy/full health that was still in the
	  // other droid, since all previous damage must be due to fighting damage,
	  // and this is exactly the sort of damage can usually be cured in refreshes.
	  Me.energy += AllEnemys[enemynum].energy;
	  Me.health += Druidmap[OpponentType].maxenergy;

	  Me.type = AllEnemys[enemynum].type;

	  RealScore += Druidmap[OpponentType].score;

	  DeathCount += OpponentType * OpponentType;   // quadratic "importance", max=529

	  AllEnemys[enemynum].status = OUT; // removed droid silently (no blast!)

	  if (LeaderColor != YourColor)	/* only won because of InvincibleMode */
	    message = "You cheat";
	  else				/* won the proper way */
	    message = "Complete";

	  FinishTakeover = TRUE;
	}				/* LeaderColor == YourColor */
      else if (LeaderColor == OpponentColor)
	{
	  // you lost, but enemy is killed too --> blast it!
	  AllEnemys[enemynum].energy = -1.0;  /* to be sure */

	  Takeover_Game_Lost_Sound ();
	  if (Me.type != DRUID001)
	    {
	      message = "Rejected";
	      Me.type = DRUID001;
	      Me.energy = RejectEnergy;
	    }
	  else
	    {
	      message = "Burnt Out";
	      Me.energy = 0;
	    }
	  FinishTakeover = TRUE;
	}			/* LeadColor == OpponentColor */
      else
	{
	  Takeover_Game_Deadlock_Sound ();
	  message = "Deadlock";
	}			/* LeadColor == REMIS */

      DisplayBanner (message, NULL , 0 );
      ShowPlayground ();
      now = SDL_GetTicks();
      while ((!FirePressedR()) && (SDL_GetTicks() - now < SHOW_WAIT) ) SDL_Delay(1);

    }	/* while !FinishTakeover */

  // restore User_Rect
  Copy_Rect (buf, User_Rect);

  ClearGraphMem();
  SDL_Flip(ne_screen);

  if (LeaderColor == YourColor)
    return TRUE;
  else
    return FALSE;

} /* Takeover() */


/*@Function============================================================
@Desc: ChooseColor():	Countdown zum Waehlen der Farbe

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
ChooseColor (void)
{
  int countdown = 100;  /* duration in 1/10 seconds given for color choosing */
  int ColorChosen = FALSE;
  char count_text[80];

  Uint32 prev_count_tick, count_tick_len;

  count_tick_len = 100; 	/* countdown in 1/10 second steps */

  prev_count_tick = SDL_GetTicks ();

  ResetMouseWheel ();  // forget about previous wheel events

  while (!ColorChosen)
    {
      if (RightPressedR() || WheelDownPressed())
	{
	  if (YourColor != VIOLETT) MoveMenuPositionSound();
	  YourColor = VIOLETT;
	  OpponentColor = GELB;
	}
      if (LeftPressedR() || WheelUpPressed())
	{
	  if (YourColor != GELB) MoveMenuPositionSound();
	  YourColor = GELB;
	  OpponentColor = VIOLETT;
	}

      if (FirePressedR())
	{
	  ColorChosen = TRUE;
	}

      /* wait for next countdown tick */
      if ( SDL_GetTicks() >= prev_count_tick + count_tick_len )
	{
	  prev_count_tick += count_tick_len; /* set for next tick */
	  countdown--;		/* Count down */
	  sprintf (count_text, "Color-%d", countdown);

	  DisplayBanner (count_text, NULL , 0);
	  ShowPlayground ();
	}


      if (countdown == 0)
	ColorChosen = TRUE;

    } /* while(!ColorChosen) */

  return;

} /* ChooseColor() */


/*-----------------------------------------------------------------
 * @Desc: the acutal Takeover game-playing is done here
 *
 *
 *-----------------------------------------------------------------*/
void
PlayGame (void)
{
  int countdown = 100;   /* lenght of Game in 1/10 seconds */
  char count_text[80];
  int FinishTakeover = FALSE;
  int row;

  Uint32 prev_count_tick = 0, count_tick_len;  /* tick vars for count-down */
  Uint32 prev_move_tick = 0, move_tick_len;    /* tick vars for motion */
  Uint32 last_movekey_time = 0, wait_move_ticks = 110;    /* number of ticks to wait before "key-repeat" */

  int up, down, set;
  int wheel_up, wheel_down;

  count_tick_len = 100;   /* countdown in 1/10 second steps */
  move_tick_len  = 60;    /* allow motion at this tick-speed in ms */

  up = down = set = FALSE;
  wheel_up = wheel_down = 0;

  prev_count_tick = prev_move_tick = SDL_GetTicks (); /* start tick clock */

  ResetMouseWheel ();  // forget about previous wheel events

  CountdownSound();
  while (!FinishTakeover)
    {
      cur_time = SDL_GetTicks ();

      /*
       * here we register if there have been key-press events in the
       * "waiting period" between move-ticks :
       */
      if ( !up && UpPressed () )
	{
	  up = TRUE;
	  last_movekey_time = SDL_GetTicks();
	}
      if (!down && DownPressed() )
	{
	  down = TRUE;
	  last_movekey_time = SDL_GetTicks();
	}

      set  = set  || FirePressed();

      if (WheelUpPressed()) wheel_up ++;
      if (WheelDownPressed()) wheel_down ++;

      /* allow for a WIN-key that give immedate victory */
      if ( KeyIsPressedR ('w') && CtrlPressed() && AltPressed() )
	{
	  LeaderColor = YourColor;   /* simple as that */
	  return;  /* leave now, to avoid changing of LeaderColor! */
	}

      if ( cur_time > prev_count_tick + count_tick_len ) /* time to count 1 down */
	{
	  prev_count_tick += count_tick_len;  /* set for next countdown tick */
	  countdown--;
	  sprintf (count_text, "Finish-%d", countdown);
	  DisplayBanner (count_text, NULL , 0 );

	  if (countdown && (countdown%10 == 0) ) CountdownSound();
	  if (countdown == 0)
	    {
	      EndCountdownSound();
	      FinishTakeover = TRUE;
	    }

	  AnimateCurrents ();  /* do some animation on the active cables */

	} /* if (countdown_tick has occurred) */


      /* time for movement */
      if ( cur_time > prev_move_tick + move_tick_len )
	{
	  prev_move_tick += move_tick_len; /* set for next motion tick */
	  EnemyMovements ();

	  if (wheel_up || (up && (SDL_GetTicks() - last_movekey_time > wait_move_ticks)))
	    {
	      CapsuleCurRow[YourColor]--;
	      if (CapsuleCurRow[YourColor] < 1)
		CapsuleCurRow[YourColor] = NUM_LINES;

	      if (!UpPressed()) up = FALSE;
	      if (wheel_up) wheel_up --;
	    }
	  if (wheel_down || (down && (SDL_GetTicks() - last_movekey_time > wait_move_ticks)))
	    {
	      CapsuleCurRow[YourColor]++;
	      if (CapsuleCurRow[YourColor] > NUM_LINES)
		CapsuleCurRow[YourColor] = 1;

	      if (!DownPressed()) down = FALSE;
	      if (wheel_down) wheel_down --;
	    }

	  if ( set && (NumCapsules[YOU] > 0))
	    {
	      set = FALSE;
	      row = CapsuleCurRow[YourColor] - 1;
	      if ((row >= 0) &&
		  (ToPlayground[YourColor][0][row] != KABELENDE) &&
		  (ActivationMap[YourColor][0][row] == INACTIVE))
		{
		  NumCapsules[YOU]--;
		  CapsuleCurRow[YourColor] = 0;
		  ToPlayground[YourColor][0][row] = VERSTAERKER;
		  ActivationMap[YourColor][0][row] = ACTIVE1;
		  CapsuleCountdown[YourColor][0][row] = CAPSULE_COUNTDOWN * 2;

		  Takeover_Set_Capsule_Sound ();
		}	/* if (row > 0 && ... ) */
	    } /* if ( set ) */

	  ProcessCapsules ();	/* count down the lifetime of the capsules */

	  ProcessPlayground ();
	  ProcessPlayground ();
	  ProcessPlayground ();
	  ProcessPlayground ();	/* this has to be done several times to be sure */

	  ProcessDisplayColumn ();

	} /* if (motion_tick has occurred) */

      ShowPlayground ();

    }	/* while !FinishTakeover */

  /* Schluss- Countdown */
  countdown = CAPSULE_COUNTDOWN;

  while (countdown--)
    {
      while ( SDL_GetTicks() < prev_count_tick + count_tick_len ) ;
      prev_count_tick += count_tick_len;
      ProcessCapsules ();	/* count down the lifetime of the capsules */
      ProcessCapsules ();	/* do it twice this time to be faster */
      AnimateCurrents ();
      ProcessPlayground ();
      ProcessPlayground ();
      ProcessPlayground ();
      ProcessPlayground ();	/* this has to be done several times to be sure */
      ProcessDisplayColumn ();
      ShowPlayground ();

    }	/* while (countdown) */

    return;

} /* PlayGame() */

/*-----------------------------------------------------------------
 * @Desc: animiert Gegner beim Uebernehm-Spiel
 *
 * @Ret: void
 * @Int:
 *-----------------------------------------------------------------*/
void
EnemyMovements (void)
{
  static int Actions = 3;
  static int MoveProbability = 100;
  static int TurnProbability = 10;
  static int SetProbability = 80;

  int action;
  static int direction = 1;	/* start with this direction */
  int row = CapsuleCurRow[OpponentColor] - 1;

  if (NumCapsules[ENEMY] == 0)
    return;


  action = MyRandom (Actions);
  switch (action)
    {
    case 0:			/* Move along */
      if (MyRandom (100) <= MoveProbability)
	{
	  row += direction;
	  if (row > NUM_LINES - 1)
	    row = 0;
	  if (row < 0)
	    row = NUM_LINES - 1;
	}
      break;

    case 1:			/* Turn around */
      if (MyRandom (100) <= TurnProbability)
	{
	  direction *= -1;
	}
      break;

    case 2:			/* Try to set  capsule */
      if (MyRandom (100) <= SetProbability)
	{
	  if ((row >= 0) &&
	      (ToPlayground[OpponentColor][0][row] != KABELENDE) &&
	      (ActivationMap[OpponentColor][0][row] == INACTIVE))
	    {
	      NumCapsules[ENEMY]--;
	      Takeover_Set_Capsule_Sound ();
	      ToPlayground[OpponentColor][0][row] = VERSTAERKER;
	      ActivationMap[OpponentColor][0][row] = ACTIVE1;
	      CapsuleCountdown[OpponentColor][0][row] = CAPSULE_COUNTDOWN;
	      row = -1;		/* For the next capsule: startpos */
	    }
	} /* if MyRandom */

      break;

    default:
      break;

    }	/* switch action */

  CapsuleCurRow[OpponentColor] = row + 1;

  return;
}	/* EnemyMovements */

/*@Function============================================================
 *
 *  define all the SDL_Rects for the takeover-game
 *
 *-----------------------------------------------------------------*/
int
set_takeover_rects (void)
{
  int i,j;
  int curx = 0, cury = 0;

  /* Set the fill-blocks */
  for (i=0; i<NUM_FILL_BLOCKS; i++,curx += TO_FillBlock.w + 2)
    Set_Rect (FillBlocks[i], curx, cury, TO_FillBlock.w, TO_FillBlock.h);

  /* Set the capsule Blocks */
  for (i = 0; i < NUM_CAPS_BLOCKS; i++, curx += TO_CapsuleRect.w + 2)
    Set_Rect (CapsuleBlocks[i], curx, cury, TO_CapsuleRect.w, TO_CapsuleRect.h-2);

  curx = 0;
  cury += TO_FillBlock.h + 2;

  /* get the game-blocks */

  for (j = 0; j < 2*NUM_PHASES; j++)
    {
      for (i = 0; i < TO_BLOCKS; i++)
	{
	  Set_Rect (ToGameBlocks[j*TO_BLOCKS+i], curx, cury, TO_ElementRect.w,TO_ElementRect.h);
	  curx += TO_ElementRect.w + 2;
	}
      curx = 0;
      cury += TO_ElementRect.h + 2;
    }

  /* Get the ground, column and leader blocks */
  for (i = 0; i < NUM_GROUND_BLOCKS; i++)
    {
      Set_Rect (ToGroundBlocks[i], curx, cury, TO_GroundRect.w, TO_GroundRect.h);
      curx += TO_GroundRect.w + 2;
    }
  cury += TO_GroundRect.h + 2;
  curx = 0;

  Set_Rect (ToColumnBlock, curx, cury, TO_ColumnRect.w, TO_ColumnRect.h);

  curx += TO_ColumnRect.w + 2;

  Set_Rect (ToLeaderBlock, curx, cury, 2*TO_LeaderLed.w-4, TO_LeaderLed.h);

  return(OK);

}	// int set_takeover_rects

/*-----------------------------------------------------------------
 * @Desc: prepares _and displays_ the current Playground
 *
 *   NOTE: this function should only change the USERFENSTER part
 *         so that we can do Infoline-setting before this
 *
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
ShowPlayground (void)
{
  int i, j;
  int color, player;
  int block;
  int xoffs, yoffs;
  SDL_Rect dst;

  xoffs = Classic_User_Rect.x;
  yoffs = Classic_User_Rect.y;

  //  SDL_SetColorKey (ne_screen, 0, 0);
  //  SDL_SetClipRect (ne_screen , &User_Rect);
  SDL_SetClipRect (ne_screen , NULL);

  //  Fill_Rect (User_Rect, to_bg_color);
  SDL_BlitSurface (takeover_bg_pic, &User_Rect, ne_screen, &User_Rect);

  PutInfluence (xoffs + DruidStart[YourColor].x, yoffs + DruidStart[YourColor].y);

  if (AllEnemys[DroidNum].status != OUT)
    PutEnemy (DroidNum, xoffs + DruidStart[!YourColor].x, yoffs + DruidStart[!YourColor].y);

  Set_Rect (dst, xoffs + TO_LeftGroundStart.x, yoffs + TO_LeftGroundStart.y, User_Rect.w, User_Rect.h);

  SDL_BlitSurface (to_blocks, &ToGroundBlocks[GELB_OBEN], ne_screen, &dst);

  dst.y += TO_GroundRect.h;

  for (i = 0; i < 12; i++)
    {
      SDL_BlitSurface (to_blocks, &ToGroundBlocks[GELB_MITTE], ne_screen, &dst);

      dst.y += TO_GroundRect.h;
    }				/* for i=1 to 12 */

  SDL_BlitSurface (to_blocks, &ToGroundBlocks[GELB_UNTEN], ne_screen, &dst);


  /* Mittlere Saeule */
  Set_Rect (dst, xoffs + TO_LeaderBlockStart.x, yoffs + TO_LeaderBlockStart.y, 0, 0);
  SDL_BlitSurface (to_blocks, &ToLeaderBlock, ne_screen, &dst);

  dst.y += TO_LeaderLed.h;
  for (i = 0; i < 12; i++, dst.y += TO_ColumnRect.h)
    SDL_BlitSurface (to_blocks, &ToColumnBlock, ne_screen, &dst);


  /* rechte Saeule */
  Set_Rect (dst, xoffs + TO_RightGroundStart.x, yoffs + TO_RightGroundStart.y, 0, 0);

  SDL_BlitSurface (to_blocks, &ToGroundBlocks[VIOLETT_OBEN], ne_screen, &dst);
  dst.y += TO_GroundRect.h;

  for (i = 0; i < 12; i++, dst.y += TO_GroundRect.h)
    SDL_BlitSurface (to_blocks, &ToGroundBlocks[VIOLETT_MITTE], ne_screen, &dst);

  SDL_BlitSurface (to_blocks, &ToGroundBlocks[VIOLETT_UNTEN], ne_screen, &dst);

  /* Fill the Leader-LED with its color */
  Set_Rect (dst, xoffs + TO_LeaderLed.x, yoffs + TO_LeaderLed.y, 0, 0);
  SDL_BlitSurface (to_blocks, &FillBlocks[LeaderColor], ne_screen, &dst);
  dst.y += TO_FillBlock.h;
  SDL_BlitSurface (to_blocks, &FillBlocks[LeaderColor], ne_screen, &dst);

  /* Fill the Display Column with its leds */
  for (i = 0; i < NUM_LINES; i++)
    {
      Set_Rect (dst, xoffs + TO_ColumnStart.x, yoffs + TO_ColumnStart.y + i*TO_ColumnRect.h, 0, 0);
      SDL_BlitSurface (to_blocks, &FillBlocks[DisplayColumn[i]], ne_screen, &dst);
    }

  /* Show the yellow playground */
  for (i = 0; i < NUM_LAYERS - 1; i++)
    for (j = 0; j < NUM_LINES; j++)
      {
	Set_Rect (dst, xoffs + PlaygroundStart[GELB].x + i * TO_ElementRect.w,
		  yoffs + PlaygroundStart[GELB].y + j * TO_ElementRect.h, 0, 0);
	block = ToPlayground[GELB][i][j] + ActivationMap[GELB][i][j]*TO_BLOCKS;
	SDL_BlitSurface (to_blocks, &ToGameBlocks[block], ne_screen, &dst);
      }

  /* Show the violett playground */
  for (i = 0; i < NUM_LAYERS - 1; i++)
    for (j = 0; j < NUM_LINES; j++)
      {
	Set_Rect (dst,
		  xoffs + PlaygroundStart[VIOLETT].x +(NUM_LAYERS-i-2)*TO_ElementRect.w,
		  yoffs + PlaygroundStart[VIOLETT].y + j * TO_ElementRect.h, 0, 0);
	block = ToPlayground[VIOLETT][i][j]+
	  (NUM_PHASES+ActivationMap[VIOLETT][i][j])*TO_BLOCKS;
	SDL_BlitSurface (to_blocks, &ToGameBlocks[block],ne_screen, &dst);
      }

  /* Show the capsules left for each player */
  for (player = 0; player < 2; player++)
    {
      if (player == YOU)
	color = YourColor;
      else
	color = OpponentColor;

      Set_Rect (dst, xoffs + CurCapsuleStart[color].x,
		yoffs + CurCapsuleStart[color].y + CapsuleCurRow[color] * TO_CapsuleRect.h,
		0,0);
      if (NumCapsules[player])
	SDL_BlitSurface (to_blocks, &CapsuleBlocks[color], ne_screen, &dst);


      for (i = 0; i < NumCapsules[player]-1; i++)
	{
	  Set_Rect (dst, xoffs + LeftCapsulesStart[color].x,
		    yoffs + LeftCapsulesStart[color].y + i*TO_CapsuleRect.h, 0, 0);
	  SDL_BlitSurface (to_blocks, &CapsuleBlocks[color],
			   ne_screen, &dst);
	} /* for capsules */
    } /* for player */

  SDL_Flip (ne_screen);

  // give CPU some air, unless requested otherwise
  if (!GameConfig.HogCPU)
    SDL_Delay(1);

  return;

}				/* ShowPlayground */


/*-----------------------------------------------------------------
 * @Desc: Clears Playground (and ActivationMap) to default start-values
 * @Ret:  void
 *
 *-----------------------------------------------------------------*/
void
ClearPlayground (void)
{
  int color, layer, row;

  for (color = GELB; color < TO_COLORS; color++)
    for (layer = 0; layer < NUM_LAYERS; layer++)
      for (row = 0; row < NUM_LINES; row++)
	{
	  ActivationMap[color][layer][row] = INACTIVE;
	  if (layer < TO_COLORS - 1)
	    ToPlayground[color][layer][row] = KABEL;
	  else
	    ToPlayground[color][layer][row] = INACTIVE;
	}

  for (row = 0; row < NUM_LINES; row++)
    DisplayColumn[row] = row % 2;

}				/* ClearPlayground */


/*-----------------------------------------------------------------
 * @Desc: generate a random Playground
 *
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
InventPlayground (void)
{
  int anElement;
  int newElement;
  int row, layer;
  int color = GELB;

  /* first clear the playground: we depend on this !! */
  ClearPlayground ();

  for (color = GELB; color < TO_COLORS; color++)
    {
      for (layer = 1; layer < NUM_LAYERS - 1; layer++)
	{
	  for (row = 0; row < NUM_LINES; row++)
	    {
	      if (ToPlayground[color][layer][row] != KABEL)
		continue;

	      newElement = MyRandom (TO_ELEMENTS-1);
	      if (MyRandom (MAX_PROB) > ElementProb[newElement])
		{
		  row--;
		  continue;
		}

	      switch (newElement)
		{
		case EL_KABEL:	/* has not to be set any more */
		  anElement = ToPlayground[color][layer - 1][row];
		  if (BlockClass[anElement] == NON_CONNECTOR)
		    ToPlayground[color][layer][row] = LEER;
		  break;

		case EL_KABELENDE:
		  anElement = ToPlayground[color][layer - 1][row];
		  if (BlockClass[anElement] == NON_CONNECTOR)
		    ToPlayground[color][layer][row] = LEER;
		  else
		    ToPlayground[color][layer][row] = KABELENDE;
		  break;

		case EL_VERSTAERKER:
		  anElement = ToPlayground[color][layer - 1][row];
		  if (BlockClass[anElement] == NON_CONNECTOR)
		    ToPlayground[color][layer][row] = LEER;
		  else
		    ToPlayground[color][layer][row] = VERSTAERKER;
		  break;

		case EL_FARBTAUSCHER:
		  if (layer != 2)
		    {		/* only existing on layer 2 */
		      row--;
		      continue;
		    }

		  anElement = ToPlayground[color][layer - 1][row];
		  if (BlockClass[anElement] == NON_CONNECTOR)
		    ToPlayground[color][layer][row] = LEER;
		  else
		    ToPlayground[color][layer][row] = FARBTAUSCHER;
		  break;

		case EL_VERZWEIGUNG:
		  if (row > NUM_LINES - 3)
		    {
		      /* try again */
		      row--;
		      break;
		    }

		  anElement = ToPlayground[color][layer - 1][row + 1];
		  if (BlockClass[anElement] == NON_CONNECTOR)
		    {
		      /* try again */
		      row--;
		      break;
		    }

		  /* dont destroy verzweigungen in prev. layer */
		  anElement = ToPlayground[color][layer - 1][row];
		  if (anElement == VERZWEIGUNG_O
		      || anElement == VERZWEIGUNG_U)
		    {
		      row--;
		      break;
		    }
		  anElement = ToPlayground[color][layer - 1][row + 2];
		  if (anElement == VERZWEIGUNG_O
		      || anElement == VERZWEIGUNG_U)
		    {
		      row--;
		      break;
		    }

		  /* cut off kabels in last layer, if any */
		  anElement = ToPlayground[color][layer - 1][row];
		  if (BlockClass[anElement] == CONNECTOR)
		    ToPlayground[color][layer - 1][row] = KABELENDE;

		  anElement = ToPlayground[color][layer - 1][row + 2];
		  if (BlockClass[anElement] == CONNECTOR)
		    ToPlayground[color][layer - 1][row + 2] = KABELENDE;

		  /* set the verzweigung itself */
		  ToPlayground[color][layer][row] = VERZWEIGUNG_O;
		  ToPlayground[color][layer][row + 1] = VERZWEIGUNG_M;
		  ToPlayground[color][layer][row + 2] = VERZWEIGUNG_U;

		  row += 2;
		  break;

		case EL_GATTER:
		  if (row > NUM_LINES - 3)
		    {
		      /* try again */
		      row--;
		      break;
		    }

		  anElement = ToPlayground[color][layer - 1][row];
		  if (BlockClass[anElement] == NON_CONNECTOR)
		    {
		      /* try again */
		      row--;
		      break;
		    }
		  anElement = ToPlayground[color][layer - 1][row + 2];
		  if (BlockClass[anElement] == NON_CONNECTOR)
		    {
		      /* try again */
		      row--;
		      break;
		    }


		  /* cut off kabels in last layer, if any */
		  anElement = ToPlayground[color][layer - 1][row + 1];
		  if (BlockClass[anElement] == CONNECTOR)
		    ToPlayground[color][layer - 1][row + 1] = KABELENDE;

		  /* set the GATTER itself */
		  ToPlayground[color][layer][row] = GATTER_O;
		  ToPlayground[color][layer][row + 1] = GATTER_M;
		  ToPlayground[color][layer][row + 2] = GATTER_U;

		  row += 2;
		  break;

		default:
                  DebugPrintf ( 0, "ERROR: unknown element '%d' drawn (must be within [0,%d])\n", newElement, TO_ELEMENTS-1 );
                  row --;
		  break;

		}		/* switch NewElement */

	    }			/* for row */

	}			/* for layer */

    }				/* for color */

}				/* InventPlayground */

/*-----------------------------------------------------------------
 * @Desc: process the playground following its intrinsic logic
 *
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
ProcessPlayground (void)
{
  int color, layer, row;
  int TurnActive = FALSE;

  for (color = GELB; color < TO_COLORS; color++)
    {
      for (layer = 1; layer < NUM_LAYERS; layer++)
	{
	  for (row = 0; row < NUM_LINES; row++)
	    {
	      if (layer == NUM_LAYERS - 1)
		{
		  if (IsActive (color, row))
		    ActivationMap[color][layer][row] = ACTIVE1;
		  else
		    ActivationMap[color][layer][row] = INACTIVE;

		  continue;
		}		/* if last layer */

	      TurnActive = FALSE;

	      switch (ToPlayground[color][layer][row])
		{
		case FARBTAUSCHER:
		case VERZWEIGUNG_M:
		case GATTER_O:
		case GATTER_U:
		case KABEL:
		  if (ActivationMap[color][layer - 1][row] >= ACTIVE1)
		    TurnActive = TRUE;
		  break;

		case VERSTAERKER:
		  if (ActivationMap[color][layer - 1][row] >= ACTIVE1)
		    TurnActive = TRUE;

		  /* Verstaerker halten sich aber auch selbst aktiv !! */
		  if (ActivationMap[color][layer][row] >= ACTIVE1)
		    TurnActive = TRUE;

		  break;

		case KABELENDE:
		  break;

		case VERZWEIGUNG_O:
		  if (ActivationMap[color][layer][row + 1] >= ACTIVE1)
		    TurnActive = TRUE;
		  break;

		case VERZWEIGUNG_U:
		  if (ActivationMap[color][layer][row - 1] >= ACTIVE1)
		    TurnActive = TRUE;
		  break;

		case GATTER_M:
		  if ((ActivationMap[color][layer][row - 1] >= ACTIVE1)
		      && (ActivationMap[color][layer][row + 1] >= ACTIVE1))
		    TurnActive = TRUE;

		  break;

		default:
		  break;
		}		/* switch */

	      if (TurnActive)
		{
		  if (ActivationMap[color][layer][row] == INACTIVE)
		    ActivationMap[color][layer][row] = ACTIVE1;
		  TurnActive = FALSE;
		}
	      else
		ActivationMap[color][layer][row] = INACTIVE;


	    }			/* for row */

	}			/* for layer */

    }				/* for color */

  return;
}				/* ProcessPlayground */



/*@Function============================================================
@Desc:  ProcessDisplayColumn(): setzt die Korrekten Werte in der Display-
        Saeule. Blinkende LEDs werden ebenfalls hier realisiert

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
ProcessDisplayColumn (void)
{
  static int CLayer = 3;	/* the connection-layer to the Column */
  static int flicker_color = 0;
  int row;
  int GelbCounter, ViolettCounter;

  flicker_color = !flicker_color;

  for (row = 0; row < NUM_LINES; row++)
    {
      /* eindeutig gelb */
      if ((ActivationMap[GELB][CLayer][row] >= ACTIVE1) &&
	  (ActivationMap[VIOLETT][CLayer][row] == INACTIVE))
	{
	  /* Farbtauscher ??? */
	  if (ToPlayground[GELB][CLayer - 1][row] == FARBTAUSCHER)
	    DisplayColumn[row] = VIOLETT;
	  else
	    DisplayColumn[row] = GELB;
	  continue;
	}

      /* eindeutig violett */
      if ((ActivationMap[GELB][CLayer][row] == INACTIVE) &&
	  (ActivationMap[VIOLETT][CLayer][row] >= ACTIVE1))
	{
	  /* Farbtauscher ??? */
	  if (ToPlayground[VIOLETT][CLayer - 1][row] == FARBTAUSCHER)
	    DisplayColumn[row] = GELB;
	  else
	    DisplayColumn[row] = VIOLETT;

	  continue;
	}

      /* unentschieden: Flimmern */
      if ((ActivationMap[GELB][CLayer][row] >= ACTIVE1) &&
	  (ActivationMap[VIOLETT][CLayer][row] >= ACTIVE1))
	{
	  /* Farbtauscher - Faelle */
	  if ((ToPlayground[GELB][CLayer - 1][row] == FARBTAUSCHER) &&
	      (ToPlayground[VIOLETT][CLayer - 1][row] != FARBTAUSCHER))
	    DisplayColumn[row] = VIOLETT;
	  else if ((ToPlayground[GELB][CLayer - 1][row] != FARBTAUSCHER) &&
		   (ToPlayground[VIOLETT][CLayer - 1][row] == FARBTAUSCHER))
	    DisplayColumn[row] = GELB;
	  else
	    {
	      if (flicker_color == 0)
		DisplayColumn[row] = GELB;
	      else
		DisplayColumn[row] = VIOLETT;
	    }			/* if - else if - else */

	}			/* if unentschieden */

    }				/* for */

  /* Win Color beurteilen */
  GelbCounter = 0;
  ViolettCounter = 0;
  for (row = 0; row < NUM_LINES; row++)
    if (DisplayColumn[row] == GELB)
      GelbCounter++;
    else
      ViolettCounter++;

  if (ViolettCounter < GelbCounter)
    LeaderColor = GELB;
  else if (ViolettCounter > GelbCounter)
    LeaderColor = VIOLETT;
  else
    LeaderColor = REMIS;

  //--------------------
  // In Freedroid, the resistance a droid against the influencers control should
  // depend on the details of the final takeover score.  Therefore we set this
  // resistance factor variable here.
  //
  // RP: yes, but as this is now a purely "classic" release, such extensions are
  // deactivated...
  //  Me.Current_Victim_Resistance_Factor = 0.2 * ( (float) 12 - abs( ViolettCounter- GelbCounter ) );

  return;
}; // ProcessDisplayColumn

/*@Function============================================================
@Desc: ProcessCapsules():	does the countdown of the capsules and
									kills them if too old

@Ret: void
@Int:
* $Function----------------------------------------------------------*/
void
ProcessCapsules (void)
{
  int row;
  int color;

  for (color = GELB; color <= VIOLETT; color++)
    for (row = 0; row < NUM_LINES; row++)
      {
	if (CapsuleCountdown[color][0][row] > 0)
	  CapsuleCountdown[color][0][row]--;

	if (CapsuleCountdown[color][0][row] == 0)
	  {
	    CapsuleCountdown[color][0][row] = -1;
	    ActivationMap[color][0][row] = INACTIVE;
	    ToPlayground[color][0][row] = KABEL;
	  }

      } /* for row */

}   /* ProcessCapsules() */


/*@Function============================================================
@Desc: IsInactive(color, row): tells, wether a Column-connection
						is active or not

@Ret: TRUE/FALSE
@Int:
* $Function----------------------------------------------------------*/
int
IsActive (int color, int row)
{
  int CLayer = 3;		/* the connective Layer */
  int TestElement = ToPlayground[color][CLayer - 1][row];

  if ((ActivationMap[color][CLayer-1][row] >= ACTIVE1) &&
      (BlockClass[TestElement] == CONNECTOR))
    return TRUE;
  else
    return FALSE;
}				/* IsActive */

/*-----------------------------------------------------------------
 *
 * Animate the active cables: this is done by cycling over
 * the active phases ACTIVE1-ACTIVE3, which are represented by
 * different pictures in the playground
 *
 *-----------------------------------------------------------------*/
void
AnimateCurrents (void)
{
  int color, layer, row;

  for (color = GELB; color <= VIOLETT; color ++)
    for (layer = 0; layer < NUM_LAYERS; layer ++)
      for (row = 0; row < NUM_LINES; row ++)
	if (ActivationMap[color][layer][row] >= ACTIVE1)
	  {
	    ActivationMap[color][layer][row] ++;
	    if (ActivationMap[color][layer][row] == NUM_PHASES)
	      ActivationMap[color][layer][row] = ACTIVE1;
	  }

  return;
}


#undef _takeover_c
