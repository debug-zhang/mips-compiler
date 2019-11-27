#pragma once

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include "midcode.h"
#include "table.h"
#include "objcode.h"

using namespace std;

class MipsGenerator {
private:
	Objcode* objcode_;
	CheckTable* check_table_;
	StringTable* string_table_;
	map<string, SymbolTable*> symbol_table_map_;
	list<Midcode*> midcode_list_;
	map<string, bool> temp_var_reg_map_;
	map<string, bool> temp_use_reg_map_;
	int reg_use_stack_[32];
	int new_reg;
	int dm_offset;

	void InsertTempUseRegMap(string name);

	void InitVariable(int level);
	void InitVariable(string function_name);
	void InitDataSeg(string function_name);
	void InitDataSeg();
	void InitStack();

	void PrintText();
	void PrintMain();
	void PrintEnd();

	bool IsInteger(string str);
	bool IsChar(string str);
	bool IsTempReg(string str);
	bool IsConstVariable(string name);
	int GetConstVariable(string name);
	bool IsThisInstr(string strs, string instr);

	Reg NumberToReg(int reg_number);
	int RegToNumber(Reg reg);

	void SaveAllReg();
	void ResetAllReg();

	int IsTempValue(string name);
	void LoadValue(string symbol, string function, Reg reg);
	void StoreValue(string symbol, string function, Reg reg);

	void LoadTable(int levle, string tableName);
	void SetStackRegUse(int number);
	void SetStackRegUnuse(int number);
	void PopStack();
	void PopSymbolMapVar(int level);
	void PopAllVariable();

	int LoadVariableToNumber(string name);
	Reg LoadVariableToReg(string name);
	int LoadTempRegToNumber(string name);
	Reg LoadTempRegToReg(string name);
	void PoptempReg(string name);

	int GetUnuseRegInTable(int& unUseReg, bool& retflag, int level);
	int GetUnuseRegNumber();
	Reg GetUnuseReg();
	void SetSymbolUse(string name, bool isUse);

	void GenerateJump(Midcode* midcode);

	Reg SetJudgeReg(Midcode* midcode, string name);

	void ResetJudgeReg(string name, Reg t0);

	void GenerateJudge(Midcode* midcode, MidcodeInstr judge);

	void GenerateSave(Midcode* midcode);

	void GenerateCall(Midcode* midcode);

	void GenerateScanf(Midcode* midcode, int type);

	void GenerateLabel(Midcode* midcode);

	void GenerateReturn(Midcode* midcode, bool is_return_value);

	void GenerateLoop();

	void GeneratePush(Midcode* midcode, int parameter_count);

	void GeneratePrintfEnd();

	void GeneratePrintfString(Midcode* midcode);

	void GeneratePrintfIntChar(Midcode* midcode, int type);

	void GenerateBody(string function_name, list<Midcode*>::iterator& iter);

	void GenerateFunction(Midcode* midcode, std::list<Midcode*>::iterator& iter);

	void Generate();

public:
	MipsGenerator(string outputFileName,
		StringTable* stringTable, CheckTable* check_table,
		map<string, SymbolTable*> symbolTableMap, list<Midcode*> midcode_list);
	void GenerateMips();
	void FileClose();
};

