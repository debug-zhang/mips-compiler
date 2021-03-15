#pragma once

#include <string>
#include <map>
#include <vector>
#include "symbol.h"

class StringTable {
private:
    std::map<int, std::string> string_map_;
    int string_number_;
public:
    StringTable();

    int AddString(const std::string &str);

    int GetStringCount() const;

    std::string GetString(int string_number);
};

class SymbolTable {
private:
    std::map<std::string, Symbol *> symbol_map_;
public:
    SymbolTable();

    Symbol *AddSymbol(const std::string &name, KindSymbol kind, TypeSymbol type);

    Symbol *FindSymbol(const std::string &name);

    std::map<std::string, Symbol *> symbol_map();
};

class CheckTable {
private:
    std::map<std::string, int> function_variable;
    std::vector<SymbolTable *> symbol_table_vector_;
public:
    CheckTable();

    Symbol *AddSymbol(const std::string &name, KindSymbol kind, TypeSymbol type, int level);

    Symbol *FindSymbol(const std::string &name);

    Symbol *FindSymbol(const std::string &name, int level);

    int GetSymbolLevel(const std::string &name);

    void ClearLevel(int level);

    void SetTable(int level, SymbolTable *symbol_table);

    SymbolTable *GetSymbolTable(int level);

    std::map<std::string, Symbol *> GetSymbolMap(int level);

    void AddFunctionVariableNumber(const std::string &function_name, int number);

    int GetFunctionVariableNumber(const std::string &function_name);
};