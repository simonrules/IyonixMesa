/* $Id: tess_clip.h,v 1.1.2.1 1999/11/15 21:21:31 gareth Exp $ */

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

/*****************************************************************************
 *
 * GLU 1.3 Polygon Tessellation - Implementation of polygon clipping
 *
 * Gareth Hughes <garethh@bell-labs.com>, November 1999
 *
 * Originally based on a modified version of the algorithms in the Generic
 * Polygon Clipper (GPC), which are a modified version of the algorithms in
 * Vatti 1992.
 *
 * GPC is Copyright (C) 1997-1999 Alan Murta (amurta@cs.man.ac.uk),
 * Advanced Interfaces Group, University of Manchester.
 *
 *****************************************************************************/

#ifndef __GLU_TESS_CLIP_H__
#define __GLU_TESS_CLIP_H__

#ifdef __cplusplus
extern "C" {
#endif

GLenum tess_clip_polygons( GLUtesselator *tobj );

#ifdef __cplusplus
}
#endif

#endif
