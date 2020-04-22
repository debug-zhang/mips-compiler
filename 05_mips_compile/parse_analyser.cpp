#include "parse_analyser.h"

ParseAnalyser::ParseAnalyser(string fileName, list<struct Lexeme>* lexList, ErrorHanding* errorHanding) {
	this->check_table_ = new CheckTable();
	this->string_table_ = new StringTable();
	this->midcode_generator_ = new MidcodeGenerator();

	this->label_count_ = 0;
	this->reg_count_ = 1;
	this->midcode_generator_->OpenMidcodeFile(fileName);
	this->iter_ = lexList->begin();
	this->iter_end_ = lexList->end();
	this->error_handing_ = errorHanding;
}

void ParseAnalyser::CountIterator(int step) {
	if (step > 0) {
		while (step--) {
			iter_++;
		}
	} else {
		while (step++) {
			iter_--;
		}
	}
}

Symbol* ParseAnalyser::InsertIdentifier(KindSymbol kind, TypeSymbol type, int level) {
	return check_table_->AddSymbol(iter_->value, kind, type, level);
}

void ParseAnalyser::InsertSymbolTable(string name, int level) {
	symbol_table_map_.insert(pair<string, SymbolTable*>(name, check_table_->GetSymbolTable(level)));
}

void ParseAnalyser::CleanLevel(int level) {
	check_table_->ClearLevel(level);
}

Symbol* ParseAnalyser::FindSymbol() {
	return check_table_->FindSymbol(iter_->value);
}

Symbol* ParseAnalyser::FindSymbol(int level) {
	return check_table_->FindSymbol(iter_->value, level);
}

bool ParseAnalyser::IsThisIdentifier(string identifier) {
	return iter_->identifier == identifier;
}

bool ParseAnalyser::IsPlusOrMinu() {
	return IsThisIdentifier(PLUS) || IsThisIdentifier(MINU);
}

bool ParseAnalyser::IsMultOrDiv() {
	return IsThisIdentifier(MULT) || IsThisIdentifier(DIV);
}

bool ParseAnalyser::IsVariableDefine() {
	if (this->IsThisIdentifier(INTTK) || this->IsThisIdentifier(CHARTK)) {
		this->CountIterator(+2);
		if (this->IsThisIdentifier(LPARENT)) {
			this->CountIterator(-2);
			return false;
		} else {
			this->CountIterator(-2);
			return true;
		}
	} else {
		return false;
	}
}

void ParseAnalyser::AddChild(SyntaxNode* node) {
	node->AddChild(new SyntaxNode(iter_->identifier, iter_->value));
	this->CountIterator(+1);
}

void ParseAnalyser::AddCommaChild(SyntaxNode* node) {
	if (this->IsThisIdentifier(COMMA)) {
		this->AddChild(node);
	}
}

void ParseAnalyser::AddSemicnChild(SyntaxNode* node) {
	if (this->IsThisIdentifier(SEMICN)) {
		this->AddChild(node);	// SEMICN
	} else {
		this->CountIterator(-1);
		error_handing_->AddError(iter_->line_number, MISSING_SEMICN);
		this->CountIterator(+1);
	}
}

void ParseAnalyser::AddWhileChild(SyntaxNode* node) {
	if (this->IsThisIdentifier(WHILETK)) {
		this->AddChild(node);	// WHILETK
	} else {
		this->CountIterator(-1);
		error_handing_->AddError(iter_->line_number, MISSING_WHILE_IN_DO_WHILE);
		this->CountIterator(+1);
	}
}

void ParseAnalyser::AddRbrackChild(SyntaxNode* node) {
	if (this->IsThisIdentifier(RBRACK)) {
		this->AddChild(node);	// RBRACK
	} else {
		this->CountIterator(-1);
		error_handing_->AddError(iter_->line_number, MISSING_RBRACK);
		this->CountIterator(+1);
	}
}

void ParseAnalyser::AddRparentChild(SyntaxNode* node) {
	if (this->IsThisIdentifier(RPARENT)) {
		this->AddChild(node);	// RPARENT
	} else {
		this->CountIterator(-1);
		error_handing_->AddError(iter_->line_number, MISSING_RPARENT);
		this->CountIterator(+1);
	}
}

SyntaxNode* ParseAnalyser::AddSyntaxChild(string syntaxName, SyntaxNode* node) {
	SyntaxNode* subRoot = new SyntaxNode(syntaxName);
	node->AddChild(subRoot);
	return subRoot;
}

void ParseAnalyser::SetSymbolType(TypeSymbol& type) {
	type = iter_->identifier == INTTK ? TypeSymbol::INT : TypeSymbol::CHAR;
}

int ParseAnalyser::AnalyzeInteger(SyntaxNode* node) {
	int op = 1;
	if (this->IsPlusOrMinu()) {
		if (this->IsThisIdentifier(MINU)) {
			op = -1;
		}
		this->AddChild(node);
	}

	int integer = op * stoi(iter_->value);
	this->AddChild(this->AddSyntaxChild(UNSIGNINT, node));		// INTCON

	return integer;
}

void ParseAnalyser::AnalyzeConstDefine(SyntaxNode* node, int level) {
	Symbol* symbol = new Symbol();
	TypeSymbol type;
	this->SetSymbolType(type);
	this->AddChild(node);	// INTTK or CHARTK

	while (this->IsThisIdentifier(IDENFR)) {
		if (this->FindSymbol(level)) {
			error_handing_->AddError(iter_->line_number, REDEFINITION);
		} else {
			symbol = this->InsertIdentifier(KindSymbol::CONST, type, level);
		}
		this->AddChild(node);	// IDENFR
		this->AddChild(node);	// ASDSIGN
		if (this->IsThisIdentifier(CHARCON)) {
			symbol->set_const_value("\'" + iter_->value + "\'");
			this->AddChild(node);	// CHARCON
		} else if (this->IsThisIdentifier(INTCON) || this->IsPlusOrMinu()) {
			int value = this->AnalyzeInteger(this->AddSyntaxChild(INTEGER, node));
			symbol->set_const_value(to_string(value));
		} else {
			error_handing_->AddError(iter_->line_number, DEFINE_CONST_OTHERS);
			this->AddChild(node);
		}
		this->AddCommaChild(node);	// COMMA
	}
}

void ParseAnalyser::AnalyzeConstDeclare(SyntaxNode* node, int level) {
	while (this->IsThisIdentifier(CONSTTK)) {
		this->AddChild(node);	// CONSTTK
		this->AnalyzeConstDefine(this->AddSyntaxChild(CONST_DEFINE, node), level);
		this->AddSemicnChild(node);	// SEMICN
	}
}

void ParseAnalyser::AnalyzeVariableDefine(SyntaxNode* node, int level) {
	TypeSymbol type;
	SetSymbolType(type);
	this->AddChild(node);	// INTTK or CHARTK
	while (this->IsThisIdentifier(IDENFR)) {
		if (this->FindSymbol(level)) {
			error_handing_->AddError(iter_->line_number, REDEFINITION);
		}
		Symbol* symbol = this->InsertIdentifier(KindSymbol::VARIABLE, type, level);
		this->midcode_generator_->PrintVariable(type, iter_->value);
		this->AddChild(node);	// IDENFR

		if (this->IsThisIdentifier(LBRACK)) {
			this->AddChild(node);	// LBRACK
			int arrayLength = stoi(iter_->value);
			this->AddChild(this->AddSyntaxChild(UNSIGNINT, node));	// INTCON
			this->AddRbrackChild(node);

			symbol->set_kind(KindSymbol::ARRAY);
			symbol->set_array_length(arrayLength);
		}
		this->AddCommaChild(node);	// COMMA
	}
}

void ParseAnalyser::AnalyzeVariableDeclare(SyntaxNode* node, int level) {
	while (this->IsVariableDefine()) {
		this->AnalyzeVariableDefine(this->AddSyntaxChild(VARIABLE_DEFINE, node), level);
		this->AddSemicnChild(node);	// SEMICN
	}
}

void ParseAnalyser::AnalyzeValuePrameterTable(SyntaxNode* node, Symbol* function) {
	if (this->IsThisIdentifier(RPARENT)) {
		if (function != NULL && function->GetParameterCount() != 0) {
			error_handing_->AddError(iter_->line_number, FUNCTION_PARAMETER_NUMBER_DONT_MATCH);
		}
		return;
	}

	string str;
	TypeSymbol type;
	int count = 0;
	if (!this->IsThisIdentifier(RPARENT)) {
		SyntaxNode* expressionNode;
		expressionNode = this->AddSyntaxChild(EXPRESSION, node);
		type = this->AnalyzeExpression(expressionNode);
		str.push_back(type == TypeSymbol::INT ? '0' : '1');
		midcode_generator_->PrintPushParameter(function->name(),
			expressionNode->value(), count++);
		while (this->IsThisIdentifier(COMMA)) {
			this->AddChild(node);	// COMMA
			expressionNode = this->AddSyntaxChild(EXPRESSION, node);
			type = this->AnalyzeExpression(expressionNode);
			str.push_back(type == TypeSymbol::INT ? '0' : '1');
			midcode_generator_->PrintPushParameter(function->name(),
				expressionNode->value(), count++);
		}
	}

	if (function != NULL && function->GetParameterCount() != str.length()) {
		error_handing_->AddError(iter_->line_number, FUNCTION_PARAMETER_NUMBER_DONT_MATCH);
	} else if (function != NULL && function->parameter() != str) {
		error_handing_->AddError(iter_->line_number, FUNCTION_PARAMETER_TYPE_DONT_MATCH);
	}
}

void ParseAnalyser::AnalyzeReturnCallSentence(SyntaxNode* node) {
	if (this->FindSymbol(0) == NULL) {
		error_handing_->AddError(iter_->line_number, UNDEFINED);
	}
	Symbol* function = this->FindSymbol(0);
	midcode_generator_->PrintSave(function->name());
	this->AddChild(node);	// IDENFR
	this->AddChild(node);	// LPARENT
	this->AnalyzeValuePrameterTable(
		this->AddSyntaxChild(VALUE_PARAMETER_TABLE, node), function);
	this->AddRparentChild(node);	// RPARENT
	midcode_generator_->PrintCallFunction(function->name());
}

TypeSymbol ParseAnalyser::AnalyzeFactor(SyntaxNode* node) {
	TypeSymbol type;
	if (this->IsThisIdentifier(IDENFR)) {
		if (this->FindSymbol(0) != NULL
			&& this->FindSymbol(0)->kind() == KindSymbol::FUNCTION
			&& this->FindSymbol(0)->type() != TypeSymbol::VOID) {
			type = this->FindSymbol(0)->type();
			this->AnalyzeReturnCallSentence(this->AddSyntaxChild(RETURN_CALL_SENTENCE, node));
			midcode_generator_->PrintAssignReturn(reg_count_++);
		} else {
			Symbol* symbol = this->FindSymbol();
			if (symbol == NULL) {
				error_handing_->AddError(iter_->line_number, UNDEFINED);
				type = TypeSymbol::INT;
			} else {
				type = symbol->type();
			}
			string name = iter_->value;
			this->AddChild(node);	// IDENFR
			if (this->IsThisIdentifier(LBRACK)) {
				this->AddChild(node);	// LBRACK
				SyntaxNode* expression = this->AddSyntaxChild(EXPRESSION, node);
				TypeSymbol expressType = this->AnalyzeExpression(expression);
				if (expressType != TypeSymbol::INT) {
					error_handing_->AddError(iter_->line_number, ILLEGAL_ARRAY_INDEX);
				}
				midcode_generator_->PrintLoadToTempReg(name,
					expression->value(), reg_count_++);
				this->AddRbrackChild(node);	// RBRACK
			} else {
				node->set_value(name);
			}
		}
	} else if (this->IsThisIdentifier(LPARENT)) {
		this->AddChild(node);	// LPARENT
		SyntaxNode* expression = this->AddSyntaxChild(EXPRESSION, node);
		type = this->AnalyzeExpression(expression);
		type = TypeSymbol::INT;
		node->set_value(expression->value());
		this->AddRparentChild(node);	// RPARENT
	} else if (IsThisIdentifier(CHARCON)) {
		type = TypeSymbol::CHAR;
		string c = "\'" + iter_->value + "\'";
		node->set_value(c);
		this->AddChild(node);
	} else {
		type = TypeSymbol::INT;
		int integer = this->AnalyzeInteger(this->AddSyntaxChild(INTEGER, node));
		node->set_value(to_string(integer));
	}
	return type;
}

TypeSymbol ParseAnalyser::AnalyzeItem(SyntaxNode* node) {
	TypeSymbol type;
	string op;
	int factorCount = 1;
	SyntaxNode* factorRoot = NULL;

	while (true) {
		SyntaxNode* anotherFactorRoot = this->AddSyntaxChild(FACTOR, node);
		int regNumber = reg_count_;
		type = this->AnalyzeFactor(anotherFactorRoot);

		if (regNumber == reg_count_) {
			if (factorCount == 1) {
				factorRoot = anotherFactorRoot;
			} else if (factorCount == 2) {
				if (factorRoot == NULL) {
					midcode_generator_->PrintRegOpNumber(reg_count_, regNumber - 1,
						anotherFactorRoot->value(), op);
					reg_count_++;
				} else {
					midcode_generator_->PrintNumberOpNumber(reg_count_,
						factorRoot->value(),
						anotherFactorRoot->value(), op);
					reg_count_++;
				}
			} else {
				midcode_generator_->PrintRegOpNumber(reg_count_, regNumber - 1,
					anotherFactorRoot->value(), op);
				reg_count_++;
			}
		} else {
			if (factorCount == 2 && factorRoot != NULL) {
				midcode_generator_->PrintNumberOpReg(reg_count_,
					factorRoot->value(), reg_count_ - 1, op);
				reg_count_++;
			} else if (factorCount != 1) {
				midcode_generator_->PrintRegOpReg(reg_count_, regNumber - 1, reg_count_ - 1, op);
				reg_count_++;
			}
		}

		if (this->IsMultOrDiv()) {
			op = iter_->value;
			factorCount++;
			this->AddChild(node);
		} else {
			break;
		}
	}

	if (factorCount == 1) {
		return type;
	} else {
		return TypeSymbol::INT;
	}
}

TypeSymbol ParseAnalyser::AnalyzeExpression(SyntaxNode* node) {
	TypeSymbol type;
	string op;
	SyntaxNode* itemRoot = NULL;
	int itemCount = 1;
	int firstOpNumber = 1;
	if (this->IsPlusOrMinu()) {
		firstOpNumber = this->IsThisIdentifier(MINU) ? -1 : 1;
		this->AddChild(node);	// PLUS or MINU
	}

	while (true) {
		SyntaxNode* anotherItemRoot = this->AddSyntaxChild(ITEM, node);
		int regNumber = reg_count_;
		type = this->AnalyzeItem(anotherItemRoot);

		if (regNumber == reg_count_) {
			if (itemCount == 1) {
				if (firstOpNumber == -1) {
					midcode_generator_->PrintNeg(reg_count_++,
						anotherItemRoot->GetFirstChildNumericalValue());
				} else {
					itemRoot = anotherItemRoot;
				}
			} else if (itemCount == 2) {
				if (itemRoot == NULL) {
					midcode_generator_->PrintRegOpNumber(reg_count_, regNumber - 1,
						anotherItemRoot->GetFirstChildNumericalValue(), op);
					reg_count_++;
				} else {
					midcode_generator_->PrintNumberOpNumber(reg_count_++,
						itemRoot->GetFirstChildNumericalValue(),
						anotherItemRoot->GetFirstChildNumericalValue(), op);
				}
			} else {
				midcode_generator_->PrintRegOpNumber(reg_count_, regNumber - 1,
					anotherItemRoot->GetFirstChildNumericalValue(), op);
				reg_count_++;
			}
		} else {
			if (itemCount == 2 && itemRoot != NULL) {
				midcode_generator_->PrintNumberOpReg(reg_count_,
					itemRoot->GetFirstChildNumericalValue(), reg_count_ - 1, op);
				reg_count_++;
			} else if (itemCount != 1) {
				midcode_generator_->PrintRegOpReg(reg_count_, regNumber - 1, reg_count_ - 1, op);
				reg_count_++;
			} else {
				if (firstOpNumber == -1) {
					midcode_generator_->PrintNeg(reg_count_,
						anotherItemRoot->GetFirstChildNumericalValue());
					reg_count_++;
				}
			}
		}

		if (this->IsPlusOrMinu()) {
			op = iter_->value;
			itemCount++;
			this->AddChild(node);
		} else {
			break;
		}
	}

	if (itemCount == 1 && itemRoot != NULL) {
		node->set_value(itemRoot->GetFirstChildNumericalValue());
	} else {
		node->set_value("#" + to_string(reg_count_ - 1));
	}

	if (itemCount == 1) {
		return type;
	} else {
		return TypeSymbol::INT;
	}
}

void ParseAnalyser::AnalyzeCondition(SyntaxNode* node,  bool 
	isFalseBranch,int label_count) {
	SyntaxNode* expression1 = this->AddSyntaxChild(EXPRESSION, node);
	if (this->AnalyzeExpression(expression1) != TypeSymbol::INT) {
		this->CountIterator(-1);
		error_handing_->AddError(iter_->line_number, ILLEGAL_TYPE_IN_IF);
		this->CountIterator(+1);
	}
	if (this->IsThisIdentifier(LSS)
		|| this->IsThisIdentifier(LEQ)
		|| this->IsThisIdentifier(GRE)
		|| this->IsThisIdentifier(GEQ)
		|| this->IsThisIdentifier(EQL)
		|| this->IsThisIdentifier(NEQ)) {
		string op = iter_->value;
		this->AddChild(node);
		SyntaxNode* expression2 = this->AddSyntaxChild(EXPRESSION, node);
		if (this->AnalyzeExpression(expression2) != TypeSymbol::INT) {
			this->CountIterator(-1);
			error_handing_->AddError(iter_->line_number, ILLEGAL_TYPE_IN_IF);
			this->CountIterator(+1);
		}
		if (op == "==") {
			midcode_generator_->PrintBeqOrBne(label_count, expression1->value(),
				expression2->value(), Judge::BEQ, isFalseBranch);
		} else if (op == "!=") {
			midcode_generator_->PrintBeqOrBne(label_count, expression1->value(),
				expression2->value(), Judge::BNE, isFalseBranch);
		} else if (op == "<") {
			midcode_generator_->PrintBgeOrBlt(label_count, expression1->value(),
				expression2->value(), Judge::BLT, isFalseBranch);
		} else if (op == ">=") {
			midcode_generator_->PrintBgeOrBlt(label_count, expression1->value(),
				expression2->value(), Judge::BGE, isFalseBranch);
		} else if (op == "<=") {
			midcode_generator_->PrintBgtOrBle(label_count, expression1->value(),
				expression2->value(), Judge::BLE, isFalseBranch);
		} else if (op == ">") {
			midcode_generator_->PrintBgtOrBle(label_count, expression1->value(),
				expression2->value(), Judge::BGT, isFalseBranch);
		}
	} else {
		midcode_generator_->PrintBezOrBnz(label_count,
			expression1->value(), isFalseBranch);
	}
}

bool ParseAnalyser::AnalyzeIfSentence(SyntaxNode* node, TypeSymbol returnType) {
	bool noReturn = true;

	this->AddChild(node);	// IFTK
	int elseLabel = ++label_count_;

	this->AddChild(node);	// LPARENT
	this->AnalyzeCondition(this->AddSyntaxChild(CONDITION, node), true, elseLabel);
	this->AddRparentChild(node);	// RPARENT
	noReturn = this->AnalyzeSentence(this->AddSyntaxChild(SENTENCE, node), returnType);

	int endifLabel = ++label_count_;
	midcode_generator_->PrintJump(endifLabel);
	midcode_generator_->PrintLabel(elseLabel);

	if (this->IsThisIdentifier(ELSETK)) {
		this->AddChild(node);	// ELSETK
		noReturn = this->AnalyzeSentence(this->AddSyntaxChild(SENTENCE, node), returnType) && noReturn;
	}
	midcode_generator_->PrintLabel(endifLabel);

	return noReturn;
}

int ParseAnalyser::AnalyzeStep(SyntaxNode* node) {
	int step = stoi(iter_->value);
	this->AddChild(this->AddSyntaxChild(UNSIGNINT, node));	// INTCON
	return step;
}

void ParseAnalyser::AnalyzeWhile(SyntaxNode* node, TypeSymbol returnType) {
	this->AddChild(node);	// WHILETK
	int whileLabel = ++label_count_;
	midcode_generator_->PrintLabel(whileLabel);
	midcode_generator_->PrintLoop();

	this->AddChild(node);	// LPARENT
	int endWhileLabel = ++label_count_;
	this->AnalyzeCondition(this->AddSyntaxChild(CONDITION, node), true, endWhileLabel);
	this->AddRparentChild(node);	// RPARENT

	this->AnalyzeSentence(this->AddSyntaxChild(SENTENCE, node), returnType);

	midcode_generator_->PrintJump(whileLabel);
	midcode_generator_->PrintLabel(endWhileLabel);
}

void ParseAnalyser::AnalyzeDoWhile(SyntaxNode* node, bool& noReturn, TypeSymbol returnType) {
	this->AddChild(node);	// DOTK

	int doLabel = ++label_count_;
	midcode_generator_->PrintLabel(doLabel);
	midcode_generator_->PrintLoop();

	noReturn = this->AnalyzeSentence(this->AddSyntaxChild(SENTENCE, node), returnType);

	this->AddWhileChild(node);	// WHILETK
	this->AddChild(node);	// LPARENT
	this->AnalyzeCondition(this->AddSyntaxChild(CONDITION, node), false, doLabel);
	this->AddRparentChild(node);	// RPARENT
}

void ParseAnalyser::AnalyzeFor(SyntaxNode* node, TypeSymbol returnType) {
	this->AddChild(node);	// FORTK
	this->AddChild(node);	// LPARENT
	if (this->FindSymbol() == NULL) {
		error_handing_->AddError(iter_->line_number, UNDEFINED);
	}

	string name = iter_->value;
	this->AddChild(node);	// IDENFR
	this->AddChild(node);	// ASSIGN
	SyntaxNode* expressionNode = this->AddSyntaxChild(EXPRESSION, node);
	this->AnalyzeExpression(expressionNode);
	midcode_generator_->PrintAssignValue(name, "", expressionNode->value());

	int forLabel = ++label_count_;
	midcode_generator_->PrintLabel(forLabel);
	midcode_generator_->PrintLoop();

	this->AddSemicnChild(node);	// SEMICN

	int endForLabel = ++label_count_;
	this->AnalyzeCondition(this->AddSyntaxChild(CONDITION, node), true, endForLabel);

	this->AddSemicnChild(node);	// SEMICN

	if (this->FindSymbol() == NULL) {
		error_handing_->AddError(iter_->line_number, UNDEFINED);
	}
	string name1 = iter_->value;
	this->AddChild(node);	// IDENFR
	this->AddChild(node);	// ASSIGN
	if (this->FindSymbol() == NULL) {
		error_handing_->AddError(iter_->line_number, UNDEFINED);
	}
	string name2 = iter_->value;
	this->AddChild(node);	// IDENFR
	string op = iter_->value;
	this->AddChild(node);	// PLUS or MINU
	int step = this->AnalyzeStep(this->AddSyntaxChild(STEP, node));
	this->AddRparentChild(node);	// RPARENT
	this->AnalyzeSentence(this->AddSyntaxChild(SENTENCE, node), returnType);
	midcode_generator_->PrintStep(name1, name2, op, step);

	midcode_generator_->PrintJump(forLabel);
	midcode_generator_->PrintLabel(endForLabel);
}

bool ParseAnalyser::AnalyzeLoopSentence(SyntaxNode* node, TypeSymbol returnType) {
	bool noReturn = true;
	if (this->IsThisIdentifier(WHILETK)) {
		this->AnalyzeWhile(node, returnType);
	} else if (this->IsThisIdentifier(DOTK)) {
		this->AnalyzeDoWhile(node, noReturn, returnType);
	} else if (this->IsThisIdentifier(FORTK)) {
		this->AnalyzeFor(node, returnType);
	}
	return noReturn;
}

void ParseAnalyser::AnalyzeAssignSentence(SyntaxNode* node) {
	KindSymbol kind;
	if (this->FindSymbol() == NULL) {
		error_handing_->AddError(iter_->line_number, UNDEFINED);
	} else {
		kind = this->FindSymbol()->kind();
		if (kind == KindSymbol::CONST) {
			error_handing_->AddError(iter_->line_number, ASSIGN_TO_CONST);
		}
	}
	string name = iter_->value;
	string arrayIndex = "";
	this->AddChild(node);	// IDENFR

	SyntaxNode* expressionNode;
	if (this->IsThisIdentifier(LBRACK)) {
		this->AddChild(node);	// LBRACK
		expressionNode = this->AddSyntaxChild(EXPRESSION, node);
		if (this->AnalyzeExpression(expressionNode) == TypeSymbol::CHAR) {
			error_handing_->AddError(iter_->line_number, ILLEGAL_ARRAY_INDEX);
		}
		arrayIndex = expressionNode->value();
		this->AddRbrackChild(node);	// RBRACK
	}
	this->AddChild(node);	// ASSIGN
	expressionNode = this->AddSyntaxChild(EXPRESSION, node);
	this->AnalyzeExpression(expressionNode);
	midcode_generator_->PrintAssignValue(name, arrayIndex, expressionNode->value());
}

void ParseAnalyser::AnalyzeScanfIdentifier(SyntaxNode* node) {
	if (this->FindSymbol() == NULL) {
		error_handing_->AddError(iter_->line_number, UNDEFINED);
	} else {
		Symbol* symbol = this->FindSymbol(0) != NULL ? this->FindSymbol(0) : this->FindSymbol(1);
		string type = symbol->type() == TypeSymbol::INT ? kIntType : kCharType;
		midcode_generator_->PrintScanf(type, symbol->name());
	}
	this->AddChild(node);	// IDENFR
}

void ParseAnalyser::AnalyzeScanfSentence(SyntaxNode* node) {
	this->AddChild(node);	// SCANFTK
	this->AddChild(node);	// LPARENT

	this->AnalyzeScanfIdentifier(node);
	while (this->IsThisIdentifier(COMMA)) {
		this->AddChild(node);	// COMMA
		this->AnalyzeScanfIdentifier(node);
	}

	this->AddRparentChild(node);	// RPARENT
}

void ParseAnalyser::AnalyzePrintfSentence(SyntaxNode* node) {
	this->AddChild(node);	// PRINTFTK
	this->AddChild(node);	// LPARENT

	if (this->IsThisIdentifier(STRCON)) {
		int stringNumber = string_table_->AddString(iter_->value);
		midcode_generator_->PrintString(stringNumber);
		this->AddChild(AddSyntaxChild(STRING, node));
		if (this->IsThisIdentifier(COMMA)) {
			this->AddChild(node);	// COMMA
			SyntaxNode* expression = this->AddSyntaxChild(EXPRESSION, node);
			TypeSymbol type = this->AnalyzeExpression(expression);
			if (type == TypeSymbol::INT) {
				midcode_generator_->PrintInteger(expression->value());
			} else {
				midcode_generator_->PrintChar(expression->value());
			}
		}
	} else {
		SyntaxNode* expression = this->AddSyntaxChild(EXPRESSION, node);
		TypeSymbol type = this->AnalyzeExpression(expression);
		if (type == TypeSymbol::INT) {
			midcode_generator_->PrintInteger(expression->value());
		} else {
			midcode_generator_->PrintChar(expression->value());
		}
	}
	midcode_generator_->PrintEnd();

	this->AddRparentChild(node);	// RPARENT
}

TypeSymbol ParseAnalyser::AnalyzeReturnSentence(SyntaxNode* node) {
	TypeSymbol type = TypeSymbol::VOID;
	this->AddChild(node);	// RETURNTK
	if (this->IsThisIdentifier(LPARENT)) {
		this->AddChild(node);	// LPARENT
		SyntaxNode* expression_root = this->AddSyntaxChild(EXPRESSION, node);
		type = this->AnalyzeExpression(expression_root);
		midcode_generator_->PrintReturn(false, expression_root->value());
		this->AddRparentChild(node);	// RPARENT
	} else {
		midcode_generator_->PrintReturn(true, "");
	}
	return type;
}

bool ParseAnalyser::AnalyzeSentence(SyntaxNode* node, TypeSymbol returnType) {
	bool noReturn = true;
	if (this->IsThisIdentifier(IFTK)) {
		noReturn = this->AnalyzeIfSentence(this->AddSyntaxChild(IF_SENTENCE, node), returnType);
	} else if (this->IsThisIdentifier(WHILETK)
		|| this->IsThisIdentifier(DOTK)
		|| this->IsThisIdentifier(FORTK)) {
		noReturn = this->AnalyzeLoopSentence(this->AddSyntaxChild(LOOP_SENTENCE, node), returnType);
	} else if (this->IsThisIdentifier(LBRACE)) {
		this->AddChild(node);	// LBRACE
		noReturn = this->AnalyzeSentenceCollection(this->AddSyntaxChild(SENTENCE_COLLECTION, node), returnType);
		this->AddChild(node);	// RBRACE
	} else if (this->IsThisIdentifier(IDENFR)) {
		noReturn = true;
		if (this->FindSymbol(0) != NULL
			&& this->FindSymbol(0)->kind() == KindSymbol::FUNCTION
			&& this->FindSymbol(0)->type() != TypeSymbol::VOID) {
			this->AnalyzeReturnCallSentence(this->AddSyntaxChild(RETURN_CALL_SENTENCE, node));
			this->AddSemicnChild(node);	// SEMICN
		} else if (this->FindSymbol(0) != NULL
			&& this->FindSymbol(0)->kind() == KindSymbol::FUNCTION
			&& this->FindSymbol(0)->type() == TypeSymbol::VOID) {
			this->AnalyzeReturnCallSentence(this->AddSyntaxChild(NO_RETURN_CALL_SENTENCE, node));
			this->AddSemicnChild(node);	// SEMICN
		} else {
			if (this->FindSymbol() != NULL) {
				this->AnalyzeAssignSentence(this->AddSyntaxChild(ASSIGN_SENTENCE, node));
				this->AddSemicnChild(node);	// SEMICN
			} else if (this->FindSymbol(0) == NULL) {
				if (this->FindSymbol(1) == NULL) {
					error_handing_->AddError(iter_->line_number, UNDEFINED);
				}
				this->CountIterator(+1);
				if (this->IsThisIdentifier(LPARENT)) {
					while (!this->IsThisIdentifier(RPARENT)) {
						this->CountIterator(+1);
					}
					this->CountIterator(+1);
				} else if (this->IsThisIdentifier(ASSIGN)) {
					while (!this->IsThisIdentifier(SEMICN)) {
						this->CountIterator(+1);
					}
					this->CountIterator(+1);
				}
			}
		}
	} else if (this->IsThisIdentifier(SCANFTK)) {
		noReturn = true;
		this->AnalyzeScanfSentence(this->AddSyntaxChild(SCANF_SENTENCE, node));
		this->AddSemicnChild(node);	// SEMICN
	} else if (this->IsThisIdentifier(PRINTFTK)) {
		noReturn = true;
		this->AnalyzePrintfSentence(this->AddSyntaxChild(PRINTF_SENTENCE, node));
		this->AddSemicnChild(node);	// SEMICN
	} else if (this->IsThisIdentifier(SEMICN)) {
		noReturn = true;
		this->AddSemicnChild(node);	// SEMICN
	} else if (this->IsThisIdentifier(RETURNTK)) {
		TypeSymbol type = this->AnalyzeReturnSentence(this->AddSyntaxChild(RETURN_SENTENCE, node));
		noReturn = type == TypeSymbol::VOID;
		this->CountIterator(-1);
		if (returnType == TypeSymbol::VOID && type != TypeSymbol::VOID) {
			error_handing_->AddError(iter_->line_number, RETURN_IN_NO_RETURN_FUNCTION);
		} else if (returnType != type) {
			error_handing_->AddError(iter_->line_number, NO_RETURN_OR_WRONG_RETURN_IN_RETURN_FUNCTION);
		}
		this->CountIterator(+1);
		this->AddSemicnChild(node);	// SEMICN
	}
	return noReturn;
}

bool ParseAnalyser::AnalyzeSentenceCollection(SyntaxNode* node, TypeSymbol returnType) {
	bool noReturn = true;
	while (!this->IsThisIdentifier(RBRACE)) {
		if (!this->AnalyzeSentence(this->AddSyntaxChild(SENTENCE, node), returnType)) {
			noReturn = false;
		}
	}

	return noReturn;
}

void ParseAnalyser::AnalyzeCompositeSentence(SyntaxNode* node, TypeSymbol returnType) {
	if (this->IsThisIdentifier(CONSTTK)) {
		this->AnalyzeConstDeclare(this->AddSyntaxChild(CONST_DECLARE, node), 1);
	}
	if (this->IsThisIdentifier(INTTK) || this->IsThisIdentifier(CHARTK)) {
		this->AnalyzeVariableDeclare(this->AddSyntaxChild(VARIABLE_DECLARE, node), 1);
	}
	bool noReturn = this->AnalyzeSentenceCollection(
		this->AddSyntaxChild(SENTENCE_COLLECTION, node), returnType);
	if (returnType != TypeSymbol::VOID && noReturn) {
		this->CountIterator(-1);
		error_handing_->AddError(iter_->line_number, NO_RETURN_OR_WRONG_RETURN_IN_RETURN_FUNCTION);
		this->CountIterator(+1);
	}
}

void ParseAnalyser::AnalyzeMain(SyntaxNode* node) {
	int temp_reg = reg_count_;
	this->AddChild(node);	// VOIDTK
	this->InsertIdentifier(KindSymbol::FUNCTION, TypeSymbol::VOID, 0);
	Symbol* function = this->FindSymbol(0);
	this->CleanLevel(1);
	this->AddChild(node);	// MAINTK
	this->AddChild(node);	// LPARENT
	this->AddRparentChild(node);	// RPARENT
	midcode_generator_->PrintVoidFuncDeclare(function);
	this->AddChild(node);	// LBRACE
	this->AnalyzeCompositeSentence(this->AddSyntaxChild(COMPOSITE_SENTENCE, node), TypeSymbol::VOID);
	midcode_generator_->PrintReturn(true, "");
	this->AddChild(node);	// RBRACE

	midcode_generator_->PrintFuncEnd();
	check_table_->AddFunctionVariableNumber(function->name(), reg_count_ - temp_reg);
	this->InsertSymbolTable(function->name(), 1);
}

void ParseAnalyser::AnalyzeParameterTable(SyntaxNode* node, Symbol* function) {
	while (this->IsThisIdentifier(INTTK) || this->IsThisIdentifier(CHARTK)) {

		TypeSymbol type;
		this->SetSymbolType(type);
		this->AddChild(node);	// INTTK or CHARTK

		if (this->FindSymbol(1) != NULL) {
			error_handing_->AddError(iter_->line_number, REDEFINITION);
		}
		function->AddParameter(type == TypeSymbol::INT ? '0' : '1');
		this->InsertIdentifier(KindSymbol::PARAMETER, type, 1);
		this->midcode_generator_->PrintParameter(type, iter_->value);
		this->AddChild(node);	// IDENTFR
		this->AddCommaChild(node);
	}
}

void ParseAnalyser::AnalyzeVoidFunc(SyntaxNode* node) {
	int temp_reg = reg_count_;
	this->AddChild(node);	// VOIDTK
	if (this->FindSymbol(0) != NULL) {
		error_handing_->AddError(iter_->line_number, REDEFINITION);
	}
	this->InsertIdentifier(KindSymbol::FUNCTION, TypeSymbol::VOID, 0);
	Symbol* function = FindSymbol(0);
	this->CleanLevel(1);
	this->AddChild(node);	// IDENFR
	this->midcode_generator_->PrintVoidFuncDeclare(function);
	this->AddChild(node);	// LPARENT
	this->AnalyzeParameterTable(
		this->AddSyntaxChild(PARAMETER_TABLE, node), function);
	this->AddRparentChild(node);	// RPARENT
	this->AddChild(node);	// LBRACE
	this->AnalyzeCompositeSentence(this->AddSyntaxChild(COMPOSITE_SENTENCE, node), TypeSymbol::VOID);
	this->midcode_generator_->PrintReturn(true, "");
	this->AddChild(node);	// RBRACE

	this->midcode_generator_->PrintFuncEnd();
	this->check_table_->AddFunctionVariableNumber(function->name(), reg_count_ - temp_reg);
	this->InsertSymbolTable(function->name(), 1);
}

void ParseAnalyser::AnalyzeHeadState(SyntaxNode* node, Symbol*& function) {
	TypeSymbol type;
	this->SetSymbolType(type);
	this->AddChild(node);	// INTTK or CHARTK
	if (this->FindSymbol(0) != NULL) {
		error_handing_->AddError(iter_->line_number, REDEFINITION);
	}
	this->InsertIdentifier(KindSymbol::FUNCTION, type, 0);
	function = this->FindSymbol(0);
	this->CleanLevel(1);
	this->AddChild(node);	// IDENFR
}

void ParseAnalyser::AnalyzeFunc(SyntaxNode* node) {
	Symbol* function = new Symbol();
	int temp_reg = reg_count_;
	this->AnalyzeHeadState(this->AddSyntaxChild(HEAD_STATE, node), function);
	this->midcode_generator_->PrintFuncDeclare(function);
	this->AddChild(node);	// LPARENT
	this->AnalyzeParameterTable(
		this->AddSyntaxChild(PARAMETER_TABLE, node), function);
	this->AddRparentChild(node);	// RPARENT
	this->AddChild(node);	// LBRACE
	this->AnalyzeCompositeSentence(this->AddSyntaxChild(COMPOSITE_SENTENCE, node),
		function->type());
	this->AddChild(node);	// RBRACE

	midcode_generator_->PrintFuncEnd();
	check_table_->AddFunctionVariableNumber(function->name(), reg_count_ - temp_reg);
	this->InsertSymbolTable(function->name(), 1);
}

void ParseAnalyser::BuildSyntaxTree(SyntaxNode* root) {
	string flag = CONST_DECLARE;

	int temp_reg = reg_count_;
	bool first = true;

	while (iter_ != iter_end_) {
		if (this->IsThisIdentifier(CONSTTK)) {
			this->AnalyzeConstDeclare(this->AddSyntaxChild(CONST_DECLARE, root), 0);
		} else if (this->IsThisIdentifier(INTTK) || this->IsThisIdentifier(CHARTK)) {
			if (flag == CONST_DECLARE) {
				this->CountIterator(+2);
				if (this->IsThisIdentifier(LPARENT)) {
					flag = RETURN_FUNCTION;
				} else {
					flag = VARIABLE_DECLARE;
				}
				this->CountIterator(-2);
			}
			if (flag == RETURN_FUNCTION) {
				this->AnalyzeFunc(this->AddSyntaxChild(RETURN_FUNCTION, root));
			} else {
				this->AnalyzeVariableDeclare(this->AddSyntaxChild(VARIABLE_DECLARE, root), 0);
				flag = RETURN_FUNCTION;
			}
		} else {
			if (first) {
				check_table_->AddFunctionVariableNumber("global", reg_count_ - temp_reg);
				first = false;
			}

			if (this->IsThisIdentifier(VOIDTK)) {
				this->CountIterator(+1);
			}
			if (this->IsThisIdentifier(MAINTK)) {
				this->CountIterator(-1);
				this->AnalyzeMain(this->AddSyntaxChild(MAIN_FUNCTION, root));
			} else {
				this->CountIterator(-1);
				this->AnalyzeVoidFunc(this->AddSyntaxChild(NO_RETURN_FUNCTION, root));
			}
		}
	}
	this->InsertSymbolTable("global", 0);
}

void ParseAnalyser::AnalyzeParse() {
	SyntaxNode* root = new SyntaxNode(PROGRAM);
	this->BuildSyntaxTree(root);
	// root->print();
}

map<string, SymbolTable*> ParseAnalyser::symbol_table_map() {
	return this->symbol_table_map_;
}

list<Midcode*> ParseAnalyser::midcode_list() {
	return this->midcode_generator_->midcode_list();
}

StringTable* ParseAnalyser::string_table() {
	return this->string_table_;
}

CheckTable* ParseAnalyser::check_table() {
	return check_table_;
}

int ParseAnalyser::reg_count() {
	return reg_count_;
}

void ParseAnalyser::FileClose() {
	midcode_generator_->FileClose();
}