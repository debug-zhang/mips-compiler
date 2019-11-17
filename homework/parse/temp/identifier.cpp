#include "identifier.h"

Identifier::Identifier() {
	this->name = "";
	this->property = ILLEGAL_PROPERTY;
	this->type = ILLEGAL_TYPE;
	this->level = 0;
}

Identifier::Identifier(string name, IDENTIFIER_PROPERTY property, IDENTIFIER_TYPE type, int level) {
	this->name = name;
	this->property = property;
	this->type = type;
	this->level = level;
}

void Identifier::SetName(string name) {
	this->name = name;
}

void Identifier::SetProperty(IDENTIFIER_PROPERTY property) {
	this->property = property;
}

void Identifier::SetType(IDENTIFIER_TYPE type) {
	this->type = type;
}

void Identifier::SetLevel(int level) {
	this->level = level;
}

string Identifier::GetName() {
	return this->name;
}

IDENTIFIER_PROPERTY Identifier::GetProperty() {
	return this->property;
}

IDENTIFIER_TYPE Identifier::GetType() {
	return this->type;
}

int Identifier::GetLevel() {
	return this->level;
}