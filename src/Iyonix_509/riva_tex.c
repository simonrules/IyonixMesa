#define FAKESNUM 0 /* fake screen number */
/*
 * RivaInitTextureHeap and RivaReleaseTextures are called by
 * our glx init/shutdown routines in riva_glx.c
 * All other exportable functions such as RivaTexImage are called
 * through the Mesa Context struct, like
 * ctx->Driver.TexImage() which we have updated to point to RivaTexImage
 *
 */

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
//#include <malloc.h>

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

/*
	BeOS includes.
*/
#include "glh.h"
#include "glh_mm.h"//from GLX common driver functions: memory manager

/*
 * GLX includes.
 */
//#include "xsmesaP.h"
//#include "glx_config.h"
//#include "hwlog.h"
//#include "glx_log.h"
//#include "glx_symbols.h"
/*
 * Riva includes.
 */
#include "riva_glh.h"
#include "riva_symbols.h"
#include "riva_tex.h"
//#include "nv_globals.h"
//#include "nv_3da.h"
#include "nv_std.h"

/*
 * Texture heap.
 */
RIVA_TEX_HEAP rivaTexHeap = {0};
memHeap_t *mmheap;
int allocatedDefaultTextures=0;


/****************************************************************************
 * Inner loop macros for converting OpenGL texel formats to RIVA formats.   *
 * These are not functions, because the critical inner part of them,        *
 *  'FETCH_TEXEL', gets redefined based on whether the texture is  using    *
 * GL_ALPHA, GL_INTENSITY, GL_RGB, or various other flags                   *
 ****************************************************************************/

#define UV_ARGB_LOOP(snum)                              \
{                                                                               \
    unsigned uInc, uInc2, vInc, uMask, vMask;                                   \
    RIVA_U_INC2_MASK(uInc, uInc2, uMask, img->HeightLog2);                      \
    RIVA_V_INC_MASK(vInc, vMask, img->WidthLog2);                               \
    for (j = ttop, v = vtop; j < ttop+theight; j += tinc, v = RIVA_UV_INC(v, vInc, vMask)) \
    {                                                                           \
        for (i = sleft, u = uleft; i < sleft+swidth; i += sinc2, u = RIVA_UV_INC(u, uInc2, uMask)) \
        {                                                                       \
            ii = i + img->Border;                                               \
            jj = j + img->Border;                                               \
            FETCH_TEXEL;                                                        \
            tex0 = ((alpha & 0xF0) << 8)                                        \
                 | ((blue   & 0xF0) << 4)                                        \
                 | ((green & 0xF0))                                             \
                 | ((red  & 0xF0) >> 4);                                       \
            ii = (i + sinc) + img->Border;                                      \
            FETCH_TEXEL;                                                        \
            tex1 = ((alpha & 0xF0) << 8)                                        \
                 | ((blue   & 0xF0) << 4)                                        \
                 | ((green & 0xF0))                                             \
                 | ((red  & 0xF0) >> 4);                                       \
            ptex = (unsigned long *)(pdst + RIVA_UV_OFFSET(u, v));              \
           *ptex = tex0 | (tex1 << 16);                                         \
        }                                                                       \
    }                                                                           \
    texFormat = 0x00000400;                                                     \
}
#define UV_RGB_LOOP(snum)                                          \
{                                                                               \
    unsigned uInc, uInc2, vInc, uMask, vMask;                                   \
    RIVA_U_INC2_MASK(uInc, uInc2, uMask, img->HeightLog2);                      \
    RIVA_V_INC_MASK(vInc, vMask, img->WidthLog2);                               \
    for (j = ttop, v = vtop; j < ttop+theight; j += tinc, v = RIVA_UV_INC(v, vInc, vMask)) \
    {                                                                           \
        for (i = sleft, u = uleft; i < sleft+swidth; i += sinc2, u = RIVA_UV_INC(u, uInc2, uMask)) \
        {                                                                       \
            ii = i + img->Border;                                               \
            jj = j + img->Border;                                               \
            FETCH_TEXEL;                                                        \
            tex0 = ((blue   & 0xF8) << 8)                                        \
                 | ((green & 0xFC) << 3)                                        \
                 | ((red  & 0xF8) >> 3);                                       \
            ii = (i + sinc)+ img->Border;                                       \
            FETCH_TEXEL;                                                        \
            tex1 = ((blue   & 0xF8) << 8)                                        \
                 | ((green & 0xFC) << 3)                                        \
                 | ((red  & 0xF8) >> 3);                                       \
            ptex = (unsigned long *)(pdst + RIVA_UV_OFFSET(u, v));              \
           *ptex = tex0 | (tex1 << 16);                                         \
        }                                                                       \
    }                                                                           \
    texFormat = 0x00000500;                                                     \
}

/************************************************************/
/* special internal convenience routines                    */
/************************************************************/

void RivaSetDefaultTexture(int snum)
{
	LOG(2,("RivaSetDefaultTexture: called\n"));

        rivaContext.texObj    = NULL;
        rivaContext.texOffset = riva_sinfo[snum].DefaultTexture->voffset;
//        rivaContext.triContext.tri03.texFormat = 0x22300000;
        rivaContext.sAdjust   = 0.0F;
        rivaContext.tAdjust   = 0.0F;

        /*
         * Make sure state gets reloaded.
         */
        riva_sinfo[snum].rivaReload3D = 1;
        RivaSync(snum);
}

/* see comments in RivaSetDefaultTexture.
 * Identical in function, except we set default AntiAlias texture
 */
void RivaSetDefaultAATexture(int snum)
{
	LOG(2,("RivaSetDefaultAATexture: called\n"));

        rivaContext.texObj    = NULL;
        rivaContext.texOffset = riva_sinfo[snum].DefaultAATexture->voffset;
//        rivaContext.triContext.tri03.texFormat = 0x22300000;
        rivaContext.sAdjust   = 0.0F;
        rivaContext.tAdjust   = 0.0F;

        /*
         * Make sure state gets reloaded.
         */
        riva_sinfo[snum].rivaReload3D = 1;
        RivaSync(snum);
}


/*
 * Move texels into texture buffer, (video memory?) converting to the
 * appropriate format for current video pixeldepth as we do so.
 * Currently supported formats:
 *  "RIVA ARGB_4444 or RGB_565."
 * Which is another way of saying we only know how to do 16bit textures.
 *
 * Return texture format value (OR'd with  'log size'. log2 size?)
 *
 */
static unsigned long RivaLoadTexels
(
    GLcontext                      *ctx,
    const struct gl_texture_image  *img,
    struct       gl_texture_object *tObj,
    unsigned short                 *pdst,
    unsigned                        left,
    unsigned                        top,
    unsigned                        width,
    unsigned                        height
)
{
	/* Note pdst gets used in MACROS. arg */
    unsigned ttop, theight, tinc, vtop, sleft, swidth, sinc, sinc2, uleft;
    unsigned texFormat, i, ii, j, jj, u, v, scale, right;
    unsigned long *ptex, tex0, tex1;
    GLubyte  red, green, blue, alpha;
    GLint    iwidth   = img->Width;   /* Includes border width */
    GLint    iwidth2  = img->Width2;  /* Power of 2 width      */
    GLint    iheight2 = img->Height2; /* Power of 2 height     */
    GLubyte *texture  = img->Data;    /* Texel data            */

	LOG(2,("RivaLoadTexels for texture(%p), mmap@%p\n", tObj, pdst));

    /*
     * Force horizontal alignment to even texels so the inner loop can go at 2 texels per transfer.
     */
    right   = left + width;
    if (left  & 0x01) left--;
    if (right & 0x01) right++;
    width   = right - left;
    vtop    = top;
    uleft   = left;
    ttop    = top;
    theight = height;
    sleft   = left;
    swidth  = width;
    tinc    = sinc = 1;
    sinc2   =        2;
    /*
     * Make sure there is texture data to convert.
     */
    if (texture)
    {
        /*
         * Expand U and V bits for swizzling.
         */
        unsigned bit, shift, uv;
        if (vtop)
        {
            uv = 0;
            shift = 0;
            for (bit = 1; bit < iheight2; bit <<= 1)
            {
                if (bit < iwidth2)
                    shift ++;
                uv |= (vtop & bit) << shift;
            }
            vtop = uv;
        }
        if (uleft)
        {
            uv = 0;
            shift = 0;
            for (bit = 1; bit < iwidth2; bit <<= 1)
            {
                uv |= (uleft & bit) << shift;
                if (bit < iheight2)
                    shift ++;
            }
            uleft = uv;
        }
        /*
         * Convert source texels into RIVA ARGB_4444 or RGB_565.
         */
        switch (img->Format)
        {
            case GL_COLOR_INDEX:
                {
                    int      index;
                    GLubyte *palette;

                    if (ctx && ctx->Texture.SharedPalette)
                        palette = ctx->Texture.Palette;
                    else
                        palette = tObj->Palette;
                    switch (tObj->PaletteFormat)
                    {
                        case GL_ALPHA:
                            red   =
                            green =
                            blue  = 0xFF;
                            #define FETCH_TEXEL                         \
                            index = texture[jj * iwidth + ii];          \
                            alpha = tObj->Palette[index];
                            UV_ARGB_LOOP(FAKESNUM);
                            #undef FETCH_TEXEL
                            break;
                        case GL_LUMINANCE:
                        case GL_INTENSITY:
                            #define FETCH_TEXEL                         \
                            index = texture[jj * iwidth + ii];          \
                            red   =                                     \
                            green =                                     \
                            blue  = palette[index];
                            UV_RGB_LOOP(FAKESNUM);
                            #undef FETCH_TEXEL
                            break;
                        case GL_LUMINANCE_ALPHA:
                            #define FETCH_TEXEL                         \
                            index = texture[jj * iwidth + ii] << 1;     \
                            red   =                                     \
                            green =                                     \
                            blue  = palette[index + 0];                 \
                            alpha = palette[index + 1];
                            UV_ARGB_LOOP(FAKESNUM);
                            #undef FETCH_TEXEL
                            break;
                        case GL_RGB:
                            #define FETCH_TEXEL                         \
                            index = texture[jj * iwidth + ii] * 3;      \
                            red   = palette[index + 0];                 \
                            green = palette[index + 1];                 \
                            blue  = palette[index + 2];
                            UV_RGB_LOOP(FAKESNUM);
                            #undef FETCH_TEXEL
                            break;
                        case GL_RGBA:
                            #define FETCH_TEXEL                         \
                            index = texture[jj * iwidth + ii] << 2;     \
                            red   = palette[index + 0];                 \
                            green = palette[index + 1];                 \
                            blue  = palette[index + 2];                 \
                            alpha = palette[index + 3];
                            UV_ARGB_LOOP(FAKESNUM);
                            #undef FETCH_TEXEL
                            break;
                        default:
                            red   =
                            green =
                            blue  = 0xFF;
                            #define FETCH_TEXEL
                            UV_RGB_LOOP(FAKESNUM);
                            #undef FETCH_TEXEL
                    }
                }
                break;
            case GL_ALPHA:
                red   =
                green =
                blue  = 0xFF;
                #define FETCH_TEXEL                                     \
                alpha = texture[jj * iwidth + ii];
                UV_ARGB_LOOP(FAKESNUM);
                #undef FETCH_TEXEL
                break;
            case GL_LUMINANCE:
            case GL_INTENSITY:
                #define FETCH_TEXEL                                     \
                red   =                                                 \
                green =                                                 \
                blue  = texture[jj * iwidth + ii];
                UV_RGB_LOOP(FAKESNUM);
                #undef FETCH_TEXEL
                break;
            case GL_LUMINANCE_ALPHA:
                #define FETCH_TEXEL                                     \
                red   =                                                 \
                green =                                                 \
                blue  = texture[((jj * iwidth + ii) << 1) + 0];         \
                alpha = texture[((jj * iwidth + ii) << 1) + 1];
                UV_ARGB_LOOP(FAKESNUM);
                #undef FETCH_TEXEL
                break;
            case GL_RGB:
                #define FETCH_TEXEL                                     \
                red   = texture[(jj * iwidth + ii) * 3 + 0];            \
                green = texture[(jj * iwidth + ii) * 3 + 1];            \
                blue  = texture[(jj * iwidth + ii) * 3 + 2];
                UV_RGB_LOOP(FAKESNUM);
                #undef FETCH_TEXEL
                break;
            case GL_RGBA:
                #define FETCH_TEXEL                                     \
                red   = texture[((jj * iwidth + ii) << 2) + 0];         \
                green = texture[((jj * iwidth + ii) << 2) + 1];         \
                blue  = texture[((jj * iwidth + ii) << 2) + 2];         \
                alpha = texture[((jj * iwidth + ii) << 2) + 3];
                UV_ARGB_LOOP(FAKESNUM);
                #undef FETCH_TEXEL
                break;
            default:
                red   =
                green =
                blue  = 0xFF;
                #define FETCH_TEXEL
                UV_RGB_LOOP(FAKESNUM);
                #undef FETCH_TEXEL
        }
    }
    else
    {
        /*
         * This must be a texture place holder.  Just give it a dummy format.
         */
        texFormat = 0x00000000;
    }
    /*
     * Return the texture format and log size.
     */
    {
        /*
         * Currently all that is set is the texture color format. (NV03 did other stuff)
         */
        return (texFormat | (img->HeightLog2 << 20) | (img->WidthLog2 << 16) | 0x000010A1);
    }
}

/* free Least-recently-used texture block, in our memory managing scheme.
 * Note that we also call RivaDeleteTexture()
 * return 1 on okay, or 0  if no more blocks to free
 */
static int freeLRU_Blk(int snum){
    /* ignore snum for now. sigh */
    RIVA_TEX_HEAP *heapp=&rivaTexHeap;
    RIVA_TEX_BINFO *pblk=heapp->HeadLRU;

    if(pblk == NULL)
    {
		LOG(2,("freeLRU_Blk: no more textures\n"));
		return 0;
    }

    /* do not delete first 2 textures, they're default ones */
    if((pblk==&rivaTexHeap.BlkInf[0]) ||
        (pblk==&rivaTexHeap.BlkInf[1]))
        {
			LOG(2,("freeLRU_Blk: Attempt to free a 'default' texture. ignoring\n"));
		    /* silently pretend we deleted it */
		    return 1;
        }

    while(pblk->NextLRU != NULL){
        pblk=pblk->NextLRU;
    }

    /* This clears pblk->Owner */
    RivaDeleteTexture(NULL, pblk->Owner);

    return 1;

}

/* Alocate a chunk o videoram, of size (numblocks*RIVA_TEX_BLOCK_SIZE)
 *
 * We have an interesting block allocation scheme.
 * As well as "allocating" the video memory, we also have to assign
 * a RIVA_TEX_BINFO block, to keep track of extra info
 * like "priority", format, and misc other things.
 * Return a pointer to BINFO.
 * We only set voffset, pmemblock, and BlkCount. Everything else is zeroed.
 * Up to caller to set the rest.
 *
 * Automatically free up memory if needed.
 * return NULL if memory size exceeded.
 */
RIVA_TEX_BINFO *allocTexBlk(int snum, int BlockCount)
{
    /* ignore snum for now. sigh */
    RIVA_TEX_HEAP *heapp=&rivaTexHeap;
    RIVA_TEX_BINFO *pblk;
    int freescan;

    if (BlockCount == 0)
    {
		LOG(4,("allocTexBlk: allocTexBlk passed 0 BlockCount\n"));
        return (NULL);
    }
    if(BlockCount > (rivaTexHeap.Total -2))
    {
		LOG(8,("allocTexBlk: ERROR: texture needs %d blocks: max space is %d\n",
			BlockCount, (rivaTexHeap.Total - 2)));
        return 0;
    }

    /*****************************************
     * Internal algorithm:
     * linear-scan for "free" infoblock.
     * I cant see having more than 1000 textures at a time, so
     * this really isnt a big deal.
     */

    /* First, grab ourselves an "info block" to keep track of the memory
     * information
     */

    for(freescan=allocatedDefaultTextures; freescan<heapp->Total; freescan++){
        if(heapp->BlkInf[freescan].pmemblock==NULL){
	    break;
	}
    }

    if(freescan>=heapp->Total)
    {
		/* This should never really happen:
		 * we have more blocks than we could ever use.
		 */
		LOG(8,("allocTexBlk: ERROR: internal err #2\n"));
		return 0;
    }


    pblk=&heapp->BlkInf[freescan];

//rudolf: was 'bzero' which does this exactly:
	memset(pblk, 0, sizeof(*pblk));

    /* Remember: it is not "allocated", until the pmemblock is non-null*/

    /*
     * Even if we know we have enough "free memory", we need a
     * single contiguous chunk o memory.
     * So if the alloc fails, keep freeing up memory until it does.
     */

    pblk->pmemblock = mmAllocMem(mmheap,
		RIVA_TEX_BLOCK_SIZE * BlockCount,6,0);

    while(pblk->pmemblock==NULL)
    {
#define REALSTUFF
#ifdef REALSTUFF
		LOG(2,("allocTexBlk: freeing up texture memory to make room\n"));
        if(!freeLRU_Blk(FAKESNUM))
        {
            /* We already checked against max size.
             * So if this hit, something is corrupted
             */
			LOG(8,("allocTexBlk: ERROR: RivaAllocateTexture: internal err.\n"));
            return NULL;
        }
        pblk->pmemblock = mmAllocMem(mmheap,
		RIVA_TEX_BLOCK_SIZE * BlockCount,6,0);
#else
		LOG(4,("allocTexBlk: EXPERIMENTAL: allocTexBlk:ENOMEM. Just returning NULL\n"));
		return NULL;
#endif
    }

    heapp->FreeBlocks -= BlockCount;

    /* offset from start of video memory:
     *   offset of start-of-texture-memory, + allocated offset
     */
    pblk->voffset = riva_sinfo[FAKESNUM].textureoffset + pblk->pmemblock->ofs;
    pblk->Blkcount = BlockCount;

    /*
     * Complete neccessary info for block.
     */

    return pblk;
}

void freeTexBlk (int snum, RIVA_TEX_BINFO *pblk)
{
	/* heap should be part of riva_sinfo. But it isnt.
	 * Keep this generic for now, anyway.
	 */
	RIVA_TEX_HEAP *heapp=&rivaTexHeap;

	/* First, remove from LRU list */
	if(pblk->PrevLRU==NULL){
		if(heapp->HeadLRU == pblk){
			heapp->HeadLRU = pblk->NextLRU;
		}
	} else {
		pblk->PrevLRU->NextLRU = pblk->NextLRU;
	}
	if(pblk->NextLRU != NULL){
		pblk->NextLRU->PrevLRU = pblk->PrevLRU;
	}


	mmFreeMem(pblk->pmemblock);
	pblk->pmemblock=NULL; /* This is the magic that signifies "free" */
 	heapp->FreeBlocks += pblk->Blkcount;
}


/* We have just used a texture that is in memory.
 * Promote it up the Least Recently Used chain
 *  (Or just add it TO the LRU chain)
 * The only purpose for this routine is for memory management
 * cache expiry purposes.
 * The only thing that uses info from here, is freeLRU_Blk()
 */
static void promoteTexBlk(int snum, RIVA_TEX_BINFO *pblk){
	/* heap should be part of riva_sinfo. But it isnt.
	 * Keep this generic pointer for now. be prepared.
	 */
	RIVA_TEX_HEAP *heapp=&rivaTexHeap;

	/* dont bother any more with debug if already head.
	 * Im more interested in turnover these days
	 */
	/*hwMsg(2," promoteTexBlk(%p)\n",pblk);*/

	if(heapp->HeadLRU==pblk)
	{
		return;
	}

	/* IFF already in the LRU chain, remove it first.*/
	if(pblk->NextLRU != NULL){
		pblk->NextLRU->PrevLRU = pblk->PrevLRU;
	}
	if(pblk->PrevLRU != NULL){
		pblk->PrevLRU->NextLRU = pblk->NextLRU;
	}
	pblk->PrevLRU=NULL;
	pblk->NextLRU=NULL;

	/* Now insert at top */
	if(heapp->HeadLRU==NULL)
	{
		LOG(2,("promoteTexBlk: promoting blk %p to be Unique head of LRU\n", pblk));
		heapp->HeadLRU=pblk;
		return;
	}

	pblk->NextLRU=heapp->HeadLRU;
	heapp->HeadLRU->PrevLRU=pblk;
	heapp->HeadLRU=pblk;
	LOG(1,("promoteTexBlk: promoting blk %p in front of %p\n", pblk, pblk->NextLRU));
}

/* Hook called by Mesa routines */
GLboolean RivaIsTextureResident( GLcontext *ctx, struct gl_texture_object *t)
{
	return (t->DriverData != NULL);
}

/*
 * Called only by RivaLoadTexture, currently.
 *
 * Make a Mesa-held Texture 'resident' in video RAM: Copy it in from
 * the Mesa copy of the texture.
 * Allocate info in our TextureHeap tracking mechanism.
 * Kick out older textures if we are short on video RAM space.
 * Mesa will still have the textures in 'main memory'
 *
 * Set the tObj->DriverData pointer to point to our private TextureHeap info.
 * This is the ONLY PLACE we set it
 *
 * return 1 (true) on okay, 0 (false) on fail.
 */
static int RivaAllocateTexture
(
    GLcontext                 *ctx,
    struct  gl_texture_object *tObj
)
{
    unsigned level, mipSize, totalSize, BlockCount;
    struct   gl_texture_image *img = tObj->Image[tObj->BaseLevel];
    RIVA_TEX_BINFO *pblk=NULL;

    /*
     * Major sanity checks first.
     */
    if (tObj->DriverData)
    {
		LOG(8,("RivaAllocateTexture: cannot alloc texture for tObj %p: already done\n",
			tObj));
        return (0);
    }

    /*
     * Calculate the memory requirements for a texture loaded into the texture buffer.
     * Determine the # of 8K blocks used be the texture, including mipmaps.
     */
    if (!img)
    {
		/* This is actually a "normal" branch, if the user calls
		 * glBindTexture() for the first time on a texture identifier.
		 * Then they fill it in with something, and next time, there should
		 * be data.
		 */
		LOG(1,("RivaAllocateTexture: Image[BaseLevel=%d]==NULL (tObj %p)\n",
			tObj->BaseLevel,tObj));
		LOG(1,("RivaAllocateTexture: ctx->texture=%p,   ReallyEnabled==%d\n",
			&ctx->Texture, ctx->Texture.ReallyEnabled));
        return (0);
    }

    /*
     * Find offset of level from base.
     */
    mipSize = 2 << (img->HeightLog2 + img->WidthLog2);
    totalSize  = 0;
    for (level = tObj->BaseLevel; (level <= tObj->P) && (tObj->Image[level]); level++)
    {
        totalSize += mipSize;
        mipSize  >>= 2;
    }
    BlockCount = (totalSize + RIVA_TEX_BLOCK_SIZE - 1) / RIVA_TEX_BLOCK_SIZE;


    pblk=allocTexBlk(FAKESNUM,BlockCount);

    if(pblk==NULL)
    {
		LOG(8,("RivaAllocateTexture: allocTexBlk failed! Setting Default texture\n"));
		RivaSetDefaultTexture(FAKESNUM);
        return 0;
    }

	LOG(1,("RivaAllocateTexture: gave pblk %p %d blocks: voffset %x-%x\n",
		pblk, BlockCount, pblk->voffset, pblk->voffset+(BlockCount*RIVA_TEX_BLOCK_SIZE)));
	LOG(1,("RivaAllocateTexture: Setting tObj(%p)->DriverData to pblk 0x%p\n",
		tObj, pblk));

    tObj->DriverData                     = pblk;

    pblk->maxLevel       = tObj->P;
    pblk->baseWidthLog2  = img->WidthLog2;
    pblk->baseHeightLog2 = img->HeightLog2;
    pblk->Owner          = tObj;
    pblk->sAdjust        = 0.5F / (float)img->Width2;
    pblk->tAdjust        = 0.5F / (float)img->Height2;

    return 1;
}

/*
 * Free a texture in the texture buffer heap.
 * This removes it from our nv-local cache of textures
 * which frees up the videoram space to be used by something else.
 * (but not the Mesa-level hash/cache of textures)
 *
 *
 * Used internally, as well as being a Mesa Driver.DeleteTexture hook.
 */
void RivaDeleteTexture
(
    GLcontext                *ctx,
    struct gl_texture_object *tObj
)
{
	RIVA_TEX_BINFO *pblk;

	if(tObj==NULL)
	{
		LOG(8,("RivaDeleteTexture: called with NULL tObj\n"));
		return;
	}

	pblk = (RIVA_TEX_BINFO *)tObj->DriverData;
	/*
	 * Make sure the texture is still resident in the texture buffer,
	 * otherwise, ignore it.
	 */
	if(pblk==NULL) return;

	if (pblk->Owner != tObj)
	{
		/*
		 * This isn't good.  Somehow things have gotten out of sync.  Bail.
		 */
		LOG(8,("RivaDeleteTexture: Inconsistent texture owner in RivaDeleteTexture!\n"));
	}

	LOG(1,("RivaDeleteTexture: freeing %d blocks (tObj %p,pblk %p, voffset %x)\n",
		pblk->Blkcount, tObj, pblk, pblk->voffset));

	tObj->DriverData = NULL;
	pblk->Owner      = NULL;

	freeTexBlk(FAKESNUM, pblk);

	/*
	 * If "current" texture, make sure we stop using it
	 * before loading new texels.
	 */
	if (rivaContext.texObj == tObj)
	{
		LOG(8,("RivaDeleteTexture: called on current texture. Setting default texture.\n"));
		RivaSetDefaultTexture(FAKESNUM);
	}
}


/* Called by RivaBindTexture, and RivaTexSubImage.
 *
 * Load a complete texture and mipmaps into buffer. (video memory)
 * While we're at it, set the tObj->DriverData private pointer to
 * point to our 'heap' id block for the texture if we do not currently
 * have it loaded into video memory.
 *
 * The copy+convert operation, via RivaLoadTexels, is rather.... inefficient.
 * You should not be calling this if we already had it loaded into
 * video memory. But happily, Mesa tends to take care of that for us.
 *
 *
 */
static GLboolean RivaLoadTexture
(
    GLcontext                *ctx,
    struct gl_texture_object *tObj
)
{
    RIVA_TEX_BINFO *pblk;
    int             level, mipWidthLog2, mipHeightLog2;

	LOG(1,("RivaLoadTexture: tObj = %p\n", tObj));

    if ((!tObj->DriverData) && (!RivaAllocateTexture(ctx, tObj)))
    {
        return (GL_FALSE);
    }

	LOG(1,("RivaLoadTexture: #2: tObj = %p, driverdata is = %p\n",
		tObj, tObj->DriverData));

    /*
     * Load all the mipmaps.
     */
    pblk = (RIVA_TEX_BINFO *)tObj->DriverData;
    /*
     * Reset max and min mipmaps.
     */
    pblk->Format  = 0x00000000;
    mipWidthLog2  = pblk->baseWidthLog2;
    mipHeightLog2 = pblk->baseHeightLog2;
    for (level = tObj->BaseLevel; (level <= tObj->P) && (tObj->Image[level]); level++)
    {
        if (mipWidthLog2 == tObj->Image[level]->WidthLog2 && mipHeightLog2 == tObj->Image[level]->HeightLog2)
        {
            RivaTexSubImage(ctx,
                            (GLenum)0,
                            tObj,
                            level,
                            0,
                            0,
                            tObj->Image[level]->Width,
                            tObj->Image[level]->Height,
                            tObj->Image[level]->IntFormat,
                            tObj->Image[level]);
            mipWidthLog2  -= 1;
            mipHeightLog2 -= 1;
        }
        else
            break;
    }
    return (pblk->Format ? GL_TRUE : GL_FALSE);
}

/*
 * Load a texture/subtexture into video memory.
 * Called by either RivaTexImage(), or RivaLoadTexture(), OR
 * as Mesa entrypoint  [ Driver.TexSubImage ]
 *
 * First, if the overall texture is not in our texture cache, add it.
 * Then,  take the pre-generated list of 'mipmaps' for the texture,and
 * figure out which one is appropriate for the distance we are using it at.
 * (Calculate the 'levelOffset')
 * Then call RivaLoadTexels, which actually does conversion of image formats
 * if neccessary, and then copies to videomemory, at the address
 * that WE CALCULATE HERE.
 *
 * Happily, Mesa is intelligent about calling us, and only calls us if
 * neccessary
 */
void RivaTexSubImage
(
    GLcontext   *ctx,
    GLenum       target,
    struct       gl_texture_object *tObj,
    GLint        level,
    GLint        xoffset,
    GLint        yoffset,
    GLint        width,
    GLint        height,
    GLint        internalFormat,
    const struct gl_texture_image *image
)
{
    RIVA_TEX_BINFO *pblk;
    int             mipSize, levelOffset, levelScan;
    unsigned        format;

    pblk = (RIVA_TEX_BINFO *)tObj->DriverData;
	if(pblk == NULL)
	{
		LOG(8,("RivaTexSubImage: ignoring tObj(%p) with null pblk\n", tObj));
        return;
    }
	else
	{
		LOG(1,("RivaTexSubImage: called with tObj %p, level %d\n", tObj, level));
    }
    /*
     * Make sure space has been allocated for this level.
     */
    if (pblk->maxLevel < level)
    {
		/*
		 * Re-allocate space for this texture. Yep, its recursive.
		 * But hopefully, only ONCE.
		 */
		LOG(4,("RivaTexSubImage: doing Delete + Load\n"));

		RivaDeleteTexture(ctx, tObj);
		RivaLoadTexture(ctx, tObj);
		return;
    }

    /*
     * Find offset of level from base.
     * MAKE SURE THIS MATCHES whatever is used in RivaAllocateTexture()!!
     * (It didnt used to, and it was very broken! )
     */
    mipSize = 2 << (pblk->baseHeightLog2 + pblk->baseWidthLog2);
    levelOffset=0;
    for (levelScan = tObj->BaseLevel; levelScan < level; levelScan++)
    {
        levelOffset += mipSize;
		LOG(1,("RivaTexSubImage: midloop: levelOffset now=%x, from mipSize=%x\n",
			levelOffset, mipSize));
        mipSize  >>= 2;
    }

    /*
     * Make sure we stop using this texture before loading new texels.
     */
    if (rivaContext.texObj == tObj)
    {
        riva_sinfo[FAKESNUM].rivaReload3D = 1;
        RivaSync(FAKESNUM);
    }
    /*
     * Move the texels into video memory.
     */
    format = RivaLoadTexels(ctx,
                            image,
                            tObj,
    (unsigned short *)      (PBLK_ADDRESS(FAKESNUM,pblk)  + levelOffset),
                            xoffset,
                            yoffset,
                            width,
                            height);

	LOG(1,("RivaTexSubImage: RivaLoadTexels has returned.\n"));

    if (!pblk->Format)
        pblk->Format = format;
    /*
     * Set min and max mipmap sizes when they get loaded.
     */
    if (((pblk->Format & 0x00F00000) < (format & 0x00F00000)) ||
        ((pblk->Format & 0x000F0000) < (format & 0x000F0000)))
              pblk->Format = (pblk->Format & 0x00FF0000) | (format & 0x00FF0000);
    /* call this BOTH in RivaBindTexture, and here, since it wont get
     * triggered in RivaBindTexture if the texture hasnt been loaded yet.
     * This is not so much "promoting" our pblk, as the initial adding
     * to the LRU list.
     */
    promoteTexBlk(FAKESNUM,pblk);
}

/* Mesa context entry point
 *  [ ctx->Driver.TexImage ]
 *
 * Doesnt too too much besides call RivaTexSubImage() for the mipmap 'level'
 * we care about.
 */
void RivaTexImage
(
    GLcontext   *ctx,
    GLenum       target,
    struct       gl_texture_object *tObj,
    GLint        level,
    GLint        internalFormat,
    const struct gl_texture_image *image
)
{
    RIVA_TEX_BINFO *pblk;
    int             mipWidthLog2, mipHeightLog2;

    pblk = (RIVA_TEX_BINFO *)tObj->DriverData;

    if(pblk == NULL)
	{
		LOG(8,("RivaTexImage: doing nothing for tObj %p(level %d): null pblk\n",
			tObj, level));
		return;
	}
	LOG(1,("RivaTexImage: (Obj %p(pblk=%p), level %d, image=%p)\n",
		tObj, tObj->DriverData, level, image));

    /*
     * Check if this image matches the size previously allocated.
     */
    mipWidthLog2  = pblk->baseWidthLog2  >> (level - tObj->BaseLevel);
    mipHeightLog2 = pblk->baseHeightLog2 >> (level - tObj->BaseLevel);

    if ((mipWidthLog2   == tObj->Image[level]->WidthLog2) &&
        (mipHeightLog2  == tObj->Image[level]->HeightLog2) &&
        (pblk->maxLevel == tObj->P))
    {
        RivaTexSubImage(ctx,
                        target,
                        tObj,
                        level,
                        0,
                        0,
                        image->Width,
                        image->Height,
                        internalFormat,
                        image);
    }
    else
    {
		LOG(4,("RivaTexImage: called, but DELETING tObj %p\n", tObj));

        /*
         * Delete texture in hardware and lazy load it later unless its the current texture.
         */
        if (rivaContext.texObj != tObj)
        {
            RivaDeleteTexture(ctx, tObj);
        }
        else
        {
            RivaDeleteTexture(ctx, tObj);
            RivaBindTexture(ctx, (GLenum)0, tObj);
        }
    }
}


/*
 * Bind texture to current context.
 * This should NOT be treated as synonymous with glBindTexture()
 *   although it is kinda close.
 * What we do here seems to be:
 *  - If texture is not already in video memory, copy it there, if
 *    need be.
 *    - if texture has no data, set "default texture" data for now.
 *  - Set "current texture" in the video hardware.
 *    This is the **only place** a user-specified texture is set.
 *    Other places set the default texture sometimes, but not
 *    a user-specified one.
 */
void RivaBindTexture
(
    GLcontext                *ctx,
    GLenum                    target,
    struct gl_texture_object *tObj
)
{
    RIVA_TEX_BINFO *pblk=NULL;

    /* need to find out why is it called when rivaglx disabled */
    if (riva_sinfo[FAKESNUM].rivaGLXEnabled == FALSE)
	{
		LOG(8,("RivaBindTexture: called when rivaGLXEnabled == FALSE\n"));
		return;
	}
	LOG(1,("RivaBindTexture: called with tObj %p\n", tObj));

    if(tObj!=NULL)
	{
        /*
         * Load the texture into memory if not already present.
         */
        if(tObj->DriverData == NULL)
		{
			RivaLoadTexture(ctx, tObj);
		}
        pblk = (RIVA_TEX_BINFO *)tObj->DriverData;
    }

    if(pblk == NULL)
	{
		LOG(8,("RivaBindTexture: setting default texture, since LoadTexture failed\n"));
        RivaSetDefaultTexture(FAKESNUM);
		return;
	}
    else
    {
        /* We have the texture loaded into memory.
         * Set it as the active texture.
         */
        rivaContext.texObj      = tObj;
        rivaContext.texOffset   = pblk->voffset;

		LOG(1,("RivaBindTexture: texture %p active (pblk %p) (voffset=%x)\n",
			tObj, pblk, rivaContext.texOffset));

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
                rivaContext.triContext.tri05.texFilter |= 0x01000000;
                rivaContext.triContext.tri05.texFormat &= 0xFFFF0FFF;
                rivaContext.triContext.tri05.texFormat |= 0x00001000;
                break;
            case GL_LINEAR:
                rivaContext.triContext.tri05.texFilter |= 0x02000000;
                rivaContext.triContext.tri05.texFormat &= 0xFFFF0FFF;
                rivaContext.triContext.tri05.texFormat |= 0x00001000;
                break;
            case GL_NEAREST_MIPMAP_NEAREST:
                rivaContext.triContext.tri05.texFilter |= 0x03000000;
                rivaContext.triContext.tri05.texFormat |= pblk->maxLevel << 12;
                break;
            case GL_NEAREST_MIPMAP_LINEAR:
                rivaContext.triContext.tri05.texFilter |= 0x04000000;
                rivaContext.triContext.tri05.texFormat |= pblk->maxLevel << 12;
                break;
            case GL_LINEAR_MIPMAP_NEAREST:
                rivaContext.triContext.tri05.texFilter |= 0x05000000;
                rivaContext.triContext.tri05.texFormat |= pblk->maxLevel << 12;
                break;
            case GL_LINEAR_MIPMAP_LINEAR:
                rivaContext.triContext.tri05.texFilter |= 0x06000000;
                rivaContext.triContext.tri05.texFormat |= pblk->maxLevel << 12;
                break;
            default:
                rivaContext.triContext.tri05.texFilter |= 0x01000000;
                rivaContext.triContext.tri05.texFormat &= 0xFFFF0FFF;
                rivaContext.triContext.tri05.texFormat |= 0x00001000;
                break;
        }
		promoteTexBlk(FAKESNUM,pblk);
    }

	/* Make sure state gets reloaded in UpdateState */
	LOG(1,("RivaBindTexture: done promote, now flagging rivaReload3d\n"));
    riva_sinfo[FAKESNUM].rivaReload3D = 1;
}

/* Release all textures including default ones (like for a mode switch). */
void RivaReleaseTextures(void)
{
    /* Kinda hacky,and not the fastest way to do it.
     * But nice and simple:
     * Keep freeing videoram blocks, until the routine says
     * there are no more blocks to free up!
     */
	LOG(2,("RivaReleaseTextures: releasing all textures\n"));
    while(freeLRU_Blk(FAKESNUM) != 0);
    allocatedDefaultTextures = 0;

    /* default textures stay in there, but delete their entry
     * and reallocate it everytime when initalizing */
    freeTexBlk(FAKESNUM, riva_sinfo[FAKESNUM].DefaultTexture);
    freeTexBlk(FAKESNUM, riva_sinfo[FAKESNUM].DefaultAATexture);
	LOG(2,("RivaReleaseTextures: released all textures!\n"));
}

/*
 * Initialize/Reinitialize texture buffer heap.
 *
 * The memory area for textures has already been allocated in
 * nvHookServerSymbols().
 * The amount is known by riva_sinfo[screennum].numtexblocks;
 *
 * This routine initializes the "heap", which
 * is essentially a memory manager for that large chunk o memory.
 *
 * The texture heap is broken up into 8K sized blocks.
 * This is the minimum texture size allowed by OpenGL (64X64).
 * 16 bit texels uses 8K minimum.
 * By pre-allocating the blocks now, alloc/free overhead is saved as well
 * as reducing memory fragmentation.
 * Texture heaps are very dynamic.
 * I prefer avoiding generic memory managers for specialized cases.
 */
int RivaInitTextureHeap(int snum)
{
    int texBlkTotal, i, j, u, v;
    unsigned short *ptex;
    RIVA_TEX_BINFO *pblk;

    /*
     * Free any previously allocated heap block info.
     */
    if (rivaTexHeap.BlkInf)
	{
		/*
			free()  frees  the  memory  space pointed to by ptr, which
			must have been returned by a previous  call  to  malloc(),
			calloc()  or  realloc().   Otherwise,  or if free(ptr) has
			already been called before,  undefined  behaviour  occurs.
			If ptr is NULL, no operation is performed.
		*/
		free (rivaTexHeap.BlkInf);
	}

    rivaTexHeap.BlkInf = NULL;
    /*
     * How much texture memory do we have?
     */
    texBlkTotal = riva_sinfo[snum].numtexblocks;

    /*
     * No memory? No bueno.
     */
    if (texBlkTotal <= 0) return (GL_FALSE);

    /* '0' means, start to manage memory at offset 0.
     * "offset 0", is actually "offset 0, from start of texture memory"
     * which is NOT THE SAME as "offset from video memory
     */
    mmheap = mmInit(0, texBlkTotal * RIVA_TEX_BLOCK_SIZE);

    /*
     * Set all the heap parameters. First two blocks are reserved for default and AA textures.
     */
    rivaTexHeap.Total           = texBlkTotal;
    rivaTexHeap.FreeBlocks      = texBlkTotal - 2;
    rivaTexHeap.HeadLRU         = NULL;
    rivaTexHeap.TailLRU         = NULL;

//rudolf: <<<<<<
/*
void *_xalloc(size_t n)
{
        void *p;
        if (!(p = malloc(n))) {
                fprintf(stderr, "_xalloc: memory allocation failed (%d)\n",
                                (int) n);
                exit(1);
        }
        memset(p, 0, n);
        return p;
}
*/
    rivaTexHeap.BlkInf = (RIVA_TEX_BINFO *)malloc(sizeof(RIVA_TEX_BINFO) * texBlkTotal);

	if(rivaTexHeap.BlkInf == NULL)
	{
		LOG(2,("RivaInitTextureHeap: malloc for BlkInf stuff FAILED??\n"));
		LOG(2,("RivaInitTextureHeap: We only tried for %d blocks...\n", texBlkTotal));
		return(GL_FALSE);
    }

	//rudolf: was 'bzero' which does this exactly:
	memset(((void *)(rivaTexHeap.BlkInf)), 0, (sizeof(RIVA_TEX_BINFO) * texBlkTotal));

    /*
     * Init and load default texture, if not already done.
     */
	LOG(2,("RivaInitTextureHeap: checking default textures\n"));
	pblk = riva_sinfo[FAKESNUM].DefaultTexture;

	pblk=allocTexBlk(FAKESNUM,1);
	pblk->Format   = 0x112215a1;

	ptex = (unsigned short*)PBLK_ADDRESS(FAKESNUM, pblk);
	for (i = 0; i < 16; i++)
	{
		ptex[i] = 0xffff;
	}

	riva_sinfo[FAKESNUM].DefaultTexture = pblk;
    allocatedDefaultTextures++;

	/* Init and load default AA texture. */
	pblk = riva_sinfo[FAKESNUM].DefaultAATexture;
	pblk=allocTexBlk(FAKESNUM,1);
	pblk->Format = 0x44200000;

	ptex = (unsigned short*)PBLK_ADDRESS(FAKESNUM,pblk);

	for (j = v = 0; j < 16; j++, v = RIVA_V_INC(v))
		for (i = u = 0; i < 16; i++, u = RIVA_U_INC(u))
		{
			float dist, alpha;
			unsigned short texel;

			dist  = (((float)j - 7.5)*((float)j - 7.5) + ((float)i - 7.5)*((float)i - 7.5)) / 64.0;
			if (dist < 0.0F) dist = 0.0F;
			if (dist > 1.0F) dist = 1.0F;
			alpha = (cos(sqrt(dist) * 3.141592654) * 0.5 + 0.55) * 15.0;
			RivaFloatToUInt(texel, alpha);
			ptex[RIVA_UV_OFFSET(u, v)] = (texel << 12) | 0x0fff;
		}

	riva_sinfo[FAKESNUM].DefaultAATexture = pblk;

    if(++allocatedDefaultTextures != 2)
	{
		LOG(2,("RivaInitTextureHeap: allocatedDefaultTextures is %d must be 2\n",
			allocatedDefaultTextures));
	}

	LOG(2,("RivaInitTextureHeap: done\n"));
    return (GL_TRUE);
}
