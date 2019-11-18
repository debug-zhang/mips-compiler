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

void SyntaxNode::setSynIdentifier(string synIdentifier) {
	this->synIdentifier = synIdentifier;
}

void SyntaxNode::setNumericalValue(string numericalValue) {
	this->numericalValue = numericalValue;
}

bool SyntaxNode::isLeaf() {
	return childList.empty();
}

bool SyntaxNode::isLexEmpty() {
	return this->lexIdentifier.empty();
}

void SyntaxNode::addChild(SyntaxNode* child) {
	childList.push_back(child);
}

string SyntaxNode::getSynIdentifier() {
	return synIdentifier;
}

string SyntaxNode::getLexIdentifier() {
	return lexIdentifier;
}

string SyntaxNode::getLexValue() {
	return lexValue;
}

string SyntaxNode::getNumericalValue() {
	return numericalValue;
}

list<SyntaxNode*> SyntaxNode::getChildList() {
	return childList;
}

void SyntaxNode::writeLexical(ofstream& output) {
	output << getLexIdentifier() << " " << getLexValue() << endl;
}

void SyntaxNode::writeSyntax(ofstream& output) {
	output << getSynIdentifier() << endl;
}

void SyntaxNode::print(ofstream& output) {
	if (isLeaf()) {
		if (isLexEmpty()) {
			writeSyntax(output);
		} else {
			writeLexical(output);
		}
	} else {
		list<SyntaxNode*> childList = getChildList();
		list<SyntaxNode*>::iterator iter = childList.begin();

		while (iter != childList.end()) {
			(*iter)->print(output);
			iter++;
		}

		if (isLexEmpty()) {
			writeSyntax(output);
		} else {
			writeLexical(output);
		}
	}
}