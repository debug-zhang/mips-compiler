#include "mips_generator.h"

MipsGenerator::MipsGenerator(string outputFileName,
	StringTable* stringTable, CheckTable* check_table,
	map<string, SymbolTable*> symbolTableMap, list<Midcode*> midcode_list) {
	this->mips_file_ = new Objcode(outputFileName);
	this->check_table_ = check_table;
	this->string_table_ = stringTable;
	this->symbol_table_map_ = symbolTableMap;
	this->midcode_list_ = midcode_list;
	this->new_reg = 0;
	this->dm_offset = 0;
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
		mips_file_->Output(MipsInstr::subi, Reg::sp, Reg::sp, count * 4);
	}
}

void MipsGenerator::InitVariable(string function_name) {
	map<string, Symbol*> table = symbol_table_map_.at(function_name)->symbol_map();
	map<string, Symbol*>::iterator iter = table.begin();
	while (iter != table.end()) {
		if (iter->second->kind() == KindSymbol::ARRAY) {
			mips_file_->Output(MipsInstr::data_identifier, iter->second->name(),
				4 * (iter->second->array_length() + 5));
		} else if (iter->second->kind() == KindSymbol::VARIABLE) {
			mips_file_->Output(MipsInstr::data_identifier, iter->second->name(), 4);
		} else if (iter->second->kind() == KindSymbol::PARAMETER) {
			mips_file_->Output(MipsInstr::data_identifier, iter->second->name(), 4);
		}
		iter++;
	}
}

void MipsGenerator::InitDataSeg(string function_name) {
	mips_file_->Output(MipsInstr::data);
	mips_file_->Output(MipsInstr::data_identifier, function_name + "_space",
		check_table_->GetFunctionVariableNumber(function_name) * 200);

	if (function_name == "global") {
		for (int i = 0; i < string_table_->GetStringCount(); i++) {
			string data_string = "str_" + to_string(i) + ": .asciiz \""
				+ string_table_->GetString(i) + "\"";
			mips_file_->Output(MipsInstr::data_string, data_string);
		}
	}

	InitVariable(function_name);

	mips_file_->Output(MipsInstr::data_align, 4);
	mips_file_->Output();
}

void MipsGenerator::InitDataSeg() {
	map<string, SymbolTable*>::iterator iter = symbol_table_map_.begin();
	while (iter != symbol_table_map_.end()) {
		InitDataSeg(iter->first);
		iter++;
	}
}

void MipsGenerator::InitStack() {
	mips_file_->Output(MipsInstr::la, Reg::k0, "global_space");
	mips_file_->Output(MipsInstr::move, Reg::gp, Reg::sp);
	mips_file_->Output(MipsInstr::move, Reg::fp, Reg::sp);
	mips_file_->Output();
}

void MipsGenerator::PrintText() {
	mips_file_->Output(MipsInstr::text);
	mips_file_->Output();
}

void MipsGenerator::PrintMain() {
	mips_file_->Output(MipsInstr::jal, "main");
}

void MipsGenerator::PrintSyscall() {
	mips_file_->Output(MipsInstr::syscall);
}

void MipsGenerator::PrintEnd() {
	mips_file_->Output(MipsInstr::li, Reg::v0, 10);
	this->PrintSyscall();
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

bool MipsGenerator::IsConstVar(string name) {
	Symbol* symbol = check_table_->FindSymbol(name);
	if (symbol != NULL && symbol->kind() == KindSymbol::CONST) {
		return true;
	} else {
		return false;
	}
}

int MipsGenerator::GetConstVar(string name) {
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

void MipsGenerator::SaveAllReg() {
	mips_file_->Output(MipsInstr::addi, Reg::sp, Reg::sp, -20 * 4);

	int offset = 0;
	for (int i = 8; i < 26; i++) {
		mips_file_->Output(MipsInstr::sw, reg::NumberToReg(i), Reg::sp, offset);
		offset += 4;
	}
	mips_file_->Output(MipsInstr::sw, Reg::fp, Reg::sp, offset);
	mips_file_->Output(MipsInstr::sw, Reg::ra, Reg::sp, offset);
}

void MipsGenerator::ResetAllReg() {
	int offset = 0;
	for (int i = 8; i < 26; i++) {
		mips_file_->Output(MipsInstr::lw, reg::NumberToReg(i), Reg::sp, offset);
		offset += 4;
	}

	mips_file_->Output(MipsInstr::addi, Reg::sp, Reg::sp, 24 * 4);
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
		this->mips_file_->Output(MipsInstr::li, Reg::t1, IsTempValue(symbol) * 4);
		this->mips_file_->Output(MipsInstr::lw, reg, Reg::t1, function + "_space");

	} else {
		this->mips_file_->Output(MipsInstr::lw, reg, Reg::t1, symbol);
	}
}

void MipsGenerator::StoreValue(string symbol, string function, Reg reg) {
	if (IsTempValue(symbol)) {
		this->mips_file_->Output(MipsInstr::li, Reg::t1, IsTempValue(symbol) * 4);
		this->mips_file_->Output(MipsInstr::sw, reg, Reg::t1, function + "_space");

	} else {
		this->mips_file_->Output(MipsInstr::sw, reg, Reg::t1, symbol);
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
		mips_file_->Output(MipsInstr::addi, Reg::sp, Reg::sp, count * 4);
	}
}

void MipsGenerator::PopSymbolMapVar(int level) {
	Symbol* symbol;
	map<string, Symbol*> symbol_map = check_table_->GetSymbolMap(level);
	map<string, Symbol*>::iterator iter = symbol_map.begin();

	while (iter != symbol_map.end()) {
		symbol = iter->second;
		if (symbol->reg_number() != 0 && !symbol->is_use()) {
			mips_file_->Output(MipsInstr::sw, reg::NumberToReg(symbol->reg_number()),
				Reg::gp, symbol->sp_offer());
			reg_use_stack_[symbol->reg_number()] = 0;
			symbol->set_reg_number(0);
		}
		iter++;
	}
}

void MipsGenerator::PopAllVar() {
	this->PopSymbolMapVar(0);
	this->PopSymbolMapVar(1);
}

int MipsGenerator::LoadValue(string name) {
	Symbol* symbol = check_table_->FindSymbol(name);
	int level = check_table_->GetSymbolLevel(name);

	if (symbol->reg_number() == 0) {
		int reg_number = this->GetUnuseReg();
		Reg t1 = level == 1 ? Reg::fp : Reg::gp;
		mips_file_->Output(MipsInstr::lw, reg::NumberToReg(reg_number), t1,
			symbol->sp_offer());
		symbol->set_reg_number(reg_number);
	}

	return symbol->reg_number();
}

int MipsGenerator::LoadMidReg(string name) {
	if (mid_var_reg_map_.at(name) > 0) {
		return mid_var_reg_map_.at(name);
	} else {
		int unuse_reg = this->GetUnuseReg();
		mips_file_->Output(MipsInstr::lw, reg::NumberToReg(unuse_reg), Reg::k0, -mid_var_reg_map_.at(name));
		mid_var_reg_map_.at(name) = unuse_reg;
		return unuse_reg;
	}
}

void MipsGenerator::PopMidReg(string name) {
	if (mid_var_reg_map_.find(name) == mid_var_reg_map_.end()) {
		return;
	}
	int reg = mid_var_reg_map_.at(name);
	if (reg > 0) {
		reg_use_stack_[reg] = 0;
	}
	mid_var_reg_map_.erase(name);
	mid_use_reg_map_.erase(name);
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
			mips_file_->Output(MipsInstr::sw, reg::NumberToReg(unuse_reg),
				Reg::gp, symbol->sp_offer());
			symbol->set_reg_number(0);
			return unuse_reg;
		}
		iter++;
	}
	retflag = false;
	return {};
}

int MipsGenerator::GetUnuseReg() {
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

	map<string, int>::iterator iter = mid_var_reg_map_.begin();
	while (iter != mid_var_reg_map_.end()) {
		if (iter->second > 0
			&& mid_use_reg_map_.find(iter->first) == mid_use_reg_map_.end()) {
			unuse_reg = iter->second;
			dm_offset += 4;
			iter->second = -dm_offset;
			mips_file_->Output(MipsInstr::sw, reg::NumberToReg(unuse_reg),
				Reg::k0, dm_offset);
			return unuse_reg;
		}
		iter++;
	}

	return 0;
}

void MipsGenerator::SetSymbolUse(string name, bool isUse) {
	check_table_->FindSymbol(name)->set_is_use(isUse);
}

void MipsGenerator::GenerateBody(string function_name, list<Midcode*>::iterator& iter) {
	Midcode* midcode;

	while (iter != midcode_list_.end()) {
		midcode = *iter;

		switch (midcode->instr()) {
		case MidcodeInstr::SCANF_INT:
			break;
		case MidcodeInstr::SCANF_CHAR:
			break;
		case MidcodeInstr::PRINTF_INT:
			break;
		case MidcodeInstr::PRINTF_CHAR:
			break;
		case MidcodeInstr::PRINTF_STRING:
			break;
		case MidcodeInstr::PRINTF_END:
			break;
		case MidcodeInstr::LABEL:
			break;
		case MidcodeInstr::JUMP:
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
			break;
		case MidcodeInstr::BGE:
			break;
		case MidcodeInstr::BLT:
			break;
		case MidcodeInstr::BLE:
			break;
		case MidcodeInstr::BEQ:
			break;
		case MidcodeInstr::BNE:
			break;
		case MidcodeInstr::BEZ:
			break;
		case MidcodeInstr::BNZ:
			break;
		case MidcodeInstr::INT_FUNC_DECLARE:
			break;
		case MidcodeInstr::CHAR_FUNC_DECLARE:
			break;
		case MidcodeInstr::VOID_FUNC_DECLARE:
			break;
		case MidcodeInstr::PUSH:
			break;
		case MidcodeInstr::CALL:
			break;
		case MidcodeInstr::SAVE:
			break;
		case MidcodeInstr::RETURN:
			break;
		case MidcodeInstr::RETURN_NON:
			break;
		case MidcodeInstr::PARA_INT:
			break;
		case MidcodeInstr::PARA_CHAR:
			break;
		case MidcodeInstr::CONST_INT:
			break;
		case MidcodeInstr::CONST_CHAR:
			break;
		case MidcodeInstr::VAR_INT:
			break;
		case MidcodeInstr::VAR_CHAR:
			break;
		default:
			assert(0);
			break;
		}

		iter++;
	}
}

void MipsGenerator::GenerateFunction() {
	list<Midcode*>::iterator iter = midcode_list_.begin();
	Midcode* midcode;

	while (iter != midcode_list_.end()) {
		midcode = *iter;

		switch (midcode->instr()) {
		case MidcodeInstr::INT_FUNC_DECLARE:
			this->GenerateBody(midcode->label(), iter);
			break;
		case MidcodeInstr::CHAR_FUNC_DECLARE:
			this->GenerateBody(midcode->label(), iter);
			break;
		case MidcodeInstr::VOID_FUNC_DECLARE:
			this->GenerateBody(midcode->label(), iter);
			break;
		default:
			assert(0);
			break;
		}
	}
}

void MipsGenerator::GenerateMips() {
	this->InitDataSeg();
	this->PrintText();
	this->InitStack();
	this->PrintMain();
	this->PrintEnd();
	this->GenerateFunction();
}

void MipsGenerator::FileClose() {
	mips_file_->FileClose();
}