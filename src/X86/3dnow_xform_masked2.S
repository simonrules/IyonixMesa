#include "assyntax.h"

   SEG_TEXT

#if !defined(NASM_ASSEMBLER) && !defined(MASM_ASSEMBLER)
#define LLBL(a) .L ## a
#else
#define LLBL(a) a
#endif



ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_general_masked)
GLNAME( gl_3dnow_transform_points2_general_masked ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(4), REGOFF(16, ECX) )
    OR_B      ( CONST(15), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( ESI )
    PUSH_L    ( EDI )
    PUSH_L    ( EBX )
    PUSH_L    ( EBP )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )   /* count                            */
    MOV_L     ( REGOFF(12, EAX), EDI )  /* input stride                     */
    MOV_L     ( REGOFF(4, EAX), EAX )
    MOV_L     ( REGOFF(36, ESP), EBP )  /* clipmask                         */
    MOV_B     ( REGOFF(40, ESP), BL )   /* clip flag                        */

    FEMMS

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    MOVD      ( REGOFF(16, ECX), MM7 )  /*               | m10                */
    PSLLQ     ( CONST(32), MM7 )        /* m10           |                    */
    POR       ( MM7, MM0 )              /* m10           | m00                */

    MOVD      ( REGOFF(4, ECX), MM1 )   /*               | m01                */
    MOVD      ( REGOFF(20, ECX), MM7 )  /*               | m11                */
    PSLLQ     ( CONST(32), MM7 )        /* m11           |                    */
    POR       ( MM7, MM1 )              /* m11           | m01                */

    MOVD      ( REGOFF(8, ECX), MM2 )   /*               | m02                */
    MOVD      ( REGOFF(24, ECX), MM7 )  /*               | m12                */
    PSLLQ     ( CONST(32), MM7 )        /* m12           |                    */
    POR       ( MM7, MM2 )              /* m12           | m02                */

    MOVD      ( REGOFF(12, ECX), MM3 )  /*               | m03                */
    MOVD      ( REGOFF(28, ECX), MM7 )  /*               | m13                */
    PSLLQ     ( CONST(32), MM7 )        /* m13           |                    */
    POR       ( MM7, MM3 )              /* m13           | m03                */

    MOVQ      ( REGOFF(48, ECX), MM4 )  /* m31           | m30                */
    MOVQ      ( REGOFF(56, ECX), MM5 )  /* m33           | m32                */

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TPGM_4 ) )

ALIGNTEXT32

LLBL( G3TPGM_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TPGM_3 ) )      /* skip vertex                        */

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
    PFACC     ( MM7, MM6 )              /* x0*m03+x1*m13 | x0*x02+x1*m12      */

    PFADD     ( MM5, MM6 )              /* x0*...*m13+m33| x0*x02+x1*m12+m32  */
    MOVQ      ( MM6, REGOFF(8, EDX) )   /* write r3, r2                       */

LLBL( G3TPGM_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    INC_L     ( EBP )                   /* next clipmask                      */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TPGM_2 ) )      /* cnt > 0 ? -> process next vertex   */

LLBL( G3TPGM_4 ):
    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET





ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_identity_masked)
GLNAME( gl_3dnow_transform_points2_identity_masked ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(2), REGOFF(16, ECX) )
    OR_B      ( CONST(3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( ESI )
    PUSH_L    ( EDI )
    PUSH_L    ( EBX )
    PUSH_L    ( EBP )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )   /* count                            */
    MOV_L     ( REGOFF(12, EAX), EDI )  /* input stride                     */
    MOV_L     ( REGOFF(4, EAX), EAX )
    MOV_L     ( REGOFF(36, ESP), EBP )  /* clipmask                         */
    MOV_B     ( REGOFF(40, ESP), BL )   /* clip flag                        */

    FEMMS

ALIGNTEXT32

LLBL( G3TPIM_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TPIM_3 ) )      /* skip vertex                        */

    MOVQ      ( REGIND(EAX), MM0 )      /* x1            | x0                 */
    MOVQ      ( MM0, REGIND(EDX) )      /* r1            | r0                 */

LLBL( G3TPIM_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    INC_L     ( EBP )                   /* next clipmask                      */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TPIM_2 ) )      /* cnt > 0 ? -> process next vertex   */

LLBL( G3TPIM_4 ):
    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET




ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_2d_masked)
GLNAME( gl_3dnow_transform_points2_2d_masked ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(2), REGOFF(16, ECX) )
    OR_B      ( CONST(3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( ESI )
    PUSH_L    ( EDI )
    PUSH_L    ( EBX )
    PUSH_L    ( EBP )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )   /* count                            */
    MOV_L     ( REGOFF(12, EAX), EDI )  /* input stride                     */
    MOV_L     ( REGOFF(4, EAX), EAX )
    MOV_L     ( REGOFF(36, ESP), EBP )  /* clipmask                         */
    MOV_B     ( REGOFF(40, ESP), BL )   /* clip flag                        */

    FEMMS

    MOVQ      ( REGIND(ECX), MM0 )      /* m01           | m00                */
    MOVQ      ( REGOFF(16, ECX), MM1 )  /* m11           | m10                */

    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */
    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP2M_4 ) )

ALIGNTEXT32

LLBL( G3TP2M_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TP2M_3 ) )      /* skip vertex                        */

    MOVD      ( REGIND(EAX), MM4 )      /*               | x0                 */
    MOVD      ( REGOFF(4, EAX), MM5 )   /*               | x1                 */

    PUNPCKLDQ ( MM4, MM4 )              /* x0            | x0                 */
    PFMUL     ( MM0, MM4 )              /* x0*m01        | x0*m00             */

    PUNPCKLDQ ( MM5, MM5 )              /* x1            | x1                 */
    PFMUL     ( MM1, MM5 )              /* x1*m11        | x1*m10             */

    PFADD     ( MM5, MM4 )              /* x0*m01+x1*m11 | x0*m00+x1*m10      */
    PFADD     ( MM2, MM4 )              /* x...x1*m11+31 | x0*m00+x1*m10+m30  */

    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */

LLBL( G3TP2M_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    INC_L     ( EBP )                   /* next clipmask                      */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    DEC_L     ( ESI )                   /* decrement vertex counter           */
    JA        ( LLBL( G3TP2M_2 ) )      /* cnt > 0 ? -> process next vertex   */

LLBL( G3TP2M_4 ):
    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET





ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_2d_no_rot_masked)
GLNAME( gl_3dnow_transform_points2_2d_no_rot_masked ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(2), REGOFF(16, ECX) )
    OR_B      ( CONST(3), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( ESI )
    PUSH_L    ( EDI )
    PUSH_L    ( EBX )
    PUSH_L    ( EBP )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )   /* count                            */
    MOV_L     ( REGOFF(12, EAX), EDI )  /* input stride                     */
    MOV_L     ( REGOFF(4, EAX), EAX )
    MOV_L     ( REGOFF(36, ESP), EBP )  /* clipmask                         */
    MOV_B     ( REGOFF(40, ESP), BL )   /* clip flag                        */

    FEMMS

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    MOVD      ( REGOFF(20, ECX), MM1 )  /*               | m11                */

    PSLLQ     ( CONST(32), MM1 )        /* m11           |                    */
    POR       ( MM1, MM0 )              /* m11           | m00                */

    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */
    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP2NRM_4 ) )

ALIGNTEXT32

LLBL( G3TP2NRM_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TP2NRM_3 ) )    /* skip vertex                        */

    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                 */
    PFMUL     ( MM0, MM4 )              /* x1*m11        | x0*m00             */

    PFADD     ( MM2, MM4 )              /* m31           | x0*m00+m30         */
    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */

LLBL( G3TP2NRM_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    INC_L     ( EBP )                   /* next clipmask                      */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TP2NRM_2 )            /* cnt > 0 ? -> process next vertex   */ )

LLBL( G3TP2NRM_4 ):
    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET





ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_3d_masked)
GLNAME( gl_3dnow_transform_points2_3d_masked ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(3), REGOFF(16, ECX) )
    OR_B      ( CONST(7), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( ESI )
    PUSH_L    ( EDI )
    PUSH_L    ( EBX )
    PUSH_L    ( EBP )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )   /* count                            */
    MOV_L     ( REGOFF(12, EAX), EDI )  /* input stride                     */
    MOV_L     ( REGOFF(4, EAX), EAX )
    MOV_L     ( REGOFF(36, ESP), EBP )  /* clipmask                         */
    MOV_B     ( REGOFF(40, ESP), BL )   /* clip flag                        */

    FEMMS

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    MOVD      ( REGOFF(16, ECX), MM7 )  /*               | m10                */
    PSLLQ     ( CONST(32), MM7 )        /* m10           |                    */
    POR       ( MM7, MM0 )              /* m10           | m00                */

    MOVD      ( REGOFF(4, ECX), MM1 )   /*               | m01                */
    MOVD      ( REGOFF(20, ECX), MM7 )  /*               | m11                */
    PSLLQ     ( CONST(32), MM7 )        /* m11           |                    */
    POR       ( MM7, MM1 )              /* m11           | m01                */

    MOVD      ( REGOFF(8, ECX), MM2 )   /*               | m02                */
    MOVD      ( REGOFF(24, ECX), MM7 )  /*               | m12                */
    PSLLQ     ( CONST(32), MM7 )        /* m12           |                    */
    POR       ( MM7, MM2 )              /* m12           | m02                */

    MOVQ      ( REGOFF(48, ECX), MM4 )  /* m31           | m30                */
    MOVD      ( REGOFF(56, ECX), MM5 )  /*               | m32                */

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP3M_4 ) )

ALIGNTEXT32

LLBL( G3TP3M_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TP3M_3 ) )      /* skip vertex                        */

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

LLBL( G3TP3M_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    INC_L     ( EBP )                   /* next clipmask                      */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TP3M_2 ) )      /* cnt > 0 ? -> process next vertex   */

LLBL( G3TP3M_4 ):
    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET




ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_3d_no_rot_masked)
GLNAME( gl_3dnow_transform_points2_3d_no_rot_masked ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(3), REGOFF(16, ECX) )
    OR_B      ( CONST(7), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( ESI )
    PUSH_L    ( EDI )
    PUSH_L    ( EBX )
    PUSH_L    ( EBP )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )   /* count                            */
    MOV_L     ( REGOFF(12, EAX), EDI )  /* input stride                     */
    MOV_L     ( REGOFF(4, EAX), EAX )
    MOV_L     ( REGOFF(36, ESP), EBP )  /* clipmask                         */
    MOV_B     ( REGOFF(40, ESP), BL )   /* clip flag                        */

    FEMMS

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    MOVD      ( REGOFF(20, ECX), MM1 )  /*               | m11                */

    PSLLQ     ( CONST(32), MM1 )        /* m11           |                    */
    POR       ( MM1, MM0 )              /* m11           | m00                */

    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */
    MOVD      ( REGOFF(56, ECX), MM3 )  /*               | m32                */

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP3NRM_4 ) )

ALIGNTEXT32

LLBL( G3TP3NRM_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TP3NRM_3 ) )    /* skip vertex                        */

    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                 */
    PFMUL     ( MM0, MM4 )              /* x1*m11        | x0*m00             */

    PFADD     ( MM2, MM4 )              /* x1*m11+m31    | x0*m00+m30         */
    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */

    MOVD      ( MM3, REGOFF(8, EDX) )   /* write r2                           */

LLBL( G3TP3NRM_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    INC_L     ( EBP )                   /* next clipmask                      */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TP3NRM_2 ) )    /* cnt > 0 ? -> process next vertex   */

LLBL( G3TP3NRM_4 ):

    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET




ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points2_perspective_masked)
GLNAME( gl_3dnow_transform_points2_perspective_masked ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(4), REGOFF(16, ECX) )
    OR_B      ( CONST(15), REGOFF(20, ECX) )
    MOV_L     ( REGOFF(8, EAX), EDX )
    MOV_L     ( EDX, REGOFF(8, ECX) )

ALIGNTEXT32

    PUSH_L    ( ESI )
    PUSH_L    ( EDI )
    PUSH_L    ( EBX )
    PUSH_L    ( EBP )

    MOV_L     ( REGOFF(4, ECX), EDX )
    MOV_L     ( ESI, ECX )
    MOV_L     ( REGOFF(8, EAX), ESI )   /* count                            */
    MOV_L     ( REGOFF(12, EAX), EDI )  /* input stride                     */
    MOV_L     ( REGOFF(4, EAX), EAX )
    MOV_L     ( REGOFF(36, ESP), EBP )  /* clipmask                         */
    MOV_B     ( REGOFF(40, ESP), BL )   /* clip flag                        */

    FEMMS

    MOVD      ( REGIND(ECX), MM0 )      /*               | m00                */
    MOVD      ( REGOFF(20, ECX), MM1 )  /*               | m11                */

    PSLLQ     ( CONST(32), MM1 )        /* m11           |                    */
    POR       ( MM1, MM0 )              /* m11           | m00                */

    MOVD      ( REGOFF(56, ECX), MM3 )  /*               | m32                */
    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TPPM_4 ) )

ALIGNTEXT32

LLBL( G3TPPM_2 ):

    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TPPM_3 ) )      /* skip vertex                        */

    MOVQ      ( REGIND(EAX), MM4 )      /* x1            | x0                 */
    PFMUL     ( MM0, MM4 )              /* x1*m11        | x0*m00             */

    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */
    MOVQ      ( MM3, REGOFF(8, EDX) )   /* write r2  (=m32), r3 (=0)          */

LLBL( G3TPPM_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    INC_L     ( EBP )                   /* next clipmask                      */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TPPM_2 ) )      /* cnt > 0 ? -> process next vertex   */

LLBL( G3TPPM_4 ):
    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET




