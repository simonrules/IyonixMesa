/****************************************************************************
*
*				 	   Mesa bindings for SciTech MGL
*
*               Copyright (C) 1996-1998 SciTech Software, Inc.
*                            All rights reserved.
*
* Language:     ANSI C
* Environment:	Any
*
* Description:	Include file defining macros to implement many of the
*				optimized rendering functions for the MGL Mesa driver.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
****************************************************************************/

/**********************************************************************/
/*****            Write spans of pixels                           *****/
/**********************************************************************/

#define	IMPLEMENT_WRITE_SPAN(DEPTH,FMT,TYPE)                             	\
void _mmesa_write_span_##DEPTH##_##FMT (GLcontext *ctx,GLuint n,GLint x,	\
	GLint y,GLubyte rgba[][4],GLubyte mask[])                               \
{                                                                           \
	TYPE *d = PACKED##DEPTH##_pixelAddr(x,FLIP(y));                         \
	if (mask) {                                                             \
		while (n >= 8) {                                                    \
			if (mask[0]) d[0] = PACK_COLOR_##FMT(rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);  \
			if (mask[1]) d[1] = PACK_COLOR_##FMT(rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]);  \
			if (mask[2]) d[2] = PACK_COLOR_##FMT(rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]);  \
			if (mask[3]) d[3] = PACK_COLOR_##FMT(rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]);  \
			if (mask[4]) d[4] = PACK_COLOR_##FMT(rgba[4][RCOMP],rgba[4][GCOMP],rgba[4][BCOMP]);  \
			if (mask[5]) d[5] = PACK_COLOR_##FMT(rgba[5][RCOMP],rgba[5][GCOMP],rgba[5][BCOMP]);  \
			if (mask[6]) d[6] = PACK_COLOR_##FMT(rgba[6][RCOMP],rgba[6][GCOMP],rgba[6][BCOMP]);  \
			if (mask[7]) d[7] = PACK_COLOR_##FMT(rgba[7][RCOMP],rgba[7][GCOMP],rgba[7][BCOMP]);  \
			d += 8; rgba += 8; mask += 8; n -= 8;     						\
			}                                                               \
		while (n--) {                                                       \
			if (mask[0]) d[0] = PACK_COLOR_##FMT(rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);  \
			d++; rgba++; mask++;                            				\
			}                                                               \
		}                                                                   \
	else {                                                                  \
		while (n >= 8) {                                                    \
			d[0] = PACK_COLOR_##FMT(rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);               \
			d[1] = PACK_COLOR_##FMT(rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]);               \
			d[2] = PACK_COLOR_##FMT(rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]);               \
			d[3] = PACK_COLOR_##FMT(rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]);               \
			d[4] = PACK_COLOR_##FMT(rgba[4][RCOMP],rgba[4][GCOMP],rgba[4][BCOMP]);               \
			d[5] = PACK_COLOR_##FMT(rgba[5][RCOMP],rgba[5][GCOMP],rgba[5][BCOMP]);               \
			d[6] = PACK_COLOR_##FMT(rgba[6][RCOMP],rgba[6][GCOMP],rgba[6][BCOMP]);               \
			d[7] = PACK_COLOR_##FMT(rgba[7][RCOMP],rgba[7][GCOMP],rgba[7][BCOMP]);               \
			d += 8; rgba += 8; n -= 8;                						\
			}                                                               \
		while (n--) {                                                       \
			d[0] = PACK_COLOR_##FMT(rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);\
			d++; rgba++;													\
			}                                                               \
		}                                                                   \
}

#define	IMPLEMENT_WRITE_SPAN_RGB(DEPTH,FMT,TYPE)                             	\
void _mmesa_write_span_rgb_##DEPTH##_##FMT (GLcontext *ctx,GLuint n,GLint x,	\
	GLint y,GLubyte rgba[][3],GLubyte mask[])                               \
{                                                                           \
	TYPE *d = PACKED##DEPTH##_pixelAddr(x,FLIP(y));                         \
	if (mask) {                                                             \
		while (n >= 8) {                                                    \
			if (mask[0]) d[0] = PACK_COLOR_##FMT(rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);  \
			if (mask[1]) d[1] = PACK_COLOR_##FMT(rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]);  \
			if (mask[2]) d[2] = PACK_COLOR_##FMT(rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]);  \
			if (mask[3]) d[3] = PACK_COLOR_##FMT(rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]);  \
			if (mask[4]) d[4] = PACK_COLOR_##FMT(rgba[4][RCOMP],rgba[4][GCOMP],rgba[4][BCOMP]);  \
			if (mask[5]) d[5] = PACK_COLOR_##FMT(rgba[5][RCOMP],rgba[5][GCOMP],rgba[5][BCOMP]);  \
			if (mask[6]) d[6] = PACK_COLOR_##FMT(rgba[6][RCOMP],rgba[6][GCOMP],rgba[6][BCOMP]);  \
			if (mask[7]) d[7] = PACK_COLOR_##FMT(rgba[7][RCOMP],rgba[7][GCOMP],rgba[7][BCOMP]);  \
			d += 8; rgba += 8; mask += 8; n -= 8;     						\
			}                                                               \
		while (n--) {                                                       \
			if (mask[0]) d[0] = PACK_COLOR_##FMT(rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);  \
			d++; rgba++; mask++;                            				\
			}                                                               \
		}                                                                   \
	else {                                                                  \
		while (n >= 8) {                                                    \
			d[0] = PACK_COLOR_##FMT(rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);               \
			d[1] = PACK_COLOR_##FMT(rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]);               \
			d[2] = PACK_COLOR_##FMT(rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]);               \
			d[3] = PACK_COLOR_##FMT(rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]);               \
			d[4] = PACK_COLOR_##FMT(rgba[4][RCOMP],rgba[4][GCOMP],rgba[4][BCOMP]);               \
			d[5] = PACK_COLOR_##FMT(rgba[5][RCOMP],rgba[5][GCOMP],rgba[5][BCOMP]);               \
			d[6] = PACK_COLOR_##FMT(rgba[6][RCOMP],rgba[6][GCOMP],rgba[6][BCOMP]);               \
			d[7] = PACK_COLOR_##FMT(rgba[7][RCOMP],rgba[7][GCOMP],rgba[7][BCOMP]);               \
			d += 8; rgba += 8; n -= 8;                						\
			}                                                               \
		while (n--) {                                                       \
			d[0] = PACK_COLOR_##FMT(rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);\
			d++; rgba++;													\
			}                                                               \
		}                                                                   \
}

#define	IMPLEMENT_WRITE_SPAN_DITHER(DEPTH,FMT,TYPE)                     	\
void _mmesa_write_span_##DEPTH##_##FMT (GLcontext *ctx,GLuint n,GLint x,	\
	GLint y,GLubyte rgba[][4],GLubyte mask[])                               \
{                                                                                   \
	TYPE *d;                                                                        \
	y = FLIP(y);                                                                    \
	d = PACKED##DEPTH##_pixelAddr(x,y);                                             \
	if (mask) {                                                                     \
		while (n >= 4) {                                                            \
			if (mask[0]) PACK_COLOR_##FMT(d[0],x,y,rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);        \
			if (mask[1]) PACK_COLOR_##FMT(d[1],x+1,y,rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]);      \
			if (mask[2]) PACK_COLOR_##FMT(d[2],x+2,y,rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]);      \
			if (mask[3]) PACK_COLOR_##FMT(d[3],x+3,y,rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]);      \
			d += 4; rgba += 4; mask += 4; x += 4; n -= 4;     						\
			}                                                                       \
		while (n--) {                                                               \
			if (mask[0]) PACK_COLOR_##FMT(d[0],x,y,rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);        \
			d++; rgba++; mask++; x++;                               				\
			}                                                                       \
		}                                                                           \
	else {                                                                          \
		while (n >= 4) {                                                            \
			PACK_COLOR_##FMT(d[0],x,y,rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);                     \
			PACK_COLOR_##FMT(d[1],x+1,y,rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]);                   \
			PACK_COLOR_##FMT(d[2],x+2,y,rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]);                   \
			PACK_COLOR_##FMT(d[3],x+3,y,rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]);                   \
			d += 4; rgba += 4; x += 4; n -= 4;                						\
			}                                                                       \
		while (n--) {                                                               \
			PACK_COLOR_##FMT(d[0],x,y,rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);                     \
			d++; rgba++; x++;                                       				\
			}                                                                       \
		}                                                                           \
}

#define	IMPLEMENT_WRITE_SPAN_RGB_DITHER(DEPTH,FMT,TYPE)                     	\
void _mmesa_write_span_rgb_##DEPTH##_##FMT (GLcontext *ctx,GLuint n,GLint x,	\
	GLint y,GLubyte rgba[][3],GLubyte mask[])                               	\
{                                                                                   \
	TYPE *d;                                                                        \
	y = FLIP(y);                                                                    \
	d = PACKED##DEPTH##_pixelAddr(x,y);                                             \
	if (mask) {                                                                     \
		while (n >= 4) {                                                            \
			if (mask[0]) PACK_COLOR_##FMT(d[0],x,y,rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);        \
			if (mask[1]) PACK_COLOR_##FMT(d[1],x+1,y,rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]);      \
			if (mask[2]) PACK_COLOR_##FMT(d[2],x+2,y,rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]);      \
			if (mask[3]) PACK_COLOR_##FMT(d[3],x+3,y,rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]);      \
			d += 4; rgba += 4; mask += 4; x += 4; n -= 4;     						\
			}                                                                       \
		while (n--) {                                                               \
			if (mask[0]) PACK_COLOR_##FMT(d[0],x,y,rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);        \
			d++; rgba++; mask++; x++;                               				\
			}                                                                       \
		}                                                                           \
	else {                                                                          \
		while (n >= 4) {                                                            \
			PACK_COLOR_##FMT(d[0],x,y,rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);                     \
			PACK_COLOR_##FMT(d[1],x+1,y,rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]);                   \
			PACK_COLOR_##FMT(d[2],x+2,y,rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]);                   \
			PACK_COLOR_##FMT(d[3],x+3,y,rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]);                   \
			d += 4; rgba += 4; x += 4; n -= 4;                						\
			}                                                                       \
		while (n--) {                                                               \
			PACK_COLOR_##FMT(d[0],x,y,rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);                     \
			d++; rgba++; x++;                                       				\
			}                                                                       \
		}                                                                           \
}

#define	IMPLEMENT_WRITE_SPAN_MONO(DEPTH,TYPE)								\
void _mmesa_write_span_mono_##DEPTH (GLcontext *ctx,GLuint n,GLint x,		\
	GLint y,GLubyte mask[])                                                 \
{                                                                           \
	TYPE *d = PACKED##DEPTH##_pixelAddr(x,FLIP(y));                         \
	while (n >= 8) {                                                        \
		if (mask[0]) d[0] = (TYPE)RC.color;                                 \
		if (mask[1]) d[1] = (TYPE)RC.color;                                 \
		if (mask[2]) d[2] = (TYPE)RC.color;                                 \
		if (mask[3]) d[3] = (TYPE)RC.color;                                 \
		if (mask[4]) d[4] = (TYPE)RC.color;                                 \
		if (mask[5]) d[5] = (TYPE)RC.color;                                 \
		if (mask[6]) d[6] = (TYPE)RC.color;                                 \
		if (mask[7]) d[7] = (TYPE)RC.color;                                 \
		d += 8; mask += 8; n -= 8;                                          \
		}                                                                   \
	while (n--) {                                                           \
		if (mask[0]) d[0] = (TYPE)RC.color;                                 \
		d++; mask++;                                                        \
		}                                                                   \
}

#define	IMPLEMENT_WRITE_SPAN_MONO_DITHER(DEPTH,FMT,TYPE)						\
void _mmesa_write_span_mono_##DEPTH##_##FMT (GLcontext *ctx,GLuint n,GLint x,	\
	GLint y,GLubyte mask[])                                                 	\
{                                                                           	\
	HALFTONE_VARS_##DEPTH;                                                      \
	TYPE *d;                                                                    \
	y = FLIP(y);                                                                \
	d = PACKED##DEPTH##_pixelAddr(x,y);                                         \
	SETUP_##FMT(y,RC.red,RC.green,RC.blue);                                     \
	while (n >= 4) {                                                            \
		if (mask[0]) PACK_COLOR2_##FMT(d[0],x);                                 \
		if (mask[1]) PACK_COLOR2_##FMT(d[1],x+1);                               \
		if (mask[2]) PACK_COLOR2_##FMT(d[2],x+2);                               \
		if (mask[3]) PACK_COLOR2_##FMT(d[3],x+3);                               \
		d += 4; mask += 4; x += 4; n -= 4;                                      \
		}                                                                       \
	while (n--) {                                                               \
		if (mask[0]) PACK_COLOR2_##FMT(d[0],x);                                 \
		d++; mask++; x++;                                                       \
		}                                                                       \
}

/**********************************************************************/
/*****              Write arrays of pixels                        *****/
/**********************************************************************/

#define	IMPLEMENT_WRITE_PIXELS(DEPTH,FMT,TYPE)																	\
void _mmesa_write_pixels_##DEPTH##_##FMT (GLcontext *ctx,GLuint n,GLint x[],									\
	GLint y[],GLubyte rgba[][4],GLubyte mask[])																	\
{                                                                                                               \
	while (n >= 4) {                                                                                            \
		if (mask[0]) *((TYPE*)PACKED##DEPTH##_pixelAddr(x[0],FLIP(y[0]))) = PACK_COLOR_##FMT(rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);   \
		if (mask[1]) *((TYPE*)PACKED##DEPTH##_pixelAddr(x[1],FLIP(y[1]))) = PACK_COLOR_##FMT(rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]);   \
		if (mask[2]) *((TYPE*)PACKED##DEPTH##_pixelAddr(x[2],FLIP(y[2]))) = PACK_COLOR_##FMT(rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]);   \
		if (mask[3]) *((TYPE*)PACKED##DEPTH##_pixelAddr(x[3],FLIP(y[3]))) = PACK_COLOR_##FMT(rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]);   \
		rgba += 4; mask += 4; x+= 4; y += 4; n -= 4;                                               				\
		}                                                                                                       \
	while (n--) {                                                                                               \
		if (mask[0]) *((TYPE*)PACKED##DEPTH##_pixelAddr(x[0],FLIP(y[0]))) = PACK_COLOR_##FMT(rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);   \
		rgba++; mask++; x++; y++;                                                                        		\
		}                                                                                                       \
}                                                                                                               

#define IMPLEMENT_WRITE_PIXELS_DITHER(DEPTH,FMT,TYPE)							\
void _mmesa_write_pixels_##DEPTH##_##FMT(GLcontext *ctx,GLuint n, GLint x[],    \
	GLint y[],GLubyte rgba[][4],GLubyte mask[])                                 \
{                                                                               \
	while (n--) {                                                               \
		if (mask[0]) {                                                          \
			TYPE *d = PACKED##DEPTH##_pixelAddr(x[0],FLIP(y[0]));               \
			PACK_COLOR_##FMT(d[0],x[0],FLIP(y[0]),rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]); \
			}                                                                   \
		rgba++; mask++; x++; y++;                                        		\
		}                                                                       \
}                                                                               

#define	IMPLEMENT_WRITE_PIXELS_MONO(DEPTH,TYPE)												\
void _mmesa_write_pixels_mono_##DEPTH (GLcontext *ctx,GLuint n,GLint x[],                   \
	GLint y[],GLubyte mask[])                                                               \
{                                                                                           \
	while (n >= 4) {                                                                        \
		if (mask[0]) *((TYPE*)PACKED##DEPTH##_pixelAddr(x[0],FLIP(y[0]))) = (TYPE)RC.color; \
		if (mask[1]) *((TYPE*)PACKED##DEPTH##_pixelAddr(x[1],FLIP(y[1]))) = (TYPE)RC.color; \
		if (mask[2]) *((TYPE*)PACKED##DEPTH##_pixelAddr(x[2],FLIP(y[2]))) = (TYPE)RC.color; \
		if (mask[3]) *((TYPE*)PACKED##DEPTH##_pixelAddr(x[3],FLIP(y[3]))) = (TYPE)RC.color; \
		mask += 4; x+= 4; y += 4; n -= 4;                                                   \
		}                                                                                   \
	while (n--) {                                                                           \
		if (mask[0]) *((TYPE*)PACKED##DEPTH##_pixelAddr(x[0],FLIP(y[0]))) = (TYPE)RC.color; \
		mask++; x++; y++;                                                                   \
		}                                                                                   \
}

#define IMPLEMENT_WRITE_PIXELS_MONO_DITHER(DEPTH,FMT,TYPE)							\
void _mmesa_write_pixels_mono_##DEPTH##_##FMT(GLcontext *ctx,GLuint n,GLint x[],    \
	GLint y[],GLubyte mask[])                                                       \
{                                                                                   \
	while (n--) {                                                                   \
		if (mask[0]) {                                                              \
			TYPE *d = PACKED##DEPTH##_pixelAddr(x[0],FLIP(y[0]));                   \
			PACK_COLOR_##FMT(d[0],x[0],FLIP(y[0]),RC.red,RC.green,RC.blue);         \
			}                                                                       \
		mask++; x++; y++;                                                           \
		}                                                                           \
}

/**********************************************************************/
/*****                 Read spans of pixels                       *****/
/**********************************************************************/

#define	IMPLEMENT_READ_SPAN(DEPTH,FMT,TYPE)											\
void _mmesa_read_span_##DEPTH##_##FMT(GLcontext *ctx,GLuint n,GLint x,GLint y,		\
	GLubyte rgba[][4])																\
{                                                                                   \
	TYPE *d = PACKED##DEPTH##_pixelAddr(x,FLIP(y));                                 \
	while (n >= 8) {                                                                \
		UNPACK_COLOR_##FMT(d[0],rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);                           \
		UNPACK_COLOR_##FMT(d[1],rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]);                           \
		UNPACK_COLOR_##FMT(d[2],rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]);                           \
		UNPACK_COLOR_##FMT(d[3],rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]);                           \
		UNPACK_COLOR_##FMT(d[4],rgba[4][RCOMP],rgba[4][GCOMP],rgba[4][BCOMP]);                           \
		UNPACK_COLOR_##FMT(d[5],rgba[5][RCOMP],rgba[5][GCOMP],rgba[5][BCOMP]);                           \
		UNPACK_COLOR_##FMT(d[6],rgba[6][RCOMP],rgba[6][GCOMP],rgba[6][BCOMP]);                           \
		UNPACK_COLOR_##FMT(d[7],rgba[7][RCOMP],rgba[7][GCOMP],rgba[7][BCOMP]);                           \
		d += 8; rgba += 8; n -= 8;                            						\
		}                                                                           \
	while (n--) {                                                                   \
		UNPACK_COLOR_##FMT(d[0],rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]);      \
		d++; rgba++;                                                				\
		}                                                                           \
}

/**********************************************************************/
/*****                   Read arrays of pixels                    *****/
/**********************************************************************/

#define	IMPLEMENT_READ_PIXELS(DEPTH,FMT,TYPE)									\
void _mmesa_read_pixels_##DEPTH##_##FMT (GLcontext *ctx,GLuint n,GLint x[],		\
	GLint y[],GLubyte rgba[][4],GLubyte mask[])									\
{                                                                               \
	while (n >= 4) {                                                            \
		if (mask[0]) {                                                          \
			TYPE *d = PACKED##DEPTH##_pixelAddr(x[0],FLIP(y[0]));               \
			UNPACK_COLOR_##FMT(d[0],rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]); \
			}                                                                   \
		if (mask[1]) {                                                          \
			TYPE *d = PACKED##DEPTH##_pixelAddr(x[1],FLIP(y[1]));               \
			UNPACK_COLOR_##FMT(d[1],rgba[1][RCOMP],rgba[1][GCOMP],rgba[1][BCOMP]); \
			}                                                                   \
		if (mask[2]) {                                                          \
			TYPE *d = PACKED##DEPTH##_pixelAddr(x[2],FLIP(y[2]));               \
			UNPACK_COLOR_##FMT(d[2],rgba[2][RCOMP],rgba[2][GCOMP],rgba[2][BCOMP]); \
			}                                                                   \
		if (mask[3]) {                                                          \
			TYPE *d = PACKED##DEPTH##_pixelAddr(x[3],FLIP(y[3]));               \
			UNPACK_COLOR_##FMT(d[3],rgba[3][RCOMP],rgba[3][GCOMP],rgba[3][BCOMP]); \
			}                                                                   \
		rgba += 4; mask += 4; x += 4; y += 4; n -= 4;     						\
		}                                                                       \
	while (n--) {                                                               \
		if (mask[0]) {                                                          \
			TYPE *d = PACKED##DEPTH##_pixelAddr(x[0],FLIP(y[0]));               \
			UNPACK_COLOR_##FMT(d[0],rgba[0][RCOMP],rgba[0][GCOMP],rgba[0][BCOMP]); \
			}                                                                   \
		rgba++; mask++; x++; y++;                               				\
		}                                                                       \
}

