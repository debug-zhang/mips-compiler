#pragma once

#include <fstream>
#include <list>
#include "error.h"

using namespace std;

class ErrorHanding {
private:
	ofstream errorFile;
	list<Error> errorList;
public:
	ErrorHanding(string fileName);
	void AddError(Error error);
	void AddError(int lineNumber, char errorType);
	void PrintError();
	void FileClose();
};

