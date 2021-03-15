#pragma once

#include <fstream>
#include <list>
#include <map>
#include <string>
#include "error_handing.h"


const std::string IDENFR = "IDENFR";
const std::string INTCON = "INTCON";
const std::string CHARCON = "CHARCON";
const std::string STRCON = "STRCON";
const std::string CONSTTK = "CONSTTK";
const std::string INTTK = "INTTK";
const std::string CHARTK = "CHARTK";
const std::string VOIDTK = "VOIDTK";
const std::string MAINTK = "MAINTK";
const std::string IFTK = "IFTK";
const std::string ELSETK = "ELSETK";
const std::string DOTK = "DOTK";
const std::string WHILETK = "WHILETK";
const std::string FORTK = "FORTK";
const std::string SCANFTK = "SCANFTK";
const std::string PRINTFTK = "PRINTFTK";
const std::string RETURNTK = "RETURNTK";
const std::string PLUS = "PLUS";
const std::string MINU = "MINU";
const std::string MULT = "MULT";
const std::string DIV = "DIV";
const std::string LSS = "LSS";
const std::string LEQ = "LEQ";
const std::string GRE = "GRE";
const std::string GEQ = "GEQ";
const std::string EQL = "EQL";
const std::string NEQ = "NEQ";
const std::string ASSIGN = "ASSIGN";
const std::string SEMICN = "SEMICN";
const std::string COMMA = "COMMA";
const std::string LPARENT = "LPARENT";
const std::string RPARENT = "RPARENT";
const std::string LBRACK = "LBRACK";
const std::string RBRACK = "RBRACK";
const std::string LBRACE = "LBRACE";
const std::string RBRACE = "RBRACE";

struct Lexeme {
    std::string identifier;
    std::string value;
    bool is_error;
    int line_number;

    Lexeme() {
        is_error = false;
        line_number = 0;
    }
};

class LexicalAnalyser {
private:
    std::ifstream testfile_;
    struct Lexeme *temp_lexeme_;
    std::list<struct Lexeme> lexeme_list_;
    ErrorHanding *error_handing_;
    int line_number_;
    std::map<std::string, std::string> key_map_;
    std::map<std::string, std::string> opera_map_;

    static bool IsUnder(char c);

    static bool IsEqu(char c);

    static bool IsSingleQuote(char c);

    static bool IsDoubleQuote(char c);

    bool IsOpera(char c);

    static bool IsCharLetter(char c);

    static bool IsStringLetter(char c);

    bool CheckKey(struct Lexeme *temp);

    bool CheckSymbol(struct Lexeme *temp);

    void MapInitial();

    void GetChar(char &c);

    void UngetChar();

    void GoForward(char &c);

    void AddList(struct Lexeme temp);

    void AddList(struct Lexeme temp, const std::string &identifier);

    bool AnalyzeKey(char &c);

    bool AnalyzeQuote(char &c);

    bool AnalyzeOpera(char &c);

    bool AnalyzeDigit(char &c);

public:
    LexicalAnalyser(const std::string &file_name, ErrorHanding *error_handing);

    std::list<struct Lexeme> *Analyze();

    void FileClose();
};

