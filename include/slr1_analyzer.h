#pragma once
#include <algorithm>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

class Item {
public:
  Item(std::string i);
  Item(std::string l, std::string r);
  auto GetLeft() const -> const std::string &;
  auto GetRight() const -> const std::string &;
  auto GetItem() -> const std::string &;
  auto GetDot() const -> int;
  void AddDot(size_t pos);

  auto HasNextDot() const -> bool;
  auto GetNext() const -> std::string;
  auto NextDot() -> std::string;
  auto operator==(Item &other) -> bool;
  int id;

private:
  std::string item_;
  std::string left_;
  std::string right_;
  static int count_;
};

// edges of DFA
struct DFA_edges {
  int from;
  int to;
  std::string path;

  DFA_edges(int f, std::string p, int t) : from(f), to(t), path(p) {}
};

// state in DFA
struct State {
  int id;               // state id
  std::set<Item> items; // item set
};

class SLR1Analyzer {
public:
  SLR1Analyzer() = default;
  SLR1Analyzer(std::vector<std::string> &prods);

  void CreateSLRTable(
      std::map<std::pair<int, std::string>, std::string> &action_table,
      std::map<std::pair<int, std::string>, int> &goto_table);

  // for debug propuses
  auto GetTerminals() -> std::vector<std::string> { return terminal_; }
  auto GetStates() -> std::set<State> { return states_; }
  auto GetNonT() -> std::vector<std::string> { return nonterminal_; }

private:
  std::vector<std::string> terminal_;
  std::vector<std::string> nonterminal_;
  std::string S;
  std::map<std::string, std::vector<std::string>> prods_;
  std::map<std::string, std::set<std::string>> first_set_;
  std::map<std::string, std::set<std::string>> follow_set_;
  std::map<std::string, int> prod2num_;
  std::set<State> states_;
  std::vector<DFA_edges> edges_;
  int number_{0};

  void Convert2Extend();
  void AddP(std::string left, std::string right);
  void AddPwithNum();
  void AddT();
  void ComputeFirst();
  void ComputeFollow();
  auto GetClosure(Item item) -> std::set<Item>;
  void BuildDFA();
  auto Isnonterminal(std::string &token) -> bool;
  auto InItemSet(Item &temp, std::set<Item> &c) -> bool;
  auto ShouldReduce(int num) -> std::string;
  auto FindReduce(int num) -> int;
  auto FindReduce(Item &item) -> int;
  auto RightNum(std::string &left, int pos) -> int;
  auto GetResult(int pos) -> std::string;
  auto HasState(std::set<Item> &target) -> int;
};