#include "midcode.h"

using namespace std;

Midcode::Midcode(MidcodeInstr instr) {
    Init();
    instr_ = instr;
}

Midcode::Midcode(MidcodeInstr instr, const string &label) {
    Init();
    instr_ = instr;
    label_ = label;
}

Midcode::Midcode(MidcodeInstr instr, int count) {
    Init();
    instr_ = instr;
    count_ = count;
}

Midcode::Midcode(MidcodeInstr instr, const string &reg1, int count) {
    Init();
    instr_ = instr;
    reg1_ = reg1;
    count_ = count;
}

Midcode::Midcode(MidcodeInstr instr, int count, const string &reg1) {
    Init();
    instr_ = instr;
    count_ = count;
    reg1_ = reg1;
}

Midcode::Midcode(MidcodeInstr instr, const string &reg_result, const string &reg1) {
    Init();
    instr_ = instr;
    reg_result_ = reg_result;
    reg1_ = reg1;
}

Midcode::Midcode(MidcodeInstr instr, const string &reg1, const string &reg2, int count) {
    Init();
    instr_ = instr;
    reg1_ = reg1;
    reg2_ = reg2;
    count_ = count;
}

Midcode::Midcode(MidcodeInstr instr, const string &reg_result, const string &reg1, const string &reg2) {
    Init();
    instr_ = instr;
    reg_result_ = reg_result;
    reg1_ = reg1;
    reg2_ = reg2;
}

Midcode::Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result, int temp1, int temp2) {
    Init();
    instr_ = instr;
    opera_member_ = opera_member;
    temp_result_ = temp_result;
    temp1_ = temp1;
    temp2_ = temp2;
}

Midcode::Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result, int temp1, const string &reg2) {
    Init();
    instr_ = instr;
    opera_member_ = opera_member;
    temp_result_ = temp_result;
    temp1_ = temp1;
    reg2_ = reg2;
}

Midcode::Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result,
                 const string &reg1, const string &reg2) {
    Init();
    instr_ = instr;
    opera_member_ = opera_member;
    temp_result_ = temp_result;
    reg1_ = reg1;
    reg2_ = reg2;
}

Midcode::Midcode(MidcodeInstr instr, OperaMember opera_member,
                 const string &reg_result, const string &reg1, const string &reg2) {
    Init();
    instr_ = instr;
    opera_member_ = opera_member;
    reg_result_ = reg_result;
    reg1_ = reg1;
    reg2_ = reg2;
}

void Midcode::Init() {
    opera_member_ = OperaMember::REG_OP_REG;

    count_ = 0;
    label_ = "";

    temp1_ = 0;
    temp2_ = 0;
    temp_result_ = 0;
}

MidcodeInstr Midcode::instr() {
    return instr_;
}

OperaMember Midcode::opera_member() {
    return opera_member_;
}

int Midcode::count() {
    return count_;
}

string Midcode::label() {
    return label_;
}

string Midcode::reg1() {
    return reg1_;
}

string Midcode::reg2() {
    return reg2_;
}

string Midcode::reg_result() {
    return reg_result_;
}

int Midcode::temp_result() {
    return temp_result_;
}

string Midcode::GetTempReg1() {
    return "#" + to_string(temp1_);
}

string Midcode::GetTempReg2() {
    return "#" + to_string(temp2_);
}

string Midcode::GetTempRegResult() {
    return "#" + to_string(temp_result_);
}

string Midcode::GetJumpLabel() {
    return "Label_" + to_string(count_);
}

string Midcode::GetLabel() {
    return "Label_" + to_string(count_);
}

string Midcode::GetString() {
    return "str_" + to_string(count_);
}

string Midcode::GetTempReg() {
    return "#" + to_string(count_);
}

int Midcode::GetInteger() {
    return stoi(label_);
}

int Midcode::GetChar() {
    return (int) label_[1];
}