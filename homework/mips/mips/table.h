#pragma once

#include <string>
#include <map>
#include <vector>
#include "symbol.h"

using namespace std;

class StringTable {
private:
	map<int, string> stringMap;
	int stringNumber;
public:
	StringTable();
	int AddString(string str);
	int GetStringNumber();
	string GetString(int stringNumber);
};

class SymbolTable {
private:
	map<string, Symbol*> symbolMap;
public:
	SymbolTable();
	Symbol* AddSymbol(string name, KIND_SYMBOL kind, TYPE_SYMBOL type);
	Symbol* FindSymbol(string name);
};

class CheckTable {
private:
	vector<SymbolTable*> symbolMapVector;
public:
	CheckTable();
	Symbol* AddSymbol(string name, KIND_SYMBOL kind, TYPE_SYMBOL type, int level);
	Symbol* FindSymbol(string name, int level);
	void ClearLevel(int level);
	SymbolTable* GetSymbolMap(int level);
};