#include "table.h"

// StringTable
StringTable::StringTable() {
	this->stringNumber = 0;
}

int StringTable::AddString(string str) {
	this->stringMap.insert(pair<int, string>(this->stringNumber, str));
	return stringNumber++;
}

int StringTable::GetStringNumber() {
	return this->stringNumber;
}

string StringTable::GetString(int stringNumber) {
	map<int, string>::iterator iter = this->stringMap.find(stringNumber);

	if (iter == this->stringMap.end()) {
		return "";
	} else {
		return iter->second;
	}
}


// SymbolTable
SymbolTable::SymbolTable() {
}

Symbol* SymbolTable::AddSymbol(string name, KIND_SYMBOL kind, TYPE_SYMBOL type) {
	Symbol* symbol = this->FindSymbol(name);
	if (symbol != NULL) {
		symbol->SetProperty(name, kind, type);
	} else {
		symbol = new Symbol();
		symbol->SetProperty(name, kind, type);
		this->symbolMap.insert(pair<string, Symbol*>(name, symbol));
	}
	return symbol;
}

Symbol* SymbolTable::FindSymbol(string name) {
	map<string, Symbol*>::iterator iter = this->symbolMap.find(name);

	if (iter == this->symbolMap.end()) {
		return NULL;
	} else {
		return iter->second;
	}
}


// CheckTable
CheckTable::CheckTable() {
	SymbolTable* map0 = new SymbolTable();
	SymbolTable* map1 = new SymbolTable();
	this->symbolMapVector.push_back(map0);
	this->symbolMapVector.push_back(map1);
}

Symbol* CheckTable::AddSymbol(string name, KIND_SYMBOL kind, TYPE_SYMBOL type, int level) {
	return this->symbolMapVector[level]->AddSymbol(name, kind, type);
}

Symbol* CheckTable::FindSymbol(string name, int level) {
	return this->symbolMapVector[level]->FindSymbol(name);
}

void CheckTable::ClearLevel(int level) {
	this->symbolMapVector[level] = new SymbolTable();
}

SymbolTable* CheckTable::GetSymbolMap(int level) {
	return this->symbolMapVector[level];
}