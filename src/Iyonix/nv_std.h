#include <stdio.h>
//#include <sys/ioctl.h>
#include <math.h>
//#include <OS.h>
#include "DriverInterface.h"
#include "nv_globals.h"
//apsed #include "nv_extern.h"
#include "nv_proto.h"
#include "nv_macros.h"
#include "nv_acc.h"

//void snooze(int time);

#define snooze(x)

#define LOG(x, y)

enum
{
  B_OK,
  B_ERROR,
};

/*enum
{
  NV04A,
  NV10A,
  NV20A,
  NV30A,
  NV40A,
};*/

enum
{
  B_CMAP8,
  B_RGB15_LITTLE,
  B_RGB16_LITTLE,
  B_RGB32_LITTLE,
  B_RGBA32_LITTLE,
};

/*enum
{
  NV25,
  NV40,
  NV41,
  NV43,
  NV44,
  NV45,
};*/
