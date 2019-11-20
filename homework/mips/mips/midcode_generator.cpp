#include "midcode_generator.h"

MidcodeGenerator::MidcodeGenerator() {
}

void MidcodeGenerator::OpenMidcodeFile(string fileName) {
	this->midcode.open(fileName);
}

void MidcodeGenerator::FileClose() {
	this->midcode.close();
}

void MidcodeGenerator::PrintFuncDeclare(Symbol* function) {
	string type = function->GetType() == INT ? INT_TYPE : CHAR_TYPE;
	this->midcode << type + " " + function->GetName() + "()" << endl;
}

void MidcodeGenerator::PrintVoidFuncDeclare(Symbol* function) {
	this->midcode << "void " + function->GetName() + "()" << endl;
}

void MidcodeGenerator::PrintReturn(bool isVoid, string value) {
	if (isVoid) {
		this->midcode << "return" << endl;
	} else {
		this->midcode << "return" + value << endl;
	}
}

void MidcodeGenerator::PrintLabel(int label) {
	this->midcode << "Label_" << label << ":" << endl;
}

void MidcodeGenerator::PrintGotoLabel(int label) {
	this->midcode << "goto Label_" << label << ":" << endl;
}

void MidcodeGenerator::PrintStep(string name1, string name2, string op, int step) {
	midcode << name1 << " = " << name2 << " " + op + " " << step << endl;
}

void MidcodeGenerator::PrintBez(int label, string expression) {
	this->midcode << "bez " << expression << " Label_" << ":" << label << endl;
}

void MidcodeGenerator::PrintBnz(int label, string expression) {
	this->midcode << "bnz " << expression << " Label_" << ":" << label << endl;
}

void MidcodeGenerator::PrintBezOrBnz(int label, string expression, bool isFalseBranch) {
	if (isFalseBranch) {
		this->PrintBez(label, expression);
	} else {
		this->PrintBnz(label, expression);
	}
}

void MidcodeGenerator::PrintBeq(int label, string expression1, string expression2) {
	this->midcode << "beq " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBne(int label, string expression1, string expression2) {
	this->midcode << "bne " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBeqOrBne(int label, string expression1, string expression2, JUDGE judge, bool isFalseBranch) {
	if (judge == BEQ) {
		if (isFalseBranch) {
			this->PrintBne(label, expression1, expression2);
		} else {
			this->PrintBeq(label, expression1, expression2);
		}
	} else {
		if (isFalseBranch) {
			this->PrintBeq(label, expression1, expression2);
		} else {
			this->PrintBne(label, expression1, expression2);
		}
	}
}

void MidcodeGenerator::PrintBge(int label, string expression1, string expression2) {
	this->midcode << "bge " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBlt(int label, string expression1, string expression2) {
	this->midcode << "blt " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBgeOrBlt(int label, string expression1, string expression2, JUDGE judge, bool isFalseBranch) {
	if (judge == BGE) {
		if (isFalseBranch) {
			this->PrintBlt(label, expression1, expression2);
		} else {
			this->PrintBge(label, expression1, expression2);
		}
	} else {
		if (isFalseBranch) {
			this->PrintBge(label, expression1, expression2);
		} else {
			this->PrintBlt(label, expression1, expression2);
		}
	}
}

void MidcodeGenerator::PrintBgt(int label, string expression1, string expression2) {
	this->midcode << "bgt " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBle(int label, string expression1, string expression2) {
	this->midcode << "ble " + expression1
		<< " " + expression2 << " Label_" << label << endl;
}

void MidcodeGenerator::PrintBgtOrBle(int label, string expression1, string expression2, JUDGE judge, bool isFalseBranch) {
	if (judge == BGT) {
		if (isFalseBranch) {
			this->PrintBle(label, expression1, expression2);
		} else {
			this->PrintBgt(label, expression1, expression2);
		}
	} else {
		if (isFalseBranch) {
			this->PrintBgt(label, expression1, expression2);
		} else {
			this->PrintBle(label, expression1, expression2);
		}
	}
}

void MidcodeGenerator::PrintString(int stringNumber) {
	midcode << "printf str_" << stringNumber << endl;
}

void MidcodeGenerator::PrintInteger(string number) {
	midcode << "printf int " + number << endl;
}

void MidcodeGenerator::PrintChar(string c) {
	midcode << "printf char " + c << endl;
}

void MidcodeGenerator::PrintScanf(string type, string identifier) {
	midcode << "scanf " + type << " " + identifier << endl;
}

void MidcodeGenerator::PrintAssignValue(string name, string arrayIndex, string value) {
	if (arrayIndex == "") {
		midcode << name + " = " + value << endl;
	} else {
		midcode << name + "[" + arrayIndex + "] = " + value << endl;
	}
}

void MidcodeGenerator::PrintLoadToTempReg(string name, string arrayIndex, int tempRegNumber) {
	if (arrayIndex == "") {
		midcode << "t" << tempRegNumber << " = " << name << endl;
	} else {
		midcode << "t" << tempRegNumber << " = " << name + "[" + arrayIndex + "]" << endl;
	}
}

void MidcodeGenerator::PrintPushParameter(string value) {
	midcode << "push " + value << endl;
}

void MidcodeGenerator::PrintCallFunction(string name) {
	midcode << "call " + name << endl;
}

void MidcodeGenerator::PrintAssignReturn(int tempRegCount) {
	midcode << "t" << tempRegCount << " = RET" << endl;
}

void MidcodeGenerator::PrintRegOpReg(int resultReg, int opReg1, int opReg2, string op) {
	midcode << "t" << resultReg << " = t" << opReg1 << " " + op + " t" << opReg2 << endl;
}

void MidcodeGenerator::PrintRegOpNumber(int resultReg, int opReg, string number, string op) {
	midcode << "t" << resultReg << " = t" << opReg << " " + op + " " + number << endl;
}

void MidcodeGenerator::PrintNumberOpReg(int resultReg, string number, int opReg, string op) {
	midcode << "t" << resultReg << " = " + number + " " + op + " t" << opReg << endl;
}

void MidcodeGenerator::PrintNumberOpNumber(int resultReg, string number1, string number2, string op) {
	midcode << "t" << resultReg << " = " + number1 + " " + op + " " + number2 << endl;
}

