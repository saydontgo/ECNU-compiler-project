#include "slr1_analyzer.h"
int Item::count_ = 0;

std::string EMPTYCH = "E";

std::string FirstWord(std::string s)
{
    s += " ";
    std::string first = s.substr(0, s.find(" "));
    return first;
}

std::vector<std::string> Split(std::string s, std::string separator)
{
    std::vector<std::string> v;

    std::string::size_type pos1, pos2;
    pos2 = s.find(separator);
    pos1 = 0;

    while (std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + separator.size();
        pos2 = s.find(separator, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));

    return v;
}
Item::Item(std::string i) {
    id = count_++;
    left_ = i.substr(0, i.find("->"));
    right_ = i.substr(i.find("->") + 2);
    item_ = left_ + "->" + right_;

    if (right_.find(".") == std::string::npos)
        AddDot(0);
}

Item::Item(std::string l, std::string r) {
    id = count_++;
    left_ = l;
    right_ = r;
    item_ = left_ + "->" + right_;

    if (right_.find(".") == std::string::npos)
        AddDot(0);
}

auto Item::GetLeft() const -> const std::string& {
    return left_;
}

auto Item::GetRight() const -> const std::string& {
    return right_;
}

auto Item::GetItem() -> const std::string& {
    item_ = left_ + "->" + right_;
    return item_;
}

auto Item::GetDot() const -> int {
    return item_.find(".");
}

void Item::AddDot(int pos) {
    if (right_[pos] == '#')
            right_ = ".";
    else if (pos == 0)
        right_.insert(pos, ". ");
    else if (pos == right_.size())
        right_.insert(pos, " .");
    else
        right_.insert(pos, " . ");
}

auto Item::HasNextDot() const -> bool {
    auto buffer = Split(right_, ".");
    if (buffer.size() > 1)
    {
        return true;
    }
    else
        return false;
}

auto Item::GetNext() const -> std::string {
    auto buffer = Split(item_, ".");
    buffer[1].erase(0, 1);
    return FirstWord(buffer[1]);
}

auto Item::NextDot() -> std::string {
    int dotPos = right_.find(".");
    auto buffer = Split(item_, ".");
    buffer[1].erase(0, 1);
    auto first = FirstWord(buffer[1]);
    int nextPos = dotPos + first.size();
    right_.erase(right_.find("."), 2);
    right_.insert(nextPos, " .");
    return right_;
}

auto Item::operator==(Item& other) -> bool {
    return GetItem() == other.GetItem();
}

bool operator<(const State &x, const State &y) { return x.id < y.id; }

bool operator<(const Item &x, const Item &y) { return x.id < y.id; }

bool operator<(const DFA_edges &x, const DFA_edges &y) { return x.from < y.from; }

bool operator==(const DFA_edges &x, const DFA_edges &y) { return x.from == y.from && x.path == y.path && x.to == y.to; }

bool operator==(const std::set<Item> &x, const std::set<Item> &y) {
    auto it1 = x.begin();
    auto it2 = y.begin();

    for (; it1 != x.end() && it2 != y.end(); it1++, it2++)  // mark
    {
        Item a = *it1;
        Item b = *it2;
        if (a == b)
            continue;
        else
            return false;
    }
    return true;
}

SLR1Analyzer::SLR1Analyzer(std::vector<std::string>& prods) {
        // 读取文法规则
        std::string line;
        for (size_t t = 0; t < prods.size(); t++)
        {
            size_t i;
            line = prods[t];
            // 读取左部
            std::string left = "";
            for (i = 0; line[i] != '-' && i < line.size(); i++)
            {
                left += line[i];
            }

            nonterminal_.push_back(left); // 左部加入非终结符号集
            // 读取右部
            std::string right = line.substr(i + 2, line.size() - i); // 获取产生式右部
            AddP(left, right);                                  // 添加产生式
        }
        AddT(); // 添加终结符
        S = *nonterminal_.begin();
}