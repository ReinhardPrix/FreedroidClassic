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

