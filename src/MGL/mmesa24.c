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
* Description:	Optimized 24bpp rendering functions.
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

#include "mgl/mmesap.h"

/*------------------------- Implementation --------------------------------*/

#pragma warn -par

/**********************************************************************/
/*****            Write spans of pixels                           *****/
/**********************************************************************/

void _mmesa_write_span_24_RGB(GLcontext *ctx,GLuint n,GLint x,GLint y,
	GLubyte rgba[][4],GLubyte mask[])
{
	uchar *d = PACKED24_pixelAddr(x,FLIP(y));
	if (mask) {
		while (n--) {
			if (mask[0]) { d[0] = rgba[0][BCOMP]; d[1] = rgba[0][GCOMP]; d[2] = rgba[0][RCOMP]; }
			d += 3; rgba++; mask++;
			}
		}
	else {
		while (n--) {
			d[0] = rgba[0][BCOMP]; d[1] = rgba[0][GCOMP]; d[2] = rgba[0][RCOMP];
			d += 3; rgba++;
			}
		}
}

void _mmesa_write_span_24_BGR(GLcontext *ctx,GLuint n,GLint x,GLint y,
	GLubyte rgba[][4],GLubyte mask[])
{
	uchar *d = PACKED24_pixelAddr(x,FLIP(y));
	if (mask) {
		while (n--) {
			if (mask[0]) { d[0] = rgba[0][RCOMP]; d[1] = rgba[0][GCOMP]; d[2] = rgba[0][BCOMP]; }
			d += 3; rgba++; mask++;
			}
		}
	else {
		while (n--) {
			d[0] = rgba[0][RCOMP]; d[1] = rgba[0][GCOMP]; d[2] = rgba[0][BCOMP];
			d += 3; rgba++;
			}
		}
}

void _mmesa_write_span_rgb_24_RGB(GLcontext *ctx,GLuint n,GLint x,GLint y,
	GLubyte rgba[][3],GLubyte mask[])
{
	uchar *d = PACKED24_pixelAddr(x,FLIP(y));
	if (mask) {
		while (n--) {
			if (mask[0]) { d[0] = rgba[0][BCOMP]; d[1] = rgba[0][GCOMP]; d[2] = rgba[0][RCOMP]; }
			d += 3; rgba++; mask++;
			}
		}
	else {
		while (n--) {
			d[0] = rgba[0][BCOMP]; d[1] = rgba[0][GCOMP]; d[2] = rgba[0][RCOMP];
			d += 3; rgba++;
			}
		}
}

void _mmesa_write_span_rgb_24_BGR(GLcontext *ctx,GLuint n,GLint x,GLint y,
	GLubyte rgba[][3],GLubyte mask[])
{
	uchar *d = PACKED24_pixelAddr(x,FLIP(y));
	if (mask) {
		while (n--) {
			if (mask[0]) { d[0] = rgba[0][RCOMP]; d[1] = rgba[0][GCOMP]; d[2] = rgba[0][BCOMP]; }
			d += 3; rgba++; mask++;
			}
		}
	else {
		while (n--) {
			d[0] = rgba[0][RCOMP]; d[1] = rgba[0][GCOMP]; d[2] = rgba[0][BCOMP];
			d += 3; rgba++;
			}
		}
}

void _mmesa_write_span_mono_24_RGB(GLcontext *ctx,GLuint n,GLint x,GLint y,
	GLubyte mask[])
{
	uchar *d = PACKED24_pixelAddr(x,FLIP(y));
	while (n--) {
		if (mask[0]) { d[0] = RC.blue; d[1] = RC.green; d[2] = RC.red; }
		d += 3; mask++;
		}
}

void _mmesa_write_span_mono_24_BGR(GLcontext *ctx,GLuint n,GLint x,GLint y,
	GLubyte mask[])
{
	uchar *d = PACKED24_pixelAddr(x,FLIP(y));
	while (n--) {
		if (mask[0]) { d[0] = RC.red; d[1] = RC.green; d[2] = RC.blue; }
		d += 3; mask++;
		}
}

/**********************************************************************/
/*****              Write arrays of pixels                        *****/
/**********************************************************************/

void _mmesa_write_pixels_24_RGB(GLcontext *ctx,GLuint n,GLint x[],GLint y[],
	GLubyte rgba[][4],GLubyte mask[])
{
	while (n--) {
		if (mask[0]) {
			uchar *d = PACKED24_pixelAddr(x[0],FLIP(y[0]));
			d[0] = rgba[0][BCOMP]; d[1] = rgba[0][GCOMP]; d[2] = rgba[0][GCOMP];
			}
		rgba++; mask++; x++; y++;
		}
}

void _mmesa_write_pixels_24_BGR(GLcontext *ctx,GLuint n,GLint x[],GLint y[],
	GLubyte rgba[][4],GLubyte mask[])
{
	while (n--) {
		if (mask[0]) {
			uchar *d = PACKED24_pixelAddr(x[0],FLIP(y[0]));
			d[0] = rgba[0][GCOMP]; d[1] = rgba[0][GCOMP]; d[2] = rgba[0][BCOMP];
			}
		rgba++; mask++; x++; y++;
		}
}

void _mmesa_write_pixels_mono_24_RGB(GLcontext *ctx,GLuint n,GLint x[],
	GLint y[],GLubyte mask[])
{
	while (n--) {
		if (mask[0]) {
			uchar *d = PACKED24_pixelAddr(x[0],FLIP(y[0]));
			d[0] = RC.blue; d[1] = RC.green; d[2] = RC.red;
			}
		mask++; x++; y++;
		}
}

void _mmesa_write_pixels_mono_24_BGR(GLcontext *ctx,GLuint n,GLint x[],
	GLint y[],GLubyte mask[])
{
	while (n--) {
		if (mask[0]) {
			uchar *d = PACKED24_pixelAddr(x[0],FLIP(y[0]));
			d[0] = RC.red; d[1] = RC.green; d[2] = RC.blue;
			}
		mask++; x++; y++;
		}
}

/**********************************************************************/
/*****                 Read spans of pixels                       *****/
/**********************************************************************/

void _mmesa_read_span_24_RGB(GLcontext *ctx,GLuint n,GLint x,GLint y,
	GLubyte rgba[][4])
{
	uchar *d = PACKED24_pixelAddr(x,FLIP(y));
	while (n--) {
		rgba[0][BCOMP] = d[0]; rgba[0][GCOMP] = d[1]; rgba[0][RCOMP] = d[2];
		rgba++; d += 3;
		}
}

void _mmesa_read_span_24_BGR(GLcontext *ctx,GLuint n,GLint x,GLint y,
	GLubyte rgba[][4])
{
	uchar *d = PACKED24_pixelAddr(x,FLIP(y));
	while (n--) {
		rgba[0][RCOMP] = d[0]; rgba[0][GCOMP] = d[1]; rgba[0][BCOMP] = d[2];
		rgba++; d += 3;
		}
}

/**********************************************************************/
/*****                   Read arrays of pixels                    *****/
/**********************************************************************/

void _mmesa_read_pixels_24_RGB(GLcontext *ctx,GLuint n,GLint x[],GLint y[],
	GLubyte rgba[][4],GLubyte mask[])
{
	while (n--) {
		if (mask[0]) {
			uchar *d = PACKED24_pixelAddr(x[0],FLIP(y[0]));
			rgba[0][BCOMP] = d[0]; rgba[0][GCOMP] = d[1]; rgba[0][RCOMP] = d[2];
			}
		rgba++; mask++; x++; y++;
		}
}

void _mmesa_read_pixels_24_BGR(GLcontext *ctx,GLuint n,GLint x[],GLint y[],
	GLubyte rgba[][4],GLubyte mask[])
{
	while (n--) {
		if (mask[0]) {
			uchar *d = PACKED24_pixelAddr(x[0],FLIP(y[0]));
			rgba[0][RCOMP] = d[0]; rgba[0][GCOMP] = d[1]; rgba[0][BCOMP] = d[2];
			}
		rgba++; mask++; x++; y++;
		}
}

