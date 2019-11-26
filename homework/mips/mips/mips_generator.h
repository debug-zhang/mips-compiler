#pragma once

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "table.h"
#include "objcode.h"

using namespace std;

class MipsGenerator {
private:
	ifstream midcode_;
	Objcode* objcode_;
	CheckTable* check_table_;
	StringTable* string_table_;
	map<string, SymbolTable*> symbol_table_map_;
	map<string, int> mid_var_reg_map_;
	map<string, int> mid_use_reg_map_;
	int reg_use_stack_[32];
	int new_reg;
	int dm_offset;

	vector<string> Split(string s, char delimiter);

	void InitVariable(int level);
	void InitVariable(string function_name);
	void InitDataSeg(string function_name);
	void InitDataSeg();
	void InitStack();

	void PrintText();
	void PrintMain();
	void PrintSyscall();
	void PrintEnd();

	bool IsInteger(string str);
	bool IsChar(string str);
	bool IsTempReg(string str);
	bool IsConstVar(string name);
	int GetConstVar(string name);
	bool IsThisInstr(string strs, string instr);

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
	void PopAllVar();
	int LoadValue(string name);
	int LoadMidReg(string name);
	void PopMidReg(string name);
	int GetUnuseRegInTable(int& unUseReg, bool& retflag, int level);
	int GetUnuseReg();
	void SetSymbolUse(string name, bool isUse);

	void GenerateBody(string function_name);

	void GenerateFunction();
	
public:
	MipsGenerator(string inputFileName, string outputFileName, StringTable* stringTable,
		CheckTable* check_table, map<string, SymbolTable*> symbolTableMap);
	void GenerateMips();
	void FileClose();
};

