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
 *
 *
 * ---------------------------------------------------------------------- */
int 
our_SDL_flip_wrapper ( SDL_Surface *screen )
{
  if ( use_open_gl )
    SDL_GL_SwapBuffers( ); 
  else
    return ( SDL_Flip ( Screen ) ) ;

  return ( 0 );
}; // int SDL_Flip ( SDL_Surface *screen )

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

#ifdef HAVE_LIBGL

  SDL_Surface* right_sized_image ;

  //--------------------
  // This fill up the image with transparent material, so that 
  // it will have powers of 2 as the dimensions, which is a requirement
  // for textures on most OpenGL capable cards.
  //
  right_sized_image = pad_image_for_texture ( our_image -> surface ) ;
  
  our_image -> texture_width = right_sized_image -> w ;
  our_image -> texture_height = right_sized_image -> h ;

  //--------------------
  // Having prepared the raw image it's now time to create the real
  // textures.
  //
  glPixelStorei( GL_UNPACK_ALIGNMENT,1 );

  //--------------------
  // We must not call glGenTextures more than once in all of Freedroid,
  // according to the nehe docu and also confirmed instances of textures
  // getting overwritten.  So all the gentexture stuff is now in the
  // initGL function and we'll use stuff from there.
  //
  // glGenTextures( 1, & our_image -> texture );
  //
  our_image -> texture = & ( all_freedroid_textures [ next_texture_index_to_use ] ) ;
  next_texture_index_to_use ++ ;
  if ( next_texture_index_to_use >= MAX_AMOUNT_OF_TEXTURES_WE_WILL_USE )
    {
      GiveStandardErrorMessage ( "make_texture_out_of_surface(...)" , "\
Ran out of initialized texture positions to use for new textures.",
				 PLEASE_INFORM, IS_FATAL );
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

#endif

}; // void make_texture_out_of_surface ( & ( floor_iso_images [ tile_type ] ) ) 

/* ----------------------------------------------------------------------
 * Initialize the OpenGL interface.
 * ---------------------------------------------------------------------- */
#ifdef HAVE_LIBGL
int 
initGL( GLvoid )
{
#else
int 
initGL( void )
{
#endif


#ifdef HAVE_LIBGL

  //--------------------
  // Set up the screne, viewport matrix, coordinate system and all that...
  //
  glViewport(0,0,640,480);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f,640.0f,480.0f,0.0f,-1.0f,1.0f);
  glMatrixMode(GL_MODELVIEW);

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

}; // int initGL( GLvoid )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_open_gl_texture_to_map_position ( iso_image our_floor_iso_image , float our_col , float our_line , float r, float g , float b ) 
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
  // glEnable(GL_BLEND);
  // glBlendFunc( GL_SRC_ALPHA , GL_ONE );
  //
  glEnable( GL_ALPHA_TEST );  
  glAlphaFunc ( GL_GREATER , 0.5 ) ;
  
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

  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
  glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

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
  
  // glColor3f( 0.5 , 0.5 , 0.5 );
  glColor3f( r , g , b );
  // glColor4f( 1, 1 , 1 , 1 );

  glBindTexture( GL_TEXTURE_2D, * ( our_floor_iso_image . texture ) );

  // glColor4f(1,1,1,1);

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

}; // void blit_open_gl_texture_to_map_position ( iso_image our_floor_iso_image , float our_col , float our_line ) 

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
  image_end_x = target_rectangle . x + our_floor_iso_image . texture_width * LIGHT_RADIUS_CRUDENESS_FACTOR  ; // + 255
  image_start_y = target_rectangle . y ;
  image_end_y = target_rectangle . y + our_floor_iso_image . texture_height * LIGHT_RADIUS_CRUDENESS_FACTOR ; // + 127
  
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

}; // void blit_open_gl_texture_to_pixel_position ( iso_image our_floor_iso_image , int x, int y ) 

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

  corner1 . x = 0 - our_iso_image . surface -> w / 2 ;
  corner1 . y = 0 - our_iso_image . surface -> h / 2 ;
  corner2 . x = 0 - our_iso_image . surface -> w / 2 ;
  corner2 . y = 0 + our_iso_image . surface -> h / 2 + ( our_iso_image . texture_height - our_iso_image . surface -> h );
  corner3 . x = 0 + our_iso_image . surface -> w / 2 + ( our_iso_image . texture_width - our_iso_image . surface -> w );
  corner3 . y = 0 + our_iso_image . surface -> h / 2 + ( our_iso_image . texture_height - our_iso_image . surface -> h );
  corner4 . x = 0 + our_iso_image . surface -> w / 2 + ( our_iso_image . texture_width - our_iso_image . surface -> w );
  corner4 . y = 0 - our_iso_image . surface -> h / 2 ;

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

  texture_start_y = 1.0 ; // 1 - ((float)(our_iso_image . surface -> h)) / 127.0 ; // 1.0 
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
	  get_iso_image_from_file_and_path ( fpath , & ( light_radius_chunk [ i ] ) ) ;
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
  glDrawPixels( 1 , 1, GL_RGBA , GL_UNSIGNED_BYTE , & pixel );
#endif

}; // void PutPixel_open_gl ( x , y , pixel ) ;


/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
GL_HighlightRectangle ( SDL_Surface* Surface , SDL_Rect Area )
{
#ifdef HAVE_LIBGL
  SDL_Rect* dstrect = & Area ;

  glRasterPos2i ( 0 , 0 ); 
  glDisable ( GL_ALPHA_TEST );
  glEnable ( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

  glColor4ub( 255 , 255 , 255 , 200 );
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

/* ----------------------------------------------------------------------
 * For blitting backgrounds and static images in various positions of the
 * game, we got this function, that handles them, taking especal care to
 * use open-gl textures for faster blitting in OpenGL settings.
 * ---------------------------------------------------------------------- */
void 
blit_special_background ( int background_code )
{
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

#define ALL_KNOWN_BACKGROUNDS 14

  static iso_image our_backgrounds [ ALL_KNOWN_BACKGROUNDS ] ;
  static int first_call = TRUE;
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
								  NE_CONSOLE_BG_PIC1_FILE } ;         // 13

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
							      { 0 , 0 , 0 , 0 } };              // 13
  int i;
  char *fpath;
  
  //--------------------
  // On the first function call, we load all the surfaces we will need, and
  // in case of OpenGL output method, we also make textures from them...
  //
  if ( first_call )
    {
      first_call = FALSE ; 
      for ( i = 0 ; i < ALL_KNOWN_BACKGROUNDS ; i ++ )
	{

	  fpath = find_file ( background_filenames [ i ] , GRAPHICS_DIR , FALSE );
	  get_iso_image_from_file_and_path ( fpath , & ( our_backgrounds [ i ] ) ) ;

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

#undef _open_gl_c
