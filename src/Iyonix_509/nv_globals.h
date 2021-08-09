#ifndef NV_GLOBALS_H
#define NV_GLOBALS_H

//rudolf: entire 3da is one module for now
//#define MODULE_BIT 0x80000000

//#include <GLView.h>
#include "nv_acc.h"
#include "DriverInterface.h"
//#include <interface/Bitmap.h>

typedef unsigned int vuint32;

typedef struct
{
  int32 left, top, right, bottom;
} BRect;

typedef struct
{
  int32 left, top, right, bottom;
} clipping_rect;

extern int fd;
extern int handle;
extern shared_info *si;

/* FIFO channel pointers */
/* note:
 * every instance of the accelerant needs to have it's own pointers, as the registers
 * are cloned to different adress ranges for each one */
extern cmd_nv4_dx5_texture_triangle* nv4_dx5_texture_triangle_ptr;
//2D surface pointers, old style:
extern cmd_nv3_surface_x* nv3_surface_0_ptr;
extern cmd_nv3_surface_x* nv3_surface_1_ptr;
//3D surface pointers, old style:
extern cmd_nv3_surface_x* nv3_surface_2_ptr;
extern cmd_nv3_surface_x* nv3_surface_3_ptr;
//3D surface pointers, new style:
extern cmd_nv4_context_surfaces_argb_zs* nv4_context_surfaces_argb_zs_ptr;
// 2D and 3D clipping
extern cmd_nv_image_black_rectangle* nv_image_black_rectangle_ptr;

extern area_id shared_info_area;
extern vuint32 *regs;
extern area_id regs_area;

/* used to track engine DMA stalls */
extern uint8 err;

extern BRect FrontBuffer;
extern sem_id drawing_lock;
extern bool swapping_buf;

extern clipping_rect *fClipList;
extern clipping_rect fClipBounds;
extern clipping_rect fWindowBounds;
//extern uint32 fNumClipRects;
//workarounds:
extern BRect WindowFrame;
//extern BRegion dvi;
//extern uint16 menu_offset;

//extern bool driver_updated;
//extern bool DirectMode;
//extern bool menu_offset_done;

extern uint32 clone_nr;

#endif
