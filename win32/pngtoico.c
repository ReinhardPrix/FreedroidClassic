/* $Id$ */
/* ----------------------------------------------------------------------- *
 *   
 *   Copyright 2001 H. Peter Anvin - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 53 Temple Place Ste 330,
 *   Bostom MA 02111-1307, USA; either version 2 of the License, or
 *   (at your option) any later version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */

/*
 * pngtoico.c
 *
 * Convert a set of palettized .png files to a Windoze .ico file
 *
 * The .png files may have transparency which will be reflected in
 * the .ico file; however, partial transparency (alpha values other
 * than 0 and 255) are not supported by the .ico format.
 *
 * Usage: pngtoico file1.png file2.png file3.png > file.ico
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <png.h>

#define min(x,y) (((x) < (y)) ? (x) : (y))
#define max(x,y) (((x) > (y)) ? (x) : (y))

/* argv[0] */
const char *program;

/* Functions to write unaligned littleendian data */
static inline void
write_le16(uint8_t *ptr, uint16_t v)
{
#if defined(__i386__) || defined(__x86_64__)
  *(uint16_t *)ptr = v;
#else
  ptr[0] = (v & 0xff);
  ptr[1] = (v >> 8) & 0xff;
#endif
}

static inline void
write_le32(uint8_t *ptr, uint32_t v)
{
#if defined(__i386__) || defined(__x86_64__)
  *(uint32_t *)ptr = v;
#else
  ptr[0] = (v & 0xff);
  ptr[1] = (v >> 8) & 0xff;
  ptr[2] = (v >> 16) & 0xff;
  ptr[3] = (v >> 24) & 0xff;
#endif
}

struct ico_common {
  uint8_t resv[2];
  uint8_t type[2];		/* == 1 */
  uint8_t count[2];		/* Number of icons */
};

struct ico_hdr {
  uint8_t width;
  uint8_t height;
  uint8_t colorcount;
  uint8_t resv;			/* Always 0 */
  uint8_t planes[2];		/* Always 1 */
  uint8_t bitcount[2];		/* Bits per pixel (1, 4, 8) */
  uint8_t sizeinbytes[4];
  uint8_t fileoffset[4];
};

struct ico_info {
  uint8_t headersize[4];
  uint8_t width[4];
  uint8_t height[4];		/* Actually 2xheight */
  uint8_t planes[2];		/* Always 1 */
  uint8_t bitcount[2];		/* Bits per pixel (1, 4, 8) */
  uint8_t unused[6*4];		/* Always 0 */
};

struct ico_color {
  uint8_t b, g, r;
  uint8_t resv;
};

struct ico_fragment {
  struct ico_fragment *next;
  uint32_t offset;

  struct ico_hdr  hdr;
  struct ico_info info;
  struct ico_color *pal;
  uint32_t pal_size;
  uint8_t *data;
  uint32_t data_size;
  uint8_t *alpha;
  uint32_t alpha_size;
};

void *
xmalloc(size_t s)
{
  void *p = malloc(s);
  if ( !p ) {
    perror(program);
    exit(1);
  }

  return p;
}

void *
zalloc(size_t s)
{
  void *p = xmalloc(s);
  memset(p, 0, s);
  return p;
}

/*
 * Read a single PNG file and create corresponding ICO header and
 * body data.
 */
static struct ico_fragment *
read_png(const char *file)
{
  struct ico_fragment *frag;
  struct ico_hdr *hdr;
  struct ico_info *info;
  struct ico_color *pal = NULL;
  uint8_t *data = NULL;
  uint8_t *alpha = NULL;
  FILE *fp = NULL;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL, end_info = NULL;
  png_uint_32 width, height;
  int dwidth, awidth, bit_depth, color_type;
  png_colorp palette;
  int num_palette, max_colors;
  png_bytep trans;
  int num_trans;
  uint8_t **row_pointers;
  uint8_t *dbyte, *abyte;
  int dshift, ashift, pixelshift;
  unsigned int x, y;
  int i;
  int datasize, alphasize;

  frag = zalloc(sizeof(*frag));
  
  hdr  = &frag->hdr;
  info = &frag->info;

  write_le32(info->headersize, sizeof(*info));
  write_le16(hdr->planes, 1);
  write_le16(info->planes, 1);

  fp = fopen(file, "rb");
  if ( !fp ) {
    goto perrexit;
  }

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
				   NULL, NULL, NULL);
  if ( !png_ptr ) {
    errno = ENOMEM;
    goto perrexit;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if ( !info_ptr ) {
    errno = ENOMEM;
    goto perrexit;
  }

  end_info = png_create_info_struct(png_ptr);
  if ( !end_info ) {
    errno = ENOMEM;
    goto perrexit;
  }

  if ( setjmp(png_ptr->jmpbuf) ) {
    fprintf(stderr, "%s: PNG format error\n", file);
    goto errexit;
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);
  

  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	       NULL, NULL, NULL);

  if ( height > 255 || width > 255 ) {
    fprintf(stderr, "%s: Image too large\n", file);
    goto errexit;
  }

  if ( color_type != PNG_COLOR_TYPE_PALETTE ) {
    fprintf(stderr, "%s: Not a palettized image\n", file);
    goto errexit;
  }

  if ( bit_depth > 8 ) {
    fprintf(stderr, "%s: Too many colors used\n", file);
    goto errexit;
  }

  if ( !png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette) ) {
    fprintf(stderr, "%s: Palette data missing\n", file);
    goto errexit;
  }
  if ( !png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, 0) ) {
    num_trans = 0;		/* Everything is fully opaque */
  }
  
  /* Expand data to one pixel/byte */
  if ( bit_depth < 8 )
    png_set_packing(png_ptr);

  /* Activate transforms */
  png_read_update_info(png_ptr, info_ptr);

  row_pointers = xmalloc(sizeof(uint8_t *) * height);
  for ( y = 0 ; y < height ; y++ ) {
    row_pointers[y] = xmalloc(width);
  }

  png_read_image(png_ptr, row_pointers);

  /* Now we have the image in memory.  Allocate memory for the
     icon output data. */

  hdr->width = width;   write_le32(info->width, width);
  hdr->height = height; write_le32(info->height, 2*height);

  if ( num_palette <= 2 ) {
    /* 1 bit/pixel */
    dwidth = ((width+31) & ~31) >> 3;
    pixelshift = 1;
  } else if ( num_palette <= 16 ) {
    /* 4 bits/pixel */
    dwidth = ((width+7) & ~7) >> 1;
    pixelshift = 4;
  } else {
    /* 8 bits/pixel */
    dwidth = (width+3) & ~3;
    pixelshift = 8;
  }
  max_colors = 1 << pixelshift;
  awidth = ((width+31) & ~31) >> 3;

  write_le16(hdr->bitcount,  pixelshift);
  write_le16(info->bitcount, pixelshift);

  frag->data  = data  = zalloc(datasize  = dwidth*height);
  frag->data_size = datasize;
  frag->alpha = alpha = zalloc(alphasize = awidth*height);
  frag->alpha_size = alphasize;

  hdr->colorcount = max_colors & 0xff;
  frag->pal = pal = zalloc(max_colors * sizeof(*pal));
  frag->pal_size = max_colors * sizeof(*pal);

  write_le32(hdr->sizeinbytes,
	     sizeof(*info)+frag->pal_size+datasize+alphasize);

  for ( i = 0 ; i < min(num_palette, max_colors) ; i++ ) {
    pal[i].r = palette[i].red;
    pal[i].g = palette[i].green;
    pal[i].b = palette[i].blue;
  }
  
  for ( y = 0 ; y < height ; y++ ) {
    dbyte = data  + (height-y-1)*dwidth;
    abyte = alpha + (height-y-1)*awidth;
    dshift = 8-pixelshift;  ashift = 7;

    for ( x = 0 ; x < width ; x++ ) {
      uint8_t index = row_pointers[y][x];
      uint8_t pixel = index;
      uint8_t ptran = (index < num_trans) ? trans[index] : 255;

      if ( ptran == 0 ) {
	pixel = 0;
      } else if ( ptran != 255 ) {
	  fprintf(stderr, "%s: Warning: partial transparency (%d/255) ignored\n",
		  file, ptran);
      }

      *dbyte |= pixel << dshift;
      dshift -= pixelshift;
      if ( dshift < 0 ) {
	dshift += 8;
	dbyte++;
      }

      *abyte |= (ptran ? 0 : 1) << ashift;
      ashift--;
      if ( ashift < 0 ) {
	ashift += 8;
	abyte++;
      }
    }
    
    /* Done with this row */
    free(row_pointers[y]);
  }
  free(row_pointers);

  /* Okay, we're done, clean up and return success */

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
  return frag;

 perrexit:
  perror(file);
 errexit:
  if ( png_ptr ) {
    png_destroy_read_struct(png_ptr ? &png_ptr : NULL,
			    info_ptr ? &info_ptr : NULL,
			    end_info ? &end_info : NULL);
  }
  if ( fp ) fclose(fp);
  if ( pal )   free(pal);
  if ( data )  free(data);
  if ( alpha ) free(alpha);
  free(frag);
  return NULL;
}

/*
 * Main program: read all files, then stitch the ICO file together.
 */

int main(int argc, char *argv[])
{
  struct ico_fragment *frag;
  struct ico_fragment *list = NULL;
  struct ico_fragment **tail = &list;
  struct ico_common common;
  int count = 0;
  int i;
  uint32_t offset;

  for ( i = 1 ; i < argc ; i++ ) {
    if ( !(frag = read_png(argv[i])) )
      return 1;

    count++;
    *tail = frag;
    tail = &frag->next;
  }

  memset(&common, 0, sizeof(common));
  write_le16(common.type, 1);
  write_le16(common.count, count);

  /* Write header */
  fwrite(&common, sizeof(common), 1, stdout);

  offset = sizeof(common) + count*sizeof(struct ico_hdr);

  for ( frag = list ; frag ; frag = frag->next ) {
    write_le32(frag->hdr.fileoffset, offset);
    fwrite(&frag->hdr, sizeof(struct ico_hdr), 1, stdout);

    offset += sizeof(struct ico_info) + frag->pal_size + frag->data_size + frag->alpha_size;
  }

  for ( frag = list ; frag ; frag = frag->next ) {
    fwrite(&frag->info, sizeof(struct ico_info), 1, stdout);
    fwrite(frag->pal, 1, frag->pal_size, stdout);
    fwrite(frag->data, 1, frag->data_size, stdout);
    fwrite(frag->alpha, 1, frag->alpha_size, stdout);
  }

  return 0;
}

