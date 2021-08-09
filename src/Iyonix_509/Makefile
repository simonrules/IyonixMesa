# Makefile for Mesa under Acorn RISC OS - GL library

# Author:		David Boddie
# Significant help:	Steven Purchase, Paul Boddie
# Last updated:		Fri 04th May 2001 (for 3.4.1)

############################################################################

include ../../Make-cfg

INCDIR = ../../include

#CC = gcc -c -munixlib -mapcs -O3 -DUSE_ARM_ASM -I$(INCDIR) -I../src
#CC = gcc -c -munixlib -mapcs -DUSE_ARM_ASM -I$(INCDIR) -I../
CC = gcc -c -I$(INCDIR) -I../ -mlibscl

CORE_OBJECTS = accum.o alpha.o alphabuf.o api1.o api2.o apiext.o attrib.o bbox.o \
               bitmap.o blend.o clip.o colortab.o \
               config.o context.o copypix.o cva.o debug_xform.o depth.o \
               dlist.o drawpix.o enable.o enums.o eval.o \
               extensions.o feedback.o fog.o get.o \
               glmisc.o hash.o image.o \
               light.o lines.o logic.o masking.o matrix.o \
               mmath.o mthreads.o pb.o pixel.o pipeline.o pointers.o \
               points.o polygon.o quads.o rastpos.o readpix.o rect.o \
               scissor.o shade.o span.o stages.o stencil.o \
               teximage.o texobj.o texstate.o texture.o \
               translate.o triangle.o varray.o vb.o vbcull.o vbfill.o \
               vbindirect.o vbrender.o vbxform.o vector.o vertices.o winpos.o \
               xform.o zoom.o

DRIVER_OBJECTS = \
	nv_3da.o \
	nv_support.o \
	nv_globals.o \
	nv_general.o \
	riva_ctx.o \
	riva_glh.o \
	riva_prim.o \
	riva_symbols.o \
	riva_tex.o \
	glh_mm.o \
	glh.o \
	nv_acc.o \
	ddiyonix.o

OBJECTS = $(DRIVER_OBJECTS)

############################################################################
# Rules

.SUFFIXES : .o .c

rosmesa.o : RISCOS/rosmesa.c
	$(CC) RISCOS/rosmesa.c -O3

ros_asm.o : RISCOS/ros_asm.s
	$(AS) RISCOS/ros_asm.s -o ros_asm.o

x86.o : X86/x86.c
	$(CC) X86/x86.c

# o.xform : RISCOS.o.xform_asm RISCOS.o.xform_c
# 	ld -aof -o $@ o.xform_asm o.xform_c

# RISCOS.o.xform_asm : RISCOS.s.xform_asm
# 	$(AS) RISCOS/xform_asm.s -o o.xform_asm

# RISCOS.o.xform_c : RISCOS.c.xform_c
# 	$(CC) RISCOS/xform_c.c -o o.xform_c

%.o : %.c
	$(CC) $<

clean :
	wipe *.o ~C ~V
