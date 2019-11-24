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
	ErrorHanding(string file_name);
	void AddError(Error error);
	void AddError(int line_number, char error_type);
	void PrintError();
	void FileClose();
};

