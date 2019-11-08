#pragma once

#include <string>
#include <list>

using namespace std;

enum FUNCTION_PROPERTY {
	VOID_FUNCTION,
	INT_FUNCTION,
	CHAR_FUNCTION,
	ILLEGAL_FUNCTION
};

enum FUNCTION_RETURN_TYPE {
	RETURN_VOID_TYPE,
	RETURN_INT_TYPE,
	RETURN_CHAR_TYPE,
	ILLEGAL_NO_RETURN_TYPE
};

class Function {
private:
	string name;
	FUNCTION_PROPERTY property;
	FUNCTION_RETURN_TYPE returnType;
	list<string> paramList;

public:
	Function();
	Function(string name, FUNCTION_PROPERTY property);

	void SetName(string name);
	void SetProperty(FUNCTION_PROPERTY property);
	void SetReturnType(FUNCTION_RETURN_TYPE returnType);

	void AddParamList(string type);

	string GetName();
	FUNCTION_PROPERTY GetProperty();
	FUNCTION_RETURN_TYPE GetReturnType();
	list<string> GetParamList();
	int GetParamNumber();
};
