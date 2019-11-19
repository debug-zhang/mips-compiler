#include "lexical_analyser.h"
#include "parse_analyser.h"
#include "mips_generator.h"
#include "error_handing.h"
#include <iostream>

using namespace std;

int main() {
	string testfile = "testfile.txt";
	string midcode = "midcode.txt";
	string mips = "mips.txt";
	string error = "error.txt";

	ErrorHanding* errorHanding = new ErrorHanding(error);

	LexicalAnalyser* lexicalAnalyser = new LexicalAnalyser(testfile, errorHanding);
	lexicalAnalyser->FileClose();

	ParseAnalyser* parseAnalyser = new ParseAnalyser(midcode, lexicalAnalyser->Analyze(), errorHanding);
	parseAnalyser->AnalyzeParse();
	parseAnalyser->FileClose();

	MipsGenerator* mipsGenerator = new MipsGenerator(midcode, mips, parseAnalyser->GetSymbolTableMap());
	mipsGenerator->FileClose();

	errorHanding->PrintError();
	errorHanding->FileClose();

	return 0;
}