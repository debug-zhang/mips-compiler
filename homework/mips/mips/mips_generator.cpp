#include "mips_generator.h"

MipsGenerator::MipsGenerator(string inputFileName, string outputFileName,
	map<string, SymbolTable*> symbolTableMap) {
	this->midcode.open(inputFileName);
	this->mips.open(outputFileName);
	this->symbolTableMap = symbolTableMap;
}

void MipsGenerator::FileClose() {
	midcode.close();
	mips.close();
}