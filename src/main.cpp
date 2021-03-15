#include <iostream>
#include "lexical_analyser.h"
#include "parse_analyser.h"
#include "mips_generator.h"


int main() {
    const std::string testfile = "file/testfile.txt";
    const std::string midcode = "file/midcode.txt";
    const std::string mips = "file/mips.txt";
    const std::string error = "file/error.txt";

    ErrorHanding error_handing = ErrorHanding(error);

    LexicalAnalyser lexical_analyser = LexicalAnalyser(testfile, &error_handing);

    ParseAnalyser parse_analyser = ParseAnalyser(midcode,
                                                 lexical_analyser.Analyze(), &error_handing);
    lexical_analyser.FileClose();
    parse_analyser.AnalyzeParse();
    parse_analyser.FileClose();

    if (error_handing.IsError()) {
        error_handing.PrintError();
        error_handing.FileClose();
        return 0;
    }
    error_handing.FileClose();

    MipsGenerator mips_generator = MipsGenerator(mips, parse_analyser.reg_count(),
                                                 parse_analyser.string_table(), parse_analyser.check_table(),
                                                 parse_analyser.symbol_table_map(),
                                                 parse_analyser.midcode_list());

    mips_generator.GenerateMips();
    mips_generator.FileClose();

    return 0;
}