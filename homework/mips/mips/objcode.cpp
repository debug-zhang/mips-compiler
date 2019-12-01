#include "objcode.h"

Objcode::Objcode(string mipsFile) {
	this->mips_.open(mipsFile);
}

void Objcode::Output() {
	mips_ << endl;
}

void Objcode::Output(MipsInstr instr) {
	switch (instr) {
	case(MipsInstr::syscall):
		mips_ << "syscall" << endl;
		break;
	case(MipsInstr::nop):
		mips_ << "nop" << endl;
		break;
	case(MipsInstr::data):
		mips_ << ".data" << endl;
		break;
	case(MipsInstr::text):
		mips_ << ".text" << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(MipsInstr instr, Reg t0) {
	switch (instr) {
	case(MipsInstr::jr):
		mips_ << "jr " << reg::RegToString(t0) << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(MipsInstr instr, int value) {
	switch (instr) {
	case(MipsInstr::data_align):
		mips_ << '\t' << ".align " << value << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(MipsInstr instr, string label) {
	switch (instr) {
	case(MipsInstr::jal):
		mips_ << "jal " << label << endl;
		break;
	case(MipsInstr::j):
		mips_ << "j " << label << endl;
		break;
	case(MipsInstr::label):
		mips_ << endl << label << ":" << endl;
		break;
	case(MipsInstr::data_string):
		mips_ << '\t' << label << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(MipsInstr instr, Reg t0, Reg t1) {
	switch (instr) {
	case(MipsInstr::move):
		mips_ << "move " << reg::RegToString(t0) << " " << reg::RegToString(t1) << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(MipsInstr instr, Reg t0, int value) {
	switch (instr) {
	case(MipsInstr::li):
		mips_ << "li " << reg::RegToString(t0) << " " << value << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(MipsInstr instr, Reg t0, string label) {
	switch (instr) {
	case(MipsInstr::la):
		mips_ << "la " << reg::RegToString(t0) << " " << label << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(MipsInstr instr, Reg t0, Reg t1, Reg t2) {
	switch (instr) {
	case(MipsInstr::add):
		mips_ << "add " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << reg::RegToString(t2) << endl;
		break;
	case(MipsInstr::sub):
		mips_ << "sub " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << reg::RegToString(t2) << endl;
		break;
	case(MipsInstr::mul):
		mips_ << "mul " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << reg::RegToString(t2) << endl;
		break;
	case(MipsInstr::div):
		mips_ << "div " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << reg::RegToString(t2) << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(MipsInstr instr, string label, int value) {
	switch (instr) {
	case(MipsInstr::data_identifier):
		mips_ << '\t' << label << ": .space " << value << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(MipsInstr instr, Reg t0, Reg t1, int value) {
	switch (instr) {
	case(MipsInstr::addi):
		mips_ << "addi " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << value << endl;
		break;
	case(MipsInstr::subi):
		mips_ << "subi " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << value << endl;
		break;
	case(MipsInstr::mul):
		mips_ << "mul " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << value << endl;
		break;
	case(MipsInstr::div):
		mips_ << "div " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << value << endl;
		break;
	case(MipsInstr::sll):
		mips_ << "sll " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << value << endl;
		break;
	case(MipsInstr::lw):
		mips_ << "lw " << reg::RegToString(t0) << " " << value << "("
			<< reg::RegToString(t1) << ")" << endl;
		break;
	case(MipsInstr::sw):
		mips_ << "sw " << reg::RegToString(t0) << " " << value << "("
			<< reg::RegToString(t1) << ")" << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(MipsInstr instr, Reg t0, Reg t1, string label) {
	switch (instr) {
	case(MipsInstr::lw):
		mips_ << "lw " << reg::RegToString(t0) << " " << label << "("
			<< reg::RegToString(t1) << ")" << endl;
		break;
	case(MipsInstr::sw):
		mips_ << "sw " << reg::RegToString(t0) << " " << label << "("
			<< reg::RegToString(t1) << ")" << endl;
		break;
	case(MipsInstr::bgt):
		mips_ << "bgt " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	case(MipsInstr::bge):
		mips_ << "bge " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	case(MipsInstr::blt):
		mips_ << "blt " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	case(MipsInstr::ble):
		mips_ << "ble " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	case(MipsInstr::beq):
		mips_ << "beq " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	case(MipsInstr::bne):
		mips_ << "bne " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::FileClose() {
	this->mips_.close();
}