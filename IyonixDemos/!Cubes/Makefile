# Iyonix Mesa application build file

CC = gcc -c -O3 -I../../include -mlibscl
LINK = gcc -o

OBJECTS = cubes.o
LIBS = ^.^.lib.libGL ^.^.lib.libGlu

#----------------------------------------------------------------------

# Rules

!RunImage : $(OBJECTS)
	$(LINK) $@ $(OBJECTS) $(LIBS) -mlibscl

%.o : %.c
	$(CC) $<
