/* $Id: tess_macros.h,v 1.6.2.3 1999/12/05 02:04:31 gareth Exp $ */

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

#ifndef __GLU_TESS_MACROS_H__
#define __GLU_TESS_MACROS_H__

#if defined(__GNUC__)
#define INLINE __inline__
#elif defined(__MSC__)
#define INLINE __inline
#else
#define INLINE
#endif

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#define X	0
#define Y	1
#define Z	2

/* Limits: */
#ifndef DBL_MAX
#define DBL_MAX			1e+37
#endif

#define MAX_GLDOUBLE		DBL_MAX
#define GLU_TESS_EPSILON	1e-06

/* Radians to degrees conversion: */

#define DEG_TO_RAD( A )		( (A) * ( M_PI / 180.0 ) )
#define RAD_TO_DEG( A )		( (A) * ( 180.0 / M_PI ) )

/* Absolute value: */

#define ABSI( X )		( ( (X) < 0 ) ? -(X) : (X) )
#define ABSF( X )		( ( (X) < 0.0F ) ? -(X) : (X) )
#define ABSD( X )		( ( (X) < 0.0 ) ? -(X) : (X) )

/* Dot product: */

#define DOT_3V( A, B )		( (A)[0]*(B)[0] + (A)[1]*(B)[1] + (A)[2]*(B)[2] )

/* Cross product: */

#define CROSS_3V( n, u, v ) 				\
do {							\
   (n)[0] = (u)[1]*(v)[2] - (u)[2]*(v)[1]; 		\
   (n)[1] = (u)[2]*(v)[0] - (u)[0]*(v)[2]; 		\
   (n)[2] = (u)[0]*(v)[1] - (u)[1]*(v)[0];		\
} while ( 0 )

/* Vector clear: */

#define ZERO_3V( DST )		(DST)[0] = (DST)[1] = (DST)[2] = 0

/* Vector subtract: */

#define SUB_2V( DST, SRCA, SRCB )			\
do {							\
    (DST)[0] = (SRCA)[0] - (SRCB)[0];			\
    (DST)[1] = (SRCA)[1] - (SRCB)[1];			\
} while ( 0 )

#define SUB_3V( DST, SRCA, SRCB )			\
do {							\
    (DST)[0] = (SRCA)[0] - (SRCB)[0];			\
    (DST)[1] = (SRCA)[1] - (SRCB)[1];			\
    (DST)[2] = (SRCA)[2] - (SRCB)[2];			\
} while ( 0 )

/* Vector copy: */

#define COPY_3V( DST, SRC )				\
do {							\
    (DST)[0] = (SRC)[0];				\
    (DST)[1] = (SRC)[1];				\
    (DST)[2] = (SRC)[2];				\
} while ( 0 )

/* Vector negate: */

#define NEG_3V( DST )					\
do {							\
    (DST)[0] = -(DST)[0];				\
    (DST)[1] = -(DST)[1];				\
    (DST)[2] = -(DST)[2];				\
} while ( 0 )

/* Assign scalers to short vectors: */

#define ASSIGN_3V( V, V0, V1, V2 ) 			\
do { 							\
    V[0] = V0;						\
    V[1] = V1;						\
    V[2] = V2;						\
} while ( 0 )

#define ASSIGN_4V( V, V0, V1, V2, V3 ) 			\
do { 							\
    V[0] = V0;						\
    V[1] = V1;						\
    V[2] = V2;						\
    V[3] = V3; 						\
} while ( 0 )

/* Comparison: */

#define IS_EQUAL( A, B )	( ABSD( (A) - (B) ) < GLU_TESS_EPSILON )

#define IS_EQUAL_3DV( A, B )				\
( ( ABSD( (A)[X] - (B)[X] ) < GLU_TESS_EPSILON ) &&	\
  ( ABSD( (A)[Y] - (B)[Y] ) < GLU_TESS_EPSILON ) &&	\
  ( ABSD( (A)[Z] - (B)[Z] ) < GLU_TESS_EPSILON ) )

#define IS_ZERO_3DV( A )				\
( ( ABSD( (A)[X] ) < GLU_TESS_EPSILON ) &&		\
  ( ABSD( (A)[Y] ) < GLU_TESS_EPSILON ) &&		\
  ( ABSD( (A)[Z] ) < GLU_TESS_EPSILON ) )

#define IS_LEQ( A, B )		( ( (A) - (B) ) < GLU_TESS_EPSILON )

/* Length: */

#define LEN_SCALAR( V1, V2 )	( sqrt( (V1)*(V1) + (V2)*(V2) ) )

#define NORMALIZE_3DV( V )				\
do {							\
    GLdouble len = LEN_SQUARED_3DV(V);			\
    if (len > 1e-50) {					\
	len = 1.0 / sqrt(len);				\
	V[0] = (GLfloat) (V[0] * len);			\
	V[1] = (GLfloat) (V[1] * len);			\
	V[2] = (GLfloat) (V[2] * len);			\
    }							\
} while ( 0 )

#define LEN_3DV( V )		( sqrt( V[0]*V[0] + V[1]*V[1] + V[2]*V[2] ) )

#define LEN_SQUARED_3DV( V )	( V[0]*V[0] + V[1]*V[1] + V[2]*V[2] )

#define NORMALIZE_2DV( V )				\
do {							\
   GLdouble len = LEN_SQUARED_2DV( V );			\
   if ( len > 1e-50 ) {					\
      len = 1.0 / sqrt( len );				\
      (V)[0] *= len;					\
      (V)[1] *= len;					\
   }							\
} while ( 0 )

#define LEN_2DV( V )		( sqrt( (V)[0]*(V)[0] + (V)[1]*(V)[1] ) )

#define LEN_SQUARED_2DV( V )	( (V)[0]*(V)[0] + (V)[1]*(V)[1] )


/* Bounding boxes: */

#define CLEAR_BBOX_2DV( MINS, MAXS )			\
do {							\
    (MINS)[X] = (MINS)[Y] = MAX_GLDOUBLE;		\
    (MAXS)[X] = (MAXS)[Y] = -MAX_GLDOUBLE;		\
} while ( 0 )

#define ACC_BBOX_2V( V, MINS, MAXS )			\
do {							\
    if ( (V)[X] < (MINS)[X] ) {				\
	(MINS)[X] = (V)[X];				\
    }							\
    if ( (V)[X] > (MAXS)[X] ) {				\
	(MAXS)[X] = (V)[X];				\
    }							\
    if ( (V)[Y] < (MINS)[Y] ) {				\
	(MINS)[Y] = (V)[Y];				\
    }							\
    if ( (V)[Y] > (MAXS)[Y] ) {				\
	(MAXS)[Y] = (V)[Y];				\
    }							\
} while ( 0 )

/* Test if bbox B is inside bbox A: */
#define CONTAINS_BBOX_2V( AMINS, AMAXS, BMINS, BMAXS )			\
( ( (AMINS)[X] < (BMINS)[X] ) && ( (AMINS)[Y] < (BMINS)[Y] ) &&		\
  ( (AMAXS)[X] > (BMAXS)[X] ) && ( (AMAXS)[Y] > (BMAXS)[Y] ) )

#define INSIDE_BBOX_2V( V, MINS, MAXS )			\
( ( (MINS)[X] < (V)[X] ) && ( (MINS)[Y] < (V)[Y] ) &&	\
  ( (MAXS)[X] > (V)[X] ) && ( (MAXS)[Y] > (V)[Y] ) )

#endif /* __GLU_TESS_MACROS_H__ */
