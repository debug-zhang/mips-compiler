#include "table.h"

// CheckTable
CheckTable::CheckTable() {
	map<string, struct Symbol> map0;
	map<string, struct Symbol> map1;
	this->symbolMap.push_back(map0);
	this->symbolMap.push_back(map1);
}

void CheckTable::AddSymbol(string name, KIND_SYMBOL kind, TYPE_SYMBOL type, int level) {
	struct Symbol* symbol = FindSymbol(name, level);
	if (symbol != NULL) {
		symbol->setProperty(name, kind, type);
	} else {
		struct Symbol symbol;
		symbol.setProperty(name, kind, type);
		symbolMap[level].insert(pair<string, struct Symbol>(name, symbol));
	}
}

struct Symbol* CheckTable::FindSymbol(string name, int level) {
	map<string, struct Symbol>::iterator iter = symbolMap[level].find(name);

	if (iter == symbolMap[level].end()) {
		return NULL;
	} else {
		return &(iter->second);
	}
}

void CheckTable::CleanLevel(int level) {
	symbolMap[level].clear();
}


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