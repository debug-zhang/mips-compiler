#include "error.h"

Error::Error() {
	line_number_ = 0;
	error_type_ = NO_ERROR;
}

Error::Error(int lineNumber, char errorType) {
	this->line_number_ = lineNumber;
	this->error_type_ = errorType;
}

void Error::set_line_number(int lineNumber) {
	this->line_number_ = lineNumber;
}

void Error::set_error_type(char errorType) {
	this->error_type_ = errorType;
}

bool Error::IsError() {
	return error_type_ != NO_ERROR;
}

int Error::line_number() {
	return line_number_;
}

char Error::error_type() {
	return error_type_;
}