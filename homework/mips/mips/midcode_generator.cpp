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
	this->midcode_ << type + " " + function->name() + "()" << endl;
}

void MidcodeGenerator::PrintVoidFuncDeclare(Symbol* function) {
	this->midcode_ << "void " + function->name() + "()" << endl;
}

void MidcodeGenerator::PrintReturn(bool isVoid, string value) {
	if (isVoid) {
		this->midcode_ << "return" << endl;
	} else {
		this->midcode_ << "return" + value << endl;
	}
}

void MidcodeGenerator::PrintLabel(int label) {
	this->midcode_ << "Label_" << label << ":" << endl;
}

void MidcodeGenerator::PrintGotoLabel(int label) {
	this->midcode_ << "goto Label_" << label << ":" << endl;
}

void MidcodeGenerator::PrintStep(string name1, string name2, string op, int step) {
	midcode_ << name1 << " = " << name2 << " " + op + " " << step << endl;
}

void MidcodeGenerator::PrintBez(int label, string expression) {
	this->midcode_ << "bez " << expression << " Label_" << ":" << label << endl;
}

void MidcodeGenerator::PrintBnz(int label, string expression) {
	this->midcode_ << "bnz " << expression << " Label_" << ":" << label << endl;
}

void MidcodeGenerator::PrintBezOrBnz(int label, string expression, bool is_false_branch) {
	if (is_false_branch) {
		this->PrintBez(label, expression);
	} else {
		this->PrintBnz(label, expression);
	}
}

void MidcodeGenerator::PrintBeq(int label, string expression1, string expression2) {
	this->midcode_ << "beq " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBne(int label, string expression1, string expression2) {
	this->midcode_ << "bne " + expression1
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
	this->midcode_ << "bge " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBlt(int label, string expression1, string expression2) {
	this->midcode_ << "blt " + expression1
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
	this->midcode_ << "bgt " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBle(int label, string expression1, string expression2) {
	this->midcode_ << "ble " + expression1
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
	midcode_ << "printf str_" << string_number << endl;
}

void MidcodeGenerator::PrintInteger(string number) {
	midcode_ << "printf int " + number << endl;
}

void MidcodeGenerator::PrintChar(string c) {
	midcode_ << "printf char " + c << endl;
}

void MidcodeGenerator::PrintLineBreak() {
	midcode_ << "printf_end" << endl;
}

void MidcodeGenerator::PrintScanf(string type, string identifier) {
	midcode_ << "scanf " + type << " " + identifier << endl;
}

void MidcodeGenerator::PrintAssignValue(string name, string array_index, string value) {
	if (array_index == "") {
		midcode_ << name + " = " + value << endl;
	} else {
		midcode_ << name + "[" + array_index + "] = " + value << endl;
	}
}

void MidcodeGenerator::PrintLoadToTempReg(string name, string array_index, int temp_reg_count) {
	if (array_index == "") {
		midcode_ << "t" << temp_reg_count << " = " << name << endl;
	} else {
		midcode_ << "t" << temp_reg_count << " = " << name + "[" + array_index + "]" << endl;
	}
}

void MidcodeGenerator::PrintPushParameter(string value) {
	midcode_ << "push " + value << endl;
}

void MidcodeGenerator::PrintCallFunction(string name) {
	midcode_ << "call " + name << endl;
}

void MidcodeGenerator::PrintAssignReturn(int temp_reg_count) {
	midcode_ << "t" << temp_reg_count << " = RET" << endl;
}

void MidcodeGenerator::PrintRegOpReg(int result_reg, int op_reg1, int op_reg2, string op) {
	midcode_ << "t" << result_reg << " = t" << op_reg1 << " " + op + " t" << op_reg2 << endl;
}

void MidcodeGenerator::PrintRegOpNumber(int result_reg, int op_reg, string number, string op) {
	midcode_ << "t" << result_reg << " = t" << op_reg << " " + op + " " + number << endl;
}

void MidcodeGenerator::PrintNumberOpReg(int result_reg, string number, int op_reg, string op) {
	midcode_ << "t" << result_reg << " = " + number + " " + op + " t" << op_reg << endl;
}

void MidcodeGenerator::PrintNumberOpNumber(int result_reg, string number1, string number2, string op) {
	midcode_ << "t" << result_reg << " = " + number1 + " " + op + " " + number2 << endl;
}

