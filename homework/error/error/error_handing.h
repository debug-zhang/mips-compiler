#pragma once

#include <fstream>
#include <list>
#include "error.h"

using namespace std;

class ErrorHanding {
private:
	list<Error> errorList;
public:
	ErrorHanding();
	void AddError(Error error);
	void AddError(int lineNumber, char errorType);
	void PrintError(ofstream& errorFile);
};

