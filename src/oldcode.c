void 
PrepareScaledSurfaceNo(void)
{
  int x;
  int y;
  byte SourcePixel;

#define BYTES_PER_PIXEL

  Lock_SDL_Screen();
  SDL_LockSurface( ScaledSurface );





  for ( y = 0; y < SCREENHOEHE; y++ )
    {
      for ( x = 0; x< SCREENBREITE; x++ )
	{
	  SourcePixel=getpixel(screen, x, y);
	  
	  switch ( SCALE_FACTOR ) 
	    {
	    case 1:
	      putpixel ( ScaledSurface , x     , y     , SourcePixel );
	      break;
	    case 2:
	      putpixel ( ScaledSurface , x*2   , y*2   , SourcePixel );
	      putpixel ( ScaledSurface , x*2+1 , y*2   , SourcePixel );
	      putpixel ( ScaledSurface , x*2   , y*2+1 , SourcePixel );
	      putpixel ( ScaledSurface , x*2+1 , y*2+1 , SourcePixel );
	      break;
	    case 3:
	      putpixel ( ScaledSurface , x*3     , y*3   , SourcePixel );
	      putpixel ( ScaledSurface , x*3+1   , y*3   , SourcePixel );
	      putpixel ( ScaledSurface , x*3     , y*3+1 , SourcePixel );
	      putpixel ( ScaledSurface , x*3+1   , y*3+1 , SourcePixel );
	      putpixel ( ScaledSurface , x*3  +2 , y*3   , SourcePixel );
	      putpixel ( ScaledSurface , x*3  +2 , y*3+1 , SourcePixel );
	      putpixel ( ScaledSurface , x*3  +1 , y*3+2 , SourcePixel );
	      putpixel ( ScaledSurface , x*3     , y*3+2 , SourcePixel );
	      putpixel ( ScaledSurface , x*3  +2 , y*3+2 , SourcePixel );
	      break;
	    default:
	      printf("\n\nvoid PrepareScaledSurface(): Unhandled SCALE_FACTOR...\n\nTerminating...\n\n");
	      break;
	    }
	}
    }

  Unlock_SDL_Screen();
  SDL_UnlockSurface( ScaledSurface );

  Update_SDL_Screen();
}

/*-----------------------------------------------------------------
 * @Desc: doesnt really _swap_ anything, but copies InternalScreen
 *        onto the Real vga screen
 *
 *-----------------------------------------------------------------*/
void
SwapScreen (void)
{

#ifdef DRAW_TO_SCREEN_VARIABLE
  int x;
  int y;


  Lock_SDL_Screen();

  for (y = 0; y < SCREENHOEHE; y++)
    {
      for (x = 0; x< SCREENBREITE; x++)
	{
	  putpixel ( screen, x, y, *(InternalScreen + SCREENBREITE * y + x) );
	}
    }

  Unlock_SDL_Screen();

  Update_SDL_Screen();

  return;
#endif

  memcpy( Outline320x200, InternalScreen, SCREENBREITE * SCREENHOEHE );

} /* SwapScreen() */

/*-----------------------------------------------------------------
 * @Desc: 
 * @Ret: 
 *
 *-----------------------------------------------------------------*/
void
ClearVGAScreen (void)
{

#ifdef DRAW_TO_SCREEN_VARIABLE

  char *LocalBlackLinePointer;
  int y;
  int x;



  LocalBlackLinePointer = malloc (SCREENBREITE + 10);
  memset (LocalBlackLinePointer, 0, SCREENBREITE);

  Lock_SDL_Screen();

  for (y = 0; y < SCREENHOEHE; y++)
    {
      for (x=0; x<SCREENBREITE; x++) 
	{
	  putpixel ( screen , x , y , 0 );
	}
    }

  Unlock_SDL_Screen();

  free (LocalBlackLinePointer);

  return;
#endif

  memset( Outline320x200, 0, SCREENBREITE * SCREENHOEHE );

} // void ClearVGAScreen(void)

/*-----------------------------------------------------------------
 * @Desc: gibt Block *block (len*height) an angegebener
 * Bildschirmposition x/y aus auf screen
 *						
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
DisplayBlock (int x, int y,
	      unsigned char *block,
	      int len, int height, unsigned char *Parameter_screen)
{
  int row, i, j;
  unsigned char *screenpos;
  unsigned char *source = block;


  /*
   * THIS IS NEW FROM AFTER THE PORT, BECAUSE 'REALSCREEN' IS NO LONGER
   * DIRECTLY ACCESSIBLE 
   */

  

  if (Parameter_screen == RealScreen) 
    {

      screenpos = Outline320x200 + y * SCREENLEN + x;
      
      for (i = 0; i < height; i++)

#ifdef DRAW_TO_SCREEN_VARIABLE

      Lock_SDL_Screen();

	for (j = 0; j < len; j++)
	  {
	    // SDL vga_setcolor (*source);
	    putpixel ( screen, j + x, i + y, *source );
	    source++;
	  }			/* for j */

      Unlock_SDL_Screen();

#else
      memcpy (screenpos, source, len);
      screenpos += SCREENLEN;
      source += len;
#endif

    }
  else
    {
      screenpos = Parameter_screen + y * SCREENLEN + x;

      for (row = 0; row < height; row++)
	{
	  memcpy (screenpos, source, len);
	  screenpos += SCREENLEN;
	  source += len;
	}			/* for row */
    }				/* else */

  return;

}				/* DisplayBlock */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SmallBlock (int LX, int LY, int BlockN, unsigned char *Parameter_Screen, int SBreite)
{
  int i, j;
  unsigned char *source = MapBlocks + BLOCKBREITE * BLOCKHOEHE * BlockN;
  unsigned char *target = Parameter_Screen + LY * SBreite + LX;

  if (Parameter_Screen == RealScreen)
    target=Outline320x200 + LY * SBreite + LX;

  //DebugPrintf("\nvoid SmallBlock(...): Function call confirmed.");
  if (LX > USERFENSTERPOSX + USERFENSTERBREITE)
    return;

  Lock_SDL_Screen();

  for (i = 0; i < 8; i++)
    {
      for (j = 0; j < 8; j++)
	{
	  *target = *source;
	  target++;
	  if (Parameter_Screen == RealScreen)
	    {
#ifdef DRAW_TO_SCREEN_VARIABLE
	      putpixel (screen, LX + i, LY + j, *source );
#else
	      *target=*source;
#endif
	    }
	  source += 4;
	  //Screen[LX+j+(LY+i)*SBreite]=
	  //MapBlocks[BlockN*BLOCKBREITE*BLOCKHOEHE+j*4+i*BLOCKBREITE*4];
	}
      target += SBreite - 8;
      source += 4 * BLOCKBREITE - 4 * 8;
    }

  Unlock_SDL_Screen();

  //DebugPrintf("\nvoid SmallBlock(...): Usual end of function reached.");
}				// void SmallBlock(...)

/*-----------------------------------------------------------------
 * @Desc: setzt Block *block (len*height) an angegebener
 *	Bildschirmposition x/y in den angeg. Bildschirm
 *	-beachtet dabei TRANSPARENTCOLOR 
 *						
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
void
DisplayMergeBlock (int x, int y, unsigned char *block,
		   int len, int height, unsigned char *Parameter_screen)
{
  int row, col;
  unsigned char *Screenpos;
  unsigned char *source = block;

  Screenpos = Parameter_screen + x + y * SCREENBREITE;

  if (Parameter_screen == NULL)
    {
      return;
    }


  if (Parameter_screen == RealScreen)
    {
      Screenpos = Outline320x200 + x + y * SCREENBREITE;
    }


  /* PORT: we do as Johannes did in DisplayBlock(): */
  /*
  if (Parameter_screen == RealScreen)
    {

      Lock_SDL_Screen();

      for (col = 0; col < height; col++)
	{
	  for (row = 0; row < len; row++)
	    {
	      // SDL vga_setcolor (*source);
	      if (*source != TRANSPARENTCOLOR)
		// putpixel (screen, x + row, y + col, *source );
		putpixel (screen, x + row, y + col, *source );
	      source++;
	    } // for row 
	} // for col 

      Unlock_SDL_Screen();

      SDL_UpdateRect(screen, x, y, len, height);
    }
else
    */

    for (row = 0; row < height; row++)
      {
	for (col = 0; col < len; col++)
	  {
	    if (*source != TRANSPARENTCOLOR)
	      *Screenpos++ = *source++;
	    else
	      {
		Screenpos++;
		source++;
	      }
	  }			/* for (col) */
	Screenpos += SCREENBREITE - len;
      }				/* for (row) */

  return;

}				/* DisplayMergeBlock */

/*@Function============================================================
@Desc: 	Diese Prozedur schreibt das im Speicher zusammengebaute Bild
			in den Bildschirmspeicher.

		   Parameter: keine
@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
PutInternFenster (void)
{
  int StartX, StartY;
  int i;
#ifdef DRAW_TO_SCREEN_VARIABLE
  int j;
#endif
#ifdef SLOW_VIDEO_CALLS
  int j;
#endif
  unsigned char *source;
  unsigned char *target;

  DebugPrintf ("void PutInternFenster(void) wurde ECHT aufgerufen...");

  if (Conceptview)
    {
      for (i = 0; i < USERFENSTERHOEHE; i++)
	{
	  memcpy (RealScreen + (USERFENSTERPOSY + i) * SCREENBREITE +
		  USERFENSTERPOSX,
		  InternWindow + i * INTERNBREITE * BLOCKBREITE,
		  USERFENSTERBREITE);
	}
      return;
    }

  StartX = (((int) Me.pos.x) % BLOCKBREITE) - BLOCKBREITE / 2;
  StartY =
    ((((int) Me.pos.y) % BLOCKHOEHE) -
     BLOCKHOEHE / 2) * BLOCKBREITE * INTERNBREITE;

  WaitVRetrace ();		//

  DisplayRahmen ( RealScreen );

  Lock_SDL_Screen();

  for (i = 0; i < USERFENSTERHOEHE; i++)
    {
      source = InternWindow +
	BLOCKBREITE * (INTERNBREITE - VIEWBREITE) / 2 +
	INTERNBREITE * BLOCKBREITE * (BLOCKHOEHE *
				      (INTERNHOEHE - VIEWHOEHE)) / 2 +
	// USERFENSTEROBEN*INTERNBREITE*BLOCKBREITE + 
	//       USERFENSTERLINKS +
	StartY + StartX + i * INTERNBREITE * BLOCKBREITE;
      target = Outline320x200 + USERFENSTERPOSX + (USERFENSTERPOSY+i) * SCREENBREITE;

#define SLOW_VIDEO_CALLS
#ifdef SLOW_VIDEO_CALLS

#undef DRAW_TO_SCREEN_VARIABLE
#ifdef DRAW_TO_SCREEN_VARIABLE
      for (j = 0; j < USERFENSTERBREITE; j++)
	{
	  // SDL vga_setcolor (*source);
	  source++;
	  putpixel (screen, USERFENSTERPOSX + j, USERFENSTERPOSY + i, *source );
	}			// for(j=0; ...
#else
      memcpy(target, source, USERFENSTERBREITE);
#endif

#else
      vga_drawscansegment (source, USERFENSTERPOSX, USERFENSTERPOSY + i,
			   USERFENSTERBREITE);
      // source+=USERFENSTERBREITE;
#endif
    }				// for(i=0; ...


  Unlock_SDL_Screen();

  // Update_SDL_Screen();

  PrepareScaledSurface();


};				// void PutInternFenster(void)

void 
MakeGridOnScreen(unsigned char* Parameter_Screen){
  int x,y;

  Lock_SDL_Screen();

  for (y=0; y<SCREENHOEHE; y++) 
    {
      for (x=0; x<SCREENBREITE; x++) 
	{
	  if ((x+y)%2 == 0) 
	    {
	      putpixel(screen, x, y, 0);
	    }
	}
    }

  Unlock_SDL_Screen();

} // void MakeGridOnSchreen(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Play_YIFF_Server_Sound (int Tune)
{
  YEventSoundPlay Music_Parameters;


  if ( !sound_on ) return;

  /* This function can and should only be compiled on machines, that have */
  /* the YIFF sound server installed.  Compilation therefore is optional and */
  /* can be toggled with the following  definition.*/

#if HAVE_LIBY2
  DebugPrintf
    ("\nvoid Play_YIFF_Server_Sound(int Tune):  Real function call confirmed.");
  DebugPrintf
    ("\nvoid Play_YIFF_Server_Sound(int Tune):  Playback is about to start!");


/*
  This part takes A LOT OF TIME and destroys game flow.
  Therefore it remains commented out

  if (YGetSoundObjectAttributes
      (BackgroundMusic_con, ExpandedSoundSampleFilenames[Tune],
       &BackgroundMusic_sndobj_attrib))
    {
      DebugPrintf
	("\nvoid Switch_Background_Music_To(int Tune):  Error: Missing or corrupt.\n");
      // Can't get sound object attributes.
      fprintf (stderr,
	       "\nvoid Play_YIFF_Server_Sound(int Tune): %s: Error: Missing or corrupt.\n",
	       ExpandedSoundSampleFilenames[Tune]);
      printf (" CWD: %s \n\n", getcwd (NULL, 0));
      Terminate (ERR);
    }
  else
    {

      DebugPrintf 
	("\nvoid Play_YIFF_Server_Sound(int Tune):  Now starting new background tune...\n");
      DebugPrintf 
	("\nvoid Play_YIFF_Server_Sound(int Tune):  The following file will be loaded: ");
      DebugPrintf (ExpandedSoundSampleFilenames[ Tune ]);

      BackgroundMusic_play_id = YStartPlaySoundObjectSimple (BackgroundMusic_con, 
							     ExpandedSoundSampleFilenames[ Tune ] );
      DebugPrintf ("\nvoid Play_YIFF_Server_Sound(int Tune):  Tune has been loaded: ");
      DebugPrintf ( ExpandedSoundSampleFilenames[ Tune ] );

      Music_Parameters.repeats = 0;
      Music_Parameters.total_repeats = 1;	// -1 here means to repeat indefinately
      Music_Parameters.left_volume = 0.5;
      Music_Parameters.right_volume = 0.5;
      Music_Parameters.sample_rate = BackgroundMusic_sndobj_attrib.sample_rate;
      Music_Parameters.length = BackgroundMusic_sndobj_attrib.sample_size;
      Music_Parameters.position = 0;
      Music_Parameters.yid = BackgroundMusic_play_id;
      Music_Parameters.flags = 0xFFFFFFFF;


      YSetPlaySoundObjectValues (BackgroundMusic_con, BackgroundMusic_play_id,
				 &Music_Parameters);

      DebugPrintf
	("\nvoid Switch_Background_Music_To(int Tune):  New tune should be played endlessly now.\n");

    }
*/


  Music_Parameters.repeats = 0;
  Music_Parameters.total_repeats = 1;	// -1 here means to repeat indefinately
  Music_Parameters.left_volume = Current_Sound_FX_Volume;
  Music_Parameters.right_volume = Current_Sound_FX_Volume;
  Music_Parameters.sample_rate =
    BackgroundMusic_sndobj_attrib.sample_rate;
  Music_Parameters.length = BackgroundMusic_sndobj_attrib.sample_size;
  Music_Parameters.position = 0;
  Music_Parameters.yid = BackgroundMusic_play_id;
  Music_Parameters.flags = YPlayValuesFlagVolume;

  //  YSetPlaySoundObjectValues (BackgroundMusic_con, play_id, &Music_Parameters);

  play_id = YStartPlaySoundObject(BackgroundMusic_con, ExpandedSoundSampleFilenames[Tune], &Music_Parameters);

  /*
  play_id =
    YStartPlaySoundObjectSimple (BackgroundMusic_con,
  			 ExpandedSoundSampleFilenames[Tune]);
  */

  /*
  Music_Parameters.repeats = 0;
  Music_Parameters.total_repeats = 1;	// -1 here means to repeat indefinately
  Music_Parameters.left_volume = Current_Sound_FX_Volume;
  Music_Parameters.right_volume = Current_Sound_FX_Volume;
  Music_Parameters.sample_rate =
    BackgroundMusic_sndobj_attrib.sample_rate;
  Music_Parameters.length = BackgroundMusic_sndobj_attrib.sample_size;
  Music_Parameters.position = 0;
  Music_Parameters.yid = play_id;
  Music_Parameters.flags = YPlayValuesFlagVolume;
  YSetPlaySoundObjectValues (BackgroundMusic_con, play_id, &Music_Parameters);
  &*/

#endif /* HAVE_LIBY2 */

}  // void Play_YIFF_Server_Sound(int Tune)
