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

#include <GL/gl.h>
#include <GL/glu.h>

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
  int bytes;
  int target_x, target_y ;

  if ( use_open_gl )
    {

      if ( src == NULL ) 
	{
	  DebugPrintf ( -1 , "\nNull source surface received. --> doing nothing." );
	  fflush ( stdout );
	  
	  return ( 0 );
	}

      if ( dst == Screen )
	{
	  
	  // DebugPrintf ( -1 , "\nReached our_SDL_blit_surface_wrapper." );
	  // fflush ( stdout );

	  if ( dstrect == NULL )
	    glRasterPos2f( 1 , 479 );
	  else
	    {
	      if ( dstrect -> x == 0 )
		target_x = 1 ;
	      else
		target_x = dstrect -> x ;

	      target_y = dstrect -> y + src -> h ;

	      if ( target_y >= 480 ) target_y = 478 ; 

	      glRasterPos2f( target_x , target_y ) ;
	    }

	  /*
	  if ( src -> flags & SDL_SRCALPHA )
	    glDrawPixels( src -> w , src -> h, GL_RGBA , GL_UNSIGNED_BYTE , src -> pixels );
	  else
	    glDrawPixels( src -> w , src -> h, GL_RGB , GL_UNSIGNED_BYTE , src -> pixels );
	  */

	  if ( src -> w != 0 )
	    bytes = src -> pitch / src -> w ;
	  else
	    {
	      DebugPrintf ( -1 , "\nSurface of width 0 encountered. --> doing nothing." );
	      fflush ( stdout );
	      return ( 0 ) ;
	    }

	  // DebugPrintf ( -1 , "\nSurface has bytes: %d. " , bytes );
	  // fflush ( stdout );
	  
	  if ( srcrect != NULL )
	    {

	      /*
	      for ( y = 0 ; y < srcrect -> h ; y ++ )
		{

		  for ( x = 0 ; x < srcrect -> w ; x ++ )
		    {
		      PutPixel ( Screen , x + dstrect -> x , y + dstrect -> y , GetPixel ( src , srcrect -> x + x , srcrect -> y + y ) ) ;
		    }
		}
	      */

	      DebugPrintf ( -1 , "\nNon-Null source rect encountered. --> doing nothing." );
	      fflush ( stdout );
	      return ( 0 ) ;
	    }

	  if ( bytes == 4 )
	    {
	      glEnable( GL_ALPHA_TEST );  
	      glAlphaFunc ( GL_GREATER , 0.05 ) ;
	      glDrawPixels( src -> w , src -> h, GL_BGRA , GL_UNSIGNED_BYTE , src -> pixels );
	    }
	  else if ( bytes == 3 )
	    {
	      DebugPrintf ( -1 , "\nSurface has bytes: %d. " , bytes );
	      fflush ( stdout );
	      glDrawPixels( src -> w , src -> h, GL_RGB , GL_UNSIGNED_BYTE , src -> pixels );
	    }
	  else if ( bytes == 2 )
	    {
	      DebugPrintf ( -1 , "\nSurface has bytes: %d. --> using GL_UNSIGNED_SHORT_5_6_5. " , bytes );
	      fflush ( stdout );
	      glDrawPixels( src -> w , src -> h, GL_RGB , GL_UNSIGNED_SHORT_5_6_5 , src -> pixels );
	    }
	  else
	    {
	      DebugPrintf ( -1 , "\nSurface has bytes: %d.--> doing nothing. " , bytes );
	      fflush ( stdout );
	    }

	  return ( 0 ) ;
	}

      return SDL_BlitSurface ( src, srcrect, dst, dstrect);
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
  Uint8 r , g , b , a ;

  if ( use_open_gl )
    {
      if ( dst == Screen )
	{
	  SDL_GetRGBA( color, Screen -> format , &r, &g, &b, &a);
	  glRasterPos2i ( 0 , 0 ); 
	  glDisable ( GL_ALPHA_TEST );
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
};

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
 *
 *
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
 *
 *
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
      DebugPrintf ( -1 , "\nWARNING!  Texture x < 64 encountered.  Raising to 64 x." ) ;
      x = 64 ;
    }
  if ( y < 64 )
    {
      DebugPrintf ( -1 , "\nWARNING!  Texture y < 64 encountered.  Raising to 64 y." ) ;
      y = 64 ;
    }

  DebugPrintf ( -1 , "\nPadding image to texture size: final is x=%d, y=%d." , x , y );
  
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
 *
 *
 * ---------------------------------------------------------------------- */
void
make_texture_out_of_surface ( iso_image* our_image ) 
{
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
  // 
  // The following code can be tested to check if the pagging procedure
  // above really does what it should.
  //

  //--------------------
  // Having prepared the raw image it's now time to create the real
  // textures.
  //
  glPixelStorei( GL_UNPACK_ALIGNMENT,1 );

  // Create The Texture 
  glGenTextures( 1, & our_image -> texture );

  //--------------------
  // Typical Texture Generation Using Data From The Bitmap 
  //
  glBindTexture( GL_TEXTURE_2D, our_image -> texture );
  
  //--------------------
  // Maybe we will want to set some storage parameters, but I'm not
  // completely sure if they would really help us in any way...
  //
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 0);
  // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  // glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  // glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  
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

}; // void make_texture_out_of_surface ( & ( floor_iso_images [ tile_type ] ) ) 

/* ----------------------------------------------------------------------
 * Initialize the OpenGL interface.
 * ---------------------------------------------------------------------- */
int 
initGL( GLvoid )
{
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
  
  return( TRUE );

}; // int initGL( GLvoid )

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_open_gl_texture_to_map_position ( iso_image our_floor_iso_image , float our_col , float our_line ) 
{
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
  // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
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
  
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
  // glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

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
  
  glBindTexture( GL_TEXTURE_2D, our_floor_iso_image . texture );
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
  
}; // void blit_open_gl_texture_to_map_position ( iso_image our_floor_iso_image , float our_col , float our_line ) 

/* ----------------------------------------------------------------------
 *
 *
 * ---------------------------------------------------------------------- */
void
blit_open_gl_texture_to_screen_position ( iso_image our_floor_iso_image , int x , int y ) 
{
  SDL_Rect target_rectangle;
  float texture_start_y;
  float texture_end_y;
  int image_start_x;
  int image_end_x;
  int image_start_y;
  int image_end_y;

  //--------------------
  // Linear Filtering is slow and maybe not nescessary here, so we
  // stick to the faster 'nearest' variant.
  //
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  
  //--------------------
  // Now of course we need to find out the proper target position.
  //
  target_rectangle . x = x ;
  target_rectangle . y = y ;
  
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

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

  glBindTexture( GL_TEXTURE_2D, our_floor_iso_image . texture );
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

}; // void blit_open_gl_texture_to_pixel_position ( iso_image our_floor_iso_image , int x, int y ) 


#undef _open_gl_c
