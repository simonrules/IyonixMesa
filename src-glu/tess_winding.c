/* $Id: tess_winding.c,v 1.10.2.6 1999/12/05 07:10:49 gareth Exp $ */

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
 * GLU 1.3 Polygon Tessellation - Implementation of winding rules
 *
 * Gareth Hughes <garethh@bell-labs.com>, August 1999
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gluP.h"

#include "tess.h"
#include "tess_macros.h"
#include "tess_hash.h"
#include "tess_heap.h"
#include "tess_winding.h"


/*****************************************************************************
 * Internal definitions:
 *****************************************************************************/
#define INSIDE		1
#define OUTSIDE		2



/*****************************************************************************
 *
 *			CONTOUR PREPROCESSING FUNCTIONS
 *
 *****************************************************************************/


/*****************************************************************************
 * point_contour_test
 *
 * Determine if the given point lies inside the given contour.  Taken from
 *  O'Rourke 1998 p244.
 *****************************************************************************/
GLboolean point_contour_test( tess_contour_t *contour, tess_vertex_t *point )
{
    tess_vertex_t	*vertex = contour->vertices;
    GLint		rcross = 0, lcross = 0;
    GLboolean		rstrad, lstrad;
    GLdouble		x;
    GLint		i;

    MSG( 1, "             comparing c: %d p: %d ...\n",
	 contour->vertices->index, point->index );

    for ( i = 0 ; i < contour->num_vertices ; i++ )
    {
	/*
	 * First check if given point is coicident with a vertex on
	 *  the contour.
	 */
	if ( vertex->index == point->index ) {
	    MSG( 1, "               p: %d coincident\n", point->index );
	    return GL_TRUE;
	}

	/*
	 * Check if current edge straddles the horizontal line through
	 *  the given point, with bias above/below.
	 */
	rstrad = (GLboolean)( ( vertex->v[Y] > point->v[Y] ) !=
			      ( vertex->next->v[Y] > point->v[Y] ) );
	lstrad = (GLboolean)( ( vertex->v[Y] < point->v[Y] ) !=
			      ( vertex->next->v[Y] < point->v[Y] ) );

	if ( rstrad || lstrad )
	{
	    MSG( 1, "               edge: %d -> %d r: %d l: %d\n",
		 vertex->index, vertex->next->index, rstrad, lstrad );
	    /*
	     * Compute intersection of current edge with the horizontal
	     *  line through the given point.
	     */
	    x = point->v[X] +
		( ( vertex->v[X] - point->v[X] ) *
		  ( vertex->next->v[Y] - point->v[Y] ) -
		  ( vertex->next->v[X] - point->v[X] ) *
		  ( vertex->v[Y] - point->v[Y] ) )
		/ ( vertex->next->v[Y] - vertex->v[Y] );

	    if ( rstrad && ( x > point->v[X] ) ) rcross++;
	    if ( lstrad && ( x < point->v[X] ) ) lcross++;

	    MSG( 1, "               x: %.2f p: %.2f rc: %d lc: %d\n",
		 x, point->v[X], rcross, lcross );
	}

	vertex = vertex->next;
    }

    MSG( 1, "             rc: %d lc: %d\n", rcross, lcross );

    /*
     * Given point is on an edge if left/right cross counts are not the
     *  same parity.
     */
    if ( ( rcross % 2 ) != ( lcross % 2 ) ) {
	MSG( 1, "               p: %d lies on edge\n", point->index );
	return GL_FALSE;
    }

    /*
     * Given point
     */
    if ( ( rcross % 2 ) == 1 ) {
	MSG( 1, "               p: %d strictly inside\n", point->index );
	return GL_TRUE;
    } else {
	MSG( 1, "               p: %d strictly outside\n", point->index );
	return GL_FALSE;
    }
}

/*****************************************************************************
 * label_contours
 *
 * Label a set of non-intersecting contours as INSIDE or OUTSIDE.
 *****************************************************************************/
static void label_contours( GLUtesselator *tobj )
{
    tess_contour_t	*contour = tobj->contours, *current;
    GLint		i;

    MSG( 1, "    -> label_contours( tobj:%p c: %p )\n", tobj, contour );

    for ( i = 0 ; i < tobj->num_contours ; i++ )
    {
	/*
	 * As our contours have been sorted by size, we can use the first
	 * contour as our outer region.
	 */
	contour->label = OUTSIDE;
	contour->winding = ( contour->orientation == GLU_CCW ) ? 1 : -1;

	if ( contour != tobj->contours )
	{
	    current = contour->prev;
	    contour->parent = NULL;

	    while ( current != contour )
	    {
		if ( point_contour_test( current, contour->vertices ) )
		{
		    contour->label = INSIDE;
		    contour->parent = current;
		    break;
		}

		current = current->prev;
	    }

	    if ( contour->parent != NULL ) {
		contour->winding += contour->parent->winding;
	    }
	}

	MSG( 1, "           contour %s w: %d orient: %s\n\n",
	     ( contour->label == OUTSIDE ) ? "OUTSIDE" : "INSIDE ",
	     contour->winding,
	     ( contour->orientation == GLU_CCW ) ? "CCW" : "CW" );

	contour = contour->next;
    }

    MSG( 1, "    <- label_contours( tobj:%p )\n", tobj );
}

/*****************************************************************************
 * remove_contour
 *****************************************************************************/
static void remove_contour( GLUtesselator *tobj, tess_contour_t *contour )
{
    if ( tobj->contours == contour ) {
	tobj->contours = contour->next;
    }
    if ( tobj->last_contour == contour ) {
	tobj->last_contour = contour->prev;
    }

    contour->prev->next = contour->next;
    contour->next->prev = contour->prev;

    tobj->num_contours--;
    tobj->num_vertices -= contour->num_vertices;
}

/*****************************************************************************
 * collect_contours
 *****************************************************************************/
static void collect_contours( GLUtesselator *tobj )
{
    tess_contour_t	*contour = tobj->contours, *next;
    GLint		i;

    MSG( 1, "    -> collect_contours( tobj:%p )\n", tobj );

#ifdef DEBUG
    switch( tobj->winding_rule )
    {
    case GLU_TESS_WINDING_ODD:
	MSG( 1, "         using ODD winding rule\n" );
	break;
    case GLU_TESS_WINDING_NONZERO:
	MSG( 1, "         using NONZERO winding rule\n" );
	break;
    case GLU_TESS_WINDING_POSITIVE:
	MSG( 1, "         using POSITIVE winding rule\n" );
	break;
    case GLU_TESS_WINDING_NEGATIVE:
	MSG( 1, "         using NEGATIVE winding rule\n" );
	break;
    case GLU_TESS_WINDING_ABS_GEQ_TWO:
	MSG( 1, "         using ABS_GEQ_TWO winding rule\n" );
	break;

    default:
	MSG( 1, "         using unknown winding rule\n" );
	break;
    }
#endif

    for ( i = 0; i < tobj->num_contours; i++ )
    {
	next = contour->next;

#ifdef DEBUG
	switch( contour->label )
	{
	case INSIDE:
	    MSG( 1, "           contour: %d label: INSIDE  %s %d\n",
		 contour->vertices->index, ( contour->orientation == GLU_CCW ) ? "CCW" : "CW", contour->winding );
	    break;
	case OUTSIDE:
	    MSG( 1, "           contour: %d label: OUTSIDE %s %d\n",
		 contour->vertices->index, ( contour->orientation == GLU_CCW ) ? "CCW" : "CW", contour->winding );
	    break;

	default:
	    MSG( 1, "           contour: %p unknown label\n", contour );
	    break;
	}
#endif
	switch ( tobj->winding_rule )
	{
	case GLU_TESS_WINDING_ODD:
	    if ( ( ABSI( contour->winding ) % 2 ) == 1 )
	    {
		if ( contour->orientation != tobj->orientation )
		{
		    MSG( 1, "             rev CW -> CCW\n" );
		    reverse_contour( contour );
		}
	    }
	    else
	    {
		if ( contour->orientation == tobj->orientation )
		{
		    MSG( 1, "             rev CCW -> CW\n" );
		    reverse_contour( contour );
		}
		else if ( tobj->contours == contour )
		{
		    MSG( 1, "             deleting contour...\n" );
		    remove_contour( tobj, contour );
		    delete_contour( &contour );
		    i--;
		}
	    }
	    break;

	case GLU_TESS_WINDING_NONZERO:
	    if ( ABSI( contour->winding ) == 1 )
	    {
		if ( contour->orientation != tobj->orientation )
		{
		    MSG( 1, "             rev CW -> CCW\n" );
		    reverse_contour( contour );
		}
	    }
	    else if ( contour->winding == 0 )
	    {
		if ( contour->orientation == tobj->orientation )
		{
		    MSG( 1, "             rev CCW -> CW\n" );
		    reverse_contour( contour );
		}
		else if ( tobj->contours == contour )
		{
		    MSG( 1, "             deleting contour...\n" );
		    remove_contour( tobj, contour );
		    delete_contour( &contour );
		    i--;
		}
	    }
	    else if ( ABSI( contour->winding ) > 1 )
	    {
		MSG( 1, "             deleting contour...\n" );
		remove_contour( tobj, contour );
		delete_contour( &contour );
		i--;
	    }
	    break;

	case GLU_TESS_WINDING_POSITIVE:
	    if ( contour->winding == 1 )
	    {
		if ( contour->orientation != tobj->orientation )
		{
		    MSG( 1, "             rev CW -> CCW\n" );
		    reverse_contour( contour );
		}
	    }
	    else if ( contour->winding == 0 )
	    {
		if ( contour->orientation == tobj->orientation )
		{
		    MSG( 1, "             rev CCW -> CW\n" );
		    reverse_contour( contour );
		}
		else if ( tobj->contours == contour )
		{
		    MSG( 1, "             deleting contour...\n" );
		    remove_contour( tobj, contour );
		    delete_contour( &contour );
		    i--;
		}
	    }
	    else if ( ( contour->winding < 0 ) ||
		      ( contour->winding > 1 ) )
	    {
		MSG( 1, "             deleting contour...\n" );
		remove_contour( tobj, contour );
		delete_contour( &contour );
		i--;
	    }
	    break;

	case GLU_TESS_WINDING_NEGATIVE:
	    if ( contour->winding == -1 )
	    {
		if ( contour->orientation != tobj->orientation )
		{
		    MSG( 1, "             rev CW -> CCW\n" );
		    reverse_contour( contour );
		}
	    }
	    else if ( contour->winding == -2 )
	    {
		if ( contour->orientation == tobj->orientation )
		{
		    MSG( 1, "             rev CCW -> CW\n" );
		    reverse_contour( contour );
		}
		else if ( tobj->contours == contour )
		{
		    MSG( 1, "             deleting contour...\n" );
		    remove_contour( tobj, contour );
		    delete_contour( &contour );
		    i--;
		}
	    }
	    else if ( ( contour->winding < -2 ) ||
		      ( contour->winding > -1 ) )
	    {
		MSG( 1, "             deleting contour...\n" );
		remove_contour( tobj, contour );
		delete_contour( &contour );
		i--;
	    }
	    break;

	case GLU_TESS_WINDING_ABS_GEQ_TWO:
	    if ( ABSI( contour->winding ) == 2 )
	    {
		if ( contour->orientation != tobj->orientation )
		{
		    MSG( 1, "             rev CW -> CCW\n" );
		    reverse_contour( contour );
		}
	    }
	    else if ( ABSI( contour->winding ) == 1 )
	    {
		if ( ( contour->orientation == tobj->orientation ) &&
		     ( contour->label == INSIDE ) )
		{
		    MSG( 1, "             rev CCW -> CW\n" );
		    reverse_contour( contour );
		}
		else if ( tobj->contours == contour )
		{
		    MSG( 1, "             deleting contour...\n" );
		    remove_contour( tobj, contour );
		    delete_contour( &contour );
		    i--;
		}
	    }
	    else if ( ( contour->winding == 0 ) ||
		      ( ABSI( contour->winding > 2 ) ) )
	    {
		MSG( 1, "             deleting contour...\n" );
		remove_contour( tobj, contour );
		delete_contour( &contour );
		i--;
	    }
	    break;

	default:
	    break;
	}

	contour = next;
    }

    MSG( 1, "    <- collect_contours( tobj:%p ) count: %d\n", tobj, tobj->num_contours );
}

/*****************************************************************************
 * cleanup
 *
 * Deallocate any memory that we've used in the contour preprocessing.
 *****************************************************************************/
static void cleanup( GLUtesselator *tobj )
{
    /* FIXME: We don't seem to need to do anything in here anymore... */
}

/*****************************************************************************
 * tess_preprocess_contours
 *****************************************************************************/
GLenum tess_preprocess_contours( GLUtesselator *tobj )
{
    label_contours( tobj );

    collect_contours( tobj );

    cleanup( tobj );

    return GLU_NO_ERROR;
}
