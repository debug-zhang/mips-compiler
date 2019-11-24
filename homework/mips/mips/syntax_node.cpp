#include "syntax_node.h"

SyntaxNode::SyntaxNode() {
}

SyntaxNode::SyntaxNode(string syntax_identifier) {
	this->syntax_identifier_ = syntax_identifier;
}

SyntaxNode::SyntaxNode(string lexeme_identifier, string lexeme_value) {
	this->lexeme_identifier_ = lexeme_identifier;
	this->lexeme_value_ = lexeme_value;
}

void SyntaxNode::set_syntax_identifier(string syntax_identifier) {
	this->syntax_identifier_ = syntax_identifier;
}

void SyntaxNode::set_value(string value) {
	this->value_ = value;
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