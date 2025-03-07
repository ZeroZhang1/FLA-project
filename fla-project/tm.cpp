#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <tuple>
#include <fstream>
#include <sstream>
#include <stack>   
#include <cassert>
#include <cstring>
#include "tm.h"

using namespace std;


TM::TM() : superverbose(true), q0(""), verbose(false){
    // 构造函数
    step = 0;
    for (int i = 0; i < 12; i++) {
        index[i] = 0;
    }
}

bool TM::tm_parser(string filename){
    if (superverbose) {
        cerr << "Running TM with file: " << filename << endl;
    }

    // 读取文件
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }

    string line;
    bool get_Q = false, get_S = false, get_G = false, get_q0 = false, get_B = false, get_F = false;
    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }
        // 去除尾注释
        size_t comment_pos = line.find(';');
        if (comment_pos != std::string::npos) {
            line.erase(comment_pos);
        }
        line.erase(0, line.find_first_not_of(" \t\r\n")); // 去除行开头的空格
        line.erase(line.find_last_not_of(" \t\r\n") + 1); // 去除行尾的空格
        
        if (line[0] == ';'|| line.empty()) {
            continue; // 去除行注释
        }
        else if (line[0] == '#') {
            // 解析状态集Q
            if (line.find("#Q = ") == 0) {
                if (get_Q) {
                    if (superverbose)
                        cerr << "Q: duplicate definition" << endl;
                    return false;
                }
                bool flag = parse_Q_set(line.substr(5), Q);
                if (!flag) {
                    cerr << "Q: syntax error" << endl;
                    return false;
                }
                get_Q = true;
            } // 解析输入符号集S
            else if (line.find("#S = ") == 0) {
                if (get_S) {
                    if (superverbose)
                        cerr << "S: duplicate definition" << endl;
                    return false;
                }
                bool flag = parse_S_set(line.substr(5), S);
                if (!flag) {
                    cerr << "S: syntax error" << endl;
                    return false;
                }
                get_S = true;
            } // 解析纸带符号集G
            else if (line.find("#G = ") == 0) {
                if (get_G) {
                    if (superverbose)
                        cerr << "G: duplicate definition" << endl;
                    return false;
                }
                bool flag = parse_G_set(line.substr(5), G);
                if (!flag) {
                    cerr << "G: syntax error" << endl;
                    return false;
                }
                get_G = true;
            } // 解析初始状态q0
            else if (line.find("#q0 = ") == 0) {
                if (get_q0) {
                    if (superverbose)
                        cerr << "q0: duplicate definition" << endl;
                    return false;
                }
                q0 = line.substr(6);
                // 检查初始状态是否在Q中
                if (Q.find(q0) == Q.end()) {
                    if (superverbose) {
                        cerr << "q0: " << q0 << " is not in Q" << endl;
                    }
                    return false;
                }
                get_q0 = true;
            } // 解析空格符号B
            else if (line.find("#B = ") == 0) {
                if (get_B) {
                    if (superverbose)
                        cerr << "B: duplicate definition" << endl;
                    return false;
                }
                B = line[5];
                // 检查空格符号是否在G中
                if (G.find(B) == G.end()) {
                    if (superverbose) {
                        cerr << "B: " << B << " is not in G" << endl;
                    }
                    return false;
                }
                get_B = true;
            } // 解析终止状态集F
            else if (line.find("#F = ") == 0) {
                if (get_F) {
                    if (superverbose)
                        cerr << "F: duplicate definition" << endl;
                    return false;
                }
                bool flag = parse_F_set(line.substr(5), F);
                if (!flag) {
                    cerr << "F: syntax error" << endl;
                    return false;
                }
                get_F = true;
            } // 解析纸带数N
            else if (line.find("#N = ") == 0) {
                N = stoi(line.substr(5));
            }
            
        }// 解析转移函数
        else {
            bool flag = parse_delta(line);
            if (!flag) {
                cerr << "Delta: syntax error" << endl;
                return false;
            }
        }
    }
    inFile.close();
    return true;
}

bool TM::parse_delta(const std::string& line) {
    // (当前状态, 当前输入符号)->(下一个状态, 新符号, 方向)
    string old_state, old_symbols, new_symbols, directions, new_state;
    
    // 解析五元组
    istringstream iss(line);
    if (!(iss >> old_state >> old_symbols >> new_symbols >> directions >> new_state)) {
        if (superverbose)
            cerr << "Delta: Invalid format" << endl;
        return false;
    }

    // 验证状态
    if (Q.find(old_state) == Q.end() || Q.find(new_state) == Q.end()) {
        if (superverbose)
            cerr << "Delta: Invalid state" << endl;
        return false;
    }

    // 验证长度
    if (old_symbols.length() != N || new_symbols.length() != N || directions.length() != N) {
        if (superverbose)
            cerr << "Delta: Invalid length" << endl;
        return false;
    }

    // 验证符号和方向
    for (int i = 0; i < N; i++) {
        // 检查旧符号
        if (old_symbols[i] != '*' && G.find(old_symbols[i]) == G.end()) {
            if (superverbose)
                cerr << "Delta: Invalid old symbol" << endl;
            return false;
        }
        // 检查新符号
        if (new_symbols[i] != '*' && G.find(new_symbols[i]) == G.end()) {
            if (superverbose)
                cerr << "Delta: Invalid new symbol" << endl;
            return false;
        }
        // 检查方向
        if (directions[i] != 'l' && directions[i] != 'r' && directions[i] != '*') {
            if (superverbose)
                cerr << "Delta: Invalid direction" << endl;
            return false;
        }
    }

    // 存储转移函数
    delta[make_pair(old_state, old_symbols)] = make_tuple(new_state, new_symbols, directions);

    if (superverbose) {
        cout << "Delta: Added transition (" << old_state << "," << old_symbols 
             << ") -> (" << new_state << "," << new_symbols << "," << directions << ")" << endl;
    }

    return true;
}


bool TM::parse_Q_set(const string& str, set<string>& result_set) {
    size_t start = str.find('{') + 1;
    size_t end = str.find('}');
    string elements = str.substr(start, end - start);
    size_t pos = 0;
    while ((pos = elements.find(',')) != string::npos) {
        string state = elements.substr(0, pos);
        state.erase(0, state.find_first_not_of(" \t"));
        state.erase(state.find_last_not_of(" \t") + 1);

        // 检查状态是否重复
        if (result_set.find(state) != result_set.end()) {
            if (superverbose) {
                cerr << "Q: " << state << " is already in the set" << endl;
            }
            return false;
        }

        result_set.insert(state);
        elements.erase(0, pos + 1);
    }
    elements.erase(0, elements.find_first_not_of(" \t"));
    elements.erase(elements.find_last_not_of(" \t") + 1);
    result_set.insert(elements);
    if (superverbose) {
        cout << "Q: ";
        for (const string& state : result_set) {
            cout << state << " ";
        }
        cout << endl;
    }
    return true;
}

bool TM::parse_S_set(const string& str, set<char>& result_set) {
    size_t start = str.find('{') + 1;
    size_t end = str.find('}');
    string elements = str.substr(start, end - start);
    size_t pos = 0;
    while ((pos = elements.find(',')) != string::npos) {
        string symbol = elements.substr(0, pos);
        symbol.erase(0, symbol.find_first_not_of(" \t"));
        symbol.erase(symbol.find_last_not_of(" \t") + 1);
        if (symbol.size() == 1 && symbol[0] != ' ' && symbol[0] != ',' && symbol[0] != ';' && symbol[0] != '{' && symbol[0] != '}' && symbol[0] != '*' && symbol[0] != '_') {
            // 检查符号是否重复
            if (result_set.find(symbol[0]) != result_set.end()) {
                if (superverbose) {
                    cerr << "S: " << symbol[0] << " is already in the set" << endl;
                }
                return false;
            }
            result_set.insert(symbol[0]);
        } else {    // 出现非法符号
            return false;
        }
        elements.erase(0, pos + 1);
    }
    elements.erase(0, elements.find_first_not_of(" \t"));
    elements.erase(elements.find_last_not_of(" \t") + 1);
    if (elements.size() == 1 && elements[0] != ' ' && elements[0] != ',' && elements[0] != ';' && elements[0] != '{' && elements[0] != '}' && elements[0] != '*' && elements[0] != '_') {
        // 检查符号是否重复
        if (result_set.find(elements[0]) != result_set.end()) {
            if (superverbose) {
                cerr << "S: " << elements[0] << " is already in the set" << endl;
            }
            return false;
        }
        result_set.insert(elements[0]);
    }
    if (superverbose) {
        cerr << "S: ";
        for (const char& symbol : result_set) {
            cerr << symbol << " ";
        }
        cerr << endl;
    }
    return true;
}

bool TM::parse_G_set(const string& str, set<char>& result_set) {
    size_t start = str.find('{') + 1;
    size_t end = str.find('}');
    string elements = str.substr(start, end - start);
    size_t pos = 0;
    while ((pos = elements.find(',')) != string::npos) {
        string symbol = elements.substr(0, pos);
        symbol.erase(0, symbol.find_first_not_of(" \t"));
        symbol.erase(symbol.find_last_not_of(" \t") + 1);
        if (symbol.size() == 1 && symbol[0] != ' ' && symbol[0] != ',' && symbol[0] != ';' && symbol[0] != '{' && symbol[0] != '}' && symbol[0] != '*') {
            // 检查符号是否重复
            if (result_set.find(symbol[0]) != result_set.end()) {
                if (superverbose) {
                    cerr << "G: " << symbol[0] << " is already in the set" << endl;
                }
                return false;
            }
            result_set.insert(symbol[0]);
        } else {    // 出现非法符号
            return false;
        }
        elements.erase(0, pos + 1);
    }
    elements.erase(0, elements.find_first_not_of(" \t"));
    elements.erase(elements.find_last_not_of(" \t") + 1);
    if (elements.size() == 1 && elements[0] != ' ' && elements[0] != ',' && elements[0] != ';' && elements[0] != '{' && elements[0] != '}' && elements[0] != '*') {
        // 检查符号是否重复
        if (result_set.find(elements[0]) != result_set.end()) {
            if (superverbose) {
                cerr << "G: " << elements[0] << " is already in the set" << endl;
            }
            return false;
        }
        result_set.insert(elements[0]);
    }
    if (superverbose) {
        cerr << "S: ";
        for (const char& symbol : result_set) {
            cerr << symbol << " ";
        }
        cerr << endl;
    }
    return true;
}

void TM::set_verbose(bool input_verbose) {
    this->verbose = input_verbose;
}

void TM::set_superverbose(bool input_superverbose) {
    this->superverbose = input_superverbose;
}

bool TM::input_check(const string& input) {
    for (size_t i = 0; i < input.length(); i++) {
        if (!verbose){
            if (S.find(input[i]) == S.end()) {
                cerr << "illegal input" << endl;
                return false;
            }
        }
        else {
            if (S.find(input[i]) == S.end()) {
                cerr << "Input: " << input << endl;
                cerr << "==================== ERR ====================" << endl;
                cerr << "error: '" << input[i] << "' was not declared in the set of input symbols" << endl;
                cerr << "Input: " << input << endl;
                cerr << string(i + 10-3, ' ') << "^" << endl;
                cerr << "==================== END ====================" << endl;
                return false;
            }
        }
        
    }
    if (verbose) {
        cout << "Input: " << input << endl;
        cout << "==================== RUN ====================" << endl;
    }


    return true;
}

void TM::tape_init(const string& input) {
    // 第一条纸带存储输入，其余纸带存储空格, 让纸带从512位置开始，两端为正负方向
    for (int i = 0; i < N; i++) {
        // 初始化每条纸带为1024个空格
        for (int j = 0; j < 1024; j++) {
            tape[i][j] = B;
        }
        index[i] = 512;             // 设置每条纸带的初始位置为512
    }

    // 将输入字符串放入第一条纸带
    for (size_t i = 0; i < input.length(); i++) {
        tape[0][512 + i] = input[i];
    }
}

bool TM::is_empty_tape(int n) const{
    for (int i = 0; i < 1024; i++) {
        if (tape[n][i] != B) {
            return false;
        }
    }
    return true;
}

bool TM::parse_F_set(const string& str, set<string>& result_set) {
    size_t start = str.find('{') + 1;
    size_t end = str.find('}');
    string elements = str.substr(start, end - start);
    size_t pos = 0;
    while ((pos = elements.find(',')) != string::npos) {
        string state = elements.substr(0, pos);
        state.erase(0, state.find_first_not_of(" \t"));
        state.erase(state.find_last_not_of(" \t") + 1);

        // 检查状态是否重复
        if (result_set.find(state) != result_set.end()) {
            return false;
        }

        // 检查状态是否在Q中
        if (Q.find(state) == Q.end()) {
            if (superverbose) {
                cerr << "F: " << state << " is not in Q" << endl;
            }
            return false;
        }

        result_set.insert(state);
        elements.erase(0, pos + 1);
    }
    
    elements.erase(0, elements.find_first_not_of(" \t"));
    elements.erase(elements.find_last_not_of(" \t") + 1);
    
    // 检查最后一个状态
    if (Q.find(elements) == Q.end()) {
        if (superverbose) {
            cerr << "F: " << elements << " is not in Q" << endl;
        }
        return false;
    }
    
    result_set.insert(elements);
    return true;
}

bool TM::run_tm(const string& input) {
    current_state = q0;
    tape_init(input);
    // TODO: 实现TM的模拟
    while (true) {
        verbose_info();

        if (!goto_next_state()) {
            break;
        }
        step ++;
    }
    // 输出第一条纸带上的非空内容
    if (verbose) {
        int first, last;
        tie(first, last) = find_first_last(0);
        cout << "Result: ";
        for (int i = first; i <= last; i++) {
            cout << tape[0][i];
        }
        cout << endl;
        cout << "==================== END ====================" << endl; 
    }
    if (F.find(current_state) != F.end()) {
        return true;
    }
    return false;
}

bool TM::goto_next_state() {
    char old_symbols[N], new_symbols[N], directions[N];
    for (int i = 0; i < N; i++) {
        old_symbols[i] = tape[i][index[i]];
    }
    // 遍历所有转移规则查找匹配项
    for (const auto& rule : delta) {
        if (rule.first.first == current_state && 
            match_symbols(rule.first.second, old_symbols)) {
            
            // 应用转移规则
            current_state = get<0>(rule.second);
            string new_symbols = get<1>(rule.second);
            string directions = get<2>(rule.second);

            // 更新纸带和头位置
            for (int i = 0; i < N; i++) {
                if (new_symbols[i] != '*') {
                    tape[i][index[i]] = new_symbols[i];
                }
                
                if (directions[i] == 'l') {
                    index[i]--;
                } else if (directions[i] == 'r') {
                    index[i]++;
                }
            }
            return true;
        }
    }
    return false;  // 没有找到匹配的转移规则
}

// 输出verbose信息
void TM::verbose_info() const{
    if (verbose) {
        cout << "Step   : " << step << endl;
        cout << "State  : " << current_state << endl;
    }
    
    
    // 遍历每条纸带
    for (int i = 0; i < N; i++) {
        if (is_empty_tape(i)) { // 纸带为空
            if (verbose){
                cout << "Index" << i << " : " << 0 << endl;
                cout << "Tape" << i << "  : " << B << endl;
                cout << "Head" << i << "  : " << "^" << endl;
            }
            
        } else {
            int first, last;
            tie(first, last) = find_first_last(i);
            if (verbose) {
                print_tape_info(i, first, last);
            }
            // 闹鬼了，这两个实现明明是相等的，但是下面这个就会出错！！！
            /*if (verbose) {
                // Index
                cout << "Index" << i << " : ";
                for (int j = first; j <= last; j++) {
                    cout << ((j-512>0) ? (j-512) : (512-j)) << " ";
                }
                cout << endl;

                // Tape
                cout << "Tape" << i << "  : ";
                if (index[i] < first){
                    cout << B << " ";
                }
                for (int j = first; j <= last; j++) {
                    cout << tape[i][j] << " ";
                }
                if (index[i] > last) {
                    cout << B << " ";
                }
                cout << endl;

                // Head
                cout << "Head" << i << "  : ";
                for (int j=0; j<index[i]-first; j++) {
                    cout << "  ";
                }
                cout << "^" << endl;
            }*/
        }
    }
    if (verbose)
        cout << "-----------------------------------" << endl;
}

// 找出一条tape中第一个非空字符和最后一个非空字符的位置
pair<int, int> TM::find_first_last(int n) const{
    int first = -1, last = -1;
    for (int i = 0; i < 1024; i++) {
        if (tape[n][i] != '_') {
            if (first == -1) {
                first = i;
            }
            last = i;
        }
    }
    return {first, last};
}

// 添加符号匹配函数
bool TM::match_symbols(const string& pattern, const char* actual) {
    for (size_t i = 0; i < N; i++) {
        if (pattern[i] != actual[i]) {
            if (pattern[i] != '*' || (actual[i] == B && pattern[i] == '*')) {
                return false;
            }
        }
    }
    return true;
}

void TM::print_tape_info(int tape_num, int first, int last) const {
    /*for (int i=0; i<N; i++){
        cout << "Index" << i << " : " << index[i] << " ";
    }
    cout << endl;*/
    // 打印Index
    cout << "Index" << tape_num << " : ";
    if (index[tape_num] < first) {
        if (index[tape_num] - 512 < 0) {
            cout << 512 - index[tape_num] << " ";
        } else
        {
            cout << index[tape_num] - 512 << " ";
        }
    }
    for (int j = first; j <= last; j++) {
        if (j - 512 < 0) {
            cout << 512 - j << " ";
        } else
        {
            cout << j - 512 << " ";
        }
    }
    if (index[tape_num] > last) {
        if (index[tape_num] - 512 < 0) {
            cout << 512 - index[tape_num] << " ";
        } else
        {
            cout << index[tape_num] - 512 << " ";
        }
    }
    cout << endl;

    // 打印Tape
    cout << "Tape" << tape_num << "  : ";
    if (index[tape_num] < first) {
        cout << B << " ";
    }
    for (int j = first; j <= last; j++) {
        cout << tape[tape_num][j] << " ";
    }
    if (index[tape_num] > last) {
        cout << B << " ";
    }
    cout << endl;

    // 打印Head
    cout << "Head" << tape_num << "  : ";
    for (int j = 0; j < index[tape_num] - first; j++) {
        cout << "  ";
    }
    cout << "^" << endl;
}

string TM::get_tape_content() const {
    string result;
    int first, last;
    tie(first, last) = find_first_last(0);
    for (int i = first; i <= last; i++) {
        result += tape[0][i];
    }
    return result;
}