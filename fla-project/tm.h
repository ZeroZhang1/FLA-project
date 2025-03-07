#ifndef TM_H
#define TM_H

#include <string>
#include <cstring>
#include <set>
#include <map>
#include <tuple>
#include <cassert>
#include <stack>

using namespace std;

class TM {
public:
    TM();
    bool tm_parser(std::string filename);
    void set_verbose(bool verbose);
    void set_superverbose(bool superverbose);
    bool input_check(const std::string& input);
    bool run_tm(const std::string& input);
    std::string get_tape_content() const;
    bool superverbose;

private:
    int step;                   // 步数
    int index[12];              // 纸带指针
    bool verbose;
    std::set<std::string> Q;    // 状态集
    std::set<char> S;           // 输入符号集
    std::set<char> G;           // 纸带符号集
    std::string q0;             // 初始状态
    char B;                     // 空格符号
    std::set<std::string> F;    // 终止状态集
    int N;                      // 纸带数
    char tape[12][1024];       // 纸带
    bool is_empty_tape(int n) const;  // 判断纸带是否为空
    // 转移函数的定义
    std::map<
        std::pair<std::string, std::string>,     // key: <旧状态, 旧符号组>
        std::tuple<std::string, std::string, std::string>  // value: <新状态, 新符号组, 方向组>
    > delta;

    std::string current_state;

    bool parse_Q_set(const std::string& str, std::set<std::string>& result_set);
    bool parse_S_set(const std::string& str, std::set<char>& result_set);
    bool parse_G_set(const std::string& str, std::set<char>& result_set);
    bool parse_F_set(const std::string& str, std::set<std::string>& result_set);
    bool parse_delta(const std::string& line);
    bool goto_next_state();
    bool is_final_state();
    void tape_init(const std::string& input);
    void verbose_info() const;
    std::pair<int, int> find_first_last(int n) const;
    bool match_symbols(const std::string& pattern, const char* actual);
    void print_tape_info(int tape_num, int first, int last) const;
};

#endif // PDA_H