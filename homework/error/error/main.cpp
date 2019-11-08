#include "lexical_analyser.h"
#include "parse_analyser.h"
#include "error_handing.h"

using namespace std;

int main() {
	ifstream testfile("testfile.txt");
	ofstream output("output.txt");
	ofstream errorfile("error.txt");

	ErrorHanding* errorHanding = new ErrorHanding();

	LexicalAnalyser* lexicalAnalyser = new LexicalAnalyser(errorHanding);
	list<struct Lexeme> lexList = lexicalAnalyser->Analyze(testfile);

	list<struct Lexeme>::iterator iter = lexList.begin();
	list<struct Lexeme>::iterator iterEnd = lexList.end();

	ParseAnalyser* parseAnalyser = new ParseAnalyser(iter, iterEnd, errorHanding);
	//parseAnalyser->AnalyzeParse(output);

	errorHanding->PrintError(errorfile);

	testfile.close();
	output.close();
	errorfile.close();

	return 0;
}