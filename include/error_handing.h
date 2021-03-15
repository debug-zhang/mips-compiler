#pragma once

#include <fstream>
#include <list>
#include "error.h"

class ErrorHanding {
private:
	std::ofstream error_file_;
    std::list<Error> error_list_;
public:
	explicit ErrorHanding(const std::string& file_name);
	void AddError(Error error);
	void AddError(int line_number, char error_type);
	void PrintError();
	void FileClose();
	bool IsError();
};

