#include "lexical_analyser.h"

using namespace std;

LexicalAnalyser::LexicalAnalyser(const string &file_name, ErrorHanding *error_handing) {
    testfile_.open(file_name);
    temp_lexeme_ = new Lexeme();
    error_handing_ = error_handing;
    line_number_ = 1;
    MapInitial();
}

bool LexicalAnalyser::IsUnder(char c) {
    if (c == '_') {
        return true;
    } else {
        return false;
    }
}

bool LexicalAnalyser::IsEqu(char c) {
    if (c == '=') {
        return true;
    } else {
        return false;
    }
}

bool LexicalAnalyser::IsSingleQuote(char c) {
    if (c == '\'') {
        return true;
    } else {
        return false;
    }
}

bool LexicalAnalyser::IsDoubleQuote(char c) {
    if (c == '"') {
        return true;
    } else {
        return false;
    }
}

bool LexicalAnalyser::IsOpera(char c) {
    string str;
    str.clear();
    str.append(1, c);

    auto iter = opera_map_.find(str);

    if (iter == opera_map_.end()) {
        return false;
    } else {
        return true;
    }
}

bool LexicalAnalyser::IsCharLetter(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '_' || isalnum(c)) {
        return true;
    }
    return false;
}

bool LexicalAnalyser::IsStringLetter(char c) {
    if (c >= 32 && c <= 126 && c != 34) {
        return true;
    }
    return false;
}

bool LexicalAnalyser::CheckKey(struct Lexeme *temp) {

    auto iter = key_map_.find(temp->value);

    if (iter == key_map_.end()) {
        return false;
    } else {
        temp->identifier = iter->second;
        return true;
    }
}

bool LexicalAnalyser::CheckSymbol(struct Lexeme *temp) {
    auto iter = opera_map_.find(temp->value);

    if (iter == opera_map_.end()) {
        return false;
    } else {
        temp->identifier = iter->second;
        return true;
    }
}

void LexicalAnalyser::MapInitial() {
    key_map_.insert(pair<string, string>("const", CONSTTK));
    key_map_.insert(pair<string, string>("int", INTTK));
    key_map_.insert(pair<string, string>("char", CHARTK));
    key_map_.insert(pair<string, string>("void", VOIDTK));
    key_map_.insert(pair<string, string>("main", MAINTK));
    key_map_.insert(pair<string, string>("if", IFTK));
    key_map_.insert(pair<string, string>("else", ELSETK));
    key_map_.insert(pair<string, string>("do", DOTK));
    key_map_.insert(pair<string, string>("while", WHILETK));
    key_map_.insert(pair<string, string>("for", FORTK));
    key_map_.insert(pair<string, string>("scanf", SCANFTK));
    key_map_.insert(pair<string, string>("printf", PRINTFTK));
    key_map_.insert(pair<string, string>("return", RETURNTK));

    opera_map_.insert(pair<string, string>("+", PLUS));
    opera_map_.insert(pair<string, string>("-", MINU));
    opera_map_.insert(pair<string, string>("*", MULT));
    opera_map_.insert(pair<string, string>("/", DIV));
    opera_map_.insert(pair<string, string>("<", LSS));
    opera_map_.insert(pair<string, string>("<=", LEQ));
    opera_map_.insert(pair<string, string>(">", GRE));
    opera_map_.insert(pair<string, string>(">=", GEQ));
    opera_map_.insert(pair<string, string>("=", ASSIGN));
    opera_map_.insert(pair<string, string>("==", EQL));
    opera_map_.insert(pair<string, string>("!=", NEQ));
    opera_map_.insert(pair<string, string>(";", SEMICN));
    opera_map_.insert(pair<string, string>(",", COMMA));
    opera_map_.insert(pair<string, string>("(", LPARENT));
    opera_map_.insert(pair<string, string>(")", RPARENT));
    opera_map_.insert(pair<string, string>("[", LBRACK));
    opera_map_.insert(pair<string, string>("]", RBRACK));
    opera_map_.insert(pair<string, string>("{", LBRACE));
    opera_map_.insert(pair<string, string>("}", RBRACE));
}

void LexicalAnalyser::GetChar(char &c) {
    c = testfile_.get();
}

void LexicalAnalyser::UngetChar() {
    testfile_.unget();
}

void LexicalAnalyser::GoForward(char &c) {
    while (isspace(c)) {
        if (c == '\n') {
            line_number_++;
        }
        GetChar(c);
    }
}

void LexicalAnalyser::AddList(struct Lexeme temp) {
    temp.line_number = line_number_;
    lexeme_list_.push_back(temp);
}

void LexicalAnalyser::AddList(struct Lexeme temp, const string &identifier) {
    temp.line_number = line_number_;
    temp.identifier = identifier;
    lexeme_list_.push_back(temp);
}

bool LexicalAnalyser::AnalyzeKey(char &c) {
    if (isalpha(c) || IsUnder(c)) {
        temp_lexeme_ = new Lexeme();
        do {
            temp_lexeme_->value.push_back(c);
            GetChar(c);
        } while (isalpha(c) || IsUnder(c) || isdigit(c));
        UngetChar();

        if (CheckKey(temp_lexeme_)) {
            AddList(*temp_lexeme_);
            return true;
        } else {
            AddList(*temp_lexeme_, IDENFR);
            return true;
        }
    }

    return false;
}

bool LexicalAnalyser::AnalyzeQuote(char &c) {
    if (IsSingleQuote(c)) {
        temp_lexeme_ = new Lexeme();
        GetChar(c);
        if (!IsCharLetter(c)) {
            error_handing_->AddError(line_number_, ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY);
        }
        temp_lexeme_->value.push_back(c);
        GetChar(c);
        if (!IsSingleQuote(c)) {
            UngetChar();
            error_handing_->AddError(line_number_, ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY);
        }
        AddList(*temp_lexeme_, CHARCON);
        return true;
    } else if (IsDoubleQuote(c)) {
        temp_lexeme_ = new Lexeme();
        while (true) {
            GetChar(c);
            if (IsDoubleQuote(c)) {
                break;
            }
            if (!IsStringLetter(c)) {
                error_handing_->AddError(line_number_, ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY);
            }
            temp_lexeme_->value.push_back(c);
            if (c == '\\') {
                temp_lexeme_->value.push_back(c);
            }
        }
        AddList(*temp_lexeme_, STRCON);
        return true;
    }

    return false;
}

bool LexicalAnalyser::AnalyzeOpera(char &c) {
    if (IsOpera(c) || c == '!') {
        temp_lexeme_ = new Lexeme();
        temp_lexeme_->value.push_back(c);

        if (c == '>' || c == '<' || c == '=') {
            GetChar(c);
            if (IsEqu(c)) {
                temp_lexeme_->value.push_back(c);
            } else {
                UngetChar();
            }
        } else if (c == '!') {
            GetChar(c);
            if (IsEqu(c)) {
                temp_lexeme_->value.push_back(c);
            } else {
                UngetChar();
                temp_lexeme_->value.push_back('=');
                error_handing_->AddError(line_number_, ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY);
            }
            temp_lexeme_->is_error = true;
        }

        if (CheckSymbol(temp_lexeme_)) {
            AddList(*temp_lexeme_);
            return true;
        }
    }

    return false;
}

bool LexicalAnalyser::AnalyzeDigit(char &c) {
    if (isdigit(c)) {
        temp_lexeme_ = new Lexeme();
        do {
            temp_lexeme_->value.push_back(c);
            GetChar(c);
        } while (isdigit(c));
        UngetChar();
        if (temp_lexeme_->value[0] == '0' && temp_lexeme_->value.size() > 1) {
            error_handing_->AddError(line_number_, ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY);
        }
        AddList(*temp_lexeme_, INTCON);
        return true;
    }

    return false;
}

list<struct Lexeme> *LexicalAnalyser::Analyze() {
    char c;

    while ((c = testfile_.get()) != -1) {
        GoForward(c);

        if (AnalyzeKey(c)
            || AnalyzeQuote(c)
            || AnalyzeOpera(c)
            || AnalyzeDigit(c)) {
            continue;
        }
    }

    return &lexeme_list_;
}

void LexicalAnalyser::FileClose() {
    testfile_.close();
}