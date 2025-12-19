#include "ll1_analyzer.h"
#include <exception>

LL1Analyzer::LL1Analyzer(
    const std::unordered_map<std::string, lex_id_t> &key_table) {
  first_[NonTerminalType::program].push_back(key_table.find("{")->second);

  first_[NonTerminalType::stmt].push_back(key_table.find("if")->second);
  first_[NonTerminalType::stmt].push_back(key_table.find("while")->second);
  first_[NonTerminalType::stmt].push_back(key_table.find("ID")->second);
  first_[NonTerminalType::stmt].push_back(key_table.find("{")->second);

  first_[NonTerminalType::compoundstmt].push_back(key_table.find("{")->second);

  first_[NonTerminalType::stmts].push_back(key_table.find("{")->second);
  first_[NonTerminalType::stmts].push_back(key_table.find("if")->second);
  first_[NonTerminalType::stmts].push_back(key_table.find("while")->second);
  first_[NonTerminalType::stmts].push_back(key_table.find("ID")->second);
  follow_[NonTerminalType::stmts].push_back(key_table.find("}")->second);

  first_[NonTerminalType::ifstmt].push_back(key_table.find("if")->second);
  first_[NonTerminalType::whilestmt].push_back(key_table.find("while")->second);
  first_[NonTerminalType::assgstmt].push_back(key_table.find("ID")->second);

  first_[NonTerminalType::boolexpr].push_back(key_table.find("ID")->second);
  first_[NonTerminalType::boolexpr].push_back(key_table.find("NUM")->second);
  first_[NonTerminalType::boolexpr].push_back(key_table.find("(")->second);

  first_[NonTerminalType::boolop].push_back(key_table.find("<")->second);
  first_[NonTerminalType::boolop].push_back(key_table.find(">")->second);
  first_[NonTerminalType::boolop].push_back(key_table.find("<=")->second);
  first_[NonTerminalType::boolop].push_back(key_table.find(">=")->second);
  first_[NonTerminalType::boolop].push_back(key_table.find("==")->second);

  first_[NonTerminalType::arithexpr].push_back(key_table.find("ID")->second);
  first_[NonTerminalType::arithexpr].push_back(key_table.find("NUM")->second);
  first_[NonTerminalType::arithexpr].push_back(key_table.find("(")->second);

  first_[NonTerminalType::arithexprprime].push_back(
      key_table.find("+")->second);
  first_[NonTerminalType::arithexprprime].push_back(
      key_table.find("-")->second);
  follow_[NonTerminalType::arithexprprime].push_back(
      key_table.find("<")->second);
  follow_[NonTerminalType::arithexprprime].push_back(
      key_table.find(">")->second);
  follow_[NonTerminalType::arithexprprime].push_back(
      key_table.find("<=")->second);
  follow_[NonTerminalType::arithexprprime].push_back(
      key_table.find(">=")->second);
  follow_[NonTerminalType::arithexprprime].push_back(
      key_table.find("==")->second);
  follow_[NonTerminalType::arithexprprime].push_back(
      key_table.find(")")->second);
  follow_[NonTerminalType::arithexprprime].push_back(
      key_table.find(";")->second);

  first_[NonTerminalType::multexpr].push_back(key_table.find("ID")->second);
  first_[NonTerminalType::multexpr].push_back(key_table.find("NUM")->second);
  first_[NonTerminalType::multexpr].push_back(key_table.find("(")->second);

  first_[NonTerminalType::multexprprime].push_back(key_table.find("*")->second);
  first_[NonTerminalType::multexprprime].push_back(key_table.find("/")->second);
  follow_[NonTerminalType::multexprprime].push_back(
      key_table.find("+")->second);
  follow_[NonTerminalType::multexprprime].push_back(
      key_table.find("-")->second);
  follow_[NonTerminalType::multexprprime].push_back(
      key_table.find("<")->second);
  follow_[NonTerminalType::multexprprime].push_back(
      key_table.find(">")->second);
  follow_[NonTerminalType::multexprprime].push_back(
      key_table.find(">=")->second);
  follow_[NonTerminalType::multexprprime].push_back(
      key_table.find("<=")->second);
  follow_[NonTerminalType::multexprprime].push_back(
      key_table.find("==")->second);
  follow_[NonTerminalType::multexprprime].push_back(
      key_table.find(")")->second);
  follow_[NonTerminalType::multexprprime].push_back(
      key_table.find(";")->second);

  first_[NonTerminalType::simpleexpr].push_back(key_table.find("ID")->second);
  first_[NonTerminalType::simpleexpr].push_back(key_table.find("NUM")->second);
  first_[NonTerminalType::simpleexpr].push_back(key_table.find("(")->second);

  // productions genration
  production_.push_back(std::vector<std::vector<int>>(
      {{static_cast<int>(NonTerminalType::compoundstmt)}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{static_cast<int>(NonTerminalType::ifstmt)},
       {static_cast<int>(NonTerminalType::whilestmt)},
       {static_cast<int>(NonTerminalType::assgstmt)},
       {static_cast<int>(NonTerminalType::compoundstmt)}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{key_table.find("{")->second, static_cast<int>(NonTerminalType::stmts),
        key_table.find("}")->second}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{static_cast<int>(NonTerminalType::stmt),
        static_cast<int>(NonTerminalType::stmts)},
       {static_cast<int>(NonTerminalType::end)}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{key_table.find("if")->second, key_table.find("(")->second,
        static_cast<int>(NonTerminalType::boolexpr),
        key_table.find(")")->second, key_table.find("then")->second,
        static_cast<int>(NonTerminalType::stmt), key_table.find("else")->second,
        static_cast<int>(NonTerminalType::stmt)}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{key_table.find("while")->second, key_table.find("(")->second,
        static_cast<int>(NonTerminalType::boolexpr),
        key_table.find(")")->second,
        static_cast<int>(NonTerminalType::stmt)}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{key_table.find("ID")->second, key_table.find("=")->second,
        static_cast<int>(NonTerminalType::arithexpr),
        key_table.find(";")->second}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{static_cast<int>(NonTerminalType::arithexpr),
        static_cast<int>(NonTerminalType::boolop),
        static_cast<int>(NonTerminalType::arithexpr)}}));
  production_.push_back(
      std::vector<std::vector<int>>({{key_table.find("<")->second},
                                     {key_table.find(">")->second},
                                     {key_table.find("<=")->second},
                                     {key_table.find(">=")->second},
                                     {key_table.find("==")->second}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{static_cast<int>(NonTerminalType::multexpr),
        static_cast<int>(NonTerminalType::arithexprprime)}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{key_table.find("+")->second,
        static_cast<int>(NonTerminalType::multexpr),
        static_cast<int>(NonTerminalType::arithexprprime)},
       {key_table.find("-")->second,
        static_cast<int>(NonTerminalType::multexpr),
        static_cast<int>(NonTerminalType::arithexprprime)},
       {static_cast<int>(NonTerminalType::end)}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{static_cast<int>(NonTerminalType::simpleexpr),
        static_cast<int>(NonTerminalType::multexprprime)}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{key_table.find("/")->second,
        static_cast<int>(NonTerminalType::simpleexpr),
        static_cast<int>(NonTerminalType::multexprprime)},
       {key_table.find("*")->second,
        static_cast<int>(NonTerminalType::simpleexpr),
        static_cast<int>(NonTerminalType::multexprprime)},
       {static_cast<int>(NonTerminalType::end)}}));
  production_.push_back(std::vector<std::vector<int>>(
      {{key_table.find("ID")->second},
       {key_table.find("NUM")->second},
       {key_table.find("(")->second,
        static_cast<int>(NonTerminalType::arithexpr),
        key_table.find(")")->second}}));
}

auto LL1Analyzer::BuildTable()
    -> std::unordered_map<NonTerminalType,
                          std::unordered_map<lex_id_t, std::vector<int>>> {
  std::unordered_map<NonTerminalType,
                     std::unordered_map<lex_id_t, std::vector<int>>>
      res;
  for (const auto &row : first_) {
    std::unordered_map<lex_id_t, std::vector<int>> tmp;
    BuildTableHelper(tmp, row);
    auto iter_follow = follow_.find(row.first);
    if (iter_follow != follow_.end()) {
      BuildTableHelper(tmp, *iter_follow);
    }
    res.insert({row.first, tmp});
  }
  return res;
}

void LL1Analyzer::BuildTableHelper(
    std::unordered_map<lex_id_t, std::vector<int>> &res,
    const std::pair<NonTerminalType, std::vector<lex_id_t>> &row) {
  auto index = static_cast<int>(row.first) -
               static_cast<int>(NonTerminalType::threshold) - 1;
  auto productions = production_[index];
  for (const auto &id : row.second) {
    for (const auto &prod : productions) {
      if (prod[0] < static_cast<const int>(NonTerminalType::threshold) &&
          prod[0] == id) {
        res.insert({id, prod});
        break;
      } else if (prod[0] > static_cast<const int>(NonTerminalType::threshold)) {
        // last production
        if (prod[0] == static_cast<const int>(NonTerminalType::end)) {
          res.insert({id, prod});
          break;
        }

        // non-terminal symbol
        auto ters = first_[static_cast<NonTerminalType>(prod[0])];
        auto flag = false;
        for (const auto &ter : ters) {
          if (ter == id) {
            res.insert({id, prod});
            flag = true;
            break;
          }
        }
        auto it = follow_.find(static_cast<NonTerminalType>(prod[0]));
        if (it != follow_.end()) {
          for (const auto &ter : it->second) {
            if (ter == id) {
              auto check = res.insert({id, prod});
              if (!check.second) {
                throw std::runtime_error("ll1 rules conflict!\n");
              }
              flag = true;
              break;
            }
          }
        }
        if (flag) {
          break;
        }
      }
    }
  }
}

// print the productions
void LL1Analyzer::PrintProds() {
  for (auto i = static_cast<int>(NonTerminalType::threshold) + 1;
       i < static_cast<int>(NonTerminalType::end); i++) {
    std::cout << i << ": " << std::endl;
    for (const auto &prod :
         production_[i - static_cast<int>(NonTerminalType::threshold) - 1]) {
      std::cout << i << " -> ";
      for (const auto &id : prod) {
        std::cout << id << " ";
      }
      std::cout << std::endl;
    }
  }
}