#include "mips_generator.h"

MipsGenerator::MipsGenerator(string inputFileName, string outputFileName) {
	this->midcode.open(inputFileName);
	this->mips.open(outputFileName);
}

void MipsGenerator::FileClose() {
	midcode.close();
	mips.close();
}