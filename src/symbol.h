#pragma once

#include <string>

using namespace std;

enum class KindSymbol {
	CONST,
	VARIABLE,
	ARRAY,
	FUNCTION,
	PARAMETER
};

enum class TypeSymbol {
	INT,
	CHAR,
	VOID
};

class Symbol {
private:
	string name_;
	string parameter_;
	string const_value_;

	int array_length_;
	int reg_number_;
	int offset_;

	KindSymbol kind_;
	TypeSymbol type_;

public:
	Symbol();

	void SetProperty(string name, KindSymbol kind, TypeSymbol type);

	string name();

	KindSymbol kind();
	void set_kind(KindSymbol kind);

	TypeSymbol type();

	void AddParameter(char c);
	string parameter();
	int GetParameterCount();

	void set_const_value(string const_value);
	string const_value();

	void set_array_length(int array_length);
	int array_length();

	void set_reg_number(int reg_number);
	int reg_number();

	void set_offset(int sp_offer);
	int offset();

};

