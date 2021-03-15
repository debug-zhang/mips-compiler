#pragma once

#include <cassert>
#include <string>
#include <fstream>
#include "reg.h"
#include "instr.h"

class Objcode {
private:
    std::ofstream mips_;

public:
    explicit Objcode(const std::string& mipsFile);

    void Output();

    void Output(MipsInstr instr);

    void Output(MipsInstr instr, Reg t0);

    void Output(MipsInstr instr, int value);

    void Output(MipsInstr instr, const std::string& label);

    void Output(MipsInstr instr, Reg t0, Reg t1);

    void Output(MipsInstr instr, Reg t0, int value);

    void Output(MipsInstr instr, Reg t0, const std::string& label);

    void Output(MipsInstr instr, Reg t0, Reg t1, Reg t2);

    void Output(MipsInstr instr, const std::string& label, int value);

    void Output(MipsInstr instr, Reg t0, Reg t1, int value);

    void Output(MipsInstr instr, Reg t0, Reg t1, const std::string& label);

    void FileClose();
};

