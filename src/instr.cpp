#include "instr.h"

namespace midcodeinstr {
    MidcodeInstr GetOperatorInstr(std::string op) {
        switch (op[0]) {
            case '+':
                return MidcodeInstr::ADD;
            case '-':
                return MidcodeInstr::SUB;
            case '*':
                return MidcodeInstr::MUL;
            case '/':
                return MidcodeInstr::DIV;
            default:
                assert(0);
        }
    }
}