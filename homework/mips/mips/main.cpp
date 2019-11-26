﻿#include <iostream>
#include "lexical_analyser.h"
#include "parse_analyser.h"
#include "mips_generator.h"
#include "error_handing.h"

using namespace std;

int main() {
	string testfile = "testfile.txt";
	string midcode = "midcode.txt";
	string mips = "mips.txt";
	string error = "error.txt";

	ErrorHanding* error_handing = new ErrorHanding(error);

	LexicalAnalyser* lexical_analyser = new LexicalAnalyser(testfile, error_handing);

	ParseAnalyser* parse_analyser = new ParseAnalyser(midcode, 
		lexical_analyser->Analyze(), error_handing);
	lexical_analyser->FileClose();
	parse_analyser->AnalyzeParse();
	parse_analyser->FileClose();

	MipsGenerator* mips_generator = new MipsGenerator(midcode, mips,
		parse_analyser->string_table(), parse_analyser->check_table(),
		parse_analyser->symbol_table_map());
	mips_generator->GenerateMips();
	mips_generator->FileClose();

	error_handing->PrintError();
	error_handing->FileClose();

	return 0;
}