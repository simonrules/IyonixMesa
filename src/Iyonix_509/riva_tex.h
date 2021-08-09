#ifndef _RIVA_TEX_H
#define _RIVA_TEX_H

//rudolf: memory manager from utahGLX common drivercode
#include "glh_mm.h"

void RivaSetDefaultTexture(int snum);
void RivaSetDefaultAATexture(int snum);

void      RivaReleaseTextures(void);
int       RivaInitTextureHeap(int);
void      RivaTexImage(GLcontext *,GLenum,struct gl_texture_object *,GLint,GLint,const struct gl_texture_image *);
void      RivaTexSubImage(GLcontext *,GLenum,struct gl_texture_object *,GLint,GLint,GLint,GLint,GLint,GLint,const struct gl_texture_image *);
void      RivaDeleteTexture(GLcontext *, struct gl_texture_object *);
void      RivaBindTexture(GLcontext *, GLenum, struct gl_texture_object *);
GLboolean RivaIsTextureResident(GLcontext *, struct gl_texture_object *);

/*
 * Texture alignment/block sizes.
 */
#define RIVA_TEX_ALIGN          256
#define RIVA_TEX_BLOCK_SIZE     8192

/* mmaped address of any particular chunk o videomem we track with a pblk*/
#define PBLK_ADDRESS(scrn,pblk)(riva_sinfo[scrn].texturebase + \
				(pblk)->pmemblock->ofs)


/*
 * Priorities.
 */
#define TEX_MAX_PRIORITY    0x3F800000
#define TEX_MIN_PRIORITY    0x00000000

/*
 * Texel copy/conversion.
 */
#define TEX_FIX_SCALE           16

/*
 * Structures defining the texture buffer heap.
 */
typedef struct riva_tex_block
{
    struct   gl_texture_object *Owner;
    unsigned Priority;
    unsigned Format;
    unsigned maxLevel;
    unsigned baseWidthLog2;
    unsigned baseHeightLog2;
    float    sAdjust;
    float    tAdjust;
    int      Blkcount;
    PMemBlock pmemblock;
    int      voffset;
    struct riva_tex_block *NextLRU;
    struct riva_tex_block *PrevLRU;
} RIVA_TEX_BINFO;

typedef struct riva_tex_heap
{
    int             Total;
    int             FreeBlocks;
    RIVA_TEX_BINFO *HeadLRU;
    RIVA_TEX_BINFO *TailLRU;


    RIVA_TEX_BINFO *BlkInf;
} RIVA_TEX_HEAP;
extern RIVA_TEX_HEAP rivaTexHeap;
#define NULL_INDEX  0


#endif /* _RIVA_TEX_H */
