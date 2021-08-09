#ifndef NV_3DA_H
#define NV_3DA_H

//#include "buffers.h"
#include "DriverInterface.h"
#include "nv_macros.h"
#include "nv_acc.h"

status_t init_3da(void);
status_t uninit_3da(void);
void *get_framebuffer(void);
void get_dm(display_mode *dm);
void nv_acc_assert_fifo(void);
status_t nv_acc_wait_idle(void);

/* support functions */
void nv_log(char *format, ...);

#define LOG(level_bit, args) do { \
	uint32 mod = (si->settings.logmask &  0xfffffff0) & MODULE_BIT; \
	uint32 lev = (si->settings.logmask & ~0xfffffff0) & level_bit; \
	if (mod && lev) nv_log args; \
} while (0)

#endif
