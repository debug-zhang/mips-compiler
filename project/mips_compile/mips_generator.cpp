#include "mips_generator.h"

MipsGenerator::MipsGenerator(string outputFileName, int temp_count,
	StringTable* stringTable, CheckTable* check_table,
	map<string, SymbolTable*> symbolTableMap, list<Midcode*> midcode_list) {

	this->objcode_ = new Objcode(outputFileName);

	this->string_table_ = stringTable;
	this->check_table_ = check_table;
	this->symbol_table_map_ = symbolTableMap;
	this->midcode_list_ = midcode_list;

	this->temp_count_ = temp_count;
	this->dm_offset_ = 0;
}

void MipsGenerator::LoadTable(int level, string name) {
	this->check_table_->SetTable(level, symbol_table_map_.at(name));
}

void MipsGenerator::InitConstString() {
	for (int i = 0; i < this->string_table_->GetStringCount(); i++) {
		string data_string = "str_"
			+ to_string(i)
			+ ": .asciiz \""
			+ this->string_table_->GetString(i)
			+ "\"";

		this->objcode_->Output(MipsInstr::data_string, data_string);
	}
}

void MipsGenerator::InitVariable(string function) {
	map<string, Symbol*> table = this->symbol_table_map_.at(function)->symbol_map();
	map<string, Symbol*>::iterator iter = table.begin();

	this->dm_offset_ = 0;
	while (iter != table.end()) {
		if (iter->second->kind() == KindSymbol::ARRAY) {
			iter->second->set_offset(this->dm_offset_);
			this->dm_offset_ += 4 * iter->second->array_length();
		} else if (iter->second->kind() == KindSymbol::VARIABLE) {
			iter->second->set_offset(this->dm_offset_);
			this->dm_offset_ += 4;
		}
		iter++;
	}
	this->function_offset_map_.insert(pair<string, int>(function, this->dm_offset_));
	this->temp_offset_ = dm_offset_;
}

void MipsGenerator::InitData() {

	map<string, SymbolTable*>::iterator iter = symbol_table_map_.begin();
	while (iter != symbol_table_map_.end()) {
		this->InitVariable(iter->first);
		iter++;
	}

	this->objcode_->Output(MipsInstr::data);
	this->LoadTable(0, "global");
	this->InitVariable("global");
	this->InitConstString();

	this->objcode_->Output(MipsInstr::data_align, 4);
	this->objcode_->Output(MipsInstr::data_identifier, RA_SPACE, 400);
	this->objcode_->Output(MipsInstr::data_identifier, PARA_SPACE, 500);
	this->objcode_->Output(MipsInstr::data_identifier, GLOBAL_SPACE, 500);
	this->objcode_->Output(MipsInstr::data_identifier, FUNC_SPACE, 2000);
	this->objcode_->Output();
}

void MipsGenerator::PrintText() {
	this->objcode_->Output(MipsInstr::text);
	this->objcode_->Output();
}

void MipsGenerator::InitStack() {
	this->objcode_->Output(MipsInstr::la, GLOBAL_POINT, GLOBAL_SPACE);
	this->objcode_->Output(MipsInstr::la, FUNC_POINT, FUNC_SPACE);
	this->objcode_->Output(MipsInstr::la, RA_POINT, RA_SPACE);
	this->objcode_->Output(MipsInstr::la, PARA_POINT, PARA_SPACE);
	this->objcode_->Output();
}

void MipsGenerator::InitText() {
	this->PrintText();
	this->InitStack();

	this->objcode_->Output(MipsInstr::jal, "main");
	this->objcode_->Output(MipsInstr::li, Reg::v0, 10);
	this->objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::SaveVariable(string name, Reg reg) {

	int offset = this->check_table_->FindSymbol(name)->offset();

	if (this->check_table_->GetSymbolLevel(name) == 0) {
		this->objcode_->Output(MipsInstr::sw, reg, GLOBAL_POINT, offset);
	} else if (this->check_table_->FindSymbol(name)->kind() == KindSymbol::VARIABLE) {
		this->objcode_->Output(MipsInstr::sw, reg, FUNC_POINT, offset);
	} else {
		this->objcode_->Output(MipsInstr::sw, reg, PARA_POINT, offset);
	}
}

void MipsGenerator::LoadVariable(string name, Reg reg) {
	int offset = this->check_table_->FindSymbol(name)->offset();

	if (this->check_table_->GetSymbolLevel(name) == 0) {
		this->objcode_->Output(MipsInstr::lw, reg, GLOBAL_POINT, offset);
	} else if (this->check_table_->FindSymbol(name)->kind() == KindSymbol::VARIABLE) {
		this->objcode_->Output(MipsInstr::lw, reg, FUNC_POINT, offset);
	} else {
		this->objcode_->Output(MipsInstr::lw, reg, PARA_POINT, offset);
	}
}

bool MipsGenerator::IsInteger(string str) {
	for (int i = 0; i < (int)str.length(); i++) {
		if (!isdigit(str[i]) && str[i] != '+' && str[i] != '-') {
			return false;
		}
	}
	return true;
}

bool MipsGenerator::IsChar(string str) {
	return str[0] == '\'';
}

bool MipsGenerator::IsTemporary(string str) {
	return str[0] == '#' && isdigit(str[1]);
}

void  MipsGenerator::LoadValue(string value, Reg reg) {
	if (this->IsInteger(value)) {
		this->objcode_->Output(MipsInstr::li, reg, stoi(value));
	} else if (this->IsChar(value)) {
		this->objcode_->Output(MipsInstr::li, reg, (int)value[1]);
	} else if (this->IsTemporary(value)) {
		this->LoadTemporary(value, reg);
	} else if (IsConstVariable(value)) {
		this->objcode_->Output(MipsInstr::li, reg, this->GetConstVariable(value));
	} else {
		this->LoadVariable(value, reg);
	}
}

void MipsGenerator::SaveTemporary(string name, Reg reg) {
	if (this->temporary_offset_map_.find(name) == this->temporary_offset_map_.end()) {
		this->temp_offset_ += 4;
		this->temporary_offset_map_.insert(pair<string, int>(name, this->temp_offset_));
	}

	int offset = this->temporary_offset_map_.at(name);
	this->objcode_->Output(MipsInstr::sw, reg, FUNC_POINT, offset);
}

void MipsGenerator::LoadTemporary(string name, Reg reg) {
	if (this->temporary_offset_map_.find(name) == this->temporary_offset_map_.end()) {
		this->temp_offset_ += 4;
		this->temporary_offset_map_.insert(pair<string, int>(name, this->temp_offset_));
	}

	int offset = this->temporary_offset_map_.at(name);
	this->objcode_->Output(MipsInstr::lw, reg, FUNC_POINT, offset);
}

bool MipsGenerator::IsConstVariable(string name) {
	Symbol* symbol = check_table_->FindSymbol(name);
	if (symbol != NULL && symbol->kind() == KindSymbol::CONST) {
		return true;
	} else {
		return false;
	}
}

int MipsGenerator::GetConstVariable(string name) {
	Symbol* symbol = check_table_->FindSymbol(name);

	if (symbol->type() == TypeSymbol::INT) {
		return stoi(symbol->const_value());
	} else {
		return symbol->const_value()[1];
	}
}

void MipsGenerator::GenerateScanf(string variable_name, int type) {
	this->objcode_->Output(MipsInstr::li, Reg::v0, type);
	this->objcode_->Output(MipsInstr::syscall);

	this->SaveVariable(variable_name, Reg::v0);
}

void MipsGenerator::GeneratePrintfIntChar(Midcode* midcode, int type) {
	string value = midcode->label();
	if (this->IsInteger(value)) {
		this->objcode_->Output(MipsInstr::li, Reg::a0, midcode->GetInteger());
	} else if (this->IsChar(value)) {
		this->objcode_->Output(MipsInstr::li, Reg::v0, midcode->GetChar());
	} else if (this->IsTemporary(value)) {
		this->LoadTemporary(value, Reg::a0);
	} else if (this->IsConstVariable(value)) {
		this->objcode_->Output(MipsInstr::li, Reg::a0, this->GetConstVariable(value));
	} else {
		this->LoadVariable(value, Reg::a0);
	}

	this->objcode_->Output(MipsInstr::li, Reg::v0, type);
	this->objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GeneratePrintfString(string str) {
	this->objcode_->Output(MipsInstr::la, Reg::a0, str);
	this->objcode_->Output(MipsInstr::li, Reg::v0, 4);
	this->objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GeneratePrintfEnd() {
	this->objcode_->Output(MipsInstr::li, Reg::a0, 10);
	this->objcode_->Output(MipsInstr::li, Reg::v0, 11);
	this->objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GenerateLabel(string label) {
	this->objcode_->Output(MipsInstr::label, label);
}

void MipsGenerator::GenerateJump(string label) {
	this->objcode_->Output(MipsInstr::j, label);
}

void MipsGenerator::GenerateAssign(string result, string value) {

	if (this->IsInteger(value)) {
		this->objcode_->Output(MipsInstr::li, RD, stoi(value));
	} else if (this->IsChar(value)) {
		this->objcode_->Output(MipsInstr::li, RD, (int)value[1]);
	} else if (this->IsTemporary(value)) {
		this->LoadTemporary(value, RS);
		this->objcode_->Output(MipsInstr::move, RD, RS);
	} else if (this->IsConstVariable(value)) {
		this->objcode_->Output(MipsInstr::li, RD, this->GetConstVariable(value));
	} else {
		this->LoadVariable(value, RS);
		this->objcode_->Output(MipsInstr::move, RD, RS);
	}

	if (this->IsTemporary(result)) {
		this->SaveTemporary(result, RD);
	} else {
		this->SaveVariable(result, RD);
	}
}

void MipsGenerator::GenerateAssignReturn(string temp) {
	this->objcode_->Output(MipsInstr::move, RD, Reg::v0);
	this->SaveTemporary(temp, RD);
}

void MipsGenerator::SetArrayIndex(string index, Reg base, int& offset, bool& is_use_temp) {
	if (this->IsInteger(index)) {
		offset += 4 * stoi(index);
	} else if (this->IsChar(index)) {
		offset += 4 * ((int)index[1]);
	} else if (this->IsTemporary(index)) {
		this->LoadTemporary(index, TEMP);
		this->objcode_->Output(MipsInstr::sll, TEMP, TEMP, 2);
		this->objcode_->Output(MipsInstr::addi, TEMP, TEMP, offset);
		this->objcode_->Output(MipsInstr::add, TEMP, base, TEMP);
		is_use_temp = true;
	} else if (this->IsConstVariable(index)) {
		this->objcode_->Output(MipsInstr::li, TEMP, this->GetConstVariable(index));
		this->objcode_->Output(MipsInstr::sll, TEMP, TEMP, 2);
		this->objcode_->Output(MipsInstr::addi, TEMP, TEMP, offset);
		this->objcode_->Output(MipsInstr::add, TEMP, base, TEMP);
		is_use_temp = true;
	} else {
		this->LoadVariable(index, TEMP);
		this->objcode_->Output(MipsInstr::sll, TEMP, TEMP, 2);
		this->objcode_->Output(MipsInstr::addi, TEMP, TEMP, offset);
		this->objcode_->Output(MipsInstr::add, TEMP, base, TEMP);
		is_use_temp = true;
	}
}

void MipsGenerator::GenerateAssignArray(string array, string index, string value) {

	bool is_use_temp = false;
	int offset = this->check_table_->FindSymbol(array)->offset();
	int level = this->check_table_->GetSymbolLevel(array);
	Reg base = level == 0 ? GLOBAL_POINT : FUNC_POINT;

	this->SetArrayIndex(index, base, offset, is_use_temp);
	this->LoadValue(value, RT);

	if (is_use_temp) {
		this->objcode_->Output(MipsInstr::sw, RT, TEMP, 0);
	} else {
		this->objcode_->Output(MipsInstr::sw, RT, base, offset);
	}
}

void MipsGenerator::GenerateLoadArray(string temp, string array, string index) {

	bool is_use_temp = false;
	int offset = this->check_table_->FindSymbol(array)->offset();
	int level = this->check_table_->GetSymbolLevel(array);
	Reg base = level == 0 ? GLOBAL_POINT : FUNC_POINT;

	this->SetArrayIndex(index, base, offset, is_use_temp);

	if (is_use_temp) {
		this->objcode_->Output(MipsInstr::lw, RT, TEMP, 0);
	} else {
		this->objcode_->Output(MipsInstr::lw, RT, base, offset);
	}

	this->SaveTemporary(temp, RT);
}

void MipsGenerator::SetOperand(string value, Reg reg, bool& is_immediate, int& immediate) {
	if (this->IsInteger(value)) {
		is_immediate = true;
		immediate = stoi(value);
	} else if (this->IsChar(value)) {
		is_immediate = true;
		immediate = (int)value[1];
	} else if (IsConstVariable(value)) {
		is_immediate = true;
		immediate = this->GetConstVariable(value);
	} else {
		this->LoadVariable(value, reg);
	}
}

void MipsGenerator::DealNumberOpNumber(string value_1, string value_2,
	int& immediate_1, bool& is_immediate_1,
	int& immediate_2, bool& is_immediate_2) {

	this->SetOperand(value_1, RS, is_immediate_1, immediate_1);
	this->SetOperand(value_2, RT, is_immediate_2, immediate_2);
}

void MipsGenerator::DealNumberOpReg(string value_1, string temp_2,
	int& immediate_1, bool& is_immediate_1) {

	this->SetOperand(value_1, RS, is_immediate_1, immediate_1);
	this->LoadTemporary(temp_2, RT);
}

void MipsGenerator::DealRegOpNumber(string temp_1, string value_2,
	int& immediate_2, bool& is_immediate_2) {

	this->LoadTemporary(temp_1, RS);
	this->SetOperand(value_2, RT, is_immediate_2, immediate_2);
}

void MipsGenerator::DealRegOpReg(string temp_1, string temp_2) {
	this->LoadTemporary(temp_1, RS);
	this->LoadTemporary(temp_2, RT);
}

void MipsGenerator::GenerateOperate(Midcode* midcode, string result,
	MidcodeInstr op, OperaMember member_type) {

	bool is_immediate_1 = false;
	int immediate_1 = 0;
	bool is_immediate_2 = false;
	int immediate_2 = 0;

	switch (member_type) {
	case OperaMember::REG_OP_REG:
		this->DealRegOpReg(midcode->GetTempReg1(), midcode->GetTempReg2());
		break;
	case OperaMember::REG_OP_NUMBER:
		this->DealRegOpNumber(midcode->GetTempReg1(), midcode->reg2(),
			immediate_2, is_immediate_2);
		break;
	case OperaMember::NUMBER_OP_REG:
		this->DealNumberOpReg(midcode->reg2(), midcode->GetTempReg1(),
			immediate_1, is_immediate_1);
		break;
	case OperaMember::NUMBER_OP_NUMBER:
		this->DealNumberOpNumber(midcode->reg1(), midcode->reg2(),
			immediate_1, is_immediate_1,
			immediate_2, is_immediate_2);
		break;
	default:
		assert(0);
		break;
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
			this->objcode_->Output(MipsInstr::add, RD, RS, RT);
			break;
		case 1:
			this->objcode_->Output(MipsInstr::addi, RD, RT, immediate_1);
			break;
		case 2:
			this->objcode_->Output(MipsInstr::addi, RD, RS, immediate_2);
			break;
		case 3:
			this->objcode_->Output(MipsInstr::li, RD, immediate_1 + immediate_2);
			break;
		default:
			assert(0);
			break;
		}
		break;
	case MidcodeInstr::SUB:
		switch (flag) {
		case 0:
			this->objcode_->Output(MipsInstr::sub, RD, RS, RT);
			break;
		case 1:
			this->objcode_->Output(MipsInstr::li, RS, immediate_1);
			this->objcode_->Output(MipsInstr::sub, RD, RS, RT);
			break;
		case 2:
			this->objcode_->Output(MipsInstr::subi, RD, RS, immediate_2);
			break;
		case 3:
			this->objcode_->Output(MipsInstr::li, RD, immediate_1 - immediate_2);
			break;
		default:
			assert(0);
			break;
		}
		break;
	case MidcodeInstr::MUL:
		switch (flag) {
		case 0:
			this->objcode_->Output(MipsInstr::mul, RD, RS, RT);
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
			break;
		}
		break;
	case MidcodeInstr::DIV:
		switch (flag) {
		case 0:
			this->objcode_->Output(MipsInstr::div, RD, RS, RT);
			break;
		case 1:
			objcode_->Output(MipsInstr::li, RS, immediate_1);
			this->objcode_->Output(MipsInstr::div, RD, RS, RT);
			break;
		case 2:
			objcode_->Output(MipsInstr::div, RD, RS, immediate_2);
			break;
		case 3:
			objcode_->Output(MipsInstr::li, RD, immediate_1 / immediate_2);
			break;
		default:
			assert(0);
			break;
		}
		break;
	default:
		assert(0);
		break;
	}

	if (this->IsTemporary(result)) {
		this->SaveTemporary(result, RD);
	} else {
		this->SaveVariable(result, RD);
	}
}

void MipsGenerator::GenerateOperate(list<Midcode*>::iterator& iter, Midcode* midcode) {
	Midcode* next_midcode = *(++iter);

	if (next_midcode->instr() == MidcodeInstr::ASSIGN
		&& next_midcode->reg1() == midcode->GetTempRegResult()) {

		this->GenerateOperate(midcode, next_midcode->reg_result(),
			midcode->instr(), midcode->opera_member());
	} else {
		iter--;
		this->GenerateOperate(midcode, midcode->GetTempRegResult(),
			midcode->instr(), midcode->opera_member());
	}
}

void MipsGenerator::GenerateStep(list<Midcode*>::iterator& iter, Midcode*& midcode) {
	midcode = *(++iter);
	this->GenerateOperate(midcode, midcode->reg_result(),
		midcode->instr(), midcode->opera_member());
}

void MipsGenerator::GenerateNeg(string temp_result, string value) {

	bool is_immediate = false;
	int immediate = 0;
	this->SetOperand(value, RS, is_immediate, immediate);

	if (is_immediate) {
		objcode_->Output(MipsInstr::li, RD, -immediate);
	} else {
		objcode_->Output(MipsInstr::sub, RD, Reg::zero, RS);
	}

	if (this->IsTemporary(temp_result)) {
		this->SaveTemporary(temp_result, RD);
	} else {
		this->SaveVariable(temp_result, RD);
	}
}

void MipsGenerator::SetJudgeReg(string value, Reg reg) {
	if (this->IsInteger(value)) {
		this->objcode_->Output(MipsInstr::li, reg, stoi(value));
	} else if (this->IsChar(value)) {
		this->objcode_->Output(MipsInstr::li, reg, (int)value[1]);
	} else if (this->IsConstVariable(value)) {
		this->objcode_->Output(MipsInstr::li, reg, this->GetConstVariable(value));
	} else if (this->IsTemporary(value)) {
		this->LoadTemporary(value, reg);
	} else {
		this->LoadVariable(value, reg);
	}
}

void MipsGenerator::GenerateJudge(Midcode* midcode, MidcodeInstr judge) {
	MipsInstr mips_instr = MipsInstr::bgt;
	bool is_two_judge = true;

	switch (judge) {
	case MidcodeInstr::BGT:
		mips_instr = MipsInstr::bgt;
		this->SetJudgeReg(midcode->reg1(), RS);
		this->SetJudgeReg(midcode->reg2(), RT);
		break;
	case MidcodeInstr::BGE:
		mips_instr = MipsInstr::bge;
		this->SetJudgeReg(midcode->reg1(), RS);
		this->SetJudgeReg(midcode->reg2(), RT);
		break;
	case MidcodeInstr::BLT:
		mips_instr = MipsInstr::blt;
		this->SetJudgeReg(midcode->reg1(), RS);
		this->SetJudgeReg(midcode->reg2(), RT);
		break;
	case MidcodeInstr::BLE:
		mips_instr = MipsInstr::ble;
		this->SetJudgeReg(midcode->reg1(), RS);
		this->SetJudgeReg(midcode->reg2(), RT);
		break;
	case MidcodeInstr::BEQ:
		mips_instr = MipsInstr::beq;
		this->SetJudgeReg(midcode->reg1(), RS);
		this->SetJudgeReg(midcode->reg2(), RT);
		break;
	case MidcodeInstr::BNE:
		mips_instr = MipsInstr::bne;
		this->SetJudgeReg(midcode->reg1(), RS);
		this->SetJudgeReg(midcode->reg2(), RT);
		break;
	case MidcodeInstr::BEZ:
		mips_instr = MipsInstr::beq;
		this->SetJudgeReg(midcode->reg1(), RS);
		is_two_judge = false;
		break;
	case MidcodeInstr::BNZ:
		mips_instr = MipsInstr::bne;
		this->SetJudgeReg(midcode->reg1(), RS);
		is_two_judge = false;
		break;
	default:
		assert(0);
		break;
	}

	if (is_two_judge) {
		this->objcode_->Output(mips_instr, RS, RT, midcode->GetLabel());
	} else {
		this->objcode_->Output(mips_instr, RS, Reg::zero, midcode->GetLabel());
	}

}

void MipsGenerator::GeneratePush(
	string value, int count) {

	this->LoadValue(value, RT);
	this->objcode_->Output(MipsInstr::sw, RT, PARA_POINT, 0);
	this->objcode_->Output(MipsInstr::addi, PARA_POINT, PARA_POINT, 4);
}

void MipsGenerator::GenerateCall(string prev_name, string call_name) {

	this->objcode_->Output(MipsInstr::addi, FUNC_POINT, FUNC_POINT, 500);

	this->objcode_->Output(MipsInstr::jal, call_name);

	this->objcode_->Output(MipsInstr::subi, FUNC_POINT, FUNC_POINT, 500);

	int length = this->check_table_->FindSymbol(call_name, 0)->GetParameterCount();
	this->objcode_->Output(MipsInstr::subi, PARA_POINT, PARA_POINT, 4 * length);

	this->objcode_->Output(MipsInstr::subi, RA_POINT, RA_POINT, 4);
	this->objcode_->Output(MipsInstr::lw, Reg::ra, RA_POINT, 0);
}

void MipsGenerator::GenerateSave() {
	this->objcode_->Output(MipsInstr::sw, Reg::ra, RA_POINT, 0);
	this->objcode_->Output(MipsInstr::addi, RA_POINT, RA_POINT, 4);
}

void MipsGenerator::GenerateFunctionEnd(list<Midcode*>::iterator& iter) {
	iter++;
	return;
}

void MipsGenerator::GenerateReturn(Midcode* midcode, bool is_return_value) {

	if (is_return_value) {
		if (this->IsInteger(midcode->label())) {
			this->objcode_->Output(MipsInstr::li, Reg::v0, midcode->GetInteger());
		} else if (this->IsChar(midcode->label())) {
			this->objcode_->Output(MipsInstr::li, Reg::v0, midcode->GetChar());
		} else if (this->IsTemporary(midcode->label())) {
			this->LoadTemporary(midcode->label(), RS);
			this->objcode_->Output(MipsInstr::move, Reg::v0, RS);
		} else if (IsConstVariable(midcode->label())) {
			this->objcode_->Output(MipsInstr::li, Reg::v0,
				this->GetConstVariable(midcode->label()));
		} else {
			this->LoadVariable(midcode->label(), RS);
			this->objcode_->Output(MipsInstr::move, Reg::v0, RS);
		}
	}

	this->objcode_->Output(MipsInstr::jr, Reg::ra);
}

void MipsGenerator::GenerateParameter(string function_name, string value, int count) {

	int parameter_count = this->check_table_->FindSymbol(
		function_name, 0)->GetParameterCount();
	int offset = 4 * (count - parameter_count);
	this->check_table_->FindSymbol(value)->set_offset(offset);
}

void MipsGenerator::GenerateBody(string function_name, list<Midcode*>::iterator& iter) {
	Midcode* midcode;
	int parameter_count = 0;

	while (iter != midcode_list_.end()) {
		midcode = *iter;

		switch (midcode->instr()) {
		case MidcodeInstr::SCANF_INT:
			this->GenerateScanf(midcode->label(), 5);
			break;
		case MidcodeInstr::SCANF_CHAR:
			this->GenerateScanf(midcode->label(), 12);
			break;
		case MidcodeInstr::PRINTF_INT:
			this->GeneratePrintfIntChar(midcode, 1);
			break;
		case MidcodeInstr::PRINTF_CHAR:
			this->GeneratePrintfIntChar(midcode, 11);
			break;
		case MidcodeInstr::PRINTF_STRING:
			this->GeneratePrintfString(midcode->GetString());
			break;
		case MidcodeInstr::PRINTF_END:
			this->GeneratePrintfEnd();
			break;
		case MidcodeInstr::LABEL:
			this->GenerateLabel(midcode->GetLabel());
			break;
		case MidcodeInstr::JUMP:
			this->GenerateJump(midcode->GetJumpLabel());
			break;
		case MidcodeInstr::ASSIGN:
			this->GenerateAssign(midcode->reg_result(), midcode->reg1());
			break;
		case MidcodeInstr::ASSIGN_RETURN:
			this->GenerateAssignReturn(midcode->GetTempReg());
			break;
		case MidcodeInstr::ASSIGN_ARRAY:
			this->GenerateAssignArray(midcode->reg_result(),
				midcode->reg1(), midcode->reg2());
			break;
		case MidcodeInstr::LOAD_ARRAY:
			this->GenerateLoadArray(midcode->GetTempReg()
				, midcode->reg1(), midcode->reg2());
			break;
		case MidcodeInstr::ADD:
			this->GenerateOperate(iter, midcode);
			break;
		case MidcodeInstr::SUB:
			this->GenerateOperate(iter, midcode);
			break;
		case MidcodeInstr::MUL:
			this->GenerateOperate(iter, midcode);
			break;
		case MidcodeInstr::DIV:
			this->GenerateOperate(iter, midcode);
			break;
		case MidcodeInstr::STEP:
			this->GenerateStep(iter, midcode);
			break;
		case MidcodeInstr::NEG:
			this->GenerateNeg("#" + to_string(midcode->count()), midcode->reg1());
			break;
		case MidcodeInstr::BGT:
			this->GenerateJudge(midcode, MidcodeInstr::BGT);
			break;
		case MidcodeInstr::BGE:
			this->GenerateJudge(midcode, MidcodeInstr::BGE);
			break;
		case MidcodeInstr::BLT:
			this->GenerateJudge(midcode, MidcodeInstr::BLT);
			break;
		case MidcodeInstr::BLE:
			this->GenerateJudge(midcode, MidcodeInstr::BLE);
			break;
		case MidcodeInstr::BEQ:
			this->GenerateJudge(midcode, MidcodeInstr::BEQ);
			break;
		case MidcodeInstr::BNE:
			this->GenerateJudge(midcode, MidcodeInstr::BNE);
			break;
		case MidcodeInstr::BEZ:
			this->GenerateJudge(midcode, MidcodeInstr::BEZ);
			break;
		case MidcodeInstr::BNZ:
			this->GenerateJudge(midcode, MidcodeInstr::BNZ);
			break;
		case MidcodeInstr::PUSH:
			this->GeneratePush(midcode->reg2(), midcode->count());
			break;
		case MidcodeInstr::CALL:
			this->GenerateCall(function_name, midcode->label());
			break;
		case MidcodeInstr::SAVE:
			this->GenerateSave();
			break;
		case MidcodeInstr::FUNCTION_END:
			return this->GenerateFunctionEnd(iter);
			break;
		case MidcodeInstr::RETURN:
			this->GenerateReturn(midcode, true);
			break;
		case MidcodeInstr::RETURN_NON:
			this->GenerateReturn(midcode, false);
			break;
		case MidcodeInstr::PARA_INT:
			this->GenerateParameter(function_name, midcode->label(), parameter_count++);
			break;
		case MidcodeInstr::PARA_CHAR:
			this->GenerateParameter(function_name, midcode->label(), parameter_count++);
			break;
		case MidcodeInstr::VAR_INT:
			break;
		case MidcodeInstr::VAR_CHAR:
			break;
		case MidcodeInstr::LOOP:
			break;
		default:
			assert(0);
			break;
		}

		iter++;
	}
}

void MipsGenerator::GenerateFunction(string function_name,
	list<Midcode*>::iterator& iter) {

	this->LoadTable(1, function_name);
	this->InitVariable(function_name);
	this->objcode_->Output(MipsInstr::label, function_name);
	iter++;
	this->GenerateBody(function_name, iter);
}

void MipsGenerator::Generate() {
	list<Midcode*>::iterator iter = midcode_list_.begin();
	Midcode* midcode;

	while (iter != midcode_list_.end()) {
		midcode = *iter;

		switch (midcode->instr()) {
		case MidcodeInstr::INT_FUNC_DECLARE:
			this->GenerateFunction(midcode->label(), iter);
			break;
		case MidcodeInstr::CHAR_FUNC_DECLARE:
			this->GenerateFunction(midcode->label(), iter);
			break;
		case MidcodeInstr::VOID_FUNC_DECLARE:
			this->GenerateFunction(midcode->label(), iter);
			break;
		case MidcodeInstr::VAR_INT:
			iter++;
			break;
		case MidcodeInstr::VAR_CHAR:
			iter++;
			break;
		default:
			assert(0);
			break;
		}
	}
}

void MipsGenerator::GenerateMips() {
	this->InitData();
	this->InitText();
	this->Generate();
}

void MipsGenerator::FileClose() {
	this->objcode_->FileClose();
}