#pragma once

#include <string>
#include <cassert>

using namespace std;

enum class MipsInstr {
	add, addi,
	sub, subi,
	mul, div,
	lw, sw,

	bgt, bge,
	blt, ble,
	beq,
	bne,

	jal, jr, j,

	la, li, move, sll,

	syscall,

	nop,

	label,
	data,
	data_identifier,
	data_string,
	data_align,
	text,
};

enum class MidcodeInstr {

	SCANF_INT,
	SCANF_CHAR,

	PRINTF_INT,
	PRINTF_CHAR,
	PRINTF_STRING,
	PRINTF_END,

	LABEL,
	JUMP,
	LOOP,
	STEP,

	ASSIGN,
	ASSIGN_ARRAY,
	ASSIGN_RETURN,

	LOAD,
	LOAD_ARRAY,

	ADD,		// t0 = t1 + t2
	ADDI,
	SUB,		// t0 = t1 - t2
	SUBI,
	NEG,		// t0 = - t1
	MUL,		// t0 = t1 * t2
	DIV,		// t0 = t1 / t2

	BGT,		// branch to t3 if t1 > t2
	BGE,		// branch to t3 if t1 >= t2
	BLT,		// branch to t3 if t1 < t2
	BLE,		// branch to t3 if t1 <= t2
	BEQ,		// branch to t3 if t1 == t2(nullable)
	BNE,		// branch to t3 if t1 != t2(nullable)
	BEZ,		// branch if t1 == zero
	BNZ,		// branch if t1 != zero

	INT_FUNC_DECLARE,
	CHAR_FUNC_DECLARE,
	VOID_FUNC_DECLARE,

	PUSH,
	CALL,
	SAVE,
	FUNCTION_END,
	RETURN,
	RETURN_NON,

	PARA_INT,
	PARA_CHAR,
	CONST_INT,
	CONST_CHAR,

	VAR_INT,
	VAR_CHAR,
};

enum class OperaMember {
	REG_OP_NUMBER,
	REG_OP_REG,
	NUMBER_OP_REG,
	NUMBER_OP_NUMBER
};

namespace midcodeinstr {
	MidcodeInstr GetOperatorInstr(string op);
}
