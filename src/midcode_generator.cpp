#include "midcode_generator.h"

using namespace std;

MidcodeGenerator::MidcodeGenerator() = default;

void MidcodeGenerator::OpenMidcodeFile(const string &file_name) {
    midcode_.open(file_name);
}

list<Midcode *> MidcodeGenerator::midcode_list() {
    return midcode_list_;
}

void MidcodeGenerator::AddMidcode(Midcode *midcode) {
    midcode_list_.push_back(midcode);
}

void MidcodeGenerator::FileClose() {
    midcode_.close();
}

void MidcodeGenerator::PrintParameter(TypeSymbol type, const string &name) {
    string parameter_type = type == TypeSymbol::INT ? kIntType : kCharType;
    midcode_ << "parameter " + parameter_type + " " + name << endl;

    MidcodeInstr midcode_instr = type == TypeSymbol::INT
                                 ? MidcodeInstr::PARA_INT : MidcodeInstr::PARA_CHAR;
    AddMidcode(new Midcode(midcode_instr, name));
}

void MidcodeGenerator::PrintVariable(TypeSymbol type, const string &name) {
    string variable_type = type == TypeSymbol::INT ? kIntType : kCharType;
    midcode_ << "variable " + variable_type + " " + name << endl;

    MidcodeInstr midcode_instr = type == TypeSymbol::INT
                                 ? MidcodeInstr::VAR_INT : MidcodeInstr::VAR_CHAR;
    AddMidcode(new Midcode(midcode_instr, name));
}

void MidcodeGenerator::PrintFuncDeclare(Symbol *function) {
    string type = function->type() == TypeSymbol::INT ? kIntType : kCharType;
    midcode_ << endl << type + " " + function->name() + "()" << endl;

    MidcodeInstr midcode_instr = function->type() == TypeSymbol::INT
                                 ? MidcodeInstr::INT_FUNC_DECLARE : MidcodeInstr::CHAR_FUNC_DECLARE;
    AddMidcode(new Midcode(midcode_instr, function->name()));
}

void MidcodeGenerator::PrintVoidFuncDeclare(Symbol *function) {
    midcode_ << endl << "void " + function->name() + "()" << endl;

    AddMidcode(new Midcode(MidcodeInstr::VOID_FUNC_DECLARE, function->name()));
}

void MidcodeGenerator::PrintReturn(bool isVoid, const string &value) {
    if (isVoid) {
        midcode_ << "return" << endl;

        AddMidcode(new Midcode(MidcodeInstr::RETURN_NON));
    } else {
        midcode_ << "return " + value << endl;

        AddMidcode(new Midcode(MidcodeInstr::RETURN, value));
    }
}

void MidcodeGenerator::PrintLabel(int label) {
    midcode_ << "Label_" << label << ":" << endl;

    AddMidcode(new Midcode(MidcodeInstr::LABEL, label));
}

void MidcodeGenerator::PrintJump(int label) {
    midcode_ << "jump Label_" << label << ":" << endl;

    AddMidcode(new Midcode(MidcodeInstr::JUMP, label));
}

void MidcodeGenerator::PrintLoop() {
    AddMidcode(new Midcode(MidcodeInstr::LOOP));
}

void MidcodeGenerator::PrintStep(const string &name1, const string &name2, const string &op, int step) {
    midcode_ << name1 << " = " << name2 << " " + op + " " << step << endl;

    AddMidcode(new Midcode(MidcodeInstr::STEP));
    AddMidcode(new Midcode(midcodeinstr::GetOperatorInstr(op),
                           OperaMember::NUMBER_OP_NUMBER, name1, name2, to_string(step)));
}

void MidcodeGenerator::PrintBez(int label, const string &expression) {
    midcode_ << "bez " << expression << " Label_" << ":" << label << endl;

    AddMidcode(new Midcode(MidcodeInstr::BEZ, expression, label));
}

void MidcodeGenerator::PrintBnz(int label, const string &expression) {
    midcode_ << "bnz " << expression << " Label_" << ":" << label << endl;

    AddMidcode(new Midcode(MidcodeInstr::BNZ, expression, label));
}

void MidcodeGenerator::PrintBezOrBnz(int label, const string &expression, bool is_false_branch) {
    if (is_false_branch) {
        PrintBez(label, expression);
    } else {
        PrintBnz(label, expression);
    }
}

void MidcodeGenerator::PrintBeq(int label, const string &expression1, const string &expression2) {
    midcode_ << "beq " + expression1
             << " " + expression2 << " Label_" << label << endl;

    AddMidcode(new Midcode(MidcodeInstr::BEQ, expression1, expression2, label));
}

void MidcodeGenerator::PrintBne(int label, const string &expression1, const string &expression2) {
    midcode_ << "bne " + expression1
             << " " + expression2 << " Label_" << label << endl;

    AddMidcode(new Midcode(MidcodeInstr::BNE, expression1, expression2, label));
}

void
MidcodeGenerator::PrintBeqOrBne(int label, const string &expression1, const string &expression2, Judge judge,
                                bool is_false_branch) {
    if (judge == Judge::BEQ) {
        if (is_false_branch) {
            PrintBne(label, expression1, expression2);
        } else {
            PrintBeq(label, expression1, expression2);
        }
    } else {
        if (is_false_branch) {
            PrintBeq(label, expression1, expression2);
        } else {
            PrintBne(label, expression1, expression2);
        }
    }
}

void MidcodeGenerator::PrintBge(int label, const string &expression1, const string &expression2) {
    midcode_ << "bge " + expression1
             << " " + expression2 << " Label_" << label << endl;

    AddMidcode(new Midcode(MidcodeInstr::BGE, expression1, expression2, label));
}

void MidcodeGenerator::PrintBlt(int label, const string &expression1, const string &expression2) {
    midcode_ << "blt " + expression1
             << " " + expression2 << " Label_" << label << endl;

    AddMidcode(new Midcode(MidcodeInstr::BLT, expression1, expression2, label));
}

void
MidcodeGenerator::PrintBgeOrBlt(int label, const string &expression1, const string &expression2, Judge judge,
                                bool is_false_branch) {
    if (judge == Judge::BGE) {
        if (is_false_branch) {
            PrintBlt(label, expression1, expression2);
        } else {
            PrintBge(label, expression1, expression2);
        }
    } else {
        if (is_false_branch) {
            PrintBge(label, expression1, expression2);
        } else {
            PrintBlt(label, expression1, expression2);
        }
    }
}

void MidcodeGenerator::PrintBgt(int label, const string &expression1, const string &expression2) {
    midcode_ << "bgt " + expression1
             << " " + expression2 << " Label_" << label << endl;

    AddMidcode(new Midcode(MidcodeInstr::BGT, expression1, expression2, label));
}

void MidcodeGenerator::PrintBle(int label, const string &expression1, const string &expression2) {
    midcode_ << "ble " + expression1
             << " " + expression2 << " Label_" << label << endl;

    AddMidcode(new Midcode(MidcodeInstr::BLE, expression1, expression2, label));
}

void
MidcodeGenerator::PrintBgtOrBle(int label, const string &expression1, const string &expression2, Judge judge,
                                bool is_false_branch) {
    if (judge == Judge::BGT) {
        if (is_false_branch) {
            PrintBle(label, expression1, expression2);
        } else {
            PrintBgt(label, expression1, expression2);
        }
    } else {
        if (is_false_branch) {
            PrintBgt(label, expression1, expression2);
        } else {
            PrintBle(label, expression1, expression2);
        }
    }
}

void MidcodeGenerator::PrintString(int string_number) {
    midcode_ << "printf str_" << string_number << endl;

    AddMidcode(new Midcode(MidcodeInstr::PRINTF_STRING, string_number));
}

void MidcodeGenerator::PrintInteger(const string &number) {
    midcode_ << "printf int " + number << endl;

    AddMidcode(new Midcode(MidcodeInstr::PRINTF_INT, number));
}

void MidcodeGenerator::PrintChar(const string &c) {
    midcode_ << "printf char " + c << endl;

    AddMidcode(new Midcode(MidcodeInstr::PRINTF_CHAR, c));
}

void MidcodeGenerator::PrintEnd() {
    midcode_ << "printf_end" << endl;

    AddMidcode(new Midcode(MidcodeInstr::PRINTF_END));
}

void MidcodeGenerator::PrintScanf(const string &type, const string &identifier) {
    midcode_ << "scanf " + type << " " + identifier << endl;

    MidcodeInstr midcode_instr = type == "int" ? MidcodeInstr::SCANF_INT : MidcodeInstr::SCANF_CHAR;

    AddMidcode(new Midcode(midcode_instr, identifier));
}

void MidcodeGenerator::PrintAssignValue(const string &name, const string &array_index, const string &value) {
    if (array_index.empty()) {
        midcode_ << name + " = " + value << endl;

        AddMidcode(new Midcode(MidcodeInstr::ASSIGN, name, value));
    } else {
        midcode_ << name + "[" + array_index + "] = " + value << endl;

        AddMidcode(new Midcode(MidcodeInstr::ASSIGN_ARRAY, name, array_index, value));
    }
}

void MidcodeGenerator::PrintLoadToTempReg(const string &name, const string &array_index, int temp_reg_count) {
    if (array_index.empty()) {
        midcode_ << "#" << temp_reg_count << " = " << name << endl;

        AddMidcode(new Midcode(MidcodeInstr::LOAD, temp_reg_count, name));
    } else {
        midcode_ << "#" << temp_reg_count << " = " << name + "[" + array_index + "]" << endl;

        AddMidcode(new Midcode(MidcodeInstr::LOAD_ARRAY, name, array_index, temp_reg_count));
    }
}

void MidcodeGenerator::PrintPushParameter(const string &function, const string &value, int count) {

    midcode_ << "push " + value << endl;

    AddMidcode(new Midcode(MidcodeInstr::PUSH, function, value, count));
}

void MidcodeGenerator::PrintCallFunction(const string &name) {
    midcode_ << "call " + name << endl;

    AddMidcode(new Midcode(MidcodeInstr::CALL, name));
}

void MidcodeGenerator::PrintFuncEnd() {
    midcode_ << "function end" << endl;

    AddMidcode(new Midcode(MidcodeInstr::FUNCTION_END));
}

void MidcodeGenerator::PrintSave(const string &function_name) {
    midcode_ << "save " + function_name << endl;

    AddMidcode(new Midcode(MidcodeInstr::SAVE, function_name));
}

void MidcodeGenerator::PrintAssignReturn(int temp_reg_count) {
    midcode_ << "#" << temp_reg_count << " = RET" << endl;

    AddMidcode(new Midcode(MidcodeInstr::ASSIGN_RETURN, temp_reg_count));
}

void MidcodeGenerator::PrintRegOpReg(int result_reg, int op_reg1, int op_reg2, const string &op) {
    midcode_ << "#" << result_reg << " = #" << op_reg1 << " " + op + " #" << op_reg2 << endl;

    AddMidcode(new Midcode(midcodeinstr::GetOperatorInstr(op),
                           OperaMember::REG_OP_REG, result_reg, op_reg1, op_reg2));
}

void MidcodeGenerator::PrintRegOpNumber(int result_reg, int op_reg, const string &number, const string &op) {
    midcode_ << "#" << result_reg << " = #" << op_reg << " " + op + " " + number << endl;

    AddMidcode(new Midcode(midcodeinstr::GetOperatorInstr(op),
                           OperaMember::REG_OP_NUMBER, result_reg, op_reg, number));
}

void MidcodeGenerator::PrintNumberOpReg(int result_reg, const string &number, int op_reg, const string &op) {
    midcode_ << "#" << result_reg << " = " + number + " " + op + " #" << op_reg << endl;

    AddMidcode(new Midcode(midcodeinstr::GetOperatorInstr(op),
                           OperaMember::NUMBER_OP_REG, result_reg, op_reg, number));
}

void
MidcodeGenerator::PrintNumberOpNumber(int result_reg, const string &number1, const string &number2, const string &op) {
    midcode_ << "#" << result_reg << " = " + number1 + " " + op + " " + number2 << endl;

    AddMidcode(new Midcode(midcodeinstr::GetOperatorInstr(op),
                           OperaMember::NUMBER_OP_NUMBER, result_reg, number1, number2));
}

void MidcodeGenerator::PrintNeg(int result_reg, const string &number) {
    midcode_ << "#" << result_reg << " = -" + number << endl;

    AddMidcode(new Midcode(MidcodeInstr::NEG, result_reg, number));
}