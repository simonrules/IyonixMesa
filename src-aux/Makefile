# Makefile for Mesa under Acorn RISC OS - aux library

# Author:		David Boddie
# Significant help:	Steven Purchase, Paul Boddie

############################################################################

include ../Make-cfg

INCDIR = ../include

#CC = gcc -O3 $(CFLAGS) -I$(INCDIR) -I../src
CC = gcc -c -O3 -I$(INCDIR) -I../src -mlibscl
CC = gcc -c -O3 -mcpu=xscale -I$(INCDIR) -I../src -mlibscl

OBJECTS = shapes.o teapot.o vect3d.o xxform.o

AR_OBJECTS = o.shapes o.teapot o.vect3d o.xxform

############################################################################
# Rules

../libAux.lib : $(AR_OBJECTS)
	$(MAKELIB) ^.lib.libAux $(AR_OBJECTS)

.SUFFIXES : .o .c

%.o : %.c
	$(CC) $<

clean :
	wipe o.* ~C ~V
