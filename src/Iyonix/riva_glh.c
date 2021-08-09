#define FAKESNUM 0
 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/

#include <stdlib.h>
//#include <OS.h>

//#include <dlfcn.h> //rudolf: posix (obos cvs)

/*
	BeOS includes.
*/
#include "glh.h"
//#include "nv_globals.h"
//#include "nv_3da.h"
#include "nv_std.h"

/*
 * X includes.
 */
//#include "X.h"
//#include "Xproto.h"
//#include "windowstr.h"
//#include "servermd.h"
/*
 * Mesa includes.
 */
#include "../context.h"
#include "../depth.h"
#include "../macros.h"
#include "../texstate.h"
#include "../triangle.h"
#include "../vb.h"
#include "../types.h"

/*
 * GLX includes.
 */
//#include "glxvisual.h"
//#include "xsmesaP.h"
//#include "glx_config.h"
//#include "hwlog.h"
//#include "glx_log.h"
//#include "glx_symbols.h"
//rudolf: looks like standard MESA headers: (not utahGLX specific???)
#include "../matrix.h"
/*
 * Riva includes.
 */
#include "riva_glh.h"
#include "riva_symbols.h"
#include "riva_tex.h"

/* comment below for disable vt switching */
#define DO_VTSWITCH

/*
 * Current X/Mesa context pointer:
 */
//rudolf: orig
//extern XSMesaContext XSMesa;
//now (previously it was in utahGLX itself, so outside of the driver:
//XSMesaContext XSMesa = NULL;

//rudolf: X specific??
//GLboolean   (*prevSwitchMode)(int scrnIndex, DisplayModePtr mode, int flags);

/*
 * Special value to make buffer pointer checks happy.
 */
//rudolf: we just give back the actual buffer location instead: that makes us much
//happier ;-)
//#define RIVA_MAGIC_NUM  0x69696969

/*
 * Busy loop to synchronise with graphics engine. Don't return until the
 * engine is idle. Implements a timeout to avoid server lockups.
 */
//rudolf: wait_idle..
void RivaSync(int snum)
{
nv_acc_wait_idle();
//    static int cnt;
//    struct _riva_hw_inst *RIVAINST;
//    int i;
//    RIVAINST=riva_sinfo[snum].riva;

//    for (i = 0; i < 0x100000; i++)
//	  {
//        if (RIVAINST->Busy(RIVAINST) == 0)
//		  {
//            cnt = 0;
//            return;
//        }
//   }
//    ErrorF("Riva sync timeout, this has happened %d time(s) now\n", ++cnt);
}
/*
 * Create a new XSMesaContext.
 * Input:  v - XSMesaVisual
 *         share_list - another XSMesaContext with which to share display
 *                      lists or NULL if no sharing is wanted.
 * Return:  an XSMesaContext or NULL if error.
 */
/*
//XSMesaContext RivaCreateContext(XSMesaVisual v, XSMesaContext share_list)
{
//    XSMesaContext c;

//    ErrorF ( "### RivaCreateContext creating new xsmesa context...\n" );
//    if (!(c = (XSMesaContext) calloc( 1, sizeof(struct xsmesa_context))))
        return (NULL);


    if (!(c->gl_ctx = gl_create_context(v->gl_visual,
                                  (share_list ? share_list->gl_ctx : NULL),
                                  ((void *)(c)),
                                  GL_TRUE )))// direct rendering

    {
        free(c);
        return(NULL);
    }

    c->xsm_visual                    = v;
//    ErrorF("   Using visual %d (db=%d, st=%d, acc=%d, depth=%d,al=%d)\n",
//	   v->pVisual->vid,
//	   v->gl_visual->DBflag,
//	   v->gl_visual->StencilBits,
//	   v->gl_visual->AccumBits,
//	   v->gl_visual->DepthBits,
//	   v->gl_visual->AlphaBits);

//    c->xsm_buffer                    = NULL;            // Set later by XSMesaMakeCurrent
    c->pixelformat                   = v->dithered_pf;  // Dithering is enabled by default
    rivaContext.NumCtxs++;
    //rudolf: was already disabled:
//    RivaUpdateState03(c->gl_ctx);
    return(c);
}

//static void RivaDestroyContext(XSMesaContext c)
{
    struct gl_texture_object *tObj;

//    if (c == XSMesa) XSMesa = NULL;
    if (c->gl_ctx)
    {
        // Make sure all this contexts textures are deleted before freeing them up.
        if (c->gl_ctx->Shared)
        {
            for (tObj = c->gl_ctx->Shared->TexObjectList; tObj; tObj = tObj->Next)
                RivaDeleteTexture(c->gl_ctx, tObj);
        }
        gl_destroy_context(c->gl_ctx);
    }
    rivaContext.NumCtxs--;
    free(c);
}
//GLboolean RivaMakeCurrent(XSMesaContext c)
{
//    if (c == XSMesa)
    {
		LOG(2,("RivaMakeCurrent: nothing to be done\n"));
    	return (GL_TRUE);
	}
//    XSMesa = c;
    if (c)
	{
		LOG(2,("RivaMakeCurrent: making context current\n"));
        gl_make_current(c->gl_ctx, c->xsm_buffer ? c->xsm_buffer->gl_buffer : NULL);
	}
    else
	{
		LOG(2,("RivaMakeCurrent: destroying context\n"));
        gl_make_current(NULL, NULL);
	}
    return(GL_TRUE);
}

// Bind buffer b to context c and make c the current rendering context.
//static GLboolean RivaBindBuffer(XSMesaContext c, XSMesaBuffer  b)
{
    if ((!c && b) || (c && !b))
    {
        return (GL_FALSE);
    }
    else if (c)
    {

//        if ((c             == XSMesa)
         && (c->xsm_buffer == b)
         && (c->xsm_buffer->wasCurrent))
            // Same context and buffer, do nothing.
            return(GL_TRUE);
        c->xsm_buffer = b;
        // Get XSMesa hooks for buffer access.
//xsrud
//        xsmesa_setup_DD_pointers(c->gl_ctx);
        if (!b->pixmap_flag)
        {
            // Only hook accelerated HW for non-pixmap buffers.
            RivaUpdateState05(c->gl_ctx);
        }
        // Make current and bind Mesa context and buffer.
        gl_make_current(c->gl_ctx, c->xsm_buffer->gl_buffer);
//        XSMesa = c;
        if (c->gl_ctx->Viewport.Width==0)
        {
            // Initialize viewport to window size.
            gl_Viewport( c->gl_ctx, 0, 0, b->width, b->height );
            c->gl_ctx->Scissor.Width  = b->width;
            c->gl_ctx->Scissor.Height = b->height;
        }
        if (c->xsm_visual->gl_visual->RGBAflag)
        {
            // Must recompute and set these pixel values because colormap
            // can be different for different windows.
//rudolf:
//typedef unsigned long XID;
            unsigned long value;
//xsrud
//            c->pixel = xsmesa_color_to_pixel( c, c->red, c->green, c->blue, c->alpha );
            value = (unsigned long)c->pixel;
//rudolf: x11 functie:
//            DoChangeGC(c->xsm_buffer->gc1, GCForeground, &value, 0);
//xsrud
//            c->clearpixel = xsmesa_color_to_pixel(c,
  //                                                c->clearcolor[0],
    //                                              c->clearcolor[1],
      //                                            c->clearcolor[2],
        //                                          c->clearcolor[3]);
            value = (unsigned long)c->clearpixel;
//rudolf: x11 functie:
//            DoChangeGC(c->xsm_buffer->cleargc, GCForeground, &value, 0);
        }
        // Solution to Stephane Rehel's problem with glXReleaseBuffersMESA().
        c->xsm_buffer->wasCurrent = GL_TRUE;
    }
    else
    {
//        XSMesa        = NULL;
        gl_make_current(NULL, NULL);
    }
    return(GL_TRUE);
}
*/
/*
 * Return a pointer to the XSMesa backbuffer Pixmap or XImage.  This function
 * is a way to get "under the hood" of X/Mesa so one can manipulate the
 * back buffer directly.
 * Output:  pixmap - pointer to back buffer's Pixmap, or 0
 *          ximage - pointer to back buffer's XImage, or NULL
 * Return:  GL_TRUE = context is double buffered
 *          GL_FALSE = context is single buffered
 */
//Rudolf: GLXProcs function: rewrite for BeOS
/*
static GLboolean RivaGetBackBuffer(
//    XSMesaBuffer b,
    PixmapPtr   *pixmap,
    XImage     **ximage
)
{
    if (b->db_state)
    {
        if (pixmap)  *pixmap = b->backpixmap;
        if (ximage)  *ximage = (XImage *)b->backimage;
        return(GL_TRUE);
    }
    else
    {
        *pixmap = 0;
        *ximage = NULL;
        return(GL_FALSE);
    }
}
*/
/*
 * Return the depth buffer associated with an XSMesaBuffer.
 * Input:  b - the XSMesa buffer handle
 * Output:  width, height - size of buffer in pixels
 *          bytesPerValue - bytes per depth value (2 or 4)
 *          buffer - pointer to depth buffer values
 * Return:  GL_TRUE or GL_FALSE to indicate success or failure.
 */
/*
static GLboolean RivaGetDepthBuffer(
//    XSMesaBuffer b,
    GLint       *width,
    GLint       *height,
    GLint       *bytesPerValue,
    void       **buffer
)
{
    if ((!b->gl_buffer) || (!b->gl_buffer->Depth))
    {
        *width         = 0;
        *height        = 0;
        *bytesPerValue = 0;
        *buffer        = 0;
        return(GL_FALSE);
    }
    else
    {
        *width         = b->gl_buffer->Width;
        *height        = b->gl_buffer->Height;
        *bytesPerValue = sizeof(GLdepth);
        *buffer        = b->gl_buffer->Depth;
        return(GL_TRUE);
    }
}
*/
/*
 * Deallocate an XSMesaBuffer structure and all related info.
 */
/*
//static void RivaDestroyBuffer(XSMesaBuffer b)
{
    // Do any special buffer management before calling the default routine.
//    if (b->gl_buffer->Depth == (GLdepth *)RIVA_MAGIC_NUM)
//rudolf: fixed
    if (b->gl_buffer->Depth == (GLdepth *)(riva_sinfo[FAKESNUM].depthbufmappedaddr))
        b->gl_buffer->Depth = NULL;
//xsrud
//    XSMesaDestroyBuffer(b);
}
*/
/*
 * XImage overrides to read and write offscreen image and
 * depth buffers.
 */
//rudolf: GLXProcs function, rewrite for BeOS if needed
/*
static unsigned long RivaGetPixelNOP(
    GLXImage *image,
    int       x,
    int       y
)

{
    return (0);
}
*/
//rudolf: GLXProcs function, rewrite for BeOS if needed
/*
static void RivaPutPixelNOP(
    GLXImage     *image,
    int           x,
    int           y,
    unsigned long pixel
)
{
}
*/

//rudolf: GLXProcs function: rewrite for BeOS
/*
static unsigned long RivaGetPixel16
(
    GLXImage *image,
    int       x,
    int       y
)
{
    CARD16 *ptr2;
    char   *rowaddr;

    x += image->pwin->drawable.x;
    y += image->pwin->drawable.y;
    if ((x >= image->pwin->clipList.extents.x1 || x < image->pwin->clipList.extents.x2)
     || (y >= image->pwin->clipList.extents.y1 || y < image->pwin->clipList.extents.y2))
    {
        rowaddr = riva_sinfo[FAKESNUM].backbufmappedaddr
                + image->bytes_per_line * y;
         // Sync on HW if 3D rendering currently in progress.
        if (!riva_sinfo[FAKESNUM].rivaReload3D || rivaSyncPix)
        {
            riva_sinfo[FAKESNUM].rivaReload3D = 1;
            rivaSyncPix  = 0;
            RivaSync(FAKESNUM);
        }
        ptr2    = (CARD16*)rowaddr;
        return (ptr2[x]);
    }
    return (0);
}
*/

//rudolf: GLXProcs function: rewrite for BeOS
/*
static unsigned long RivaGetPixel32
(
    GLXImage *image,
    int       x,
    int       y
)
{
    uint32 *ptr2;
    char   *rowaddr;

    x += image->pwin->drawable.x;
    y += image->pwin->drawable.y;
    if ((x >= image->pwin->clipList.extents.x1 || x < image->pwin->clipList.extents.x2)
     || (y >= image->pwin->clipList.extents.y1 || y < image->pwin->clipList.extents.y2))
    {
        rowaddr = riva_sinfo[FAKESNUM].backbufmappedaddr
                + image->bytes_per_line * y;
         // Sync on HW if 3D rendering currently in progress.
        if (!riva_sinfo[FAKESNUM].rivaReload3D || rivaSyncPix)
        {
            riva_sinfo[FAKESNUM].rivaReload3D = 1;
            rivaSyncPix  = 0;
            RivaSync(FAKESNUM);
        }
        ptr2    = (uint32 *)rowaddr;
        return (ptr2[x]);
    }
    return (0);
}
*/

//#ifdef NOTUSED
/* See RivaPutPixel now */

/* 16bit equivalent of RivaPutPixel32.
 * same thing, except we reduce the color info down.
 * Then we write the value directly to framebuffer memory
 */
/*
static void RivaPutPixel16
(
    GLXImage     *image,
    int           x,
    int           y,
    unsigned long pixel
)
{
    CARD16 *ptr2;
    char   *rowaddr;

	fprintf(stderr,"RivaPutPixel16: image=%p(w=%d,h=%d),x=%d,y=%d,pixcol=%ld\n",
		image,image->width,image->height,
		x,y,pixel);

    // convert x and y coords to screen-absolute coords first
    x += image->pwin->drawable.x;
    y += image->pwin->drawable.y;

    // Then, if another X11 window is not covering us... blast pixel out!
    if ((x >= image->pwin->clipList.extents.x1 || x < image->pwin->clipList.extents.x2)
     || (y >= image->pwin->clipList.extents.y1 || y < image->pwin->clipList.extents.y2))
    {
        rowaddr = riva_sinfo[FAKESNUM].backbufmappedaddr
                + image->bytes_per_line * y;
        ptr2    = (CARD16*)rowaddr;
        // Sync on HW if 3D rendering currently in progress.
        if (!riva_sinfo[FAKESNUM].rivaReload3D || rivaSyncPix)
        {
            riva_sinfo[FAKESNUM].rivaReload3D = 1;
            rivaSyncPix  = 0;
            RivaSync(FAKESNUM);
        }
        ptr2[x] = pixel;
    }
}
static void RivaPutPixel32
(
    GLXImage     *image,
    int           x,
    int           y,
    unsigned long pixel
)
{
    uint32 *ptr2;
    char   *rowaddr;

    x += image->pwin->drawable.x;
    y += image->pwin->drawable.y;
    if ((x >= image->pwin->clipList.extents.x1 || x < image->pwin->clipList.extents.x2)
     || (y >= image->pwin->clipList.extents.y1 || y < image->pwin->clipList.extents.y2))
    {
        rowaddr = riva_sinfo[FAKESNUM].backbufmappedaddr
                + image->bytes_per_line * y;
        ptr2    = (uint32 *)rowaddr;
        // Sync on HW if 3D rendering currently in progress.
        if (!riva_sinfo[FAKESNUM].rivaReload3D || rivaSyncPix)
        {
            riva_sinfo[FAKESNUM].rivaReload3D = 1;
            rivaSyncPix  = 0;
            RivaSync(FAKESNUM);
        }
        ptr2[x] = pixel;
    }
}
#endif // NOTUSED
*/

/* Merged version of  RivaPutPixel16/32.
 * We can only have 16bit or 32bit "putpixel" routines.
 *
 * We do the same thing either way:
 * write directly to video memory.
 * The only difference being how many bytes of color we write at a time.
 *
 * We have a merged routine now, to theoretically allow
 * different contexts to have different bit depths.
 *
 * Note that we write TO THE BACK BUFFER ONLY, right now.
 *
 * Note that we only seem to get called via GLXProcs.PutPixel
 *  (or by RivaBitmap, so far)
 * which only gets called in xsmesa2.c:write_span_XXXXX_ximage() routines,
 * Each of which always pass in xsmesa_buffer->BACKBUFFER as the "image".
 *
 */
//rudolf: modify for BeOS function instead (orig GLXProcs)
/*
void RivaPutPixel
(
    GLXImage     *image,
    int           x,
    int           y,
    unsigned long pixel
)
{
	CARD16 *ptr16;
	uint32 *ptr32;
	char   *rowaddr;

	// convert x and y coords to screen-absolute coords first
	x += image->pwin->drawable.x;
	y += image->pwin->drawable.y;

	// Ignore if pixel is covered by another window
	if ((x < image->pwin->clipList.extents.x1 ||
	       x >= image->pwin->clipList.extents.x2) ||
	    (y < image->pwin->clipList.extents.y1 ||
	       y >= image->pwin->clipList.extents.y2))
	{
		return;
	}
        rowaddr = riva_sinfo[FAKESNUM].backbufmappedaddr
                + image->bytes_per_line * y;
         // Sync on HW if 3D rendering currently in progress.
        if (!riva_sinfo[FAKESNUM].rivaReload3D || rivaSyncPix)
        {
            riva_sinfo[FAKESNUM].rivaReload3D = 1;
            rivaSyncPix  = 0;
            RivaSync(FAKESNUM);
        }

	// We kinda assume we are on an LSB chip, writing to
	// a LSB graphics card. Which, given our circumstances,
	// should be a reasonable assumption to make? !
	switch(image->bits_per_pixel){
		case 15:
		case 16:
			ptr16    = (CARD16*)rowaddr;
			ptr16[x] = pixel;
			break;

		// allegedly, we get passed 24bit even when we are
		// really in 32bit mode
		//
		case 24:
		case 32:
			ptr32    = (uint32 *)rowaddr;
			ptr32[x] = pixel;
			break;
	}
}
*/

/*
 * This routine should probably be called
 * CreateBackBuffer, not CreateImage.
 * It seems to only be called by
 *   xsmesa_alloc_back_buffer
 *
 * I THINK it is used primarily to create
 * info for the 'back buffer' memory for a particular
 * client, using a particular visual.
 *
 * It is not clear what the "old_image" is for, other than
 * "you must free it"
 *
 * If we were doing software only, image->data would point to
 *  a malloced backbuffer.
 * Not sure what we should make of it for hardware.
 *
 * Thing is... this "GLXImage" never gets used except by US, either directly
 * or indirectly. So we can theoretically set it to anything we like.
 * Thats why we can get away with setting  data=0;
 * But for the record, we'll set devPriv to be the top of framebuffer
 * memory that we should use.
 * Either the start of the direct video memory, or
 * the start of the duplicate memory we use as backbuffer.
 * (Which is the entire size of front video ram)
 * Except that we dont know whether to set front or back here.
 * We cant see the Mesa context!
 * So it may be set somewhere else, like RivaBitmap().
 * If it is not set, programs should assume to use backbuffer memory.
 *
 */
//rudolf: seems not used at all!
/*
static GLXImage * RivaCreateImage
(
    WindowPtr pwindow,
    GLvisual *visual,
    int       width,
    int       height,
    GLXImage *old_image
)
{
    GLXImage *image;
    int depth;
    ScrnInfoPtr scrninfoP;
    int snum;

    fprintf(stderr,"DEBUG: RivaCreateImage: DBflag=%d\n",visual->DBflag);
    fprintf(stderr,
        " accumbits=%d, dptbits=%d, stenbits=%d,softalpha=%d,alphabits=%d\n",
	visual->AccumBits,visual->DepthBits,visual->StencilBits,
	visual->SoftwareAlpha,visual->AlphaBits);

    // would be really nice to find a cleaner way of getting screen num
    snum=pwindow->drawable.pScreen->myNum;
    scrninfoP=glxsym.scrninfoList[snum];

    if (!pwindow) return (GLXCreateImage(pwindow, visual, width, height, old_image));
    if (old_image) GLXProcs.DestroyImage(old_image);

    image                 = (GLXImage *)xalloc(sizeof(GLXImage));
    image->pwin           = pwindow;
    image->width          = width;
    image->height         = height;
    depth = visual->RedBits + visual->GreenBits + visual->BlueBits;

    // the visual lies to us...
//was already off:    image->bits_per_pixel = depth;
    image->bits_per_pixel =scrninfoP->bitsPerPixel;

//was already off:    image->data           = riva_sinfo[snum].backbufmappedaddr;
    image->data           = 0;

    GLXProcs.PutPixel=RivaPutPixel;

    // XXX bad bad bad.. this should be context specific,
    // NOT GLOBAL!!
    switch (depth)
    {
        case 15:
        case 16:
            if (riva_sinfo[snum].rivaGLXEnabled)
            {
                GLXProcs.GetPixel = RivaGetPixel16;
            }
            else
            {
                GLXProcs.GetPixel = RivaGetPixelNOP;
                GLXProcs.PutPixel = RivaPutPixelNOP;
            }
            break;
        case 24: // in 32bpp we get 24 passed in as the depth!
        case 32:
            if (riva_sinfo[snum].rivaGLXEnabled)
            {
                GLXProcs.GetPixel = RivaGetPixel32;
            }
            else
            {
                GLXProcs.GetPixel = RivaGetPixelNOP;
                GLXProcs.PutPixel = RivaPutPixelNOP;
            }
            break;
        default:
            ErrorF("Bad depth (%d) in RivaCreateImage\n", depth);
            xfree(image);
            return (NULL);
            break;
    }
    image->bytes_per_line = riva_sinfo[snum].riva->CurrentState->pitch1;
    return (image);
}
*/

/*
static GLdepth RivaGetDepthNOP
(
//    XSMesaContext xsmesa,
    int           x,
    int           y
)
{
    return (0);
}
static void RivaPutDepthNOP
(
//    XSMesaContext xsmesa,
    int           x,
    int           y,
    GLdepth       depth
)
{
}
*/

//rudolf: GLXProcs function: patched in
GLdepth RivaGetDepth(GLcontext* ctx, int x, int y)
{
//    vuint16 *ptr2;
//    uint16* rowaddr;
//rudtst:
GLdepth *rowaddr;
//    WindowPtr pwin = (WindowPtr)(xsmesa->xsm_buffer->frontbuffer);

//	LOG(2,("RivaGetDepth: x %d, y%d\n",x,y ));

//rudolf: pwin->drawable.x and y description: /* window: screen absolute, pixmap: 0 */
//rudolf: only frontbuffer != 0???
//test: (assuming 0)
//    x += pwin->drawable.x;
//    y += pwin->drawable.y;
//x-=200;
//y-=150;

//rudolf: cliplist doc:  clipping rectangle for output
//(so only for frontbuffer rendering?????????????!!)
//rudolf: re-enable:
//    if ((x >= pwin->clipList.extents.x1 || x < pwin->clipList.extents.x2)
//     || (y >= pwin->clipList.extents.y1 || y < pwin->clipList.extents.y2))
//    if ((x >= 0 || x < 800)
//     || (y >= 0 || y < 600))
    {
/*
        rowaddr = (vuint8 *)riva_sinfo[FAKESNUM].depthbufmappedaddr
//rudolf:riva->CurrentState->pitch3 == bytes_per_row 2D mode!
//                + ((riva_sinfo[FAKESNUM].riva->CurrentState->pitch3) * y);
			+ si->fbc.bytes_per_row * y;
        ptr2    = (vuint16 *)rowaddr;//rudolf: depth buffer is fixed 16bit AFAIK
        return ((GLdepth)(ptr2[x]));
*/
        rowaddr = ((GLdepth *)(riva_sinfo[FAKESNUM].depthbufmappedaddr))
//rudolf:riva->CurrentState->pitch3 == bytes_per_row 2D mode (but fullscreen only!?)
//                + ((riva_sinfo[FAKESNUM].riva->CurrentState->pitch3) * y);
//rudolf: gl_context/ GLcontext is defined in types.h, line 1859<<<<<<<<<
//rudolf: Buffer is a GLframebuffer/gl_frame_buffer defined at line 1452<<<<<<<<
//rudolf: xsmesa->gl_ctx->Buffer->Width is the output window width!! (frontbuffer)
			+ (ctx->Buffer->Width * y + x);

        return ((GLdepth)(*rowaddr));
//        return ((GLdepth)65535);
    }
    return (0);
}

//rudolf: GLXProcs function: patched in
void RivaPutDepth(GLcontext* ctx, int x, int y, GLdepth depth)
{
//	vuint16 *ptr2;
//    vuint8 *rowaddr;
//rud: temp?
GLdepth *rowaddr;

//    WindowPtr pwin = (WindowPtr)(xsmesa->xsm_buffer->frontbuffer);

//	LOG(2,("RivaPutDepth: x %d, y%d\n",x,y ));

//rudolf: pwin->drawable.x and y description: /* window: screen absolute, pixmap: 0 */
//test: (assuming 0)
//    x += pwin->drawable.x;
//    y += pwin->drawable.y;

//x-=200;
//y-=150;
//rudolf: re-enable:
//    if ((x >= pwin->clipList.extents.x1 || x < pwin->clipList.extents.x2)
//     || (y >= pwin->clipList.extents.y1 || y < pwin->clipList.extents.y2))
    {
//        rowaddr = (vuint8 *)riva_sinfo[FAKESNUM].depthbufmappedaddr
//			+ si->fbc.bytes_per_row * y;
  //      ptr2    = (vuint16 *)rowaddr;
    //    ptr2[x] = depth;
        rowaddr = (GLdepth *)(riva_sinfo[FAKESNUM].depthbufmappedaddr)
//rudolf: xsmesa->gl_ctx->Buffer->Width is the output window width!! (frontbuffer)
			+ (ctx->Buffer->Width * y + x);

        *rowaddr = depth;
    }
//304 voor teapot!
//	LOG(2,("RivaPutDepth: width = %d\n", (xsmesa->gl_ctx->Buffer->Width)));

}

/*
 * Allocate a new depth buffer.  If there's already a depth buffer allocated
 * it will be free()'d.  The new depth buffer will be uniniitalized.
 * This function is only called through Driver.alloc_depth_buffer.
 */
void RivaCreateDepthBuffer(GLcontext *ctx)
{
/*
//from MESA:
   // deallocate current depth buffer if present
   if (ctx->Buffer->Depth) {
      FREE(ctx->Buffer->Depth);
      ctx->Buffer->Depth = NULL;
   }

   // allocate new depth buffer, but don't initialize it
   ctx->Buffer->Depth = (GLdepth *) MALLOC( ctx->Buffer->Width
                                            * ctx->Buffer->Height
                                            * sizeof(GLdepth) );

   if (!ctx->Buffer->Depth) {
      // out of memory
      ctx->Depth.Test = GL_FALSE;
      ctx->NewState |= NEW_RASTER_OPS;
      gl_error( ctx, GL_OUT_OF_MEMORY, "Couldn't allocate depth buffer" );
   }
*/

//rudolf: disabled org:
    /*
     * Use a magic # so other code doesn't think its NULL.
     */
//    ctx->Buffer->Depth = (GLdepth *)RIVA_MAGIC_NUM;

//rudolf: BeOS fake for now: (just give back the location of our existing buffer)
	ctx->Buffer->Depth = (GLdepth *)(riva_sinfo[FAKESNUM].depthbufmappedaddr);

	LOG(2,("RivaCreateDepthBuffer: adress is $%08x\n", ctx->Buffer->Depth));
}

//rudolf: linux only (we have no legacy textmode :-)
/*
#ifdef DO_VTSWITCH
 // vt switch hooks.
 // used to override pScrn->EnterVT, pScrn->LeaveVT with ours, and call
 // the generic ones saved as prevEnterVT(), prevLeaveVT().
static GLboolean RivaGLXEnterVT(int scrnindex, int flags)
{
    GLboolean retval;

    fprintf (stderr, "nvglx: EnterVT\n");
    // is retval needs checking?
    retval = prevEnterVT(scrnindex, flags);
    fprintf (stderr, "nvglx: prevEnterVT returned %d\n", retval);
    nvInitGLX(FAKESNUM);
    fprintf (stderr, "nvglx: EnterVT done!\n");
    return retval;
}

// this will be called when entering text mode
static void RivaGLXLeaveVT(int scrnindex, int flags)
{
     // Disable all texture code and rendering code.
    fprintf(stderr, "nvglx: LeaveVT\n");
    riva_sinfo[FAKESNUM].rivaGLXEnabled    = FALSE;
//rudolf: was already disabled:
//    GLXProcs.GetDepth = RivaGetDepthNOP;
//    GLXProcs.PutDepth = RivaPutDepthNOP;
//    GLXProcs.GetPixel = RivaGetPixelNOP;
//    GLXProcs.PutPixel = RivaPutPixelNOP;
    RivaReleaseTextures();
    riva_sinfo[FAKESNUM].rivaReload3D = rivaSyncPix = 0;

    prevLeaveVT(scrnindex, flags);
    fprintf (stderr, "nvglx: LeaveVT done!\n");
    return;
}
#endif // DO_VTSWITCH
*/

/* video mode switching hook, without this texture-buffer corruptions occur */
//rudolf: probably linux only as well.. (2D drv inits 3D engine after a modeswitch?!?)
/*
static GLboolean RivaGLXSwitchMode(int scrnIndex, DisplayModePtr mode, int flags)
{
    GLboolean retval;

    fprintf(stderr, "nvglx: entering video mode switching\n");
    // disable everything
    riva_sinfo[FAKESNUM].rivaGLXEnabled    = FALSE;
    GLXProcs.GetDepth = RivaGetDepthNOP;
    GLXProcs.PutDepth = RivaPutDepthNOP;
    GLXProcs.GetPixel = RivaGetPixelNOP;
    GLXProcs.PutPixel = RivaPutPixelNOP;
    RivaReleaseTextures();
    riva_sinfo[FAKESNUM].rivaReload3D = rivaSyncPix = 0;

    // do real mode switch
    retval = prevSwitchMode(scrnIndex, mode, flags);

    // enable again
    nvInitGLX(FAKESNUM);
    fprintf(stderr, "nvglx: video mode switching done!\n");
    return retval;
}
*/

/*
 * Test configuration for 3D accleration.
 */
static  GLboolean RivaAccelerate3D()
{
    /*
     * Assume not enabled.
     */
    riva_sinfo[FAKESNUM].rivaGLXEnabled = FALSE;
    /*
     * If Riva version mismatch or not installed bail out.
     * rudolf: actually: check 2D driverversion here..
     */
//rudolf: disabling for now
//    if (riva_sinfo[FAKESNUM].riva->Version != RIVA_SW_VERSION)
//        return(GL_FALSE);
    /*
     * If wrong bit depth bail out. (check current mode from 2D driver)
     */
//rudolf: disabling for now
//    if (riva_sinfo[FAKESNUM].riva->CurrentState->bpp != 15 && riva_sinfo[FAKESNUM].riva->CurrentState->bpp != 16 && riva_sinfo[FAKESNUM].riva->CurrentState->bpp != 32)
//        return(GL_FALSE);

    /*
     * If no available buffers bail out.
     */
	/*rudolf: return (GL_FALSE) here for software rendering testing!!!!!!!!!!!!!!!!!! */
    if (!riva_sinfo[FAKESNUM].backbufferoffset && !riva_sinfo[FAKESNUM].depthbufferoffset && !riva_sinfo[FAKESNUM].texturebase[0])
        return(GL_FALSE);
    /*
     * Looks OK.
     */
    riva_sinfo[FAKESNUM].rivaGLXEnabled = TRUE;
    return (GL_TRUE);
}


/*
 * Create visuals for RIVA.  Hacked from the XSMesaCreateVisuals.
 * Need to fill out __glScreens[snum].visuals[] with Mesa visuals.
 * Make sure pVisual points to "normal" X11 Visual.
 * GLFinalizeVisuals will do the rest.
 *
 * Things needed to consider when deciding which visual types add in/set:
 *
 * 1. Is the feature feasible to do in hardware, for the given
 *    visual type and bitdepth?
 *  (eg: alpha blending doesnt happen in "indexed" visuals - only RGB ones)
 *   (right?)
 *
 * 2. If it is going to be in hardware, how do we make sure the mesa
 *    stuff doesnt try to also do it in software?
 *
 * 3. contrariwise, if NOT in hardware, how do we make sure mesa DOES
 *    do it in software?
 *
 * Some of this should be answered by XSMesaCreateVisual()
 *
 * Hack city:
 *  number GLX visuals starting at 100, with screen#1 starting at 200.
 *  except I think this is ignore anyways.
 */

//rudolf: beos version:
/*
GLboolean nvInitVisuals
(
    uint32		*visualp,
    DepthPtr     *depthp,
    int          *nvisualp,
    int          *ndepthp,
    int          *rootDepthp,
    unsigned int *defaultVisp,
    unsigned long sizes,
    int           bitsPerRGB
)
{
    int       i;
    int       nvisual       = *nvisualp;
    //VisualPtr visual        = *visualp;
    //DepthPtr  depths        = *depthp;
    //int       ndepths       = *ndepthp;
    int screennum=FAKESNUM;
    GLScreen* glscreen=&__glScreens[screennum];
//rudolf: pointer to screen:
    ScreenPtr pScreen=screenInfo.screens[screennum];
    int ximage_flag=1;  // use 'ximage' format, not 'pixmap' format.
						// which is to say, "write directly to video ram"
    int use_doublebuf=0, use_depth=0;
//    XSMesaVisual *xmvisuals;
    int cur_visual=0;
    int vmultiplier=0;

    fprintf(stderr,"DEBUG: entered nvInitVisuals\n");

     // Call default visual creation if no HW acceleration.
    if (!RivaAccelerate3D())
//        return (XSMesaInitVisuals(visualp, depthp, nvisualp, ndepthp, rootDepthp, defaultVisp, sizes, bitsPerRGB));

    if(riva_sinfo[screennum].backbufferoffset!=0) use_doublebuf=1;
    if(riva_sinfo[screennum].depthbufferoffset!=0) use_depth=1;
    if(use_doublebuf){
	ErrorF("nvInitVisuals: doublebuffer is enabled\n");
    }
    if(use_depth){
	ErrorF("nvInitVisuals: depth buffer is enabled\n");
    }

    // multiplier depends on how many feature twiddles you add.
    // x2 if using doublebuffering, etc.
    vmultiplier=(use_doublebuf+1)  * (use_depth+1)
                *2 // stencil
		*2 ; // accum


//    xmvisuals=(XSMesaVisual *)xalloc(sizeof(XSMesaVisual) *vmultiplier * nvisual);
    if(xmvisuals==NULL){
	ErrorF("nvInitVisuals: alloc of visuals failed\n");
	return GL_FALSE;
    }
    fprintf(stderr,"nvInitVisuals: preallocated space for %d visuals\n",
             vmultiplier * nvisual);
    glscreen->visuals=xmvisuals;


     // Fill out list of GLX visuals, layered around X11 visual.
     //  Only RGB visual types supported.
    for (i=0;i<nvisual;i++)
    {
        int db, softalpha, stencil, depth, accum;
        VisualPtr pVisual=(*visualp)+i;
        if (( (pVisual->CLASS) != TrueColor) &&
	    ( (pVisual->CLASS) != DirectColor))
        {
	  continue;
	}

	// The strategy for "start from 0" or "start from 1", is:
	// give the type we like, first.
	// So if we have hardware accel, give feature enabled first.
	for(db=use_doublebuf; db>=0; db--)
	//db=use_doublebuf;
        {
	    for (softalpha=0;softalpha<=1;softalpha++)

	    // I dont think we support hardware alpha at the pixel level.
	    // But at any rate, this doesnt seem to make any difference
	    // being 0 or 1 for the riva/nv driver. Wierd.
	    // Dunno for other drivers.

            //softalpha = 0;
            {
	        // Does stencil=1 turn off hardware acceleration?
			// Dunno, but it sure seems to CRASH the nv stuff

                for(stencil=0; stencil<=0; stencil++)
                {
		    for(depth=use_depth; depth>=0; depth--)
		    //depth=0;
                    {
		        // Don't have accum. but allow for fake in software
                        for(accum=0;accum<=1;accum++)
                        {

			glscreen->visuals[cur_visual++]=
			GLXProcs.CreateVisual(	pScreen,
				pVisual,
				GL_TRUE, // rgb
				softalpha,
				db,
				ximage_flag,
				depth*DEPTH_BITS,
				stencil*STENCIL_BITS,
				accum*ACCUM_BITS,
				0 // overlay/underlay
				);

                        }
                    }
                }
            }
	}
    }
    fprintf(stderr,"nvInitVisuals returning okay: %d visuals made\n",
	    cur_visual);

    glscreen->numVisuals=cur_visual;

    return (GL_TRUE);
}
*/

/*
 * Update GLX software-only hooks, with hardware-accelerated ones.
 * Plus minor bits of housekeeping, like setting GLXProcs hooks,
 * setting initial context info, and calling RivaInitTextureHeap
 * Called after nvHookServerSymbols()
 */
GLboolean nvInitGLX(int snum)
{
    int i;
//rudolf: disabled for now, is 2D driver private info (shared_info for Haiku)
//    ScrnInfoPtr pScrn = xf86Screens[snum];
    char *logfile;

    if (!RivaAccelerate3D())
        return (GL_FALSE);
    /*
     * Cool, there is enough here to accelerate.
     */
//    ErrorF("\nInitializing RIVA GLX on top of nv driver Version = %d.%d\n",
//	   riva_sinfo[snum].riva->Version >> 16,
//	   riva_sinfo[snum].riva->Version & 0xFFFF);

    /* initialize hardware-specific logfile */
//rudolf: not interesting currently: we log on same system we render.
//    if(__glx_is_server){
//	logfile=glx_getvar_secure("hw_logfile");
//	fprintf(stderr,"nv: value of hw_logfile is %s\n",logfile);
//    } else {
//	logfile=glx_getvar_secure("hw_direct_logfile");
//	fprintf(stderr,"nv: value of hw_direct_logfile is %s\n",logfile);
//    }
//    hwOpenLog(logfile, "[nv] ");
//    if (glx_getvar("hw_loglevel")) {
//	hwSetLogLevel(glx_getint("hw_loglevel"));
  //  } else {
	//hwSetLogLevel(DBG_LEVEL_BASE);
//    }

    /*
     * Do some initializtion.
     */
    rivaContext.bufRender    = 0;
    rivaContext.texObj       = 0;
    rivaContext.texOffset    = 0;
    rivaContext.triContext.tri05.texColorKey = 0;
    rivaContext.triContext.tri05.texFormat   = 0;
    rivaContext.triContext.tri05.texFilter   = 0;
    rivaContext.triContext.tri05.triBlend    = 0;
    rivaContext.triContext.tri05.triControl  = 0;
    rivaContext.triContext.tri05.triFogColor = 0;
    rivaContext.sAdjust      = 0.0F;
    rivaContext.tAdjust      = 0.0F;
    for (i = 0; i < 16; i++)
        rivaContext.VCache[i] = 0xFFFF;
    rivaContext.NumCtxs      = 0;
    rivaContext.xyAdjust = 0.5F;

//rudolf: test:
/*
	int rudstat = RivaInitTextureHeap(snum);
	LOG(2,("nvInitGLX: RivaInitTextureHeap "));
	if (rudstat == TRUE)
		LOG(2,(" successfully setup\n"));
	else
		LOG(2,(" not setup!\n"));

	riva_sinfo[snum].rivaGLXEnabled = rudstat;
	if (riva_sinfo[snum].rivaGLXEnabled)
*/
   if ((riva_sinfo[snum].rivaGLXEnabled = RivaInitTextureHeap(snum)))
   {

//	fprintf (stderr, "nvglx: hooking into nv pScrn funcs\n");
//rudolf: we don't do this: 2D driver's responsibility for now.
/*
	if ((pScrn->SwitchMode != NULL) && (pScrn->SwitchMode != RivaGLXSwitchMode))
	{
	    prevSwitchMode = pScrn->SwitchMode;
	    pScrn->SwitchMode = RivaGLXSwitchMode;
	}
#ifdef DO_VTSWITCH
        if ((pScrn->EnterVT != NULL) && (pScrn->EnterVT != RivaGLXEnterVT))
        {
            prevEnterVT = pScrn->EnterVT;
            pScrn->EnterVT = RivaGLXEnterVT;
        }
        if ((pScrn->LeaveVT != NULL) && (pScrn->LeaveVT != RivaGLXLeaveVT))
        {
            prevLeaveVT = pScrn->LeaveVT;
            pScrn->LeaveVT = RivaGLXLeaveVT;
        }
#else
	if (EnableVTSwitch == NULL)
	{
	    EnableVTSwitch = dlsym(handle,"xf86EnableVTSwitch");
	    if (EnableVTSwitch) {
		ErrorF("nvglx: disabling VT switching\n");
		(*EnableVTSwitch)(FALSE);
	    } else {
		if ((errstr = dlerror()) != NULL)
		    ErrorF("%s\n", errstr);
		else
		    ErrorF("nvglx: couldn't disable VT switching\n");
	    }
	}
#endif
*/
        /*
         * Hook the GLX procs needed to support Riva acceleration.
         */
/*
        GLXProcs.CreateContext      = RivaCreateContext;
        GLXProcs.DestroyContext     = RivaDestroyContext;
        GLXProcs.MakeCurrent        = RivaMakeCurrent;
        GLXProcs.SwapBuffers        = RivaSwapBuffers;
        GLXProcs.BindBuffer         = RivaBindBuffer;
        GLXProcs.GetBackBuffer      = RivaGetBackBuffer;
        GLXProcs.GetDepthBuffer     = RivaGetDepthBuffer;
        GLXProcs.DestroyBuffer      = RivaDestroyBuffer;
        GLXProcs.CreateImage        = RivaCreateImage;
        GLXProcs.GetDepth           = RivaGetDepth;
        GLXProcs.PutDepth           = RivaPutDepth;
        GLXProcs.CreateDepthBuffer  = RivaCreateDepthBuffer;
*/
        /*
         * Set 3D state reload flag.
         */
        riva_sinfo[snum].rivaReload3D = rivaSyncPix = 1;
    }

//    ErrorF("nvInitGLX() completed\n");

    return (riva_sinfo[snum].rivaGLXEnabled);
}
