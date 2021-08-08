/* $Id: tess_typedefs.h,v 1.9.2.5 1999/12/05 17:01:17 gareth Exp $ */

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
 * GLU 1.3 Polygon Tessellation by Gareth Hughes <garethh@bell-labs.com>
 *
 *****************************************************************************/

#ifndef __GLU_TESS_TYPEDEFS_H__
#define __GLU_TESS_TYPEDEFS_H__

#include <stdarg.h>
#include <stdio.h>

#include "gluP.h"

#include "tess_hash.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Tessellation type definitions:
 *****************************************************************************/
typedef struct
{
    void (GLCALLBACK *begin)( GLenum );
    void (GLCALLBACK *beginData)( GLenum , void * );
    void (GLCALLBACK *edgeFlag)( GLboolean );
    void (GLCALLBACK *edgeFlagData)( GLboolean , void * );
    void (GLCALLBACK *vertex)( void * );
    void (GLCALLBACK *vertexData)( void *, void * );
    void (GLCALLBACK *end)( void );
    void (GLCALLBACK *endData)( void * );
    void (GLCALLBACK *error)( GLenum );
    void (GLCALLBACK *errorData)( GLenum , void * );
    void (GLCALLBACK *combine)( GLdouble[3], void *[4],
				GLfloat[4], void ** );
    void (GLCALLBACK *combineData)( GLdouble[3], void *[4],
				    GLfloat[4], void **,
				    void * );
} tess_callbacks_t;

typedef struct
{
    GLdouble		normal[3];
    GLdouble		dist;
} tess_plane_t;

typedef struct vertex_s
{
    GLint		index;
    void		*data;
    GLdouble		coords[3];
    GLdouble		v[2];
    GLboolean		edge_flag;
    GLdouble		side;
    GLdouble		angle;
    struct vertex_s	*next, *prev;
} tess_vertex_t;

typedef struct contour_s
{
    GLenum		type;
    tess_plane_t	plane;
    GLdouble		area;
    GLenum		orientation;
    GLuint		label;
    GLint		winding;
    GLdouble		rotx, roty;
    GLdouble		mins[2], maxs[2];
    GLint		num_vertices;
    tess_vertex_t	*vertices, *last_vertex;
    hashtable_t		*reflex_vertices;
    struct contour_s	*parent;
    struct contour_s	*next, *prev;
} tess_contour_t;

#ifdef __cplusplus
}
#endif

#endif /* __GLU_TESS_TYPEDEFS_H__ */
