#pragma once

#include <string>

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

class Symbol {
private:
	string name;
	string parameter;
	string constValue;

	int arrayLength;
	int regNumber;
	int spOffer;

	KIND_SYMBOL kind;
	TYPE_SYMBOL type;

	bool isUse;

public:
	Symbol();

	void SetProperty(string name, KIND_SYMBOL kind, TYPE_SYMBOL type);
	string GetName();
	KIND_SYMBOL GetKind();
	TYPE_SYMBOL GetType();

	void AddParameter(char c);
	string GetParameter();
	int GetParameterCount();

	void SetConstValue(string constValue);
	string GetConstValue();

	void SetArrayLength(int arrayLength);
	int GetArrayLength();

	void SetRegNumber(int regNumber);
	int GetRegNumber();

	void SetSpOffer(int spOffer);
	int GetSpOffer();

	void SetUse(bool isUse);
	bool IsUse();
};

