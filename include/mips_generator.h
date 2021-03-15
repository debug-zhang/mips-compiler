#pragma once

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include "midcode.h"
#include "table.h"
#include "objcode.h"

#define RS                Reg::t0
#define RT                Reg::t1
#define RD                Reg::t2
#define TEMP            Reg::t3

#define GLOBAL_POINT    Reg::s0
#define FUNC_POINT        Reg::s1
#define RA_POINT        Reg::s2
#define PARA_POINT        Reg::s3

#define FUNC_SPACE        "stack_space"
#define RA_SPACE        "ra_space"
#define PARA_SPACE        "para_space"

class MipsGenerator {
private:
    Objcode *objcode_;
    CheckTable *check_table_;
    StringTable *string_table_;
    std::map<std::string, SymbolTable *> symbol_table_map_;
    std::list<Midcode *> midcode_list_;

    std::map<std::string, int> temporary_offset_map_;
    std::map<std::string, int> function_offset_map_;

    int temp_count_;
    int temp_offset_;
    int dm_offset_;

    void LoadTable(int level, const std::string &name);

    void InitConstString();

    void InitVariable(const std::string &function);

    void InitData();

    void PrintText();

    void InitStack();

    void InitText();

    void SaveVariable(const std::string &name, Reg reg);

    void LoadVariable(const std::string &name, Reg reg);

    static bool IsInteger(const std::string &str);

    static bool IsChar(const std::string &str);

    static bool IsTemporary(const std::string &str);

    void SaveTemporary(const std::string &name, Reg reg);

    void LoadTemporary(const std::string &name, Reg reg);

    void LoadValue(const std::string &value, Reg reg);

    bool IsConstVariable(const std::string &name);

    int GetConstVariable(const std::string &name);

    void GenerateScanf(const std::string &variable_name, int type);

    void GeneratePrintfIntChar(Midcode *midcode, int type);

    void GeneratePrintfString(const std::string &str);

    void GeneratePrintfEnd();

    void GenerateLabel(const std::string &label);

    void GenerateJump(const std::string &label);

    void GenerateAssign(const std::string &result, const std::string &value);

    void GenerateAssignReturn(const std::string &temp);

    void SetArrayIndex(const std::string &index, Reg base, int &offset, bool &is_use_temp);

    void GenerateAssignArray(const std::string &array, const std::string &index, const std::string &value);

    void GenerateLoadArray(const std::string &temp, const std::string &array, const std::string &index);

    void SetOperand(const std::string &value, Reg reg, bool &is_immediate, int &immediate);

    void DealNumberOpNumber(const std::string &value_1, const std::string &value_2,
                            int &immediate_1, bool &is_immediate_1,
                            int &immediate_2, bool &is_immediate_2);

    void DealNumberOpReg(const std::string &value_1, const std::string &temp_2,
                         int &immediate_1, bool &is_immediate_1);

    void DealRegOpNumber(const std::string &temp_1, const std::string &value_2,
                         int &immediate_2, bool &is_immediate_2);

    void DealRegOpReg(const std::string &temp_1, const std::string &temp_2);

    void GenerateOperate(Midcode *midcode, const std::string &result,
                         MidcodeInstr op, OperaMember member_type);

    void GenerateOperate(std::list<Midcode *>::iterator &iter, Midcode *midcode);

    void GenerateStep(std::list<Midcode *>::iterator &iter, Midcode *&midcode);

    void GenerateNeg(const std::string &temp_result, const std::string &value);

    void SetJudgeReg(const std::string &value, Reg reg);

    void GenerateJudge(Midcode *midcode, MidcodeInstr judge);

    void GeneratePush(const std::string &value);

    void GenerateCall(const std::string &prev_name, const std::string &call_name);

    void GenerateSave();

    static void GenerateFunctionEnd(std::list<Midcode *>::iterator &iter);

    void GenerateReturn(Midcode *midcode, bool is_return_value);

    void GenerateParameter(const std::string &function_name, const std::string &value, int count);

    void GenerateBody(const std::string &function_name, std::list<Midcode *>::iterator &iter);

    void GenerateFunction(const std::string &function_name, std::list<Midcode *>::iterator &iter);

    void Generate();

public:
    MipsGenerator(const std::string &outputFileName, int temp_count,
                  StringTable *stringTable, CheckTable *check_table,
                  std::map<std::string, SymbolTable *> symbolTableMap, std::list<Midcode *> midcode_list);

    void GenerateMips();

    void FileClose();
};

