#pragma once

enum class Instr {
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