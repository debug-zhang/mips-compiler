#include "syntax_node.h"

SyntaxNode::SyntaxNode() {
}

SyntaxNode::SyntaxNode(string synIdentifier) {
	this->syntax_identifier_ = synIdentifier;
}

SyntaxNode::SyntaxNode(string lexIdentifier, string lexValue) {
	this->lexeme_identifier_ = lexIdentifier;
	this->lexeme_value_ = lexValue;
}

void SyntaxNode::set_syntax_identifier(string synIdentifier) {
	this->syntax_identifier_ = synIdentifier;
}

void SyntaxNode::set_value(string numericalValue) {
	this->value_ = numericalValue;
}

bool SyntaxNode::IsLeaf() {
	return child_list_.empty();
}

bool SyntaxNode::IsLexemeEmpty() {
	return this->lexeme_identifier_.empty();
}

void SyntaxNode::AddChild(SyntaxNode* child) {
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

list<SyntaxNode*> SyntaxNode::GetChildList() {
	return child_list_;
}

void SyntaxNode::WriteLexical(ofstream& output) {
	output << this->lexeme_identifier() << " " << lexeme_value() << endl;
}

void SyntaxNode::WriteSyntax(ofstream& output) {
	output << this->syntax_identifier() << endl;
}

void SyntaxNode::Print(ofstream& output) {
	if (this->IsLeaf()) {
		if (this->IsLexemeEmpty()) {
			this->WriteSyntax(output);
		} else {
			this->WriteLexical(output);
		}
	} else {
		list<SyntaxNode*> childList = this->GetChildList();
		list<SyntaxNode*>::iterator iter = childList.begin();

		while (iter != childList.end()) {
			(*iter)->Print(output);
			iter++;
		}

		if (this->IsLexemeEmpty()) {
			this->WriteSyntax(output);
		} else {
			this->WriteLexical(output);
		}
	}
}