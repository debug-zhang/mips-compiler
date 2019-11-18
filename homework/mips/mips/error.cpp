#include "error.h"

Error::Error() {
	lineNumber = 0;
	errorType = NO_ERROR;
}

Error::Error(int lineNumber, char errorType) {
	this->lineNumber = lineNumber;
	this->errorType = errorType;
}

void Error::SetLineNumber(int lineNumber) {
	this->lineNumber = lineNumber;
}

void Error::SetErrorType(char errorType) {
	this->errorType = errorType;
}

bool Error::IsError() {
	return errorType != NO_ERROR;
}

int Error::GetLineNumber() {
	return lineNumber;
}

char Error::GetErrorType() {
	return errorType;
}