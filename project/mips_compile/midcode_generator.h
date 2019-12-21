#pragma once

#include <fstream>
#include <string>
#include <list>
#include "midcode.h"
#include "symbol.h"
#include "instr.h"

using namespace std;

enum class Judge {
	BEQ,
	BNE,
	BGE,
	BLT,
	BGT,
	BLE
};

const string kIntType = "int";
const string kCharType = "char";

class MidcodeGenerator {
private:
	ofstream midcode_;
	list<Midcode*> midcode_list_;

	void PrintBez(int label, string expression);
	void PrintBnz(int label, string expression);

	void PrintBeq(int label, string expression1, string expression2);
	void PrintBne(int label, string expression1, string expression2);
	void PrintBge(int label, string expression1, string expression2);
	void PrintBgt(int label, string expression1, string expression2);
	void PrintBle(int label, string expression1, string expression2);
	void PrintBlt(int label, string expression1, string expression2);
public:
	MidcodeGenerator();
	void OpenMidcodeFile(string file_name);
	list<Midcode*> midcode_list();
	void AddMidcode(Midcode* midcode);
	void FileClose();

	void PrintParameter(TypeSymbol type, string name);
	void PrintVariable(TypeSymbol type, string name);
	void PrintFuncDeclare(Symbol* function);
	void PrintVoidFuncDeclare(Symbol* function);
	void PrintReturn(bool isVoid, string value);

	void PrintLoop();
	void PrintLabel(int label);
	void PrintJump(int label);
	void PrintStep(string name1, string name2, string op, int step);

	void PrintBezOrBnz(int label, string expression, bool is_false_branch);
	void PrintBeqOrBne(int label, string expression, string expression2, Judge judge, bool is_false_branch);
	void PrintBgeOrBlt(int label, string expression, string expression2, Judge judge, bool is_false_branch);
	void PrintBgtOrBle(int label, string expression, string expression2, Judge judge, bool is_false_branch);

	void PrintString(int string_number);
	void PrintInteger(string number);
	void PrintChar(string c);
	void PrintEnd();

	void PrintScanf(string type, string identifier);

	void PrintAssignValue(string name, string array_index, string value);
	void PrintLoadToTempReg(string name, string array_index, int temp_reg_count);

	void PrintPushParameter(string function, string value, int count);
	void PrintCallFunction(string name);
	void PrintFuncEnd();
	void PrintSave(string function_name);
	void PrintAssignReturn(int temp_reg_count);

	void PrintRegOpReg(int result_reg, int op_reg1, int op_reg2, string op);
	void PrintRegOpNumber(int result_reg, int op_reg, string number, string op);
	void PrintNumberOpReg(int result_reg, string number, int op_reg, string op);
	void PrintNumberOpNumber(int result_reg, string number1, string number2, string op);

	void PrintNeg(int result_reg, string number);
};

