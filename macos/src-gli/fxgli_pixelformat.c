/*
 * Mesa 3-D graphics library MacOS Driver
 * Version:  3.2
 *
 * Copyright (C) 1999  Mikl—s Fazekas   All Rights Reserved.
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

#include "fxgli.h"
#include "gliutils.h"


/***********************************************************************************************
 *
 * PixelFormat to Attrib list: 
 *  Converts the GLIPixelFormat to FX-Attribute list. The attrib param should be at least 
 *  32-length!
 *
 ***********************************************************************************************/
void fxgli_PixelFormatToFXAttribList(const GLIPixelFormat *fmt, GLint attrib[])
{
	int current = 0; 
	
	if (fmt->buffer_mode & GLI_DOUBLEBUFFER_BIT)
		attrib[current++] = FXMESA_DOUBLEBUFFER;
	
	if (fmt->depth_mode == GLI_16_BIT)
	{
		attrib[current++] = FXMESA_DEPTH_SIZE;
		attrib[current++] = 16;
	}
	
	if (fmt->stencil_mode == GLI_8_BIT)
	{
		attrib[current++] = FXMESA_STENCIL_SIZE;
		attrib[current++] = 8;
	}
	
	if (fmt->color_mode == GLI_RGB565_A8_BIT)
	{
		attrib[current++] = FXMESA_ALPHA_SIZE;
		attrib[current++] = 8;
	}
	
	/* ToDo Accum*/
	attrib[current++] = FXMESA_NONE;
}


/***********************************************************************************************
 *
 * gliChoosePixelFormat 
 *
 * Description: Chooses a pixel format from the attrib list.
 *
 ***********************************************************************************************/
/*
** Constant's used for generating all possible pixel-format configurations.
*/
#define FORMAT_COUNT 				4
#define DOUBLE_BUFFER_BIT			0x0001
#define DEPTH_BUFFER_BIT			0x0002

GLenum gliChoosePixelFormat(
	GLIPixelFormat **fmt_ret,
	const GLIDevice *device,
	GLint ndevs,
	const GLint *attribs)
{
    GLIPixelFormat *format;
    int i;
    int data;
    int format_num;
   	GLIPixelFormat 	best;
   	GLIPixelFormat  current;
   	int				best_score = -1; 
    data = attribs[0];
    if (!fmt_ret)
    	return (GLenum)GLI_BAD_POINTER;
    /* Initialize *fmt_ret */
    *fmt_ret = NULL;

    /* Allocate pixel format */
    format = (GLIPixelFormat *) malloc(1 * sizeof(GLIPixelFormat) + sizeof(GLIDevice)*_gliUtilsGetGDevicesNum());
    if(!format) return (GLenum)GLI_BAD_ALLOC;

	format_num = 0;
	/*
	 * Set devices:
	 */
	current.num_devices = _gliUtilsGetGDevicesNum();
	current.devices = (GLIDevice *) (format + 1);
	{
		GDHandle dev;
		i = 0;
		dev =_gliUtilsGetFirstGDevice();
		while (dev != NULL)
		{
			current.devices[i].gdevice = dev;
			i++;
			dev = _gliUtilsGetNextGDevice(dev);
		}
	}
	
    for(i = 0; i < FORMAT_COUNT; i++)
    {
    	GLint 			current_score = -1;
	    
		/* Pointer to next format */
		current.next_pixel_format = NULL;

		/* Set the renderer id */
		current.renderer_id       = GLI_RENDERER_MESA_3DFX_ID;
		
		/* OS support feature bits */
		current.os_support        =  GLI_ACCELERATED_BIT | GLI_NEEDED_BIT;
		if (fxgli_IsEmulateWindowRendering())
			current.os_support	|= GLI_WINDOW_BIT;
		
		current.os_support 	|= GLI_FULLSCREEN_BIT;
		
		
		if (fxgli_IsEmulateCompliance())
			current.os_support 	|= GLI_COMPLIANT_BIT;
		
		/* Double buffer */
		if (i & DOUBLE_BUFFER_BIT) 
			current.buffer_mode 	= /* GLI_MONOSCOPIC_BIT | */GLI_DOUBLEBUFFER_BIT;
		else
			current.buffer_mode 	= 0/* GLI_MONOSCOPIC_BIT | GLI_SINGLEBUFFER_BIT*/;
			
		/* Buffer size info */
		current.color_mode 			= 0x0008000; /*GLI_RGB565_BIT; */
		

		current.accum_mode       	= 0;
		
		if (i & DEPTH_BUFFER_BIT)
		    current.depth_mode		= GLI_16_BIT;
		else
		    current.depth_mode		= GLI_0_BIT; 

		current.stencil_mode     	= GLI_0_BIT;
		current.level 				= 0;
		current.aux_buffers 		= 0;
		
		current_score 				= _gliUtilsScorePixelFormat(&current,device,ndevs,attribs);
		
		if (current_score > best_score)
		{
			best = current;
			best_score = current_score; 
		}
     }
	
	if (best_score >= 0)
	{
		format[0] = best;
		format[0].next_pixel_format = NULL;

     	*fmt_ret = format;
	}
	else
	{
		free(format);
		*fmt_ret = NULL;
	}
     return GLI_NO_ERROR;
}
/***********************************************************************************************
 *
 * gliDestroyPixelFormat 
 *
 * Description: Deletas a previously allocated pixel format.
 *
 ***********************************************************************************************/

GLenum gliDestroyPixelFormat(GLIPixelFormat *fmt)
{
    /*
    ** Initial error checking
    */
  	if (!fmt)
		return GLI_BAD_POINTER;
	if (fmt->renderer_id != GLI_RENDERER_MESA_3DFX_ID)
		return GLI_BAD_PIXELFMT;
	
    /*
    ** Main part
    */ 
   	free((char *) fmt);
 	return GLI_NO_ERROR;
}