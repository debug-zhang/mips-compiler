#include "lexical_analyser.h"


const string key[] = { "const", "int",   "char", "void",  "main",   "if",    "else",
				"do",    "while", "for",  "scanf", "printf", "return" };

const string keyCode[] = { CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK,
					IFTK,    ELSETK,   DOTK,    WHILETK,FORTK,
					SCANFTK, PRINTFTK, RETURNTK };

const string symbol[] = { "+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!=",
				   "=", ";", ",", "(", ")", "[",  "]", "{",  "}" };

const string symbolCode[] = { PLUS,   MINU,   MULT,   DIV,     LSS,
					   LEQ,    GRE,    GEQ,    EQL,     NEQ,
					   ASSIGN, SEMICN, COMMA,  LPARENT, RPARENT,
					   LBRACK, RBRACK, LBRACE, RBRACE };

const char opera[] = { '+', '-', '*', '/', '<', '>', '=', ';', '!',
				',', '(', ')', '[', ']', '{', '}' };

const int keyLength = 13;
const int symbolLength = 19;
const int operaLength = 16;

LexicalAnalyser::LexicalAnalyser() {
	this->tempLexeme = new Lexeme();
}

bool LexicalAnalyser::IsLetter(char c) {
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
		return true;
	} else {
		return false;
	}
}

bool LexicalAnalyser::IsDigit(char c) {
	if (c >= '0' && c <= '9') {
		return true;
	} else {
		return false;
	}
}

bool LexicalAnalyser::IsUnder(char c) {
	if (c == '_') {
		return true;
	} else {
		return false;
	}
}

bool LexicalAnalyser::IsEqu(char c) {
	if (c == '=') {
		return true;
	} else {
		return false;
	}
}

bool LexicalAnalyser::IsSingleQuote(char c) {
	if (c == '\'') {
		return true;
	} else {
		return false;
	}
}

bool LexicalAnalyser::IsDoubleQuote(char c) {
	if (c == '"') {
		return true;
	} else {
		return false;
	}
}

bool LexicalAnalyser::IsOpera(char c) {
	for (int i = 0; i < operaLength; i++) {
		if (c == opera[i]) {
			return true;
		}
	}
	return false;
}

bool LexicalAnalyser::IsOther(char c) {
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
		return true;
	} else {
		return false;
	}
}

int LexicalAnalyser::CheckKey(struct Lexeme temp) {
	for (int i = 0; i < keyLength; i++) {
		if (!temp.value.compare(key[i])) {
			return i;
		}
	}
	return -1;
}

int LexicalAnalyser::CheckSymbol(struct Lexeme temp) {
	for (int i = 0; i < symbolLength; i++) {
		if (!temp.value.compare(symbol[i])) {
			return i;
		}
	}
	return -1;
}

void LexicalAnalyser::DelOther(char& c, ifstream& testfile) {
	while (IsOther(c)) {
		c = testfile.get();
	}
}

void LexicalAnalyser::AddList(struct Lexeme temp, string identifier) {
	temp.identifier = identifier;
	lexList.push_back(temp);
}

int LexicalAnalyser::AnalyzeKey(char& c, ifstream& testfile) {
	if (IsLetter(c) || IsUnder(c)) {
		tempLexeme = new Lexeme;
		do {
			tempLexeme->value.push_back(c);
			c = testfile.get();
		} while (IsLetter(c) || IsUnder(c) || IsDigit(c));
		testfile.unget();

		int value = CheckKey(*tempLexeme);
		if (value != -1) {
			AddList(*tempLexeme, keyCode[value]);
			return 1;
		} else {
			AddList(*tempLexeme, IDENFR);
			return 1;
		}

	}

	return 0;
}

int LexicalAnalyser::AnalyzeQuote(char& c, ifstream& testfile) {
	if (IsSingleQuote(c)) {
		tempLexeme = new Lexeme;
		c = testfile.get();
		tempLexeme->value.push_back(c);
		AddList(*tempLexeme, CHARCON);
		c = testfile.get();
		return 1;
	} else if (IsDoubleQuote(c)) {
		tempLexeme = new Lexeme;
		while (!IsDoubleQuote((c = testfile.get()))) {
			tempLexeme->value.push_back(c);
		}
		AddList(*tempLexeme, STRCON);
		return 1;
	}

	return 0;
}

int LexicalAnalyser::AnalyzeOpera(char& c, ifstream& testfile) {
	if (IsOpera(c)) {
		tempLexeme = new Lexeme;
		tempLexeme->value.push_back(c);

		if (c == '>' || c == '<' || c == '=' || c == '!') {
			c = testfile.get();
			if (IsEqu(c)) {
				tempLexeme->value.push_back(c);
			} else {
				testfile.unget();
			}
		}

		int value = CheckSymbol(*tempLexeme);
		if (value != -1) {
			AddList(*tempLexeme, symbolCode[value]);
			return 1;
		} else {
			AddList(*tempLexeme, INTCON);
			return 1;
		}
	}

	return 0;
}

int LexicalAnalyser::AnalyzeDigit(char& c, ifstream& testfile) {
	if (IsDigit(c)) {
		tempLexeme = new Lexeme;
		do {
			tempLexeme->value.push_back(c);
			c = testfile.get();
		} while (IsDigit(c));
		testfile.unget();
		AddList(*tempLexeme, INTCON);
		return 1;
	}

	return 0;
}

list<struct Lexeme> LexicalAnalyser::Analyze(ifstream& testfile) {
	char c;

	while ((c = testfile.get()) != -1) {
		DelOther(c, testfile);

		if (AnalyzeKey(c, testfile)
			|| AnalyzeQuote(c, testfile)
			|| AnalyzeOpera(c, testfile)
			|| AnalyzeDigit(c, testfile)) {
			continue;
		}
	}

	return lexList;
}