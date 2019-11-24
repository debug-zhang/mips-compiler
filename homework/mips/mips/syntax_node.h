#pragma once

#include <fstream>
#include <string>
#include <list>

using namespace std;

class SyntaxNode
{
private:
	string syntax_identifier_;
	string lexeme_identifier_;
	string lexeme_value_;
	string value_;
	list<SyntaxNode*> child_list_;

	void WriteLexical(ofstream& output);
	void WriteSyntax(ofstream& output);

public:
	SyntaxNode();
	SyntaxNode(string synIdentifier);
	SyntaxNode(string lexIdentifier, string lexValue);
	void set_syntax_identifier(string synIdentifier);
	void set_value(string numericalValue);
	bool IsLeaf();
	bool IsLexemeEmpty();
	void AddChild(SyntaxNode* child);
	string syntax_identifier();
	string lexeme_identifier();
	string lexeme_value();
	string value();
	string GetFirstChildNumericalValue();
	list<SyntaxNode*> GetChildList();
	void Print(ofstream& output);
};

