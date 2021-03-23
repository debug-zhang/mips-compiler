#include "syntax_node.h"
#include <utility>

using namespace std;

SyntaxNode::SyntaxNode() = default;

SyntaxNode::SyntaxNode(const string &syntax_identifier) {
    syntax_identifier_ = syntax_identifier;
}

SyntaxNode::SyntaxNode(const string &lexeme_identifier, const string &lexeme_value) {
    lexeme_identifier_ = lexeme_identifier;
    lexeme_value_ = lexeme_value;
}

void SyntaxNode::set_syntax_identifier(const string &syntax_identifier) {
    syntax_identifier_ = syntax_identifier;
}

void SyntaxNode::set_value(const string &value) {
    value_ = value;
}

bool SyntaxNode::IsLeaf() {
    return child_list_.empty();
}

bool SyntaxNode::IsLexemeEmpty() {
    return lexeme_identifier_.empty();
}

void SyntaxNode::AddChild(SyntaxNode *child) {
    child_list_.push_back(child);
}

string SyntaxNode::syntax_identifier() {
    return syntax_identifier_;
}

string SyntaxNode::lexeme_identifier() {
    return lexeme_identifier_;
}

string SyntaxNode::lexeme_value() {
    return lexeme_value_;
}

string SyntaxNode::value() {
    return value_;
}

string SyntaxNode::GetFirstChildNumericalValue() {
    return child_list_.front()->value();
}

list<SyntaxNode *> SyntaxNode::GetChildList() {
    return child_list_;
}

void SyntaxNode::WriteLexical(ofstream &output) {
    output << lexeme_identifier() << " " << lexeme_value() << endl;
}

void SyntaxNode::WriteSyntax(ofstream &output) {
    output << syntax_identifier() << endl;
}

void SyntaxNode::Print(ofstream &output) {
    if (IsLeaf()) {
        if (IsLexemeEmpty()) {
            WriteSyntax(output);
        } else {
            WriteLexical(output);
        }
    } else {
        auto childList = GetChildList();
        auto iter = childList.begin();

        while (iter != childList.end()) {
            (*iter)->Print(output);
            iter++;
        }

        if (IsLexemeEmpty()) {
            WriteSyntax(output);
        } else {
            WriteLexical(output);
        }
    }
}