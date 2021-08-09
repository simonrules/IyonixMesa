//#include <assert.h>
#include <stdio.h>
//#include <graphic_driver.h>
//#include <OS.h>
//#include <KernelExport.h>
//#include <malloc.h>
//#include <image.h>

#include "nv_std.h"
//#include <dirent.h>
#include <string.h>
#include "riva_glh.h"

void *get_framebuffer(void)
{
//	LOG(2,("get_framebuffer called, returning $%08lx\n", (uint32)(si->fbc.frame_buffer)));
	return si->fbc.frame_buffer;
}

void get_dm(display_mode *dm)
{
	dm = &(si->dm);
}

status_t nv_acc_wait_idle()
{
	/* wait until engine completely idle */
	while (ACCR(STATUS))
	{
		/* snooze a bit so I do not hammer the bus */
		snooze (100);
	}

	return B_OK;
}
#if 0
void nv_acc_assert_fifo(void)
{
	/* does every engine cmd this accelerant needs have a FIFO channel? */
	//fixme: can probably be optimized for both speed and channel selection...
	if (!si->engine.fifo.ch_ptr[NV_IMAGE_BLACK_RECTANGLE] ||
		!si->engine.fifo.ch_ptr[NV4_DX5_TEXTURE_TRIANGLE] /*||
		!si->engine.fifo.ch_ptr[NV4_CONTEXT_SURFACES_ARGB_ZS]*/)
	{
	LOG(2,("assert_fifo failed..\n"));

		uint16 cnt;

		/* no, wait until the engine is idle before re-assigning the FIFO */
		nv_acc_wait_idle();

		/* free the FIFO channels we want from the currently assigned cmd's */
		si->engine.fifo.ch_ptr[si->engine.fifo.handle[1]] = 0;
		si->engine.fifo.ch_ptr[si->engine.fifo.handle[7]] = 0;
//		si->engine.fifo.ch_ptr[si->engine.fifo.handle[6]] = 0;

		/* set new object handles */
		si->engine.fifo.handle[1] = NV_IMAGE_BLACK_RECTANGLE;
		si->engine.fifo.handle[7] = NV4_DX5_TEXTURE_TRIANGLE;
//		si->engine.fifo.handle[6] = NV4_CONTEXT_SURFACES_ARGB_ZS;

		/* set handle's pointers to their assigned FIFO channels */
		for (cnt = 0; cnt < 0x08; cnt++)
		{
			si->engine.fifo.ch_ptr[(si->engine.fifo.handle[cnt])] =
				(NVACC_FIFO + (cnt * 0x00002000));
		}

		/* program new FIFO assignments */
		ACCW(FIFO_CH1, (0x80000000 | si->engine.fifo.handle[1])); /* Clip */
		ACCW(FIFO_CH7, (0x80000000 | si->engine.fifo.handle[7])); /* Textured Triangle */
//		ACCW(FIFO_CH6, (0x80000000 | si->engine.fifo.handle[6])); /* surface */
	}

	/* update our local pointers */
	nv4_dx5_texture_triangle_ptr = (cmd_nv4_dx5_texture_triangle*)
		&(regs[(si->engine.fifo.ch_ptr[NV4_DX5_TEXTURE_TRIANGLE]) >> 2]);

	nv_image_black_rectangle_ptr = (cmd_nv_image_black_rectangle*)
		&(regs[(si->engine.fifo.ch_ptr[NV_IMAGE_BLACK_RECTANGLE]) >> 2]);

//	nv4_context_surfaces_argb_zs_ptr = (cmd_nv4_context_surfaces_argb_zs*)
//		&(regs[(si->engine.fifo.ch_ptr[NV4_CONTEXT_SURFACES_ARGB_ZS]) >> 2]);
}
#endif
