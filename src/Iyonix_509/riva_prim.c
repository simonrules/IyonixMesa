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

/*
 * X includes.
 */
//#include "X.h"
//#include "Xproto.h"
//#include "windowstr.h"
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
#include "../image.h"

/*
	BeOS includes.
*/
#include "glh.h"

/*
 * GLX includes.
 */
//#include "xsmesaP.h"
//#include "glx_log.h"
//#include "glx_symbols.h"
/*
 * Riva includes.
 */
//#include "compiler.h" /* for mem_barrier */
#include "riva_glh.h"
#include "riva_symbols.h"
//#include "nv_globals.h"
//#include "nv_3da.h"
#include "nv_std.h"

/*
 * Get the color of a vertex. SLW: R/B swap
 */
#define RivaColor(vv)                                                               \
 (((GLuint)VB->ColorPtr->data[vv][2]) << 16)                                                 \
|(((GLuint)VB->ColorPtr->data[vv][1]) << 8)                                                  \
|(((GLuint)VB->ColorPtr->data[vv][0]))                                                       \
|(((GLuint)VB->ColorPtr->data[vv][3]) << 24)                                                 \
|rivaContext.modeMask

/*
 * Compute the fog value for a given distance in eye corrdinates. Taken from Mesa.
 */
static GLint RivaFog
(
    GLcontext *ctx,
    GLfloat    z
)
{
    GLfloat d, f;
    GLuint  fog;

    switch (ctx->Fog.Mode)
    {
        case GL_LINEAR:
            d = 1.0F / (ctx->Fog.End - ctx->Fog.Start);
            f = (ctx->Fog.End - z) * d;
            f = CLAMP(f, 0.0F, 1.0F) * 255.0F;
            break;
        case GL_EXP:
            d = -ctx->Fog.Density;
            f = exp(d * z);
            f = CLAMP(f, 0.0F, 1.0F) * 255.0F;
            break;
        case GL_EXP2:
            d = -(ctx->Fog.Density*ctx->Fog.Density);
            f = exp(d * z*z);
            f = CLAMP(f, 0.0F, 1.0F) * 255.0F;
            break;
        default:
            f = 0.0F;
    }
    RivaFloatToUInt(fog, f);
    return (fog << 24);
}
/*
 * Mesa acceleration for rasterizing 3D primitives.
 */
void RivaPointsNOP
(
    GLcontext *ctx,
    GLuint first,
    GLuint last
)
{
}

void RivaPoints3D05(GLcontext *ctx, GLuint first, GLuint last)
{
    int                   nclipRects, clipped;
    BoxPtr                pclipRect;
    BoxRec                intersectRect, boundRect;
    GLfloat               x, y, z, s, t;
    GLuint                pv, argb, fog;
    struct vertex_buffer *VB        = ctx->VB;
//    XSMesaContext         xsmesa    = (XSMesaContext) ctx->DriverCtx;
//rudolf: disabled by me
//    WindowPtr             pwin      = (WindowPtr)(xsmesa->xsm_buffer->frontbuffer);
//rudolf: testing..
//    GLfloat               winOrgX   = (GLfloat)(pwin->drawable.x) - rivaContext.xyAdjust;
//    GLfloat               winOrgY   = (GLfloat)(pwin->drawable.y + pwin->drawable.height) - rivaContext.xyAdjust;
	GLfloat               winOrgX = 0 - rivaContext.xyAdjust;
	GLfloat               winOrgY =
		(FrontBuffer.bottom - FrontBuffer.top + 1) - rivaContext.xyAdjust;
    GLfloat               pointSize = ctx->Point.Size * 0.75;

	LOG(1,("RivaPoints3D05: called\n"));

	/* block rendering while a mode-change is being executed! */
	if (si->mode_changed) return;

    /*
     * Get window clip rects.
     */
//rudolf: fixme..
/*
    if (pwin->clipList.data)
    {
        if (pwin->clipList.data->numRects == 0)
            return;
        else if (rivaContext.SimpleClip)
        {
            nclipRects = 1;
            pclipRect  = &pwin->clipList.extents;
        }
        else
        {
            nclipRects = pwin->clipList.data->numRects;
            pclipRect  = (BoxPtr)(pwin->clipList.data + 1);
        }
    }
    else
    {
        nclipRects = 1;
        pclipRect  = &pwin->clipList.extents;
    }
*/
    /*
     * Iterate through all clip rects.
     */
    clipped = FALSE;
//rud: fix if frontbuffer rendering!
//    while (nclipRects--)
    {
		//rudolf: relocate for frontbuffer rendering!
		AQUIRE_BEN(si->engine.lock)
		nv_acc_assert_fifo();
		//rudolf: only set state for first point
		riva_sinfo[FAKESNUM].rivaReload3D = 1;
        /*
         * Iterate throught all points.
         */
        for (pv = first; pv <= last; pv++)
        {
            /*
             * Get point extents.
             */
            x = winOrgX + VB->Win.data[pv][0];
            y = winOrgY - VB->Win.data[pv][1];
            z =           VB->Win.data[pv][2] + ctx->PointZoffset;
            RivaFloatToInt(boundRect.x1, x - pointSize);
            RivaFloatToInt(boundRect.x2, x + pointSize);
            RivaFloatToInt(boundRect.y1, y - pointSize);
            RivaFloatToInt(boundRect.y2, y + pointSize);
			LOG(1,("RivaPoints3D05: boundRect %d,%d;%d,%d\n",
				boundRect.x1, boundRect.y1, boundRect.x2, boundRect.y2));
            /*
             * Intersect line bounds with clip rect.
             */
//rud: fix if frontbuffer rendering!
//        intersectRect.x1 = max(pclipRect->x1, boundRect.x1);
//        intersectRect.y1 = max(pclipRect->y1, boundRect.y1);
//        intersectRect.x2 = min(pclipRect->x2, boundRect.x2);
//        intersectRect.y2 = min(pclipRect->y2, boundRect.y2);
//        pclipRect++;
//rud: testing:
			intersectRect.x1 = max(0, boundRect.x1);
			intersectRect.y1 = max(0, boundRect.y1);
			intersectRect.x2 = min(
				(uint16)(FrontBuffer.right - FrontBuffer.left + 1), boundRect.x2);
			intersectRect.y2 = min(
				(uint16)(FrontBuffer.bottom - FrontBuffer.top + 1), boundRect.y2);

            /*
             * Intersect with scissor clip if enabled.
             */
            if (ctx->Scissor.Enabled)
            {
				LOG(1,("RivaPoints3D05: Scissor is enabled\n"));
                BoxRec scissor;

//rudolf: testing:
//                scissor.x1 = pwin->drawable.x + ctx->Scissor.X;
//                scissor.y2 = pwin->drawable.y + pwin->drawable.height - ctx->Scissor.Y;
	            scissor.x1 = ctx->Scissor.X;
    	        scissor.y2 = (uint16)(FrontBuffer.bottom - FrontBuffer.top + 1) - ctx->Scissor.Y;
                scissor.x2 = scissor.x1       + ctx->Scissor.Width;
                scissor.y1 = scissor.y2       - ctx->Scissor.Height;
                intersectRect.x1 = max(scissor.x1, intersectRect.x1);
                intersectRect.y1 = max(scissor.y1, intersectRect.y1);
                intersectRect.x2 = min(scissor.x2, intersectRect.x2);
                intersectRect.y2 = min(scissor.y2, intersectRect.y2);
            }
            /*
             * Set clip rect if there is a bounds-clip intersection.
             */
            if ((intersectRect.x1 != boundRect.x1)
             || (intersectRect.x2 != boundRect.x2)
             || (intersectRect.y1 != boundRect.y1)
             || (intersectRect.y2 != boundRect.y2))
            {
				LOG(1,("RivaPoints3D05: clipped is true\n"));
                /*
                 * Skip this triangle if clipped away.
                 */
//rud: fix if frontbuffer rendering!
//	            if ((intersectRect.x1 >= intersectRect.x2) || (intersectRect.y1 >= intersectRect.y2))
//  	             continue;
                RIVA_CLIP3D(FAKESNUM,intersectRect.x1, intersectRect.y1,
                            intersectRect.x2, intersectRect.y2);
                clipped = TRUE;
            }
            /*
             * Send line state.
             */
	        if (riva_sinfo[FAKESNUM].rivaReload3D)
            {
                riva_sinfo[FAKESNUM].rivaReload3D = 0;
				//rudolf: due to a shortcoming in MESA3.2 we need to overwrite filtering related
				//stuff if no texturing is used (see riva_ctx.cpp, RivaUpdateState05())
	    	    if (!(ctx->Texture.Enabled) || !(ctx->Texture.Unit[0].CurrentD[2]))
				{
					rivaContext.texOffset = riva_sinfo[FAKESNUM].DefaultTexture->voffset;
					rivaContext.triContext.tri05.texFormat = 0x112215A1;
				}
                RIVA_STATE3D_05(FAKESNUM,
				rivaContext.triContext.tri05.texColorKey,
                                rivaContext.texOffset,
                                rivaContext.triContext.tri05.texFormat,
                                rivaContext.triContext.tri05.texFilter,
                                rivaContext.triContext.tri05.triBlend,
                                rivaContext.triContext.tri05.triControl,
                                rivaContext.triContext.tri05.triFogColor);
            }
            /*
             * Send point as quad.
             */
            if (ctx->Fog.Enabled)
                fog = RivaFog(ctx, VB->Clip.data[pv][3]);
            else
                fog = 0x00000000;
            argb = RivaColor(pv);
            s = VB->TexCoordPtr[0]->data[pv][0];
            t = VB->TexCoordPtr[0]->data[pv][1];
            RIVA_VERTEX3D_05(FAKESNUM,0, x - pointSize, y + pointSize, z, 1.0F, argb, fog, s, t);
            RIVA_VERTEX3D_05(FAKESNUM,1, x - pointSize, y - pointSize, z, 1.0F, argb, fog, s, t);
            RIVA_VERTEX3D_05(FAKESNUM,2, x + pointSize, y - pointSize, z, 1.0F, argb, fog, s, t);
            RIVA_VERTEX3D_05(FAKESNUM,3, x + pointSize, y + pointSize, z, 1.0F, argb, fog, s, t);
//            RIVA_DRAWTRI3D_05(FAKESNUM,2, 0, 1);
//            RIVA_DRAWTRI3D_05(FAKESNUM,3, 0, 2);
			RIVA_DRAWQUAD3D_05(FAKESNUM,0, 1, 2, 3);
        }
    }
    /*
     * Reset clip rect if needed.
     */
    if (clipped)
        RIVA_CLIP3D(FAKESNUM,0, 0, 0x7FFF, 0x7FFF);

	//rudolf: release here!
	RELEASE_BEN(si->engine.lock)

//rud: sync needed here??
//rudtst: (should this be moved to drawing/reading pixels etc in BGLView???)
        if (0)//rivaSyncPix)
        {
			AQUIRE_BEN(si->engine.lock)
            rivaSyncPix  = 0;
            RivaSync(FAKESNUM);
			RELEASE_BEN(si->engine.lock)
        }
//end rudtst.
}

void RivaAAPoints3D05
(
    GLcontext *ctx,
    GLuint first,
    GLuint last
)
{
    int                   nclipRects, clipped;
    BoxPtr                pclipRect;
    BoxRec                intersectRect, boundRect;
    GLfloat               x, y, z;
    GLuint                pv, argb, fog;
    struct vertex_buffer *VB        = ctx->VB;
//    XSMesaContext         xsmesa    = (XSMesaContext) ctx->DriverCtx;
//rudolf: disabled by me..
//    WindowPtr             pwin      = (WindowPtr)(xsmesa->xsm_buffer->frontbuffer);
//rudolf: testing..
//    GLfloat               winOrgX   = (GLfloat)(pwin->drawable.x) - rivaContext.xyAdjust;
//    GLfloat               winOrgY   = (GLfloat)(pwin->drawable.y + pwin->drawable.height) - rivaContext.xyAdjust;
	GLfloat               winOrgX = 0 - rivaContext.xyAdjust;
	GLfloat               winOrgY =
		(FrontBuffer.bottom - FrontBuffer.top + 1) - rivaContext.xyAdjust;
    GLfloat               pointSize = ctx->Point.Size * 0.75;

	LOG(1,("RivaAAPoints3D05: called\n"));

	/* block rendering while a mode-change is being executed! */
	if (si->mode_changed) return;

    /*
     * Get window clip rects.
     */
//rudolf: fixme..
/*
    if (pwin->clipList.data)
    {
        if (pwin->clipList.data->numRects == 0)
            return;
        else if (rivaContext.SimpleClip)
        {
            nclipRects = 1;
            pclipRect  = &pwin->clipList.extents;
        }
        else
        {
            nclipRects = pwin->clipList.data->numRects;
            pclipRect  = (BoxPtr)(pwin->clipList.data + 1);
        }
    }
    else
    {
        nclipRects = 1;
        pclipRect  = &pwin->clipList.extents;
    }
*/
    /*
     * Iterate through all clip rects.
     */
    clipped = FALSE;
//rud: fix if frontbuffer rendering!
//    while (nclipRects--)
    {
		//rudolf: relocate for frontbuffer rendering!
		AQUIRE_BEN(si->engine.lock)
		nv_acc_assert_fifo();
		//rudolf: only set state for first point
		riva_sinfo[FAKESNUM].rivaReload3D = 1;
        /*
         * Iterate throught all points.
         */
        for (pv = first; pv <= last; pv++)
        {
            /*
             * Get point extents.
             */
            x = winOrgX + VB->Win.data[pv][0];
            y = winOrgY - VB->Win.data[pv][1];
            z =           VB->Win.data[pv][2] + ctx->PointZoffset;
            RivaFloatToInt(boundRect.x1, x - pointSize);
            RivaFloatToInt(boundRect.x2, x + pointSize);
            RivaFloatToInt(boundRect.y1, y - pointSize);
            RivaFloatToInt(boundRect.y2, y + pointSize);
			LOG(1,("RivaAAPoints3D05: boundRect %d,%d;%d,%d\n",
				boundRect.x1, boundRect.y1, boundRect.x2, boundRect.y2));

            /*
             * Intersect line bounds with clip rect.
             */
//rud: fix if frontbuffer rendering!
//        intersectRect.x1 = max(pclipRect->x1, boundRect.x1);
//        intersectRect.y1 = max(pclipRect->y1, boundRect.y1);
//        intersectRect.x2 = min(pclipRect->x2, boundRect.x2);
//        intersectRect.y2 = min(pclipRect->y2, boundRect.y2);
//        pclipRect++;
//rud: testing:
			intersectRect.x1 = max(0, boundRect.x1);
			intersectRect.y1 = max(0, boundRect.y1);
			intersectRect.x2 = min(
				(uint16)(FrontBuffer.right - FrontBuffer.left + 1), boundRect.x2);
			intersectRect.y2 = min(
				(uint16)(FrontBuffer.bottom - FrontBuffer.top + 1), boundRect.y2);

            /*
             * Intersect with scissor clip if enabled.
             */
            if (ctx->Scissor.Enabled)
            {
				LOG(1,("RivaAAPoints3D05: Scissor is enabled\n"));
                BoxRec scissor;

//rudolf: testing:
//                scissor.x1 = pwin->drawable.x + ctx->Scissor.X;
//                scissor.y2 = pwin->drawable.y + pwin->drawable.height - ctx->Scissor.Y;
	            scissor.x1 = ctx->Scissor.X;
    	        scissor.y2 = (uint16)(FrontBuffer.bottom - FrontBuffer.top + 1) - ctx->Scissor.Y;
                scissor.x2 = scissor.x1       + ctx->Scissor.Width;
                scissor.y1 = scissor.y2       - ctx->Scissor.Height;
                intersectRect.x1 = max(scissor.x1, intersectRect.x1);
                intersectRect.y1 = max(scissor.y1, intersectRect.y1);
                intersectRect.x2 = min(scissor.x2, intersectRect.x2);
                intersectRect.y2 = min(scissor.y2, intersectRect.y2);
            }
            /*
             * Set clip rect if there is a bounds-clip intersection.
             */
            if ((intersectRect.x1 != boundRect.x1)
             || (intersectRect.x2 != boundRect.x2)
             || (intersectRect.y1 != boundRect.y1)
             || (intersectRect.y2 != boundRect.y2))
            {
				LOG(1,("RivaAAPoints3D05: clipped is true\n"));
                /*
                 * Skip this triangle if clipped away.
                 */
//rud: fix if frontbuffer rendering!
//	            if ((intersectRect.x1 >= intersectRect.x2) || (intersectRect.y1 >= intersectRect.y2))
//  	             continue;
                RIVA_CLIP3D(FAKESNUM,intersectRect.x1, intersectRect.y1,
                            intersectRect.x2, intersectRect.y2);
                clipped = TRUE;
            }
            /*
             * Send line state.
             */
	        if (riva_sinfo[FAKESNUM].rivaReload3D)
            {
                riva_sinfo[FAKESNUM].rivaReload3D = 0;
				//rudolf: due to a shortcoming in MESA3.2 we need to overwrite filtering related
				//stuff if no texturing is used (see riva_ctx.cpp, RivaUpdateState05())
	    	    if (!(ctx->Texture.Enabled) || !(ctx->Texture.Unit[0].CurrentD[2]))
				{
					rivaContext.texOffset = riva_sinfo[FAKESNUM].DefaultAATexture->voffset;
					rivaContext.triContext.tri05.texFormat = 0x33441461;
				}
                RIVA_STATE3D_05(FAKESNUM,rivaContext.triContext.tri05.texColorKey,
                                rivaContext.texOffset,
                                rivaContext.triContext.tri05.texFormat,
                                rivaContext.triContext.tri05.texFilter,
                                rivaContext.triContext.tri05.triBlend,
                                rivaContext.triContext.tri05.triControl,
                                rivaContext.triContext.tri05.triFogColor);
            }
            /*
             * Send point as quad.
             */
            if (ctx->Fog.Enabled)
                fog = RivaFog(ctx, VB->Clip.data[pv][3]);
            else
                fog = 0x00000000;
            argb = RivaColor(pv);
            RIVA_VERTEX3D_05(FAKESNUM,0, x - pointSize, y + pointSize, z, 1.0F, argb, fog, 0.0F, 1.0F);
            RIVA_VERTEX3D_05(FAKESNUM,1, x - pointSize, y - pointSize, z, 1.0F, argb, fog, 0.0F, 0.0F);
            RIVA_VERTEX3D_05(FAKESNUM,2, x + pointSize, y - pointSize, z, 1.0F, argb, fog, 1.0F, 0.0F);
            RIVA_VERTEX3D_05(FAKESNUM,3, x + pointSize, y + pointSize, z, 1.0F, argb, fog, 1.0F, 1.0F);
//            RIVA_DRAWTRI3D_05(FAKESNUM,2, 0, 1);
//            RIVA_DRAWTRI3D_05(FAKESNUM,3, 0, 2);
			RIVA_DRAWQUAD3D_05(FAKESNUM,0, 1, 2, 3);
        }
    }
    /*
     * Reset clip rect if needed.
     */
    if (clipped)
        RIVA_CLIP3D(FAKESNUM,0, 0, 0x7FFF, 0x7FFF);

	//rudolf: release here!
	RELEASE_BEN(si->engine.lock)
}

void RivaLineNOP
(
    GLcontext *ctx,
    GLuint     v0,
    GLuint     v1,
    GLuint     pv
)
{
}
void RivaLine3D05
(
    GLcontext *ctx,
    GLuint     v0,
    GLuint     v1,
    GLuint     pv
)
{
    int                   nclipRects, clipped;
    BoxPtr                pclipRect;
    BoxRec                intersectRect, boundRect;
    GLfloat               x0, x1, dx, y0, y1, dy, z, m, s, t;
    GLuint                argb, fog0, fog1;
    struct vertex_buffer *VB        = ctx->VB;
//    XSMesaContext         xsmesa    = (XSMesaContext) ctx->DriverCtx;
//rudolf: disabled by me..
//    WindowPtr             pwin      = (WindowPtr)(xsmesa->xsm_buffer->frontbuffer);
//rudolf: testing..
//    GLfloat               winOrgX   = (GLfloat)(pwin->drawable.x) - rivaContext.xyAdjust;
//    GLfloat               winOrgY   = (GLfloat)(pwin->drawable.y + pwin->drawable.height) - rivaContext.xyAdjust;
	GLfloat               winOrgX = 0 - rivaContext.xyAdjust;
	GLfloat               winOrgY =
		(FrontBuffer.bottom - FrontBuffer.top + 1) - rivaContext.xyAdjust;
	GLfloat               lineWidth = ctx->Line.Width * 0.5F;

	LOG(1,("RivaLine3D05: called\n"));

	/* block rendering while a mode-change is being executed! */
	if (si->mode_changed) return;

    /*
     * Get window clip rects.
     */
//rudolf: fixme:
/*
    if (pwin->clipList.data)
    {
        if (pwin->clipList.data->numRects == 0)
            return;
        else if (rivaContext.SimpleClip)
        {
            nclipRects = 1;
            pclipRect  = &pwin->clipList.extents;
        }
        else
        {
            nclipRects = pwin->clipList.data->numRects;
            pclipRect  = (BoxPtr)(pwin->clipList.data + 1);
        }
    }
    else
    {
        nclipRects = 1;
        pclipRect  = &pwin->clipList.extents;
    }
*/
    /*
     * Init fog.
     */
    if (ctx->Fog.Enabled)
    {
		LOG(1,("RivaLine3D05: Fog enabled\n"));
        fog0 = RivaFog(ctx, VB->Clip.data[v0][3]);
        fog1 = RivaFog(ctx, VB->Clip.data[v1][3]);
    }
    else
    {
        fog0 =
        fog1 = 0x00000000;
    }
    /*
     * Get line extents.
     */
    x0 = winOrgX + VB->Win.data[v0][0]; y0 = winOrgY - VB->Win.data[v0][1];
    x1 = winOrgX + VB->Win.data[v1][0]; y1 = winOrgY - VB->Win.data[v1][1];
	LOG(1,("RivaLine3D05: line extents %f,%f;%f,%f\n", x0, y0, x1, y1));
    if (x0 < x1)
    {
        RivaFloatToInt(boundRect.x1, x0 - (0.5F + lineWidth));
        RivaFloatToInt(boundRect.x2, x1 + (0.5F + lineWidth));
    }
    else
    {
        RivaFloatToInt(boundRect.x1, x1 - (0.5F + lineWidth));
        RivaFloatToInt(boundRect.x2, x0 + (0.5F + lineWidth));
    }
    if (y0 < y1)
    {
        RivaFloatToInt(boundRect.y1, y0 - (0.5F + lineWidth));
        RivaFloatToInt(boundRect.y2, y1 + (0.5F + lineWidth));
    }
    else
    {
        RivaFloatToInt(boundRect.y1, y1 - (0.5F + lineWidth));
        RivaFloatToInt(boundRect.y2, y0 + (0.5F + lineWidth));
    }
	LOG(1,("RivaLine3D05: boundRect %d,%d;%d,%d\n",
		boundRect.x1, boundRect.y1, boundRect.x2, boundRect.y2));
    /*
     * Determine major and minor axis.
     */
    dx = x1 - x0; dy = y1 - y0;
    if (fabs(dx) > fabs(dy))
    {
        dy = lineWidth;
        dx = 0.0F;
    }
    else
    {
        dx = lineWidth;
        dy = 0.0F;
    }
    /*
     * Iterate through all clip rects.
     */
    clipped = FALSE;
//rud: fix if frontbuffer rendering!
//    while (nclipRects--)
    {
        /*
         * Intersect line bounds with clip rect.
         */
//rud: fix if frontbuffer rendering!
//        intersectRect.x1 = max(pclipRect->x1, boundRect.x1);
//        intersectRect.y1 = max(pclipRect->y1, boundRect.y1);
//        intersectRect.x2 = min(pclipRect->x2, boundRect.x2);
//        intersectRect.y2 = min(pclipRect->y2, boundRect.y2);
//        pclipRect++;
//rud: testing:
        intersectRect.x1 = max(0, boundRect.x1);
        intersectRect.y1 = max(0, boundRect.y1);
        intersectRect.x2 = min(
        	(uint16)(FrontBuffer.right - FrontBuffer.left + 1), boundRect.x2);
        intersectRect.y2 = min(
        	(uint16)(FrontBuffer.bottom - FrontBuffer.top + 1), boundRect.y2);
        /*
         * Intersect with scissor clip if enabled.
         */
        if (ctx->Scissor.Enabled)
        {
			LOG(1,("RivaLine3D05: Scissor is enabled\n"));
            BoxRec scissor;

//rudolf: testing:
//            scissor.x1 = pwin->drawable.x + ctx->Scissor.X;
//            scissor.y2 = pwin->drawable.y + pwin->drawable.height - ctx->Scissor.Y;
            scissor.x1 = ctx->Scissor.X;
            scissor.y2 = (uint16)(FrontBuffer.bottom - FrontBuffer.top + 1) - ctx->Scissor.Y;
            scissor.x2 = scissor.x1       + ctx->Scissor.Width;
            scissor.y1 = scissor.y2       - ctx->Scissor.Height;
            intersectRect.x1 = max(scissor.x1, intersectRect.x1);
            intersectRect.y1 = max(scissor.y1, intersectRect.y1);
            intersectRect.x2 = min(scissor.x2, intersectRect.x2);
            intersectRect.y2 = min(scissor.y2, intersectRect.y2);
        }
		//rudolf: relocate for frontbuffer rendering!
		AQUIRE_BEN(si->engine.lock)
		nv_acc_assert_fifo();
        /*
         * Set clip rect if there is a bounds-clip intersection.
         */
        if ((intersectRect.x1 != boundRect.x1)
         || (intersectRect.x2 != boundRect.x2)
         || (intersectRect.y1 != boundRect.y1)
         || (intersectRect.y2 != boundRect.y2))
        {
			LOG(1,("RivaLine3D05: clipped is true\n"));
             /*
             * Skip this triangle if clipped away.
             */
//rud: fix if frontbuffer rendering!
//            if ((intersectRect.x1 >= intersectRect.x2) || (intersectRect.y1 >= intersectRect.y2))
//               continue;
            RIVA_CLIP3D(FAKESNUM,intersectRect.x1, intersectRect.y1,
                        intersectRect.x2, intersectRect.y2);
            clipped = TRUE;
        }
        /* Send line state */
//rudolf: fixme: do this whenever the 2D driver did a function!!!!!!!!!!!!!!!!!!!!!!<<<<<
        if (1)//riva_sinfo[FAKESNUM].rivaReload3D)
        {
            riva_sinfo[FAKESNUM].rivaReload3D = 0;
			//rudolf: due to a shortcoming in MESA3.2 we need to overwrite filtering related
			//stuff if no texturing is used (see riva_ctx.cpp, RivaUpdateState05())
	        if (!(ctx->Texture.Enabled) || !(ctx->Texture.Unit[0].CurrentD[2]))
			{
				rivaContext.texOffset = riva_sinfo[FAKESNUM].DefaultTexture->voffset;
				rivaContext.triContext.tri05.texFormat = 0x112215A1;
			}
            RIVA_STATE3D_05(FAKESNUM, rivaContext.triContext.tri05.texColorKey,
                            rivaContext.texOffset,
                            rivaContext.triContext.tri05.texFormat,
                            rivaContext.triContext.tri05.texFilter,
                            rivaContext.triContext.tri05.triBlend,
                            rivaContext.triContext.tri05.triControl,
                            rivaContext.triContext.tri05.triFogColor);
        }
        /* Send vertex 0 */
        if (ctx->Light.ShadeModel == GL_SMOOTH)
            argb = RivaColor(v0);
        else
            argb = RivaColor(pv);

//rud tst info: (from linetemp.h)!! (LOOK AT UNSCALE and SCALE!!!)
//(#define FloatToFixed(X) ((GLfixed) ((X) * FIXED_SCALE)))
//GLfixed == int;  FIXED_SCALE  =   2048.0f (expand to 27 bits?)
//#  if DEPTH_BITS==16
//      z = FloatToFixed(VB->Win.data[v0][2]);
//#  else
//      z = (int) VB->Win.data[v0][2];
//end test

//org:
        z = VB->Win.data[v0][2] + ctx->LineZoffset;
        m = INV_W(v0);
        s = VB->TexCoordPtr[0]->data[v0][0];
        t = VB->TexCoordPtr[0]->data[v0][1];
        RIVA_VERTEX3D_05(FAKESNUM,0, x0 - dx, y0 - dy, z, m, argb, fog0, s, t);
        RIVA_VERTEX3D_05(FAKESNUM,1, x0 + dx, y0 + dy, z, m, argb, fog0, s, t);
        /* Send vertex 1 */
        if (ctx->Light.ShadeModel == GL_SMOOTH)
            argb = RivaColor(v1);
        z = VB->Win.data[v1][2] + ctx->LineZoffset;
        m = INV_W(v1);
        s = VB->TexCoordPtr[0]->data[v1][0];
        t = VB->TexCoordPtr[0]->data[v1][1];
        RIVA_VERTEX3D_05(FAKESNUM,2, x1 + dx, y1 + dy, z, m, argb, fog1, s, t);
        RIVA_VERTEX3D_05(FAKESNUM,3, x1 - dx, y1 - dy, z, m, argb, fog1, s, t);
//rud: quad is faster (check teapot with disabled filled polygons: used line func!!
//        RIVA_DRAWTRI3D_05(FAKESNUM,2, 0, 1);
//        RIVA_DRAWTRI3D_05(FAKESNUM,3, 0, 2);
		RIVA_DRAWQUAD3D_05(FAKESNUM,0, 1, 2, 3);
    }
    /*
     * Reset clip rect if needed.
     */
    if (clipped)
        RIVA_CLIP3D(FAKESNUM,0, 0, 0x7FFF, 0x7FFF);

	//rudolf: release here!
	RELEASE_BEN(si->engine.lock)
//rud: sync needed here??
//rudtst: (should this be moved to drawing/reading pixels etc in BGLView???)
        if (0)//rivaSyncPix)
        {
			AQUIRE_BEN(si->engine.lock)
            rivaSyncPix  = 0;
            RivaSync(FAKESNUM);
			RELEASE_BEN(si->engine.lock)
        }
//end rudtst.
}

void RivaAALine3D05
(
    GLcontext *ctx,
    GLuint     v0,
    GLuint     v1,
    GLuint     pv
)
{
    int                   nclipRects, clipped;
    BoxPtr                pclipRect;
    BoxRec                intersectRect, boundRect;
    GLfloat               x0, x1, dx, y0, y1, dy, z, m;
    GLuint                argb, fog0, fog1;
    struct vertex_buffer *VB        = ctx->VB;
//    XSMesaContext         xsmesa    = (XSMesaContext) ctx->DriverCtx;
//rudolf: disabled by me..
//    WindowPtr             pwin      = (WindowPtr)(xsmesa->xsm_buffer->frontbuffer);
//rudolf: testing..
//    GLfloat               winOrgX   = (GLfloat)(pwin->drawable.x) - rivaContext.xyAdjust;
//    GLfloat               winOrgY   = (GLfloat)(pwin->drawable.y + pwin->drawable.height) - rivaContext.xyAdjust;
	GLfloat               winOrgX = 0 - rivaContext.xyAdjust;
	GLfloat               winOrgY =
		(FrontBuffer.bottom - FrontBuffer.top + 1) - rivaContext.xyAdjust;
    GLfloat               lineWidth = ctx->Line.Width;

	LOG(1,("RivaAALine3D05: called\n"));

	/* block rendering while a mode-change is being executed! */
	if (si->mode_changed) return;

    /*
     * Get window clip rects.
     */
//rudolf: fixme:
/*
    if (pwin->clipList.data)
    {
        if (pwin->clipList.data->numRects == 0)
            return;
        else if (rivaContext.SimpleClip)
        {
            nclipRects = 1;
            pclipRect  = &pwin->clipList.extents;
        }
        else
        {
            nclipRects = pwin->clipList.data->numRects;
            pclipRect  = (BoxPtr)(pwin->clipList.data + 1);
        }
    }
    else
    {
        nclipRects = 1;
        pclipRect  = &pwin->clipList.extents;
    }
*/
    /*
     * Init fog.
     */
    if (ctx->Fog.Enabled)
    {
		LOG(1,("RivaAALine3D05: Fog enabled\n"));
        fog0 = RivaFog(ctx, VB->Clip.data[v0][3]);
        fog1 = RivaFog(ctx, VB->Clip.data[v1][3]);
    }
    else
    {
        fog0 =
        fog1 = 0x00000000;
    }
    /*
     * Get line extents.
     */
    x0 = winOrgX + VB->Win.data[v0][0]; y0 = winOrgY - VB->Win.data[v0][1];
    x1 = winOrgX + VB->Win.data[v1][0]; y1 = winOrgY - VB->Win.data[v1][1];
	LOG(1,("RivaAALine3D05: line extents %f,%f;%f,%f\n", x0, y0, x1, y1));
    if (x0 < x1)
    {
        RivaFloatToInt(boundRect.x1, x0 - (0.5F + lineWidth));
        RivaFloatToInt(boundRect.x2, x1 + (0.5F + lineWidth));
    }
    else
    {
        RivaFloatToInt(boundRect.x1, x1 - (0.5F + lineWidth));
        RivaFloatToInt(boundRect.x2, x0 + (0.5F + lineWidth));
    }
    if (y0 < y1)
    {
        RivaFloatToInt(boundRect.y1, y0 - (0.5F + lineWidth));
        RivaFloatToInt(boundRect.y2, y1 + (0.5F + lineWidth));
    }
    else
    {
        RivaFloatToInt(boundRect.y1, y1 - (0.5F + lineWidth));
        RivaFloatToInt(boundRect.y2, y0 + (0.5F + lineWidth));
    }
	LOG(1,("RivaAALine3D05: boundRect %d,%d;%d,%d\n",
		boundRect.x1, boundRect.y1, boundRect.x2, boundRect.y2));
    /*
     * Determine major and minor axis.
     */
    dx = x1 - x0; dy = y1 - y0;
    if (fabs(dx) > fabs(dy))
    {
        dy = lineWidth;
        dx = 0.0F;
    }
    else
    {
        dx = lineWidth;
        dy = 0.0F;
    }
    /*
     * Iterate through all clip rects.
     */
    clipped = FALSE;
//rud: fix if frontbuffer rendering!
//    while (nclipRects--)
    {
        /*
         * Intersect line bounds with clip rect.
         */
//rud: fix if frontbuffer rendering!
//        intersectRect.x1 = max(pclipRect->x1, boundRect.x1);
//        intersectRect.y1 = max(pclipRect->y1, boundRect.y1);
//        intersectRect.x2 = min(pclipRect->x2, boundRect.x2);
//        intersectRect.y2 = min(pclipRect->y2, boundRect.y2);
//        pclipRect++;
//rud: testing:
        intersectRect.x1 = max(0, boundRect.x1);
        intersectRect.y1 = max(0, boundRect.y1);
        intersectRect.x2 = min(
        	(uint16)(FrontBuffer.right - FrontBuffer.left + 1), boundRect.x2);
        intersectRect.y2 = min(
        	(uint16)(FrontBuffer.bottom - FrontBuffer.top + 1), boundRect.y2);
        /*
         * Intersect with scissor clip if enabled.
         */
        if (ctx->Scissor.Enabled)
        {
			LOG(1,("RivaAALine3D05: Scissor is enabled\n"));
            BoxRec scissor;

//rudolf: testing:
//            scissor.x1 = pwin->drawable.x + ctx->Scissor.X;
//            scissor.y2 = pwin->drawable.y + pwin->drawable.height - ctx->Scissor.Y;
            scissor.x1 = ctx->Scissor.X;
            scissor.y2 = (uint16)(FrontBuffer.bottom - FrontBuffer.top + 1) - ctx->Scissor.Y;
            scissor.x2 = scissor.x1       + ctx->Scissor.Width;
            scissor.y1 = scissor.y2       - ctx->Scissor.Height;
            intersectRect.x1 = max(scissor.x1, intersectRect.x1);
            intersectRect.y1 = max(scissor.y1, intersectRect.y1);
            intersectRect.x2 = min(scissor.x2, intersectRect.x2);
            intersectRect.y2 = min(scissor.y2, intersectRect.y2);
        }
		//rudolf: relocate for frontbuffer rendering!
		AQUIRE_BEN(si->engine.lock)
		nv_acc_assert_fifo();
        /*
         * Set clip rect if there is a bounds-clip intersection.
         */
        if ((intersectRect.x1 != boundRect.x1)
         || (intersectRect.x2 != boundRect.x2)
         || (intersectRect.y1 != boundRect.y1)
         || (intersectRect.y2 != boundRect.y2))
        {
			LOG(1,("RivaAALine3D05: clipped is true\n"));
            /*
             * Skip this triangle if clipped away.
             */
//rud: fix if frontbuffer rendering!
//            if ((intersectRect.x1 >= intersectRect.x2) || (intersectRect.y1 >= intersectRect.y2))
//               continue;
            RIVA_CLIP3D(FAKESNUM,intersectRect.x1, intersectRect.y1,
                        intersectRect.x2, intersectRect.y2);
            clipped = TRUE;
        }
        /*
         * Send line state.
         */
//rudolf: fixme: do this whenever the 2D driver did a function!!!!!!!!!!!!!!!!!!!!!!<<<<<
        if (1)//riva_sinfo[FAKESNUM].rivaReload3D)
        {
            riva_sinfo[FAKESNUM].rivaReload3D = 0;
			//rudolf: due to a shortcoming in MESA3.2 we need to overwrite filtering related
			//stuff if no texturing is used (see riva_ctx.cpp, RivaUpdateState05())
    	    if (!(ctx->Texture.Enabled) || !(ctx->Texture.Unit[0].CurrentD[2]))
			{
				rivaContext.texOffset = riva_sinfo[FAKESNUM].DefaultAATexture->voffset;
				rivaContext.triContext.tri05.texFormat = 0x33441461;
			}
            RIVA_STATE3D_05(FAKESNUM,rivaContext.triContext.tri05.texColorKey,
                            rivaContext.texOffset,
                            rivaContext.triContext.tri05.texFormat,
                            rivaContext.triContext.tri05.texFilter,
                            rivaContext.triContext.tri05.triBlend,
                            rivaContext.triContext.tri05.triControl,
                            rivaContext.triContext.tri05.triFogColor);
        }
        /*
         * Send vertex 0.
         */
        if (ctx->Light.ShadeModel == GL_SMOOTH)
            argb = RivaColor(v0);
        else
            argb = RivaColor(pv);
        z = VB->Win.data[v0][2] + ctx->LineZoffset;
        m = (ctx->Fog.Enabled) ? INV_W(v0) : 1.0F;
        RIVA_VERTEX3D_05(FAKESNUM,0, x0 - dx, y0 - dy, z, m,
			 argb, fog0, 0.5F, 0.0F);
        RIVA_VERTEX3D_05(FAKESNUM,1, x0 + dx, y0 + dy, z, m,
			 argb, fog0, 0.5F, 1.0F);
        /*
         * Send vertex 1.
         */
        if (ctx->Light.ShadeModel == GL_SMOOTH)
            argb = RivaColor(v1);
        z = VB->Win.data[v1][2] + ctx->LineZoffset;
        m = (ctx->Fog.Enabled) ? INV_W(v1) : 1.0F;
        RIVA_VERTEX3D_05(FAKESNUM,2, x1 + dx, y1 + dy, z, m, argb, fog1, 0.5F, 1.0F);
        RIVA_VERTEX3D_05(FAKESNUM,3, x1 - dx, y1 - dy, z, m, argb, fog1, 0.5F, 0.0F);
//        RIVA_DRAWTRI3D_05(FAKESNUM,2, 0, 1);
//        RIVA_DRAWTRI3D_05(FAKESNUM,3, 0, 2);
		RIVA_DRAWQUAD3D_05(FAKESNUM,0, 1, 2, 3);
    }
    /*
     * Reset clip rect if needed.
     */
    if (clipped)
        RIVA_CLIP3D(FAKESNUM,0, 0, 0x7FFF, 0x7FFF);

	//rudolf: release here!
	RELEASE_BEN(si->engine.lock)
}

void RivaTriangleNOP(GLcontext *ctx, GLuint v0, GLuint v1, GLuint v2, GLuint pv)
{
}

void RivaTriangle3D05(GLcontext *ctx, GLuint v0, GLuint v1, GLuint v2, GLuint pv)
{
    int                   nclipRects, clipped;
    BoxPtr                pclipRect;
    BoxRec                intersectRect, boundRect;
    GLfloat               x0, x1, x2, y0, y1, y2, m0, m1, m2;
    GLfloat               xmin, xmax, ymin, ymax;
    GLuint                argb0, argb1, argb2, fog0, fog1, fog2;
    struct vertex_buffer *VB      = ctx->VB;
//    XSMesaContext         xsmesa  = (XSMesaContext) ctx->DriverCtx;
//rudolf: disabled by me..
//    WindowPtr             pwin    = (WindowPtr)(xsmesa->xsm_buffer->frontbuffer);

//rudolf: testing..
//    GLfloat               winOrgX = (GLfloat)(pwin->drawable.x) - rivaContext.xyAdjust;
//    GLfloat               winOrgY = (GLfloat)(pwin->drawable.y + pwin->drawable.height) - rivaContext.xyAdjust;
	GLfloat               winOrgX = 0 - rivaContext.xyAdjust;
	GLfloat               winOrgY =
		(FrontBuffer.bottom - FrontBuffer.top + 1) - rivaContext.xyAdjust;

    GLfloat               ooa;

	LOG(1,("RivaTriangle3D05: called\n"));

	/* block rendering while a mode-change is being executed! */
	if (si->mode_changed) return;

    /*
     * Determine triangle area for back face culling
     */
    x0 = VB->Win.data[v0][0];
    y0 = VB->Win.data[v0][1];
    x1 = VB->Win.data[v1][0];
    y1 = VB->Win.data[v1][1];
    x2 = VB->Win.data[v2][0];
    y2 = VB->Win.data[v2][1];

    ooa = ((x1 - x0) * (y0 - y2) + (y1 - y0) * (x2 - x0));
    if (ooa * ctx->backface_sign < 0) {
        return;        /* culled */
    }
    /*
     * Translate window coordinates into screen coordinates.
     */
    x0 = winOrgX + x0;
    y0 = winOrgY - y0;
    x1 = winOrgX + x1;
    y1 = winOrgY - y1;
    x2 = winOrgX + x2;
    y2 = winOrgY - y2;
    /*
     * Init fog.
     */
    if (ctx->Fog.Enabled)
    {
		LOG(1,("RivaTriangle3D05: Fog enabled\n"));
        fog0 = RivaFog(ctx, VB->Clip.data[v0][3]);
        fog1 = RivaFog(ctx, VB->Clip.data[v1][3]);
        fog2 = RivaFog(ctx, VB->Clip.data[v2][3]);
    }
    else
    {
        fog0 =
        fog1 =
        fog2 = 0x00000000;
    }
    /*
     * Vertex color.
     */
    if (ctx->Light.ShadeModel == GL_SMOOTH)
    {
		LOG(1,("RivaTriangle3D05: ShadeModel == GL_SMOOTH\n"));
        argb0 = RivaColor(v0);
        argb1 = RivaColor(v1);
        argb2 = RivaColor(v2);
    }
    else
    {
        argb0 =
        argb1 =
        argb2 = RivaColor(pv);
    }
    /*
     * Vertex 1/w.
     */
    m0 = INV_W(v0);
    m1 = INV_W(v1);
    m2 = INV_W(v2);
    /*
     * Get window clip rects.
     */
//rudolf: fixme:
/*
    if (pwin->clipList.data)
    {
        if (pwin->clipList.data->numRects == 0)
            return;
        else if (rivaContext.SimpleClip)
        {
            nclipRects = 1;
            pclipRect  = &pwin->clipList.extents;
        }
        else
        {
            nclipRects = pwin->clipList.data->numRects;
            pclipRect  = (BoxPtr)(pwin->clipList.data + 1);
        }
    }
    else
    {
        nclipRects = 1;
        pclipRect  = &pwin->clipList.extents;
    }
*/
    /*
     * Check for clipping.
     */
    if ( !ctx->Scissor.Enabled
//rudolf: fixme for singlebuffered rendering:
//     &&  (nclipRects == 1)
//rudolf: same fixme:
/*
     && ((pclipRect->x2 - pclipRect->x1) == pwin->drawable.width)
     && ((pclipRect->y2 - pclipRect->y1) == pwin->drawable.height)*/)
    {
		LOG(1,("RivaTriangle3D05: Scissor disabled\n"));
		//rudolf: relocate for frontbuffer rendering!
		AQUIRE_BEN(si->engine.lock)
		nv_acc_assert_fifo();

        /*
         * Send triangle state if needed.
         */
//rudolf: fixme: do this whenever the 2D driver did a function!!!!!!!!!!!!!!!!!!!!!!<<<<<
        if (1)//riva_sinfo[FAKESNUM].rivaReload3D)
        {
            riva_sinfo[FAKESNUM].rivaReload3D = 0;
			//rudolf: due to a shortcoming in MESA3.2 we need to overwrite filtering related
			//stuff if no texturing is used (see riva_ctx.cpp, RivaUpdateState05())
	        if (!(ctx->Texture.Enabled) || !(ctx->Texture.Unit[0].CurrentD[2]))
			{
				rivaContext.texOffset = riva_sinfo[FAKESNUM].DefaultTexture->voffset;
				rivaContext.triContext.tri05.texFormat = 0x112215A1;
			}
            RIVA_STATE3D_05(FAKESNUM,rivaContext.triContext.tri05.texColorKey,
                            rivaContext.texOffset,
                            rivaContext.triContext.tri05.texFormat,
                            rivaContext.triContext.tri05.texFilter,
                            rivaContext.triContext.tri05.triBlend,
                            rivaContext.triContext.tri05.triControl,
                            rivaContext.triContext.tri05.triFogColor);
        }
        /*
         * Send vertex 0
         */
        RIVA_VERTEX3D_05(FAKESNUM,0,
                         x0,
                         y0,
                         VB->Win.data[v0][2] + ctx->PolygonZoffset,
                         m0,
                         argb0,
                         fog0,
                         VB->TexCoordPtr[0]->data[v0][0],
                         VB->TexCoordPtr[0]->data[v0][1]);
        /*
         * Send vertex 1
         */
        RIVA_VERTEX3D_05(FAKESNUM,1,
                         x1,
                         y1,
                         VB->Win.data[v1][2] + ctx->PolygonZoffset,
                         m1,
                         argb1,
                         fog1,
                         VB->TexCoordPtr[0]->data[v1][0],
                         VB->TexCoordPtr[0]->data[v1][1]);
        /*
         * Send vertex 2
         */
        RIVA_VERTEX3D_05(FAKESNUM,2,
                         x2,
                         y2,
                         VB->Win.data[v2][2] + ctx->PolygonZoffset,
                         m2,
                         argb2,
                         fog2,
                         VB->TexCoordPtr[0]->data[v2][0],
                         VB->TexCoordPtr[0]->data[v2][1]);
        RIVA_DRAWTRI3D_05(FAKESNUM,0, 1, 2);

		RELEASE_BEN(si->engine.lock)
    }
    else
    {
		LOG(1,("RivaTriangle3D05: Scissor enabled\n"));
        /*
         * Get triangle extents for clipping.
         */
        clipped = FALSE;
        xmin = xmax = x0;
        ymin = ymax = y0;
        if (x1 < xmin) xmin = x1; else if (x1 > xmax) xmax = x1;
        if (y1 < ymin) ymin = y1; else if (y1 > ymax) ymax = y1;
        if (x2 < xmin) xmin = x2; else if (x2 > xmax) xmax = x2;
        if (y2 < ymin) ymin = y2; else if (y2 > ymax) ymax = y2;
        RivaFloatToInt(boundRect.x1, xmin - 0.5F);
        RivaFloatToInt(boundRect.y1, ymin - 0.5F);
        RivaFloatToInt(boundRect.x2, xmax + 0.5F);
        RivaFloatToInt(boundRect.y2, ymax + 0.5F);
        /*
         * Iterate through all clip rects.
         */
//rud: testing: fix for singlebuffered rendering!
//        while (nclipRects--)
        {
            /*
             * Intersect rectangle bounds with clip rect.
             */
//rud: fix if frontbuffer rendering!
//            intersectRect.x1 = max(pclipRect->x1, boundRect.x1);
//            intersectRect.y1 = max(pclipRect->y1, boundRect.y1);
//            intersectRect.x2 = min(pclipRect->x2, boundRect.x2);
//            intersectRect.y2 = min(pclipRect->y2, boundRect.y2);
//            pclipRect++;
//rud: testing:
			intersectRect.x1 = max(0, boundRect.x1);
			intersectRect.y1 = max(0, boundRect.y1);
			intersectRect.x2 = min(
				(uint16)(FrontBuffer.right - FrontBuffer.left + 1), boundRect.x2);
			intersectRect.y2 = min(
				(uint16)(FrontBuffer.bottom - FrontBuffer.top + 1), boundRect.y2);

            /*
             * Intersect with scissor clip if enabled.
             */
//always enabled or we wouldn't be here.
//            if (ctx->Scissor.Enabled)
            {
                BoxRec scissor;

//rudolf: testing:
//				scissor.x1 = pwin->drawable.x + ctx->Scissor.X;
//				scissor.y2 = pwin->drawable.y + pwin->drawable.height - ctx->Scissor.Y;
				scissor.x1 = ctx->Scissor.X;
				scissor.y2 = (uint16)(FrontBuffer.bottom - FrontBuffer.top + 1) - ctx->Scissor.Y;
				scissor.x2 = scissor.x1       + ctx->Scissor.Width;
				scissor.y1 = scissor.y2       - ctx->Scissor.Height;
                intersectRect.x1 = max(scissor.x1, intersectRect.x1);
                intersectRect.y1 = max(scissor.y1, intersectRect.y1);
                intersectRect.x2 = min(scissor.x2, intersectRect.x2);
                intersectRect.y2 = min(scissor.y2, intersectRect.y2);
            }

			//rudolf: relocate for frontbuffer rendering!
			AQUIRE_BEN(si->engine.lock)
			nv_acc_assert_fifo();

            /*
             * Set clip rect if there is a bounds-clip intersection.
             */
            if ((intersectRect.x1 != boundRect.x1)
             || (intersectRect.x2 != boundRect.x2)
             || (intersectRect.y1 != boundRect.y1)
             || (intersectRect.y2 != boundRect.y2))
            {
                /*
                 * Skip this triangle if clipped away.
                 */
//rud: fix if frontbuffer rendering!
//                if ((intersectRect.x1 >= intersectRect.x2) || (intersectRect.y1 >= intersectRect.y2))
//                    continue;
                RIVA_CLIP3D(FAKESNUM,intersectRect.x1, intersectRect.y1,
                            intersectRect.x2, intersectRect.y2);
                clipped = TRUE;
            }
            /*
             * Send triangle state if needed.
             */
//rudolf: fixme: do this whenever the 2D driver did a function!!!!!!!!!!!!!!!!!!!!!!<<<<<
            if (1)//riva_sinfo[FAKESNUM].rivaReload3D)
            {
                riva_sinfo[FAKESNUM].rivaReload3D = 0;
				//rudolf: due to a shortcoming in MESA3.2 we need to overwrite filtering related
				//stuff if no texturing is used (see riva_ctx.cpp, RivaUpdateState05())
	    	    if (!(ctx->Texture.Enabled) || !(ctx->Texture.Unit[0].CurrentD[2]))
				{
					rivaContext.texOffset = riva_sinfo[FAKESNUM].DefaultTexture->voffset;
					rivaContext.triContext.tri05.texFormat = 0x112215A1;
				}
                RIVA_STATE3D_05(FAKESNUM,rivaContext.triContext.tri05.texColorKey,
                                rivaContext.texOffset,
                                rivaContext.triContext.tri05.texFormat,
                                rivaContext.triContext.tri05.texFilter,
                                rivaContext.triContext.tri05.triBlend,
                                rivaContext.triContext.tri05.triControl,
                                rivaContext.triContext.tri05.triFogColor);
            }
            /*
             * Send vertex 0
             */
            RIVA_VERTEX3D_05(FAKESNUM,0,
                             x0,
                             y0,
                             VB->Win.data[v0][2] + ctx->PolygonZoffset,
                             m0,
                             argb0,
                             fog0,
                             VB->TexCoordPtr[0]->data[v0][0],
                             VB->TexCoordPtr[0]->data[v0][1]);
            /*
             * Send vertex 1
             */
            RIVA_VERTEX3D_05(FAKESNUM,1,
                             x1,
                             y1,
                             VB->Win.data[v1][2] + ctx->PolygonZoffset,
                             m1,
                             argb1,
                             fog1,
                             VB->TexCoordPtr[0]->data[v1][0],
                             VB->TexCoordPtr[0]->data[v1][1]);
            /*
             * Send vertex 2
             */
            RIVA_VERTEX3D_05(FAKESNUM,2,
                             x2,
                             y2,
                             VB->Win.data[v2][2] + ctx->PolygonZoffset,
                             m2,
                             argb2,
                             fog2,
                             VB->TexCoordPtr[0]->data[v2][0],
                             VB->TexCoordPtr[0]->data[v2][1]);
            RIVA_DRAWTRI3D_05(FAKESNUM,0, 1, 2);
        }
        /*
         * Reset clip rect if needed.
         */
        if (clipped)
            RIVA_CLIP3D(FAKESNUM,0, 0, 0x7FFF, 0x7FFF);

		RELEASE_BEN(si->engine.lock)
    }
}

/*
 * This is a workaround, because the mesa software rendering SHOULD
 * be working for this stuff, but isnt.
 * mesas render_bitmap() normally should handle this stuff.
 *
 * Also, it currently has problems in that it only draws to the
 * backbuffer. This FAILS HORRIBLY in singlebuffer mode. does nothing.
 * This is probably  because nv currently returns a doublebuffer context
 * even if user requests singlebuffer
 *
 * returns GL_FALSE if need software fallback (Mesa),
 *   or GL_TRUE if we handled it here.
 * TODO: moving window sometimes need refresh
 * Also... we probably should just return GL_FALSE if any special
 * effects like stencil, fog, antialiasing, etc are in
 * effect, since we dont handle that
 */
//rud: not used....?
/*
GLboolean RivaBitmap0
(
    GLcontext *ctx,
    GLint x,
    GLint y,
    GLsizei width,
    GLsizei height,
    const struct gl_pixelstore_attrib *unpack,
    const GLubyte *bitmap
)
{
//    XSMesaContext mesa_ctx  = (XSMesaContext) ctx->DriverCtx;
//    XSMesaBuffer mesabufinfo=mesa_ctx->xsm_buffer;
//rudolf: replace by GLH instead of this utahGLX var (disabled by me)
//    GLXImage *imageptr=mesabufinfo->backimage;
//rudolf:CARD32 == uint32
    uint32 pixelval=mesa_ctx->pixel;
    int windowwidth, windowheight;
    const GLubyte *bitmap_parse=bitmap;
    GLubyte bitmask;
    int xparse,yparse;


    fprintf(stderr,"DEBUG: RivaBitmap0 called: image=%p, db_state=%d, gl-x/x=%d/%d\n",
	    imageptr,mesabufinfo->db_state,
	    x,y);
    fprintf(stderr,"    width=%d,height=%d\n",
	    width,height);

//rudolf: disabled by me: fixme...

    if(imageptr==NULL){
	fprintf(stderr,"RivaBitmap: ERR: backimage==NULL\n");
	return GL_FALSE;
    }

//rudolf: set to 0 by me:
    windowwidth = 0;//imageptr->width;
    windowheight = 0;//imageptr->height;

    if(ctx->Visual->RGBAflag==0){
	pixelval=ctx->Current.RasterIndex;
    } else {
	GLint r,g,b,a;
	r = (GLint) (ctx->Current.RasterColor[0] * 255.0F);
	g = (GLint) (ctx->Current.RasterColor[1] * 255.0F);
	b = (GLint) (ctx->Current.RasterColor[2] * 255.0F);
	a = (GLint) (ctx->Current.RasterColor[3] * 255.0F);
//xsrud
//	pixelval=xsmesa_color_to_pixel(mesa_ctx,r,g,b,a);
    }

    if (riva_sinfo[FAKESNUM].rivaGLXEnabled == FALSE) {
//	fprintf(stderr, "RivaBitmap: called when glx disabled!!\n");
	return GL_TRUE;
    }

    destx = (GLint)ctx->Current.RasterPos[0] + 0.5f;
    desty = (GLint)ctx->Current.RasterPos[1] + 0.5f;

    // There are a few things to remember here:
    // - PutPixel takes coordinates in X11 window-relative values
    // - GL supplies y coordinates "upside down" to X11

//    destx=imageptr->width - x;
//    destx=x;
//    desty=imageptr->height - y;



    // almost identical to  mesa/src/bitmap.c:render_bitmap()
    // I didnt used to use the gl_pixel_addr_in_image,
    // but without it, it seems to only work sometimes.
    // Probably due to some kind of alignment issues.

    // XXX This is Inefficient.
    // should really use some kind of "span" routine,
    // since clipping checks are done in PutPixel.

    if(unpack->LsbFirst){
    bitmask=1;
    for(yparse=0; yparse<height; yparse++){
//	fprintf(stderr," RivaBitmap doing LsbFirst packing\n");

	    bitmap_parse=(const GLubyte*) gl_pixel_addr_in_image( unpack,
               bitmap, width, height, GL_COLOR_INDEX, GL_BITMAP, 0, yparse,0);

	    for(xparse=0; xparse <width; xparse++){

		if(*bitmap_parse & bitmask){
		//    RivaPutPixel(imageptr, x+xparse,windowheight-(y+yparse),
		//		      pixelval);
		}
		bitmask = bitmask <<1;
		if (bitmask == 0U) {
			bitmap_parse++;
			bitmask = 1U;
		}

	    }
	}
    } else {
	bitmask=0x80;
//	fprintf(stderr," RivaBitmap doing MsbFirst packing\n");
		// MsbFirst packing
	for(yparse=0; yparse<height; yparse++){

	    bitmap_parse=(const GLubyte*) gl_pixel_addr_in_image( unpack,
               bitmap, width, height, GL_COLOR_INDEX, GL_BITMAP, 0, yparse,0);

	    for(xparse=0; xparse <width; xparse++){

		if(*bitmap_parse & bitmask){
		  //  RivaPutPixel(imageptr, x+xparse,windowheight-(y+yparse),
			//	      pixelval);
		}
		bitmask = bitmask >>1;
		if (bitmask == 0U) {
			bitmap_parse++;
			bitmask = 0x80;
		}

	    }
	}
    }


    return GL_TRUE;
}
*/
