#pragma once

#include <fstream>
#include <string>
#include <list>
#include "midcode.h"
#include "symbol.h"
#include "instr.h"


enum class Judge {
    BEQ,
    BNE,
    BGE,
    BLT,
    BGT,
    BLE
};

const std::string kIntType = "int";
const std::string kCharType = "char";

class MidcodeGenerator {
private:
    std::ofstream midcode_;
    std::list<Midcode *> midcode_list_;

    void PrintBez(int label, const std::string &expression);

    void PrintBnz(int label, const std::string &expression);

    void PrintBeq(int label, const std::string &expression1, const std::string &expression2);

    void PrintBne(int label, const std::string &expression1, const std::string &expression2);

    void PrintBge(int label, const std::string &expression1, const std::string &expression2);

    void PrintBgt(int label, const std::string &expression1, const std::string &expression2);

    void PrintBle(int label, const std::string &expression1, const std::string &expression2);

    void PrintBlt(int label, const std::string &expression1, const std::string &expression2);

public:
    MidcodeGenerator();

    void OpenMidcodeFile(const std::string &file_name);

    std::list<Midcode *> midcode_list();

    void AddMidcode(Midcode *midcode);

    void FileClose();

    void PrintParameter(TypeSymbol type, const std::string &name);

    void PrintVariable(TypeSymbol type, const std::string &name);

    void PrintFuncDeclare(Symbol *function);

    void PrintVoidFuncDeclare(Symbol *function);

    void PrintReturn(bool isVoid, const std::string &value);

    void PrintLoop();

    void PrintLabel(int label);

    void PrintJump(int label);

    void PrintStep(const std::string &name1, const std::string &name2, const std::string &op, int step);

    void PrintBezOrBnz(int label, const std::string &expression, bool is_false_branch);

    void PrintBeqOrBne(int label, const std::string &expression, const std::string &expression2, Judge judge,
                       bool is_false_branch);

    void PrintBgeOrBlt(int label, const std::string &expression, const std::string &expression2, Judge judge,
                       bool is_false_branch);

    void PrintBgtOrBle(int label, const std::string &expression, const std::string &expression2, Judge judge,
                       bool is_false_branch);

    void PrintString(int string_number);

    void PrintInteger(const std::string &number);

    void PrintChar(const std::string &c);

    void PrintEnd();

    void PrintScanf(const std::string &type, const std::string &identifier);

    void PrintAssignValue(const std::string &name, const std::string &array_index, const std::string &value);

    void PrintLoadToTempReg(const std::string &name, const std::string &array_index, int temp_reg_count);

    void PrintPushParameter(const std::string &function, const std::string &value, int count);

    void PrintCallFunction(const std::string &name);

    void PrintFuncEnd();

    void PrintSave(const std::string &function_name);

    void PrintAssignReturn(int temp_reg_count);

    void PrintRegOpReg(int result_reg, int op_reg1, int op_reg2, const std::string &op);

    void PrintRegOpNumber(int result_reg, int op_reg, const std::string &number, const std::string &op);

    void PrintNumberOpReg(int result_reg, const std::string &number, int op_reg, const std::string &op);

    void
    PrintNumberOpNumber(int result_reg, const std::string &number1, const std::string &number2, const std::string &op);

    void PrintNeg(int result_reg, const std::string &number);
};

