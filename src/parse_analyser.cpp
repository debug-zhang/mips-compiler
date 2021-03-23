#include "parse_analyser.h"

#include <utility>

using namespace std;

ParseAnalyser::ParseAnalyser(const string &fileName, list<struct Lexeme> *lexList, ErrorHanding *errorHanding) {
    check_table_ = new CheckTable();
    string_table_ = new StringTable();
    midcode_generator_ = new MidcodeGenerator();

    label_count_ = 0;
    reg_count_ = 1;
    midcode_generator_->OpenMidcodeFile(fileName);
    iter_ = lexList->begin();
    iter_end_ = lexList->end();
    error_handing_ = errorHanding;
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

Symbol *ParseAnalyser::InsertIdentifier(KindSymbol kind, TypeSymbol type, int level) {
    return check_table_->AddSymbol(iter_->value, kind, type, level);
}

void ParseAnalyser::InsertSymbolTable(const string &name, int level) {
    symbol_table_map_.insert(pair<string, SymbolTable *>(name, check_table_->GetSymbolTable(level)));
}

void ParseAnalyser::CleanLevel(int level) {
    check_table_->ClearLevel(level);
}

Symbol *ParseAnalyser::FindSymbol() {
    return check_table_->FindSymbol(iter_->value);
}

Symbol *ParseAnalyser::FindSymbol(int level) {
    return check_table_->FindSymbol(iter_->value, level);
}

bool ParseAnalyser::IsThisIdentifier(const string &identifier) {
    return iter_->identifier == identifier;
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

void ParseAnalyser::AddChild(SyntaxNode *node) {
    node->AddChild(new SyntaxNode(iter_->identifier, iter_->value));
    CountIterator(+1);
}

void ParseAnalyser::AddCommaChild(SyntaxNode *node) {
    if (IsThisIdentifier(COMMA)) {
        AddChild(node);
    }
}

void ParseAnalyser::AddSemicnChild(SyntaxNode *node) {
    if (IsThisIdentifier(SEMICN)) {
        AddChild(node);    // SEMICN
    } else {
        CountIterator(-1);
        error_handing_->AddError(iter_->line_number, MISSING_SEMICN);
        CountIterator(+1);
    }
}

void ParseAnalyser::AddWhileChild(SyntaxNode *node) {
    if (IsThisIdentifier(WHILETK)) {
        AddChild(node);    // WHILETK
    } else {
        CountIterator(-1);
        error_handing_->AddError(iter_->line_number, MISSING_WHILE_IN_DO_WHILE);
        CountIterator(+1);
    }
}

void ParseAnalyser::AddRbrackChild(SyntaxNode *node) {
    if (IsThisIdentifier(RBRACK)) {
        AddChild(node);    // RBRACK
    } else {
        CountIterator(-1);
        error_handing_->AddError(iter_->line_number, MISSING_RBRACK);
        CountIterator(+1);
    }
}

void ParseAnalyser::AddRparentChild(SyntaxNode *node) {
    if (IsThisIdentifier(RPARENT)) {
        AddChild(node);    // RPARENT
    } else {
        CountIterator(-1);
        error_handing_->AddError(iter_->line_number, MISSING_RPARENT);
        CountIterator(+1);
    }
}

SyntaxNode *ParseAnalyser::AddSyntaxChild(const string &syntaxName, SyntaxNode *node) {
    auto subRoot = new SyntaxNode(syntaxName);
    node->AddChild(subRoot);
    return subRoot;
}

void ParseAnalyser::SetSymbolType(TypeSymbol &type) {
    type = iter_->identifier == INTTK ? TypeSymbol::INT : TypeSymbol::CHAR;
}

int ParseAnalyser::AnalyzeInteger(SyntaxNode *node) {
    int op = 1;
    if (IsPlusOrMinu()) {
        if (IsThisIdentifier(MINU)) {
            op = -1;
        }
        AddChild(node);
    }

    int integer = op * stoi(iter_->value);
    AddChild(AddSyntaxChild(UNSIGNINT, node));        // INTCON

    return integer;
}

void ParseAnalyser::AnalyzeConstDefine(SyntaxNode *node, int level) {
    auto symbol = new Symbol();
    TypeSymbol type;
    SetSymbolType(type);
    AddChild(node);    // INTTK or CHARTK

    while (IsThisIdentifier(IDENFR)) {
        if (FindSymbol(level)) {
            error_handing_->AddError(iter_->line_number, REDEFINITION);
        } else {
            symbol = InsertIdentifier(KindSymbol::CONST, type, level);
        }
        AddChild(node);    // IDENFR
        AddChild(node);    // ASDSIGN
        if (IsThisIdentifier(CHARCON)) {
            symbol->set_const_value("\'" + iter_->value + "\'");
            AddChild(node);    // CHARCON
        } else if (IsThisIdentifier(INTCON) || IsPlusOrMinu()) {
            int value = AnalyzeInteger(AddSyntaxChild(INTEGER, node));
            symbol->set_const_value(to_string(value));
        } else {
            error_handing_->AddError(iter_->line_number, DEFINE_CONST_OTHERS);
            AddChild(node);
        }
        AddCommaChild(node);    // COMMA
    }
}

void ParseAnalyser::AnalyzeConstDeclare(SyntaxNode *node, int level) {
    while (IsThisIdentifier(CONSTTK)) {
        AddChild(node);    // CONSTTK
        AnalyzeConstDefine(AddSyntaxChild(CONST_DEFINE, node), level);
        AddSemicnChild(node);    // SEMICN
    }
}

void ParseAnalyser::AnalyzeVariableDefine(SyntaxNode *node, int level) {
    TypeSymbol type;
    SetSymbolType(type);
    AddChild(node);    // INTTK or CHARTK
    while (IsThisIdentifier(IDENFR)) {
        if (FindSymbol(level)) {
            error_handing_->AddError(iter_->line_number, REDEFINITION);
        }
        Symbol *symbol = InsertIdentifier(KindSymbol::VARIABLE, type, level);
        midcode_generator_->PrintVariable(type, iter_->value);
        AddChild(node);    // IDENFR

        if (IsThisIdentifier(LBRACK)) {
            AddChild(node);    // LBRACK
            int arrayLength = stoi(iter_->value);
            AddChild(AddSyntaxChild(UNSIGNINT, node));    // INTCON
            AddRbrackChild(node);

            symbol->set_kind(KindSymbol::ARRAY);
            symbol->set_array_length(arrayLength);
        }
        AddCommaChild(node);    // COMMA
    }
}

void ParseAnalyser::AnalyzeVariableDeclare(SyntaxNode *node, int level) {
    while (IsVariableDefine()) {
        AnalyzeVariableDefine(AddSyntaxChild(VARIABLE_DEFINE, node), level);
        AddSemicnChild(node);    // SEMICN
    }
}

void ParseAnalyser::AnalyzeValuePrameterTable(SyntaxNode *node, Symbol *function) {
    if (IsThisIdentifier(RPARENT)) {
        if (function != nullptr && function->GetParameterCount() != 0) {
            error_handing_->AddError(iter_->line_number, FUNCTION_PARAMETER_NUMBER_DONT_MATCH);
        }
        return;
    }

    string str;
    TypeSymbol type;
    int count = 0;
    if (!IsThisIdentifier(RPARENT)) {
        SyntaxNode *expressionNode;
        expressionNode = AddSyntaxChild(EXPRESSION, node);
        type = AnalyzeExpression(expressionNode);
        str.push_back(type == TypeSymbol::INT ? '0' : '1');
        midcode_generator_->PrintPushParameter(function->name(),
                                               expressionNode->value(), count++);
        while (IsThisIdentifier(COMMA)) {
            AddChild(node);    // COMMA
            expressionNode = AddSyntaxChild(EXPRESSION, node);
            type = AnalyzeExpression(expressionNode);
            str.push_back(type == TypeSymbol::INT ? '0' : '1');
            midcode_generator_->PrintPushParameter(function->name(),
                                                   expressionNode->value(), count++);
        }
    }

    if (function != nullptr && function->GetParameterCount() != str.length()) {
        error_handing_->AddError(iter_->line_number, FUNCTION_PARAMETER_NUMBER_DONT_MATCH);
    } else if (function != nullptr && function->parameter() != str) {
        error_handing_->AddError(iter_->line_number, FUNCTION_PARAMETER_TYPE_DONT_MATCH);
    }
}

void ParseAnalyser::AnalyzeReturnCallSentence(SyntaxNode *node) {
    if (FindSymbol(0) == nullptr) {
        error_handing_->AddError(iter_->line_number, UNDEFINED);
    }
    Symbol *function = FindSymbol(0);
    midcode_generator_->PrintSave(function->name());
    AddChild(node);    // IDENFR
    AddChild(node);    // LPARENT
    AnalyzeValuePrameterTable(
            AddSyntaxChild(VALUE_PARAMETER_TABLE, node), function);
    AddRparentChild(node);    // RPARENT
    midcode_generator_->PrintCallFunction(function->name());
}

TypeSymbol ParseAnalyser::AnalyzeFactor(SyntaxNode *node) {
    TypeSymbol type;
    if (IsThisIdentifier(IDENFR)) {
        if (FindSymbol(0) != nullptr
            && FindSymbol(0)->kind() == KindSymbol::FUNCTION
            && FindSymbol(0)->type() != TypeSymbol::VOID) {
            type = FindSymbol(0)->type();
            AnalyzeReturnCallSentence(AddSyntaxChild(RETURN_CALL_SENTENCE, node));
            midcode_generator_->PrintAssignReturn(reg_count_++);
        } else {
            Symbol *symbol = FindSymbol();
            if (symbol == nullptr) {
                error_handing_->AddError(iter_->line_number, UNDEFINED);
                type = TypeSymbol::INT;
            } else {
                type = symbol->type();
            }
            string name = iter_->value;
            AddChild(node);    // IDENFR
            if (IsThisIdentifier(LBRACK)) {
                AddChild(node);    // LBRACK
                SyntaxNode *expression = AddSyntaxChild(EXPRESSION, node);
                TypeSymbol expressType = AnalyzeExpression(expression);
                if (expressType != TypeSymbol::INT) {
                    error_handing_->AddError(iter_->line_number, ILLEGAL_ARRAY_INDEX);
                }
                midcode_generator_->PrintLoadToTempReg(name,
                                                       expression->value(), reg_count_++);
                AddRbrackChild(node);    // RBRACK
            } else {
                node->set_value(name);
            }
        }
    } else if (IsThisIdentifier(LPARENT)) {
        AddChild(node);    // LPARENT
        SyntaxNode *expression = AddSyntaxChild(EXPRESSION, node);
        type = AnalyzeExpression(expression);
        node->set_value(expression->value());
        AddRparentChild(node);    // RPARENT
    } else if (IsThisIdentifier(CHARCON)) {
        type = TypeSymbol::CHAR;
        string c = "\'" + iter_->value + "\'";
        node->set_value(c);
        AddChild(node);
    } else {
        type = TypeSymbol::INT;
        int integer = AnalyzeInteger(AddSyntaxChild(INTEGER, node));
        node->set_value(to_string(integer));
    }
    return type;
}

TypeSymbol ParseAnalyser::AnalyzeItem(SyntaxNode *node) {
    TypeSymbol type;
    string op;
    int factorCount = 1;
    SyntaxNode *factorRoot = nullptr;

    while (true) {
        SyntaxNode *anotherFactorRoot = AddSyntaxChild(FACTOR, node);
        int regNumber = reg_count_;
        type = AnalyzeFactor(anotherFactorRoot);

        if (regNumber == reg_count_) {
            if (factorCount == 1) {
                factorRoot = anotherFactorRoot;
            } else if (factorCount == 2) {
                if (factorRoot == nullptr) {
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
            if (factorCount == 2 && factorRoot != nullptr) {
                midcode_generator_->PrintNumberOpReg(reg_count_,
                                                     factorRoot->value(), reg_count_ - 1, op);
                reg_count_++;
            } else if (factorCount != 1) {
                midcode_generator_->PrintRegOpReg(reg_count_, regNumber - 1, reg_count_ - 1, op);
                reg_count_++;
            }
        }

        if (IsMultOrDiv()) {
            op = iter_->value;
            factorCount++;
            AddChild(node);
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

TypeSymbol ParseAnalyser::AnalyzeExpression(SyntaxNode *node) {
    TypeSymbol type;
    string op;
    SyntaxNode *itemRoot = nullptr;
    int itemCount = 1;
    int firstOpNumber = 1;
    if (IsPlusOrMinu()) {
        firstOpNumber = IsThisIdentifier(MINU) ? -1 : 1;
        AddChild(node);    // PLUS or MINU
    }

    while (true) {
        SyntaxNode *anotherItemRoot = AddSyntaxChild(ITEM, node);
        int regNumber = reg_count_;
        type = AnalyzeItem(anotherItemRoot);

        if (regNumber == reg_count_) {
            if (itemCount == 1) {
                if (firstOpNumber == -1) {
                    midcode_generator_->PrintNeg(reg_count_++,
                                                 anotherItemRoot->GetFirstChildNumericalValue());
                } else {
                    itemRoot = anotherItemRoot;
                }
            } else if (itemCount == 2) {
                if (itemRoot == nullptr) {
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
            if (itemCount == 2 && itemRoot != nullptr) {
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

        if (IsPlusOrMinu()) {
            op = iter_->value;
            itemCount++;
            AddChild(node);
        } else {
            break;
        }
    }

    if (itemCount == 1 && itemRoot != nullptr) {
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

void ParseAnalyser::AnalyzeCondition(SyntaxNode *node, bool
isFalseBranch, int label_count) {
    SyntaxNode *expression1 = AddSyntaxChild(EXPRESSION, node);
    if (AnalyzeExpression(expression1) != TypeSymbol::INT) {
        CountIterator(-1);
        error_handing_->AddError(iter_->line_number, ILLEGAL_TYPE_IN_IF);
        CountIterator(+1);
    }
    if (IsThisIdentifier(LSS)
        || IsThisIdentifier(LEQ)
        || IsThisIdentifier(GRE)
        || IsThisIdentifier(GEQ)
        || IsThisIdentifier(EQL)
        || IsThisIdentifier(NEQ)) {
        string op = iter_->value;
        AddChild(node);
        SyntaxNode *expression2 = AddSyntaxChild(EXPRESSION, node);
        if (AnalyzeExpression(expression2) != TypeSymbol::INT) {
            CountIterator(-1);
            error_handing_->AddError(iter_->line_number, ILLEGAL_TYPE_IN_IF);
            CountIterator(+1);
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

bool ParseAnalyser::AnalyzeIfSentence(SyntaxNode *node, TypeSymbol returnType) {
    AddChild(node);    // IFTK
    int elseLabel = ++label_count_;

    AddChild(node);    // LPARENT
    AnalyzeCondition(AddSyntaxChild(CONDITION, node), true, elseLabel);
    AddRparentChild(node);    // RPARENT
    bool noReturn = AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType);

    int endifLabel = ++label_count_;
    midcode_generator_->PrintJump(endifLabel);
    midcode_generator_->PrintLabel(elseLabel);

    if (IsThisIdentifier(ELSETK)) {
        AddChild(node);    // ELSETK
        noReturn = AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType) && noReturn;
    }
    midcode_generator_->PrintLabel(endifLabel);

    return noReturn;
}

int ParseAnalyser::AnalyzeStep(SyntaxNode *node) {
    int step = stoi(iter_->value);
    AddChild(AddSyntaxChild(UNSIGNINT, node));    // INTCON
    return step;
}

void ParseAnalyser::AnalyzeWhile(SyntaxNode *node, TypeSymbol returnType) {
    AddChild(node);    // WHILETK
    int whileLabel = ++label_count_;
    midcode_generator_->PrintLabel(whileLabel);
    midcode_generator_->PrintLoop();

    AddChild(node);    // LPARENT
    int endWhileLabel = ++label_count_;
    AnalyzeCondition(AddSyntaxChild(CONDITION, node), true, endWhileLabel);
    AddRparentChild(node);    // RPARENT

    AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType);

    midcode_generator_->PrintJump(whileLabel);
    midcode_generator_->PrintLabel(endWhileLabel);
}

void ParseAnalyser::AnalyzeDoWhile(SyntaxNode *node, bool &noReturn, TypeSymbol returnType) {
    AddChild(node);    // DOTK

    int doLabel = ++label_count_;
    midcode_generator_->PrintLabel(doLabel);
    midcode_generator_->PrintLoop();

    noReturn = AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType);

    AddWhileChild(node);    // WHILETK
    AddChild(node);    // LPARENT
    AnalyzeCondition(AddSyntaxChild(CONDITION, node), false, doLabel);
    AddRparentChild(node);    // RPARENT
}

void ParseAnalyser::AnalyzeFor(SyntaxNode *node, TypeSymbol returnType) {
    AddChild(node);    // FORTK
    AddChild(node);    // LPARENT
    if (FindSymbol() == nullptr) {
        error_handing_->AddError(iter_->line_number, UNDEFINED);
    }

    string name = iter_->value;
    AddChild(node);    // IDENFR
    AddChild(node);    // ASSIGN
    SyntaxNode *expressionNode = AddSyntaxChild(EXPRESSION, node);
    AnalyzeExpression(expressionNode);
    midcode_generator_->PrintAssignValue(name, "", expressionNode->value());

    int forLabel = ++label_count_;
    midcode_generator_->PrintLabel(forLabel);
    midcode_generator_->PrintLoop();

    AddSemicnChild(node);    // SEMICN

    int endForLabel = ++label_count_;
    AnalyzeCondition(AddSyntaxChild(CONDITION, node), true, endForLabel);

    AddSemicnChild(node);    // SEMICN

    if (FindSymbol() == nullptr) {
        error_handing_->AddError(iter_->line_number, UNDEFINED);
    }
    string name1 = iter_->value;
    AddChild(node);    // IDENFR
    AddChild(node);    // ASSIGN
    if (FindSymbol() == nullptr) {
        error_handing_->AddError(iter_->line_number, UNDEFINED);
    }
    string name2 = iter_->value;
    AddChild(node);    // IDENFR
    string op = iter_->value;
    AddChild(node);    // PLUS or MINU
    int step = AnalyzeStep(AddSyntaxChild(STEP, node));
    AddRparentChild(node);    // RPARENT
    AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType);
    midcode_generator_->PrintStep(name1, name2, op, step);

    midcode_generator_->PrintJump(forLabel);
    midcode_generator_->PrintLabel(endForLabel);
}

bool ParseAnalyser::AnalyzeLoopSentence(SyntaxNode *node, TypeSymbol returnType) {
    bool noReturn = true;
    if (IsThisIdentifier(WHILETK)) {
        AnalyzeWhile(node, returnType);
    } else if (IsThisIdentifier(DOTK)) {
        AnalyzeDoWhile(node, noReturn, returnType);
    } else if (IsThisIdentifier(FORTK)) {
        AnalyzeFor(node, returnType);
    }
    return noReturn;
}

void ParseAnalyser::AnalyzeAssignSentence(SyntaxNode *node) {
    KindSymbol kind;
    if (FindSymbol() == nullptr) {
        error_handing_->AddError(iter_->line_number, UNDEFINED);
    } else {
        kind = FindSymbol()->kind();
        if (kind == KindSymbol::CONST) {
            error_handing_->AddError(iter_->line_number, ASSIGN_TO_CONST);
        }
    }
    string name = iter_->value;
    string arrayIndex;
    AddChild(node);    // IDENFR

    SyntaxNode *expressionNode;
    if (IsThisIdentifier(LBRACK)) {
        AddChild(node);    // LBRACK
        expressionNode = AddSyntaxChild(EXPRESSION, node);
        if (AnalyzeExpression(expressionNode) == TypeSymbol::CHAR) {
            error_handing_->AddError(iter_->line_number, ILLEGAL_ARRAY_INDEX);
        }
        arrayIndex = expressionNode->value();
        AddRbrackChild(node);    // RBRACK
    }
    AddChild(node);    // ASSIGN
    expressionNode = AddSyntaxChild(EXPRESSION, node);
    AnalyzeExpression(expressionNode);
    midcode_generator_->PrintAssignValue(name, arrayIndex, expressionNode->value());
}

void ParseAnalyser::AnalyzeScanfIdentifier(SyntaxNode *node) {
    if (FindSymbol() == nullptr) {
        error_handing_->AddError(iter_->line_number, UNDEFINED);
    } else {
        Symbol *symbol = FindSymbol(0) != nullptr ? FindSymbol(0) : FindSymbol(1);
        string type = symbol->type() == TypeSymbol::INT ? kIntType : kCharType;
        midcode_generator_->PrintScanf(type, symbol->name());
    }
    AddChild(node);    // IDENFR
}

void ParseAnalyser::AnalyzeScanfSentence(SyntaxNode *node) {
    AddChild(node);    // SCANFTK
    AddChild(node);    // LPARENT

    AnalyzeScanfIdentifier(node);
    while (IsThisIdentifier(COMMA)) {
        AddChild(node);    // COMMA
        AnalyzeScanfIdentifier(node);
    }

    AddRparentChild(node);    // RPARENT
}

void ParseAnalyser::AnalyzePrintfSentence(SyntaxNode *node) {
    AddChild(node);    // PRINTFTK
    AddChild(node);    // LPARENT

    if (IsThisIdentifier(STRCON)) {
        int stringNumber = string_table_->AddString(iter_->value);
        midcode_generator_->PrintString(stringNumber);
        AddChild(AddSyntaxChild(STRING, node));
        if (IsThisIdentifier(COMMA)) {
            AddChild(node);    // COMMA
            SyntaxNode *expression = AddSyntaxChild(EXPRESSION, node);
            TypeSymbol type = AnalyzeExpression(expression);
            if (type == TypeSymbol::INT) {
                midcode_generator_->PrintInteger(expression->value());
            } else {
                midcode_generator_->PrintChar(expression->value());
            }
        }
    } else {
        SyntaxNode *expression = AddSyntaxChild(EXPRESSION, node);
        TypeSymbol type = AnalyzeExpression(expression);
        if (type == TypeSymbol::INT) {
            midcode_generator_->PrintInteger(expression->value());
        } else {
            midcode_generator_->PrintChar(expression->value());
        }
    }
    midcode_generator_->PrintEnd();

    AddRparentChild(node);    // RPARENT
}

TypeSymbol ParseAnalyser::AnalyzeReturnSentence(SyntaxNode *node) {
    TypeSymbol type = TypeSymbol::VOID;
    AddChild(node);    // RETURNTK
    if (IsThisIdentifier(LPARENT)) {
        AddChild(node);    // LPARENT
        SyntaxNode *expression_root = AddSyntaxChild(EXPRESSION, node);
        type = AnalyzeExpression(expression_root);
        midcode_generator_->PrintReturn(false, expression_root->value());
        AddRparentChild(node);    // RPARENT
    } else {
        midcode_generator_->PrintReturn(true, "");
    }
    return type;
}

bool ParseAnalyser::AnalyzeSentence(SyntaxNode *node, TypeSymbol returnType) {
    bool noReturn = true;
    if (IsThisIdentifier(IFTK)) {
        noReturn = AnalyzeIfSentence(AddSyntaxChild(IF_SENTENCE, node), returnType);
    } else if (IsThisIdentifier(WHILETK)
               || IsThisIdentifier(DOTK)
               || IsThisIdentifier(FORTK)) {
        noReturn = AnalyzeLoopSentence(AddSyntaxChild(LOOP_SENTENCE, node), returnType);
    } else if (IsThisIdentifier(LBRACE)) {
        AddChild(node);    // LBRACE
        noReturn = AnalyzeSentenceCollection(AddSyntaxChild(SENTENCE_COLLECTION, node), returnType);
        AddChild(node);    // RBRACE
    } else if (IsThisIdentifier(IDENFR)) {
        noReturn = true;
        if (FindSymbol(0) != nullptr
            && FindSymbol(0)->kind() == KindSymbol::FUNCTION
            && FindSymbol(0)->type() != TypeSymbol::VOID) {
            AnalyzeReturnCallSentence(AddSyntaxChild(RETURN_CALL_SENTENCE, node));
            AddSemicnChild(node);    // SEMICN
        } else if (FindSymbol(0) != nullptr
                   && FindSymbol(0)->kind() == KindSymbol::FUNCTION
                   && FindSymbol(0)->type() == TypeSymbol::VOID) {
            AnalyzeReturnCallSentence(AddSyntaxChild(NO_RETURN_CALL_SENTENCE, node));
            AddSemicnChild(node);    // SEMICN
        } else {
            if (FindSymbol() != nullptr) {
                AnalyzeAssignSentence(AddSyntaxChild(ASSIGN_SENTENCE, node));
                AddSemicnChild(node);    // SEMICN
            } else if (FindSymbol(0) == nullptr) {
                if (FindSymbol(1) == nullptr) {
                    error_handing_->AddError(iter_->line_number, UNDEFINED);
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
        AddSemicnChild(node);    // SEMICN
    } else if (IsThisIdentifier(PRINTFTK)) {
        noReturn = true;
        AnalyzePrintfSentence(AddSyntaxChild(PRINTF_SENTENCE, node));
        AddSemicnChild(node);    // SEMICN
    } else if (IsThisIdentifier(SEMICN)) {
        noReturn = true;
        AddSemicnChild(node);    // SEMICN
    } else if (IsThisIdentifier(RETURNTK)) {
        TypeSymbol type = AnalyzeReturnSentence(AddSyntaxChild(RETURN_SENTENCE, node));
        noReturn = type == TypeSymbol::VOID;
        CountIterator(-1);
        if (returnType == TypeSymbol::VOID && type != TypeSymbol::VOID) {
            error_handing_->AddError(iter_->line_number, RETURN_IN_NO_RETURN_FUNCTION);
        } else if (returnType != type) {
            error_handing_->AddError(iter_->line_number, NO_RETURN_OR_WRONG_RETURN_IN_RETURN_FUNCTION);
        }
        CountIterator(+1);
        AddSemicnChild(node);    // SEMICN
    }
    return noReturn;
}

bool ParseAnalyser::AnalyzeSentenceCollection(SyntaxNode *node, TypeSymbol returnType) {
    bool noReturn = true;
    while (!IsThisIdentifier(RBRACE)) {
        if (!AnalyzeSentence(AddSyntaxChild(SENTENCE, node), returnType)) {
            noReturn = false;
        }
    }

    return noReturn;
}

void ParseAnalyser::AnalyzeCompositeSentence(SyntaxNode *node, TypeSymbol returnType) {
    if (IsThisIdentifier(CONSTTK)) {
        AnalyzeConstDeclare(AddSyntaxChild(CONST_DECLARE, node), 1);
    }
    if (IsThisIdentifier(INTTK) || IsThisIdentifier(CHARTK)) {
        AnalyzeVariableDeclare(AddSyntaxChild(VARIABLE_DECLARE, node), 1);
    }
    bool noReturn = AnalyzeSentenceCollection(
            AddSyntaxChild(SENTENCE_COLLECTION, node), returnType);
    if (returnType != TypeSymbol::VOID && noReturn) {
        CountIterator(-1);
        error_handing_->AddError(iter_->line_number, NO_RETURN_OR_WRONG_RETURN_IN_RETURN_FUNCTION);
        CountIterator(+1);
    }
}

void ParseAnalyser::AnalyzeMain(SyntaxNode *node) {
    int temp_reg = reg_count_;
    AddChild(node);    // VOIDTK
    InsertIdentifier(KindSymbol::FUNCTION, TypeSymbol::VOID, 0);
    Symbol *function = FindSymbol(0);
    CleanLevel(1);
    AddChild(node);    // MAINTK
    AddChild(node);    // LPARENT
    AddRparentChild(node);    // RPARENT
    midcode_generator_->PrintVoidFuncDeclare(function);
    AddChild(node);    // LBRACE
    AnalyzeCompositeSentence(AddSyntaxChild(COMPOSITE_SENTENCE, node), TypeSymbol::VOID);
    midcode_generator_->PrintReturn(true, "");
    AddChild(node);    // RBRACE

    midcode_generator_->PrintFuncEnd();
    check_table_->AddFunctionVariableNumber(function->name(), reg_count_ - temp_reg);
    InsertSymbolTable(function->name(), 1);
}

void ParseAnalyser::AnalyzeParameterTable(SyntaxNode *node, Symbol *function) {
    while (IsThisIdentifier(INTTK) || IsThisIdentifier(CHARTK)) {

        TypeSymbol type;
        SetSymbolType(type);
        AddChild(node);    // INTTK or CHARTK

        if (FindSymbol(1) != nullptr) {
            error_handing_->AddError(iter_->line_number, REDEFINITION);
        }
        function->AddParameter(type == TypeSymbol::INT ? '0' : '1');
        InsertIdentifier(KindSymbol::PARAMETER, type, 1);
        midcode_generator_->PrintParameter(type, iter_->value);
        AddChild(node);    // IDENTFR
        AddCommaChild(node);
    }
}

void ParseAnalyser::AnalyzeVoidFunc(SyntaxNode *node) {
    int temp_reg = reg_count_;
    AddChild(node);    // VOIDTK
    if (FindSymbol(0) != nullptr) {
        error_handing_->AddError(iter_->line_number, REDEFINITION);
    }
    InsertIdentifier(KindSymbol::FUNCTION, TypeSymbol::VOID, 0);
    Symbol *function = FindSymbol(0);
    CleanLevel(1);
    AddChild(node);    // IDENFR
    midcode_generator_->PrintVoidFuncDeclare(function);
    AddChild(node);    // LPARENT
    AnalyzeParameterTable(
            AddSyntaxChild(PARAMETER_TABLE, node), function);
    AddRparentChild(node);    // RPARENT
    AddChild(node);    // LBRACE
    AnalyzeCompositeSentence(AddSyntaxChild(COMPOSITE_SENTENCE, node), TypeSymbol::VOID);
    midcode_generator_->PrintReturn(true, "");
    AddChild(node);    // RBRACE

    midcode_generator_->PrintFuncEnd();
    check_table_->AddFunctionVariableNumber(function->name(), reg_count_ - temp_reg);
    InsertSymbolTable(function->name(), 1);
}

void ParseAnalyser::AnalyzeHeadState(SyntaxNode *node, Symbol *&function) {
    TypeSymbol type;
    SetSymbolType(type);
    AddChild(node);    // INTTK or CHARTK
    if (FindSymbol(0) != nullptr) {
        error_handing_->AddError(iter_->line_number, REDEFINITION);
    }
    InsertIdentifier(KindSymbol::FUNCTION, type, 0);
    function = FindSymbol(0);
    CleanLevel(1);
    AddChild(node);    // IDENFR
}

void ParseAnalyser::AnalyzeFunc(SyntaxNode *node) {
    auto function = new Symbol();
    int temp_reg = reg_count_;
    AnalyzeHeadState(AddSyntaxChild(HEAD_STATE, node), function);
    midcode_generator_->PrintFuncDeclare(function);
    AddChild(node);    // LPARENT
    AnalyzeParameterTable(
            AddSyntaxChild(PARAMETER_TABLE, node), function);
    AddRparentChild(node);    // RPARENT
    AddChild(node);    // LBRACE
    AnalyzeCompositeSentence(AddSyntaxChild(COMPOSITE_SENTENCE, node),
                             function->type());
    AddChild(node);    // RBRACE

    midcode_generator_->PrintFuncEnd();
    check_table_->AddFunctionVariableNumber(function->name(), reg_count_ - temp_reg);
    InsertSymbolTable(function->name(), 1);
}

void ParseAnalyser::BuildSyntaxTree(SyntaxNode *root) {
    string flag = CONST_DECLARE;

    int temp_reg = reg_count_;
    bool first = true;

    while (iter_ != iter_end_) {
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
            if (first) {
                check_table_->AddFunctionVariableNumber("global", reg_count_ - temp_reg);
                first = false;
            }

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
    InsertSymbolTable("global", 0);
}

void ParseAnalyser::AnalyzeParse() {
    auto root = new SyntaxNode(PROGRAM);
    BuildSyntaxTree(root);
}

map<string, SymbolTable *> ParseAnalyser::symbol_table_map() {
    return symbol_table_map_;
}

list<Midcode *> ParseAnalyser::midcode_list() {
    return midcode_generator_->midcode_list();
}

StringTable *ParseAnalyser::string_table() {
    return string_table_;
}

CheckTable *ParseAnalyser::check_table() {
    return check_table_;
}

int ParseAnalyser::reg_count() const {
    return reg_count_;
}

void ParseAnalyser::FileClose() {
    midcode_generator_->FileClose();
}