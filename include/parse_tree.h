#include "token.h"
#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

enum rank { boolexpr, plusminus, multdiv };
class ParseTreeNode {
public:
  ParseTreeNode() = delete;
  ParseTreeNode(std::string name, lex_id_t id);

  void PushBack(std::shared_ptr<ParseTreeNode> child);
  void PushFront(std::shared_ptr<ParseTreeNode> child);
  void Abstract();
  void PrintOutput() const;
  auto GetName() const -> const std::string &;
  auto GetId() const -> lex_id_t;

  static std::shared_ptr<std::unordered_set<lex_id_t>> invalid_table_;
  static std::unordered_map<std::string, lex_id_t> key_table_;
  static std::unordered_map<std::string, int> rank_table_;

private:
  void PrintDfs(std::shared_ptr<ParseTreeNode> root, int layer) const;
  static auto ConvertToAst(std::shared_ptr<ParseTreeNode> node)
      -> std::shared_ptr<ParseTreeNode>;
  static auto IsInvalid(lex_id_t id) -> bool;
  static void Normalize(std::shared_ptr<ParseTreeNode> node);
  std::deque<std::shared_ptr<ParseTreeNode>> children_;
  std::string name_;
  lex_id_t id_;
};