#pragma once

#include <string>
#include <fstream>
#include <cassert>
#include "symbol.h"
#include "instr.h"

using namespace std;

class Midcode {
private:
	MidcodeInstr instr_;
	OperaMember opera_member_;

	int count_;
	string label_;

	string reg1_;
	string reg2_;
	string reg_result_;

	int temp1_;
	int temp2_;
	int temp_result_;

public:
	Midcode(MidcodeInstr instr);
	Midcode(MidcodeInstr instr, string label);
	Midcode(MidcodeInstr instr, int count);
	Midcode(MidcodeInstr instr, string reg1, int count);
	Midcode(MidcodeInstr instr, int count, string reg1);
	Midcode(MidcodeInstr instr, string reg_result, string reg1);
	Midcode(MidcodeInstr instr, string reg1, string reg2, int count);
	Midcode(MidcodeInstr instr, string reg_result, string reg1, string reg2);
	Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result, int temp1, int temp2);
	Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result, int temp1, string reg2);
	Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result, string reg1, string reg2);
	Midcode(MidcodeInstr instr, OperaMember opera_member, string reg_result, string reg1, string reg2);

	void Init();

	MidcodeInstr instr();
	OperaMember opera_member();

	int count();
	string label();

	string reg1();
	string reg2();
	string reg_result();

	int temp_result();

	string GetTempReg1();
	string GetTempReg2();
	string GetTempRegResult();

	string GetJumpLabel();
	string GetLabel();
	string GetString();
	string GetTempReg();
	int GetInteger();
	int GetChar();
};

