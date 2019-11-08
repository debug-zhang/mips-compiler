#pragma once

#include <fstream>
#include <list>
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
	int lineNumber;
	bool isError;
	Lexeme() {
		lineNumber = 0;
		isError = false;
	}
};

class LexicalAnalyser {
private:
	struct Lexeme* tempLexeme;
	list<struct Lexeme> lexList;
	ErrorHanding* errorHanding;

	bool IsLetter(char c);
	bool IsDigit(char c);
	bool IsUnder(char c);
	bool IsEqu(char c);
	bool IsSingleQuote(char c);
	bool IsDoubleQuote(char c);
	bool IsOpera(char c);
	bool IsOther(char c);

	int CheckKey(struct Lexeme temp);
	int CheckSymbol(struct Lexeme temp);

	void DelOther(char& c, ifstream& testfile);
	void AddList(struct Lexeme temp, string identifier);

	int AnalyzeKey(char& c, ifstream& testfile);
	int AnalyzeQuote(char& c, ifstream& testfile);
	int AnalyzeOpera(char& c, ifstream& testfile);
	int AnalyzeDigit(char& c, ifstream& testfile);

public:
	LexicalAnalyser(ErrorHanding* errorHanding);
	list<struct Lexeme> Analyze(ifstream& testfile);
};

