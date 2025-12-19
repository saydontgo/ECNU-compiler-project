#include "slr1_analyzer.h"
int Item::count_ = 0;

std::string EMPTYCH = "E";

std::string FirstWord(std::string s) {
  s += " ";
  std::string first = s.substr(0, s.find(" "));
  return first;
}

std::vector<std::string> Split(std::string s, std::string separator) {
  std::vector<std::string> v;

  std::string::size_type pos1, pos2;
  pos2 = s.find(separator);
  pos1 = 0;

  while (std::string::npos != pos2) {
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

auto Item::GetLeft() const -> const std::string & { return left_; }

auto Item::GetRight() const -> const std::string & { return right_; }

auto Item::GetItem() -> const std::string & {
  item_ = left_ + "->" + right_;
  return item_;
}

auto Item::GetDot() const -> int { return item_.find("."); }

void Item::AddDot(size_t pos) {
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
  if (buffer.size() > 1) {
    return true;
  } else
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

auto Item::operator==(Item &other) -> bool {
  return GetItem() == other.GetItem();
}

bool operator<(const State &x, const State &y) { return x.id < y.id; }

bool operator<(const Item &x, const Item &y) { return x.id < y.id; }

bool operator<(const DFA_edges &x, const DFA_edges &y) {
  return x.from < y.from;
}

bool operator==(const DFA_edges &x, const DFA_edges &y) {
  return x.from == y.from && x.path == y.path && x.to == y.to;
}

bool operator==(const std::set<Item> &x, const std::set<Item> &y) {
  auto it1 = x.begin();
  auto it2 = y.begin();

  for (; it1 != x.end() && it2 != y.end(); it1++, it2++) // mark
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

SLR1Analyzer::SLR1Analyzer(std::vector<std::string> &prods) {
  // 读取文法规则
  std::string line;
  for (size_t t = 0; t < prods.size(); t++) {
    size_t i;
    line = prods[t];
    // 读取左部
    std::string left = "";
    for (i = 0; line[i] != '-' && i < line.size(); i++) {
      left += line[i];
    }

    nonterminal_.push_back(left); // 左部加入非终结符号集
    // 读取右部
    std::string right = line.substr(i + 2, line.size() - i); // 获取产生式右部
    AddP(left, right);                                       // 添加产生式
  }
  AddT(); // 添加终结符
  S = *nonterminal_.begin();

  Convert2Extend();
  ComputeFirst();
  ComputeFollow();
  BuildDFA();
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
          if ((find(nonterminal_.begin(), nonterminal_.end(), tmp) ==
               nonterminal_.end()) &&
              tmp != EMPTYCH) {
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
  for (std::string X : terminal_) {
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
            if (right[right.find(B) + B.size()] != ' ' &&
                right[right.find(B) + B.size()] != '\0') {
              std::string s = right.substr(right.find(B));              // E'b
              std::string tmp = right.substr(right.find(B) + B.size()); //' b

              // A->E'
              if (tmp.find(" ") == std::string::npos) {
                B = s; // B:E->E'
                follow_set_[B].insert(follow_set_[A].begin(),
                                      follow_set_[A].end());
                flag = 1;
              } else {
                // A->E'b
                B = s.substr(0, s.find(" "));
                tmp = tmp.substr(tmp.find(" ") + 1); // b
                if (tmp.find(" ") == std::string::npos) {
                  b = tmp;
                } else {
                  b = tmp.substr(0, tmp.find(" "));
                }
              }
            } else if (right[right.find(B) + B.size()] == ' ') { // A->aEb

              std::string tmp = right.substr(right.find(B) + B.size() + 1);
              if (tmp.find(" ") == std::string::npos) {
                b = tmp;
              } else {
                b = tmp.substr(0, tmp.find(" "));
              }
            } else { // A->aE
              follow_set_[B].insert(follow_set_[A].begin(),
                                    follow_set_[A].end());
              flag = 1;
            }

            // get follow_set_[B]
            if (flag == 0) {
              // first_set_[b] doesn't include E
              if (first_set_[b].find(EMPTYCH) == first_set_[b].end()) {
                follow_set_[B].insert(first_set_[b].begin(),
                                      first_set_[b].end());
              } else {
                for (std::string follow : first_set_[b]) {
                  if (follow == EMPTYCH) {
                    continue;
                  } else {
                    follow_set_[B].insert(follow);
                  }
                }
                follow_set_[B].insert(follow_set_[A].begin(),
                                      follow_set_[A].end());
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
        for (auto it2 = prods_[first].begin(); it2 != prods_[first].end();
             it2++) {
          Item tmp(first, *it2);
          if (!InItemSet(tmp, C)) {
            C.insert(tmp);
            bfs.push(tmp);
          }
        }
      }
    }
  }
  return C;
}

void SLR1Analyzer::BuildDFA() {
  State s0;          // initial item sets
  s0.id = number_++; // state numbers

  // initialize item set
  std::string firstRight = *(prods_[S].begin());
  Item start(S, firstRight);
  s0.items = GetClosure(start);
  states_.insert(s0);

  // build DFA
  for (State s : states_) {
    std::map<std::string, int> Paths;
    for (Item now : s.items) {
      now.GetItem();
      if (now.HasNextDot()) {
        std::string path = now.GetNext();
        Item nextD(now.GetLeft(), now.NextDot());
        std::set<Item> next = GetClosure(nextD);
        int oldDes;
        if (Paths.find(path) != Paths.end()) {
          oldDes = Paths.find(path)->second;

          for (State dest : states_) {
            if (dest.id == oldDes) {
              dest.items.insert(next.begin(), next.end());
              next = dest.items;

              // update states
              states_.erase(dest);
              states_.insert(dest);

              int tID = HasState(next);
              if (tID != -1) {
                for (size_t i = 0; i < edges_.size(); i++) {
                  if (edges_[i].to == oldDes) {
                    edges_[i].to = tID;
                  }
                }
              }
            }
          }
        }

        int tID = HasState(next);
        if (tID == -1) {
          State t;
          t.id = number_++;
          t.items = next;
          states_.insert(t);
          Paths.insert(std::pair<std::string, int>(path, t.id));
          edges_.push_back(DFA_edges(s.id, path, t.id));
        } else {
          Paths.insert(std::pair<std::string, int>(path, tID));
          edges_.push_back(DFA_edges(s.id, path, tID));
        }
      }
    }
  }

  // delete duplicated DFA_edges
  sort(edges_.begin(), edges_.end());
  edges_.erase(unique(edges_.begin(), edges_.end()), edges_.end());
  // handle duplicated states
  for (auto i = states_.begin(); i != states_.end(); i++) {
    for (auto j = states_.begin(); j != states_.end(); j++) {
      // find one
      if ((*j).id > (*i).id && (*i).items == (*j).items) {
        int erase_id = (*j).id;
        j = states_.erase(j);
        j--;

        for (State s : states_) {
          if (s.id > erase_id) {
            // update again
            State &newS = const_cast<State &>(*states_.find(s));
            newS.id--;
          }
        }

        for (size_t i = 0; i < edges_.size(); i++) {
          if (edges_[i].from == erase_id || edges_[i].to == erase_id)
            edges_.erase(find(edges_.begin(), edges_.end(), edges_[i]));
          if (edges_[i].from > erase_id)
            edges_[i].from--;
          if (edges_[i].to > erase_id)
            edges_[i].to--;
        }
      }
    }
  }
}
void SLR1Analyzer::CreateSLRTable(
    std::map<std::pair<int, std::string>, std::string> &action_table,
    std::map<std::pair<int, std::string>, int> &goto_table) {
  AddPwithNum();
  std::string s = S;
  s = s.erase(s.find("'"));

  std::pair<int, std::string> title(1, "#");
  action_table[title] = "acc";

  for (DFA_edges go : edges_) {
    // 目的地是nonterminal_
    if (Isnonterminal(go.path)) {
      std::pair<int, std::string> title(go.from, go.path);
      goto_table[title] = go.to;
    }
    // 加入action表
    else {
      // shift
      std::pair<int, std::string> title(go.from, go.path);
      action_table[title] = "s" + std::to_string(go.to);
    }
    // reduce
    std::string rnonterminal_ = ShouldReduce(go.to);
    if (rnonterminal_ != "") {
      if (go.path != s) {
        std::vector<std::string> x = terminal_;
        x.push_back("#");

        for (std::string p : x) {
          std::set<std::string> follow = follow_set_[rnonterminal_];
          if (follow.find(p) != follow.end()) {
            std::pair<int, std::string> title(go.to, p);
            action_table[title] = "r" + std::to_string(FindReduce(go.to));
          }
        }
      }
    }
  }
}
auto SLR1Analyzer::Isnonterminal(std::string &token) -> bool {
  return find(nonterminal_.begin(), nonterminal_.end(), token) !=
         nonterminal_.end();
}

auto SLR1Analyzer::InItemSet(Item &target, std::set<Item> &c) -> bool {
  for (Item i : c) {
    if (i.GetItem() == target.GetItem())
      return true;
  }
  return false;
}

// whether it should be reduced
auto SLR1Analyzer::ShouldReduce(int num) -> std::string {
  for (State s : states_) {
    if (s.id == num) {
      for (Item i : s.items) {
        if (i.HasNextDot())
          return "";
        else
          return i.GetLeft();
      }
    }
  }
  return "";
}

// find the production to reduce based on state numbers and return its numbers
auto SLR1Analyzer::FindReduce(int num) -> int {
  for (State s : states_) {
    if (s.id == num) {
      for (Item i : s.items) {
        std::string tmp = i.GetItem();
        tmp.erase(tmp.find("."));
        tmp.pop_back();
        return prod2num_.find(tmp)->second;
      }
    }
  }
  return -1;
}

// find the production to reduce based on item set and return its numbers
auto SLR1Analyzer::FindReduce(Item &item) -> int {
  std::string tmp = item.GetItem();
  tmp.erase(tmp.find("."));
  tmp.pop_back();
  if (prod2num_.find(tmp) != prod2num_.end())
    return prod2num_.find(tmp)->second;
  return -1;
}

auto SLR1Analyzer::RightNum(std::string &left, int pos) -> int {
  for (auto it = prod2num_.begin(); it != prod2num_.end(); it++) {
    if (it->second == pos) {
      std::string target = it->first;
      left = target.substr(0, target.find("->"));
      std::string right = target.substr(target.find("->") + 2);
      std::vector<std::string> tmp = Split(right, " ");
      return tmp.size();
    }
  }
  return 0;
}

auto SLR1Analyzer::GetResult(int pos) -> std::string {
  for (auto it = prod2num_.begin(); it != prod2num_.end(); it++) {
    if (it->second == pos) {
      return it->first;
    }
  }
  return 0;
}

auto SLR1Analyzer::HasState(std::set<Item> &target) -> int {
  for (State s : states_) {
    if (s.items.size() != target.size())
      continue;

    if (s.items == target)
      return s.id;
    else
      continue;
  }
  return -1;
}