!include <win32.mak>

CFLAGS        = $(cvarsdll) /Ox /G5 /D__MSC__ /DFX /D__WIN32__ \
                /DWIN32 /DMESA_MINWARN /I..\include

OBJS          = glu.obj mipmap.obj nurbs.obj nurbscrv.obj nurbssrf.obj nurbsutl.obj \
	project.obj quadric.obj tess.obj tess_fist.obj tess_hash.obj tess_heap.obj \
	tess_winding.obj tess_clip.obj

PROGRAM       = ..\lib\GLU32.dll

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS)
                $(link) $(dlllflags) /out:$(PROGRAM) \
                        /def:MesaGLU.def $(OBJS) $(guilibsdll) ..\lib\OpenGL32.lib winmm.lib > link.log