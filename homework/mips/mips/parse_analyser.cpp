#include "parse_analyser.h"

ParseAnalyser::ParseAnalyser(string fileName, list<struct Lexeme>* lexList, ErrorHanding* errorHanding) {
	this->output.open(fileName);
	this->iter = lexList->begin();
	this->iterEnd = lexList->end();
	this->errorHanding = errorHanding;
}

void ParseAnalyser::CountIterator(int step) {
	if (step > 0) {
		while (step--) {
			iter++;
		}
	} else {
		while (step++) {
			iter--;
		}
	}
}

void ParseAnalyser::InsertIdentifier(IDENTIFIER_KIND kind, IDENTIFIER_TYPE type, int level) {
	symbolTable.AddIdentifier(iter->value, kind, type, level);
}

void ParseAnalyser::CleanLevel(int level) {
	symbolTable.CleanLevel(level);
}

struct Identifier* ParseAnalyser::FindSymbol(int level) {
	return symbolTable.findIdentifier(iter->value, level);
}

bool ParseAnalyser::IsThisIdentifier(string identifier) {
	return iter->identifier == identifier;
}

bool ParseAnalyser::IsPlusOrMinu() {
	return IsThisIdentifier(PLUS) || IsThisIdentifier(MINU);
}

bool ParseAnalyser::IsMultOrDiv() {
	return IsThisIdentifier(MULT) || IsThisIdentifier(DIV);
}

bool ParseAnalyser::IsVariableDefine() {
	if (IsThisIdentifier(INTTK) || IsThisIdentifier(CHARTK)) {
		CountIterator(+2);
		if (IsThisIdentifier(LPARENT)) {
			CountIterator(-2);
			return false;
		} else {
			CountIterator(-2);
			return true;
		}
	} else {
		return false;
	}
}

void ParseAnalyser::AddChild(SyntaxNode* node) {
	node->addChild(new SyntaxNode(iter->identifier, iter->value));
	CountIterator(+1);
}

void ParseAnalyser::AddCommaChild(SyntaxNode* node) {
	if (IsThisIdentifier(COMMA)) {
		AddChild(node);
	}
}

void ParseAnalyser::AddSemicnChild(SyntaxNode* node) {
	if (IsThisIdentifier(SEMICN)) {
		AddChild(node);	// SEMICN
	} else {
		CountIterator(-1);
		errorHanding->AddError(iter->lineNumber, MISSING_SEMICN);
		CountIterator(+1);
	}
}

void ParseAnalyser::AddWhileChild(SyntaxNode* node) {
	if (IsThisIdentifier(WHILETK)) {
		AddChild(node);	// WHILETK
	} else {
		CountIterator(-1);
		errorHanding->AddError(iter->lineNumber, MISSING_WHILE_IN_DO_WHILE);
		CountIterator(+1);
	}
}

void ParseAnalyser::AddRbrackChild(SyntaxNode* node) {
	if (IsThisIdentifier(RBRACK)) {
		AddChild(node);	// RBRACK
	} else {
		CountIterator(-1);
		errorHanding->AddError(iter->lineNumber, MISSING_RBRACK);
		CountIterator(+1);
	}
}

void ParseAnalyser::AddRparentChild(SyntaxNode* node) {
	if (IsThisIdentifier(RPARENT)) {
		AddChild(node);	// RPARENT
	} else {
		CountIterator(-1);
		errorHanding->AddError(iter->lineNumber, MISSING_RPARENT);
		CountIterator(+1);
	}
}

SyntaxNode* ParseAnalyser::AddSyntaxChild(string syntaxName, SyntaxNode* node) {
	SyntaxNode* subRoot = new SyntaxNode(syntaxName);
	node->addChild(subRoot);
	return subRoot;
}

void ParseAnalyser::AnalyzeInteger(SyntaxNode* node) {
	if (IsPlusOrMinu()) {
		AddChild(node);
	}

	AddChild(AddSyntaxChild(UNSIGNINT, node));		// INTCON
}

void ParseAnalyser::AnalyzeConstDefine(SyntaxNode* node, int level) {
	IDENTIFIER_TYPE type = iter->identifier == INTTK ? INT : CHAR;
	AddChild(node);	// INTTK or CHARTK
	while (IsThisIdentifier(IDENFR)) {
		if (FindSymbol(level)) {
			errorHanding->AddError(iter->lineNumber, REDEFINITION);
		} else {
			InsertIdentifier(CONST, type, level);
		}
		AddChild(node);	// IDENFR
		AddChild(node);	// ASDSIGN
		if (IsThisIdentifier(CHARCON)) {
			AddChild(node);	// CHARCON
		} else if (IsThisIdentifier(INTCON) || IsThisIdentifier(PLUS) || IsThisIdentifier(MINU)) {
			AnalyzeInteger(AddSyntaxChild(INTEGER, node));
		} else {
			errorHanding->AddError(iter->lineNumber, DEFINE_CONST_OTHERS);
			AddChild(node);
		}
		AddCommaChild(node);	// COMMA
	}
}

void ParseAnalyser::AnalyzeConstDeclare(SyntaxNode* node, int level) {
	while (IsThisIdentifier(CONSTTK)) {
		AddChild(node);	// CONSTTK
		AnalyzeConstDefine(AddSyntaxChild(CONST_DEFINE, node), level);
		AddSemicnChild(node);	// SEMICN
	}
}

void ParseAnalyser::AnalyzeVariableDefine(SyntaxNode* node, int level) {
	IDENTIFIER_TYPE type = iter->identifier == INTTK ? INT : CHAR;
	AddChild(node);	// INTTK or CHARTK
	while (IsThisIdentifier(IDENFR)) {
		if (FindSymbol(level)) {
			errorHanding->AddError(iter->lineNumber, REDEFINITION);
		}
		InsertIdentifier(VARIABLE, type, level);
		AddChild(node);	// IDENFR

		if (IsThisIdentifier(LBRACK)) {
			AddChild(node);	// LBRACK
			AddChild(AddSyntaxChild(UNSIGNINT, node));	// INTCON
			AddRbrackChild(node);
			InsertIdentifier(ARRAY, type, level);
		}
		AddCommaChild(node);	// COMMA
	}
}

void ParseAnalyser::AnalyzeVariableDeclare(SyntaxNode* node, int level) {
	while (IsVariableDefine()) {
		AnalyzeVariableDefine(AddSyntaxChild(VARIABLE_DEFINE, node), level);
		AddSemicnChild(node);	// SEMICN
	}
}

void ParseAnalyser::AnalyzeValuePrameterTable(SyntaxNode* node) {
	if (IsThisIdentifier(RPARENT)) {
		if (tempFunction != NULL && tempFunction->parameter.length() != 0) {
			errorHanding->AddError(iter->lineNumber, FUNCTION_PARAMETER_NUMBER_DONT_MATCH);
		}
		return;
	}
	string str;
	IDENTIFIER_TYPE type;
	while (!IsThisIdentifier(RPARENT)) {
		type = AnalyzeExpression(AddSyntaxChild(EXPRESSION, node));
		str.push_back(type == INT ? '0' : '1');
		while (IsThisIdentifier(COMMA)) {
			AddChild(node);	// COMMA
			type = AnalyzeExpression(AddSyntaxChild(EXPRESSION, node));
			str.push_back(type == INT ? '0' : '1');
		}
	}

	if (tempFunction != NULL && tempFunction->parameter.length() != str.length()) {
		errorHanding->AddError(iter->lineNumber, FUNCTION_PARAMETER_NUMBER_DONT_MATCH);
	} else if (tempFunction != NULL && tempFunction->parameter != str) {
		errorHanding->AddError(iter->lineNumber, FUNCTION_PARAMETER_TYPE_DONT_MATCH);
	}
}

void ParseAnalyser::AnalyzeReturnCallSentence(SyntaxNode* node) {
	if (FindSymbol(0) == NULL) {
		errorHanding->AddError(iter->lineNumber, UNDEFINED);
	}
	tempFunction = FindSymbol(0);
	AddChild(node);	// IDENFR
	AddChild(node);	// LPARENT
	AnalyzeValuePrameterTable(AddSyntaxChild(VALUE_PARAMETER_TABLE, node));
	AddRparentChild(node);	// RPARENT
}

void ParseAnalyser::AnalyzeNoReturnCallSentence(SyntaxNode* node) {
	AnalyzeReturnCallSentence(node);
}

IDENTIFIER_TYPE ParseAnalyser::AnalyzeFactor(SyntaxNode* node) {
	IDENTIFIER_TYPE type;
	if (IsThisIdentifier(IDENFR)) {
		if (FindSymbol(0) != NULL && FindSymbol(0)->kind == FUNCTION && FindSymbol(0)->type != VOID) {
			type = FindSymbol(0)->type;
			AnalyzeReturnCallSentence(AddSyntaxChild(RETURN_CALL_SENTENCE, node));
		} else {
			if (FindSymbol(0) == NULL) {
				if (FindSymbol(1) == NULL) {
					errorHanding->AddError(iter->lineNumber, UNDEFINED);
					type = INT;
				} else {
					type = FindSymbol(1)->type;
				}
			} else {
				type = FindSymbol(0)->type;
			}
			AddChild(node);	// IDENFR
			if (IsThisIdentifier(LBRACK)) {
				AddChild(node);	// LBRACK
				IDENTIFIER_TYPE expressType = AnalyzeExpression(AddSyntaxChild(EXPRESSION, node));
				if (expressType != INT) {
					errorHanding->AddError(iter->lineNumber, ILLEGAL_ARRAY_INDEX);
				}
				AddRbrackChild(node);	// RBRACK
			}
		}
	} else if (IsThisIdentifier(LPARENT)) {
		AddChild(node);	// LPARENT
		type = AnalyzeExpression(AddSyntaxChild(EXPRESSION, node));
		AddRparentChild(node);	// RPARENT
	} else if (IsThisIdentifier(CHARCON)) {
		type = CHAR;
		AddChild(node);
	} else {
		type = INT;
		AnalyzeInteger(AddSyntaxChild(INTEGER, node));
	}
	return type;
}

IDENTIFIER_TYPE ParseAnalyser::AnalyzeItem(SyntaxNode* node) {
	IDENTIFIER_TYPE type;
	type = AnalyzeFactor(AddSyntaxChild(FACTOR, node));
	while (IsMultOrDiv()) {
		AddChild(node);
		AnalyzeFactor(AddSyntaxChild(FACTOR, node));
		type = INT;
	}
	return type;
}

IDENTIFIER_TYPE ParseAnalyser::AnalyzeExpression(SyntaxNode* node) {
	IDENTIFIER_TYPE type;
	if (IsPlusOrMinu()) {
		AddChild(node);	// PLUS or MINU
	}
	type = AnalyzeItem(AddSyntaxChild(ITEM, node));
	while (IsPlusOrMinu()) {
		AddChild(node);
		AnalyzeItem(AddSyntaxChild(ITEM, node));
		type = INT;
	}
	return type;
}

void ParseAnalyser::AnalyzeCondition(SyntaxNode* node) {
	if (AnalyzeExpression(AddSyntaxChild(EXPRESSION, node)) != INT) {
		CountIterator(-1);
		errorHanding->AddError(iter->lineNumber, ILLEGAL_TYPE_IN_IF);
		CountIterator(+1);
	}
	if (IsThisIdentifier(LSS)
		|| IsThisIdentifier(LEQ)
		|| IsThisIdentifier(GRE)
		|| IsThisIdentifier(GEQ)
		|| IsThisIdentifier(EQL)
		|| IsThisIdentifier(NEQ)) {
		AddChild(node);
		if (AnalyzeExpression(AddSyntaxChild(EXPRESSION, node)) != INT) {
			CountIterator(-1);
			errorHanding->AddError(iter->lineNumber, ILLEGAL_TYPE_IN_IF);
			CountIterator(+1);
		}
	}
}

bool ParseAnalyser::AnalyzeIfSentence(SyntaxNode* node, IDENTIFIER_TYPE returnType) {
	bool noReturn = true;
	AddChild(node);	// IFTK
	AddChild(node);	// LPARENT
	AnalyzeCondition(AddSyntaxChild(CONDITION, node));
	AddRparentChild(node);	// RPARENT
	noReturn = AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType);
	if (IsThisIdentifier(ELSETK)) {
		AddChild(node);	// ELSETK
		noReturn = AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType) && noReturn;
	}
	return noReturn;
}

void ParseAnalyser::AnalyzeStep(SyntaxNode* node) {
	AddChild(AddSyntaxChild(UNSIGNINT, node));	// INTCON
}

bool ParseAnalyser::AnalyzeLoopSentence(SyntaxNode* node, IDENTIFIER_TYPE returnType) {
	bool noReturn = true;
	if (IsThisIdentifier(WHILETK)) {
		AddChild(node);	// WHILETK
		AddChild(node);	// LPARENT
		AnalyzeCondition(AddSyntaxChild(CONDITION, node));
		AddRparentChild(node);	// RPARENT
		AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType);
	} else if (IsThisIdentifier(DOTK)) {
		AddChild(node);	// DOTK
		noReturn = AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType);
		AddWhileChild(node);	// WHILETK
		AddChild(node);	// LPARENT
		AnalyzeCondition(AddSyntaxChild(CONDITION, node));
		AddRparentChild(node);	// RPARENT
	} else if (IsThisIdentifier(FORTK)) {
		AddChild(node);	// FORTK
		AddChild(node);	// LPARENT
		if (FindSymbol(0) == NULL && FindSymbol(1) == NULL) {
			errorHanding->AddError(iter->lineNumber, UNDEFINED);
		}
		AddChild(node);	// IDENFR
		AddChild(node);	// ASSIGN
		AnalyzeExpression(AddSyntaxChild(EXPRESSION, node));
		AddSemicnChild(node);	// SEMICN
		AnalyzeCondition(AddSyntaxChild(CONDITION, node));
		AddSemicnChild(node);	// SEMICN
		if (FindSymbol(0) == NULL && FindSymbol(1) == NULL) {
			errorHanding->AddError(iter->lineNumber, UNDEFINED);
		}
		AddChild(node);	// IDENFR
		AddChild(node);	// ASSIGN
		if (FindSymbol(0) == NULL && FindSymbol(1) == NULL) {
			errorHanding->AddError(iter->lineNumber, UNDEFINED);
		}
		AddChild(node);	// IDENFR
		AddChild(node);	// PLUS or MINU
		AnalyzeStep(AddSyntaxChild(STEP, node));
		AddRparentChild(node);	// RPARENT
		AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType);
	}
	return noReturn;
}

void ParseAnalyser::AnalyzeAssignSentence(SyntaxNode* node) {
	IDENTIFIER_KIND kind;
	if (FindSymbol(0) == NULL) {
		if (FindSymbol(1) == NULL) {
			errorHanding->AddError(iter->lineNumber, UNDEFINED);
		} else {
			kind = FindSymbol(1)->kind;
			if (kind == CONST) {
				errorHanding->AddError(iter->lineNumber, ASSIGN_TO_CONST);
			}
		}
	} else {
		kind = FindSymbol(0)->kind;
		if (kind == CONST) {
			errorHanding->AddError(iter->lineNumber, ASSIGN_TO_CONST);
		}
	}
	AddChild(node);	// IDENFR
	if (IsThisIdentifier(LBRACK)) {
		AddChild(node);	// LBRACK
		if (AnalyzeExpression(AddSyntaxChild(EXPRESSION, node)) == CHAR) {
			errorHanding->AddError(iter->lineNumber, ILLEGAL_ARRAY_INDEX);
		}
		AddRbrackChild(node);	// RBRACK
	}
	AddChild(node);	// ASSIGN
	AnalyzeExpression(AddSyntaxChild(EXPRESSION, node));
}

void ParseAnalyser::AnalyzeScanfSentence(SyntaxNode* node) {
	AddChild(node);	// SCANFTK
	AddChild(node);	// LPARENT

	if (FindSymbol(0) == NULL && FindSymbol(1) == NULL) {
		errorHanding->AddError(iter->lineNumber, UNDEFINED);
	}
	AddChild(node);	// IDENFR
	while (IsThisIdentifier(COMMA)) {
		AddChild(node);	// COMMA
		if (FindSymbol(0) == NULL && FindSymbol(1) == NULL) {
			errorHanding->AddError(iter->lineNumber, UNDEFINED);
		}
		AddChild(node);	// IDENFR
	}

	AddRparentChild(node);	// RPARENT
}

void ParseAnalyser::AnalyzePrintfSentence(SyntaxNode* node) {
	AddChild(node);	// PRINTFTK
	AddChild(node);	// LPARENT

	if (IsThisIdentifier(STRCON)) {
		AddChild(AddSyntaxChild(STRING, node));
		if (IsThisIdentifier(COMMA)) {
			AddChild(node);	// COMMA
			AnalyzeExpression(AddSyntaxChild(EXPRESSION, node));
		}
	} else {
		AnalyzeExpression(AddSyntaxChild(EXPRESSION, node));
	}

	AddRparentChild(node);	// RPARENT
}

IDENTIFIER_TYPE ParseAnalyser::AnalyzeReturnSentence(SyntaxNode* node) {
	IDENTIFIER_TYPE type = VOID;
	AddChild(node);	// RETURNTK
	if (IsThisIdentifier(LPARENT)) {
		AddChild(node);	// LPARENT
		type = AnalyzeExpression(AddSyntaxChild(EXPRESSION, node));
		AddRparentChild(node);	// RPARENT
	}
	return type;
}

bool ParseAnalyser::AnalyzeSentence(SyntaxNode* node, IDENTIFIER_TYPE returnType) {
	bool noReturn = true;
	if (IsThisIdentifier(IFTK)) {
		noReturn = AnalyzeIfSentence(AddSyntaxChild(IF_SENTENCE, node), returnType);
	} else if (IsThisIdentifier(WHILETK)
		|| IsThisIdentifier(DOTK)
		|| IsThisIdentifier(FORTK)) {
		noReturn = AnalyzeLoopSentence(AddSyntaxChild(LOOP_SENTENCE, node), returnType);
	} else if (IsThisIdentifier(LBRACE)) {
		AddChild(node);	// LBRACE
		noReturn = AnalyzeSentenceCollection(AddSyntaxChild(SENTENCE_COLLECTION, node), returnType);
		AddChild(node);	// RBRACE
	} else if (IsThisIdentifier(IDENFR)) {
		noReturn = true;
		if (FindSymbol(0) != NULL && FindSymbol(0)->kind == FUNCTION && FindSymbol(0)->type != VOID) {
			AnalyzeReturnCallSentence(AddSyntaxChild(RETURN_CALL_SENTENCE, node));
			AddSemicnChild(node);	// SEMICN
		} else if (FindSymbol(0) != NULL && FindSymbol(0)->kind == FUNCTION && FindSymbol(0)->type == VOID) {
			AnalyzeReturnCallSentence(AddSyntaxChild(NO_RETURN_CALL_SENTENCE, node));
			AddSemicnChild(node);	// SEMICN
		} else {
			if (FindSymbol(0) != NULL || FindSymbol(1) != NULL) {
				AnalyzeAssignSentence(AddSyntaxChild(ASSIGN_SENTENCE, node));
				AddSemicnChild(node);	// SEMICN
			}else if (FindSymbol(0) == NULL) {
				if (FindSymbol(1) == NULL) {
					errorHanding->AddError(iter->lineNumber, UNDEFINED);
				}
				CountIterator(+1);
				if (IsThisIdentifier(LPARENT)) {
					while (!IsThisIdentifier(RPARENT)) {
						CountIterator(+1);
					}
					CountIterator(+1);
				} else if (IsThisIdentifier(ASSIGN)) {
					while (!IsThisIdentifier(SEMICN)) {
						CountIterator(+1);
					}
					CountIterator(+1);
				}
			}
		}
	} else if (IsThisIdentifier(SCANFTK)) {
		noReturn = true;
		AnalyzeScanfSentence(AddSyntaxChild(SCANF_SENTENCE, node));
		AddSemicnChild(node);	// SEMICN
	} else if (IsThisIdentifier(PRINTFTK)) {
		noReturn = true;
		AnalyzePrintfSentence(AddSyntaxChild(PRINTF_SENTENCE, node));
		AddSemicnChild(node);	// SEMICN
	} else if (IsThisIdentifier(SEMICN)) {
		noReturn = true;
		AddSemicnChild(node);	// SEMICN
	} else if (IsThisIdentifier(RETURNTK)) {
		IDENTIFIER_TYPE type = AnalyzeReturnSentence(AddSyntaxChild(RETURN_SENTENCE, node));
		noReturn = type == VOID;
		CountIterator(-1);
		if (returnType == VOID && type != VOID) {
			errorHanding->AddError(iter->lineNumber, RETURN_IN_NO_RETURN_FUNCTION);
		} else if (returnType != type) {
			errorHanding->AddError(iter->lineNumber, NO_RETURN_OR_WRONG_RETURN_IN_RETURN_FUNCTION);
		}
		CountIterator(+1);
		AddSemicnChild(node);	// SEMICN
	}
	return noReturn;
}

bool ParseAnalyser::AnalyzeSentenceCollection(SyntaxNode* node, IDENTIFIER_TYPE returnType) {
	bool noReturn = true;
	while (!IsThisIdentifier(RBRACE)) {
		if (!AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType)) {
			noReturn = false;
		}
	}

	return noReturn;
}

void ParseAnalyser::AnalyzeCompositeSentence(SyntaxNode* node, IDENTIFIER_TYPE returnType) {
	if (IsThisIdentifier(CONSTTK)) {
		AnalyzeConstDeclare(AddSyntaxChild(CONST_DECLARE, node), 1);
	}
	if (IsThisIdentifier(INTTK) || IsThisIdentifier(CHARTK)) {
		AnalyzeVariableDeclare(AddSyntaxChild(VARIABLE_DECLARE, node), 1);
	}
	bool noReturn = AnalyzeSentenceCollection(AddSyntaxChild(SENTENCE_COLLECTION, node), returnType);
	if (returnType != VOID && noReturn) {
		CountIterator(-1);
		errorHanding->AddError(iter->lineNumber, NO_RETURN_OR_WRONG_RETURN_IN_RETURN_FUNCTION);
		CountIterator(+1);
	}
}

void ParseAnalyser::AnalyzeMain(SyntaxNode* node) {
	AddChild(node);	// VOIDTK
	InsertIdentifier(FUNCTION, VOID, 0);
	tempFunction = FindSymbol(0);
	CleanLevel(1);
	AddChild(node);	// MAINTK
	AddChild(node);	// LPARENT
	AddRparentChild(node);	// RPARENT
	AddChild(node);	// LBRACE

	AnalyzeCompositeSentence(AddSyntaxChild(COMPOSITE_SENTENCE, node), VOID);

	AddChild(node);	// RBRACE
}

void ParseAnalyser::AnalyzeParameterTable(SyntaxNode* node) {
	while (IsThisIdentifier(INTTK) || IsThisIdentifier(CHARTK)) {

		IDENTIFIER_TYPE type = iter->identifier == INTTK ? INT : CHAR;
		AddChild(node);	// INTTK or CHARTK

		if (FindSymbol(1) != NULL) {
			errorHanding->AddError(iter->lineNumber, REDEFINITION);
		}
		tempFunction->parameter.push_back(type == INT ? '0' : '1');
		InsertIdentifier(VARIABLE, type, 1);
		AddChild(node);	// IDENTFR
		AddCommaChild(node);
	}
}

void ParseAnalyser::AnalyzeVoidFunc(SyntaxNode* node) {
	AddChild(node);	// VOIDTK
	if (FindSymbol(0) != NULL) {
		errorHanding->AddError(iter->lineNumber, REDEFINITION);
	}
	InsertIdentifier(FUNCTION, VOID, 0);
	tempFunction = FindSymbol(0);
	CleanLevel(1);
	AddChild(node);	// IDENFR
	AddChild(node);	// LPARENT
	AnalyzeParameterTable(AddSyntaxChild(PARAMETER_TABLE, node));
	AddRparentChild(node);	// RPARENT
	AddChild(node);	// LBRACE
	AnalyzeCompositeSentence(AddSyntaxChild(COMPOSITE_SENTENCE, node), VOID);
	AddChild(node);	// RBRACE
}

void ParseAnalyser::AnalyzeHeadState(SyntaxNode* node) {
	IDENTIFIER_TYPE type = iter->identifier == INTTK ? INT : CHAR;
	AddChild(node);	// INTTK or CHARTK
	if (FindSymbol(0) != NULL) {
		errorHanding->AddError(iter->lineNumber, REDEFINITION);
	}
	InsertIdentifier(FUNCTION, type, 0);
	tempFunction = FindSymbol(0);
	CleanLevel(1);
	AddChild(node);	// IDENFR
}

void ParseAnalyser::AnalyzeFunc(SyntaxNode* node) {
	AnalyzeHeadState(AddSyntaxChild(HEAD_STATE, node));
	AddChild(node);	// LPARENT
	AnalyzeParameterTable(AddSyntaxChild(PARAMETER_TABLE, node));
	AddRparentChild(node);	// RPARENT
	AddChild(node);	// LBRACE
	AnalyzeCompositeSentence(AddSyntaxChild(COMPOSITE_SENTENCE, node), tempFunction->type);
	AddChild(node);	// RBRACE
}

void ParseAnalyser::BuildSyntaxTree(SyntaxNode* root) {
	string flag = CONST_DECLARE;

	while (iter != iterEnd) {
		if (IsThisIdentifier(CONSTTK)) {
			AnalyzeConstDeclare(AddSyntaxChild(CONST_DECLARE, root), 0);
		} else if (IsThisIdentifier(INTTK) || IsThisIdentifier(CHARTK)) {
			if (flag == CONST_DECLARE) {
				CountIterator(+2);
				if (IsThisIdentifier(LPARENT)) {
					flag = RETURN_FUNCTION;
				} else {
					flag = VARIABLE_DECLARE;
				}
				CountIterator(-2);
			}
			if (flag == RETURN_FUNCTION) {
				AnalyzeFunc(AddSyntaxChild(RETURN_FUNCTION, root));
			} else {
				AnalyzeVariableDeclare(AddSyntaxChild(VARIABLE_DECLARE, root), 0);
				flag = RETURN_FUNCTION;
			}
		} else {
			if (IsThisIdentifier(VOIDTK)) {
				CountIterator(+1);
			}
			if (IsThisIdentifier(MAINTK)) {
				CountIterator(-1);
				AnalyzeMain(AddSyntaxChild(MAIN_FUNCTION, root));
			} else {
				CountIterator(-1);
				AnalyzeVoidFunc(AddSyntaxChild(NO_RETURN_FUNCTION, root));
			}
		}
	}

}

void ParseAnalyser::AnalyzeParse() {
	SyntaxNode* root = new SyntaxNode(PROGRAM);
	BuildSyntaxTree(root);
	root->print(output);
}

void ParseAnalyser::FileClose() {
	output.close();
}