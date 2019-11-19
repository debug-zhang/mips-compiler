#pragma once

#include <fstream>
#include <string>
#include "table.h"

using namespace std;

class MipsGenerator {
private:
	ifstream midcode;
	ofstream mips;
	map<string, SymbolTable*> symbolTableMap;
public:
	MipsGenerator(string inputFileName, string outputFileName, 
		map<string, SymbolTable*> symbolTableMap);
	void FileClose();
};

