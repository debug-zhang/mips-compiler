#pragma once

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include "midcode.h"
#include "table.h"
#include "objcode.h"

#define RS				Reg::t0
#define RT				Reg::t1
#define RD				Reg::t2
#define TEMP			Reg::t3

#define BASE_POINT		Reg::s0
#define SAVE_POINT		Reg::s1
#define PARA_POINT		Reg::s2

#define BASE_SPACE		"base_space"
#define SAVE_SPACE		"save_space"
#define PARA_SPACE		"para_space"

using namespace std;

class MipsGenerator {
private:
	Objcode* objcode_;
	CheckTable* check_table_;
	StringTable* string_table_;
	map<string, SymbolTable*> symbol_table_map_;
	list<Midcode*> midcode_list_;

	map<string, int> temporary_offset_map_;

	int dm_offset_;
	int temp_count_;

	void LoadTable(int level, string table_name);
	void InitConstString();
	int InitVariable(string function_name);
	void InitData();

	void PrintText();
	void ResetBasePoint();
	void InitStack();
	void InitText();

	void SaveVariable(string name, Reg reg);
	void LoadVariable(string name, Reg reg);

	bool IsInteger(string str);
	bool IsChar(string str);

	bool IsTemporary(string str);
	void SaveTemporary(string name, Reg reg);
	void LoadTemporary(string name, Reg reg);

	void LoadValue(string value, Reg reg);

	bool IsConstVariable(string name);
	int GetConstVariable(string name);

	void GenerateScanf(string variable_name, int type);

	void GeneratePrintfIntChar(Midcode* midcode, int type);
	void GeneratePrintfString(string str);
	void GeneratePrintfEnd();

	void GenerateLabel(string label);

	void GenerateJump(string label);

	void GenerateAssign(string result, string value);
	void GenerateAssignReturn(string temp);

	void SetArrayIndex(string index, int& offset, bool& is_use_temp);
	void GenerateAssignArray(string array, string index, string value);
	void GenerateLoadArray(string temp, string array, string index);

	void SetOperand(string value, Reg reg, bool& is_immediate, int& immediate);

	void DealNumberOpNumber(string value_1, string value_2,
		int& immediate_1, bool& is_immediate_1,
		int& immediate_2, bool& is_immediate_2);
	void DealNumberOpReg(string value_1, string temp_2,
		int& immediate_1, bool& is_immediate_1);
	void DealRegOpNumber(string temp_1, string value_2,
		int& immediate_2, bool& is_immediate_2);
	void DealRegOpReg(string temp_1, string temp_2);

	void GenerateOperate(Midcode* midcode, string result,
		MidcodeInstr op, OperaMember member_type);
	void GenerateOperate(list<Midcode*>::iterator& iter, Midcode* midcode);

	void GenerateStep(list<Midcode*>::iterator& iter, Midcode*& midcode);

	void GenerateNeg(string temp_result, string value);

	void SetJudgeReg(string value, Reg reg);
	void GenerateJudge(Midcode* midcode, MidcodeInstr judge);

	void GeneratePush(string value, int count);

	void GenerateCall(string name);

	void GenerateSave();

	void GenerateFunctionEnd(list<Midcode*>::iterator& iter);

	void GenerateReturn(Midcode* midcode, bool is_return_value);

	void GenerateParameter(string function_name, string value, int count);

	void GenerateBody(string function_name, list<Midcode*>::iterator& iter);
	void GenerateFunction(string function_name, list<Midcode*>::iterator& iter);
	void Generate();

public:
	MipsGenerator(string outputFileName, int temp_count,
		StringTable* stringTable, CheckTable* check_table,
		map<string, SymbolTable*> symbolTableMap, list<Midcode*> midcode_list);
	void GenerateMips();
	void FileClose();
};

