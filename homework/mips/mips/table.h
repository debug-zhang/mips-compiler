#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

enum KIND_SYMBOL {
	CONST,
	VARIABLE,
	ARRAY,
	FUNCTION,
	PARAMETER
};

enum TYPE_SYMBOL {
	INT,
	CHAR,
	VOID
};

struct Symbol {
	string name;
	string parameter;

	int regNumber;
	int spOffer;

	KIND_SYMBOL kind;
	TYPE_SYMBOL type;

	bool isUse;

	Symbol() {
		regNumber = 0;
		spOffer = 0;
		kind = CONST;
		type = INT;
		isUse = false;
	};

	void setProperty(string name, KIND_SYMBOL kind, TYPE_SYMBOL type) {
		this->name = name;
		this->kind = kind;
		this->type = type;
	}
};

class StringTable {
private:
	map<int, string> stringMap;
	int stringNumber;
public:
	StringTable();
	void AddString(string str);
	int GetStringNumber();
	string GetString(int stringNumber);
};

class SymbolTable {
private:
	map<string, struct Symbol> symbolMap;
public:
	SymbolTable();
	void AddSymbol(string name, KIND_SYMBOL kind, TYPE_SYMBOL type);
	struct Symbol* FindSymbol(string name);
};

class CheckTable {
private:
	vector<SymbolTable*> symbolMapVector;
public:
	CheckTable();
	void AddSymbol(string name, KIND_SYMBOL kind, TYPE_SYMBOL type, int level);
	struct Symbol* FindSymbol(string name, int level);
	string GetString(int stringNumber);
	void ClearLevel(int level);
	SymbolTable* GetSymbolMap(int level);
};