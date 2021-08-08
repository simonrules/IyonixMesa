#include "assyntax.h"

   SEG_TEXT


#if !defined(NASM_ASSEMBLER) && !defined(MASM_ASSEMBLER)
#define LLBL(a) .L ## a
#else
#define LLBL(a) a
#endif





ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points1_general_masked)
GLNAME( gl_3dnow_transform_points1_general_masked ):

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

    MOVQ      ( REGIND(ECX), MM0 )      /* m01           | m00                */
    MOVQ      ( REGOFF(8, ECX), MM1 )   /* m03           | m02                */

    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */
    MOVQ      ( REGOFF(56, ECX), MM3 )  /* m33           | m32                */

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TPGM_4 ) )


ALIGNTEXT32
LLBL( G3TPGM_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TPGM_3 ) )      /* skip vertex                        */

    MOVD      ( REGIND(EAX), MM4 )      /*               | x0                 */
    PUNPCKLDQ ( MM4, MM4 )              /* x0            | x0                 */

    MOVQ      ( MM4, MM5 )              /* x0            | x0                 */
    PFMUL     ( MM0, MM4 )              /* x0*m01        | x0*m00             */

    PFMUL     ( MM1, MM5 )              /* x0*m03        | x0*m02             */
    PFADD     ( MM2, MM4 )              /* x0*m01+m31    | x0*m00+m30         */

    PFADD     ( MM3, MM5 )              /* x0*m03+m33    | x0*m02+m32         */
    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */

    MOVQ      ( MM5, REGOFF(8, EDX) )   /* write r3, r2                       */

LLBL( G3TPGM_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    INC_L     ( EBP )                   /* next clipmask                      */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

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
GLOBL GLNAME(gl_3dnow_transform_points1_identity_masked)
GLNAME( gl_3dnow_transform_points1_identity_masked ):

    PUSH_L    ( ESI )
    MOV_L     ( REGOFF(8, ESP), ECX )
    MOV_L     ( REGOFF(12, ESP), ESI )
    MOV_L     ( REGOFF(16, ESP), EAX )
    MOV_L     ( CONST(1), REGOFF(16, ECX) )
    OR_B      ( CONST(1), REGOFF(20, ECX) )
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
    JNZ       ( LLBL( G3TPIM_3 )              /* skip vertex                        */ )

    MOVD      ( REGIND(EAX), MM0 )      /*               | x0                 */
    MOVD      ( MM0, REGIND(EDX) )      /*               | r0                 */

LLBL( G3TPIM_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    INC_L     ( EBP )                   /* next clipmask                      */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TPIM_2 )              /* cnt > 0 ? -> process next vertex   */ )

LLBL( G3TPIM_4 ):
    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET




ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points1_2d_masked)
GLNAME( gl_3dnow_transform_points1_2d_masked ):

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
    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP2M_4 ) )


ALIGNTEXT32

LLBL( G3TP2M_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TP2M_3 ) )      /* skip vertex                        */

    MOVD      ( REGIND(EAX), MM4 )      /*               | x0                 */
    PUNPCKLDQ ( MM4, MM4 )              /* x0            | x0                 */

    PFMUL     ( MM0, MM4 )              /* x0*m01        | x0*m00             */
    PFADD     ( MM2, MM4 )              /* x0*m01+m31    | x0*m00+m30         */

    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */

LLBL( G3TP2M_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    ADD_L     ( CONST(16), EDX )        /* next r                             */
    INC_L     ( EBP )                   /* next clipmask                      */

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
GLOBL GLNAME(gl_3dnow_transform_points1_2d_no_rot_masked)
GLNAME( gl_3dnow_transform_points1_2d_no_rot_masked ):

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
    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP2NRM_4 ) )


ALIGNTEXT32

LLBL( G3TP2NRM_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TP2NRM_3 )            /* skip vertex                        */ )

    MOVD      ( REGIND(EAX), MM4 )      /*               | x0                 */
    PFMUL     ( MM0, MM4 )              /*               | x0*m00             */

    PFADD     ( MM2, MM4 )              /* m31           | x0*m00+m30         */
    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */

LLBL( G3TP2NRM_3 ):
    ADD_L     ( CONST(16), EDX )        /* next r                             */
    ADD_L     ( EDI, EAX )              /* next vertex                        */

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
GLOBL GLNAME(gl_3dnow_transform_points1_3d_masked)
GLNAME( gl_3dnow_transform_points1_3d_masked ):

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

    MOVQ      ( REGIND(ECX), MM0 )      /* m01           | m00                */
    MOVD      ( REGOFF(8, ECX), MM1 )   /*               | m02                */

    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */
    MOVD      ( REGOFF(56, ECX), MM3 )  /*               | m32                */

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP3M_4 ) )

ALIGNTEXT32

LLBL( G3TP3M_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TP3M_3 )              /* skip vertex                        */ )

    MOVD      ( REGIND(EAX), MM4 )      /*               | x0                 */
    PUNPCKLDQ ( MM4, MM4 )              /* x0            | x0                 */

    MOVQ      ( MM4, MM5 )              /*               | x0                 */
    PFMUL     ( MM0, MM4 )              /* x0*m01        | x0*m00             */

    PFMUL     ( MM1, MM5 )              /*               | x0*m02             */
    PFADD     ( MM2, MM4 )              /* x0*m01+m31    | x0*m00+m30         */

    PFADD     ( MM3, MM5 )              /*               | x0*m02+m32         */
    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */

    MOVD      ( MM5, REGOFF(8, EDX) )   /* write r2                           */

LLBL( G3TP3M_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    ADD_L     ( CONST(16), EDX )        /* next r                             */
    INC_L     ( EBP )                   /* next clipmask                      */

    DEC_L     ( ESI )                   /* decrement vertex counter           */
    JA        ( LLBL( G3TP3M_2 )              /* cnt > 0 ? -> process next vertex   */ )

LLBL( G3TP3M_4 ):
    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET




ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points1_3d_no_rot_masked)
GLNAME( gl_3dnow_transform_points1_3d_no_rot_masked ):

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
    MOVQ      ( REGOFF(48, ECX), MM2 )  /* m31           | m30                */

    MOVD      ( REGOFF(56, ECX), MM3 )  /*               | m32                */
    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TP3NRM_4 ) )

ALIGNTEXT32

LLBL( G3TP3NRM_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TP3NRM_3 )            /* skip vertex                        */ )

    MOVD      ( REGIND(EAX), MM4 )      /*               | x0                 */
    PFMUL     ( MM0, MM4 )              /*               | x0*m00             */

    PFADD     ( MM2, MM4 )              /* m31           | x0*m00+m30         */
    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */

    MOVD      ( MM3, REGOFF(8, EDX) )   /* write r2                           */

LLBL( G3TP3NRM_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */

    ADD_L     ( CONST(16), EDX )        /* next r                             */
    INC_L     ( EBP )                   /* next clipmask                      */

    DEC_L     ( ESI )                   /* decrement vertex counter           */
    JA        ( LLBL( G3TP3NRM_2 )            /* cnt > 0 ? -> process next vertex   */ )

LLBL( G3TP3NRM_4 ):
    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET




ALIGNTEXT16
GLOBL GLNAME(gl_3dnow_transform_points1_perspective_masked)
GLNAME( gl_3dnow_transform_points1_perspective_masked ):

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
    MOVD      ( REGOFF(56, ECX), MM3 )  /*               | m32                */

    CMP_L     ( CONST(0), ESI )
    JE        ( LLBL( G3TPPM_4 ) )

ALIGNTEXT32

LLBL( G3TPPM_2 ):
    TEST_B    ( BL, REGIND(EBP) )       /* mask [i] != clip flag ??           */
    JNZ       ( LLBL( G3TPPM_3 )              /* skip vertex                        */ )

    MOVD      ( REGIND(EAX), MM4 )      /* 0             | x0                 */
    PFMUL     ( MM0, MM4 )              /* 0             | x0*m00             */

    MOVQ      ( MM4, REGIND(EDX) )      /* write r1, r0                       */
    MOVQ      ( MM3, REGOFF(8, EDX) )   /* write r2  (=m32), r3 (=0)          */

LLBL( G3TPPM_3 ):
    ADD_L     ( EDI, EAX )              /* next vertex                        */
    ADD_L     ( CONST(16), EDX )        /* next r                             */

    INC_L     ( EBP )                   /* next clipmask                      */
    DEC_L     ( ESI )                   /* decrement vertex counter           */

    JA        ( LLBL( G3TPPM_2 )              /* cnt > 0 ? -> process next vertex   */ )

LLBL( G3TPPM_4 ):
    FEMMS

    POP_L     ( EBP )
    POP_L     ( EBX )
    POP_L     ( EDI )
    POP_L     ( ESI )

    POP_L     ( ESI )
    RET







