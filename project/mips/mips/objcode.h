#pragma once

#include <cassert>
#include <string>
#include <fstream>
#include "reg.h"
#include "instr.h"

using namespace std;

class Objcode {
private:
	ofstream mips_;

public:
	Objcode(string mipsFile);

	void Output();
	void Output(MipsInstr instr);
	void Output(MipsInstr instr, Reg t0);
	void Output(MipsInstr instr, int value);
	void Output(MipsInstr instr, string label);
	void Output(MipsInstr instr, Reg t0, Reg t1);
	void Output(MipsInstr instr, Reg t0, int value);
	void Output(MipsInstr instr, Reg t0, string label);
	void Output(MipsInstr instr, Reg t0, Reg t1, Reg t2);
	void Output(MipsInstr instr, string label, int value);
	void Output(MipsInstr instr, Reg t0, Reg t1, int value);
	void Output(MipsInstr instr, Reg t0, Reg t1, string label);

	void FileClose();
};

