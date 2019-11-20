#include "syntax_node.h"

SyntaxNode::SyntaxNode() {
}

SyntaxNode::SyntaxNode(string synIdentifier) {
	this->synIdentifier = synIdentifier;
}

SyntaxNode::SyntaxNode(string lexIdentifier, string lexValue) {
	this->lexIdentifier = lexIdentifier;
	this->lexValue = lexValue;
}

void SyntaxNode::SetSynIdentifier(string synIdentifier) {
	this->synIdentifier = synIdentifier;
}

void SyntaxNode::SetNumericalValue(string numericalValue) {
	this->numericalValue = numericalValue;
}

bool SyntaxNode::IsLeaf() {
	return childList.empty();
}

bool SyntaxNode::IsLexEmpty() {
	return this->lexIdentifier.empty();
}

void SyntaxNode::AddChild(SyntaxNode* child) {
	childList.push_back(child);
}

string SyntaxNode::GetSynIdentifier() {
	return synIdentifier;
}

string SyntaxNode::GetLexIdentifier() {
	return lexIdentifier;
}

string SyntaxNode::GetLexValue() {
	return lexValue;
}

string SyntaxNode::GetNumericalValue() {
	return numericalValue;
}

string SyntaxNode::GetFirstChildNumericalValue() {
	return childList.front()->GetNumericalValue();
}

list<SyntaxNode*> SyntaxNode::GetChildList() {
	return childList;
}

void SyntaxNode::WriteLexical(ofstream& output) {
	output << GetLexIdentifier() << " " << GetLexValue() << endl;
}

void SyntaxNode::WriteSyntax(ofstream& output) {
	output << GetSynIdentifier() << endl;
}

void SyntaxNode::Print(ofstream& output) {
	if (IsLeaf()) {
		if (IsLexEmpty()) {
			WriteSyntax(output);
		} else {
			WriteLexical(output);
		}
	} else {
		list<SyntaxNode*> childList = GetChildList();
		list<SyntaxNode*>::iterator iter = childList.begin();

		while (iter != childList.end()) {
			(*iter)->Print(output);
			iter++;
		}

		if (IsLexEmpty()) {
			WriteSyntax(output);
		} else {
			WriteLexical(output);
		}
	}
}