#include "lexer.h"
#include "parser.h"
using namespace std;

int main() {
	ifstream testfile("testfile.txt");
	ofstream output("output.txt");

	Lexer* lexer = new Lexer();
	list<struct Lexeme> lexList = lexer->Analyze(testfile);

	list<struct Lexeme>::iterator iter = lexList.begin();
	list<struct Lexeme>::iterator iterEnd = lexList.end();

	Parser* parser = new Parser(iter, iterEnd);
	parser->AnalyzeParse(output);

	testfile.close();
	output.close();

	return 0;
}