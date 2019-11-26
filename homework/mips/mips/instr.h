#pragma once

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

	label,
	data,
	data_identifier,
	data_string,
	data_align,
	text,
};

namespace midcodeinstr{
	const string FUNC_DECLARE = "FUNC_DECLARE";
	const string VOID_FUNC_DECLARE = "VOID_FUNC_DECLARE";
	const string PUSH = "PUSH";
	const string CALL = "CALL";
	const string SAVE = "SAVE";
	const string RETURN = "RETURN";

	const string SCANF = "SCANF";
	const string PRINTF = "PRINTF";
	const string PRINTF_END = "PRINTF_END";

	const string LABEL = "LABEL";
	const string GO_TO_LABEL = "GO_TO_LABEL";

	const string ASSIGN = "ASSIGN";
	const string ASSIGN_RETURN = "ASSIGN_RETURN";
	const string LOAD = "LOAD";


	const string REG_OP_NUMBER = "REG_OP_NUMBER";
	const string REG_OP_REG = "REG_OP_REG";
	const string NUMBER_OP_REG = "NUMBER_OP_REG";
	const string NUMBER_OP_NUMBER = "NUMBER_OP_NUMBER";

	const string BEZ = "BEZ";
	const string BNZ = "BNZ";
	const string BEQ = "BEQ";
	const string BNE = "BNE";
	const string BGE = "BGE";
	const string BLT = "BLT";
	const string BGT = "BGT";
	const string BLE = "BLE";
}