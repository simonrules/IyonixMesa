/*
	This file contains stuff from utahGLX that's not utahGLX specific.
	(GLX stands for 'GL extensions to X-windows', linux: this deals with interfacing
	 the drivers to the windowmanager so you can actually see the output from your
	 3D applications.)

	This stuff will transform into 'GLH' (GL extensions to Haiku) or something.
	It contains general stuff which all drivers (could) need. For more info also
	look at the MESA example driverfile ddsample.c.

	Rudolf Cornelissen 3/2005
*/

#ifndef _GLH_H
#define _GLH_H

//#include <OS.h>
#include "../src/context.h"

//rudolf: find out why this doesn't exist by default here:
#define FALSE 0
#define TRUE 1

//what's a XIMAGE?????? (don't really want to know..)
//#define XIMAGE None

#define min(X, Y) ((X) < (Y) ? (X) : (Y))
#define max(X, Y) ((X) < (Y) ? (Y) : (X))

/*
//from xsmesaP.h of utahGLX:
// XImage buffer.
struct glx_image
{
//    WindowPtr pwin;
    int       width, height;
    int       bytes_per_line;
    int       bits_per_pixel;
    char     *data;
    void     *devPriv;
};

//from xsmesa.h of utahGLX:
typedef struct glx_image GLXImage;
*/
/*
// Mesa wrapper for XVisualInfo
//struct xsmesa_visual
{
    GLvisual *gl_visual;        // the core visual
//    ScreenPtr pScreen;      // the X11 screen
//    VisualPtr pVisual;      // the X11 visual

    GLint level;            // 0=normal, 1=overlay, etc

    GLboolean ximage_flag;  // use XImage for back buffer (not pixmap)?

    GLuint dithered_pf; // Pixel format when dithering
    GLuint undithered_pf;   // Pixel format when not dithering

    GLfloat RedGamma;   // Gamma values, 1.0 is default
    GLfloat GreenGamma;
    GLfloat BlueGamma;

    GLint rmult, gmult, bmult;  // Range of color values
    GLint index_bits;       // Bits per pixel in CI mode

    // For PF_TRUECOLOR
    GLint rshift, gshift, bshift;   // Pixel color component shifts
    GLubyte Kernel[16];     // Dither kernel
    unsigned long RtoPixel[512];    // RGB to pixel conversion
    unsigned long GtoPixel[512];
    unsigned long BtoPixel[512];
    GLubyte PixelToR[256];      // Pixel to RGB conversion
    GLubyte PixelToG[256];
    GLubyte PixelToB[256];

    // For PF_1BIT
    int bitFlip;
};

//typedef struct xsmesa_visual  *XSMesaVisual;
*/
/*
typedef struct _Drawable {
    unsigned char	type;	// DRAWABLE_<type>
//    unsigned char	class;	// specific to type
    unsigned char	depth;
    unsigned char	bitsPerPixel;
    unsigned long	id;	// resource id
    short		x;	// window: screen absolute, pixmap: 0
    short		y;	// window: screen absolute, pixmap: 0
    unsigned short	width;//rudolf: screen width???!!!
    unsigned short	height;//rudolf: screen height???!!!
//    ScreenPtr		pScreen;
    unsigned long	serialNumber;
} DrawableRec;

typedef struct _Drawable *DrawablePtr;

// PIXMAP -- device dependent
typedef struct _Pixmap {
    DrawableRec		drawable;
    int			refcnt;
    int			devKind;
//    DevUnion		devPrivate;
#ifdef PIXPRIV
    DevUnion		*devPrivates; // real devPrivates like gcs & windows
#endif
} PixmapRec;

typedef struct _Pixmap *PixmapPtr;
*/
/*
//from gc_struct.h (utahglx)
// there is padding in the bit fields because the Sun compiler doesn't
// force alignment to 32-bit boundaries.  losers.
typedef struct _GC {
//    ScreenPtr		pScreen;
    unsigned char	depth;
    unsigned char	alu;
    unsigned short	lineWidth;
    unsigned short	dashOffset;
    unsigned short	numInDashList;
    unsigned char	*dash;
    unsigned int	lineStyle : 2;
    unsigned int	capStyle : 2;
    unsigned int	joinStyle : 2;
    unsigned int	fillStyle : 2;
    unsigned int	fillRule : 1;
    unsigned int 	arcMode : 1;
    unsigned int	subWindowMode : 1;
    unsigned int	graphicsExposures : 1;
    unsigned int	clientClipType : 2; // CT_<kind>
    unsigned int	miTranslate:1; // should mi things translate?
    unsigned int	tileIsPixel:1; // tile is solid pixel
    unsigned int	fExpose:1;     // Call exposure handling
    unsigned int	freeCompClip:1; // Free composite clip
    unsigned int	unused:14; // see comment above
    unsigned long	planemask;
    unsigned long	fgPixel;
    unsigned long	bgPixel;
    // alas -- both tile and stipple must be here as they
    // are independently specifiable
//    PixUnion		tile;
    PixmapPtr		stipple;
//    DDXPointRec		patOrg;		// origin for (tile, stipple)
    struct _Font	*font;
//    DDXPointRec		clipOrg;
//    DDXPointRec		lastWinOrg;	// position of window last validated
//    pointer		clientClip;
    unsigned long	stateChanges;	// masked with GC_<kind>
    unsigned long       serialNumber;
    GCFuncs		*funcs;
    GCOps		*ops;
    DevUnion		*devPrivates;

    // The following were moved here from private storage to allow device-
    // independent access to them from screen wrappers.
    // --- 1997.11.03  Marc Aurele La France (tsi@xfree86.org)
    PixmapPtr		pRotatedPixmap; // tile/stipple rotated for alignment
    RegionPtr		pCompositeClip;
    // fExpose & freeCompClip defined above
} GC;

typedef struct _GC *GCPtr;
*/

/*
 * Mesa wrapper for X window or Pixmap
 */
/*
//struct xsmesa_buffer
{
//    struct xsmesa_buffer *Next;  // Linked list pointer:

    GLboolean wasCurrent;    // was ever the current buffer?
    GLframebuffer *gl_buffer;    // depth, stencil, accum, etc buffers
//    XSMesaVisual xsm_visual; // the X/Mesa visual

    GLboolean pixmap_flag;   // is the buffer a Pixmap?
    DrawablePtr frontbuffer; // either a window or pixmap
    PixmapPtr backpixmap;    // back buffer Pixmap
    GLXImage *backimage;         // back buffer XImage

    DrawablePtr buffer;  // the current buffer, either equal to
                         // frontbuffer, backpixmap or XIMAGE (None)

//    ColormapPtr cmap;        // the X colormap

    GLint db_state;      // 0 = single buffered
    // BACK_PIXMAP = use Pixmap for back buffer
    // BACK_XIMAGE = use XImage for back buffer

    GLuint width, height;    // size of buffer

    GLint bottom;        // used for FLIP macro below
    GLubyte *ximage_origin1; // used for PIXELADDR1 macro
    GLint ximage_width1;
    GLushort *ximage_origin2;    // used for PIXELADDR2 macro
    GLint ximage_width2;
    GLuint *ximage_origin4;  // used for PIXELADDR4 macro
    GLint ximage_width4;

//    PixmapPtr stipple_pixmap;    // For polygon stippling
//    GCPtr stipple_gc;        // For polygon stippling

//    GCPtr gc1;           // GC for infrequent color changes
//    GCPtr gc2;           // GC for frequent color changes
//    GCPtr cleargc;       // GC for clearing the color buffer

    char scandata[4*MAX_WIDTH];

    // The following are here instead of in the XSMesaVisual
    // because they depend on the window's colormap.

    // For PF_DITHER, PF_LOOKUP, PF_GRAYSCALE
    unsigned long color_table[576];      // RGB -> pixel value

    // For PF_DITHER, PF_LOOKUP, PF_GRAYSCALE
    GLubyte pixel_to_r[65536];       // pixel value -> red
    GLubyte pixel_to_g[65536];       // pixel value -> green
    GLubyte pixel_to_b[65536];       // pixel value -> blue

    // Used to do XAllocColor/XFreeColors accounting:
    int num_alloced;
    unsigned long alloced_colors[256];
};

//typedef struct xsmesa_buffer *XSMesaBuffer;
*/
/*
// Mesa wrapper for core rendering context
//struct xsmesa_context
{
    GLcontext *gl_ctx;      // the core library context
//    void      *hw_ctx;      // HW specific context

//    XSMesaVisual xsm_visual;        // Describes the buffers
//    XSMesaBuffer xsm_buffer;        // current framebuffer

//    ScreenPtr pScreen;  // == xm_visual->display
    GLboolean swapbytes;    // Host byte order != display byte order?

    GLuint pixelformat;     // Current pixel format

    GLubyte red, green, blue, alpha;// current drawing color
    unsigned long pixel;        // current drawing pixel value

    GLubyte clearcolor[4];      // current clearing color
    unsigned long clearpixel;   // current clearing pixel value

    // For the protocol dispatcher
//    unsigned int large_len;
//    unsigned int large_op;
//    int last_large_request;
//    char* large_buf;
//    int large_bufp;

   // Direct contexts
//    int try_direct;

    // disable hw rendering just for this context
    int	no_accel;
};
*/
/*
 * Special Mesa types
 */

//typedef struct xsmesa_context *XSMesaContext;

//looks like X11, but is defined in drivers instead???!!
typedef struct _Box {
    short x1, y1, x2, y2;
} BoxRec;

typedef struct _Box *BoxPtr;
/*
//do we need this X11 stuff? if so, complete...

//clip region
typedef struct _RegData {
    long	size;
    long 	numRects;
//  BoxRec	rects[size];   in memory but not explicitly declared
} RegDataRec, *RegDataPtr;

typedef struct _Region RegionRec, *RegionPtr;	// from xfree regionstr.h
struct _Region {
    BoxRec 	extents;
    RegDataPtr	data;
};
*/
/*
typedef struct _Window {						// from xfree windowstr.h
    DrawableRec		drawable;
//    WindowPtr		parent;		// ancestor chain
//    WindowPtr		nextSib;	// next lower sibling
//    WindowPtr		prevSib;	// next higher sibling
//    WindowPtr		firstChild;	// top-most child
//    WindowPtr		lastChild;	// bottom-most child
    RegionRec		clipList;	// clipping rectangle for output
//    RegionRec		borderClip;	// NotClippedByChildren + border
//    union _Validate	*valdata;
//    RegionRec		winSize;
//    RegionRec		borderSize;
//    DDXPointRec		origin;		// position relative to parent
//    unsigned short	borderWidth;
//    unsigned short	deliverableEvents;
//    Mask		eventMask;
//    PixUnion		background;
//    PixUnion		border;
//    pointer		backStorage;	// null when BS disabled
//    WindowOptPtr	optional;
//    unsigned		backgroundState:2; // None, Relative, Pixel, Pixmap
//    unsigned		borderIsPixel:1;
//    unsigned		cursorIsNone:1;	// else real cursor (might inherit)
//    unsigned		backingStore:2;
//    unsigned		saveUnder:1;
//    unsigned		DIXsaveUnder:1;
//    unsigned		bitGravity:4;
//    unsigned		winGravity:4;
//    unsigned		overrideRedirect:1;
//    unsigned		visibility:2;
//    unsigned		mapped:1;
//    unsigned		realized:1;	// ancestors are all mapped
//    unsigned		viewable:1;	// realized && InputOutput
//    unsigned		dontPropagate:3;// index into DontPropagateMasks
//    unsigned		forcedBS:1;	// system-supplied backingStore
//#ifdef NEED_DBE_BUF_BITS
//#define DBE_FRONT_BUFFER 1
//#define DBE_BACK_BUFFER  0
//    unsigned		dstBuffer:1;	// destination buffer for rendering
//    unsigned		srcBuffer:1;	// source buffer for rendering
//#endif
//    DevUnion		*devPrivates;
} WindowRec;

typedef struct _Window *WindowPtr;				// from xfree window.h
*/
/*
typedef struct _Visual {
//rudolf:
//    VisualID		vid;
	uint32		vid;
//    short		class;
    short		bitsPerRGBValue;
    short		ColormapEntries;
    short		nplanes;// = log2 (ColormapEntries). This does not
				 // imply that the screen has this many planes.
				 // it may have more or fewer
    unsigned long	redMask, greenMask, blueMask;
    int			offsetRed, offsetGreen, offsetBlue;
  } VisualRec;

typedef struct _Visual *VisualPtr;

typedef struct _Depth {
    unsigned char	depth;
    short		numVids;
//rudo
//    VisualID		*vids;    // block of visual ids for this depth
	uint32	*vids;    // block of visual ids for this depth
  } DepthRec;

typedef struct _Depth  *DepthPtr;
*/
GLuint depth_test_span( GLcontext* ctx, GLuint n, GLint x, GLint y,
                               const GLdepth z[], GLubyte mask[] );
void depth_test_pixels( GLcontext* ctx, GLuint n, const GLint x[], const GLint y[],
                               const GLdepth z[], GLubyte mask[] );
void clear_color( GLcontext *ctx,
                         GLubyte r, GLubyte g, GLubyte b, GLubyte a );
//unsigned long xsmesa_color_to_pixel( XSMesaContext xsmesa,
//                                     GLubyte r, GLubyte g, GLubyte b, GLubyte a);

#endif /* _GLH_H */
