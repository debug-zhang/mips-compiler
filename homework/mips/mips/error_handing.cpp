#include "error_handing.h"


ErrorHanding::ErrorHanding(string fileName) {
	this->error_file_.open(fileName);
}

void ErrorHanding::AddError(Error error) {
	error_list_.push_back(error);
}

void ErrorHanding::AddError(int lineNumber, char errorType) {
	error_list_.push_back(Error(lineNumber, errorType));
}

void ErrorHanding::PrintError() {
	list<Error>::iterator iter = error_list_.begin();

	while (iter != error_list_.end()) {
		error_file_ << iter->line_number() << " " << iter->error_type() << endl;
		iter++;
	}
}

void ErrorHanding::FileClose() {
	error_file_.close();
}