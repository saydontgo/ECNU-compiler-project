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

void SLR1Analyzer::Convert2Extend() {
    auto newS = S;
    newS += "'";
    nonterminal_.insert(nonterminal_.begin(), newS);
    prods_[newS].push_back(S);
    S = newS;
}

void SLR1Analyzer::AddP(std::string left, std::string right) {
    right += "#";
    std::string pRight = "";
    for (size_t i = 0; i < right.size(); i++) {
        if (right[i] == '|' || right[i] == '#') {
            prods_[left].push_back(pRight);
            pRight = "";
        } else {
            pRight += right[i];
        }
    }
}

void SLR1Analyzer::AddPwithNum() {
    int i = 0;
    for (std::string left : nonterminal_) {
        for (std::string right : prods_[left]) {
            prod2num_[left + "->" + right] = i;
            i++;
        }
    }
}

void SLR1Analyzer::AddT() {
    std::string tmp = "";
    for (std::string left : nonterminal_) {
        for (std::string right : prods_[left]) {
            right += "#";
            for (size_t i = 0; i < right.size(); i++) {
                if (right[i] == '|' || right[i] == ' ' || right[i] == '#') {
                    // add it to terminal
                    if ((find(nonterminal_.begin(), nonterminal_.end(), tmp) == nonterminal_.end()) && tmp != EMPTYCH) {
                        terminal_.push_back(tmp);
                    }
                    tmp = "";
                } else {
                    tmp += right[i];
                }
            }
        }
    } 

    sort(terminal_.begin(), terminal_.end());
    terminal_.erase(unique(terminal_.begin(), terminal_.end()), terminal_.end());
}

void SLR1Analyzer::ComputeFirst() {
    first_set_.clear();

    first_set_[EMPTYCH].insert(EMPTYCH);
    for (std::string X : terminal_)
    {
        first_set_[X].insert(X);
    }

    // for nonterminal
    int j = 0;
    while (j < 10) {
        for (size_t i = 0; i < nonterminal_.size(); i++) {
            std::string A = nonterminal_[i];

            // iterate A prods
            for (size_t k = 0; k < prods_[A].size(); k++) {
                int Continue = 1;
                std::string right = prods_[A][k];

                std::string first_token;
                if (right.find(" ") == std::string::npos)
                    first_token = right;
                else
                    first_token = right.substr(0, right.find(" "));

                if (!first_set_[first_token].empty()) {
                    for (std::string firstX : first_set_[first_token]) {
                        if (firstX == EMPTYCH) {
                            continue;
                        } else {
                            first_set_[A].insert(firstX);
                            Continue = 0;
                        }
                    }
                    if (Continue) {
                        first_set_[A].insert(EMPTYCH);
                    }
                }
            }
        }
        j++;
    }
}

void SLR1Analyzer::ComputeFollow() {
    follow_set_[S].insert("#");

    auto j = 0;
    while (j < 10) {
        // iterate nonterminal
        for (std::string A : nonterminal_) {
            for (std::string right : prods_[A]) {
                for (std::string B : nonterminal_) {
                    // A->Bb
                    if (right.find(B) != std::string::npos) {
                        // find b
                        std::string b;
                        int flag = 0;
                        // found E'
                        if (right[right.find(B) + B.size()] != ' ' && right[right.find(B) + B.size()] != '\0') {
                            std::string s = right.substr(right.find(B));               // E'b
                            std::string temp = right.substr(right.find(B) + B.size()); //' b

                            // A->E'
                            if (temp.find(" ") == std::string::npos) {
                                B = s;                                                // B:E->E'
                                follow_set_[B].insert(follow_set_[A].begin(), follow_set_[A].end()); 
                                flag = 1;
                            } else {
                                // A->E'b
                                B = s.substr(0, s.find(" "));
                                temp = temp.substr(temp.find(" ") + 1); // b
                                if (temp.find(" ") == std::string::npos) {
                                    b = temp;
                                } else {
                                    b = temp.substr(0, temp.find(" "));
                                }
                            }
                        } else if (right[right.find(B) + B.size()] == ' ') {  // A->aEb
                        
                            std::string temp = right.substr(right.find(B) + B.size() + 1);
                            if (temp.find(" ") == std::string::npos) {
                                b = temp;
                            } else {
                                b = temp.substr(0, temp.find(" "));
                            }
                        } else {  // A->aE
                            follow_set_[B].insert(follow_set_[A].begin(), follow_set_[A].end());
                            flag = 1;
                        }

                        // get follow_set_[B]
                        if (flag == 0) {
                            // first_set_[b] doesn't include E
                            if (first_set_[b].find(EMPTYCH) == first_set_[b].end()) {
                                follow_set_[B].insert(first_set_[b].begin(), first_set_[b].end());
                            } else {
                                for (std::string follow : first_set_[b]) {
                                    if (follow == EMPTYCH) {
                                        continue;
                                    } else {
                                        follow_set_[B].insert(follow);
                                    }
                                }
                                follow_set_[B].insert(follow_set_[A].begin(), follow_set_[A].end());
                            }
                        }
                    }
                }
            }
        }
        j++;
    }
}

auto SLR1Analyzer::GetClosure(Item item) -> std::set<Item> {
    std::set<Item> C; // closures
    C.insert(item);

    std::queue<Item> bfs;
    bfs.push(item);

    while (!bfs.empty()) {
        Item now = bfs.front();
        bfs.pop();

        std::vector<std::string> buffer = Split(now.GetRight(), ".");

        if (buffer.size() > 1) {
            std::string first = FirstWord(buffer[1].erase(0, 1));

            if (Isnonterminal(first)) {
                for (auto it2 = prods_[first].begin(); it2 != prods_[first].end(); it2++) {
                    Item temp(first, *it2);
                    if (!InItemSet(temp, C)) {
                        C.insert(temp);
                        bfs.push(temp);
                    }
                }
            }
        }
    }
    return C;
}

void SLR1Analyzer::
void SLR1Analyzer::
void SLR1Analyzer::
void SLR1Analyzer::
void SLR1Analyzer::
void SLR1Analyzer::
void SLR1Analyzer::
void SLR1Analyzer::
void SLR1Analyzer::