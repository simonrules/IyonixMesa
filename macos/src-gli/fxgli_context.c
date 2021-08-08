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
/* ANSI */
#include <stdlib.h>
#include <stdio.h>

#include <console.h>

#if MESA_3DFX_PROFILE
	#include "profiler.h"
#endif


/***********************************************************************************************
 *
 * current_context global holds, the current context.
 *
 ***********************************************************************************************/

TFXContext *current_context;

/***********************************************************************************************
 *
 * gliCreateContext:
 *
 ***********************************************************************************************/
/***********************************************************************************************
 * Notes:
 *	The FX driver (see fxapi.h) doesn't have function like this.
 *  The current soultion to this problem is that we are creating a "fake" context here, and
 *  we create the real context for gliAttachDrawable.
 *  The limitaion of this code:
 *    - One can't share context's. (This isn't a real limitation, since 3DFX usually will handle
 *	  								only one context at a time.)
 *	The soultion:
 *	  - Split the fxApi so that it will have similiar functions.
 *
 ***********************************************************************************************/
#ifdef MESA_DEBUG
int MESA_VERBOSE = 0;
int MESA_DEBUG_FLAGS = 0;
#endif
GLenum gliCreateContext(GLIContext *ctx_ret, const GLIPixelFormat *fmt, GLIContext share_list)
{
	char**argv;
	TFXContext *result;
	
	
#ifdef MESA_DEBUG
/*	ccommand(&argv);
	MESA_VERBOSE = VERBOSE_DRIVER;
	MESA_DEBUG_FLAGS = 0;*/
#endif
	/*
	** Initial Error Checking
	*/
   	if (fmt->renderer_id != GLI_RENDERER_MESA_3DFX_ID)
   		return (GLenum)GLI_BAD_PIXELFMT;
   	if (ctx_ret == NULL)
   		return (GLenum)GLI_BAD_POINTER;
	if (share_list != NULL)
		return (GLenum)GLI_BAD_CONTEXT;
   	/*
   	** Main
   	*/
	result = (TFXContext*)malloc(sizeof(TFXContext));
	if (result == NULL)
	    return (GLenum)GLI_BAD_ALLOC;
	    
	result->renderer_id = GLI_RENDERER_MESA_3DFX_ID;
	result->drawable.type = GLI_NONE;
	result->ctx = NULL;
	result->gl_ctx = NULL;
	result->fmt = (*fmt);
	
	
	setDefaultIntegerValues(result);
    
   	*ctx_ret = (GLIContext)result;
		return (GLenum)GLI_NO_ERROR;
}

/***********************************************************************************************
 *
 * gliDestroyContext: Destroys the context created by gliCreateContext.
 *
 ***********************************************************************************************/
GLenum gliDestroyContext(GLIContext ctx)
{
	TFXContext *context = (TFXContext*)ctx;
	/*
	** Initial error checking
	*/

	if(context == NULL)
   		return (GLenum)GLI_BAD_CONTEXT;
   	
   
   	if (context->renderer_id != GLI_RENDERER_MESA_3DFX_ID) 
   		return (GLenum)GLI_BAD_CONTEXT;
   	/*
   	** Main
   	*/
   	if (context->ctx != NULL)
   		fxMesaDestroyContext(context->ctx);
   	
   	if (context == current_context)
   		current_context = NULL;
   		
   	free(context);   
   	
   	return (GLenum)GLI_NO_ERROR;  
}
