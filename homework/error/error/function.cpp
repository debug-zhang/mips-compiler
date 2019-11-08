#include "function.h"

Function::Function() {
	this->name = "";
	this->property = ILLEGAL_FUNCTION;
	this->returnType = ILLEGAL_NO_RETURN_TYPE;
}

Function::Function(string name, FUNCTION_PROPERTY property) {
	this->name = name;
	this->property = property;
	this->returnType = ILLEGAL_NO_RETURN_TYPE;
}

void Function::SetName(string name) {
	this->name = name;
}

void Function::SetProperty(FUNCTION_PROPERTY property) {
	this->property = property;
}

void Function::SetReturnType(FUNCTION_RETURN_TYPE returnType) {
	this->returnType = returnType;
}

void Function::AddParamList(string type) {
	this->paramList.push_back(type);
}

string Function::GetName() {
	return this->name;
}

FUNCTION_PROPERTY Function::GetProperty() {
	return this->property;
}

FUNCTION_RETURN_TYPE Function::GetReturnType() {
	return this->returnType;
}

list<string> Function::GetParamList() {
	return this->paramList;
}
int Function::GetParamNumber() {
	return this->paramList.size();
}