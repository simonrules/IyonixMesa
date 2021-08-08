/* $Id: hash.h,v 1.1.1.1.2.1 2000/01/24 16:20:17 brianp Exp $ */

/*
 * Mesa 3-D graphics library
 * Version:  3.2
 * 
 * Copyright (C) 1999-2000  Brian Paul   All Rights Reserved.
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


#ifndef HASH_H
#define HASH_H


#include "GL/gl.h"


struct _mesa_HashTable;



extern struct _mesa_HashTable *_mesa_NewHashTable(void);

extern void _mesa_DeleteHashTable(struct _mesa_HashTable *table);

extern void *_mesa_HashLookup(const struct _mesa_HashTable *table, GLuint key);

extern void _mesa_HashInsert(struct _mesa_HashTable *table, GLuint key, void *data);

extern void _mesa_HashRemove(struct _mesa_HashTable *table, GLuint key);

extern GLuint _mesa_HashFirstEntry(const struct _mesa_HashTable *table);

extern void _mesa_HashPrint(const struct _mesa_HashTable *table);

extern GLuint _mesa_HashFindFreeKeyBlock(const struct _mesa_HashTable *table, GLuint numKeys);


#endif
