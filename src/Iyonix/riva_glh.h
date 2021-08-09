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

#ifndef _NV_GLH_H
#define _NV_GLH_H

//rudolf:
//#include "glh.h"
#include "DriverInterface.h"
#include "../src/context.h"

/*
 * RIVA includes from X server.
 */
/* temp hack to maybe make things cleaner, and avoid including nv_local.h,
 * which then needs a WHOLE LOT MORE includes added
 */
//#define U032 CARD32
//#define U016 CARD16
//#define U008 CARD8

//rudolf: replace with Haiku 2D driver private includes
//#include "riva_hw.h"

/*
 * Buffer types for rivaBufferOffset[]
 */
#define RIVA_FRONT_BUFFER       0
#define RIVA_BACK_BUFFER        1
#define RIVA_DEPTH_BUFFER       2
#define RIVA_TEXTURE_BUFFER     3
#define RIVA_CACHE_BUFFER       4
/*
 * !!! This is the same variable !!!
 */
#define rivaReload3D    rivaRendered2D
/*
 * Sync get/put pixel with HW.
 */
extern int rivaSyncPix;
/*
 * Current 3D state context.
 */
typedef struct riva_tri05_ctx
{
    unsigned                  texColorKey;
    unsigned                  texFormat;
    unsigned                  texFilter;
    unsigned                  triBlend;
    unsigned                  triControl;
    unsigned                  triFogColor;
} RIVA_TRI05_CTX;
typedef struct riva_3d_ctx
{
    struct gl_texture_object *texObj;
    unsigned                  texOffset;
    union
    {
        RIVA_TRI05_CTX        tri05;
    } triContext;
    unsigned                  modeMask;
    float                     sAdjust;
    float                     tAdjust;
    int                       bufRender;
    unsigned short            VCache[16];
    int                       NumCtxs;
    int                       SimpleClip;
    float                     xyAdjust;
    unsigned                  BugFix;
    float                     InvWScale;
} RIVA_3D_CTX;
extern RIVA_3D_CTX rivaContext;


#define RIVA_VCACHE_MASK        0x07
#define NV4_Z_ENABLE_BUG        0x00000001
#define NV4_ALPHATEST_BUG       0x00000002

/*
 * Some cool macros for use during texture swizzling.
 */
#define RIVA_U_INC(uu)          (((uu) + 0xAAAAAAAB) & 0x55555555)
#define RIVA_U_INC2(uu)         (((uu) + 0xAAAAAAAC) & 0x55555555)
#define RIVA_V_INC(vv)          (((vv) + 0x55555556) & 0xAAAAAAAA)
#define RIVA_UV_OFFSET(vv,uu)   ((vv)|(uu))
/*
 * More cool texture swizzling macros for non-square texture support.
 */
#define RIVA_U_INC_MASK(i, m, lh)                           \
{                                                           \
    m = ((1 << (2 * lh)) - 1);                              \
    i = (0xAAAAAAAA & m) + 1;                               \
    m = (0x55555555 & m) | (~m);                            \
}
#define RIVA_U_INC2_MASK(i1, i2, m, lh)                     \
{                                                           \
    m = ((1 << (2 * lh)) - 1);                              \
    i1 = (0xAAAAAAAA & m) + 1;                              \
    i2 = (0xAAAAAAAA & m) + 2;                              \
    m = (0x55555555 & m) | (~m);                            \
}
#define RIVA_V_INC_MASK(i, m, lw)                           \
{                                                           \
    m = ((1 << (2 * lw)) - 1);                              \
    i = (0x55555555 & m) + 1;                               \
    m = (0xAAAAAAAA & m) | (~m);                            \
}
#define RIVA_UV_INC(uv,i,m)     (((uv) + i) & m)
/*
 * HW macros.
 */
//rudolf: fixed set by 2D driver to 0,0,$8000,$8000. Enough for clear cmd??
#define RIVA_CLIP3D(scrnnum,xx1, yy1, xx2, yy2)                                             \
{ \
	riva_sinfo[scrnnum].rivaReload3D = 1; \
	while (((nv_image_black_rectangle_ptr->FifoFree) >> 2) < 2) \
	{ \
		/* snooze a bit so I do not hammer the bus */ \
		snooze (10); \
	} \
	nv_image_black_rectangle_ptr->TopLeft = ((yy1) << 16) | (xx1); \
	nv_image_black_rectangle_ptr->HeightWidth = ((yy2-yy1) << 16) | (xx2-xx1); \
}
#define DEPTH_UNSCALE   (1.0F/(float)DEPTH_SCALE)
#define RIVA_STATE3D_05(scrnnum, ck, t0, t1, t2, bb, cc, fc)                                 \
{ \
	while (((nv4_dx5_texture_triangle_ptr->FifoFree) >> 2) < 7) \
	{ \
		/* snooze a bit so I do not hammer the bus */ \
		snooze (10); \
	} \
/*LOG(2,("state 1st FiFoFree is $%04x\n", nv4_dx5_texture_triangle_ptr->FifoFree));*/ \
	nv4_dx5_texture_triangle_ptr->Colorkey = ck; \
	nv4_dx5_texture_triangle_ptr->Offset = t0; \
	nv4_dx5_texture_triangle_ptr->Format = t1; \
	nv4_dx5_texture_triangle_ptr->Filter = t2; \
	nv4_dx5_texture_triangle_ptr->Blend = bb; \
	nv4_dx5_texture_triangle_ptr->Control = cc; \
	nv4_dx5_texture_triangle_ptr->FogColor = fc; \
/*LOG(2,("state 2nd FiFoFree is $%04x\n", nv4_dx5_texture_triangle_ptr->FifoFree));*/ \
}
#define RIVA_VERTEX3D_05(scrnnum,ii, xx, yy, zz, mm, argb, spec, ss, tt)                    \
{ \
	while (((nv4_dx5_texture_triangle_ptr->FifoFree) >> 2) < 8) \
	{ \
		/* snooze a bit so I do not hammer the bus */ \
		snooze (10); \
	} \
/*LOG(2,("vertex 1st FiFoFree is $%04x\n", nv4_dx5_texture_triangle_ptr->FifoFree));*/ \
	nv4_dx5_texture_triangle_ptr->TLVertex[ii].ScreenX = xx; \
	nv4_dx5_texture_triangle_ptr->TLVertex[ii].ScreenY = yy; \
	nv4_dx5_texture_triangle_ptr->TLVertex[ii].ScreenZ = (zz)*DEPTH_UNSCALE; \
	nv4_dx5_texture_triangle_ptr->TLVertex[ii].RWH = mm; \
	nv4_dx5_texture_triangle_ptr->TLVertex[ii].Color = argb; \
	nv4_dx5_texture_triangle_ptr->TLVertex[ii].Specular = spec; \
	nv4_dx5_texture_triangle_ptr->TLVertex[ii].TU = ss; \
	nv4_dx5_texture_triangle_ptr->TLVertex[ii].TV = tt; \
/*LOG(2,("vertex 2nd FiFoFree is $%04x\n", nv4_dx5_texture_triangle_ptr->FifoFree));*/ \
}
#define RIVA_DRAWTRI3D_05(scrnnum,v0, v1, v2)                                               \
{ \
	while (((nv4_dx5_texture_triangle_ptr->FifoFree) >> 2) < 1) \
	{ \
		/* snooze a bit so I do not hammer the bus */ \
		snooze (10); \
	} \
/*LOG(2,("drawtri3d 1st FiFoFree is $%04x\n", nv4_dx5_texture_triangle_ptr->FifoFree));*/ \
	nv4_dx5_texture_triangle_ptr->TLVDrawPrim[0] = (((v2)<<8)|((v1)<<4)|(v0)); \
/*LOG(2,("drawtri3d 2nd FiFoFree is $%04x\n", nv4_dx5_texture_triangle_ptr->FifoFree));*/ \
}
#define RIVA_DRAWQUAD3D_05(scrnnum,v0, v1, v2, v3)                                               \
{ \
	while (((nv4_dx5_texture_triangle_ptr->FifoFree) >> 2) < 1) \
	{ \
		/* snooze a bit so I do not hammer the bus */ \
		snooze (10); \
	} \
/*LOG(2,("drawtri3d 1st FiFoFree is $%04x\n", nv4_dx5_texture_triangle_ptr->FifoFree));*/ \
	nv4_dx5_texture_triangle_ptr->TLVDrawPrim[0] = (((v3)<<20)|((v2)<<16)|((v0)<<12)|((v2)<<8)|((v1)<<4)|(v0)); \
/*LOG(2,("drawtri3d 2nd FiFoFree is $%04x\n", nv4_dx5_texture_triangle_ptr->FifoFree));*/ \
}
/*
 * Get 1.0/W out of win coordinatea.
 */
#define INV_W(vv)   VB->Win.data[vv][3] * rivaContext.InvWScale
/*
 * Fast float->int conversions.
 */
#define RivaFloatToInt(intbits, fp)                                                 \
{                                                                                   \
    GLfloat fpbits  = fp + 8388608.0F;                                              \
    intbits = (*(GLint *)&fpbits & 0x80000000)                                      \
            ? -(*(GLint *)&fpbits & 0x007FFFFF) : *(GLint *)&fpbits & 0x007FFFFF;   \
}
#define RivaFloatToUInt(intbits, fp)                                                \
{                                                                                   \
    GLfloat fpbits  = fp + 8388608.0F;                                              \
    intbits = *(GLint *)&fpbits & 0x007FFFFF;                                       \
}
/*
 * Link for hooking accelerated funcitons.
 */
void      RivaPointsNOP(GLcontext *, GLuint, GLuint);
void      RivaAAPoints3D03(GLcontext *, GLuint, GLuint);
void      RivaPoints3D05(GLcontext *, GLuint, GLuint);
void      RivaAAPoints3D05(GLcontext *, GLuint, GLuint);
void      RivaLineNOP(GLcontext *, GLuint, GLuint, GLuint);
void      RivaLine3D05(GLcontext *, GLuint, GLuint, GLuint);
void      RivaAALine3D05(GLcontext *, GLuint, GLuint, GLuint);
void      RivaTriangleNOP(GLcontext *, GLuint, GLuint, GLuint, GLuint);
void      RivaTriangle3D05(GLcontext *, GLuint, GLuint, GLuint, GLuint);
GLboolean RivaRenderVB(GLcontext *, GLboolean);
void      RivaUpdateState05(GLcontext *ctx);
//rudolf: modify for BeOS function instead (orig GLXProcs)
//void      RivaSwapBuffersDirect();//XSMesaBuffer);
//void      RivaSwapBuffersIndirect();//XSMesaBuffer);
void      RivaSwapBuffersIyonix();//XSMesaBuffer);
void      RivaSync(int snum);
//rudolf: modify for BeOS function instead (orig GLXProcs)
//void      RivaPutPixel(GLXImage *,int x, int y, unsigned long pixel);

//rudolf:
GLboolean nvInitGLX(int);
//XSMesaContext RivaCreateContext(XSMesaVisual v, XSMesaContext share_list);
//GLboolean RivaMakeCurrent(XSMesaContext c);
GLdepth RivaGetDepth(GLcontext* ctx, int x, int y);
void RivaPutDepth(GLcontext* ctx, int x, int y, GLdepth depth);
void RivaCreateDepthBuffer(GLcontext *ctx);


//rudolf: beos version:
//GLboolean nvInitVisuals(VisualPtr *,/* DepthPtr *,*/ int *, int *, int *,/* VisualID *,*/ unsigned long, int);

#endif /* _NV_GLH_H */
