#include "table.h"
#include <utility>

using namespace std;

// StringTable
StringTable::StringTable() {
    string_number_ = 0;
}

int StringTable::AddString(const string &str) {
    string_map_.insert(pair<int, string>(string_number_, str));
    return string_number_++;
}

int StringTable::GetStringCount() const {
    return string_number_;
}

string StringTable::GetString(int string_number) {
    auto iter = string_map_.find(string_number);

    if (iter == string_map_.end()) {
        return "";
    } else {
        return iter->second;
    }
}


// SymbolTable
SymbolTable::SymbolTable() = default;

Symbol *SymbolTable::AddSymbol(const string &name, KindSymbol kind, TypeSymbol type) {
    Symbol *symbol = FindSymbol(name);
    if (symbol != nullptr) {
        symbol->SetProperty(name, kind, type);
    } else {
        symbol = new Symbol();
        symbol->SetProperty(name, kind, type);
        symbol_map_.insert(pair<string, Symbol *>(name, symbol));
    }
    return symbol;
}

Symbol *SymbolTable::FindSymbol(const string &name) {
    auto iter = symbol_map_.find(name);

    if (iter == symbol_map_.end()) {
        return nullptr;
    } else {
        return iter->second;
    }
}

map<string, Symbol *> SymbolTable::symbol_map() {
    return symbol_map_;
}


// CheckTable
CheckTable::CheckTable() {
    auto map0 = new SymbolTable();
    auto map1 = new SymbolTable();
    symbol_table_vector_.push_back(map0);
    symbol_table_vector_.push_back(map1);
}

Symbol *CheckTable::AddSymbol(const string &name, KindSymbol kind, TypeSymbol type, int level) {
    return symbol_table_vector_[level]->AddSymbol(name, kind, type);
}

Symbol *CheckTable::FindSymbol(const string &name) {
    Symbol *symbol;
    symbol = symbol_table_vector_[1]->FindSymbol(name);
    if (symbol == nullptr) {
        symbol = symbol_table_vector_[0]->FindSymbol(name);
    }
    return symbol;
}

Symbol *CheckTable::FindSymbol(const string &name, int level) {
    return symbol_table_vector_[level]->FindSymbol(name);
}

int CheckTable::GetSymbolLevel(const string &name) {
    if (FindSymbol(name, 1) != nullptr) {
        return 1;
    } else {
        return 0;
    }
}

void CheckTable::ClearLevel(int level) {
    symbol_table_vector_[level] = new SymbolTable();
}

void CheckTable::SetTable(int level, SymbolTable *symbol_table) {
    symbol_table_vector_[level] = symbol_table;
}

SymbolTable *CheckTable::GetSymbolTable(int level) {
    return symbol_table_vector_[level];
}

map<string, Symbol *> CheckTable::GetSymbolMap(int level) {
    return GetSymbolTable(level)->symbol_map();
}

void CheckTable::AddFunctionVariableNumber(const string &function_name, int number) {
    function_variable.insert(pair<string, int>(function_name, number));
}

int CheckTable::GetFunctionVariableNumber(const string &function_name) {
    return function_variable.at(function_name);
}