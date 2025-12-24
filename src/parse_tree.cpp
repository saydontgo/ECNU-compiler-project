#include "parse_tree.h"
#include "ll1_analyzer.h"
#include "token.h"
#include <memory>

std::shared_ptr<std::unordered_set<lex_id_t>> ParseTreeNode::invalid_table_ =
    std::make_shared<std::unordered_set<lex_id_t>>();
std::unordered_map<std::string, lex_id_t> ParseTreeNode::key_table_ =
    std::unordered_map<std::string, lex_id_t>();
std::unordered_map<std::string, int> ParseTreeNode::rank_table_ =
    std::unordered_map<std::string, int>();
ParseTreeNode::ParseTreeNode(std::string name, lex_id_t id)
    : name_(name), id_(id) {}

void ParseTreeNode::PushBack(std::shared_ptr<ParseTreeNode> child) {
  children_.push_back(child);
}

void ParseTreeNode::PushFront(std::shared_ptr<ParseTreeNode> child) {
  children_.push_front(child);
}

auto ParseTreeNode::IsInvalid(lex_id_t id) -> bool {
  return invalid_table_->count(id);
}

void ParseTreeNode::Normalize(std::shared_ptr<ParseTreeNode> node) {
  lex_id_t target = -1;
  std::string nor_name;
  switch (node->id_) {
  case static_cast<lex_id_t>(NonTerminalType::assgstmt):
    target = key_table_["="];
    nor_name = "=";
    break;
  case static_cast<lex_id_t>(NonTerminalType::ifstmt):
    target = key_table_["if"];
    nor_name = "if";
    break;
  case static_cast<lex_id_t>(NonTerminalType::whilestmt):
    target = key_table_["while"];
    nor_name = "while";
    break;
  case static_cast<lex_id_t>(NonTerminalType::arithexpr):
    for (const auto &child : node->children_) {
      switch (child->name_[0]) {
      case '*':
        target = key_table_["*"];
        nor_name = "*";
        break;
      case '+':
        target = key_table_["+"];
        nor_name = "+";
        break;
      case '-':
        target = key_table_["-"];
        nor_name = "-";
        break;
      case '/':
        target = key_table_["/"];
        nor_name = "/";
        break;
      default:
        return;
      }
    }
    break;
  default:
    return;
  }
  node->id_ = target;
  node->name_ = nor_name;
  int delete_flag = 0;
  std::shared_ptr<ParseTreeNode> tmp;
  for (auto &child : node->children_) {
    if (child->id_ == node->id_) {
      if (!child->children_.empty()) {
        child = child->children_[0];
      } else {
        delete_flag = 1;
        tmp = child;
      }
      break;
    }
  }

  if (delete_flag) {
    node->children_.erase(
        std::remove(node->children_.begin(), node->children_.end(), tmp),
        node->children_.end());
  }
}

auto ParseTreeNode::ConvertToAst(std::shared_ptr<ParseTreeNode> node)
    -> std::shared_ptr<ParseTreeNode> {
  // convert all children first
  for (auto &child : node->children_) {
    child = ConvertToAst(child);
  }

  // remove invalid children
  node->children_.erase(
      std::remove(node->children_.begin(), node->children_.end(), nullptr),
      node->children_.end());

  if (IsInvalid(node->GetId())) {
    switch (node->children_.size()) {
    case 0:
      return nullptr;
    case 1:
      return node->children_[0];
    default:
      std::shared_ptr<ParseTreeNode> tmp_child;
      for (auto &child : node->children_) {
        if (child->name_ == "+" || child->name_ == "*" || child->name_ == "-" ||
            child->name_ == "/" || child->name_ == "==" ||
            child->name_ == ">=" || child->name_ == "<=" ||
            child->name_ == ">" || child->name_ == "<") {
          if (tmp_child == nullptr) {
            tmp_child = child;
          } else if (rank_table_[tmp_child->name_] >
                     rank_table_[child->name_]) {
            tmp_child = child;
          }
        }
      }
      node->id_ = tmp_child->id_;
      node->name_ = tmp_child->name_;
      auto grand_children = tmp_child->children_;
      node->children_.erase(std::remove(node->children_.begin(),
                                        node->children_.end(), tmp_child),
                            node->children_.end());
      node->children_.insert(node->children_.end(), grand_children.begin(),
                             grand_children.end());
      return node;
    }
  }

  Normalize(node);
  return node;
}

void ParseTreeNode::Abstract() {
  // convert all children first
  for (auto &child : children_) {
    child = ConvertToAst(child);
  }

  // remove invalid children
  children_.erase(std::remove(children_.begin(), children_.end(), nullptr),
                  children_.end());
}

void ParseTreeNode::PrintDfs(std::shared_ptr<ParseTreeNode> root,
                             int layer) const {
  for (int i = 0; i < layer; i++) {
    std::cout << "\t";
  }
  std::cout << root->name_ << std::endl;
  if (!root->children_.empty()) {
    for (const auto &child : root->children_) {
      PrintDfs(child, layer + 1);
    }
  }
}

void ParseTreeNode::PrintOutput() const {
  PrintDfs(std::make_shared<ParseTreeNode>(*this), 0);
}

auto ParseTreeNode::GetName() const -> const std::string & { return name_; }

auto ParseTreeNode::GetId() const -> lex_id_t { return id_; }