#include "mips_generator.h"

#include <utility>

using namespace std;

MipsGenerator::MipsGenerator(const string &outputFileName, int temp_count,
                             StringTable *stringTable, CheckTable *check_table,
                             map<string, SymbolTable *> symbolTableMap, list<Midcode *> midcode_list) {

    objcode_ = new Objcode(outputFileName);

    string_table_ = stringTable;
    check_table_ = check_table;
    symbol_table_map_ = std::move(symbolTableMap);
    midcode_list_ = std::move(midcode_list);

    temp_count_ = temp_count;
    dm_offset_ = 0;
    temp_offset_ = 0;
}

void MipsGenerator::LoadTable(int level, const string &name) {
    check_table_->SetTable(level, symbol_table_map_.at(name));
}

void MipsGenerator::InitConstString() {
    for (int i = 0; i < string_table_->GetStringCount(); i++) {
        string data_string = "str_"
                             + to_string(i)
                             + ": .asciiz \""
                             + string_table_->GetString(i)
                             + "\"";

        objcode_->Output(MipsInstr::data_string, data_string);
    }
}

void MipsGenerator::InitVariable(const string &function) {
    auto table = symbol_table_map_.at(function)->symbol_map();
    auto iter = table.begin();

    dm_offset_ = 0;
    while (iter != table.end()) {
        if (iter->second->kind() == KindSymbol::ARRAY) {
            iter->second->set_offset(dm_offset_);
            dm_offset_ += 4 * iter->second->array_length();
        } else if (iter->second->kind() == KindSymbol::VARIABLE
                   || iter->second->kind() == KindSymbol::PARAMETER) {
            iter->second->set_offset(dm_offset_);
            dm_offset_ += 4;
        }
        iter++;
    }
    function_offset_map_.insert(pair<string, int>(function, dm_offset_));
    temp_offset_ = dm_offset_;
}

void MipsGenerator::InitData() {

    auto iter = symbol_table_map_.begin();
    while (iter != symbol_table_map_.end()) {
        InitVariable(iter->first);
        iter++;
    }

    objcode_->Output(MipsInstr::data);
    LoadTable(0, "global");
    InitVariable("global");
    InitConstString();

    objcode_->Output(MipsInstr::data_align, 4);
    objcode_->Output(MipsInstr::data_identifier, RA_SPACE, 500);
    objcode_->Output(MipsInstr::data_identifier, PARA_SPACE, 500);
    objcode_->Output(MipsInstr::data_identifier, FUNC_SPACE, 2000);
    objcode_->Output();
}

void MipsGenerator::PrintText() {
    objcode_->Output(MipsInstr::text);
    objcode_->Output();
}

void MipsGenerator::InitStack() {
    objcode_->Output(MipsInstr::la, GLOBAL_POINT, FUNC_SPACE);
    objcode_->Output(MipsInstr::la, FUNC_POINT, FUNC_SPACE);
    objcode_->Output(MipsInstr::la, RA_POINT, RA_SPACE);
    objcode_->Output(MipsInstr::la, PARA_POINT, PARA_SPACE);
    objcode_->Output(MipsInstr::addi, FUNC_POINT, FUNC_POINT, dm_offset_);
    objcode_->Output();
}

void MipsGenerator::InitText() {
    PrintText();
    InitStack();

    objcode_->Output(MipsInstr::jal, "main");
    objcode_->Output(MipsInstr::li, Reg::v0, 10);
    objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::SaveVariable(const string &name, Reg reg) {

    int offset = check_table_->FindSymbol(name)->offset();

    if (check_table_->GetSymbolLevel(name) == 0) {
        objcode_->Output(MipsInstr::sw, reg, GLOBAL_POINT, offset);
    } else {
        objcode_->Output(MipsInstr::sw, reg, FUNC_POINT, offset);
    }
}

void MipsGenerator::LoadVariable(const string &name, Reg reg) {
    int offset = check_table_->FindSymbol(name)->offset();

    if (check_table_->GetSymbolLevel(name) == 0) {
        objcode_->Output(MipsInstr::lw, reg, GLOBAL_POINT, offset);
    } else {
        objcode_->Output(MipsInstr::lw, reg, FUNC_POINT, offset);
    }
}

bool MipsGenerator::IsInteger(const string &str) {
    for (char i : str) {
        if (!isdigit(i) && i != '+' && i != '-') {
            return false;
        }
    }
    return true;
}

bool MipsGenerator::IsChar(const string &str) {
    return str[0] == '\'';
}

bool MipsGenerator::IsTemporary(const string &str) {
    return str[0] == '#' && isdigit(str[1]);
}

void MipsGenerator::LoadValue(const string &value, Reg reg) {
    if (IsInteger(value)) {
        objcode_->Output(MipsInstr::li, reg, stoi(value));
    } else if (IsChar(value)) {
        objcode_->Output(MipsInstr::li, reg, (int) value[1]);
    } else if (IsTemporary(value)) {
        LoadTemporary(value, reg);
    } else if (IsConstVariable(value)) {
        objcode_->Output(MipsInstr::li, reg, GetConstVariable(value));
    } else {
        LoadVariable(value, reg);
    }
}

void MipsGenerator::SaveTemporary(const string &name, Reg reg) {
    if (temporary_offset_map_.find(name) == temporary_offset_map_.end()) {
        temp_offset_ += 4;
        temporary_offset_map_.insert(pair<string, int>(name, temp_offset_));
    }

    int offset = temporary_offset_map_.at(name);
    objcode_->Output(MipsInstr::sw, reg, FUNC_POINT, offset);
}

void MipsGenerator::LoadTemporary(const string &name, Reg reg) {
    if (temporary_offset_map_.find(name) == temporary_offset_map_.end()) {
        temp_offset_ += 4;
        temporary_offset_map_.insert(pair<string, int>(name, temp_offset_));
    }

    int offset = temporary_offset_map_.at(name);
    objcode_->Output(MipsInstr::lw, reg, FUNC_POINT, offset);
}

bool MipsGenerator::IsConstVariable(const string &name) {
    Symbol *symbol = check_table_->FindSymbol(name);
    if (symbol != nullptr && symbol->kind() == KindSymbol::CONST) {
        return true;
    } else {
        return false;
    }
}

int MipsGenerator::GetConstVariable(const string &name) {
    Symbol *symbol = check_table_->FindSymbol(name);

    if (symbol->type() == TypeSymbol::INT) {
        return stoi(symbol->const_value());
    } else {
        return symbol->const_value()[1];
    }
}

void MipsGenerator::GenerateScanf(const string &variable_name, int type) {
    objcode_->Output(MipsInstr::li, Reg::v0, type);
    objcode_->Output(MipsInstr::syscall);

    SaveVariable(variable_name, Reg::v0);
}

void MipsGenerator::GeneratePrintfIntChar(Midcode *midcode, int type) {
    string value = midcode->label();
    if (IsInteger(value)) {
        objcode_->Output(MipsInstr::li, Reg::a0, midcode->GetInteger());
    } else if (IsChar(value)) {
        objcode_->Output(MipsInstr::li, Reg::v0, midcode->GetChar());
    } else if (IsTemporary(value)) {
        LoadTemporary(value, Reg::a0);
    } else if (IsConstVariable(value)) {
        objcode_->Output(MipsInstr::li, Reg::a0, GetConstVariable(value));
    } else {
        LoadVariable(value, Reg::a0);
    }

    objcode_->Output(MipsInstr::li, Reg::v0, type);
    objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GeneratePrintfString(const string &str) {
    objcode_->Output(MipsInstr::la, Reg::a0, str);
    objcode_->Output(MipsInstr::li, Reg::v0, 4);
    objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GeneratePrintfEnd() {
    objcode_->Output(MipsInstr::li, Reg::a0, 10);
    objcode_->Output(MipsInstr::li, Reg::v0, 11);
    objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GenerateLabel(const string &label) {
    objcode_->Output(MipsInstr::label, label);
}

void MipsGenerator::GenerateJump(const string &label) {
    objcode_->Output(MipsInstr::j, label);
}

void MipsGenerator::GenerateAssign(const string &result, const string &value) {

    if (IsInteger(value)) {
        objcode_->Output(MipsInstr::li, RD, stoi(value));
    } else if (IsChar(value)) {
        objcode_->Output(MipsInstr::li, RD, (int) value[1]);
    } else if (IsTemporary(value)) {
        LoadTemporary(value, RS);
        objcode_->Output(MipsInstr::move, RD, RS);
    } else if (IsConstVariable(value)) {
        objcode_->Output(MipsInstr::li, RD, GetConstVariable(value));
    } else {
        LoadVariable(value, RS);
        objcode_->Output(MipsInstr::move, RD, RS);
    }

    if (IsTemporary(result)) {
        SaveTemporary(result, RD);
    } else {
        SaveVariable(result, RD);
    }
}

void MipsGenerator::GenerateAssignReturn(const string &temp) {
    objcode_->Output(MipsInstr::move, RD, Reg::v0);
    SaveTemporary(temp, RD);
}

void MipsGenerator::SetArrayIndex(const string &index, Reg base, int &offset, bool &is_use_temp) {
    if (IsInteger(index)) {
        offset += 4 * stoi(index);
    } else if (IsChar(index)) {
        offset += 4 * ((int) index[1]);
    } else if (IsTemporary(index)) {
        LoadTemporary(index, TEMP);
        objcode_->Output(MipsInstr::sll, TEMP, TEMP, 2);
        objcode_->Output(MipsInstr::addi, TEMP, TEMP, offset);
        objcode_->Output(MipsInstr::add, TEMP, base, TEMP);
        is_use_temp = true;
    } else if (IsConstVariable(index)) {
        objcode_->Output(MipsInstr::li, TEMP, GetConstVariable(index));
        objcode_->Output(MipsInstr::sll, TEMP, TEMP, 2);
        objcode_->Output(MipsInstr::addi, TEMP, TEMP, offset);
        objcode_->Output(MipsInstr::add, TEMP, base, TEMP);
        is_use_temp = true;
    } else {
        LoadVariable(index, TEMP);
        objcode_->Output(MipsInstr::sll, TEMP, TEMP, 2);
        objcode_->Output(MipsInstr::addi, TEMP, TEMP, offset);
        objcode_->Output(MipsInstr::add, TEMP, base, TEMP);
        is_use_temp = true;
    }
}

void MipsGenerator::GenerateAssignArray(const string &array, const string &index, const string &value) {

    bool is_use_temp = false;
    int offset = check_table_->FindSymbol(array)->offset();
    int level = check_table_->GetSymbolLevel(array);
    Reg base = level == 0 ? GLOBAL_POINT : FUNC_POINT;

    SetArrayIndex(index, base, offset, is_use_temp);
    LoadValue(value, RT);

    if (is_use_temp) {
        objcode_->Output(MipsInstr::sw, RT, TEMP, 0);
    } else {
        objcode_->Output(MipsInstr::sw, RT, base, offset);
    }
}

void MipsGenerator::GenerateLoadArray(const string &temp, const string &array, const string &index) {

    bool is_use_temp = false;
    int offset = check_table_->FindSymbol(array)->offset();
    int level = check_table_->GetSymbolLevel(array);
    Reg base = level == 0 ? GLOBAL_POINT : FUNC_POINT;

    SetArrayIndex(index, base, offset, is_use_temp);

    if (is_use_temp) {
        objcode_->Output(MipsInstr::lw, RT, TEMP, 0);
    } else {
        objcode_->Output(MipsInstr::lw, RT, base, offset);
    }

    SaveTemporary(temp, RT);
}

void MipsGenerator::SetOperand(const string &value, Reg reg, bool &is_immediate, int &immediate) {
    if (IsInteger(value)) {
        is_immediate = true;
        immediate = stoi(value);
    } else if (IsChar(value)) {
        is_immediate = true;
        immediate = (int) value[1];
    } else if (IsConstVariable(value)) {
        is_immediate = true;
        immediate = GetConstVariable(value);
    } else {
        LoadVariable(value, reg);
    }
}

void MipsGenerator::DealNumberOpNumber(const string &value_1, const string &value_2,
                                       int &immediate_1, bool &is_immediate_1,
                                       int &immediate_2, bool &is_immediate_2) {

    SetOperand(value_1, RS, is_immediate_1, immediate_1);
    SetOperand(value_2, RT, is_immediate_2, immediate_2);
}

void MipsGenerator::DealNumberOpReg(const string &value_1, const string &temp_2,
                                    int &immediate_1, bool &is_immediate_1) {

    SetOperand(value_1, RS, is_immediate_1, immediate_1);
    LoadTemporary(temp_2, RT);
}

void MipsGenerator::DealRegOpNumber(const string &temp_1, const string &value_2,
                                    int &immediate_2, bool &is_immediate_2) {

    LoadTemporary(temp_1, RS);
    SetOperand(value_2, RT, is_immediate_2, immediate_2);
}

void MipsGenerator::DealRegOpReg(const string &temp_1, const string &temp_2) {
    LoadTemporary(temp_1, RS);
    LoadTemporary(temp_2, RT);
}

void MipsGenerator::GenerateOperate(Midcode *midcode, const string &result,
                                    MidcodeInstr op, OperaMember member_type) {

    bool is_immediate_1 = false;
    int immediate_1 = 0;
    bool is_immediate_2 = false;
    int immediate_2 = 0;

    switch (member_type) {
        case OperaMember::REG_OP_REG:
            DealRegOpReg(midcode->GetTempReg1(), midcode->GetTempReg2());
            break;
        case OperaMember::REG_OP_NUMBER:
            DealRegOpNumber(midcode->GetTempReg1(), midcode->reg2(),
                            immediate_2, is_immediate_2);
            break;
        case OperaMember::NUMBER_OP_REG:
            DealNumberOpReg(midcode->reg2(), midcode->GetTempReg1(),
                            immediate_1, is_immediate_1);
            break;
        case OperaMember::NUMBER_OP_NUMBER:
            DealNumberOpNumber(midcode->reg1(), midcode->reg2(),
                               immediate_1, is_immediate_1,
                               immediate_2, is_immediate_2);
            break;
        default:
            assert(0);
    }

    int flag;
    if (is_immediate_1 && is_immediate_2) {
        flag = 3;
    } else if (is_immediate_2) {
        flag = 2;
    } else if (is_immediate_1) {
        flag = 1;
    } else {
        flag = 0;
    }

    switch (op) {
        case MidcodeInstr::ADD:
            switch (flag) {
                case 0:
                    objcode_->Output(MipsInstr::add, RD, RS, RT);
                    break;
                case 1:
                    objcode_->Output(MipsInstr::addi, RD, RT, immediate_1);
                    break;
                case 2:
                    objcode_->Output(MipsInstr::addi, RD, RS, immediate_2);
                    break;
                case 3:
                    objcode_->Output(MipsInstr::li, RD, immediate_1 + immediate_2);
                    break;
                default:
                    assert(0);
            }
            break;
        case MidcodeInstr::SUB:
            switch (flag) {
                case 0:
                    objcode_->Output(MipsInstr::sub, RD, RS, RT);
                    break;
                case 1:
                    objcode_->Output(MipsInstr::li, RS, immediate_1);
                    objcode_->Output(MipsInstr::sub, RD, RS, RT);
                    break;
                case 2:
                    objcode_->Output(MipsInstr::subi, RD, RS, immediate_2);
                    break;
                case 3:
                    objcode_->Output(MipsInstr::li, RD, immediate_1 - immediate_2);
                    break;
                default:
                    assert(0);
            }
            break;
        case MidcodeInstr::MUL:
            switch (flag) {
                case 0:
                    objcode_->Output(MipsInstr::mul, RD, RS, RT);
                    break;
                case 1:
                    objcode_->Output(MipsInstr::mul, RD, RT, immediate_1);
                    break;
                case 2:
                    objcode_->Output(MipsInstr::mul, RD, RS, immediate_2);
                    break;
                case 3:
                    objcode_->Output(MipsInstr::li, RD, immediate_1 * immediate_2);
                    break;
                default:
                    assert(0);
            }
            break;
        case MidcodeInstr::DIV:
            switch (flag) {
                case 0:
                    objcode_->Output(MipsInstr::div, RD, RS, RT);
                    break;
                case 1:
                    objcode_->Output(MipsInstr::li, RS, immediate_1);
                    objcode_->Output(MipsInstr::div, RD, RS, RT);
                    break;
                case 2:
                    objcode_->Output(MipsInstr::div, RD, RS, immediate_2);
                    break;
                case 3:
                    objcode_->Output(MipsInstr::li, RD, immediate_1 / immediate_2);
                    break;
                default:
                    assert(0);
            }
            break;
        default:
            assert(0);
    }

    if (IsTemporary(result)) {
        SaveTemporary(result, RD);
    } else {
        SaveVariable(result, RD);
    }
}

void MipsGenerator::GenerateOperate(list<Midcode *>::iterator &iter, Midcode *midcode) {
    Midcode *next_midcode = *(++iter);

    if (next_midcode->instr() == MidcodeInstr::ASSIGN
        && next_midcode->reg1() == midcode->GetTempRegResult()) {

        GenerateOperate(midcode, next_midcode->reg_result(),
                        midcode->instr(), midcode->opera_member());
    } else {
        iter--;
        GenerateOperate(midcode, midcode->GetTempRegResult(),
                        midcode->instr(), midcode->opera_member());
    }
}

void MipsGenerator::GenerateStep(list<Midcode *>::iterator &iter, Midcode *&midcode) {
    midcode = *(++iter);
    GenerateOperate(midcode, midcode->reg_result(),
                    midcode->instr(), midcode->opera_member());
}

void MipsGenerator::GenerateNeg(const string &temp_result, const string &value) {

    bool is_immediate = false;
    int immediate = 0;
    SetOperand(value, RS, is_immediate, immediate);

    if (is_immediate) {
        objcode_->Output(MipsInstr::li, RD, -immediate);
    } else {
        objcode_->Output(MipsInstr::sub, RD, Reg::zero, RS);
    }

    if (IsTemporary(temp_result)) {
        SaveTemporary(temp_result, RD);
    } else {
        SaveVariable(temp_result, RD);
    }
}

void MipsGenerator::SetJudgeReg(const string &value, Reg reg) {
    if (IsInteger(value)) {
        objcode_->Output(MipsInstr::li, reg, stoi(value));
    } else if (IsChar(value)) {
        objcode_->Output(MipsInstr::li, reg, (int) value[1]);
    } else if (IsConstVariable(value)) {
        objcode_->Output(MipsInstr::li, reg, GetConstVariable(value));
    } else if (IsTemporary(value)) {
        LoadTemporary(value, reg);
    } else {
        LoadVariable(value, reg);
    }
}

void MipsGenerator::GenerateJudge(Midcode *midcode, MidcodeInstr judge) {
    MipsInstr mips_instr;
    bool is_two_judge = true;

    switch (judge) {
        case MidcodeInstr::BGT:
            mips_instr = MipsInstr::bgt;
            SetJudgeReg(midcode->reg1(), RS);
            SetJudgeReg(midcode->reg2(), RT);
            break;
        case MidcodeInstr::BGE:
            mips_instr = MipsInstr::bge;
            SetJudgeReg(midcode->reg1(), RS);
            SetJudgeReg(midcode->reg2(), RT);
            break;
        case MidcodeInstr::BLT:
            mips_instr = MipsInstr::blt;
            SetJudgeReg(midcode->reg1(), RS);
            SetJudgeReg(midcode->reg2(), RT);
            break;
        case MidcodeInstr::BLE:
            mips_instr = MipsInstr::ble;
            SetJudgeReg(midcode->reg1(), RS);
            SetJudgeReg(midcode->reg2(), RT);
            break;
        case MidcodeInstr::BEQ:
            mips_instr = MipsInstr::beq;
            SetJudgeReg(midcode->reg1(), RS);
            SetJudgeReg(midcode->reg2(), RT);
            break;
        case MidcodeInstr::BNE:
            mips_instr = MipsInstr::bne;
            SetJudgeReg(midcode->reg1(), RS);
            SetJudgeReg(midcode->reg2(), RT);
            break;
        case MidcodeInstr::BEZ:
            mips_instr = MipsInstr::beq;
            SetJudgeReg(midcode->reg1(), RS);
            is_two_judge = false;
            break;
        case MidcodeInstr::BNZ:
            mips_instr = MipsInstr::bne;
            SetJudgeReg(midcode->reg1(), RS);
            is_two_judge = false;
            break;
        default:
            assert(0);
    }

    if (is_two_judge) {
        objcode_->Output(mips_instr, RS, RT, midcode->GetLabel());
    } else {
        objcode_->Output(mips_instr, RS, Reg::zero, midcode->GetLabel());
    }

}

void MipsGenerator::GeneratePush(const string &value) {
    LoadValue(value, RT);
    objcode_->Output(MipsInstr::sw, RT, PARA_POINT, 0);
    objcode_->Output(MipsInstr::addi, PARA_POINT, PARA_POINT, 4);
}

void MipsGenerator::GenerateCall(const string &prev_name, const string &call_name) {

    objcode_->Output(MipsInstr::addi, FUNC_POINT, FUNC_POINT, 2000);

    objcode_->Output(MipsInstr::jal, call_name);

    objcode_->Output(MipsInstr::subi, FUNC_POINT, FUNC_POINT, 2000);

    int length = check_table_->FindSymbol(call_name, 0)->GetParameterCount();
    objcode_->Output(MipsInstr::subi, PARA_POINT, PARA_POINT, 4 * length);

    objcode_->Output(MipsInstr::subi, RA_POINT, RA_POINT, 4);
    objcode_->Output(MipsInstr::lw, Reg::ra, RA_POINT, 0);
}

void MipsGenerator::GenerateSave() {
    objcode_->Output(MipsInstr::sw, Reg::ra, RA_POINT, 0);
    objcode_->Output(MipsInstr::addi, RA_POINT, RA_POINT, 4);
}

void MipsGenerator::GenerateFunctionEnd(list<Midcode *>::iterator &iter) {
    iter++;
}

void MipsGenerator::GenerateReturn(Midcode *midcode, bool is_return_value) {

    if (is_return_value) {
        if (IsInteger(midcode->label())) {
            objcode_->Output(MipsInstr::li, Reg::v0, midcode->GetInteger());
        } else if (IsChar(midcode->label())) {
            objcode_->Output(MipsInstr::li, Reg::v0, midcode->GetChar());
        } else if (IsTemporary(midcode->label())) {
            LoadTemporary(midcode->label(), RS);
            objcode_->Output(MipsInstr::move, Reg::v0, RS);
        } else if (IsConstVariable(midcode->label())) {
            objcode_->Output(MipsInstr::li, Reg::v0,
                             GetConstVariable(midcode->label()));
        } else {
            LoadVariable(midcode->label(), RS);
            objcode_->Output(MipsInstr::move, Reg::v0, RS);
        }
    }

    objcode_->Output(MipsInstr::jr, Reg::ra);
}

void MipsGenerator::GenerateParameter(const string &function_name, const string &value, int count) {

    int parameter_count = check_table_->FindSymbol(
            function_name, 0)->GetParameterCount();
    int para_offset = 4 * (count - parameter_count);
    int func_offset = check_table_->FindSymbol(value)->offset();

    objcode_->Output(MipsInstr::lw, RT, PARA_POINT, para_offset);
    objcode_->Output(MipsInstr::sw, RT, FUNC_POINT, func_offset);
}

void MipsGenerator::GenerateBody(const string &function_name, list<Midcode *>::iterator &iter) {
    Midcode *midcode;
    int parameter_count = 0;

    while (iter != midcode_list_.end()) {
        midcode = *iter;

        switch (midcode->instr()) {
            case MidcodeInstr::SCANF_INT:
                GenerateScanf(midcode->label(), 5);
                break;
            case MidcodeInstr::SCANF_CHAR:
                GenerateScanf(midcode->label(), 12);
                break;
            case MidcodeInstr::PRINTF_INT:
                GeneratePrintfIntChar(midcode, 1);
                break;
            case MidcodeInstr::PRINTF_CHAR:
                GeneratePrintfIntChar(midcode, 11);
                break;
            case MidcodeInstr::PRINTF_STRING:
                GeneratePrintfString(midcode->GetString());
                break;
            case MidcodeInstr::PRINTF_END:
                GeneratePrintfEnd();
                break;
            case MidcodeInstr::LABEL:
                GenerateLabel(midcode->GetLabel());
                break;
            case MidcodeInstr::JUMP:
                GenerateJump(midcode->GetJumpLabel());
                break;
            case MidcodeInstr::ASSIGN:
                GenerateAssign(midcode->reg_result(), midcode->reg1());
                break;
            case MidcodeInstr::ASSIGN_RETURN:
                GenerateAssignReturn(midcode->GetTempReg());
                break;
            case MidcodeInstr::ASSIGN_ARRAY:
                GenerateAssignArray(midcode->reg_result(),
                                    midcode->reg1(), midcode->reg2());
                break;
            case MidcodeInstr::LOAD_ARRAY:
                GenerateLoadArray(midcode->GetTempReg(), midcode->reg1(), midcode->reg2());
                break;
            case MidcodeInstr::ADD:
                GenerateOperate(iter, midcode);
                break;
            case MidcodeInstr::SUB:
                GenerateOperate(iter, midcode);
                break;
            case MidcodeInstr::MUL:
                GenerateOperate(iter, midcode);
                break;
            case MidcodeInstr::DIV:
                GenerateOperate(iter, midcode);
                break;
            case MidcodeInstr::STEP:
                GenerateStep(iter, midcode);
                break;
            case MidcodeInstr::NEG:
                GenerateNeg("#" + to_string(midcode->count()), midcode->reg1());
                break;
            case MidcodeInstr::BGT:
                GenerateJudge(midcode, MidcodeInstr::BGT);
                break;
            case MidcodeInstr::BGE:
                GenerateJudge(midcode, MidcodeInstr::BGE);
                break;
            case MidcodeInstr::BLT:
                GenerateJudge(midcode, MidcodeInstr::BLT);
                break;
            case MidcodeInstr::BLE:
                GenerateJudge(midcode, MidcodeInstr::BLE);
                break;
            case MidcodeInstr::BEQ:
                GenerateJudge(midcode, MidcodeInstr::BEQ);
                break;
            case MidcodeInstr::BNE:
                GenerateJudge(midcode, MidcodeInstr::BNE);
                break;
            case MidcodeInstr::BEZ:
                GenerateJudge(midcode, MidcodeInstr::BEZ);
                break;
            case MidcodeInstr::BNZ:
                GenerateJudge(midcode, MidcodeInstr::BNZ);
                break;
            case MidcodeInstr::PUSH:
                GeneratePush(midcode->reg2());
                break;
            case MidcodeInstr::CALL:
                GenerateCall(function_name, midcode->label());
                break;
            case MidcodeInstr::SAVE:
                GenerateSave();
                break;
            case MidcodeInstr::FUNCTION_END:
                return GenerateFunctionEnd(iter);
            case MidcodeInstr::RETURN:
                GenerateReturn(midcode, true);
                break;
            case MidcodeInstr::RETURN_NON:
                GenerateReturn(midcode, false);
                break;
            case MidcodeInstr::PARA_INT:
                GenerateParameter(function_name, midcode->label(), parameter_count++);
                break;
            case MidcodeInstr::PARA_CHAR:
                GenerateParameter(function_name, midcode->label(), parameter_count++);
                break;
            case MidcodeInstr::VAR_INT:
                break;
            case MidcodeInstr::VAR_CHAR:
                break;
            case MidcodeInstr::LOOP:
                break;
            default:
                assert(0);
        }

        iter++;
    }
}

void MipsGenerator::GenerateFunction(const string &function_name,
                                     list<Midcode *>::iterator &iter) {

    LoadTable(1, function_name);
    InitVariable(function_name);
    objcode_->Output(MipsInstr::label, function_name);
    iter++;
    GenerateBody(function_name, iter);
}

void MipsGenerator::Generate() {
    auto iter = midcode_list_.begin();
    Midcode *midcode;

    while (iter != midcode_list_.end()) {
        midcode = *iter;

        switch (midcode->instr()) {
            case MidcodeInstr::INT_FUNC_DECLARE:
                GenerateFunction(midcode->label(), iter);
                break;
            case MidcodeInstr::CHAR_FUNC_DECLARE:
                GenerateFunction(midcode->label(), iter);
                break;
            case MidcodeInstr::VOID_FUNC_DECLARE:
                GenerateFunction(midcode->label(), iter);
                break;
            case MidcodeInstr::VAR_INT:
                iter++;
                break;
            case MidcodeInstr::VAR_CHAR:
                iter++;
                break;
            default:
                assert(0);
        }
    }
}

void MipsGenerator::GenerateMips() {
    InitData();
    InitText();
    Generate();
}

void MipsGenerator::FileClose() {
    objcode_->FileClose();
}