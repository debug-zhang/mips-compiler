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

const std::string STRING = "<字符串>";
const std::string PROGRAM = "<程序>";
const std::string CONST_DECLARE = "<常量说明>";
const std::string CONST_DEFINE = "<常量定义>";
const std::string UNSIGNINT = "<无符号整数>";
const std::string INTEGER = "<整数>";
const std::string HEAD_STATE = "<声明头部>";
const std::string VARIABLE_DECLARE = "<变量说明>";
const std::string VARIABLE_DEFINE = "<变量定义>";
const std::string RETURN_FUNCTION = "<有返回值函数定义>";
const std::string NO_RETURN_FUNCTION = "<无返回值函数定义>";
const std::string COMPOSITE_SENTENCE = "<复合语句>";
const std::string PARAMETER_TABLE = "<参数表>";
const std::string MAIN_FUNCTION = "<主函数>";
const std::string EXPRESSION = "<表达式>";
const std::string ITEM = "<项>";
const std::string FACTOR = "<因子>";
const std::string SENTENCE = "<语句>";
const std::string ASSIGN_SENTENCE = "<赋值语句>";
const std::string IF_SENTENCE = "<条件语句>";
const std::string CONDITION = "<条件>";
const std::string LOOP_SENTENCE = "<循环语句>";
const std::string STEP = "<步长>";
const std::string RETURN_CALL_SENTENCE = "<有返回值函数调用语句>";
const std::string NO_RETURN_CALL_SENTENCE = "<无返回值函数调用语句>";
const std::string VALUE_PARAMETER_TABLE = "<值参数表>";
const std::string SENTENCE_COLLECTION = "<语句列>";
const std::string SCANF_SENTENCE = "<读语句>";
const std::string PRINTF_SENTENCE = "<写语句>";
const std::string RETURN_SENTENCE = "<返回语句>";

class ParseAnalyser {
private:
    int label_count_;
    int reg_count_;
    std::list<struct Lexeme>::iterator iter_;
    std::list<struct Lexeme>::iterator iter_end_;
    CheckTable *check_table_;
    StringTable *string_table_;
    std::map<std::string, SymbolTable *> symbol_table_map_;
    MidcodeGenerator *midcode_generator_;
    ErrorHanding *error_handing_;

    void CountIterator(int step);

    Symbol *FindSymbol();

    Symbol *FindSymbol(int level);

    Symbol *InsertIdentifier(KindSymbol kind, TypeSymbol type, int level);

    void InsertSymbolTable(const std::string &name, int level);

    void CleanLevel(int level);

    void AddChild(SyntaxNode *node);

    void AddCommaChild(SyntaxNode *node);

    void AddSemicnChild(SyntaxNode *node);

    void AddWhileChild(SyntaxNode *node);

    void AddRbrackChild(SyntaxNode *node);

    void AddRparentChild(SyntaxNode *node);

    static SyntaxNode *AddSyntaxChild(const std::string &syntaxName, SyntaxNode *node);

    void SetSymbolType(TypeSymbol &type);

    bool IsThisIdentifier(const std::string &identifier);

    bool IsPlusOrMinu();

    bool IsMultOrDiv();

    bool IsVariableDefine();

    int AnalyzeInteger(SyntaxNode *node);

    void AnalyzeConstDefine(SyntaxNode *node, int level);

    void AnalyzeConstDeclare(SyntaxNode *node, int level);

    void AnalyzeVariableDefine(SyntaxNode *node, int level);

    void AnalyzeVariableDeclare(SyntaxNode *node, int level);

    void AnalyzeValuePrameterTable(SyntaxNode *node, Symbol *function);

    void AnalyzeReturnCallSentence(SyntaxNode *node);

    TypeSymbol AnalyzeFactor(SyntaxNode *node);

    TypeSymbol AnalyzeItem(SyntaxNode *node);

    TypeSymbol AnalyzeExpression(SyntaxNode *node);

    void AnalyzeCondition(SyntaxNode *node, bool isFalseBranch, int label_count);

    bool AnalyzeIfSentence(SyntaxNode *node, TypeSymbol returnType);

    int AnalyzeStep(SyntaxNode *node);

    void AnalyzeWhile(SyntaxNode *node, TypeSymbol returnType);

    void AnalyzeDoWhile(SyntaxNode *node, bool &noReturn, TypeSymbol returnType);

    void AnalyzeFor(SyntaxNode *node, TypeSymbol returnType);

    bool AnalyzeLoopSentence(SyntaxNode *node, TypeSymbol returnType);

    void AnalyzeAssignSentence(SyntaxNode *node);

    void AnalyzeScanfIdentifier(SyntaxNode *node);

    void AnalyzeScanfSentence(SyntaxNode *node);

    void AnalyzePrintfSentence(SyntaxNode *node);

    TypeSymbol AnalyzeReturnSentence(SyntaxNode *node);

    bool AnalyzeSentence(SyntaxNode *node, TypeSymbol returnType);

    bool AnalyzeSentenceCollection(SyntaxNode *node, TypeSymbol returnType);

    void AnalyzeCompositeSentence(SyntaxNode *node, TypeSymbol returnType);

    void AnalyzeMain(SyntaxNode *node);

    void AnalyzeParameterTable(SyntaxNode *node, Symbol *function);

    void AnalyzeVoidFunc(SyntaxNode *node);

    void AnalyzeHeadState(SyntaxNode *node, Symbol *&function);

    void AnalyzeFunc(SyntaxNode *node);

    void BuildSyntaxTree(SyntaxNode *root);

public:
    ParseAnalyser(const std::string &fileName, std::list<struct Lexeme> *lexList, ErrorHanding *errorHanding);

    void AnalyzeParse();

    std::map<std::string, SymbolTable *> symbol_table_map();

    std::list<Midcode *> midcode_list();

    StringTable *string_table();

    CheckTable *check_table();

    int reg_count() const;

    void FileClose();
};

