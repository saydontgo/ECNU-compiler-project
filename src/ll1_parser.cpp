#include "ll1_parser.h"
#include <cassert>
#include <memory>
#include <stdexcept>

LL1Parser::LL1Parser(const std::unordered_map<std::string, lex_id_t> &key_table)
    : reporter_(std::make_shared<ErrorReporter>()), root_(nullptr),
      invalid_table_(std::make_shared<std::unordered_set<lex_id_t>>()) {
  LL1Analyzer ana(key_table);
  predict_table_ = ana.BuildTable();

  for (const auto &KVpair : key_table) {
    name_table_.insert({KVpair.second, KVpair.first});
  }

  name_table_.insert(
      {static_cast<int>(NonTerminalType::program), std::string("program")});
  name_table_.insert(
      {static_cast<int>(NonTerminalType::stmt), std::string("stmt")});
  name_table_.insert({static_cast<int>(NonTerminalType::compoundstmt),
                      std::string("compoundstmt")});
  name_table_.insert(
      {static_cast<int>(NonTerminalType::stmts), std::string("stmts")});
  name_table_.insert(
      {static_cast<int>(NonTerminalType::ifstmt), std::string("ifstmt")});
  name_table_.insert(
      {static_cast<int>(NonTerminalType::whilestmt), std::string("whilestmt")});
  name_table_.insert(
      {static_cast<int>(NonTerminalType::assgstmt), std::string("assgstmt")});
  name_table_.insert(
      {static_cast<int>(NonTerminalType::boolexpr), std::string("boolexpr")});
  name_table_.insert(
      {static_cast<int>(NonTerminalType::boolop), std::string("boolop")});
  name_table_.insert(
      {static_cast<int>(NonTerminalType::arithexpr), std::string("arithexpr")});
  name_table_.insert({static_cast<int>(NonTerminalType::arithexprprime),
                      std::string("arithexprprime")});
  name_table_.insert(
      {static_cast<int>(NonTerminalType::multexpr), std::string("multexpr")});
  name_table_.insert({static_cast<int>(NonTerminalType::multexprprime),
                      std::string("multexprprime")});
  name_table_.insert({static_cast<int>(NonTerminalType::simpleexpr),
                      std::string("simpleexpr")});
  name_table_.insert(
      {static_cast<int>(NonTerminalType::end), std::string("E")});

  invalid_table_->insert(static_cast<int>(NonTerminalType::stmts));
  invalid_table_->insert(static_cast<int>(NonTerminalType::stmt));
  invalid_table_->insert(static_cast<int>(NonTerminalType::compoundstmt));
  invalid_table_->insert(static_cast<int>(NonTerminalType::boolexpr));
  invalid_table_->insert(static_cast<int>(NonTerminalType::boolop));
  invalid_table_->insert(static_cast<int>(NonTerminalType::arithexpr));
  invalid_table_->insert(static_cast<int>(NonTerminalType::arithexprprime));
  invalid_table_->insert(static_cast<int>(NonTerminalType::multexpr));
  invalid_table_->insert(static_cast<int>(NonTerminalType::multexprprime));
  invalid_table_->insert(static_cast<int>(NonTerminalType::simpleexpr));
  invalid_table_->insert(static_cast<int>(NonTerminalType::end));
  invalid_table_->insert(key_table.find("{")->second);
  invalid_table_->insert(key_table.find("}")->second);
  invalid_table_->insert(key_table.find("(")->second);
  invalid_table_->insert(key_table.find(")")->second);
  invalid_table_->insert(key_table.find(";")->second);
  invalid_table_->insert(key_table.find("then")->second);

  ParseTreeNode::invalid_table_ = this->invalid_table_;
  ParseTreeNode::key_table_ = key_table;

  ParseTreeNode::rank_table_.insert({"==", boolexpr});
  ParseTreeNode::rank_table_.insert({">=", boolexpr});
  ParseTreeNode::rank_table_.insert({"<=", boolexpr});
  ParseTreeNode::rank_table_.insert({"<", boolexpr});
  ParseTreeNode::rank_table_.insert({">", boolexpr});

  ParseTreeNode::rank_table_.insert({"+", plusminus});
  ParseTreeNode::rank_table_.insert({"-", plusminus});

  ParseTreeNode::rank_table_.insert({"*", multdiv});
  ParseTreeNode::rank_table_.insert({"/", multdiv});
}

/*
 *  parse the tokens into a parse tree (not an abstract one), if there is
 * already a root, it will rewrite the root with a new one.
 */
auto LL1Parser::ParseTokens(std::shared_ptr<const TokenStream> tokens)
    -> std::shared_ptr<ParseTreeNode> {
  std::stack<int> parsing_stack;
  std::stack<std::shared_ptr<ParseTreeNode>> nodes;
  size_t index = 0;
  auto start_symbol = static_cast<int>(NonTerminalType::threshold) + 1;
  parsing_stack.push(start_symbol);
  root_ =
      std::make_shared<ParseTreeNode>(name_table_[start_symbol], start_symbol);
  nodes.push(root_);

  while (!parsing_stack.empty()) {
    auto symbol = parsing_stack.top();
    auto node = nodes.top();
    nodes.pop();
    parsing_stack.pop();
    assert(symbol == node->GetId());
    auto token = Token({-1, -1, " unknown sentence ",
                        tokens->TokenAt(tokens->Size() - 1).line_,
                        tokens->TokenAt(tokens->Size() - 1).col_ + 1});
    if (index < tokens->Size()) {
      token = tokens->TokenAt(index);
    }
    if (symbol < static_cast<int>(NonTerminalType::threshold)) {
      // it is a terminal symbol
      if (symbol == token.id_) {
        index++;
      } else {
        // missing non-terminal symbol
        auto last_token = token;
        if (index - 1 >= 0) {
          last_token = tokens->TokenAt(index - 1);
        }
        reporter_->Report(ErrorLevel::Error, ErrorCode::MissingSymbol,
                          last_token.line_, last_token.col_,
                          name_table_[symbol]);
      }
    } else if (symbol < static_cast<int>(NonTerminalType::end)) {
      // it is a non-terminal symbol
      auto prods = predict_table_[static_cast<NonTerminalType>(symbol)];
      if (prods.find(token.id_) != prods.end()) {
        auto prod = prods[token.id_];
        for (int i = prod.size() - 1; i > -1; i--) {
          parsing_stack.push(prod[i]);
          auto new_node =
              std::make_shared<ParseTreeNode>(name_table_[prod[i]], prod[i]);
          assert(node != nullptr);
          node->PushFront(new_node);
          nodes.push(new_node);
        }
      } else {
        auto flag = false;
        // check for eplison productions
        for (const auto &prod : prods) {
          if (prod.second[0] == static_cast<int>(NonTerminalType::end)) {
            auto new_node = std::make_shared<ParseTreeNode>(
                name_table_[prod.second[0]], prod.second[0]);
            assert(node != nullptr);
            node->PushFront(new_node);
            flag = true;
            break;
          }
        }
        if (!flag) {
          // unexpected token
          reporter_->Report(ErrorLevel::Error,
                            ErrorCode::UnexpectedCharacterChinese, token.line_,
                            token.col_, token.lexeme_);
          index++;
        }
      }
    } else if (symbol > static_cast<int>(NonTerminalType::end)) {
      throw std::runtime_error("invalid symbol!\n");
    }
  }

  for (; index < tokens->Size(); index++) {
    auto token = tokens->TokenAt(index);
    reporter_->Report(ErrorLevel::Error, ErrorCode::UnexpectedCharacterChinese,
                      token.line_, token.col_, token.lexeme_);
  }
  return root_;
}

void LL1Parser::PrintErrors() const { reporter_->PrintLL1(); }