#pragma once

#include <string>
#include <map>
#include <vector>
#include "symbol.h"

using namespace std;

class StringTable {
private:
	map<int, string> string_map_;
	int string_number_;
public:
	StringTable();
	int AddString(string str);
	int GetStringCount();
	string GetString(int string_number);
};

class SymbolTable {
private:
	map<string, Symbol*> symbol_map_;
public:
	SymbolTable();
	Symbol* AddSymbol(string name, KindSymbol kind, TypeSymbol type);
	Symbol* FindSymbol(string name);
	map<string, Symbol*> symbol_map();
};

class CheckTable {
private:
	map<string, int> function_variable;
	vector<SymbolTable*> symbol_table_vector_;
public:
	CheckTable();
	Symbol* AddSymbol(string name, KindSymbol kind, TypeSymbol type, int level);
	Symbol* FindSymbol(string name);
	Symbol* FindSymbol(string name, int level);
	int GetSymbolLevel(string name);
	void ClearLevel(int level);
	void SetTable(int level, SymbolTable* symbol_table);
	SymbolTable* GetSymbolTable(int level);
	map<string, Symbol*> GetSymbolMap(int level);
	void AddFunctionVariableNumber(string function_name, int number);
	int GetFunctionVariableNumber(string function_name);
};