#include "error.h"

Error::Error() {
    line_number_ = 0;
    error_type_ = NO_ERROR;
}

Error::Error(int line_number, char error_type) {
    line_number_ = line_number;
    error_type_ = error_type;
}

void Error::set_line_number(int line_number) {
    line_number_ = line_number;
}

void Error::set_error_type(char error_type) {
    error_type_ = error_type;
}

bool Error::IsError() const {
    return error_type_ != NO_ERROR;
}

int Error::line_number() const {
    return line_number_;
}

char Error::error_type() const {
    return error_type_;
}