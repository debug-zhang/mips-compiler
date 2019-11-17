#include "lexical_analyser.h"

LexicalAnalyser::LexicalAnalyser(string fileName, ErrorHanding* errorHanding) {
	this->testfile.open(fileName);
	this->tempLexeme = new Lexeme();
	this->errorHanding = errorHanding;
	this->lineNumber = 1;
	MapInitial();
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
	string str;
	str.clear();
	str.append(1, c);

	map<string, string>::iterator iter = operaMap.find(str);

	if (iter == operaMap.end()) {
		return false;
	} else {
		return true;
	}
}

bool LexicalAnalyser::isCharLetter(char c) {
	if (c == '+' || c == '-' || c == '*' || c == '/' || c== '_' || isalnum(c)) {
		return true;
	}
	return false;
}

bool LexicalAnalyser::isStringLetter(char c) {
	if (c >= 32 && c <= 126 && c != 34) {
		return true;
	}
	return false;
}

bool LexicalAnalyser::CheckKey(struct Lexeme* temp) {

	map<string, string>::iterator iter = keyMap.find(temp->value);

	if (iter == keyMap.end()) {
		return false;
	} else {
		temp->identifier = iter->second;
		return true;
	}
}

bool LexicalAnalyser::CheckSymbol(struct Lexeme* temp) {
	map<string, string>::iterator iter = operaMap.find(temp->value);

	if (iter == operaMap.end()) {
		return false;
	} else {
		temp->identifier = iter->second;
		return true;
	}
}

void LexicalAnalyser::MapInitial() {
	keyMap.insert(pair<string, string>("const", CONSTTK));
	keyMap.insert(pair<string, string>("int", INTTK));
	keyMap.insert(pair<string, string>("char", CHARTK));
	keyMap.insert(pair<string, string>("void", VOIDTK));
	keyMap.insert(pair<string, string>("main", MAINTK));
	keyMap.insert(pair<string, string>("if", IFTK));
	keyMap.insert(pair<string, string>("else", ELSETK));
	keyMap.insert(pair<string, string>("do", DOTK));
	keyMap.insert(pair<string, string>("while", WHILETK));
	keyMap.insert(pair<string, string>("for", FORTK));
	keyMap.insert(pair<string, string>("scanf", SCANFTK));
	keyMap.insert(pair<string, string>("printf", PRINTFTK));
	keyMap.insert(pair<string, string>("return", RETURNTK));

	operaMap.insert(pair<string, string>("+", PLUS));
	operaMap.insert(pair<string, string>("-", MINU));
	operaMap.insert(pair<string, string>("*", MULT));
	operaMap.insert(pair<string, string>("/", DIV));
	operaMap.insert(pair<string, string>("<", LSS));
	operaMap.insert(pair<string, string>("<=", LEQ));
	operaMap.insert(pair<string, string>(">", GRE));
	operaMap.insert(pair<string, string>(">=", GEQ));
	operaMap.insert(pair<string, string>("=", ASSIGN));
	operaMap.insert(pair<string, string>("==", EQL));
	operaMap.insert(pair<string, string>("!=", NEQ));
	operaMap.insert(pair<string, string>(";", SEMICN));
	operaMap.insert(pair<string, string>(",", COMMA));
	operaMap.insert(pair<string, string>("(", LPARENT));
	operaMap.insert(pair<string, string>(")", RPARENT));
	operaMap.insert(pair<string, string>("[", LBRACK));
	operaMap.insert(pair<string, string>("]", RBRACK));
	operaMap.insert(pair<string, string>("{", LBRACE));
	operaMap.insert(pair<string, string>("}", RBRACE));
}

void LexicalAnalyser::GetChar(char& c) {
	c = testfile.get();
}

void LexicalAnalyser::UngetChar() {
	testfile.unget();
}

void LexicalAnalyser::GoForward(char& c) {
	while (isspace(c)) {
		if (c == '\n') {
			this->lineNumber++;
		}
		GetChar(c);
	}
}

void LexicalAnalyser::AddList(struct Lexeme temp) {
	temp.lineNumber = this->lineNumber;
	lexList.push_back(temp);
}

void LexicalAnalyser::AddList(struct Lexeme temp, string identifier) {
	temp.lineNumber = this->lineNumber;
	temp.identifier = identifier;
	lexList.push_back(temp);
}

bool LexicalAnalyser::AnalyzeKey(char& c) {
	if (isalpha(c) || IsUnder(c)) {
		tempLexeme = new Lexeme();
		do {
			tempLexeme->value.push_back(c);
			GetChar(c);
		} while (isalpha(c) || IsUnder(c) || isdigit(c));
		UngetChar();

		if (CheckKey(tempLexeme)) {
			AddList(*tempLexeme);
			return true;
		} else {
			AddList(*tempLexeme, IDENFR);
			return true;
		}
	}

	return false;
}

bool LexicalAnalyser::AnalyzeQuote(char& c) {
	if (IsSingleQuote(c)) {
		tempLexeme = new Lexeme();
		GetChar(c);
		if (!isCharLetter(c)) {
			errorHanding->AddError(lineNumber, ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY);
		}
		tempLexeme->value.push_back(c);
		GetChar(c);
		if (!IsSingleQuote(c)) {
			UngetChar();
			errorHanding->AddError(lineNumber, ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY);
		}
		AddList(*tempLexeme, CHARCON);
		return true;
	} else if (IsDoubleQuote(c)) {
		tempLexeme = new Lexeme();
		while (true) {
			GetChar(c);
			if (IsDoubleQuote(c)) {
				break;
			}
			if (!isStringLetter(c)) {
				errorHanding->AddError(lineNumber, ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY);
			}
			tempLexeme->value.push_back(c);
		}
		AddList(*tempLexeme, STRCON);
		return true;
	}

	return false;
}

bool LexicalAnalyser::AnalyzeOpera(char& c) {
	if (IsOpera(c) || c == '!') {
		tempLexeme = new Lexeme();
		tempLexeme->value.push_back(c);

		if (c == '>' || c == '<' || c == '=') {
			GetChar(c);
			if (IsEqu(c)) {
				tempLexeme->value.push_back(c);
			} else {
				UngetChar();
			}
		} else if (c == '!') {
			GetChar(c);
			if (IsEqu(c)) {
				tempLexeme->value.push_back(c);
			} else {
				UngetChar();
				tempLexeme->value.push_back('=');
				errorHanding->AddError(lineNumber, ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY);
			}
			tempLexeme->isError = true;
		}

		if (CheckSymbol(tempLexeme)) {
			AddList(*tempLexeme);
			return true;
		}
	}

	return false;
}

bool LexicalAnalyser::AnalyzeDigit(char& c) {
	if (isdigit(c)) {
		tempLexeme = new Lexeme();
		do {
			tempLexeme->value.push_back(c);
			GetChar(c);
		} while (isdigit(c));
		UngetChar();
		if (tempLexeme->value[0] == '0' && tempLexeme->value.size() > 1) {
			errorHanding->AddError(lineNumber, ILLEGAL_SYMBOL_OR_LEXICAL_INCONFORMITY);
		}
		AddList(*tempLexeme, INTCON);
		return true;
	}

	return false;
}

list<struct Lexeme>* LexicalAnalyser::Analyze() {
	char c;

	while ((c = testfile.get()) != -1) {
		GoForward(c);

		if (AnalyzeKey(c)
			|| AnalyzeQuote(c)
			|| AnalyzeOpera(c)
			|| AnalyzeDigit(c)) {
			continue;
		}
	}

	return &lexList;
}

void LexicalAnalyser::FileClose() {
	testfile.close();
}