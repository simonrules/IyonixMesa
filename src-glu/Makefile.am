## Process this file with automake to produce Makefile.in

#AUTOMAKE_OPTIONS = no-dependencies

INCLUDES = -I$(top_srcdir)/include

# Build a libtool library for installation in libdir.
lib_LTLIBRARIES = libGLU.la

libGLU_la_LDFLAGS = -version-info $(LIBGLU_VERSION)
libGLU_la_SOURCES = \
	all.h \
	glu.c \
	gluP.h \
	mipmap.c \
	nurbs.c \
	nurbs.h \
	nurbscrv.c \
	nurbssrf.c \
	nurbsutl.c \
	project.c \
	quadric.c \
	tess.c \
	tess_fist.c \
	tess_hash.c \
	tess_heap.c \
	tess_winding.c \
	tess_clip.c

strip:
	-if strip -V 2>&1  | grep "GNU" > /dev/null; then \
		strip --strip-unneeded .libs/*.so; \
		strip --strip-debug .libs/*.a; \
	fi
