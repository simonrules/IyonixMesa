#include <stdio.h>
//#include <graphic_driver.h>
//#include <OS.h>
//#include <KernelExport.h>
//#include <malloc.h>
#include "DriverInterface.h"
#include <stdarg.h>
#include <kernel.h>
#include <swis.h>

#define Timer_Start 0x490C0
#define Timer_Stop  0x490C1

/* Requires TimerMod */
/*void snooze(int time)
{
   int elapsed;
   _kernel_swi_regs r;

   _kernel_swi(Timer_Start, &r, &r);

   do
   {
      _kernel_swi(Timer_Stop, &r, &r);
   }
   while(r.r[1] < time);
}*/

/*debug logging*/
void nv_log(char *fmt, ...)
{
#if 0
	char     buffer[1024];
	char     fname[64];
	FILE    *myhand;
	va_list  args;

	sprintf (fname, "/boot/home/" DRIVER_PREFIX ".accelerant.3d.log");
	myhand=fopen(fname,"a+");

	if (myhand == NULL) return;

	va_start(args,fmt);
	vsprintf (buffer, fmt, args);
	fprintf(myhand, "%s", buffer);
	fclose(myhand);
#endif
}
