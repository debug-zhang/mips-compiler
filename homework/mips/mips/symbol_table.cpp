#include "table.h"

SymbolTable::SymbolTable() {
	map<string, struct Identifier> map0;
	map<string, struct Identifier> map1;
	this->identifierMap.push_back(map0);
	this->identifierMap.push_back(map1);
}

void SymbolTable::AddIdentifier(string name, IDENTIFIER_KIND kind, IDENTIFIER_TYPE type, int level) {
	struct Identifier* identifier = findIdentifier(name, level);
	if (identifier != NULL) {
		identifier->name = name;
		identifier->kind = kind;
		identifier->type = type;
	} else {
		struct Identifier identifier;
		identifier.name = name;
		identifier.kind = kind;
		identifier.type = type;
		identifierMap[level].insert(pair<string, struct Identifier>(name, identifier));
	}
}

struct Identifier* SymbolTable::findIdentifier(string name, int level) {
	map<string, struct Identifier>::iterator iter = identifierMap[level].find(name);

	if (iter == identifierMap[level].end()) {
		return NULL;
	} else {
		return &(iter->second);
	}
}

void SymbolTable::CleanLevel(int level) {
	identifierMap[level].clear();
}
