#include "symbol.h"

Symbol::Symbol() {
	this->regNumber = 0;
	this->spOffer = 0;
	this->kind = CONST;
	this->type = INT;
	this->isUse = false;
};

void Symbol::SetProperty(string name, KIND_SYMBOL kind, TYPE_SYMBOL type) {
	this->name = name;
	this->kind = kind;
	this->type = type;
}

string Symbol::GetName() {
	return this->name;
}

KIND_SYMBOL Symbol::GetKind() {
	return this->kind;
}

TYPE_SYMBOL Symbol::GetType() {
	return this->type;
}

void Symbol::AddParameter(char c) {
	this->parameter.push_back(c);
}
string Symbol::GetParameter() {
	return this->parameter;
}

int Symbol::GetParameterCount() {
	return this->parameter.length();
}

void Symbol::SetRegNumber(int regNumber) {
	this->regNumber = regNumber;
}

int Symbol::GetRegNumber() {
	return this->regNumber;
}

void Symbol::SetSpOffer(int spOffer) {
	this->spOffer = spOffer;
}

int Symbol::GetSpOffer() {
	return this->spOffer;
}

void Symbol::SetUse(bool isUse) {
	this->isUse = isUse;
}

bool Symbol::IsUse() {
	return isUse;
}