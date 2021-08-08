/* $Id: tess_heap.c,v 1.9.2.4 1999/12/05 02:04:31 gareth Exp $ */

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gluP.h"

#include "tess.h"
#include "tess_macros.h"
#include "tess_heap.h"


/*****************************************************************************
 * Internal definitions:
 *****************************************************************************/

#define PARENT(i)		(((i+1)>>1)-1)
#define LEFT(i)			(((i+1)<<1)-1)
#define RIGHT(i)		((i+1)<<1)



/*****************************************************************************
 *
 *				HEAP FUNCTIONS
 *
 *****************************************************************************/


/*****************************************************************************
 * heapify
 *
 * Heapify the heap :)
 *****************************************************************************/
static void heapify( heap_t *heap, GLint n )
{
    GLint	left = LEFT( n );
    GLint	right = RIGHT( n );
    GLint	largest;

    if ( ( left < heap->size ) &&
	 ( heap->elements[left]->value > heap->elements[n]->value ) )
    {
	largest = left;
    }
    else
    {
	largest = n;
    }

    if ( ( right < heap->size ) &&
	 ( heap->elements[right]->value >
	   heap->elements[largest]->value ) )
    {
	largest = right;
    }

    if ( largest != n )
    {
	heap_elt_t	*element;

	element = heap->elements[n];

	heap->elements[n] = heap->elements[largest];
	heap->elements[n]->index = n;

	heap->elements[largest] = element;
	heap->elements[largest]->index = largest;

	heapify( heap, largest );
    }
}


/*****************************************************************************
 * heap_init
 *
 * Allocate and initialize a new heap.
 *****************************************************************************/
heap_t *heap_init()
{
    heap_t	*heap;
    GLint	i;

    heap = (heap_t *) malloc( sizeof(heap_t) );
    if ( heap == NULL ) {
	return NULL;
    }

    heap->elements = (heap_elt_t **)
	malloc( HEAP_ALLOC * sizeof(heap_elt_t *) );
    if ( heap->elements == NULL ) {
	free( heap );
	return NULL;
    }

    heap->length = HEAP_ALLOC;
    heap->size = 0;
    heap->flags = 0;

    for ( i = 0 ; i < heap->length ; i++ ) {
	heap->elements[i] = NULL;
    }

    return heap;
}


/*****************************************************************************
 * heap_build
 *
 * Build a heap from an unordered array.
 *****************************************************************************/
void heap_build( heap_t *heap )
{
    GLint	i;

    heap->size = heap->length;

    for ( i = PARENT( heap->length ) ; i >= 0 ; i-- ) {
	heapify( heap, i );
    }
}


/*****************************************************************************
 * heap_extract_max
 *
 * Remove and return the maximum element in the heap.
 *****************************************************************************/
heap_elt_t *heap_extract_max( heap_t *heap )
{
    heap_elt_t	*max;

    if ( heap->size < 1 ) {
	return NULL;
    }

    max = heap->elements[0];

    heap->elements[0] = heap->elements[heap->size-1];
    heap->elements[0]->index = 0;

    heap->size--;

    heapify( heap, 0 );

    return max;
}


/*****************************************************************************
 * heap_insert
 *
 * Insert a new element into the heap.
 *****************************************************************************/
GLboolean heap_insert( heap_t *heap, heap_elt_t *element )
{
    GLint	i;

    heap->size++;

    if ( heap->size > heap->length )
    {
	/* Allocate some more space for the heap. */
	if ( ( heap->elements =
	           realloc( heap->elements, ( heap->length + HEAP_ALLOC )
			    * sizeof(heap_elt_t *) ) ) == NULL )
	{
	    return GL_FALSE;
	}

	heap->length += HEAP_ALLOC;
    }

    i = heap->size - 1;

    while ( ( i > 0 ) &&
	    ( heap->elements[PARENT( i )]->value < element->value ) )
    {
	heap->elements[i] = heap->elements[PARENT( i )];
	heap->elements[i]->index = i;

	i = PARENT( i );
    }

    heap->elements[i] = element;
    heap->elements[i]->index = i;

    return GL_TRUE;
}


/*****************************************************************************
 * heap_delete
 *
 * Delete element n from the heap.
 *****************************************************************************/
heap_elt_t *heap_delete( heap_t *heap, GLint n )
{
    heap_elt_t	*element;

    if ( ( heap->size < 1 ) || ( n >= heap->size ) ) {
	return NULL;
    }

    element = heap->elements[n];

    heap->elements[n] = heap->elements[heap->size-1];
    heap->elements[n]->index = n;

    heap->size--;

    heapify( heap, n );

    return element;
}


/*****************************************************************************
 * heap_delete_ptr
 *
 * Delete the element with the given data pointer from the heap.
 *****************************************************************************/
heap_elt_t *heap_delete_ptr( heap_t *heap, void *ptr )
{
    heap_elt_t	*element = NULL;
    GLint	i;

    if ( ( heap->size < 1 ) || ( ptr == NULL ) ) {
	return NULL;
    }

    for ( i = 0 ; i < heap->size ; i++ )
    {
	if ( heap->elements[i]->ptr == ptr ) {
	    element = heap->elements[i];
	    break;
	}
    }

    if ( element != NULL )
    {
	heap->elements[i] = heap->elements[heap->size-1];
	heap->elements[i]->index = i;

	heap->size--;

	heapify( heap, i );
    }

    return element;
}


/*****************************************************************************
 * heap_cleanup
 *
 * Deallocate all memory associated with the heap.
 *****************************************************************************/
void heap_cleanup( heap_t **heap )
{
    GLint	i;

    if ( *heap )
    {
	if ( (*heap)->elements )
	{
	    for ( i = 0; i < (*heap)->size; i++ )
	    {
		if ( (*heap)->elements[i] ) {
		    free( (*heap)->elements[i] );
		}
	    }
	    free( (*heap)->elements );
	}
	free( *heap );
	*heap = NULL;
    }
}
