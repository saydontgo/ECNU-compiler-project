#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>


class Item {
public:
    Item(std::string i);
    Item(std::string l, std::string r);
    auto GetLeft() const -> std::string&;
    auto GetRight() const -> std::string&;
    auto GetItem() const -> std::string&;
    auto GetDot() const -> int;
    void AddDot(int pos);

    auto HasNextDot() const -> bool;
    auto GetNext() const -> std::string;
    auto NextDot() const -> std::string;
    auto operator==(const Item& other) -> bool;
    int id;

private:
    std::string item_;
    std::string left_;
    std::string right_;
    static int count_;
};

// DFA的边
struct DFA_edges
{
    int from;
    int to;
    std::string path;

    DFA_edges(int f, std::string p, int t) : from(f), to(t), path(p) {}
};

// state in DFA
struct State
{
    int id;          // 状态编号
    std::set<Item> items; // 项目集
};

class SLR1Analyzer {
public:
    SLR1Analyzer();
    SLR1Analyzer(std::vector<std::string>& prods);

    void CreateSLRTable(std::map<std::pair<int, std::string>, std::string>& action_table,
        std::map<std::pair<int, std::string>, int>& goto_table);

private:
    std::vector<std::string> terminal_;                           // 终结符号集合
    std::vector< std::string> nonterminal_;                          // 非终结符号集合
    std::string S;                                   // 开始符号
    std::map< std::string, std::vector<std::string>> prods_;     // 产生式
    std::map<std::string, std::set<std::string>> first_set_;             // first std::set_集
    std::map<std::string, std::set<std::string>> follow_set_;            // follow std::set_集
    std::map<std::string, int> prod2num_;                    // 编号的产生式集合，用于规约
    std::set<State> states_;                          // 状态集合
    std::vector<DFA_edges> edges_;                            // 转换函数
};