#include "error_handing.h"


ErrorHanding::ErrorHanding() {

}

void ErrorHanding::AddError(Error error) {
	errorList.push_back(error);
}

void ErrorHanding::AddError(int lineNumber, char errorType) {
	errorList.push_back(Error(lineNumber, errorType));
}

void ErrorHanding::PrintError(ofstream& errorFile) {
	list<Error>::iterator iter = errorList.begin();

	while (iter != errorList.end()) {
		errorFile << iter->GetLineNumber() << " " << iter->GetErrorType() << endl;
		iter++;
	}
}