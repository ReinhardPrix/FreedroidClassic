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

