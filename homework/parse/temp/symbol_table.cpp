#include "symbol_table.h"

SymbolTable::SymbolTable() {

}

void SymbolTable::AddIdentifier(Identifier* identifier) {
	this->identifierMap.insert(pair<string, Identifier*>(identifier->GetName(), identifier));
}

void SymbolTable::AddIdentifier(string name, IDENTIFIER_PROPERTY property, IDENTIFIER_TYPE type, int level) {
	this->identifierMap.insert(pair<string, Identifier*>(name, new Identifier(name, property, type, level)));
}

void SymbolTable::AddFunction(Function* function) {
	this->functionMap.insert(pair<string, Function*>(function->GetName(), function));
}

void SymbolTable::AddFunction(string name, FUNCTION_PROPERTY property) {
	this->functionMap.insert(pair<string, Function*>(name, new Function(name, property)));
}

Identifier* SymbolTable::findIdentifier(string name) {
	map<string, Identifier*>::iterator iter = identifierMap.find(name);

	if (iter == identifierMap.end()) {
		return NULL;
	} else {
		return iter->second;
	}
}

Function* SymbolTable::findFunction(string name) {
	map<string, Function*>::iterator iter = functionMap.find(name);

	if (iter == functionMap.end()) {
		return NULL;
	} else {
		return iter->second;
	}
}

void SymbolTable::CleanLevelIdentifier(int level) {
	map<string, Identifier*>::iterator iter = identifierMap.begin();

	while (iter != identifierMap.end()) {
		if (iter->second->GetLevel() >= level) {
			identifierMap.erase(iter);
		}
		iter++;
	}
}

void SymbolTable::SetFunctionReturnType(string name, FUNCTION_RETURN_TYPE returnType) {
	map<string, Function*>::iterator iter = functionMap.find(name);

	if (iter != functionMap.end()) {
		iter->second->SetReturnType(returnType);
	}
}
