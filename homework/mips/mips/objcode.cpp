#include "objcode.h"

Objcode::Objcode(string mipsFile) {
	this->mips_.open(mipsFile);
}

void Objcode::Output(Instr instr) {
	switch (instr) {
	case(Instr::syscall):
		mips_ << "syscall" << endl;
		break;
	case(Instr::data):
		mips_ << endl << ".data" << endl;
		break;
	case(Instr::text):
		mips_ << ".text" << endl << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0) {
	switch (instr) {
	case(Instr::jr):
		mips_ << "jr " << RegToString(t0) << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, int value) {
	switch (instr) {
	case(Instr::data_align):
		mips_ << '\t' << ".align " << value << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, string label) {
	switch (instr) {
	case(Instr::jal):
		mips_ << "jal " << label << endl;
		break;
	case(Instr::j):
		mips_ << "j " << label << endl;
		break;
	case(Instr::label):
		mips_ << endl << label << ":" << endl;
		break;
	case(Instr::data_string):
		mips_ << '\t' << label << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0, Reg t1) {
	switch (instr) {
	case(Instr::move):
		mips_ << "move " << RegToString(t0) << " " << RegToString(t1) << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0, int value) {
	switch (instr) {
	case(Instr::li):
		mips_ << "li " << RegToString(t0) << " " << value << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0, string label) {
	switch (instr) {
	case(Instr::la):
		mips_ << "la " << RegToString(t0) << " " << label << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0, Reg t1, Reg t2) {
	switch (instr) {
	case(Instr::add):
		mips_ << "add " << RegToString(t0) << " " << RegToString(t1) << " " << RegToString(t2) << endl;
		break;
	case(Instr::sub):
		mips_ << "sub " << RegToString(t0) << " " << RegToString(t1) << " " << RegToString(t2) << endl;
		break;
	case(Instr::mul):
		mips_ << "mul " << RegToString(t0) << " " << RegToString(t1) << " " << RegToString(t2) << endl;
		break;
	case(Instr::div):
		mips_ << "div " << RegToString(t0) << " " << RegToString(t1) << " " << RegToString(t2) << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, string label, int value) {
	switch (instr) {
	case(Instr::data_identifier):
		mips_ << '\t' << label << ": .space " << value << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0, Reg t1, int value) {
	switch (instr) {
	case(Instr::addi):
		mips_ << "addi " << RegToString(t0) << " " << RegToString(t1) << " " << value << endl;
		break;
	case(Instr::subi):
		mips_ << "subi " << RegToString(t0) << " " << RegToString(t1) << " " << value << endl;
		break;
	case(Instr::sll):
		mips_ << "sll " << RegToString(t0) << " " << RegToString(t1) << " " << value << endl;
		break;
	case(Instr::lw):
		mips_ << "lw " << RegToString(t0) << " " << value << "(" << RegToString(t1) << ")" << endl;
		break;
	case(Instr::sw):
		mips_ << "sw " << RegToString(t0) << " " << value << "(" << RegToString(t1) << ")" << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0, Reg t1, string label) {
	switch (instr) {
	case(Instr::lw):
		mips_ << "lw " << RegToString(t0) << " " << label << "(" << RegToString(t1) << ")" << endl;
		break;
	case(Instr::sw):
		mips_ << "sw " << RegToString(t0) << " " << label << "(" << RegToString(t1) << ")" << endl;
		break;
	case(Instr::bgt):
		mips_ << "bgt " << RegToString(t0) << " " << RegToString(t1) << " " << label << endl;
		break;
	case(Instr::bge):
		mips_ << "bge " << RegToString(t0) << " " << RegToString(t1) << " " << label << endl;
		break;
	case(Instr::blt):
		mips_ << "blt " << RegToString(t0) << " " << RegToString(t1) << " " << label << endl;
		break;
	case(Instr::ble):
		mips_ << "ble " << RegToString(t0) << " " << RegToString(t1) << " " << label << endl;
		break;
	case(Instr::beq):
		mips_ << "beq " << RegToString(t0) << " " << RegToString(t1) << " " << label << endl;
		break;
	case(Instr::bne):
		mips_ << "bne " << RegToString(t0) << " " << RegToString(t1) << " " << label << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::FileClose() {
	this->mips_.close();
}