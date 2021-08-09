# Makefile for Mesa under Acorn RISC OS - aux library

# Author:		David Boddie
# Significant help:	Steven Purchase, Paul Boddie

############################################################################

include ../Make-cfg

INCDIR = ../include

#CC = gcc $(CFLAGS) -I$(INCDIR) -I../src
CC = gcc -c -O3 -I$(INCDIR) -I../src -mlibscl
CC = gcc -c -O3 -mcpu=xscale -I$(INCDIR) -I../src -mlibscl

OBJECTS = glu.o mipmap.o nurbs.o nurbscrv.o nurbssrf.o nurbsutl.o \
	project.o quadric.o tess.o tess_clip.o tess_fist.o tess_hash.o \
	tess_heap.o tess_winding.o
	
AR_OBJECTS = o.glu o.mipmap o.nurbs o.nurbscrv o.nurbssrf o.nurbsutl \
	o.project o.quadric o.tess o.tess_clip o.tess_fist o.tess_hash \
	o.tess_heap o.tess_winding

############################################################################
# Rules

../libGlu.lib : $(AR_OBJECTS)
	$(MAKELIB) ^.lib.libGlu $(AR_OBJECTS)

.SUFFIXES : .o .c

%.o : %.c
	$(CC) $<

clean :
	wipe o.* ~C ~V
