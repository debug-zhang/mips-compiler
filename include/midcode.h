#pragma once

#include <string>
#include <fstream>
#include <cassert>
#include "symbol.h"
#include "instr.h"

class Midcode {
private:
    MidcodeInstr instr_;
    OperaMember opera_member_;

    int count_;
    std::string label_;

    std::string reg1_;
    std::string reg2_;
    std::string reg_result_;

    int temp1_;
    int temp2_;
    int temp_result_;

public:
    Midcode(MidcodeInstr instr);

    Midcode(MidcodeInstr instr, const std::string &label);

    Midcode(MidcodeInstr instr, int count);

    Midcode(MidcodeInstr instr, const std::string &reg1, int count);

    Midcode(MidcodeInstr instr, int count, const std::string &reg1);

    Midcode(MidcodeInstr instr, const std::string &reg_result, const std::string &reg1);

    Midcode(MidcodeInstr instr, const std::string &reg1, const std::string &reg2, int count);

    Midcode(MidcodeInstr instr, const std::string &reg_result, const std::string &reg1, const std::string &reg2);

    Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result, int temp1, int temp2);

    Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result, int temp1, const std::string &reg2);

    Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result,
            const std::string &reg1, const std::string &reg2);

    Midcode(MidcodeInstr instr, OperaMember opera_member,
            const std::string &reg_result, const std::string &reg1, const std::string &reg2);

    void Init();

    MidcodeInstr instr();

    OperaMember opera_member();

    int count();

    std::string label();

    std::string reg1();

    std::string reg2();

    std::string reg_result();

    int temp_result();

    std::string GetTempReg1();

    std::string GetTempReg2();

    std::string GetTempRegResult();

    std::string GetJumpLabel();

    std::string GetLabel();

    std::string GetString();

    std::string GetTempReg();

    int GetInteger();

    int GetChar();
};

