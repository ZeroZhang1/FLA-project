#include <iostream>
#include <string>
#include <cstring>
#include "pda.h"
#include "tm.h"

using namespace std;

void print_usage() {
    cout << "usage:   fla [-h|--help] <pda|tm> <input>" << endl;
    cout << "         fla [-v|--verbose] [-h|--help] <pda|tm> <input>" << endl;
}
int main(int argc, char* argv[]) {
    bool verbose = false;
    string filename;
    string input;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (filename.empty()) {
            filename = argv[i];
        } else {
            input = argv[i];
        }
    }
    if (filename.empty()) {
        // Error: 参数不足,输出错误信息
        cerr << "Error: Missing filename or input" << endl;    
    }
    if (input.empty()) {
        // 将input设置为空字符串
        input = "";
    }
    
    // 分析filename的后缀，判断是PDA还是TM
    string suffix = filename.substr(filename.find_last_of(".") + 1);
    if (suffix == "pda") {
        PDA pda;
        pda.set_superverbose(false);
        pda.set_verbose(verbose);

        // 解析pda

        bool parse_result = pda.pda_parser(filename, false);
        if (!parse_result) {
            cerr << "syntax error" << endl;
            return 1;
        }
        if (!pda.input_check(input)) {
            return 1;
        }
        // 模拟pda运行
        bool result = pda.run_pda(input);

        if (result) {
            cout << "true" << endl;
        } else {
            cerr << "false" << endl;
        }

        return 0;

    } else if (suffix == "tm") {
        // TODO: 实现Turing Machine的解析和模拟
        TM tm;
        tm.set_superverbose(false);
        tm.set_verbose(verbose);
        // 解析tm
        bool parse_result = tm.tm_parser(filename);
        if (!parse_result) {
            cerr << "syntax error" << endl;
            return 1;
        }
        // 模拟tm运行
        bool input_check_result = tm.input_check(input);
        if (!input_check_result) {
            return 1;
        }
        bool result = tm.run_tm(input);
        if (!verbose) {
            cout << tm.get_tape_content() << endl;
        }
        return 0;
    } else {
        // Error: 文件后缀不正确
        cerr << "Error: Invalid file type" << endl;
        return 1;
    }
    return 0;
}