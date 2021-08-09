#ifndef _NV_SYMBOLS_H
#define _NV_SYMBOLS_H

//#include "xf86xv.h" /* includes xf86str.h, among other things */
//rudolf: replace with Haiku private 2D driver header if needed 
//#include "nv_type.h" /* xfree4 nv driver include */


#include "riva_tex.h"

/* Information that is specific to a particular card.
 * This is made this way in the hopes that one day, we will be able
 * to support hardware accel on more than one card.
 * Some of this stuff is merely copied over from the core xfree nv
 * driver info structs. Other parts are calculated.
 * We have this info here becuase hopefully it will result in less
 * dereferencing. PLUS, it lets us set up the info once in nvInitGLX(),
 * then use a consistant interface to it for all other routines.
 */

typedef struct riva_glx_info {
	int rivaGLXEnabled;
//rudolf disabled this:
//	NVPtr xfree_nvrec;   /* xfree4 nv driver private data */
	/* Sub-values of interest:
	 * xfree_nvrec->FbAddress == physical MemBase
	 * xfree_nvrec->FbBase    == MMAPPED video memory
	 * xfree_nvrec->IOAddress == [physical addr for "MMIO" ?]
	 * xfree_nvrec->IOBase    == MMAPED IOAddress
	 * xfree_nvrec->FbMapSize == vidram in BYTES
	 * xfree_nvrec->FbUsableSize == subset of FbMapSize:
	 *                              reserves either 32k or 128k mem
	 */
//rudolf disabled this: (struct from 2D driver for all kinds if driver info:
//                       kind of: 'shared_info' struct for Haiku)
//	RIVA_HW_INST *riva;   /* shortcut for xfree_nvrec->riva */
//rudolf: CARD32 = uint32:
	uint32 videoareasize; /* amount of mem that is actually displayed
	                      		 * onscreen as the user-visible screen
			      				 */

	unsigned char *privMemBase; /* mmapped address of our special vidmem*/

//rudolf: CARD32 = uint32:
	uint32 privMemOffset; /* offset from "vidram address 0".*/
	int privMemSize;	/* bytes in privMemBase mmap */

	/* This section is replacement for rivaBufferOffset[] numbers */
	 /* They are the byte offsets from "videomem byte 0" */
	uint32 frontbufferoffset;
	uint32 backbufferoffset; 
	uint32 depthbufferoffset;
	/* and now obviously the mmaped addresses */
	char *backbufmappedaddr; 
	char *depthbufmappedaddr; 
	
	/* These two are offshoots of original rivaBuffOffset[RIVA_TEXTURE...]
	 * where the original was a single 'base' value, I now precalculate
	 * all the address positions and offsets.
	 * Multiplication is slow compared to pointer dereference.
	 * 'offset' value is purely from start of video ram. .
	 */
//rudolf: CARD32 = uint32:
	uint32 textureoffset;

	/*
	 * this is really   'char *texturebase[numblocks]'
	 * This gets directly accessed in RivaInitTexture...,
	 * so it must point to process addressible memory, not phys mem.
	 */
	char *texturebase;
				/* Note that according to riva_glx.h,
				 * current size of a texture block is
				 * merely 8192 bytes
				 */
	int numtexblocks;
        RIVA_TEX_BINFO *DefaultTexture;
        RIVA_TEX_BINFO *DefaultAATexture;
	
	int bitsperpixel;
	int bytesperpixel;


	int rivaRendered2D;	/* rivaReload3D is aliased to this also */
	
//rudolf: updated.
	void (*SetSurfaces2D)(uint16 pitch0, uint16 pitch1, uint32 surf0, uint32 surf1);
	void (*SetSurfaces3D)(uint32 surf0, uint32 surf1);
} RIVA_GLX_INFO;

extern RIVA_GLX_INFO *riva_sinfo; /* per-screen info */

int nvHookServerSymbols( void *handle );
void nvUpdateBuffers(void);
int nvUnhookServerSymbols(void);

#endif /* _NV_SYMBOLS_H */
