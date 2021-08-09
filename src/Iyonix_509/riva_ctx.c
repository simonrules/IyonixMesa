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

/* this is for sprintf() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <OS.h>

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
//#include "xsmesaP.h" /* has hacks for GC */

//#include "xf86.h"
//#include "xf86Priv.h"
//#include "xaa.h"
#include "../matrix.h"

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
#include "../vbxform.h"
#include "../dd.h"
/*
 * GLX includes.
 */
//#include "glx_config.h"
//#include "hwlog.h"
//#include "glx_log.h"
//#include "glx_symbols.h"
/*
 * Riva includes.
 */
//#include "compiler.h" /* for mem_barrier define */
#include "riva_glh.h"
#include "riva_symbols.h"
#include "riva_tex.h"

/*
 * Riva 3D context.
 */
RIVA_3D_CTX rivaContext;
/*
 * Sync flag for put/get pix.
 */
int rivaSyncPix;
/*
 * Blend equation conversion table.
 */
#define RIVA_SRCNORM_SRCA_DESTC_SRCC        0x00000100
#define RIVA_SRCNORM_SRCA_ZERO_SRCC         0x40000100
#define RIVA_SRCNORM_SRCA_DESTC_ZERO        0x80000100
#define RIVA_SRCNORM_SRCA_ZERO_ZERO         0xC0000100
#define RIVA_SRCNORM_DESTC_DESTC_SRCC       0x20000100
#define RIVA_SRCNORM_DESTC_ZERO_SRCC        0x60000100
#define RIVA_SRCNORM_DESTC_DESTC_ZERO       0xA0000100
#define RIVA_SRCNORM_ADD_WITH_SATURATION    0x10000000
#define RIVA_COLORINV_SRCA_DESTC_ZERO       0x80000500
#define RIVA_COLORINV_DESTC_DESTC_SRCC      0x20000500
#define RIVA_COLORINV_DESTC_ZERO_SRCC       0x60000500
#define RIVA_COLORINV_ADD_WITH_SATURATION   0x10000500
#define RIVA_ALPHAINV_SRCA_DESTC_SRCC       0x00000900
#define RIVA_ALPHAINV_SRCA_ZERO_SRCC        0x40000900
#define RIVA_ALPHAINV_SRCA_DESTC_ZERO       0x80000900
#define RIVA_ALPHAINV_DESTC_DESTC_SRCC      0x20000900
#define RIVA_ALPHA1_SRCA_DESTC_SRCC         0x00000D00
#define RIVA_ALPHA1_SRCA_ZERO_SRCC          0x40000D00
#define RIVA_SRCALPHA_ONE_4444              0x00000100
#define RIVA_BLEND_DEFAULT                  RIVA_ALPHA1_SRCA_ZERO_SRCC
#define RIVA_NO_BLEND_DEFAULT               RIVA_ALPHA1_SRCA_ZERO_SRCC
#define RIVA_NO_SECOND_PASS                 0xFFFFFFFF
unsigned long rivaBlendXlate[11][11][2] =
{
    {
        {RIVA_SRCNORM_SRCA_ZERO_ZERO,             RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_ZERO_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_ALPHAINV_SRCA_DESTC_ZERO,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_SRCA_DESTC_ZERO,            RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_SRCNORM_DESTC_ZERO_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_ZERO,           RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_SRCA_DESTC_ZERO,           RIVA_SRCNORM_DESTC_DESTC_ZERO},
    },
    {
        {RIVA_ALPHA1_SRCA_ZERO_SRCC,              RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_ZERO_SRCC,            RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_ALPHAINV_SRCA_DESTC_ZERO,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_SRCNORM_SRCA_DESTC_ZERO,            RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_SRCNORM_DESTC_DESTC_ZERO,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_ALPHAINV_SRCA_DESTC_SRCC},
    },
    {
        {RIVA_ALPHA1_SRCA_DESTC_SRCC,             RIVA_SRCNORM_DESTC_ZERO_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_SRCNORM_DESTC_ZERO_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_ALPHAINV_SRCA_DESTC_SRCC},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_ALPHAINV_SRCA_DESTC_SRCC,           RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_ALPHAINV_SRCA_DESTC_SRCC,           RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_SRCNORM_DESTC_ZERO_SRCC},
    },
    {
        {RIVA_ALPHA1_SRCA_DESTC_SRCC,             RIVA_COLORINV_DESTC_ZERO_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_COLORINV_ADD_WITH_SATURATION,       RIVA_SRCNORM_DESTC_ZERO_SRCC},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_COLORINV_DESTC_ZERO_SRCC},
        {RIVA_COLORINV_SRCA_DESTC_ZERO,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_SRCA_DESTC_ZERO,            RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_DESTC_DESTC_SRCC,          RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_DESTC_SRCC,          RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_SRCNORM_SRCA_DESTC_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_DESTC_DESTC_SRCC,          RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_DESTC_DESTC_SRCC},
    },
    {
        {RIVA_SRCNORM_SRCA_ZERO_SRCC,             RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_SRCA_DESTC_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_ZERO_SRCC,            RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_COLORINV_SRCA_DESTC_ZERO},
        {RIVA_SRCNORM_SRCA_DESTC_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_SRCNORM_SRCA_DESTC_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_ZERO,           RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_COLORINV_SRCA_DESTC_ZERO},
    },
    {
        {RIVA_ALPHAINV_SRCA_ZERO_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_SRCNORM_DESTC_ZERO_SRCC,            RIVA_ALPHAINV_SRCA_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_ALPHAINV_SRCA_DESTC_SRCC},
        {RIVA_ALPHAINV_SRCA_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_SRCNORM_SRCA_DESTC_ZERO},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_ALPHAINV_SRCA_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_ALPHAINV_SRCA_DESTC_SRCC},
        {RIVA_ALPHAINV_SRCA_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_ZERO,           RIVA_ALPHAINV_SRCA_DESTC_SRCC},
        {RIVA_SRCNORM_DESTC_ZERO_SRCC,            RIVA_ALPHAINV_SRCA_DESTC_SRCC},
    },
    {
        {RIVA_SRCNORM_DESTC_DESTC_ZERO,           RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_COLORINV_ADD_WITH_SATURATION,       RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_DESTC_SRCC,          RIVA_NO_SECOND_PASS},
        {RIVA_ALPHAINV_SRCA_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_SRCA_DESTC_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_SRCNORM_SRCA_DESTC_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_DESTC_DESTC_SRCC,          RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_SRCNORM_DESTC_ZERO_SRCC,            RIVA_SRCNORM_SRCA_DESTC_SRCC},
    },
    {
        {RIVA_SRCNORM_DESTC_DESTC_ZERO,           RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_DESTC_SRCC,          RIVA_NO_SECOND_PASS},
        {RIVA_ALPHAINV_SRCA_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_SRCA_DESTC_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_ALPHAINV_SRCA_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_DESTC_DESTC_SRCC,          RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_COLORINV_ADD_WITH_SATURATION,       RIVA_SRCNORM_DESTC_ZERO_SRCC},
    },
    {
        {RIVA_SRCNORM_DESTC_ZERO_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_ALPHAINV_DESTC_DESTC_SRCC,          RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_SRCC,           RIVA_NO_SECOND_PASS},
    },
    {
        {RIVA_COLORINV_ADD_WITH_SATURATION,       RIVA_SRCNORM_DESTC_DESTC_ZERO},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_ALPHA1_SRCA_ZERO_SRCC,              RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_COLORINV_DESTC_ZERO_SRCC},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_COLORINV_SRCA_DESTC_ZERO},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_SRCNORM_SRCA_DESTC_ZERO},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_SRCNORM_DESTC_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_ADD_WITH_SATURATION},
        {RIVA_ALPHAINV_SRCA_ZERO_SRCC,            RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_SRCNORM_DESTC_DESTC_ZERO,           RIVA_SRCNORM_SRCA_DESTC_SRCC},
    },
    {
        {RIVA_SRCNORM_SRCA_ZERO_SRCC,             RIVA_SRCNORM_DESTC_DESTC_ZERO},
        {RIVA_SRCNORM_SRCA_DESTC_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_ZERO_SRCC,            RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_DESTC_SRCC,          RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_ADD_WITH_SATURATION,        RIVA_COLORINV_SRCA_DESTC_ZERO},
        {RIVA_SRCNORM_SRCA_DESTC_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_COLORINV_DESTC_ZERO_SRCC,           RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_SRCNORM_SRCA_DESTC_SRCC,            RIVA_NO_SECOND_PASS},
        {RIVA_SRCNORM_DESTC_DESTC_ZERO,           RIVA_SRCNORM_SRCA_DESTC_SRCC},
        {RIVA_COLORINV_SRCA_DESTC_ZERO,           RIVA_SRCNORM_DESTC_DESTC_SRCC},
    }
};

/***************************************
 * Mesa's Driver Functions
 ***************************************/
static void RivaRenderStart( GLcontext *ctx)
{
	riva_sinfo[FAKESNUM].rivaReload3D = 1;
	return;
}

static void RivaRenderFinish( GLcontext *ctx)
{
	riva_sinfo[FAKESNUM].rivaReload3D = 1;
	return;
}

static void RivaNearFar( GLcontext *ctx, GLfloat nearVal, GLfloat farVal )
{
#define INVWMAX 128.0F

    rivaContext.InvWScale = 1.0;
    if (nearVal > 0) {
        int exp2;
        if (frexp(INVWMAX * nearVal,&exp2) != 0) {
            if (exp2 >= 2) {
                rivaContext.InvWScale = 1 << (exp2-1);
            } else if (exp2 <= 0) {
                rivaContext.InvWScale = 1.0 / (1 << (-exp2+1));
            }
        }
    }
}


/*
 * String identifier.
 */
static const char *RivaString(void)
{
    static char string[20];
    sprintf(string, "BeOS UtahGLX nVidia driver 0.1\n");
    return (string);
}
/*
 * Parameters
 */
static GLint RivaGetParameteri(const GLcontext *ctx, GLint param)
{
    switch (param) {

    case DD_HAVE_HARDWARE_FOG:
        return 1;
    default:
        return -1; /* Should I really return 0? */
    }
}
/*
GLboolean RivaSetBuffer ( GLcontext *ctx, GLenum mode )
{
//    XSMesaContext xsmesa = (XSMesaContext) ctx->DriverCtx;

//	LOG(2,("riva_ctx/RivaSetBuffer: called\n"));

//    if (mode == GL_FRONT || !xsmesa->xsm_buffer->db_state)
    {
        // Set front buffer rendering.
//        xsmesa->xsm_buffer->buffer = xsmesa->xsm_buffer->frontbuffer;
//rudolf: done via GLView:
//        xsmesa_setup_DD_pointers(ctx);
//        RivaUpdateState05(ctx);
        return ((mode == GL_FRONT) ? GL_TRUE : GL_FALSE);
    }
    else
    {
        // Set back buffer rendering.
 //       if (xsmesa->xsm_buffer->backpixmap)
        {
//            xsmesa->xsm_buffer->buffer = (DrawablePtr)xsmesa->xsm_buffer->backpixmap;
        }
//rudolf: disabled: should not be needeD?
//        else if (xsmesa->xsm_buffer->backimage)
  //      {
    //        xsmesa->xsm_buffer->buffer = None;
      //  }
//rudolf: done via GLView:
//        xsmesa_setup_DD_pointers(ctx);
 //       RivaUpdateState05(ctx);
        return (GL_TRUE);
    }
}
*/

/*
 * Pipeline flush stuff.
 */
static void RivaFinish ( GLcontext *ctx )
{
    RivaSync(FAKESNUM);
}
static void RivaFlush ( GLcontext *ctx )
{
    RivaSync(FAKESNUM);
}

/*
 * Buffer operations.
 */
/* clear buffer */
//rudolf: from ddsample.c:
/*
static GLbitfield clear( GLcontext *ctx, GLbitfield mask, GLboolean all,
                        GLint x, GLint y, GLint width, GLint height )
{
   struct foo_mesa_context *foo = (struct foo_mesa_context *) ctx->DriverCtx;

 // Clear the specified region of the buffers indicated by 'mask'
 // using the clear color or index as specified by one of the two
 // functions above.
 // If all==GL_TRUE, clear whole buffer, else just clear region defined
 // by x,y,width,height


   return mask;  // return mask of buffers remaining to be cleared
}
*/
//rud: Mesa fallback:
//void gl_clear_depth_buffer( GLcontext* ctx )
//BTW:
//looks like this clear routine is more expensive than software clearing!!???
//checkout with real HWacc, and checkout 2D fill_rect function use if possible
GLbitfield RivaClearBuffers05//rud: Z-clr, Quake OK in 16bit color, 32bit has artifacts...
(
    GLcontext *ctx,
    GLbitfield mask,
    GLboolean all,
    GLint x,
    GLint y,
    GLint width,
    GLint height
)
//rud: NOTE! o,o is left-top for frontbuffer, but left-bottom for z-buffer in HW!!
{
    int           nclipRects, red, green, blue, color24, color16, control, blend, clearmask;
//rudtst: (see fallback)
    GLfloat       depth, m, uv;
    BoxPtr        pclipRect;
    BoxRec        clearRect, intersectRect;
//    XSMesaContext xsmesa = (XSMesaContext)ctx->DriverCtx;
//rudolf: windowPtr has more stuff than DrawableRec, but it begins with DrawableRec...
//        so what does that mean for the additional stuff? filled out 'auto' via cast???
//    WindowPtr     pwin   = (WindowPtr)(xsmesa->xsm_buffer->frontbuffer);

    RivaFloatToUInt(red,   ctx->Color.ClearColor[2] * 255.0F); // SLW: R/B swap
    RivaFloatToUInt(green, ctx->Color.ClearColor[1] * 255.0F);
    RivaFloatToUInt(blue,  ctx->Color.ClearColor[0] * 255.0F); // SLW: R/B swap

    if (red   > 0xFF) red   = 0xFF;
    if (green > 0xFF) green = 0xFF;
    if (blue  > 0xFF) blue  = 0xFF;
    color24   = 0xFF000000 | (red << 16) | (green << 8) | blue;
    color16   = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
//rud: disabled both DEPTH_SCALE and DEPTH_UNSCALE as we are in fixed depth!!
//rud: here OK, but real acc messes up of course (as depth as set here is not use: doh!)
//rudtst: (sets cleardepth)
    depth     = ctx->Depth.Clear * DEPTH_SCALE;//rud: mesa define, can be user-set!!
    blend     = 0x21100162;
/*rud: control:
0x40186800;
 b8-11=ALPHAFUNC_ALWAYS
 b13=ORIGIN_CORNER
 b14=ZENABLE_TRUE
 b19=ZFUNC_ALWAYS
 b20=CULLMODE_NONE
 b30,31=Z_FORMAT_FIXED (float hangs sys!)
*/
    control   = 0x40186800;
    clearmask = 0;
    m = 1.0f;
    uv = 0.0f;

    if (mask & GL_COLOR_BUFFER_BIT)
    {
        blend &= 0x00FFFFFF;
        blend |= 0x12000000;
        clearmask |= GL_COLOR_BUFFER_BIT;
    }
    if (mask & GL_DEPTH_BUFFER_BIT)
    {
        control   |= 0x01000000;
        clearmask |= GL_DEPTH_BUFFER_BIT;
    }
	/* block rendering while a mode-change is being executed! */
	if (si->mode_changed) return (mask ^ clearmask);

/*    if (sizeof(GLdepth)==2)
		LOG(2,("RivaClearBuffers05: depth is 16 bits\n"));
	else
		LOG(2,("RivaClearBuffers05: depth != 16 bits\n"));
*/
//rudolf: must be an error??????????
//    if (riva_sinfo[FAKESNUM].rivaGLXEnabled)
    {
        /*
         * Get window clip rects.
         */
//rudolf: probably fix..
//rudolf: probably relocate..
/*
	OutWindowPtr = myView->Window();
	OutWindow = OutWindowPtr->Frame();
	LOG(2,("RivaClearBuffers05: left-top is %f,%f; right-bottom is %f,%f\n",
		OutWindow.left, OutWindow.top, OutWindow.right, OutWindow.bottom));
	myView->LockLooper();
	myView->GetClippingRegion(&OutWinRegion);
	NumOutRegs = OutWinRegion.CountRects();
	if (NumOutRegs)
	{
		BRect b = OutWinRegion.RectAt(0);
	LOG(2,("RivaClearBuffers05: left-top is %f,%f; right-bottom is %f,%f\n",
		b.left, b.top, b.right, b.bottom));
	}
	myView->UnlockLooper();
*/


/*        if (pwin->clipList.data)
        {
            if (pwin->clipList.data->numRects == 0)
                return (mask ^ clearmask);
            else
            {
                nclipRects = pwin->clipList.data->numRects;
                pclipRect  = (BoxPtr)(pwin->clipList.data + 1);/rudolf: list of rects..
            }
        }
        else
 */       {
            nclipRects = 1;
            //pclipRect  = &pwin->clipList.extents; //rudolf: .. but just one rect here.
        }
        if (all) //clear whole buffer! (only window for windowed apps! (speed!!!)
        {
			LOG(2,("RivaClearBuffers05: clear all requested\n"));
//rudolf: pwin->drawable.x and y description: /* window: screen absolute, pixmap: 0 */
//rudolf: assuming 0..
            clearRect.x1 = 0;//pwin->drawable.x;
            clearRect.y1 = 0;//pwin->drawable.y;
//rudolf: testing..
//            clearRect.x2 = clearRect.x1 + pwin->drawable.width;
//            clearRect.y2 = clearRect.y1 + pwin->drawable.height;
//rudolf: probably modify to be exactly the frontbuffer's (window) size????
            clearRect.x2 = clearRect.x1 + si->dm.virtual_width;
            clearRect.y2 = clearRect.y1 + si->dm.virtual_height;
        }
        else //clear x,y,width,height as specified to this routine!
        {
			LOG(2,("RivaClearBuffers05: clear box requested\n"));
//rudolf: testing..
//            clearRect.x1 = pwin->drawable.x                         + x;
//            clearRect.y2 = pwin->drawable.y + pwin->drawable.height - y;
//            clearRect.x2 = clearRect.x1 + width;
//            clearRect.y1 = clearRect.y2 - height;
            clearRect.x1 = 0 + x;
            clearRect.y2 = 0 + si->dm.virtual_height - y;
            clearRect.x2 = clearRect.x1 + width;
            clearRect.y1 = clearRect.y2 - height;
        }

        /*
         * Intersect with scissor clip if enabled.
         */
        if (ctx->Scissor.Enabled)
        {
			LOG(2,("RivaClearBuffers05: Scissor is enabled\n"));

            BoxRec scissor;

//rudolf: testing...
//            scissor.x1 = pwin->drawable.x + ctx->Scissor.X;
//            scissor.y2 = pwin->drawable.y + pwin->drawable.height - ctx->Scissor.Y;
//            scissor.x2 = scissor.x1       + ctx->Scissor.Width;
//            scissor.y1 = scissor.y2       - ctx->Scissor.Height;
            scissor.x1 = 0 + ctx->Scissor.X;
            scissor.y2 = 0 + si->dm.virtual_height - ctx->Scissor.Y;
            scissor.x2 = scissor.x1       + ctx->Scissor.Width;
            scissor.y1 = scissor.y2       - ctx->Scissor.Height;
            clearRect.x1 = max(scissor.x1, clearRect.x1);
            clearRect.y1 = max(scissor.y1, clearRect.y1);
            clearRect.x2 = min(scissor.x2, clearRect.x2);
            clearRect.y2 = min(scissor.y2, clearRect.y2);
        }
        /*
         * Set 3D state reload flag (but only for the first time).
         */
        riva_sinfo[FAKESNUM].rivaReload3D = 1;
        while (nclipRects--)
        {
            /*
             * Clear through clip rect.
             */
//rudolf: fix
/*
            intersectRect.x1 = max(pclipRect->x1, clearRect.x1);
            intersectRect.y1 = max(pclipRect->y1, clearRect.y1);
            intersectRect.x2 = min(pclipRect->x2, clearRect.x2);
            intersectRect.y2 = min(pclipRect->y2, clearRect.y2);
*/
//rudtst:
            intersectRect.x1 = clearRect.x1;
            intersectRect.y1 = clearRect.y1;
            intersectRect.x2 = clearRect.x2;
            intersectRect.y2 = clearRect.y2;

            intersectRect.x1 = 0;
            intersectRect.y1 = 0;
//rud: this also accounts for slopspace :)
            intersectRect.x2 = (uint16)(FrontBuffer.right - FrontBuffer.left + 1);
            intersectRect.y2 = (uint16)(FrontBuffer.bottom - FrontBuffer.top + 1);
//end rudtst.

            if ((intersectRect.x1 < intersectRect.x2) && (intersectRect.y1 < intersectRect.y2))
            {
                /*
                 * Send quad to clear buffers.
                 */
				//rudolf: aquire and assert here! (3D cmd's may not be interrupted!)
				AQUIRE_BEN(si->engine.lock)
				nv_acc_assert_fifo();
//rudolf: fixme: do this whenever the 2D driver did a function!!!!!!!!!!!!!!!!!!!!!!<<<<<
                if (riva_sinfo[FAKESNUM].rivaReload3D)
                {
                    riva_sinfo[FAKESNUM].rivaReload3D = 0;
                    RIVA_STATE3D_05(FAKESNUM,0x00000000,
                                    riva_sinfo[FAKESNUM].textureoffset,
                                    0x11221551,
                                    0x11000000,
                                    blend,
                                    control,
                                    0x00000000);
                }
                RIVA_VERTEX3D_05(FAKESNUM,0, intersectRect.x1, intersectRect.y1, depth, m, color24, 0x00000000, uv, uv);
                RIVA_VERTEX3D_05(FAKESNUM,1, intersectRect.x2, intersectRect.y1, depth, m, color24, 0x00000000, uv, uv);
                RIVA_VERTEX3D_05(FAKESNUM,2, intersectRect.x2, intersectRect.y2, depth, m, color24, 0x00000000, uv, uv);
                RIVA_VERTEX3D_05(FAKESNUM,3, intersectRect.x1, intersectRect.y2, depth, m, color24, 0x00000000, uv, uv);
//                RIVA_DRAWTRI3D_05(FAKESNUM,0, 1, 2);
//                RIVA_DRAWTRI3D_05(FAKESNUM,0, 2, 3);
				RIVA_DRAWQUAD3D_05(FAKESNUM,0, 1, 2, 3);
				//rudolf: release here!
				RELEASE_BEN(si->engine.lock)
            }
            pclipRect++;
        }
        /*
         * Set 3D state reload flag.
         */
         //rudolf: do wait_idle when this flag is checked in code:
        riva_sinfo[FAKESNUM].rivaReload3D = rivaSyncPix = 1;
    }
    /*
     * This should return the original mask, minus the cleared buffer bits.
     */
//rudtst: (should this be moved to drawing/reading pixels etc in BGLView???) YES!
        if (0)//rivaSyncPix)
        {
			AQUIRE_BEN(si->engine.lock)
            rivaSyncPix  = 0;
            RivaSync(FAKESNUM);
			RELEASE_BEN(si->engine.lock)
        }
//end rudtst:

    return (mask ^ clearmask);
}

//rud:
cmd_nv_rop5_solid* nv_rop5_solid_ptr;
cmd_nv_image_pattern* nv_image_pattern_ptr;
cmd_nv_image_blit* nv_image_blit_ptr;

/* screen to screen blit - i.e. move windows around and scroll within them. */
//#if 0
status_t nv_acc_setup_blit()
{

//rud, tst:
	/* update our local pointers */
	nv_rop5_solid_ptr = (cmd_nv_rop5_solid*)
		&(regs[(si->engine.fifo.ch_ptr[NV_ROP5_SOLID]) >> 2]);

	nv_image_pattern_ptr = (cmd_nv_image_pattern*)
		&(regs[(si->engine.fifo.ch_ptr[NV_IMAGE_PATTERN]) >> 2]);

	nv_image_blit_ptr = (cmd_nv_image_blit*)
		&(regs[(si->engine.fifo.ch_ptr[NV_IMAGE_BLIT]) >> 2]);
//

	/* setup solid pattern:
	 * wait for room in fifo for pattern cmd if needed.
	 * (fifo holds 256 32bit words: count those, not bytes) */
	while (((nv_image_pattern_ptr->FifoFree) >> 2) < 5)
	{
		/* snooze a bit so I do not hammer the bus */
		snooze (10);
	}
	/* now setup pattern (writing 5 32bit words) */
	nv_image_pattern_ptr->SetShape = 0x00000000; /* 0 = 8x8, 1 = 64x1, 2 = 1x64 */
	nv_image_pattern_ptr->SetColor0 = 0xffffffff;
	nv_image_pattern_ptr->SetColor1 = 0xffffffff;
	nv_image_pattern_ptr->SetPattern[0] = 0xffffffff;
	nv_image_pattern_ptr->SetPattern[1] = 0xffffffff;

	/* ROP registers (Raster OPeration):
	 * wait for room in fifo for ROP cmd if needed.
	 * (fifo holds 256 32bit words: count those, not bytes) */
	while (((nv_rop5_solid_ptr->FifoFree) >> 2) < 1)
	{
		/* snooze a bit so I do not hammer the bus */
		snooze (10);
	}
	/* now setup ROP (writing 1 32bit word) */
	nv_rop5_solid_ptr->SetRop5 = 0xcc;

	return B_OK;
}

status_t nv_acc_blit(uint16 xs,uint16 ys,uint16 xd,uint16 yd,uint16 w,uint16 h)
{
	/* Note: blit-copy direction is determined inside riva hardware: no setup needed */

	/* instruct engine what to blit:
	 * wait for room in fifo for blit cmd if needed.
	 * (fifo holds 256 32bit words: count those, not bytes) */
	while (((nv_image_blit_ptr->FifoFree) >> 2) < 3)
	{
		/* snooze a bit so I do not hammer the bus */
		snooze (10);
	}
	/* now setup blit (writing 3 32bit words) */
	nv_image_blit_ptr->SourceOrg = ((ys << 16) | xs);
	nv_image_blit_ptr->DestOrg = ((yd << 16) | xd);
	nv_image_blit_ptr->HeightWidth = (((h + 1) << 16) | (w + 1));

	return B_OK;
}
//#endif
/*
	setup engine for 'dual' surfaces, do 2D blits, and reset engine for 'single'
	surface. (do clipped back to front colorbuffer blit)
*/
void RivaSwapBuffersIyonix()
{
	nv_acc_assert_fifo();

	/* Switch to 'dual' buffer surfaces */
	riva_sinfo[FAKESNUM].SetSurfaces2D(
		si->fbc.bytes_per_row,
		si->fbc.bytes_per_row,
		riva_sinfo[FAKESNUM].frontbufferoffset,
		riva_sinfo[FAKESNUM].backbufferoffset);

	//printf("riva_sinfo[FAKESNUM].frontbufferoffset: %x\n", riva_sinfo[FAKESNUM].frontbufferoffset);
	//printf("riva_sinfo[FAKESNUM].backbufferoffset: %x\n", riva_sinfo[FAKESNUM].backbufferoffset);

	/* doing 2D blits inside of 3D driver */
	nv_acc_setup_blit();

	nv_acc_blit(0, 0, 0, 0, si->dm.virtual_width, si->dm.virtual_height); // SLW-TODO: Fix for rendering in a window

	/* Sync get/put pixel with HW. */
	rivaSyncPix = 1;

	/* Switch back to 'single' buffer surface */
	riva_sinfo[FAKESNUM].SetSurfaces2D(
		si->fbc.bytes_per_row,
		si->fbc.bytes_per_row,
		riva_sinfo[FAKESNUM].frontbufferoffset,
		riva_sinfo[FAKESNUM].frontbufferoffset);
}

/*
	setup engine for 'dual' surfaces, do 2D blits, and reset engine for 'single'
	surface. (do clipped back to front colorbuffer blit)
*/
#if 0
void RivaSwapBuffersDirect()
{
	uint16 zpitch = (uint16)(FrontBuffer.right - FrontBuffer.left + 1);

	/* block rendering while a mode-change is being executed! */
	if (si->mode_changed) return;

	switch(si->dm.space)
	{
	case B_RGB32_LITTLE:
//rud: set 32bit Z, works more or less..?
		zpitch = ((zpitch * 4) + 0x3f) & ~0x3f;//we only support 16bit depth..
		break;
	default:
		zpitch = ((zpitch * 2) + 0x3f) & ~0x3f;
		break;
	}

	AQUIRE_BEN(si->engine.lock)
	nv_acc_assert_fifo();

	/* Switch to 'dual' buffer surfaces */
	riva_sinfo[FAKESNUM].SetSurfaces2D(
		si->fbc.bytes_per_row,
		zpitch,
		riva_sinfo[FAKESNUM].frontbufferoffset,
		riva_sinfo[FAKESNUM].backbufferoffset);

	/* doing 2D blits inside of 3D driver */
	nv_acc_setup_blit();

	if (fNumClipRects)
	{
		LOG(2,("RivaSwapBuffersDirect: cliplist count %d\n", fNumClipRects));
		LOG(2,("window_bounds is %d,%d;%d,%d\n",
			fWindowBounds.left, fWindowBounds.top,
			fWindowBounds.right, fWindowBounds.bottom));

		for (uint32 cnt = 0; cnt < fNumClipRects; cnt++)
		{
			LOG(2,("cliplist[%d] is %d,%d;%d,%d\n", cnt,
				fClipList[cnt].left, fClipList[cnt].top,
				fClipList[cnt].right, fClipList[cnt].bottom));

			nv_acc_blit(
				(uint16)fClipList[cnt].left - fWindowBounds.left,
				(uint16)fClipList[cnt].top - fWindowBounds.top - menu_offset,
				(uint16)(fClipList[cnt].left),
				(uint16)(fClipList[cnt].top),
				(uint16)(fClipList[cnt].right - fClipList[cnt].left),
				(uint16)(fClipList[cnt].bottom - fClipList[cnt].top));
		}
	}
	else
	{
		LOG(2,("RivaSwapBuffersDirect: no cliplist!\n"));
	}

	/* Sync get/put pixel with HW. */
	rivaSyncPix = 1;

	/* Switch back to 'single' buffer surface */
	riva_sinfo[FAKESNUM].SetSurfaces2D(
		si->fbc.bytes_per_row,
		si->fbc.bytes_per_row,
		riva_sinfo[FAKESNUM].frontbufferoffset,
		riva_sinfo[FAKESNUM].frontbufferoffset);

//rudolf: do this in bits read/write functions instead!!
	if (0)//rivaSyncPix)
	{
		rivaSyncPix  = 0;
		RivaSync(FAKESNUM);
	}
	RELEASE_BEN(si->engine.lock)
}
#endif

/*
	setup engine for 'dual' surfaces, do 2D blits, and reset engine for 'single'
	surface. (do clipped back to front colorbuffer blit)
*/
#if 0
void RivaSwapBuffersIndirect()
{
	uint16 zpitch = (uint16)(FrontBuffer.right - FrontBuffer.left + 1);

	/* block rendering while a mode-change is being executed! */
	if (si->mode_changed) return;

	switch(si->dm.space)
	{
	case B_RGB32_LITTLE:
//rud: set 32bit Z, works more or less..?
		zpitch = ((zpitch * 4) + 0x3f) & ~0x3f;//we only support 16bit depth..
		break;
	default:
		zpitch = ((zpitch * 2) + 0x3f) & ~0x3f;
		break;
	}

	AQUIRE_BEN(si->engine.lock)
	nv_acc_assert_fifo();

	/* Switch to 'dual' buffer surfaces */
	riva_sinfo[FAKESNUM].SetSurfaces2D(
		si->fbc.bytes_per_row,
		zpitch,
		riva_sinfo[FAKESNUM].frontbufferoffset,
		riva_sinfo[FAKESNUM].backbufferoffset);

	/* doing 2D blits inside of 3D driver */
	nv_acc_setup_blit();

	if (uint16 rects = dvi.CountRects())
	{
		LOG(2,("RivaSwapBuffersIndirect: (hack) cliplist count %d\n", rects));

		LOG(2,("window_bounds is %d,%d;%d,%d\n",
			fWindowBounds.left, fWindowBounds.top,
			fWindowBounds.right, fWindowBounds.bottom));

		for (uint16 cnt = 0; cnt < rects; cnt++)
		{
			/* HACK alert!! BDirectWindow doesn't have a clip_list in non-direct
			 * mode: using View cliplist. */
			BRect clip = dvi.RectAt(cnt);

			LOG(2,("cliplist[%d] is %f,%f;%f,%f\n", cnt,
				clip.left, clip.top, clip.right, clip.bottom));

			nv_acc_blit(
				(uint16)clip.left,
				(uint16)clip.top,
				(uint16)(WindowFrame.left + clip.left + FrontBuffer.left),
				(uint16)(WindowFrame.top + clip.top + FrontBuffer.top),
				(uint16)(clip.right - clip.left),
				(uint16)(clip.bottom - clip.top));
		}
	}
	else
	{
		LOG(2,("RivaSwapBuffersIndirect: no cliplist!\n"));
	}

	/* Sync get/put pixel with HW. */
	rivaSyncPix = 1;

	/* Switch back to 'single' buffer surface */
	riva_sinfo[FAKESNUM].SetSurfaces2D(
		si->fbc.bytes_per_row,
		si->fbc.bytes_per_row,
		riva_sinfo[FAKESNUM].frontbufferoffset,
		riva_sinfo[FAKESNUM].frontbufferoffset);

//rudolf: do this in bits read/write functions instead!!
	if (0)//rivaSyncPix)
	{
		rivaSyncPix  = 0;
		RivaSync(FAKESNUM);
	}
	RELEASE_BEN(si->engine.lock)
}
#endif

/*
 * Hook into Mesa GL context.
 */
#define RIVA_GET_BLEND_FUNC(hf, bf)                 \
{                                                   \
    if ((bf == GL_ONE) || (bf == GL_ZERO))          \
        hf = bf + 1;                                \
    else                                            \
        hf = bf - GL_SRC_COLOR + 3;                 \
}

/*
 * This routine is primarily called by the Mesa back-end, through
 * ctx->Driver.UpdateState()
 *  Called by gl_update_state(), which if you believe the comments in
 * Mesa, gets called if ctx->NewState !=0, before drawing a primitive.
 *
 * I dont understand why we reset the damn ctx->Driver.func()
 * hooks here. but apparently, we do.
 *
 * Additionally, we do a LOT of stuff....
 * - Set our rivaContext.texObj based on ctx->Texture.Unit[0]
 *   (or set card to use DefaultTexture, if need be)
 *   - May call RivaBindTexture if ctx->Texture.Unit[].xxx does not
 *     match rivaContext.texObj !!
 * - Compare ctx.Color.DrawBuffer to rivaContext.bufRender, and switch
 *   our bufRender if needed.
 * - do a fog thing if needed
 * - set rivaContext.SimpleClip, if not doublebuffered or something.
 *
 * Set riva_sinfo[].rivaReload3D=1 for some reason.
 *
 * We should really only do this stuff if it NEEDS updating,
 * IMO. But since the original author wrote it that way,
 * I'm not gutsy enough to stop doing it for now. -- Phil
 *
 */
void RivaUpdateState05(GLcontext *ctx)
{
    int src, dst;

//	LOG(2,("riva_ctx/RivaUpdateState05: entered\n"));

//rudolf: added from now disabled routine:
//void xsmesa_setup_DD_pointers( GLcontext *ctx )

//rudolf: these three _must_ go together!!! (all or nothing)
	    ctx->Driver.AllocDepthBuffer  = RivaCreateDepthBuffer;
//testing (modified copies from utahGLX):
		  ctx->Driver.DepthTestSpan = depth_test_span;
		  ctx->Driver.DepthTestPixels = depth_test_pixels;

//rudolf: literal copy from mesa 3.2's context.c:
/*
		if (ctx->Depth.Mask)
		{
			LOG(2,("RivaUpdateState05: Depth mask set\n"));
			switch (ctx->Depth.Func)
		    {
			case GL_LESS:
				LOG(2,("RivaUpdateState05: Depth func is GL_LESS\n"));
				ctx->Driver.DepthTestSpan = gl_depth_test_span_less;
				ctx->Driver.DepthTestPixels = gl_depth_test_pixels_less;
				break;
			case GL_GREATER:
				LOG(2,("RivaUpdateState05: Depth func is GL_GREATER\n"));
				ctx->Driver.DepthTestSpan = gl_depth_test_span_greater;
				ctx->Driver.DepthTestPixels = gl_depth_test_pixels_greater;
				break;
			default:
				LOG(2,("RivaUpdateState05: default exec for depth func\n"));
				ctx->Driver.DepthTestSpan = gl_depth_test_span_generic;
				ctx->Driver.DepthTestPixels = gl_depth_test_pixels_generic;
			}
		}
		else
		{
			LOG(2,("RivaUpdateState05: No depth mask set\n"));
			ctx->Driver.DepthTestSpan = gl_depth_test_span_generic;
			ctx->Driver.DepthTestPixels = gl_depth_test_pixels_generic;
		}
*/
    if (!riva_sinfo[FAKESNUM].rivaGLXEnabled)
    {
		LOG(2,("RivaUpdateState05: GLX is disabled!\n"));

        /*
         * Reset basic driver hooks to use NOP routines.
         */
//rudolf: GLteapot not influenced by killing this one..
//        ctx->Driver.PointsFunc        = RivaPointsNOP;
//rudolf: GLteapots FPS display works via this:
//        ctx->Driver.LineFunc          = RivaLineNOP;
//rudolf: GLteaports rendering of teapot works via this:
//        ctx->Driver.TriangleFunc      = RivaTriangleNOP;
/*        xsmesa_setup_DD_pointers(ctx); */
//rud: leaving preset GLView stuff (which calls this routine now)
//       ctx->Driver.UpdateState       = RivaUpdateState05;
        ctx->Driver.RendererString    = RivaString;
//rud: rest of NULL fu's off fixes the touch_teapot fault..
/*
        ctx->Driver.Flush             = NULL;
        ctx->Driver.Finish            = NULL;
        ctx->Driver.NearFar           = NULL;
*/

//quake: not yet ok (crash on blend_modulate)
//        ctx->Driver.SetBuffer         = RivaSetBuffer;

//called from BGLView driver:
//        ctx->Driver.Clear             = RivaClearBuffers05;
/*
        ctx->Driver.TexImage          = NULL;
        ctx->Driver.TexSubImage       = NULL;
        ctx->Driver.DeleteTexture     = NULL;
        ctx->Driver.BindTexture       = NULL;
        ctx->Driver.RenderStart       = NULL;
        ctx->Driver.RenderFinish      = NULL;
        ctx->Driver.RenderVBRawTab    = NULL;
        ctx->Driver.RenderVBCulledTab = NULL;
		ctx->Driver.Bitmap            = NULL;
*/
//rudolf, added: but no crash influence .. (but grabbing pot doesn't work then)
//        ctx->Driver.QuadFunc = NULL;


//rudolf: testing: (assuming NV04 arch accepts this as well, instead of NV3 style!!!)
		riva_sinfo[FAKESNUM].SetSurfaces3D(
			((rivaContext.bufRender == GL_FRONT) ?
			riva_sinfo[FAKESNUM].frontbufferoffset :
			riva_sinfo[FAKESNUM].backbufferoffset),
			riva_sinfo[FAKESNUM].depthbufferoffset);

		return;
    }

	LOG(2,("RivaUpdateState05: GLX is enabled, inserting HW hooks\n"));
	/*
     * Reset basic driver hooks.
     */
    ctx->Driver.PointsFunc        = RivaPoints3D05;
//    ctx->Driver.PointsFunc        = RivaPointsNOP;
    ctx->Driver.LineFunc          = RivaLine3D05;
//    ctx->Driver.LineFunc          = RivaLineNOP;
    ctx->Driver.TriangleFunc      = RivaTriangle3D05;
//    ctx->Driver.TriangleFunc      = RivaTriangleNOP;

//rud: leaving preset GLView stuff (which calls this routine now)
//    ctx->Driver.UpdateState       = RivaUpdateState05;
    ctx->Driver.RendererString    = RivaString;
    ctx->Driver.Flush             = RivaFlush;
    ctx->Driver.NearFar           = RivaNearFar;
    ctx->Driver.Finish            = RivaFinish;
//rud: not needed?? (doing BGLView instead)
//    ctx->Driver.SetBuffer         = RivaSetBuffer;
//called from BGLView driver:
//        ctx->Driver.Clear             = RivaClearBuffers05;
    ctx->Driver.TexImage          = RivaTexImage;
    ctx->Driver.TexSubImage       = RivaTexSubImage;
    ctx->Driver.DeleteTexture     = RivaDeleteTexture;
    ctx->Driver.BindTexture       = RivaBindTexture;
    ctx->Driver.GetParameteri     = RivaGetParameteri;
    ctx->Driver.RenderStart	      = RivaRenderStart;
    ctx->Driver.RenderFinish      = RivaRenderFinish;

    /*
     * Unsupported render modes.  Bail to software rendering.
     */
//rud: added check for GL_FRONT for now..
    if (ctx->Stencil.Enabled || ctx->Line.StippleFlag || ctx->Polygon.StippleFlag ||
    	(ctx->Color.DrawBuffer == GL_FRONT))
    {
		LOG(2,("RivaUpdateState05: unsupported render mode, fallback to SW rendering\n"));
        /*
         * Bail to SW rendering.
         * Mesa will fill in generic Funcs.
         */
        ctx->Driver.PointsFunc        = NULL;
        ctx->Driver.LineFunc          = NULL;
        ctx->Driver.TriangleFunc      = NULL;
        ctx->Driver.QuadFunc          = NULL;
        ctx->Driver.RectFunc          = NULL;
        ctx->Driver.RenderStart       = NULL;
        ctx->Driver.RenderFinish      = NULL;
        ctx->Driver.RenderVBRawTab    = NULL;
	  	ctx->Driver.RenderVBCulledTab = NULL;
//rud: leaving preset GLView stuff (which calls this routine now)
//        ctx->Driver.UpdateState       = RivaUpdateState05;
        return;
    }

	/* Now for hardware register updating. we do this a lot */

        rivaContext.triContext.tri05.triBlend   = 0x00000120;
        rivaContext.triContext.tri05.triControl = 0x40100000;
        /*
         * Blending state.
         */
        if (ctx->Color.BlendEnabled)
        {
            RIVA_GET_BLEND_FUNC(src, ctx->Color.BlendSrcRGB);
            RIVA_GET_BLEND_FUNC(dst, ctx->Color.BlendDstRGB);
            rivaContext.triContext.tri05.triBlend |= (dst << 28) | (src << 24) | 0x00100000;
        }
        else
            rivaContext.triContext.tri05.triBlend |= 0x12000000;
        /*
         * Shading.
         */
        if (ctx->Light.ShadeModel == GL_FLAT)
            rivaContext.triContext.tri05.triBlend |= 0x00000040;
        else
            rivaContext.triContext.tri05.triBlend |= 0x00000080;
        /*
         * Alpha test.
         */
        if (ctx->Color.AlphaEnabled)
            rivaContext.triContext.tri05.triControl |= ctx->Color.AlphaRef | (((ctx->Color.AlphaFunc & 0x07) + 1) << 8) | 0x00001000;
        else
            rivaContext.triContext.tri05.triControl |= 0x00000800;
        /*
         * Depth test.
         */
        if (ctx->Depth.Test)
        {
            rivaContext.triContext.tri05.triControl |= (((ctx->Depth.Func & 0x07) + 1) << 16)
                                                    |   ((ctx->Depth.Mask) ? 0x01000000 : 0x00000000)
                                                    |  0x00004000;
        }
        else
            rivaContext.triContext.tri05.triControl |= 0x00080000;
        /*
         * Dithering.
         */
        rivaContext.triContext.tri05.triControl |= (ctx->Color.DitherFlag) ? 0x00400000 : 0x00000000;
        /*
         * Texturing.
         */
        if (ctx->Texture.Enabled && ctx->Texture.Unit[0].CurrentD[2])
        {
			LOG(2,("RivaUpdateState05: setting texturing\n"));

            struct gl_texture_object *tObj = ctx->Texture.Unit[0].CurrentD[2];
            if (rivaContext.texObj != tObj)
            {
                RivaBindTexture(ctx, (GLenum)0, tObj);
            }
            else
            {
                RIVA_TEX_BINFO *pblk = (RIVA_TEX_BINFO *)tObj->DriverData;
                rivaContext.triContext.tri05.texFormat  = pblk->Format & 0x00FFFFFF;
                rivaContext.triContext.tri05.texFilter &= 0x00FFFFFF;
                /*
                 * Wrap/clamp.
                 */
                rivaContext.triContext.tri05.texFormat |= (tObj->WrapS == GL_CLAMP) ? 0x03000000 : 0x01000000;
                rivaContext.triContext.tri05.texFormat |= (tObj->WrapT == GL_CLAMP) ? 0x30000000 : 0x10000000;
                /*
                 * Filtering. Make sure we adhere to OGL texture sampling with each filter mode.
                 */
                if (tObj->MagFilter == GL_LINEAR)
                    rivaContext.triContext.tri05.texFilter |= 0x20000000;
                else
                    rivaContext.triContext.tri05.texFilter |= 0x10000000;
                switch (tObj->MinFilter)
                {
                    case GL_NEAREST:
						LOG(2,("RivaUpdateState05: filter is GL_NEAREST\n"));
                        rivaContext.triContext.tri05.texFilter |= 0x01000000;
                        rivaContext.triContext.tri05.texFormat &= 0xFFFF0FFF;
                        rivaContext.triContext.tri05.texFormat |= 0x00001000;
                        break;
                    case GL_LINEAR:
						LOG(2,("RivaUpdateState05: filter is GL_LINEAR\n"));
                        rivaContext.triContext.tri05.texFilter |= 0x02000000;
                        rivaContext.triContext.tri05.texFormat &= 0xFFFF0FFF;
                        rivaContext.triContext.tri05.texFormat |= 0x00001000;
                        break;
                    case GL_NEAREST_MIPMAP_NEAREST:
						LOG(2,("RivaUpdateState05: filter is GL_NEAREST_MIPMAP_NEAREST\n"));
                        rivaContext.triContext.tri05.texFilter |= 0x03000000;
                        rivaContext.triContext.tri05.texFormat |= pblk->maxLevel << 12;
                        break;
                    case GL_NEAREST_MIPMAP_LINEAR:
						LOG(2,("RivaUpdateState05: filter is GL_NEAREST_MIPMAP_LINEAR\n"));
                        rivaContext.triContext.tri05.texFilter |= 0x04000000;
                        rivaContext.triContext.tri05.texFormat |= pblk->maxLevel << 12;
                        break;
                    case GL_LINEAR_MIPMAP_NEAREST:
						LOG(2,("RivaUpdateState05: filter is GL_LINEAR_MIPMAP_NEAREST\n"));
                        rivaContext.triContext.tri05.texFilter |= 0x05000000;
                        rivaContext.triContext.tri05.texFormat |= pblk->maxLevel << 12;
                        break;
                    case GL_LINEAR_MIPMAP_LINEAR:
						LOG(2,("RivaUpdateState05: filter is GL_LINEAR_MIPMAP_LINEAR\n"));
                        rivaContext.triContext.tri05.texFilter |= 0x06000000;
                        rivaContext.triContext.tri05.texFormat |= pblk->maxLevel << 12;
                        break;
                    default:
						LOG(2,("RivaUpdateState05: filter is unknown\n"));
                        rivaContext.triContext.tri05.texFilter |= 0x01000000;
                        rivaContext.triContext.tri05.texFormat &= 0xFFFF0FFF;
                        rivaContext.triContext.tri05.texFormat |= 0x00001000;
                        break;
                }
            }
            /*
             *
             */
            rivaContext.triContext.tri05.triBlend |= (ctx->Texture.Unit[0].EnvMode == GL_MODULATE) ? 0x00000004 : 0x00000001;
        }
        else
        {
			LOG(2,("RivaUpdateState05: using default texture\n"));

            /*
             * Load default texture.
             */
            rivaContext.texObj                      = NULL;
            rivaContext.texOffset                   = riva_sinfo[FAKESNUM].DefaultTexture->voffset;
            rivaContext.triContext.tri05.texFormat  = 0x112215A1;
            rivaContext.triContext.tri05.triBlend  |= 0x00000004;
            rivaContext.triContext.tri05.texFilter |= 0x11000000;

            /*
             * Check for antialiased lines and points.
             */
//rudolf: MESA fault:
// setting texoffset and tricontext here won't work OK if an app uses intermixed
// settings for lines, points and tris!!!
// (points, lines, triangles need to be seperate. BTW smooth tri's are not in mesa3.2)
            if (ctx->Point.SmoothFlag)
            {
				LOG(2,("RivaUpdateState05: using AA points\n"));
                ctx->Driver.PointsFunc                 = RivaAAPoints3D05;
                rivaContext.texOffset                  = riva_sinfo[FAKESNUM].DefaultAATexture->voffset;
                rivaContext.triContext.tri05.texFormat = 0x33441461;
            }
            if (ctx->Line.SmoothFlag)
            {
				LOG(2,("RivaUpdateState05: using AA lines\n"));
                ctx->Driver.LineFunc                     = RivaAALine3D05;
                rivaContext.texOffset                    = riva_sinfo[FAKESNUM].DefaultAATexture->voffset;
                rivaContext.triContext.tri05.texFormat   = 0x33441461;
            }
            rivaContext.triContext.tri05.triFogColor = 0;
        }
        /*
         * Fog Color.
         */
        if (ctx->Fog.Enabled)
        {
			LOG(2,("RivaUpdateState05: using fog\n"));
            int r, g, b;

            RivaFloatToUInt(r, ctx->Fog.Color[2] * 255.0F); // SLW: R/B swap
            RivaFloatToUInt(g, ctx->Fog.Color[1] * 255.0F);
            RivaFloatToUInt(b, ctx->Fog.Color[0] * 255.0F); // SLW: R/B swap
            rivaContext.triContext.tri05.triFogColor  = (r << 16) | (b << 8) | b | 0xFF000000;
            rivaContext.triContext.tri05.triBlend    |= 0x00010000;
        }
        if (ctx->Color.DrawBuffer != rivaContext.bufRender)
        {
            rivaContext.bufRender = ctx->Color.DrawBuffer;
			//rudolf: fixed:
			riva_sinfo[FAKESNUM].SetSurfaces3D(
			((rivaContext.bufRender == GL_FRONT) ?
			riva_sinfo[FAKESNUM].frontbufferoffset :
			riva_sinfo[FAKESNUM].backbufferoffset),
			riva_sinfo[FAKESNUM].depthbufferoffset);
        }
        /*
         * Check for simple clip case.
         */
        rivaContext.SimpleClip = (rivaContext.bufRender != GL_FRONT) && (rivaContext.NumCtxs == 1);
        /*
         * Update HW context flag.
         */
        riva_sinfo[FAKESNUM].rivaReload3D = 1;
}
