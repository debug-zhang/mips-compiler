#pragma once

const char ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY = 'a';
const char REDEFINITION = 'b';
const char UNDEFINED = 'c';
const char FUNCTION_PARAMETER_NUMBER_DONT_MATCH = 'd';
const char FUNCTION_PARAMETER_TYPE_DONT_MATCH = 'e';
const char ILLEGAL_TYPE_IN_IF = 'f';
const char RETURN_IN_NO_RETURN_FUNCTION = 'g';
const char NO_RETURN_IN_RETURN_FUNCTION = 'h';
const char ILLEGAL_ARRAY_INDEX = 'i';
const char ASSIGN_TO_CONST = 'j';
const char MISSING_SEMICN = 'k';
const char MISSING_RPARENT = 'l';
const char MISSING_RBRACK = 'm';
const char MISSING_WHILE_IN_DO_WHILE = 'n';
const char DEFINE_CONST_OTHERS = 'o';
const char NO_ERROR = 'p';

class Error {
private:
	int lineNumber;
	char errorType;
public:
	Error();
	Error(int lineNumber, char errorType);
	void SetLineNumber(int lineNumber);
	void SetErrorType(char errorType);
	bool IsError();
	int GetLineNumber();
	char GetErrorType();
};

