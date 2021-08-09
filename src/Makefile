# Makefile for Mesa under Acorn RISC OS - GL library

# Author:		David Boddie
# Significant help:	Steven Purchase, Paul Boddie
# Last updated:		Fri 04th May 2001 (for 3.4.1)

############################################################################

include ../Make-cfg

INCDIR = ../include

#CC = gcc -c -munixlib -mapcs -O3 -DUSE_ARM_ASM -I$(INCDIR) -I../src
#CC = gcc -c -munixlib -mapcs -DUSE_ARM_ASM -I$(INCDIR) -I../src
#CC = gcc -c -O3 -I$(INCDIR) -I../src -mlibscl
CC = gcc -c -mcpu=xscale -I$(INCDIR) -I../src -mlibscl

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
	OSmesa/osmesa.o \
	Iyonix/nv_3da.o \
	Iyonix/nv_support.o \
	Iyonix/nv_globals.o \
	Iyonix/nv_general.o \
	Iyonix/riva_ctx.o \
	Iyonix/riva_glh.o \
	Iyonix/riva_prim.o \
	Iyonix/riva_symbols.o \
	Iyonix/riva_tex.o \
	Iyonix/glh_mm.o \
	Iyonix/glh.o \
	Iyonix/nv_acc.o \
	Iyonix/ddiyonix.o
	
AR_CORE_OBJECTS = o.accum o.alpha o.alphabuf o.api1 o.api2 o.apiext o.attrib o.bbox \
               o.bitmap o.blend o.clip o.colortab \
               o.config o.context o.copypix o.cva o.debug_xform o.depth \
               o.dlist o.drawpix o.enable o.enums o.eval \
               o.extensions o.feedback o.fog o.get \
               o.glmisc o.hash o.image \
               o.light o.lines o.logic o.masking o.matrix \
               o.mmath o.mthreads o.pb o.pixel o.pipeline o.pointers \
               o.points o.polygon o.quads o.rastpos o.readpix o.rect \
               o.scissor o.shade o.span o.stages o.stencil \
               o.teximage o.texobj o.texstate o.texture \
               o.translate o.triangle o.varray o.vb o.vbcull o.vbfill \
               o.vbindirect o.vbrender o.vbxform o.vector o.vertices o.winpos \
               o.xform o.zoom
               
AR_DRIVER_OBJECTS = \
	OSmesa.o.osmesa \
	Iyonix.o.nv_3da \
	Iyonix.o.nv_support \
	Iyonix.o.nv_globals \
	Iyonix.o.nv_general \
	Iyonix.o.riva_ctx \
	Iyonix.o.riva_glh \
	Iyonix.o.riva_prim \
	Iyonix.o.riva_symbols \
	Iyonix.o.riva_tex \
	Iyonix.o.glh_mm \
	Iyonix.o.glh \
	Iyonix.o.nv_acc \
	Iyonix.o.ddiyonix

OBJECTS = $(CORE_OBJECTS) $(DRIVER_OBJECTS)

AR_OBJECTS = $(AR_CORE_OBJECTS) $(AR_DRIVER_OBJECTS)

############################################################################
# Rules

#default : ../lib/libGL

../lib/libGL : $(OBJECTS)
	$(MAKELIB) ^.lib.libGL $(AR_OBJECTS)

.SUFFIXES : .o .c

%.o : %.c
	$(CC) $< -o $@

clean :
	wipe *.o ~C ~V
