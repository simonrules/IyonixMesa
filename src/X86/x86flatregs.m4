divert(-1)
############################################################
## These m4 macros make the x86 floating point register
## stack look more like normal registers.
##
## They do *NOT* know about jump instructions or any other
## form of conditional execution!
############################################################

########################################
## define the stack
########################################
define(`F_STACK', `')

########################################
## print the stack
########################################
define(`F_SHOW_STACK', `patsubst(F_STACK, `[0-7]', `F`'\& ')')

########################################
## define the flat floating point regs
########################################
define(`F0', 0)
define(`F1', 1)
define(`F2', 2)
define(`F3', 3)
define(`F4', 4)
define(`F5', 5)
define(`F6', 6)
define(`F7', 7)

########################################
## F_PUSH_STACK(REG)
## push a register
########################################
define(`F_PUSH_STACK',
`ifelse(len(F_STACK), 8, ` [ERROR - STACK OVERFLOW] ')dnl
ifelse(eval(F_FIND_REG($1) != -1), 1, ` [ERROR - F`'$1 PUSHED TWICE] ')dnl
define(`F_STACK', eval($1)`'F_STACK)')

########################################
## F_POP_STACK()
## pop top of stack
########################################
define(`F_POP_STACK', 
`ifelse(len(F_STACK), 0, ` [ERROR - STACK UNDERFLOW] ')dnl
define(`F_STACK', substr(F_STACK, 1))')

########################################
## F_FIND_REG(REG)
## find register
########################################
define(`F_FIND_REG', `index(F_STACK, eval($1))')

########################################
## F_XCH_TO_TOP(REG)
## swap to top of stack
########################################
define(`F_XCH_TO_TOP',
`define(`F_INDEX', F_FIND_REG($1))dnl
ifelse(
eval(F_INDEX > 0), 1,
`FXCH( ST('F_FIND_REG($1)`) )dnl
define(`F_STACK',
 translit(F_STACK,
 substr(F_STACK, 0, 1)`'eval($1),
 eval($1)`'substr(F_STACK, 0, 1)))F_PCOMMENT(`F$1 to top')
	',
eval(F_INDEX == -1), 1,
` [ERROR - F`'$1 NOT ON STACK] ')')

########################################
## F_PUSH(INSN, SRC, DST)
## SRC - memory
## DST - register
## execute an insn that pushes a register
########################################
define(`F_PUSH',
`$1( $2 )F_PUSH_STACK($3)F_PCOMMENT(`$1 $2, F$3')')

########################################
## F_PUSHR(INSN, SRC, DST)
## SRC - register
## DST - register
## execute an insn that pushes a register
########################################
define(`F_PUSHR',
`$1( ST(F_FIND_REG($2) )F_PUSH_STACK($3)F_PCOMMENT(`$1 F$2, F$3')')

########################################
## F_PUSH0(INSN, DST)
## DST - register
## execute an insn that pushes a register
########################################
define(`F_PUSH0',
`$1`'F_PUSH_STACK($2)F_PCOMMENT(`$1 F$3')')

########################################
## F_POP(INSN, SRC, DST)
## SRC - register
## DST - memory
## execute an insn that pops a register
########################################
define(`F_POP',
`F_XCH_TO_TOP($2)dnl
$1( $3 )F_POP_STACK()F_PCOMMENT(`$1 F$2, $3')')

########################################
## F_POPR(INSN, SRC, DST)
## SRC - register
## DST - register
## execute an insn that pops a register
########################################
define(`F_POPR',
`F_XCH_TO_TOP($2)dnl
$1( ST(0), ST(F_FIND_REG($3)) )F_POP_STACK()F_PCOMMENT(`$1 F$2, F$3')')

########################################
## F_POP1(INSN, SRC, DST)
## SRC - register
## DST - register
## execute an insn that pops a register
## with st(0) arg omitted
########################################
define(`F_POP1',
`F_XCH_TO_TOP($2)dnl
$1( ST(F_FIND_REG($3)) )F_POP_STACK()F_PCOMMENT(`$1 F$2, F$3')')

########################################
## F_EXEC(INSN, SRC, DST)
## SRC - memory
## DST - register
## execute an insn that doesn't change the stack
########################################
define(`F_EXEC',
`F_XCH_TO_TOP($3)dnl
$1( $2 )F_COMMENT(`$1 $2, F$3')')

########################################
## F_EXECR(INSN, SRC, DST)
## SRC - register
## DST - register
## execute an insn that doesn't change the stack
########################################
define(`F_EXECR',
`F_XCH_TO_TOP($3)dnl
$1( ST(F_FIND_REG($2)), ST(0) )F_COMMENT(`$1 F$2, F$3')')

########################################
## F_EXEC0(INSN, REG)
## REG - register
## execute an insn that doesn't change
## the stack or have any arguments
## besides the stack top.
########################################
define(`F_EXEC0',
`F_XCH_TO_TOP($2)dnl
$1`'F_COMMENT(`$1 F$2')')

########################################
## Some common fp operations -
########################################
define(`F_FADD2', `F_EXECR( FADD2, $1, $2 )')
define(`F_FADD_S', `F_EXEC( FADD_S, $1, $2 )')
define(`F_FADDP', `F_POPR( FADDP, $1, $2 )')

define(`F_FDIV2', `F_EXECR( FDIV2, $1, $2 )')
define(`F_FDIV_S', `F_EXEC( FDIV_S, $1, $2 )')
define(`F_FDIVP', `F_POPR( FDIVP, $1, $2 )')

define(`F_FDIVR2', `F_EXECR( FDIVR2, $1, $2 )')
define(`F_FDIVR_S', `F_EXEC( FDIVR_S, $1, $2 )')
define(`F_FDIVRP', `F_POPR( FDIVRP, $1, $2 )')

define(`F_FLD_S', `F_PUSH( FLD_S, $1, $2 )')

define(`F_FMUL2', `F_EXECR( FMUL2, $1, $2 )')
define(`F_FMUL_S', `F_EXEC( FMUL_S, $1, $2 )')
define(`F_FMULP', `F_POPR( FMULP, $1, $2 )')

define(`F_FSTP_S', `F_POP( FSTP_S, $1, $2 )')
define(`F_FSTP', `F_POP1( FSTP, $1, $2 )')

define(`F_FSUB2', `F_EXECR( FSUB2, $1, $2 )')
define(`F_FSUB_S', `F_EXEC( FSUB_S, $1, $2 )')
define(`F_FSUBP', `F_POPR( FSUBP, $1, $2 )')

define(`F_FSUBR2', `F_EXECR( FSUBR2, $1, $2 )')
define(`F_FSUBR_S', `F_EXEC( FSUBR_S, $1, $2 )')
define(`F_FSUBRP', `F_POPR( FSUBRP, $1, $2 )')

########################################
## Comments
########################################
## define(`F_COMMENT', ` 	/* `$1' */')
## define(`F_PCOMMENT', ` 	/* `$1' { F_SHOW_STACK} */')
define(`F_COMMENT', `')
define(`F_PCOMMENT', ` 	/* F_SHOW_STACK*/')

divert`'dnl
