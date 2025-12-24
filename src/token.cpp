#include "token.h"
#include <cstddef>

void TokenStream::PushBack(lex_id_t id, std::string &&lexeme, int line,
                           int column) {
  tokens_.push_back(
      Token({pos_.fetch_add(1), id, std::move(lexeme), line, column}));
}

void TokenStream::PrintAll() const {
  for (const auto &token : tokens_) {
    std::cout << token.pos_ << ": <" << token.lexeme_ << "," << token.id_
              << ">\n";
  }
}

void TokenStream::Print(int index) const {
  assert(index > -1 && size_t(index) < tokens_.size());
  std::cout << tokens_[index].pos_ << ": <" << tokens_[index].lexeme_ << ","
            << tokens_[index].id_ << ">\n";
}

auto TokenStream::TokenAt(int index) const -> const Token & {
  assert(index > -1 && size_t(index) < tokens_.size());
  return tokens_[index];
}

auto TokenStream::Size() const -> size_t { return tokens_.size(); }