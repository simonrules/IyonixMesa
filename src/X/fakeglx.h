/* $Id: fakeglx.h,v 1.3.2.2 2000/02/23 23:06:55 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.1
 * 
 * Copyright (C) 1999  Brian Paul   All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */





/*
 * GLX API functions which either call fake or real GLX implementations
 */



#ifndef FAKEGLX_H
#define FAKEGLX_H


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "GL/glx.h"


extern XVisualInfo *Fake_glXChooseVisual( Display *dpy,
                                          int screen, int *list );


extern int Fake_glXGetConfig( Display *dpy, XVisualInfo *visinfo,
                              int attrib, int *value );


extern GLXContext Fake_glXCreateContext( Display *dpy, XVisualInfo *visinfo,
                                         GLXContext shareList, Bool direct );


extern void Fake_glXDestroyContext( Display *dpy, GLXContext ctx );


extern void Fake_glXCopyContext( Display *dpy, GLXContext src, GLXContext dst,
                                 unsigned long mask );


extern Bool Fake_glXMakeCurrent( Display *dpy, GLXDrawable drawable,
                                 GLXContext ctx );


extern GLXContext Fake_glXGetCurrentContext( void );


extern GLXDrawable Fake_glXGetCurrentDrawable( void );


extern GLXPixmap Fake_glXCreateGLXPixmap( Display *dpy, XVisualInfo *visinfo,
                                          Pixmap pixmap );


extern void Fake_glXDestroyGLXPixmap( Display *dpy, GLXPixmap pixmap );


extern Bool Fake_glXQueryExtension( Display *dpy, int *errorb, int *event );


extern Bool Fake_glXIsDirect( Display *dpy, GLXContext ctx );


extern void Fake_glXSwapBuffers( Display *dpy, GLXDrawable drawable );


extern void Fake_glXCopySubBufferMESA( Display *dpy, GLXDrawable drawable,
                                       int x, int y, int width, int height );


extern Bool Fake_glXQueryVersion( Display *dpy, int *maj, int *min );


extern void Fake_glXUseXFont( Font font, int first, int count, int listBase );


extern void Fake_glXWaitGL( void );


extern void Fake_glXWaitX( void );


/* GLX 1.1 and later */
extern const char *Fake_glXQueryExtensionsString( Display *dpy, int screen );


/* GLX 1.1 and later */
extern const char *Fake_glXQueryServerString( Display *dpy, int screen,
                                              int name );


/* GLX 1.1 and later */
extern const char *Fake_glXGetClientString( Display *dpy, int name );


#ifdef GLX_MESA_release_buffers
extern Bool Fake_glXReleaseBuffersMESA( Display *dpy, Window w );
#endif


#ifdef GLX_MESA_pixmap_colormap
extern GLXPixmap Fake_glXCreateGLXPixmapMESA( Display *dpy,
                                              XVisualInfo *visinfo,
                                              Pixmap pixmap, Colormap cmap );
#endif


#ifdef GLX_MESA_set_3dfx_mode
extern GLboolean Fake_glXSet3DfxModeMESA( GLint mode );
#endif


#ifdef GLX_ARB_get_proc_address
extern void (*Fake_glXGetProcAddress( const GLubyte *procName ))();
#endif

#endif
