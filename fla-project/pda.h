#ifndef PDA_H
#define PDA_H

#include <string>
#include <cstring>
#include <set>
#include <map>
#include <tuple>
#include <cassert>
#include <stack>
using namespace std;

class PDA {
public:
    PDA();
    bool pda_parser(std::string filename, bool superverbose);
    void set_verbose(bool superverbose);
    void set_superverbose(bool superverbose);
    bool input_check(const std::string& input);
    bool run_pda(const std::string& input);
    bool superverbose;

private:
    bool verbose;
    std::set<std::string> Q;  // 状态集
    std::set<char> S;         // 输入符号集
    std::set<char> G;         // 栈符号集
    std::string q0;           // 初始状态
    char z0;                  // 初始栈符号
    std::set<std::string> F;  // 终止状态集
    std::map<std::tuple<std::string, char, char>, std::set<std::tuple<std::string, std::string>>> delta; // 转移函数

    std::string current_state;
    std::stack<char> stack;


    bool parse_Q_set(const std::string& str, std::set<std::string>& result_set);
    bool parse_S_set(const std::string& str, std::set<char>& result_set);
    bool parse_F_set(const std::string& str, std::set<std::string>& result_set);
    bool parse_delta(const std::string& line);
    void pda_init();
    bool goto_next_state(char input_symbol);
    bool is_final_state();
};

#endif // PDA_H