#pragma once

#include <fstream>
#include <string>
#include <list>

using namespace std;

class SyntaxNode
{
private:
	string synIdentifier;
	string lexIdentifier;
	string lexValue;
	string numericalValue;
	list<SyntaxNode*> childList;
	void WriteLexical(ofstream& output);
	void WriteSyntax(ofstream& output);

public:
	SyntaxNode();
	SyntaxNode(string synIdentifier);
	SyntaxNode(string lexIdentifier, string lexValue);
	void SetSynIdentifier(string synIdentifier);
	void SetNumericalValue(string numericalValue);
	bool IsLeaf();
	bool IsLexEmpty();
	void AddChild(SyntaxNode* child);
	string GetSynIdentifier();
	string GetLexIdentifier();
	string GetLexValue();
	string GetNumericalValue();
	list<SyntaxNode*> GetChildList();
	void Print(ofstream& output);
};

