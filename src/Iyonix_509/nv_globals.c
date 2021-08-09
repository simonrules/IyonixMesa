/*
	Copyright 1999, Be Incorporated.   All Rights Reserved.
	This file may be used under the terms of the Be Sample Code License.

	Other authors:
	Mark Watson,
	Rudolf Cornelissen 8/2004-5/2005
*/

#include "nv_std.h"

int fd;
int handle;
shared_info *si;

/* FIFO channel pointers */
/* note:
 * every instance of the accelerant needs to have it's own pointers, as the registers
 * are cloned to different adress ranges for each one */
cmd_nv4_dx5_texture_triangle* nv4_dx5_texture_triangle_ptr;
//2D surface pointers, old style:
cmd_nv3_surface_x* nv3_surface_0_ptr;
cmd_nv3_surface_x* nv3_surface_1_ptr;
//3D surface pointers, old style:
cmd_nv3_surface_x* nv3_surface_2_ptr;
cmd_nv3_surface_x* nv3_surface_3_ptr;
//3D surface pointers, new style:
cmd_nv4_context_surfaces_argb_zs* nv4_context_surfaces_argb_zs_ptr;
// 2D and 3D clipping
cmd_nv_image_black_rectangle* nv_image_black_rectangle_ptr;

area_id shared_info_area;
area_id dma_cmd_buf_area;
vuint32 *regs;
area_id regs_area;
display_mode *my_mode_list;
area_id	my_mode_list_area;
int accelerantIsClone;
BRect FrontBuffer; // SLW-TODO: fix!

nv_get_set_pci nv_pci_access=
	{
		NV_PRIVATE_DATA_MAGIC,
		0,
		4,
		0
	};

nv_in_out_isa nv_isa_access=
	{
		NV_PRIVATE_DATA_MAGIC,
		0,
		1,
		0
	};
