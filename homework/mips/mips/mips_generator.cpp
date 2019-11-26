#include "mips_generator.h"

MipsGenerator::MipsGenerator(string inputFileName, string outputFileName,
	StringTable* stringTable, CheckTable* check_table, 
	map<string, SymbolTable*> symbolTableMap) {
	this->midcode_.open(inputFileName);
	this->objcode_ = new Objcode(outputFileName);
	this->check_table_ = check_table;
	this->string_table_ = stringTable;
	this->symbol_table_map_ = symbolTableMap;
	this->new_reg = 0;
	this->dm_offset = 0;
}

vector<string> MipsGenerator::Split(string s, char delimiter) {
	vector<string> tokens;
	string token;
	istringstream token_stream(s);
	while (getline(token_stream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
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

void MipsGenerator::PrintSyscall() {
	objcode_->Output(MipsInstr::syscall);
}

void MipsGenerator::PrintEnd() {
	objcode_->Output(MipsInstr::li, Reg::v0, 10);
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
	objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, -20 * 4);

	int offset = 0;
	for (int i = 8; i < 26; i++) {
		objcode_->Output(MipsInstr::sw, reg::NumberToReg(i), Reg::sp, offset);
		offset += 4;
	}
	objcode_->Output(MipsInstr::sw, Reg::fp, Reg::sp, offset);
	objcode_->Output(MipsInstr::sw, Reg::ra, Reg::sp, offset);
}

void MipsGenerator::ResetAllReg() {
	int offset = 0;
	for (int i = 8; i < 26; i++) {
		objcode_->Output(MipsInstr::lw, reg::NumberToReg(i), Reg::sp, offset);
		offset += 4;
	}

	objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, 24 * 4);
}

int MipsGenerator::IsTempValue(string name) {
	if (name[0] == '#') {
		int offset = 0;

		string::iterator iter = name.begin();
		while (iter!= name.end()) {
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
		this->objcode_->Output(MipsInstr::li, Reg::t1, IsTempValue(symbol)*4);
		this->objcode_->Output(MipsInstr::lw, reg, Reg::t1, function+"_space");
		
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
			objcode_->Output(MipsInstr::sw, reg::NumberToReg(symbol->reg_number()),
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
		objcode_->Output(MipsInstr::lw, reg::NumberToReg(reg_number), t1,
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
		objcode_->Output(MipsInstr::lw, reg::NumberToReg(unuse_reg), Reg::k0, -mid_var_reg_map_.at(name));
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
			objcode_->Output(MipsInstr::sw, reg::NumberToReg(unuse_reg),
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
			objcode_->Output(MipsInstr::sw, reg::NumberToReg(unuse_reg),
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

void MipsGenerator::GenerateBody(string function_name) {
	string line;

	while (!midcode_.eof()) {
		getline(midcode_, line);

		if (line.empty()) {
			return;
		}

		vector<string> strs = this->Split(line, ' ');
		if (this->IsThisInstr(strs[0], midcodeinstr::FUNC_DECLARE)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::VOID_FUNC_DECLARE)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::PUSH)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::CALL)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::SAVE)) {
			SaveAllReg();
		} else if (this->IsThisInstr(strs[0], midcodeinstr::RETURN)) {
			objcode_->Output(MipsInstr::addi, Reg::sp, Reg::sp, 8);
			objcode_->Output(MipsInstr::jr, Reg::ra);
			break;
		} else if (this->IsThisInstr(strs[0], midcodeinstr::SCANF)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::PRINTF)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::PRINTF_END)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::LABEL)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::GO_TO_LABEL)) {
			objcode_->Output(MipsInstr::j, strs[2]);
		} else if (this->IsThisInstr(strs[0], midcodeinstr::ASSIGN)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::ASSIGN_RETURN)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::LOAD)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::REG_OP_NUMBER)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::REG_OP_REG)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::NUMBER_OP_REG)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::NUMBER_OP_NUMBER)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::BEZ)) {
			objcode_->Output(MipsInstr::beq, Reg::zero, strs[3]);
		} else if (this->IsThisInstr(strs[0], midcodeinstr::BNZ)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::BEQ)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::BNE)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::BGE)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::BLT)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::BGT)) {

		} else if (this->IsThisInstr(strs[0], midcodeinstr::BLE)) {

		} else {
			assert(0);
		}
	}
}

void MipsGenerator::GenerateFunction() {
	string line;

	while (!midcode_.eof()) {
		getline(midcode_, line);

		if (line.empty()) {
			return;
		}

		vector<string> strs = this->Split(line, ' ');

		if (this->IsThisInstr(strs[0], midcodeinstr::FUNC_DECLARE)) {
			GenerateBody(strs[2]);
		} else if (this->IsThisInstr(strs[0], midcodeinstr::VOID_FUNC_DECLARE)) {
			GenerateBody(strs[2]);
		} else {
			assert(0);
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
	midcode_.close();
	objcode_->FileClose();
}