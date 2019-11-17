#pragma once
#include <string>

using namespace std;

enum IDENTIFIER_PROPERTY {
	CONST_PROPERTY,
	VAR_PROPERTY,
	ILLEGAL_PROPERTY
};

enum IDENTIFIER_TYPE {
	INT_TYPE,
	CHAR_TYPE,
	ILLEGAL_TYPE
};

class Identifier {
private:
	string name;
	IDENTIFIER_PROPERTY property;
	IDENTIFIER_TYPE type;
	int level;

public:
	Identifier();
	Identifier(string name, IDENTIFIER_PROPERTY property, IDENTIFIER_TYPE type, int level);

	void SetName(string name);
	void SetProperty(IDENTIFIER_PROPERTY property);
	void SetType(IDENTIFIER_TYPE type);
	void SetLevel(int level);

	string GetName();
	IDENTIFIER_PROPERTY GetProperty();
	IDENTIFIER_TYPE GetType();
	int GetLevel();
};

