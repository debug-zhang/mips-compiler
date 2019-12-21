#include "symbol.h"

Symbol::Symbol() {
	this->array_length_ = 0;
	this->reg_number_ = 0;
	this->offset_ = 0;
	this->kind_ = KindSymbol::CONST;
	this->type_ = TypeSymbol::INT;
};

void Symbol::SetProperty(string name, KindSymbol kind, TypeSymbol type) {
	this->name_ = name;
	this->kind_ = kind;
	this->type_ = type;
}

string Symbol::name() {
	return this->name_;
}

KindSymbol Symbol::kind() {
	return this->kind_;
}

void Symbol::set_kind(KindSymbol kind) {
	this->kind_ = kind;
}

TypeSymbol Symbol::type() {
	return this->type_;
}

void Symbol::AddParameter(char c) {
	this->parameter_.push_back(c);
}

string Symbol::parameter() {
	return this->parameter_;
}

int Symbol::GetParameterCount() {
	return this->parameter_.length();
}

void Symbol::set_const_value(string const_value) {
	this->const_value_ = const_value;
}

string Symbol::const_value() {
	return this->const_value_;
}

void Symbol::set_array_length(int array_length) {
	this->array_length_ = array_length;
}

int Symbol::array_length() {
	return this->array_length_;
}

void Symbol::set_reg_number(int reg_number) {
	this->reg_number_ = reg_number;
}

int Symbol::reg_number() {
	return this->reg_number_;
}

void Symbol::set_offset(int sp_offer) {
	this->offset_ = sp_offer;
}

int Symbol::offset() {
	return this->offset_;
}