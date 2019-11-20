#pragma once

#include <fstream>
#include <list>
#include <map>
#include <string>
#include "lexical_analyser.h"
#include "syntax_node.h"
#include "table.h"
#include "midcode_generator.h"
#include "error_handing.h"

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

class ParseAnalyser {
private:
	int labelCount;
	int regCount;
	list<struct Lexeme>::iterator iter;
	list<struct Lexeme>::iterator iterEnd;
	CheckTable* checkTable;
	StringTable* stringTable;
	map<string, SymbolTable*> symbolTableMap;
	MidcodeGenerator* midcodeGenerator;
	ErrorHanding* errorHanding;
	Symbol* tempFunction;

	void CountIterator(int step);

	Symbol* FindSymbol(int level);
	Symbol* InsertIdentifier(KIND_SYMBOL kind, TYPE_SYMBOL type, int level);
	void InsertSymbolTable(string name, int level);
	void CleanLevel(int level);
	void AddChild(SyntaxNode* node);
	void AddCommaChild(SyntaxNode* node);
	void AddSemicnChild(SyntaxNode* node);
	void AddWhileChild(SyntaxNode* node);
	void AddRbrackChild(SyntaxNode* node);
	void AddRparentChild(SyntaxNode* node);
	SyntaxNode* AddSyntaxChild(string syntaxName, SyntaxNode* node);

	bool IsThisIdentifier(string identifier);
	bool IsPlusOrMinu();
	bool IsMultOrDiv();
	bool IsVariableDefine();

	int AnalyzeInteger(SyntaxNode* node);
	void AnalyzeConstDefine(SyntaxNode* node, int level);
	void AnalyzeConstDeclare(SyntaxNode* node, int level);
	void AnalyzeVariableDefine(SyntaxNode* node, int level);
	void AnalyzeVariableDeclare(SyntaxNode* node, int level);

	void AnalyzeValuePrameterTable(SyntaxNode* node);
	void AnalyzeReturnCallSentence(SyntaxNode* node);

	TYPE_SYMBOL AnalyzeFactor(SyntaxNode* node);
	TYPE_SYMBOL AnalyzeItem(SyntaxNode* node);
	TYPE_SYMBOL AnalyzeExpression(SyntaxNode* node);

	void AnalyzeCondition(SyntaxNode* node, bool isFalseBranch);
	bool AnalyzeIfSentence(SyntaxNode* node, TYPE_SYMBOL returnType);

	int AnalyzeStep(SyntaxNode* node);
	void AnalyseWhile(SyntaxNode* node, TYPE_SYMBOL returnType);
	void AnalyseDoWhile(SyntaxNode* node, bool& noReturn, TYPE_SYMBOL returnType);
	void AnalyseFor(SyntaxNode* node, TYPE_SYMBOL returnType);
	bool AnalyzeLoopSentence(SyntaxNode* node, TYPE_SYMBOL returnType);

	void AnalyzeAssignSentence(SyntaxNode* node);
	void AnalyseScanfIdentifier(SyntaxNode* node);
	void AnalyzeScanfSentence(SyntaxNode* node);
	void AnalyzePrintfSentence(SyntaxNode* node);
	TYPE_SYMBOL AnalyzeReturnSentence(SyntaxNode* node);

	bool AnalyzeSentence(SyntaxNode* node, TYPE_SYMBOL returnType);
	bool AnalyzeSentenceCollection(SyntaxNode* node, TYPE_SYMBOL returnType);
	void AnalyzeCompositeSentence(SyntaxNode* node, TYPE_SYMBOL returnType);

	void AnalyzeMain(SyntaxNode* node);
	void AnalyzeParameterTable(SyntaxNode* node);
	void AnalyzeVoidFunc(SyntaxNode* node);
	void AnalyzeHeadState(SyntaxNode* node);
	void AnalyzeFunc(SyntaxNode* node);

	void BuildSyntaxTree(SyntaxNode* root);

public:
	ParseAnalyser(string fileName, list<struct Lexeme>* lexList, ErrorHanding* errorHanding);
	void AnalyzeParse();
	map<string, SymbolTable*> GetSymbolTableMap();
	void FileClose();
};

