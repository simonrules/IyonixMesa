#include "assyntax.h"

   SEG_TEXT


#if !defined(NASM_ASSEMBLER) && !defined(MASM_ASSEMBLER)
#define LLBL(a) .L ## a
#else
#define LLBL(a) a
#endif



#define  VEC_SIZE_1   1
#define  VEC_SIZE_2   3
#define  VEC_SIZE_3   7
#define  VEC_SIZE_4   15




ALIGNTEXT32
GLOBL GLNAME(gl_3dnow_transform_points3_general_raw)
GLNAME( gl_3dnow_transform_points3_general_raw ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(4), REGOFF(16, ECX) )
    OR_B      ( CONST(VEC_SIZE_4), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

    PUSH_L    ( EDI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TPGR_2 ) )

    PREFETCHW ( REGIND(EDX) )

    MOVQ      ( REGIND(EAX), MM0 )      /* x1            | x0                */
    MOVD      ( REGOFF(8, EAX), MM2 )   /*               | x2                */

    ADD_L     ( EDI, EAX )              /* next vertex                       */

ALIGNTEXT32
LLBL( G3TPGR_1 ):

    PREFETCHW ( REGOFF(32, EDX) )       /* prefetch 2 vertices ahead         */

    PREFETCH  ( REGIND(EAX) )

    MOVQ      ( MM0, MM1 )              /* x1            | x0                */
    PUNPCKLDQ ( MM2, MM2 )              /* x2            | x2                */

    PUNPCKLDQ ( MM0, MM0 )              /* x0            | x0                */
    MOVQ      ( MM2, MM5 )              /* x2            | x2                */

    PUNPCKHDQ ( MM1, MM1 )              /* x1            | x1                */
    PFMUL     ( REGOFF(32, ECX), MM2 )  /* x2*m9         | x2*m8             */

    MOVQ      ( MM0, MM3 )              /* x0            | x0                */
    PFMUL     ( REGOFF(40, ECX), MM5 )  /* x2*m11        | x2*m10            */

    MOVQ      ( MM1, MM4 )              /* x1            | x1                */
    PFMUL     ( REGIND(ECX), MM0 )      /* x0*m1         | x0*m0             */

    PFADD     ( REGOFF(48, ECX), MM2 )  /* x2*m9+m13     | x2*m8+m12         */
    PFMUL     ( REGOFF(16, ECX), MM1 )  /* x1*m5         | x1*m4             */

    PFADD     ( REGOFF(56, ECX), MM5 )  /* x2*m11+m15    | x2*m10+m14        */
    PFADD     ( MM0, MM1 )              /* x0*m1+x1*m5   | x0*m0+x1*m4       */

    PFMUL     ( REGOFF(8, ECX), MM3 )   /* x0*m3         | x0*m2             */
    PFADD     ( MM1, MM2 )              /* result r1     | r0                */

    PFMUL     ( REGOFF(24, ECX), MM4 )  /* x1*m7         | x1*m6             */
    ADD_L     ( CONST(16), EDX )        /* next r                            */

    PFADD     ( MM3, MM4 )              /* x0*m3+x1*m7   | x0*m2+x1*m6       */
    MOVQ      ( MM2, REGOFF(-16, EDX) ) /* write r0, r1                      */

    PFADD     ( MM4, MM5 )              /* r3            | r2                */
    MOVQ      ( MM5, REGOFF(-8, EDX) )  /* write r2, r3                      */

    MOVQ      ( REGIND(EAX), MM0 )      /* x1            | x0                */
    MOVD      ( REGOFF(8, EAX), MM2 )   /*               | x2                */

    ADD_L     ( EDI, EAX )              /* next vertex                       */
    DEC_L     ( ESI )                   /* decrement vertex counter          */
    JA        ( LLBL( G3TPGR_1 ) )      /* cnt > 0 ? -> process next vertex  */

ALIGNTEXT32
LLBL( G3TPGR_2 ):

    FEMMS
    POP_L     ( EDI )
    POP_L     ( ESI )
    RET





ALIGNTEXT32
GLOBL GLNAME(gl_3dnow_transform_points3_identity_raw)
GLNAME( gl_3dnow_transform_points3_identity_raw ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(3), REGOFF(16, ECX) )
    OR_B      ( CONST(VEC_SIZE_3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

    PUSH_L    ( EDI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TPIR_2 ) )

    PREFETCHW ( REGIND(EDX) )

    MOVQ      ( REGIND(EAX), MM0 )      /* x1            | x0                */
    MOVD      ( REGOFF(8, EAX), MM1 )   /*               | x2                */

ALIGNTEXT32
LLBL( G3TPIR_1 ):

    PREFETCHW ( REGOFF(32, EDX) )

    ADD_L     ( EDI, EAX )              /* next vertex                       */
    ADD_L     ( CONST(16), EDX )        /* next r                            */

    DEC_L     ( ESI )                   /* decrement vertex counter          */
    MOVQ      ( MM0, REGOFF(-16, EDX) ) /* r1            | r0                */

    MOVD      ( MM1, REGOFF(-8, EDX) )  /*               | r2                */
    MOVQ      ( REGIND(EAX), MM0 )      /* x1            | x0                */

    MOVD      ( REGOFF(8, EAX), MM1 )   /*               | x2                */
    JA        ( LLBL( G3TPIR_1 ) )      /* cnt > 0 ? -> process next vertex  */

ALIGNTEXT32
LLBL( G3TPIR_2 ):

    FEMMS
    POP_L     ( EDI )
    POP_L     ( ESI )
    RET




ALIGNTEXT32
GLOBL GLNAME(gl_3dnow_transform_points3_2d_raw)
GLNAME( gl_3dnow_transform_points3_2d_raw ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(3), REGOFF(16, ECX) )
    OR_B      ( CONST(VEC_SIZE_3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

    PUSH_L    ( EDI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP2R_3 ) )

    PREFETCH  ( REGIND(EAX) )

    PREFETCHW ( REGIND(EDX) )

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    PUNPCKLDQ ( REGOFF(16, ECX), MM0 )  /* m10           | m00                */

    MOVD      ( REGOFF(4, ECX), MM1 )   /*               | m01                */
    PUNPCKLDQ ( REGOFF(20, ECX), MM1 )  /* m11           | m01                */

    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */
    MOVQ      ( REGIND(EAX), MM3 )      /* x1            | x0                 */

    MOVD      ( REGOFF(8, EAX), MM5 )   /*               | x2                 */
    ADD_L     ( EDI, EAX )              /* next vertex                        */

ALIGNTEXT32
LLBL( G3TP2R_2 ):

    PREFETCHW ( REGOFF(32, EDX) )       /* prefetch 2 vertices ahead         */

    PREFETCH  ( REGIND(EAX) )

    MOVQ      ( MM3, MM4 )              /* x1            | x0                 */
    PFMUL     ( MM0, MM3 )              /* x1*m10        | x0*m00             */

    ADD_L     ( CONST(16), EDX )        /* next r                             */
    PFMUL     ( MM1, MM4 )              /* x1*m11        | x0*m01             */

    PFACC     ( MM4, MM3 )              /* x0*m00+x1*m10 | x0*m01+x1*m11      */
    MOVD      ( MM5, REGOFF(-8, EDX) )  /* write r2 (=x2)                     */

    PFADD     ( MM2, MM3 )              /* x0*...*m10+m30| x0*m01+x1*m11+m31  */
    MOVQ      ( MM3, REGOFF(-16, EDX) ) /* write r0, r1                       */

    MOVQ      ( REGIND(EAX), MM3 )      /* x1            | x0                 */
    MOVD      ( REGOFF(8, EAX), MM5 )   /*               | x2                 */

    ADD_L     ( EDI, EAX )              /* next vertex                        */
    DEC_L     ( ESI )                   /* decrement vertex counter           */
    JA        ( LLBL( G3TP2R_2 ) )      /* cnt > 0 ? -> process next vertex   */

ALIGNTEXT32
LLBL( G3TP2R_3 ):

    FEMMS
    POP_L     ( EDI )
    POP_L     ( ESI )
    RET





ALIGNTEXT32
GLOBL GLNAME(gl_3dnow_transform_points3_2d_no_rot_raw)
GLNAME( gl_3dnow_transform_points3_2d_no_rot_raw ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(3), REGOFF(16, ECX) )
    OR_B      ( CONST(VEC_SIZE_3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

    PUSH_L    ( EDI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP2NRR_2 ) )

    PREFETCH  ( REGIND(EAX) )

    PREFETCHW ( REGIND(EDX) )

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    PUNPCKLDQ ( REGOFF(20, ECX), MM0 )  /* m11           | m00                */

    MOVQ      ( REGOFF(48, ECX), MM1 )  /* m31           | m30                */
    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                 */

    MOVD      ( REGOFF(8, EAX), MM5 )   /*               | x2                 */
    ADD_L     ( EDI, EAX )              /* next vertex                       */


ALIGNTEXT32
LLBL( G3TP2NRR_1 ):

    PREFETCHW ( REGOFF(32, EDX) )       /* prefetch 2 vertices ahead         */

    PREFETCH  ( REGIND(EAX) )

    PFMUL     ( MM0, MM4 )              /* x1*m11        | x0*m00            */
    ADD_L     ( CONST(16), EDX )        /* next r                            */

    PFADD     ( MM1, MM4 )              /* x1*m11+m31    | x0*m00+m30        */

    MOVQ      ( MM4, REGOFF(-16, EDX) ) /* write r0, r1                      */
    MOVD      ( MM5, REGOFF(-8, EDX) )  /* write r2 (=x2)                    */

    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                */
    MOVD      ( REGOFF(8, EAX), MM5 )   /*               | x2                */

    ADD_L     ( EDI, EAX )              /* next vertex                       */
    DEC_L     ( ESI )                   /* decrement vertex counter          */

    JA        ( LLBL( G3TP2NRR_1 ) )    /* cnt > 0 ? -> process next vertex  */

ALIGNTEXT32
LLBL( G3TP2NRR_2 ):

    FEMMS
    POP_L     ( EDI )
    POP_L     ( ESI )
    RET




ALIGNTEXT32
GLOBL GLNAME(gl_3dnow_transform_points3_3d_raw)
GLNAME( gl_3dnow_transform_points3_3d_raw ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(3), REGOFF(16, ECX) )
    OR_B      ( CONST(VEC_SIZE_3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

    PUSH_L    ( EDI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP3R_2 ) )

    PREFETCH  ( REGIND(EAX) )

    PREFETCH  ( REGIND(EDX) )

    MOVD      ( REGOFF(8, ECX), MM7 )   /*               | m2                */
    PUNPCKLDQ ( REGOFF(24, ECX), MM7 )  /* m6            | m2                */

    MOVQ      ( REGIND(EAX), MM0 )      /* x1            | x0                */
    MOVD      ( REGOFF(8, EAX), MM1 )   /*               | x2                */

    ADD_L     ( EDI, EAX )              /* next vertex                       */

ALIGNTEXT32
LLBL( G3TP3R_1 ):

    PREFETCHW ( REGOFF(32, EDX) )       /* prefetch 2 vertices ahead         */

    PREFETCH  ( REGIND(EAX) )

    MOVQ      ( MM0, MM2 )              /* x1            | x0                */
    ADD_L     ( CONST(16), EDX )        /* next r                            */

    PUNPCKLDQ ( MM2, MM2 )              /* x0            | x0                */
    MOVQ      ( MM0, MM3 )              /* x1            | x0                */

    PFMUL     ( REGIND(ECX), MM2 )      /* x0*m1         | x0*m0             */
    PUNPCKHDQ ( MM3, MM3 )              /* x1            | x1                */

    MOVQ      ( MM1, MM4 )              /*               | x2                */
    PFMUL     ( REGOFF(16, ECX), MM3 )  /* x1*m5         | x1*m4             */

    PUNPCKLDQ ( MM4, MM4 )              /* x2            | x2                */
    PFADD     ( MM2, MM3 )              /* x0*m1+x1*m5   | x0*m0+x1*m4       */

    PFMUL     ( REGOFF(32, ECX), MM4 )  /* x2*m9         | x2*m8             */
    PFADD     ( REGOFF(48, ECX), MM3 )  /* x0*m1+...+m11 | x0*m0+x1*m4+m12   */

    PFMUL     ( MM7, MM0 )              /* x1*m6         | x0*m2             */
    PFADD     ( MM4, MM3 )              /* r1            | r0                */

    PFMUL     ( REGOFF(40, ECX), MM1 )  /*               | x2*m10            */
    PUNPCKLDQ ( REGOFF(56, ECX), MM1 )  /* m14           | x2*m10            */

    PFACC     ( MM0, MM1 )

    MOVQ      ( MM3, REGOFF(-16, EDX) ) /* write r0, r1                      */
    PFACC     ( MM1, MM1 )              /*               | r2                */

    MOVD      ( MM1, REGOFF(-8, EDX) )  /* write r2                          */
    MOVQ      ( REGIND(EAX), MM0 )      /* x1            | x0                */

    MOVD      ( REGOFF(8, EAX), MM1 )   /*               | x2                */

    ADD_L     ( EDI, EAX )              /* next vertex                       */
    DEC_L     ( ESI )                   /* decrement vertex counter          */
    JA        ( LLBL( G3TP3R_1 ) )      /* cnt > 0 ? -> process next vertex  */

ALIGNTEXT32
LLBL( G3TP3R_2 ):

    FEMMS
    POP_L     ( EDI )
    POP_L     ( ESI )
    RET




ALIGNTEXT32
GLOBL GLNAME(gl_3dnow_transform_points3_3d_no_rot_raw)
GLNAME( gl_3dnow_transform_points3_3d_no_rot_raw ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(3), REGOFF(16, ECX) )
    OR_B      ( CONST(VEC_SIZE_3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

    PUSH_L    ( EDI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP3NRR_2 ) )

    PREFETCH  ( REGIND(EAX) )

    PREFETCHW ( REGIND(EDX) )

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    PUNPCKLDQ ( REGOFF(20, ECX), MM0 )  /* m11           | m00                */

    MOVD      ( REGOFF(40, ECX), MM2 )  /*               | m22                */
    PUNPCKLDQ ( MM2, MM2 )              /* m22           | m22                */

    MOVQ      ( REGOFF(48, ECX), MM1 )  /* m31           | m30                */
    MOVD      ( REGOFF(56, ECX), MM3 )  /*               | m32                */

    PUNPCKLDQ ( MM3, MM3 )              /* m32           | m32                */
    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                 */

    MOVD      ( REGOFF(8, EAX), MM5 )   /*               | x2                 */


ALIGNTEXT32
LLBL( G3TP3NRR_1 ):
    PREFETCHW ( REGOFF(32, EDX) )       /* prefetch 2 vertices ahead         */
    ADD_L     ( EDI, EAX )              /* next vertex                       */

    PREFETCHW ( REGIND(EAX) )
    PFMUL     ( MM0, MM4 )              /* x1*m11        | x0*m00            */

    PFADD     ( MM1, MM4 )              /* x1*m11+m31    | x0*m00+m30        */
    PFMUL     ( MM2, MM5 )              /*               | x2*m22            */

    PFADD     ( MM3, MM5 )              /*               | x2*m22+m32        */
    MOVQ      ( MM4, REGIND(EDX) )      /* write r0, r1                      */

    ADD_L     ( CONST(16), EDX )        /* next r                            */
    DEC_L     ( ESI )                   /* decrement vertex counter          */

    MOVD      ( MM5, REGOFF(-8, EDX) )  /* write r2                          */
    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                */

    MOVD      ( REGOFF(8, EAX), MM5 )   /*               | x2                */
    JA        ( LLBL( G3TP3NRR_1 ) )    /* cnt > 0 ? -> process next vertex  */

ALIGNTEXT32
LLBL( G3TP3NRR_2 ):

    FEMMS
    POP_L     ( EDI )
    POP_L     ( ESI )
    RET




ALIGNTEXT32
GLOBL GLNAME(gl_3dnow_transform_points3_perspective_raw)
GLNAME( gl_3dnow_transform_points3_perspective_raw ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(4), REGOFF(16, ECX) )
    OR_B      ( CONST(VEC_SIZE_4), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

    PUSH_L    ( EDI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TPPR_2 ) )

    PREFETCH  ( REGIND(EAX) )

    PREFETCHW ( REGIND(EDX) )

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00               */
    PUNPCKLDQ ( REGOFF(20, ECX), MM0 )  /* m11           | m00               */

    MOVQ      ( REGOFF(32, ECX), MM1 )  /* m21           | m20               */
    MOVD      ( REGOFF(40, ECX), MM2 )  /*               | m22               */

    MOVD      ( REGOFF(56, ECX), MM3 )  /*               | m32               */

    MOVD      ( REGOFF(8, EAX), MM5 )   /*               | x2                */
    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                */

    ADD_L     ( EDI, EAX )              /* next vertex                       */

ALIGNTEXT32
LLBL( G3TPPR_1 ):

    PREFETCHW ( REGOFF(32, EDX) )       /* prefetch 2 vertices ahead         */

    PREFETCH  ( REGIND(EAX) )

    PXOR      ( MM7, MM7 )              /* 0             | 0                 */
    MOVQ      ( MM5, MM6 )              /*               | x2                */

    PFMUL     ( MM0, MM4 )              /* x1*m11        | x0*m00            */
    PFSUB     ( MM5, MM7 )              /*               | -x2               */

    PFMUL     ( MM2, MM6 )              /*               | x2*m22            */
    PUNPCKLDQ ( MM5, MM5 )              /* x2            | x2                */

    ADD_L     ( CONST(16), EDX )        /* next r                            */
    PFMUL     ( MM1, MM5 )              /* x2*m21        | x2*m20            */

    PFADD     ( MM3, MM6 )              /*               | x2*m22+m32        */
    PFADD     ( MM4, MM5 )              /* x1*m11+x2*m21 | x0*m00+x2*m20     */

    MOVQ      ( MM5, REGOFF(-16, EDX) ) /* write r0, r1                      */
    MOVD      ( MM6, REGOFF(-8, EDX) )  /* write r2                          */

    MOVD      ( MM7, REGOFF(-4, EDX) )  /* write r3                          */
    MOVD      ( REGOFF(8, EAX), MM5 )   /*               | x2                */

    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                */
    ADD_L     ( EDI, EAX )              /* next vertex                       */

    DEC_L     ( ESI )                   /* decrement vertex counter          */
    JA        ( LLBL( G3TPPR_1 ) )      /* cnt > 0 ? -> process next vertex  */

ALIGNTEXT32
LLBL( G3TPPR_2 ):

    FEMMS
    POP_L     ( EDI )
    POP_L     ( ESI )
    RET





