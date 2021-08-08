/* $Id: tess_hash.h,v 1.7.2.2 1999/11/29 18:16:11 gareth Exp $ */

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
 * GLU 1.3 Polygon Tessellation - Implementation of hash table
 *
 * Gareth Hughes <garethh@bell-labs.com>, August 1999
 *
 *****************************************************************************/

#ifndef __GLU_TESS_HASH_H__
#define __GLU_TESS_HASH_H__

#include "tess_typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HT_DEFAULT_SIZE		9

#define HASH_VERTEX( v )	((tess_vertex_t *) (v)->ptr)

typedef struct hashtable_elt_s
{
    GLint		key;
    void		*ptr;
    struct hashtable_elt_s *prev, *next;
} hashtable_elt_t;

typedef struct hashtable_s
{
    GLint		size;
    GLint		num_elements;
    hashtable_elt_t	**elements;
} hashtable_t;

hashtable_t	*hashtable_init( GLint size );
void		hashtable_insert( hashtable_t *table, GLint key, void *ptr );
GLboolean	hashtable_search( hashtable_t *table, GLint key );
void		*hashtable_element( hashtable_t *table, GLint key );
void		*hashtable_delete( hashtable_t *table, GLint key );

void		hashtable_cleanup( hashtable_t **table );

#ifdef __cplusplus
}
#endif

#endif /* __GLU_TESS_HASH_H__ */
