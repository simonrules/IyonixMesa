/* $Id: xmesa3.c,v 1.3 1999/10/08 09:27:12 keithw Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.1
 * 
 * Copyright (C) 1999  Brian Paul   All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */



/*
 * Mesa/X11 interface, part 3.
 *
 * This file contains "accelerated" point, line, and triangle functions.
 * It should be fairly easy to write new special-purpose point, line or
 * triangle functions and hook them into this module.
 */
/* $XFree86: xc/lib/GL/mesa/src/X/xmesa3.c,v 1.2 1999/03/14 03:21:02 dawes Exp $ */

struct timespec;  /* this silences a compiler warning on several systems */
struct itimerspec;

#ifdef HAVE_CONFIG_H
#include "conf.h"
#endif

#ifndef XFree86Server
#include <sys/time.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#endif
#include "depth.h"
#include "macros.h"
#include "vb.h"
#include "types.h"
#include "xmesaP.h"
#ifdef XFree86Server
#include "gcstruct.h"
#include "GL/xf86glx.h"
#endif



/**********************************************************************/
/***                    Point rendering                             ***/
/**********************************************************************/


/*
 * Render an array of points into a pixmap, any pixel format.
 */
static void draw_points_ANY_pixmap( GLcontext *ctx, GLuint first, GLuint last )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XMesaDisplay *dpy = xmesa->xm_visual->display;
   XMesaDrawable buffer = xmesa->xm_buffer->buffer;
   XMesaGC gc = xmesa->xm_buffer->gc2;
   struct vertex_buffer *VB = ctx->VB;
   register GLuint i;

   if (xmesa->xm_visual->gl_visual->RGBAflag) {
      /* RGB mode */
      for (i=first;i<=last;i++) {
         if (VB->ClipMask[i]==0) {
            register int x, y;
	    const GLubyte *color = VB->ColorPtr->data[i];
            unsigned long pixel = xmesa_color_to_pixel( xmesa,
				      color[0], color[1], color[2], color[3] );
            XMesaSetForeground( dpy, gc, pixel );
            x =       (GLint) VB->Win.data[i][0];
            y = FLIP( (GLint) VB->Win.data[i][1] );
            XMesaDrawPoint( dpy, buffer, gc, x, y);
         }
      }
   }
   else {
      /* Color index mode */
      for (i=first;i<=last;i++) {
         if (VB->ClipMask[i]==0) {
            register int x, y;
            XMesaSetForeground( dpy, gc, VB->IndexPtr->data[i] );
            x =       (GLint) VB->Win.data[i][0];
            y = FLIP( (GLint) VB->Win.data[i][1] );
            XMesaDrawPoint( dpy, buffer, gc, x, y);
         }
      }
   }
}



/*
 * Analyze context state to see if we can provide a fast points drawing
 * function, like those in points.c.  Otherwise, return NULL.
 */
points_func xmesa_get_points_func( GLcontext *ctx )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;

   if (ctx->Point.Size==1.0F && !ctx->Point.SmoothFlag && ctx->RasterMask==0
       && !ctx->Texture.Enabled) {
      if (xmesa->xm_buffer->buffer==XIMAGE) {
         return NULL; /*draw_points_ximage;*/
      }
      else {
         return draw_points_ANY_pixmap;
      }
   }
   else {
      return NULL;
   }
}



/**********************************************************************/
/***                      Line rendering                            ***/
/**********************************************************************/

/*
 * Render a line into a pixmap, any pixel format.
 */
static void flat_pixmap_line( GLcontext *ctx,
                              GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   register int x0, y0, x1, y1;
   XMesaGC gc;
   unsigned long pixel;
   if (xmesa->xm_visual->gl_visual->RGBAflag) {
      const GLubyte *color = VB->ColorPtr->data[pv];
      pixel = xmesa_color_to_pixel( xmesa, color[0], color[1], color[2], color[3] );
   }
   else {
      pixel = VB->IndexPtr->data[pv];
   }
   gc = xmesa->xm_buffer->gc2;
   XMesaSetForeground( xmesa->display, gc, pixel );

   x0 =       (GLint) VB->Win.data[vert0][0];
   y0 = FLIP( (GLint) VB->Win.data[vert0][1] );
   x1 =       (GLint) VB->Win.data[vert1][0];
   y1 = FLIP( (GLint) VB->Win.data[vert1][1] );
   XMesaDrawLine( xmesa->display, xmesa->xm_buffer->buffer, gc,
		  x0, y0, x1, y1 );
}



/*
 * Draw a flat-shaded, PF_TRUECOLOR line into an XImage.
 */
static void flat_TRUECOLOR_line( GLcontext *ctx,
                                 GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   XMesaImage *img = xmesa->xm_buffer->backimage;
   unsigned long pixel;
   PACK_TRUECOLOR( pixel, color[0], color[1], color[2] );

#define INTERP_XY 1
#define CLIP_HACK 1
#define PLOT(X,Y) XMesaPutPixel( img, X, FLIP(Y), pixel );

#include "linetemp.h"
}



/*
 * Draw a flat-shaded, PF_8A8B8G8R line into an XImage.
 */
static void flat_8A8B8G8R_line( GLcontext *ctx,
                                GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLuint pixel = PACK_8B8G8R( color[0], color[1], color[2] );

#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, PF_8R8G8B line into an XImage.
 */
static void flat_8R8G8B_line( GLcontext *ctx,
                              GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLuint pixel = PACK_8R8G8B( color[0], color[1], color[2] );

#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, PF_8R8G8B24 line into an XImage.
 */
static void flat_8R8G8B24_line( GLcontext *ctx,
                              GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];

#define PIXEL_TYPE bgr_t
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR3(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) {			\
      pixelPtr->r = color[RCOMP];	\
      pixelPtr->g = color[GCOMP];	\
      pixelPtr->b = color[BCOMP];	\
}

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, PF_5R6G5B line into an XImage.
 */
static void flat_5R6G5B_line( GLcontext *ctx,
                              GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLushort pixel = PACK_5R6G5B( color[0], color[1], color[2] );

#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, PF_DITHER_5R6G5B line into an XImage.
 */
static void flat_DITHER_5R6G5B_line( GLcontext *ctx,
                                     GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];

#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) PACK_TRUEDITHER( *pixelPtr, X, Y, color[0], color[1], color[2] );

#include "linetemp.h"
}



/*
 * Draw a flat-shaded, PF_DITHER 8-bit line into an XImage.
 */
static void flat_DITHER8_line( GLcontext *ctx,
                               GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLint r = color[0], g = color[1], b = color[2];
   DITHER_SETUP;

#define INTERP_XY 1
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = DITHER(X,Y,r,g,b);

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, PF_LOOKUP 8-bit line into an XImage.
 */
static void flat_LOOKUP8_line( GLcontext *ctx,
                               GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLubyte pixel;
   LOOKUP_SETUP;
   pixel = (GLubyte) LOOKUP( color[0], color[1], color[2] );

#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = pixel;

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, PF_HPCR line into an XImage.
 */
static void flat_HPCR_line( GLcontext *ctx,
                            GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLint r = color[0], g = color[1], b = color[2];

#define INTERP_XY 1
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y) *pixelPtr = (GLubyte) DITHER_HPCR(X,Y,r,g,b);

#include "linetemp.h"
}



/*
 * Draw a flat-shaded, Z-less, PF_TRUECOLOR line into an XImage.
 */
static void flat_TRUECOLOR_z_line( GLcontext *ctx,
                                   GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   XMesaImage *img = xmesa->xm_buffer->backimage;
   unsigned long pixel;
   PACK_TRUECOLOR( pixel, color[0], color[1], color[2] );

#define INTERP_XY 1
#define INTERP_Z 1
#define CLIP_HACK 1
#define PLOT(X,Y)					\
	if (Z < *zPtr) {				\
	   *zPtr = Z;					\
           XMesaPutPixel( img, X, FLIP(Y), pixel );	\
	}

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_8A8B8G8R line into an XImage.
 */
static void flat_8A8B8G8R_z_line( GLcontext *ctx,
                                  GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLuint pixel = PACK_8B8G8R( color[0], color[1], color[2] );

#define INTERP_Z 1
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		\
	if (Z < *zPtr) {	\
	   *zPtr = Z;		\
	   *pixelPtr = pixel;	\
	}

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_8R8G8B line into an XImage.
 */
static void flat_8R8G8B_z_line( GLcontext *ctx,
                                GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLuint pixel = PACK_8R8G8B( color[0], color[1], color[2] );

#define INTERP_Z 1
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		\
	if (Z < *zPtr) {	\
	   *zPtr = Z;		\
	   *pixelPtr = pixel;	\
	}

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_8R8G8B24 line into an XImage.
 */
static void flat_8R8G8B24_z_line( GLcontext *ctx,
                                    GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];

#define INTERP_Z 1
#define PIXEL_TYPE bgr_t
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR3(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)			\
	if (Z < *zPtr) {		\
	   *zPtr = Z;			\
           pixelPtr->r = color[RCOMP];	\
           pixelPtr->g = color[GCOMP];	\
           pixelPtr->b = color[BCOMP];	\
	}

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_5R6G5B line into an XImage.
 */
static void flat_5R6G5B_z_line( GLcontext *ctx,
                                GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLushort pixel = PACK_5R6G5B( color[0], color[1], color[2] );

#define INTERP_Z 1
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		\
	if (Z < *zPtr) {	\
	   *zPtr = Z;		\
	   *pixelPtr = pixel;	\
	}
#include "linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_DITHER_5R6G5B line into an XImage.
 */
static void flat_DITHER_5R6G5B_z_line( GLcontext *ctx,
                                       GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];

#define INTERP_Z 1
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		\
	if (Z < *zPtr) {	\
	   *zPtr = Z;		\
	   PACK_TRUEDITHER(*pixelPtr, X, Y, color[0], color[1], color[2]); \
	}
#include "linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_DITHER 8-bit line into an XImage.
 */
static void flat_DITHER8_z_line( GLcontext *ctx,
                                 GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLint r = color[0], g = color[1], b = color[2];
   DITHER_SETUP;

#define INTERP_XY 1
#define INTERP_Z 1
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)						\
	if (Z < *zPtr) {					\
	   *zPtr = Z;						\
	   *pixelPtr = (GLubyte) DITHER( X, Y, r, g, b);	\
	}
#include "linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_LOOKUP 8-bit line into an XImage.
 */
static void flat_LOOKUP8_z_line( GLcontext *ctx,
                                 GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLubyte pixel;
   LOOKUP_SETUP;
   pixel = (GLubyte) LOOKUP( color[0], color[1], color[2] );

#define INTERP_Z 1
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)		\
	if (Z < *zPtr) {	\
	   *zPtr = Z;		\
	   *pixelPtr = pixel;	\
	}

#include "linetemp.h"
}


/*
 * Draw a flat-shaded, Z-less, PF_HPCR line into an XImage.
 */
static void flat_HPCR_z_line( GLcontext *ctx,
                              GLuint vert0, GLuint vert1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   const GLubyte *color = ctx->VB->ColorPtr->data[pv];
   GLint r = color[0], g = color[1], b = color[2];

#define INTERP_XY 1
#define INTERP_Z 1
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define CLIP_HACK 1
#define PLOT(X,Y)						\
	if (Z < *zPtr) {					\
	   *zPtr = Z;						\
	   *pixelPtr = (GLubyte) DITHER_HPCR( X, Y, r, g, b);	\
	}

#include "linetemp.h"
}


/*
 * Examine ctx->Line attributes and set xmesa->xm_buffer->gc1
 * and xmesa->xm_buffer->gc2 appropriately.
 */
static void setup_x_line_options( GLcontext *ctx )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   int i, state, state0, new_state, len, offs;
   int tbit;
   char *dptr;
   int n_segments = 0;
   char dashes[20];
   int line_width, line_style;

   /*** Line Stipple ***/
   if (ctx->Line.StippleFlag) {
      const int pattern = ctx->Line.StipplePattern;

      dptr = dashes;
      state0 = state = ((pattern & 1) != 0);

      /* Decompose the pattern */
      for (i=1,tbit=2,len=1;i<16;++i,tbit=(tbit<<1))
	{
	  new_state = ((tbit & pattern) != 0);
	  if (state != new_state)
	    {
	      *dptr++ = ctx->Line.StippleFactor * len;
	      len = 1;
	      state = new_state;
	    }
	  else
	    ++len;
	}
      *dptr = ctx->Line.StippleFactor * len;
      n_segments = 1 + (dptr - dashes);

      /* ensure an even no. of segments, or X may toggle on/off for consecutive patterns */
      /* if (n_segments & 1)  dashes [n_segments++] = 0;  value of 0 not allowed in dash list */

      /* Handle case where line style starts OFF */
      if (state0 == 0)
        offs = dashes[0];
      else
        offs = 0;

#if 0
fprintf (stderr, "input pattern: 0x%04x, offset %d, %d segments:", pattern, offs, n_segments);
for (i = 0;  i < n_segments;  i++)
fprintf (stderr, " %d", dashes[i]);
fprintf (stderr, "\n");
#endif

      XMesaSetDashes( xmesa->display, xmesa->xm_buffer->gc1,
		      offs, dashes, n_segments );
      XMesaSetDashes( xmesa->display, xmesa->xm_buffer->gc2,
		      offs, dashes, n_segments );

      line_style = LineOnOffDash;
   }
   else {
      line_style = LineSolid;
   }

   /*** Line Width ***/
   line_width = (int) (ctx->Line.Width+0.5F);
   if (line_width < 2) {
      /* Use fast lines when possible */
      line_width = 0;
   }

   /*** Set GC attributes ***/
   XMesaSetLineAttributes( xmesa->display, xmesa->xm_buffer->gc1,
			   line_width, line_style, CapButt, JoinBevel);
   XMesaSetLineAttributes( xmesa->display, xmesa->xm_buffer->gc2,
			   line_width, line_style, CapButt, JoinBevel);
   XMesaSetFillStyle( xmesa->display, xmesa->xm_buffer->gc1, FillSolid );
   XMesaSetFillStyle( xmesa->display, xmesa->xm_buffer->gc2, FillSolid );
}



/*
 * Analyze context state to see if we can provide a fast line drawing
 * function, like those in lines.c.  Otherwise, return NULL.
 */
line_func xmesa_get_line_func( GLcontext *ctx )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   int depth = GET_VISUAL_DEPTH(xmesa->xm_visual);

   (void) DitherValues;  /* silence unused var warning */
   (void) kernel1;  /* silence unused var warning */

   if (ctx->Line.SmoothFlag)              return NULL;
   if (ctx->Texture.Enabled)              return NULL;
   if (ctx->Light.ShadeModel!=GL_FLAT)    return NULL;
   /* X line stippling doesn't match OpenGL stippling */
   if (ctx->Line.StippleFlag==GL_TRUE)    return NULL;

   if (xmesa->xm_buffer->buffer==XIMAGE
       && ctx->RasterMask==DEPTH_BIT
       && ctx->Depth.Func==GL_LESS
       && ctx->Depth.Mask==GL_TRUE
       && ctx->Line.Width==1.0F) {
      switch (xmesa->pixelformat) {
         case PF_TRUECOLOR:
            return flat_TRUECOLOR_z_line;
         case PF_8A8B8G8R:
            return flat_8A8B8G8R_z_line;
         case PF_8R8G8B:
            return flat_8R8G8B_z_line;
         case PF_8R8G8B24:
            return flat_8R8G8B24_z_line;
         case PF_5R6G5B:
            return flat_5R6G5B_z_line;
         case PF_DITHER_5R6G5B:
            return flat_DITHER_5R6G5B_z_line;
         case PF_DITHER:
            return (depth==8) ? flat_DITHER8_z_line : NULL;
         case PF_LOOKUP:
            return (depth==8) ? flat_LOOKUP8_z_line : NULL;
         case PF_HPCR:
            return flat_HPCR_z_line;
         default:
            return NULL;
      }
   }
   if (xmesa->xm_buffer->buffer==XIMAGE
       && ctx->RasterMask==0
       && ctx->Line.Width==1.0F) {
      switch (xmesa->pixelformat) {
         case PF_TRUECOLOR:
            return flat_TRUECOLOR_line;
         case PF_8A8B8G8R:
            return flat_8A8B8G8R_line;
         case PF_8R8G8B:
            return flat_8R8G8B_line;
         case PF_8R8G8B24:
            return flat_8R8G8B24_line;
         case PF_5R6G5B:
            return flat_5R6G5B_line;
         case PF_DITHER_5R6G5B:
            return flat_DITHER_5R6G5B_line;
         case PF_DITHER:
            return (depth==8) ? flat_DITHER8_line : NULL;
         case PF_LOOKUP:
            return (depth==8) ? flat_LOOKUP8_line : NULL;
         case PF_HPCR:
            return flat_HPCR_line;
	 default:
	    return NULL;
      }
   }
   if (xmesa->xm_buffer->buffer!=XIMAGE && ctx->RasterMask==0) {
      setup_x_line_options( ctx );
      return flat_pixmap_line;
   }
   return NULL;
}
