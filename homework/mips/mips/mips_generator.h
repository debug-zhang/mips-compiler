#pragma once

#include <fstream>
#include <string>

using namespace std;

class MipsGenerator {
private:
	ifstream midcode;
	ofstream mips;
public:
	MipsGenerator(string inputFileName, string outputFileName);
	void FileClose();
};

