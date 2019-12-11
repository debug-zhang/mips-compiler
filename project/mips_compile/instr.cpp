#include "instr.h"

namespace midcodeinstr {
	MidcodeInstr GetOperatorInstr(string op) {
		switch (op[0]) {
		case '+':
			return MidcodeInstr::ADD;
			break;
		case '-':
			return MidcodeInstr::SUB;
			break;
		case '*':
			return MidcodeInstr::MUL;
			break;
		case '/':
			return MidcodeInstr::DIV;
			break;
		default:
			assert(0);
			break;
		}
	}
}