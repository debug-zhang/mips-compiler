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
	string label_;
	int value_;

	Symbol* t0_;
	Symbol* t1_;
	Symbol* t2_;
public:
	Midcode(MidcodeInstr instr, string label, int value,
		Symbol* t0, Symbol* t1, Symbol* t2);
	Midcode(MidcodeInstr instr, string label, int value,
		Symbol* t0);
	Midcode(MidcodeInstr instr, string label, int value);
	Midcode(MidcodeInstr instr, string label,
		Symbol* t0);
	Midcode(MidcodeInstr instr, string label);
	Midcode(MidcodeInstr instr, int value,
		Symbol* t0, Symbol* t1);
	Midcode(MidcodeInstr instr, int value, 
		Symbol* t0);
	Midcode(MidcodeInstr instr, 
		Symbol* t0);

	static string GetLabel();
	void output(ofstream&);
};

