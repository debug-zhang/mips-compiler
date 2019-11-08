#pragma once

#include <string>
#include <map>
#include "identifier.h"
#include "function.h"

using namespace std;

class SymbolTable {
private:
	map<string, Identifier*> identifierMap;
	map<string, Function*> functionMap;
public:
	SymbolTable();

	void AddIdentifier(Identifier* identifier);
	void AddIdentifier(string name, IDENTIFIER_PROPERTY property, IDENTIFIER_TYPE type, int level);
	void AddFunction(Function* function);
	void AddFunction(string name, FUNCTION_PROPERTY property);

	Identifier* findIdentifier(string name);
	Function* findFunction(string name);

	void CleanLevelIdentifier(int level);
	void SetFunctionReturnType(string name, FUNCTION_RETURN_TYPE returnType);
};

