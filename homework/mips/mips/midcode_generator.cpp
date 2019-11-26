#include "midcode_generator.h"

MidcodeGenerator::MidcodeGenerator() {
}

void MidcodeGenerator::OpenMidcodeFile(string file_name) {
	this->midcode_.open(file_name);
}

void MidcodeGenerator::FileClose() {
	this->midcode_.close();
}

void MidcodeGenerator::PrintFuncDeclare(Symbol* function) {
	string type = function->type() == TypeSymbol::INT ? kIntType : kCharType;
	this->midcode_ << midcodeinstr::FUNC_DECLARE + " "
		<< type + " " + function->name() << endl;
}

void MidcodeGenerator::PrintVoidFuncDeclare(Symbol* function) {
	this->midcode_ << midcodeinstr::VOID_FUNC_DECLARE + " "
		<< "void " + function->name() << endl;
}

void MidcodeGenerator::PrintReturn(bool isVoid, string value) {
	if (isVoid) {
		this->midcode_ << midcodeinstr::RETURN + " "
			<< "return" << endl;
	} else {
		this->midcode_ << midcodeinstr::RETURN + " "
			<< "return " + value << endl;
	}
}

void MidcodeGenerator::PrintLabel(int label) {
	this->midcode_ << midcodeinstr::LABEL + " " <<
		"Label_" << label << ":" << endl;
}

void MidcodeGenerator::PrintGotoLabel(int label) {
	this->midcode_ << midcodeinstr::GO_TO_LABEL + " "
		<< "goto Label_" << label << ":" << endl;
}

void MidcodeGenerator::PrintStep(string name1, string name2, string op, int step) {
	midcode_ << midcodeinstr::REG_OP_NUMBER + " "
		<< name1 << " = " << name2 << " " + op + " " << step << endl;
}

void MidcodeGenerator::PrintBez(int label, string expression) {
	this->midcode_ << midcodeinstr::BEZ + " "
		<< "bez " << expression << " Label_" << ":" << label << endl;
}

void MidcodeGenerator::PrintBnz(int label, string expression) {
	this->midcode_ << midcodeinstr::BNZ + " "
		<< "bnz " << expression << " Label_" << ":" << label << endl;
}

void MidcodeGenerator::PrintBezOrBnz(int label, string expression, bool is_false_branch) {
	if (is_false_branch) {
		this->PrintBez(label, expression);
	} else {
		this->PrintBnz(label, expression);
	}
}

void MidcodeGenerator::PrintBeq(int label, string expression1, string expression2) {
	this->midcode_ << midcodeinstr::BEQ + " "
		<< "beq " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBne(int label, string expression1, string expression2) {
	this->midcode_ << midcodeinstr::BNE + " "
		<< "bne " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBeqOrBne(int label, string expression1, string expression2, Judge judge, bool is_false_branch) {
	if (judge == Judge::BEQ) {
		if (is_false_branch) {
			this->PrintBne(label, expression1, expression2);
		} else {
			this->PrintBeq(label, expression1, expression2);
		}
	} else {
		if (is_false_branch) {
			this->PrintBeq(label, expression1, expression2);
		} else {
			this->PrintBne(label, expression1, expression2);
		}
	}
}

void MidcodeGenerator::PrintBge(int label, string expression1, string expression2) {
	this->midcode_ << midcodeinstr::BGE + " "
		<< "bge " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBlt(int label, string expression1, string expression2) {
	this->midcode_ << midcodeinstr::BLT + " "
		<< "blt " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBgeOrBlt(int label, string expression1, string expression2, Judge judge, bool is_false_branch) {
	if (judge == Judge::BGE) {
		if (is_false_branch) {
			this->PrintBlt(label, expression1, expression2);
		} else {
			this->PrintBge(label, expression1, expression2);
		}
	} else {
		if (is_false_branch) {
			this->PrintBge(label, expression1, expression2);
		} else {
			this->PrintBlt(label, expression1, expression2);
		}
	}
}

void MidcodeGenerator::PrintBgt(int label, string expression1, string expression2) {
	this->midcode_ << midcodeinstr::BGT + " "
		<< "bgt " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBle(int label, string expression1, string expression2) {
	this->midcode_ << midcodeinstr::BLE + " "
		<< "ble " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBgtOrBle(int label, string expression1, string expression2, Judge judge, bool is_false_branch) {
	if (judge == Judge::BGT) {
		if (is_false_branch) {
			this->PrintBle(label, expression1, expression2);
		} else {
			this->PrintBgt(label, expression1, expression2);
		}
	} else {
		if (is_false_branch) {
			this->PrintBgt(label, expression1, expression2);
		} else {
			this->PrintBle(label, expression1, expression2);
		}
	}
}

void MidcodeGenerator::PrintString(int string_number) {
	midcode_ << midcodeinstr::PRINTF + " "
		<< "printf str_" << string_number << endl;
}

void MidcodeGenerator::PrintInteger(string number) {
	midcode_ << midcodeinstr::PRINTF + " "
		<< "printf int " + number << endl;
}

void MidcodeGenerator::PrintChar(string c) {
	midcode_ << midcodeinstr::PRINTF + " "
		<< "printf char " + c << endl;
}

void MidcodeGenerator::PrintEnd() {
	midcode_ << midcodeinstr::PRINTF_END + " "
		<< "printf_end" << endl;
}

void MidcodeGenerator::PrintScanf(string type, string identifier) {
	midcode_ << midcodeinstr::SCANF + " "
		<< "scanf " + type << " " + identifier << endl;
}

void MidcodeGenerator::PrintAssignValue(string name, string array_index, string value) {
	if (array_index == "") {
		midcode_ << midcodeinstr::ASSIGN + " "
			<< name + " = " + value << endl;
	} else {
		midcode_ << midcodeinstr::ASSIGN + " "
			<< name + "[" + array_index + "] = " + value << endl;
	}
}

void MidcodeGenerator::PrintLoadToTempReg(string name, string array_index, int temp_reg_count) {
	if (array_index == "") {
		midcode_ << midcodeinstr::LOAD + " "
			<< "#" << temp_reg_count << " = " << name << endl;
	} else {
		midcode_ << midcodeinstr::LOAD + " "
			<< "#" << temp_reg_count << " = " << name + "[" + array_index + "]" << endl;
	}
}

void MidcodeGenerator::PrintPushParameter(string value) {
	midcode_ << midcodeinstr::PUSH + " "
		<< "push " + value << endl;
}

void MidcodeGenerator::PrintCallFunction(string name) {
	midcode_ << midcodeinstr::CALL + " "
		<< "call " + name << endl;
}

void MidcodeGenerator::PrintSave() {
	midcode_ << midcodeinstr::SAVE + " "
		<< "save" << endl;
}

void MidcodeGenerator::PrintAssignReturn(int temp_reg_count) {
	midcode_ << midcodeinstr::ASSIGN_RETURN + " "
		<< "#" << temp_reg_count << " = RET" << endl;
}

void MidcodeGenerator::PrintRegOpReg(int result_reg, int op_reg1, int op_reg2, string op) {
	midcode_ << midcodeinstr::REG_OP_REG + " "
		<< "#" << result_reg << " = #" << op_reg1 << " " + op + " #" << op_reg2 << endl;
}

void MidcodeGenerator::PrintRegOpNumber(int result_reg, int op_reg, string number, string op) {
	midcode_ << midcodeinstr::REG_OP_NUMBER + " "
		<< "#" << result_reg << " = #" << op_reg << " " + op + " " + number << endl;
}

void MidcodeGenerator::PrintNumberOpReg(int result_reg, string number, int op_reg, string op) {
	midcode_ << midcodeinstr::NUMBER_OP_REG + " "
		<< "#" << result_reg << " = " + number + " " + op + " #" << op_reg << endl;
}

void MidcodeGenerator::PrintNumberOpNumber(int result_reg, string number1, string number2, string op) {
	midcode_ << midcodeinstr::NUMBER_OP_NUMBER + " "
		<< "#" << result_reg << " = " + number1 + " " + op + " " + number2 << endl;
}

