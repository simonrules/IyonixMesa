#include "assyntax.h"

   SEG_TEXT


#if !defined(NASM_ASSEMBLER) && !defined(MASM_ASSEMBLER)
#define LLBL(a) .L ## a
#else
#define LLBL(a) a
#endif




ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_general_raw)
GLNAME(gl_3dnow_transform_points2_general_raw):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(4), REGOFF(16, ECX) )
    OR_B      ( CONST(15), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( EDI )
    PUSH_L    ( ESI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    FEMMS

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TPGR_3 ) )

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    PUNPCKLDQ ( REGOFF(16, ECX), MM0 )  /* m10           | m00                */

    MOVD      ( REGOFF(4, ECX), MM1 )   /*               | m01                */
    PUNPCKLDQ ( REGOFF(20, ECX), MM1 )  /* m11           | m01                */

    MOVD      ( REGOFF(8, ECX), MM2 )   /*               | m02                */
    PUNPCKLDQ ( REGOFF(24, ECX), MM2 )  /* m12           | m02                */

    MOVD      ( REGOFF(12, ECX), MM3 )  /*               | m03                */
    PUNPCKLDQ ( REGOFF(28, ECX), MM3 )  /* m13           | m03                */

    MOVQ      ( REGOFF(48, ECX), MM4 )  /* m31           | m30                */
    MOVQ      ( REGOFF(56, ECX), MM5 )  /* m33           | m32                */


ALIGNTEXT32
LLBL( G3TPGR_2 ):

    MOVQ      ( REGIND(EAX), MM6 )      /* x1            | x0                 */
    MOVQ      ( MM6, MM7 )              /* x1            | x0                 */

    PFMUL     ( MM0, MM6 )              /* x1*m10        | x0*m00             */
    PFMUL     ( MM1, MM7 )              /* x1*m11        | x0*m01             */

    PFACC     ( MM7, MM6 )              /* x0*m01+x1*m11 | x0*x00+x1*m10      */
    PFADD     ( MM4, MM6 )              /* x0*...*m11+m31| x0*x00+x1*m10+m30  */

    MOVQ      ( MM6, REGIND(EDX) )      /* write r1, r0                       */
    MOVQ      ( REGIND(EAX), MM6 )      /* x1            | x0                 */

    MOVQ      ( MM6, MM7 )              /* x1            | x0                 */
    PFMUL     ( MM2, MM6 )              /* x1*m12        | x0*m02             */

    PFMUL     ( MM3, MM7 )              /* x1*m13        | x0*m03             */
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    PFACC     ( MM7, MM6 )              /* x0*m03+x1*m13 | x0*x02+x1*m12      */
    PFADD     ( MM5, MM6 )              /* x0*...*m13+m33| x0*x02+x1*m12+m32  */

    MOVQ      ( MM6, REGOFF(8, EDX) )   /* write r3, r2                       */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    DEC_L     ( ESI )                   /* decrement vertex counter           */
    JA        ( LLBL( G3TPGR_2 ) )      /* cnt > 0 ? -> process next vertex   */

LLBL( G3TPGR_3 ):
    FEMMS

    POP_L     ( ESI )
    POP_L     ( EDI )

    POP_L     ( ESI )
    RET






ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_identity_raw)
GLNAME(gl_3dnow_transform_points2_identity_raw):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(2), REGOFF(16, ECX) )
    OR_B      ( CONST(3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( EDI )
    PUSH_L    ( ESI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    FEMMS

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TPIR_3 ) )

LLBL( G3TPIR_3 ):
    MOVQ      ( REGIND(EAX), MM0 )      /* x1            | x0                 */
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    MOVQ      ( MM0, REGIND(EDX) )      /* r1            | r0                 */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    DEC_L     ( ESI )                   /* decrement vertex counter           */
    JA        ( LLBL( G3TPIR_3 ) )      /* cnt > 0 ? -> process next vertex   */

LLBL( G3TPIR_4 ):
    FEMMS

    POP_L     ( ESI )
    POP_L     ( EDI )

    POP_L     ( ESI )
    RET




ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_2d_raw)
GLNAME(gl_3dnow_transform_points2_2d_raw):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(2), REGOFF(16, ECX) )
    OR_B      ( CONST(3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( EDI )
    PUSH_L    ( ESI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    FEMMS

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP2R_3 ) )

    MOVQ      ( REGIND(ECX), MM0 )      /* m01           | m00                */
    MOVQ      ( REGOFF(16, ECX), MM1 )  /* m11           | m10                */

    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */

ALIGNTEXT32
LLBL( G3TP2R_2 ):

    MOVD      ( REGIND(EAX), MM4 )      /*               | x0                 */
    MOVD      ( REGOFF(4, EAX), MM5 )   /*               | x1                 */

    PUNPCKLDQ ( MM4, MM4 )              /* x0            | x0                 */
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    PFMUL     ( MM0, MM4 )              /* x0*m01        | x0*m00             */
    PUNPCKLDQ ( MM5, MM5 )              /* x1            | x1                 */

    PFMUL     ( MM1, MM5 )              /* x1*m11        | x1*m10             */
    PFADD     ( MM2, MM4 )              /* x...x1*m11+31 | x0*m00+x1*m10+m30  */

    PFADD     ( MM5, MM4 )              /* x0*m01+x1*m11 | x0*m00+x1*m10      */
    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */

    ADD_L     ( CONST(16), EDX )        /* next r                             */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TP2R_2 ) )      /* cnt > 0 ? -> process next vertex   */

LLBL( G3TP2R_3 ):
    FEMMS

    POP_L     ( ESI )
    POP_L     ( EDI )

    POP_L     ( ESI )
    RET




ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_2d_no_rot_raw)
GLNAME(gl_3dnow_transform_points2_2d_no_rot_raw):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(2), REGOFF(16, ECX) )
    OR_B      ( CONST(3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( EDI )
    PUSH_L    ( ESI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    FEMMS

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP2NRR_3 ) )

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    PUNPCKLDQ ( REGOFF(20, ECX), MM0 )  /* m11           | m00                */

    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */

ALIGNTEXT32
LLBL( G3TP2NRR_2 ):

    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                 */
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    PFMUL     ( MM0, MM4 )              /* x1*m11        | x0*m00             */
    PFADD     ( MM2, MM4 )              /* m31           | x0*m00+m30         */

    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    DEC_L     ( ESI )                   /* decrement vertex counter           */
    JA        ( LLBL( G3TP2NRR_2 ) )    /* cnt > 0 ? -> process next vertex   */

LLBL( G3TP2NRR_3 ):
    FEMMS

    POP_L     ( ESI )
    POP_L     ( EDI )

    POP_L     ( ESI )
    RET





ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_3d_raw)
GLNAME(gl_3dnow_transform_points2_3d_raw):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(3), REGOFF(16, ECX) )
    OR_B      ( CONST(7), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( EDI )
    PUSH_L    ( ESI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    FEMMS

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP3R_3 ) )

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    PUNPCKLDQ ( REGOFF(16, ECX), MM0 )  /* m10           | m00                */

    MOVD      ( REGOFF(4, ECX), MM1 )   /*               | m01                */
    PUNPCKLDQ ( REGOFF(20, ECX), MM1 )  /* m11           | m01                */

    MOVD      ( REGOFF(8, ECX), MM2 )   /*               | m02                */
    PUNPCKLDQ ( REGOFF(24, ECX), MM2 )  /* m12           | m02                */

    MOVQ      ( REGOFF(48, ECX), MM4 )  /* m31           | m30                */
    MOVD      ( REGOFF(56, ECX), MM5 )  /*               | m32                */


ALIGNTEXT32
LLBL( G3TP3R_2 ):

    MOVQ      ( REGIND(EAX), MM6 )      /* x1            | x0                 */
    MOVQ      ( MM6, MM7 )              /* x1            | x0                 */

    PFMUL     ( MM0, MM6 )              /* x1*m10        | x0*m00             */
    PFMUL     ( MM1, MM7 )              /* x1*m11        | x0*m01             */

    PFACC     ( MM7, MM6 )              /* x0*m01+x1*m11 | x0*x00+x1*m10      */
    PFADD     ( MM4, MM6 )              /* x0*...*m11+m31| x0*x00+x1*m10+m30  */

    MOVQ      ( MM6, REGIND(EDX) )      /* write r1, r0                       */
    MOVQ      ( REGIND(EAX), MM6 )      /* x1            | x0                 */

    MOVQ      ( MM6, MM7 )              /* x1            | x0                 */
    PFMUL     ( MM2, MM6 )              /* x1*m12        | x0*m02             */

    PFACC     ( MM7, MM6 )              /* ***trash***   | x0*x02+x1*m12      */
    PFADD     ( MM5, MM6 )              /* ***trash***   | x0*x02+x1*m12+m32  */

    MOVD      ( MM6, REGOFF(8, EDX) )   /* write r2                           */
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    ADD_L     ( CONST(16), EDX )        /* next r                             */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TP3R_2 ) )      /* cnt > 0 ? -> process next vertex   */

LLBL( G3TP3R_3 ):
    FEMMS

    POP_L     ( ESI )
    POP_L     ( EDI )

    POP_L     ( ESI )
    RET




ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_3d_no_rot_raw)
GLNAME(gl_3dnow_transform_points2_3d_no_rot_raw):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(3), REGOFF(16, ECX) )
    OR_B      ( CONST(7), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( EDI )
    PUSH_L    ( ESI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    FEMMS

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP3NRR_3 ) )

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    PUNPCKLDQ ( REGOFF(20, ECX), MM0 )  /* m11           | m00                */

    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */
    MOVD      ( REGOFF(56, ECX), MM3 )  /*               | m32                */


ALIGNTEXT32
LLBL( G3TP3NRR_2 ):

    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                 */
    PFMUL     ( MM0, MM4 )              /* x1*m11        | x0*m00             */

    PFADD     ( MM2, MM4 )              /* x1*m11+m31    | x0*m00+m30         */
    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */

    MOVD      ( MM3, REGOFF(8, EDX) )   /* write r2                           */
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    ADD_L     ( CONST(16), EDX )        /* next r                             */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TP3NRR_2 ) )    /* cnt > 0 ? -> process next vertex   */

LLBL( G3TP3NRR_3 ):
    FEMMS

    POP_L     ( ESI )
    POP_L     ( EDI )

    POP_L     ( ESI )
    RET





ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_perspective_raw)
GLNAME(gl_3dnow_transform_points2_perspective_raw):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(4), REGOFF(16, ECX) )
    OR_B      ( CONST(15), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( EDI )
    PUSH_L    ( ESI )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )
    MOV_L     ( REGOFF(12, EAX), EDI )
    MOV_L     ( REGOFF(4, EAX), EAX )

    FEMMS

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TPPR_3 ) )

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    PUNPCKLDQ ( REGOFF(20, ECX), MM0 )  /* m11           | m00                */

    MOVD      ( REGOFF(56, ECX), MM3 )  /*               | m32                */

ALIGNTEXT32
LLBL( G3TPPR_2 ):

    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                 */
    PFMUL     ( MM0, MM4 )              /* x1*m11        | x0*m00             */

    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */
    MOVQ      ( MM3, REGOFF(8, EDX) )   /* write r2  (=m32), r3 (=0)          */

    ADD_L     ( EDI, EAX )              /* next vertex                        */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    DEC_L     ( ESI )                   /* decrement vertex counter           */
    JA        ( LLBL( G3TPPR_2 ) )      /* cnt > 0 ? -> process next vertex   */

LLBL( G3TPPR_3 ):
    FEMMS

    POP_L     ( ESI )
    POP_L     ( EDI )

    POP_L     ( ESI )
    RET




