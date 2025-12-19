#pragma once
#include "error_reporter.h"
#include "exception.hpp"
#include "symbol_table.h"
#include "token.h"
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

enum class StateType {
  Initial,
  Identifier,
  Integer,
  Float,
  Minus,
  MinusMinus,
  MinusEqual,
  Dereference,
  PlusPlus,
  PlusEqual,
  LeftParenthesis,
  RightParenthesis,

  Comma,
  Period,
  Colon,
  Semicolon,
  LeftBracket,
  RightBracket,
  LeftBrace,
  RightBrace,
  Tilde,
  Reverse,
  ModSelf,
  AndSelf,
  AndAnd,
  MultiplySelf,
  DivisionSelf,
  XorSelf,
  OrSelf,
  OrOr,
  LeftMoveEqual,
  LessEqual,
  EqualEqual,
  GreaterEqual,
  RightMoveEqual,
  RightWrapperHalfComments,
  QuestionMark,

  SingleQuotationMark,
  DoubleQuotationMark,
  Mod,
  And,
  Multiply,
  Division,
  Xor,
  Or,
  LeftArrow,
  LeftMove,
  Equal,
  RightArrow,
  RightMove,
  LineComments,
  LeftWrapperComments,
  RightWrapperComments,
  Plus,
  Finish
};

enum class FlagType { Identifier, Comments, Constant, Other };
class LexicalAnalyzer {
public:
  explicit LexicalAnalyzer(std::string &source);
  LexicalAnalyzer() = delete;
  LexicalAnalyzer(const LexicalAnalyzer &other) = delete;
  LexicalAnalyzer &operator=(const LexicalAnalyzer &) = delete;
  inline auto Peek() const -> char;
  inline void Advance();
  inline auto PeekNext() const -> char;
  inline auto IsEnd() const -> bool;
  auto Tokenize() -> std::shared_ptr<const TokenStream>;
  auto GetTable() const -> const std::unordered_map<std::string, lex_id_t> &;
  void PrintErrors();

private:
  std::shared_ptr<ErrorReporter> reporter_;
  std::shared_ptr<SymbolTable> symbol_table_;
  std::shared_ptr<TokenStream> tokens_;
  std::string source_;
  int pos_;
  StateType cur_state_;
  int line_{1};
  int col_{1};
  std::unordered_map<std::string, lex_id_t> C_keys_table_;
};

char ToEscape(const char ch);
