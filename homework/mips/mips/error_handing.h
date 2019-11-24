#pragma once

#include <fstream>
#include <list>
#include "error.h"

using namespace std;

class ErrorHanding {
private:
	ofstream error_file_;
	list<Error> error_list_;
public:
	ErrorHanding(string fileName);
	void AddError(Error error);
	void AddError(int lineNumber, char errorType);
	void PrintError();
	void FileClose();
};

