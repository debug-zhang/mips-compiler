#pragma once

#include <string>

enum class KindSymbol {
    CONST,
    VARIABLE,
    ARRAY,
    FUNCTION,
    PARAMETER
};

enum class TypeSymbol {
    INT,
    CHAR,
    VOID
};

class Symbol {
private:
    std::string name_;
    std::string parameter_;
    std::string const_value_;

    int array_length_;
    int reg_number_;
    int offset_;

    KindSymbol kind_;
    TypeSymbol type_;

public:
    Symbol();

    void SetProperty(const std::string &name, KindSymbol kind, TypeSymbol type);

    std::string name();

    KindSymbol kind();

    void set_kind(KindSymbol kind);

    TypeSymbol type();

    void AddParameter(char c);

    std::string parameter();

    int GetParameterCount();

    void set_const_value(const std::string &const_value);

    std::string const_value();

    void set_array_length(int array_length);

    int array_length() const;

    void set_reg_number(int reg_number);

    int reg_number() const;

    void set_offset(int sp_offer);

    int offset() const;

};

