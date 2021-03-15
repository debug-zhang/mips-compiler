#pragma once

#include <fstream>
#include <string>
#include <list>

class SyntaxNode {
private:
    std::string syntax_identifier_;
    std::string lexeme_identifier_;
    std::string lexeme_value_;
    std::string value_;
    std::list<SyntaxNode *> child_list_;

    void WriteLexical(std::ofstream &output);

    void WriteSyntax(std::ofstream &output);

public:
    SyntaxNode();

    SyntaxNode(const std::string & syntax_identifier);

    SyntaxNode(const std::string & lexeme_identifier, const std::string & lexeme_value);

    void set_syntax_identifier(const std::string & syntax_identifier);

    void set_value(const std::string & value);

    bool IsLeaf();

    bool IsLexemeEmpty();

    void AddChild(SyntaxNode *child);

    std::string syntax_identifier();

    std::string lexeme_identifier();

    std::string lexeme_value();

    std::string value();

    std::string GetFirstChildNumericalValue();

    std::list<SyntaxNode *> GetChildList();

    void Print(std::ofstream &output);
};

