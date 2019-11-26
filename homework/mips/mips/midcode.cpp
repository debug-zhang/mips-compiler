#include "midcode.h"

Midcode::Midcode(MidcodeInstr instr, string label, int value, 
	Symbol* t0, Symbol* t1, Symbol* t2) {
	this->instr_ = instr;
	this->label_ = label;
	this->value_ = value;

	this->t0_ = t0;
	this->t1_ = t1;
	this->t2_ = t2;
}

Midcode::Midcode(MidcodeInstr instr, string label, int value, 
	Symbol* t0) {
	this->instr_ = instr;
	this->label_ = label;
	this->value_ = value;

	this->t0_ = t0;
	this->t1_ = NULL;
	this->t2_ = NULL;
}

Midcode::Midcode(MidcodeInstr instr, string label, int value) {
	this->instr_ = instr;
	this->label_ = label;
	this->value_ = value;

	this->t0_ = NULL;
	this->t1_ = NULL;
	this->t2_ = NULL;
}

Midcode::Midcode(MidcodeInstr instr, string label, 
	Symbol* t0) {
	this->instr_ = instr;
	this->label_ = label;
	this->value_ = 0;

	this->t0_ = t0;
	this->t1_ = NULL;
	this->t2_ = NULL;
}

Midcode::Midcode(MidcodeInstr instr, string label) {
	this->instr_ = instr;
	this->label_ = label;
	this->value_ = 0;

	this->t0_ = NULL;
	this->t1_ = NULL;
	this->t2_ = NULL;
}

Midcode::Midcode(MidcodeInstr instr, int value,
	Symbol* t0, Symbol* t1) {
	this->instr_ = instr;
	this->label_ = "";
	this->value_ = 0;

	this->t0_ = t0;
	this->t1_ = t1;
	this->t2_ = NULL;
}

Midcode::Midcode(MidcodeInstr instr,  int value, 
	Symbol* t0) {
	this->instr_ = instr;
	this->label_ = "";
	this->value_ =value;

	this->t0_ = t0;
	this->t1_ = NULL;
	this->t2_ = NULL;
}

Midcode::Midcode(MidcodeInstr instr,
	Symbol* t0) {
	this->instr_ = instr;
	this->label_ = "";
	this->value_ = 0;

	this->t0_ = t0;
	this->t1_ = NULL;
	this->t2_ = NULL;
}

string Midcode::GetLabel() {
	static int label_count = 1;
	return "label_" + to_string(label_count++);
}

void Midcode::output(ofstream& midcode_file) {
	switch (this->instr_) {
	case(MidcodeInstr::ADD):
		midcode_file << t0_->name() << " = " << t1_->name()<< " + " << t2_->name()<<endl;
		break;
	case(MidcodeInstr::ADDI):
		midcode_file << t0_->name()<< " = " << t1_->name()<< " + " << value_<<endl;
		break;
	case(MidcodeInstr::SUB):
		midcode_file << t0_->name()<< " = " << t1_->name()<< " - " << t2_->name()<<endl;
		break;
	case(MidcodeInstr::SUBI):
		midcode_file << t0_->name()<< " = " << t1_->name()<< " - " << value_<<endl;
		break;
	case(MidcodeInstr::NEG):
		midcode_file << t0_->name()<< " = " << "-" << t1_->name()<<endl;
		break;
	case(MidcodeInstr::MULT):
		midcode_file << t0_->name()<< " = " << t1_->name()<< " * " << t2_->name()<<endl;
		break;
	case(MidcodeInstr::DIV):
		midcode_file << t0_->name()<< " = " << t1_->name()<< " / " << t2_->name()<<endl;
		break;
	case(MidcodeInstr::LOAD_IND):
		midcode_file << t0_->name()<< " = " << t1_->name()<< "[" << t2_->name()<< "]"<<endl;
		break;
	case(MidcodeInstr::STORE_IND):
		midcode_file << t0_->name()<< "[" << t1_->name()<< "]" << " = " << t2_->name()<<endl;
		break;
	case(MidcodeInstr::ASSIGN):
		if (t1_) midcode_file << t0_->name()<< " = " << t1_->name()<<endl;
		else midcode_file << t0_->name()<< " = " << label_<<endl;
		break;
	case(MidcodeInstr::ASSIGN_INT):
		midcode_file << t0_->name()<< " = " << value_<<endl;
		break;
	case(MidcodeInstr::ASSIGN_CHAR):
		midcode_file << t0_->name()<< " = " << '\'' << char(value_) << '\''<<endl;
		break;
	case(MidcodeInstr::PUSH):
		midcode_file << "PUSH " << t0_->name()<<endl;
		break;
	case(MidcodeInstr::CALL):
		midcode_file << "CALL " << label_<<endl;
		break;
	case(MidcodeInstr::RETURN):
		if (t0_ == NULL) midcode_file << "RETURN"<<endl;
		else midcode_file << "RETURN " << t0_->name()<<endl;
		break;
	case(MidcodeInstr::SCANF):
		midcode_file << "SCANF " << t0_->name()<<endl;
		break;
	case(MidcodeInstr::PRINTF):
		if (label_ != "") midcode_file << "PRINTF " << label_<<endl;
		if (t0_ != NULL)  midcode_file << "PRINTF " << t0_->name()<<endl;
		break;
	case(MidcodeInstr::BGT):
		midcode_file << "BGT " << t0_->name()<< " > " << t1_->name()<< " -> " << label_<<endl;
		break;
	case(MidcodeInstr::BGE):
		midcode_file << "BGE " << t0_->name()<< " >= " << t1_->name()<< " -> " << label_<<endl;
		break;
	case(MidcodeInstr::BLT):
		midcode_file << "BLT " << t0_->name()<< " < " << t1_->name()<< " -> " << label_<<endl;
		break;
	case(MidcodeInstr::BLE):
		midcode_file << "BLE " << t0_->name()<< " <= " << t1_->name()<< " -> " << label_<<endl;
		break;
	case(MidcodeInstr::BEQ):
		midcode_file << "BEQ " << t0_->name()<< " == " << t1_->name()<< " -> " << label_<<endl;
		break;
	case(MidcodeInstr::BNE):
		midcode_file << "BNE " << t0_->name()<< " != " << t1_->name()<< " -> " << label_<<endl;
		break;
	case(MidcodeInstr::BZ):
		midcode_file << "BZ " << t0_->name()<< " == " << "0" << " -> " << label_<<endl;
		break;
	case(MidcodeInstr::BNZ):
		midcode_file << "BNZ " << t0_->name()<< " != " << "0" << " -> " << label_<<endl;
		break;
	case(MidcodeInstr::JUMP):
		midcode_file << "JUMP " << label_<<endl;
		break;
	case(MidcodeInstr::LABEL):
		midcode_file << "LABEL: " << label_<<endl;
		break;
	case(MidcodeInstr::FUNC_VOID):
		midcode_file << "VOID_FUNC " << label_<<endl;
		break;
	case(MidcodeInstr::FUNC_INT):
		midcode_file << "INT_FUNC " << label_<<endl;
		break;
	case(MidcodeInstr::FUNC_CHAR):
		midcode_file << "CHAR_FUNC " << label_<<endl;
		break;
	case(MidcodeInstr::PARA_INT):
		midcode_file << "PARA INT " << t0_->name()<<endl;
		break;
	case(MidcodeInstr::PARA_CHAR):
		midcode_file << "PARA CHAR " << t0_->name()<<endl;
		break;
	case(MidcodeInstr::VAR_INT):
		midcode_file << "VAR INT " << t0_->name() <<endl;
		break;
	case(MidcodeInstr::VAR_CHAR):
		midcode_file << "VAR CHAR " << t0_->name() <<endl;
		break;
	default:
		assert(0);
		break;
	}
}