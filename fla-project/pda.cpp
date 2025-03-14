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
#include "pda.h"
using namespace std;

PDA::PDA() : superverbose(true), q0(""), verbose(false){
    // set 类型的成员变量 Q, S, G, F, delta 会自动初始化为空
}

bool PDA::pda_parser(string filename, bool superverbose){
    this->superverbose = superverbose;
    if (superverbose) {
        cerr << "Running PDA with file: " << filename << endl;
    }
    // 这里需要实现具体的PDA模拟器

    // 读取文件
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }

    string line;
    bool get_Q = false, get_S = false, get_G = false, get_q0 = false, get_z0 = false, get_F = false;
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
            continue; // 去除注释
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
            } // 解析栈符号集G 
            else if (line.find("#G = ") == 0) {
                if (get_G) {
                    if (superverbose)
                        cerr << "G: duplicate definition" << endl;
                    return false;
                }
                bool flag = parse_S_set(line.substr(5), G);
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
            } // 解析初始栈符号z0 
            else if (line.find("#z0 = ") == 0) {
                if (get_z0) {
                    if (superverbose)
                        cerr << "z0: duplicate definition" << endl;
                    return false;
                }
                z0 = line[6];
                // 检查初始栈符号是否在G中
                if (G.find(z0) == G.end()) {
                    if (superverbose) {
                        cerr << "z0: " << z0 << " is not in G" << endl;
                    }
                    return false;
                }
                get_z0 = true;
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
            } 
        } // 解析转移函数
        else {
                bool flag = parse_delta(line);
                if (!flag) {
                    cerr << "Delta: syntax error" << endl;
                    return false;
                }
        }
        
    }
    inFile.close();

    return true; // 示例返回值
}

void PDA::set_verbose(bool superverbose) {
    this->verbose = superverbose;
}

void PDA::set_superverbose(bool superverbose) {
    this->superverbose = superverbose;
}

bool PDA::input_check(const string& input) {
    // 检查input是否合法
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

bool PDA::run_pda(const string& input) {
    // TODO: 模拟PDA程序的运行逻辑
    pda_init();
    for (char c : input) {
        // 输出栈中所有元素
        if (verbose) { 
            cout << "State: " << current_state << endl;
            cout << "Stack: ";
            std::stack<char> temp_stack = stack;
            while (!temp_stack.empty()) {
                cout << temp_stack.top();
                temp_stack.pop();
            }
            cout << endl;
            cout << "---------------------------------------------" << endl;

        }
        bool success_goto = goto_next_state(c);

        if (!success_goto) {
            return false;
        }
    }
    // 检查是否到达终止状态
    return is_final_state();
}
/*解析器部分代码*/ 
bool PDA::parse_Q_set(const string& str, set<string>& result_set) {
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

// S和G的解析可以共用这个函数
bool PDA::parse_S_set(const string& str, set<char>& result_set) {
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

bool PDA::parse_F_set(const string& str, set<string>& result_set) {
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

        result_set.insert(state);
        elements.erase(0, pos + 1);
    }
    elements.erase(0, elements.find_first_not_of(" \t"));
    elements.erase(elements.find_last_not_of(" \t") + 1);
    result_set.insert(elements);

    // F不为空
    if (result_set.empty()) {
        return false;
    }
    // F是Q的子集
    for (const string& state : result_set) {
        if (Q.find(state) == Q.end()) {
            if (superverbose) {
                cerr << "F: " << state << " is not in Q" << endl;
            }
            return false;
        }
    }

    if (superverbose) {
        cout << "F: ";
        for (const string& state : result_set) {
            cout << state << " ";
        }
        cout << endl;
    }
    return true;
}

bool PDA::parse_delta(const string& line) {
    // (当前状态, 当前输入, 当前栈顶)->(下一个状态, 新栈序列)
    istringstream iss(line);
    string current_state, input_symbol, stack_top, new_state, stack_sequence;
    
    // 读取五个部分
    if (!(iss >> current_state >> input_symbol >> stack_top >> new_state >> stack_sequence)) {
        if (superverbose)
            cerr << "Delta: Invalid format" << endl;
        return false;
    }
    // 验证当前状态
    if (Q.find(current_state) == Q.end()) {
        if (superverbose)
            cerr << "Delta: Invalid current state " << current_state << endl;
        return false;
    }
    // 验证输入符号 (_表示空字符)
    if (input_symbol != "_" && (S.find(input_symbol[0]) == S.end())) {
        if (superverbose)
            cerr << "Delta: Invalid input symbol " << input_symbol << endl;
        return false;
    }
    // 验证栈顶符号
    if (G.find(stack_top[0]) == G.end()) {
        if (superverbose)
            cerr << "Delta: Invalid stack top symbol " << stack_top << endl;
        return false;
    }
    // 验证新状态
    if (Q.find(new_state) == Q.end()) {
        if (superverbose)
            cerr << "Delta: Invalid new state " << new_state << endl;
        return false;
    }
    // 验证栈序列中的每个符号 (_表示弹出栈顶不压入新符号)
    if (stack_sequence != "_") {
        for (char c : stack_sequence) {
            if (G.find(c) == G.end()) {
                if (superverbose)
                        cerr << "Delta: Invalid stack sequence symbol " << c << endl;
                return false;
            }
        }
    }
    // 构造转换函数的键和值
    auto key = make_tuple(current_state, input_symbol[0], stack_top[0]);
    auto value = make_pair(new_state, stack_sequence);
    // 检查转换规则是否已存在
    auto it = delta.find(key);
    if (it != delta.end() && it->second.find(value) != it->second.end()) {
        if (superverbose)
            cerr << "Delta: Duplicate transition rule" << endl;
        return false;
    }
    // 将转换规则添加到delta中
    delta[key].insert(value);
    if (superverbose) {
        cout << "Delta: Added transition (" << current_state << "," << input_symbol 
            << "," << stack_top << ") -> (" << new_state << "," << stack_sequence << ")" << endl;
    }
    return true;
}

/*模拟器代码*/
void PDA::pda_init(){
    current_state = q0;
    stack.push(z0);
}

bool PDA::goto_next_state(char input_symbol) {
    assert(input_symbol != '_');
    
    char stack_top;
    if (stack.empty()) {
        stack_top = '_';
    } else {
        stack_top = stack.top();
        // stack.pop();
    }
    // 在delta中进行查找
    auto key1 = make_tuple(current_state, input_symbol, stack_top); // 有输入符号
    bool flag1 = false;
    

    // Try with actual input symbol
    if (delta.find(key1) != delta.end()) {
        auto transition = *delta[key1].begin();
        string next_state = std::get<0>(transition);
        string stack_symbols = std::get<1>(transition);
        if (stack_top != '_') {
            stack.pop();
        }
        if (stack_symbols != "_") {
            // XXZ栈字符序列（当前栈顶字符为X, 栈底为Z)
            for (auto it = stack_symbols.rbegin(); it != stack_symbols.rend(); ++it) {
                stack.push(*it);
            } 
        }
        current_state = next_state;
        flag1 = true;
    }

    // 进行所有空转移
    bool flag2 = false;
    set<string> visited_states;  // 记录已访问状态
    while (true) {
        if (visited_states.count(current_state)) {
            break;  // 避免循环
        }
        visited_states.insert(current_state);

        if (stack.empty()) {
            stack_top = '_';
        } else {
            stack_top = stack.top();
        }

        auto key2 = make_tuple(current_state, '_', stack_top);
        if (delta.find(key2) != delta.end()) {
            auto transition = *delta[key2].begin();
            string next_state = std::get<0>(transition);
            string stack_symbols = std::get<1>(transition);
            if (stack_symbols != "_") {
                stack.pop();
                for (int i = stack_symbols.length() - 1; i >= 0; i--) {
                    stack.push(stack_symbols[i]);
                }
            }
            current_state = next_state;
            flag2 = true;
        } else {
            break;
        }
    }
    
    if (flag1||flag2) {
        return true;
    }
    
    return false;
}

bool PDA::is_final_state() {
    return F.find(current_state) != F.end();
}



