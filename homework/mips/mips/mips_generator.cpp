#include "mips_generator.h"

MipsGenerator::MipsGenerator(string outputFileName,
	StringTable* stringTable, CheckTable* check_table,
	map<string, SymbolTable*> symbolTableMap, list<Midcode*> midcode_list) {
	this->objcode_ = new Objcode(outputFileName);
	this->check_table_ = check_table;
	this->string_table_ = stringTable;
	this->symbol_table_map_ = symbolTableMap;
	this->midcode_list_ = midcode_list;
	this->new_reg = 0;
	this->dm_offset = 0;
}

void MipsGenerator::InsertTempUseRegMap(string name) {
	this->temp_use_reg_map_.insert(pair<string, bool>(name, true));
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
			objcode_->Output(MipsInstr::data_identifier, iter->second->name(),
				4 * (iter->second->array_length() + 5));
		} else if (iter->second->kind() == KindSymbol::VARIABLE) {
			objcode_->Output(MipsInstr::data_identifier, iter->second->name(), 4);
		} else if (iter->second->kind() == KindSymbol::PARAMETER) {
			objcode_->Output(MipsInstr::data_identifier, iter->second->name(), 4);
		}
		iter++;
	}
}

void MipsGenerator::InitDataSeg(string function_name) {
	objcode_->Output(MipsInstr::data);
	objcode_->Output(MipsInstr::data_identifier, function_name + "_space",
		check_table_->GetFunctionVariableNumber(function_name) * 200);

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
	objcode_->Output(MipsInstr::la, Reg::k0, "global_space");
	objcode_->Output(MipsInstr::move, Reg::gp, Reg::sp);
	objcode_->Output(MipsInstr::move, Reg::fp, Reg::sp);
	objcode_->Output();
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
	objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, -18 * 4);

	int offset = 0;
	for (int i = 8; i < 24; i++) {
		objcode_->Output(MipsInstr::sw, this->NumberToReg(i), Reg::sp, offset);
		offset += 4;
	}
	objcode_->Output(MipsInstr::sw, Reg::fp, Reg::sp, offset);
	objcode_->Output(MipsInstr::sw, Reg::ra, Reg::sp, offset);
}

void MipsGenerator::ResetAllReg() {
	int offset = 0;
	for (int i = 8; i < 24; i++) {
		objcode_->Output(MipsInstr::lw, this->NumberToReg(i), Reg::sp, offset);
		offset += 4;
	}

	objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, 18 * 4);
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

void MipsGenerator::LoadValue(string symbol, string function, Reg reg) {
	if (IsTempValue(symbol)) {
		this->objcode_->Output(MipsInstr::li, Reg::t1, IsTempValue(symbol) * 4);
		this->objcode_->Output(MipsInstr::lw, reg, Reg::t1, function + "_space");

	} else {
		this->objcode_->Output(MipsInstr::lw, reg, Reg::t1, symbol);
	}
}

void MipsGenerator::StoreValue(string symbol, string function, Reg reg) {
	if (IsTempValue(symbol)) {
		this->objcode_->Output(MipsInstr::li, Reg::t1, IsTempValue(symbol) * 4);
		this->objcode_->Output(MipsInstr::sw, reg, Reg::t1, function + "_space");

	} else {
		this->objcode_->Output(MipsInstr::sw, reg, Reg::t1, symbol);
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
		if (symbol->is_use()) {
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

	if (count != 0) {
		objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, count * 4);
	}
}

void MipsGenerator::PopSymbolMapVar(int level) {
	Symbol* symbol;
	map<string, Symbol*> symbol_map = check_table_->GetSymbolMap(level);
	map<string, Symbol*>::iterator iter = symbol_map.begin();

	while (iter != symbol_map.end()) {
		symbol = iter->second;
		if (symbol->reg_number() != 0 && !symbol->is_use()) {
			objcode_->Output(MipsInstr::sw, this->NumberToReg(symbol->reg_number()),
				Reg::gp, symbol->sp_offer());
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
	if (temp_var_reg_map_.at(name) > 0) {
		return temp_var_reg_map_.at(name);
	} else {
		int unuse_reg = this->GetUnuseRegNumber();
		objcode_->Output(MipsInstr::lw, this->NumberToReg(unuse_reg),
			Reg::k0, -temp_var_reg_map_.at(name));
		temp_var_reg_map_.at(name) = unuse_reg;
		return unuse_reg;
	}
}

Reg MipsGenerator::LoadTempRegToReg(string name) {
	return this->NumberToReg(this->LoadTempRegToNumber(name));
}

void MipsGenerator::PoptempReg(string name) {
	if (temp_var_reg_map_.find(name) == temp_var_reg_map_.end()) {
		return;
	}
	int reg = temp_var_reg_map_.at(name);
	if (reg > 0) {
		reg_use_stack_[reg] = 0;
	}
	temp_var_reg_map_.erase(name);
	temp_use_reg_map_.erase(name);
}

int MipsGenerator::GetUnuseRegInTable(int& unuse_reg, bool& retflag, int level) {
	Symbol* symbol;
	map<string, Symbol*> symbol_map = check_table_->GetSymbolMap(level);
	map<string, Symbol*>::iterator iter = symbol_map.begin();

	retflag = true;
	while (iter != symbol_map.end()) {
		symbol = iter->second;
		if (symbol->reg_number() != 0 && !symbol->is_use()) {
			unuse_reg = symbol->reg_number();
			objcode_->Output(MipsInstr::sw, this->NumberToReg(unuse_reg),
				Reg::gp, symbol->sp_offer());
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
	for (int i = 8; i < 32; i++) {
		if (reg_use_stack_[new_reg] == 0) {
			reg_use_stack_[new_reg] = 1;
			unuse_reg = new_reg;
			if (new_reg == 25) {
				new_reg = 0;
			} else {
				new_reg++;
			}
			return unuse_reg;
		} else {
			if (new_reg == 25) {
				new_reg = 0;
			} else {
				new_reg++;
			}
		}
	}

	bool retflag;
	int retval;

	retval = this->GetUnuseRegInTable(unuse_reg, retflag, 1);
	if (retflag) return retval;

	retval = this->GetUnuseRegInTable(unuse_reg, retflag, 0);
	if (retflag) return retval;

	map<string, bool>::iterator iter = temp_var_reg_map_.begin();
	while (iter != temp_var_reg_map_.end()) {
		if (iter->second > 0
			&& temp_use_reg_map_.find(iter->first) == temp_use_reg_map_.end()) {
			unuse_reg = iter->second;
			dm_offset += 4;
			iter->second = -dm_offset;
			objcode_->Output(MipsInstr::sw, this->NumberToReg(unuse_reg),
				Reg::k0, dm_offset);
			return unuse_reg;
		}
		iter++;
	}

	return 0;
}

Reg MipsGenerator::GetUnuseReg() {
	int unuse_reg;
	for (int i = 8; i < 32; i++) {
		if (reg_use_stack_[new_reg] == 0) {
			reg_use_stack_[new_reg] = 1;
			unuse_reg = new_reg;
			if (new_reg == 25) {
				new_reg = 0;
			} else {
				new_reg++;
			}
			return this->NumberToReg(unuse_reg);
		} else {
			if (new_reg == 25) {
				new_reg = 0;
			} else {
				new_reg++;
			}
		}
	}

	bool retflag;
	int retval;

	retval = this->GetUnuseRegInTable(unuse_reg, retflag, 1);
	if (retflag) return this->NumberToReg(retval);

	retval = this->GetUnuseRegInTable(unuse_reg, retflag, 0);
	if (retflag) return this->NumberToReg(retval);

	map<string, bool>::iterator iter = temp_var_reg_map_.begin();
	while (iter != temp_var_reg_map_.end()) {
		if (iter->second > 0
			&& temp_use_reg_map_.find(iter->first) == temp_use_reg_map_.end()) {
			unuse_reg = iter->second;
			dm_offset += 4;
			iter->second = -dm_offset;
			objcode_->Output(MipsInstr::sw, this->NumberToReg(unuse_reg),
				Reg::k0, dm_offset);
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
		this->objcode_->Output(MipsInstr::li, reg, midcode->GetInteger());
	} else if (this->IsChar(name)) {
		reg = this->GetUnuseReg();
		this->objcode_->Output(MipsInstr::li, reg, midcode->GetChar());
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
		this->SetSymbolUse(name, false);
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

	this->ResetJudgeReg(midcode->reg1, t0);
	if (flag) {
		this->ResetJudgeReg(midcode->reg2, t1);
	}
}

void MipsGenerator::GenerateSave(Midcode* midcode) {
	this->SaveAllReg();
	int stack_length = check_table_->FindSymbol(
		midcode->label(), 0)->GetParameterCount() * 4;
	this->objcode_->Output(MipsInstr::move, Reg::fp, Reg::sp);
	this->objcode_->Output(MipsInstr::subi, Reg::sp, Reg::sp, -stack_length);
}

void MipsGenerator::GenerateCall(Midcode* midcode) {
	this->objcode_->Output(MipsInstr::jal, midcode->label());
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

	this->PopStack();
	this->objcode_->Output(MipsInstr::jr, Reg::ra);
}

void MipsGenerator::GenerateLoop() {
	this->PopAllVariable();
}

void MipsGenerator::GeneratePush(Midcode* midcode, int parameter_count) {
	if (this->IsInteger(midcode->label())) {
		this->objcode_->Output(MipsInstr::li, Reg::t8, midcode->GetInteger());
		this->objcode_->Output(MipsInstr::sw, Reg::t8, Reg::fp,
			-4 * parameter_count);
	} else if (this->IsChar(midcode->label())) {
		this->objcode_->Output(MipsInstr::li, Reg::t8, midcode->GetChar());
		this->objcode_->Output(MipsInstr::sw, Reg::t8, Reg::fp,
			-4 * parameter_count);
	} else if (this->IsTempReg(midcode->label())) {
		this->objcode_->Output(MipsInstr::sw,
			this->LoadTempRegToReg(midcode->label()),
			Reg::fp, -4 * parameter_count);
		this->PoptempReg(midcode->label());
	} else if (IsConstVariable(midcode->label())) {
		this->objcode_->Output(MipsInstr::li, Reg::t8,
			this->GetConstVariable(midcode->label()));
		this->objcode_->Output(MipsInstr::sw, Reg::t8, Reg::fp,
			-4 * parameter_count);
	} else {
		this->objcode_->Output(MipsInstr::sw,
			this->LoadVariableToReg(midcode->label()),
			Reg::fp,
			-4 * parameter_count);
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

void MipsGenerator::GeneratePrintfIntChar(Midcode* midcode, int type) {
	if (this->IsInteger(midcode->label())) {
		objcode_->Output(MipsInstr::li, Reg::a0, midcode->GetInteger());
	} else if (this->IsChar(midcode->label())) {
		objcode_->Output(MipsInstr::li, Reg::v0, midcode->GetChar());
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

void MipsGenerator::GenerateBody(string function_name, list<Midcode*>::iterator& iter) {
	static int parameter_count = 1;
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
			this->GenerateLoop();
			break;
		case MidcodeInstr::LABEL:
			this->GenerateLabel(midcode);
			break;
		case MidcodeInstr::JUMP:
			this->GenerateJump(midcode);
			break;
		case MidcodeInstr::ASSIGN:
			break;
		case MidcodeInstr::ASSIGN_ARRAY:
			break;
		case MidcodeInstr::ASSIGN_RETURN:
			break;
		case MidcodeInstr::LOAD:
			break;
		case MidcodeInstr::LOAD_ARRAY:
			break;
		case MidcodeInstr::ADD:
			break;
		case MidcodeInstr::ADDI:
			break;
		case MidcodeInstr::SUB:
			break;
		case MidcodeInstr::SUBI:
			break;
		case MidcodeInstr::NEG:
			break;
		case MidcodeInstr::MULT:
			break;
		case MidcodeInstr::DIV:
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
			parameter_count++;
			break;
		case MidcodeInstr::CALL:
			this->GenerateCall(midcode);
			break;
		case MidcodeInstr::SAVE:
			this->GenerateSave(midcode);
			break;
		case MidcodeInstr::RETURN:
			this->GenerateReturn(midcode, true);
			parameter_count = 1;
			return;
			break;
		case MidcodeInstr::RETURN_NON:
			this->GenerateReturn(midcode, false);
			parameter_count = 1;
			return;
			break;
		case MidcodeInstr::PARA_INT:
			assert(0);
			break;
		case MidcodeInstr::PARA_CHAR:
			assert(0);
			break;
		case MidcodeInstr::CONST_INT:
			assert(0);
			break;
		case MidcodeInstr::CONST_CHAR:
			assert(0);
			break;
		case MidcodeInstr::VAR_INT:
			assert(0);
			break;
		case MidcodeInstr::VAR_CHAR:
			assert(0);
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
	this->InitVariable(1);
	this->PopAllVariable();
	this->objcode_->Output(MipsInstr::label, midcode->label());
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
	this->PrintMain();
	this->PrintEnd();
	this->Generate();
}

void MipsGenerator::FileClose() {
	objcode_->FileClose();
}