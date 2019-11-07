#include "lexical_analyser.h"
#include "parse_analyser.h"
using namespace std;

int main() {
	ifstream testfile("testfile.txt");
	ofstream output("output.txt");

	LexicalAnalyser* lexicalAnalyser = new LexicalAnalyser();
	list<struct Lexeme> lexList = lexicalAnalyser->Analyze(testfile);

	list<struct Lexeme>::iterator iter = lexList.begin();
	list<struct Lexeme>::iterator iterEnd = lexList.end();

	ParseAnalyser* parseAnalyser = new ParseAnalyser(iter, iterEnd);
	parseAnalyser->AnalyzeParse(output);

	testfile.close();
	output.close();

	return 0;
}