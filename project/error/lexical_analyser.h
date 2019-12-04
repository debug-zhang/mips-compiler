#pragma once

#include <fstream>
#include <list>
#include <map>
#include <string>
#include "error_handing.h"

using namespace std;

const string IDENFR		= "IDENFR";
const string INTCON		= "INTCON";
const string CHARCON	= "CHARCON";
const string STRCON		= "STRCON";
const string CONSTTK	= "CONSTTK";
const string INTTK		= "INTTK";
const string CHARTK		= "CHARTK";
const string VOIDTK		= "VOIDTK";
const string MAINTK		= "MAINTK";
const string IFTK		= "IFTK";
const string ELSETK		= "ELSETK";
const string DOTK		= "DOTK";
const string WHILETK	= "WHILETK";
const string FORTK		= "FORTK";
const string SCANFTK	= "SCANFTK";
const string PRINTFTK	= "PRINTFTK";
const string RETURNTK	= "RETURNTK";
const string PLUS		= "PLUS";
const string MINU		= "MINU";
const string MULT		= "MULT";
const string DIV		= "DIV";
const string LSS		= "LSS";
const string LEQ		= "LEQ";
const string GRE		= "GRE";
const string GEQ		= "GEQ";
const string EQL		= "EQL";
const string NEQ		= "NEQ";
const string ASSIGN		= "ASSIGN";
const string SEMICN		= "SEMICN";
const string COMMA		= "COMMA";
const string LPARENT	= "LPARENT";
const string RPARENT	= "RPARENT";
const string LBRACK		= "LBRACK";
const string RBRACK		= "RBRACK";
const string LBRACE		= "LBRACE";
const string RBRACE		= "RBRACE";

struct Lexeme {
	string identifier;
	string value;
	bool isError;
	int lineNumber;
	Lexeme() {
		isError = false;
		lineNumber = 0;
	}
};

class LexicalAnalyser {
private:
	ifstream testfile;
	struct Lexeme* tempLexeme;
	list<struct Lexeme> lexList;
	ErrorHanding* errorHanding;
	int lineNumber;
	map<string, string> keyMap;
	map<string, string> operaMap;

	bool IsUnder(char c);
	bool IsEqu(char c);
	bool IsSingleQuote(char c);
	bool IsDoubleQuote(char c);
	bool IsOpera(char c);
	bool isCharLetter(char c);
	bool isStringLetter(char c);

	bool CheckKey(struct Lexeme* temp);
	bool CheckSymbol(struct Lexeme* temp);

	void MapInitial();
	void GetChar(char& c);
	void UngetChar();
	void GoForward(char& c);
	void AddList(struct Lexeme temp);
	void AddList(struct Lexeme temp, string identifier);

	bool AnalyzeKey(char& c);
	bool AnalyzeQuote(char& c);
	bool AnalyzeOpera(char& c);
	bool AnalyzeDigit(char& c);

public:
	LexicalAnalyser(string fileName, ErrorHanding* errorHanding);
	list<struct Lexeme>* Analyze();
	void FileClose();
};

