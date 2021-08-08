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


#include "gliDispatch.h"
#include "fxgli.h"

/* Mesa */
#include "context.h"

/* ANSI */
#include "stdio.h"



#if MESA_3DFX_STANDALONE_GLI_PLUGIN

#define API_PREFIX static

#define API_NAME(name)	name

/* This will be an unused parameter */
#define CTX_ARG		GLIContext param_ctx,
#define CTX_VOID	GLIContext param_ctx
#define CTX_PRM		param_ctx,
#define CTX_VPRM	param_ctx

#include "dispproto.h"
#include "gl_mangle.h"

#if MESA_3DFX_OPT_TEXSUBIMAGE
#undef glTexSubImage2D
#define glTexSubImage2D __mglTexSubImage2D
#endif

#include "api1.c"
#include "api2.c"
#ifdef TEXCOORD1
#undef TEXCOORD1
#undef TEXCOORD2
#undef TEXCOORD3
#undef TEXCOORD4
#endif
#include "apiext.c"

#include "cva.c"
#include "varray.c"


#if MESA_3DFX_OPT_TEXSUBIMAGE
extern GLboolean fastFxDDTexSubImage2D(
				GLcontext *ctx,
				GLenum target, GLint level,
        		GLint xoffset, GLint yoffset,
        		GLsizei width, GLsizei height,
        		GLenum format, GLenum type,
       		   const GLvoid *pixels );
       		   
void mglTexSubImage2D(CTX_ARG GLenum target, GLint level,
                                       GLint xoffset, GLint yoffset,
                                       GLsizei width, GLsizei height,
                                       GLenum format, GLenum type,
                                       const GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   if (fastFxDDTexSubImage2D(CC,target,level,xoffset,yoffset,width,height,format,type,pixels))
      return;
   __mglTexSubImage2D(CTX_PRM target,level,xoffset,yoffset,width,height,format,type,pixels);
}                              
#endif

static void unimplemented()
{
	DebugStr("\pAlways call glGetExtensions!");
}

#include "mesa_2_gli.h"

#else	/* !MESA_3DFX_STANDALONE_GLI_PLUGIN */
	static void FillDispatchTable(GLIFunctionDispatch *table)
	{
		(void)table;
	}
	static void FillExtDispatchTable(GLIExtensionDispatch *table)
	{ 
		(void)table;
	}
#endif

GLenum gliGetSystemDispatch(GLIContext ctx, GLIFunctionDispatch *table, GLbitfield change_flags)
{
   printf("gliGetSystemDispatch: %p %p %i\n",ctx,table,change_flags);
   DebugStr("\p gliGetSystemDispatch was called!!!");
   return (GLenum)GLI_NO_ERROR;
} 
GLenum gliGetFunctionDispatch(GLenum mode,GLIFunctionDispatch *table)
{
   (void)mode;
   FillDispatchTable(table);
   return (GLenum)GLI_NO_ERROR;
}
GLenum gliGetExtensionDispatch(GLenum mode,GLIExtensionDispatch *table)
{
   (void)mode;
   FillExtDispatchTable(table);
   return (GLenum)GLI_NO_ERROR;
}