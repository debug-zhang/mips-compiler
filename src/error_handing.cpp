#include "error_handing.h"

using namespace std;

ErrorHanding::ErrorHanding(const string &file_name) {
    error_file_.open(file_name);
}

void ErrorHanding::AddError(Error error) {
    error_list_.push_back(error);
}

void ErrorHanding::AddError(int line_number, char error_type) {
    error_list_.emplace_back(line_number, error_type);
}

void ErrorHanding::PrintError() {
    auto iter = error_list_.begin();

    while (iter != error_list_.end()) {
        error_file_ << iter->line_number() << " " << iter->error_type() << endl;
        iter++;
    }
}

void ErrorHanding::FileClose() {
    error_file_.close();
}

bool ErrorHanding::IsError() {
    return !error_list_.empty();
}