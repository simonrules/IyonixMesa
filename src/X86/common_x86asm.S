
/*
 * Mesa 3-D graphics library
 * Version:  3.1
 *
 * Copyright (C) 1999  Brian Paul   All Rights Reserved.
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

/*
 *  Check extended CPU capabilities.
 *
 *  Written by Holger Waechtler <holger@akaflieg.extern.tu-berlin.de>
 */




#include "assyntax.h"

        SEG_TEXT

#if !defined(NASM_ASSEMBLER) && !defined(MASM_ASSEMBLER)
#define LLBL(a) .L ## a
#else
#define LLBL(a) a
#endif


#define  GL_CPU_GenuineIntel     CONST(1)
#define  GL_CPU_MMX              CONST(2)
#define  GL_CPU_3Dnow            CONST(4)



GLOBL GLNAME(gl_identify_x86_cpu_features)
    ALIGNTEXT4
GLNAME(gl_identify_x86_cpu_features):

    PUSH_L  ( EBX )
    PUSH_L  ( ESI )
    XOR_L   ( ESI, ESI )

    PUSHF_L                             /*  test for the cpuid command        */
    POP_L   ( EAX )                     /*   (if the ID-Bit (Bit 21) in       */
    MOV_L   ( EAX, EBX )                /*    EEFLAGS is writable, the        */
    XOR_L   ( CONST(0x00200000), EAX )  /*    cpuid command is present)       */
    PUSH_L  ( EAX )
    POPF_L
    PUSHF_L
    POP_L   ( EAX )
    CMP_L   ( EBX, EAX )
    JZ      ( LLBL ( Gidcpu_end ) )   /*  cpuid command not supported       */

    XOR_L   ( EAX, EAX )
    CPUID


LLBL (Gidcpu_intel):
    CMP_L  ( CONST(0x756e6547), EBX )  /*  compare                           */
    JNE    ( LLBL ( Gidcpu_amd ) )     /*  vendor string == 'GenuineIntel' ? */
    CMP_L  ( CONST(0x49656e69), EDX )
    JNE    ( LLBL ( Gidcpu_amd ) )
    CMP_L  ( CONST(0x6c65746e), ECX )
    JNE    ( LLBL ( Gidcpu_amd ) )
    OR_L   ( GL_CPU_GenuineIntel, ESI )
    JMP    ( LLBL ( Gidcpu_mmx ) )


LLBL ( Gidcpu_amd ):
    CMP_L  ( CONST(0x68747541), EBX )  /*  compare vendor string == 'Auth'?  */
    JNE    ( LLBL ( Gidcpu_other ) )
    CMP_L  ( CONST(0x69746e65), EDX )  /*  'enti'                            */
    JNE    ( LLBL ( Gidcpu_other ) )
    CMP_L  ( CONST(0x444d4163), ECX )  /*  'cAMD'   (= 'AuthenticAMD')       */
    JE     ( LLBL ( Gidcpu_3dnow ) )


LLBL ( Gidcpu_other ):

/*   insert here other vendorstrings with 3dnow capable cpu vendors          */

JMP    ( LLBL ( Gidcpu_mmx ) )


LLBL ( Gidcpu_3dnow ): 
    MOV_L   ( CONST(0x80000000), EAX )  /*  extended functions supported ?    */

    CPUID
    TEST_L  ( EAX, EAX )
    JZ      ( LLBL ( Gidcpu_end ) )

    MOV_L   ( CONST(0x80000001), EAX )
    CPUID                              /*  test for 3Dnow!                   */
    TEST_L  ( CONST(0x80000000), EDX )
    JZ      ( LLBL ( Gidcpu_mmx ) )
    OR_L    ( GL_CPU_3Dnow, ESI )      /*  (detected 3Dnow! extension)       */

LLBL ( Gidcpu_mmx ):
    MOV_L   ( CONST(0x1), EAX )
    CPUID
    TEST_L  ( CONST(0x00800000), EDX ) /*  Bit 23 set ?                      */
    JZ      ( LLBL ( Gidcpu_end ) )
    OR_L    ( GL_CPU_MMX, ESI )        /*  (detected mmx extension)          */


LLBL ( Gidcpu_end ):
    MOV_L   ( ESI, EAX )
    STC
    POP_L   ( ESI )
    POP_L   ( EBX )

    RET

