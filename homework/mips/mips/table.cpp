#include "table.h"

// StringTable
StringTable::StringTable() {
	this->stringNumber = 0;
}

void StringTable::AddString(string str) {
	stringMap.insert(pair<int, string>(stringNumber++, str));
}

int StringTable::GetStringNumber() {
	return stringNumber;
}

string StringTable::GetString(int stringNumber) {
	map<int, string>::iterator iter = stringMap.find(stringNumber);

	if (iter == stringMap.end()) {
		return "";
	} else {
		return iter->second;
	}
}


// SymbolTable
SymbolTable::SymbolTable() {
}

void SymbolTable::AddSymbol(string name, KIND_SYMBOL kind, TYPE_SYMBOL type) {
	struct Symbol* symbol = FindSymbol(name);
	if (symbol != NULL) {
		symbol->setProperty(name, kind, type);
	} else {
		struct Symbol symbol;
		symbol.setProperty(name, kind, type);
		symbolMap.insert(pair<string, struct Symbol>(name, symbol));
	}
}

struct Symbol* SymbolTable::FindSymbol(string name) {
	map<string, struct Symbol>::iterator iter = symbolMap.find(name);

	if (iter == symbolMap.end()) {
		return NULL;
	} else {
		return &(iter->second);
	}
}


// CheckTable
CheckTable::CheckTable() {
	SymbolTable* map0 = new SymbolTable();
	SymbolTable* map1 = new SymbolTable();
	this->symbolMapVector.push_back(map0);
	this->symbolMapVector.push_back(map1);
}

void CheckTable::AddSymbol(string name, KIND_SYMBOL kind, TYPE_SYMBOL type, int level) {
	symbolMapVector[level]->AddSymbol(name, kind, type);
}

struct Symbol* CheckTable::FindSymbol(string name, int level) {
	return symbolMapVector[level]->FindSymbol(name);
}

void CheckTable::ClearLevel(int level) {
	symbolMapVector[level] = new SymbolTable();
}

SymbolTable* CheckTable::GetSymbolMap(int level) {
	return symbolMapVector[level];
}