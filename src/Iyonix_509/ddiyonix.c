/* File:       ddiyonix.c
   Written by: Simon Wilson
 */

#include <stdlib.h>
#include <stdio.h>
#include "GL/iyonixmesa.h"
#include "context.h"
#include "matrix.h"
#include "types.h"
#include "vb.h"

/* nVidia 3D add-on */
#include "nv_std.h"
#include "riva_glh.h"
#include "riva_ctx.h"
#include "riva_symbols.h"

/* RISC OS */
#include <kernel.h>
#include <swis.h>

#define PCI_HardwareAddress   0x0005038a
#define PCI_ConfigurationRead 0x00050388
#define PCI_FindByID          0x0005038e

struct iyonix_mesa_context {
	GLcontext *gl_ctx;			/* The core GL/Mesa context */
	GLvisual *gl_visual;			/* Describes the buffers */
	GLframebuffer *gl_buffer;	/* Depth, stencil, accum, etc buffers */

	GLuint ClearIndex;
	GLubyte ClearColor[4];
	GLuint CurrentIndex;
	GLubyte CurrentColor[4];

	GLint width, height;			/* size of color buffer */
};

static IyonixMesaContext ctx = NULL;

static void setup_DD_pointers( GLcontext *ctx );


/**********************************************************************/
/*****              Miscellaneous device driver funcs             *****/
/**********************************************************************/


static void finish( GLcontext *ctx )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;
   /* OPTIONAL FUNCTION: implements glFinish if possible */
}



static void flush( GLcontext *ctx )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;
   /* OPTIONAL FUNCTION: implements glFlush if possible */
}



static void clear_index( GLcontext *ctx, GLuint index )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;

   iyo->ClearIndex = index;
}



static void clear_color( GLcontext *ctx, GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;
   iyo->ClearColor[0] = r;
   iyo->ClearColor[1] = g;
   iyo->ClearColor[2] = b;
   iyo->ClearColor[3] = a;
}



static GLbitfield clear( GLcontext *ctx, GLbitfield mask, GLboolean all,
                        GLint x, GLint y, GLint width, GLint height )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;
/*
 * Clear the specified region of the buffers indicated by 'mask'
 * using the clear color or index as specified by one of the two
 * functions above.
 * If all==GL_TRUE, clear whole buffer, else just clear region defined
 * by x,y,width,height
 */

   mask &= RivaClearBuffers05(ctx, mask, all, x, y, width, height);

   return mask;  /* return mask of buffers remaining to be cleared */
}


static const GLubyte *get_string( GLcontext *ctx, GLenum name )
{
   switch (name)
   {
   case GL_RENDERER:
      return (const GLubyte *) "Mesa 3.2 Iyonix with NVidia 3D accelerant (alpha 1 final)";
      break;
   default:
      /* Let core library handle all other cases */
      return NULL;
      break;
   }
}



static void set_index( GLcontext *ctx, GLuint index )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;

   iyo->CurrentIndex = index;
}



static void set_color( GLcontext *ctx, GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;
   iyo->CurrentColor[0] = r;
   iyo->CurrentColor[1] = g;
   iyo->CurrentColor[2] = b;
   iyo->CurrentColor[3] = a;
}



/*
 * OPTIONAL FUNCTION: implement glIndexMask if possible, else
 * return GL_FALSE
 */
static GLboolean index_mask( GLcontext *ctx, GLuint mask )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;

   return GL_FALSE;
}



/*
 * OPTIONAL FUNCTION: implement glColorMask if possible, else
 * return GL_FALSE
 */
static GLboolean color_mask( GLcontext *ctx,
                             GLboolean rmask, GLboolean gmask,
                             GLboolean bmask, GLboolean amask)
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;

   return GL_FALSE;
}



/*
 * OPTIONAL FUNCTION:
 * Implements glLogicOp if possible.  Return GL_TRUE if the device driver
 * can perform the operation, otherwise return GL_FALSE.  If GL_FALSE
 * is returned, the logic op will be done in software by Mesa.
 */
static GLboolean logicop( GLcontext *ctx, GLenum op )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;

   return GL_FALSE;
}



/*
 * OPTIONAL FUNCTION: enable/disable dithering if applicable
 */
static void dither( GLcontext *ctx, GLboolean enable )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;
}



/*
 * Set the current drawing/reading buffer, return GL_TRUE or GL_FALSE
 * for success/failure.
 */
static GLboolean set_buffer( GLcontext *ctx, GLenum mode )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;

   setup_DD_pointers( ctx );

   if((mode == GL_FRONT_LEFT) || (mode == GL_BACK_LEFT))
      return GL_TRUE;
   else
      return GL_FALSE;
}



/*
 * Return the width and height of the current buffer.
 * If anything special has to been done when the buffer/window is
 * resized, do it now.
 */
static void get_buffer_size( GLcontext *ctx, GLuint *width, GLuint *height )
{
   struct iyonix_mesa_context *iyo = (struct iyonix_mesa_context *) ctx->DriverCtx;

   *width  = iyo->width; /* SLW-TODO: check this is OK */
   *height = iyo->height;
}



/**********************************************************************/
/*****            Dummy functions                                 *****/
/**********************************************************************/

static void WriteCIPixel( GLint x, GLint y, GLuint index )
{
}

static void WriteRGBAPixel( GLint x, GLint y, const GLubyte color[4] )
{
}

static void WriteRGBPixel( GLint x, GLint y, const GLubyte color[3] )
{
}

static GLuint ReadCIPixel( GLint x, GLint y )
{
   return 0;
}

static void ReadRGBAPixel( GLint x, GLint y, GLubyte color[4] )
{
}

#define FLIP(y)  iyo->Buffer->Height - (y) - 1;


/**********************************************************************/
/*****           Accelerated point, line, triangle rendering      *****/
/**********************************************************************/

/* All in Riva driver */

/**********************************************************************/
/*****            Write spans of pixels                           *****/
/**********************************************************************/

/* No code in here yet */

static void write_index8_span( const GLcontext *ctx,
                               GLuint n, GLint x, GLint y,
                               const GLubyte index[],
                               const GLubyte mask[] )
{

}


static void write_index32_span( const GLcontext *ctx,
                                GLuint n, GLint x, GLint y,
                                const GLuint index[],
                                const GLubyte mask[] )
{

}



static void write_mono_index_span( const GLcontext *ctx,
                                   GLuint n, GLint x, GLint y,
                                   const GLubyte mask[] )
{

}



static void write_rgba_span( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                             const GLubyte rgba[][4], const GLubyte mask[] )
{

}


static void write_rgb_span( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                            const GLubyte rgb[][3], const GLubyte mask[] )
{

}



static void write_mono_rgba_span( const GLcontext *ctx,
                                  GLuint n, GLint x, GLint y,
                                  const GLubyte mask[])
{

}



/**********************************************************************/
/*****                 Read spans of pixels                       *****/
/**********************************************************************/


static void read_index_span( const GLcontext *ctx,
                             GLuint n, GLint x, GLint y, GLuint index[])
{

}



static void read_rgba_span( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                            GLubyte rgba[][4] )
{

}



/**********************************************************************/
/*****              Write arrays of pixels                        *****/
/**********************************************************************/


static void write_index_pixels( const GLcontext *ctx,
                                GLuint n, const GLint x[], const GLint y[],
                                const GLuint index[], const GLubyte mask[] )
{

}



static void write_mono_index_pixels( const GLcontext *ctx,
                                     GLuint n,
                                     const GLint x[], const GLint y[],
                                     const GLubyte mask[] )
{

}



static void write_rgba_pixels( const GLcontext *ctx,
                               GLuint n, const GLint x[], const GLint y[],
                               const GLubyte rgba[][4], const GLubyte mask[] )
{

}



static void write_mono_rgba_pixels( const GLcontext *ctx,
                                    GLuint n, const GLint x[], const GLint y[],
                                    const GLubyte mask[] )
{

}




/**********************************************************************/
/*****                   Read arrays of pixels                    *****/
/**********************************************************************/

/* Read an array of color index pixels. */
static void read_index_pixels( const GLcontext *ctx,
                               GLuint n, const GLint x[], const GLint y[],
                               GLuint index[], const GLubyte mask[] )
{

}



static void read_rgba_pixels( const GLcontext *ctx,
                              GLuint n, const GLint x[], const GLint y[],
                              GLubyte rgba[][4], const GLubyte mask[] )
{

}




/**********************************************************************/
/**********************************************************************/


static void setup_DD_pointers( GLcontext *ctx )
{
   /* Initialize all the pointers in the DD struct.  Do this whenever */
   /* a new context is made current or we change buffers via set_buffer! */

   ctx->Driver.UpdateState = setup_DD_pointers;
   ctx->Driver.SetBuffer = set_buffer;
   ctx->Driver.Color = set_color;
   ctx->Driver.Index = set_index;
   ctx->Driver.ClearIndex = clear_index;
   ctx->Driver.ClearColor = clear_color;
   ctx->Driver.GetBufferSize = get_buffer_size;
   ctx->Driver.GetString = get_string;

   ctx->Driver.Clear = clear;

   /* Pixel/span writing functions: */
   ctx->Driver.WriteRGBASpan       = write_rgba_span;
   ctx->Driver.WriteRGBSpan        = write_rgb_span;
   ctx->Driver.WriteRGBAPixels     = write_rgba_pixels;
   ctx->Driver.WriteMonoRGBASpan   = write_mono_rgba_span;
   ctx->Driver.WriteMonoRGBAPixels = write_mono_rgba_pixels;

   ctx->Driver.WriteCI32Span       = write_index32_span;
   ctx->Driver.WriteCI8Span        = write_index8_span;
   ctx->Driver.WriteMonoCISpan     = write_mono_index_span;
   ctx->Driver.WriteCI32Pixels     = write_index_pixels;
   ctx->Driver.WriteMonoCIPixels   = write_mono_index_pixels;

   /* Pixel/span reading functions: */
   ctx->Driver.ReadRGBASpan        = read_rgba_span;
   ctx->Driver.ReadRGBAPixels      = read_rgba_pixels;
   ctx->Driver.ReadCI32Span        = read_index_span;
   ctx->Driver.ReadCI32Pixels      = read_index_pixels;

   RivaUpdateState05(ctx);
}



/**********************************************************************/
/*****               Iyonix/Mesa Public API Functions                *****/
/**********************************************************************/

void IyonixMesaInitialise()
{
	GLboolean rgb_flag = GL_TRUE;
	GLboolean alpha_flag = GL_FALSE;
	GLboolean double_flag = GL_FALSE;
	GLfloat red_max = 255.0f, green_max = 255.0f, blue_max = 255.0f, alpha_max = 255.0f;
	GLint red_bits = 8, green_bits = 8, blue_bits = 8, alpha_bits = 8;
	GLint depth_bits = 16, stencil_bits = 0, accum_bits = 0, index_bits = 0;

	_kernel_swi_regs r;
	//int handle;
	void *dummy;
	int *mode_block;
	int screen_width, screen_height;

	ctx = NULL;

	ctx = (IyonixMesaContext) calloc(1, sizeof(struct iyonix_mesa_context));
	if (!ctx)
		return;

	ctx->gl_visual = gl_create_visual(rgb_flag, alpha_flag,
		GL_TRUE, GL_FALSE,
		depth_bits, stencil_bits, accum_bits, index_bits,
		red_bits, green_bits, blue_bits, alpha_bits);

	ctx->gl_ctx = gl_create_context(ctx->gl_visual, NULL, ctx, GL_TRUE);

	/*_mesa_initialize_context(ctx->gl_ctx);*/

	ctx->gl_buffer = gl_create_framebuffer(ctx->gl_visual);

	/* RISC OS initialisation */
	si = (shared_info *)malloc(sizeof(shared_info));

	/* Get handle of graphics card */
	r.r[0] = 0x10de; // Vendor ID "nVidia"
	r.r[1] = -1;
	r.r[2] = -1;
	r.r[3] = 0;
	r.r[4] = -1;
	_kernel_swi(PCI_FindByID, &r, &r);
	handle = r.r[3];

	/* Get register base */
	r.r[0] = 0x200;
	r.r[1] = 0;
	r.r[3] = handle;
	_kernel_swi(PCI_HardwareAddress, &r, &r);
	regs = (vuint32 *)r.r[4];

	/* Get framebuffer base */
	r.r[0] = 0x200;
	r.r[1] = 1;
	r.r[3] = handle;
	_kernel_swi(PCI_HardwareAddress, &r, &r);
	si->framebuffer = (void *)r.r[4];
	si->framebuffer_pci = (void *)r.r[1];

	/* Determine card type */
	nv_general_powerup();

	/* Get screen dimensions */
        r.r[0] = 1;
        _kernel_swi(OS_ScreenMode, &r, &r);
        mode_block = (int *)r.r[1];

        if(mode_block[3] != 5)
        {
           printf("Only 32bpp modes supported at the moment, sorry\n");

           /* SLW-TODO: Probably should clean up here ;) */

           exit(1);
        }

        screen_width = mode_block[1];
        screen_height = mode_block[2];

	si->fbc.frame_buffer = si->framebuffer;
	si->fbc.bytes_per_row = screen_width * 4;

	si->dm.space = B_RGB32_LITTLE;
	si->dm.virtual_width = screen_width;
	si->dm.virtual_height = screen_height;
	si->mem_low = screen_width * screen_height * 4;
	si->mem_high = 32 << 20; // SLW-TODO: fix me!
	si->ps.memory_size = 32 << 20;

	FrontBuffer.left = 0;
	FrontBuffer.top = 0;
	FrontBuffer.right = screen_width - 1;
	FrontBuffer.bottom = screen_height - 1;

	/* Initialise the acceleration engine, 2D and 3D */
	nv_acc_init();

	/* Setup graphics card memory */
	nvHookServerSymbols(dummy);

	/* Setup device driver pointers */
	setup_DD_pointers(ctx->gl_ctx);

	gl_make_current(ctx->gl_ctx, ctx->gl_buffer);

	ctx->width = screen_width;
	ctx->height = screen_height;

	gl_Viewport(ctx->gl_ctx, 0, 0, ctx->width, ctx->height);
}

void IyonixMesaCloseDown()
{
	if(ctx)
	{
		gl_destroy_visual(ctx->gl_visual);
		if (ctx->gl_buffer->Depth)
		{
			ctx->gl_buffer->Depth = NULL;
		}
		gl_destroy_framebuffer(ctx->gl_buffer);
		gl_destroy_context(ctx->gl_ctx);
		free(ctx);
	}

	free(si);
}

void IyonixMesaSwapBuffers()
{
	RivaSwapBuffersIyonix();
}
