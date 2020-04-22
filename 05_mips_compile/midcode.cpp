#include "midcode.h"

Midcode::Midcode(MidcodeInstr instr) {
	this->Init();
	this->instr_ = instr;
}

Midcode::Midcode(MidcodeInstr instr, string label) {
	this->Init();
	this->instr_ = instr;
	this->label_ = label;
}

Midcode::Midcode(MidcodeInstr instr, int count) {
	this->Init();
	this->instr_ = instr;
	this->count_ = count;
}

Midcode::Midcode(MidcodeInstr instr, string reg1, int count) {
	this->Init();
	this->instr_ = instr;
	this->reg1_ = reg1;
	this->count_ = count;
}

Midcode::Midcode(MidcodeInstr instr, int count, string reg1) {
	this->Init();
	this->instr_ = instr;
	this->count_ = count;
	this->reg1_ = reg1;
}

Midcode::Midcode(MidcodeInstr instr, string reg_result, string reg1) {
	this->Init();
	this->instr_ = instr;
	this->reg_result_ = reg_result;
	this->reg1_ = reg1;
}

Midcode::Midcode(MidcodeInstr instr, string reg1, string reg2, int count) {
	this->Init();
	this->instr_ = instr;
	this->reg1_ = reg1;
	this->reg2_ = reg2;
	this->count_ = count;
}

Midcode::Midcode(MidcodeInstr instr, string reg_result, string reg1, string reg2) {
	this->Init();
	this->instr_ = instr;
	this->reg_result_ = reg_result;
	this->reg1_ = reg1;
	this->reg2_ = reg2;
}

Midcode::Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result, int temp1, int temp2) {
	this->Init();
	this->instr_ = instr;
	this->opera_member_ = opera_member;
	this->temp_result_ = temp_result;
	this->temp1_ = temp1;
	this->temp2_ = temp2;
}

Midcode::Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result, int temp1, string reg2) {
	this->Init();
	this->instr_ = instr;
	this->opera_member_ = opera_member;
	this->temp_result_ = temp_result;
	this->temp1_ = temp1;
	this->reg2_ = reg2;
}

Midcode::Midcode(MidcodeInstr instr, OperaMember opera_member, int temp_result, string reg1, string reg2) {
	this->Init();
	this->instr_ = instr;
	this->opera_member_ = opera_member;
	this->temp_result_ = temp_result;
	this->reg1_ = reg1;
	this->reg2_ = reg2;
}

Midcode::Midcode(MidcodeInstr instr, OperaMember opera_member, string reg_result, string reg1, string reg2) {
	this->Init();
	this->instr_ = instr;
	this->opera_member_ = opera_member;
	this->reg_result_ = reg_result;
	this->reg1_ = reg1;
	this->reg2_ = reg2;
}

void Midcode::Init() {
	this->opera_member_ = OperaMember::REG_OP_REG;

	this->count_ = 0;
	this->label_ = "";

	this->temp1_ = 0;
	this->temp2_ = 0;
	this->temp_result_ = 0;
}

MidcodeInstr Midcode::instr() {
	return this->instr_;
}

OperaMember Midcode::opera_member() {
	return this->opera_member_;
}

int Midcode::count() {
	return this->count_;
}

string Midcode::label() {
	return this->label_;
}

string Midcode::reg1() {
	return this->reg1_;
}

string Midcode::reg2() {
	return this->reg2_;
}

string Midcode::reg_result() {
	return this->reg_result_;
}

int Midcode::temp_result() {
	return this->temp_result_;
}

string Midcode::GetTempReg1() {
	return "#" + to_string(this->temp1_);
}

string Midcode::GetTempReg2() {
	return "#" + to_string(this->temp2_);
}

string Midcode::GetTempRegResult() {
	return "#" + to_string(this->temp_result_);
}

string Midcode::GetJumpLabel() {
	return "Label_" + to_string(this->count_);
}

string Midcode::GetLabel() {
	return "Label_" + to_string(this->count_);
}

string Midcode::GetString() {
	return "str_" + to_string(this->count_);
}

string Midcode::GetTempReg() {
	return "#" + to_string(this->count_);
}

int Midcode::GetInteger() {
	return stoi(this->label_);
}

int Midcode::GetChar() {
	return (int)this->label_[1];
}