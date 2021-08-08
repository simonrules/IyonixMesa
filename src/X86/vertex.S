#include "assyntax.h"

	
	SEG_TEXT

#if !defined(NASM_ASSEMBLER) && !defined(MASM_ASSEMBLER)
#define LLBL(a) .L##a
#else
#define LLBL(a) a
#endif

/*#define MAT_SX 0*/        /*  accessed by REGIND !! */
#define MAT_SY 20
#define MAT_SZ 40
#define MAT_TX 48
#define MAT_TY 52
#define MAT_TZ 56




/*
 * void gl_v16_x86_general_xform (	GLfloat *dest,
 *				const GLfloat *m,
 *				const GLfloat *src,
 *                              GLuint src_stride,
 *				GLuint count )
 */


/* This is nothing more glamorous than an objdump of one of Josh's
 * routines hacked to match the above.    
 */
ALIGNTEXT16
GLOBL GLNAME( gl_v16_x86_general_xform )
GLNAME( gl_v16_x86_general_xform ):


	PUSH_L ( EDI )
	PUSH_L ( ESI )

    	MOV_L     ( REGOFF(12, ESP), EAX ) /* dest */
    	MOV_L     ( REGOFF(16, ESP), ESI ) /* mat */
    	MOV_L     ( REGOFF(20, ESP), EDX ) /* src */
    	MOV_L     ( REGOFF(24, ESP), EDI ) /* src_stride */
    	MOV_L     ( REGOFF(28, ESP), ECX ) /* count */


LLBL(v16x86_loop):	
        FLD_S   ( REGOFF( 0x0, EDX ) )
        FMUL_S  ( REGOFF( 0x0, ESI ) )
        FLD_S   ( REGOFF( 0x0, EDX ) )
        FMUL_S  ( REGOFF( 0x4, ESI ) )
        FLD_S   ( REGOFF( 0x0, EDX ) )
        FMUL_S  ( REGOFF( 0x8, ESI ) )
        FLD_S   ( REGOFF( 0x0, EDX ) )
        FMUL_S  ( REGOFF( 0xc, ESI ) )
        FLD_S   ( REGOFF( 0x4, EDX ) )
        FMUL_S  ( REGOFF( 0x10, ESI ) )
        FLD_S   ( REGOFF( 0x4, EDX ) )
        FMUL_S  ( REGOFF( 0x14, ESI ) )
        FLD_S   ( REGOFF( 0x4, EDX ) )
        FMUL_S  ( REGOFF( 0x18, ESI ) )
        FLD_S   ( REGOFF( 0x4, EDX ) )
        FMUL_S  ( REGOFF( 0x1c, ESI ) )
        FXCH    ( ST(3) )
        FADDP   ( ST(0),ST(7) )
        FXCH    ( ST(1) )
        FADDP   ( ST(0),ST(5) )
        FADDP   ( ST(0),ST(3) )
        FADDP   ( ST(0),ST(1) )
        FLD_S   ( REGOFF( 0x8, EDX ) )
        FMUL_S  ( REGOFF( 0x20, ESI ) )
        FLD_S   ( REGOFF( 0x8, EDX ) )
        FMUL_S  ( REGOFF( 0x24, ESI ) )
        FLD_S   ( REGOFF( 0x8, EDX ) )
        FMUL_S  ( REGOFF( 0x28, ESI ) )
        FLD_S   ( REGOFF( 0x8, EDX ) )
        FMUL_S  ( REGOFF( 0x2c, ESI ) )
        FXCH    ( ST(3) )
        FADDP   ( ST(0),ST(7) )
        FXCH    ( ST(1) )
        FADDP   ( ST(0),ST(5) )
        FADDP   ( ST(0),ST(3) )
        FADDP   ( ST(0),ST(1) )
        FXCH    ( ST(3) )
        FADD_S  ( REGOFF( 0x30, ESI ) )
        FXCH    ( ST(2) )
        FADD_S  ( REGOFF( 0x34, ESI ) )
        FXCH    ( ST(1) )
        FADD_S  ( REGOFF( 0x38, ESI ) )
        FXCH    ( ST(3) )
        FADD_S  ( REGOFF( 0x3c, ESI ) )
        FXCH    ( ST(2) )
        FSTP_S  ( REGOFF( 0x0, EAX ) )
        FSTP_S  ( REGOFF( 0x4, EAX ) )
        FXCH    ( ST(1) )
        FSTP_S  ( REGOFF( 0x8, EAX ) )
        FSTP_S  ( REGOFF( 0xc, EAX ) )
        ADD_L   ( CONST(64), EAX )
        ADD_L   ( EDI, EDX )
        DEC_L   ( ECX )
        JNE     ( LLBL(v16x86_loop) )

	POP_L ( ESI )
	POP_L ( EDI )
	RET

