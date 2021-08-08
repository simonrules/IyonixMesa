/* $Id: tess_heap.h,v 1.7.2.3 1999/11/29 18:16:12 gareth Exp $ */

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
 * GLU 1.3 Polygon Tessellation - Implementation of priority queue
 *
 * Gareth Hughes <garethh@bell-labs.com>, April 1999
 *
 *****************************************************************************/

#ifndef __GLU_TESS_HEAP_H__
#define __GLU_TESS_HEAP_H__

#include "tess_typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HEAP_ALLOC		8

#define HEAP_VERTEX( v )	((tess_vertex_t *) (v)->ptr)

typedef struct heap_elt_s
{
    GLint		index;
    GLdouble		value;
    void		*ptr;
    struct heap_elt_s	*next, *prev;
} heap_elt_t;

typedef struct heap_s
{
    GLint		length;
    GLint		size;
    GLint		flags;
    heap_elt_t		**elements;
} heap_t;

heap_t		*heap_init();
void		heap_build( heap_t *heap );
GLboolean	heap_insert( heap_t *heap, heap_elt_t *element );
heap_elt_t	*heap_extract_max( heap_t *heap );
heap_elt_t	*heap_delete( heap_t *heap, GLint n );
heap_elt_t	*heap_delete_ptr( heap_t *heap, void *ptr );

void		heap_cleanup( heap_t **heap );

#ifdef __cplusplus
}
#endif

#endif /* __GLU_TESS_HEAP_H__ */
