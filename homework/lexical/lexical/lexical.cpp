#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

string key[] = { "const", "int",   "char", "void",  "main",   "if",    "else",
				"do",    "while", "for",  "scanf", "printf", "return" };

string keyCode[] = { "CONSTTK", "INTTK",    "CHARTK",  "VOIDTK",  "MAINTK",
					"IFTK",    "ELSETK",   "DOTK",    "WHILETK", "FORTK",
					"SCANFTK", "PRINTFTK", "RETURNTK" };

string symbol[] = { "+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!=",
				   "=", ";", ",", "(", ")", "[",  "]", "{",  "}" };

string symbolCode[] = { "PLUS",   "MINU",   "MULT",   "DIV",     "LSS",
					   "LEQ",    "GRE",    "GEQ",    "EQL",     "NEQ",
					   "ASSIGN", "SEMICN", "COMMA",  "LPARENT", "RPARENT",
					   "LBRACK", "RBRACK", "LBRACE", "RBRACE" };

char opera[] = { '+', '-', '*', '/', '<', '>', '=', ';', '!',
				',', '(', ')', '[', ']', '{', '}' };

char buf[1000];

int length = 0;
int keyLength = 13;
int symbolLength = 19;
int operaLength = 16;

bool isLetter(char c) {
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
		return true;
	}
	else {
		return false;
	}
}

bool isDigit(char c) {
	if (c >= '0' && c <= '9') {
		return true;
	}
	else {
		return false;
	}
}

bool isUnder(char c) {
	if (c == '_') {
		return true;
	}
	else {
		return false;
	}
}

bool isEqu(char c) {
	if (c == '=') {
		return true;
	}
	else {
		return false;
	}
}

bool isSingleQuote(char c) {
	if (c == '\'') {
		return true;
	}
	else {
		return false;
	}
}

bool isDoubleQuote(char c) {
	if (c == '"') {
		return true;
	}
	else {
		return false;
	}
}

bool isOpera(char c) {
	for (int i = 0; i < operaLength; i++) {
		if (c == opera[i]) {
			return true;
		}
	}
	return false;
}

bool isOther(char c) {
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
		return true;
	}
	else {
		return false;
	}
}

int checkKey() {
	string word = buf;
	for (int i = 0; i < keyLength; i++) {
		if (!word.compare(key[i])) {
			memset(buf, 0, sizeof(buf));
			length = 0;
			return i;
		}
	}
	return -1;
}

int checkSymbol() {
	string word = buf;
	for (int i = 0; i < symbolLength; i++) {
		if (!word.compare(symbol[i])) {
			memset(buf, 0, sizeof(buf));
			length = 0;
			return i;
		}
	}
	return -1;
}

void writeBuf(ofstream& output, string type) {
	string idenfr = buf;
	output << type
		<< " " << idenfr << endl;
	memset(buf, 0, sizeof(buf));
	length = 0;
	return;
}

int main() {
	char c;

	ifstream testfile("testfile.txt");
	ofstream output("output.txt");

	while ((c = testfile.get()) != -1) {

		while (isOther(c)) {
			c = testfile.get();
		}
		if (isLetter(c) || isUnder(c)) {
			buf[length++] = c;
			c = testfile.get();
			while (isLetter(c) || isUnder(c) || isDigit(c)) {
				buf[length++] = c;
				c = testfile.get();
			}
			testfile.unget();
		
			int value = checkKey();
			if (value != -1) {
				output << keyCode[value] << " " << key[value] << endl;
				continue;
			}
			else {
				writeBuf(output, "IDENFR");
				continue;
			}

		}


		while (isOther(c)) {
			c = testfile.get();
		}
		if (isSingleQuote(c)) {
			c = testfile.get();
			output << "CHARCON"
				<< " " << c << endl;
			c = testfile.get();
			continue;
		}
		else if (isDoubleQuote(c)) {
			char strcon[1000] = {};
			int length = 0;
			c = testfile.get();
			while (!isDoubleQuote(c)) {
				strcon[length++] = c;
				c = testfile.get();
			}
			strcon[length] = '\0';
			output << "STRCON"
				<< " " << strcon << endl;
		}


		while (isOther(c)) {
			c = testfile.get();
		}
		if (isOpera(c)) {
			buf[length++] = c;
			c = testfile.get();
			if (isEqu(c)) {
				buf[length++] = c;
			}
			else {
				testfile.unget();
			}

			int value = checkSymbol();
			if (value != -1) {
				output << symbolCode[value] << " " << symbol[value] << endl;
				continue;
			}
			else {
				writeBuf(output, "IDENFR");
				continue;
			}
		}


		while (isOther(c)) {
			c = testfile.get();
		}
		if (isDigit(c)) {
			buf[length++] = c;
			c = testfile.get();
			while (isDigit(c)) {
				buf[length++] = c;
				c = testfile.get();
			}
			testfile.unget();
			writeBuf(output, "INTCON");
		}
	}

	testfile.close();
	output.close();
	return 0;
}