#pragma once
#include "token.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
enum class NonTerminalType {
  threshold = 100,
  program,
  stmt,
  compoundstmt,
  stmts,
  ifstmt,
  whilestmt,
  assgstmt,
  boolexpr,
  boolop,
  arithexpr,
  arithexprprime,
  multexpr,
  multexprprime,
  simpleexpr,
  end
};
class LL1Analyzer {
public:
  explicit LL1Analyzer(
      const std::unordered_map<std::string, lex_id_t> &key_table);
  auto BuildTable()
      -> std::unordered_map<NonTerminalType,
                            std::unordered_map<lex_id_t, std::vector<int>>>;

  // for debug and output purposes, print all the productions
  void PrintProds();

private:
  std::unordered_map<NonTerminalType, std::vector<lex_id_t>> first_;
  std::unordered_map<NonTerminalType, std::vector<lex_id_t>> follow_;
  std::vector<std::vector<std::vector<int>>> production_;

  void BuildTableHelper(
      std::unordered_map<lex_id_t, std::vector<int>> &res,
      const std::pair<NonTerminalType, std::vector<lex_id_t>> &row);
};