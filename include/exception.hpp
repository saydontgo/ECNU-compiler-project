#pragma once
#include <iostream>
#include <stdexcept>

enum class Module { lexical_analyzer, symbol_table, token };
class LexerException : public std::exception {
public:
  explicit LexerException(std::string msg,
                          Module mod = Module::lexical_analyzer)
      : message_(std::move(msg)), module_(mod) {}

  // rewrite what() method
  const char *what() const noexcept override { return message_.c_str(); }

  // return the error module
  const std::string getModule() const noexcept {
    switch (module_) {
    case Module::lexical_analyzer:
      return "lexical_analyzer";
    case Module::symbol_table:
      return "symbol_table";
    case Module::token:
      return "token";
    }
  }

  // output function
  std::string toString() const {
    std::string mod = "";
    switch (module_) {
    case Module::lexical_analyzer:
      mod = "lexical_analyzer";
    case Module::symbol_table:
      mod = "symbol_table";
    case Module::token:
      mod = "token";
    }
    return "[" + mod + "] LexerException : " + message_;
  }

private:
  std::string message_;
  Module module_; // module
};