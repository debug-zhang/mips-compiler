#include "mips_generator.h"

MipsGenerator::MipsGenerator(string outputFileName,
	StringTable* stringTable, CheckTable* check_table,
	map<string, SymbolTable*> symbolTableMap, list<Midcode*> midcode_list) {
	this->objcode_ = new Objcode(outputFileName);
	this->check_table_ = check_table;
	this->string_table_ = stringTable;
	this->symbol_table_map_ = symbolTableMap;
	this->midcode_list_ = midcode_list;
	this->new_reg = TEMP_REG_START;
	this->dm_offset = 0;
	this->parameter_count_old = REG_START;
}

void MipsGenerator::InsertTempUseRegMap(string name) {
	this->temp_use_reg_map_.insert(pair<string, int>(name, 1));
}

void MipsGenerator::DeleteTempUseRegMap(string name) {
	this->temp_use_reg_map_.erase(name);
}

void MipsGenerator::InitVariable(int level) {
	int count = 0;
	int offset = 0;

	map<string, Symbol*> symbolMap = check_table_->GetSymbolMap(level);
	map<string, Symbol*>::iterator iter = symbolMap.begin();

	Symbol* symbol;
	while (iter != symbolMap.end()) {
		symbol = iter->second;
		if (symbol->kind() == KindSymbol::PARAMETER) {
			offset -= 4;
			symbol->set_sp_offer(offset);
		} else if (symbol->kind() == KindSymbol::VARIABLE) {
			offset -= 4;
			symbol->set_sp_offer(offset);
			count++;
		} else if (symbol->kind() == KindSymbol::ARRAY) {
			offset -= 4 * symbol->array_length();
			symbol->set_sp_offer(offset);
			count += symbol->array_length();
		}
		iter++;
	}

	if (count != 0) {
		objcode_->Output(MipsInstr::subi, Reg::sp, Reg::sp, count * 4);
	}
}

void MipsGenerator::InitVariable(string function_name) {
	map<string, Symbol*> table = symbol_table_map_.at(function_name)->symbol_map();
	map<string, Symbol*>::iterator iter = table.begin();
	while (iter != table.end()) {
		if (iter->second->kind() == KindSymbol::ARRAY) {
			objcode_->Output(MipsInstr::data_identifier,
				iter->second->name() + "_" + function_name,
				4 * (iter->second->array_length() + 5));
		} else if (iter->second->kind() == KindSymbol::VARIABLE) {
			objcode_->Output(MipsInstr::data_identifier,
				iter->second->name() + "_" + function_name,
				4);
		} else if (iter->second->kind() == KindSymbol::PARAMETER) {
			objcode_->Output(MipsInstr::data_identifier,
				iter->second->name() + "_" + function_name,
				4);
		}
		iter++;
	}
}

void MipsGenerator::InitDataSeg(string function_name) {
	objcode_->Output(MipsInstr::data);
	objcode_->Output(MipsInstr::data_identifier, function_name + "_space",
		(check_table_->GetFunctionVariableNumber(function_name) + 100) * 4);

	if (function_name == "global") {
		for (int i = 0; i < string_table_->GetStringCount(); i++) {
			string data_string = "str_" + to_string(i) + ": .asciiz \""
				+ string_table_->GetString(i) + "\"";
			objcode_->Output(MipsInstr::data_string, data_string);
		}
	}

	InitVariable(function_name);

	objcode_->Output(MipsInstr::data_align, 4);
	objcode_->Output();
}

void MipsGenerator::InitDataSeg() {
	map<string, SymbolTable*>::iterator iter = symbol_table_map_.begin();
	while (iter != symbol_table_map_.end()) {
		InitDataSeg(iter->first);
		iter++;
	}
}

void MipsGenerator::InitStack() {
	objcode_->Output(MipsInstr::la, Reg::v1, "global_space");
	objcode_->Output(MipsInstr::move, Reg::gp, Reg::sp);
	objcode_->Output(MipsInstr::move, Reg::fp, Reg::sp);
	objcode_->Output();
}

void MipsGenerator::PrintNop() {
	objcode_->Output(MipsInstr::nop);
}

void MipsGenerator::PrintText() {
	objcode_->Output(MipsInstr::text);
	objcode_->Output();
}

void MipsGenerator::PrintMain() {
	objcode_->Output(MipsInstr::jal, "main");
}

void MipsGenerator::PrintEnd() {
	objcode_->Output(MipsInstr::li, Reg::v0, 10);
	objcode_->Output(MipsInstr::syscall);
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

bool MipsGenerator::IsTempReg(string str) {
	return str[0] == '#' && isdigit(str[1]);
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

bool MipsGenerator::IsThisInstr(string strs, string instr) {
	return strs == instr;
}

Reg MipsGenerator::NumberToReg(int reg_number) {
	return reg::NumberToReg(reg_number);
}

int MipsGenerator::RegToNumber(Reg reg) {
	return reg::RegToNumber(reg);
}

void MipsGenerator::SaveAllReg() {
	objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, -REG_STACK_LENGTH * 4);

	int offset = 0;
	for (int i = TEMP_REG_START; i <= REG_END; i++) {
		objcode_->Output(MipsInstr::sw, this->NumberToReg(i), Reg::sp, offset);
		offset += 4;
	}
	objcode_->Output(MipsInstr::sw, Reg::fp, Reg::sp, offset);
	objcode_->Output(MipsInstr::sw, Reg::ra, Reg::sp, offset + 4);
}

void MipsGenerator::SaveAllReg(string function_name) {
	int reg_stack_length = 27;
	objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, -reg_stack_length * 4);

	int offset = 0;
	for (int i = 5; i <= 31; i++) {
		objcode_->Output(MipsInstr::sw, this->NumberToReg(i), Reg::sp, offset);
		offset += 4;
	}
}

void MipsGenerator::ResetAllReg() {
	int offset = 0;
	for (int i = TEMP_REG_START; i <= REG_END; i++) {
		objcode_->Output(MipsInstr::lw, this->NumberToReg(i), Reg::sp, offset);
		offset += 4;
	}
	objcode_->Output(MipsInstr::lw, Reg::fp, Reg::sp, offset);
	objcode_->Output(MipsInstr::lw, Reg::ra, Reg::sp, offset + 4);

	objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, REG_STACK_LENGTH * 4);
}

void MipsGenerator::ResetAllReg(string function_name) {
	int reg_stack_length = 27;

	int offset = 0;
	for (int i = 5; i <= 31; i++) {
		objcode_->Output(MipsInstr::lw, this->NumberToReg(i), Reg::sp, offset);
		offset += 4;
	}

	objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, reg_stack_length * 4);
}

int MipsGenerator::IsTempValue(string name) {
	if (name[0] == '#') {
		int offset = 0;

		string::iterator iter = name.begin();
		while (iter != name.end()) {
			offset = offset * 10 + *iter - '0';
			iter++;
		}

		return offset;
	} else {
		return 0;
	}
}

void MipsGenerator::LoadTable(int level, string table_name) {
	this->check_table_->SetTable(level, symbol_table_map_.at(table_name));
}

void MipsGenerator::SetStackRegUse(int number) {
	reg_use_stack_[number] = 1;
}

void MipsGenerator::SetStackRegUnuse(int number) {
	reg_use_stack_[number] = 0;
}

void MipsGenerator::PopStack() {
	int count = 0;
	Symbol* symbol;
	map<string, Symbol*> symbol_map;
	map<string, Symbol*>::iterator iter;

	symbol_map = check_table_->GetSymbolMap(1);
	iter = symbol_map.begin();
	while (iter != symbol_map.end()) {
		symbol = iter->second;
		if (symbol->is_use() && symbol->reg_number() < REG_START) {
			this->SetStackRegUnuse(symbol->reg_number());
			symbol->set_reg_number(0);
		}
		switch (symbol->kind()) {
		case KindSymbol::VARIABLE:
		case KindSymbol::PARAMETER:
			count++;
			break;
		case KindSymbol::ARRAY:
			count += symbol->array_length();
			break;
		default:
			break;
		}
		iter++;
	}
}

void MipsGenerator::PopStackLength() {
	int count = 0;
	Symbol* symbol;
	map<string, Symbol*> symbol_map;
	map<string, Symbol*>::iterator iter;

	symbol_map = check_table_->GetSymbolMap(1);
	iter = symbol_map.begin();
	while (iter != symbol_map.end()) {
		symbol = iter->second;
		switch (symbol->kind()) {
		case KindSymbol::VARIABLE:
		case KindSymbol::PARAMETER:
			count++;
			break;
		case KindSymbol::ARRAY:
			count += symbol->array_length();
			break;
		default:
			break;
		}
		iter++;
	}

	if (count != 0) {
		objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, count * 4);
	}
}

void MipsGenerator::PopSymbolMapVar(int level) {
	Symbol* symbol;
	map<string, Symbol*> symbol_map = check_table_->GetSymbolMap(level);
	map<string, Symbol*>::iterator iter = symbol_map.begin();
	Reg point = level == 1 ? Reg::fp : Reg::gp;

	while (iter != symbol_map.end()) {
		symbol = iter->second;
		if (symbol->reg_number() != 0 && !symbol->is_use()) {
			objcode_->Output(MipsInstr::sw, this->NumberToReg(symbol->reg_number()),
				point, symbol->sp_offer());
			reg_use_stack_[symbol->reg_number()] = 0;
			symbol->set_reg_number(0);
		}
		iter++;
	}
}

void MipsGenerator::PopAllVariable() {
	this->PopSymbolMapVar(0);
	this->PopSymbolMapVar(1);
}

int MipsGenerator::LoadVariableToNumber(string name) {
	Symbol* symbol = check_table_->FindSymbol(name);
	int level = check_table_->GetSymbolLevel(name);

	if (symbol->reg_number() == 0) {
		int reg_number = this->GetUnuseRegNumber();
		Reg t1 = level == 1 ? Reg::fp : Reg::gp;
		objcode_->Output(MipsInstr::lw, this->NumberToReg(reg_number), t1,
			symbol->sp_offer());
		symbol->set_reg_number(reg_number);
	}

	return symbol->reg_number();
}

Reg MipsGenerator::LoadVariableToReg(string name) {
	return this->NumberToReg(this->LoadVariableToNumber(name));
}

int MipsGenerator::LoadTempRegToNumber(string name) {
	if (temp_reg_map_.at(name) > 0) {
		return temp_reg_map_.at(name);
	} else {
		int unuse_reg = this->GetUnuseRegNumber();
		objcode_->Output(MipsInstr::lw, this->NumberToReg(unuse_reg),
			Reg::v1, -temp_reg_map_.at(name));
		temp_reg_map_.at(name) = unuse_reg;
		return unuse_reg;
	}
}

Reg MipsGenerator::LoadTempRegToReg(string name) {
	return this->NumberToReg(this->LoadTempRegToNumber(name));
}

void MipsGenerator::PoptempReg(string name) {
	if (temp_reg_map_.find(name) == temp_reg_map_.end()) {
		return;
	}
	int reg = temp_reg_map_.at(name);
	if (reg > 0) {
		reg_use_stack_[reg] = 0;
	}
	temp_reg_map_.erase(name);
	temp_use_reg_map_.erase(name);
}

void MipsGenerator::PoptempReg(Reg reg) {
	reg_use_stack_[this->RegToNumber(reg)] = 0;
}

void MipsGenerator::PoptempReg(int number) {
	reg_use_stack_[number] = 0;
}

int MipsGenerator::GetUnuseRegInTable(int& unuse_reg, bool& retflag, int level) {
	Symbol* symbol;
	map<string, Symbol*> symbol_map = check_table_->GetSymbolMap(level);
	map<string, Symbol*>::iterator iter = symbol_map.begin();
	Reg point = level == 1 ? Reg::fp : Reg::gp;

	retflag = true;
	while (iter != symbol_map.end()) {
		symbol = iter->second;
		if (symbol->reg_number() != 0 && !symbol->is_use()) {
			unuse_reg = symbol->reg_number();
			objcode_->Output(MipsInstr::sw, this->NumberToReg(unuse_reg),
				point, symbol->sp_offer());
			symbol->set_reg_number(0);
			return unuse_reg;
		}
		iter++;
	}
	retflag = false;
	return {};
}

int MipsGenerator::GetUnuseRegNumber() {
	int unuse_reg;
	for (int i = TEMP_REG_START; i <= TEMP_REG_END; i++) {
		if (reg_use_stack_[new_reg] == 0) {
			reg_use_stack_[new_reg] = 1;
			unuse_reg = new_reg;
			if (new_reg == TEMP_REG_END) {
				new_reg = TEMP_REG_START;
			} else {
				new_reg++;
			}
			return unuse_reg;
		} else {
			if (new_reg == TEMP_REG_END) {
				new_reg = TEMP_REG_START;
			} else {
				new_reg++;
			}
		}
	}

	/*
	bool retflag;
	int retval;

	retval = this->GetUnuseRegInTable(unuse_reg, retflag, 0);
	if (retflag) return retval;
	*/

	map<string, int>::iterator iter = temp_reg_map_.begin();
	while (iter != temp_reg_map_.end()) {
		if (iter->second > 0
			&& temp_use_reg_map_.find(iter->first) == temp_use_reg_map_.end()) {
			unuse_reg = iter->second;
			dm_offset += 4;
			iter->second = -dm_offset;
			objcode_->Output(MipsInstr::sw, this->NumberToReg(unuse_reg),
				Reg::v1, dm_offset);
			return unuse_reg;
		}
		iter++;
	}

	return 0;
}

Reg MipsGenerator::GetUnuseReg() {
	int unuse_reg;
	for (int i = TEMP_REG_START; i <= TEMP_REG_END; i++) {
		if (reg_use_stack_[new_reg] == 0) {
			reg_use_stack_[new_reg] = 1;
			unuse_reg = new_reg;
			if (new_reg == TEMP_REG_END) {
				new_reg = TEMP_REG_START;
			} else {
				new_reg++;
			}
			return this->NumberToReg(unuse_reg);
		} else {
			if (new_reg == TEMP_REG_END) {
				new_reg = TEMP_REG_START;
			} else {
				new_reg++;
			}
		}
	}

	/*
	bool retflag;
	int retval;

	retval = this->GetUnuseRegInTable(unuse_reg, retflag, 0);
	if (retflag) return retval;
	*/

	map<string, int>::iterator iter = temp_reg_map_.begin();
	while (iter != temp_reg_map_.end()) {
		if (iter->second > 0
			&& temp_use_reg_map_.find(iter->first) == temp_use_reg_map_.end()) {
			unuse_reg = iter->second;
			dm_offset += 4;
			iter->second = -dm_offset;
			objcode_->Output(MipsInstr::sw, this->NumberToReg(unuse_reg),
				Reg::v1, dm_offset);
			return this->NumberToReg(unuse_reg);
		}
		iter++;
	}

	return Reg::zero;
}

void MipsGenerator::SetSymbolUse(string name, bool isUse) {
	check_table_->FindSymbol(name)->set_is_use(isUse);
}

void MipsGenerator::GenerateJump(Midcode* midcode) {
	this->objcode_->Output(MipsInstr::j, midcode->GetJumpLabel());
}

Reg MipsGenerator::SetJudgeReg(Midcode* midcode, string name) {
	Reg reg;
	if (this->IsInteger(name)) {
		reg = this->GetUnuseReg();
		this->objcode_->Output(MipsInstr::li, reg, stoi(name));
	} else if (this->IsChar(name)) {
		reg = this->GetUnuseReg();
		this->objcode_->Output(MipsInstr::li, reg, (int)name[1]);
	} else if (this->IsConstVariable(name)) {
		reg = this->GetUnuseReg();
		this->objcode_->Output(MipsInstr::li, reg, this->GetConstVariable(name));
	} else if (this->IsTempReg(name)) {
		reg = this->LoadTempRegToReg(name);
		this->InsertTempUseRegMap(name);
	} else {
		reg = this->LoadVariableToReg(name);
		this->SetSymbolUse(name, true);
	}
	return reg;
}

void MipsGenerator::ResetJudgeReg(string name, Reg t0) {
	if (this->IsInteger(name)) {
		this->SetStackRegUnuse(this->RegToNumber(t0));
	} else if (this->IsChar(name)) {
		this->SetStackRegUnuse(this->RegToNumber(t0));
	} else if (this->IsConstVariable(name)) {
		this->SetStackRegUnuse(this->RegToNumber(t0));
	} else if (this->IsTempReg(name)) {
		this->PoptempReg(name);
	} else {
		// this->SetSymbolUse(name, false);
	}
}

void MipsGenerator::GenerateJudge(Midcode* midcode, MidcodeInstr judge) {
	MipsInstr mips_instr = MipsInstr::bgt;
	Reg t0 = Reg::zero;
	Reg t1 = Reg::zero;
	bool flag = true;

	switch (judge) {
	case MidcodeInstr::BGT:
		mips_instr = MipsInstr::bgt;
		t0 = SetJudgeReg(midcode, midcode->reg1());
		t1 = SetJudgeReg(midcode, midcode->reg2());
		break;
	case MidcodeInstr::BGE:
		mips_instr = MipsInstr::bge;
		t0 = SetJudgeReg(midcode, midcode->reg1());
		t1 = SetJudgeReg(midcode, midcode->reg2());
		break;
	case MidcodeInstr::BLT:
		mips_instr = MipsInstr::blt;
		t0 = SetJudgeReg(midcode, midcode->reg1());
		t1 = SetJudgeReg(midcode, midcode->reg2());
		break;
	case MidcodeInstr::BLE:
		mips_instr = MipsInstr::ble;
		t0 = SetJudgeReg(midcode, midcode->reg1());
		t1 = SetJudgeReg(midcode, midcode->reg2());
		break;
	case MidcodeInstr::BEQ:
		mips_instr = MipsInstr::beq;
		t0 = SetJudgeReg(midcode, midcode->reg1());
		t1 = SetJudgeReg(midcode, midcode->reg2());
		break;
	case MidcodeInstr::BNE:
		mips_instr = MipsInstr::bne;
		t0 = SetJudgeReg(midcode, midcode->reg1());
		t1 = SetJudgeReg(midcode, midcode->reg2());
		break;
	case MidcodeInstr::BEZ:
		mips_instr = MipsInstr::beq;
		t0 = SetJudgeReg(midcode, midcode->reg1());
		flag = false;
		break;
	case MidcodeInstr::BNZ:
		mips_instr = MipsInstr::bne;
		t0 = SetJudgeReg(midcode, midcode->reg1());
		flag = false;
		break;
	default:
		assert(0);
		break;
	}

	this->objcode_->Output(mips_instr, t0, t1, midcode->GetLabel());

	this->ResetJudgeReg(midcode->reg1(), t0);
	if (flag) {
		this->ResetJudgeReg(midcode->reg2(), t1);
	}
}

void MipsGenerator::GenerateCall(Midcode* midcode, int& parameter_count, int& call_and_save) {
	this->objcode_->Output(MipsInstr::jal, midcode->label());
	this->ResetAllReg(midcode->label());
	parameter_count = this->parameter_count_old;

	call_and_save--;
	if (call_and_save == 0) {
		parameter_count = REG_START;
	}
}

void MipsGenerator::GenerateSave(Midcode* midcode, int& parameter_count, int& call_and_save) {
	this->parameter_count_old = parameter_count;
	parameter_count = REG_START;

	this->SaveAllReg(midcode->label());
	int stack_length = check_table_->FindSymbol(
		midcode->label(), 0)->GetParameterCount() * 4;
	this->objcode_->Output(MipsInstr::move, Reg::fp, Reg::sp);
	this->objcode_->Output(MipsInstr::subi, Reg::sp, Reg::sp, stack_length);

	call_and_save++;
}

void MipsGenerator::GenerateScanf(Midcode* midcode, int type) {
	this->objcode_->Output(MipsInstr::li, Reg::v0, type);
	this->objcode_->Output(MipsInstr::syscall);
	this->objcode_->Output(MipsInstr::move,
		this->LoadVariableToReg(midcode->label()), Reg::v0);
}

void MipsGenerator::GenerateLabel(Midcode* midcode) {
	this->objcode_->Output(MipsInstr::label, midcode->GetLabel());
}

void MipsGenerator::GenerateReturn(Midcode* midcode, bool is_return_value) {

	if (is_return_value) {
		if (this->IsInteger(midcode->label())) {
			this->objcode_->Output(MipsInstr::li, Reg::v0, midcode->GetInteger());
		} else if (this->IsChar(midcode->label())) {
			this->objcode_->Output(MipsInstr::li, Reg::v0, midcode->GetChar());
		} else if (this->IsTempReg(midcode->label())) {
			this->objcode_->Output(MipsInstr::move, Reg::v0,
				this->LoadTempRegToReg(midcode->label()));
		} else if (IsConstVariable(midcode->label())) {
			this->objcode_->Output(MipsInstr::li, Reg::v0,
				this->GetConstVariable(midcode->label()));
		} else {
			this->objcode_->Output(MipsInstr::move, Reg::v0,
				this->LoadVariableToReg(midcode->label()));
		}
	}

	this->PopStackLength();
	this->objcode_->Output(MipsInstr::jr, Reg::ra);
}

void MipsGenerator::GenerateFuncEnd(int& variable_count,
	list<Midcode*>::iterator& iter) {
	this->PopStack();

	variable_count = 1;
	iter++;
	return;
}

void MipsGenerator::GenerateLoop() {
	this->PopAllVariable();
}

void MipsGenerator::GeneratePush(Midcode* midcode, int& parameter_count) {
	Reg reg = this->NumberToReg(parameter_count++);
	if (this->IsInteger(midcode->label())) {
		this->objcode_->Output(MipsInstr::li, reg, midcode->GetInteger());
	} else if (this->IsChar(midcode->label())) {
		this->objcode_->Output(MipsInstr::li, reg, midcode->GetChar());
	} else if (this->IsTempReg(midcode->label())) {
		this->objcode_->Output(MipsInstr::move, reg,
			this->LoadTempRegToReg(midcode->label()));
	} else if (IsConstVariable(midcode->label())) {
		this->objcode_->Output(MipsInstr::li, reg,
			this->GetConstVariable(midcode->label()));
	} else {
		this->objcode_->Output(MipsInstr::move, reg,
			this->LoadVariableToReg(midcode->label()));
	}
}

void MipsGenerator::GeneratePrintfEnd() {
	objcode_->Output(MipsInstr::li, Reg::a0, 10);
	objcode_->Output(MipsInstr::li, Reg::v0, 11);
	objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GeneratePrintfString(Midcode* midcode) {
	objcode_->Output(MipsInstr::la, Reg::a0, midcode->GetString());
	objcode_->Output(MipsInstr::li, Reg::v0, 4);
	objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GeneratePrintfString(int str_count) {
	objcode_->Output(MipsInstr::la, Reg::a0, "str_" + to_string(str_count));
	objcode_->Output(MipsInstr::li, Reg::v0, 4);
	objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GeneratePrintfIntChar(Midcode* midcode, int type) {
	if (this->IsInteger(midcode->label())) {
		objcode_->Output(MipsInstr::li, Reg::a0, midcode->GetInteger());
	} else if (this->IsChar(midcode->label())) {
		objcode_->Output(MipsInstr::li, Reg::v0, midcode->GetChar());
	} else if (this->IsTempReg(midcode->label())) {
		objcode_->Output(MipsInstr::move, Reg::a0,
			this->LoadTempRegToReg(midcode->label()));
	} else if (this->IsConstVariable(midcode->label())) {
		objcode_->Output(MipsInstr::li, Reg::a0,
			this->GetConstVariable(midcode->label()));
	} else {
		objcode_->Output(MipsInstr::move, Reg::a0,
			this->LoadVariableToReg(midcode->label()));
	}

	objcode_->Output(MipsInstr::li, Reg::v0, type);
	objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GeneratePrintfInt(int integer) {
	objcode_->Output(MipsInstr::li, Reg::a0, integer);
	objcode_->Output(MipsInstr::li, Reg::v0, 1);
	objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::GenerateAssignReturn(Midcode* midcode) {
	int reg = this->GetUnuseRegNumber();

	this->temp_use_reg_map_.insert(pair<string, int>(
		midcode->GetTempReg(), 1));
	this->temp_reg_map_.insert(pair<string, int>(
		midcode->GetTempReg(), reg));

	objcode_->Output(MipsInstr::move, this->NumberToReg(reg), Reg::v0);

	this->DeleteTempUseRegMap(midcode->GetTempReg());
}

void MipsGenerator::SetArrayIndex(int& offset, Symbol* symbol,
	string& array_index, Reg& reg_index) {
	int is_use_t9 = false;
	offset = symbol->sp_offer();

	if (this->IsInteger(array_index)) {
		offset += 4 * stoi(array_index);
	} else if (this->IsChar(array_index)) {
		offset += 4 * ((int)array_index[1]);
	} else if (this->IsTempReg(array_index)) {
		objcode_->Output(MipsInstr::sll, Reg::t9,
			this->LoadTempRegToReg(array_index), 2);
		objcode_->Output(MipsInstr::add, Reg::t9, Reg::t9, Reg::v1);
		is_use_t9 = true;
	} else if (this->IsConstVariable(array_index)) {
		objcode_->Output(MipsInstr::li, Reg::t9, this->GetConstVariable(array_index));
		objcode_->Output(MipsInstr::sll, Reg::t9, Reg::t9, 2);
		objcode_->Output(MipsInstr::add, Reg::t9, Reg::t9, Reg::v1);
		is_use_t9 = true;
	} else {
		objcode_->Output(MipsInstr::sll, Reg::t9,
			this->LoadVariableToReg(array_index), 2);
		objcode_->Output(MipsInstr::add, Reg::t9, Reg::t9, Reg::v1);
		is_use_t9 = true;
	}

	reg_index = is_use_t9 ? Reg::t9 : Reg::v1;
}

void MipsGenerator::GenerateLoadArray(Midcode* midcode) {
	int reg = this->GetUnuseRegNumber();

	this->temp_use_reg_map_.insert(pair<string, int>(
		midcode->GetTempReg(), 1));
	this->temp_reg_map_.insert(pair<string, int>(
		midcode->GetTempReg(), reg));

	Reg reg_index;
	int offset;

	Symbol* symbol = this->check_table_->FindSymbol(midcode->reg1());
	string array_index = midcode->reg2();

	this->SetArrayIndex(offset, symbol, array_index, reg_index);

	objcode_->Output(MipsInstr::lw, this->NumberToReg(reg), reg_index, offset);

	this->DeleteTempUseRegMap(midcode->GetTempReg());
}

void MipsGenerator::GenerateAssignArray(Midcode* midcode) {
	Reg reg_index;
	int offset;

	Symbol* symbol = this->check_table_->FindSymbol(midcode->reg_result());
	string array_index = midcode->reg1();

	this->SetArrayIndex(offset, symbol, array_index, reg_index);

	Reg reg_value;
	string value = midcode->reg2();

	if (this->IsInteger(value)) {
		this->objcode_->Output(MipsInstr::li, Reg::t8, stoi(value));
		reg_value = Reg::t8;
	} else if (this->IsChar(value)) {
		this->objcode_->Output(MipsInstr::li, Reg::t8, (int)value[1]);
		reg_value = Reg::t8;
	} else if (this->IsTempReg(value)) {
		reg_value = this->LoadTempRegToReg(value);
	} else if (IsConstVariable(value)) {
		this->objcode_->Output(MipsInstr::li, Reg::t8, this->GetConstVariable(value));
		reg_value = Reg::t8;
	} else {
		reg_value = this->LoadVariableToReg(value);
	}

	objcode_->Output(MipsInstr::sw, reg_value, reg_index, offset);
}

void MipsGenerator::GenerateAssign(Midcode* midcode) {
	int reg_result = 0;
	string result = midcode->reg_result();

	if (this->IsTempReg(result)) {
		reg_result = this->GetUnuseRegNumber();
		this->temp_use_reg_map_.insert(pair<string, int>(
			result, 1));
		this->temp_reg_map_.insert(pair<string, int>(
			result, reg_result));
	} else {
		reg_result = this->LoadVariableToNumber(result);
		this->SetSymbolUse(result, true);
	}

	string value = midcode->reg1();
	if (this->IsInteger(value)) {
		this->objcode_->Output(MipsInstr::li,
			this->NumberToReg(reg_result), stoi(value));
	} else if (this->IsChar(value)) {
		this->objcode_->Output(MipsInstr::li,
			this->NumberToReg(reg_result), (int)value[1]);
	} else if (this->IsTempReg(value)) {
		this->objcode_->Output(MipsInstr::move,
			this->NumberToReg(reg_result), this->LoadTempRegToReg(value));
		this->PoptempReg(value);
	} else if (IsConstVariable(value)) {
		this->objcode_->Output(MipsInstr::li,
			this->NumberToReg(reg_result), this->GetConstVariable(value));
	} else {
		this->objcode_->Output(MipsInstr::move,
			this->NumberToReg(reg_result), this->LoadVariableToReg(value));
	}

	if (this->IsTempReg(result)) {
		this->DeleteTempUseRegMap(result);
	} else {
		this->SetSymbolUse(result, false);
	}
}

void MipsGenerator::GenerateNeg(Midcode* midcode, string temp_reg_result) {
	int reg_result = 0;

	reg_result = this->GetUnuseRegNumber();
	this->InsertTempUseRegMap(temp_reg_result);
	this->temp_reg_map_.insert(pair<string, int>(temp_reg_result, reg_result));

	Reg reg1 = Reg::zero;
	string value1 = midcode->reg1();
	bool is_immediate = false;
	int immediate = 0;
	if (this->IsInteger(value1)) {
		is_immediate = true;
		immediate = -stoi(value1);
	} else if (this->IsChar(value1)) {
		is_immediate = true;
		immediate = -(int)value1[1];
	} else if (IsConstVariable(value1)) {
		is_immediate = true;
		immediate = this->GetConstVariable(value1);
	} else {
		reg1 = this->LoadVariableToReg(value1);
	}

	if (is_immediate) {
		objcode_->Output(MipsInstr::li,
			this->NumberToReg(reg_result), immediate);
	} else {
		objcode_->Output(MipsInstr::sub,
			this->NumberToReg(reg_result), Reg::zero, reg1);
		this->PoptempReg(reg1);
	}

	this->DeleteTempUseRegMap(temp_reg_result);
}

void MipsGenerator::DealRegOpReg(Midcode* midcode, MidcodeInstr op, int reg_result) {
	Reg reg1 = this->LoadTempRegToReg(midcode->GetTempReg1());
	this->InsertTempUseRegMap(midcode->GetTempReg1());

	Reg reg2 = this->LoadTempRegToReg(midcode->GetTempReg2());

	switch (op) {
	case MidcodeInstr::ADD:
		objcode_->Output(MipsInstr::add,
			this->NumberToReg(reg_result), reg1, reg2);
		break;
	case MidcodeInstr::SUB:
		objcode_->Output(MipsInstr::sub,
			this->NumberToReg(reg_result), reg1, reg2);
		break;
	case MidcodeInstr::MUL:
		objcode_->Output(MipsInstr::mul,
			this->NumberToReg(reg_result), reg1, reg2);
		break;
	case MidcodeInstr::DIV:
		objcode_->Output(MipsInstr::div,
			this->NumberToReg(reg_result), reg1, reg2);
		break;
	default:
		assert(0);
		break;
	}

	this->PoptempReg(midcode->GetTempReg1());
	this->PoptempReg(midcode->GetTempReg2());
}

void MipsGenerator::DealRegOpNumber(Midcode* midcode, MidcodeInstr op, int reg_result) {
	Reg reg1 = this->LoadTempRegToReg(midcode->GetTempReg1());

	Reg reg2 = Reg::zero;
	string value = midcode->reg2();
	bool is_immediate = false;
	int immediate = 0;
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
		reg2 = this->LoadVariableToReg(value);
	}

	switch (op) {
	case MidcodeInstr::ADD:
		if (is_immediate) {
			objcode_->Output(MipsInstr::addi,
				this->NumberToReg(reg_result), reg1, immediate);
		} else {
			objcode_->Output(MipsInstr::add,
				this->NumberToReg(reg_result), reg1, reg2);
		}
		break;
	case MidcodeInstr::SUB:
		if (is_immediate) {
			objcode_->Output(MipsInstr::subi,
				this->NumberToReg(reg_result), reg1, immediate);
		} else {
			objcode_->Output(MipsInstr::sub,
				this->NumberToReg(reg_result), reg1, reg2);
		}
		break;
	case MidcodeInstr::MUL:
		if (is_immediate) {
			objcode_->Output(MipsInstr::mul,
				this->NumberToReg(reg_result), reg1, immediate);
		} else {
			objcode_->Output(MipsInstr::mul,
				this->NumberToReg(reg_result), reg1, reg2);
		}
		break;
	case MidcodeInstr::DIV:
		if (is_immediate) {
			objcode_->Output(MipsInstr::div,
				this->NumberToReg(reg_result), reg1, immediate);
		} else {
			objcode_->Output(MipsInstr::div,
				this->NumberToReg(reg_result), reg1, reg2);
		}
		break;
	default:
		assert(0);
		break;
	}

	this->PoptempReg(midcode->GetTempReg1());
	this->PoptempReg(value);
}

void MipsGenerator::DealNumberOpNumber(Midcode* midcode, MidcodeInstr op, int reg_result) {
	Reg reg1 = Reg::zero;
	string value1 = midcode->reg1();
	bool is_immediate1 = false;
	int immediate1 = 0;
	if (this->IsInteger(value1)) {
		is_immediate1 = true;
		immediate1 = stoi(value1);
	} else if (this->IsChar(value1)) {
		is_immediate1 = true;
		immediate1 = (int)value1[1];
	} else if (IsConstVariable(value1)) {
		is_immediate1 = true;
		immediate1 = this->GetConstVariable(value1);
	} else {
		reg1 = this->LoadVariableToReg(value1);
	}

	Reg reg2 = Reg::zero;
	string value2 = midcode->reg2();
	bool is_immediate2 = false;
	int immediate2 = 0;
	if (this->IsInteger(value2)) {
		is_immediate2 = true;
		immediate2 = stoi(value2);
	} else if (this->IsChar(value2)) {
		is_immediate2 = true;
		immediate2 = (int)(value2[1]);
	} else if (IsConstVariable(value2)) {
		is_immediate2 = true;
		immediate2 = this->GetConstVariable(value2);
	} else {
		reg2 = this->LoadVariableToReg(value2);
	}

	if (is_immediate1 && is_immediate2) {
		switch (op) {
		case MidcodeInstr::ADD:
			objcode_->Output(MipsInstr::li,
				this->NumberToReg(reg_result), immediate1 + immediate2);
			break;
		case MidcodeInstr::SUB:
			objcode_->Output(MipsInstr::li,
				this->NumberToReg(reg_result), immediate1 - immediate2);
			break;
		case MidcodeInstr::MUL:
			objcode_->Output(MipsInstr::li,
				this->NumberToReg(reg_result), immediate1 * immediate2);
			break;
		case MidcodeInstr::DIV:
			objcode_->Output(MipsInstr::li,
				this->NumberToReg(reg_result), immediate1 / immediate2);
			break;
		default:
			assert(0);
			break;
		}
	} else if (is_immediate1) {
		objcode_->Output(MipsInstr::li,
			this->NumberToReg(reg_result), immediate1);

		switch (op) {
		case MidcodeInstr::ADD:
			if (is_immediate2) {
				objcode_->Output(MipsInstr::addi,
					this->NumberToReg(reg_result),
					this->NumberToReg(reg_result), immediate2);
			} else {
				objcode_->Output(MipsInstr::add,
					this->NumberToReg(reg_result),
					this->NumberToReg(reg_result), reg2);
			}
			break;
		case MidcodeInstr::SUB:
			if (is_immediate2) {
				objcode_->Output(MipsInstr::subi,
					this->NumberToReg(reg_result),
					this->NumberToReg(reg_result), immediate2);
			} else {
				objcode_->Output(MipsInstr::sub,
					this->NumberToReg(reg_result),
					this->NumberToReg(reg_result), reg2);
			}
			break;
		case MidcodeInstr::MUL:
			if (is_immediate2) {
				objcode_->Output(MipsInstr::mul,
					this->NumberToReg(reg_result),
					this->NumberToReg(reg_result), immediate2);
			} else {
				objcode_->Output(MipsInstr::mul,
					this->NumberToReg(reg_result),
					this->NumberToReg(reg_result), reg2);
			}
			break;
		case MidcodeInstr::DIV:
			if (is_immediate2) {
				objcode_->Output(MipsInstr::div,
					this->NumberToReg(reg_result),
					this->NumberToReg(reg_result), immediate2);
			} else {
				objcode_->Output(MipsInstr::div,
					this->NumberToReg(reg_result),
					this->NumberToReg(reg_result), reg2);
			}
			break;
		default:
			assert(0);
			break;
		}
	} else {
		switch (op) {
		case MidcodeInstr::ADD:
			if (is_immediate2) {
				objcode_->Output(MipsInstr::addi,
					this->NumberToReg(reg_result), reg1, immediate2);
			} else {
				objcode_->Output(MipsInstr::add,
					this->NumberToReg(reg_result), reg1, reg2);
			}
			break;
		case MidcodeInstr::SUB:
			if (is_immediate2) {
				objcode_->Output(MipsInstr::subi,
					this->NumberToReg(reg_result), reg1, immediate2);
			} else {
				objcode_->Output(MipsInstr::sub,
					this->NumberToReg(reg_result), reg1, reg2);
			}
			break;
		case MidcodeInstr::MUL:
			if (is_immediate2) {
				objcode_->Output(MipsInstr::mul,
					this->NumberToReg(reg_result), reg1, immediate2);
			} else {
				objcode_->Output(MipsInstr::mul,
					this->NumberToReg(reg_result), reg1, reg2);
			}
			break;
		case MidcodeInstr::DIV:
			if (is_immediate2) {
				objcode_->Output(MipsInstr::div,
					this->NumberToReg(reg_result), reg1, immediate2);
			} else {
				objcode_->Output(MipsInstr::div,
					this->NumberToReg(reg_result), reg1, reg2);
			}
			break;
		default:
			assert(0);
			break;
		}
	}

	this->PoptempReg(value1);
	this->PoptempReg(value2);
}

void MipsGenerator::DealNumberOpReg(Midcode* midcode, MidcodeInstr op, int reg_result) {
	Reg reg1 = Reg::zero;
	string value = midcode->reg2();
	bool is_immediate = false;
	int immediate = 0;
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
		reg1 = this->LoadVariableToReg(value);
	}

	Reg reg2 = this->LoadTempRegToReg(midcode->GetTempReg1());

	switch (op) {
	case MidcodeInstr::ADD:
		if (is_immediate) {
			objcode_->Output(MipsInstr::addi,
				this->NumberToReg(reg_result), reg2, immediate);
		} else {
			objcode_->Output(MipsInstr::add,
				this->NumberToReg(reg_result), reg1, reg2);
		}
		break;
	case MidcodeInstr::SUB:
		if (is_immediate) {
			objcode_->Output(MipsInstr::li,
				this->NumberToReg(reg_result), immediate);
			objcode_->Output(MipsInstr::sub,
				this->NumberToReg(reg_result),
				this->NumberToReg(reg_result), reg2);
		} else {
			objcode_->Output(MipsInstr::sub,
				this->NumberToReg(reg_result), reg1, reg2);
		}
		break;
	case MidcodeInstr::MUL:
		if (is_immediate) {
			objcode_->Output(MipsInstr::mul,
				this->NumberToReg(reg_result), reg2, immediate);
		} else {
			objcode_->Output(MipsInstr::mul,
				this->NumberToReg(reg_result), reg1, reg2);
		}
		break;
	case MidcodeInstr::DIV:
		if (is_immediate) {
			objcode_->Output(MipsInstr::li,
				this->NumberToReg(reg_result), immediate);
			objcode_->Output(MipsInstr::div,
				this->NumberToReg(reg_result),
				this->NumberToReg(reg_result), reg2);
		} else {
			objcode_->Output(MipsInstr::div,
				this->NumberToReg(reg_result), reg1, reg2);
		}
		break;
	default:
		assert(0);
		break;
	}

	this->PoptempReg(value);
	this->PoptempReg(midcode->GetTempReg1());
}

void MipsGenerator::GenerateOperate(list<Midcode*>::iterator& iter, Midcode* midcode, MidcodeInstr op, string result) {
	int reg_result = 0;

	Midcode* next_midcode = *(++iter);
	if (next_midcode->instr() == MidcodeInstr::ASSIGN 
		&& next_midcode->reg1() == midcode->GetTempRegResult()) {
		result = next_midcode->reg_result();
	} else {
		iter--;
	}

	if (this->IsTempReg(result)) {
		reg_result = this->GetUnuseRegNumber();
		this->InsertTempUseRegMap(result);
		this->temp_reg_map_.insert(pair<string, int>(
			result, reg_result));
	} else {
		reg_result = this->LoadVariableToNumber(result);
		this->SetSymbolUse(result, true);
	}

	switch (midcode->opera_member()) {
	case OperaMember::REG_OP_REG:
		this->DealRegOpReg(midcode, op, reg_result);
		break;
	case OperaMember::REG_OP_NUMBER:
		this->DealRegOpNumber(midcode, op, reg_result);
		break;
	case OperaMember::NUMBER_OP_NUMBER:
		this->DealNumberOpNumber(midcode, op, reg_result);
		break;
	case OperaMember::NUMBER_OP_REG:
		this->DealNumberOpReg(midcode, op, reg_result);
		break;
	default:
		assert(0);
		break;
	}

	if (this->IsTempReg(result)) {
		this->DeleteTempUseRegMap(result);
	} else {
		// this->SetSymbolUse(result, false);
	}
}

void MipsGenerator::GenerateStep(list<Midcode*>::iterator& iter, Midcode*& midcode) {
	midcode = *(++iter);
	this->GenerateOperate(iter, midcode, midcode->instr(), midcode->reg_result());
}

void MipsGenerator::SetFunctionVariable(Midcode* midcode, int& variable_count) {
	this->check_table_->FindSymbol(midcode->label())->set_reg_number(
		variable_count++);
	this->check_table_->FindSymbol(midcode->label())->set_is_use(true);
}

void MipsGenerator::GenerateBody(string function_name, list<Midcode*>::iterator& iter) {
	int parameter_count = REG_START;
	int variable_count = REG_START;
	int call_and_save = 0;
	Midcode* midcode;

	while (iter != midcode_list_.end()) {
		midcode = *iter;

		switch (midcode->instr()) {
		case MidcodeInstr::SCANF_INT:
			this->GenerateScanf(midcode, 5);
			break;
		case MidcodeInstr::SCANF_CHAR:
			this->GenerateScanf(midcode, 12);
			break;
		case MidcodeInstr::PRINTF_INT:
			this->GeneratePrintfIntChar(midcode, 1);
			break;
		case MidcodeInstr::PRINTF_CHAR:
			this->GeneratePrintfIntChar(midcode, 11);
			break;
		case MidcodeInstr::PRINTF_STRING:
			this->GeneratePrintfString(midcode);
			break;
		case MidcodeInstr::PRINTF_END:
			this->GeneratePrintfEnd();
			break;
		case MidcodeInstr::LOOP:
			// this->GenerateLoop();
			break;
		case MidcodeInstr::STEP:
			this->GenerateStep(iter, midcode);
			break;
		case MidcodeInstr::LABEL:
			this->GenerateLabel(midcode);
			break;
		case MidcodeInstr::JUMP:
			this->GenerateJump(midcode);
			break;
		case MidcodeInstr::ASSIGN:
			this->GenerateAssign(midcode);
			break;
		case MidcodeInstr::ASSIGN_ARRAY:
			this->GenerateAssignArray(midcode);
			break;
		case MidcodeInstr::ASSIGN_RETURN:
			this->GenerateAssignReturn(midcode);
			break;
		case MidcodeInstr::LOAD:
			assert(0);
			break;
		case MidcodeInstr::LOAD_ARRAY:
			this->GenerateLoadArray(midcode);
			break;
		case MidcodeInstr::ADD:
			this->GenerateOperate(iter, midcode,
				MidcodeInstr::ADD, midcode->GetTempRegResult());
			break;
		case MidcodeInstr::ADDI:
			assert(0);
			break;
		case MidcodeInstr::SUB:
			this->GenerateOperate(iter, midcode,
				MidcodeInstr::SUB, midcode->GetTempRegResult());
			break;
		case MidcodeInstr::SUBI:
			assert(0);
			break;
		case MidcodeInstr::NEG:
			this->GenerateNeg(midcode, "#" + to_string(midcode->count()));
			break;
		case MidcodeInstr::MUL:
			this->GenerateOperate(iter, midcode,
				MidcodeInstr::MUL, midcode->GetTempRegResult());
			break;
		case MidcodeInstr::DIV:
			this->GenerateOperate(iter, midcode,
				MidcodeInstr::DIV, midcode->GetTempRegResult());
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
			this->GeneratePush(midcode, parameter_count);
			break;
		case MidcodeInstr::CALL:
			this->GenerateCall(midcode, parameter_count, call_and_save);
			break;
		case MidcodeInstr::SAVE:
			this->GenerateSave(midcode, parameter_count, call_and_save);
			break;
		case MidcodeInstr::FUNCTION_END:
			return this->GenerateFuncEnd(variable_count, iter);
			break;
		case MidcodeInstr::RETURN:
			this->GenerateReturn(midcode, true);
			break;
		case MidcodeInstr::RETURN_NON:
			this->GenerateReturn(midcode, false);
			break;
		case MidcodeInstr::PARA_INT:
			this->SetFunctionVariable(midcode, variable_count);
			break;
		case MidcodeInstr::PARA_CHAR:
			this->SetFunctionVariable(midcode, variable_count);
			break;
		case MidcodeInstr::CONST_INT:
			assert(0);
			break;
		case MidcodeInstr::CONST_CHAR:
			assert(0);
			break;
		case MidcodeInstr::VAR_INT:
			this->SetFunctionVariable(midcode, variable_count);
			break;
		case MidcodeInstr::VAR_CHAR:
			this->SetFunctionVariable(midcode, variable_count);
			break;
		default:
			assert(0);
			break;
		}

		iter++;
	}
}

void MipsGenerator::GenerateFunction(Midcode* midcode, std::list<Midcode*>::iterator& iter) {
	this->LoadTable(1, midcode->label());
	this->objcode_->Output(MipsInstr::label, midcode->label());
	this->InitVariable(1);
	this->PopAllVariable();
	iter++;
	this->GenerateBody(midcode->label(), iter);
}

void MipsGenerator::Generate() {
	list<Midcode*>::iterator iter = midcode_list_.begin();
	Midcode* midcode;

	while (iter != midcode_list_.end()) {
		midcode = *iter;

		switch (midcode->instr()) {
		case MidcodeInstr::INT_FUNC_DECLARE:
			this->GenerateFunction(midcode, iter);
			break;
		case MidcodeInstr::CHAR_FUNC_DECLARE:
			this->GenerateFunction(midcode, iter);
			break;
		case MidcodeInstr::VOID_FUNC_DECLARE:
			this->GenerateFunction(midcode, iter);
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
	this->LoadTable(0, "global");
	this->InitDataSeg();
	this->PrintText();
	this->InitStack();
	this->InitVariable(0);
	this->PrintMain();
	this->PrintEnd();
	this->Generate();
}

void MipsGenerator::FileClose() {
	objcode_->FileClose();
}