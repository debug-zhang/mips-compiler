#include "symbol.h"

#include <utility>

using namespace std;

Symbol::Symbol() {
    array_length_ = 0;
    reg_number_ = 0;
    offset_ = 0;
    kind_ = KindSymbol::CONST;
    type_ = TypeSymbol::INT;
}

void Symbol::SetProperty(const string &name, KindSymbol kind, TypeSymbol type) {
    name_ = name;
    kind_ = kind;
    type_ = type;
}

string Symbol::name() {
    return name_;
}

KindSymbol Symbol::kind() {
    return kind_;
}

void Symbol::set_kind(KindSymbol kind) {
    kind_ = kind;
}

TypeSymbol Symbol::type() {
    return type_;
}

void Symbol::AddParameter(char c) {
    parameter_.push_back(c);
}

string Symbol::parameter() {
    return parameter_;
}

int Symbol::GetParameterCount() {
    return parameter_.length();
}

void Symbol::set_const_value(const string &const_value) {
    const_value_ = const_value;
}

string Symbol::const_value() {
    return const_value_;
}

void Symbol::set_array_length(int array_length) {
    array_length_ = array_length;
}

int Symbol::array_length() const {
    return array_length_;
}

void Symbol::set_reg_number(int reg_number) {
    reg_number_ = reg_number;
}

int Symbol::reg_number() const {
    return reg_number_;
}

void Symbol::set_offset(int sp_offer) {
    offset_ = sp_offer;
}

int Symbol::offset() const {
    return offset_;
}