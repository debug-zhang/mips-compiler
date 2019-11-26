#include "table.h"

// StringTable
StringTable::StringTable() {
	this->string_number_ = 0;
}

int StringTable::AddString(string str) {
	this->string_map_.insert(pair<int, string>(this->string_number_, str));
	return string_number_++;
}

int StringTable::GetStringCount() {
	return this->string_number_;
}

string StringTable::GetString(int string_number) {
	map<int, string>::iterator iter = this->string_map_.find(string_number);

	if (iter == this->string_map_.end()) {
		return "";
	} else {
		return iter->second;
	}
}


// SymbolTable
SymbolTable::SymbolTable() {
}

Symbol* SymbolTable::AddSymbol(string name, KindSymbol kind, TypeSymbol type) {
	Symbol* symbol = this->FindSymbol(name);
	if (symbol != NULL) {
		symbol->SetProperty(name, kind, type);
	} else {
		symbol = new Symbol();
		symbol->SetProperty(name, kind, type);
		this->symbol_map_.insert(pair<string, Symbol*>(name, symbol));
	}
	return symbol;
}

Symbol* SymbolTable::FindSymbol(string name) {
	map<string, Symbol*>::iterator iter = this->symbol_map_.find(name);

	if (iter == this->symbol_map_.end()) {
		return NULL;
	} else {
		return iter->second;
	}
}

map<string, Symbol*> SymbolTable::symbol_map() {
	return this->symbol_map_;
}


// CheckTable
CheckTable::CheckTable() {
	SymbolTable* map0 = new SymbolTable();
	SymbolTable* map1 = new SymbolTable();
	this->symbol_table_vector_.push_back(map0);
	this->symbol_table_vector_.push_back(map1);
}

Symbol* CheckTable::AddSymbol(string name, KindSymbol kind, TypeSymbol type, int level) {
	return this->symbol_table_vector_[level]->AddSymbol(name, kind, type);
}

Symbol* CheckTable::FindSymbol(string name) {
	Symbol* symbol;
	symbol = this->symbol_table_vector_[1]->FindSymbol(name);
	if (symbol == NULL) {
		symbol = this->symbol_table_vector_[0]->FindSymbol(name);
	}
	return symbol;
}

Symbol* CheckTable::FindSymbol(string name, int level) {
	return this->symbol_table_vector_[level]->FindSymbol(name);
}

int CheckTable::GetSymbolLevel(string name) {
	if (this->FindSymbol(name, 1) != NULL) {
		return 1;
	} else {
		return 0;
	}
}

void CheckTable::ClearLevel(int level) {
	this->symbol_table_vector_[level] = new SymbolTable();
}

void CheckTable::SetTable(int level, SymbolTable* symbol_table) {
	this->symbol_table_vector_[level] = symbol_table;
}

SymbolTable* CheckTable::GetSymbolTable(int level) {
	return this->symbol_table_vector_[level];
}

map<string, Symbol*> CheckTable::GetSymbolMap(int level) {
	return this->GetSymbolTable(level)->symbol_map();
}

void CheckTable::AddFunctionVariableNumber(string function_name, int number) {
	function_variable.insert(pair<string, int>(function_name, number));
}

int CheckTable::GetFunctionVariableNumber(string function_name) {
	return function_variable.at(function_name);
}