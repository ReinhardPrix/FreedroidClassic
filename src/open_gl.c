/*
 *
 *   Copyright (c) 2003 Johannes Prix
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
 * This file contains function relevant for OpenGL based graphics output.
 * ---------------------------------------------------------------------- */

#define _open_gl_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "SDL_rotozoom.h"

/* ----------------------------------------------------------------------
 * This is a wrapper for the SDL_Flip function, that will use either the
 * OpenGL buffer-swapping or the classic SDL flipper, depending on the
 * current output method, like OpenGL or not.
 * ---------------------------------------------------------------------- */
int 
our_SDL_flip_wrapper ( SDL_Surface *screen )
{
    if ( use_open_gl )
	SDL_GL_SwapBuffers( ); 
    else
	return ( SDL_Flip ( screen ) ) ;
    
    return ( 0 );
}; // int our_SDL_flip_wrapper ( SDL_Surface *screen )

/* ----------------------------------------------------------------------
 * Here comes our SDL wrapper, that will either do a normal SDL blit or,
 * if OpenGL is present and enabled, use OpenGL to draw the scene.
 * ---------------------------------------------------------------------- */
int
our_SDL_blit_surface_wrapper(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
#ifdef HAVE_LIBGL
  int bytes;
  int target_x, target_y ;
#endif

  if ( use_open_gl )
    {
#ifdef HAVE_LIBGL
      if ( src == NULL ) 
	{
	  DebugPrintf ( -4 , "\nNull source surface received. --> doing nothing." );
	  fflush ( stdout );
	  raise ( SIGSEGV );
	  return ( 0 );
	}

      if ( dst == Screen )
	{
	  
	  // DebugPrintf ( -1 , "\nReached our_SDL_blit_surface_wrapper." );
	  // fflush ( stdout );

	  if ( dstrect == NULL )
	    // glRasterPos2f( 0 , 480 );
	    glRasterPos2f( 1 , 479 );
	  else
	    {
	      if ( dstrect -> x == 0 )
		target_x = 1 ;
	      else
		target_x = dstrect -> x ;

	      target_y = dstrect -> y + src -> h ;
	      // target_y = 479 - dstrect -> y ;

	      //--------------------
	      // Here we add some extra security against raster positions (e.g. for
	      // character screen and the like) SLIGHTLY out of bounds, which would
	      // cause the entire blit to be canceled due to OpenGL internal policy.
	      //
	      if ( ( target_y >= 480 ) && ( target_y <= 482 ) ) target_y = 478 ; 

	      glRasterPos2i( target_x , target_y ) ;
	    }

	  if ( src -> w != 0 )
	    bytes = src -> pitch / src -> w ;
	  else
	    {
	      DebugPrintf ( -4 , "\nSurface of width 0 encountered. --> doing nothing." );
	      fflush ( stdout );
	      return ( 0 ) ;
	    }

	  // DebugPrintf ( -1 , "\nSurface has bytes: %d. " , bytes );
	  // fflush ( stdout );
	  
	  if ( srcrect != NULL )
	    {
	      DebugPrintf ( -4 , "\nNon-Null source rect encountered. --> doing nothing." );
	      fflush ( stdout );
	      return ( 0 ) ;
	    }

	  if ( bytes == 4 )
	    {
	      glEnable( GL_ALPHA_TEST );  
	      glAlphaFunc ( GL_GREATER , 0.5 ) ;
	      glDrawPixels( src -> w , src -> h, GL_BGRA , GL_UNSIGNED_BYTE , src -> pixels );
	    }
	  else if ( bytes == 3 )
	    {
	      DebugPrintf ( 1 , "\nSurface has bytes: %d. " , bytes );
	      fflush ( stdout );
	      glDrawPixels( src -> w , src -> h, GL_RGB , GL_UNSIGNED_BYTE , src -> pixels );
	    }
	  else if ( bytes == 2 )
	    {
	      DebugPrintf ( 1 , "\nSurface has bytes: %d. --> using GL_UNSIGNED_SHORT_5_6_5. " , bytes );
	      fflush ( stdout );
	      glDrawPixels( src -> w , src -> h, GL_RGB , GL_UNSIGNED_SHORT_5_6_5 , src -> pixels );
	    }
	  else
	    {
	      DebugPrintf ( -4 , "\nSurface has bytes: %d.--> doing nothing. " , bytes );
	      fflush ( stdout );
	    }

	  return ( 0 ) ;
	}

      return SDL_BlitSurface ( src, srcrect, dst, dstrect);
#endif
    }
  else
    {
      return SDL_BlitSurface ( src, srcrect, dst, dstrect);
    }

  return 0 ;

}; // void our_SDL_blit_surface_wrapper(image, NULL, Screen, NULL)

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void 
our_SDL_update_rect_wrapper ( SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h ) 
{
  if ( use_open_gl )
    {
      our_SDL_flip_wrapper ( screen ) ;
    }
  else
    {
      SDL_UpdateRect ( screen, x, y, w, h ) ;
    }
}; // void our_SDL_update_rect_wrapper ( SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
int 
our_SDL_fill_rect_wrapper (SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color)
{
#ifdef HAVE_LIBGL
  Uint8 r , g , b , a ;
#endif

  if ( use_open_gl )
    {
#ifdef HAVE_LIBGL
      if ( dst == Screen )
	{
	  glDisable( GL_TEXTURE_2D );
	  glDisable(GL_DEPTH_TEST);
	  glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
	  glDisable( GL_ALPHA_TEST );  
	  glEnable(GL_BLEND);
	  glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
	  
	  SDL_GetRGBA( color, Screen -> format , &r, &g, &b, &a);
	  glRasterPos2i ( 0 , 0 ); 
	  glColor4ub( r , g , b , a );
	  if ( dstrect == NULL )
	    {
	      glBegin(GL_QUADS);
	      glVertex2i( 0       , 480 );
	      glVertex2i( 0       ,   0 );
	      glVertex2i( 0 + 640 ,   0 );
	      glVertex2i( 0 + 640 , 480 );
	      glEnd( );
	    }
	  else
	    {
	      glBegin(GL_QUADS);
	      glVertex2i( dstrect -> x                , dstrect -> y );
	      glVertex2i( dstrect -> x                , dstrect -> y + dstrect -> h );
	      glVertex2i( dstrect -> x + dstrect -> w , dstrect -> y + dstrect -> h );
	      glVertex2i( dstrect -> x + dstrect -> w , dstrect -> y );
	      glEnd( );
	    }
	  return ( 0 );
	}
#endif
    }

  return ( SDL_FillRect ( dst, dstrect, color) ) ;

}; // int our_SDL_fill_rect_wrapper (SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color)

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
SDL_Surface*
our_SDL_display_format_wrapper ( SDL_Surface *surface )
{
  if ( use_open_gl )
    {
      if ( surface == Screen ) return ( NULL );
      return ( SDL_DisplayFormat ( surface ) ) ; 
    }
  else
    {
      return ( SDL_DisplayFormat ( surface ) ) ; 
    }

  return ( NULL );
}; // SDL_Surface* our_SDL_display_format_wrapper ( SDL_Surface *surface )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
SDL_Surface*
our_SDL_display_format_wrapperAlpha ( SDL_Surface *surface )
{
  SDL_Surface* return_surface;

  if ( use_open_gl )
    {
      if ( surface == Screen ) return ( NULL );
      return_surface = SDL_DisplayFormatAlpha ( surface ) ;
      SDL_SetColorKey( return_surface , 0 , 0 ); // this should clear any color key in the dest surface
      SDL_SetAlpha( return_surface , SDL_SRCALPHA , SDL_ALPHA_OPAQUE );
      return ( return_surface ) ;
    }
  else
    {
      return ( SDL_DisplayFormatAlpha ( surface ) ) ; 
    }

  return ( NULL );
};

/* ----------------------------------------------------------------------
 * This function flips a given SDL_Surface around the x-axis, i.e. up-down.
 * 
 * This is particularly nescessary, since OpenGL has a different native
 * coordinate system than SDL and therefore images often appear flipped
 * around if one doesn't counter this effect with OpenGL by flipping the
 * images just once more in the same fashion.  That is what this function
 * does.
 * ---------------------------------------------------------------------- */
void
flip_image_horizontally ( SDL_Surface* tmp1 ) 
{
  int x , y ;
  Uint32 temp;

  for ( y = 0 ; y < ( tmp1 -> h ) / 2 ; y ++ )
    {
      for ( x = 0 ; x < (tmp1 -> w ) ; x ++ )
	{
	  temp = GetPixel ( tmp1 , x , y ) ;
	  PutPixel ( tmp1 , x , y , GetPixel ( tmp1 , x , ( tmp1 -> h - y - 1 ) ) ) ;
	  PutPixel ( tmp1 , x , ( tmp1 -> h - y - 1 ) , temp ) ;
	}
    }
}; // void flip_image_horizontally ( SDL_Surface* tmp1 ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
SDL_Surface* 
our_IMG_load_wrapper( const char *file )
{
  SDL_Surface* tmp1;

  if ( use_open_gl )
    {
      tmp1 = IMG_Load ( file ) ;

      if ( tmp1 == NULL ) return ( NULL );

      flip_image_horizontally ( tmp1 ) ;

      return ( tmp1 ) ;
    }
  else
    {
      return ( IMG_Load ( file ) ) ;
    }
}; // SDL_Surface* our_IMG_load_wrapper( const char *file )

/* ----------------------------------------------------------------------
 * There is need to do some padding, cause OpenGL textures need to have
 * a format: width and length both each a power of two.  Therefore some
 * extra alpha to the sides must be inserted.  This is what this function
 * is supposed to do:  manually adding hte proper amount of padding to
 * the surface, so that the dimensions will reach the next biggest power
 * of two in both directions, width and length.
 * ---------------------------------------------------------------------- */
SDL_Surface*
pad_image_for_texture ( SDL_Surface* our_surface ) 
{
  int i ; 
  int x = 1 ;
  int y = 1 ;
  SDL_Surface* padded_surf;
  SDL_Surface* tmp_surf;
  SDL_Rect dest;
  Uint32 target_color;

  for ( i = 1 ; i < 100 ; i ++ )
    {
      if ( x >= our_surface -> w )
	break;
      x = x * 2 ;
    }

  for ( i = 1 ; i < 100 ; i ++ )
    {
      if ( y >= our_surface -> h )
	break;
      y = y * 2 ;
    }

  if ( x < 64 )
    {
      DebugPrintf ( 1 , "\nWARNING!  Texture x < 64 encountered.  Raising to 64 x." ) ;
      x = 64 ;
    }
  if ( y < 64 )
    {
      DebugPrintf ( 1 , "\nWARNING!  Texture y < 64 encountered.  Raising to 64 y." ) ;
      y = 64 ;
    }

  DebugPrintf ( 1 , "\nPadding image to texture size: final is x=%d, y=%d." , x , y );
  
  padded_surf = SDL_CreateRGBSurface( 0 , x , y , 32, 0x0FF000000 , 0x000FF0000  , 0x00000FF00 , 0x000FF );
  tmp_surf = SDL_DisplayFormatAlpha ( padded_surf ) ;
  SDL_FreeSurface ( padded_surf );

  SDL_SetAlpha( our_surface , 0 , 0 );
  SDL_SetColorKey( our_surface , 0 , 0x0FF );
  
  dest . x = 0;
  dest . y = y - our_surface -> h ;
  dest . w = our_surface -> w ;
  dest . h = our_surface -> h ;
  
  target_color = SDL_MapRGBA( tmp_surf -> format, 0, 0, 0, 0 );
  for ( x = 0 ; x < tmp_surf -> w ; x ++ )
    {
      for ( y = 0 ; y < tmp_surf -> h ; y ++ )
	{
	  PutPixel ( tmp_surf , x , y , target_color ) ;
	}
    }

  our_SDL_blit_surface_wrapper ( our_surface, NULL , tmp_surf , & dest );

  return ( tmp_surf );

}; // SDL_Surface* pad_image_for_texture ( SDL_Surface* our_surface ) 

/* ----------------------------------------------------------------------
 * If OpenGL is in use, we need to make textured quads out of our normal
 * SDL surfaces, so that the image information can reside in texture 
 * memory and that means ON THE GRAPHICS CARD, avoiding the bottleneck
 * of the AGP port for *much* faster blitting of the graphics.
 * 
 * So this function should create appropriate OpenGL textures.  It relys
 * on and checks the fact, that a proper amount of OpenGL textures has
 * been requested in the beginning, knowing that this texture creation
 * call MUST ONLY BE CALLED AT MOST ONCE, SEE NEHE TUTORIALS AND SIMILAR
 * SOURCES, OR YOUR OLD TEXTURES WILL GET OVERWRITTEN AGAIN AND AGAIN!
 *
 * There is need to do some padding, cause OpenGL textures need to have
 * a format: width and length both each a power of two.  Therefore some
 * extra alpha to the sides must be inserted.
 *
 * ---------------------------------------------------------------------- */
void
make_texture_out_of_surface ( iso_image* our_image ) 
{
  SDL_Surface* right_sized_image ;

#ifdef HAVE_LIBGL

  if ( ! use_open_gl ) return;

  //--------------------
  // If the texture has been created before and this function is called
  // for the second time, this is a major error, that is considered a
  // cause for immediate program termination.
  //
  if ( our_image -> texture_has_been_created )
    {
      GiveStandardErrorMessage ( __FUNCTION__  , 
"Texture has been created already according to flag...\n\
hmmm... either the surface has been freed and the pointer moved cleanly to NULL\n\
(which is good for bug detection) or something is not right here...",
				 PLEASE_INFORM, IS_FATAL ); // WARNING_ONLY );
      return;
    }

  //--------------------
  // This fill up the image with transparent material, so that 
  // it will have powers of 2 as the dimensions, which is a requirement
  // for textures on most OpenGL capable cards.
  //
  right_sized_image = pad_image_for_texture ( our_image -> surface ) ;
  our_image -> texture_width = right_sized_image -> w ;
  our_image -> texture_height = right_sized_image -> h ;
  our_image -> original_image_width = our_image -> surface -> w ;
  our_image -> original_image_height = our_image -> surface -> h ;

  //--------------------
  // Having prepared the raw image it's now time to create the real
  // textures.
  //
  glPixelStorei( GL_UNPACK_ALIGNMENT,1 );

  //--------------------
  // We must not call glGenTextures more than once in all of Freedroid,
  // according to the nehe docu and also confirmed instances of textures
  // getting overwritten.  So all the gentexture stuff is now in the
  // initialzize_our_default_open_gl_parameters function and we'll use stuff from there.
  //
  // glGenTextures( 1, & our_image -> texture );
  //
  our_image -> texture = & ( all_freedroid_textures [ next_texture_index_to_use ] ) ;
  our_image -> texture_has_been_created = TRUE ;
  next_texture_index_to_use ++ ;
  if ( next_texture_index_to_use >= MAX_AMOUNT_OF_TEXTURES_WE_WILL_USE )
    {
      GiveStandardErrorMessage ( __FUNCTION__  , 
"Ran out of initialized texture positions to use for new textures.",
				 PLEASE_INFORM, IS_FATAL );
    }
  else
    {
      DebugPrintf ( 0 , "\nTexture positions remaining: %d." , MAX_AMOUNT_OF_TEXTURES_WE_WILL_USE - next_texture_index_to_use );
    }

  //--------------------
  // Typical Texture Generation Using Data From The Bitmap 
  //
  glBindTexture( GL_TEXTURE_2D, * ( our_image -> texture ) );
  
  //--------------------
  // Maybe we will want to set some storage parameters, but I'm not
  // completely sure if they would really help us in any way...
  //
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
  // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  // glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  // glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

  //--------------------
  // Setting texture parameters like in NeHe tutorial...
  //
  glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );

  //--------------------
  // We will use the 'GL_REPLACE' texturing environment or get 
  // unusable (and slow) results.
  //
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

  // Generate The Texture 
  glTexImage2D( GL_TEXTURE_2D, 0, 4, right_sized_image ->w,
		right_sized_image -> h, 0, GL_BGRA,
		GL_UNSIGNED_BYTE, right_sized_image -> pixels );

  SDL_FreeSurface ( right_sized_image );

  //--------------------
  // Now that the texture has been created, we assume that the image is
  // not needed any more and can be freed now!  BEWARE!  Keep this in mind,
  // that creating the texture now removes the surface...
  //
  // So as to detect any occurances of access to the surface once the 
  // texture has been created, we set the surface to the NULL pointer to
  // ENCOURAGE SEGMENTATION FAULTS when doing this so as to eliminate
  // these occurances with the debugger...
  //
  SDL_FreeSurface ( our_image -> surface );
  our_image -> surface = NULL ;

#endif

}; // void make_texture_out_of_surface ( iso_image* our_image )

/* ----------------------------------------------------------------------
 * If OpenGL is in use, we need to make textured quads out of our normal
 * SDL surfaces, so that the image information can reside in texture 
 * memory and that means ON THE GRAPHICS CARD, avoiding the bottleneck
 * of the AGP port for *much* faster blitting of the graphics.
 * 
 * So this function should create appropriate OpenGL textures.  It relys
 * on and checks the fact, that a proper amount of OpenGL textures has
 * been requested in the beginning, knowing that this texture creation
 * call MUST ONLY BE CALLED AT MOST ONCE, SEE NEHE TUTORIALS AND SIMILAR
 * SOURCES, OR YOUR OLD TEXTURES WILL GET OVERWRITTEN AGAIN AND AGAIN!
 *
 * ALSO THIS FUNCTION EXPECTS PRE-PADDED IMAGE SIZES!  An outside tool
 * like gluem must have preprated the images in question in advance for
 * better loading times, or this function will simply fail (with proper
 * error message of course...)
 *
 * ---------------------------------------------------------------------- */
void
make_texture_out_of_prepadded_image ( iso_image* our_image ) 
{
    int factor;

#ifdef HAVE_LIBGL
    
    if ( ! use_open_gl ) return;
    
    //--------------------
    // If the texture has been created before and this function is called
    // for the second time, this is a major error, that is considered a
    // cause for immediate program termination.
    //
    if ( our_image -> texture_has_been_created )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , 
				   "Texture has been created already according to flag...\n\
hmmm... either the surface has been freed and the pointer moved cleanly to NULL\n\
(which is good for bug detection) or something is not right here...",
				   PLEASE_INFORM, IS_FATAL ); // WARNING_ONLY );
	return;
    }
    
    //--------------------
    // Now we check if the image received really has the desired form, i.e. if
    // the width and height of the image are really powers of two.  We do that
    // by testing for the prime factors of the size and width received...
    //
    for ( factor = 3 ; factor < sqrt ( our_image -> surface -> w ) + 1 ; factor ++ )
    {
	if ( ! ( our_image -> surface -> w / factor ) )
	{
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "ERROR!  Prime factor unequal 2 identified in the image width...\n\
hmmm... this does not seem like a pre-padded OpenGL-prepared surface.  Breaking off... " ,
				       PLEASE_INFORM, IS_FATAL );
	return;
	}
    }
    for ( factor = 3 ; factor < sqrt ( our_image -> surface -> h ) + 1 ; factor ++ )
    {
	if ( ! ( our_image -> surface -> h / factor ) )
	{
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "ERROR!  Prime factor unequal 2 identified in the image height...\n\
hmmm... this does not seem like a pre-padded OpenGL-prepared surface.  Breaking off... " ,
				       PLEASE_INFORM, IS_FATAL );
	return;
	}
    }

    //--------------------
    // The image data in question is already arranged to have dimensions
    // which are powers of two, so all we need to do is fill the image
    // specs from the parameter list into the image struct.
    //
    our_image -> texture_width = our_image -> surface -> w ;
    our_image -> texture_height = our_image -> surface -> h ;
    
    //--------------------
    // Having prepared the raw image it s now time to create the real 
    // textures
    //
    glPixelStorei( GL_UNPACK_ALIGNMENT,1 );
    
    //--------------------
    // We must not call glGenTextures more than once in all of Freedroid,
    // according to the nehe docu and also confirmed instances of textures
    // getting overwritten.  So all the gentexture stuff is now in the
    // initialzize_our_default_open_gl_parameters function and we will use 
    // stuff from there.
    //
    // glGenTextures( 1, & our_image -> texture );
    //
    our_image -> texture = & ( all_freedroid_textures [ next_texture_index_to_use ] ) ;
    our_image -> texture_has_been_created = TRUE ;
    next_texture_index_to_use ++ ;
    if ( next_texture_index_to_use >= MAX_AMOUNT_OF_TEXTURES_WE_WILL_USE )
    {
	GiveStandardErrorMessage ( __FUNCTION__  , 
				   "Ran out of initialized texture positions to use for new textures.",
				   PLEASE_INFORM, IS_FATAL );
    }
    else
    {
	DebugPrintf ( 0 , "\nTexture positions remaining: %d." , MAX_AMOUNT_OF_TEXTURES_WE_WILL_USE - next_texture_index_to_use );
    }
    
    //--------------------
    // Typical Texture Generation Using Data From The Bitmap 
    //
    glBindTexture( GL_TEXTURE_2D, * ( our_image -> texture ) );
    
    //--------------------
    // Maybe we will want to set some storage parameters, but I am not
    // completely sure if they would really help us in any way...
    //
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
    // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    // glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    // glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    
    //--------------------
    // Setting texture parameters like in NeHe tutorial...
    //
    glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );
    
    //--------------------
    // We will use the 'GL_REPLACE' texturing environment or get 
    // unusable (and slow) results.
    //
    // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
    // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    
    // Generate The Texture 
    glTexImage2D( GL_TEXTURE_2D, 0, 4, our_image -> surface -> w,
		  our_image -> surface -> h , 0, GL_BGRA,
		  GL_UNSIGNED_BYTE, our_image -> surface -> pixels );
    
    //--------------------
    // Now that the texture has been created, we assume that the image is
    // not needed any more and can be freed now!  BEWARE!  Keep this in mind,
    // that creating the texture now removes the surface...
    //
    // So as to detect any occurances of access to the surface once the 
    // texture has been created, we set the surface to the NULL pointer to
    // ENCOURAGE SEGMENTATION FAULTS when doing this so as to eliminate
    // these occurances with the debugger...
    //
    SDL_FreeSurface ( our_image -> surface );
    our_image -> surface = NULL ;
    
#endif
    
}; // void make_texture_out_of_prepadded_image (...)

/* ----------------------------------------------------------------------
 * This function checks the error status of the OpenGL driver.  An error
 * will produce at least a warning message, maybe even program termination
 * if the errors are really severe.
 * ---------------------------------------------------------------------- */
void
open_gl_check_error_status ( char* name_of_calling_function  )
{

    // return;
#ifdef HAVE_LIBGL
    
    switch ( glGetError( ) )
    {
	case GL_NO_ERROR:
	    //--------------------
	    // All is well.  No messages need to be generated...
	    break;
	case GL_INVALID_ENUM:
	    fprintf ( stderr , "\ncalling function was: %s." , name_of_calling_function );
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "Error code GL_INVALID_ENUM received!", PLEASE_INFORM, IS_FATAL );
	    
	    break;
	case GL_INVALID_VALUE:
	    fprintf ( stderr , "\ncalling function was: %s." , name_of_calling_function );
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "Error code GL_INVALID_VALUE received!", PLEASE_INFORM, IS_FATAL );
	    
	    break;
	case GL_INVALID_OPERATION:
	    fprintf ( stderr , "\ncalling function was: %s." , name_of_calling_function );
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "Error code GL_INVALID_OPERATION received!", PLEASE_INFORM, IS_FATAL );
	    break;
	case GL_STACK_OVERFLOW:
	    fprintf ( stderr , "\ncalling function was: %s." , name_of_calling_function );
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "Error code GL_STACK_OVERFLOW received!", PLEASE_INFORM, IS_FATAL );
	    break;
	case GL_STACK_UNDERFLOW:
	    fprintf ( stderr , "\ncalling function was: %s." , name_of_calling_function );
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "Error code GL_STACK_UNDERFLOW received!", PLEASE_INFORM, IS_FATAL );
	    break;
	case GL_OUT_OF_MEMORY:
	    fprintf ( stderr , "\ncalling function was: %s." , name_of_calling_function );
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "Error code GL_OUT_OF_MEMORY received!", PLEASE_INFORM, IS_FATAL );
	    break;
	default:
	    fprintf ( stderr , "\ncalling function was: %s." , name_of_calling_function );
	    GiveStandardErrorMessage ( __FUNCTION__  , 
				       "Unhandled error code received!", PLEASE_INFORM, IS_FATAL );
	    break;
    }
#endif
}; // void open_gl_check_error_status ( char* name_of_calling_function  )

void clear_screen() {
#ifdef HAVE_LIBGL
	
  if ( use_open_gl )
    glClear(GL_COLOR_BUFFER_BIT);
#endif

}

/* ----------------------------------------------------------------------
 * Initialize the OpenGL interface.
 * ---------------------------------------------------------------------- */
int
initialzize_our_default_open_gl_parameters ( void )
{

#ifdef HAVE_LIBGL

  //--------------------
  // Set up the screne, viewport matrix, coordinate system and all that...
  //
  glViewport(0,0,640,480);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f,640.0f,480.0f,0.0f,-1.0f,1.0f);
  glMatrixMode(GL_MODELVIEW);

  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  
  //--------------------
  // We turn off a lot of stuff for faster blitting.  All this can
  // be turned on again later anyway as needed.  
  //
  // (Well, yes, the above is true, but function call overhead might
  //  be enourmous!  So don't change too much inside of quick loops
  //  if it can be avoided.)
  //
  glDisable (GL_BLEND);
  glDisable (GL_DITHER);
  glDisable (GL_FOG);
  glDisable (GL_LIGHTING);
  glDisable (GL_TEXTURE_1D);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_TEXTURE_3D);
  glShadeModel (GL_FLAT);

  //--------------------
  // We disable depth test for all purposes.
  //
  glDisable(GL_DEPTH_TEST);
  
  //--------------------
  // The default output method will be alpha test with threshold 0.5.
  //
  glEnable( GL_ALPHA_TEST );  
  glAlphaFunc ( GL_GREATER , 0.5 ) ;

  //--------------------
  // NeHe docu reveals, that it can happen (and it does happen) that textures
  // get deleted and overwritten when there are multiple calls to glgentexture.
  // So we can't have that any more.  We need to create all the textures we 
  // need once and forall and then later never again call glgentextures.
  // 
  // So we do the texture generation (not the texture loading) here once and
  // for all:
  //
  glGenTextures( MAX_AMOUNT_OF_TEXTURES_WE_WILL_USE , & ( all_freedroid_textures [ 0 ] ) );  

  //--------------------
  // Also we must initialize an index to the texture array, so that we can keep
  // track of which textures have been created and bound already.
  //
  // BEWARE:  Index 0 must not be used according to some docu...
  //
  next_texture_index_to_use = 1 ;

#endif

  return( TRUE );

}; // int initialzize_our_default_open_gl_parameters ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_open_gl_texture_to_map_position ( iso_image our_floor_iso_image , 
				       float our_col , float our_line , 
				       float r, float g , float b , 
				       int highlight_texture, int blend ) 
{

#ifdef HAVE_LIBGL

    SDL_Rect target_rectangle;
    float texture_start_y;
    float texture_end_y;
    int image_start_x;
    int image_end_x;
    int image_start_y;
    int image_end_y;
    
    //--------------------
    // At first we need to enable texture mapping for all of the following.
    // Without that, we'd just get (faster, but plain white) rectangles.
    //
    glEnable( GL_TEXTURE_2D );
    
    //--------------------
    // Linear Filtering is slow and maybe not nescessary here, so we
    // stick to the faster 'nearest' variant.
    //
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    
    //--------------------
    // Blending can be used, if there is no suitable alpha checking so that
    // I could get it to work right....
    //
    // But alpha check functions ARE a bit faster, even on my hardware, so
    // let's stick with that possibility for now, especially with the floor.
    //
    if ( ( blend == TRANSPARENCY_FOR_WALLS ) && GameConfig . transparency ) 
    {
	glEnable ( GL_BLEND ) ;
	glBlendFunc( GL_SRC_ALPHA , GL_DST_ALPHA);
    }
    else if ( blend == TRANSPARENCY_FOR_SEE_THROUGH_OBJECTS ) 
    {
	//--------------------
	// Sometimes some obstacles are partly transparent.  In that case
	// alpha test is not the right thing but rather true blending is
	// to be used...
	//
	glEnable ( GL_BLEND ) ;
	glDisable( GL_ALPHA_TEST );  
	// glBlendFunc( GL_SRC_ALPHA , GL_DST_ALPHA);
    }

    //
    // glEnable( GL_ALPHA_TEST );  
    // glAlphaFunc ( GL_GREATER , 0.5 ) ;
    
    // glDisable(GL_BLEND);
    // glDisable( GL_ALPHA_TEST );  
  
    //--------------------
    // Now of course we need to find out the proper target position.
    //
    target_rectangle . x = 
	translate_map_point_to_screen_pixel ( our_col , our_line , TRUE ) + 
	our_floor_iso_image . offset_x ;
    target_rectangle . y = 
	translate_map_point_to_screen_pixel ( our_col , our_line , FALSE ) +
	our_floor_iso_image . offset_y ;
    
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    //--------------------
    // Depending on whether to highlight the object in question, we
    // set a different color for the way the texture is applied.
    //
    // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
    glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    if ( highlight_texture )
    {
	// glBlendFunc ( GL_DST_COLOR, GL_SRC_COLOR ) ;
	// glBlendFunc ( GL_ONE, GL_ONE ) ;
	// glBlendFunc( GL_SRC_ALPHA , GL_ONE );
	// glBlendFunc (GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
	glColor3f( r , g , b );
    }
    else
    {
	glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glColor3f( r * 0.75 , g * 0.75 , b * 0.75 );
    }
    
    
    
    //--------------------
    // Now we can begin to draw the actual textured rectangle.
    //
    image_start_x = target_rectangle . x ;
    image_end_x = target_rectangle . x + our_floor_iso_image . texture_width ; // + 255
    image_start_y = target_rectangle . y ;
    image_end_y = target_rectangle . y + our_floor_iso_image . texture_height ; // + 127
    
    // DebugPrintf ( -1 , "\nheight: %d." , our_floor_iso_image . surface -> h ) ;
    
    texture_start_y = 1.0 ; // 1 - ((float)(our_floor_iso_image . surface -> h)) / 127.0 ; // 1.0 
    texture_end_y = 0.0 ;
    
    glBindTexture( GL_TEXTURE_2D, * ( our_floor_iso_image . texture ) );
    
    glBegin(GL_QUADS);
    
    glTexCoord2i( 0.0f, texture_start_y ); 
    glVertex2i( image_start_x , image_start_y );
    glTexCoord2i( 0.0f, texture_end_y ); 
    glVertex2i( image_start_x , image_end_y );
    glTexCoord2i( 1.0f, texture_end_y ); 
    glVertex2i( image_end_x , image_end_y );
    glTexCoord2f( 1.0f, texture_start_y ); 
    glVertex2i( image_end_x , image_start_y );
    
    glEnd( );
    
    //--------------------
    // But for the rest of the drawing function, the peripherals and other
    // things that are to be blitted after that, we should not forget to
    // disable the texturing things again, or HORRIBLE framerates will result...
    //
    // So we revert everything that we might have touched to normal state.
    //
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );
    glEnable( GL_ALPHA_TEST );  
    glAlphaFunc ( GL_GREATER , 0.5 ) ;
    
#endif

}; // void blit_open_gl_texture_to_map_position ( ... )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_zoomed_open_gl_texture_to_map_position ( iso_image our_floor_iso_image , float our_col , float our_line , float r, float g , float b , int highlight_texture , int blend ) 
{

#ifdef HAVE_LIBGL

  SDL_Rect target_rectangle;
  float texture_start_y;
  float texture_end_y;
  int image_start_x;
  int image_end_x;
  int image_start_y;
  int image_end_y;
  float zoom_factor = ( 1.0 / LEVEL_EDITOR_ZOOM_OUT_FACT ) ;

  //--------------------
  // At first we need to enable texture mapping for all of the following.
  // Without that, we'd just get (faster, but plain white) rectangles.
  //
  glEnable( GL_TEXTURE_2D );
  
  //--------------------
  // Linear Filtering is slow and maybe not nescessary here, so we
  // stick to the faster 'nearest' variant.
  //
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  
  //--------------------
  // Blending can be used, if there is no suitable alpha checking so that
  // I could get it to work right....
  //
  // But alpha check functions ARE a bit faster, even on my hardware, so
  // let's stick with that possibility for now, especially with the floor.
  //
  //
  if ( blend && GameConfig . transparency ) 
  {
      glEnable(GL_BLEND);
      glBlendFunc( GL_SRC_ALPHA , GL_ONE );
  }

  glEnable( GL_ALPHA_TEST );  
  glAlphaFunc ( GL_GREATER , 0.5 ) ;
  
  // glDisable(GL_BLEND);
  // glDisable( GL_ALPHA_TEST );  
  
  //--------------------
  // Now of course we need to find out the proper target position.
  //
  target_rectangle . x = 
    translate_map_point_to_zoomed_screen_pixel ( our_col , our_line , TRUE ) + 
    our_floor_iso_image . offset_x * zoom_factor ;
  target_rectangle . y = 
    translate_map_point_to_zoomed_screen_pixel ( our_col , our_line , FALSE ) +
    our_floor_iso_image . offset_y * zoom_factor ;
  
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  //--------------------
  // Depending on whether to highlight the object in question, we
  // set a different color for the way the texture is applied.
  //
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
  glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  if ( highlight_texture )
    {
      // glBlendFunc ( GL_DST_COLOR, GL_SRC_COLOR ) ;
      // glBlendFunc ( GL_ONE, GL_ONE ) ;
      // glBlendFunc( GL_SRC_ALPHA , GL_ONE );
      // glBlendFunc (GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
      glColor3f( r , g , b );
    }
  else
    {
      glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      glColor3f( r * 0.75 , g * 0.75 , b * 0.75 );
    }

  //--------------------
  // Now we can begin to draw the actual textured rectangle.
  //
  image_start_x = target_rectangle . x ;
  image_end_x = target_rectangle . x + our_floor_iso_image . texture_width * zoom_factor ; // + 255
  image_start_y = target_rectangle . y ;
  image_end_y = target_rectangle . y + our_floor_iso_image . texture_height * zoom_factor ; // + 127
  
  // DebugPrintf ( -1 , "\nheight: %d." , our_floor_iso_image . surface -> h ) ;
  
  texture_start_y = 1.0 ; // 1 - ((float)(our_floor_iso_image . surface -> h)) / 127.0 ; // 1.0 
  texture_end_y = 0.0 ;
  

  glBindTexture( GL_TEXTURE_2D, * ( our_floor_iso_image . texture ) );

  glBegin(GL_QUADS);
  
  glTexCoord2i( 0.0f, texture_start_y ); 
  glVertex2i( image_start_x , image_start_y );
  glTexCoord2i( 0.0f, texture_end_y ); 
  glVertex2i( image_start_x , image_end_y );
  glTexCoord2i( 1.0f, texture_end_y ); 
  glVertex2i( image_end_x , image_end_y );
  glTexCoord2f( 1.0f, texture_start_y ); 
  glVertex2i( image_end_x , image_start_y );
  
  glEnd( );
  
  //--------------------
  // But for the rest of the drawing function, the peripherals and other
  // things that are to be blitted after that, we should not forget to
  // disable the texturing things again, or HORRIBLE framerates will result...
  //
  // So we revert everything that we might have touched to normal state.
  //
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_BLEND );
  glEnable( GL_ALPHA_TEST );  
  glAlphaFunc ( GL_GREATER , 0.5 ) ;
  
#endif

}; // void blit_zoomed_open_gl_texture_to_map_position ( iso_image our_floor_iso_image , float our_col , float our_line ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_open_gl_texture_to_screen_position ( iso_image our_floor_iso_image , int x , int y , int set_gl_parameters ) 
{

#ifdef HAVE_LIBGL

    SDL_Rect target_rectangle;
    float texture_start_y;
    float texture_end_y;
    int image_start_x;
    int image_end_x;
    int image_start_y;
    int image_end_y;
    
    if ( set_gl_parameters )
    {
	//--------------------
	// At first we need to enable texture mapping for all of the following.
	// Without that, we'd just get (faster, but plain white) rectangles.
	//
	glEnable( GL_TEXTURE_2D );
	
	// glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
	// glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
	// glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glEnable ( GL_BLEND );
	glDisable ( GL_ALPHA_TEST );
	
	// glDisable ( GL_BLEND );
	// glEnable ( GL_ALPHA_TEST );

	glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
    }
    
    //--------------------
    // Now of course we need to find out the proper target position.
    //
    target_rectangle . x = x ;
    target_rectangle . y = y ;
    
    //--------------------
    // Now we can begin to draw the actual textured rectangle.
    //
    image_start_x = target_rectangle . x ;
    image_end_x = target_rectangle . x + our_floor_iso_image . texture_width ; // * LIGHT_RADIUS_CRUDENESS_FACTOR  ; // + 255
    image_start_y = target_rectangle . y ;
    image_end_y = target_rectangle . y + our_floor_iso_image . texture_height ; // * LIGHT_RADIUS_CRUDENESS_FACTOR ; // + 127
    
    if ( image_start_x > 640 ) return ;
    if ( image_end_x < 0 ) return ;
    if ( image_start_y > 480 ) return;
    if ( image_end_y < 0 ) return;

    // DebugPrintf ( -1 , "\nheight: %d." , our_floor_iso_image . surface -> h ) ;
    
    texture_start_y = 1.0 ; // 1 - ((float)(our_floor_iso_image . surface -> h)) / 127.0 ; // 1.0 
    texture_end_y = 0.0 ;

    // glColor3f( 1 , 1 , 1 );

    glBindTexture( GL_TEXTURE_2D, * ( our_floor_iso_image . texture ) );
    glBegin(GL_QUADS);
    glTexCoord2i( 0.0f, texture_start_y ); 
    glVertex2i( image_start_x , image_start_y );
    glTexCoord2i( 0.0f, texture_end_y ); 
    glVertex2i( image_start_x , image_end_y );
    glTexCoord2i( 1.0f, texture_end_y ); 
    glVertex2i( image_end_x , image_end_y );
    glTexCoord2f( 1.0f, texture_start_y ); 
    glVertex2i( image_end_x , image_start_y );
    glEnd( );
    
    if ( set_gl_parameters )
    {
	glDisable( GL_TEXTURE_2D );
    }
    
#endif

}; // void blit_open_gl_texture_to_screen_position ( iso_image our_floor_iso_image , int x , int y , int set_gl_parameters ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_semitransparent_open_gl_texture_to_screen_position ( iso_image our_floor_iso_image , int x , int y ) 
{

#ifdef HAVE_LIBGL

    SDL_Rect target_rectangle;
    float texture_start_y;
    float texture_end_y;
    int image_start_x;
    int image_end_x;
    int image_start_y;
    int image_end_y;
    
    //--------------------
    // At first we need to enable texture mapping for all of the following.
    // Without that, we'd just get (faster, but plain white) rectangles.
    //
    glEnable( GL_TEXTURE_2D );
    
    // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
    // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    
    glEnable ( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA , GL_DST_ALPHA);
    glDisable ( GL_ALPHA_TEST );
    
    //--------------------
    // Now of course we need to find out the proper target position.
    //
    target_rectangle . x = x ;
    target_rectangle . y = y ;
    
    //--------------------
    // Now we can begin to draw the actual textured rectangle.
    //
    image_start_x = target_rectangle . x ;
    image_end_x = target_rectangle . x + our_floor_iso_image . texture_width ; // * LIGHT_RADIUS_CRUDENESS_FACTOR  ; // + 255
    image_start_y = target_rectangle . y ;
    image_end_y = target_rectangle . y + our_floor_iso_image . texture_height ; // * LIGHT_RADIUS_CRUDENESS_FACTOR ; // + 127
    
    if ( image_start_x > 640 ) return ;
    if ( image_end_x < 0 ) return ;
    if ( image_start_y > 480 ) return;
    if ( image_end_y < 0 ) return;

    // DebugPrintf ( -1 , "\nheight: %d." , our_floor_iso_image . surface -> h ) ;
    
    texture_start_y = 1.0 ; // 1 - ((float)(our_floor_iso_image . surface -> h)) / 127.0 ; // 1.0 
    texture_end_y = 0.0 ;

    // glColor3f( 1 , 1 , 1 );

    glBindTexture( GL_TEXTURE_2D, * ( our_floor_iso_image . texture ) );
    glBegin(GL_QUADS);
    glTexCoord2i( 0.0f, texture_start_y ); 
    glVertex2i( image_start_x , image_start_y );
    glTexCoord2i( 0.0f, texture_end_y ); 
    glVertex2i( image_start_x , image_end_y );
    glTexCoord2i( 1.0f, texture_end_y ); 
    glVertex2i( image_end_x , image_end_y );
    glTexCoord2f( 1.0f, texture_start_y ); 
    glVertex2i( image_end_x , image_start_y );
    glEnd( );
    

    glDisable ( GL_BLEND );
    glEnable ( GL_ALPHA_TEST );
    glDisable( GL_TEXTURE_2D );

#endif

}; // void blit_semitransparent_open_gl_texture_to_screen_position ( ... )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_zoomed_open_gl_texture_to_screen_position ( iso_image* our_floor_iso_image , int x , int y , int set_gl_parameters , float zoom_factor ) 
{

#ifdef HAVE_LIBGL

  SDL_Rect target_rectangle;
  float texture_start_y;
  float texture_end_y;
  int image_start_x;
  int image_end_x;
  int image_start_y;
  int image_end_y;

  if ( set_gl_parameters )
    {
      //--------------------
      // At first we need to enable texture mapping for all of the following.
      // Without that, we'd just get (faster, but plain white) rectangles.
      //
      glEnable( GL_TEXTURE_2D );
      
      // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
      // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
      // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
      glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

      glEnable ( GL_BLEND );
      glDisable ( GL_ALPHA_TEST );
    }

  //--------------------
  // Now of course we need to find out the proper target position.
  //
  target_rectangle . x = x ;
  target_rectangle . y = y ;

  //--------------------
  // Now we can begin to draw the actual textured rectangle.
  //
  image_start_x = target_rectangle . x ;
  image_end_x = target_rectangle . x + our_floor_iso_image -> texture_width * zoom_factor ; // * LIGHT_RADIUS_CRUDENESS_FACTOR  ; // + 255
  image_start_y = target_rectangle . y ;
  image_end_y = target_rectangle . y + our_floor_iso_image -> texture_height * zoom_factor ; // * LIGHT_RADIUS_CRUDENESS_FACTOR ; // + 127
  
  if ( image_start_x > 640 ) return ;
  if ( image_end_x < 0 ) return ;
  if ( image_start_y > 480 ) return;
  if ( image_end_y < 0 ) return;

  // DebugPrintf ( -1 , "\nheight: %d." , our_floor_iso_image . surface -> h ) ;
  
  texture_start_y = 1.0 ; // 1 - ((float)(our_floor_iso_image . surface -> h)) / 127.0 ; // 1.0 
  texture_end_y = 0.0 ;

  // glColor3f( 1 , 1 , 1 );

  glBindTexture( GL_TEXTURE_2D, * ( our_floor_iso_image -> texture ) );
  glBegin(GL_QUADS);
  glTexCoord2i( 0.0f, texture_start_y ); 
  glVertex2i( image_start_x , image_start_y );
  glTexCoord2i( 0.0f, texture_end_y ); 
  glVertex2i( image_start_x , image_end_y );
  glTexCoord2i( 1.0f, texture_end_y ); 
  glVertex2i( image_end_x , image_end_y );
  glTexCoord2f( 1.0f, texture_start_y ); 
  glVertex2i( image_end_x , image_start_y );
  glEnd( );

  if ( set_gl_parameters )
    {
      glDisable( GL_TEXTURE_2D );
    }

#endif

}; // void blit_zoomed_open_gl_texture_to_screen_position ( iso_image our_floor_iso_image , int x , int y , int set_gl_parameters , float zoom_factor ) 


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_rotated_open_gl_texture_with_center ( iso_image our_iso_image , int x , int y , float angle_in_degree ) 
{

#ifdef HAVE_LIBGL

  SDL_Rect target_rectangle;
  float texture_start_y;
  float texture_end_y;

  int image_start_x;
  int image_end_x;
  int image_start_y;
  int image_end_y;

  moderately_finepoint corner1, corner2, corner3, corner4;

  glEnable( GL_TEXTURE_2D );  

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  //--------------------
  // Linear Filtering is slow and maybe not nescessary here, so we
  // stick to the faster 'nearest' variant.
  //
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  
  glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  
  glEnable( GL_ALPHA_TEST );  
  glAlphaFunc ( GL_GREATER , 0.5 ) ;

  // glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );
  // glBlendFunc( GL_SRC_ALPHA , GL_ONE );

  glColor4f( 1, 1, 1 , 0 );

  //--------------------
  // Now of course we need to find out the proper target position.
  //
  target_rectangle . x = x ;
  target_rectangle . y = y ;
  
  //--------------------
  // Now we can begin to draw the actual textured rectangle.
  //
  image_start_x = target_rectangle . x ;
  image_end_x = target_rectangle . x + our_iso_image . texture_width ; 
  image_start_y = target_rectangle . y ;
  image_end_y = target_rectangle . y + our_iso_image . texture_height ;

  corner1 . x = 0 - our_iso_image . original_image_width / 2 ;
  corner1 . y = 0 - our_iso_image . original_image_height / 2 ;
  corner2 . x = 0 - our_iso_image . original_image_width / 2 ;
  corner2 . y = 0 + our_iso_image . original_image_height / 2 + ( our_iso_image . texture_height - our_iso_image . original_image_height );
  corner3 . x = 0 + our_iso_image . original_image_width / 2 + ( our_iso_image . texture_width - our_iso_image . original_image_width );
  corner3 . y = 0 + our_iso_image . original_image_height / 2 + ( our_iso_image . texture_height - our_iso_image . original_image_height );
  corner4 . x = 0 + our_iso_image . original_image_width / 2 + ( our_iso_image . texture_width - our_iso_image . original_image_width );
  corner4 . y = 0 - our_iso_image . original_image_height / 2 ;

  RotateVectorByAngle ( & corner1 , angle_in_degree );
  RotateVectorByAngle ( & corner2 , angle_in_degree );
  RotateVectorByAngle ( & corner3 , angle_in_degree );
  RotateVectorByAngle ( & corner4 , angle_in_degree );

  corner1 . x += x ;
  corner1 . y += y ;
  corner2 . x += x ;
  corner2 . y += y ;
  corner3 . x += x ;
  corner3 . y += y ;
  corner4 . x += x ;
  corner4 . y += y ;

  if ( image_start_x > 640 ) return ;
  if ( image_end_x < 0 ) return ;
  if ( image_start_y > 480 ) return;
  if ( image_end_y < 0 ) return;

  texture_start_y = 1.0 ; // 1 - ((float)(our_iso_image . original_image_height)) / 127.0 ; // 1.0 
  texture_end_y = 0.0 ;

  glBindTexture( GL_TEXTURE_2D, * ( our_iso_image . texture ) );
  glBegin(GL_QUADS);
  glTexCoord2i( 0.0f, texture_start_y ); 
  glVertex2i( corner1 . x , corner1 . y );
  glTexCoord2i( 0.0f, texture_end_y ); 
  glVertex2i( corner2 . x , corner2 . y );
  glTexCoord2i( 1.0f, texture_end_y ); 
  glVertex2i( corner3 . x , corner3 . y );
  glTexCoord2f( 1.0f, texture_start_y ); 
  glVertex2i( corner4 . x , corner4 . y );
  glEnd( );

#endif

}; // void blit_rotated_open_gl_texture_with_center ( iso_image our_iso_image , int x , int y , float angle_in_degree ) 

/* ----------------------------------------------------------------------
 * This function restores the menu background, that must have been stored
 * before using the function of similar name.
 * ---------------------------------------------------------------------- */
void
RestoreMenuBackground ( int backup_slot )
{
  if ( use_open_gl )
    {
      return;

#ifdef HAVE_LIBGL
      glRasterPos2i( 0 , 479 ) ; 
      glDrawPixels( 640, 478, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) StoredMenuBackground [ backup_slot ] );
      return ;
#endif
    }
  else
    {
      our_SDL_blit_surface_wrapper ( StoredMenuBackground [ backup_slot ] , NULL , Screen , NULL );
    }
}; // void RestoreMenuBackground ( void )

/* ----------------------------------------------------------------------
 * This function stores the current background as the background for a
 * menu, so that it can be refreshed much faster than by reassembling it
 * every frame.
 * ---------------------------------------------------------------------- */
void
StoreMenuBackground ( int backup_slot )
{
  static int first_call = TRUE ;

  if ( first_call )
    {
      StoredMenuBackground [ 0 ] = NULL ;
      StoredMenuBackground [ 1 ] = NULL ;
      first_call = FALSE ;
    }

  if ( use_open_gl )
    {
#ifdef HAVE_LIBGL
      //--------------------
      // WARNING!  WE ARE USING THE SDL_SURFACE* here much like a char*!!!
      // BEWARE!
      //
      if ( StoredMenuBackground [ backup_slot ] == NULL )
	{
	  StoredMenuBackground [ backup_slot ] = malloc ( 641 * 481 * 4 ) ;
	}

      glReadPixels( 0 , 1, 640, 479, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*) ( StoredMenuBackground [ backup_slot ] ) );
#endif
    }
  else
    {
      //--------------------
      // If the memory was not yet allocated, we need to do that now...
      //
      // otherwise we free the old surface and create a new copy of the
      // current screen content...
      //
      if ( StoredMenuBackground [ backup_slot ] == NULL )
	{
	  StoredMenuBackground [ backup_slot ] = our_SDL_display_format_wrapper ( Screen );
	}
      else
	{
	  SDL_FreeSurface ( StoredMenuBackground [ backup_slot ] );
	  StoredMenuBackground [ backup_slot ] = our_SDL_display_format_wrapper ( Screen );
	}
    }

}; // void StoreMenuBackground ( int backup_slot )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
remove_open_gl_blending_mode_again ( void )
{

#ifdef HAVE_LIBGL

  //--------------------
  // But for the rest of the drawing function, the peripherals and other
  // things that are to be blitted after that, we should not forget to
  // disable the texturing things again, or HORRIBLE framerates will result...
  //
  // So we revert everything that we might have touched to normal state.
  //
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_BLEND );
  glEnable( GL_ALPHA_TEST );  
  glAlphaFunc ( GL_GREATER , 0.5 ) ;
  
#endif 

}; // void remove_open_gl_blending_mode_again ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
prepare_open_gl_for_light_radius ( void )
{

#ifdef HAVE_LIBGL

  glDisable( GL_TEXTURE_2D );  
  glEnable(GL_BLEND);
  glDisable( GL_ALPHA_TEST );  
  glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

#endif
  
}; // void prepare_open_gl_for_light_radius ( void )
/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
prepare_open_gl_for_blending_textures( void )
{
#ifdef HAVE_LIBGL

  //--------------------
  // Linear Filtering is slow and maybe not nescessary here, so we
  // stick to the faster 'nearest' variant.
  //
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  
  glEnable( GL_TEXTURE_2D );  
  glEnable(GL_BLEND);
  glDisable( GL_ALPHA_TEST );  

  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
  glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

#endif

}; // void prepare_open_gl_for_blending_textures( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_open_gl_cheap_light_radius ( void )
{
#ifdef HAVE_LIBGL
  int our_height, our_width;
  int light_strength;
  int window_offset_x;
  Uint8 r , g , b , a ;
  moderately_finepoint target_pos;

  prepare_open_gl_for_light_radius ();

  window_offset_x = - ( SCREEN_WIDTH / 2 ) + UserCenter_x ;

  for ( our_height = 0 ; our_height < 40 ; our_height ++ )
    {
      for ( our_width = 0 ; our_width < 40 ; our_width ++ )
	{
	  if ( our_width % LIGHT_RADIUS_CRUDENESS_FACTOR ) continue;
	  if ( our_height % LIGHT_RADIUS_CRUDENESS_FACTOR ) continue;

	  target_pos . x = translate_pixel_to_map_location ( 0 , ( 0 + our_width ) * 16 - UserCenter_x + 16 ,
							   ( 0 + our_height ) * 12 - UserCenter_y + 16 , TRUE );
	  target_pos . y = translate_pixel_to_map_location ( 0 , ( 0 + our_width ) * 16 - UserCenter_x + 16 ,
							   ( 0 + our_height ) * 12 - UserCenter_y + 16 , FALSE );

	  light_strength = get_light_strength ( target_pos );

	  if ( light_strength >= NUMBER_OF_SHADOW_IMAGES ) light_strength = NUMBER_OF_SHADOW_IMAGES -1 ;
	  if ( light_strength <= 0 ) continue ;

	  r = 0 ; b = 0 ; g = 0 ; a = ( 255.0 / ( (float) NUMBER_OF_SHADOW_IMAGES ) ) * ( (float) light_strength ) ; 

	  glDisable ( GL_ALPHA_TEST );
	  glColor4ub( r , g , b , a );
	  
	  glBegin(GL_QUADS);
	  glVertex2i( ( 0 + our_width ) * 16 , ( 1 + our_height ) * 12 ) ;
	  glVertex2i( ( 0 + our_width ) * 16 , ( 0 + our_height ) * 12 ) ;
	  glVertex2i( ( 1 + our_width ) * 16 , ( 0 + our_height ) * 12 ) ;
	  glVertex2i( ( 1 + our_width ) * 16 , ( 1 + our_height ) * 12 ) ;
	  glEnd( );

	}
    }

  remove_open_gl_blending_mode_again ( ) ;

#endif

}; // void blit_open_gl_cheap_light_radius ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_open_gl_light_radius ( void )
{

#ifdef HAVE_LIBGL

  static int first_call = TRUE ;
  int i, j ;
  char* fpath;
  char constructed_file_name[2000];
  int our_height, our_width, our_max_height, our_max_width;
  int light_strength;
  moderately_finepoint target_pos;
  static int pos_x_grid [ (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ] [ (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2 ) ] ;
  static int pos_y_grid [ (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ] [ (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2 ) ] ;
  static SDL_Rect target_rectangle;
  int chunk_size_x;
  int chunk_size_y;
  int window_offset_x;
  int light_bonus = curShip . AllLevels [ Me [ 0 ] . pos . z ] -> light_radius_bonus ;
  SDL_Surface* tmp;

  //--------------------
  // At first we need to enable texture mapping for all of the following.
  // Without that, we'd just get (faster, but plain white) rectangles.
  //
  glEnable( GL_TEXTURE_2D );
  //--------------------
  // We disable depth test for all purposes.
  //
  glDisable(GL_DEPTH_TEST);

  //--------------------
  // We will use the 'GL_REPLACE' texturing environment or get 
  // unusable (and slow) results.
  //
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
  glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

  //--------------------
  // Blending can be used, if there is no suitable alpha checking so that
  // I could get it to work right....
  //
  // But alpha check functions ARE a bit faster, even on my hardware, so
  // let's stick with that possibility for now, especially with the floor.
  //
  glDisable( GL_ALPHA_TEST );  
  glEnable(GL_BLEND);
  glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

  //--------------------
  // If the darkenss chunks have not yet been loaded, we load them...
  //
  if ( first_call )
    {
      first_call = FALSE;
      for ( i = 0 ; i < NUMBER_OF_SHADOW_IMAGES ; i ++ )
	{
	  sprintf ( constructed_file_name , "light_radius_chunks/iso_light_radius_darkness_%04d.png" , i + 1 );
	  fpath = find_file ( constructed_file_name , GRAPHICS_DIR , FALSE );
	  get_iso_image_from_file_and_path ( fpath , & ( light_radius_chunk [ i ] ) , TRUE ) ;
	  tmp = light_radius_chunk [ i ] . surface ;
	  light_radius_chunk [ i ] . surface = SDL_DisplayFormatAlpha ( light_radius_chunk [ i ] . surface ) ; 
	  SDL_FreeSurface ( tmp ) ;
	  make_texture_out_of_surface ( & ( light_radius_chunk [ i ] ) ) ;
	}

      pos_x_grid [ 0 ] [ 0 ] = translate_map_point_to_screen_pixel ( Me [ 0 ] . pos . x - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) , Me [ 0 ] . pos . y - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) , TRUE ) - 10 ;
      pos_y_grid [ 0 ] [ 0 ] = translate_map_point_to_screen_pixel ( Me [ 0 ] . pos . x - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) , Me [ 0 ] . pos . y - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) , FALSE ) - 42 ;

      chunk_size_x = 26 /2 + 1 ;
      chunk_size_y = 14 /2 ; 

      for ( i = 0 ; i < (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ; i ++ )
	{
	  for ( j = 0 ; j < (int)(FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2) ; j ++ )
	    {
	      pos_x_grid [ i ] [ j ] = pos_x_grid [ 0 ] [ 0 ] + ( i - j ) * chunk_size_x ;
	      pos_y_grid [ i ] [ j ] = pos_y_grid [ 0 ] [ 0 ] + ( i + j ) * chunk_size_y ;
	    }
	}
    }

  //--------------------
  // Now it's time to apply the light radius
  //
  our_max_width = FLOOR_TILES_VISIBLE_AROUND_TUX * ( 1.0 / LIGHT_RADIUS_CHUNK_SIZE ) * 2 ;
  our_max_height = our_max_width;

  window_offset_x = - ( SCREEN_WIDTH / 2 ) + UserCenter_x ;

  for ( our_height = 0 ; our_height < our_max_height ; our_height ++ )
    {
      for ( our_width = 0 ; our_width < our_max_width ; our_width ++ )
	{
	  if ( our_width % LIGHT_RADIUS_CRUDENESS_FACTOR ) continue;
	  if ( our_height % LIGHT_RADIUS_CRUDENESS_FACTOR ) continue;

	  target_pos . x = Me [ 0 ] . pos . x - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) + our_width * LIGHT_RADIUS_CHUNK_SIZE ;
	  target_pos . y = Me [ 0 ] . pos . y - ( FLOOR_TILES_VISIBLE_AROUND_TUX ) + our_height * LIGHT_RADIUS_CHUNK_SIZE;
	  light_strength = (int) ( sqrt ( ( Me [ 0 ] . pos . x - target_pos . x ) * ( Me [ 0 ] . pos . x - target_pos . x ) + ( Me [ 0 ] . pos . y - target_pos . y ) * ( Me [ 0 ] . pos . y - target_pos . y ) ) * 4.0 ) - light_bonus ;
	  if ( light_strength >= NUMBER_OF_SHADOW_IMAGES ) light_strength = NUMBER_OF_SHADOW_IMAGES -1 ;
	  if ( light_strength <= 0 ) continue ;

	  // blit_iso_image_to_map_position ( light_radius_chunk [ light_strength ] , target_pos . x , target_pos . y );
	  target_rectangle . x = pos_x_grid [ our_width ] [ our_height ] + window_offset_x ;
	  target_rectangle . y = pos_y_grid [ our_width ] [ our_height ] ;

	  blit_open_gl_texture_to_screen_position ( light_radius_chunk [ light_strength ] , target_rectangle . x , target_rectangle . y , FALSE ) ;
	}
    }

  //--------------------
  // But for the rest of the drawing function, the peripherals and other
  // things that are to be blitted after that, we should not forget to
  // disable the texturing things again, or HORRIBLE framerates will result...
  //
  // So we revert everything that we might have touched to normal state.
  //
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_BLEND );
  glEnable( GL_ALPHA_TEST );  
  glAlphaFunc ( GL_GREATER , 0.5 ) ;
  
#endif

}; // void blit_open_gl_light_radius ( void )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
PutPixel_open_gl ( int x, int y, Uint32 pixel)
{

#ifdef HAVE_LIBGL
  glRasterPos2i( x , y ) ;
  // glDrawPixels( 1 , 1, GL_RGBA , GL_UNSIGNED_BYTE , & pixel );
  glDrawPixels( 1 , 1, GL_RGB , GL_UNSIGNED_BYTE , & pixel );
#endif

}; // void PutPixel_open_gl ( x , y , pixel ) ;


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
GL_HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area , unsigned char r , unsigned char g , unsigned char b , unsigned char alpha )
{
#ifdef HAVE_LIBGL
  SDL_Rect* dstrect = & Area ;

  glRasterPos2i ( 0 , 0 ); 
  glDisable ( GL_ALPHA_TEST );
  glEnable ( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

  // glColor4ub( 255 , 255 , 255 , 200 );
  // glColor4ub( r , g , b , 150 );
  // glColor4ub( r , g , b , 80 );
  glColor4ub( r , g , b , alpha );
  if ( dstrect == NULL )
    {
      glBegin(GL_QUADS);
      glVertex2i( 0       , 480 );
      glVertex2i( 0       ,   0 );
      glVertex2i( 0 + 639 ,   0 );
      glVertex2i( 0 + 639 , 480 );
      glEnd( );
    }
  else
    {
      glBegin(GL_QUADS);
      glVertex2i( dstrect -> x                , dstrect -> y );
      glVertex2i( dstrect -> x                , dstrect -> y + dstrect -> h );
      glVertex2i( dstrect -> x + dstrect -> w , dstrect -> y + dstrect -> h );
      glVertex2i( dstrect -> x + dstrect -> w , dstrect -> y );
      glEnd( );
    }
#endif

  return;

}; // void GL_HighlightRectangle


#define CHARACTER_SCREEN_BACKGROUND_FILE "backgrounds/character.png" 
#define SKILL_SCREEN_BACKGROUND_FILE "backgrounds/SkillScreen.png" 
#define SKILL_EXPLANATION_SCREEN_BACKGROUND_FILE "backgrounds/SkillExplanationScreen.png" 
#define INVENTORY_SCREEN_BACKGROUND_FILE "backgrounds/inventory.png"
#define NE_TITLE_PIC_FILE       "backgrounds/title.jpg"
#define NE_CREDITS_PIC_FILE     "backgrounds/credits.jpg"
#define SHOP_BACKGROUND_IMAGE   "backgrounds/shoppe.jpg"
#define ITEM_BROWSER_BG_PIC_FILE "backgrounds/item_browser.jpg"
#define ITEM_BROWSER_SHOP_FILE "backgrounds/item_browser_shop.jpg"
#define NE_CONSOLE_FG_1_FILE     "backgrounds/console_fg_1.png" 
#define NE_CONSOLE_FG_2_FILE     "backgrounds/console_fg_2.png" 
#define NE_CONSOLE_FG_3_FILE     "backgrounds/console_fg_3.png" 
#define NE_CONSOLE_FG_4_FILE     "backgrounds/console_fg_4.png" 
#define NE_CONSOLE_BG_PIC1_FILE "backgrounds/console_bg1.jpg"
#define NE_CONSOLE_BG_PIC2_FILE "backgrounds/console_bg2.jpg"
#define LEVEL_EDITOR_BANNER_FILE1 "backgrounds/LevelEditorSelectionBar1.png"
#define LEVEL_EDITOR_BANNER_FILE2 "backgrounds/LevelEditorSelectionBar2.png"
#define LEVEL_EDITOR_BANNER_FILE3 "backgrounds/LevelEditorSelectionBar3.png"
#define LEVEL_EDITOR_BANNER_FILE4 "backgrounds/LevelEditorSelectionBar4.png"
#define LEVEL_EDITOR_BANNER_FILE5 "backgrounds/LevelEditorSelectionBar5.png"
#define LEVEL_EDITOR_BANNER_FILE6 "backgrounds/LevelEditorSelectionBar6.png"
#define LEVEL_EDITOR_BANNER_FILE7 "backgrounds/LevelEditorSelectionBar7.png"
#define FREEDROID_LOADING_PICTURE_NAME "backgrounds/startup1.jpg"
#define MOUSE_BUTTON_CHA_BACKGROUND_PICTURE "mouse_buttons/CHAButton.png"           
#define MOUSE_BUTTON_INV_BACKGROUND_PICTURE "mouse_buttons/INVButton.png"           
#define MOUSE_BUTTON_SKI_BACKGROUND_PICTURE "mouse_buttons/SKIButton.png"           
#define MOUSE_BUTTON_PLUS_BACKGROUND_PICTURE "mouse_buttons/PLUSButton.png"          
#define CHAT_BACKGROUND_IMAGE_FILE "backgrounds/conversation.jpg"
#define TO_BG_FILE		"lanzz_theme/to_background.jpg"

#define ALL_KNOWN_BACKGROUNDS 29

static iso_image our_backgrounds [ ALL_KNOWN_BACKGROUNDS ] ;
static int backgrounds_should_be_loaded_now = TRUE;

/* ----------------------------------------------------------------------
 * For blitting backgrounds and static images in various positions of the
 * game, we got this function, that handles them, taking especal care to
 * use open-gl textures for faster blitting in OpenGL settings.
 * ---------------------------------------------------------------------- */
void 
blit_special_background ( int background_code )
{
  SDL_Surface* tmp_surf_1;
  SDL_Rect src_rect;
  static char* background_filenames [ ALL_KNOWN_BACKGROUNDS ] = { INVENTORY_SCREEN_BACKGROUND_FILE ,  // 0
								  CHARACTER_SCREEN_BACKGROUND_FILE ,  // 1 
								  SKILL_SCREEN_BACKGROUND_FILE ,      // 2
								  SKILL_EXPLANATION_SCREEN_BACKGROUND_FILE , // 3
								  NE_TITLE_PIC_FILE ,                 // 4
								  NE_CREDITS_PIC_FILE ,               // 5
								  SHOP_BACKGROUND_IMAGE ,             // 6
								  ITEM_BROWSER_BG_PIC_FILE ,          // 7
								  ITEM_BROWSER_SHOP_FILE ,            // 8 
								  NE_CONSOLE_FG_1_FILE ,              // 9 
								  NE_CONSOLE_FG_2_FILE ,              // 10
								  NE_CONSOLE_FG_3_FILE ,              // 11
								  NE_CONSOLE_FG_4_FILE ,              // 12
								  NE_CONSOLE_BG_PIC1_FILE ,           // 13
                                                                  LEVEL_EDITOR_BANNER_FILE1 ,          // 14
                                                                  LEVEL_EDITOR_BANNER_FILE2 ,          // 15
                                                                  LEVEL_EDITOR_BANNER_FILE3 ,          // 16
                                                                  LEVEL_EDITOR_BANNER_FILE4 ,          // 17
                                                                  LEVEL_EDITOR_BANNER_FILE5 ,          // 18
                                                                  LEVEL_EDITOR_BANNER_FILE6 ,          // 19
                                                                  LEVEL_EDITOR_BANNER_FILE7 ,          // 20
								  FREEDROID_LOADING_PICTURE_NAME ,    // 21
								  MOUSE_BUTTON_CHA_BACKGROUND_PICTURE , // 22
								  MOUSE_BUTTON_INV_BACKGROUND_PICTURE , // 23
								  MOUSE_BUTTON_SKI_BACKGROUND_PICTURE , // 24 
								  MOUSE_BUTTON_PLUS_BACKGROUND_PICTURE , // 25
								  CHAT_BACKGROUND_IMAGE_FILE ,        // 26
								  CHAT_BACKGROUND_IMAGE_FILE ,        // 27
                                                                  TO_BG_FILE };        // 28

  SDL_Rect our_background_rects [ ALL_KNOWN_BACKGROUNDS ] = { { 0 , 0 , 0 , 0 } ,               // 0
							      { CHARACTERRECT_X , 0 , 0 , 0 } , // 1 
							      { CHARACTERRECT_X , 0 , 0 , 0 } , // 2
							      { 0 , 0 , 0 , 0 } ,               // 3
							      { 0 , 0 , 0 , 0 } ,               // 4 
							      { 0 , 0 , 0 , 0 } ,               // 5
							      { 0 , 0 , 0 , 0 } ,               // 6
							      { 0 , 0 , 0 , 0 } ,               // 7
							      { 0 , 0 , 0 , 0 } ,               // 8

							      { 32, 180, CONS_MENU_LENGTH, CONS_MENU_HEIGHT } , // 9
							      { 32, 180, CONS_MENU_LENGTH, CONS_MENU_HEIGHT } , // 10
							      { 32, 180, CONS_MENU_LENGTH, CONS_MENU_HEIGHT } , // 11
							      { 32, 180, CONS_MENU_LENGTH, CONS_MENU_HEIGHT } , // 12
							      { 0 , 0 , 0 , 0 } ,               // 13
							      { 0 , 0 , 0 , 0 } ,               // 14
							      { 0 , 0 , 0 , 0 } ,               // 15
							      { 0 , 0 , 0 , 0 } ,               // 16
							      { 0 , 0 , 0 , 0 } ,               // 17
							      { 0 , 0 , 0 , 0 } ,               // 18
							      { 0 , 0 , 0 , 0 } ,               // 19
							      { 0 , 0 , 0 , 0 } ,               // 20
							      { 0 , 0 , 0 , 0 } ,               // 21
							      { 560 , 434 ,  38 ,  45 } ,       // 22
							      { 600 , 420 ,  38 ,  40 } ,       // 23 
							      { 590 , 376 ,  38 ,  47 } ,       // 24
							      { 560 , 434 ,  38 ,  45 } ,       // 25
                                                              { 0 , 0 , 0 , 0 } ,               // 26
                                                              { CHAT_SUBDIALOG_WINDOW_X , 
								CHAT_SUBDIALOG_WINDOW_Y , 
								CHAT_SUBDIALOG_WINDOW_W , 
								CHAT_SUBDIALOG_WINDOW_H } ,     // 27
                                                              { 0 , 0 , 0 , 0 } } ;             // 28
  int i;
  char *fpath;
  
  //--------------------
  // On the first function call, we load all the surfaces we will need, and
  // in case of OpenGL output method, we also make textures from them...
  //
  if ( backgrounds_should_be_loaded_now )
    {
      backgrounds_should_be_loaded_now = FALSE ; 
      for ( i = 0 ; i < ALL_KNOWN_BACKGROUNDS ; i ++ )
	{

	  fpath = find_file ( background_filenames [ i ] , GRAPHICS_DIR , FALSE );
	  get_iso_image_from_file_and_path ( fpath , & ( our_backgrounds [ i ] ) , FALSE ) ;

	  //--------------------
	  // For the dialog, we need not only the dialog background, but also some smaller
	  // parts of the background image, so we can re-do the background part that is in
	  // the dialog partners chat output window.  We don't make a separate image on disk
	  // but rather extract the info inside the code.  That makes for easier adaption
	  // of the window dimensions from inside the code...
	  //
	  if ( i == CHAT_DIALOG_BACKGROUND_EXCERPT_CODE )
	    {
	      tmp_surf_1 = SDL_CreateRGBSurface ( SDL_SWSURFACE , CHAT_SUBDIALOG_WINDOW_W , CHAT_SUBDIALOG_WINDOW_H , 
						  32 , 0x000000ff , 0x0000ff00 , 0x00ff0000 , 0xff000000 );

	      src_rect . x = CHAT_SUBDIALOG_WINDOW_X ;
	      src_rect . w = CHAT_SUBDIALOG_WINDOW_W ;
	      src_rect . h = CHAT_SUBDIALOG_WINDOW_H ;
	      //--------------------
	      // With OpenGL, the image is flipped at this point already, so we
	      // just copy the image in flipped form, cause later it should be 
	      // flipped anyway.  Cool, eh?  Of course this way only the location
	      // of the rectangle has to be adapted a bit...
	      //
	      if ( use_open_gl )
		{
		  src_rect . y = 480 - CHAT_SUBDIALOG_WINDOW_Y - CHAT_SUBDIALOG_WINDOW_H ;
		}
	      else
		{
		  src_rect . y = CHAT_SUBDIALOG_WINDOW_Y ;
		}

	      SDL_BlitSurface ( our_backgrounds [ i ] . surface , &src_rect , tmp_surf_1 , NULL );
	      SDL_FreeSurface ( our_backgrounds [ i ] . surface ) ;
	      our_backgrounds [ i ] . surface = SDL_DisplayFormat ( tmp_surf_1 );
	    }

	  if ( use_open_gl )
	    {
	      make_texture_out_of_surface ( & ( our_backgrounds [ i ] ) ) ;
	    }
	}
    }

  //--------------------
  // Now that all the surfaces are loaded, we can start to blit the backgrounds
  // in question to their proper locations.
  //
  if ( use_open_gl )
    {
      blit_open_gl_texture_to_screen_position ( our_backgrounds [ background_code ] , our_background_rects [ background_code ] . x , our_background_rects [ background_code ] . y , TRUE ) ;
    }
  else
    {
      SDL_SetClipRect( Screen, NULL );
      our_SDL_blit_surface_wrapper ( our_backgrounds [ background_code ] . surface , NULL , Screen , &( our_background_rects [ background_code ] ) );
    }
  
}; // void blit_special_background ( int background_code )

/* ----------------------------------------------------------------------
 * Sometimes it might be convenient for development purposes, that the
 * backgrounds can be exchanged on disk and the game need not be restarted
 * to try out these new backgrounds.  So we introduce some 'cache flushing'
 * function here...
 * ---------------------------------------------------------------------- */
void
flush_background_image_cache ( void )
{
  int i;
  static iso_image empty_image = UNLOADED_ISO_IMAGE ;

  //--------------------
  // Of course the display function must be informed, that it must
  // reload all background images...
  //
  backgrounds_should_be_loaded_now = TRUE;

  //--------------------
  // Also we dutifully set all background variables to 'empty'
  // status, cause re-initializing stuff might cause error or
  // warning messages, if the 'has_been_created' flags are still
  // set from last time...
  //
  for ( i = 0 ; i < ALL_KNOWN_BACKGROUNDS ; i ++ )
    {
      memcpy ( & ( our_backgrounds [ i ] ) , & empty_image , sizeof ( iso_image ) );
    }
 
}; // void flush_background_image_cache ( void )

#undef _open_gl_c
