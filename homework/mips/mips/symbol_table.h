#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

enum IDENTIFIER_KIND {
	CONST,
	VARIABLE,
	ARRAY,
	FUNCTION
};

enum IDENTIFIER_TYPE {
	INT,
	CHAR,
	VOID
};

struct Identifier {
	string name;
	IDENTIFIER_KIND kind;
	IDENTIFIER_TYPE type;
	string parameter;
	Identifier() {
		kind = CONST;
		type = INT;
	};
};

class SymbolTable {
private:
	vector<map<string, struct Identifier>> identifierMap;
public:
	SymbolTable();

	void AddIdentifier(string name, IDENTIFIER_KIND kind, IDENTIFIER_TYPE type, int level);

	struct Identifier* findIdentifier(string name, int level);

	void CleanLevel(int level);
};

class StringTable {
private:
	vector<map<string, string>> identifierMap;
public:
	SymbolTable();

	void AddIdentifier(string name, IDENTIFIER_KIND kind, IDENTIFIER_TYPE type, int level);

	struct Identifier* findIdentifier(string name, int level);

	void CleanLevel(int level);
};
