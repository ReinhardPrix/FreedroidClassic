/*  

  SDL_rotozoom.c - rotozoomer for 32bit or 8bit surfaces

  LGPL (c) A. Schiffler

*/

#include "proto.h"
#include "SDL_rotozoom.h"

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))

/* 
 
 32bit Zoomer with optional anti-aliasing by bilinear interpolation.

 Zoomes 32bit RGBA/ABGR 'src' surface to 'dst' surface.
 
*/

int zoomSurfaceRGBA(SDL_Surface * src, SDL_Surface * dst, int smooth)
{
    int x, y, sx, sy, *sax, *say, *csax, *csay, csx, csy, ex, ey, t1, t2, sstep;
    tColorRGBA *c00, *c01, *c10, *c11;
    tColorRGBA *sp, *csp, *dp;
    int sgap, dgap;

    /*
     * Variable setup 
     */
    if (smooth) {
	/*
	 * For interpolation: assume source dimension is one pixel 
	 */
	/*
	 * smaller to avoid overflow on right and bottom edge.     
	 */
	sx = (int) (65536.0 * (float) (src->w - 1) / (float) dst->w);
	sy = (int) (65536.0 * (float) (src->h - 1) / (float) dst->h);
    } else {
	sx = (int) (65536.0 * (float) src->w / (float) dst->w);
	sy = (int) (65536.0 * (float) src->h / (float) dst->h);
    }

    /*
     * Allocate memory for row increments 
     */
    if ((sax = (int *) malloc((dst->w + 1) * sizeof(Uint32))) == NULL) {
	return (-1);
    }
    if ((say = (int *) malloc((dst->h + 1) * sizeof(Uint32))) == NULL) {
	free(sax);
	return (-1);
    }

    /*
     * Precalculate row increments 
     */
    csx = 0;
    csax = sax;
    for (x = 0; x <= dst->w; x++) {
	*csax = csx;
	csax++;
	csx &= 0xffff;
	csx += sx;
    }
    csy = 0;
    csay = say;
    for (y = 0; y <= dst->h; y++) {
	*csay = csy;
	csay++;
	csy &= 0xffff;
	csy += sy;
    }

    /*
     * Pointer setup 
     */
    sp = csp = (tColorRGBA *) src->pixels;
    dp = (tColorRGBA *) dst->pixels;
    sgap = src->pitch - src->w * 4;
    dgap = dst->pitch - dst->w * 4;

    /*
     * Switch between interpolating and non-interpolating code 
     */
    if (smooth) {

	/*
	 * Interpolating Zoom 
	 */

	/*
	 * Scan destination 
	 */
	csay = say;
	for (y = 0; y < dst->h; y++) {
	    /*
	     * Setup color source pointers 
	     */
	    c00 = csp;
	    c01 = csp;
	    c01++;
	    c10 = (tColorRGBA *) ((Uint8 *) csp + src->pitch);
	    c11 = c10;
	    c11++;
	    csax = sax;
	    for (x = 0; x < dst->w; x++) {

		/*
		 * Interpolate colors 
		 */
		ex = (*csax & 0xffff);
		ey = (*csay & 0xffff);
		t1 = ((((c01->r - c00->r) * ex) >> 16) + c00->r) & 0xff;
		t2 = ((((c11->r - c10->r) * ex) >> 16) + c10->r) & 0xff;
		dp->r = (((t2 - t1) * ey) >> 16) + t1;
		t1 = ((((c01->g - c00->g) * ex) >> 16) + c00->g) & 0xff;
		t2 = ((((c11->g - c10->g) * ex) >> 16) + c10->g) & 0xff;
		dp->g = (((t2 - t1) * ey) >> 16) + t1;
		t1 = ((((c01->b - c00->b) * ex) >> 16) + c00->b) & 0xff;
		t2 = ((((c11->b - c10->b) * ex) >> 16) + c10->b) & 0xff;
		dp->b = (((t2 - t1) * ey) >> 16) + t1;
		t1 = ((((c01->a - c00->a) * ex) >> 16) + c00->a) & 0xff;
		t2 = ((((c11->a - c10->a) * ex) >> 16) + c10->a) & 0xff;
		dp->a = (((t2 - t1) * ey) >> 16) + t1;

		/*
		 * Advance source pointers 
		 */
		csax++;
		sstep = (*csax >> 16);
		c00 += sstep;
		c01 += sstep;
		c10 += sstep;
		c11 += sstep;
		/*
		 * Advance destination pointer 
		 */
		dp++;
	    }
	    /*
	     * Advance source pointer 
	     */
	    csay++;
	    csp = (tColorRGBA *) ((Uint8 *) csp + (*csay >> 16) * src->pitch);
	    /*
	     * Advance destination pointers 
	     */
	    dp = (tColorRGBA *) ((Uint8 *) dp + dgap);
	}

    } else {

	/*
	 * Non-Interpolating Zoom 
	 */

	csay = say;
	for (y = 0; y < dst->h; y++) {
	    sp = csp;
	    csax = sax;
	    for (x = 0; x < dst->w; x++) {
		/*
		 * Draw 
		 */
		*dp = *sp;
		/*
		 * Advance source pointers 
		 */
		csax++;
		sp += (*csax >> 16);
		/*
		 * Advance destination pointer 
		 */
		dp++;
	    }
	    /*
	     * Advance source pointer 
	     */
	    csay++;
	    csp = (tColorRGBA *) ((Uint8 *) csp + (*csay >> 16) * src->pitch);
	    /*
	     * Advance destination pointers 
	     */
	    dp = (tColorRGBA *) ((Uint8 *) dp + dgap);
	}

    }

    /*
     * Remove temp arrays 
     */
    free(sax);
    free(say);

    return (0);
}

/* 
 
 8bit Zoomer without smoothing.

 Zoomes 8bit palette/Y 'src' surface to 'dst' surface.
 
*/

int zoomSurfaceY(SDL_Surface * src, SDL_Surface * dst)
{
    Uint32 x, y, sx, sy, *sax, *say, *csax, *csay, csx, csy;
    Uint8 *sp, *dp, *csp;
    int dgap;

    /*
     * Variable setup 
     */
    sx = (Uint32) (65536.0 * (float) src->w / (float) dst->w);
    sy = (Uint32) (65536.0 * (float) src->h / (float) dst->h);

    /*
     * Allocate memory for row increments 
     */
    if ((sax = (Uint32 *) malloc(dst->w * sizeof(Uint32))) == NULL) {
	return (-1);
    }
    if ((say = (Uint32 *) malloc(dst->h * sizeof(Uint32))) == NULL) {
	if (sax != NULL) {
	    free(sax);
	}
	return (-1);
    }

    /*
     * Precalculate row increments 
     */
    csx = 0;
    csax = sax;
    for ( x = 0; x < ( ( Uint32 ) dst -> w ) ; x ++ ) {
	csx += sx;
	*csax = (csx >> 16);
	csx &= 0xffff;
	csax++;
    }
    csy = 0;
    csay = say;
    for ( y = 0; y < ( ( Uint32 ) dst -> h ) ; y++ ) {
	csy += sy;
	*csay = (csy >> 16);
	csy &= 0xffff;
	csay++;
    }

    csx = 0;
    csax = sax;
    for (x = 0; x < ( ( Uint32 ) dst -> w ) ; x++ ) {
	csx += (*csax);
	csax++;
    }
    csy = 0;
    csay = say;
    for ( y = 0; y < ( ( Uint32 ) dst -> h ) ; y++ ) {
	csy += (*csay);
	csay++;
    }

    /*
     * Pointer setup 
     */
    sp = csp = (Uint8 *) src->pixels;
    dp = (Uint8 *) dst->pixels;
    dgap = dst->pitch - dst->w;

    /*
     * Draw 
     */
    csay = say;
    for (y = 0; y < ( ( Uint32 ) dst -> h ) ; y++ ) {
	csax = sax;
	sp = csp;
	for ( x = 0; x < ( ( Uint32 ) dst -> w ) ; x++ ) {
	    /*
	     * Draw 
	     */
	    *dp = *sp;
	    /*
	     * Advance source pointers 
	     */
	    sp += (*csax);
	    csax++;
	    /*
	     * Advance destination pointer 
	     */
	    dp++;
	}
	/*
	 * Advance source pointer (for row) 
	 */
	csp += ((*csay) * src->pitch);
	csay++;
	/*
	 * Advance destination pointers 
	 */
	dp += dgap;
    }

    /*
     * Remove temp arrays 
     */
    free(sax);
    free(say);

    return (0);
}

/* 
 
 32bit Rotozoomer with optional anti-aliasing by bilinear interpolation.

 Rotates and zoomes 32bit RGBA/ABGR 'src' surface to 'dst' surface.
 
*/

void transformSurfaceRGBA(SDL_Surface * src, SDL_Surface * dst, int cx, int cy, int isin, int icos, int smooth)
{
    int x, y, t1, t2, dx, dy, xd, yd, sdx, sdy, ax, ay, ex, ey, sw, sh;
    tColorRGBA c00, c01, c10, c11;
    tColorRGBA *pc, *sp; //, *spb;
    int gap;

    /*
     * Variable setup 
     */
    xd = ((src->w - dst->w) << 15);
    yd = ((src->h - dst->h) << 15);
    ax = (cx << 16) - (icos * cx);
    ay = (cy << 16) - (isin * cx);
    sw = src->w - 1;
    sh = src->h - 1;
    pc = dst->pixels;
    gap = dst->pitch - dst->w * 4;

    /*
     * Switch between interpolating and non-interpolating code 
     */
    if (smooth) {
	for (y = 0; y < dst->h; y++) {
	    dy = cy - y;
	    sdx = (ax + (isin * dy)) + xd;
	    sdy = (ay - (icos * dy)) + yd;
	    for (x = 0; x < dst->w; x++) {
		dx = (sdx >> 16);
		dy = (sdy >> 16);
		if ((dx >= -1) && (dy >= -1) && (dx < src->w) && (dy < src->h)) {
		    if ((dx >= 0) && (dy >= 0) && (dx < sw) && (dy < sh)) {
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			sp += dx;
			c00 = *sp;
			sp += 1;
			c01 = *sp;
			sp = (tColorRGBA *) ((Uint8 *) sp + src->pitch);
			sp -= 1;
			c10 = *sp;
			sp += 1;
			c11 = *sp;
		    } else if ((dx == sw) && (dy == sh)) {
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			sp += dx;
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			c11 = *sp;
		    } else if ((dx == -1) && (dy == -1)) {
			sp = (tColorRGBA *) (src->pixels);
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			c11 = *sp;
		    } else if ((dx == -1) && (dy == sh)) {
			sp = (tColorRGBA *) (src->pixels);
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			c11 = *sp;
		    } else if ((dx == sw) && (dy == -1)) {
			sp = (tColorRGBA *) (src->pixels);
			sp += dx;
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			c11 = *sp;
		    } else if (dx == -1) {
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			sp = (tColorRGBA *) ((Uint8 *) sp + src->pitch);
			c11 = *sp;
		    } else if (dy == -1) {
			sp = (tColorRGBA *) (src->pixels);
			sp += dx;
			c00 = *sp;
			c01 = *sp;
			c10 = *sp;
			sp += 1;
			c11 = *sp;
		    } else if (dx == sw) {
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			sp += dx;
			c00 = *sp;
			c01 = *sp;
			sp = (tColorRGBA *) ((Uint8 *) sp + src->pitch);
			c10 = *sp;
			c11 = *sp;
		    } else if (dy == sh) {
			sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
			sp += dx;
			c00 = *sp;
			sp += 1;
			c01 = *sp;
			c10 = *sp;
			c11 = *sp;
		    }
		    /*
		     * Interpolate colors 
		     */
		    ex = (sdx & 0xffff);
		    ey = (sdy & 0xffff);
		    t1 = ((((c01.r - c00.r) * ex) >> 16) + c00.r) & 0xff;
		    t2 = ((((c11.r - c10.r) * ex) >> 16) + c10.r) & 0xff;
		    pc->r = (((t2 - t1) * ey) >> 16) + t1;
		    t1 = ((((c01.g - c00.g) * ex) >> 16) + c00.g) & 0xff;
		    t2 = ((((c11.g - c10.g) * ex) >> 16) + c10.g) & 0xff;
		    pc->g = (((t2 - t1) * ey) >> 16) + t1;
		    t1 = ((((c01.b - c00.b) * ex) >> 16) + c00.b) & 0xff;
		    t2 = ((((c11.b - c10.b) * ex) >> 16) + c10.b) & 0xff;
		    pc->b = (((t2 - t1) * ey) >> 16) + t1;
		    t1 = ((((c01.a - c00.a) * ex) >> 16) + c00.a) & 0xff;
		    t2 = ((((c11.a - c10.a) * ex) >> 16) + c10.a) & 0xff;
		    pc->a = (((t2 - t1) * ey) >> 16) + t1;
		}
		sdx += icos;
		sdy += isin;
		pc++;
	    }
	    pc = (tColorRGBA *) ((Uint8 *) pc + gap);
	}
    } else {
	for (y = 0; y < dst->h; y++) {
	    dy = cy - y;
	    sdx = (ax + (isin * dy)) + xd;
	    sdy = (ay - (icos * dy)) + yd;
	    for (x = 0; x < dst->w; x++) {
		dx = (short) (sdx >> 16);
		dy = (short) (sdy >> 16);
		if ((dx >= 0) && (dy >= 0) && (dx < src->w) && (dy < src->h)) {
		    sp = (tColorRGBA *) ((Uint8 *) src->pixels + src->pitch * dy);
		    sp += dx;
		    *pc = *sp;
		}
		sdx += icos;
		sdy += isin;
		pc++;
	    }
	    pc = (tColorRGBA *) ((Uint8 *) pc + gap);
	}
    }
}

/* 
 
 8bit Rotozoomer without smoothing

 Rotates and zoomes 8bit palette/Y 'src' surface to 'dst' surface.
 
*/

void transformSurfaceY(SDL_Surface * src, SDL_Surface * dst, int cx, int cy, int isin, int icos)
{
    int x, y, dx, dy, xd, yd, sdx, sdy, ax, ay, sw, sh;
    tColorY *pc, *sp;
    int gap;

    /*
     * Variable setup 
     */
    xd = ((src->w - dst->w) << 15);
    yd = ((src->h - dst->h) << 15);
    ax = (cx << 16) - (icos * cx);
    ay = (cy << 16) - (isin * cx);
    sw = src->w - 1;
    sh = src->h - 1;
    pc = dst->pixels;
    gap = dst->pitch - dst->w;
    /*
     * Clear surface to colorkey 
     */
    memset(pc, (unsigned char) (src->format->colorkey & 0xff), dst->pitch * dst->h);
    /*
     * Iterate through destination surface 
     */
    for (y = 0; y < dst->h; y++) {
	dy = cy - y;
	sdx = (ax + (isin * dy)) + xd;
	sdy = (ay - (icos * dy)) + yd;
	for (x = 0; x < dst->w; x++) {
	    dx = (short) (sdx >> 16);
	    dy = (short) (sdy >> 16);
	    if ((dx >= 0) && (dy >= 0) && (dx < src->w) && (dy < src->h)) {
		sp = (tColorY *) (src->pixels);
		sp += (src->pitch * dy + dx);
		*pc = *sp;
	    }
	    sdx += icos;
	    sdy += isin;
	    pc++;
	}
	pc += gap;
    }
}

/* 
 
 rotozoomSurface()

 Rotates and zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'angle' is the rotation in degrees. 'zoom' a scaling factor. If 'smooth' is 1
 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.

*/

#define VALUE_LIMIT	0.001


/* Local rotozoom-size function with trig result return */

void rotozoomSurfaceSizeTrig(int width, int height, double angle, double zoom, int *dstwidth, int *dstheight,
			     double *canglezoom, double *sanglezoom)
{
  double x, y, cx, cy, sx, sy;
  double radangle;
  int dstwidthhalf, dstheighthalf;

    /*
     * Determine destination width and height by rotating a centered source box 
     */
    radangle = angle * (M_PI / 180.0);
    *sanglezoom = sin(radangle);
    *canglezoom = cos(radangle);
    *sanglezoom *= zoom;
    *canglezoom *= zoom;
    x = width / 2;
    y = height / 2;
    cx = *canglezoom * x;
    cy = *canglezoom * y;
    sx = *sanglezoom * x;
    sy = *sanglezoom * y;
    dstwidthhalf = MAX((int)
		       ceil(MAX(MAX(MAX(fabs(cx + sy), fabs(cx - sy)), fabs(-cx + sy)), fabs(-cx - sy))), 1);
    dstheighthalf = MAX((int)
			ceil(MAX(MAX(MAX(fabs(sx + cy), fabs(sx - cy)), fabs(-sx + cy)), fabs(-sx - cy))), 1);
    *dstwidth = 2 * dstwidthhalf;
    *dstheight = 2 * dstheighthalf;
}


/* Publically available rotozoom-size function */

void rotozoomSurfaceSize(int width, int height, double angle, double zoom, int *dstwidth, int *dstheight)
{
    double dummy_sanglezoom, dummy_canglezoom;

    rotozoomSurfaceSizeTrig(width, height, angle, zoom, dstwidth, dstheight, &dummy_sanglezoom, &dummy_canglezoom);
}


/* Publically available rotozoom function */

SDL_Surface *rotozoomSurface(SDL_Surface * src, double angle, double zoom, int smooth)
{
    SDL_Surface *rz_src;
    SDL_Surface *rz_dst;
    double zoominv;
    double sanglezoom, canglezoom, sanglezoominv, canglezoominv;
    int dstwidthhalf, dstwidth, dstheighthalf, dstheight;
    // double x, y, cx, cy, sx, sy;
    int is32bit;
    int i, src_converted;

    /*
     * Sanity check 
     */
    if (src == NULL)
	return (NULL);

    /*
     * Determine if source surface is 32bit or 8bit 
     */
    is32bit = (src->format->BitsPerPixel == 32);
    // ATTENTION!! HACKING THIS!!! jp, 10.8.2002
    // is32bit = 0;
    if ((is32bit) || (src->format->BitsPerPixel == 8)) {
	/*
	 * Use source surface 'as is' 
	 */
	rz_src = src;
	src_converted = 0;
    } else {
	/*
	 * New source surface is 32bit with a defined RGBA ordering 
	 */
	rz_src =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	our_SDL_blit_surface_wrapper(src, NULL, rz_src, NULL);
	src_converted = 1;
	is32bit = 1;
    }

    /*
     * Sanity check zoom factor 
     */
    if (zoom < VALUE_LIMIT) {
	zoom = VALUE_LIMIT;
    }
    zoominv = 65536.0 / (zoom * zoom);

    /*
     * Check if we have a rotozoom or just a zoom 
     */
    if (fabs(angle) > VALUE_LIMIT) {

	/*
	 * Angle!=0: full rotozoom 
	 */
	/*
	 * ----------------------- 
	 */

	/* Determine target size */
	rotozoomSurfaceSizeTrig(rz_src->w, rz_src->h, angle, zoom, &dstwidth, &dstheight, &canglezoom, &sanglezoom);

	/*
	 * Calculate target factors from sin/cos and zoom 
	 */
	sanglezoominv = sanglezoom;
	canglezoominv = canglezoom;
	sanglezoominv *= zoominv;
	canglezoominv *= zoominv;

	/* Calculate half size */
	dstwidthhalf = dstwidth / 2;
	dstheighthalf = dstheight / 2;

	/*
	 * Alloc space to completely contain the rotated surface 
	 */
	rz_dst = NULL;
	if (is32bit) {
	    /*
	     * Target surface is 32bit with source RGBA/ABGR ordering 
	     */
	    rz_dst =
		SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 32,
				     rz_src->format->Rmask, rz_src->format->Gmask,
				     rz_src->format->Bmask, rz_src->format->Amask);
	} else {
	    /*
	     * Target surface is 8bit 
	     */
	    rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 8, 0, 0, 0, 0);
	}

	/*
	 * Lock source surface 
	 */
	SDL_LockSurface(rz_src);
	/*
	 * Check which kind of surface we have 
	 */
	if (is32bit) {
	    /*
	     * Call the 32bit transformation routine to do the rotation (using alpha) 
	     */
	    transformSurfaceRGBA(rz_src, rz_dst, dstwidthhalf, dstheighthalf,
				 (int) (sanglezoominv), (int) (canglezoominv), smooth);
	    /*
	     * Turn on source-alpha support 
	     */
	    SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
	} else {
	    /*
	     * Copy palette and colorkey info 
	     */
	    for (i = 0; i < rz_src->format->palette->ncolors; i++) {
		rz_dst->format->palette->colors[i] = rz_src->format->palette->colors[i];
	    }
	    rz_dst->format->palette->ncolors = rz_src->format->palette->ncolors;
	    /*
	     * Call the 8bit transformation routine to do the rotation 
	     */
	    transformSurfaceY(rz_src, rz_dst, dstwidthhalf, dstheighthalf,
			      (int) (sanglezoominv), (int) (canglezoominv));
	    SDL_SetColorKey(rz_dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, rz_src->format->colorkey);
	}
	/*
	 * Unlock source surface 
	 */
	SDL_UnlockSurface(rz_src);

    } else {

	/*
	 * Angle=0: Just a zoom 
	 */
	/*
	 * -------------------- 
	 */

	/*
	 * Calculate target size
	 */
	zoomSurfaceSize(rz_src->w, rz_src->h, zoom, zoom, &dstwidth, &dstheight);

	/*
	 * Alloc space to completely contain the zoomed surface 
	 */
	rz_dst = NULL;
	if (is32bit) {
	    /*
	     * Target surface is 32bit with source RGBA/ABGR ordering 
	     */
	    rz_dst =
		SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 32,
				     rz_src->format->Rmask, rz_src->format->Gmask,
				     rz_src->format->Bmask, rz_src->format->Amask);
	} else {
	    /*
	     * Target surface is 8bit 
	     */
	    rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 8, 0, 0, 0, 0);
	}

	/*
	 * Lock source surface 
	 */
	SDL_LockSurface(rz_src);
	/*
	 * Check which kind of surface we have 
	 */
	if (is32bit) {
	    /*
	     * Call the 32bit transformation routine to do the zooming (using alpha) 
	     */
	    zoomSurfaceRGBA(rz_src, rz_dst, smooth);
	    /*
	     * Turn on source-alpha support 
	     */
	    SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
	} else {
	    /*
	     * Copy palette and colorkey info 
	     */
	    for (i = 0; i < rz_src->format->palette->ncolors; i++) {
		rz_dst->format->palette->colors[i] = rz_src->format->palette->colors[i];
	    }
	    rz_dst->format->palette->ncolors = rz_src->format->palette->ncolors;
	    /*
	     * Call the 8bit transformation routine to do the zooming 
	     */
	    zoomSurfaceY(rz_src, rz_dst);
	    SDL_SetColorKey(rz_dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, rz_src->format->colorkey);
	}
	/*
	 * Unlock source surface 
	 */
	SDL_UnlockSurface(rz_src);
    }

    /*
     * Cleanup temp surface 
     */
    if (src_converted) {
	SDL_FreeSurface(rz_src);
    }

    /*
     * Return destination surface 
     */
    return (rz_dst);
}

/* 
 
 zoomSurface()

 Zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'zoomx' and 'zoomy' are scaling factors for width and height. If 'smooth' is 1
 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.

*/

#define VALUE_LIMIT	0.001

void zoomSurfaceSize(int width, int height, double zoomx, double zoomy, int *dstwidth, int *dstheight)
{
    /*
     * Sanity check zoom factors 
     */
    if (zoomx < VALUE_LIMIT) {
	zoomx = VALUE_LIMIT;
    }
    if (zoomy < VALUE_LIMIT) {
	zoomy = VALUE_LIMIT;
    }

    /*
     * Calculate target size 
     */
    *dstwidth = (int) ((double) width * zoomx);
    *dstheight = (int) ((double) height * zoomy);
    if (*dstwidth < 1) {
	*dstwidth = 1;
    }
    if (*dstheight < 1) {
	*dstheight = 1;
    }
}

SDL_Surface *zoomSurface(SDL_Surface * src, double zoomx, double zoomy, int smooth)
{
    SDL_Surface *rz_src;
    SDL_Surface *rz_dst;
    int dstwidth, dstheight;
    int is32bit;
    int i, src_converted;

    /*
     * Sanity check 
     */
    if (src == NULL)
	return (NULL);

    /*
     * Determine if source surface is 32bit or 8bit 
     */
    is32bit = (src->format->BitsPerPixel == 32);
    if ((is32bit) || (src->format->BitsPerPixel == 8)) {
	/*
	 * Use source surface 'as is' 
	 */
	rz_src = src;
	src_converted = 0;
    } else {
	/*
	 * New source surface is 32bit with a defined RGBA ordering 
	 */
	rz_src =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	our_SDL_blit_surface_wrapper(src, NULL, rz_src, NULL);
	src_converted = 1;
	is32bit = 1;
    }

    /* Get size if target */
    zoomSurfaceSize(rz_src->w, rz_src->h, zoomx, zoomy, &dstwidth, &dstheight);

    /*
     * Alloc space to completely contain the zoomed surface 
     */
    rz_dst = NULL;
    if (is32bit) {
	/*
	 * Target surface is 32bit with source RGBA/ABGR ordering 
	 */
	rz_dst =
	    SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 32,
				 rz_src->format->Rmask, rz_src->format->Gmask,
				 rz_src->format->Bmask, rz_src->format->Amask);
	if(rz_dst == NULL) {
        fprintf(stderr, "\nCreateRGBSurface failed: %s	\n", SDL_GetError());
        	exit(1);
	    }
    } else {
	/*
	 * Target surface is 8bit 
	 */
	rz_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dstwidth, dstheight, 8, 0, 0, 0, 0);
    }

    /*
     * Lock source surface 
     */
    SDL_LockSurface(rz_src);
    /*
     * Check which kind of surface we have 
     */
    if (is32bit) {
	/*
	 * Call the 32bit transformation routine to do the zooming (using alpha) 
	 */
	zoomSurfaceRGBA(rz_src, rz_dst, smooth);
	/*
	 * Turn on source-alpha support 
	 */
	SDL_SetAlpha(rz_dst, SDL_SRCALPHA, 255);
    } else {
	/*
	 * Copy palette and colorkey info 
	 */
	for (i = 0; i < rz_src->format->palette->ncolors; i++) {
	    rz_dst->format->palette->colors[i] = rz_src->format->palette->colors[i];
	}
	rz_dst->format->palette->ncolors = rz_src->format->palette->ncolors;
	/*
	 * Call the 8bit transformation routine to do the zooming 
	 */
	zoomSurfaceY(rz_src, rz_dst);
	SDL_SetColorKey(rz_dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, rz_src->format->colorkey);
    }
    /*
     * Unlock source surface 
     */
    SDL_UnlockSurface(rz_src);

    /*
     * Cleanup temp surface 
     */
    if (src_converted) {
	SDL_FreeSurface(rz_src);
    }

    //--------------------
    // If the original surface did have some color key set, this should also be 
    // true for the zoomed surface again, so we make sure these properties are
    // properly inherited...
    //
    if ( src -> flags & SDL_SRCCOLORKEY )
      {
	rz_dst -> flags =
	  rz_dst -> flags | SDL_SRCCOLORKEY ;
	
	rz_dst -> format -> colorkey =
	  src -> format -> colorkey ;
      }


    /*
     * Return destination surface 
     */
    return (rz_dst);
}
