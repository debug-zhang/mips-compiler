#pragma once

#include <fstream>
#include <list>
#include <map>
#include <string>
#include "lexer.h"
#include "syntax_node.h"

using namespace std;

const string STRING = "<字符串>";
const string PROGRAM = "<程序>";
const string CONST_DECLARE = "<常量说明>";
const string CONST_DEFINE = "<常量定义>";
const string UNSIGNINT = "<无符号整数>";
const string INTEGER = "<整数>";
const string HEAD_STATE = "<声明头部>";
const string VARIABLE_DECLARE = "<变量说明>";
const string VARIABLE_DEFINE = "<变量定义>";
const string RETURN_FUNCTION = "<有返回值函数定义>";
const string NO_RETURN_FUNCTION = "<无返回值函数定义>";
const string COMPOSITE_SENTENCE = "<复合语句>";
const string PARAMETER_TABLE = "<参数表>";
const string MAIN_FUNCTION = "<主函数>";
const string EXPRESSION = "<表达式>";
const string ITEM = "<项>";
const string FACTOR = "<因子>";
const string SENTENCE = "<语句>";
const string ASSIGN_SENTENCE = "<赋值语句>";
const string IF_SENTENCE = "<条件语句>";
const string CONDITION = "<条件>";
const string LOOP_SENTENCE = "<循环语句>";
const string STEP = "<步长>";
const string RETURN_CALL_SENTENCE = "<有返回值函数调用语句>";
const string NO_RETURN_CALL_SENTENCE = "<无返回值函数调用语句>";
const string VALUE_PARAMETER_TABLE = "<值参数表>";
const string SENTENCE_COLLECTION = "<语句列>";
const string SCANF_SENTENCE = "<读语句>";
const string PRINTF_SENTENCE = "<写语句>";
const string RETURN_SENTENCE = "<返回语句>";

class Parser {
private:
	map<string, string> symbolMap;
	list<struct Lexeme>::iterator iter;
	list<struct Lexeme>::iterator iterEnd;

	void CountIterator(int step);

	string FindSymbol();
	void AddSymbol(string define);

	void AddChild(SyntaxNode* node);
	SyntaxNode* AddSyntaxChild(string syntaxName, SyntaxNode* node);
	void AddCommaChild(SyntaxNode* node);

	bool IsThisIdentifier(string identifier);
	bool IsPlusOrMinu();
	bool IsMultOrDiv();
	bool isVariableDefine();

	void AnalyzeInteger(SyntaxNode* node);
	void AnalyzeConstDefine(SyntaxNode* node);
	void AnalyzeConstDeclare(SyntaxNode* node);
	void AnalyzeVariableDefine(SyntaxNode* node);
	void AnalyzeVariableDeclare(SyntaxNode* node);
	void AnalyzeValuePrameterTable(SyntaxNode* node);
	void AnalyzeReturnCallSentence(SyntaxNode* node);
	void AnalyzeNoReturnCallSentence(SyntaxNode* node);
	void AnalyzeFactor(SyntaxNode* node);
	void AnalyzeItem(SyntaxNode* node);
	void AnalyzeExpression(SyntaxNode* node);
	void AnalyzeCondition(SyntaxNode* node);
	void AnalyzeIfSentence(SyntaxNode* node);
	void AnalyzeStep(SyntaxNode* node);
	void AnalyzeLoopSentence(SyntaxNode* node);
	void AnalyzeAssignSentence(SyntaxNode* node);
	void AnalyzeScanfSentence(SyntaxNode* node);
	void AnalyzePrintfSentence(SyntaxNode* node);
	void AnalyzeReturnSentence(SyntaxNode* node);
	void AnalyzeSentence(SyntaxNode* node);
	void AnalyzeSentenceCollection(SyntaxNode* node);
	void AnalyzeCompositeSentence(SyntaxNode* node);
	void AnalyzeMain(SyntaxNode* node);
	void AnalyzeParameterTable(SyntaxNode* node);
	void AnalyzeVoidFunc(SyntaxNode* node);
	void AnalyzeHeadState(SyntaxNode* node);
	void AnalyzeFunc(SyntaxNode* node);
	void BuildSyntaxTree(SyntaxNode* root);

public:
	Parser(list<struct Lexeme>::iterator& iter, list<struct Lexeme>::iterator& iterEnd);
	void AnalyzeParse(ofstream& output);
};

