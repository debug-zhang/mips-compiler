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
		mips_ << "jr " << reg::RegToString(t0) << endl;
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
		mips_ << "move " << reg::RegToString(t0) << " " << reg::RegToString(t1) << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0, int value) {
	switch (instr) {
	case(Instr::li):
		mips_ << "li " << reg::RegToString(t0) << " " << value << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0, string label) {
	switch (instr) {
	case(Instr::la):
		mips_ << "la " << reg::RegToString(t0) << " " << label << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0, Reg t1, Reg t2) {
	switch (instr) {
	case(Instr::add):
		mips_ << "add " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << reg::RegToString(t2) << endl;
		break;
	case(Instr::sub):
		mips_ << "sub " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << reg::RegToString(t2) << endl;
		break;
	case(Instr::mul):
		mips_ << "mul " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << reg::RegToString(t2) << endl;
		break;
	case(Instr::div):
		mips_ << "div " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << reg::RegToString(t2) << endl;
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
		mips_ << "addi " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << value << endl;
		break;
	case(Instr::subi):
		mips_ << "subi " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << value << endl;
		break;
	case(Instr::sll):
		mips_ << "sll " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << value << endl;
		break;
	case(Instr::lw):
		mips_ << "lw " << reg::RegToString(t0) << " " << value << "("
			<< reg::RegToString(t1) << ")" << endl;
		break;
	case(Instr::sw):
		mips_ << "sw " << reg::RegToString(t0) << " " << value << "("
			<< reg::RegToString(t1) << ")" << endl;
		break;
	default:
		assert(0);
	}
}

void Objcode::Output(Instr instr, Reg t0, Reg t1, string label) {
	switch (instr) {
	case(Instr::lw):
		mips_ << "lw " << reg::RegToString(t0) << " " << label << "("
			<< reg::RegToString(t1) << ")" << endl;
		break;
	case(Instr::sw):
		mips_ << "sw " << reg::RegToString(t0) << " " << label << "("
			<< reg::RegToString(t1) << ")" << endl;
		break;
	case(Instr::bgt):
		mips_ << "bgt " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	case(Instr::bge):
		mips_ << "bge " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	case(Instr::blt):
		mips_ << "blt " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	case(Instr::ble):
		mips_ << "ble " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	case(Instr::beq):
		mips_ << "beq " << reg::RegToString(t0) << " " << reg::RegToString(t1)
			<< " " << label << endl;
		break;
	case(Instr::bne):
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