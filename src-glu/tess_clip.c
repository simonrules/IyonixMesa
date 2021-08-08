/* $Id: tess_clip.c,v 1.1.2.13 2000/03/04 00:15:03 gareth Exp $ */

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "gluP.h"

#include "tess.h"
#include "tess_macros.h"
#include "tess_clip.h"


/*****************************************************************************
 * Internal definitions:
 *****************************************************************************/

#define LEFT			0
#define RIGHT			1

#define ABOVE			0
#define BELOW			1

#define CLIP               	1
#define SUBJ               	0

#define BOT               	0
#define TOP               	1

/* Edge intersection classes: */
#define EDGE_EMPTY		0x0
#define EDGE_EXT_MAX		0x1
#define EDGE_EXT_LI		0x2
#define EDGE_TOP		0x3
#define EDGE_EXT_RI		0x4
#define EDGE_RIGHT		0x5
#define EDGE_INT_MAX_MIN	0x6
#define EDGE_INT_MIN		0x7
#define EDGE_EXT_MIN		0x8
#define EDGE_EXT_MAX_MIN	0x9
#define EDGE_LEFT		0xa
#define EDGE_INT_LI		0xb
#define EDGE_BOTTOM		0xc
#define EDGE_INT_RI		0xd
#define EDGE_INT_MAX		0xe
#define EDGE_FULL		0xf

/* Edge bundle states: */
#define UNBUNDLED		0
#define BUNDLE_HEAD		1
#define BUNDLE_TAIL		2

/* Horizontal edge states: */
#define HORIZ_NONE		0
#define HORIZ_BOT		1
#define HORIZ_TOP		2

/* Winding number quadrants: */
#define WIND_NE			0
#define WIND_SE			1
#define WIND_SW			2
#define WIND_NW			3

/* Horizontal edge state transitions within scanbeam boundary: */
const GLuint next_state[3][6] =
{
    { HORIZ_BOT, HORIZ_TOP,    HORIZ_TOP, HORIZ_BOT,    HORIZ_NONE, HORIZ_NONE },
    { HORIZ_NONE, HORIZ_NONE,  HORIZ_NONE, HORIZ_NONE,  HORIZ_TOP, HORIZ_TOP },
    { HORIZ_NONE, HORIZ_NONE,  HORIZ_NONE, HORIZ_NONE,  HORIZ_BOT, HORIZ_BOT }
};

#define OPTIMAL( vertex )						\
	    ( ( vertex->v[Y] != vertex->prev->v[Y] ) ||			\
	      ( vertex->v[Y] != vertex->next->v[Y] ) )

#define FWD_MIN( edge )							\
	    ( ( edge->prev->vertex->v[Y] >= edge->vertex->v[Y] ) &&	\
	      ( edge->next->vertex->v[Y] > edge->vertex->v[Y] ) )

#define FWD_MAX( edge )							\
	    ( edge->next->vertex->v[Y] <= edge->vertex->v[Y] )

#define REV_MIN( edge )							\
	    ( ( edge->prev->vertex->v[Y] > edge->vertex->v[Y] ) &&	\
	      ( edge->next->vertex->v[Y] >= edge->vertex->v[Y] ) )

#define REV_MAX( edge )							\
	    ( edge->prev->vertex->v[Y] <= edge->vertex->v[Y] )


/*****************************************************************************
 * Internal type definitions:
 *****************************************************************************/

typedef struct tess_edge_s
{
    tess_vertex_t	*vertex;
    struct tess_edge_s	*prev;
    struct tess_edge_s	*next;
} tess_edge_t;

typedef struct clip_contour_s
{
    GLint		active;
    GLint		hole;
    tess_vertex_t	*vertices[2];
    tess_vertex_t	*floating[2];
    struct clip_contour_s *next;
    struct clip_contour_s *proxy;
} clip_contour_t;

typedef struct edge_node_s
{
    tess_vertex_t	*vertices[2];
    GLdouble		bot[2];
    GLdouble		top[2];
    GLdouble		xbot;
    GLdouble		xtop;
    GLdouble		dx;
    GLint		type;
    GLboolean		forward;
    GLboolean		bundle[2][2];
    GLuint		bside[2];
    GLuint		bstate[2];
    clip_contour_t	*out[2];
    struct edge_node_s	*prev;
    struct edge_node_s	*next;
    struct edge_node_s	*pred;
    struct edge_node_s	*succ;
    struct edge_node_s	*next_bound;
} edge_node_t;

typedef struct lmt_node_s
{
    GLdouble		y;
    edge_node_t		*bounds;
    struct lmt_node_s	*next;
} lmt_node_t;

typedef struct sb_tree_s
{
    GLdouble		y;
    struct sb_tree_s	*less;
    struct sb_tree_s	*more;
} sb_tree_t;

typedef struct it_node_s
{
    edge_node_t		*ie[2];
    GLdouble		v[2];
    GLdouble		coords[3];
    void		*data;
    struct it_node_s	*next;
} it_node_t;

typedef struct st_node_s
{
    edge_node_t		*edge;
    GLdouble		xbot;
    GLdouble		xtop;
    GLdouble		dx;
    struct st_node_s    *prev;
} st_node_t;


/*****************************************************************************
 *
 *			POLYGON CLIPPING FUNCTIONS
 *
 *****************************************************************************/


/*****************************************************************************
 * insert_bound
 *
 * Insert the given bound e (where a bound is the set of edges from a local
 * minimum to a local maximum) into the edge table b.
 *****************************************************************************/
static void insert_bound( edge_node_t **b, edge_node_t *e )
{
    edge_node_t		*bound;

    if ( !*b )
    {
	MSG( 1, "                  bound() new tail (%.2f, %.2f)\n",
	     e->bot[X], e->bot[Y] );

	/* Link node e to the tail of the list */
	*b = e;
    }
    else
    {
	/* Do primary sort on the x field */
	if ( e->bot[X] < (*b)->bot[X] )
	{
	    MSG( 1, "                  bound() x less, insert (%.2f, %.2f)\n",
		 e->bot[X], e->bot[Y] );

	    /* Insert a new node mid-list */
	    bound = *b;
	    *b = e;
	    (*b)->next_bound = bound;
	}
	else
	{
	    if ( e->bot[X] == (*b)->bot[X] )
	    {
		/* Do secondary sort on the dx field */
		if ( e->dx < (*b)->dx )
		{
		    MSG( 1, "                  bound() dx less, insert (%.2f, %.2f)\n",
			 e->bot[X], e->bot[Y] );

		    /* Insert a new node mid-list */
		    bound = *b;
		    *b = e;
		    (*b)->next_bound = bound;
		}
		else
		{
		    /* Head further down the list */
		    insert_bound( &((*b)->next_bound), e );
		}
	    }
	    else
	    {
		/* Head further down the list */
		insert_bound( &((*b)->next_bound), e );
	    }
	}
    }
}

/*****************************************************************************
 * bound_list
 *
 * Return the bound list for the left and right bounds of a given local
 * minima table entry.
 *****************************************************************************/
static edge_node_t **bound_list( lmt_node_t **lmt, tess_vertex_t *vertex )
{
    lmt_node_t		*node;

    if ( !*lmt )
    {
	MSG( 1, "                  bound_list() new tail node\n" );

	/* Add node onto the tail end of the LMT */
	*lmt = (lmt_node_t *) malloc( sizeof(lmt_node_t) );

	(*lmt)->y = vertex->v[Y];
	(*lmt)->bounds = NULL;
	(*lmt)->next = NULL;

	return &((*lmt)->bounds);
    }
    else
    {
	if ( vertex->v[Y] < (*lmt)->y )
	{
	    MSG( 1, "                  bound_list() new node before y: %.2f\n",
		 (*lmt)->y );

	    /* Insert a new LMT node before the current node */
	    node = *lmt;

	    *lmt = (lmt_node_t *) malloc( sizeof(lmt_node_t) );

	    (*lmt)->y = vertex->v[Y];
	    (*lmt)->bounds = NULL;
	    (*lmt)->next = node;

	    return &((*lmt)->bounds);
	}
	else
	{
	    if ( vertex->v[Y] > (*lmt)->y )
	    {
		/* Head further up the LMT */
		return bound_list( &((*lmt)->next), vertex );
	    }
	    else
	    {
		MSG( 1, "                  bound_list() use current y: %.2f\n",
		     (*lmt)->y );

		/* Use this existing LMT node */
		return &((*lmt)->bounds);
	    }
	}
    }
}


/*****************************************************************************
 * add_to_sb_tree
 *
 * Add the given vertex to the scanbeam tree.
 *****************************************************************************/
static void add_to_sb_tree( GLint *entries, sb_tree_t **sb_tree,
			    tess_vertex_t *vertex )
{
    if ( !*sb_tree )
    {
	MSG( 1, "              sb_tree() adding y: %.2f\n", vertex->v[Y] );

	/* Add a new tree node here */
	*sb_tree = (sb_tree_t *) malloc( sizeof(sb_tree_t) );

	(*sb_tree)->y = vertex->v[Y];
	(*sb_tree)->less = NULL;
	(*sb_tree)->more = NULL;

	(*entries)++;
    }
    else
    {
	if ( (*sb_tree)->y > vertex->v[Y] )
	{
	    /* Head into the 'less' sub-tree */
	    add_to_sb_tree( entries, &((*sb_tree)->less), vertex );
	}
	else if ( (*sb_tree)->y < vertex->v[Y] )
	{
	    /* Head into the 'more' sub-tree */
	    add_to_sb_tree( entries, &((*sb_tree)->more), vertex );
	}
	else
	{
	    MSG( 1, "              sb_tree() not adding, same y: %.2f\n",
		 vertex->v[Y] );
	}
    }
}

/*****************************************************************************
 * build_sbt
 *
 * From the given scanbeam tree, construct the scanbeam table.  This table
 * consists only of an array of y values, and has no vertex information.
 *****************************************************************************/
static void build_sbt( GLint *entries, GLdouble *sbt, sb_tree_t *sb_tree )
{
    if ( sb_tree->less ) {
	build_sbt( entries, sbt, sb_tree->less );
    }


    MSG( 1, "          sbt[%d] = %.2f\n", *entries, sb_tree->y );
    sbt[*entries] = sb_tree->y;
    (*entries)++;

    if ( sb_tree->more ) {
	build_sbt( entries, sbt, sb_tree->more );
    }
}

/*****************************************************************************
 * cleanup_sb_tree
 *
 * Clean up the given scanbeam tree.
 *****************************************************************************/
static void cleanup_sb_tree( sb_tree_t **sb_tree )
{
    if ( *sb_tree )
    {
	cleanup_sb_tree( &((*sb_tree)->less) );
	cleanup_sb_tree( &((*sb_tree)->more) );

	free( *sb_tree );
    }
}


/*****************************************************************************
 * count_optimal_vertices
 *
 * Count the number of vertices, not including any horizontally collinear
 * vertices.
 *****************************************************************************/
static GLint count_optimal_vertices( tess_contour_t *contour )
{
    tess_vertex_t	*vertex = contour->vertices;
    GLint		ret = 0;
    GLint		i;

    /* Ignore non-contributing contours */
    if ( contour->num_vertices > 0 )
    {
	for ( i = 0 ; i < contour->num_vertices ; i++)
	{
	    /* Ignore superfluous vertices embedded in horizontal edges */
	    if ( OPTIMAL( vertex ) ) {
		ret++;
	    }
	    vertex = vertex->next;
	}
    }

    return ret;
}

/*****************************************************************************
 * print_lmt
 *
 * Dump the contents of the local minima table.
 *****************************************************************************/
static void print_lmt( lmt_node_t *lmt )
{
#ifdef DEBUG
    lmt_node_t		*node;
    edge_node_t		*bound;
    edge_node_t		*edge;

    MSG( 1, "\n" );
    MSG( 1, "            lmt contents:\n" );

    for ( node = lmt ; node ; node = node->next )
    {
	MSG( 1, "              min at %.2f:\n", node->y );

	for ( bound = node->bounds ; bound ; bound = bound->next_bound )
	{
	    MSG( 1, "                bound:\n" );

	    for ( edge = bound ; edge ; edge = edge->succ )
	    {
		MSG( 1, "                  edge (%.2f, %.2f) -> (%.2f, %.2f)\n",
		     edge->bot[X], edge->bot[Y],
		     edge->top[X], edge->top[Y] );
	    }
	}
    }
    MSG( 1, "\n" );
#endif
}

/*****************************************************************************
 * build_lmt
 *
 * Construct the local minima table for the input polygon, possibly
 * consisting of several contours, associated with the tessellator object.
 *****************************************************************************/
static edge_node_t *build_lmt( GLUtesselator *tobj,
			       lmt_node_t **lmt, sb_tree_t **sb_tree,
			       GLint *sbt_entries )
{
    GLint		num_edges, num_vertices;
    GLint		total_vertices = 0, e_index = 0;
    edge_node_t		*edge_table;
    edge_node_t		*edge;
    tess_contour_t	*contour;
    tess_vertex_t	*vertex;
    tess_edge_t		*edges, *min, *max, *e;
    GLint		i, j;

    MSG( 1, "      --> build_lmt()\n" );

    for ( contour = tobj->contours ; contour ; contour = contour->next ) {
	total_vertices += count_optimal_vertices( contour );
    }
    MSG( 1, "            optimal vertices: %d\n", total_vertices );

    /* Create the entire input polygon edge table in one go */
    edges = (tess_edge_t *)
	malloc( total_vertices * sizeof(tess_edge_t) );
    edge_table = (edge_node_t *)
	malloc( total_vertices * sizeof(edge_node_t) );

    for ( contour = tobj->contours ; contour ; contour = contour->next )
    {
	if ( contour->num_vertices < 0 )
	{
	    /* Ignore the non-contributing contour and repair the vertex
	       count */
	    contour->num_vertices = -contour->num_vertices;
	}
	else
	{
	    /* Perform contour optimisation */
	    num_vertices = 0;
	    vertex = contour->vertices;

	    for ( i = 0 ; i < contour->num_vertices ; i++ )
	    {
		if ( OPTIMAL( vertex ) )
		{
		    MSG( 1, "            adding edge: %d v: (%.2f, %.2f)\n",
			 num_vertices, vertex->v[X], vertex->v[Y] );

		    edges[num_vertices].vertex = vertex;

		    /* Set up linked list inside the array */
		    if ( i > 0 ) {
			edges[num_vertices].prev =
			    &edges[num_vertices-1];
			edges[num_vertices-1].next =
			    &edges[num_vertices];
		    }

		    /* Record vertex in the scanbeam table */
		    add_to_sb_tree( sbt_entries, sb_tree, vertex );

		    num_vertices++;
		}
		else
		{
		    MSG( 1, "            not adding v: (%.2f, %.2f)\n",
			 vertex->v[X], vertex->v[Y] );
		}

		vertex = vertex->next;
	    }

#ifdef DEBUG
	    edges[0].prev = NULL;
	    edges[num_vertices-1].next = NULL;

	    MSG( 1, "            edge table:\n" );
	    for ( e = edges ; e ; e = e->next ) {
		MSG( 1, "              v: (%.2f, %.2f)\n",
		     e->vertex->v[X], e->vertex->v[Y] );
	    }
#endif
	    /* Wrap linked list of edges */
	    edges[0].prev = &edges[num_vertices-1];
	    edges[num_vertices-1].next = &edges[0];

	    /*
	     * ****** CONTOUR FORWARD PASS ******
	     */
	    MSG( 1, "            fwd pass:\n" );

	    for ( min = edges, i = 0 ; i < num_vertices ;
		  min = min->next, i++ )
	    {
		/* If a forward local minimum... */
		if ( FWD_MIN( min ) )
		{
		    MSG( 1, "              local min (%.2f, %.2f)\n",
			 min->vertex->v[X], min->vertex->v[Y] );

		    /* Search for the next local maximum... */
		    num_edges = 1;
		    max = min->next;

		    while ( ! FWD_MAX( max ) )
		    {
			MSG( 1, "              not local max (%.2f, %.2f)\n",
			     max->vertex->v[X], max->vertex->v[Y] );

			num_edges++;
			max = max->next;
		    }

		    MSG( 1, "              local max (%.2f, %.2f), %d edges (%d)\n",
			 max->vertex->v[X], max->vertex->v[Y],
			 num_edges, e_index );

		    /* Build the next edge list */
		    edge = &edge_table[e_index];
		    e_index += num_edges;
		    e = min;

		    edge[0].bstate[BELOW] = UNBUNDLED;
		    edge[0].bundle[BELOW][SUBJ] = GL_FALSE;

		    for ( j = 0 ; j < num_edges ; j++ )
		    {
			edge[j].vertices[BOT] = e->vertex;
			edge[j].vertices[TOP] = e->vertex->next;

			edge[j].xbot   = e->vertex->v[X];
			edge[j].bot[X] = e->vertex->v[X];
			edge[j].bot[Y] = e->vertex->v[Y];

			e = e->next;

			edge[j].top[X] = e->vertex->v[X];
			edge[j].top[Y] = e->vertex->v[Y];
			edge[j].dx = ( (e->vertex->v[X] - edge[j].bot[X]) /
				       ( edge[j].top[Y] - edge[j].bot[Y] ) );

			edge[j].type = SUBJ;
			edge[j].forward = GL_TRUE;
			edge[j].out[ABOVE] = NULL;
			edge[j].out[BELOW] = NULL;
			edge[j].next = NULL;
			edge[j].prev = NULL;

			edge[j].succ =
			    ( ( num_edges > 1 ) &&
			      ( j < (num_edges - 1) ) ) ? &(edge[j+1]) : NULL;
			edge[j].pred =
			    ( ( num_edges > 1 ) &&
			      ( j > 0 ) ) ? &(edge[j-1]) : NULL;

			edge[j].next_bound = NULL;
			edge[j].bside[SUBJ] = LEFT;

			MSG( 1, "                edge b: (%.2f, %.2f) t: (%.2f, %.2f)\n", edge[j].bot[X], edge[j].bot[Y], edge[j].top[X], edge[j].top[Y] );
			MSG( 1, "                vertex: (%.2f, %.2f) -> (%.2f, %.2f)\n", edge[j].vertices[BOT]->v[X], edge[j].vertices[BOT]->v[Y], edge[j].vertices[TOP]->v[X], edge[j].vertices[TOP]->v[Y] );
		    }

		    insert_bound( bound_list( lmt, min->vertex ), edge );
		}
	    }
	    MSG( 1, "            fwd pass complete!\n" );

	    /*
	     * ****** CONTOUR REVERSE PASS ******
	     */
	    MSG( 1, "            rev pass:\n" );

	    for ( min = edges, i = 0 ; i < num_vertices ;
		  min = min->next, i++ )
	    {
		/* If a reverse local minimum... */
		if ( REV_MIN( min ) )
		{
		    MSG( 1, "              local min (%.2f, %.2f)\n",
			 min->vertex->v[X], min->vertex->v[Y] );

		    /* Search for the previous local maximum... */
		    num_edges = 1;
		    max = min->prev;

		    while ( ! REV_MAX( max ) )
		    {
			MSG( 1, "              not local max (%.2f, %.2f)\n",
			     max->vertex->v[X], max->vertex->v[Y] );

			num_edges++;
			max = max->prev;
		    }
		    MSG( 1, "              local max (%.2f, %.2f), %d edges (%d)\n",
			 max->vertex->v[X], max->vertex->v[Y],
			 num_edges, e_index );

		    /* Build the previous edge list */
		    edge = &edge_table[e_index];
		    e_index += num_edges;
		    e = min;

		    edge[0].bstate[BELOW] = UNBUNDLED;
		    edge[0].bundle[BELOW][SUBJ] = GL_FALSE;

		    for ( j = 0 ; j < num_edges ; j++ )
		    {
			edge[j].vertices[BOT] = e->vertex;
			edge[j].vertices[TOP] = e->vertex->prev;

			edge[j].xbot   = e->vertex->v[X];
			edge[j].bot[X] = e->vertex->v[X];
			edge[j].bot[Y] = e->vertex->v[Y];

			e = e->prev;

			edge[j].top[X] = e->vertex->v[X];
			edge[j].top[Y] = e->vertex->v[Y];
			edge[j].dx = ( ( e->vertex->v[X] - edge[j].bot[X] ) /
				       ( edge[j].top[Y] - edge[j].bot[Y] ) );

			edge[j].type = SUBJ;
			edge[j].forward = GL_FALSE;
			edge[j].out[ABOVE] = NULL;
			edge[j].out[BELOW] = NULL;
			edge[j].next = NULL;
			edge[j].prev = NULL;

			edge[j].succ =
			    ( ( num_edges > 1 ) &&
			      ( j < (num_edges - 1) ) ) ? &(edge[j+1]) : NULL;
			edge[j].pred =
			    ( ( num_edges > 1 ) &&
			      ( j > 0 ) ) ? &(edge[j-1]) : NULL;

			edge[j].next_bound = NULL;
			edge[j].bside[SUBJ] = LEFT;

			MSG( 1, "                edge b: (%.2f, %.2f) t: (%.2f, %.2f)\n", edge[j].bot[X], edge[j].bot[Y], edge[j].top[X], edge[j].top[Y] );
			MSG( 1, "                vertex: (%.2f, %.2f) <- (%.2f, %.2f)\n", edge[j].vertices[BOT]->v[X], edge[j].vertices[BOT]->v[Y], edge[j].vertices[TOP]->v[X], edge[j].vertices[TOP]->v[Y] );
		    }

		    insert_bound( bound_list( lmt, min->vertex ), edge );
		}
	    }
	    MSG( 1, "            rev pass complete!\n" );
	}
    }

    /* Free temporary edge list */
    if ( edges ) {
	free( edges );
    }

    print_lmt( *lmt );

    MSG( 1, "      <-- build_lmt()\n" );
    return edge_table;
}

/*****************************************************************************
 * cleanup_lmt
 *
 * Clean up the given local minima table.
 *****************************************************************************/
static void cleanup_lmt( lmt_node_t **lmt )
{
    lmt_node_t	*node;

    while ( *lmt )
    {
	node = (*lmt)->next;
	free( *lmt );

	*lmt = node;
    }
}


/*****************************************************************************
 * add_edge_to_aet
 *
 * Add the given edge to the active edge table.  The active edge table is
 * sorted by x coordinate, using an insertion sort.
 *****************************************************************************/
static void add_edge_to_aet( edge_node_t **aet,
			     edge_node_t *edge, edge_node_t *prev )
{
    if ( !*aet )
    {
	MSG( 1, "              aet() new tail (%.2f, %.2f)\n",
	     edge->bot[X], edge->bot[Y] );

	/* Append edge onto the tail end of the AET */
	*aet = edge;
	edge->prev = prev;
	edge->next = NULL;
    }
    else
    {
	/* Do primary sort on the xb field */
	if ( edge->xbot < (*aet)->xbot )
	{
	    MSG( 1, "              aet() x less, insert (%.2f, %.2f)\n",
		 edge->bot[X], edge->bot[Y] );

	    /* Insert edge here (before the AET edge) */
	    edge->prev = prev;
	    edge->next = *aet;
	    (*aet)->prev = edge;
	    *aet = edge;
	}
	else if ( edge->xbot == (*aet)->xbot )
	{
	    /* Do secondary sort on the dx field */
	    if ( edge->dx < (*aet)->dx )
	    {
		MSG( 1, "              aet() dx less, insert (%.2f, %.2f)\n",
		     edge->bot[X], edge->bot[Y] );

		/* Insert edge here (before the AET edge) */
		edge->prev = prev;
		edge->next = *aet;
		(*aet)->prev = edge;
		*aet = edge;
	    }
	    else
	    {
		/* Head further into the AET */
		add_edge_to_aet( &((*aet)->next), edge, *aet );
	    }
	}
	else
	{
	    /* Head further into the AET */
	    add_edge_to_aet( &((*aet)->next), edge, *aet );
	}
    }
}


/*****************************************************************************
 * tess_combine_callback
 *****************************************************************************/
void tess_combine_callback( GLUtesselator *tobj, GLdouble coords[3],
			    void *vertex_data[4], GLfloat weight[4],
			    void **out_data )
{
    if ( tobj->callbacks.combineData != NULL )
    {
	( tobj->callbacks.combineData )( coords, vertex_data, weight,
					 out_data, tobj->data );
    }
    else if ( tobj->callbacks.combine != NULL )
    {
	( tobj->callbacks.combine )( coords, vertex_data, weight, out_data );
    }

    MSG( 15, "combine coords: (%.2f, %.2f, %.2f) data: %p\n", coords[0], coords[1], coords[2], *out_data );
}

/*****************************************************************************
 * intersect_edges
 *
 * Intersect edges (a, b) and (c, d) to obtain the 3D intersection point
 * and any data from the combine callback.
 *****************************************************************************/
static GLboolean intersect_edges( GLUtesselator *tobj, it_node_t *it,
				  tess_vertex_t *a, tess_vertex_t *b,
				  tess_vertex_t *c, tess_vertex_t *d )
{
    /*
     * Calculate the values required for the intersection test.  Taken
     *  directly from the comp.graphics.algorithms FAQ.
     */
    GLdouble	denom =
	( ( b->v[X] - a->v[X] ) * ( d->v[Y] - c->v[Y] ) ) -
	( ( b->v[Y] - a->v[Y] ) * ( d->v[X] - c->v[X] ) );

    GLdouble	r =
	( ( a->v[Y] - c->v[Y] ) * ( d->v[X] - c->v[X] ) ) -
	( ( a->v[X] - c->v[X] ) * ( d->v[Y] - c->v[Y] ) );

    GLdouble	s =
	( ( a->v[Y] - c->v[Y] ) * ( b->v[X] - a->v[X] ) ) -
	( ( a->v[X] - c->v[X] ) * ( b->v[Y] - a->v[Y] ) );

    void	*vertex_data[4];
    GLfloat	weight[4];

    /* Return false if lines are parallel */
    if ( ABSD( denom ) < GLU_TESS_EPSILON ) return GL_FALSE;

    r = r / denom;
    s = s / denom;

    /* Return false if the line segments do not intersect. */
    if ( ( r <= 0.0 ) || ( r >= 1.0 ) ||
	 ( s <= 0.0 ) || ( s >= 1.0 ) )
    {
	return GL_FALSE;
    }

    ASSIGN_4V( vertex_data,
	       a->data, b->data, c->data, d->data );
    ASSIGN_4V( weight,
	       (GLfloat)(( 1.0 - r ) * 0.5), (GLfloat)(r * 0.5),
	       (GLfloat)(( 1.0 - s ) * 0.5), (GLfloat)(s * 0.5) );

    /*
     * Calculate the actual point of intersection.  Again, taken
     *  from the comp.graphics.alrogithms FAQ.
     */
    it->coords[X] = a->coords[X] + r * ( b->coords[X] - a->coords[X] );
    it->coords[Y] = a->coords[Y] + r * ( b->coords[Y] - a->coords[Y] );
    it->coords[Z] = a->coords[Z] + r * ( b->coords[Z] - a->coords[Z] );

    it->v[X] = a->v[X] + r * ( b->v[X] - a->v[X] );
    it->v[Y] = a->v[Y] + r * ( b->v[Y] - a->v[Y] );

    it->data = NULL;

    /* Combine the intersection into a new vertex. */
    tess_combine_callback( tobj, it->coords, vertex_data,
			   weight, &(it->data) );

    MSG( 1, "                  r: %.2f s: %.2f new: (%.2f, %.2f, %.2f)\n",
	 r, s, it->coords[X], it->coords[Y], it->coords[Z] );

    return GL_TRUE;
}

/*****************************************************************************
 * it_vertex
 *
 * Using the given intersection table entry, create a new vertex at the
 * intersection point with the data from the combine callback.
 *****************************************************************************/
static tess_vertex_t *it_vertex( GLUtesselator *tobj, it_node_t *it )
{
    tess_vertex_t	*vertex;

    MSG( 15, "it_vertex() v: (%.2f, %.2f) data: %p\n",
	 it->v[X], it->v[Y], it->data );

    /* Create a new vertex at the intersection point */
    vertex = (tess_vertex_t *) malloc( sizeof(tess_vertex_t) );

    vertex->index = -1;
    vertex->data = it->data;

    vertex->coords[X] = it->coords[X];
    vertex->coords[Y] = it->coords[Y];
    vertex->coords[Z] = it->coords[Z];

    vertex->v[X] = it->v[X];
    vertex->v[Y] = it->v[Y];

    vertex->edge_flag = GL_TRUE;

    vertex->side = 0.0;

    vertex->next = vertex->prev = NULL;

    /* Increment the global number of vertices */
    tobj->num_vertices++;

    return vertex;
}

/*****************************************************************************
 * add_intersection
 *
 * Add an entry to the intersection table at the given point between the
 * given two edges.
 *****************************************************************************/
static void add_intersection( GLUtesselator *tobj, it_node_t **it,
			      edge_node_t *edge0, edge_node_t *edge1,
			      GLdouble x, GLdouble y )
{
    it_node_t	*node;
    GLboolean	result = GL_TRUE;

    if ( !*it )
    {
	MSG( 1, "                it() new tail (%.2f, %.2f)\n", x, y );

	/* Append a new node to the tail of the list */
	*it = (it_node_t *) malloc( sizeof(it_node_t) );

	(*it)->ie[0] = edge0;
	(*it)->ie[1] = edge1;
	(*it)->v[X] = x;
	(*it)->v[Y] = y;
	(*it)->next = NULL;

	/* Perform tessellation combine callback */
	result = intersect_edges( tobj, *it,
				  edge0->vertices[BOT], edge0->vertices[TOP],
				  edge1->vertices[BOT], edge1->vertices[TOP] );
    }
    else
    {
	if ( (*it)->v[Y] > y )
	{
	    MSG( 1, "                it() insert (%.2f, %.2f)\n", x, y );

	    /* Insert a new node mid-list */
	    node = *it;

	    *it = (it_node_t *) malloc( sizeof(it_node_t) );

	    (*it)->ie[0] = edge0;
	    (*it)->ie[1] = edge1;
	    (*it)->v[X] = x;
	    (*it)->v[Y] = y;
	    (*it)->next = node;

	    /* Perform tessellation combine callback */
	    result = intersect_edges( tobj, *it,
				      edge0->vertices[BOT], edge0->vertices[TOP],
				      edge1->vertices[BOT], edge1->vertices[TOP] );
	}
	else
	{
	    /* Head further down the list */
	    add_intersection( tobj, &((*it)->next), edge0, edge1, x, y );
	}
    }

    /* Clean up if no intersection found */
    if ( !result ) {
	node = *it;
	*it = node->next;
	free( node );
    }
}

/*****************************************************************************
 * add_st_edge
 *
 * Add the edge to the sorted edge table.  If the edge intersects another
 * edge already in the table, calculate the required intersection
 * information.
 *****************************************************************************/
static void add_st_edge( GLUtesselator *tobj, st_node_t **st, it_node_t **it,
			 edge_node_t *edge, GLdouble dy )
{
    st_node_t	*node;
    GLdouble	den, r, x, y;

    if ( !*st )
    {
	MSG( 1, "                st() new tail (%.2f, %.2f)\n",
	     edge->bot[X], edge->bot[Y] );

	/* Append edge onto the tail end of the ST */
	*st = (st_node_t *) malloc( sizeof(st_node_t) );

	(*st)->edge = edge;
	(*st)->xbot = edge->xbot;
	(*st)->xtop = edge->xtop;
	(*st)->dx = edge->dx;
	(*st)->prev = NULL;
    }
    else
    {
	den = ((*st)->xtop - (*st)->xbot) - (edge->xtop - edge->xbot);

	/* If new edge and ST edge don't cross */
	if ( ( edge->xtop >= (*st)->xtop ) || ( edge->dx == (*st)->dx ) ||
	     ( ABSD( den ) <= GLU_TESS_EPSILON ) )
	{
	    MSG( 1, "                st() insert (%.2f, %.2f)\n",
		 edge->bot[X], edge->bot[Y] );

	    /* No intersection - insert edge here (before the ST edge) */
	    node = *st;

	    *st = (st_node_t *) malloc( sizeof(st_node_t) );

	    (*st)->edge = edge;
	    (*st)->xbot = edge->xbot;
	    (*st)->xtop = edge->xtop;
	    (*st)->dx = edge->dx;
	    (*st)->prev = node;
	}
	else
	{
	    /* Compute intersection between new edge and ST edge */
	    r = (edge->xbot - (*st)->xbot) / den;
	    x = (*st)->xbot + r * ((*st)->xtop - (*st)->xbot);
	    y = (*st)->edge->bot[Y] + r * dy;

	    MSG( 1, "            *** st() intersection at (%.2f, %.2f)\n",
		 x, y );

	    /* Insert the edge pointers and the intersection point in the IT */
	    add_intersection( tobj, it, (*st)->edge, edge, x, y );

	    /* Head further into the ST */
	    add_st_edge( tobj, &((*st)->prev), it, edge, dy );
	}
    }
}

/*****************************************************************************
 * cleanup_it
 *
 * Clean up the given intersection table.
 *****************************************************************************/
static void cleanup_it( it_node_t **it )
{
    it_node_t	*node;

    while ( *it )
    {
	node = (*it)->next;
	free( *it );

	*it = node;
    }
}

/*****************************************************************************
 * build_intersection_table
 *
 * From the current active edge table, build the corresponding intersection
 * table for the current scanbeam.
 *****************************************************************************/
static void build_intersection_table( GLUtesselator *tobj, it_node_t **it,
				      edge_node_t *aet, GLdouble dy )
{
    st_node_t		*st, *stp;
    edge_node_t		*edge;

    /* Build intersection table for the current scanbeam */
    cleanup_it( it );
    st = NULL;

    /* Process each AET edge */
    for ( edge = aet ; edge ; edge = edge->next )
    {
	if ( ( edge->bstate[ABOVE] == BUNDLE_HEAD ) ||
	     edge->bundle[ABOVE][SUBJ] )
	{
	    add_st_edge( tobj, &st, it, edge, dy );
	}
    }

    /* Free the sorted edge table */
    while ( st )
    {
	stp = st->prev;
	free( st );
	st = stp;
    }
}

/*****************************************************************************
 * find_intersection
 *
 * Find the point of intersection between the given edge and an edge in the
 * AET that lies in the given scanbeam.  Return the quadrant that the area
 * with the highest winding number lies in.
 *****************************************************************************/
static GLint find_intersection( GLUtesselator *tobj, it_node_t **it,
				edge_node_t *aet, edge_node_t *edge,
				GLdouble ybot, GLdouble ytop )
{
    edge_node_t		*e;
    tess_vertex_t	*horiz[2] = { NULL, NULL };
    GLboolean		forward = GL_FALSE;
    GLboolean		done = GL_FALSE;
    GLint		ret = -1;

    MSG( 1, "            *** searching for intersection...\n" );

    /* Make sure the intersection table node is empty before we begin */
    cleanup_it( it );

    /* Allocate a new intersection table entry */
    *it = (it_node_t *) malloc( sizeof(it_node_t) );
    if ( *it == NULL ) {
	tess_error_callback( tobj, GLU_OUT_OF_MEMORY );
	return ret;
    }

    (*it)->next = NULL;

    /* Process each AET edge */
    for ( e = aet ; e ; e = e->next )
    {
	MSG( 1, "                e (%.2f, %.2f) -> (%.2f %.2f)\n",
	     e->bot[X], e->bot[Y], e->top[X], e->top[Y] );

	if ( e == edge ) continue;

	/* Test if the two edges intersect */
	done = intersect_edges( tobj, *it,
				edge->vertices[BOT], edge->vertices[TOP],
				e->vertices[BOT], e->vertices[TOP] );

	if ( done && ( ( (*it)->v[Y] < ybot ) || ( (*it)->v[Y] > ytop ) ) )
	{
	    /*
	     * The intersection point lies outside the current scanbeam,
	     * so we need to keep looking.
	     */
	    MSG( 1, "                int %.2f outside yb: %.2f yt: %.2f\n",
		 (*it)->v[Y], ybot, ytop );
	    done = GL_FALSE;
	}

	/*
	 * FIXME: This is a little dodgy, and should become a list of all
	 * horizontal edges on the current scanline.  Then, all these edges
	 * should be tested for intersection.
	 */

	/* Save the current horizontal edge, if one exists */
	if ( horiz[BOT] == NULL )
	{
	    if ( e->vertices[BOT]->v[Y] == ybot )
	    {
		horiz[BOT] = e->vertices[BOT];
		forward = GL_TRUE;
	    }
	    else if ( e->vertices[TOP]->v[Y] == ybot )
	    {
		horiz[BOT] = e->vertices[TOP];
		forward = GL_FALSE;
	    }
	}
	if ( e->vertices[BOT]->v[Y] == ybot ) {
	    horiz[TOP] = e->vertices[BOT];
	} else if ( e->vertices[TOP]->v[Y] == ybot ) {
	    horiz[TOP] = e->vertices[TOP];
	}

	/*
	 * If a valid intersection point has been found, exit here so the
	 * intersecting edge is saved.
	 */
	if ( done ) {
	    break;
	}
    }

    /* Check the horizontal edge that is implicitly in the AET */
    if ( !done )
    {
	MSG( 1, "            *** checking horizontal edge...\n" );
	MSG( 1, "                e (%.2f, %.2f) -> (%.2f %.2f)\n",
	     horiz[BOT]->v[X], horiz[BOT]->v[Y],
	     horiz[TOP]->v[X], horiz[TOP]->v[Y] );

	done = intersect_edges( tobj, *it,
				edge->vertices[BOT], edge->vertices[TOP],
				horiz[BOT], horiz[TOP] );

	if ( done )
	{
	    MSG( 1, "                found int (%.2f, %.2f)\n",
		 (*it)->v[X], (*it)->v[Y] );
	}
	else
	{
	    MSG( 1, "something's really wrong here...\n" );
	}
    }
    else
    {
	forward = e->forward;
    }

    /* Calculate the quadrant with the highest winding number */
    if ( edge->forward )
    {
	ret = ( forward ) ? WIND_NW : WIND_SW;
    }
    else
    {
	ret = ( forward ) ? WIND_NE : WIND_SE;
    }

#ifdef DEBUG
    switch ( ret )
    {
    case WIND_NE:
	MSG( 1, "                highest winding number NE\n" );
	break;
    case WIND_SE:
	MSG( 1, "                highest winding number SE\n" );
	break;
    case WIND_SW:
	MSG( 1, "                highest winding number SW\n" );
	break;
    case WIND_NW:
	MSG( 1, "                highest winding number NW\n" );
	break;
    }
#endif
    return ret;
}


/*****************************************************************************
 * add_left
 *
 * Add the given vertex to the left end of the contour's vertex list.
 *****************************************************************************/
static void add_left( clip_contour_t *contour, tess_vertex_t *vertex )
{
    vertex->prev = contour->proxy->vertices[RIGHT];
    vertex->next = contour->proxy->vertices[LEFT];

    contour->proxy->vertices[LEFT]->prev  = vertex;
    contour->proxy->vertices[RIGHT]->next = vertex;

    contour->proxy->vertices[LEFT] = vertex;

    MSG( 1, "  add_left()       v: (%.2f, %.2f)\n",
	 vertex->v[X], vertex->v[Y] );
}

/*****************************************************************************
 * merge_left
 *****************************************************************************/
static void merge_left( clip_contour_t *p, clip_contour_t *q,
			clip_contour_t *list )
{
    clip_contour_t	*target;

    MSG( 1, "  merge_left()\n" );

    /* Label contour as a hole */
    q->proxy->hole = GL_TRUE;

    if ( p->proxy != q->proxy )
    {
	/* Assign p's vertex list to the left end of q's list */
	p->proxy->vertices[RIGHT]->next = q->proxy->vertices[LEFT];
	p->proxy->vertices[LEFT]->prev = q->proxy->vertices[RIGHT];

	q->proxy->vertices[LEFT]->prev = p->proxy->vertices[RIGHT];
	q->proxy->vertices[RIGHT]->next = p->proxy->vertices[LEFT];

	q->proxy->vertices[LEFT] = p->proxy->vertices[LEFT];
	p->proxy->vertices[RIGHT] = q->proxy->vertices[RIGHT];

	/* Redirect any p->proxy references to q->proxy */
	for ( target = p->proxy ; list ; list = list->next )
	{
	    if ( list->proxy == target )
	    {
		list->active = GL_FALSE;
		list->proxy = q->proxy;
	    }
	}
    }
}

/*****************************************************************************
 * add_right
 *
 * Add the given vertex to the right end of the contour's vertex list.
 *****************************************************************************/
static void add_right( clip_contour_t *contour, tess_vertex_t *vertex )
{
    vertex->prev = contour->proxy->vertices[RIGHT];
    vertex->next = contour->proxy->vertices[LEFT];

    contour->proxy->vertices[RIGHT]->next = vertex;
    contour->proxy->vertices[LEFT]->prev  = vertex;

    contour->proxy->vertices[RIGHT] = vertex;

    MSG( 1, "  add_right()      v: (%.2f, %.2f)\n",
	 vertex->v[X], vertex->v[Y] );
}

/*****************************************************************************
 * merge_right
 *****************************************************************************/
static void merge_right( clip_contour_t *p, clip_contour_t *q,
			 clip_contour_t *list )
{
    clip_contour_t	*target;

    MSG( 1, "  merge_right()\n" );

    /* Label contour as external */
    q->proxy->hole = GL_FALSE;

    if ( p->proxy != q->proxy )
    {
	/* Assign p's vertex list to the right end of q's list */
	q->proxy->vertices[RIGHT]->next = p->proxy->vertices[LEFT];
	q->proxy->vertices[LEFT]->prev = p->proxy->vertices[RIGHT];

	p->proxy->vertices[LEFT]->prev = q->proxy->vertices[RIGHT];
	p->proxy->vertices[RIGHT]->next = q->proxy->vertices[LEFT];

	p->proxy->vertices[LEFT] = p->proxy->vertices[LEFT];
	q->proxy->vertices[RIGHT] = p->proxy->vertices[RIGHT];

	/* Redirect any p->proxy references to q->proxy */
	for ( target = p->proxy ; list ; list = list->next )
	{
	    if ( list->proxy == target )
	    {
		list->active = GL_FALSE;
		list->proxy = q->proxy;
	    }
	}
    }
}

/*****************************************************************************
 * add_local_min
 *
 * Add the given vertex as a new local minimum.  Create a new contour entry
 * to hold the new local minimum.
 *****************************************************************************/
static void add_local_min( clip_contour_t **contour, edge_node_t *edge,
			   tess_vertex_t *vertex )
{
    clip_contour_t	*min;

    min = *contour;

    *contour = (clip_contour_t *) malloc( sizeof(clip_contour_t) );

    vertex->prev = NULL;
    vertex->next = NULL;

    /* Initialise proxy to point to p itself */
    (*contour)->proxy = (*contour);
    (*contour)->active = GL_TRUE;
    (*contour)->next = min;

    /* Make v[LEFT] and v[RIGHT] point to new vertex nv */
    (*contour)->vertices[LEFT]  = vertex;
    (*contour)->vertices[RIGHT] = vertex;

    /* Assign polygon p to the edge */
    edge->out[ABOVE] = *contour;

    MSG( 1, "  add_local_min()  v: (%.2f, %.2f)\n",
	 (*contour)->vertices[LEFT]->v[X], (*contour)->vertices[LEFT]->v[Y] );
}


/*****************************************************************************
 * new_contour
 *
 * Create a new output contour.
 *****************************************************************************/
static tess_contour_t *new_contour( GLUtesselator *tobj )
{
    tess_contour_t	*contour;

    contour = malloc( sizeof(tess_contour_t) );
    if ( contour == NULL ) {
	tess_error_callback( tobj, GLU_OUT_OF_MEMORY );
	return NULL;
    }

    COPY_3V( contour->plane.normal, tobj->plane.normal );
    contour->plane.dist = tobj->plane.dist;

    contour->area = 0.0;
    contour->orientation = GLU_UNKNOWN;

    contour->label = 0;
    contour->winding = 0;

    contour->rotx = contour->roty = 0.0;

    CLEAR_BBOX_2DV( contour->mins, contour->maxs );

    contour->num_vertices = 0;
    contour->vertices = contour->last_vertex = NULL;

    contour->reflex_vertices = NULL;

    return contour;
}

/*****************************************************************************
 * inspect_contour
 *****************************************************************************/
static void inspect_contour( tess_contour_t *contour )
{
    tess_vertex_t	*vertex = contour->vertices;
    GLdouble		area;
    GLint		i;

    for ( i = 0; i < contour->num_vertices; i++ )
    {
	ACC_BBOX_2V( vertex->v, contour->mins, contour->maxs );

	vertex = vertex->next;
    }

    area = twice_contour_area( contour );

    if ( area >= 0.0 )
    {
	contour->orientation = GLU_CCW;
	contour->area = area;
    }
    else
    {
	contour->orientation = GLU_CW;
	contour->area = -area;
    }

    MSG( 1, "              contour area: %.2f\n", contour->area );
}

/*****************************************************************************
 * output_contours
 *
 * Create the new contour entries for all valid clip contours.
 *****************************************************************************/
static void output_contours( GLUtesselator *tobj, clip_contour_t *out )
{
    tess_contour_t	*contour, *next_contour;
    tess_vertex_t	*vertex, *next_vertex;
    clip_contour_t	*clip, *next_clip;
    GLint		num_contours, num_vertices;
    GLint		i;

    MSG( 1, "      --> output_contours( tobj:%p out:%p )\n", tobj, out );

    for ( clip = out, num_contours = 0 ; clip ; clip = clip->next )
    {
	if ( clip->active )
	{
	    /* Count the vertices in the current contour */
	    num_vertices = 0;
	    vertex = clip->proxy->vertices[LEFT];
	    do
	    {
		num_vertices++;
		vertex = vertex->next;
	    }
	    while ( vertex != clip->proxy->vertices[LEFT] );

	    MSG( 1, "            clip: %p nv: %d\n", clip, num_vertices );

	    /* Record valid vertex counts in the active field */
	    if ( num_vertices > 2 )
	    {
		clip->active = num_vertices;
		num_contours++;
	    }
	    else
	    {
		/* Invalid contour: just free the heap */
		vertex = clip->proxy->vertices[LEFT];
		do
		{
		    next_vertex = vertex->next;
		    free( vertex );
		    vertex = next_vertex;
		}
		while ( vertex != clip->proxy->vertices[LEFT] );

		clip->active = 0;
	    }
	}
    }
    MSG( 1, "            num contours: %d\n", num_contours );

    /* Delete all existing contours */
    for ( contour = tobj->contours, i = 0 ; i < tobj->num_contours ; i++ )
    {
	next_contour = contour->next;
	free( contour );
	contour = next_contour;
    }

    tobj->contours = NULL;
    tobj->last_contour = NULL;
    tobj->num_contours = num_contours;

    if ( tobj->num_contours > 0 )
    {
	for ( clip = out ; clip ; clip = next_clip )
	{
	    next_clip = clip->next;

	    if ( clip->active )
	    {
		/* Generate a new contour */
		tobj->current_contour = new_contour( tobj );

		tobj->current_contour->num_vertices = clip->active;

		/* Extract the vertex loop and assign it to the new contour */
		tobj->current_contour->vertices =
		    clip->proxy->vertices[LEFT];
		tobj->current_contour->last_vertex =
		    clip->proxy->vertices[RIGHT];

		/* Ensure the vertex loop is closed correctly */
		tobj->current_contour->vertices->prev =
		    tobj->current_contour->last_vertex;
		tobj->current_contour->last_vertex->next =
		    tobj->current_contour->vertices;

		/* Calculate the bounding box, area and orientation */
		inspect_contour( tobj->current_contour );

		/* Save the new contour */
		if ( tobj->contours == NULL )
		{
		    tobj->current_contour->next =
			tobj->current_contour->prev = NULL;

		    tobj->contours = tobj->current_contour;
		    tobj->last_contour = tobj->current_contour;
		}
		else
		{
		    tobj->last_contour->next = tobj->current_contour;
		    tobj->current_contour->prev = tobj->last_contour;

		    tobj->last_contour = tobj->current_contour;
		}
#ifdef DEBUG
		contour_dump( tobj->current_contour );
#endif
		tobj->current_contour = NULL;
	    }

	    free( clip );
	}

	/* Ensure the contour loop is closed correctly */
	tobj->last_contour->next = tobj->contours;
	tobj->contours->prev = tobj->last_contour;
    }

    MSG( 1, "      <-- output_contours( tobj:%p out:%p )\n", tobj, out );
}

/*****************************************************************************
 * print_contour
 *****************************************************************************/
static void print_contour( clip_contour_t *clip )
{
    tess_vertex_t	*vertex;
    GLint		i = 0;

    MSG( 1, "    contour: %p proxy: %p active: %d\n",
	 clip, clip->proxy, clip->active );
    MSG( 1, "    l: (%.2f, %.2f) r: (%.2f, %.2f)\n",
	 clip->proxy->vertices[LEFT]->v[X],
	 clip->proxy->vertices[LEFT]->v[Y],
	 clip->proxy->vertices[RIGHT]->v[X],
	 clip->proxy->vertices[RIGHT]->v[Y] );

    vertex = clip->proxy->vertices[LEFT];
    do
    {
	MSG( 1, "      v: (%.2f, %.2f)\n", vertex->v[X], vertex->v[Y] );
	vertex = vertex->next;
	i++;
    }
    while ( ( vertex != NULL ) &&
	    ( vertex != clip->proxy->vertices[LEFT] ) &&
	    ( i < 20 ) );
}

/*****************************************************************************
 * print_contours
 *****************************************************************************/
static void print_all_contours( clip_contour_t *out )
{
    clip_contour_t	*clip;

    for ( clip = out ; clip ; clip = clip->next )
    {
	if ( clip->active ) {
	    print_contour( clip );
	}
    }
}


/*****************************************************************************
 * class_string
 *
 * Print out the given class as a meaningful string.
 *****************************************************************************/
static char *class_string( GLint vclass )
{
    switch ( vclass )
    {
    case EDGE_EMPTY:
	return "EMPTY";
    case EDGE_EXT_MAX:
	return "EXT_MAX";
    case EDGE_EXT_LI:
	return "EXT_LI";
    case EDGE_TOP:
	return "TOP";
    case EDGE_EXT_RI:
	return "EXT_RI";
    case EDGE_RIGHT:
	return "RIGHT";
    case EDGE_INT_MAX_MIN:
	return "INT_MAX_MIN";
    case EDGE_INT_MIN:
	return "INT_MIN";
    case EDGE_EXT_MIN:
	return "EXT_MIN";
    case EDGE_EXT_MAX_MIN:
	return "EXT_MAX_MIN";
    case EDGE_LEFT:
	return "LEFT";
    case EDGE_INT_LI:
	return "INT_LI";
    case EDGE_BOTTOM:
	return "BOTTOM";
    case EDGE_INT_RI:
	return "INT_RI";
    case EDGE_INT_MAX:
	return "INT_MAX";
    case EDGE_FULL:
	return "FULL";
    default:
	return "UNKNOWN";
    }
}


/*****************************************************************************
 * tess_clip_polygons
 *
 * Perform any contour clipping (boolean operations) on the input contours
 * as required.
 *****************************************************************************/
GLenum tess_clip_polygons( GLUtesselator *tobj )
{
    sb_tree_t		*sb_tree = NULL;
    it_node_t		*it = NULL, *intersect;
    edge_node_t		*edge, *prev_edge, *next_edge, *succ_edge, *e0, *e1;
    edge_node_t		*aet = NULL, *heap = NULL;
    lmt_node_t		*lmt = NULL, *local_min;
    clip_contour_t	*out_poly = NULL, *p, *q;
    clip_contour_t	*current[2] = { NULL, NULL };
    GLint		horiz[2];
    GLint		in[2], exists[2], parity[2] = { LEFT, LEFT };
    GLint		contributing, scanbeam = 0, sbt_entries = 0;
    GLint		vclass, bl, br, tl, tr;
    GLdouble		*sbt = NULL;
    GLdouble		xb, prevx, yb = 0.0, yt = 0.0, dy = 0.0;
    GLdouble		ix = 0.0, iy = 0.0;
    GLboolean		forward;
    GLboolean		search;
    GLint		quadrant;

    MSG( 1, "    --> clip_polygons( tobj:%p )\n", tobj );

    /* Break the contour loop for now. */
    tobj->contours->prev = NULL;
    tobj->last_contour->next = NULL;

    /* Build LMT */
    if ( tobj->num_contours > 0 ) {
	heap = build_lmt( tobj, &lmt, &sb_tree, &sbt_entries );
    }

    /* Return a NULL result if no contours contribute */
    if ( lmt == NULL )
    {
	/* FIXME: Delete all contour entries as well... */
	tobj->num_contours = 0;
	tobj->contours = NULL;
	cleanup_lmt( &lmt );
	free( heap );
	return GLU_NO_ERROR;
    }

    /* Build scanbeam table from scanbeam tree */
    sbt = (GLdouble *) malloc( sbt_entries * sizeof(GLdouble) );

    build_sbt( &scanbeam, sbt, sb_tree );
    cleanup_sb_tree( &sb_tree );

    scanbeam = 0;
    local_min = lmt;

    MSG( 1, "          processing scanbeams...\n" );

    while ( scanbeam < sbt_entries )
    {
	/* Set yb and yt to the bottom and top of the scanbeam */
	yb = sbt[scanbeam++];
	if ( scanbeam < sbt_entries ) {
	    yt = sbt[scanbeam];
	    dy = yt - yb;
	}

	MSG( 1, "\n" );
	MSG( 1, "   ****** scanbeam %d yb: %.2f yt: %.2f dy: %.2f\n", scanbeam - 1, yb, yt, dy );
	MSG( 1, "\n" );

	/*
	 * ****** SCANBEAM BOUNDARY PROCESSING ******
	 */

	/* If LMT node corresponding to yb exists */
	if ( ( local_min ) && ( local_min->y == yb ) )
	{
	    /* Add edges starting at this local minimum to the AET */
	    for ( edge = local_min->bounds ; edge ;
		  edge = edge->next_bound )
	    {
		MSG( 1, "            edge (%.2f, %.2f) -> (%.2f, %.2f)\n",
		     edge->bot[X], edge->bot[Y], edge->top[X], edge->top[Y] );

		add_edge_to_aet( &aet, edge, NULL );
	    }

	    local_min = local_min->next;

	    MSG( 1, "\n" );
	}

	/* Set dummy previous x value */
	prevx = -DBL_MAX;

	/* Create bundles within AET */
	e0 = aet;
	e1 = aet;

	/* Set up bundle fields of first edge */
	aet->bundle[ABOVE][ aet->type] = (GLboolean)( aet->top[Y] != yb );
	aet->bundle[ABOVE][!aet->type] = GL_FALSE;
	aet->bstate[ABOVE] = UNBUNDLED;

	for ( next_edge = aet->next ; next_edge ; next_edge = next_edge->next )
	{
	    /* Set up bundle fields of next edge */
	    next_edge->bundle[ABOVE][ next_edge->type] =
		(GLboolean)( next_edge->top[Y] != yb );
	    next_edge->bundle[ABOVE][!next_edge->type] = GL_FALSE;
	    next_edge->bstate[ABOVE] = UNBUNDLED;

	    /* Bundle edges above the scanbeam boundary if they coincide */
	    if ( next_edge->bundle[ABOVE][next_edge->type] )
	    {
		if ( IS_EQUAL( e0->xbot, next_edge->xbot ) &&
		     IS_EQUAL( e0->dx, next_edge->dx ) &&
		     ( e0->top[Y] != yb ) )
		{
		    next_edge->bundle[ABOVE][next_edge->type] ^=
			e0->bundle[ABOVE][next_edge->type];

		    next_edge->bundle[ABOVE][!next_edge->type] =
			e0->bundle[ABOVE][!next_edge->type];

		    next_edge->bstate[ABOVE] = BUNDLE_HEAD;

		    e0->bundle[ABOVE][SUBJ] = GL_FALSE;
		    e0->bstate[ABOVE] = BUNDLE_TAIL;
		}

		e0 = next_edge;
	    }

	    MSG( 1, "            next (%.2f, %.2f) -> (%.2f %.2f)\n",
		 next_edge->bot[X], next_edge->bot[Y],
		 next_edge->top[X], next_edge->top[Y] );
	    MSG( 1, "              bundle: %s state: %s fwd: %s\n",
		 ( next_edge->bundle[ABOVE][SUBJ] ) ? "TRUE" : "FALSE",
		 ( next_edge->bstate[ABOVE] == BUNDLE_HEAD ) ? "HEAD" :
		 ( next_edge->bstate[ABOVE] == BUNDLE_TAIL ) ? "TAIL" : "UNBUNDLED",
		 ( next_edge->forward ) ? "TRUE" : "FALSE" );
	}
	MSG( 1, "\n" );

	horiz[SUBJ] = HORIZ_NONE;

	/* Process each edge at this scanbeam boundary */
	for ( edge = aet ; edge ; edge = edge->next )
	{
	    exists[SUBJ] = edge->bundle[ABOVE][SUBJ] +
		(edge->bundle[BELOW][SUBJ] << 1);

	    MSG( 1, "            edge (%.2f, %.2f) -> (%.2f, %.2f) e: %d\n",
		 edge->bot[X], edge->bot[Y], edge->top[X], edge->top[Y],
		 exists[SUBJ] );

	    if ( exists[SUBJ] )
	    {
		MSG( 1, "              parity s: %s fwd: %s\n",
		     ( parity[SUBJ] ) ? "RIGHT" : "LEFT",
		     ( edge->forward ) ? "TRUE" : "FALSE" );

		/* Set bundle side */
		edge->bside[SUBJ] = parity[SUBJ];

		/* Determine contributing status and quadrant occupancies */
		contributing = exists[SUBJ];

		br = ( parity[SUBJ] );

		bl = ( parity[SUBJ] ^ edge->bundle[ABOVE][SUBJ] );

		tr = ( parity[SUBJ] ^ ( horiz[SUBJ] != HORIZ_NONE ) );

		tl = ( parity[SUBJ] ^ ( horiz[SUBJ] != HORIZ_NONE )
		       ^ edge->bundle[BELOW][SUBJ] );

		/* If the edge has been reversed, swap the flags */
		if ( ( ( parity[SUBJ] == LEFT  ) &&  edge->forward ) ||
		     ( ( parity[SUBJ] == RIGHT ) && !edge->forward ) )
		{
		    MSG( 1, "              swapping flags...\n" );
		    br = !br; bl = !bl; tr = !tr; tl = !tl;
		}

		/* Update parity */
		parity[SUBJ] ^= edge->bundle[ABOVE][SUBJ];

		/* Update horizontal state */
		if ( exists[SUBJ] ) {
		    horiz[SUBJ] = next_state[ horiz[SUBJ] ][ ((exists[SUBJ]-1)<<1) + parity[SUBJ] ];
		}

		vclass = tr + (tl << 1) + (br << 2) + (bl << 3);

		MSG( 1, "              cont: %d bl: %d br: %d tl: %d tr: %d %s\n",
		     contributing, bl, br, tl, tr, class_string( vclass ) );

		MSG( 1, "\n" );
		MSG( 1, "BEFORE:\n" );
		if ( current[ABOVE] ) {
		    MSG( 1, "  ca:\n" );
		    print_contour( current[ABOVE] );
		}
		if ( current[BELOW] ) {
		    MSG( 1, "  cb:\n" );
		    print_contour( current[BELOW] );
		}
		if ( edge->out[ABOVE] ) {
		    MSG( 1, "  above:\n" );
		    print_contour( edge->out[ABOVE] );
		}
		if ( edge->out[BELOW] ) {
		    MSG( 1, "  below:\n" );
		    print_contour( edge->out[BELOW] );
		}
		MSG( 1, "\n" );

		if ( contributing )
		{
		    xb = edge->xbot;

		    MSG( 1, "              xb: %.2f px: %.2f\n",
			 xb, ( prevx != -DBL_MAX ) ? prevx : -666.0 );
		    MSG( 1, "                  add (%.2f, %.2f)\n", xb, yb );

		    switch ( vclass )
		    {
		    case EDGE_EXT_MIN:
		    case EDGE_INT_MIN:
			add_local_min( &out_poly, edge, edge->vertices[BOT] );
			prevx = xb;
			current[BELOW] = edge->out[ABOVE];
			forward = edge->forward;
			break;
		    case EDGE_EXT_RI:
			if ( xb != prevx )
			{
			    add_right( current[BELOW], edge->vertices[BOT] );
			    prevx = xb;
			}
			edge->out[ABOVE] = current[BELOW];
			current[BELOW] = NULL;
			break;
		    case EDGE_EXT_LI:
			add_left( edge->out[BELOW], edge->vertices[TOP] );
			prevx = xb;
			current[BELOW] = edge->out[BELOW];
			forward = edge->forward;
			break;
		    case EDGE_EXT_MAX:
			if ( xb != prevx )
			{
			    add_right( current[BELOW], edge->vertices[TOP] );
			    prevx = xb;
			}
			merge_left( current[BELOW], edge->out[BELOW],
				    out_poly );
			current[BELOW] = NULL;
			break;
		    case EDGE_INT_MAX:
			if ( xb != prevx )
			{
			    add_left( current[BELOW], edge->vertices[TOP] );
			    prevx = xb;
			}
			merge_right( current[BELOW], edge->out[BELOW],
				     out_poly );
			current[BELOW] = NULL;
			break;
		    case EDGE_INT_LI:
			if ( xb != prevx )
			{
			    add_left( current[BELOW], edge->vertices[BOT] );
			    prevx = xb;
			}
			edge->out[ABOVE] = current[BELOW];
			current[BELOW] = NULL;
			break;
		    case EDGE_INT_RI:
			add_right( edge->out[BELOW], edge->vertices[TOP] );
			prevx = xb;
			current[BELOW] = edge->out[BELOW];
			edge->out[BELOW] = NULL;
			break;
		    case EDGE_EXT_MAX_MIN:
		    case EDGE_INT_MAX_MIN:
			quadrant = find_intersection( tobj, &it,
						      aet, edge, yb, yt );
			switch ( quadrant )
			{
			case WIND_NE:
			    if ( xb != prevx ) {
				add_right( current[BELOW],
					   it_vertex( tobj, it ) );
				merge_left( current[BELOW], edge->out[BELOW],
					    out_poly );
			    }
			    add_local_min( &out_poly, edge,
					   it_vertex( tobj, it ) );
			    prevx = xb;
			    current[BELOW] = edge->out[ABOVE];
			    forward = edge->forward;
			    break;
			case WIND_SE:
			    if ( xb != prevx ) {
				add_left( current[BELOW],
					  it_vertex( tobj, it ) );
			    }
			    add_left( edge->out[BELOW],
				      it_vertex( tobj, it ) );
			    prevx = xb;
			    edge->out[ABOVE] = current[BELOW];
			    current[BELOW] = edge->out[BELOW];
			    break;
			case WIND_SW:
			    if ( xb != prevx ) {
				add_left( current[BELOW],
					  it_vertex( tobj, it ) );
				merge_right( current[BELOW], edge->out[BELOW],
					     out_poly );
			    }
			    add_local_min( &out_poly, edge,
					   it_vertex( tobj, it ) );
			    prevx = xb;
			    current[BELOW] = edge->out[ABOVE];
			    forward = edge->forward;
			    break;
			case WIND_NW:
			    if ( xb != prevx ) {
				add_right( current[BELOW],
					   it_vertex( tobj, it ) );
			    }
			    add_right( edge->out[BELOW],
				       it_vertex( tobj, it ) );
			    prevx = xb;
			    edge->out[ABOVE] = current[BELOW];
			    current[BELOW] = edge->out[BELOW];
			    edge->out[BELOW] = NULL;
			    break;
			}
			break;
		    case EDGE_LEFT:
			if ( edge->bot[Y] == yb ) {
			    add_left( edge->out[BELOW], edge->vertices[BOT] );
			}
			edge->out[ABOVE] = edge->out[BELOW];
			prevx = xb;
			break;
		    case EDGE_RIGHT:
			if ( edge->bot[Y] == yb ) {
			    add_right( edge->out[BELOW], edge->vertices[BOT] );
			}
			edge->out[ABOVE] = edge->out[BELOW];
			prevx = xb;
			break;
		    default:
			break;
		    }
		}

		MSG( 1, "\n" );
		MSG( 1, "AFTER:\n" );
		if ( current[BELOW] ) {
		    MSG( 1, "  cb:\n" );
		    print_contour( current[BELOW] );
		}
		if ( out_poly ) {
		    MSG( 1, "  out:\n" );
		    print_all_contours( out_poly );
		}
	    }
	    MSG( 1, "\n" );
	}

	/* Delete terminating edges from the AET, otherwise compute xt */
	for ( edge = aet ; edge ; edge = edge->next )
	{
	    if ( edge->top[Y] == yb )
	    {
		prev_edge = edge->prev;
		next_edge = edge->next;

		if ( prev_edge ) {
		    prev_edge->next = next_edge;
		} else {
		    aet= next_edge;
		}

		if ( next_edge ) {
		    next_edge->prev = prev_edge;
		}

		/* Copy bundle state to the adjacent tail edge if required */
		if ( ( edge->bstate[BELOW] == BUNDLE_HEAD ) && prev_edge )
		{
		    if ( prev_edge->bstate[BELOW] == BUNDLE_TAIL )
		    {
			prev_edge->out[BELOW] = edge->out[BELOW];
			prev_edge->bstate[BELOW] = UNBUNDLED;

			if ( ( prev_edge->prev ) &&
			     ( prev_edge->prev->bstate[BELOW]
			       == BUNDLE_TAIL ) )
			{
			    prev_edge->bstate[BELOW] = BUNDLE_HEAD;
			}
		    }
		}
	    }
	    else
	    {
		if ( edge->top[Y] == yt ) {
		    edge->xtop = edge->top[X];
		} else {
		    edge->xtop = edge->bot[X]
			+ edge->dx * (yt - edge->bot[Y]);
		}
	    }
	}

	if ( scanbeam < sbt_entries )
	{
	    /*
	     * ****** SCANBEAM INTERIOR PROCESSING ******
	     */

	    build_intersection_table( tobj, &it, aet, dy );

	    /* Process each node in the intersection table */
	    for ( intersect = it; intersect ; intersect = intersect->next )
	    {
		e0 = intersect->ie[0];
		e1 = intersect->ie[1];

		MSG( 1, "          *** intersect: (%.2f, %.2f) e0: %d e1: %d\n",
		     intersect->v[X], intersect->v[Y],
		     e0->forward, e1->forward );

		MSG( 1, "            e0 (%.2f, %.2f) -> (%.2f, %.2f)\n",
		     e0->bot[X], e0->bot[Y], e0->top[X], e0->top[Y] );
		MSG( 1, "            e1 (%.2f, %.2f) -> (%.2f, %.2f)\n",
		     e1->bot[X], e1->bot[Y], e1->top[X], e1->top[Y] );

		/* Only generate output for contributing intersections */
		if ( e0->bundle[ABOVE][SUBJ] && e1->bundle[ABOVE][SUBJ] )
		{
		    p = e0->out[ABOVE];
		    q = e1->out[ABOVE];

		    ix = intersect->v[X];
		    iy = intersect->v[Y];

		    in[SUBJ] =
			(  e0->bundle[ABOVE][SUBJ] && !e0->bside[SUBJ] ) ||
			(  e1->bundle[ABOVE][SUBJ] &&  e1->bside[SUBJ] ) ||
			( !e0->bundle[ABOVE][SUBJ] && !e1->bundle[ABOVE][SUBJ]
			    && e0->bside[SUBJ] && e1->bside[SUBJ] );

		    /* Determine quadrant occupancies */
		    tr = ( in[SUBJ] );

		    tl = ( in[SUBJ] ^ e1->bundle[ABOVE][SUBJ] );

		    br = ( in[SUBJ] ^ e0->bundle[ABOVE][SUBJ] );

		    bl = ( in[SUBJ] ^ e1->bundle[ABOVE][SUBJ]
			   ^ e0->bundle[ABOVE][SUBJ] );

		    /* If the edge has been reversed, swap the flags */
		    if ( !e0->forward || e1->forward ) {
			MSG( 1, "              swapping flags...\n" );
			br = !br; bl = !bl; tr = !tr; tl = !tl;
		    }

		    vclass = tr + (tl << 1) + (br << 2) + (bl << 3);

		    MSG( 1, "              bl: %d br: %d tl: %d tr: %d %s\n",
			 bl, br, tl, tr, class_string( vclass ) );

		    MSG( 1, "                  add (%.2f, %.2f)\n", ix, iy );

		    MSG( 1, "\n" );
		    MSG( 1, "BEFORE:\n" );
		    if ( p ) {
			MSG( 1, "  p:\n" );
			print_contour( p );
		    }
		    if ( q ) {
			MSG( 1, "  q:\n" );
			print_contour( q );
		    }
		    if ( e0->out[ABOVE] ) {
			MSG( 1, "  e0 above:\n" );
			print_contour( e0->out[ABOVE] );
		    }
		    if ( e0->out[BELOW] ) {
			MSG( 1, "  e0 below:\n" );
			print_contour( e0->out[BELOW] );
		    }
		    if ( e1->out[ABOVE] ) {
			MSG( 1, "  e1 above:\n" );
			print_contour( e1->out[ABOVE] );
		    }
		    if ( e1->out[BELOW] ) {
			MSG( 1, "  e1 below:\n" );
			print_contour( e1->out[BELOW] );
		    }
		    MSG( 1, "\n" );

		    switch ( vclass )
		    {
		    case EDGE_EXT_MIN:
			add_local_min( &out_poly, e0,
				       it_vertex( tobj, intersect ) );
			e1->out[ABOVE] = e0->out[ABOVE];
			break;
		    case EDGE_EXT_RI:
			if ( p )
			{
			    add_right( p, e1->vertices[BOT] );
			    e1->out[ABOVE] = p;
			    e0->out[ABOVE] = NULL;
			}
			break;
		    case EDGE_EXT_LI:
			if ( q )
			{
			    add_left( q, e0->vertices[TOP] );
			    e0->out[ABOVE] = q;
			    e1->out[ABOVE] = NULL;
			}
			break;
		    case EDGE_EXT_MAX:
			if ( p && q )
			{
			    add_left( p, e1->vertices[TOP] );
			    merge_right( p, q, out_poly );
			    e0->out[ABOVE] = NULL;
			    e1->out[ABOVE] = NULL;
			}
			break;
		    case EDGE_INT_MIN:
			add_local_min( &out_poly, e0, e0->vertices[BOT] );
			e1->out[ABOVE] = e0->out[ABOVE];
			break;
		    case EDGE_INT_LI:
			if ( p )
			{
			    add_left( p, e1->vertices[BOT] );
			    e1->out[ABOVE] = p;
			    e0->out[ABOVE] = NULL;
			}
			break;
		    case EDGE_INT_RI:
			if ( q )
			{
			    add_right( q, e0->vertices[TOP] );
			    e0->out[ABOVE] = q;
			    e1->out[ABOVE] = NULL;
			}
			break;
		    case EDGE_INT_MAX:
			if ( p && q )
			{
			    add_right( p, e1->vertices[TOP] );
			    merge_left( p, q, out_poly );
			    e0->out[ABOVE] = NULL;
			    e1->out[ABOVE] = NULL;
			}
			break;
		    case EDGE_INT_MAX_MIN:
			if ( p && q )
			{
			    add_right( p, it_vertex( tobj, intersect ) );
			    merge_left( p, q, out_poly );
			    add_local_min( &out_poly, e0,
					   it_vertex( tobj, intersect ) );
			    e1->out[ABOVE] = e0->out[ABOVE];
			}
			break;
		    case EDGE_EXT_MAX_MIN:
			if ( p && q )
			{
			    if ( e0->forward ) {
				add_right( p, it_vertex( tobj, intersect ) );
			    } else {
				add_left( p, it_vertex( tobj, intersect ) );
			    }
			    if ( e1->forward ) {
				add_right( q, it_vertex( tobj, intersect ) );
			    } else {
				add_left( q, it_vertex( tobj, intersect ) );
			    }

			    e0->out[ABOVE] = q;
			    e1->out[ABOVE] = p;
			}
			break;
		    default:
			break;
		    }

		    MSG( 1, "\n" );
		    MSG( 1, "AFTER:\n" );
		    if ( p ) {
			MSG( 1, "  p:\n" );
			print_contour( p );
		    }
		    if ( q ) {
			MSG( 1, "  q:\n" );
			print_contour( q );
		    }
		    if ( e0->out[ABOVE] ) {
			MSG( 1, "  e0 above:\n" );
			print_contour( e0->out[ABOVE] );
		    }
		    if ( e0->out[BELOW] ) {
			MSG( 1, "  e0 below:\n" );
			print_contour( e0->out[BELOW] );
		    }
		    if ( e1->out[ABOVE] ) {
			MSG( 1, "  e1 above:\n" );
			print_contour( e1->out[ABOVE] );
		    }
		    if ( e1->out[BELOW] ) {
			MSG( 1, "  e1 below:\n" );
			print_contour( e1->out[BELOW] );
		    }
		    if ( out_poly ) {
			MSG( 1, "  out:\n" );
			print_all_contours( out_poly );
		    }
		    MSG( 1, "\n" );
		}

		/* Swap bundle sides in response to edge crossing */
		if ( e0->bundle[ABOVE][SUBJ] ) {
		    e1->bside[SUBJ] = !e1->bside[SUBJ];
		}
		if ( e1->bundle[ABOVE][SUBJ] ) {
		    e0->bside[SUBJ] = !e0->bside[SUBJ];
		}

		/* Swap e0 and e1 bundles in the AET */
		prev_edge = e0->prev;
		next_edge = e1->next;

		if ( next_edge ) {
		    next_edge->prev= e0;
		}

		if ( e0->bstate[ABOVE] == BUNDLE_HEAD )
		{
		    search = GL_TRUE;

		    while ( search )
		    {
			prev_edge = prev_edge->prev;

			if ( prev_edge )
			{
			    if ( prev_edge->bstate[ABOVE] != BUNDLE_TAIL ) {
				search = GL_FALSE;
			    }
			}
			else
			{
			    search = GL_FALSE;
			}
		    }
		}

		if ( !prev_edge )
		{
		    aet->prev = e1;
		    e1->next = aet;
		    aet = e0->next;
		}
		else
		{
		    prev_edge->next->prev = e1;
		    e1->next = prev_edge->next;
		    prev_edge->next = e0->next;
		}

		e0->next->prev = prev_edge;
		e1->next->prev = e1;
		e0->next = next_edge;
	    }

	    /* Prepare for next scanbeam */
	    for ( edge = aet ; edge ; edge = next_edge )
	    {
		next_edge = edge->next;
		succ_edge = edge->succ;

		if ( ( edge->top[Y] == yt ) && succ_edge )
		{
		    /* Replace AET edge by its successor */
		    succ_edge->out[BELOW] = edge->out[ABOVE];
		    succ_edge->bstate[BELOW] = edge->bstate[ABOVE];
		    succ_edge->bundle[BELOW][SUBJ] = edge->bundle[ABOVE][SUBJ];
		    prev_edge = edge->prev;

		    if ( prev_edge ) {
			prev_edge->next = succ_edge;
		    } else {
			aet = succ_edge;
		    }

		    if ( next_edge ) {
			next_edge->prev = succ_edge;
		    }

		    succ_edge->prev = prev_edge;
		    succ_edge->next = next_edge;
		}
		else
		{
		    /* Update this edge */
		    edge->out[BELOW] = edge->out[ABOVE];
		    edge->bstate[BELOW] = edge->bstate[ABOVE];
		    edge->bundle[BELOW][SUBJ] = edge->bundle[ABOVE][SUBJ];
		    edge->xbot = edge->xtop;
		}

		edge->out[ABOVE] = NULL;
	    }
	}
    }
    MSG( 1, "\n" );
    MSG( 1, "          scanbeam processing complete!\n" );

    output_contours( tobj, out_poly );

    cleanup_it( &it );
    cleanup_lmt( &lmt );

    if ( heap ) {
	free( heap );
    }
    if ( sbt ) {
	free( sbt );
    }

    MSG( 1, "    <-- clip_polygons( tobj:%p )\n", tobj );
    return GLU_NO_ERROR;
}
