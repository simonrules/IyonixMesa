#define FAKESNUM 0 /* fake screen number */

#include <stdio.h>
#include <stdlib.h>
//#include <OS.h>

/*
 * Mesa includes.
 */
#include "../src/context.h"

#include "riva_glh.h"
#include "riva_symbols.h"
#include "riva_tex.h"
//#include "glh.h"
//#include "nv_globals.h"
//#include "nv_3da.h"
#include "nv_std.h"

#define RIVA_MINMEMSIZE ( (1024 * 1024) + (128 * 1024) )

#define RIVA_PREFERREDMEMSIZE ( (4 * 1024 * 1024) + (128 * 1024) )

#define true 1
#define false 0

/* This is where we keep per-card information, like
 * cached info on where the mapped video ram is
 * This will be allocated as riva_sinfo[numscreens]
 */
RIVA_GLX_INFO *riva_sinfo=NULL;

/* allocate back, depth buffers,
 * called from nvHookSymbols only for now.
 * required memory checking already done in nvMapPrivMem() */
static void nvAllocateBuffers(void)
{
   int snum = FAKESNUM;
   char *currfreeptr = (char *)(riva_sinfo[snum].privMemBase);
   int freebytesleft = riva_sinfo[snum].privMemSize;
	/* offset from vidram 0 */
   int freeoffset = riva_sinfo[snum].privMemOffset;
   int texturebufsize, backbufsize, depthbufsize;

   /* use videoareasize as back and depth buffer size for now */
	riva_sinfo[snum].videoareasize = si->dm.virtual_height * si->fbc.bytes_per_row;

   backbufsize = riva_sinfo[snum].videoareasize;
   depthbufsize = riva_sinfo[snum].videoareasize;

   /* This is just assumed to always be 0, but it's a consistency thing
    * to have it defined */
   riva_sinfo[snum].frontbufferoffset = 0;
//rudolf: not for us: we might have an offset (hw cursor)!
	//if (si->settings.hardcursor) riva_sinfo[snum].frontbufferoffset = 2048; // SLW-TODO

	LOG(2,("nvAllocateBuffers: initial free space is $%08x\n", freebytesleft));

   riva_sinfo[snum].depthbufferoffset = freeoffset;
   riva_sinfo[snum].depthbufmappedaddr = currfreeptr;
   freeoffset += depthbufsize;
   currfreeptr += depthbufsize;
   freebytesleft -= depthbufsize;

	LOG(2,("nvAllocateBuffers: depthbuffer mapped to $%08x, offset $%08x, size $%08x, free bytes left now $%08x\n",
		riva_sinfo[snum].depthbufmappedaddr,
		riva_sinfo[snum].depthbufferoffset,
		depthbufsize, freebytesleft));

   riva_sinfo[snum].backbufferoffset = freeoffset;
   riva_sinfo[snum].backbufmappedaddr = currfreeptr;
   freeoffset += backbufsize;
   currfreeptr += backbufsize;
   freebytesleft -= backbufsize;

   /*fprintf(stderr,"nvglx: backbuffer mapped to %p, offset=%d, size=%d, free bytes=%d\n",
	   riva_sinfo[snum].backbufmappedaddr,
	   riva_sinfo[snum].backbufferoffset,
	   backbufsize, freebytesleft);*/

   /* without reserving some memory, random crashes occur.
    * taken from xfree nv driver (nv_driver.c) */
   freebytesleft -= (128 * 1024);

//   ErrorF("nvglx: %d bytes free in video memory\n", freebytesleft);

   riva_sinfo[snum].numtexblocks = freebytesleft / RIVA_TEX_BLOCK_SIZE;

   texturebufsize = riva_sinfo[snum].numtexblocks * RIVA_TEX_BLOCK_SIZE;
   freebytesleft -= texturebufsize;

   /*fprintf(stderr,"nvglx: will alloc %d texture blocks(0x%x/%d bytes) at %p\n",
	   riva_sinfo[snum].numtexblocks, texturebufsize, texturebufsize,
	   currfreeptr);*/

    riva_sinfo[snum].textureoffset = freeoffset;
    riva_sinfo[snum].texturebase = currfreeptr;
}

//rudolf: replace with haiku 3D or 2D driver functions!
/*
static void nv4SetSurfaces2D
(
    RIVA_HW_INST *chip,//shared_info etc
    unsigned     surf0,
    unsigned     surf1
)
{
    RivaSurface *Surface = (RivaSurface *)&(chip->FIFO[0x0000E000/4]);

    chip->FIFO[0x00003800] = 0x80000003;
    Surface->Offset        = surf0;
    chip->FIFO[0x00003800] = 0x80000004;
    Surface->Offset        = surf1;
    chip->FIFO[0x00003800] = 0x80000014;
}
*/

static void nv10SetSurfaces2D(uint16 pitch0, uint16 pitch1, uint32 surf0, uint32 surf1)
{
//note:aquire/release in main code because 2D drv could kick in!!
	LOG(2,("nv10SetSurfaces2D called\n"));

//rudolf: fixme, we are 'borrowing' the FIFO from the triangle cmd!!
	nv3_surface_0_ptr = (cmd_nv3_surface_x*) nv4_dx5_texture_triangle_ptr;
	nv3_surface_1_ptr = (cmd_nv3_surface_x*) nv4_dx5_texture_triangle_ptr;

	while (((nv3_surface_0_ptr->FifoFree) >> 2) < 7)
	{
		/* snooze a bit so I do not hammer the bus */
		snooze (10);
	}
	//re-assign fifo channel:
	ACCW(FIFO_CH7, (0x80000000 | NV3_SURFACE_0));
	//set surface
	nv3_surface_0_ptr->Pitch = pitch0;
	nv3_surface_0_ptr->SetOffset = surf0;
	//re-assign fifo channel again:
	ACCW(FIFO_CH7, (0x80000000 | NV3_SURFACE_1));
	//set surface
	nv3_surface_1_ptr->Pitch = pitch1;
	nv3_surface_1_ptr->SetOffset = surf1;
	//give channel back to triangle cmd:
	ACCW(FIFO_CH7, (0x80000000 | si->engine.fifo.handle[7])); /* Textured Triangle */
}

static void nv4SetSurfaces3D(uint32 surf0, uint32 surf1)
{
	LOG(2,("nv4SetSurfaces3D called\n"));

	AQUIRE_BEN(si->engine.lock)
	nv_acc_assert_fifo();

//rudolf: fixme, we are 'borrowing' the FIFO from the triangle cmd!!
	nv3_surface_2_ptr = (cmd_nv3_surface_x*) nv4_dx5_texture_triangle_ptr;
	nv3_surface_3_ptr = (cmd_nv3_surface_x*) nv4_dx5_texture_triangle_ptr;

	while (((nv3_surface_2_ptr->FifoFree) >> 2) < 5)
	{
		/* snooze a bit so I do not hammer the bus */
		snooze (10);
	}
	//re-assign fifo channel:
	ACCW(FIFO_CH7, (0x80000000 | NV3_SURFACE_2));
	//set surface
	nv3_surface_2_ptr->SetOffset = surf0;
	//re-assign fifo channel again:
	ACCW(FIFO_CH7, (0x80000000 | NV3_SURFACE_3));
	//set surface
	nv3_surface_3_ptr->SetOffset = surf1;
	//give channel back to triangle cmd:
	ACCW(FIFO_CH7, (0x80000000 | si->engine.fifo.handle[7])); /* Textured Triangle */

	RELEASE_BEN(si->engine.lock)
}

static void nv10SetSurfaces3D(uint32 surf0, uint32 surf1)
{
	LOG(2,("nv10SetSurfaces3D called\n"));

	AQUIRE_BEN(si->engine.lock)
	nv_acc_assert_fifo();

//rudolf: fixme, we are 'borrowing' the FIFO from the triangle cmd!!
	nv4_context_surfaces_argb_zs_ptr =
		(cmd_nv4_context_surfaces_argb_zs*) nv4_dx5_texture_triangle_ptr;

	while (((nv4_context_surfaces_argb_zs_ptr->FifoFree) >> 2) < 5)
	{
		/* snooze a bit so I do not hammer the bus */
		snooze (10);
	}

	//re-assign fifo channel:
	ACCW(FIFO_CH7, (0x80000000 | NV4_CONTEXT_SURFACES_ARGB_ZS));
	//set surfaces
	//rud: granularity is 64 (looks like bytes)!!! (otherwise engine crash.)
//setup pitch: this will greatly improve speed for windowed apps!!
//rud: we have to keep the Z-buffer at 16bit pitch!!??? (fixme for 8-bit)
//rud fixme:checkout nv20 for granularity of 128 bytes!!!

//rudtst:
uint16 zpitch = (uint16)(FrontBuffer.right - FrontBuffer.left + 1);
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
//	LOG(2,("zpitch is $%04x, pitch is $%04x\n", zpitch, si->fbc.bytes_per_row));

	nv4_context_surfaces_argb_zs_ptr->Pitch =
//		si->fbc.bytes_per_row | (zpitch << 16);
		zpitch | (zpitch << 16);

//	nv4_context_surfaces_argb_zs_ptr->Pitch =
//		si->fbc.bytes_per_row | ((si->fbc.bytes_per_row & 0xfffe) << 15);
//rudolf:
//should set actual width, adhering to suported granularity of HW
//		xsmesa->gl_ctx->Buffer->Width | ((xsmesa->gl_ctx->Buffer->Width) << 16);
	nv4_context_surfaces_argb_zs_ptr->SetOffsetColor = surf0;
	nv4_context_surfaces_argb_zs_ptr->SetOffsetZeta = surf1;

	//give channel back to triangle cmd:
	ACCW(FIFO_CH7, (0x80000000 | si->engine.fifo.handle[7])); /* Textured Triangle */

//	LOG(2,("nv10SetSurfaces3D FiFoFree after is $%04x\n", nv4_context_surfaces_argb_zs_ptr->FifoFree));

	RELEASE_BEN(si->engine.lock)
}

/*
 * Very important routine.
 * Not only is this routine responsible for "Hooking in XFree server symbols"
 * but also for a lot of large memory allocation:
 *    BACK BUFFER
 *    DEPTH BUFFER
 *    TEXTURE [memory, not exactly a "buffer"]
 * I would LIKE to do this in the more appropriately named nvInitGLX,
 * however, that routine depends on this stuff already being set up.
 *
 * return true (1) on okay, false (0) on fail
 */

//rudolf: well, we don't need those symbols, just the mem allocation: fixme!

int nvHookServerSymbols( void *handle )
{
   int snum=FAKESNUM; /* XXX hardcode screennum. Bad Boy */
//rudolf: disabled for now, is 2D driver private info (shared_info for Haiku)
//   ScrnInfoPtr scrninfoP;
//rudolf: disabled by me:
//   ScreenPtr screenP;
//rudolf disabled this:
//   NVPtr nvptr;/* xfree4 nv driver private data */
   int bytesperpixel;

   //fprintf(stderr,"DEBUG: in nvHookServerSymbols\n");

	//driver_updated = true;
	si->mode_changed = false;
	//DirectMode = false;
	//menu_offset_done = false;
	//menu_offset = 0;
	//fNumClipRects = 0;

   /* Hook into symbols that are in the XFree nv driver specifically */
	if(riva_sinfo !=NULL)
	{
		fprintf(stderr, "DEBUG: nvHookServerSymbols: riva_sinfo not NULL?\n");
		return 0;
	}
//rudolf disabled this all:
//   scrninfoP=glxsym.scrninfoList[snum];
//(rudolf: pointer to screen:)
//   screenP=screenInfo.screens[scrninfoP->scrnIndex];
//   nvptr=(NVPtr) scrninfoP->driverPrivate;

//rudolf: create 'shared_info': we do this differently, so not needed.
//   riva_sinfo=(RIVA_GLX_INFO*)xalloc(sizeof (RIVA_GLX_INFO) * screenInfo.numScreens);
//   if(!riva_sinfo){
//	ErrorF("nvHookServerSymbols: xalloc failed\n");
//	return 0;
//   }

//rudolf added:
	riva_sinfo = (RIVA_GLX_INFO*)malloc(sizeof (RIVA_GLX_INFO) * 1 /* #screens */);
	if(!riva_sinfo)
	{
		fprintf(stderr, "DEBUG: nvHookServerSymbols: malloc failed\n");
		return 0;
	}
    memset((void *)(riva_sinfo), 0, sizeof (RIVA_GLX_INFO) * 1 /* #screens */);

//rudolf: disabled by me: (clear 'shared_info' memory: not needed by us).
//rudolf: was 'bzero' which does this exactly:
//	memset(riva_sinfo, 0, sizeof (RIVA_GLX_INFO) * screenInfo.numScreens);

//rudolf: set to 32 for now.
	switch (si->dm.space)
	{
	case B_RGB15_LITTLE:
		riva_sinfo[snum].bitsperpixel = 15;
		bytesperpixel = 2;
		break;
	case B_RGB16_LITTLE:
		riva_sinfo[snum].bitsperpixel = 16;
		bytesperpixel = 2;
		break;
   case B_RGB32_LITTLE:
		riva_sinfo[snum].bitsperpixel = 32;
		bytesperpixel = 4;
		break;
	default:
//      fprintf (stderr, "nvglx: unusable depth %d bpp\n",
//         scrninfoP->bitsPerPixel);
		return 0;
	}
//   fprintf(stderr,"GLX: nvHookServerSym: using %d bytesperpixel(bits=%d)\n",
//	   bytesperpixel, scrninfoP->bitsPerPixel);
//   fprintf(stderr,"   Xserver rgb masks:%lx/%lx/%lx rgbbits:%x\n",
//	   scrninfoP->mask.red,
//	   scrninfoP->mask.green,
//	   scrninfoP->mask.blue,
//	   scrninfoP->rgbBits);

	riva_sinfo[snum].bytesperpixel = bytesperpixel;

//rudolf disabled this:
//   riva_sinfo[snum].xfree_nvrec= nvptr; /* make copy of xfree4 nv driver private data */
//rudolf disabled this: (struct from 2D driver for all kinds if driver info:
//                       kind of: 'shared_info' struct for Haiku)
//   riva_sinfo[snum].riva= &nvptr->riva;

	/* virtual screen size in bytes */
	riva_sinfo[snum].videoareasize = si->dm.virtual_height * si->fbc.bytes_per_row;

   /* map video memory */
	//rudolf: check enough space!!! orig:
	// first check we have really enough memory
	// a backbuffer, a depthbuffer, min required texture memory
/*
	minmemsize = (2 * riva_sinfo[screennum].videoareasize)
	    + RIVA_MINMEMSIZE;
	if (maxmemsize < minmemsize) {
	    fprintf (stderr, "nvglx: not enough memory, min %d bytes needed, %d available\n",
		minmemsize,maxmemsize);
	    fprintf (stderr, "  note: supposedly videoareasize=%ld bytes\n",
		     riva_sinfo[screennum].videoareasize);
*/

	riva_sinfo[snum].privMemSize = (si->mem_high - si->mem_low) + 1;
	riva_sinfo[snum].privMemOffset = si->mem_low;
	/* start of visible screen */
	riva_sinfo[snum].privMemBase = (unsigned char *)si->framebuffer + si->mem_low;

   /* allocate back and depth buffers. */
   nvAllocateBuffers();

//rudolf: fixme.. if needed
//   switch (riva_sinfo[snum].riva->Architecture) {
//   case NV_ARCH_04:
//      riva_sinfo[snum].SetSurfaces2D = nv4SetSurfaces2D;
//      riva_sinfo[snum].SetSurfaces3D = nv4SetSurfaces3D;
//      break;
//   default:
//      riva_sinfo[snum].SetSurfaces2D = nv10SetSurfaces2D;
//rudolf: should work on NV04 as well!
      riva_sinfo[snum].SetSurfaces2D = nv10SetSurfaces2D;
      riva_sinfo[snum].SetSurfaces3D = nv10SetSurfaces3D;
//   }

	LOG(2,("nvHookServerSymbols: depthoffset = $%08x, backoffset = $%08x\n",
		riva_sinfo[snum].depthbufferoffset,	riva_sinfo[snum].backbufferoffset));
	LOG(2,("nvHookServerSymbols: textureoffset = $%08lx\n",
		(riva_sinfo[snum].textureoffset)));

   /*fprintf(stderr,"DEBUG: nvHook wrapup: depthoffset=0x%x, backoffset=0x%x, textureoffset=0x%lx\n",
		riva_sinfo[snum].depthbufferoffset,
		riva_sinfo[snum].backbufferoffset,
		riva_sinfo[snum].textureoffset);*/

//rudfix:
    nvInitGLX(snum);
/*
{
//	XSMesaContext c;
//	XSMesaVisual v;

	c = RivaCreateContext(v, NULL); //rudolf assuming NULL!

	if (!RivaMakeCurrent(c))
	{
		LOG(2,("nvHookServerSymbols: RivaMakeCurrent failed!\n"));
	}
}
*/
   return 1;
}

/* called after a 2D modeswitch is completed */
void nvUpdateBuffers(void)
{
   int snum=FAKESNUM; /* XXX hardcode screennum. Bad Boy */
   int bytesperpixel;

	//fixme: just place NULL rendering functions in the driverinterface during mode-
	//switches, instead of letting them check for a modeswitch one-by-one!!!!!!!!!

	/* release all textures */
	RivaReleaseTextures();

	/* we need to wait a bit to be sure the engine's 2D init is completely done */
	snooze(100000);

	switch (si->dm.space)
	{
	case B_RGB15_LITTLE:
		riva_sinfo[snum].bitsperpixel = 15;
		bytesperpixel = 2;
		break;
	case B_RGB16_LITTLE:
		riva_sinfo[snum].bitsperpixel = 16;
		bytesperpixel = 2;
		break;
   case B_RGB32_LITTLE:
		riva_sinfo[snum].bitsperpixel = 32;
		bytesperpixel = 4;
		break;
	default:
//      fprintf (stderr, "nvglx: unusable depth %d bpp\n",
//         scrninfoP->bitsPerPixel);
		return;
	}

	riva_sinfo[snum].bytesperpixel = bytesperpixel;

	/* virtual screen size in bytes */
	riva_sinfo[snum].videoareasize = si->dm.virtual_height * si->fbc.bytes_per_row;

	riva_sinfo[snum].privMemSize = (si->mem_high - si->mem_low) + 1;
	riva_sinfo[snum].privMemOffset = si->mem_low;
	/* start of visible screen */
	riva_sinfo[snum].privMemBase = (unsigned char *)si->framebuffer + si->mem_low;

   /* allocate back and depth buffers. */
   nvAllocateBuffers();

   /*fprintf(stderr,"DEBUG: nvHook wrapup: depthoffset=0x%x, backoffset=0x%x, textureoffset=0x%lx\n",
		riva_sinfo[snum].depthbufferoffset,
		riva_sinfo[snum].backbufferoffset,
		riva_sinfo[snum].textureoffset);*/

	glResizeBuffersMESA();

	/* reinit texture heap and reload default textures */
	RivaInitTextureHeap(snum);

	riva_sinfo[FAKESNUM].SetSurfaces3D(
		((rivaContext.bufRender == GL_FRONT) ?
			riva_sinfo[FAKESNUM].frontbufferoffset :
			riva_sinfo[FAKESNUM].backbufferoffset),
		riva_sinfo[FAKESNUM].depthbufferoffset);

	/* signal OK to start rendering again */
	si->mode_changed = false;
	//driver_updated = true;
}

int nvUnhookServerSymbols(void)
{
	free (riva_sinfo);
	riva_sinfo = NULL;
}
