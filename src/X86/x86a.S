#include "assyntax.h"

	SEG_TEXT

#define FP_ONE 1065353216
#define FP_ZERO 0

#if !defined(NASM_ASSEMBLER) && !defined(MASM_ASSEMBLER)
#define LLBL(a) .L ## a
#else
#define LLBL(a) a
#endif

#define S(i) 	REGOFF(i * 4, ESI)
#define D(i) 	REGOFF(i * 4, EDI)
#define M(i) 	REGOFF(i * 4, EDX)

/*
 * 	Offsets into GLvector4f
 */
#define V4F_DATA 	0
#define V4F_START 	4
#define V4F_COUNT 	8
#define V4F_STRIDE 	12
#define V4F_SIZE 	16
#define V4F_FLAGS 	20

#define VEC_SIZE_1   1
#define VEC_SIZE_2   3
#define VEC_SIZE_3   7
#define VEC_SIZE_4   15

/*
 *	Offsets for transform_func arguments
 *
 *	typedef void (*transform_func)( GLvector4f *to_vec, 
 *	                                const GLmatrix *mat, 
 *	                                const GLvector4f *from_vec, 
 *	                                const GLubyte *clipmask,
 *	                                const GLubyte flag );
 */
#define OFFSET_DEST 4
#define OFFSET_MATRIX 8
#define OFFSET_SOURCE 12
#define OFFSET_CLIP 16
#define OFFSET_FLAG 20

#define ARG_DEST 	REGOFF(FRAME_OFFSET+OFFSET_DEST, ESP)
#define ARG_MATRIX 	REGOFF(FRAME_OFFSET+OFFSET_MATRIX, ESP)
#define ARG_SOURCE 	REGOFF(FRAME_OFFSET+OFFSET_SOURCE, ESP)
#define ARG_CLIP 	REGOFF(FRAME_OFFSET+OFFSET_CLIP, ESP)
#define ARG_FLAG 	REGOFF(FRAME_OFFSET+OFFSET_FLAG, ESP)

/*
########################################
##
## Masked versions
##
########################################
*/

/*
########################################
##
## gl_x86_transform_points2_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_general_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_general_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p2mgm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */



	ALIGNTEXT4ifNOP
LLBL(p2mgm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p2mgm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FADD_S( M(12) )
	FXCH( ST(2) ) 	/* F5 F6 F4 F7 */
	FADD_S( M(13) )
	FXCH( ST(1) ) 	/* F6 F5 F4 F7 */
	FADD_S( M(14) )
	FXCH( ST(3) ) 	/* F7 F5 F4 F6 */
	FADD_S( M(15) )

	FXCH( ST(2) ) 	/* F4 F5 F7 F6 */
	FSTP_S( D(0)   ) 	/* F5 F7 F6 */
	FSTP_S( D(1)   ) 	/* F7 F6 */
	FXCH( ST(1) ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p2mgm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2mgm_top) )


LLBL(p2mgm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_identity_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_identity_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p2mim_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_2), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(2), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */


	CMP_L( ESI, EDI )
	JE( LLBL(p2mim_finish) )

	ALIGNTEXT4ifNOP
LLBL(p2mim_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p2mim_skip) )

	MOV_L( S(0), EAX )
	MOV_L( S(1), EDX )

	MOV_L( EAX, D(0) )
	MOV_L( EDX, D(1) )
LLBL(p2mim_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2mim_top) )


LLBL(p2mim_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_2d_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_2d_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p2m2dm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_2), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(2), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */



	ALIGNTEXT4ifNOP
LLBL(p2m2dm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p2m2dm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FXCH( ST(1) ) 	/* F4 F5 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 */
	FADD_S( M(13) )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */

LLBL(p2m2dm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2m2dm_top) )


LLBL(p2m2dm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_2d_no_rot_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_2d_no_rot_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p2m2dnrm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_2), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(2), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */



	ALIGNTEXT4ifNOP
LLBL(p2m2dnrm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p2m2dnrm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */
	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */

LLBL(p2m2dnrm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2m2dnrm_top) )


LLBL(p2m2dnrm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_3d_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_3d_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p2m3dm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */



	ALIGNTEXT4ifNOP
LLBL(p2m3dm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p2m3dm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 F6 */
	FADD_S( M(13) )
	FXCH( ST(2) ) 	/* F6 F4 F5 */
	FADD_S( M(14) )

	FXCH( ST(1) ) 	/* F4 F6 F5 */
	FSTP_S( D(0)   ) 	/* F6 F5 */
	FXCH( ST(1) ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

LLBL(p2m3dm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2m3dm_top) )


LLBL(p2m3dm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_3d_no_rot_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_3d_no_rot_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p2m3dnrm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */


	MOV_L( M(14), EAX )
	ALIGNTEXT4ifNOP
LLBL(p2m3dnrm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p2m3dnrm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */
	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EAX, D(2) )

LLBL(p2m3dnrm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2m3dnrm_top) )


LLBL(p2m3dnrm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_perspective_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_perspective_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p2mpm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */


	MOV_L( M(14), EAX )
	ALIGNTEXT4ifNOP
LLBL(p2mpm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p2mpm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FSTP_S( D(0)   ) 	/* F1 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EAX, D(2) )
	MOV_L( CONST(FP_ZERO), D(3) )

LLBL(p2mpm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2mpm_top) )


LLBL(p2mpm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_general_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_general_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p3mgm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */



	ALIGNTEXT4ifNOP
LLBL(p3mgm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p3mgm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(10) )
	FLD_S( S(2) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(11) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FADD_S( M(12) )
	FXCH( ST(2) ) 	/* F5 F6 F4 F7 */
	FADD_S( M(13) )
	FXCH( ST(1) ) 	/* F6 F5 F4 F7 */
	FADD_S( M(14) )
	FXCH( ST(3) ) 	/* F7 F5 F4 F6 */
	FADD_S( M(15) )

	FXCH( ST(2) ) 	/* F4 F5 F7 F6 */
	FSTP_S( D(0)   ) 	/* F5 F7 F6 */
	FSTP_S( D(1)   ) 	/* F7 F6 */
	FXCH( ST(1) ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p3mgm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3mgm_top) )


LLBL(p3mgm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_identity_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_identity_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p3mim_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */
	MOV_L( ECX, ARG_DEST ) 			/* need ecx; put dest+count in ARG_DEST */


	CMP_L( ESI, EDI )
	JE( LLBL(p3mim_finish) )

	ALIGNTEXT4ifNOP
LLBL(p3mim_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p3mim_skip) )

	MOV_L( S(0), EAX )
	MOV_L( S(1), ECX )
	MOV_L( S(2), EDX )

	MOV_L( EAX, D(0) )
	MOV_L( ECX, D(1) )
	MOV_L( EDX, D(2) )
LLBL(p3mim_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ARG_DEST, EDI )
	JNE( LLBL(p3mim_top) )


LLBL(p3mim_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_2d_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_2d_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p3m2dm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */


	ALIGNTEXT4ifNOP
LLBL(p3m2dm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p3m2dm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FXCH( ST(1) ) 	/* F4 F5 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 */
	FADD_S( M(13) )

	MOV_L( S(2), EAX )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EAX, D(2) )

LLBL(p3m2dm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3m2dm_top) )


LLBL(p3m2dm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_2d_no_rot_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_2d_no_rot_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p3m2dnrm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */


	ALIGNTEXT4ifNOP
LLBL(p3m2dnrm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p3m2dnrm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */

	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	MOV_L( S(2), EAX )

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EAX, D(2) )

LLBL(p3m2dnrm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3m2dnrm_top) )


LLBL(p3m2dnrm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_3d_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_3d_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p3m3dm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */



	ALIGNTEXT4ifNOP
LLBL(p3m3dm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p3m3dm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 F6 */
	FADD_S( M(13) )
	FXCH( ST(2) ) 	/* F6 F4 F5 */
	FADD_S( M(14) )

	FXCH( ST(1) ) 	/* F4 F6 F5 */
	FSTP_S( D(0)   ) 	/* F6 F5 */
	FXCH( ST(1) ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

LLBL(p3m3dm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3m3dm_top) )


LLBL(p3m3dm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_3d_no_rot_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_3d_no_rot_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p3m3dnrm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */



	ALIGNTEXT4ifNOP
LLBL(p3m3dnrm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p3m3dnrm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F2 F1 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F4 F1 F2 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 F2 */
	FXCH( ST(2) ) 	/* F1 F4 F5 F2 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 F2 */
	FLD_S( M(14) ) 	/* F6 F4 F5 F2 */
	FXCH( ST(3) ) 	/* F2 F4 F5 F6 */
	FADDP( ST(0), ST(3) ) 	/* F4 F5 F6 */

	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

LLBL(p3m3dnrm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3m3dnrm_top) )


LLBL(p3m3dnrm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_perspective_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_perspective_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p3mpm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */


	ALIGNTEXT4ifNOP
LLBL(p3mpm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p3mpm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F0 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F5 F4 */
	FADDP( ST(0), ST(4) ) 	/* F1 F2 F5 F4 */
	FADDP( ST(0), ST(2) ) 	/* F2 F5 F4 */
	FLD_S( M(14) ) 	/* F6 F2 F5 F4 */
	FXCH( ST(1) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(2), EAX )
	XOR_L( CONST(-2147483648), EAX ) 	/* change sign */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EAX, D(3) )

LLBL(p3mpm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3mpm_top) )


LLBL(p3mpm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_general_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_general_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p4mgm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */



	ALIGNTEXT4ifNOP
LLBL(p4mgm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p4mgm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(10) )
	FLD_S( S(2) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(11) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(3) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(14) )
	FLD_S( S(3) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(15) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FSTP_S( D(0)   ) 	/* F6 F5 F7 */
	FXCH( ST(1) ) 	/* F5 F6 F7 */
	FSTP_S( D(1)   ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p4mgm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4mgm_top) )


LLBL(p4mgm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_identity_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_identity_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p4mim_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */


	CMP_L( ESI, EDI )
	JE( LLBL(p4mim_finish) )

	ALIGNTEXT4ifNOP
LLBL(p4mim_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p4mim_skip) )

	MOV_L( S(0), EAX )
	MOV_L( S(1), EDX )

	MOV_L( EAX, D(0) )
	MOV_L( EDX, D(1) )

	MOV_L( S(2), EAX )
	MOV_L( S(3), EDX )

	MOV_L( EAX, D(2) )
	MOV_L( EDX, D(3) )
LLBL(p4mim_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4mim_top) )


LLBL(p4mim_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_2d_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_2d_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p4m2dm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */
	MOV_L( ECX, ARG_DEST ) 			/* need ecx; put dest+count in ARG_DEST */


	ALIGNTEXT4ifNOP
LLBL(p4m2dm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p4m2dm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FLD_S( S(3) ) 	/* F0 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(13) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	MOV_L( S(2), EAX )
	MOV_L( S(3), ECX )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EAX, D(2) )
	MOV_L( ECX, D(3) )

LLBL(p4m2dm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ARG_DEST, EDI )
	JNE( LLBL(p4m2dm_top) )


LLBL(p4m2dm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_2d_no_rot_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_2d_no_rot_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p4m2dnrm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */
	MOV_L( ECX, ARG_DEST ) 			/* need ecx; put dest+count in ARG_DEST */


	ALIGNTEXT4ifNOP
LLBL(p4m2dnrm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p4m2dnrm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(3) ) 	/* F0 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(13) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	MOV_L( S(2), EAX )
	MOV_L( S(3), ECX )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EAX, D(2) )
	MOV_L( ECX, D(3) )

LLBL(p4m2dnrm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ARG_DEST, EDI )
	JNE( LLBL(p4m2dnrm_top) )


LLBL(p4m2dnrm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_3d_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_3d_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p4m3dm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */


	ALIGNTEXT4ifNOP
LLBL(p4m3dm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p4m3dm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(3) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(14) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(3), EAX )

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EAX, D(3) )

LLBL(p4m3dm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4m3dm_top) )


LLBL(p4m3dm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_3d_no_rot_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_3d_no_rot_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p4m3dnrm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */


	ALIGNTEXT4ifNOP
LLBL(p4m3dnrm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p4m3dnrm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F6 F5 F4 */
	FMUL_S( M(10) )

	FLD_S( S(3) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(14) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(3), EAX )

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EAX, D(3) )

LLBL(p4m3dnrm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4m3dnrm_top) )


LLBL(p4m3dnrm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_perspective_masked)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_perspective_masked):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 		/* ptr to source GLvector4f */
	MOV_L( ARG_DEST, EDI ) 			/* ptr to dest GLvector4f */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_CLIP, EBP ) 			/* ptr to clip mask array */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_B( ARG_FLAG, BL ) 			/* clip mask flags */

	TEST_L( ECX, ECX )
	JZ( LLBL(p4mpm_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */


	ALIGNTEXT4ifNOP
LLBL(p4mpm_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(p4mpm_skip) )


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F0 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F6 F1 F0 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F6 F5 F4 */
	FADDP( ST(0), ST(4) ) 	/* F1 F6 F5 F4 */
	FADDP( ST(0), ST(2) ) 	/* F6 F5 F4 */

	FLD_S( S(3) ) 	/* F2 F6 F5 F4 */
	FMUL_S( M(14) )

	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(2), EAX )
	XOR_L( CONST(-2147483648), EAX ) 	/* change sign */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EAX, D(3) )

LLBL(p4mpm_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( ARG_SOURCE, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4mpm_top) )


LLBL(p4mpm_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET



/*
########################################
##
## Unmasked versions
##
########################################
*/

/*
########################################
##
## gl_x86_transform_points2_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_general_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_general_raw):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */


	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2mgr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p2mgr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FADD_S( M(12) )
	FXCH( ST(2) ) 	/* F5 F6 F4 F7 */
	FADD_S( M(13) )
	FXCH( ST(1) ) 	/* F6 F5 F4 F7 */
	FADD_S( M(14) )
	FXCH( ST(3) ) 	/* F7 F5 F4 F6 */
	FADD_S( M(15) )

	FXCH( ST(2) ) 	/* F4 F5 F7 F6 */
	FSTP_S( D(0)   ) 	/* F5 F7 F6 */
	FSTP_S( D(1)   ) 	/* F7 F6 */
	FXCH( ST(1) ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p2mgr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2mgr_top) )


LLBL(p2mgr_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_identity_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_identity_raw):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2mir_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_2), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(2), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	CMP_L( ESI, EDI )
	JE( LLBL(p2mir_finish) )

	ALIGNTEXT4ifNOP
LLBL(p2mir_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EDX, D(1) )
LLBL(p2mir_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2mir_top) )


LLBL(p2mir_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_2d_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_2d_raw):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */


	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m2dr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_2), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(2), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p2m2dr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FXCH( ST(1) ) 	/* F4 F5 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 */
	FADD_S( M(13) )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */

LLBL(p2m2dr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2m2dr_top) )


LLBL(p2m2dr_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_2d_no_rot_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_2d_no_rot_raw):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */


	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m2dnrr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_2), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(2), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p2m2dnrr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */
	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */

LLBL(p2m2dnrr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2m2dnrr_top) )


LLBL(p2m2dnrr_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_3d_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_3d_raw):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */


	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m3dr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p2m3dr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 F6 */
	FADD_S( M(13) )
	FXCH( ST(2) ) 	/* F6 F4 F5 */
	FADD_S( M(14) )

	FXCH( ST(1) ) 	/* F4 F6 F5 */
	FSTP_S( D(0)   ) 	/* F6 F5 */
	FXCH( ST(1) ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

LLBL(p2m3dr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2m3dr_top) )


LLBL(p2m3dr_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_3d_no_rot_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_3d_no_rot_raw):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m3dnrr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	MOV_L( M(14), EBX )
	ALIGNTEXT4ifNOP
LLBL(p2m3dnrr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */
	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

LLBL(p2m3dnrr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2m3dnrr_top) )


LLBL(p2m3dnrr_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_perspective_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_perspective_raw):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2mpr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	MOV_L( M(14), EBX )
	ALIGNTEXT4ifNOP
LLBL(p2mpr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FSTP_S( D(0)   ) 	/* F1 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( CONST(FP_ZERO), D(3) )

LLBL(p2mpr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p2mpr_top) )


LLBL(p2mpr_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_general_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_general_raw):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */


	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3mgr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p3mgr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(10) )
	FLD_S( S(2) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(11) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FADD_S( M(12) )
	FXCH( ST(2) ) 	/* F5 F6 F4 F7 */
	FADD_S( M(13) )
	FXCH( ST(1) ) 	/* F6 F5 F4 F7 */
	FADD_S( M(14) )
	FXCH( ST(3) ) 	/* F7 F5 F4 F6 */
	FADD_S( M(15) )

	FXCH( ST(2) ) 	/* F4 F5 F7 F6 */
	FSTP_S( D(0)   ) 	/* F5 F7 F6 */
	FSTP_S( D(1)   ) 	/* F7 F6 */
	FXCH( ST(1) ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p3mgr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3mgr_top) )


LLBL(p3mgr_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_identity_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_identity_raw):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3mir_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	CMP_L( ESI, EDI )
	JE( LLBL(p3mir_finish) )

	ALIGNTEXT4ifNOP
LLBL(p3mir_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EBP )
	MOV_L( S(2), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EBP, D(1) )
	MOV_L( EDX, D(2) )
LLBL(p3mir_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3mir_top) )


LLBL(p3mir_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_2d_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_2d_raw):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m2dr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p3m2dr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FXCH( ST(1) ) 	/* F4 F5 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 */
	FADD_S( M(13) )

	MOV_L( S(2), EBX )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

LLBL(p3m2dr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3m2dr_top) )


LLBL(p3m2dr_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_2d_no_rot_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_2d_no_rot_raw):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m2dnrr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p3m2dnrr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */

	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	MOV_L( S(2), EBX )

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

LLBL(p3m2dnrr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3m2dnrr_top) )


LLBL(p3m2dnrr_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_3d_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_3d_raw):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */


	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m3dr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p3m3dr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 F6 */
	FADD_S( M(13) )
	FXCH( ST(2) ) 	/* F6 F4 F5 */
	FADD_S( M(14) )

	FXCH( ST(1) ) 	/* F4 F6 F5 */
	FSTP_S( D(0)   ) 	/* F6 F5 */
	FXCH( ST(1) ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

LLBL(p3m3dr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3m3dr_top) )


LLBL(p3m3dr_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_3d_no_rot_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_3d_no_rot_raw):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */


	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m3dnrr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_3), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p3m3dnrr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F2 F1 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F4 F1 F2 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 F2 */
	FXCH( ST(2) ) 	/* F1 F4 F5 F2 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 F2 */
	FLD_S( M(14) ) 	/* F6 F4 F5 F2 */
	FXCH( ST(3) ) 	/* F2 F4 F5 F6 */
	FADDP( ST(0), ST(3) ) 	/* F4 F5 F6 */

	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

LLBL(p3m3dnrr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3m3dnrr_top) )


LLBL(p3m3dnrr_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_perspective_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_perspective_raw):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3mpr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p3mpr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F0 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F5 F4 */
	FADDP( ST(0), ST(4) ) 	/* F1 F2 F5 F4 */
	FADDP( ST(0), ST(2) ) 	/* F2 F5 F4 */
	FLD_S( M(14) ) 	/* F6 F2 F5 F4 */
	FXCH( ST(1) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(2), EBX )
	XOR_L( CONST(-2147483648), EBX ) 	/* change sign */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p3mpr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p3mpr_top) )


LLBL(p3mpr_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_general_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_general_raw):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */


	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4mgr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p4mgr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(10) )
	FLD_S( S(2) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(11) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(3) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(14) )
	FLD_S( S(3) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(15) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FSTP_S( D(0)   ) 	/* F6 F5 F7 */
	FXCH( ST(1) ) 	/* F5 F6 F7 */
	FSTP_S( D(1)   ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p4mgr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4mgr_top) )


LLBL(p4mgr_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_identity_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_identity_raw):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4mir_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	CMP_L( ESI, EDI )
	JE( LLBL(p4mir_finish) )

	ALIGNTEXT4ifNOP
LLBL(p4mir_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EDX, D(1) )

	MOV_L( S(2), EBX )
	MOV_L( S(3), EDX )

	MOV_L( EBX, D(2) )
	MOV_L( EDX, D(3) )
LLBL(p4mir_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4mir_top) )


LLBL(p4mir_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_2d_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_2d_raw):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m2dr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p4m2dr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FLD_S( S(3) ) 	/* F0 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(13) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	MOV_L( S(2), EBX )
	MOV_L( S(3), EBP )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( EBP, D(3) )

LLBL(p4m2dr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4m2dr_top) )


LLBL(p4m2dr_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_2d_no_rot_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_2d_no_rot_raw):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m2dnrr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p4m2dnrr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(3) ) 	/* F0 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(13) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	MOV_L( S(2), EBX )
	MOV_L( S(3), EBP )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( EBP, D(3) )

LLBL(p4m2dnrr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4m2dnrr_top) )


LLBL(p4m2dnrr_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_3d_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_3d_raw):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m3dr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p4m3dr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(3) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(14) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(3), EBX )

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4m3dr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4m3dr_top) )


LLBL(p4m3dr_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_3d_no_rot_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_3d_no_rot_raw):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m3dnrr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p4m3dnrr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F6 F5 F4 */
	FMUL_S( M(10) )

	FLD_S( S(3) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(14) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(3), EBX )

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4m3dnrr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4m3dnrr_top) )


LLBL(p4m3dnrr_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_perspective_raw)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_perspective_raw):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

	PUSH_L( EBX )

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4mpr_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(4), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */


	ALIGNTEXT4ifNOP
LLBL(p4mpr_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F0 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F6 F1 F0 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F6 F5 F4 */
	FADDP( ST(0), ST(4) ) 	/* F1 F6 F5 F4 */
	FADDP( ST(0), ST(2) ) 	/* F6 F5 F4 */

	FLD_S( S(3) ) 	/* F2 F6 F5 F4 */
	FMUL_S( M(14) )

	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(2), EBX )
	XOR_L( CONST(-2147483648), EBX ) 	/* change sign */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4mpr_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(p4mpr_top) )


LLBL(p4mpr_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET



#undef OFFSET_DEST
#undef OFFSET_MATRIX
#undef OFFSET_SOURCE
#undef OFFSET_CLIP
#undef OFFSET_FLAG

#undef ARG_DEST
#undef ARG_MATRIX
#undef ARG_SOURCE
#undef ARG_CLIP
#undef ARG_FLAG

#define OFFSET_DEST 4
#define OFFSET_MATRIX 8
#define OFFSET_SOURCE 12
#define OFFSET_STRIDE 16
#define OFFSET_COUNT 20

#define ARG_DEST 	REGOFF(FRAME_OFFSET+OFFSET_DEST, ESP)
#define ARG_MATRIX 	REGOFF(FRAME_OFFSET+OFFSET_MATRIX, ESP)
#define ARG_SOURCE 	REGOFF(FRAME_OFFSET+OFFSET_SOURCE, ESP)
#define ARG_STRIDE 	REGOFF(FRAME_OFFSET+OFFSET_STRIDE, ESP)
#define ARG_COUNT 	REGOFF(FRAME_OFFSET+OFFSET_COUNT, ESP)

/*
########################################
##
## clean (no vertex struct) versions
##
########################################
*/


/*
########################################
##
## gl_x86_transform_points2_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_general_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_general_v8):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2mgv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2mgv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FADD_S( M(12) )
	FXCH( ST(2) ) 	/* F5 F6 F4 F7 */
	FADD_S( M(13) )
	FXCH( ST(1) ) 	/* F6 F5 F4 F7 */
	FADD_S( M(14) )
	FXCH( ST(3) ) 	/* F7 F5 F4 F6 */
	FADD_S( M(15) )

	FXCH( ST(2) ) 	/* F4 F5 F7 F6 */
	FSTP_S( D(0)   ) 	/* F5 F7 F6 */
	FSTP_S( D(1)   ) 	/* F7 F6 */
	FXCH( ST(1) ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p2mgv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2mgv8_top) )


LLBL(p2mgv8_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_identity_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_identity_v8):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2miv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	CMP_L( ESI, EDI )
	JE( LLBL(p2miv8_finish) )

	ALIGNTEXT4ifNOP
LLBL(p2miv8_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EDX, D(1) )
	MOV_L( CONST(FP_ZERO), D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2miv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2miv8_top) )


LLBL(p2miv8_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_2d_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_2d_v8):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m2dv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2m2dv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FXCH( ST(1) ) 	/* F4 F5 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 */
	FADD_S( M(13) )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */

	MOV_L( CONST(FP_ZERO), D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m2dv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m2dv8_top) )


LLBL(p2m2dv8_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_2d_no_rot_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_2d_no_rot_v8):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m2dnrv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2m2dnrv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */
	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */

	MOV_L( CONST(FP_ZERO), D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m2dnrv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m2dnrv8_top) )


LLBL(p2m2dnrv8_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_3d_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_3d_v8):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m3dv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2m3dv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 F6 */
	FADD_S( M(13) )
	FXCH( ST(2) ) 	/* F6 F4 F5 */
	FADD_S( M(14) )

	FXCH( ST(1) ) 	/* F4 F6 F5 */
	FSTP_S( D(0)   ) 	/* F6 F5 */
	FXCH( ST(1) ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m3dv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m3dv8_top) )


LLBL(p2m3dv8_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_3d_no_rot_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_3d_no_rot_v8):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m3dnrv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	MOV_L( M(14), EBX )
	ALIGNTEXT4ifNOP
LLBL(p2m3dnrv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */
	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m3dnrv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m3dnrv8_top) )


LLBL(p2m3dnrv8_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_perspective_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_perspective_v8):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2mpv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	MOV_L( M(14), EBX )
	ALIGNTEXT4ifNOP
LLBL(p2mpv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FSTP_S( D(0)   ) 	/* F1 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( CONST(FP_ZERO), D(3) )

LLBL(p2mpv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2mpv8_top) )


LLBL(p2mpv8_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_general_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_general_v8):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3mgv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3mgv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(10) )
	FLD_S( S(2) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(11) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FADD_S( M(12) )
	FXCH( ST(2) ) 	/* F5 F6 F4 F7 */
	FADD_S( M(13) )
	FXCH( ST(1) ) 	/* F6 F5 F4 F7 */
	FADD_S( M(14) )
	FXCH( ST(3) ) 	/* F7 F5 F4 F6 */
	FADD_S( M(15) )

	FXCH( ST(2) ) 	/* F4 F5 F7 F6 */
	FSTP_S( D(0)   ) 	/* F5 F7 F6 */
	FSTP_S( D(1)   ) 	/* F7 F6 */
	FXCH( ST(1) ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p3mgv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3mgv8_top) )


LLBL(p3mgv8_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_identity_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_identity_v8):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3miv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	CMP_L( ESI, EDI )
	JE( LLBL(p3miv8_finish) )

	ALIGNTEXT4ifNOP
LLBL(p3miv8_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EBP )
	MOV_L( S(2), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EBP, D(1) )
	MOV_L( EDX, D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3miv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3miv8_top) )


LLBL(p3miv8_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_2d_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_2d_v8):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m2dv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m2dv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FXCH( ST(1) ) 	/* F4 F5 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 */
	FADD_S( M(13) )

	MOV_L( S(2), EBX )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m2dv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m2dv8_top) )


LLBL(p3m2dv8_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_2d_no_rot_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_2d_no_rot_v8):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m2dnrv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m2dnrv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */

	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	MOV_L( S(2), EBX )

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m2dnrv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m2dnrv8_top) )


LLBL(p3m2dnrv8_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_3d_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_3d_v8):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m3dv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m3dv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 F6 */
	FADD_S( M(13) )
	FXCH( ST(2) ) 	/* F6 F4 F5 */
	FADD_S( M(14) )

	FXCH( ST(1) ) 	/* F4 F6 F5 */
	FSTP_S( D(0)   ) 	/* F6 F5 */
	FXCH( ST(1) ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m3dv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m3dv8_top) )


LLBL(p3m3dv8_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_3d_no_rot_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_3d_no_rot_v8):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m3dnrv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m3dnrv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F2 F1 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F4 F1 F2 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 F2 */
	FXCH( ST(2) ) 	/* F1 F4 F5 F2 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 F2 */
	FLD_S( M(14) ) 	/* F6 F4 F5 F2 */
	FXCH( ST(3) ) 	/* F2 F4 F5 F6 */
	FADDP( ST(0), ST(3) ) 	/* F4 F5 F6 */

	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m3dnrv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m3dnrv8_top) )


LLBL(p3m3dnrv8_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_perspective_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_perspective_v8):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3mpv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3mpv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F0 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F5 F4 */
	FADDP( ST(0), ST(4) ) 	/* F1 F2 F5 F4 */
	FADDP( ST(0), ST(2) ) 	/* F2 F5 F4 */
	FLD_S( M(14) ) 	/* F6 F2 F5 F4 */
	FXCH( ST(1) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(2), EBX )
	XOR_L( CONST(-2147483648), EBX ) 	/* change sign */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p3mpv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3mpv8_top) )


LLBL(p3mpv8_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_general_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_general_v8):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4mgv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4mgv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(10) )
	FLD_S( S(2) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(11) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(3) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(14) )
	FLD_S( S(3) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(15) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FSTP_S( D(0)   ) 	/* F6 F5 F7 */
	FXCH( ST(1) ) 	/* F5 F6 F7 */
	FSTP_S( D(1)   ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p4mgv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4mgv8_top) )


LLBL(p4mgv8_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_identity_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_identity_v8):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4miv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	CMP_L( ESI, EDI )
	JE( LLBL(p4miv8_finish) )

	ALIGNTEXT4ifNOP
LLBL(p4miv8_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EDX, D(1) )

	MOV_L( S(2), EBX )
	MOV_L( S(3), EDX )

	MOV_L( EBX, D(2) )
	MOV_L( EDX, D(3) )
LLBL(p4miv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4miv8_top) )


LLBL(p4miv8_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_2d_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_2d_v8):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m2dv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m2dv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FLD_S( S(3) ) 	/* F0 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(13) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	MOV_L( S(2), EBX )
	MOV_L( S(3), EBP )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( EBP, D(3) )

LLBL(p4m2dv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m2dv8_top) )


LLBL(p4m2dv8_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_2d_no_rot_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_2d_no_rot_v8):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m2dnrv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m2dnrv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(3) ) 	/* F0 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(13) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	MOV_L( S(2), EBX )
	MOV_L( S(3), EBP )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( EBP, D(3) )

LLBL(p4m2dnrv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m2dnrv8_top) )


LLBL(p4m2dnrv8_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_3d_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_3d_v8):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m3dv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m3dv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(3) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(14) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(3), EBX )

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4m3dv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m3dv8_top) )


LLBL(p4m3dv8_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_3d_no_rot_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_3d_no_rot_v8):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m3dnrv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m3dnrv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F6 F5 F4 */
	FMUL_S( M(10) )

	FLD_S( S(3) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(14) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(3), EBX )

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4m3dnrv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m3dnrv8_top) )


LLBL(p4m3dnrv8_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_perspective_v8)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_perspective_v8):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4mpv8_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4mpv8_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F0 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F6 F1 F0 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F6 F5 F4 */
	FADDP( ST(0), ST(4) ) 	/* F1 F6 F5 F4 */
	FADDP( ST(0), ST(2) ) 	/* F6 F5 F4 */

	FLD_S( S(3) ) 	/* F2 F6 F5 F4 */
	FMUL_S( M(14) )

	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(2), EBX )
	XOR_L( CONST(-2147483648), EBX ) 	/* change sign */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4mpv8_skip):
	ADD_L( CONST(32), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4mpv8_top) )


LLBL(p4mpv8_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET



/*
########################################
##
## gl_x86_transform_points2_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_general_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_general_v12):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2mgv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2mgv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FADD_S( M(12) )
	FXCH( ST(2) ) 	/* F5 F6 F4 F7 */
	FADD_S( M(13) )
	FXCH( ST(1) ) 	/* F6 F5 F4 F7 */
	FADD_S( M(14) )
	FXCH( ST(3) ) 	/* F7 F5 F4 F6 */
	FADD_S( M(15) )

	FXCH( ST(2) ) 	/* F4 F5 F7 F6 */
	FSTP_S( D(0)   ) 	/* F5 F7 F6 */
	FSTP_S( D(1)   ) 	/* F7 F6 */
	FXCH( ST(1) ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p2mgv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2mgv12_top) )


LLBL(p2mgv12_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_identity_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_identity_v12):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2miv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	CMP_L( ESI, EDI )
	JE( LLBL(p2miv12_finish) )

	ALIGNTEXT4ifNOP
LLBL(p2miv12_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EDX, D(1) )
	MOV_L( CONST(FP_ZERO), D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2miv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2miv12_top) )


LLBL(p2miv12_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_2d_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_2d_v12):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m2dv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2m2dv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FXCH( ST(1) ) 	/* F4 F5 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 */
	FADD_S( M(13) )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */

	MOV_L( CONST(FP_ZERO), D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m2dv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m2dv12_top) )


LLBL(p2m2dv12_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_2d_no_rot_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_2d_no_rot_v12):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m2dnrv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2m2dnrv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */
	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */

	MOV_L( CONST(FP_ZERO), D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m2dnrv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m2dnrv12_top) )


LLBL(p2m2dnrv12_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_3d_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_3d_v12):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m3dv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2m3dv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 F6 */
	FADD_S( M(13) )
	FXCH( ST(2) ) 	/* F6 F4 F5 */
	FADD_S( M(14) )

	FXCH( ST(1) ) 	/* F4 F6 F5 */
	FSTP_S( D(0)   ) 	/* F6 F5 */
	FXCH( ST(1) ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m3dv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m3dv12_top) )


LLBL(p2m3dv12_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_3d_no_rot_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_3d_no_rot_v12):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m3dnrv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	MOV_L( M(14), EBX )
	ALIGNTEXT4ifNOP
LLBL(p2m3dnrv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */
	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m3dnrv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m3dnrv12_top) )


LLBL(p2m3dnrv12_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_perspective_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_perspective_v12):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2mpv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	MOV_L( M(14), EBX )
	ALIGNTEXT4ifNOP
LLBL(p2mpv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FSTP_S( D(0)   ) 	/* F1 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( CONST(FP_ZERO), D(3) )

LLBL(p2mpv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2mpv12_top) )


LLBL(p2mpv12_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_general_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_general_v12):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3mgv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3mgv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(10) )
	FLD_S( S(2) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(11) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FADD_S( M(12) )
	FXCH( ST(2) ) 	/* F5 F6 F4 F7 */
	FADD_S( M(13) )
	FXCH( ST(1) ) 	/* F6 F5 F4 F7 */
	FADD_S( M(14) )
	FXCH( ST(3) ) 	/* F7 F5 F4 F6 */
	FADD_S( M(15) )

	FXCH( ST(2) ) 	/* F4 F5 F7 F6 */
	FSTP_S( D(0)   ) 	/* F5 F7 F6 */
	FSTP_S( D(1)   ) 	/* F7 F6 */
	FXCH( ST(1) ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p3mgv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3mgv12_top) )


LLBL(p3mgv12_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_identity_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_identity_v12):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3miv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	CMP_L( ESI, EDI )
	JE( LLBL(p3miv12_finish) )

	ALIGNTEXT4ifNOP
LLBL(p3miv12_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EBP )
	MOV_L( S(2), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EBP, D(1) )
	MOV_L( EDX, D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3miv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3miv12_top) )


LLBL(p3miv12_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_2d_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_2d_v12):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m2dv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m2dv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FXCH( ST(1) ) 	/* F4 F5 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 */
	FADD_S( M(13) )

	MOV_L( S(2), EBX )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m2dv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m2dv12_top) )


LLBL(p3m2dv12_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_2d_no_rot_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_2d_no_rot_v12):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m2dnrv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m2dnrv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */

	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	MOV_L( S(2), EBX )

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m2dnrv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m2dnrv12_top) )


LLBL(p3m2dnrv12_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_3d_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_3d_v12):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m3dv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m3dv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 F6 */
	FADD_S( M(13) )
	FXCH( ST(2) ) 	/* F6 F4 F5 */
	FADD_S( M(14) )

	FXCH( ST(1) ) 	/* F4 F6 F5 */
	FSTP_S( D(0)   ) 	/* F6 F5 */
	FXCH( ST(1) ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m3dv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m3dv12_top) )


LLBL(p3m3dv12_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_3d_no_rot_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_3d_no_rot_v12):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m3dnrv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m3dnrv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F2 F1 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F4 F1 F2 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 F2 */
	FXCH( ST(2) ) 	/* F1 F4 F5 F2 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 F2 */
	FLD_S( M(14) ) 	/* F6 F4 F5 F2 */
	FXCH( ST(3) ) 	/* F2 F4 F5 F6 */
	FADDP( ST(0), ST(3) ) 	/* F4 F5 F6 */

	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m3dnrv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m3dnrv12_top) )


LLBL(p3m3dnrv12_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_perspective_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_perspective_v12):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3mpv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3mpv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F0 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F5 F4 */
	FADDP( ST(0), ST(4) ) 	/* F1 F2 F5 F4 */
	FADDP( ST(0), ST(2) ) 	/* F2 F5 F4 */
	FLD_S( M(14) ) 	/* F6 F2 F5 F4 */
	FXCH( ST(1) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(2), EBX )
	XOR_L( CONST(-2147483648), EBX ) 	/* change sign */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p3mpv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3mpv12_top) )


LLBL(p3mpv12_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_general_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_general_v12):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4mgv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4mgv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(10) )
	FLD_S( S(2) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(11) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(3) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(14) )
	FLD_S( S(3) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(15) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FSTP_S( D(0)   ) 	/* F6 F5 F7 */
	FXCH( ST(1) ) 	/* F5 F6 F7 */
	FSTP_S( D(1)   ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p4mgv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4mgv12_top) )


LLBL(p4mgv12_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_identity_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_identity_v12):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4miv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	CMP_L( ESI, EDI )
	JE( LLBL(p4miv12_finish) )

	ALIGNTEXT4ifNOP
LLBL(p4miv12_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EDX, D(1) )

	MOV_L( S(2), EBX )
	MOV_L( S(3), EDX )

	MOV_L( EBX, D(2) )
	MOV_L( EDX, D(3) )
LLBL(p4miv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4miv12_top) )


LLBL(p4miv12_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_2d_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_2d_v12):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m2dv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m2dv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FLD_S( S(3) ) 	/* F0 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(13) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	MOV_L( S(2), EBX )
	MOV_L( S(3), EBP )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( EBP, D(3) )

LLBL(p4m2dv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m2dv12_top) )


LLBL(p4m2dv12_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_2d_no_rot_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_2d_no_rot_v12):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m2dnrv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m2dnrv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(3) ) 	/* F0 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(13) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	MOV_L( S(2), EBX )
	MOV_L( S(3), EBP )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( EBP, D(3) )

LLBL(p4m2dnrv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m2dnrv12_top) )


LLBL(p4m2dnrv12_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_3d_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_3d_v12):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m3dv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m3dv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(3) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(14) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(3), EBX )

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4m3dv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m3dv12_top) )


LLBL(p4m3dv12_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_3d_no_rot_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_3d_no_rot_v12):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m3dnrv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m3dnrv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F6 F5 F4 */
	FMUL_S( M(10) )

	FLD_S( S(3) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(14) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(3), EBX )

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4m3dnrv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m3dnrv12_top) )


LLBL(p4m3dnrv12_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_perspective_v12)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_perspective_v12):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4mpv12_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4mpv12_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F0 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F6 F1 F0 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F6 F5 F4 */
	FADDP( ST(0), ST(4) ) 	/* F1 F6 F5 F4 */
	FADDP( ST(0), ST(2) ) 	/* F6 F5 F4 */

	FLD_S( S(3) ) 	/* F2 F6 F5 F4 */
	FMUL_S( M(14) )

	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(2), EBX )
	XOR_L( CONST(-2147483648), EBX ) 	/* change sign */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4mpv12_skip):
	ADD_L( CONST(48), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4mpv12_top) )


LLBL(p4mpv12_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET



/*
########################################
##
## gl_x86_transform_points2_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_general_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_general_v16):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2mgv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2mgv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FADD_S( M(12) )
	FXCH( ST(2) ) 	/* F5 F6 F4 F7 */
	FADD_S( M(13) )
	FXCH( ST(1) ) 	/* F6 F5 F4 F7 */
	FADD_S( M(14) )
	FXCH( ST(3) ) 	/* F7 F5 F4 F6 */
	FADD_S( M(15) )

	FXCH( ST(2) ) 	/* F4 F5 F7 F6 */
	FSTP_S( D(0)   ) 	/* F5 F7 F6 */
	FSTP_S( D(1)   ) 	/* F7 F6 */
	FXCH( ST(1) ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p2mgv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2mgv16_top) )


LLBL(p2mgv16_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_identity_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_identity_v16):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2miv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	CMP_L( ESI, EDI )
	JE( LLBL(p2miv16_finish) )

	ALIGNTEXT4ifNOP
LLBL(p2miv16_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EDX, D(1) )
	MOV_L( CONST(FP_ZERO), D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2miv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2miv16_top) )


LLBL(p2miv16_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_2d_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_2d_v16):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m2dv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2m2dv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FXCH( ST(1) ) 	/* F4 F5 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 */
	FADD_S( M(13) )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */

	MOV_L( CONST(FP_ZERO), D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m2dv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m2dv16_top) )


LLBL(p2m2dv16_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_2d_no_rot_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_2d_no_rot_v16):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m2dnrv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2m2dnrv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */
	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */

	MOV_L( CONST(FP_ZERO), D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m2dnrv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m2dnrv16_top) )


LLBL(p2m2dnrv16_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_3d_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_3d_v16):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m3dv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p2m3dv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 F6 */
	FADD_S( M(13) )
	FXCH( ST(2) ) 	/* F6 F4 F5 */
	FADD_S( M(14) )

	FXCH( ST(1) ) 	/* F4 F6 F5 */
	FSTP_S( D(0)   ) 	/* F6 F5 */
	FXCH( ST(1) ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m3dv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m3dv16_top) )


LLBL(p2m3dv16_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_3d_no_rot_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_3d_no_rot_v16):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2m3dnrv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	MOV_L( M(14), EBX )
	ALIGNTEXT4ifNOP
LLBL(p2m3dnrv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */
	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p2m3dnrv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2m3dnrv16_top) )


LLBL(p2m3dnrv16_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points2_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points2_perspective_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points2_perspective_v16):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p2mpv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	MOV_L( M(14), EBX )
	ALIGNTEXT4ifNOP
LLBL(p2mpv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FSTP_S( D(0)   ) 	/* F1 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( CONST(FP_ZERO), D(3) )

LLBL(p2mpv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p2mpv16_top) )


LLBL(p2mpv16_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_general_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_general_v16):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3mgv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3mgv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(10) )
	FLD_S( S(2) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(11) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FADD_S( M(12) )
	FXCH( ST(2) ) 	/* F5 F6 F4 F7 */
	FADD_S( M(13) )
	FXCH( ST(1) ) 	/* F6 F5 F4 F7 */
	FADD_S( M(14) )
	FXCH( ST(3) ) 	/* F7 F5 F4 F6 */
	FADD_S( M(15) )

	FXCH( ST(2) ) 	/* F4 F5 F7 F6 */
	FSTP_S( D(0)   ) 	/* F5 F7 F6 */
	FSTP_S( D(1)   ) 	/* F7 F6 */
	FXCH( ST(1) ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p3mgv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3mgv16_top) )


LLBL(p3mgv16_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_identity_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_identity_v16):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3miv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	CMP_L( ESI, EDI )
	JE( LLBL(p3miv16_finish) )

	ALIGNTEXT4ifNOP
LLBL(p3miv16_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EBP )
	MOV_L( S(2), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EBP, D(1) )
	MOV_L( EDX, D(2) )
	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3miv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3miv16_top) )


LLBL(p3miv16_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_2d_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_2d_v16):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m2dv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m2dv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FXCH( ST(1) ) 	/* F4 F5 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 */
	FADD_S( M(13) )

	MOV_L( S(2), EBX )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m2dv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m2dv16_top) )


LLBL(p3m2dv16_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_2d_no_rot_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_2d_no_rot_v16):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m2dnrv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m2dnrv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F4 F1 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 */

	FXCH( ST(2) ) 	/* F1 F4 F5 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 */

	MOV_L( S(2), EBX )

	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m2dnrv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m2dnrv16_top) )


LLBL(p3m2dnrv16_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_3d_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_3d_v16):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m3dv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m3dv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FADD_S( M(12) )
	FXCH( ST(1) ) 	/* F5 F4 F6 */
	FADD_S( M(13) )
	FXCH( ST(2) ) 	/* F6 F4 F5 */
	FADD_S( M(14) )

	FXCH( ST(1) ) 	/* F4 F6 F5 */
	FSTP_S( D(0)   ) 	/* F6 F5 */
	FXCH( ST(1) ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m3dv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m3dv16_top) )


LLBL(p3m3dv16_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_3d_no_rot_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_3d_no_rot_v16):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3m3dnrv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3m3dnrv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F1 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F2 F1 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F4 F1 F2 */
	FADD_S( M(12) )
	FLD_S( M(13) ) 	/* F5 F4 F1 F2 */
	FXCH( ST(2) ) 	/* F1 F4 F5 F2 */
	FADDP( ST(0), ST(2) ) 	/* F4 F5 F2 */
	FLD_S( M(14) ) 	/* F6 F4 F5 F2 */
	FXCH( ST(3) ) 	/* F2 F4 F5 F6 */
	FADDP( ST(0), ST(3) ) 	/* F4 F5 F6 */

	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */

	MOV_L( CONST(FP_ONE), D(3) )
LLBL(p3m3dnrv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3m3dnrv16_top) )


LLBL(p3m3dnrv16_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points3_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points3_perspective_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points3_perspective_v16):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p3mpv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p3mpv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F0 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F5 F4 */
	FADDP( ST(0), ST(4) ) 	/* F1 F2 F5 F4 */
	FADDP( ST(0), ST(2) ) 	/* F2 F5 F4 */
	FLD_S( M(14) ) 	/* F6 F2 F5 F4 */
	FXCH( ST(1) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(2), EBX )
	XOR_L( CONST(-2147483648), EBX ) 	/* change sign */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p3mpv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p3mpv16_top) )


LLBL(p3mpv16_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_general
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_general_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_general_v16):

#define FRAME_OFFSET 8
	PUSH_L( ESI )
	PUSH_L( EDI )


	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4mgv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4mgv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )
	FLD_S( S(0) ) 	/* F7 F6 F5 F4 */
	FMUL_S( M(3) )

	FLD_S( S(1) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(6) )
	FLD_S( S(1) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(7) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(10) )
	FLD_S( S(2) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(11) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FLD_S( S(3) ) 	/* F0 F7 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(14) )
	FLD_S( S(3) ) 	/* F3 F2 F1 F0 F7 F6 F5 F4 */
	FMUL_S( M(15) )

	FXCH( ST(3) ) 	/* F0 F2 F1 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(7) ) 	/* F2 F1 F3 F7 F6 F5 F4 */
	FXCH( ST(1) ) 	/* F1 F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F2 F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F3 F7 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F7 F6 F5 F4 */

	FXCH( ST(3) ) 	/* F4 F6 F5 F7 */
	FSTP_S( D(0)   ) 	/* F6 F5 F7 */
	FXCH( ST(1) ) 	/* F5 F6 F7 */
	FSTP_S( D(1)   ) 	/* F6 F7 */
	FSTP_S( D(2)   ) 	/* F7 */
	FSTP_S( D(3)   ) 	/* */

LLBL(p4mgv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4mgv16_top) )


LLBL(p4mgv16_finish):
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_identity
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_identity_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_identity_v16):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4miv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	CMP_L( ESI, EDI )
	JE( LLBL(p4miv16_finish) )

	ALIGNTEXT4ifNOP
LLBL(p4miv16_top):

	MOV_L( S(0), EBX )
	MOV_L( S(1), EDX )

	MOV_L( EBX, D(0) )
	MOV_L( EDX, D(1) )

	MOV_L( S(2), EBX )
	MOV_L( S(3), EDX )

	MOV_L( EBX, D(2) )
	MOV_L( EDX, D(3) )
LLBL(p4miv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4miv16_top) )


LLBL(p4miv16_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_2d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_2d_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_2d_v16):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m2dv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m2dv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )

	FLD_S( S(1) ) 	/* F0 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(5) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	FLD_S( S(3) ) 	/* F0 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(13) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	MOV_L( S(2), EBX )
	MOV_L( S(3), EBP )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( EBP, D(3) )

LLBL(p4m2dv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m2dv16_top) )


LLBL(p4m2dv16_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_2d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_2d_no_rot_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_2d_no_rot_v16):

#define FRAME_OFFSET 16
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )
	PUSH_L( EBP )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m2dnrv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m2dnrv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(3) ) 	/* F0 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(13) )

	FXCH( ST(1) ) 	/* F0 F1 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F1 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F5 F4 */

	MOV_L( S(2), EBX )
	MOV_L( S(3), EBP )

	FXCH( ST(1) ) 	/* F4 F5 */
	FSTP_S( D(0)   ) 	/* F5 */
	FSTP_S( D(1)   ) 	/* */
	MOV_L( EBX, D(2) )
	MOV_L( EBP, D(3) )

LLBL(p4m2dnrv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m2dnrv16_top) )


LLBL(p4m2dnrv16_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_3d
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_3d_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_3d_v16):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m3dv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m3dv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )
	FLD_S( S(0) ) 	/* F5 F4 */
	FMUL_S( M(1) )
	FLD_S( S(0) ) 	/* F6 F5 F4 */
	FMUL_S( M(2) )

	FLD_S( S(1) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(4) )
	FLD_S( S(1) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(5) )
	FLD_S( S(1) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(6) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(2) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	FLD_S( S(3) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(14) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(3), EBX )

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4m3dv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m3dv16_top) )


LLBL(p4m3dv16_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_3d_no_rot
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_3d_no_rot_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_3d_no_rot_v16):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4m3dnrv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4m3dnrv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F6 F5 F4 */
	FMUL_S( M(10) )

	FLD_S( S(3) ) 	/* F0 F6 F5 F4 */
	FMUL_S( M(12) )
	FLD_S( S(3) ) 	/* F1 F0 F6 F5 F4 */
	FMUL_S( M(13) )
	FLD_S( S(3) ) 	/* F2 F1 F0 F6 F5 F4 */
	FMUL_S( M(14) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(5) ) 	/* F1 F2 F6 F5 F4 */
	FADDP( ST(0), ST(3) ) 	/* F2 F6 F5 F4 */
	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(3), EBX )

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4m3dnrv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4m3dnrv16_top) )


LLBL(p4m3dnrv16_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET

/*
########################################
##
## gl_x86_transform_points4_perspective
##
##
*/
	GLOBL GLNAME(gl_x86_transform_points4_perspective_v16)
	ALIGNTEXT4
GLNAME(gl_x86_transform_points4_perspective_v16):

#define FRAME_OFFSET 12
	PUSH_L( ESI )
	PUSH_L( EDI )

	PUSH_L( EBX )

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(p4mpv16_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */


	ALIGNTEXT4ifNOP
LLBL(p4mpv16_top):


	FLD_S( S(0) ) 	/* F4 */
	FMUL_S( M(0) )

	FLD_S( S(1) ) 	/* F5 F4 */
	FMUL_S( M(5) )

	FLD_S( S(2) ) 	/* F0 F5 F4 */
	FMUL_S( M(8) )
	FLD_S( S(2) ) 	/* F1 F0 F5 F4 */
	FMUL_S( M(9) )
	FLD_S( S(2) ) 	/* F6 F1 F0 F5 F4 */
	FMUL_S( M(10) )

	FXCH( ST(2) ) 	/* F0 F1 F6 F5 F4 */
	FADDP( ST(0), ST(4) ) 	/* F1 F6 F5 F4 */
	FADDP( ST(0), ST(2) ) 	/* F6 F5 F4 */

	FLD_S( S(3) ) 	/* F2 F6 F5 F4 */
	FMUL_S( M(14) )

	FADDP( ST(0), ST(1) ) 	/* F6 F5 F4 */

	MOV_L( S(2), EBX )
	XOR_L( CONST(-2147483648), EBX ) 	/* change sign */

	FXCH( ST(2) ) 	/* F4 F5 F6 */
	FSTP_S( D(0)   ) 	/* F5 F6 */
	FSTP_S( D(1)   ) 	/* F6 */
	FSTP_S( D(2)   ) 	/* */
	MOV_L( EBX, D(3) )

LLBL(p4mpv16_skip):
	ADD_L( CONST(64), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(p4mpv16_top) )


LLBL(p4mpv16_finish):
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
#undef FRAME_OFFSET



#undef OFFSET_DEST
#undef OFFSET_MATRIX
#undef OFFSET_SOURCE
#undef OFFSET_STRIDE
#undef OFFSET_COUNT

#undef ARG_DEST
#undef ARG_MATRIX
#undef ARG_SOURCE
#undef ARG_STRIDE
#undef ARG_COUNT


/*
 * Table for clip test.
 *
 * 	bit6 = S(3) < 0
 * 	bit5 = S(2) < 0
 * 	bit4 = abs(S(2)) > abs(S(3))
 * 	bit3 = S(1) < 0
 * 	bit2 = abs(S(1)) > abs(S(3))
 * 	bit1 = S(0) < 0
 * 	bit0 = abs(S(0)) > abs(S(3))
 */


	SEG_DATA

clip_table:
	D_BYTE 0, 1, 0, 2, 4, 5, 4, 6
	D_BYTE 0, 1, 0, 2, 8, 9, 8, 10
	D_BYTE 32, 33, 32, 34, 36, 37, 36, 38
	D_BYTE 32, 33, 32, 34, 40, 41, 40, 42
	D_BYTE 0, 1, 0, 2, 4, 5, 4, 6
	D_BYTE 0, 1, 0, 2, 8, 9, 8, 10
	D_BYTE 16, 17, 16, 18, 20, 21, 20, 22
	D_BYTE 16, 17, 16, 18, 24, 25, 24, 26
	D_BYTE 63, 61, 63, 62, 55, 53, 55, 54
	D_BYTE 63, 61, 63, 62, 59, 57, 59, 58
	D_BYTE 47, 45, 47, 46, 39, 37, 39, 38
	D_BYTE 47, 45, 47, 46, 43, 41, 43, 42
	D_BYTE 63, 61, 63, 62, 55, 53, 55, 54
	D_BYTE 63, 61, 63, 62, 59, 57, 59, 58
	D_BYTE 31, 29, 31, 30, 23, 21, 23, 22
	D_BYTE 31, 29, 31, 30, 27, 25, 27, 26

	SEG_TEXT

/*
 *	Offsets for clip_func arguments
 *
 *	typedef GLvector4f *(*clip_func)( GLvector4f *vClip, 
 *	                                  GLvector4f *vProj, 
 *	                                  GLubyte clipMask[],
 *	                                  GLubyte *orMask, 
 *	                                  GLubyte *andMask );
 */

#define OFFSET_SOURCE 4
#define OFFSET_DEST 8
#define OFFSET_CLIP 12
#define OFFSET_OR 16
#define OFFSET_AND 20

#define ARG_SOURCE 	REGOFF(FRAME_OFFSET+OFFSET_SOURCE, ESP)
#define ARG_DEST 	REGOFF(FRAME_OFFSET+OFFSET_DEST, ESP)
#define ARG_CLIP 	REGOFF(FRAME_OFFSET+OFFSET_CLIP, ESP)
#define ARG_OR 	REGOFF(FRAME_OFFSET+OFFSET_OR, ESP)
#define ARG_AND 	REGOFF(FRAME_OFFSET+OFFSET_AND, ESP)

/*
########################################
##
## gl_x86_cliptest_points4
##
##   AL:  ormask
##   AH:  andmask
##   EBX: temp0
##   ECX: temp1
##   EDX: clipmask[]
##   ESI: clip[]
##   EDI: proj[]
##   EBP: temp2
##
########################################
*/

#if defined(__ELF__) && defined(__PIC__) && !defined(ELFPIC)
#define ELFPIC
#endif

	GLOBL GLNAME(gl_x86_cliptest_points4)
	ALIGNTEXT4

GLNAME(gl_x86_cliptest_points4):
#ifdef ELFPIC
#define FRAME_OFFSET 20
#else
#define FRAME_OFFSET 16
#endif
	PUSH_L( ESI )
	PUSH_L( EDI )
	PUSH_L( EBP )
	PUSH_L( EBX )

#ifdef ELFPIC
	/* store pointer to clip_table on stack */
	CALL( LLBL(ctp4_get_eip) )
	ADD_L( CONST(_GLOBAL_OFFSET_TABLE_), EBX )
	MOV_L( REGOFF(clip_table@GOT, EBX), EBX )
	PUSH_L( EBX )
	JMP( LLBL(ctp4_clip_table_ready) )

LLBL(ctp4_get_eip):
	/* store eip in ebx */
	MOV_L( REGIND(ESP), EBX )
	RET

LLBL(ctp4_clip_table_ready):
#endif

	MOV_L( ARG_SOURCE, ESI )
	MOV_L( ARG_DEST, EDI )

	MOV_L( ARG_CLIP, EDX )
	MOV_L( ARG_OR, EBX )

	MOV_L( ARG_AND, EBP )
	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* source stride */

	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	OR_L( CONST(VEC_SIZE_4), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */
	MOV_L( EAX, ARG_SOURCE ) 		/* put stride in ARG_SOURCE */

	MOV_L( CONST(3), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */
	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDX, ECX )

	MOV_L( ECX, ARG_CLIP ) 			/* put clipmask + count in ARG_CLIP */
	CMP_L( ECX, EDX )

	MOV_B( REGIND(EBX), AL )
	MOV_B( REGIND(EBP), AH )

	JZ( LLBL(ctp4_finish) )

	ALIGNTEXT4ifNOP
LLBL(ctp4_top):
	FLD1 	/* F3 */
	FDIV_S( S(3) )

	MOV_L( S(3), EBP )
	MOV_L( S(2), EBX )

	XOR_L( ECX, ECX )
	ADD_L( EBP, EBP )	/* ebp = abs(S(3))*2 ; carry = sign of S(3) */

	ADC_L( ECX, ECX )
	ADD_L( EBX, EBX )	/* ebx = abs(S(2))*2 ; carry = sign of S(2) */

	ADC_L( ECX, ECX )
	CMP_L( EBX, EBP )	/* carry = abs(S(2))*2 > abs(S(3))*2 */

	ADC_L( ECX, ECX )
	MOV_L( S(1), EBX )

	ADD_L( EBX, EBX )	/* ebx = abs(S(1))*2 ; carry = sign of S(1) */

	ADC_L( ECX, ECX )
	CMP_L( EBX, EBP )	/* carry = abs(S(1))*2 > abs(S(3))*2 */

	ADC_L( ECX, ECX )
	MOV_L( S(0), EBX )

	ADD_L( EBX, EBX )	/* ebx = abs(S(0))*2 ; carry = sign of S(0) */

	ADC_L( ECX, ECX )
	CMP_L( EBX, EBP )	/* carry = abs(S(0))*2 > abs(S(3))*2 */

	ADC_L( ECX, ECX )

#ifdef ELFPIC
	MOV_L( REGIND(ESP), EBP ) 	/* clip_table */

	MOV_B( REGBI(EBP, ECX), CL )
#else
	MOV_B( REGOFF(clip_table,ECX), CL )
#endif

	OR_B( CL, AL )
	AND_B( CL, AH )

	TEST_B( CL, CL )
	MOV_B( CL, REGIND(EDX) )

	JZ( LLBL(ctp4_proj) )

	FSTP( ST(0) ) 	/* */
	JMP( LLBL(ctp4_next) )

LLBL(ctp4_proj):
	FLD_S( S(0) ) 	/* F0 F3 */
	FMUL2( ST(1), ST(0) )

	FLD_S( S(1) ) 	/* F1 F0 F3 */
	FMUL2( ST(2), ST(0) )

	FLD_S( S(2) ) 	/* F2 F1 F0 F3 */
	FMUL2( ST(3), ST(0) )

	FXCH( ST(2) ) 	/* F0 F1 F2 F3 */
	FSTP_S( D(0)   ) 	/* F1 F2 F3 */
	FSTP_S( D(1)   ) 	/* F2 F3 */
	FSTP_S( D(2)   ) 	/* F3 */
	FSTP_S( D(3)   ) 	/* */

LLBL(ctp4_next):
	INC_L( EDX )
	ADD_L( CONST(16), EDI )

	ADD_L( ARG_SOURCE, ESI )
	CMP_L( EDX, ARG_CLIP )

	JNZ( LLBL(ctp4_top) )

	MOV_L( ARG_OR, ECX )
	MOV_L( ARG_AND, EDX )

	MOV_B( AL, REGIND(ECX) )
	MOV_B( AH, REGIND(EDX) )

LLBL(ctp4_finish):
	MOV_L( ARG_DEST, EAX )
#ifdef ELFPIC
	POP_L( ESI ) 	/* discard ptr to clip_table */
#endif
	POP_L( EBX )
	POP_L( EBP )
	POP_L( EDI )
	POP_L( ESI )

	RET




/*
########################################
##
## gl_v16_x86_cliptest_points4
##
## Performs cliptesting equivalent to that done by cliptest_v16()
## in vertices.c
##
## This is a hacked version of the original above.
##
########################################
*/



#define OFFSET_V16_SOURCE 4
#define OFFSET_V16_LAST 8
#define OFFSET_V16_OR 12
#define OFFSET_V16_AND 16
#define OFFSET_V16_MASK 20

#define ARG_V16_SOURCE	REGOFF(V16_FRAME_OFFSET+OFFSET_V16_SOURCE, ESP)
#define ARG_V16_LAST 	REGOFF(V16_FRAME_OFFSET+OFFSET_V16_LAST, ESP)
#define ARG_V16_OR 	REGOFF(V16_FRAME_OFFSET+OFFSET_V16_OR, ESP)
#define ARG_V16_AND 	REGOFF(V16_FRAME_OFFSET+OFFSET_V16_AND, ESP)
#define ARG_V16_MASK 	REGOFF(V16_FRAME_OFFSET+OFFSET_V16_MASK, ESP)


#if defined(__ELF__) && defined(__PIC__) && !defined(ELFPIC)
#define ELFPIC
#endif

	GLOBL GLNAME(gl_v16_x86_cliptest_points4)
	ALIGNTEXT4

GLNAME(gl_v16_x86_cliptest_points4):
#ifdef ELFPIC
#define V16_FRAME_OFFSET 20
#else
#define V16_FRAME_OFFSET 16
#endif
	PUSH_L( ESI )
	PUSH_L( EDI )
	PUSH_L( EBP )
	PUSH_L( EBX )

#ifdef ELFPIC
	/* store pointer to clip_table on stack */
	CALL( LLBL(v16_ctp4_get_eip) )
	ADD_L( CONST(_GLOBAL_OFFSET_TABLE_), EBX )
	MOV_L( REGOFF(clip_table@GOT, EBX), EBX )
	PUSH_L( EBX )
	JMP( LLBL(v16_ctp4_clip_table_ready) )

LLBL(v16_ctp4_get_eip):
	/* store eip in ebx */
	MOV_L( REGIND(ESP), EBX )
	RET

LLBL(v16_ctp4_clip_table_ready):
#endif

	MOV_L( ARG_V16_SOURCE, ESI )            /* ptr to first source vertex */
	MOV_L( ARG_V16_LAST, EDX ) 	        /* ptr to last source vertex */
	MOV_L( ARG_V16_OR, EBX )
	MOV_L( ARG_V16_AND, EBP )
	MOV_L( ARG_V16_MASK, EDI )

	CMP_L( EDX, ESI )

	MOV_B( REGIND(EBX), AL )
	MOV_B( REGIND(EBP), AH )

	JZ( LLBL(v16_ctp4_finish) )

	ALIGNTEXT4ifNOP
LLBL(v16_ctp4_top):
#if 0
	FLD1 	/* F0 */
	FDIV_S( S(3) )
#endif

	MOV_L( S(3), EBP )
	MOV_L( S(2), EBX )

	XOR_L( ECX, ECX )
	ADD_L( EBP, EBP )	/* ebp = abs(S(3))*2 ; carry = sign of S(3) */

	ADC_L( ECX, ECX )
	ADD_L( EBX, EBX )	/* ebx = abs(S(2))*2 ; carry = sign of S(2) */

	ADC_L( ECX, ECX )
	CMP_L( EBX, EBP )	/* carry = abs(S(2))*2 > abs(S(3))*2 */

	ADC_L( ECX, ECX )
	MOV_L( S(1), EBX )

	ADD_L( EBX, EBX )	/* ebx = abs(S(1))*2 ; carry = sign of S(1) */

	ADC_L( ECX, ECX )
	CMP_L( EBX, EBP )	/* carry = abs(S(1))*2 > abs(S(3))*2 */

	ADC_L( ECX, ECX )
	MOV_L( S(0), EBX )

	ADD_L( EBX, EBX )	/* ebx = abs(S(0))*2 ; carry = sign of S(0) */

	ADC_L( ECX, ECX )
	CMP_L( EBX, EBP )	/* carry = abs(S(0))*2 > abs(S(3))*2 */

	ADC_L( ECX, ECX )

#ifdef ELFPIC
	MOV_L( REGIND(ESP), EBP ) 	/* clip_table */

	MOV_B( REGBI(EBP, ECX), CL )
#else
	MOV_B( REGOFF(clip_table,ECX), CL )
#endif

	OR_B( CL, AL )
	AND_B( CL, AH )

	MOV_B( CL, REGIND(EDI) )     /* save clipmask */
	INC_L( EDI )                 /* next clipmask */

#if 0
	FSTP_S( S(8)   ) 	/* */ 	/* GR_VERTEX_OOW_OFFSET */
#endif

	ADD_L( CONST(64), ESI )      /* next fxVertex  */

	CMP_L( EDX, ESI )            /* finished? */
	JNZ( LLBL(v16_ctp4_top) )

	MOV_L( ARG_V16_OR, ECX )
	MOV_L( ARG_V16_AND, EDX )

	MOV_B( AL, REGIND(ECX) )
	MOV_B( AH, REGIND(EDX) )

LLBL(v16_ctp4_finish):

#ifdef ELFPIC
	POP_L( ESI ) 	/* discard ptr to clip_table */
#endif
	POP_L( EBX )
	POP_L( EBP )
	POP_L( EDI )
	POP_L( ESI )

	RET

