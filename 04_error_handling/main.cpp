#include "lexical_analyser.h"
#include "parse_analyser.h"
#include "error_handing.h"
#include <iostream>

using namespace std;

int main() {

	ErrorHanding* errorHanding = new ErrorHanding("error.txt");

	LexicalAnalyser* lexicalAnalyser = new LexicalAnalyser("testfile.txt", errorHanding);
	list<struct Lexeme>* lexList = lexicalAnalyser->Analyze();

	ParseAnalyser* parseAnalyser = new ParseAnalyser("output.txt", lexicalAnalyser->Analyze(), errorHanding);
	parseAnalyser->AnalyzeParse();

	errorHanding->PrintError();

	lexicalAnalyser->FileClose();
	parseAnalyser->FileClose();
	errorHanding->FileClose();

	return 0;
}