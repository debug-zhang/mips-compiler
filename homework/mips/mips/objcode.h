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

	void Output(Instr instr);
	void Output(Instr instr, Reg t0);
	void Output(Instr instr, int value);
	void Output(Instr instr, string label);
	void Output(Instr instr, Reg t0, Reg t1);
	void Output(Instr instr, Reg t0, int value);
	void Output(Instr instr, Reg t0, string label);
	void Output(Instr instr, Reg t0, Reg t1, Reg t2);
	void Output(Instr instr, string label, int value);
	void Output(Instr instr, Reg t0, Reg t1, int value);
	void Output(Instr instr, Reg t0, Reg t1, string label);

	void FileClose();
};

