#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glh.h"
#include "riva_glh.h"
//#include "nv_globals.h"
//#include "nv_3da.h"
#include "nv_std.h"


//#include "mesaglx/macros.h"
//#include "mesaglx/types.h"
//#include "mesaglx/lines.h"
//#include "mesaglx/points.h"
//#include "mesaglx/triangle.h"
#include "../context.h"
//#include "xsmesaP.h"
//#include "xsmdither.h"

/*
rudolf:

search for xsrud to find calls to these functions in the nvidia driver:
-------
in riva_ctx.c:
//xsmesa_setup_DD_pointers(ctx);

in riva_glh.c:
//xsmesa_setup_DD_pointers(ctx);
//xsmesa_color_to_pixel
//XSMesaDestroyBuffer

in riva_prim.c:
//xsmesa_color_to_pixel
*/

//from xsmesa2.c
//#define GLX_READ_PIXEL(x,y)     GLXProcs.GetPixel(img,x,y)
//#define GLX_WRITE_PIXEL(x,y,p)  GLXProcs.PutPixel(img,x,y,p)
#define GLX_READ_DEPTH(x,y)     RivaGetDepth(ctx,x,y)
#define GLX_WRITE_DEPTH(x,y,d)  RivaPutDepth(ctx,x,y,d)

//from xsmesa2.c
/**********************************************************************/
/*****                   Depth Testing Functions                  *****/
/**********************************************************************/

/*
 * Depth test horizontal spans of fragments.  These functions are called
 * via ctx->Driver.depth_test_span only.
 *
 * Input:  n - number of pixels in the span
 *         x, y - location of leftmost pixel in span in window coords
 *         z - array [n] of integer depth values
 * In/Out:  mask - array [n] of flags (1=draw pixel, 0=don't draw)
 * Return:  number of pixels which passed depth test
 */
#include "riva_symbols.h"
/*
 * glDepthFunc( any ) and glDepthMask( GL_TRUE or GL_FALSE ).
 */
GLuint depth_test_span( GLcontext* ctx, GLuint n, GLint x, GLint y,
                               const GLdepth z[], GLubyte mask[] )
{
//    XSMesaContext xsmesa = (XSMesaContext) ctx->DriverCtx;
    GLubyte *m = mask;
    GLuint i;
    GLuint passed = 0;

    /* Flip Y */
//rud: because (NOTE!)
// o,o is left-top for frontbuffer, but left-bottom for z-buffer in HW!!

    y = ctx->Buffer->Height - y - 1;

    /* switch cases ordered from most frequent to less frequent */
    switch (ctx->Depth.Func)
    {
        case GL_LESS:
//			LOG(2,("depth_test_span: Depth func is GL_LESS\n"));

            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0; i<n; i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] < GLX_READ_DEPTH(x,y))
                        {
                            /* pass */
                            GLX_WRITE_DEPTH(x,y,z[i]);
                            passed++;
                        }
                        else
                        {
                            /* fail */
                            *m = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0; i<n; i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] < GLX_READ_DEPTH(x,y))
                        {
                            /* pass */
                            passed++;
                        }
                        else
                        {
                            *m = 0;
                        }
                    }
                }
            }
            break;
        case GL_LEQUAL:
//			LOG(2,("depth_test_span: Depth func is GL_LEQUAL\n"));
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] <= GLX_READ_DEPTH(x,y))
                        {
                            GLX_WRITE_DEPTH(x,y,z[i]);
                            passed++;
                        }
                        else
                        {
                            *m = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] <= GLX_READ_DEPTH(x,y))
                        {
                            /* pass */
                            passed++;
                        }
                        else
                        {
                            *m = 0;
                        }
                    }
                }
            }
            break;
        case GL_GEQUAL:
//			LOG(2,("depth_test_span: Depth func is GL_GEQUAL\n"));
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] >= GLX_READ_DEPTH(x,y))
                        {
                            GLX_WRITE_DEPTH(x,y,z[i]);
                            passed++;
                        }
                        else
                        {
                            *m = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] >= GLX_READ_DEPTH(x,y))
                        {
                            /* pass */
                            passed++;
                        }
                        else
                        {
                            *m = 0;
                        }
                    }
                }
            }
            break;
        case GL_GREATER:
//			LOG(2,("depth_test_span: Depth func is GL_GREATER\n"));
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] > GLX_READ_DEPTH(x,y))
                        {
                            GLX_WRITE_DEPTH(x,y,z[i]);
                            passed++;
                        }
                        else
                        {
                            *m = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] > GLX_READ_DEPTH(x,y))
                        {
                            /* pass */
                            passed++;
                        }
                        else
                        {
                            *m = 0;
                        }
                    }
                }
            }
            break;
        case GL_NOTEQUAL:
//			LOG(2,("depth_test_span: Depth func is GL_NOTEQUAL\n"));
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] != GLX_READ_DEPTH(x,y))
                        {
                            GLX_WRITE_DEPTH(x,y,z[i]);
                            passed++;
                        }
                        else
                        {
                            *m = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] != GLX_READ_DEPTH(x,y))
                        {
                            /* pass */
                            passed++;
                        }
                        else
                        {
                            *m = 0;
                        }
                    }
                }
            }
            break;
        case GL_EQUAL:
//			LOG(2,("depth_test_span: Depth func is GL_EQUAL\n"));
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] == GLX_READ_DEPTH(x,y))
                        {
                            GLX_WRITE_DEPTH(x,y,z[i]);
                            passed++;
                        }
                        else
                        {
                            *m =0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        if (z[i] == GLX_READ_DEPTH(x,y))
                        {
                            /* pass */
                            passed++;
                        }
                        else
                        {
                            *m =0;
                        }
                    }
                }
            }
            break;
        case GL_ALWAYS:
//			LOG(2,("depth_test_span: Depth func is GL_ALWAYS\n"));
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0;i<n;i++,x++,m++)
                {
                    if (*m)
                    {
                        GLX_WRITE_DEPTH(x,y,z[i]);
                        passed++;
                    }
                }
            }
            else
            {
                /* Don't update Z buffer or mask */
                passed = n;
            }
            break;
        case GL_NEVER:
//			LOG(2,("depth_test_span: Depth func is GL_NEVER\n"));
            for (i=0;i<n;i++)
            {
                mask[i] = 0;
            }
            break;
        default:
			LOG(2,("depth_test_span: problem: bad depth func\n"));
    } /*switch*/

    return (passed);
}

//still from xsmesa2.c
/*
 * glDepthFunc( any ) and glDepthMask( GL_TRUE or GL_FALSE ).
 */
void depth_test_pixels( GLcontext* ctx, GLuint n, const GLint x[], const GLint y[],
                               const GLdepth z[], GLubyte mask[] )
{
//    XSMesaContext xsmesa = (XSMesaContext) ctx->DriverCtx;
    GLuint i;

//rud: because (NOTE!)
// o,o is left-top for frontbuffer, but left-bottom for z-buffer in HW!!

    /* switch cases ordered from most frequent to less frequent */
    switch (ctx->Depth.Func)
    {
        case GL_LESS:
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
//rud: Y is flipped, because (NOTE!)
// o,o is left-top for frontbuffer, but left-bottom for z-buffer in HW!!
                        if (z[i] < GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                            GLX_WRITE_DEPTH(x[i],ctx->Buffer->Height - y[i]-1,z[i]);
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] < GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            break;
        case GL_LEQUAL:
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] <= GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                            GLX_WRITE_DEPTH(x[i],ctx->Buffer->Height - y[i]-1,z[i]);
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] <= GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            break;
        case GL_GEQUAL:
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] >= GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                            GLX_WRITE_DEPTH(x[i],ctx->Buffer->Height - y[i]-1,z[i]);
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] >= GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            break;
        case GL_GREATER:
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] > GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                            GLX_WRITE_DEPTH(x[i],ctx->Buffer->Height - y[i]-1,z[i]);
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] > GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            break;
        case GL_NOTEQUAL:
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] != GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                            GLX_WRITE_DEPTH(x[i],ctx->Buffer->Height - y[i]-1,z[i]);
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] != GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            break;
        case GL_EQUAL:
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] == GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                            GLX_WRITE_DEPTH(x[i],ctx->Buffer->Height - y[i]-1,z[i]);
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            else
            {
                /* Don't update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        if (z[i] == GLX_READ_DEPTH(x[i],ctx->Buffer->Height - y[i]-1))
                        {
                            /* pass */
                        }
                        else
                        {
                            /* fail */
                            mask[i] = 0;
                        }
                    }
                }
            }
            break;
        case GL_ALWAYS:
            if (ctx->Depth.Mask)
            {
                /* Update Z buffer */
                for (i=0; i<n; i++)
                {
                    if (mask[i])
                    {
                        GLX_WRITE_DEPTH(x[i],ctx->Buffer->Height - y[i]-1,z[i]);
                    }
                }
            }
            else
            {
                /* Don't update Z buffer or mask */
            }
            break;
        case GL_NEVER:
            /* depth test never passes */
            for (i=0;i<n;i++)
            {
                mask[i] = 0;
            }
            break;
        default:
            gl_problem(ctx, "Bad depth func in gl_depth_test_pixels_generic");
    } /*switch*/
}
