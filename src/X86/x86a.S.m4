include(x86flatregs.m4)dnl
dnl ############################################################
dnl ## output_x86_transform_functions
dnl ##
dnl ## This macro is called twice.  Once with mask checking
dnl ## versions of FUNCTION_HEAD, LOOP_HEAD, LOOP_TAIL and
dnl ## FUNCTION_TAIL, and once with versions that don't check
dnl ## the clip mask.
dnl ##
dnl ## The parameters for FUNCTION_HEAD are:
dnl ## - The function's name.  The mask checking version will
dnl ##   append "_masked" to this, the unmasked version appends
dnl ##   "_raw".
dnl ## - A unique string (for labels).
dnl ## - The number of integer registers needed.  Can be 0-2.
dnl ##   If not zero, IREG_0 and/or IREG_1 will be defined.
dnl ## - The output vector size.
dnl ############################################################
define(`output_x86_transform_functions', `dnl
/*
########################################
##
## gl_x86_transform_points2_general
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points2_general, p2mg, 0, 4)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )
	F_FLD_S( S(0), F5 )
	F_FMUL_S( M(1), F5 )
	F_FLD_S( S(0), F6 )
	F_FMUL_S( M(2), F6 )
	F_FLD_S( S(0), F7 )
	F_FMUL_S( M(3), F7 )

	F_FLD_S( S(1), F0 )
	F_FMUL_S( M(4), F0 )
	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )
	F_FLD_S( S(1), F2 )
	F_FMUL_S( M(6), F2 )
	F_FLD_S( S(1), F3 )
	F_FMUL_S( M(7), F3 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )
	F_FADDP( F3, F7 )

	F_FADD_S( M(12), F4 )
	F_FADD_S( M(13), F5 )
	F_FADD_S( M(14), F6 )
	F_FADD_S( M(15), F7 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	F_FSTP_S( F6, D(2) )
	F_FSTP_S( F7, D(3) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points2_identity
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points2_identity, p2mi, 1, 2)

	CMP_L( ESI, EDI )
	JE( LLBL(SHORT_NAME`'_finish) )

LOOP_HEAD
	MOV_L( S(0), IREG_0 )
	MOV_L( S(1), EDX )

	MOV_L( IREG_0, D(0) )
	MOV_L( EDX, D(1) )
LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points2_2d
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points2_2d, p2m2d, 0, 2)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )
	F_FLD_S( S(0), F5 )
	F_FMUL_S( M(1), F5 )

	F_FLD_S( S(1), F0 )
	F_FMUL_S( M(4), F0 )
	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )

	F_FADD_S( M(12), F4 )
	F_FADD_S( M(13), F5 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points2_2d_no_rot
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points2_2d_no_rot, p2m2dnr, 0, 2)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )

	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )

	F_FADD_S( M(12), F4 )
	F_FLD_S( M(13), F5 )
	F_FADDP( F1, F5 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points2_3d
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points2_3d, p2m3d, 0, 3)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )
	F_FLD_S( S(0), F5 )
	F_FMUL_S( M(1), F5 )
	F_FLD_S( S(0), F6 )
	F_FMUL_S( M(2), F6 )

	F_FLD_S( S(1), F0 )
	F_FMUL_S( M(4), F0 )
	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )
	F_FLD_S( S(1), F2 )
	F_FMUL_S( M(6), F2 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )

	F_FADD_S( M(12), F4 )
	F_FADD_S( M(13), F5 )
	F_FADD_S( M(14), F6 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	F_FSTP_S( F6, D(2) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points2_3d_no_rot
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points2_3d_no_rot, p2m3dnr, 1, 3)

	MOV_L( M(14), IREG_0 )
LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )

	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )

	F_FADD_S( M(12), F4 )
	F_FLD_S( M(13), F5 )
	F_FADDP( F1, F5 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	MOV_L( IREG_0, D(2) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points2_perspective
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points2_perspective, p2mp, 1, 4)

	MOV_L( M(14), IREG_0 )
LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )

	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F1, D(1) )
	MOV_L( IREG_0, D(2) )
	MOV_L( CONST(FP_ZERO), D(3) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points3_general
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points3_general, p3mg, 0, 4)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )
	F_FLD_S( S(0), F5 )
	F_FMUL_S( M(1), F5 )
	F_FLD_S( S(0), F6 )
	F_FMUL_S( M(2), F6 )
	F_FLD_S( S(0), F7 )
	F_FMUL_S( M(3), F7 )

	F_FLD_S( S(1), F0 )
	F_FMUL_S( M(4), F0 )
	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )
	F_FLD_S( S(1), F2 )
	F_FMUL_S( M(6), F2 )
	F_FLD_S( S(1), F3 )
	F_FMUL_S( M(7), F3 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )
	F_FADDP( F3, F7 )

	F_FLD_S( S(2), F0 )
	F_FMUL_S( M(8), F0 )
	F_FLD_S( S(2), F1 )
	F_FMUL_S( M(9), F1 )
	F_FLD_S( S(2), F2 )
	F_FMUL_S( M(10), F2 )
	F_FLD_S( S(2), F3 )
	F_FMUL_S( M(11), F3 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )
	F_FADDP( F3, F7 )

	F_FADD_S( M(12), F4 )
	F_FADD_S( M(13), F5 )
	F_FADD_S( M(14), F6 )
	F_FADD_S( M(15), F7 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	F_FSTP_S( F6, D(2) )
	F_FSTP_S( F7, D(3) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points3_identity
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points3_identity, p3mi, 2, 3)

	CMP_L( ESI, EDI )
	JE( LLBL(SHORT_NAME`'_finish) )

LOOP_HEAD
	MOV_L( S(0), IREG_0 )
	MOV_L( S(1), IREG_1 )
	MOV_L( S(2), EDX )

	MOV_L( IREG_0, D(0) )
	MOV_L( IREG_1, D(1) )
	MOV_L( EDX, D(2) )
LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points3_2d
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points3_2d, p3m2d, 1, 3)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )
	F_FLD_S( S(0), F5 )
	F_FMUL_S( M(1), F5 )

	F_FLD_S( S(1), F0 )
	F_FMUL_S( M(4), F0 )
	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )

	F_FADD_S( M(12), F4 )
	F_FADD_S( M(13), F5 )

	MOV_L( S(2), IREG_0 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	MOV_L( IREG_0, D(2) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points3_2d_no_rot
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points3_2d_no_rot, p3m2dnr, 1, 3)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )

	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )

	F_FADD_S( M(12), F4 )
	F_FLD_S( M(13), F5 )

	F_FADDP( F1, F5 )

	MOV_L( S(2), IREG_0 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	MOV_L( IREG_0, D(2) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points3_3d
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points3_3d, p3m3d, 0, 3)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )
	F_FLD_S( S(0), F5 )
	F_FMUL_S( M(1), F5 )
	F_FLD_S( S(0), F6 )
	F_FMUL_S( M(2), F6 )

	F_FLD_S( S(1), F0 )
	F_FMUL_S( M(4), F0 )
	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )
	F_FLD_S( S(1), F2 )
	F_FMUL_S( M(6), F2 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )

	F_FLD_S( S(2), F0 )
	F_FMUL_S( M(8), F0 )
	F_FLD_S( S(2), F1 )
	F_FMUL_S( M(9), F1 )
	F_FLD_S( S(2), F2 )
	F_FMUL_S( M(10), F2 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )

	F_FADD_S( M(12), F4 )
	F_FADD_S( M(13), F5 )
	F_FADD_S( M(14), F6 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	F_FSTP_S( F6, D(2) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points3_3d_no_rot
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points3_3d_no_rot, p3m3dnr, 0, 3)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )

	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )

	F_FLD_S( S(2), F2 )
	F_FMUL_S( M(10), F2 )

	F_FADD_S( M(12), F4 )
	F_FLD_S( M(13), F5 )
	F_FADDP( F1, F5 )
	F_FLD_S( M(14), F6 )
	F_FADDP( F2, F6 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	F_FSTP_S( F6, D(2) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points3_perspective
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points3_perspective, p3mp, 1, 4)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )

	F_FLD_S( S(1), F5 )
	F_FMUL_S( M(5), F5 )

	F_FLD_S( S(2), F0 )
	F_FMUL_S( M(8), F0 )
	F_FLD_S( S(2), F1 )
	F_FMUL_S( M(9), F1 )
	F_FLD_S( S(2), F2 )
	F_FMUL_S( M(10), F2 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FLD_S( M(14), F6 )
	F_FADDP( F2, F6 )

	MOV_L( S(2), IREG_0 )
	XOR_L( CONST(eval(0x80000000)), IREG_0 ) 	/* change sign */

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	F_FSTP_S( F6, D(2) )
	MOV_L( IREG_0, D(3) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points4_general
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points4_general, p4mg, 0, 4)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )
	F_FLD_S( S(0), F5 )
	F_FMUL_S( M(1), F5 )
	F_FLD_S( S(0), F6 )
	F_FMUL_S( M(2), F6 )
	F_FLD_S( S(0), F7 )
	F_FMUL_S( M(3), F7 )

	F_FLD_S( S(1), F0 )
	F_FMUL_S( M(4), F0 )
	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )
	F_FLD_S( S(1), F2 )
	F_FMUL_S( M(6), F2 )
	F_FLD_S( S(1), F3 )
	F_FMUL_S( M(7), F3 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )
	F_FADDP( F3, F7 )

	F_FLD_S( S(2), F0 )
	F_FMUL_S( M(8), F0 )
	F_FLD_S( S(2), F1 )
	F_FMUL_S( M(9), F1 )
	F_FLD_S( S(2), F2 )
	F_FMUL_S( M(10), F2 )
	F_FLD_S( S(2), F3 )
	F_FMUL_S( M(11), F3 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )
	F_FADDP( F3, F7 )

	F_FLD_S( S(3), F0 )
	F_FMUL_S( M(12), F0 )
	F_FLD_S( S(3), F1 )
	F_FMUL_S( M(13), F1 )
	F_FLD_S( S(3), F2 )
	F_FMUL_S( M(14), F2 )
	F_FLD_S( S(3), F3 )
	F_FMUL_S( M(15), F3 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )
	F_FADDP( F3, F7 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	F_FSTP_S( F6, D(2) )
	F_FSTP_S( F7, D(3) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points4_identity
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points4_identity, p4mi, 1, 4)

	CMP_L( ESI, EDI )
	JE( LLBL(SHORT_NAME`'_finish) )

LOOP_HEAD
	MOV_L( S(0), IREG_0 )
	MOV_L( S(1), EDX )

	MOV_L( IREG_0, D(0) )
	MOV_L( EDX, D(1) )

	MOV_L( S(2), IREG_0 )
	MOV_L( S(3), EDX )

	MOV_L( IREG_0, D(2) )
	MOV_L( EDX, D(3) )
LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points4_2d
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points4_2d, p4m2d, 2, 4)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )
	F_FLD_S( S(0), F5 )
	F_FMUL_S( M(1), F5 )

	F_FLD_S( S(1), F0 )
	F_FMUL_S( M(4), F0 )
	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )

	F_FLD_S( S(3), F0 )
	F_FMUL_S( M(12), F0 )
	F_FLD_S( S(3), F1 )
	F_FMUL_S( M(13), F1 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )

	MOV_L( S(2), IREG_0 )
	MOV_L( S(3), IREG_1 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	MOV_L( IREG_0, D(2) )
	MOV_L( IREG_1, D(3) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points4_2d_no_rot
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points4_2d_no_rot, p4m2dnr, 2, 4)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )

	F_FLD_S( S(1), F5 )
	F_FMUL_S( M(5), F5 )

	F_FLD_S( S(3), F0 )
	F_FMUL_S( M(12), F0 )
	F_FLD_S( S(3), F1 )
	F_FMUL_S( M(13), F1 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )

	MOV_L( S(2), IREG_0 )
	MOV_L( S(3), IREG_1 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	MOV_L( IREG_0, D(2) )
	MOV_L( IREG_1, D(3) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points4_3d
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points4_3d, p4m3d, 1, 4)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )
	F_FLD_S( S(0), F5 )
	F_FMUL_S( M(1), F5 )
	F_FLD_S( S(0), F6 )
	F_FMUL_S( M(2), F6 )

	F_FLD_S( S(1), F0 )
	F_FMUL_S( M(4), F0 )
	F_FLD_S( S(1), F1 )
	F_FMUL_S( M(5), F1 )
	F_FLD_S( S(1), F2 )
	F_FMUL_S( M(6), F2 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )

	F_FLD_S( S(2), F0 )
	F_FMUL_S( M(8), F0 )
	F_FLD_S( S(2), F1 )
	F_FMUL_S( M(9), F1 )
	F_FLD_S( S(2), F2 )
	F_FMUL_S( M(10), F2 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )

	F_FLD_S( S(3), F0 )
	F_FMUL_S( M(12), F0 )
	F_FLD_S( S(3), F1 )
	F_FMUL_S( M(13), F1 )
	F_FLD_S( S(3), F2 )
	F_FMUL_S( M(14), F2 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )

	MOV_L( S(3), IREG_0 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	F_FSTP_S( F6, D(2) )
	MOV_L( IREG_0, D(3) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points4_3d_no_rot
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points4_3d_no_rot, p4m3dnr, 1, 4)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )

	F_FLD_S( S(1), F5 )
	F_FMUL_S( M(5), F5 )

	F_FLD_S( S(2), F6 )
	F_FMUL_S( M(10), F6 )

	F_FLD_S( S(3), F0 )
	F_FMUL_S( M(12), F0 )
	F_FLD_S( S(3), F1 )
	F_FMUL_S( M(13), F1 )
	F_FLD_S( S(3), F2 )
	F_FMUL_S( M(14), F2 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )
	F_FADDP( F2, F6 )

	MOV_L( S(3), IREG_0 )

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	F_FSTP_S( F6, D(2) )
	MOV_L( IREG_0, D(3) )

LOOP_TAIL

FUNCTION_TAIL

/*
########################################
##
## gl_x86_transform_points4_perspective
##
##
*/
FUNCTION_HEAD(gl_x86_transform_points4_perspective, p4mp, 1, 4)

LOOP_HEAD

	F_FLD_S( S(0), F4 )
	F_FMUL_S( M(0), F4 )

	F_FLD_S( S(1), F5 )
	F_FMUL_S( M(5), F5 )

	F_FLD_S( S(2), F0 )
	F_FMUL_S( M(8), F0 )
	F_FLD_S( S(2), F1 )
	F_FMUL_S( M(9), F1 )
	F_FLD_S( S(2), F6 )
	F_FMUL_S( M(10), F6 )

	F_FADDP( F0, F4 )
	F_FADDP( F1, F5 )

	F_FLD_S( S(3), F2 )
	F_FMUL_S( M(14), F2 )

	F_FADDP( F2, F6 )

	MOV_L( S(2), IREG_0 )
	XOR_L( CONST(eval(0x80000000)), IREG_0 ) 	/* change sign */

	F_FSTP_S( F4, D(0) )
	F_FSTP_S( F5, D(1) )
	F_FSTP_S( F6, D(2) )
	MOV_L( IREG_0, D(3) )

LOOP_TAIL

FUNCTION_TAIL

')dnl
#include "assyntax.h"

	SEG_TEXT

`#'define FP_ONE eval(0x3f800000)
`#'define FP_ZERO 0

`#'if !defined(NASM_ASSEMBLER) && !defined(MASM_ASSEMBLER)
`#'define LLBL(a) .L ## a
`#'else
`#'define LLBL(a) a
`#'endif

`#'define S(i) 	REGOFF(i * 4, ESI)
`#'define D(i) 	REGOFF(i * 4, EDI)
`#'define M(i) 	REGOFF(i * 4, EDX)

/*
 * 	Offsets into GLvector4f
 */
`#'define V4F_DATA 	0
`#'define V4F_START 	4
`#'define V4F_COUNT 	8
`#'define V4F_STRIDE 	12
`#'define V4F_SIZE 	16
`#'define V4F_FLAGS 	20

define(`VEC_DIRTY_0', 1)dnl
define(`VEC_DIRTY_1', 2)dnl
define(`VEC_DIRTY_2', 4)dnl
define(`VEC_DIRTY_3', 8)dnl
`#'define VEC_SIZE_1   eval(VEC_DIRTY_0)
`#'define VEC_SIZE_2   eval(VEC_DIRTY_0|VEC_DIRTY_1)
`#'define VEC_SIZE_3   eval(VEC_DIRTY_0|VEC_DIRTY_1|VEC_DIRTY_2)
`#'define VEC_SIZE_4   eval(VEC_DIRTY_0|VEC_DIRTY_1|VEC_DIRTY_2|VEC_DIRTY_3)

/*
 *	Offsets for transform_func arguments
 *
 *	typedef void (*transform_func)( GLvector4f *to_vec, 
 *	                                const GLmatrix *mat, 
 *	                                const GLvector4f *from_vec, 
 *	                                const GLubyte *clipmask,
 *	                                const GLubyte flag );
 */
`#'define OFFSET_DEST 4
`#'define OFFSET_MATRIX 8
`#'define OFFSET_SOURCE 12
`#'define OFFSET_CLIP 16
`#'define OFFSET_FLAG 20

`#'define ARG_DEST 	REGOFF(FRAME_OFFSET+OFFSET_DEST, ESP)
`#'define ARG_MATRIX 	REGOFF(FRAME_OFFSET+OFFSET_MATRIX, ESP)
`#'define ARG_SOURCE 	REGOFF(FRAME_OFFSET+OFFSET_SOURCE, ESP)
`#'define ARG_CLIP 	REGOFF(FRAME_OFFSET+OFFSET_CLIP, ESP)
`#'define ARG_FLAG 	REGOFF(FRAME_OFFSET+OFFSET_FLAG, ESP)

/*
########################################
##
## Masked versions
##
########################################
*/
divert(-1)
####################
## FUNCTION_HEAD(
##   long_name,
##   short_name,
##   regs_needed,
##   output_vector_size
## );
##
##   EAX: stride or temp0
##   EBX: clipflag
##   ECX: dst + count or temp1
##   EDX: matrix
##   ESI: src[]
##   EDI: dst[]
##   EBP: clipmask[]
##
####################
define(`FUNCTION_HEAD', `dnl
define(`LONG_NAME', $1`'_masked)dnl
define(`SHORT_NAME', $2`'m)dnl
define(`TEMP_REGS_NEEDED', $3)dnl
define(`VEC_SIZE', $4)dnl
	GLOBL GLNAME(LONG_NAME)
	ALIGNTEXT4
GLNAME(LONG_NAME):

`#'define FRAME_OFFSET 16
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
	JZ( LLBL(SHORT_NAME`'_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(`VEC_SIZE_'VEC_SIZE), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(VEC_SIZE), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */

dnl	##########
ifelse(eval(TEMP_REGS_NEEDED > 0), 1,
`	MOV_L( EAX, ARG_SOURCE ) 		/* need eax; put stride in ARG_SOURCE */
define(`IREG_0', `EAX')define(`STRIDE', `ARG_SOURCE')dnl
',
`define(`STRIDE', `EAX')')dnl
dnl	##########
ifelse(eval(TEMP_REGS_NEEDED > 1), 1,
`	MOV_L( ECX, ARG_DEST ) 			/* need ecx; put dest+count in ARG_DEST */
define(`IREG_1', `ECX')define(`DEST_END', `ARG_DEST')dnl
',
`define(`DEST_END', `ECX')')dnl
dnl	##########
')

####################
## LOOP_HEAD
####################
define(`LOOP_HEAD',
`	ALIGNTEXT4ifNOP
LLBL(SHORT_NAME`'_top):

	TEST_B( BL, REGIND(EBP) )
	JNZ( LLBL(SHORT_NAME`'_skip) )
')

####################
## LOOP_TAIL
####################
define(`LOOP_TAIL',
`LLBL(SHORT_NAME`'_skip):
	INC_L( EBP )
	ADD_L( CONST(16), EDI )
	ADD_L( STRIDE, ESI )
	CMP_L( DEST_END, EDI )
	JNE( LLBL(SHORT_NAME`'_top) )
')

####################
## FUNCTION_TAIL
####################
define(`FUNCTION_TAIL',
`LLBL(SHORT_NAME`'_finish):
	POP_L( EBP )
	POP_L( EBX )
	POP_L( EDI )
	POP_L( ESI )
	RET
undefine(`LONG_NAME')dnl
undefine(`SHORT_NAME')dnl
undefine(`TEMP_REGS_NEEDED')dnl
undefine(`VEC_SIZE')dnl
undefine(`IREG_0')dnl
undefine(`IREG_1')dnl
undefine(`STRIDE')dnl
undefine(`DEST_END')dnl
`#'undef FRAME_OFFSET')

####################
divert
output_x86_transform_functions

/*
########################################
##
## Unmasked versions
##
########################################
*/
divert(-1)
####################
## FUNCTION_HEAD(
##   long_name,
##   short_name,
##   regs_needed,
##   output_vector_size
## );
##
##   EAX: stride
##   EBX: temp0
##   ECX: dst + count
##   EDX: matrix
##   ESI: src[]
##   EDI: dst[]
##   EBP: temp1
##
####################
define(`FUNCTION_HEAD', `dnl
define(`LONG_NAME', $1`'_raw)dnl
define(`SHORT_NAME', $2`'r)dnl
define(`TEMP_REGS_NEEDED', $3)dnl
define(`VEC_SIZE', $4)dnl
	GLOBL GLNAME(LONG_NAME)
	ALIGNTEXT4
GLNAME(LONG_NAME):

`#'define FRAME_OFFSET eval(TEMP_REGS_NEEDED * 4 + 8)
	PUSH_L( ESI )
	PUSH_L( EDI )

	MOV_L( REGOFF(OFFSET_SOURCE+8, ESP), ESI ) 	/* ptr to source GLvector4f */
	MOV_L( REGOFF(OFFSET_DEST+8, ESP), EDI ) 	/* ptr to dest GLvector4f */

ifelse(eval(TEMP_REGS_NEEDED > 0), 1,
`	PUSH_L( EBX )define(`IREG_0', EBX)
')dnl
ifelse(eval(TEMP_REGS_NEEDED > 1), 1,
`	PUSH_L( EBP )define(`IREG_1', EBP)
')dnl

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( REGOFF(V4F_COUNT, ESI), ECX ) 	/* source count */

	TEST_L( ECX, ECX)
	JZ( LLBL(SHORT_NAME`'_finish) ) 	/* count was zero; go to finish */

	MOV_L( REGOFF(V4F_STRIDE, ESI), EAX ) 	/* stride */
	OR_L( CONST(`VEC_SIZE_'VEC_SIZE), REGOFF(V4F_FLAGS, EDI) ) 	/* set dest flags */

	MOV_L( ECX, REGOFF(V4F_COUNT, EDI) ) 	/* set dest count */
	MOV_L( CONST(VEC_SIZE), REGOFF(V4F_SIZE, EDI) ) 	/* set dest size */

	SHL_L( CONST(4), ECX ) 			/* count *= 16 */
	MOV_L( REGOFF(V4F_START, ESI), ESI ) 	/* ptr to first source vertex */

	MOV_L( REGOFF(V4F_START, EDI), EDI ) 	/* ptr to first dest vertex */
	ADD_L( EDI, ECX ) 			/* count += dest ptr */
')

####################
## LOOP_HEAD
####################
define(`LOOP_HEAD',
`	ALIGNTEXT4ifNOP
LLBL(SHORT_NAME`'_top):
')

####################
## LOOP_TAIL
####################
define(`LOOP_TAIL',
`LLBL(SHORT_NAME`'_skip):
	ADD_L( CONST(16), EDI )
	ADD_L( EAX, ESI )
	CMP_L( ECX, EDI )
	JNE( LLBL(SHORT_NAME`'_top) )
')

####################
## FUNCTION_TAIL
####################
define(`FUNCTION_TAIL',
`LLBL(SHORT_NAME`'_finish):
ifelse(eval(TEMP_REGS_NEEDED > 1), 1,
`	POP_L( EBP )
')dnl
ifelse(eval(TEMP_REGS_NEEDED > 0), 1,
`	POP_L( EBX )
')dnl
	POP_L( EDI )
	POP_L( ESI )
	RET
undefine(`LONG_NAME')dnl
undefine(`SHORT_NAME')dnl
undefine(`TEMP_REGS_NEEDED')dnl
undefine(`VEC_SIZE')dnl
undefine(`IREG_0')dnl
undefine(`IREG_1')dnl
`#'undef FRAME_OFFSET')


####################
divert
output_x86_transform_functions

`#'undef OFFSET_DEST
`#'undef OFFSET_MATRIX
`#'undef OFFSET_SOURCE
`#'undef OFFSET_CLIP
`#'undef OFFSET_FLAG

`#'undef ARG_DEST
`#'undef ARG_MATRIX
`#'undef ARG_SOURCE
`#'undef ARG_CLIP
`#'undef ARG_FLAG

`#'define OFFSET_DEST 4
`#'define OFFSET_MATRIX 8
`#'define OFFSET_SOURCE 12
`#'define OFFSET_STRIDE 16
`#'define OFFSET_COUNT 20

`#'define ARG_DEST 	REGOFF(FRAME_OFFSET+OFFSET_DEST, ESP)
`#'define ARG_MATRIX 	REGOFF(FRAME_OFFSET+OFFSET_MATRIX, ESP)
`#'define ARG_SOURCE 	REGOFF(FRAME_OFFSET+OFFSET_SOURCE, ESP)
`#'define ARG_STRIDE 	REGOFF(FRAME_OFFSET+OFFSET_STRIDE, ESP)
`#'define ARG_COUNT 	REGOFF(FRAME_OFFSET+OFFSET_COUNT, ESP)

/*
########################################
##
## clean (no vertex struct) versions
##
########################################
*/
divert(-1)
####################
## FUNCTION_HEAD(
##   long_name,
##   short_name,
##   regs_needed,
##   output_vector_size
## );
##
##   EAX: stride
##   EBX: temp0
##   ECX: dst + count
##   EDX: matrix
##   ESI: src[]
##   EDI: dst[]
##   EBP: temp1
##
####################
define(`FUNCTION_HEAD', `dnl
define(`LONG_NAME', $1`'_v`'DEST_STRIDE)dnl
define(`SHORT_NAME', $2`'v`'DEST_STRIDE)dnl
define(`TEMP_REGS_NEEDED', $3)dnl
define(`VEC_SIZE', $4)dnl
	GLOBL GLNAME(LONG_NAME)
	ALIGNTEXT4
GLNAME(LONG_NAME):

`#'define FRAME_OFFSET eval(TEMP_REGS_NEEDED * 4 + 8)
	PUSH_L( ESI )
	PUSH_L( EDI )

ifelse(eval(TEMP_REGS_NEEDED > 0), 1,
`	PUSH_L( EBX )define(`IREG_0', EBX)
')dnl
ifelse(eval(TEMP_REGS_NEEDED > 1), 1,
`	PUSH_L( EBP )define(`IREG_1', EBP)
')dnl

	MOV_L( ARG_SOURCE, ESI ) 	/* ptr to source */
	MOV_L( ARG_DEST, EDI ) 	/* ptr to dest */

	MOV_L( ARG_MATRIX, EDX ) 		/* ptr to matrix */
	MOV_L( ARG_COUNT, ECX ) 	/* count */

	TEST_L( ECX, ECX)
	JZ( LLBL(SHORT_NAME`'_finish) ) 	/* count was zero; go to finish */

	MOV_L( ARG_STRIDE, EAX ) 	/* stride */
')

####################
## LOOP_HEAD
####################
define(`LOOP_HEAD',
`	ALIGNTEXT4ifNOP
LLBL(SHORT_NAME`'_top):
')

####################
## LOOP_TAIL
####################
define(`LOOP_TAIL',
`ifelse(eval(VEC_SIZE == 1), 1, `	MOV_L( CONST(FP_ZERO), D(1) )
')dnl
ifelse(eval(VEC_SIZE <= 2), 1, `	MOV_L( CONST(FP_ZERO), D(2) )
')dnl
ifelse(eval(VEC_SIZE <= 3), 1, `	MOV_L( CONST(FP_ONE), D(3) )
')dnl
LLBL(SHORT_NAME`'_skip):
	ADD_L( CONST(eval(DEST_STRIDE*4)), EDI )
	ADD_L( EAX, ESI )
	DEC_L( ECX )
	JNZ( LLBL(SHORT_NAME`'_top) )
')

####################
## FUNCTION_TAIL
####################
define(`FUNCTION_TAIL',
`LLBL(SHORT_NAME`'_finish):
ifelse(eval(TEMP_REGS_NEEDED > 1), 1,
`	POP_L( EBP )
')dnl
ifelse(eval(TEMP_REGS_NEEDED > 0), 1,
`	POP_L( EBX )
')dnl
	POP_L( EDI )
	POP_L( ESI )
	RET
undefine(`LONG_NAME')dnl
undefine(`SHORT_NAME')dnl
undefine(`TEMP_REGS_NEEDED')dnl
undefine(`VEC_SIZE')dnl
undefine(`IREG_0')dnl
undefine(`IREG_1')dnl
`#'undef FRAME_OFFSET')


####################
divert

define(`DEST_STRIDE', 8)dnl
output_x86_transform_functions
undefine(`DEST_STRIDE')dnl

define(`DEST_STRIDE', 12)dnl
output_x86_transform_functions
undefine(`DEST_STRIDE')dnl

define(`DEST_STRIDE', 16)dnl
output_x86_transform_functions
undefine(`DEST_STRIDE')dnl

`#'undef OFFSET_DEST
`#'undef OFFSET_MATRIX
`#'undef OFFSET_SOURCE
`#'undef OFFSET_STRIDE
`#'undef OFFSET_COUNT

`#'undef ARG_DEST
`#'undef ARG_MATRIX
`#'undef ARG_SOURCE
`#'undef ARG_STRIDE
`#'undef ARG_COUNT


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
divert(-1)
# Vertex buffer clipping flags (from vb.h)
define(`CLIP_RIGHT_BIT',  `0x01')
define(`CLIP_LEFT_BIT',   `0x02')
define(`CLIP_TOP_BIT',    `0x04')
define(`CLIP_BOTTOM_BIT', `0x08')
define(`CLIP_NEAR_BIT',   `0x10')
define(`CLIP_FAR_BIT',    `0x20')

define(`MAGN_X', `(~((($1) & 1) - 1))')
define(`SIGN_X', `(~(((($1) >> 1) & 1) - 1))')
define(`MAGN_Y', `(~(((($1) >> 2) & 1) - 1))')
define(`SIGN_Y', `(~(((($1) >> 3) & 1) - 1))')
define(`MAGN_Z', `(~(((($1) >> 4) & 1) - 1))')
define(`SIGN_Z', `(~(((($1) >> 5) & 1) - 1))')
define(`SIGN_W', `(~(((($1) >> 6) & 1) - 1))')

define(`CLIP_VALUE', `eval(
	(CLIP_RIGHT_BIT
	  & ((~SIGN_X($1) & SIGN_W($1))
	     | (~SIGN_X($1) & ~SIGN_W($1) & MAGN_X($1))
	     | (SIGN_X($1) & SIGN_W($1) & ~MAGN_X($1))))
	 | (CLIP_LEFT_BIT
	    & ((SIGN_X($1) & SIGN_W($1))
	       | (~SIGN_X($1) & SIGN_W($1) & ~MAGN_X($1))
	       | (SIGN_X($1) & ~SIGN_W($1) & MAGN_X($1))))
	 | (CLIP_TOP_BIT
	    & ((~SIGN_Y($1) & SIGN_W($1))
	       | (~SIGN_Y($1) & ~SIGN_W($1) & MAGN_Y($1))
	       | (SIGN_Y($1) & SIGN_W($1) & ~MAGN_Y($1))))
	 | (CLIP_BOTTOM_BIT
	    & ((SIGN_Y($1) & SIGN_W($1))
	       | (~SIGN_Y($1) & SIGN_W($1) & ~MAGN_Y($1))
	       | (SIGN_Y($1) & ~SIGN_W($1) & MAGN_Y($1))))
	 | (CLIP_FAR_BIT
	    & ((~SIGN_Z($1) & SIGN_W($1))
	       | (~SIGN_Z($1) & ~SIGN_W($1) & MAGN_Z($1))
	       | (SIGN_Z($1) & SIGN_W($1) & ~MAGN_Z($1))))
	 | (CLIP_NEAR_BIT
	    & ((SIGN_Z($1) & SIGN_W($1))
	       | (~SIGN_Z($1) & SIGN_W($1) & ~MAGN_Z($1))
	       | (SIGN_Z($1) & ~SIGN_W($1) & MAGN_Z($1))))
)')
define(`CLIP_VALUE8',
`CLIP_VALUE($1 + 0), CLIP_VALUE($1 + 1), CLIP_VALUE($1 + 2), CLIP_VALUE($1 + 3), dnl
CLIP_VALUE($1 + 4), CLIP_VALUE($1 + 5), CLIP_VALUE($1 + 6), CLIP_VALUE($1 + 7)')
divert

	SEG_DATA

clip_table:
	D_BYTE CLIP_VALUE8(0x00)
	D_BYTE CLIP_VALUE8(0x08)
	D_BYTE CLIP_VALUE8(0x10)
	D_BYTE CLIP_VALUE8(0x18)
	D_BYTE CLIP_VALUE8(0x20)
	D_BYTE CLIP_VALUE8(0x28)
	D_BYTE CLIP_VALUE8(0x30)
	D_BYTE CLIP_VALUE8(0x38)
	D_BYTE CLIP_VALUE8(0x40)
	D_BYTE CLIP_VALUE8(0x48)
	D_BYTE CLIP_VALUE8(0x50)
	D_BYTE CLIP_VALUE8(0x58)
	D_BYTE CLIP_VALUE8(0x60)
	D_BYTE CLIP_VALUE8(0x68)
	D_BYTE CLIP_VALUE8(0x70)
	D_BYTE CLIP_VALUE8(0x78)

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

`#'define OFFSET_SOURCE 4
`#'define OFFSET_DEST 8
`#'define OFFSET_CLIP 12
`#'define OFFSET_OR 16
`#'define OFFSET_AND 20

`#'define ARG_SOURCE 	REGOFF(FRAME_OFFSET+OFFSET_SOURCE, ESP)
`#'define ARG_DEST 	REGOFF(FRAME_OFFSET+OFFSET_DEST, ESP)
`#'define ARG_CLIP 	REGOFF(FRAME_OFFSET+OFFSET_CLIP, ESP)
`#'define ARG_OR 	REGOFF(FRAME_OFFSET+OFFSET_OR, ESP)
`#'define ARG_AND 	REGOFF(FRAME_OFFSET+OFFSET_AND, ESP)

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

`#'if defined(__ELF__) && defined(__PIC__) && !defined(ELFPIC)
`#'define ELFPIC
`#'endif

	GLOBL GLNAME(gl_x86_cliptest_points4)
	ALIGNTEXT4

GLNAME(gl_x86_cliptest_points4):
`#'ifdef ELFPIC
`#'define FRAME_OFFSET 20
`#'else
`#'define FRAME_OFFSET 16
`#'endif
	PUSH_L( ESI )
	PUSH_L( EDI )
	PUSH_L( EBP )
	PUSH_L( EBX )

`#'ifdef ELFPIC
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
`#'endif

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
	F_PUSH0( FLD1, F3 )
	F_FDIV_S( S(3), F3 )

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

`#'ifdef ELFPIC
	MOV_L( REGIND(ESP), EBP ) 	/* clip_table */

	MOV_B( REGBI(EBP, ECX), CL )
`#'else
	MOV_B( REGOFF(clip_table,ECX), CL )
`#'endif

	OR_B( CL, AL )
	AND_B( CL, AH )

	TEST_B( CL, CL )
	MOV_B( CL, REGIND(EDX) )

	JZ( LLBL(ctp4_proj) )

pushdef(`F_STACK', F_STACK)dnl
	F_FSTP( F3, F3 )
popdef(`F_STACK')dnl
	JMP( LLBL(ctp4_next) )

LLBL(ctp4_proj):
	F_FLD_S( S(0), F0 )
	F_FMUL2( F3, F0 )

	F_FLD_S( S(1), F1 )
	F_FMUL2( F3, F1 )

	F_FLD_S( S(2), F2 )
	F_FMUL2( F3, F2 )

	F_FSTP_S( F0, D(0) )
	F_FSTP_S( F1, D(1) )
	F_FSTP_S( F2, D(2) )
	F_FSTP_S( F3, D(3) )

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
`#'ifdef ELFPIC
	POP_L( ESI ) 	/* discard ptr to clip_table */
`#'endif
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



`#'define OFFSET_V16_SOURCE 4
`#'define OFFSET_V16_LAST 8
`#'define OFFSET_V16_OR 12
`#'define OFFSET_V16_AND 16
`#'define OFFSET_V16_MASK 20

`#'define ARG_V16_SOURCE	REGOFF(V16_FRAME_OFFSET+OFFSET_V16_SOURCE, ESP)
`#'define ARG_V16_LAST 	REGOFF(V16_FRAME_OFFSET+OFFSET_V16_LAST, ESP)
`#'define ARG_V16_OR 	REGOFF(V16_FRAME_OFFSET+OFFSET_V16_OR, ESP)
`#'define ARG_V16_AND 	REGOFF(V16_FRAME_OFFSET+OFFSET_V16_AND, ESP)
`#'define ARG_V16_MASK 	REGOFF(V16_FRAME_OFFSET+OFFSET_V16_MASK, ESP)


`#'if defined(__ELF__) && defined(__PIC__) && !defined(ELFPIC)
`#'define ELFPIC
`#'endif

	GLOBL GLNAME(gl_v16_x86_cliptest_points4)
	ALIGNTEXT4

GLNAME(gl_v16_x86_cliptest_points4):
`#'ifdef ELFPIC
`#'define V16_FRAME_OFFSET 20
`#'else
`#'define V16_FRAME_OFFSET 16
`#'endif
	PUSH_L( ESI )
	PUSH_L( EDI )
	PUSH_L( EBP )
	PUSH_L( EBX )

`#'ifdef ELFPIC
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
`#'endif

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
`#'if 0
	F_PUSH0( FLD1, F0 )
	F_FDIV_S( S(3), F0 )
`#'endif

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

`#'ifdef ELFPIC
	MOV_L( REGIND(ESP), EBP ) 	/* clip_table */

	MOV_B( REGBI(EBP, ECX), CL )
`#'else
	MOV_B( REGOFF(clip_table,ECX), CL )
`#'endif

	OR_B( CL, AL )
	AND_B( CL, AH )

	MOV_B( CL, REGIND(EDI) )     /* save clipmask */
	INC_L( EDI )                 /* next clipmask */

`#'if 0
	F_FSTP_S( F0, S(8) ) 	/* GR_VERTEX_OOW_OFFSET */
`#'endif

	ADD_L( CONST(64), ESI )      /* next fxVertex  */

	CMP_L( EDX, ESI )            /* finished? */
	JNZ( LLBL(v16_ctp4_top) )

	MOV_L( ARG_V16_OR, ECX )
	MOV_L( ARG_V16_AND, EDX )

	MOV_B( AL, REGIND(ECX) )
	MOV_B( AH, REGIND(EDX) )

LLBL(v16_ctp4_finish):

`#'ifdef ELFPIC
	POP_L( ESI ) 	/* discard ptr to clip_table */
`#'endif
	POP_L( EBX )
	POP_L( EBP )
	POP_L( EDI )
	POP_L( ESI )

	RET

